/*!
 * http_server.c - http server for mako
 * Copyright (c) 2021, Christopher Jeffrey (MIT License).
 * https://github.com/chjj/mako
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <io/core.h>
#include <io/http.h>
#include <io/loop.h>

#include "http_common.h"
#include "http_parser.h"

/*
 * Types
 */

typedef struct http_conn_s {
  http_server_t *server;
  btc_socket_t *socket;
  struct http_parser parser;
  struct http_parser_settings settings;
  http_req_t *req;
  int last_was_value;
  size_t total_buffered;
} http_conn_t;

/*
 * Request
 */

static void
http_req_init(http_req_t *req) {
  req->method = 0;
  http_string_init(&req->path);
  http_head_init(&req->headers);
  http_string_init(&req->body);
}

static void
http_req_clear(http_req_t *req) {
  http_string_clear(&req->path);
  http_head_clear(&req->headers);
  http_string_clear(&req->body);
}

static http_req_t *
http_req_create(void) {
  http_req_t *req = http_malloc(sizeof(http_req_t));
  http_req_init(req);
  return req;
}

static void
http_req_destroy(http_req_t *req) {
  http_req_clear(req);
  free(req);
}

const http_string_t *
http_req_header(const http_req_t *req, const char *name) {
  size_t len = strlen(name);
  size_t i;

  for (i = 0; i < req->headers.length; i++) {
    const http_header_t *hdr = req->headers.items[i];

    if (http_string_equal(&hdr->field, name, len))
      return &hdr->value;
  }

  return NULL;
}

/*
 * Response
 */

static void
http_res_init(http_res_t *res, btc_socket_t *socket) {
  res->socket = socket;
  http_head_init(&res->headers);
}

static void
http_res_clear(http_res_t *res) {
  http_head_clear(&res->headers);
}

static http_res_t *
http_res_create(btc_socket_t *socket) {
  http_res_t *res = http_malloc(sizeof(http_res_t));
  http_res_init(res, socket);
  return res;
}

static void
http_res_destroy(http_res_t *res) {
  http_res_clear(res);
  free(res);
}

static int
http_res_write(http_res_t *res, void *data, size_t size) {
  int rc = btc_socket_write(res->socket, data, size);

  if (rc == -1) {
    btc_socket_close(res->socket);
    return 0;
  }

  if (rc == 0) {
    if (btc_socket_buffered(res->socket) > HTTP_MAX_BUFFER)
      btc_socket_close(res->socket);

    return 0;
  }

  return 1;
}

static int
http_res_put(http_res_t *res, const char *str, size_t len) {
  void *data;

  if (len == 0)
    return 1;

  data = http_malloc(len);

  memcpy(data, str, len);

  return http_res_write(res, data, len);
}

void
http_res_header(http_res_t *res, const char *field, const char *value) {
  http_head_push_item(&res->headers, field, value);
}

static int
http_gmt_date(char *buf, size_t size) {
  /* Required by the HTTP standard. */
  time_t ts = time(NULL);
  struct tm *gmt;
#ifndef _WIN32
  struct tm tmp;
#endif

  if (ts == (time_t)-1)
    return 0;

  /* Could check TIMER_ABSTIME
     instead of _WIN32 here. */
#if defined(_WIN32)
  gmt = gmtime(&ts);
#else
  gmt = gmtime_r(&ts, &tmp);
#endif

  if (gmt == NULL)
    return 0;

  /* Example: Fri, 05 Nov 2021 06:42:12 GMT */
  return strftime(buf, size, "%a, %d %b %Y %H:%M:%S GMT", gmt) != 0;
}

static size_t
http_res_size_head(http_res_t *res, const char *desc, const char *type) {
  size_t size = 0;
  size_t i;

  size += 12 + 10 + strlen(desc); /* HTTP/1.1 %u %s */
  size += 8 + 63;                 /* Date: %s */
  size += 16 + strlen(type);      /* Content-Type: %s */
  size += 18 + 20;                /* Content-Length: %lu */
  size += 24;                     /* Connection: keep-alive */

  for (i = 0; i < res->headers.length; i++) {
    http_header_t *hdr = res->headers.items[i];

    size += 4 + hdr->field.length + hdr->value.length; /* %s: %s */
  }

  size += 2; /* \r\n */
  size += 1; /* \0 */

  return size;
}

static int
http_res_write_head(http_res_t *res,
                    unsigned int status,
                    const char *type,
                    unsigned long length) {
  const char *desc = http_status_str(status);
  char *head = http_malloc(http_res_size_head(res, desc, type));
  char *zp = head;
  char date[64];
  size_t i;

  zp += sprintf(zp, "HTTP/1.1 %u %s\r\n", status, desc);

  if (http_gmt_date(date, sizeof(date)))
    zp += sprintf(zp, "Date: %s\r\n", date);

  zp += sprintf(zp, "Content-Type: %s\r\n", type);
  zp += sprintf(zp, "Content-Length: %lu\r\n", length);
  zp += sprintf(zp, "Connection: keep-alive\r\n");

  for (i = 0; i < res->headers.length; i++) {
    http_header_t *hdr = res->headers.items[i];

    zp += sprintf(zp, "%s: %s\r\n", hdr->field.data,
                                    hdr->value.data);
  }

  zp += sprintf(zp, "\r\n");

  return http_res_write(res, head, zp - head);
}

void
http_res_send(http_res_t *res,
              unsigned int status,
              const char *type,
              const char *body) {
  size_t length = strlen(body);

  http_res_write_head(res, status, type, length);
  http_res_put(res, body, length);
}

void
http_res_send_data(http_res_t *res,
                   unsigned int status,
                   const char *type,
                   void *body,
                   size_t length) {
  http_res_write_head(res, status, type, length);
  http_res_write(res, body, length);
}

void
http_res_error(http_res_t *res, unsigned int status) {
  char body[33];

  sprintf(body, "%s\n", http_status_str(status));

  http_res_send(res, status, "text/plain", body);
}

/*
 * Connection
 */

static void
http_conn_init(http_conn_t *conn, http_server_t *server);

static void
http_conn_clear(http_conn_t *conn) {
  if (conn->req != NULL)
    http_req_destroy(conn->req);
}

static http_conn_t *
http_conn_create(http_server_t *server) {
  http_conn_t *conn = http_malloc(sizeof(http_conn_t));
  http_conn_init(conn, server);
  return conn;
}

static void
http_conn_destroy(http_conn_t *conn) {
  http_conn_clear(conn);
  free(conn);
}

static int
http_conn_abort(http_conn_t *conn) {
  if (conn->req != NULL)
    http_req_destroy(conn->req);

  btc_socket_close(conn->socket);

  conn->req = NULL;
  conn->last_was_value = 0;
  conn->total_buffered = 0;

  return 1;
}

static void
on_close(btc_socket_t *socket) {
  http_conn_t *conn = btc_socket_get_data(socket);
  http_conn_destroy(conn);
}

static void
on_error(btc_socket_t *socket) {
  btc_socket_close(socket);
}

static int
on_data(btc_socket_t *socket, const void *data, size_t size) {
  http_conn_t *conn = btc_socket_get_data(socket);

  size_t nparsed = http_parser_execute(&conn->parser,
                                       &conn->settings,
                                       data,
                                       size);

  if (conn->parser.upgrade || nparsed != size || size == 0)
    btc_socket_close(socket);

  return 1;
}

static int
on_message_begin(struct http_parser *parser) {
  http_conn_t *conn = parser->data;

  if (conn->req != NULL)
    http_req_destroy(conn->req);

  conn->req = http_req_create();
  conn->last_was_value = 0;
  conn->total_buffered = 0;

  return 0;
}

static int
on_url(struct http_parser *parser, const char *at, size_t length) {
  http_conn_t *conn = parser->data;
  http_req_t *req = conn->req;

  http_string_append(&req->path, at, length);

  conn->total_buffered += length;

  if (req->path.length > HTTP_MAX_FIELD_SIZE)
    return http_conn_abort(conn);

  if (conn->total_buffered > HTTP_MAX_BUFFER)
    return http_conn_abort(conn);

  return 0;
}

static int
on_header_field(struct http_parser *parser, const char *at, size_t length) {
  http_conn_t *conn = parser->data;
  http_req_t *req = conn->req;

  if (req->headers.length > 0 && conn->last_was_value == 0) {
    http_header_t *hdr = req->headers.items[req->headers.length - 1];

    http_string_append(&hdr->field, at, length);

    if (hdr->field.length > HTTP_MAX_FIELD_SIZE)
      return http_conn_abort(conn);
  } else {
    http_header_t *hdr = http_header_create();

    http_string_assign(&hdr->field, at, length);

    if (hdr->field.length > HTTP_MAX_FIELD_SIZE)
      return http_conn_abort(conn);

    http_head_push(&req->headers, hdr);
  }

  conn->last_was_value = 0;
  conn->total_buffered += length;

  if (req->headers.length > HTTP_MAX_HEADERS)
    return http_conn_abort(conn);

  if (conn->total_buffered > HTTP_MAX_BUFFER)
    return http_conn_abort(conn);

  return 0;
}

static int
on_header_value(struct http_parser *parser, const char *at, size_t length) {
  http_conn_t *conn = parser->data;
  http_req_t *req = conn->req;
  http_header_t *hdr = req->headers.items[req->headers.length - 1];

  http_string_append(&hdr->value, at, length);

  conn->last_was_value = 1;
  conn->total_buffered += length;

  if (hdr->value.length > HTTP_MAX_FIELD_SIZE)
    return http_conn_abort(conn);

  if (conn->total_buffered > HTTP_MAX_BUFFER)
    return http_conn_abort(conn);

  return 0;
}

static int
on_headers_complete(struct http_parser *parser) {
  http_conn_t *conn = parser->data;
  http_req_t *req = conn->req;
  size_t i;

  req->method = parser->method;

  for (i = 0; i < req->headers.length; i++) {
    http_header_t *hdr = req->headers.items[i];

    http_string_lower(&hdr->field);
  }

  return 0;
}

static int
on_body(struct http_parser *parser, const char *at, size_t length) {
  http_conn_t *conn = parser->data;
  http_req_t *req = conn->req;

  http_string_append(&req->body, at, length);

  conn->total_buffered += length;

  if (conn->total_buffered > HTTP_MAX_BUFFER)
    return http_conn_abort(conn);

  return 0;
}

static int
on_message_complete(struct http_parser *parser) {
  http_conn_t *conn = parser->data;
  http_server_t *server = conn->server;
  http_req_t *req = conn->req;
  http_res_t *res = http_res_create(conn->socket);

  conn->req = NULL;
  conn->last_was_value = 0;
  conn->total_buffered = 0;

  if (server->on_request != NULL) {
    if (!server->on_request(server, req, res)) {
      http_req_destroy(req);
      http_res_destroy(res);
      btc_socket_close(conn->socket);
      return 1;
    }
  }

  http_req_destroy(req);
  http_res_destroy(res);

  return 0;
}

static void
http_conn_init(http_conn_t *conn, http_server_t *server) {
  conn->server = server;
  conn->socket = NULL;

  http_parser_init(&conn->parser, HTTP_REQUEST);
  http_parser_settings_init(&conn->settings);

  conn->parser.data = conn;

  conn->settings.on_message_begin = on_message_begin;
  conn->settings.on_url = on_url;
  conn->settings.on_status = NULL;
  conn->settings.on_header_field = on_header_field;
  conn->settings.on_header_value = on_header_value;
  conn->settings.on_headers_complete = on_headers_complete;
  conn->settings.on_body = on_body;
  conn->settings.on_message_complete = on_message_complete;
  conn->settings.on_chunk_header = NULL;
  conn->settings.on_chunk_complete = NULL;

  conn->req = NULL;
  conn->last_was_value = 0;
  conn->total_buffered = 0;
}

static void
http_conn_accept(http_conn_t *conn, btc_socket_t *socket) {
  conn->socket = socket;

  btc_socket_set_data(socket, conn);
  btc_socket_on_close(socket, on_close);
  btc_socket_on_error(socket, on_error);
  btc_socket_on_data(socket, on_data);
}

/*
 * Server
 */

static void
http_server_init(http_server_t *server, btc_loop_t *loop) {
  server->loop = loop;
  server->socket = NULL;
  server->on_request = NULL;
  server->data = NULL;
}

static void
http_server_clear(http_server_t *server) {
  (void)server;
}

http_server_t *
http_server_create(btc_loop_t *loop) {
  http_server_t *server = http_malloc(sizeof(http_server_t));
  http_server_init(server, loop);
  return server;
}

void
http_server_destroy(http_server_t *server) {
  http_server_clear(server);
  free(server);
}

static void
on_socket(btc_socket_t *parent, btc_socket_t *child) {
  http_server_t *server = btc_socket_get_data(parent);
  http_conn_t *conn = http_conn_create(server);

  http_conn_accept(conn, child);
}

static void
on_server_close(btc_socket_t *socket) {
  http_server_t *server = btc_socket_get_data(socket);
  server->socket = NULL;
}

int
http_server_open(http_server_t *server, const btc_sockaddr_t *addr) {
  server->socket = btc_loop_listen(server->loop, addr);

  if (server->socket == NULL)
    return 0;

  btc_socket_set_data(server->socket, server);
  btc_socket_on_socket(server->socket, on_socket);
  btc_socket_on_close(server->socket, on_server_close);

  return 1;
}

void
http_server_close(http_server_t *server) {
  if (server->socket != NULL)
    btc_socket_close(server->socket);
}

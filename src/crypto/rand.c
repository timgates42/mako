/*!
 * rand.c - RNG for mako
 * Copyright (c) 2020, Christopher Jeffrey (MIT License).
 * https://github.com/chjj/mako
 */

/**
 * Random Number Generation
 *
 * We use a ChaCha20 RNG with a design inspired by
 * libsodium[1]. Our primary difference is a much
 * more complicated seeding procedure which ensures
 * strong randomness (similar to Bitcoin Core[2]).
 *
 * The seeding procedure uses a combination of OS
 * entropy, hardware entropy, and entropy manually
 * gathered from the environment. See entropy/ for
 * more information.
 *
 * We expose a global fork-aware and thread-safe
 * RNG. We use thread local storage for the global
 * context. This avoids us having to link to
 * pthread and deal with other OS compat issues.
 *
 * The RNG below is not used anywhere internally,
 * and as such, mako can build without it (in
 * the case that more portability is desired).
 *
 * [1] https://github.com/jedisct1/libsodium/blob/master/src/libsodium
 *     /randombytes/internal/randombytes_internal_random.c
 * [2] https://github.com/bitcoin/bitcoin/blob/master/src/random.cpp
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <mako/crypto/rand.h>
#include <mako/crypto/stream.h>
#include <mako/util.h>
#include "sysrand.h"
#include "../internal.h"

/*
 * RNG
 */

typedef struct rng_s {
  uint32_t key[8];
  uint64_t nonce;
  uint32_t pool[128];
  size_t pos;
  int started;
  long pid;
} rng_t;

static int
rng_init(rng_t *rng) {
  memset(rng, 0, sizeof(*rng));
  return btc_sysrand(rng->key, 32);
}

static void
rng_crypt(const rng_t *rng, void *data, size_t size) {
  btc_chacha20_t ctx;

  btc_chacha20_init(&ctx, (const uint8_t *)rng->key, 32,
                          (const uint8_t *)&rng->nonce, 8,
                          0);

  btc_chacha20_crypt(&ctx, (uint8_t *)data,
                           (const uint8_t *)data,
                           size);

  btc_memzero(&ctx, sizeof(ctx));
}

static void
rng_read(const rng_t *rng, void *dst, size_t size) {
  if (size > 0)
    memset(dst, 0, size);

  rng_crypt(rng, dst, size);
}

static void
rng_generate(rng_t *rng, void *dst, size_t size) {
  /* Read the keystream. */
  rng_read(rng, dst, size);

  /* Mix in some user entropy. */
  rng->key[0] ^= (uint32_t)size;

  /* Re-key immediately. */
  rng->nonce++;

  /* At this point, the CTR-DRBG simply reads the
     keystream again in order to rekey. We mimic
     libsodium instead by XOR'ing the partially
     modified key with its own keystream. In truth,
     there's probably not really a difference in
     terms of security, as the outputs in both
     scenarios are dependent on the key. */
  rng_crypt(rng, rng->key, 32);
}

static uint32_t
rng_random(rng_t *rng) {
  uint32_t x;
  size_t i;

  if (rng->pos == 0) {
    /* Read the keystream. */
    rng_read(rng, rng->pool, 512);

    /* Re-key every 512 bytes. */
    for (i = 0; i < 8; i++)
      rng->key[i] ^= rng->pool[120 + i];

    for (i = 0; i < 8; i++)
      rng->pool[120 + i] = 0;

    rng->nonce++;
    rng->pos = 120;
  }

  x = rng->pool[--rng->pos];

  rng->pool[rng->pos] = 0;

  return x;
}

static uint32_t
rng_uniform(rng_t *rng, uint32_t max) {
  /* See: http://www.pcg-random.org/posts/bounded-rands.html */
  uint32_t x, r;

  if (max <= 1)
    return 0;

  do {
    x = rng_random(rng);
    r = x % max;
  } while (x - r > (-max));

  return r;
}

/*
 * Global Lock
 */

#if defined(__MINGW32__) && defined(_WIN32)
/* MinGW autolinks to libwinpthread when TLS
 * is used. This means our library will not be
 * redistributable on Windows unless we ship
 * libwinpthread.dll as well.
 *
 * To avoid this, we utilize the win32 API
 * directly and use a global lock instead.
 */

#undef BTC_TLS

#include <windows.h>

static CRITICAL_SECTION rng_lock;

static void
rng_global_lock(void) {
  static int initialized = 0;
  static HANDLE event = NULL;
  HANDLE created, existing;

  if (initialized == 0) {
    created = CreateEvent(NULL, 1, 0, NULL);

    if (created == NULL)
      btc_abort(); /* LCOV_EXCL_LINE */

    existing = InterlockedCompareExchangePointer(&event, created, NULL);

    if (existing == NULL) {
      InitializeCriticalSection(&rng_lock);

      if (!SetEvent(created))
        btc_abort(); /* LCOV_EXCL_LINE */

      initialized = 1;
    } else {
      CloseHandle(created);

      if (WaitForSingleObject(existing, INFINITE) != WAIT_OBJECT_0)
        btc_abort(); /* LCOV_EXCL_LINE */
    }
  }

  EnterCriticalSection(&rng_lock);
}

static void
rng_global_unlock(void) {
  LeaveCriticalSection(&rng_lock);
}

#else /* !__MINGW32__ */

#if !defined(BTC_TLS) && defined(BTC_HAVE_PTHREAD)
#  define BTC_USE_PTHREAD
#endif

#ifdef BTC_USE_PTHREAD
#  include <pthread.h>
static pthread_mutex_t rng_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

static void
rng_global_lock(void) {
#ifdef BTC_USE_PTHREAD
  if (pthread_mutex_lock(&rng_lock) != 0)
    btc_abort(); /* LCOV_EXCL_LINE */
#endif
}

static void
rng_global_unlock(void) {
#ifdef BTC_USE_PTHREAD
  if (pthread_mutex_unlock(&rng_lock) != 0)
    btc_abort(); /* LCOV_EXCL_LINE */
#endif
}

#endif /* !__MINGW32__ */

/*
 * Global Context
 */

#if defined(BTC_TLS)
static BTC_TLS rng_t rng_state;
#else
static rng_t rng_state;
#endif

static void
rng_global_init(void) {
  long pid = btc_getpid();

  if (!rng_state.started || rng_state.pid != pid) {
    /* LCOV_EXCL_START */
    if (!rng_init(&rng_state)) {
      btc_abort();
      return;
    }
    /* LCOV_EXCL_STOP */

    rng_state.started = 1;
    rng_state.pid = pid;
  }
}

/*
 * Random
 */

int
btc_getentropy(void *dst, size_t size) {
  return btc_sysrand(dst, size);
}

void
btc_getrandom(void *dst, size_t size) {
  rng_global_lock();
  rng_global_init();
  rng_generate(&rng_state, dst, size);
  rng_global_unlock();
}

uint32_t
btc_random(void) {
  uint32_t num;

  rng_global_lock();
  rng_global_init();

  num = rng_random(&rng_state);

  rng_global_unlock();

  return num;
}

uint32_t
btc_uniform(uint32_t max) {
  uint32_t num;

  rng_global_lock();
  rng_global_init();

  num = rng_uniform(&rng_state, max);

  rng_global_unlock();

  return num;
}

uint64_t
btc_nonce(void) {
  uint32_t hi, lo;

  rng_global_lock();
  rng_global_init();

  hi = rng_random(&rng_state);
  lo = rng_random(&rng_state);

  rng_global_unlock();

  return ((uint64_t)hi << 32) | lo;
}

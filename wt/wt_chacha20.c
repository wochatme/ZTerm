#include "wt_chacha20.h"

static void mbedtls_platform_zeroize(void* buf, size_t len)
{
    if (buf && len > 0) 
    {
        unsigned char* p = (unsigned char*)buf;
        for (size_t i = 0; i < len; i++)
            *p++ = 0;
    }
}

static const uint16_t mbedtls_byte_order_detector = { 0x100 };
#define MBEDTLS_IS_BIG_ENDIAN (*((unsigned char *) (&mbedtls_byte_order_detector)) == 0x01)


/*
 * Detect MSVC built-in byteswap routines
 */
#if defined(_MSC_VER)
#if !defined(MBEDTLS_BSWAP16)
#define MBEDTLS_BSWAP16 _byteswap_ushort
#endif
#if !defined(MBEDTLS_BSWAP32)
#define MBEDTLS_BSWAP32 _byteswap_ulong
#endif
#if !defined(MBEDTLS_BSWAP64)
#define MBEDTLS_BSWAP64 _byteswap_uint64
#endif
#endif /* defined(_MSC_VER) */

 /**
  * Write the unsigned 32 bits integer to the given address, which need not
  * be aligned.
  *
  * \param   p pointer to 4 bytes of data
  * \param   x data to write
  */
#if defined(__IAR_SYSTEMS_ICC__)
#pragma inline = forced
#elif defined(__GNUC__)
__attribute__((always_inline))
#endif
static inline void mbedtls_put_unaligned_uint32(void* p, uint32_t x)
{
#if defined(UINT_UNALIGNED)
    mbedtls_uint32_unaligned_t* p32 = (mbedtls_uint32_unaligned_t*)p;
    *p32 = x;
#else
    memcpy(p, &x, sizeof(x));
#endif
}

 /**
  * Read the unsigned 32 bits integer from the given address, which need not
  * be aligned.
  *
  * \param   p pointer to 4 bytes of data
  * \return  Data at the given address
  */
#if defined(__IAR_SYSTEMS_ICC__)
#pragma inline = forced
#elif defined(__GNUC__)
__attribute__((always_inline))
#endif
static inline uint32_t mbedtls_get_unaligned_uint32(const void* p)
{
    uint32_t r;
#if defined(UINT_UNALIGNED)
    mbedtls_uint32_unaligned_t* p32 = (mbedtls_uint32_unaligned_t*)p;
    r = *p32;
#else
    memcpy(&r, p, sizeof(r));
#endif
    return r;
}

/**
 * Get the unsigned 32 bits integer corresponding to four bytes in
 * little-endian order (LSB first).
 *
 * \param   data    Base address of the memory to get the four bytes from.
 * \param   offset  Offset from \p data of the first and least significant
 *                  byte of the four bytes to build the 32 bits unsigned
 *                  integer from.
 */
#define MBEDTLS_GET_UINT32_LE(data, offset)                                \
    ((MBEDTLS_IS_BIG_ENDIAN)                                               \
        ? MBEDTLS_BSWAP32(mbedtls_get_unaligned_uint32((data) + (offset))) \
        : mbedtls_get_unaligned_uint32((data) + (offset))                  \
    )

#define CHACHA20_BLOCK_SIZE_BYTES (4U * 16U)
#define CHACHA20_CTR_INDEX (12U)

#define ROTL32(value, amount) \
    ((uint32_t) ((value) << (amount)) | ((value) >> (32 - (amount))))

 /**
  * \brief           ChaCha20 quarter round operation.
  *
  *                  The quarter round is defined as follows (from RFC 7539):
  *                      1.  a += b; d ^= a; d <<<= 16;
  *                      2.  c += d; b ^= c; b <<<= 12;
  *                      3.  a += b; d ^= a; d <<<= 8;
  *                      4.  c += d; b ^= c; b <<<= 7;
  *
  * \param state     ChaCha20 state to modify.
  * \param a         The index of 'a' in the state.
  * \param b         The index of 'b' in the state.
  * \param c         The index of 'c' in the state.
  * \param d         The index of 'd' in the state.
  */
static inline void chacha20_quarter_round(uint32_t state[16],
    size_t a,
    size_t b,
    size_t c,
    size_t d)
{
    /* a += b; d ^= a; d <<<= 16; */
    state[a] += state[b];
    state[d] ^= state[a];
    state[d] = ROTL32(state[d], 16);

    /* c += d; b ^= c; b <<<= 12 */
    state[c] += state[d];
    state[b] ^= state[c];
    state[b] = ROTL32(state[b], 12);

    /* a += b; d ^= a; d <<<= 8; */
    state[a] += state[b];
    state[d] ^= state[a];
    state[d] = ROTL32(state[d], 8);

    /* c += d; b ^= c; b <<<= 7; */
    state[c] += state[d];
    state[b] ^= state[c];
    state[b] = ROTL32(state[b], 7);
}

 /**
  * \brief           Perform the ChaCha20 inner block operation.
  *
  *                  This function performs two rounds: the column round and the
  *                  diagonal round.
  *
  * \param state     The ChaCha20 state to update.
  */
static void chacha20_inner_block(uint32_t state[16])
{
    chacha20_quarter_round(state, 0, 4, 8, 12);
    chacha20_quarter_round(state, 1, 5, 9, 13);
    chacha20_quarter_round(state, 2, 6, 10, 14);
    chacha20_quarter_round(state, 3, 7, 11, 15);

    chacha20_quarter_round(state, 0, 5, 10, 15);
    chacha20_quarter_round(state, 1, 6, 11, 12);
    chacha20_quarter_round(state, 2, 7, 8, 13);
    chacha20_quarter_round(state, 3, 4, 9, 14);
}

/**
 * Put in memory a 32 bits unsigned integer in little-endian order.
 *
 * \param   n       32 bits unsigned integer to put in memory.
 * \param   data    Base address of the memory where to put the 32
 *                  bits unsigned integer in.
 * \param   offset  Offset from \p data where to put the least significant
 *                  byte of the 32 bits unsigned integer \p n.
 */
#define MBEDTLS_PUT_UINT32_LE(n, data, offset)                                   \
    {                                                                            \
        if (MBEDTLS_IS_BIG_ENDIAN)                                               \
        {                                                                        \
            mbedtls_put_unaligned_uint32((data) + (offset), MBEDTLS_BSWAP32((uint32_t) (n))); \
        }                                                                        \
        else                                                                     \
        {                                                                        \
            mbedtls_put_unaligned_uint32((data) + (offset), ((uint32_t) (n)));   \
        }                                                                        \
    }

 /**
  * \brief               Generates a keystream block.
  *
  * \param initial_state The initial ChaCha20 state (key, nonce, counter).
  * \param keystream     Generated keystream bytes are written to this buffer.
  */
static void chacha20_block(const uint32_t initial_state[16], unsigned char keystream[64])
{
    uint32_t working_state[16];
    size_t i;

    memcpy(working_state,
        initial_state,
        CHACHA20_BLOCK_SIZE_BYTES);

    for (i = 0U; i < 10U; i++) {
        chacha20_inner_block(working_state);
    }

    working_state[0] += initial_state[0];
    working_state[1] += initial_state[1];
    working_state[2] += initial_state[2];
    working_state[3] += initial_state[3];
    working_state[4] += initial_state[4];
    working_state[5] += initial_state[5];
    working_state[6] += initial_state[6];
    working_state[7] += initial_state[7];
    working_state[8] += initial_state[8];
    working_state[9] += initial_state[9];
    working_state[10] += initial_state[10];
    working_state[11] += initial_state[11];
    working_state[12] += initial_state[12];
    working_state[13] += initial_state[13];
    working_state[14] += initial_state[14];
    working_state[15] += initial_state[15];

    for (i = 0U; i < 16; i++) {
        size_t offset = i * 4U;

        MBEDTLS_PUT_UINT32_LE(working_state[i], keystream, offset);
    }

    mbedtls_platform_zeroize(working_state, sizeof(working_state));
}

/* Always inline mbedtls_xor() for similar reasons as mbedtls_xor_no_simd(). */
#if defined(__IAR_SYSTEMS_ICC__)
#pragma inline = forced
#elif defined(__GNUC__)
__attribute__((always_inline))
#endif

/**
 * Perform a fast block XOR operation, such that
 * r[i] = a[i] ^ b[i] where 0 <= i < n
 *
 * \param   r Pointer to result (buffer of at least \p n bytes). \p r
 *            may be equal to either \p a or \p b, but behaviour when
 *            it overlaps in other ways is undefined.
 * \param   a Pointer to input (buffer of at least \p n bytes)
 * \param   b Pointer to input (buffer of at least \p n bytes)
 * \param   n Number of bytes to process.
 *
 * \note      Depending on the situation, it may be faster to use either mbedtls_xor() or
 *            mbedtls_xor_no_simd() (these are functionally equivalent).
 *            If the result is used immediately after the xor operation in non-SIMD code (e.g, in
 *            AES-CBC), there may be additional latency to transfer the data from SIMD to scalar
 *            registers, and in this case, mbedtls_xor_no_simd() may be faster. In other cases where
 *            the result is not used immediately (e.g., in AES-CTR), mbedtls_xor() may be faster.
 *            For targets without SIMD support, they will behave the same.
 */
static inline void mbedtls_xor(unsigned char* r, const unsigned char* a, const unsigned char* b, size_t n)
{
    size_t i = 0;
#if defined(MBEDTLS_EFFICIENT_UNALIGNED_ACCESS)
#if defined(MBEDTLS_HAVE_NEON_INTRINSICS) && \
    (!(defined(MBEDTLS_COMPILER_IS_GCC) && MBEDTLS_GCC_VERSION < 70300))
    /* Old GCC versions generate a warning here, so disable the NEON path for these compilers */
    for (; (i + 16) <= n; i += 16) {
        uint8x16_t v1 = vld1q_u8(a + i);
        uint8x16_t v2 = vld1q_u8(b + i);
        uint8x16_t x = veorq_u8(v1, v2);
        vst1q_u8(r + i, x);
    }
#if defined(__IAR_SYSTEMS_ICC__)
    /* This if statement helps some compilers (e.g., IAR) optimise out the byte-by-byte tail case
     * where n is a constant multiple of 16.
     * For other compilers (e.g. recent gcc and clang) it makes no difference if n is a compile-time
     * constant, and is a very small perf regression if n is not a compile-time constant. */
    if (n % 16 == 0) {
        return;
    }
#endif
#elif defined(MBEDTLS_ARCH_IS_X64) || defined(MBEDTLS_ARCH_IS_ARM64)
    /* This codepath probably only makes sense on architectures with 64-bit registers */
    for (; (i + 8) <= n; i += 8) {
        uint64_t x = mbedtls_get_unaligned_uint64(a + i) ^ mbedtls_get_unaligned_uint64(b + i);
        mbedtls_put_unaligned_uint64(r + i, x);
    }
#if defined(__IAR_SYSTEMS_ICC__)
    if (n % 8 == 0) {
        return;
    }
#endif
#else
    for (; (i + 4) <= n; i += 4) {
        uint32_t x = mbedtls_get_unaligned_uint32(a + i) ^ mbedtls_get_unaligned_uint32(b + i);
        mbedtls_put_unaligned_uint32(r + i, x);
    }
#if defined(__IAR_SYSTEMS_ICC__)
    if (n % 4 == 0) {
        return;
    }
#endif
#endif
#endif
    for (i = 0; i < n; i++) 
    {
        r[i] = a[i] ^ b[i];
    }
}

void wt_chacha20_init(wt_chacha20_context* ctx)
{
    if (ctx)
    {
        mbedtls_platform_zeroize(ctx->state, sizeof(ctx->state));
        mbedtls_platform_zeroize(ctx->keystream8, sizeof(ctx->keystream8));

        /* Initially, there's no keystream bytes available */
        ctx->keystream_bytes_used = CHACHA20_BLOCK_SIZE_BYTES;
    }
}

int wt_chacha20_setkey(wt_chacha20_context* ctx, const unsigned char key[32])
{
    int ret = 1;
    if (ctx)
    {
        /* ChaCha20 constants - the string "expand 32-byte k" */
        ctx->state[0] = 0x61707865;
        ctx->state[1] = 0x3320646e;
        ctx->state[2] = 0x79622d32;
        ctx->state[3] = 0x6b206574;

        /* Set key */
        ctx->state[4] = MBEDTLS_GET_UINT32_LE(key, 0);
        ctx->state[5] = MBEDTLS_GET_UINT32_LE(key, 4);
        ctx->state[6] = MBEDTLS_GET_UINT32_LE(key, 8);
        ctx->state[7] = MBEDTLS_GET_UINT32_LE(key, 12);
        ctx->state[8] = MBEDTLS_GET_UINT32_LE(key, 16);
        ctx->state[9] = MBEDTLS_GET_UINT32_LE(key, 20);
        ctx->state[10] = MBEDTLS_GET_UINT32_LE(key, 24);
        ctx->state[11] = MBEDTLS_GET_UINT32_LE(key, 28);

        ret = 0;
    }
    return ret;
}


int wt_chacha20_starts(wt_chacha20_context* ctx, const unsigned char nonce[12], uint32_t counter)
{
    /* Counter */
    ctx->state[12] = counter;

    /* Nonce */
    ctx->state[13] = MBEDTLS_GET_UINT32_LE(nonce, 0);
    ctx->state[14] = MBEDTLS_GET_UINT32_LE(nonce, 4);
    ctx->state[15] = MBEDTLS_GET_UINT32_LE(nonce, 8);

    mbedtls_platform_zeroize(ctx->keystream8, sizeof(ctx->keystream8));

    /* Initially, there's no keystream bytes available */
    ctx->keystream_bytes_used = CHACHA20_BLOCK_SIZE_BYTES;

    return 0;
}

int wt_chacha20_update(wt_chacha20_context* ctx, size_t size, const unsigned char* input, unsigned char* output)
{
    size_t offset = 0U;

    /* Use leftover keystream bytes, if available */
    while (size > 0U && ctx->keystream_bytes_used < CHACHA20_BLOCK_SIZE_BYTES) 
    {
        output[offset] = input[offset] ^ ctx->keystream8[ctx->keystream_bytes_used];
        ctx->keystream_bytes_used++;
        offset++;
        size--;
    }

    /* Process full blocks */
    while (size >= CHACHA20_BLOCK_SIZE_BYTES) 
    {
        /* Generate new keystream block and increment counter */
        chacha20_block(ctx->state, ctx->keystream8);
        ctx->state[CHACHA20_CTR_INDEX]++;

        mbedtls_xor(output + offset, input + offset, ctx->keystream8, 64U);

        offset += CHACHA20_BLOCK_SIZE_BYTES;
        size -= CHACHA20_BLOCK_SIZE_BYTES;
    }

    /* Last (partial) block */
    if (size > 0U) 
    {
        /* Generate new keystream block and increment counter */
        chacha20_block(ctx->state, ctx->keystream8);
        ctx->state[CHACHA20_CTR_INDEX]++;

        mbedtls_xor(output + offset, input + offset, ctx->keystream8, size);

        ctx->keystream_bytes_used = size;

    }
    return 0;
}

void wt_chacha20_free(wt_chacha20_context* ctx)
{
    if (ctx != NULL) 
    {
        mbedtls_platform_zeroize(ctx, sizeof(wt_chacha20_context));
    }
}

bool wt_IsBigEndian()
{
    return MBEDTLS_IS_BIG_ENDIAN;
}

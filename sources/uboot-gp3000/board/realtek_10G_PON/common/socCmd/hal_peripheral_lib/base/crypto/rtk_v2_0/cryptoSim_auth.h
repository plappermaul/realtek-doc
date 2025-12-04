#ifndef _CRYPTO_SIM_AUTH_H_
#define _CRYPTO_SIM_AUTH_H_

#ifdef  __cplusplus
extern "C" {
#endif

#define MBEDTLS_MD_MAX_SIZE 32

#define MBEDTLS_ERR_MD_BAD_INPUT_DATA               -0x5100  /**< Bad input parameters to function. */

/* md5 context */
typedef struct{

    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[4];          /*!< intermediate digest state  */
    unsigned char buffer[64];    /*!< data block being processed */
}mbedtls_md5_context;

/**
 * \brief          SHA-1 context structure
 */
typedef struct
{
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[5];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
}mbedtls_sha1_context;

typedef struct
{
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[8];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
    int is224;                  /*!< 0 => SHA-256, else SHA-224 */
}mbedtls_sha256_context;


typedef enum{
    MBEDTLS_MD_NONE=0,
    MBEDTLS_MD_MD5=3,
    MBEDTLS_MD_SHA1,
    MBEDTLS_MD_SHA224,
    MBEDTLS_MD_SHA256

} mbedtls_md_type_t;

typedef struct{
    /** Digest identifier */
    mbedtls_md_type_t type;
    
    /** Name of the message digest */
    const char *name;

    /** Output length of the digest function in bytes */
    int size;

    /** Block length of the digest function in bytes */
    int block_size;
    
    /** Digest initialisation function */
    void (*starts_func) (void *ctx);
    
    /** Digest update function */
    void (*update_func) (void *ctx, const unsigned char *input, size_t ilen);

    /** Digest finalisation function */
    void (*finish_func) (void *ctx, unsigned char *output);

    /** Generic digest function */
    void (*digest_func) (const unsigned char *input, size_t ilen,
                         unsigned char *output);

    /** Allocate a new context */
    void * (*ctx_alloc_func) (void);

    /** Free the given context */
    void (*ctx_free_func) (void *ctx);

    /** Clone state from a context */
    void (*clone_func) (void *dst, const void *src);

    /** Internal use only */
    void (*process_func) (void *ctx, const unsigned char *input);

}mbedtls_md_info_t;

typedef struct{
    /** Information about the associated message digest */
    const mbedtls_md_info_t *md_info;

    /** Digest-specific context */
    void *md_ctx;

    /** HMAC part of the context */
    void *hmac_ctx;
}mbedtls_md_context_t;

void mbedtls_mix_hmac_init(void);

void mbedtls_md5_init( mbedtls_md5_context *ctx );
void mbedtls_md5_free( mbedtls_md5_context *ctx );
void mbedtls_md5_starts( mbedtls_md5_context *ctx );
void mbedtls_md5_update( mbedtls_md5_context *ctx, const unsigned char *input, size_t ilen );
void mbedtls_md5_finish( mbedtls_md5_context *ctx, unsigned char output[16] );
						
void mbedtls_sha1_init( mbedtls_sha1_context *ctx );
void mbedtls_sha1_free( mbedtls_sha1_context *ctx );
void mbedtls_sha1_starts( mbedtls_sha1_context *ctx );
void mbedtls_sha1_update( mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen );
void mbedtls_sha1_finish( mbedtls_sha1_context *ctx, unsigned char output[20] );

void mbedtls_sha256_init( mbedtls_sha256_context *ctx );
void mbedtls_sha256_free( mbedtls_sha256_context *ctx );
void mbedtls_sha256_starts( mbedtls_sha256_context *ctx, int is224 );
void mbedtls_sha256_update( mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen );
void mbedtls_sha256_finish( mbedtls_sha256_context *ctx, unsigned char output[32] );

void mbedtls_hmac_init(mbedtls_md_context_t *md_hmac_con, s32 authtype);
int mbedtls_md_hmac_starts(mbedtls_md_context_t *ctx,const unsigned char *key,size_t keylen);
int mbedtls_md_hmac_update(mbedtls_md_context_t *ctx,const unsigned char *input, size_t ilen);
int mbedtls_md_hmac_finish(mbedtls_md_context_t *ctx, unsigned char *output);

void mbedtls_md5_swvrf(const unsigned char *input, size_t ilen, unsigned char output[16]);

void mbedtls_sha1_swvrf(const unsigned char *input, size_t ilen, unsigned char output[20]);

void mbedtls_sha2_swvrf(const unsigned char *input, size_t ilen, unsigned char output[32], int is224);

int mbedtls_hmac_md5_swvrf(const unsigned char *key, size_t keylen,
                           const unsigned char *input, size_t ilen, 
                           unsigned char *output);

int mbedtls_hmac_sha1_swvrf(const unsigned char *key, size_t keylen,
                            const unsigned char *input, size_t ilen, 
                            unsigned char *output);

int mbedtls_hmac_sha2_swvrf(const unsigned char *key, size_t keylen,
                              const unsigned char *input, size_t ilen, 
                              unsigned char *output, int is224);

#ifdef  __cplusplus
}
#endif

#endif // _CRYPTO_SIM_AUTH_H_

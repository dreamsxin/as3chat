/* use OpenSSL functions when available */
#define HAVE_SSL 1
#ifdef HAVE_SSL
#include <openssl/md5.h>

#define md5_state_t MD5_CTX
#define md5_init(c) MD5_Init(c)
#define md5_append(c, data, len) MD5_Update(c, data, len);
#define md5_finish(c, md) MD5_Final(md, c)

#else

typedef uint8_t md5_byte_t; /* 8-bit byte */
typedef uint32_t md5_word_t; /* 32-bit word */

/* Define the state of the MD5 Algorithm. */
typedef struct md5_state_s {
    md5_word_t count[2]; /* message length in bits, lsw first */
    md5_word_t abcd[4]; /* digest buffer */
    md5_byte_t buf[64]; /* accumulate block */
} md5_state_t;

#ifdef __cplusplus
extern "C" {
#endif

    void md5_init(md5_state_t *pms);
    void md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes);
    void md5_finish(md5_state_t *pms, md5_byte_t digest[16]);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* HAVE_SSL */

#include <common.h>

#include "rtk_crypto_base_common.h"
#include "cryptoSim_auth.h"
#include "cryptoSim_cipher.h"
#include "cryptoSim_swvrf.h"
#include "rtk_crypto_base.h"
#include "rtk_crypto_base_hal.h"
#include "rtk_crypto_base_type.h"
#include "rtk_crypto_base_ctrl.h"
#include "rtk_crypto_base_test.h"


static u64 src_addr=0x80010000, dst_addr=0x80000000;
static u8 addr_test_flag=0;
//
// Auth test vectors
//

// MD5-128 test vectors
u8 md5_test_msg0[] = {};

u8 md5_test_res0[] = {
    0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
    0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e
};

u8 md5_test_msg1[] = {
    0x61, 0x62, 0x63
};

u8 md5_test_res1[] = {
    0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0,
    0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72
};

u8 md5_test_msg2[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63
};

u8 md5_test_res2[] = {
    0x9d, 0x9c, 0x48, 0x75, 0xf2, 0xa3, 0x7c, 0x14,
    0xa0, 0x4a, 0x78, 0x85, 0x21, 0x0e, 0xd8, 0x78
};

u8 md5_seq_test_msg_p1[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e
};

u8 md5_seq_test_msg_p2[] = {
    0x6f, 0x70, 0x61, 0x62, 0x63
};

u8 md5_seq_test_res[] = {
    0x9d, 0x9c, 0x48, 0x75, 0xf2, 0xa3, 0x7c, 0x14,
    0xa0, 0x4a, 0x78, 0x85, 0x21, 0x0e, 0xd8, 0x78
};

// SHA1-160 test vectors
u8 sha1_test_msg0[] = {};

u8 sha1_test_res0[] = {
    0xDA, 0x39, 0xA3, 0xEE, 0x5E, 0x6B, 0x4B, 0x0D,
    0x32, 0x55, 0xBF, 0xEF, 0x95, 0x60, 0x18, 0x90,
    0xAF, 0xD8, 0x07, 0x09
};

u8 sha1_test_msg1[] = {
    0x61, 0x62, 0x63
};

u8 sha1_test_res1[] = {
    0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a,
    0xba, 0x3e, 0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c,
    0x9c, 0xd0, 0xd8, 0x9d
};

u8 sha1_test_msg2[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63
};

u8 sha1_test_res2[] = {
    0x1a, 0x5b, 0x5a, 0xb7, 0x10, 0x77, 0xca, 0x85,
    0x15, 0x74, 0x17, 0x39, 0x4f, 0x07, 0xae, 0xe7,
    0xe2, 0x82, 0x81, 0x6b
};

u8 sha1_seq_test_msg_p1[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e
};

u8 sha1_seq_test_msg_p2[] = {
    0x6f, 0x70, 0x61, 0x62, 0x63
};

u8 sha1_seq_test_res[] = {
    0x1a, 0x5b, 0x5a, 0xb7, 0x10, 0x77, 0xca, 0x85,
    0x15, 0x74, 0x17, 0x39, 0x4f, 0x07, 0xae, 0xe7,
    0xe2, 0x82, 0x81, 0x6b
};

// SHA2-224 test vectors
u8 sha2_224_test_msg0[] = {};

u8 sha2_224_test_res0[] = {
    0xD1, 0x4A, 0x02, 0x8C, 0x2A, 0x3A, 0x2B, 0xC9,
    0x47, 0x61, 0x02, 0xBB, 0x28, 0x82, 0x34, 0xC4,
    0x15, 0xA2, 0xB0, 0x1F, 0x82, 0x8E, 0xA6, 0x2A,
    0xC5, 0xB3, 0xE4, 0x2F
};

u8 sha2_224_test_msg1[] = {
    0x61, 0x62, 0x63
};

u8 sha2_224_test_res1[] = {
    0x23, 0x09, 0x7d, 0x22, 0x34, 0x05, 0xd8, 0x22,
    0x86, 0x42, 0xa4, 0x77, 0xbd, 0xa2, 0x55, 0xb3,
    0x2a, 0xad, 0xbc, 0xe4, 0xbd, 0xa0, 0xb3, 0xf7,
    0xe3, 0x6c, 0x9d, 0xa7
};

u8 sha2_224_test_msg2[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63
};

u8 sha2_224_test_res2[] = {
    0x5c, 0xea, 0xdc, 0x82, 0x80, 0xb3, 0xc7, 0xf0,
    0x64, 0x2a, 0x89, 0x26, 0xd1, 0xa7, 0x17, 0x1c,
    0x2c, 0xc2, 0x8c, 0x14, 0x23, 0x5d, 0x4d, 0x11,
    0xa7, 0x7e, 0xdf, 0xae
};

u8 sha2_224_seq_test_msg_p1[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e
};

u8 sha2_224_seq_test_msg_p2[] = {
    0x6f, 0x70, 0x61, 0x62, 0x63
};

u8 sha2_224_seq_test_res[] = {
    0x5c, 0xea, 0xdc, 0x82, 0x80, 0xb3, 0xc7, 0xf0,
    0x64, 0x2a, 0x89, 0x26, 0xd1, 0xa7, 0x17, 0x1c,
    0x2c, 0xc2, 0x8c, 0x14, 0x23, 0x5d, 0x4d, 0x11,
    0xa7, 0x7e, 0xdf, 0xae
};

// SHA2-256 test vectors
u8 sha2_256_test_msg0[] = {};

u8 sha2_256_test_res0[] = {
    0xE3, 0xB0, 0xC4, 0x42, 0x98, 0xFC, 0x1C, 0x14,
    0x9A, 0xFB, 0xF4, 0xC8, 0x99, 0x6F, 0xB9, 0x24,
    0x27, 0xAE, 0x41, 0xE4, 0x64, 0x9B, 0x93, 0x4C,
    0xA4, 0x95, 0x99, 0x1B, 0x78, 0x52, 0xB8, 0x55
};

u8 sha2_256_test_msg1[] = {
    0x61, 0x62, 0x63
};

u8 sha2_256_test_res1[] = {
    0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
    0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
    0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
    0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
};

u8 sha2_256_test_msg2[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63
};

u8 sha2_256_test_res2[] = {
    0x29, 0xf2, 0xcb, 0xa3, 0xa2, 0xda, 0xaf, 0x16,
    0x90, 0x9d, 0xe9, 0x03, 0xc3, 0x79, 0xe2, 0xf0,
    0x7b, 0x8c, 0xbb, 0x1d, 0xcc, 0x07, 0x2f, 0x03,
    0xb5, 0x7d, 0x28, 0xc9, 0x03, 0xaf, 0x49, 0xaf
};


u8 sha2_256_seq_test_msg_p1[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e
};

u8 sha2_256_seq_test_msg_p2[] = {
    0x6f, 0x70, 0x61, 0x62, 0x63
};

u8 sha2_256_seq_test_res[] = {
    0x29, 0xf2, 0xcb, 0xa3, 0xa2, 0xda, 0xaf, 0x16,
    0x90, 0x9d, 0xe9, 0x03, 0xc3, 0x79, 0xe2, 0xf0,
    0x7b, 0x8c, 0xbb, 0x1d, 0xcc, 0x07, 0x2f, 0x03,
    0xb5, 0x7d, 0x28, 0xc9, 0x03, 0xaf, 0x49, 0xaf
};

// HMAC-MD5-128 test vectors
u8 hmac_test_key[] __attribute__((aligned(byte_align))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70
};

u8 hmac_md5_test_msg1[] = {
    0x61, 0x62, 0x63
};

u8 hmac_md5_test_res1[] = {
    0x4a, 0x05, 0xb9, 0x09, 0x7a, 0x02, 0x98, 0x77,
    0xe2, 0x76, 0x26, 0x20, 0xae, 0x6d, 0xd6, 0x12
};

u8 hmac_md5_test_msg2[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63
};

u8 hmac_md5_test_res2[] = {
    0xcf, 0x06, 0x98, 0x5f, 0x51, 0x2c, 0x94, 0x07,
    0xda, 0xe5, 0x1f, 0xd1, 0x13, 0x9a, 0xe0, 0xad
};

u8 hmac_md5_seq_test_msg_p1[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e
};

u8 hmac_md5_seq_test_msg_p2[] = {
    0x6f, 0x70, 0x61, 0x62, 0x63
};

u8 hmac_md5_seq_test_res[] = {
    0xcf, 0x06, 0x98, 0x5f, 0x51, 0x2c, 0x94, 0x07,
    0xda, 0xe5, 0x1f, 0xd1, 0x13, 0x9a, 0xe0, 0xad
};

// HMAC-SHA1-160 test vectors
u8 hmac_sha1_test_msg1[] = {
    0x61, 0x62, 0x63
};

u8 hmac_sha1_test_res1[] = {
    0x77, 0xbd, 0x91, 0x2d, 0x42, 0xaf, 0xbb, 0x1c,
    0x4e, 0x66, 0x9a, 0x8e, 0x8e, 0x86, 0xbf, 0xf0,
    0x89, 0x9e, 0x68, 0x99
};

u8 hmac_sha1_test_msg2[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63
};

u8 hmac_sha1_test_res2[] = {
    0x5d, 0x12, 0xb4, 0x5c, 0x34, 0x4a, 0x64, 0xa4,
    0xc3, 0xe2, 0xe3, 0x8a, 0xc7, 0xab, 0xb0, 0x7f,
    0xb4, 0xfa, 0x4d, 0x19
};

u8 hmac_sha1_seq_test_msg_p1[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e
};

u8 hmac_sha1_seq_test_msg_p2[] = {
    0x6f, 0x70, 0x61, 0x62, 0x63
};

u8 hmac_sha1_seq_test_res[] = {
    0x5d, 0x12, 0xb4, 0x5c, 0x34, 0x4a, 0x64, 0xa4,
    0xc3, 0xe2, 0xe3, 0x8a, 0xc7, 0xab, 0xb0, 0x7f,
    0xb4, 0xfa, 0x4d, 0x19
};

// HMAC-SHA2-224 test vectors
u8 hmac_sha2_224_test_msg1[] = {
    0x61, 0x62, 0x63
};

u8 hmac_sha2_224_test_res1[] = {
    0x87, 0x69, 0x9b, 0x98, 0x2d, 0xe9, 0xd4, 0x36,
    0x81, 0xdf, 0x89, 0x77, 0x2f, 0xb6, 0xb4, 0x09,
    0xaf, 0x8a, 0x5d, 0x0a, 0xc4, 0xb5, 0x06, 0x00,
    0xfc, 0x04, 0x8e, 0x4b
};

u8 hmac_sha2_224_test_msg2[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63
};

u8 hmac_sha2_224_test_res2[] = {
    0xe0, 0xb2, 0x4e, 0x6d, 0xad, 0xac, 0x5e, 0x52,
    0x4f, 0x9b, 0xd5, 0x4a, 0x08, 0x52, 0x9d, 0xbd,
    0x66, 0x28, 0x7c, 0xc1, 0x4e, 0x04, 0xec, 0x3e,
    0x39, 0x1e, 0x91, 0xd4
};

u8 hmac_sha2_224_seq_test_msg_p1[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e
};

u8 hmac_sha2_224_seq_test_msg_p2[] = {
    0x6f, 0x70, 0x61, 0x62, 0x63
};

u8 hmac_sha2_224_seq_test_res[] = {
    0xe0, 0xb2, 0x4e, 0x6d, 0xad, 0xac, 0x5e, 0x52,
    0x4f, 0x9b, 0xd5, 0x4a, 0x08, 0x52, 0x9d, 0xbd,
    0x66, 0x28, 0x7c, 0xc1, 0x4e, 0x04, 0xec, 0x3e,
    0x39, 0x1e, 0x91, 0xd4
};

// HMAC-SHA2-256 test vectors
u8 hmac_sha2_256_test_msg1[] = {
    0x61, 0x62, 0x63
};

u8 hmac_sha2_256_test_res1[] = {
    0xdf, 0xb2, 0x69, 0x88, 0x39, 0x91, 0x0b, 0xda,
    0x14, 0x8b, 0x97, 0xc5, 0x41, 0x4e, 0x2e, 0xb6,
    0xc2, 0x3f, 0xa4, 0x4d, 0xc3, 0x4b, 0x14, 0x29,
    0xcc, 0x78, 0x65, 0x09, 0x75, 0x94, 0xd2, 0x03
};

u8 hmac_sha2_256_test_msg2[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63
};

u8 hmac_sha2_256_test_res2[] = {
    0x08, 0x0b, 0xc5, 0xe9, 0xec, 0x87, 0xa9, 0x2e,
    0x28, 0xb5, 0x47, 0xf8, 0xc8, 0x54, 0x90, 0xaf,
    0xb6, 0x2d, 0xa3, 0x69, 0x9c, 0x37, 0x4b, 0x70,
    0x55, 0x78, 0x99, 0xe3, 0xa8, 0x31, 0xcb, 0x19
};

u8 hmac_sha2_256_seq_test_msg_p1[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e
};

u8 hmac_sha2_256_seq_test_msg_p2[] = {
    0x6f, 0x70, 0x61, 0x62, 0x63
};

u8 hmac_sha2_256_seq_test_res[] = {
    0x08, 0x0b, 0xc5, 0xe9, 0xec, 0x87, 0xa9, 0x2e,
    0x28, 0xb5, 0x47, 0xf8, 0xc8, 0x54, 0x90, 0xaf,
    0xb6, 0x2d, 0xa3, 0x69, 0x9c, 0x37, 0x4b, 0x70,
    0x55, 0x78, 0x99, 0xe3, 0xa8, 0x31, 0xcb, 0x19
};

//
// Cipher test vectors
//

// DES common test vectors
u8 des_test_msg[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70
};

u8 des_test_key[] __attribute__((aligned(byte_align))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68
};

u8 des_test_iv[] __attribute__((aligned(byte_align))) = {
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78
};

// DES-ECB test vectors
u8 des_ecb_test_res[] = {
    0x2a, 0x8d, 0x69, 0xde, 0x9d, 0x5f, 0xdf, 0xf9,
    0xbd, 0x0a, 0xac, 0x78, 0x21, 0x19, 0x7f, 0xa4,
    0x2a, 0x8d, 0x69, 0xde, 0x9d, 0x5f, 0xdf, 0xf9,
    0xbd, 0x0a, 0xac, 0x78, 0x21, 0x19, 0x7f, 0xa4
};

// DES-CBC test vectors
u8 des_cbc_test_res[] = {
    0xb1, 0x4e, 0x63, 0xb8, 0x0b, 0xcb, 0xe2, 0x4c,
    0xf6, 0x8a, 0xca, 0x80, 0xef, 0x15, 0xf5, 0xaf,
    0x52, 0xd9, 0x71, 0xe2, 0x52, 0x3d, 0x5d, 0xd6,
    0xd5, 0xb2, 0x2d, 0xbe, 0x83, 0x19, 0x2e, 0x21
};

// DES-CFB test vectors
u8 des_cfb_test_res[] = {
    0x77, 0x68, 0xf8, 0xf6, 0xf1, 0xe7, 0x07, 0xa8,
    0x81, 0x03, 0xe9, 0x3f, 0x82, 0x8e, 0xa4, 0x9c,
    0x7c, 0x47, 0x39, 0x84, 0x7f, 0x4d, 0x3f, 0x4c,
    0x8f, 0x8c, 0xfb, 0xdd, 0xfa, 0x1e, 0xad, 0xd1
};

// DES-OFB test vectors
u8 des_ofb_test_res[] = {
    0x77, 0x68, 0xf8, 0xf6, 0xf1, 0xe7, 0x07, 0xa8,
    0xf3, 0xef, 0x91, 0xab, 0x86, 0xa1, 0x8f, 0xbf,
    0xb0, 0x5e, 0x99, 0x64, 0xf8, 0xf4, 0x8e, 0x68,
    0x5a, 0xda, 0x3f, 0x51, 0xcc, 0x0f, 0xc3, 0xb7
};

// DES-CTR test vectors
u8 des_ctr_test_res[] = {
    0x77, 0x68, 0xf8, 0xf6, 0xf1, 0xe7, 0x07, 0xa8,
    0xf2, 0x01, 0x67, 0x17, 0x59, 0xf2, 0xd5, 0xb2,
    0x2a, 0x13, 0x90, 0x42, 0xfc, 0x89, 0x2d, 0x34,
    0xab, 0x4d, 0xf3, 0xef, 0x98, 0x83, 0xc0, 0x2a
};

// 3DES common test vectors
u8 trides_test_msg[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70
};

u8 trides_test_key[] __attribute__((aligned(byte_align))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68
};

u8 trides_test_iv[] __attribute__((aligned(byte_align))) = {
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78
};

// 3DES-ECB test vectors
u8 trides_ecb_test_res[] = {
    0x2a, 0x8d, 0x69, 0xde, 0x9d, 0x5f, 0xdf, 0xf9,
    0xbd, 0x0a, 0xac, 0x78, 0x21, 0x19, 0x7f, 0xa4,
    0x2a, 0x8d, 0x69, 0xde, 0x9d, 0x5f, 0xdf, 0xf9,
    0xbd, 0x0a, 0xac, 0x78, 0x21, 0x19, 0x7f, 0xa4,
    0x2a, 0x8d, 0x69, 0xde, 0x9d, 0x5f, 0xdf, 0xf9,
    0xbd, 0x0a, 0xac, 0x78, 0x21, 0x19, 0x7f, 0xa4
};

// 3DES-CBC test vectors
u8 trides_cbc_test_res[] = {
    0xb1, 0x4e, 0x63, 0xb8, 0x0b, 0xcb, 0xe2, 0x4c,
    0xf6, 0x8a, 0xca, 0x80, 0xef, 0x15, 0xf5, 0xaf,
    0x52, 0xd9, 0x71, 0xe2, 0x52, 0x3d, 0x5d, 0xd6,
    0xd5, 0xb2, 0x2d, 0xbe, 0x83, 0x19, 0x2e, 0x21,
    0x43, 0x1f, 0x60, 0x0a, 0x35, 0xd0, 0x50, 0x87,
    0xc1, 0xc7, 0x09, 0x5b, 0x67, 0xfb, 0xaf, 0x42
};

// 3DES-CFB test vectors
u8 trides_cfb_test_res[] = {
    0x77, 0x68, 0xf8, 0xf6, 0xf1, 0xe7, 0x07, 0xa8,
    0x81, 0x03, 0xe9, 0x3f, 0x82, 0x8e, 0xa4, 0x9c,
    0x7c, 0x47, 0x39, 0x84, 0x7f, 0x4d, 0x3f, 0x4c,
    0x8f, 0x8c, 0xfb, 0xdd, 0xfa, 0x1e, 0xad, 0xd1,
    0xa5, 0xda, 0xee, 0x93, 0x54, 0x83, 0xda, 0xa3,
    0xb7, 0x33, 0xb3, 0x54, 0xb9, 0x28, 0x6b, 0x5d
};

// 3DES-OFB test vectors
u8 trides_ofb_test_res[] = {
    0x77, 0x68, 0xf8, 0xf6, 0xf1, 0xe7, 0x07, 0xa8,
    0xf3, 0xef, 0x91, 0xab, 0x86, 0xa1, 0x8f, 0xbf,
    0xb0, 0x5e, 0x99, 0x64, 0xf8, 0xf4, 0x8e, 0x68,
    0x5a, 0xda, 0x3f, 0x51, 0xcc, 0x0f, 0xc3, 0xb7,
    0xb6, 0x60, 0x2d, 0x56, 0xc1, 0xa1, 0xcc, 0xbe,
    0xb5, 0x9d, 0xb4, 0xbc, 0xa0, 0xa4, 0xe4, 0x3c
};

// 3DES-CTR test vectors
u8 trides_ctr_test_res[] = {
    0x77, 0x68, 0xf8, 0xf6, 0xf1, 0xe7, 0x07, 0xa8,
    0xf2, 0x01, 0x67, 0x17, 0x59, 0xf2, 0xd5, 0xb2,
    0x2a, 0x13, 0x90, 0x42, 0xfc, 0x89, 0x2d, 0x34,
    0xab, 0x4d, 0xf3, 0xef, 0x98, 0x83, 0xc0, 0x2a,
    0x78, 0x1f, 0xdb, 0x5a, 0xd5, 0x19, 0xb8, 0x6c,
    0x0b, 0x27, 0x24, 0xd4, 0x25, 0x8e, 0xdf, 0xfe
};

// AES-ECB test vectors
u8 aes_ecb_128_test_msg[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70
};

u8 aes_ecb_128_test_key[] __attribute__((aligned(byte_align))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70
};

u8 aes_ecb_128_test_res[] = {
    0xa9, 0x13, 0x29, 0xaf, 0x99, 0xa7, 0x8d, 0x02,
    0xae, 0xc1, 0x7c, 0x50, 0x77, 0x57, 0xaa, 0xef,
    0xa9, 0x13, 0x29, 0xaf, 0x99, 0xa7, 0x8d, 0x02,
    0xae, 0xc1, 0x7c, 0x50, 0x77, 0x57, 0xaa, 0xef
};

u8 aes_ecb_192_test_msg[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68
};

u8 aes_ecb_192_test_key[] __attribute__((aligned(byte_align))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x70, 0x6f, 0x6e, 0x6d, 0x6c, 0x6b, 0x6a, 0x69
};

u8 aes_ecb_192_test_res[] = {
    0x4d, 0xd1, 0x40, 0x79, 0x0c, 0x05, 0xbd, 0x23,
    0x40, 0x1c, 0xe6, 0x62, 0x1c, 0xd0, 0x3f, 0xa8,
    0xa2, 0x32, 0xb2, 0xb4, 0x3b, 0xf3, 0xdb, 0xba,
    0x47, 0xc9, 0xb2, 0x69, 0x92, 0xa8, 0xef, 0xe6,
    0x92, 0xf9, 0xd4, 0xae, 0xdf, 0x59, 0xf4, 0xe3,
    0x95, 0xe4, 0xcd, 0x97, 0x72, 0x13, 0x02, 0xf7
};

u8 aes_ecb_256_test_msg[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70
};

u8 aes_ecb_256_test_key[] __attribute__((aligned(byte_align))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x70, 0x6f, 0x6e, 0x6d, 0x6c, 0x6b, 0x6a, 0x69,
    0x68, 0x67, 0x66, 0x65, 0x64, 0x63, 0x62, 0x61
};

u8 aes_ecb_256_test_res[] = {
    0x02, 0xfa, 0xe8, 0x39, 0x60, 0x94, 0x54, 0x86,
    0x61, 0x47, 0xcc, 0x79, 0xaa, 0xdf, 0x7b, 0x6d,
    0x02, 0xfa, 0xe8, 0x39, 0x60, 0x94, 0x54, 0x86,
    0x61, 0x47, 0xcc, 0x79, 0xaa, 0xdf, 0x7b, 0x6d,
    0x02, 0xfa, 0xe8, 0x39, 0x60, 0x94, 0x54, 0x86,
    0x61, 0x47, 0xcc, 0x79, 0xaa, 0xdf, 0x7b, 0x6d,
    0x02, 0xfa, 0xe8, 0x39, 0x60, 0x94, 0x54, 0x86,
    0x61, 0x47, 0xcc, 0x79, 0xaa, 0xdf, 0x7b, 0x6d
};

// AES-CBC test vectors
u8 aes_cbc_128_test_msg[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70
};

u8 aes_cbc_128_test_key[] __attribute__((aligned(byte_align))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70
};

u8 aes_cbc_128_test_iv[] __attribute__((aligned(byte_align))) = {
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78
};

u8 aes_cbc_128_test_res[] = {
    0x44, 0xc3, 0x7e, 0x06, 0xce, 0x2b, 0x3d, 0x00,
    0x4c, 0x99, 0x45, 0x39, 0xa4, 0xa9, 0x82, 0x95,
    0x9d, 0xa0, 0x7b, 0x14, 0x1c, 0x91, 0xfe, 0x96,
    0xdf, 0xcb, 0x60, 0x3f, 0x65, 0x0b, 0x97, 0x28
};

u8 aes_cbc_128_test_res_one[] = {
    0x5b, 0x56, 0x2b, 0x8b, 0x07, 0x61, 0xfa, 0x5e,
    0xc1, 0x3c, 0x2e, 0x71, 0x4d, 0x02, 0x0e, 0x2b,
    0x73, 0x9b, 0x2d, 0x08, 0x96, 0x19, 0xf7, 0x41,
    0x12, 0x82, 0xb4, 0x83, 0x0f, 0xe4, 0x87, 0x3a
};

u8 aes_cbc_192_test_msg[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68
};

u8 aes_cbc_192_test_key[] __attribute__((aligned(byte_align))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x70, 0x6f, 0x6e, 0x6d, 0x6c, 0x6b, 0x6a, 0x69
};

u8 aes_cbc_192_test_iv[] __attribute__((aligned(byte_align))) = {
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78
};

u8 aes_cbc_192_test_res[] = {
    0xfb, 0x8c, 0x02, 0xd5, 0xe9, 0x5c, 0xa1, 0x5c,
    0x4e, 0x04, 0xe7, 0x05, 0x49, 0x6c, 0xe5, 0xe5,
    0x76, 0x53, 0x7b, 0xf7, 0xf8, 0xd7, 0x66, 0x08,
    0x16, 0xfc, 0x24, 0xdd, 0x2b, 0x4e, 0x12, 0xe1,
    0x39, 0x0a, 0x65, 0x4e, 0x27, 0x94, 0xa0, 0x28,
    0xce, 0xb9, 0xed, 0x58, 0x76, 0x60, 0x23, 0x8b
};

u8 aes_cbc_256_test_msg[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70
};

u8 aes_cbc_256_test_key[] __attribute__((aligned(byte_align))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x70, 0x6f, 0x6e, 0x6d, 0x6c, 0x6b, 0x6a, 0x69,
    0x68, 0x67, 0x66, 0x65, 0x64, 0x63, 0x62, 0x61
};

u8 aes_cbc_256_test_iv[] __attribute__((aligned(byte_align))) = {
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78
};

u8 aes_cbc_256_test_res[] = {
    0x0d, 0x61, 0x3b, 0x54, 0x7b, 0xe7, 0xbf, 0x97,
    0xea, 0x63, 0xaf, 0xdf, 0x00, 0x0c, 0x5e, 0x1b,
    0x1e, 0xb4, 0xe7, 0xd6, 0x00, 0xc9, 0x89, 0x26,
    0x6a, 0x1a, 0x0f, 0xe2, 0xfb, 0xce, 0x8c, 0xbf,
    0xe0, 0x8c, 0xc7, 0xad, 0x65, 0x0a, 0xa7, 0x1c,
    0x1b, 0x92, 0xb2, 0x34, 0xf7, 0x35, 0x74, 0xfc,
    0x16, 0x99, 0x85, 0xec, 0xed, 0x84, 0x6a, 0x93,
    0x07, 0xad, 0x47, 0x66, 0xba, 0x43, 0xc0, 0xb8
};

// AES-CTR test vectors
u8 aes_ctr_128_test_msg[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70
};

u8 aes_ctr_128_test_key[] __attribute__((aligned(byte_align))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70
};

u8 aes_ctr_128_test_iv[] __attribute__((aligned(byte_align))) = {
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78
};

u8 aes_ctr_128_test_res[] = {
    0x1d, 0x9a, 0xf6, 0x8d, 0xbe, 0x2a, 0x6f, 0xba,
    0x66, 0xb4, 0xc1, 0xcd, 0x06, 0x29, 0xdf, 0xb8,
    0x82, 0x7f, 0xc8, 0x28, 0xa2, 0x72, 0xe9, 0xe8,
    0xb8, 0x00, 0xde, 0x89, 0xa3, 0xda, 0x2f, 0xde
};

u8 aes_ctr_192_test_msg[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68
};

u8 aes_ctr_192_test_key[] __attribute__((aligned(byte_align))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x70, 0x6f, 0x6e, 0x6d, 0x6c, 0x6b, 0x6a, 0x69
};

u8 aes_ctr_192_test_iv[] __attribute__((aligned(byte_align))) = {
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78
};

u8 aes_ctr_192_test_res[] = {
    0x22, 0x05, 0x67, 0x27, 0xb5, 0x25, 0x3c, 0xf6,
    0xd8, 0xc8, 0x8c, 0xec, 0x4d, 0xab, 0x83, 0xae,
    0xa9, 0x77, 0xb9, 0x1d, 0xe1, 0xe8, 0x56, 0x36,
    0xcb, 0x65, 0x4a, 0x27, 0x3a, 0x5a, 0x3a, 0x93,
    0x8e, 0xa6, 0x84, 0x24, 0xfd, 0x0a, 0xe3, 0x4e,
    0xc6, 0x80, 0xfd, 0x12, 0x82, 0x7e, 0x3e, 0x25
};

u8 aes_ctr_256_test_msg[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70
};

u8 aes_ctr_256_test_key[] __attribute__((aligned(byte_align))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x70, 0x6f, 0x6e, 0x6d, 0x6c, 0x6b, 0x6a, 0x69,
    0x68, 0x67, 0x66, 0x65, 0x64, 0x63, 0x62, 0x61
};

u8 aes_ctr_256_test_iv[] __attribute__((aligned(byte_align))) = {
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78
};

u8 aes_ctr_256_test_res[] = {
    0xad, 0x56, 0x6e, 0x8e, 0x3a, 0xb8, 0x3a, 0x6f,
    0xc3, 0x53, 0x4f, 0x35, 0x80, 0x42, 0x06, 0x49,
    0xce, 0xac, 0x0b, 0x44, 0x01, 0x65, 0xa9, 0x1d,
    0x3f, 0x4d, 0xa0, 0x23, 0xa7, 0x55, 0x71, 0x83,
    0xc6, 0x77, 0x7c, 0x21, 0x72, 0xfd, 0x0d, 0xd5,
    0x2e, 0xf1, 0x33, 0x79, 0x22, 0xd5, 0xcd, 0x21,
    0x6f, 0x19, 0xf6, 0x02, 0x2b, 0x6e, 0x3b, 0xf7,
    0xb4, 0x0a, 0xa8, 0x24, 0x80, 0x74, 0x22, 0x1b
};

// AES-CFB test vectors
u8 aes_cfb_128_test_msg[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70
};

u8 aes_cfb_128_test_key[] __attribute__((aligned(byte_align))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70
};

u8 aes_cfb_128_test_iv[] __attribute__((aligned(byte_align))) = {
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78
};

u8 aes_cfb_128_test_res[] = {
    0x1d, 0x9a, 0xf6, 0x8d, 0xbe, 0x2a, 0x6f, 0xba,
    0x66, 0xb4, 0xc1, 0xcd, 0x06, 0x29, 0xdf, 0xb8,
    0xc8, 0x8c, 0xe1, 0x79, 0x05, 0x1e, 0xb7, 0x24,
    0xbf, 0xeb, 0x46, 0xda, 0xe9, 0x4b, 0x65, 0x00
};

u8 aes_cfb_192_test_msg[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68
};

u8 aes_cfb_192_test_key[] __attribute__((aligned(byte_align))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x70, 0x6f, 0x6e, 0x6d, 0x6c, 0x6b, 0x6a, 0x69
};

u8 aes_cfb_192_test_iv[] __attribute__((aligned(byte_align))) = {
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78
};

u8 aes_cfb_192_test_res[] = {
    0x22, 0x05, 0x67, 0x27, 0xb5, 0x25, 0x3c, 0xf6,
    0xd8, 0xc8, 0x8c, 0xec, 0x4d, 0xab, 0x83, 0xae,
    0x36, 0x93, 0xa5, 0x1c, 0x5d, 0x2b, 0x8f, 0x3a,
    0x7d, 0x88, 0x2b, 0xac, 0xb2, 0xb1, 0x8c, 0xca,
    0xaa, 0x11, 0x20, 0x5c, 0xac, 0x18, 0xf1, 0x37,
    0xcb, 0xb8, 0x7b, 0x70, 0x45, 0x4a, 0x8f, 0xc6
};

u8 aes_cfb_256_test_msg[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70
};

u8 aes_cfb_256_test_key[] __attribute__((aligned(byte_align))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x70, 0x6f, 0x6e, 0x6d, 0x6c, 0x6b, 0x6a, 0x69,
    0x68, 0x67, 0x66, 0x65, 0x64, 0x63, 0x62, 0x61
};

u8 aes_cfb_256_test_iv[] __attribute__((aligned(byte_align))) = {
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78
};

u8 aes_cfb_256_test_res[] = {
    0xad, 0x56, 0x6e, 0x8e, 0x3a, 0xb8, 0x3a, 0x6f,
    0xc3, 0x53, 0x4f, 0x35, 0x80, 0x42, 0x06, 0x49,
    0x93, 0x1d, 0xde, 0x8f, 0x07, 0x8e, 0x9c, 0xac,
    0x2c, 0x96, 0xd6, 0x8e, 0xfd, 0x97, 0xd4, 0xcf,
    0x05, 0xe3, 0x34, 0x19, 0xdf, 0x14, 0x23, 0xea,
    0x3b, 0x4d, 0x95, 0x02, 0x5d, 0xb1, 0xce, 0x2d,
    0x3d, 0x4d, 0x1c, 0x4b, 0xb4, 0x35, 0x80, 0x85,
    0x58, 0x11, 0x17, 0x96, 0xb2, 0xf1, 0xc2, 0x8c
};

// AES-OFB test vectors
u8 aes_ofb_128_test_msg[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70
};

u8 aes_ofb_128_test_key[] __attribute__((aligned(byte_align))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70
};

u8 aes_ofb_128_test_iv[] __attribute__((aligned(byte_align))) = {
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78
};

u8 aes_ofb_128_test_res[] = {
    0x1d, 0x9a, 0xf6, 0x8d, 0xbe, 0x2a, 0x6f, 0xba,
    0x66, 0xb4, 0xc1, 0xcd, 0x06, 0x29, 0xdf, 0xb8,
    0xa6, 0xa0, 0xb2, 0x6d, 0x83, 0x34, 0x44, 0xca,
    0xb7, 0xf8, 0xb3, 0x7e, 0x00, 0x4f, 0xde, 0x3c
};

u8 aes_ofb_192_test_msg[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68
};

u8 aes_ofb_192_test_key[] __attribute__((aligned(byte_align))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x70, 0x6f, 0x6e, 0x6d, 0x6c, 0x6b, 0x6a, 0x69
};

u8 aes_ofb_192_test_iv[] __attribute__((aligned(byte_align))) = {
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78
};

u8 aes_ofb_192_test_res[] = {
    0x22, 0x05, 0x67, 0x27, 0xb5, 0x25, 0x3c, 0xf6,
    0xd8, 0xc8, 0x8c, 0xec, 0x4d, 0xab, 0x83, 0xae,
    0x74, 0xa7, 0x19, 0x38, 0x36, 0x84, 0x01, 0x2e,
    0x57, 0xca, 0xa9, 0x2d, 0x34, 0x9e, 0x57, 0xf2,
    0x0a, 0xf1, 0x4b, 0x5b, 0x60, 0x22, 0xbd, 0x2e,
    0x7b, 0x0a, 0xe2, 0x61, 0xc3, 0x24, 0xf9, 0xf6
};

u8 aes_ofb_256_test_msg[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70
};

u8 aes_ofb_256_test_key[] __attribute__((aligned(byte_align))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x70, 0x6f, 0x6e, 0x6d, 0x6c, 0x6b, 0x6a, 0x69,
    0x68, 0x67, 0x66, 0x65, 0x64, 0x63, 0x62, 0x61
};

u8 aes_ofb_256_test_iv[] __attribute__((aligned(byte_align))) = {
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78
};

u8 aes_ofb_256_test_res[] = {
    0xad, 0x56, 0x6e, 0x8e, 0x3a, 0xb8, 0x3a, 0x6f,
    0xc3, 0x53, 0x4f, 0x35, 0x80, 0x42, 0x06, 0x49,
    0x24, 0xd6, 0xd8, 0xb0, 0xed, 0xc3, 0x22, 0x5b,
    0xa1, 0xd3, 0x4d, 0xba, 0x40, 0xef, 0x1a, 0x31,
    0x68, 0x95, 0xa7, 0x8a, 0xc3, 0x3d, 0x9b, 0xce,
    0xed, 0x14, 0x62, 0x5f, 0xed, 0xce, 0xaf, 0x00,
    0xf5, 0x51, 0xdb, 0x03, 0x9d, 0xa1, 0xaa, 0x1d,
    0x0d, 0x7b, 0xab, 0x71, 0x84, 0xc2, 0x6f, 0x45
};

// AES-GCTR test vectors
u8 aes_gctr_128_test_key[] __attribute__((aligned(byte_align))) = {
    0xdd, 0x66, 0xd4, 0x5f, 0xb5, 0xf2, 0x0c, 0xf5,
    0x75, 0x69, 0xd1, 0x9c, 0x76, 0x08, 0x92, 0xe3
};

u8 aes_gctr_128_test_iv[] __attribute__((aligned(byte_align))) = {
    0x98, 0x91, 0xa9, 0x9d, 0x8a, 0x5a, 0xad, 0x15,
    0x7f, 0x9f, 0x5f, 0xc8
};

u8 aes_gctr_128_test_msg[] = {
    0xa7, 0x04, 0xdf, 0x78, 0x89, 0x3f, 0xb7, 0xa6,
    0xc9, 0xf9, 0xf8, 0x21, 0x0e, 0x40, 0xf8, 0x8c,
    0x40, 0x6a, 0x8c, 0xe6, 0x62, 0xcf, 0x78, 0xd7,
    0x9e, 0x7d, 0x60, 0x9b, 0xce, 0x08, 0x8b, 0x80
};

u8 aes_gctr_128_test_res[] = {
    0xa1, 0x6a, 0x06, 0xe2, 0xe7, 0x5c, 0x3d, 0x10,
    0xcf, 0xda, 0x5b, 0x2a, 0x7c, 0x03, 0xc5, 0xa8,
    0xcf, 0xa8, 0xe5, 0x37, 0x87, 0x30, 0x0c, 0x10,
    0x4b, 0xf4, 0x84, 0x99, 0xf1, 0x7f, 0x39, 0x76
};

u8 aes_gctr_192_test_key[] __attribute__((aligned(byte_align))) = {
    0xd1, 0x00, 0x7b, 0xd1, 0x7c, 0x6c, 0x3e, 0xc9,
    0xef, 0xba, 0x9b, 0xaa, 0x75, 0x84, 0xa9, 0xc1,
    0x2f, 0x61, 0xbc, 0x36, 0xfd, 0x8f, 0xe9, 0x5f
};

u8 aes_gctr_192_test_iv[] __attribute__((aligned(byte_align))) = {
    0x95, 0x13, 0xf4, 0x47, 0x3b, 0x2b, 0xd9, 0x41,
    0x0d, 0x2e, 0x2f, 0x1f
};

u8 aes_gctr_192_test_msg[] = {
    0x17, 0xf5, 0xcc, 0xf5, 0xdb, 0xe6, 0xe9, 0x7b,
    0x27, 0x78, 0x24, 0xf7, 0x17, 0xd0, 0x2f, 0x8b,
    0x5b, 0x19, 0x9d, 0x01, 0x44, 0x44, 0xf7, 0x2d,
    0x1a, 0x00, 0xfb, 0x83, 0x1e, 0x1a, 0x9d, 0x6f
};

u8 aes_gctr_192_test_res[] = {
    0xdb, 0x6a, 0xee, 0x7f, 0x53, 0xb8, 0x62, 0x73,
    0x23, 0xe7, 0xe7, 0x7e, 0x01, 0x55, 0x34, 0x63,
    0x30, 0x2e, 0xff, 0xc1, 0x98, 0x84, 0x55, 0x3f,
    0xcd, 0xca, 0x6d, 0x7d, 0x0c, 0x0d, 0xdd, 0x33
};

u8 aes_gctr_256_test_key[] __attribute__((aligned(byte_align))) = {
    0x58, 0x53, 0xc0, 0x20, 0x94, 0x6b, 0x35, 0xf2,
    0xc5, 0x8e, 0xc4, 0x27, 0x15, 0x2b, 0x84, 0x04,
    0x20, 0xc4, 0x00, 0x29, 0x63, 0x6a, 0xdc, 0xbb,
    0x02, 0x74, 0x71, 0x37, 0x8c, 0xfd, 0xde, 0x0f
};

u8 aes_gctr_256_test_iv[] __attribute__((aligned(byte_align))) = {
    0xee, 0xc3, 0x13, 0xdd, 0x07, 0xcc, 0x1b, 0x3e,
    0x6b, 0x06, 0x8a, 0x47
};

u8 aes_gctr_256_test_msg[] = {
    0xce, 0x74, 0x58, 0xe5, 0x6a, 0xef, 0x90, 0x61,
    0xcb, 0x0c, 0x42, 0xec, 0x23, 0x15, 0x56, 0x5e,
    0x61, 0x68, 0xf5, 0xa6, 0x24, 0x9f, 0xfd, 0x31,
    0x61, 0x0b, 0x6d, 0x17, 0xab, 0x64, 0x93, 0x5e
};

u8 aes_gctr_256_test_res[] = {
    0xea, 0xdc, 0x3b, 0x87, 0x66, 0xa7, 0x7d, 0xed,
    0x1a, 0x58, 0xcb, 0x72, 0x7e, 0xca, 0x2a, 0x97,
    0x90, 0x49, 0x6c, 0x29, 0x86, 0x54, 0xcd, 0xa7,
    0x8f, 0xeb, 0xf0, 0xda, 0x16, 0xb6, 0x90, 0x3b
};

// AES-GMAC test vectors
u8 aes_gmac_128_test_key[] __attribute__((aligned(byte_align))) = {
    0xdd, 0x66, 0xd4, 0x5f, 0xb5, 0xf2, 0x0c, 0xf5,
    0x75, 0x69, 0xd1, 0x9c, 0x76, 0x08, 0x92, 0xe3
};

u8 aes_gmac_128_test_iv[] __attribute__((aligned(byte_align))) = {
    0x98, 0x91, 0xa9, 0x9d, 0x8a, 0x5a, 0xad, 0x15,
    0x7f, 0x9f, 0x5f, 0xc8
};

u8 aes_gmac_128_test_msg[] = {
    0xa7, 0x04, 0xdf, 0x78, 0x89, 0x3f, 0xb7, 0xa6,
    0xc9, 0xf9, 0xf8, 0x21, 0x0e, 0x40, 0xf8, 0x8c,
    0x40, 0x6a, 0x8c, 0xe6, 0x62, 0xcf, 0x78, 0xd7,
    0x9e, 0x7d, 0x60, 0x9b, 0xce, 0x08, 0x8b, 0x80
};

u8 aes_gmac_128_test_aad[] __attribute__((aligned(byte_align))) = {
    0x20, 0xf4, 0xd5, 0xaa, 0x96, 0x11, 0x37, 0x41,
    0xa5, 0x8a, 0x36, 0x3f, 0x3e, 0xe6, 0xfa, 0xbd,
    0xda, 0xad, 0x6e, 0x4f
};

u8 aes_gmac_128_test_res[] = {
    0xa1, 0x6a, 0x06, 0xe2, 0xe7, 0x5c, 0x3d, 0x10,
    0xcf, 0xda, 0x5b, 0x2a, 0x7c, 0x03, 0xc5, 0xa8,
    0xcf, 0xa8, 0xe5, 0x37, 0x87, 0x30, 0x0c, 0x10,
    0x4b, 0xf4, 0x84, 0x99, 0xf1, 0x7f, 0x39, 0x76
};

u8 aes_gmac_128_test_tag[] __attribute__((aligned(byte_align))) = {
    0x40, 0xc4, 0xd7, 0x02, 0x75, 0x8c, 0x46, 0x7c,
    0x55, 0xa3, 0x8d, 0x82, 0x04, 0xd0
};

u8 aes_gmac_192_test_key[] __attribute__((aligned(byte_align))) = {
    0xd1, 0x00, 0x7b, 0xd1, 0x7c, 0x6c, 0x3e, 0xc9,
    0xef, 0xba, 0x9b, 0xaa, 0x75, 0x84, 0xa9, 0xc1,
    0x2f, 0x61, 0xbc, 0x36, 0xfd, 0x8f, 0xe9, 0x5f
};

u8 aes_gmac_192_test_iv[] __attribute__((aligned(byte_align))) = {
    0x95, 0x13, 0xf4, 0x47, 0x3b, 0x2b, 0xd9, 0x41,
    0x0d, 0x2e, 0x2f, 0x1f
};

u8 aes_gmac_192_test_msg[] = {
    0x17, 0xf5, 0xcc, 0xf5, 0xdb, 0xe6, 0xe9, 0x7b,
    0x27, 0x78, 0x24, 0xf7, 0x17, 0xd0, 0x2f, 0x8b,
    0x5b, 0x19, 0x9d, 0x01, 0x44, 0x44, 0xf7, 0x2d,
    0x1a, 0x00, 0xfb, 0x83, 0x1e, 0x1a, 0x9d, 0x6f
};

u8 aes_gmac_192_test_aad[] __attribute__((aligned(byte_align))) = {
    0xab, 0x9c, 0x76, 0xff, 0x9b, 0x1d, 0xe5, 0x50,
    0xcd, 0xe5, 0xfd, 0x42, 0xc6, 0xae, 0xae, 0x95,
    0xcb, 0xc0, 0x39, 0x5a, 0x6c, 0x39, 0xac, 0x3e,
    0xdf, 0x9d, 0xb7, 0x5b, 0x76, 0xa6, 0x68, 0x84,
    0x61, 0xb8, 0xcf, 0x2f, 0xca, 0xf2, 0xef, 0xcd,
    0x8d, 0x8d, 0xd4, 0x26, 0x5b, 0x30, 0xe2, 0x91
};

u8 aes_gmac_192_test_res[] = {
    0xdb, 0x6a, 0xee, 0x7f, 0x53, 0xb8, 0x62, 0x73,
    0x23, 0xe7, 0xe7, 0x7e, 0x01, 0x55, 0x34, 0x63,
    0x30, 0x2e, 0xff, 0xc1, 0x98, 0x84, 0x55, 0x3f,
    0xcd, 0xca, 0x6d, 0x7d, 0x0c, 0x0d, 0xdd, 0x33
};

u8 aes_gmac_192_test_tag[] __attribute__((aligned(byte_align))) = {
    0x8c, 0x41, 0x3e, 0x2f, 0x67, 0xa4, 0x12, 0x4f,
    0x1e, 0xf9, 0xd7, 0xab, 0x6c, 0x23, 0x87, 0x84
};

u8 aes_gmac_256_test_key[] __attribute__((aligned(byte_align))) = {
    0x58, 0x53, 0xc0, 0x20, 0x94, 0x6b, 0x35, 0xf2,
    0xc5, 0x8e, 0xc4, 0x27, 0x15, 0x2b, 0x84, 0x04,
    0x20, 0xc4, 0x00, 0x29, 0x63, 0x6a, 0xdc, 0xbb,
    0x02, 0x74, 0x71, 0x37, 0x8c, 0xfd, 0xde, 0x0f
};

u8 aes_gmac_256_test_iv[] __attribute__((aligned(byte_align))) = {
    0xee, 0xc3, 0x13, 0xdd, 0x07, 0xcc, 0x1b, 0x3e,
    0x6b, 0x06, 0x8a, 0x47
};

u8 aes_gmac_256_test_msg[] = {
    0xce, 0x74, 0x58, 0xe5, 0x6a, 0xef, 0x90, 0x61,
    0xcb, 0x0c, 0x42, 0xec, 0x23, 0x15, 0x56, 0x5e,
    0x61, 0x68, 0xf5, 0xa6, 0x24, 0x9f, 0xfd, 0x31,
    0x61, 0x0b, 0x6d, 0x17, 0xab, 0x64, 0x93, 0x5e
};

u8 aes_gmac_256_test_aad[] __attribute__((aligned(byte_align))) = {
    0x13, 0x89, 0xb5, 0x22, 0xc2, 0x4a, 0x77, 0x41,
    0x81, 0x70, 0x05, 0x53, 0xf0, 0x24, 0x6b, 0xba,
    0xbd, 0xd3, 0x8d, 0x6f
};

u8 aes_gmac_256_test_res[] = {
    0xea, 0xdc, 0x3b, 0x87, 0x66, 0xa7, 0x7d, 0xed,
    0x1a, 0x58, 0xcb, 0x72, 0x7e, 0xca, 0x2a, 0x97,
    0x90, 0x49, 0x6c, 0x29, 0x86, 0x54, 0xcd, 0xa7,
    0x8f, 0xeb, 0xf0, 0xda, 0x16, 0xb6, 0x90, 0x3b
};

u8 aes_gmac_256_test_tag[] __attribute__((aligned(byte_align))) = {
    0x3d, 0x49, 0xa5, 0xb3, 0x2f, 0xde, 0x7e, 0xaf,
    0xcc, 0xe9, 0x00, 0x79, 0x21, 0x7f, 0xfb, 0x57
};

// AES-GHASH test vectors
u8 aes_ghash_test_key[] __attribute__((aligned(byte_align))) = {
    0x58, 0x53, 0xc0, 0x20, 0x94, 0x6b, 0x35, 0xf2,
    0xc5, 0x8e, 0xc4, 0x27, 0x15, 0x2b, 0x84, 0x04
};

u8 aes_ghash_test_msg[] = {
    0xce, 0x74, 0x58, 0xe5, 0x6a, 0xef, 0x90, 0x61,
    0xcb, 0x0c, 0x42, 0xec, 0x23, 0x15, 0x56, 0x5e,
    0x61, 0x68, 0xf5, 0xa6, 0x24, 0x9f, 0xfd, 0x31,
    0x61, 0x0b, 0x6d, 0x17, 0xab, 0x64, 0x93, 0x5e
};

u8 aes_ghash_test_tag[] __attribute__((aligned(byte_align))) = {
    0xfc, 0x98, 0x41, 0xd5, 0xb8, 0xe9, 0x3a, 0x14,
    0x2d, 0x17, 0x70, 0xac, 0xbe, 0x95, 0xcb, 0x8a
};

// AES-GCM test vectors
u8 aes_gcm_128_test_key[] __attribute__((aligned(byte_align))) = {
    0xdd, 0x66, 0xd4, 0x5f, 0xb5, 0xf2, 0x0c, 0xf5,
    0x75, 0x69, 0xd1, 0x9c, 0x76, 0x08, 0x92, 0xe3
};

u8 aes_gcm_128_test_iv[] __attribute__((aligned(byte_align))) = {
    0x98, 0x91, 0xa9, 0x9d, 0x8a, 0x5a, 0xad, 0x15,
    0x7f, 0x9f, 0x5f, 0xc8
};

u8 aes_gcm_128_test_msg[] = {
    0xa7, 0x04, 0xdf, 0x78, 0x89, 0x3f, 0xb7, 0xa6,
    0xc9, 0xf9, 0xf8, 0x21, 0x0e, 0x40, 0xf8, 0x8c,
    0x40, 0x6a, 0x8c, 0xe6, 0x62, 0xcf, 0x78, 0xd7,
    0x9e, 0x7d, 0x60, 0x9b, 0xce, 0x08, 0x8b, 0x80
};

u8 aes_gcm_128_test_aad[] __attribute__((aligned(byte_align))) = {
    0x20, 0xf4, 0xd5, 0xaa, 0x96, 0x11, 0x37, 0x41,
    0xa5, 0x8a, 0x36, 0x3f, 0x3e, 0xe6, 0xfa, 0xbd,
    0xda, 0xad, 0x6e, 0x4f
};

u8 aes_gcm_128_test_res[] = {
    0xa1, 0x6a, 0x06, 0xe2, 0xe7, 0x5c, 0x3d, 0x10,
    0xcf, 0xda, 0x5b, 0x2a, 0x7c, 0x03, 0xc5, 0xa8,
    0xcf, 0xa8, 0xe5, 0x37, 0x87, 0x30, 0x0c, 0x10,
    0x4b, 0xf4, 0x84, 0x99, 0xf1, 0x7f, 0x39, 0x76
};

u8 aes_gcm_128_test_tag[] __attribute__((aligned(byte_align))) = {
    0x40, 0xc4, 0xd7, 0x02, 0x75, 0x8c, 0x46, 0x7c,
    0x55, 0xa3, 0x8d, 0x82, 0x04, 0xd0
};

u8 aes_gcm_192_test_key[] __attribute__((aligned(byte_align))) = {
    0xd1, 0x00, 0x7b, 0xd1, 0x7c, 0x6c, 0x3e, 0xc9,
    0xef, 0xba, 0x9b, 0xaa, 0x75, 0x84, 0xa9, 0xc1,
    0x2f, 0x61, 0xbc, 0x36, 0xfd, 0x8f, 0xe9, 0x5f
};

u8 aes_gcm_192_test_iv[] __attribute__((aligned(byte_align))) = {
    0x95, 0x13, 0xf4, 0x47, 0x3b, 0x2b, 0xd9, 0x41,
    0x0d, 0x2e, 0x2f, 0x1f
};

u8 aes_gcm_192_test_msg[] = {
    0x17, 0xf5, 0xcc, 0xf5, 0xdb, 0xe6, 0xe9, 0x7b,
    0x27, 0x78, 0x24, 0xf7, 0x17, 0xd0, 0x2f, 0x8b,
    0x5b, 0x19, 0x9d, 0x01, 0x44, 0x44, 0xf7, 0x2d,
    0x1a, 0x00, 0xfb, 0x83, 0x1e, 0x1a, 0x9d, 0x6f
};

u8 aes_gcm_192_test_aad[] __attribute__((aligned(byte_align))) = {
    0xab, 0x9c, 0x76, 0xff, 0x9b, 0x1d, 0xe5, 0x50,
    0xcd, 0xe5, 0xfd, 0x42, 0xc6, 0xae, 0xae, 0x95,
    0xcb, 0xc0, 0x39, 0x5a, 0x6c, 0x39, 0xac, 0x3e,
    0xdf, 0x9d, 0xb7, 0x5b, 0x76, 0xa6, 0x68, 0x84,
    0x61, 0xb8, 0xcf, 0x2f, 0xca, 0xf2, 0xef, 0xcd,
    0x8d, 0x8d, 0xd4, 0x26, 0x5b, 0x30, 0xe2, 0x91
};

u8 aes_gcm_192_test_res[] = {
    0xdb, 0x6a, 0xee, 0x7f, 0x53, 0xb8, 0x62, 0x73,
    0x23, 0xe7, 0xe7, 0x7e, 0x01, 0x55, 0x34, 0x63,
    0x30, 0x2e, 0xff, 0xc1, 0x98, 0x84, 0x55, 0x3f,
    0xcd, 0xca, 0x6d, 0x7d, 0x0c, 0x0d, 0xdd, 0x33
};

u8 aes_gcm_192_test_tag[] __attribute__((aligned(byte_align))) = {
    0x8c, 0x41, 0x3e, 0x2f, 0x67, 0xa4, 0x12, 0x4f,
    0x1e, 0xf9, 0xd7, 0xab, 0x6c, 0x23, 0x87, 0x84
};

u8 aes_gcm_256_test_key[] __attribute__((aligned(byte_align))) = {
    0x58, 0x53, 0xc0, 0x20, 0x94, 0x6b, 0x35, 0xf2,
    0xc5, 0x8e, 0xc4, 0x27, 0x15, 0x2b, 0x84, 0x04,
    0x20, 0xc4, 0x00, 0x29, 0x63, 0x6a, 0xdc, 0xbb,
    0x02, 0x74, 0x71, 0x37, 0x8c, 0xfd, 0xde, 0x0f
};

u8 aes_gcm_256_test_iv[] __attribute__((aligned(byte_align))) = {
    0xee, 0xc3, 0x13, 0xdd, 0x07, 0xcc, 0x1b, 0x3e,
    0x6b, 0x06, 0x8a, 0x47
};

u8 aes_gcm_256_test_msg[] = {
    0xce, 0x74, 0x58, 0xe5, 0x6a, 0xef, 0x90, 0x61,
    0xcb, 0x0c, 0x42, 0xec, 0x23, 0x15, 0x56, 0x5e,
    0x61, 0x68, 0xf5, 0xa6, 0x24, 0x9f, 0xfd, 0x31,
    0x61, 0x0b, 0x6d, 0x17, 0xab, 0x64, 0x93, 0x5e
};

u8 aes_gcm_256_test_aad[] __attribute__((aligned(byte_align))) = {
    0x13, 0x89, 0xb5, 0x22, 0xc2, 0x4a, 0x77, 0x41,
    0x81, 0x70, 0x05, 0x53, 0xf0, 0x24, 0x6b, 0xba,
    0xbd, 0xd3, 0x8d, 0x6f
};

u8 aes_gcm_256_test_res[] = {
    0xea, 0xdc, 0x3b, 0x87, 0x66, 0xa7, 0x7d, 0xed,
    0x1a, 0x58, 0xcb, 0x72, 0x7e, 0xca, 0x2a, 0x97,
    0x90, 0x49, 0x6c, 0x29, 0x86, 0x54, 0xcd, 0xa7,
    0x8f, 0xeb, 0xf0, 0xda, 0x16, 0xb6, 0x90, 0x3b
};

u8 aes_gcm_256_test_tag[] __attribute__((aligned(byte_align))) = {
    0x3d, 0x49, 0xa5, 0xb3, 0x2f, 0xde, 0x7e, 0xaf,
    0xcc, 0xe9, 0x00, 0x79, 0x21, 0x7f, 0xfb, 0x57
};

u8 keystore_rw_test_key[] __attribute__((aligned(byte_align))) = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
};

u8 keystore_test_zero[] __attribute__((aligned(byte_align))) = {
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};

u8 keystore_test_one[] __attribute__((aligned(byte_align))) = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

u8 keypadstore_rw_test_key[] __attribute__((aligned(byte_align))) = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
};

u8 keypadstore_usage_test_key[] __attribute__((aligned(byte_align))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70
};

u8 keypadstore_test_zero[] __attribute__((aligned(byte_align))) = {
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};

u8 keypadstore_test_one[] __attribute__((aligned(byte_align))) = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

s32 keypad_storage_read_cmp(u8 index, u8 *keypad_ans, u32 keypadlen)
{
    s32 ret = FAIL;
    u32 i;
    u32 keypadTmp;

    CRYPTO_Type *pcrypto = (CRYPTO_Type*)CRYPTO_BASE_ADDR;

    // input password to enable dbg
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x52;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x54;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x4B;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x32;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x33;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x37;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x39;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x43;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x4E;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x33;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x88;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x99;

    __dbg_printf("read keypad storage %d @ 0x%lx:\n", index, (size_t)(pcrypto->ipsskp_reg + index));
    for (i = 0; i < keypadlen/4; i++) { // CRYPTO_PADSIZE/4
        keypadTmp = (((u32)(keypad_ans)[0+4*i] << 24) |
                     ((u32)(keypad_ans)[1+4*i] << 16) |
                     ((u32)(keypad_ans)[2+4*i] << 8)  |
                     ((u32)(keypad_ans)[3+4*i] << 0));
        if (keypadTmp == pcrypto->ipsskp_reg[index][i]) {
            //__dbg_printf("    (u32)keypadTmp[%d] = %x\n", i, keypadTmp);
            //__dbg_printf("    p[%d] = %x\n", i, pcrypto->ipsskp_reg[index][i]);
            ret = SUCCESS;
        } else {
            __err_printf("    (u32)keypadTmp[%d] = %x\n", i, keypadTmp);
            __err_printf("    p[%d] = %x\n", i, pcrypto->ipsskp_reg[index][i]);
            ret = FAIL;
            break;
        }
    }

    // input an arbitrary value to disable dbg
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x00;

    return ret;
}

s32 keypad_storage_test(
    IN u8 storage_algo
)
{
    int ret = FAIL;

    u32 skp_index;

    u8 *key = NULL;
    u32 keylen = 0;
    u8 *message = NULL;
    u32 msglen = 0;
    u8 hw_digest[32], *sw_digest = NULL;
    u32 digestlen = 0;

    switch (storage_algo) {
        case TEST_REG_RW:
            for (skp_index = 0; skp_index < KEYPAD_STORAGE_MAX_NUM; skp_index++) {
                ret = hal_crypto_keypad_storage_write(keypadstore_rw_test_key, skp_index, 0, sizeof(keypadstore_rw_test_key));
                if (ret != SUCCESS) {
                    __err_printf("hal_crypto_keypad_storage_write fail, ret = %d\n", ret);
                    return ret;
                }
                ret = keypad_storage_read_cmp(skp_index, keypadstore_rw_test_key, sizeof(keypadstore_rw_test_key));
                if (ret != SUCCESS) {
                    __err_printf("keypad_storage_read sk %d fail, ret = %d\n", skp_index, ret);
                    return ret;
                }
            }

            break;
        case TEST_REG_USAGE:
            for (skp_index = 0; skp_index < KEYPAD_STORAGE_MAX_NUM; skp_index++) {
                // clear zero
                //ret = hal_crypto_keypad_storage_write(keypadstore_test_zero, skp_index, 0, sizeof(keypadstore_test_zero)); CHK(ret);
                //ret = keypad_storage_read_cmp(skp_index, keypadstore_test_zero, sizeof(keypadstore_test_zero)); CHK(ret);
                // write keypad reg
                ret = hal_crypto_keypad_storage_write(keypadstore_usage_test_key, skp_index, 0, sizeof(keypadstore_usage_test_key)); CHK(ret);
                ret = keypad_storage_read_cmp(skp_index, keypadstore_usage_test_key, sizeof(keypadstore_usage_test_key)); CHK(ret);

                ret = hal_crypto_keypad_storage_usage(skp_index, sizeof(keypadstore_usage_test_key)); CHK(ret);

                key = keystore_test_zero; // point to wrong key keystore_test_zero for test
                keylen = sizeof(hmac_test_key);
                message = hmac_sha2_256_test_msg1;
                msglen = sizeof(hmac_sha2_256_test_msg1);
                digestlen = 32;
                sw_digest = hmac_sha2_256_test_res1;
                ret = hal_crypto_hmac_sha2_256(message, msglen, key, keylen, hw_digest);

                if (rtlc_memcmp(hw_digest, sw_digest, digestlen) != 0) {
                    __err_printf("vector HMAC-SHA2256 fail\n");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_digest, digestlen, "hw_digest: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_digest, digestlen, "sw_digest: ");
                    return FAIL;
                }

                ret = hal_crypto_keypad_storage_deusage(skp_index); CHK(ret);
            }
            break;
        default:
            __err_printf("storage_algo(%08x): no this storage_algo\n", storage_algo);
            break;
    }

    return ret;
}

s32 key_storage_read_cmp(u8 index, u8 *key_ans, u8 keylen)
{
    s32 ret = FAIL;
    u32 i;
    u32 keyTmp;

    CRYPTO_Type *pcrypto = (CRYPTO_Type*)CRYPTO_BASE_ADDR;

    // input password to enable dbg
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x52;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x54;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x4B;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x32;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x33;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x37;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x39;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x43;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x4E;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x33;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x88;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x99;

    __dbg_printf("read key storage %d @ 0x%lx:\n", index, (size_t)(pcrypto->ipssk_reg + index));
    for (i = 0; i < keylen/4; i++) { // CRYPTO_MAX_KEY_LENGTH/4
        keyTmp = (((u32)(key_ans)[0+4*i] << 24)  |
                  ((u32)(key_ans)[1+4*i] << 16)  |
                  ((u32)(key_ans)[2+4*i] << 8) |
                  ((u32)(key_ans)[3+4*i] << 0));
        if (keyTmp == pcrypto->ipssk_reg[index][i]) {
            //__dbg_printf("    (u32)keyTmp[%d] = %x\n", i, keyTmp);
            //__dbg_printf("    p[%d] = %x\n", i, pcrypto->ipssk_reg[index][i]);
            ret = SUCCESS;
        } else {
            __err_printf("    (u32)keyTmp[%d] = %x\n", i, keyTmp);
            __err_printf("    p[%d] = %x\n", i, pcrypto->ipssk_reg[index][i]);
            ret = FAIL;
            break;
        }
    }

    // input an arbitrary value to disable dbg
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x00;

    return ret;
}

s32 otp_key_storage_write(u8 *keyin, u8 index, u32 keylen)
{
    u32 i;
    s32 ret = FAIL;
    u8 keyinTmp[CRYPTO_MAX_KEY_LENGTH];
    u32 *key_storage_otp_test_reg0 = (u32 *)0xF550B7C0; // OTP key14 simulated reg
    u32 *key_storage_otp_test_reg1 = (u32 *)0xF550B7E0; // OTP key15 simulated reg

    rtlc_memset(keyinTmp, 0x0, CRYPTO_MAX_KEY_LENGTH);
    rtlc_memcpy(keyinTmp, keyin, keylen);

    switch (index) {
        case SKDR_14:
            for (i = 0; i < 8; i++) {
                // word swap: write in opposite way
                *(key_storage_otp_test_reg0 + (7 - i)) = (((u32)(keyinTmp)[0+4*i] << 24) |
                                                          ((u32)(keyinTmp)[1+4*i] << 16) |
                                                          ((u32)(keyinTmp)[2+4*i] << 8)  |
                                                          ((u32)(keyinTmp)[3+4*i] << 0));
                //__dbg_printf("*(key_storage_otp_test_reg0 + (7 - i)) = %x\n", *(key_storage_otp_test_reg0 + (7 - i)));
                //__dbg_printf("&(*(key_storage_otp_test_reg0 + (7 - i))) = %x\n", &(*(key_storage_otp_test_reg0 + (7 - i))));
            }
            ret = SUCCESS;
            break;
        case SKDR_15:
            for (i = 0; i < 8; i++) {
                // word swap: write in opposite way
                *(key_storage_otp_test_reg1 + (7 - i)) = (((u32)(keyinTmp)[0+4*i] << 24) |
                                                          ((u32)(keyinTmp)[1+4*i] << 16) |
                                                          ((u32)(keyinTmp)[2+4*i] << 8)  |
                                                          ((u32)(keyinTmp)[3+4*i] << 0));
                //__dbg_printf("*(key_storage_otp_test_reg1 + (7 - i)) = %x\n", *(key_storage_otp_test_reg1 + (7 - i)));
                //__dbg_printf("&(*(key_storage_otp_test_reg1 + (7 - i))) = %x\n", &(*(key_storage_otp_test_reg1 + (7 - i))));
            }
            ret = SUCCESS;
            break;
        default:
            __err_printf("no this key_storage_otp_test_reg\n");
            ret = FAIL;
            break;
    }

    return ret;
}

s32 otp_key_storage_read_cmp(u8 index, u8 *key_ans, u8 keylen)
{
    s32 ret = FAIL;
    u32 i;
    u32 keyTmp;

    CRYPTO_Type *pcrypto = (CRYPTO_Type*)CRYPTO_BASE_ADDR;

    // input password to enable dbg
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x52;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x54;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x4B;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x32;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x33;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x37;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x39;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x43;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x4E;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x33;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x88;
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x99;

    index -= 14;

    __dbg_printf("read key storage %d @ 0x%lx:\n", index, (size_t)(pcrypto->ipsotp_sk_reg + index));
    for (i = 0; i < keylen/4; i++) { // CRYPTO_MAX_KEY_LENGTH/4
        keyTmp = (((u32)(key_ans)[0+4*i] << 24) |
                  ((u32)(key_ans)[1+4*i] << 16) |
                  ((u32)(key_ans)[2+4*i] << 8)  |
                  ((u32)(key_ans)[3+4*i] << 0));
        if (keyTmp == pcrypto->ipsotp_sk_reg[index][i]) {
            //__dbg_printf("    (u32)keyTmp[%d] = %x\n", i, keyTmp);
            //__dbg_printf("    p[%d] = %x\n", i, pcrypto->ipsotp_sk_reg[index][i]);
            ret = SUCCESS;
        } else {
            __err_printf("    (u32)keyTmp[%d] = %x\n", i, keyTmp);
            __err_printf("    p[%d] = %x\n", i, pcrypto->ipsotp_sk_reg[index][i]);
            ret = FAIL;
            break;
        }
    }

    // input an arbitrary value to disable dbg
    pcrypto->ipskey_storage_dbg_reg_b.dbg_read = 0x00;

    return ret;
}

s32 key_storage_test(
    IN u8 storage_algo
)
{
    int ret = FAIL;

    u32 sk_index;

    u8 *key = NULL;
    u32 keylen = 0;
    u8 *iv = NULL;
    u32 ivlen = 0;
    u8 *message = NULL;
    u32 msglen = 0;
    u8 hw_result[64], *sw_result = NULL;

    u8 hw_digest[32], *sw_digest = NULL;
    u32 digestlen = 0;

    switch (storage_algo) {
        case TEST_REG_RW:
            for (sk_index = 0; sk_index < KEY_STORAGE_MAX_NUM; sk_index++) {
                ret = hal_crypto_key_storage_write(keystore_rw_test_key, sk_index, 0, sizeof(keystore_rw_test_key));
                if (ret != SUCCESS) {
                    __err_printf("hal_crypto_key_storage_write fail, ret = %d\n", ret);
                    return ret;
                }
                ret = key_storage_read_cmp(sk_index, keystore_rw_test_key, sizeof(keystore_rw_test_key));
                if (ret != SUCCESS) {
                    __err_printf("key_storage_read sk %d fail, ret = %d\n", sk_index, ret);
                    return ret;
                }
            }
            break;
        case TEST_OTP_RW:
            for (sk_index = 14; sk_index < 16; sk_index++) {
                //ret = otp_key_storage_write(keystore_rw_test_key, sk_index, sizeof(keystore_rw_test_key));
                //if (ret != SUCCESS) {
                //    __err_printf("otp_key_storage_write fail, ret = %d\n", ret);
                //    return ret;
                //}
                ret = otp_key_storage_read_cmp(sk_index, keystore_test_one, sizeof(keystore_test_one));
                if (ret != SUCCESS) {
                    __err_printf("otp_key_storage_read_cmp sk %d fail, ret = %d\n", sk_index, ret);
                    return ret;
                }
            }
            break;
        case TEST_REG_USAGE: // use AES-CBC enc vecor for testing
            for (sk_index = 0; sk_index < KEY_STORAGE_MAX_NUM; sk_index++) {
                ret = hal_crypto_key_storage_write(aes_cbc_128_test_key, sk_index, 0, sizeof(aes_cbc_128_test_key)); CHK(ret);
                ret = key_storage_read_cmp(sk_index, aes_cbc_128_test_key, sizeof(aes_cbc_128_test_key)); CHK(ret);

                ret = hal_crypto_key_storage_usage(sk_index); CHK(ret);

                key = hmac_test_key; // point to wrong key hmac_test_key for test
                keylen = sizeof(aes_cbc_128_test_key);
                iv = aes_cbc_128_test_iv;
                ivlen = sizeof(aes_cbc_128_test_iv);
                message = aes_cbc_128_test_msg;
                msglen = sizeof(aes_cbc_128_test_msg);
                sw_result = aes_cbc_128_test_res;

                ret = hal_crypto_aes_cbc_init(key, keylen); CHK(ret);
                ret = hal_crypto_aes_cbc_encrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);

                if (rtlc_memcmp(hw_result, sw_result, msglen) != 0) {
                    __err_printf("AES256-CBC enc with key storage %d test fail\n", sk_index);
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
                    return FAIL;
                }

                ret = hal_crypto_key_storage_deusage(); CHK(ret);
            }
            break;
        case TEST_OTP_USAGE:
            for (sk_index = 14; sk_index < 16; sk_index++) {
                //ret = otp_key_storage_write(aes_cbc_128_test_key, sk_index, sizeof(aes_cbc_128_test_key)); CHK(ret);
                //ret = otp_key_storage_read_cmp(sk_index, aes_cbc_128_test_key, sizeof(aes_cbc_128_test_key)); CHK(ret);

                ret = hal_crypto_key_storage_usage(sk_index); CHK(ret);

                key = hmac_test_key; // point to wrong key hmac_test_key for test
                keylen = sizeof(aes_cbc_128_test_key);
                iv = aes_cbc_128_test_iv;
                ivlen = sizeof(aes_cbc_128_test_iv);
                message = aes_cbc_128_test_msg;
                msglen = sizeof(aes_cbc_128_test_msg);
                sw_result = aes_cbc_128_test_res_one;

                ret = hal_crypto_aes_cbc_init(key, keylen); CHK(ret);
                ret = hal_crypto_aes_cbc_encrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);

                if (rtlc_memcmp(hw_result, sw_result, msglen) != 0) {
                    __err_printf("AES256-CBC enc with otp key storage %d test fail\n", sk_index);
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
                    return FAIL;
                }

                ret = hal_crypto_key_storage_deusage(); CHK(ret);
            }
            break;
        case TEST_WRITE_BACK:
            // SHA256 writeback test
            for (sk_index = 0; sk_index < KEY_STORAGE_MAX_NUM; sk_index++) {
                ret = hal_crypto_key_storage_write(keystore_test_zero, sk_index, 0, sizeof(keystore_test_zero)); CHK(ret);
                ret = hal_crypto_key_storage_writeback(sk_index); CHK(ret);

                key = NULL;
                keylen = 0;
                message = sha2_256_test_msg1;
                msglen = sizeof(sha2_256_test_msg1);
                digestlen = 32;
                sw_digest = sha2_256_test_res1;
                ret = hal_crypto_sha2_256(message, msglen, hw_digest);

                if (rtlc_memcmp(hw_digest, sw_digest, digestlen) != 0) {
                    __err_printf("vector SHA2-256 fail\n");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_digest, digestlen, "hw_digest: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_digest, digestlen, "sw_digest: ");
                    return FAIL;
                }

                ret = key_storage_read_cmp(sk_index, sw_digest, sizeof(sha2_256_test_res1)); CHK(ret);
                if (ret != SUCCESS) {
                    __err_printf("key_storage_read sk %d fail, ret = %d\n", sk_index, ret);
                    return ret;
                }

                ret = hal_crypto_key_storage_dewriteback(); CHK(ret);
            }

            __dbg_printf("SHA2-256 writeback test PASS\n");

            // HMAC-SHA256 writeback test
            for (sk_index = 0; sk_index < KEY_STORAGE_MAX_NUM; sk_index++) {
                ret = hal_crypto_key_storage_write(keystore_test_zero, sk_index, 0, sizeof(keystore_test_zero)); CHK(ret);
                ret = hal_crypto_key_storage_writeback(sk_index); CHK(ret);

                key = hmac_test_key;
                keylen = sizeof(hmac_test_key);
                message = hmac_sha2_256_test_msg1;
                msglen = sizeof(hmac_sha2_256_test_msg1);
                digestlen = 32;
                sw_digest = hmac_sha2_256_test_res1;
                ret = hal_crypto_hmac_sha2_256(message, msglen, key, keylen, hw_digest);

                if (rtlc_memcmp(hw_digest, sw_digest, digestlen) != 0) {
                    __err_printf("vector HMAC-SHA2256 fail\n");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_digest, digestlen, "hw_digest: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_digest, digestlen, "sw_digest: ");
                    return FAIL;
                }

                ret = key_storage_read_cmp(sk_index, sw_digest, sizeof(hmac_sha2_256_test_res1)); CHK(ret);
                if (ret != SUCCESS) {
                    __err_printf("key_storage_read sk %d fail, ret = %d\n", sk_index, ret);
                    return ret;
                }

                ret = hal_crypto_key_storage_dewriteback(); CHK(ret);
            }

            __dbg_printf("HMAC-SHA2-256 writeback test PASS\n");

            break;
        default:
            __err_printf("storage_algo(%08x): no this storage_algo\n", storage_algo);
            break;
    }

    return ret;
}

#if 0
u8 _hw_orig[threads_num][32+max_auth_msglen+32] __attribute__((aligned(32)));
//u8 _hw_orig2[threads_num][32+max_auth_msglen+32] __attribute__((aligned(32)));

//u8 _hw_iv[threads_num][64] __attribute__((aligned(32)));
//u8 _hw_iv2[threads_num][64] __attribute__((aligned(32)));

u8 _hw_key[threads_num][64] __attribute__((aligned(32)));
//u8 _hw_aad[threads_num][128] __attribute__((aligned(32)));

u8 _hw_hash_key[threads_num][64] __attribute__((aligned(32)));

////u8 _hw_auth_digest[threads_num][64];
////u8 _sw_auth_digest[threads_num][64];

//u8 _hw_result[threads_num][32+max_auth_msglen+32];
//u8 _sw_result[threads_num][32+max_auth_msglen+32];

//u8 _hw_tag[threads_num][32+32];
//u8 _hw_tag2[threads_num][32+32];

//u8 _sw_tag[threads_num][32+32];
//u8 _sw_tag2[threads_num][32+32];

u32 count_fail[threads_num] = {0};
#endif

u8 mix_cipher_key_in[max_cipher_keylen] __attribute__((aligned(byte_align)));
u8 mix_auth_key_in[max_auth_keylen] __attribute__((aligned(byte_align)));
u8 mix_iv_in[max_cipher_ivlen] __attribute__((aligned(byte_align)));
u8 mix_aad_in[max_cipher_aadlen] __attribute__((aligned(byte_align)));
u8 mix_msg_in[byte_align + max_cipher_msglen + byte_align] __attribute__((aligned(byte_align)));
u8 hw_mix_cipher_msg_out[byte_align + max_cipher_msglen + byte_align] __attribute__((aligned(byte_align)));
u8 hw_mix_cipher_msg_out2[byte_align + max_cipher_msglen + byte_align] __attribute__((aligned(byte_align)));
u8 hw_mix_auth_msg_out[byte_align + max_auth_digestlen + byte_align] __attribute__((aligned(byte_align)));
u8 hw_mix_auth_msg_out2[byte_align + max_auth_digestlen + byte_align] __attribute__((aligned(byte_align)));
u8 sw_mix_cipher_msg_out[max_cipher_msglen] __attribute__((aligned(byte_align)));
u8 sw_mix_cipher_msg_out2[max_cipher_msglen] __attribute__((aligned(byte_align)));
u8 sw_mix_auth_msg_out[max_auth_digestlen] __attribute__((aligned(byte_align)));
u8 sw_mix_auth_msg_out2[max_auth_digestlen] __attribute__((aligned(byte_align)));

u32 throughput_mix_test(
    IN u8 mix_mode,
    IN u8 mix_algo,
    IN u32 mix_loop,
    IN u8 keylen_in
)
{
    s32 ret = FAIL;
    u32 i, cnt_i;
    u32 enc_startCount = 0, enc_endCount = 0, enc_totalCount = 0;
    u32 dec_startCount = 0, dec_endCount = 0, dec_totalCount = 0;

    u8 *cipher_key = NULL;
    u8 *auth_key = NULL;
    u32 cipher_keylen = 0, auth_keylen = 0;
    u8 *iv = NULL;
    u32 ivlen = 0;
    u8 *aad = NULL;
    u32 aadlen = 0;
    u8 *message = NULL;
    u32 msglen = 0;
    u8 *hw_result = NULL;
    u8 *hw_tag = NULL;

    u8 cipher_type = 0, auth_type = 0;
    u8 cipher_type_index, auth_type_index;
    u8 cipher_type_list[5] = { CIPHER_TYPE_AES_ECB,
                               CIPHER_TYPE_AES_CBC,
                               CIPHER_TYPE_AES_CFB,
                               CIPHER_TYPE_AES_OFB,
                               CIPHER_TYPE_AES_CTR };
    u8 auth_type_list[8] = { AUTH_TYPE_MD5,
                             AUTH_TYPE_SHA1,
                             AUTH_TYPE_SHA2 | AUTH_TYPE_SHA2_224,
                             AUTH_TYPE_SHA2 | AUTH_TYPE_SHA2_256,
                             AUTH_TYPE_HMAC_MD5,
                             AUTH_TYPE_HMAC_SHA1,
                             AUTH_TYPE_HMAC_SHA2 | AUTH_TYPE_SHA2_224,
                             AUTH_TYPE_HMAC_SHA2 | AUTH_TYPE_SHA2_256 };

    switch (mix_algo) {
        case TEST_MIX_SSH:
        case TEST_MIX_ESP:
        case TEST_MIX_TLS:
            cipher_type_index = 0;
            cipher_type = cipher_type_list[cipher_type_index];
            auth_type_index = 0;
            auth_type = auth_type_list[auth_type_index];

            cipher_key = mix_cipher_key_in;
            cipher_keylen = keylen_in;
            //rtlc_memset(cipher_key, 0x5a, cipher_keylen);
            if (auth_type == AUTH_TYPE_HMAC_MD5 ||
                auth_type == AUTH_TYPE_HMAC_SHA1 ||
                auth_type == (AUTH_TYPE_HMAC_SHA2 | AUTH_TYPE_SHA2_224) ||
                auth_type == (AUTH_TYPE_HMAC_SHA2 | AUTH_TYPE_SHA2_256) ) {
                auth_key = mix_auth_key_in;
                auth_keylen = thr_mix_auth_keylen;
                //rtlc_memset(auth_key, 0x5a, auth_keylen);
            }
            if (cipher_type != CIPHER_TYPE_AES_ECB) {
                ivlen = thr_mix_ivlen;
            }
            aad = mix_aad_in;
            aadlen = thr_mix_aadlen;
            //rtlc_memset(aad, 0x5a, aadlen);
            message = mix_msg_in;
            msglen = thr_mix_msglen;
            //rtlc_memset(message, 0x5a, msglen);
            hw_result = hw_mix_cipher_msg_out;
            hw_tag = hw_mix_auth_msg_out;
            break;
        default:
            __err_printf("mix_algo(%08x): no this mix_algo\n", mix_algo);
            break;
    }

    __info_printf("cipher_keylen = %d\n", cipher_keylen);
    __info_printf("auth_keylen = %d\n", auth_keylen);
    __info_printf("msglen = %d\n", msglen);

    // test start
    // encryption
    // reset counter
    cnt_i = read_sysreg(PMCR_EL0) | PMCR_EL0_P | PMCR_EL0_C;
    write_sysreg(cnt_i, PMCR_EL0);
    // enable
    cnt_i = PMCR_EL0_E | read_sysreg(PMCR_EL0);
    write_sysreg(cnt_i, PMCR_EL0);
    enc_startCount = read_sysreg(PMCCNTR_EL0);
    // counter enabble
    write_sysreg(PMCNTENSET_EL0_C, PMCNTENSET_EL0);

    switch (mix_algo) {
        case TEST_MIX_SSH:
            for (i = 0; i< mix_loop; i++) {
                ret = hal_crypto_mix_mode_init(cipher_type, auth_type,
                    cipher_key, cipher_keylen, auth_key, auth_keylen); CHK(ret);
                ret = hal_crypto_mix_ssh_encrypt(cipher_type, message, msglen,
                    iv, ivlen, aad, aadlen, auth_type, hw_result, hw_tag); CHK(ret);
            }
            break;
        case TEST_MIX_ESP:
            for (i = 0; i< mix_loop; i++) {
                ret = hal_crypto_mix_mode_init(cipher_type, auth_type,
                    cipher_key, cipher_keylen, auth_key, auth_keylen); CHK(ret);
                ret = hal_crypto_mix_esp_encrypt(cipher_type, message, msglen,
                    iv, ivlen, aad, aadlen, auth_type, hw_result, hw_tag); CHK(ret);
            }
            break;
        case TEST_MIX_TLS:
            for (i = 0; i< mix_loop; i++) {
                ret = hal_crypto_mix_mode_init(cipher_type, auth_type,
                    cipher_key, cipher_keylen, auth_key, auth_keylen); CHK(ret);
                ret = hal_crypto_mix_ssl_tls_encrypt(cipher_type, message, msglen,
                    iv, ivlen, aad, aadlen, auth_type, hw_result); CHK(ret);
            }
        default:
            __err_printf("mix_algo(%08x): no this mix_algo\n", mix_algo);
    }

    enc_endCount = read_sysreg(PMCCNTR_EL0);
    __info_printf("(enc_startCount, enc_endCount) = (%x, %x)\n", enc_startCount, enc_endCount);
    enc_totalCount = enc_endCount - enc_startCount;
    __info_printf("enc_totalCount = %d\n", enc_totalCount);

    // decryption
    // reset counter
    cnt_i = read_sysreg(PMCR_EL0) | PMCR_EL0_P | PMCR_EL0_C;
    write_sysreg(cnt_i, PMCR_EL0);
    // enable
    cnt_i = PMCR_EL0_E | read_sysreg(PMCR_EL0);
    write_sysreg(cnt_i, PMCR_EL0);
    dec_startCount = read_sysreg(PMCCNTR_EL0);
    // counter enabble
    write_sysreg(PMCNTENSET_EL0_C, PMCNTENSET_EL0);

    switch (mix_algo) {
        case TEST_MIX_SSH:
            for (i = 0; i< mix_loop; i++) {
                ret = hal_crypto_mix_mode_init(cipher_type, auth_type,
                    cipher_key, cipher_keylen, auth_key, auth_keylen); CHK(ret);
                ret = hal_crypto_mix_ssh_decrypt(cipher_type, message, msglen,
                    iv, ivlen, aad, aadlen, auth_type, hw_result, hw_tag); CHK(ret);
            }
            break;
        case TEST_MIX_ESP:
            for (i = 0; i< mix_loop; i++) {
                ret = hal_crypto_mix_mode_init(cipher_type, auth_type,
                    cipher_key, cipher_keylen, auth_key, auth_keylen); CHK(ret);
                ret = hal_crypto_mix_esp_decrypt(cipher_type, message, msglen,
                    iv, ivlen, aad, aadlen, auth_type, hw_result, hw_tag); CHK(ret);
            }
            break;
        case TEST_MIX_TLS:
            // no decryption
            __info_printf("no encryption in TLS mode\n");
            break;
        default:
            __err_printf("mix_algo(%08x): no this mix_algo\n", mix_algo);
    }

    dec_endCount = read_sysreg(PMCCNTR_EL0);
    __info_printf("(dec_startCount, dec_endCount) = (%x, %x)\n", dec_startCount, dec_endCount);
    dec_totalCount = dec_endCount - dec_startCount;
    __info_printf("dec_totalCount = %d\n", dec_totalCount);

    return (enc_totalCount + dec_totalCount);
}

s32 random_mix_test(
    IN u8 mix_mode,
    IN u8 mix_algo
)
{
    s32 ret = FAIL;
    u32 i;

    u8 *cipher_key = NULL;
    u8 *auth_key = NULL;
    u32 cipher_keylen = 0, auth_keylen = 0;
    u8 *iv = NULL;
    u32 ivlen = 0;
    u8 *aad = NULL;
    u32 aadlen = 0;
    u8 *message_ori = NULL;
    u8 *hw_message_dec = NULL, *sw_message_dec = NULL;
    u32 src_pos = 0, dst_pos = 0;
    u32 msglen = 0;
    u8 *hw_result = NULL;
    u8 *hw_result2 = NULL;
    u8 *sw_result = NULL;
    u8 *sw_result2 = NULL;
    u8 *hw_tag = NULL;
    u8 *hw_tag2 = NULL;
    u8 *sw_tag = NULL;
    u8 *sw_tag2 = NULL;
    u32 taglen = 0;

    u8 cipher_type = 0, auth_type = 0;
    u8 cipher_type_index, auth_type_index;
    //u8 cipher_type_list[5] = {0x20, 0x21, 0x22, 0x23, 0x24};
    //u8 auth_type_list[8] = {0x2, 0x0, 0x11, 0x21, 0x6, 0x4, 0x15, 0x25};

    u8 cipher_type_list[5] = { CIPHER_TYPE_AES_ECB,
                               CIPHER_TYPE_AES_CBC,
                               CIPHER_TYPE_AES_CFB,
                               CIPHER_TYPE_AES_OFB,
                               CIPHER_TYPE_AES_CTR };
    u8 auth_type_list[8] = { AUTH_TYPE_MD5,
                             AUTH_TYPE_SHA1,
                             AUTH_TYPE_SHA2 | AUTH_TYPE_SHA2_224,
                             AUTH_TYPE_SHA2 | AUTH_TYPE_SHA2_256,
                             AUTH_TYPE_HMAC_MD5,
                             AUTH_TYPE_HMAC_SHA1,
                             AUTH_TYPE_HMAC_SHA2 | AUTH_TYPE_SHA2_224,
                             AUTH_TYPE_HMAC_SHA2 | AUTH_TYPE_SHA2_256 };

    cipher_type_index = rtlc_random() % 5;
    cipher_type = cipher_type_list[cipher_type_index];

    auth_type_index = rtlc_random() % 8;
    auth_type = auth_type_list[auth_type_index];

    // generate random cipher key
    cipher_keylen = ((rtlc_random() % 3) + 2) * 8; // 16, 24, 32
    for (i = 0; i < cipher_keylen; i++) {
        mix_cipher_key_in[i] = rtlc_random() & 0xFF;
    }

    // generate random auth key
    if (auth_type == AUTH_TYPE_HMAC_MD5 ||
        auth_type == AUTH_TYPE_HMAC_SHA1 ||
        auth_type == (AUTH_TYPE_HMAC_SHA2 | AUTH_TYPE_SHA2_224) ||
        auth_type == (AUTH_TYPE_HMAC_SHA2 | AUTH_TYPE_SHA2_256) ) {
        auth_keylen = rtlc_random() % 64 + 1;
        for (i = 0; i < auth_keylen; i++) {
            mix_auth_key_in[i] = rtlc_random() & 0xFF;
        }
    }

    // generate random iv
    if (cipher_type == CIPHER_TYPE_AES_ECB) {
        ivlen = 0;
    } else {
        ivlen = 16;
    }
    for (i = 0; i < ivlen; i++) {
        mix_iv_in[i] = rtlc_random() & 0xFF;
    }

    // generate random aad
    while ((aadlen % 16) == 0) {
        aadlen = rtlc_random() % 112 + 1;
    }
    for (i = 0; i < aadlen; i++) {
        mix_aad_in[i] = rtlc_random() & 0xFF;
    }

    // generate random message for non-alignment case
    src_pos = rtlc_random() % byte_align;
    msglen = (((rtlc_random() % (max_cipher_msglen)) / 16) + 1) * 16;
    for (i = 0; i < msglen; i++) {
        mix_msg_in[src_pos + i] = rtlc_random() & 0xFF;
    }

    // prepare hw result for non-alignment case & sw result
    dst_pos = rtlc_random() % byte_align;
    hw_result = &hw_mix_cipher_msg_out[dst_pos];
    hw_result2 = &hw_mix_cipher_msg_out2[dst_pos];
    hw_tag = &hw_mix_auth_msg_out[dst_pos];
    hw_tag2 = &hw_mix_auth_msg_out2[dst_pos];
    __dbg_printf("msglen = %d, src_pos = %d, dst_pos = %d\r\n", msglen, src_pos, dst_pos);
    sw_result = sw_mix_cipher_msg_out;
    sw_result2 = sw_mix_cipher_msg_out2;
    sw_tag = sw_mix_auth_msg_out;
    sw_tag2 = sw_mix_auth_msg_out2;
    rtlc_memset(sw_result, 0x0, max_cipher_msglen);
    rtlc_memset(sw_tag, 0x0, max_auth_digestlen);
    rtlc_memset(sw_tag2, 0x0, max_auth_digestlen);

    // set parameters for sw usage
    u8 sw_mix_algo_enc = 0, sw_mix_algo_dec = 0;
    u8 sw_cipher_type_enc = 0, sw_cipher_type_dec = 0;
    u8 sw_auth_type = 0;

    if (mix_algo == TEST_MIX_SSH) {
        sw_mix_algo_enc = CRYPTO_MIX_MODE_SW_SSH_ENC;
        sw_mix_algo_dec = CRYPTO_MIX_MODE_SW_SSH_DEC;
    } else if (mix_algo == TEST_MIX_ESP) {
        sw_mix_algo_enc = CRYPTO_MIX_MODE_SW_ESP_ENC;
        sw_mix_algo_dec = CRYPTO_MIX_MODE_SW_ESP_DEC;
    }

    if (cipher_type == CIPHER_TYPE_AES_ECB) {
        sw_cipher_type_enc = SWENCRYPT_ECB_AES;
        sw_cipher_type_dec = SWDECRYPT_ECB_AES;
    } else if (cipher_type == CIPHER_TYPE_AES_CBC) {
        sw_cipher_type_enc = SWENCRYPT_CBC_AES;
        sw_cipher_type_dec = SWDECRYPT_CBC_AES;
    } else if (cipher_type == CIPHER_TYPE_AES_CFB) {
        sw_cipher_type_enc = SWENCRYPT_CFB_AES;
        sw_cipher_type_dec = SWDECRYPT_CFB_AES;
    } else if (cipher_type == CIPHER_TYPE_AES_OFB) {
        sw_cipher_type_enc = SWENCRYPT_OFB_AES;
        sw_cipher_type_dec = SWDECRYPT_OFB_AES;
    } else if (cipher_type == CIPHER_TYPE_AES_CTR) {
        sw_cipher_type_enc = SWENCRYPT_CTR_AES;
        sw_cipher_type_dec = SWDECRYPT_CTR_AES;
    }

    if (auth_type == AUTH_TYPE_MD5) {
        sw_auth_type = CRYPTO_AUTH_MD5;
        taglen = 16;
    } else if (auth_type == AUTH_TYPE_SHA1) {
        sw_auth_type = CRYPTO_AUTH_SHA1;
        taglen = 20;
    } else if (auth_type == (AUTH_TYPE_SHA2 | AUTH_TYPE_SHA2_224)) {
        sw_auth_type = CRYPTO_AUTH_SHA2_224;
        taglen = 28;
    } else if (auth_type == (AUTH_TYPE_SHA2 | AUTH_TYPE_SHA2_256)) {
        sw_auth_type = CRYPTO_AUTH_SHA2_256;
        taglen = 32;
    } else if (auth_type == AUTH_TYPE_HMAC_MD5) {
        sw_auth_type = CRYPTO_AUTH_HMAC_MD5;
        taglen = 16;
    } else if (auth_type == AUTH_TYPE_HMAC_SHA1) {
        sw_auth_type = CRYPTO_AUTH_HMAC_SHA1;
        taglen = 20;
    } else if (auth_type == (AUTH_TYPE_HMAC_SHA2 | AUTH_TYPE_SHA2_224)) {
        sw_auth_type = CRYPTO_AUTH_HMAC_SHA2_224;
        taglen = 28;
    } else if (auth_type == (AUTH_TYPE_HMAC_SHA2 | AUTH_TYPE_SHA2_256)) {
        sw_auth_type = CRYPTO_AUTH_HMAC_SHA2_256;
        taglen = 32;
    }

    // test start
    // encryption
    switch (mix_algo) {
        case TEST_MIX_SSH:
            cipher_key = mix_cipher_key_in;
            auth_key = mix_auth_key_in;
            if (cipher_type == CIPHER_TYPE_AES_ECB) {
                iv = NULL;
            } else {
                iv = mix_iv_in;
            }
            aad = mix_aad_in;
            message_ori = mix_msg_in;
            // hw computation
            ret = hal_crypto_mix_mode_init(cipher_type, auth_type,
                cipher_key, cipher_keylen, auth_key, auth_keylen); CHK(ret);
            ret = hal_crypto_mix_ssh_encrypt(cipher_type, message_ori, msglen,
                iv, ivlen, aad, aadlen, auth_type, hw_result, hw_tag);
            // sw computation
            rtl_crypto_swvrf_init();
            rtl_crypto_swvrf_mix_process(sw_mix_algo_enc, sw_cipher_type_enc,
                sw_auth_type, message_ori, msglen, cipher_key, cipher_keylen, iv, ivlen,
                aad, aadlen, auth_key, auth_keylen, sw_result, sw_tag, taglen);
            break;
        case TEST_MIX_ESP:
            cipher_key = mix_cipher_key_in;
            auth_key = mix_auth_key_in;
            if (cipher_type == CIPHER_TYPE_AES_ECB) {
                iv = NULL;
            } else {
                iv = mix_iv_in;
            }
            aad = mix_aad_in;
            message_ori = mix_msg_in;
            // hw computation
            ret = hal_crypto_mix_mode_init(cipher_type, auth_type,
                cipher_key, cipher_keylen, auth_key, auth_keylen); CHK(ret);
            ret = hal_crypto_mix_esp_encrypt(cipher_type, message_ori, msglen,
                iv, ivlen, aad, aadlen, auth_type, hw_result, hw_tag);
            // sw computation
            rtl_crypto_swvrf_init();
            rtl_crypto_swvrf_mix_process(sw_mix_algo_enc, sw_cipher_type_enc,
                sw_auth_type, message_ori, msglen, cipher_key, cipher_keylen, iv, ivlen,
                aad, aadlen, auth_key, auth_keylen, sw_result, sw_tag, taglen);
            break;
        #if 0
        case TEST_MIX_TLS:
            cipher_key = mix_cipher_key_in;
            auth_key = mix_auth_key_in;
            iv = mix_iv_in;
            aad = mix_aad_in;
            message_ori = mix_msg_in;
            // hw computation
            ret = hal_crypto_mix_mode_init(cipher_type, auth_type,
                cipher_key, cipher_keylen, auth_key, auth_keylen); CHK(ret);
            ret = hal_crypto_mix_ssl_tls_encrypt(cipher_type, message_ori, msglen,
                iv, ivlen, aad, aadlen, auth_type, hw_result);
            // sw computation
            //
            break;
        #endif
        default:
            __err_printf("mix_algo(%08x): no this mix_algo\n", mix_algo);
    }

    if (ret == SUCCESS) {
        if (rtlc_memcmp(hw_result, sw_result, msglen) == 0) {
            __dbg_printf("random mix enc_result correct, mix_algo = %08x\n", mix_algo);
            if (rtlc_memcmp(hw_tag, sw_tag, taglen) == 0) {
                __dbg_printf("random mix enc_tag correct, mix_algo = %08x\n", mix_algo);
                //return SUCCESS;
            } else {
                __err_printf("random mix enc_tag incorrect, mix_algo = %08x\n", mix_algo);
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)cipher_key, cipher_keylen, "cipher_key: ");
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)auth_key, auth_keylen, "auth_key: ");
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)aad, aadlen, "aad: ");
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)message_ori, msglen, "message_ori: ");
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_tag, taglen, "hw_tag: ");
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_tag, taglen, "sw_tag: ");
                return FAIL;
            }
        } else {
            __err_printf("random mix enc_result incorrect, mix_algo = %08x\n", mix_algo);
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)cipher_key, cipher_keylen, "cipher_key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)auth_key, auth_keylen, "auth_key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)aad, aadlen, "aad: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message_ori, msglen, "message_ori: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
        }
    } else {
        __err_printf("Crypto Engine random mix enc compute fail, ret = %d\n", ret);
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)cipher_key, cipher_keylen, "cipher_key: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)auth_key, auth_keylen, "auth_key: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)aad, aadlen, "aad: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)message_ori, msglen, "message_ori: ");
        //__dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
        //__dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
        //__dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_tag, taglen, "hw_tag: ");
        //__dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_tag, taglen, "sw_tag: ");
        return FAIL;
    }

    // decryption
    switch (mix_algo) {
        case TEST_MIX_SSH:
            cipher_key = mix_cipher_key_in;
            auth_key = mix_auth_key_in;
            if (cipher_type == CIPHER_TYPE_AES_ECB) {
                iv = NULL;
            } else {
                iv = mix_iv_in;
            }
            aad = mix_aad_in;
            hw_message_dec = hw_result;
            // hw computation
            ret = hal_crypto_mix_mode_init(cipher_type, auth_type,
                cipher_key, cipher_keylen, auth_key, auth_keylen); CHK(ret);
            ret = hal_crypto_mix_ssh_decrypt(cipher_type, hw_message_dec, msglen,
                iv, ivlen, aad, aadlen, auth_type, hw_result2, hw_tag2);
            // sw computation
            sw_message_dec = sw_result;
            rtl_crypto_swvrf_mix_process(sw_mix_algo_dec, sw_cipher_type_dec,
                sw_auth_type, sw_message_dec, msglen, cipher_key, cipher_keylen, iv, ivlen,
                aad, aadlen, auth_key, auth_keylen, sw_result2, sw_tag2, taglen);
            break;
        case TEST_MIX_ESP:
            cipher_key = mix_cipher_key_in;
            auth_key = mix_auth_key_in;
            if (cipher_type == CIPHER_TYPE_AES_ECB) {
                iv = NULL;
            } else {
                iv = mix_iv_in;
            }
            aad = mix_aad_in;
            hw_message_dec = hw_result;
            // hw computation
            ret = hal_crypto_mix_mode_init(cipher_type, auth_type,
                cipher_key, cipher_keylen, auth_key, auth_keylen); CHK(ret);
            ret = hal_crypto_mix_esp_decrypt(cipher_type, hw_message_dec, msglen,
                iv, ivlen, aad, aadlen, auth_type, hw_result2, hw_tag2);
            // sw computation
            sw_message_dec = sw_result;
            rtl_crypto_swvrf_mix_process(sw_mix_algo_dec, sw_cipher_type_dec,
                sw_auth_type, sw_message_dec, msglen, cipher_key, cipher_keylen, iv, ivlen,
                aad, aadlen, auth_key, auth_keylen, sw_result2, sw_tag2, taglen);
            break;
        default:
            __err_printf("mix_algo(%08x): no this mix_algo\n", mix_algo);
    }

    if (ret == SUCCESS) {
        if (rtlc_memcmp(hw_result2, sw_result2, msglen) == 0) {
            __dbg_printf("random mix dec_result correct, mix_algo = %08x\n", mix_algo);
            if (rtlc_memcmp(hw_tag2, sw_tag2, taglen) == 0) {
                __dbg_printf("random mix dec_tag correct, mix_algo = %08x\n", mix_algo);
                return SUCCESS;
            } else {
                __err_printf("random mix dec_tag fail, mix_algo = %08x\n", mix_algo);
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)cipher_key, cipher_keylen, "cipher_key: ");
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)auth_key, auth_keylen, "auth_key: ");
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)aad, aadlen, "aad: ");
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)message_ori, msglen, "message_ori: ");
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_tag, taglen, "hw_tag: ");
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_tag2, taglen, "hw_tag2: ");
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_tag, taglen, "sw_tag: ");
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_tag2, taglen, "sw_tag2: ");
                return FAIL;
            }
        } else {
            __err_printf("random mix dec_result incorrect, mix_algo = %08x\n", mix_algo);
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)cipher_key, cipher_keylen, "cipher_key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)auth_key, auth_keylen, "auth_key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)aad, aadlen, "aad: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message_ori, msglen, "message_ori: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
        }
    } else {
        __err_printf("Crypto Engine random mix dec compute fail, ret = %d\n", ret);
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)cipher_key, cipher_keylen, "cipher_key: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)auth_key, auth_keylen, "auth_key: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)aad, aadlen, "aad: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)message_ori, msglen, "message_ori: ");
        //__dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
        //__dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
        //__dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_tag, taglen, "hw_tag: ");
        //__dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_tag2, taglen, "hw_tag2: ");
        //__dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_tag, taglen, "sw_tag: ");
        //__dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_tag2, taglen, "sw_tag2: ");
        return FAIL;
    }

    // return FAIL;
}

u8 cipher_key_in[max_cipher_keylen] __attribute__((aligned(byte_align)));
u8 cipher_iv_in[max_cipher_ivlen] __attribute__((aligned(byte_align)));
u8 cipher_aad_in[max_cipher_aadlen] __attribute__((aligned(byte_align)));
u8 cipher_msg_in[byte_align + max_cipher_msglen + byte_align] __attribute__((aligned(byte_align)));
u8 hw_cipher_msg_out[byte_align + max_cipher_msglen + byte_align] __attribute__((aligned(byte_align)));

u32 throughput_cipher_des_test(
    IN u8 thr_algo,
    IN u32 thr_loop
)
{
    s32 ret = FAIL;
    u32 i, cnt_i;
    u32 enc_startCount = 0, enc_endCount = 0, enc_totalCount = 0;
    u32 dec_startCount = 0, dec_endCount = 0, dec_totalCount = 0;

    u8 *key = NULL;
    u32 keylen = 0;
    u8 *iv = NULL;
    u32 ivlen = 0;
    u8 *message = NULL;
    u32 msglen = 0;
    u8 *hw_result = NULL;

    switch (thr_algo) {
        case TEST_CIPHER_DES_ECB:
            key = cipher_key_in;
            keylen = thr_cipher_des_keylen;
            //rtlc_memset(key, 0x5a, keylen);
            iv = NULL;
            ivlen = 0;
            message = cipher_msg_in;
            msglen = thr_cipher_des_msglen;
            //rtlc_memset(message, 0x5a, msglen);
            break;
        case TEST_CIPHER_DES_CBC:
        case TEST_CIPHER_DES_CFB:
        case TEST_CIPHER_DES_OFB:
        case TEST_CIPHER_DES_CTR:
            key = cipher_key_in;
            keylen = thr_cipher_des_keylen;
            //rtlc_memset(key, 0x5a, keylen);
            iv = cipher_iv_in;
            ivlen = thr_cipher_des_ivlen;
            //rtlc_memset(iv, 0x5a, ivlen);
            message = cipher_msg_in;
            msglen = thr_cipher_des_msglen;
            //rtlc_memset(message, 0x5a, msglen);
            break;
        default:
            __err_printf("thr_algo(%08x): no this thr_algo\n", thr_algo);
            break;
    }

    hw_result = hw_cipher_msg_out;

    __info_printf("keylen = %d\n", keylen);
    __info_printf("ivlen = %d\n", ivlen);
    __info_printf("msglen = %d\n", msglen);

    // test start
    // encryption
    // reset counter
    cnt_i = read_sysreg(PMCR_EL0) | PMCR_EL0_P | PMCR_EL0_C;
    write_sysreg(cnt_i, PMCR_EL0);
    // enable
    cnt_i = PMCR_EL0_E | read_sysreg(PMCR_EL0);
    write_sysreg(cnt_i, PMCR_EL0);
    enc_startCount = read_sysreg(PMCCNTR_EL0);
    // counter enabble
    write_sysreg(PMCNTENSET_EL0_C, PMCNTENSET_EL0);

    switch (thr_algo) {
        case TEST_CIPHER_DES_ECB:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_des_ecb_init(key, keylen); CHK(ret);
                ret = hal_crypto_des_ecb_encrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_DES_CBC:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_des_cbc_init(key, keylen); CHK(ret);
                ret = hal_crypto_des_cbc_encrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_DES_CFB:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_des_cfb_init(key, keylen); CHK(ret);
                ret = hal_crypto_des_cfb_encrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_DES_OFB:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_des_ofb_init(key, keylen); CHK(ret);
                ret = hal_crypto_des_ofb_encrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_DES_CTR:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_des_ofb_init(key, keylen); CHK(ret);
                ret = hal_crypto_des_ofb_encrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        default:
            __err_printf("thr_algo(%08x): no this thr_algo\n", thr_algo);
            break;
    }

    enc_endCount = read_sysreg(PMCCNTR_EL0);
    __info_printf("(enc_startCount, enc_endCount) = (%x, %x)\n", enc_startCount, enc_endCount);
    enc_totalCount = enc_endCount - enc_startCount;
    __info_printf("enc_totalCount = %d\n", enc_totalCount);

    // decryption
    // reset counter
    cnt_i = read_sysreg(PMCR_EL0) | PMCR_EL0_P | PMCR_EL0_C;
    write_sysreg(cnt_i, PMCR_EL0);
    // enable
    cnt_i = PMCR_EL0_E | read_sysreg(PMCR_EL0);
    write_sysreg(cnt_i, PMCR_EL0);
    dec_startCount = read_sysreg(PMCCNTR_EL0);
    // counter enabble
    write_sysreg(PMCNTENSET_EL0_C, PMCNTENSET_EL0);

    switch (thr_algo) {
        case TEST_CIPHER_DES_ECB:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_des_ecb_init(key, keylen); CHK(ret);
                ret = hal_crypto_des_ecb_decrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_DES_CBC:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_des_cbc_init(key, keylen); CHK(ret);
                ret = hal_crypto_des_cbc_decrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_DES_CFB:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_des_cfb_init(key, keylen); CHK(ret);
                ret = hal_crypto_des_cfb_decrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_DES_OFB:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_des_ofb_init(key, keylen); CHK(ret);
                ret = hal_crypto_des_ofb_decrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_DES_CTR:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_des_ofb_init(key, keylen); CHK(ret);
                ret = hal_crypto_des_ofb_decrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        default:
            __err_printf("thr_algo(%08x): no this thr_algo\n", thr_algo);
            break;
    }

    dec_endCount = read_sysreg(PMCCNTR_EL0);
    __info_printf("(dec_startCount, dec_endCount) = (%x, %x)\n", dec_startCount, dec_endCount);
    dec_totalCount = dec_endCount - dec_startCount;
    __info_printf("dec_totalCount = %d\n", dec_totalCount);

    return (enc_totalCount + dec_totalCount);
}

u32 throughput_cipher_3des_test(
    IN u8 thr_algo,
    IN u32 thr_loop
)
{
    s32 ret = FAIL;
    u32 i, cnt_i;
    u32 enc_startCount = 0, enc_endCount = 0, enc_totalCount = 0;
    u32 dec_startCount = 0, dec_endCount = 0, dec_totalCount = 0;

    u8 *key = NULL;
    u32 keylen = 0;
    u8 *iv = NULL;
    u32 ivlen = 0;
    u8 *message = NULL;
    u32 msglen = 0;
    u8 *hw_result = NULL;

    switch (thr_algo) {
        case TEST_CIPHER_3DES_ECB:
            key = cipher_key_in;
            keylen = thr_cipher_3des_keylen;
            //rtlc_memset(key, 0x5a, keylen);
            iv = NULL;
            ivlen = 0;
            message = cipher_msg_in;
            msglen = thr_cipher_des_msglen;
            //rtlc_memset(message, 0x5a, msglen);
            break;
        case TEST_CIPHER_3DES_CBC:
        case TEST_CIPHER_3DES_CFB:
        case TEST_CIPHER_3DES_OFB:
        case TEST_CIPHER_3DES_CTR:
            key = cipher_key_in;
            keylen = thr_cipher_3des_keylen;
            //rtlc_memset(key, 0x5a, keylen);
            iv = cipher_iv_in;
            ivlen = thr_cipher_des_ivlen;
            //rtlc_memset(iv, 0x5a, ivlen);
            message = cipher_msg_in;
            msglen = thr_cipher_des_msglen;
            //rtlc_memset(message, 0x5a, msglen);
            break;
        default:
            __err_printf("thr_algo(%08x): no this thr_algo\n", thr_algo);
            break;
    }

    hw_result = hw_cipher_msg_out;

    __info_printf("keylen = %d\n", keylen);
    __info_printf("ivlen = %d\n", ivlen);
    __info_printf("msglen = %d\n", msglen);

    // test start
    // encryption
    // reset counter
    cnt_i = read_sysreg(PMCR_EL0) | PMCR_EL0_P | PMCR_EL0_C;
    write_sysreg(cnt_i, PMCR_EL0);
    // enable
    cnt_i = PMCR_EL0_E | read_sysreg(PMCR_EL0);
    write_sysreg(cnt_i, PMCR_EL0);
    enc_startCount = read_sysreg(PMCCNTR_EL0);
    // counter enabble
    write_sysreg(PMCNTENSET_EL0_C, PMCNTENSET_EL0);

    switch (thr_algo) {
        case TEST_CIPHER_3DES_ECB:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_3des_ecb_init(key, keylen); CHK(ret);
                ret = hal_crypto_3des_ecb_encrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_3DES_CBC:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_3des_cbc_init(key, keylen); CHK(ret);
                ret = hal_crypto_3des_cbc_encrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_3DES_CFB:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_3des_cfb_init(key, keylen); CHK(ret);
                ret = hal_crypto_3des_cfb_encrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_3DES_OFB:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_3des_ofb_init(key, keylen); CHK(ret);
                ret = hal_crypto_3des_ofb_encrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_3DES_CTR:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_3des_ofb_init(key, keylen); CHK(ret);
                ret = hal_crypto_3des_ofb_encrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        default:
            __err_printf("thr_algo(%08x): no this thr_algo\n", thr_algo);
            break;
    }

    enc_endCount = read_sysreg(PMCCNTR_EL0);
    __info_printf("(enc_startCount, enc_endCount) = (%x, %x)\n", enc_startCount, enc_endCount);
    enc_totalCount = enc_endCount - enc_startCount;
    __info_printf("enc_totalCount = %d\n", enc_totalCount);

    // decryption
    // reset counter
    cnt_i = read_sysreg(PMCR_EL0) | PMCR_EL0_P | PMCR_EL0_C;
    write_sysreg(cnt_i, PMCR_EL0);
    // enable
    cnt_i = PMCR_EL0_E | read_sysreg(PMCR_EL0);
    write_sysreg(cnt_i, PMCR_EL0);
    dec_startCount = read_sysreg(PMCCNTR_EL0);
    // counter enabble
    write_sysreg(PMCNTENSET_EL0_C, PMCNTENSET_EL0);

    switch (thr_algo) {
        case TEST_CIPHER_3DES_ECB:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_3des_ecb_init(key, keylen); CHK(ret);
                ret = hal_crypto_3des_ecb_decrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_3DES_CBC:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_3des_cbc_init(key, keylen); CHK(ret);
                ret = hal_crypto_3des_cbc_decrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_3DES_CFB:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_3des_cfb_init(key, keylen); CHK(ret);
                ret = hal_crypto_3des_cfb_decrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_3DES_OFB:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_3des_ofb_init(key, keylen); CHK(ret);
                ret = hal_crypto_3des_ofb_decrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_3DES_CTR:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_3des_ofb_init(key, keylen); CHK(ret);
                ret = hal_crypto_3des_ofb_decrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        default:
            __err_printf("thr_algo(%08x): no this thr_algo\n", thr_algo);
            break;
    }

    dec_endCount = read_sysreg(PMCCNTR_EL0);
    __info_printf("(dec_startCount, dec_endCount) = (%x, %x)\n", dec_startCount, dec_endCount);
    dec_totalCount = dec_endCount - dec_startCount;
    __info_printf("dec_totalCount = %d\n", dec_totalCount);

    return (enc_totalCount + dec_totalCount);
}

u32 throughput_cipher_aes_test(
    IN u8 thr_algo,
    IN u32 thr_loop,
    IN u8 keylen_in
)
{
    s32 ret = FAIL;
    u32 i, cnt_i;
    u32 enc_startCount = 0, enc_endCount = 0, enc_totalCount = 0;
    u32 dec_startCount = 0, dec_endCount = 0, dec_totalCount = 0;

    u8 *key = NULL;
    u32 keylen = 0;
    u8 *iv = NULL;
    u32 ivlen = 0;
    u8 *aad = NULL;
    u32 aadlen = 0;
    u8 *message = NULL;
    u32 msglen = 0;
    u8 *hw_result = NULL;
    u8 hw_tag[max_cipher_taglen];

    // set input
    switch (thr_algo) {
        case TEST_CIPHER_AES_ECB:
            key = cipher_key_in;
            keylen = keylen_in;
            //rtlc_memset(key, 0x5a, keylen);
            iv = NULL;
            ivlen = 0;
            message = cipher_msg_in;
            msglen = thr_cipher_aes_msglen;
            //rtlc_memset(message, 0x5a, msglen);
            break;
        case TEST_CIPHER_AES_CBC:
        case TEST_CIPHER_AES_CFB:
        case TEST_CIPHER_AES_OFB:
        case TEST_CIPHER_AES_CTR:
            key = cipher_key_in;
            keylen = keylen_in;
            //rtlc_memset(key, 0x5a, keylen);
            iv = cipher_iv_in;
            ivlen = thr_cipher_aes_ivlen;
            //rtlc_memset(iv, 0x5a, ivlen);
            message = cipher_msg_in;
            msglen = thr_cipher_aes_msglen;
            //rtlc_memset(message, 0x5a, msglen);
        case TEST_CIPHER_AES_GCTR:
        case TEST_CIPHER_AES_GMAC:
        case TEST_CIPHER_AES_GHASH:
        case TEST_CIPHER_AES_GCM:
            key = cipher_key_in;
            keylen = keylen_in;
            //rtlc_memset(key, 0x5a, keylen);
            iv = cipher_iv_in;
            ivlen = thr_cipher_aes_ivlen;
            //rtlc_memset(iv, 0x5a, ivlen);
            aad = cipher_aad_in;
            aadlen = thr_cipher_aes_aadlen;
            //rtlc_memset(aad, 0x5a, aadlen);
            message = cipher_msg_in;
            msglen = thr_cipher_aes_msglen;
            //rtlc_memset(message, 0x5a, msglen);
            break;
        default:
            __err_printf("thr_algo(%08x): no this thr_algo\n", thr_algo);
            break;
    }

    hw_result = hw_cipher_msg_out;

    __info_printf("keylen = %d\n", keylen);
    __info_printf("ivlen = %d\n", ivlen);
    __info_printf("msglen = %d\n", msglen);
    __info_printf("aadlen = %d\n", aadlen);

    // test start
    // encryption
    // reset counter
    cnt_i = read_sysreg(PMCR_EL0) | PMCR_EL0_P | PMCR_EL0_C;
    write_sysreg(cnt_i, PMCR_EL0);
    // enable
    cnt_i = PMCR_EL0_E | read_sysreg(PMCR_EL0);
    write_sysreg(cnt_i, PMCR_EL0);
    enc_startCount = read_sysreg(PMCCNTR_EL0);
    // counter enabble
    write_sysreg(PMCNTENSET_EL0_C, PMCNTENSET_EL0);

    switch (thr_algo) {
        case TEST_CIPHER_AES_ECB:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_aes_ecb_init(key, keylen); CHK(ret);
                ret = hal_crypto_aes_ecb_encrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_AES_CBC:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_aes_cbc_init(key, keylen); CHK(ret);
                ret = hal_crypto_aes_cbc_encrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_AES_CFB:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_aes_cfb_init(key, keylen); CHK(ret);
                ret = hal_crypto_aes_cfb_encrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_AES_OFB:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_aes_ofb_init(key, keylen); CHK(ret);
                ret = hal_crypto_aes_ofb_encrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_AES_CTR:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_aes_ofb_init(key, keylen); CHK(ret);
                ret = hal_crypto_aes_ofb_encrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_AES_GCTR:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_aes_gctr_init(key, keylen); CHK(ret);
                ret = hal_crypto_aes_gctr_encrypt(message, msglen, iv, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_AES_GMAC:
            // no encryption
            __info_printf("no encryption in GMAC mode\n");
            break;
        case TEST_CIPHER_AES_GHASH:
            // no encryption
            __info_printf("no encryption in GHASH mode\n");
            break;
        case TEST_CIPHER_AES_GCM:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_aes_gcm_init(key, keylen); CHK(ret);
                ret = hal_crypto_aes_gcm_encrypt(message, msglen, iv, aad, aadlen, hw_result, hw_tag); CHK(ret);
            }
            break;
        default:
            __err_printf("thr_algo(%08x): no this thr_algo\n", thr_algo);
            break;
    }

    enc_endCount = read_sysreg(PMCCNTR_EL0);
    __info_printf("(enc_startCount, enc_endCount) = (%x, %x)\n", enc_startCount, enc_endCount);
    enc_totalCount = enc_endCount - enc_startCount;
    __info_printf("enc_totalCount = %d\n", enc_totalCount);

    // decryption
    // reset counter
    cnt_i = read_sysreg(PMCR_EL0) | PMCR_EL0_P | PMCR_EL0_C;
    write_sysreg(cnt_i, PMCR_EL0);
    // enable
    cnt_i = PMCR_EL0_E | read_sysreg(PMCR_EL0);
    write_sysreg(cnt_i, PMCR_EL0);
    dec_startCount = read_sysreg(PMCCNTR_EL0);
    // counter enabble
    write_sysreg(PMCNTENSET_EL0_C, PMCNTENSET_EL0);

    switch (thr_algo) {
        case TEST_CIPHER_AES_ECB:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_aes_ecb_init(key, keylen); CHK(ret);
                ret = hal_crypto_aes_ecb_decrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_AES_CBC:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_aes_cbc_init(key, keylen); CHK(ret);
                ret = hal_crypto_aes_cbc_decrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_AES_CFB:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_aes_cfb_init(key, keylen); CHK(ret);
                ret = hal_crypto_aes_cfb_decrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_AES_OFB:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_aes_ofb_init(key, keylen); CHK(ret);
                ret = hal_crypto_aes_ofb_decrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_AES_CTR:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_aes_ofb_init(key, keylen); CHK(ret);
                ret = hal_crypto_aes_ofb_decrypt(message, msglen, iv, ivlen, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_AES_GCTR:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_aes_gctr_init(key, keylen); CHK(ret);
                ret = hal_crypto_aes_gctr_decrypt(message, msglen, iv, hw_result); CHK(ret);
            }
            break;
        case TEST_CIPHER_AES_GMAC:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_aes_gmac(message, msglen, key, keylen, iv, aad, aadlen, hw_tag); CHK(ret);
            }
            break;
        case TEST_CIPHER_AES_GHASH:
            if (keylen == 16) {
                for (i = 0; i < thr_loop; i++) {
                    ret = hal_crypto_aes_ghash(message, msglen, key, keylen, hw_tag); CHK(ret);
                }
            } else {
                __info_printf("not support for keylen != 16\n");
            }
            break;
        case TEST_CIPHER_AES_GCM:
            for (i = 0; i < thr_loop; i++) {
                ret = hal_crypto_aes_gcm_init(key, keylen); CHK(ret);
                ret = hal_crypto_aes_gcm_decrypt(message, msglen, iv, aad, aadlen, hw_result, hw_tag); CHK(ret);
            }
            break;
        default:
            __err_printf("thr_algo(%08x): no this thr_algo\n", thr_algo);
            break;
    }

    dec_endCount = read_sysreg(PMCCNTR_EL0);
    __info_printf("(dec_startCount, dec_endCount) = (%x, %x)\n", dec_startCount, dec_endCount);
    dec_totalCount = dec_endCount - dec_startCount;
    __info_printf("dec_totalCount = %d\n", dec_totalCount);

    return (enc_totalCount + dec_totalCount);
}

u32 throughput_cipher_test(
    IN u8 thr_mode,
    IN u8 thr_algo,
    IN u32 thr_loop
)
{
    u32 totalTime = 0;
    u32 totalTime128 = 0, totalTime192 = 0, totalTime256 = 0;

    switch (thr_mode) {
        case TEST_CIPHER_DES:
            totalTime = throughput_cipher_des_test(thr_algo, thr_loop);
            break;
        case TEST_CIPHER_3DES:
            totalTime = throughput_cipher_3des_test(thr_algo, thr_loop);
            break;
        case TEST_CIPHER_AES:
            totalTime128 = throughput_cipher_aes_test(thr_algo, thr_loop, thr_cipher_aes128_keylen);
            totalTime192 = throughput_cipher_aes_test(thr_algo, thr_loop, thr_cipher_aes192_keylen);
            totalTime256 = throughput_cipher_aes_test(thr_algo, thr_loop, thr_cipher_aes256_keylen);
            totalTime = totalTime128 + totalTime192 + totalTime256;
            break;
        default:
            __err_printf("thr_mode(%08x): no this thr_mode\n", thr_mode);
    }

    return totalTime;
}

s32 random_cipher_des_test(
    IN u8 ran_algo
)
{
    s32 ret = FAIL;
    u32 i;
    u8 *key = NULL;
    u32 keylen = 0;
    u8 *iv = NULL;
    u32 ivlen = 0;
    u8 *hw_message_dec = NULL, *sw_message_dec = NULL;
    u8 *message_ori = NULL;
    u32 src_pos = 0, dst_pos = 0;
    u32 msglen = 0;
    u8 *hw_result;
    u8 sw_result[max_cipher_msglen];
    u8 *message_src = NULL;
    // generate random key
    keylen = 8;
    for (i = 0; i < keylen; i++) {
        cipher_key_in[i] = rtlc_random() & 0xFF;
    }

    // generate random iv
    if (ran_algo != TEST_CIPHER_DES_ECB) {
        ivlen = 8;
    }
    for (i = 0; i < ivlen; i++) {
        cipher_iv_in[i] = rtlc_random() & 0xFF;
    }

    // generate random message for non-alignment case
    src_pos = rtlc_random() % byte_align;
    msglen = (((rtlc_random() % (max_cipher_msglen)) / 16) + 1) * 16;

    for (i = 0; i < msglen; i++) {
        cipher_msg_in[src_pos + i] = rtlc_random() & 0xFF;
    }

    if(addr_test_flag && src_addr){
        memset( (void *)src_addr, 0x0, byte_align + max_cipher_msglen + byte_align);
        message_src =  (u8 *)(src_addr+ src_pos);
        memcpy(message_src, &cipher_msg_in[src_pos], msglen );
    }

    // prepare hw result for non-alignment case
    dst_pos = rtlc_random() % byte_align;

	if(addr_test_flag && dst_addr){
		memset( (void *)dst_addr, 0x0, byte_align + max_cipher_msglen + byte_align);
		hw_result = (u8 *)(dst_addr+ dst_pos);
	}else{
	    hw_result = &hw_cipher_msg_out[dst_pos];
	}
    __dbg_printf("msglen = %d, src_pos = %d, dst_pos = %d\r\n", msglen, src_pos, dst_pos);
	__dbg_printf("User assigned: message_src = %p, hw_result=%p\n", message_src, hw_result);
    // encryption
    switch (ran_algo) {
        case TEST_CIPHER_DES_ECB:
            key = cipher_key_in;
            //keylen = ;
            iv = NULL;
            //ivlen = ;
            if(message_src == NULL){
                message_src = &cipher_msg_in[src_pos];
            }
            message_ori = &cipher_msg_in[src_pos];

            //msglen = ;
            // hw computation
            ret = hal_crypto_des_ecb_init(key, keylen); CHK(ret);
            ret = hal_crypto_des_ecb_encrypt(message_src, msglen, iv, ivlen, hw_result);
            // sw computation
            desSim_ecb_encrypt(message_ori, sw_result, msglen, key, TRUE);
            break;
        case TEST_CIPHER_DES_CBC:
            key = cipher_key_in;
            //keylen = ;
            iv = cipher_iv_in;
            //ivlen = ;
			if(message_src == NULL){
				message_src = &cipher_msg_in[src_pos];
			}
            message_ori = &cipher_msg_in[src_pos];
            //msglen = ;
            // hw computation
            ret = hal_crypto_des_cbc_init(key, keylen); CHK(ret);
            ret = hal_crypto_des_cbc_encrypt(message_src, msglen, iv, ivlen, hw_result);
            // sw computation
            desSim_cbc_encrypt(message_ori, sw_result, msglen, key, iv, TRUE);
            break;
        case TEST_CIPHER_DES_CFB:
            // break;
        case TEST_CIPHER_DES_OFB:
            // break;
        case TEST_CIPHER_DES_CTR:
            // break;
        default:
            __err_printf("ran_algo(%08x): no this ran_algo\n", ran_algo);
            break;
    }

    if (ret == SUCCESS) {
        if (rtlc_memcmp(hw_result, sw_result, msglen) == 0) {
            __dbg_printf("random DES enc test success, ran_algo = %08x\n", ran_algo);
            // return SUCCESS;
        } else {
            __err_printf("random DES enc test fail, ran_algo = %08x\n", ran_algo);
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message_ori, msglen, "message_ori: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
        }
    } else {
        __err_printf("Crypto Engine random DES enc compute fail, ret = %d\n", ret);
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)message_ori, msglen, "message_ori: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
        return FAIL;
    }

    // decryption: message -> hw_reault; sw_result -> message
    switch (ran_algo) {
        case TEST_CIPHER_DES_ECB:
            key = cipher_key_in;
            //keylen = ;
            iv = NULL;
            //ivlen = ;
            hw_message_dec = hw_result;
            //msglen = ;
            // hw computation
            ret = hal_crypto_des_ecb_init(key, keylen); CHK(ret);
            ret = hal_crypto_des_ecb_decrypt(hw_message_dec, msglen, iv, ivlen, hw_result);
            // sw computation
            sw_message_dec = sw_result;
            desSim_ecb_encrypt(sw_message_dec, sw_result, msglen, key, FALSE);
            break;
        case TEST_CIPHER_DES_CBC:
            key = cipher_key_in;
            //keylen = ;
            iv = cipher_iv_in;
            //ivlen = ;
            hw_message_dec = hw_result;
            //msglen = ;
            // hw computation
            ret = hal_crypto_des_cbc_init(key, keylen); CHK(ret);
            ret = hal_crypto_des_cbc_decrypt(hw_message_dec, msglen, iv, ivlen, hw_result);
            // sw computation
            sw_message_dec = sw_result;
            desSim_cbc_encrypt(sw_message_dec, sw_result, msglen, key, iv, FALSE);
            break;
        case TEST_CIPHER_DES_CFB:
            // break;
        case TEST_CIPHER_DES_OFB:
            // break;
        case TEST_CIPHER_DES_CTR:
            // break;
        default:
            __err_printf("ran_algo(%08x): no this ran_algo\n", ran_algo);
            break;
    }

    if (ret == SUCCESS) {
        if (rtlc_memcmp(hw_result, sw_result, msglen) == 0) {
            __dbg_printf("random DES dec test success, ran_algo = %08x\n", ran_algo);

            return SUCCESS;
        } else {
            __err_printf("random DES dec test fail, ran_algo = %08x\n", ran_algo);
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message_ori, msglen, "message_ori: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
        }
    } else {
        __err_printf("Crypto Engine random DES dec compute fail, ret = %d\n", ret);
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)message_ori, msglen, "message_ori: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
        return FAIL;
    }

    //return FAIL;
}

s32 random_cipher_3des_test(
    IN u8 ran_algo
)
{
    s32 ret = FAIL;
    u32 i;
    u8 *key = NULL;
    u32 keylen = 0;
    u8 *iv = NULL;
    u8 cipher_iv_in[16];
    u32 ivlen = 0;
    u8 *message_ori = NULL;
    u8 *hw_message_dec = NULL, *sw_message_dec = NULL;
    u32 src_pos = 0, dst_pos = 0;
    //u8 cipher_msg_in[byte_align + max_cipher_msglen + byte_align];
    u32 msglen = 0;
    //u8 hw_cipher_msg_out[byte_align + max_cipher_msglen + byte_align];
    u8 *hw_result;
    u8 sw_result[max_cipher_msglen];
	u8 *message_src = NULL;

    // generate random key
    keylen = 24;
    for (i = 0; i < keylen; i++) {
        cipher_key_in[i] = rtlc_random() & 0xFF;
    }

    // generate random iv
    if (ran_algo != TEST_CIPHER_3DES_ECB) {
        ivlen = 8;
    }
    for (i = 0; i < ivlen; i++) {
        cipher_iv_in[i] = rtlc_random() & 0xFF;
    }

    // generate random message for non-alignment case
    src_pos = rtlc_random() % byte_align;
    msglen = (((rtlc_random() % (max_cipher_msglen)) / 16) + 1) * 16;
    for (i = 0; i < msglen; i++) {
        cipher_msg_in[src_pos + i] = rtlc_random() & 0xFF;
    }

    if(addr_test_flag && src_addr){
            memset( (void *)src_addr, 0x0, byte_align + max_cipher_msglen + byte_align);
            message_src =  (u8 *)(src_addr+ src_pos);
            memcpy(message_src, &cipher_msg_in[src_pos], msglen );
    }

    // prepare hw result for non-alignment case
    dst_pos = rtlc_random() % byte_align;
//    hw_result = &hw_cipher_msg_out[dst_pos];
    if(addr_test_flag && dst_addr){
         memset( (void *)dst_addr, 0x0, byte_align + max_cipher_msglen + byte_align);
         hw_result = (u8 *)(dst_addr+ dst_pos);
    }else{
         hw_result = &hw_cipher_msg_out[dst_pos];
    }

    __dbg_printf("msglen = %d, src_pos = %d, dst_pos = %d\r\n", msglen, src_pos, dst_pos);
	__dbg_printf("User assigned: message_src = %p, hw_result=%p\n", message_src, hw_result);
    // encryption
    switch (ran_algo) {
        case TEST_CIPHER_3DES_ECB:
            key = cipher_key_in;
            //keylen = ;
            iv = NULL;
            //ivlen = ;
            if(message_src == NULL){
                 message_src = &cipher_msg_in[src_pos];
            }
            message_ori = &cipher_msg_in[src_pos];
            //msglen = ;
            // hw computation
            ret = hal_crypto_3des_ecb_init(key, keylen); CHK(ret);
            ret = hal_crypto_3des_ecb_encrypt(message_src, msglen, iv, ivlen, hw_result);
            // sw computation
            desSim_ede_ecb_encrypt(message_ori, sw_result, msglen, key, TRUE);
            break;
        case TEST_CIPHER_3DES_CBC:
            key = cipher_key_in;
            //keylen = ;
            iv = cipher_iv_in;
            //ivlen = ;
            if(message_src == NULL){
                 message_src = &cipher_msg_in[src_pos];
            }
            message_ori = &cipher_msg_in[src_pos];
            //msglen = ;
            // hw computation
            ret = hal_crypto_3des_cbc_init(key, keylen); CHK(ret);
            ret = hal_crypto_3des_cbc_encrypt(message_src, msglen, iv, ivlen, hw_result);
            // sw computation
            desSim_ede_cbc_encrypt(message_ori, sw_result, msglen, key, iv, TRUE);
            break;
        case TEST_CIPHER_3DES_CFB:
            // break;
        case TEST_CIPHER_3DES_OFB:
            // break;
        case TEST_CIPHER_3DES_CTR:
            // break;
        default:
            __err_printf("ran_algo(%08x): no this ran_algo\n", ran_algo);
            break;
    }

    if (ret == SUCCESS) {
        if (rtlc_memcmp(hw_result, sw_result, msglen) == 0) {
            __dbg_printf("random 3DES enc test success, ran_algo = %08x\n", ran_algo);
            // return SUCCESS;
        } else {
            __err_printf("random 3DES enc test fail, ran_algo = %08x\n", ran_algo);
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message_ori, msglen, "message_ori: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
        }
    } else {
        __err_printf("Crypto Engine random 3DES enc compute fail, ret = %d\n", ret);
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)message_ori, msglen, "message_ori: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
        return FAIL;
    }

    // decryption: message -> hw_reault; sw_result -> message
    switch (ran_algo) {
        case TEST_CIPHER_3DES_ECB:
            key = cipher_key_in;
            //keylen = ;
            iv = NULL;
            //ivlen = ;
            hw_message_dec = hw_result;
            //msglen = ;
            // hw computation
            ret = hal_crypto_3des_ecb_init(key, keylen); CHK(ret);
            ret = hal_crypto_3des_ecb_decrypt(hw_message_dec, msglen, iv, ivlen, hw_result);
            // sw computation
            sw_message_dec = sw_result;
            desSim_ede_ecb_encrypt(sw_message_dec, sw_result, msglen, key, FALSE);
            break;
        case TEST_CIPHER_3DES_CBC:
            key = cipher_key_in;
            //keylen = ;
            iv = cipher_iv_in;
            //ivlen = ;
            hw_message_dec = hw_result;
            //msglen = ;
            // hw computation
            ret = hal_crypto_3des_cbc_init(key, keylen); CHK(ret);
            ret = hal_crypto_3des_cbc_decrypt(hw_message_dec, msglen, iv, ivlen, hw_result);
            // sw computation
            sw_message_dec = sw_result;
            desSim_ede_cbc_encrypt(sw_message_dec, sw_result, msglen, key, iv, FALSE);
            break;
        case TEST_CIPHER_3DES_CFB:
            // break;
        case TEST_CIPHER_3DES_OFB:
            // break;
        case TEST_CIPHER_3DES_CTR:
            // break;
        default:
            __err_printf("ran_algo(%08x): no this ran_algo\n", ran_algo);
            break;
    }

    if (ret == SUCCESS) {
        if (rtlc_memcmp(hw_result, sw_result, msglen) == 0) {
            __dbg_printf("random 3DES dec test success, ran_algo = %08x\n", ran_algo);
            return SUCCESS;
        } else {
            __err_printf("random 3DES dec test fail, ran_algo = %08x\n", ran_algo);
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message_ori, msglen, "message_ori: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
        }
    } else {
        __err_printf("Crypto Engine random 3DES dec compute fail, ret = %d\n", ret);
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)message_ori, msglen, "message_ori: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
        return FAIL;
    }

    //return FAIL;
}

s32 random_cipher_aes_test(
    IN u8 ran_algo
)
{
    s32 ret = FAIL;
    u32 i;
    u8 *key = NULL;
    u32 keylen = 0;
    u8 *iv = NULL;
    u32 ivlen = 0;
    u8 *message_ori = NULL;
    u8 *hw_message_dec = NULL, *sw_message_dec = NULL;
    u32 src_pos = 0, dst_pos = 0;
    u32 msglen = 0;
    u8 *aad = NULL;
    u32 aadlen = 0;
    u32 taglen = 0;
    u8 *hw_result;
    u8 sw_result[max_cipher_msglen];
    u8 hw_tag[max_cipher_taglen], *sw_tag = NULL;
	u8 process_iv[max_cipher_ivlen] = { 0 };
	u8 *message_src = NULL;

    size_t offset; // for sw AES-CTR
    s8 nonce_ctr[16], stream_blk[16]; // for sw AES-CTR
    mbedtls_aes_context ctx;
    mbedtls_gcm_context ctx_gcm; // for sw AES-GCM
    mbedtls_cipher_id_t cipher; // for sw AES-GCM

    // generate random key
    if (ran_algo == TEST_CIPHER_AES_GHASH) {
        keylen = 16;
    } else {
        keylen = ((rtlc_random() % 3) + 2) * 8; // 16, 24, 32
    }
    for (i = 0; i < keylen; i++) {
        cipher_key_in[i] = rtlc_random() & 0xFF;
    }

    // generate random iv
    if (ran_algo == TEST_CIPHER_AES_ECB) {
        ivlen = 0;
    } else if (ran_algo == TEST_CIPHER_AES_GCM) {
        ivlen = 12;
    } else {
        ivlen = 16;
    }
    for (i = 0; i < ivlen; i++) {
        cipher_iv_in[i] = rtlc_random() & 0xFF;
    }

    // generate random aad
    if (ran_algo == TEST_CIPHER_AES_GCM || ran_algo == TEST_CIPHER_AES_GMAC) {
        aadlen = rtlc_random() % 112 + 1;
    }
    for (i = 0; i < aadlen; i++) {
        cipher_aad_in[i] = rtlc_random() & 0xFF;
    }

    // generate random message for non-alignment case
    src_pos = rtlc_random() % byte_align;
    msglen = (((rtlc_random() % (max_cipher_msglen)) / 16) + 1) * 16;
    for (i = 0; i < msglen; i++) {
        cipher_msg_in[src_pos + i] = rtlc_random() & 0xFF;
    }

    if(addr_test_flag && src_addr){
         memset( (void *)src_addr, 0x0, byte_align + max_cipher_msglen + byte_align);
         message_src =  (u8 *)(src_addr+ src_pos);
         memcpy(message_src, &cipher_msg_in[src_pos], msglen );
    }


    // prepare hw result for non-alignment case
    dst_pos = rtlc_random() % byte_align;
//    hw_result = &hw_cipher_msg_out[dst_pos];
    if(addr_test_flag && dst_addr){
        memset( (void *)dst_addr, 0x0, byte_align + max_cipher_msglen + byte_align);
        hw_result = (u8 *)(dst_addr+ dst_pos);
    }else{
        hw_result = &hw_cipher_msg_out[dst_pos];
    }

    __dbg_printf("msglen = %d, src_pos = %d, dst_pos = %d\r\n", msglen, src_pos, dst_pos);
    __dbg_printf("User assigned: message_src = %p, hw_result=%p\n", message_src, hw_result);
    // encryption
    switch (ran_algo) {
        case TEST_CIPHER_AES_ECB:
            key = cipher_key_in;
            //keylen = ;
            iv = NULL;
            //ivlen = ;
            if(message_src == NULL){
                  message_src = &cipher_msg_in[src_pos];
            }

            message_ori = &cipher_msg_in[src_pos];
            //msglen = ;
            // hw computation
            ret = hal_crypto_aes_ecb_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ecb_encrypt(message_src, msglen, iv, ivlen, hw_result);
            // sw computation
            mbedtls_aes_init(&ctx);
            mbedtls_aes_setkey_enc(&ctx, key, keylen * 8);
            for (i = 0; i < msglen; i += 16) {
                mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, &message_ori[i], &sw_result[i]);
            }
            break;
        case TEST_CIPHER_AES_CBC:
            key = cipher_key_in;
            //keylen = ;
            memcpy(process_iv, cipher_iv_in, ivlen);
            iv = process_iv;
            //ivlen = ;
            if(message_src == NULL){
                  message_src = &cipher_msg_in[src_pos];
            }
            message_ori = &cipher_msg_in[src_pos];
            //msglen = ;
            // hw computation
            ret = hal_crypto_aes_cbc_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_cbc_encrypt(message_src, msglen, iv, ivlen, hw_result);
            // sw computation
            mbedtls_aes_init(&ctx);
            mbedtls_aes_setkey_enc(&ctx, key, keylen * 8);
            mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, msglen, iv, message_ori, sw_result);
            break;
        case TEST_CIPHER_AES_CFB:
            key = cipher_key_in;
            //keylen = ;
            memcpy(process_iv, cipher_iv_in, ivlen);
            iv = process_iv;
            //ivlen = ;
            if(message_src == NULL){
                  message_src = &cipher_msg_in[src_pos];
            }
            message_ori = &cipher_msg_in[src_pos];
            //msglen = ;
            // hw computation
            ret = hal_crypto_aes_cfb_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_cfb_encrypt(message_src, msglen, iv, ivlen, hw_result);
            // sw computation
            offset = 0;
            mbedtls_aes_init(&ctx);
            mbedtls_aes_setkey_enc(&ctx, key, keylen * 8);
            mbedtls_aes_crypt_cfb128(&ctx, MBEDTLS_AES_ENCRYPT, msglen, &offset, iv, message_ori, sw_result);
            break;
        case TEST_CIPHER_AES_OFB:
            key = cipher_key_in;
            //keylen = ;
            iv = cipher_iv_in;
            //ivlen = ;
           if(message_src == NULL){
                  message_src = &cipher_msg_in[src_pos];
            }
            message_ori = &cipher_msg_in[src_pos];
            //msglen = ;
            // hw computation
            ret = hal_crypto_aes_ofb_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ofb_encrypt(message_src, msglen, iv, ivlen, hw_result);
            // sw computation
            offset = 0;
            mbedtls_aes_init(&ctx);
            mbedtls_aes_setkey_enc(&ctx, key, keylen * 8);
            mbedtls_aes_crypt_ofb128(&ctx, msglen, &offset, iv, message_ori, sw_result);
            break;
        case TEST_CIPHER_AES_CTR:
            key = cipher_key_in;
            //keylen = ;
            iv = cipher_iv_in;
            //ivlen = ;
           if(message_src == NULL){
                  message_src = &cipher_msg_in[src_pos];
            }
            message_ori = &cipher_msg_in[src_pos];
            //msglen = ;
            // hw computation
            ret = hal_crypto_aes_ctr_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ctr_encrypt(message_src, msglen, iv, ivlen, hw_result);
            // sw computation
            offset = 0;
            rtlc_memcpy(nonce_ctr, iv, sizeof(nonce_ctr));
            rtlc_memset(stream_blk, 0x0, sizeof(stream_blk));
            mbedtls_aes_init(&ctx);
            mbedtls_aes_setkey_enc(&ctx, key, keylen * 8);
            mbedtls_aes_crypt_ctr(&ctx, msglen, &offset, (u8 *)nonce_ctr, (u8 *)stream_blk, message_ori, sw_result);
            break;
        case TEST_CIPHER_AES_GCTR:
            key = cipher_key_in;
            //keylen = ;
            iv = cipher_iv_in;
            //ivlen = ;
           if(message_src == NULL){
                  message_src = &cipher_msg_in[src_pos];
            }
            message_ori = &cipher_msg_in[src_pos];
            //msglen = ;
            // hw computation
            ret = hal_crypto_aes_gctr_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_gctr_encrypt(message_src, msglen, iv, hw_result);
            // sw computation
            mbedtls_aes_init(&ctx);
            rtlc_memcpy(nonce_ctr, iv, sizeof(nonce_ctr));
            aes_gctr_swvrf(key, keylen, (u8 *)nonce_ctr, message_ori, msglen, sw_result);
            break;
        case TEST_CIPHER_AES_GMAC:
            __dbg_printf("No enc for AES_GMAC\n");
            break;
        case TEST_CIPHER_AES_GHASH:
            __dbg_printf("No enc for AES_GHASH\n");
            break;
        case TEST_CIPHER_AES_GCM:
            key = cipher_key_in;
            //keylen = ;
            iv = cipher_iv_in;
            //ivlen = ;
            aad = cipher_aad_in;
            //aadlen = ;
           if(message_src == NULL){
                  message_src = &cipher_msg_in[src_pos];
            }
            message_ori = &cipher_msg_in[src_pos];
            //msglen = ;
            // hw computation
            ret = hal_crypto_aes_gcm_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_gcm_encrypt(message_src, msglen, iv, aad, aadlen, hw_result, hw_tag);
            // sw computation
            taglen = 16;
            mbedtls_aes_init(&ctx);
            mbedtls_gcm_init(&ctx_gcm);
            cipher = MBEDTLS_CIPHER_ID_AES;
            mbedtls_gcm_setkey(&ctx_gcm, cipher, key, keylen * 8);
            mbedtls_gcm_crypt_and_tag(&ctx_gcm, MBEDTLS_GCM_ENCRYPT, msglen, iv, 12,
                                        aad, aadlen, message_ori, sw_result, taglen, sw_tag);
            mbedtls_gcm_free( &ctx_gcm);
            break;
        default:
            __err_printf("ran_algo(%08x): no this ran_algo\n", ran_algo);
            break;
    }

    if (ret == SUCCESS) {
        if (rtlc_memcmp(hw_result, sw_result, msglen) == 0) {
            if (ran_algo == TEST_CIPHER_AES_GCM) {
                if (rtlc_memcmp(hw_tag, sw_tag, taglen) == 0) {
                    __dbg_printf("random AES enc test success, ran_algo = %08x\n", ran_algo);
                    // return SUCCESS;
                } else {
                    __info_printf("random AES enc tag test success, ran_algo = %08x\n", ran_algo);
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)aad, aadlen, "aad: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_tag, taglen, "hw_tag: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_tag, taglen, "sw_tag: ");
                    return FAIL;
                }
            } else {
                __dbg_printf("random AES enc test success, ran_algo = %08x\n", ran_algo);
                // return SUCCESS;
            }
        } else {
            __err_printf("random AES enc test fail, ran_algo = %08x\n", ran_algo);
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message_ori, msglen, "message_ori: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
        }
    } else {
        if (ran_algo == TEST_CIPHER_AES_GMAC || ran_algo == TEST_CIPHER_AES_GHASH) {
            // do nothing, go to descryption part
        } else {
            __err_printf("Crypto Engine random AES enc compute fail, ret = %d\n", ret);
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message_ori, msglen, "message_ori: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
        }
    }

    // decryption
    switch (ran_algo) {
        case TEST_CIPHER_AES_ECB:
            key = cipher_key_in;
            //keylen = ;
            iv = NULL;
            //ivlen = ;
            hw_message_dec = hw_result;
            //msglen = ;
            // hw computation
            ret = hal_crypto_aes_ecb_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ecb_decrypt(hw_message_dec, msglen, iv, ivlen, hw_result);
            // sw computation
            sw_message_dec = sw_result;
            mbedtls_aes_init(&ctx);
            mbedtls_aes_setkey_dec(&ctx, key, keylen * 8);
            for (i = 0; i < msglen; i += 16) {
                mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_DECRYPT, &sw_message_dec[i], &sw_result[i]);
            }
            break;
        case TEST_CIPHER_AES_CBC:
            key = cipher_key_in;
            //keylen = ;
            memcpy(process_iv, cipher_iv_in, ivlen);
            iv = process_iv;
            //ivlen = ;
            hw_message_dec = hw_result;
            //msglen = ;
            // hw computation
            ret = hal_crypto_aes_cbc_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_cbc_decrypt(hw_message_dec, msglen, iv, ivlen, hw_result);
            // sw computation
            sw_message_dec = sw_result;
            mbedtls_aes_init(&ctx);
            mbedtls_aes_setkey_dec(&ctx, key, keylen * 8);
            mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, msglen, iv, sw_message_dec, sw_result);
            break;
        case TEST_CIPHER_AES_CFB:
            key = cipher_key_in;
            //keylen = ;
            memcpy(process_iv, cipher_iv_in, ivlen);
            iv = process_iv;
            //ivlen = ;
            hw_message_dec = hw_result;
            //msglen = ;
            // hw computation
            ret = hal_crypto_aes_cfb_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_cfb_decrypt(hw_message_dec, msglen, iv, ivlen, hw_result);
            // sw computation
            sw_message_dec = sw_result;
            offset = 0;
            mbedtls_aes_init(&ctx);
            mbedtls_aes_setkey_enc(&ctx, key, keylen * 8); // set dec key in CFB mode is the same as enc
            mbedtls_aes_crypt_cfb128(&ctx, MBEDTLS_AES_DECRYPT, msglen, &offset, iv, sw_message_dec, sw_result);
            break;
        case TEST_CIPHER_AES_OFB:
            key = cipher_key_in;
            //keylen = ;
            iv = cipher_iv_in;
            //ivlen = ;
            hw_message_dec = hw_result;
            //msglen = ;
            // hw computation
            ret = hal_crypto_aes_ofb_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ofb_decrypt(hw_message_dec, msglen, iv, ivlen, hw_result);
            // sw computation
            sw_message_dec = sw_result;
            offset = 0;
            mbedtls_aes_init(&ctx);
            mbedtls_aes_setkey_enc(&ctx, key, keylen * 8); // set dec key in OFB mode is the same as enc
            mbedtls_aes_crypt_ofb128(&ctx, msglen, &offset, iv, sw_message_dec, sw_result);
            break;
        case TEST_CIPHER_AES_CTR:
            key = cipher_key_in;
            //keylen = ;
            iv = cipher_iv_in;
            //ivlen = ;
            hw_message_dec = hw_result;
            //msglen = ;
            // hw computation
            ret = hal_crypto_aes_ctr_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ctr_decrypt(hw_message_dec, msglen, iv, ivlen, hw_result);
            // sw computation
            sw_message_dec = sw_result;
            offset = 0;
            rtlc_memcpy(nonce_ctr, iv, sizeof(nonce_ctr));
            rtlc_memset(stream_blk, 0x0, sizeof(stream_blk));
            mbedtls_aes_init(&ctx);
            mbedtls_aes_setkey_enc(&ctx, key, keylen * 8); // set dec key in CTR mode is the same as enc
            mbedtls_aes_crypt_ctr(&ctx, msglen, &offset, (u8 *)nonce_ctr, (u8 *)stream_blk, sw_message_dec, sw_result);
            break;
        case TEST_CIPHER_AES_GCTR:
            key = cipher_key_in;
            //keylen = ;
            iv = cipher_iv_in;
            //ivlen = ;
            hw_message_dec = hw_result;
            //msglen = ;
            // hw computation
            ret = hal_crypto_aes_gctr_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_gctr_decrypt(hw_message_dec, msglen, iv, hw_result);
            // sw computation
            sw_message_dec = sw_result;
            mbedtls_aes_init(&ctx);
            rtlc_memcpy(nonce_ctr, iv, sizeof(nonce_ctr));
            aes_gctr_swvrf(key, keylen, (u8 *)nonce_ctr, sw_message_dec, msglen, sw_result);
            break;
        case TEST_CIPHER_AES_GMAC:
            key = cipher_key_in;
            //keylen = ;
            iv = cipher_iv_in;
            //ivlen = ;
            aad = cipher_aad_in;
            //aadlen = ;
            hw_message_dec = &cipher_msg_in[src_pos];
            //msglen = ;
            // hw computation
            ret = hal_crypto_aes_gmac_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_gmac_process(hw_message_dec, msglen, iv, aad, aadlen, hw_tag);
            // sw computation
            taglen = 16;
            sw_message_dec = &cipher_msg_in[src_pos];
            mbedtls_aes_init(&ctx);
            aes_gmac_swvrf(key, keylen, iv, sw_message_dec, msglen, aad, aadlen, sw_tag);
            break;
        case TEST_CIPHER_AES_GHASH:
            key = cipher_key_in;
            //keylen = ;
            hw_message_dec = &cipher_msg_in[src_pos];
            //msglen = ;
            // hw computation
            ret = hal_crypto_aes_ghash_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ghash_process(hw_message_dec, msglen, hw_tag);
            // sw computation
            taglen = 16;
            sw_message_dec = &cipher_msg_in[src_pos];
            mbedtls_aes_init(&ctx);
            aes_ghash_swvrf(key, keylen, sw_message_dec, msglen, sw_tag);
            break;
        case TEST_CIPHER_AES_GCM:
            key = cipher_key_in;
            //keylen = ;
            iv = cipher_iv_in;
            //ivlen = ;
            aad = cipher_aad_in;
            //aadlen = ;
            hw_message_dec = hw_result;
            //msglen = ;
            // hw computation
            ret = hal_crypto_aes_gcm_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_gcm_decrypt(hw_message_dec, msglen, iv, aad, aadlen, hw_result, hw_tag);
            // sw computation
            sw_message_dec = sw_result;
            taglen = 16;
            mbedtls_aes_init(&ctx);
            mbedtls_gcm_init(&ctx_gcm);
            cipher = MBEDTLS_CIPHER_ID_AES;
            mbedtls_gcm_setkey(&ctx_gcm, cipher, key, keylen * 8);
            mbedtls_gcm_crypt_and_tag(&ctx_gcm, MBEDTLS_GCM_DECRYPT, msglen, iv, 12,
                                        aad, aadlen, sw_message_dec, sw_result, taglen, sw_tag);
            mbedtls_gcm_free( &ctx_gcm);
            break;
        default:
            __err_printf("ran_algo(%08x): no this ran_algo\n", ran_algo);
            break;
    }

    if (ret == SUCCESS) {
        if (ran_algo == TEST_CIPHER_AES_GMAC || ran_algo == TEST_CIPHER_AES_GHASH) {
            if (rtlc_memcmp(hw_tag, sw_tag, taglen) == 0) {
                __dbg_printf("random AES dec test success, ran_algo = %08x\n", ran_algo);
                return SUCCESS;
            } else {
                __info_printf("random AES dec tag test success, ran_algo = %08x\n", ran_algo);
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_message_dec, msglen, "hw_message_dec: ");
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)aad, aadlen, "aad: ");
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_tag, taglen, "hw_tag: ");
                __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_tag, taglen, "sw_tag: ");
                return FAIL;
            }
        }
        if (rtlc_memcmp(hw_result, message_ori, msglen) == 0) {
            if (ran_algo == TEST_CIPHER_AES_GCM) {
                if (rtlc_memcmp(hw_tag, sw_tag, taglen) == 0) {
                    __dbg_printf("random AES dec test success, ran_algo = %08x\n", ran_algo);
                    return SUCCESS;
                } else {
                    __info_printf("random AES dec tag test success, ran_algo = %08x\n", ran_algo);
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)aad, aadlen, "aad: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_tag, taglen, "hw_tag: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_tag, taglen, "sw_tag: ");
                    return FAIL;
                }
            } else {
                __dbg_printf("random AES dec test success, ran_algo = %08x\n", ran_algo);
                return SUCCESS;
            }
        } else {
            __err_printf("random AES dec test fail, ran_algo = %08x\n", ran_algo);
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message_ori, msglen, "message_ori: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
        }
    } else {
        __err_printf("Crypto Engine random AES dec compute fail, ret = %d\n", ret);
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)message_ori, msglen, "message_ori: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
        return FAIL;
    }

    //return FAIL;
}

s32 random_cipher_test(
    IN u8 ran_mode,
    IN u8 ran_algo
)
{
    s32 ret = FAIL;

    switch (ran_mode) {
        case TEST_CIPHER_DES:
            ret = random_cipher_des_test(ran_algo);
            break;
        case TEST_CIPHER_3DES:
            ret = random_cipher_3des_test(ran_algo);
            break;
        case TEST_CIPHER_AES:
            ret = random_cipher_aes_test(ran_algo);
            break;
        default:
            __err_printf("ran_mode(%08x): no this ran_mode\n", ran_mode);
    }

    return ret;
}

s32 vector_cipher_des_test(
	IN u8 vec_algo
)
{
	s32 ret = FAIL;
	u8 *key = NULL;
	u32 keylen = 0;
	u8 *iv = NULL;
	u32 ivlen = 0;
	u8 *message = NULL;
	u32 msglen = 0;
	u8 hw_result[64], *sw_result = NULL;

	// encryption
	switch (vec_algo) {
		case TEST_CIPHER_DES_ECB:
			key = des_test_key;
			keylen = sizeof(des_test_key);
			iv = NULL;
			ivlen = 0;
			message = des_test_msg;
			msglen = sizeof(des_test_msg);
			sw_result = des_ecb_test_res;
			ret = hal_crypto_des_ecb_init(key, keylen); CHK(ret);
			ret = hal_crypto_des_ecb_encrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_DES_CBC:
			key = des_test_key;
			keylen = sizeof(des_test_key);
			iv = des_test_iv;
			ivlen = sizeof(iv);
			message = des_test_msg;
			msglen = sizeof(des_test_msg);
			sw_result = des_cbc_test_res;
			ret = hal_crypto_des_cbc_init(key, keylen); CHK(ret);
			ret = hal_crypto_des_cbc_encrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_DES_CFB:
			key = des_test_key;
			keylen = sizeof(des_test_key);
			iv = des_test_iv;
			ivlen = sizeof(iv);
			message = des_test_msg;
			msglen = sizeof(des_test_msg);
			sw_result = des_cfb_test_res;
			ret = hal_crypto_des_cfb_init(key, keylen); CHK(ret);
			ret = hal_crypto_des_cfb_encrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_DES_OFB:
			key = des_test_key;
			keylen = sizeof(des_test_key);
			iv = des_test_iv;
			ivlen = sizeof(iv);
			message = des_test_msg;
			msglen = sizeof(des_test_msg);
			sw_result = des_ofb_test_res;
			ret = hal_crypto_des_ofb_init(key, keylen); CHK(ret);
			ret = hal_crypto_des_ofb_encrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_DES_CTR:
			key = des_test_key;
            keylen = sizeof(des_test_key);
            iv = des_test_iv;
            ivlen = sizeof(iv);
            message = des_test_msg;
            msglen = sizeof(des_test_msg);
            sw_result = des_ctr_test_res;
            ret = hal_crypto_des_ctr_init(key, keylen); CHK(ret);
            ret = hal_crypto_des_ctr_encrypt(message, msglen, iv, ivlen, hw_result);
			break;
		default:
			__err_printf("vec_algo(%08x): no this vec_algo\n", vec_algo);
			break;
	}

	if (ret == SUCCESS) {
		if (rtlc_memcmp(hw_result, sw_result, msglen) == 0) {
			__info_printf("vector DES enc test success, vec_algo = %08x\n", vec_algo);
			// return SUCCESS;
		} else {
			__err_printf("vector DES enc test fail, vec_algo = %08x\n", vec_algo);
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
			__dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
		}
	} else {
		__err_printf("Crypto Engine vector DES enc compute fail, ret = %d\n", ret);
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
        return FAIL;
	}

	// decryption: message -> hw_reault; sw_result -> message
	switch (vec_algo) {
		case TEST_CIPHER_DES_ECB:
			key = des_test_key;
			keylen = sizeof(des_test_key);
			iv = NULL;
			ivlen = 0;
			message = hw_result;
			msglen = sizeof(des_test_msg);
			sw_result = des_test_msg;
			ret = hal_crypto_des_ecb_init(key, keylen); CHK(ret);
			ret = hal_crypto_des_ecb_decrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_DES_CBC:
			key = des_test_key;
			keylen = sizeof(des_test_key);
			iv = des_test_iv;
			ivlen = sizeof(des_test_iv);
			message = hw_result;
			msglen = sizeof(des_test_msg);
			sw_result = des_test_msg;
			ret = hal_crypto_des_cbc_init(key, keylen); CHK(ret);
			ret = hal_crypto_des_cbc_decrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_DES_CFB:
			key = des_test_key;
			keylen = sizeof(des_test_key);
			iv = des_test_iv;
			ivlen = sizeof(des_test_iv);
			message = hw_result;
			msglen = sizeof(des_test_msg);
			sw_result = des_test_msg;
			ret = hal_crypto_des_cfb_init(key, keylen); CHK(ret);
			ret = hal_crypto_des_cfb_decrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_DES_OFB:
			key = des_test_key;
			keylen = sizeof(des_test_key);
			iv = des_test_iv;
			ivlen = sizeof(des_test_iv);
			message = hw_result;
			msglen = sizeof(des_test_msg);
			sw_result = des_test_msg;
			ret = hal_crypto_des_ofb_init(key, keylen); CHK(ret);
			ret = hal_crypto_des_ofb_decrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_DES_CTR:
			key = des_test_key;
            keylen = sizeof(des_test_key);
            iv = des_test_iv;
            ivlen = sizeof(des_test_iv);
            message = hw_result;
            msglen = sizeof(des_test_msg);
            sw_result = des_test_msg;
            ret = hal_crypto_des_ctr_init(key, keylen); CHK(ret);
            ret = hal_crypto_des_ctr_decrypt(message, msglen, iv, ivlen, hw_result);
			break;
		default:
			__err_printf("vec_algo(%08x): no this vec_algo\n", vec_algo);
			break;
	}

	if (ret == SUCCESS) {
		if (rtlc_memcmp(hw_result, sw_result, msglen) == 0) {
			__info_printf("vector DES dec test success, vec_algo = %08x\n", vec_algo);
			return SUCCESS;
		} else {
			__err_printf("vector DES dec test fail, vec_algo = %08x\n", vec_algo);
			__dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
		}
	} else {
		__err_printf("Crypto Engine vector DES dec compute fail, ret = %d\n", ret);
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
        return FAIL;
	}

	//return FAIL;
}

s32 vector_cipher_3des_test(
	IN u8 vec_algo
)
{
	s32 ret = FAIL;
	u8 *key = NULL;
	u32 keylen = 0;
	u8 *iv = NULL;
	u32 ivlen = 0;
	u8 *message = NULL;
	u32 msglen = 0;
	u8 hw_result[64], *sw_result = NULL;

	// encryption
	switch (vec_algo) {
		case TEST_CIPHER_3DES_ECB:
			key = trides_test_key;
			keylen = sizeof(trides_test_key);
			iv = NULL;
			ivlen = 0;
			message = trides_test_msg;
			msglen = sizeof(trides_test_msg);
			sw_result = trides_ecb_test_res;
			ret = hal_crypto_3des_ecb_init(key, keylen); CHK(ret);
			ret = hal_crypto_3des_ecb_encrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_3DES_CBC:
			key = trides_test_key;
			keylen = sizeof(trides_test_key);
			iv = trides_test_iv;
			ivlen = sizeof(trides_test_iv);
			message = trides_test_msg;
			msglen = sizeof(trides_test_msg);
			sw_result = trides_cbc_test_res;
			ret = hal_crypto_3des_cbc_init(key, keylen); CHK(ret);
			ret = hal_crypto_3des_cbc_encrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_3DES_CFB:
			key = trides_test_key;
			keylen = sizeof(trides_test_key);
			iv = trides_test_iv;
			ivlen = sizeof(trides_test_iv);
			message = trides_test_msg;
			msglen = sizeof(trides_test_msg);
			sw_result = trides_cfb_test_res;
			ret = hal_crypto_3des_cfb_init(key, keylen); CHK(ret);
			ret = hal_crypto_3des_cfb_encrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_3DES_OFB:
			key = trides_test_key;
			keylen = sizeof(trides_test_key);
			iv = trides_test_iv;
			ivlen = sizeof(trides_test_iv);
			message = trides_test_msg;
			msglen = sizeof(trides_test_msg);
			sw_result = trides_ofb_test_res;
			ret = hal_crypto_3des_ofb_init(key, keylen); CHK(ret);
			ret = hal_crypto_3des_ofb_encrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_3DES_CTR:
			key = trides_test_key;
            keylen = sizeof(trides_test_key);
            iv = trides_test_iv;
            ivlen = sizeof(trides_test_iv);
            message = trides_test_msg;
            msglen = sizeof(trides_test_msg);
            sw_result = trides_ctr_test_res;
            ret = hal_crypto_3des_ctr_init(key, keylen); CHK(ret);
            ret = hal_crypto_3des_ctr_encrypt(message, msglen, iv, ivlen, hw_result);
			break;
		default:
			__err_printf("vec_algo(%08x): no this vec_algo\n", vec_algo);
			break;
	}

	if (ret == SUCCESS) {
		if (rtlc_memcmp(hw_result, sw_result, msglen) == 0) {
			__info_printf("vector 3DES enc test success, vec_algo = %08x\n", vec_algo);
			// return SUCCESS;
		} else {
			__err_printf("vector 3DES enc test fail, vec_algo = %08x\n", vec_algo);
			__dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
		}
	} else {
		__err_printf("Crypto Engine vector 3DES enc compute fail, ret = %d\n", ret);
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
        return FAIL;
	}

	// decryption: message -> hw_reault; sw_result -> message
	switch (vec_algo) {
		case TEST_CIPHER_3DES_ECB:
			key = trides_test_key;
			keylen = sizeof(trides_test_key);
			iv = NULL;
			ivlen = 0;
			message = hw_result;
			msglen = sizeof(trides_test_msg);
			sw_result = trides_test_msg;
			ret = hal_crypto_3des_ecb_init(key, keylen); CHK(ret);
			ret = hal_crypto_3des_ecb_decrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_3DES_CBC:
			key = trides_test_key;
			keylen = sizeof(trides_test_key);
			iv = trides_test_iv;
			ivlen = sizeof(trides_test_iv);
			message = hw_result;
			msglen = sizeof(trides_test_msg);
			sw_result = trides_test_msg;
			ret = hal_crypto_3des_cbc_init(key, keylen); CHK(ret);
			ret = hal_crypto_3des_cbc_decrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_3DES_CFB:
			key = trides_test_key;
			keylen = sizeof(trides_test_key);
			iv = trides_test_iv;
			ivlen = sizeof(trides_test_iv);
			message = hw_result;
			msglen = sizeof(trides_test_msg);
			sw_result = trides_test_msg;
			ret = hal_crypto_3des_cfb_init(key, keylen); CHK(ret);
			ret = hal_crypto_3des_cfb_decrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_3DES_OFB:
			key = trides_test_key;
			keylen = sizeof(trides_test_key);
			iv = trides_test_iv;
			ivlen = sizeof(trides_test_iv);
			message = hw_result;
			msglen = sizeof(trides_test_msg);
			sw_result = trides_test_msg;
			ret = hal_crypto_3des_ofb_init(key, keylen); CHK(ret);
			ret = hal_crypto_3des_ofb_decrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_3DES_CTR:
			key = trides_test_key;
            keylen = sizeof(trides_test_key);
            iv = trides_test_iv;
            ivlen = sizeof(trides_test_iv);
            message = hw_result;
            msglen = sizeof(trides_test_msg);
            sw_result = trides_test_msg;
            ret = hal_crypto_3des_ctr_init(key, keylen); CHK(ret);
            ret = hal_crypto_3des_ctr_decrypt(message, msglen, iv, ivlen, hw_result);
			break;
		default:
			__err_printf("vec_algo(%08x): no this vec_algo\n", vec_algo);
			break;
	}

	if (ret == SUCCESS) {
		if (rtlc_memcmp(hw_result, sw_result, msglen) == 0) {
			__info_printf("vector 3DES dec test success, vec_algo = %08x\n", vec_algo);
			return SUCCESS;
		} else {
			__err_printf("vector 3DES dec test fail, vec_algo = %08x\n", vec_algo);
			__dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
		}
	} else {
		__err_printf("Crypto Engine vector 3DES dec compute fail, ret = %d\n", ret);
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
        return FAIL;
	}

	//return FAIL;
}

s32 vector_cipher_aes128_test(
    IN u8 vec_algo
)
{
    s32 ret = FAIL;
    u8 *key = NULL;
    u32 keylen = 0;
    u8 *iv = NULL;
    u32 ivlen = 0;
    u8 *message = NULL;
    u32 msglen = 0;
    u8 *aad = NULL;
    u32 aadlen = 0;
    u8 hw_result[64], *sw_result = NULL;
    u8 hw_tag[max_cipher_taglen], *sw_tag = NULL;

    // encryption
    switch (vec_algo) {
        case TEST_CIPHER_AES_ECB:
            key = aes_ecb_128_test_key;
            keylen = sizeof(aes_ecb_128_test_key);
            iv = NULL;
            ivlen = 0;
            message = aes_ecb_128_test_msg;
            msglen = sizeof(aes_ecb_128_test_msg);
            sw_result = aes_ecb_128_test_res;
            ret = hal_crypto_aes_ecb_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ecb_encrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_CBC:
            key = aes_cbc_128_test_key;
            keylen = sizeof(aes_cbc_128_test_key);
            iv = aes_cbc_128_test_iv;
            ivlen = sizeof(aes_cbc_128_test_iv);
            message = aes_cbc_128_test_msg;
            msglen = sizeof(aes_cbc_128_test_msg);
            sw_result = aes_cbc_128_test_res;
            ret = hal_crypto_aes_cbc_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_cbc_encrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_CFB:
            key = aes_cfb_128_test_key;
            keylen = sizeof(aes_cfb_128_test_key);
            iv = aes_cfb_128_test_iv;
            ivlen = sizeof(aes_cfb_128_test_iv);
            message = aes_cfb_128_test_msg;
            msglen = sizeof(aes_cfb_128_test_msg);
            sw_result = aes_cfb_128_test_res;
            ret = hal_crypto_aes_cfb_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_cfb_encrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_OFB:
            key = aes_ofb_128_test_key;
            keylen = sizeof(aes_ofb_128_test_key);
            iv = aes_ofb_128_test_iv;
            ivlen = sizeof(aes_ofb_128_test_iv);
            message = aes_ofb_128_test_msg;
            msglen = sizeof(aes_ofb_128_test_msg);
            sw_result = aes_ofb_128_test_res;
            ret = hal_crypto_aes_ofb_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ofb_encrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_CTR:
            key = aes_ctr_128_test_key;
            keylen = sizeof(aes_ctr_128_test_key);
            iv = aes_ctr_128_test_iv;
            ivlen = sizeof(aes_ctr_128_test_iv);
            message = aes_ctr_128_test_msg;
            msglen = sizeof(aes_ctr_128_test_msg);
            sw_result = aes_ctr_128_test_res;
            ret = hal_crypto_aes_ctr_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ctr_encrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_GCTR:
            key = aes_gctr_128_test_key;
            keylen = sizeof(aes_gctr_128_test_key);
            iv = aes_gctr_128_test_iv;
            ivlen = sizeof(aes_gctr_128_test_iv);
            message = aes_gctr_128_test_msg;
            msglen = sizeof(aes_gctr_128_test_msg);
            sw_result = aes_gctr_128_test_res;
            ret = hal_crypto_aes_gctr_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_gctr_encrypt(message, msglen, iv, hw_result);
            break;
        case TEST_CIPHER_AES_GMAC:
            // dec only
            break;
        case TEST_CIPHER_AES_GHASH:
            // dec only
            break;
        case TEST_CIPHER_AES_GCM:
            key = aes_gcm_128_test_key;
            keylen = sizeof(aes_gcm_128_test_key);
            iv = aes_gcm_128_test_iv;
            ivlen = sizeof(aes_gcm_128_test_iv);
            aad = aes_gcm_128_test_aad;
            aadlen = sizeof(aes_gcm_128_test_aad);
            message = aes_gcm_128_test_msg;
            msglen = sizeof(aes_gcm_128_test_msg);
            sw_result = aes_gcm_128_test_res;
            sw_tag = aes_gcm_128_test_tag;
            ret = hal_crypto_aes_gcm_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_gcm_encrypt(message, msglen, iv, aad, aadlen, hw_result, hw_tag);
            break;
        default:
            __err_printf("vec_algo(%08x): no this vec_algo\n", vec_algo);
            break;
    }

    if (ret == SUCCESS) {
        if (rtlc_memcmp(hw_result, sw_result, msglen) == 0) {
            if (vec_algo == TEST_CIPHER_AES_GCM) {
                if (rtlc_memcmp(hw_tag, sw_tag, sizeof(sw_tag)) == 0) {
                    __info_printf("vector AES128 enc test success, vec_algo = %08x\n", vec_algo);
                    // return SUCCESS;
                } else {
                    __info_printf("vector AES128 enc tag test fail, vec_algo = %08x\n", vec_algo);
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)aad, aadlen, "aad: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_tag, 32, "hw_tag: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_tag, 32, "sw_tag: ");
                    return FAIL;
                }
            } else {
                __info_printf("vector AES128 enc test success, vec_algo = %08x\n", vec_algo);
                // return SUCCESS;
            }
        } else {
            __err_printf("vector AES128 enc test fail, vec_algo = %08x\n", vec_algo);
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
        }
    } else {
        if (vec_algo == TEST_CIPHER_AES_GMAC || vec_algo == TEST_CIPHER_AES_GHASH) {
            // do nothing, wait dec
        } else {
            __err_printf("Crypto Engine vector AES128 enc compute fail, ret = %d\n", ret);
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
        }
    }

    // decryption: message -> hw_reault; sw_result -> message
    switch (vec_algo) {
        case TEST_CIPHER_AES_ECB:
            key = aes_ecb_128_test_key;
            keylen = sizeof(aes_ecb_128_test_key);
            iv = NULL;
            ivlen = 0;
            message = hw_result;
            msglen = sizeof(aes_ecb_128_test_msg);
            sw_result = aes_ecb_128_test_msg;
            ret = hal_crypto_aes_ecb_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ecb_decrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_CBC:
            key = aes_cbc_128_test_key;
            keylen = sizeof(aes_cbc_128_test_key);
            iv = aes_cbc_128_test_iv;
            ivlen = sizeof(aes_cbc_128_test_iv);
            message = hw_result;
            msglen = sizeof(aes_cbc_128_test_msg);
            sw_result = aes_cbc_128_test_msg;
            ret = hal_crypto_aes_cbc_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_cbc_decrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_CFB:
            key = aes_cfb_128_test_key;
            keylen = sizeof(aes_cfb_128_test_key);
            iv = aes_cfb_128_test_iv;
            ivlen = sizeof(aes_cfb_128_test_iv);
            message = hw_result;
            msglen = sizeof(aes_cfb_128_test_msg);
            sw_result = aes_cfb_128_test_msg;
            ret = hal_crypto_aes_cfb_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_cfb_decrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_OFB:
            key = aes_ofb_128_test_key;
            keylen = sizeof(aes_ofb_128_test_key);
            iv = aes_ofb_128_test_iv;
            ivlen = sizeof(aes_ofb_128_test_iv);
            message = sw_result;
            msglen = sizeof(aes_ofb_128_test_msg);
            sw_result = aes_ofb_128_test_msg;
            ret = hal_crypto_aes_ofb_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ofb_decrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_CTR:
            key = aes_ctr_128_test_key;
            keylen = sizeof(aes_ctr_128_test_key);
            iv = aes_ctr_128_test_iv;
            ivlen = sizeof(aes_ctr_128_test_iv);
            message = hw_result;
            msglen = sizeof(aes_ctr_128_test_msg);
            sw_result = aes_ctr_128_test_msg;
            ret = hal_crypto_aes_ctr_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ctr_decrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_GCTR:
            key = aes_gctr_128_test_key;
            keylen = sizeof(aes_gctr_128_test_key);
            iv = aes_gctr_128_test_iv;
            ivlen = sizeof(aes_gctr_128_test_iv);
            message = hw_result;
            msglen = sizeof(aes_gctr_128_test_msg);
            sw_result = aes_gctr_128_test_msg;
            ret = hal_crypto_aes_gctr_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_gctr_decrypt(message, msglen, iv, hw_result);
            break;
        case TEST_CIPHER_AES_GMAC:
            // dec only
            key = aes_gmac_128_test_key;
            keylen = sizeof(aes_gmac_128_test_key);
            iv = aes_gmac_128_test_iv;
            ivlen = sizeof(aes_gmac_128_test_iv);
            aad = aes_gmac_128_test_aad;
            aadlen = sizeof(aes_gmac_128_test_aad);
            message = aes_gmac_128_test_res;
            msglen = sizeof(aes_gmac_128_test_res);
            sw_result = NULL;
            sw_tag = aes_gmac_128_test_tag;
            ret = hal_crypto_aes_gmac_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_gmac_process(message, msglen, iv, aad, aadlen, hw_tag);
            break;
        case TEST_CIPHER_AES_GHASH:
            // dec only
            key = aes_ghash_test_key;
            keylen = sizeof(aes_ghash_test_key);
            iv = NULL;
            ivlen = 0;
            message = aes_ghash_test_msg;
            msglen = sizeof(aes_ghash_test_msg);
            sw_result = NULL;
            sw_tag = aes_ghash_test_tag;
            ret = hal_crypto_aes_ghash_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ghash_process(message, msglen, hw_tag);
            break;
        case TEST_CIPHER_AES_GCM:
            key = aes_gcm_128_test_key;
            keylen = sizeof(aes_gcm_128_test_key);
            iv = aes_gcm_128_test_iv;
            ivlen = sizeof(aes_gcm_128_test_iv);
            aad = aes_gcm_128_test_aad;
            aadlen = sizeof(aes_gcm_128_test_aad);
            message = hw_result;
            msglen = sizeof(aes_gcm_128_test_msg);
            sw_result = aes_gcm_128_test_msg;
            sw_tag = aes_gcm_128_test_tag;
            ret = hal_crypto_aes_gcm_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_gcm_decrypt(message, msglen, iv, aad, aadlen, hw_result, hw_tag);
            break;
        default:
            __err_printf("vec_algo(%08x): no this vec_algo\n", vec_algo);
            break;
    }

    if (ret == SUCCESS) {
        if (vec_algo == TEST_CIPHER_AES_GMAC || vec_algo == TEST_CIPHER_AES_GHASH) {
            // compare tag only
            if (rtlc_memcmp(hw_tag, sw_tag, sizeof(sw_tag)) == 0) {
                    __info_printf("vector AES128 dec test success, vec_algo = %08x\n", vec_algo);
                    return SUCCESS;
                } else {
                    __info_printf("vector AES128 dec tag test fail, vec_algo = %08x\n", vec_algo);
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)aad, aadlen, "aad: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_tag, 32, "hw_tag: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_tag, 32, "sw_tag: ");
                    return FAIL;
                }
        }
        if (rtlc_memcmp(hw_result, sw_result, msglen) == 0) {
            if (vec_algo == TEST_CIPHER_AES_GCM) {
                if (rtlc_memcmp(hw_tag, sw_tag, sizeof(sw_tag)) == 0) {
                    __info_printf("vector AES dec test success, vec_algo = %08x\n", vec_algo);
                    return SUCCESS;
                } else {
                    __info_printf("vector AES128 dec tag test fail, vec_algo = %08x\n", vec_algo);
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)aad, aadlen, "aad: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_tag, 32, "hw_tag: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_tag, 32, "sw_tag: ");
                    return FAIL;
                }
            } else {
                __info_printf("vector AES128 dec test success, vec_algo = %08x\n", vec_algo);
                return SUCCESS;
            }
        } else {
            __err_printf("vector AES128 dec test fail, vec_algo = %08x\n", vec_algo);
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
        }
    } else {
        __err_printf("Crypto Engine vector AES128 dec compute fail, ret = %d\n", ret);
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
        return FAIL;
    }

    //return FAIL;
}

s32 vector_cipher_aes192_test(
    IN u8 vec_algo
)
{
    s32 ret = FAIL;
    u8 *key = NULL;
    u32 keylen = 0;
    u8 *iv = NULL;
    u32 ivlen = 0;
    u8 *message = NULL;
    u32 msglen = 0;
    u8 *aad = NULL;
    u32 aadlen = 0;
    u8 hw_result[64], *sw_result = NULL;
    u8 hw_tag[max_cipher_taglen], *sw_tag = NULL;

    // encryption
    switch (vec_algo) {
        case TEST_CIPHER_AES_ECB:
            key = aes_ecb_192_test_key;
            keylen = sizeof(aes_ecb_192_test_key);
            iv = NULL;
            ivlen = 0;
            message = aes_ecb_192_test_msg;
            msglen = sizeof(aes_ecb_192_test_msg);
            sw_result = aes_ecb_192_test_res;
            ret = hal_crypto_aes_ecb_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ecb_encrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_CBC:
            key = aes_cbc_192_test_key;
            keylen = sizeof(aes_cbc_192_test_key);
            iv = aes_cbc_192_test_iv;
            ivlen = sizeof(aes_cbc_192_test_iv);
            message = aes_cbc_192_test_msg;
            msglen = sizeof(aes_cbc_192_test_msg);
            sw_result = aes_cbc_192_test_res;
            ret = hal_crypto_aes_cbc_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_cbc_encrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_CFB:
            key = aes_cfb_192_test_key;
            keylen = sizeof(aes_cfb_192_test_key);
            iv = aes_cfb_192_test_iv;
            ivlen = sizeof(aes_cfb_192_test_iv);
            message = aes_cfb_192_test_msg;
            msglen = sizeof(aes_cfb_192_test_msg);
            sw_result = aes_cfb_192_test_res;
            ret = hal_crypto_aes_cfb_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_cfb_encrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_OFB:
            key = aes_ofb_192_test_key;
            keylen = sizeof(aes_ofb_192_test_key);
            iv = aes_ofb_192_test_iv;
            ivlen = sizeof(aes_ofb_192_test_iv);
            message = aes_ofb_192_test_msg;
            msglen = sizeof(aes_ofb_192_test_msg);
            sw_result = aes_ofb_192_test_res;
            ret = hal_crypto_aes_ofb_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ofb_encrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_CTR:
            key = aes_ctr_192_test_key;
            keylen = sizeof(aes_ctr_192_test_key);
            iv = aes_ctr_192_test_iv;
            ivlen = sizeof(aes_ctr_192_test_iv);
            message = aes_ctr_192_test_msg;
            msglen = sizeof(aes_ctr_192_test_msg);
            sw_result = aes_ctr_192_test_res;
            ret = hal_crypto_aes_ctr_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ctr_encrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_GCTR:
            key = aes_gctr_192_test_key;
            keylen = sizeof(aes_gctr_192_test_key);
            iv = aes_gctr_192_test_iv;
            ivlen = sizeof(aes_gctr_192_test_iv);
            message = aes_gctr_192_test_msg;
            msglen = sizeof(aes_gctr_192_test_msg);
            sw_result = aes_gctr_192_test_res;
            ret = hal_crypto_aes_gctr_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_gctr_encrypt(message, msglen, iv, hw_result);
            break;
        case TEST_CIPHER_AES_GMAC:
            // dec only
            break;
        case TEST_CIPHER_AES_GHASH:
            // dec only
            break;
        case TEST_CIPHER_AES_GCM:
            key = aes_gcm_192_test_key;
            keylen = sizeof(aes_gcm_192_test_key);
            iv = aes_gcm_192_test_iv;
            ivlen = sizeof(aes_gcm_192_test_iv);
            aad = aes_gcm_192_test_aad;
            aadlen = sizeof(aes_gcm_192_test_aad);
            message = aes_gcm_192_test_msg;
            msglen = sizeof(aes_gcm_192_test_msg);
            sw_result = aes_gcm_192_test_res;
            sw_tag = aes_gcm_192_test_tag;
            ret = hal_crypto_aes_gcm_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_gcm_encrypt(message, msglen, iv, aad, aadlen, hw_result, hw_tag);
            break;
        default:
            __err_printf("vec_algo(%08x): no this vec_algo\n", vec_algo);
            break;
    }

    if (ret == SUCCESS) {
        if (rtlc_memcmp(hw_result, sw_result, msglen) == 0) {
            if (vec_algo == TEST_CIPHER_AES_GCM) {
                if (rtlc_memcmp(hw_tag, sw_tag, sizeof(sw_tag)) == 0) {
                    __info_printf("vector AES192 enc test success, vec_algo = %08x\n", vec_algo);
                    // return SUCCESS;
                } else {
                    __info_printf("vector AES192 enc tag test fail, vec_algo = %08x\n", vec_algo);
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)aad, aadlen, "aad: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_tag, 32, "hw_tag: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_tag, 32, "sw_tag: ");
                    return FAIL;
                }
            } else {
                __info_printf("vector AES192 enc test success, vec_algo = %08x\n", vec_algo);
                // return SUCCESS;
            }
        } else {
            __err_printf("vector AES192 enc test fail, vec_algo = %08x\n", vec_algo);
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
        }
    } else {
        if (vec_algo == TEST_CIPHER_AES_GMAC || vec_algo == TEST_CIPHER_AES_GHASH) {
            // do nothing, wait dec
        } else {
            __err_printf("Crypto Engine vector AES192 enc compute fail, ret = %d\n", ret);
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
        }
    }

    // decryption: message -> hw_reault; sw_result -> message
    switch (vec_algo) {
        case TEST_CIPHER_AES_ECB:
            key = aes_ecb_192_test_key;
            keylen = sizeof(aes_ecb_192_test_key);
            iv = NULL;
            ivlen = 0;
            message = hw_result;
            msglen = sizeof(aes_ecb_192_test_msg);
            sw_result = aes_ecb_192_test_msg;
            ret = hal_crypto_aes_ecb_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ecb_decrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_CBC:
            key = aes_cbc_192_test_key;
            keylen = sizeof(aes_cbc_192_test_key);
            iv = aes_cbc_192_test_iv;
            ivlen = sizeof(aes_cbc_192_test_iv);
            message = hw_result;
            msglen = sizeof(aes_cbc_192_test_msg);
            sw_result = aes_cbc_192_test_msg;
            ret = hal_crypto_aes_cbc_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_cbc_decrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_CFB:
            key = aes_cfb_192_test_key;
            keylen = sizeof(aes_cfb_192_test_key);
            iv = aes_cfb_192_test_iv;
            ivlen = sizeof(aes_cfb_192_test_iv);
            message = hw_result;
            msglen = sizeof(aes_cfb_192_test_msg);
            sw_result = aes_cfb_192_test_msg;
            ret = hal_crypto_aes_cfb_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_cfb_decrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_OFB:
            key = aes_ofb_192_test_key;
            keylen = sizeof(aes_ofb_192_test_key);
            iv = aes_ofb_192_test_iv;
            ivlen = sizeof(aes_ofb_192_test_iv);
            message = sw_result;
            msglen = sizeof(aes_ofb_192_test_msg);
            sw_result = aes_ofb_192_test_msg;
            ret = hal_crypto_aes_ofb_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ofb_decrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_CTR:
            key = aes_ctr_192_test_key;
            keylen = sizeof(aes_ctr_192_test_key);
            iv = aes_ctr_192_test_iv;
            ivlen = sizeof(aes_ctr_192_test_iv);
            message = hw_result;
            msglen = sizeof(aes_ctr_192_test_msg);
            sw_result = aes_ctr_192_test_msg;
            ret = hal_crypto_aes_ctr_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ctr_decrypt(message, msglen, iv, ivlen, hw_result);
            break;
        case TEST_CIPHER_AES_GCTR:
            key = aes_gctr_192_test_key;
            keylen = sizeof(aes_gctr_192_test_key);
            iv = aes_gctr_192_test_iv;
            ivlen = sizeof(aes_gctr_192_test_iv);
            message = hw_result;
            msglen = sizeof(aes_gctr_192_test_msg);
            sw_result = aes_gctr_192_test_msg;
            ret = hal_crypto_aes_gctr_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_gctr_decrypt(message, msglen, iv, hw_result);
            break;
        case TEST_CIPHER_AES_GMAC:
            // dec only
            key = aes_gmac_192_test_key;
            keylen = sizeof(aes_gmac_192_test_key);
            iv = aes_gmac_192_test_iv;
            ivlen = sizeof(aes_gmac_192_test_iv);
            aad = aes_gmac_192_test_aad;
            aadlen = sizeof(aes_gmac_192_test_aad);
            message = aes_gmac_192_test_res;
            msglen = sizeof(aes_gmac_192_test_res);
            sw_result = NULL;
            sw_tag = aes_gmac_192_test_tag;
            ret = hal_crypto_aes_gmac_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_gmac_process(message, msglen, iv, aad, aadlen, hw_tag);
            break;
        case TEST_CIPHER_AES_GHASH:
            // dec only
            key = aes_ghash_test_key;
            keylen = sizeof(aes_ghash_test_key);
            iv = NULL;
            ivlen = 0;
            message = aes_ghash_test_msg;
            msglen = sizeof(aes_ghash_test_msg);
            sw_result = NULL;
            sw_tag = aes_ghash_test_tag;
            ret = hal_crypto_aes_ghash_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ghash_process(message, msglen, hw_tag);
            break;
        case TEST_CIPHER_AES_GCM:
            key = aes_gcm_192_test_key;
            keylen = sizeof(aes_gcm_192_test_key);
            iv = aes_gcm_192_test_iv;
            ivlen = sizeof(aes_gcm_192_test_iv);
            aad = aes_gcm_192_test_aad;
            aadlen = sizeof(aes_gcm_192_test_aad);
            message = hw_result;
            msglen = sizeof(aes_gcm_192_test_msg);
            sw_result = aes_gcm_192_test_msg;
            sw_tag = aes_gcm_192_test_tag;
            ret = hal_crypto_aes_gcm_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_gcm_decrypt(message, msglen, iv, aad, aadlen, hw_result, hw_tag);
            break;
        default:
            __err_printf("vec_algo(%08x): no this vec_algo\n", vec_algo);
            break;
    }

    if (ret == SUCCESS) {
        if (vec_algo == TEST_CIPHER_AES_GMAC || vec_algo == TEST_CIPHER_AES_GHASH) {
            // compare tag only
            if (rtlc_memcmp(hw_tag, sw_tag, sizeof(sw_tag)) == 0) {
                    __info_printf("vector AES192 dec test success, vec_algo = %08x\n", vec_algo);
                    return SUCCESS;
                } else {
                    __info_printf("vector AES192 dec tag test fail, vec_algo = %08x\n", vec_algo);
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)aad, aadlen, "aad: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_tag, 32, "hw_tag: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_tag, 32, "sw_tag: ");
                    return FAIL;
                }
        }
        if (rtlc_memcmp(hw_result, sw_result, msglen) == 0) {
            if (vec_algo == TEST_CIPHER_AES_GCM) {
                if (rtlc_memcmp(hw_tag, sw_tag, sizeof(sw_tag)) == 0) {
                    __info_printf("vector AES dec test success, vec_algo = %08x\n", vec_algo);
                    return SUCCESS;
                } else {
                    __info_printf("vector AES192 dec tag test fail, vec_algo = %08x\n", vec_algo);
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)aad, aadlen, "aad: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_tag, 32, "hw_tag: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_tag, 32, "sw_tag: ");
                    return FAIL;
                }
            } else {
                __info_printf("vector AES192 dec test success, vec_algo = %08x\n", vec_algo);
                return SUCCESS;
            }
        } else {
            __err_printf("vector AES192 dec test fail, vec_algo = %08x\n", vec_algo);
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
        }
    } else {
        __err_printf("Crypto Engine vector AES192 dec compute fail, ret = %d\n", ret);
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
        return FAIL;
    }

    //return FAIL;
}

s32 vector_cipher_aes256_test(
	IN u8 vec_algo
)
{
	s32 ret = FAIL;
	u8 *key = NULL;
	u32 keylen = 0;
	u8 *iv = NULL;
	u32 ivlen = 0;
	u8 *message = NULL;
	u32 msglen = 0;
	u8 *aad = NULL;
	u32 aadlen = 0;
	u8 hw_result[64], *sw_result = NULL;
	u8 hw_tag[max_cipher_taglen], *sw_tag = NULL;

	// encryption
	switch (vec_algo) {
		case TEST_CIPHER_AES_ECB:
			key = aes_ecb_256_test_key;
			keylen = sizeof(aes_ecb_256_test_key);
			iv = NULL;
			ivlen = 0;
			message = aes_ecb_256_test_msg;
			msglen = sizeof(aes_ecb_256_test_msg);
			sw_result = aes_ecb_256_test_res;
			ret = hal_crypto_aes_ecb_init(key, keylen); CHK(ret);
			ret = hal_crypto_aes_ecb_encrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_AES_CBC:
			key = aes_cbc_256_test_key;
			keylen = sizeof(aes_cbc_256_test_key);
			iv = aes_cbc_256_test_iv;
			ivlen = sizeof(aes_cbc_256_test_iv);
			message = aes_cbc_256_test_msg;
			msglen = sizeof(aes_cbc_256_test_msg);
			sw_result = aes_cbc_256_test_res;
			ret = hal_crypto_aes_cbc_init(key, keylen); CHK(ret);
			ret = hal_crypto_aes_cbc_encrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_AES_CFB:
			key = aes_cfb_256_test_key;
			keylen = sizeof(aes_cfb_256_test_key);
			iv = aes_cfb_256_test_iv;
			ivlen = sizeof(aes_cfb_256_test_iv);
			message = aes_cfb_256_test_msg;
			msglen = sizeof(aes_cfb_256_test_msg);
			sw_result = aes_cfb_256_test_res;
			ret = hal_crypto_aes_cfb_init(key, keylen); CHK(ret);
			ret = hal_crypto_aes_cfb_encrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_AES_OFB:
			key = aes_ofb_256_test_key;
			keylen = sizeof(aes_ofb_256_test_key);
			iv = aes_ofb_256_test_iv;
			ivlen = sizeof(aes_ofb_256_test_iv);
			message = aes_ofb_256_test_msg;
			msglen = sizeof(aes_ofb_256_test_msg);
			sw_result = aes_ofb_256_test_res;
			ret = hal_crypto_aes_ofb_init(key, keylen); CHK(ret);
			ret = hal_crypto_aes_ofb_encrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_AES_CTR:
			key = aes_ctr_256_test_key;
			keylen = sizeof(aes_ctr_256_test_key);
			iv = aes_ctr_256_test_iv;
			ivlen = sizeof(aes_ctr_256_test_iv);
			message = aes_ctr_256_test_msg;
			msglen = sizeof(aes_ctr_256_test_msg);
			sw_result = aes_ctr_256_test_res;
			ret = hal_crypto_aes_ctr_init(key, keylen); CHK(ret);
			ret = hal_crypto_aes_ctr_encrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_AES_GCTR:
			key = aes_gctr_256_test_key;
            keylen = sizeof(aes_gctr_256_test_key);
            iv = aes_gctr_256_test_iv;
            ivlen = sizeof(aes_gctr_256_test_iv);
            message = aes_gctr_256_test_msg;
            msglen = sizeof(aes_gctr_256_test_msg);
            sw_result = aes_gctr_256_test_res;
            ret = hal_crypto_aes_gctr_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_gctr_encrypt(message, msglen, iv, hw_result);
            break;
		case TEST_CIPHER_AES_GMAC:
			// dec only
            break;
		case TEST_CIPHER_AES_GHASH:
            // dec only
			break;
		case TEST_CIPHER_AES_GCM:
			key = aes_gcm_256_test_key;
			keylen = sizeof(aes_gcm_256_test_key);
			iv = aes_gcm_256_test_iv;
			ivlen = sizeof(aes_gcm_256_test_iv);
			aad = aes_gcm_256_test_aad;
			aadlen = sizeof(aes_gcm_256_test_aad);
			message = aes_gcm_256_test_msg;
			msglen = sizeof(aes_gcm_256_test_msg);
			sw_result = aes_gcm_256_test_res;
			sw_tag = aes_gcm_256_test_tag;
			ret = hal_crypto_aes_gcm_init(key, keylen); CHK(ret);
			ret = hal_crypto_aes_gcm_encrypt(message, msglen, iv, aad, aadlen, hw_result, hw_tag);
			break;
		default:
			__err_printf("vec_algo(%08x): no this vec_algo\n", vec_algo);
			break;
	}

	if (ret == SUCCESS) {
		if (rtlc_memcmp(hw_result, sw_result, msglen) == 0) {
			if (vec_algo == TEST_CIPHER_AES_GCM) {
				if (rtlc_memcmp(hw_tag, sw_tag, sizeof(sw_tag)) == 0) {
					__info_printf("vector AES256 enc test success, vec_algo = %08x\n", vec_algo);
					// return SUCCESS;
				} else {
                    __info_printf("vector AES256 enc tag test fail, vec_algo = %08x\n", vec_algo);
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)aad, aadlen, "aad: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_tag, 32, "hw_tag: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_tag, 32, "sw_tag: ");
                    return FAIL;
                }
			} else {
				__info_printf("vector AES256 enc test success, vec_algo = %08x\n", vec_algo);
				// return SUCCESS;
			}
		} else {
			__err_printf("vector AES256 enc test fail, vec_algo = %08x\n", vec_algo);
			__dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
		}
	} else {
        if (vec_algo == TEST_CIPHER_AES_GMAC || vec_algo == TEST_CIPHER_AES_GHASH) {
            // do nothing, wait dec
        } else {
        	__err_printf("Crypto Engine vector AES256 enc compute fail, ret = %d\n", ret);
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
        }
	}

	// decryption: message -> hw_reault; sw_result -> message
	switch (vec_algo) {
		case TEST_CIPHER_AES_ECB:
			key = aes_ecb_256_test_key;
			keylen = sizeof(aes_ecb_256_test_key);
			iv = NULL;
			ivlen = 0;
			message = hw_result;
			msglen = sizeof(aes_ecb_256_test_msg);
			sw_result = aes_ecb_256_test_msg;
			ret = hal_crypto_aes_ecb_init(key, keylen); CHK(ret);
			ret = hal_crypto_aes_ecb_decrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_AES_CBC:
			key = aes_cbc_256_test_key;
			keylen = sizeof(aes_cbc_256_test_key);
			iv = aes_cbc_256_test_iv;
			ivlen = sizeof(aes_cbc_256_test_iv);
			message = hw_result;
			msglen = sizeof(aes_cbc_256_test_msg);
			sw_result = aes_cbc_256_test_msg;
			ret = hal_crypto_aes_cbc_init(key, keylen); CHK(ret);
			ret = hal_crypto_aes_cbc_decrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_AES_CFB:
			key = aes_cfb_256_test_key;
			keylen = sizeof(aes_cfb_256_test_key);
			iv = aes_cfb_256_test_iv;
			ivlen = sizeof(aes_cfb_256_test_iv);
			message = hw_result;
			msglen = sizeof(aes_cfb_256_test_msg);
			sw_result = aes_cfb_256_test_msg;
			ret = hal_crypto_aes_cfb_init(key, keylen); CHK(ret);
			ret = hal_crypto_aes_cfb_decrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_AES_OFB:
			key = aes_ofb_256_test_key;
			keylen = sizeof(aes_ofb_256_test_key);
			iv = aes_ofb_256_test_iv;
			ivlen = sizeof(aes_ofb_256_test_iv);
			message = sw_result;
			msglen = sizeof(aes_ofb_256_test_msg);
			sw_result = aes_ofb_256_test_msg;
			ret = hal_crypto_aes_ofb_init(key, keylen); CHK(ret);
			ret = hal_crypto_aes_ofb_decrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_AES_CTR:
			key = aes_ctr_256_test_key;
			keylen = sizeof(aes_ctr_256_test_key);
			iv = aes_ctr_256_test_iv;
			ivlen = sizeof(aes_ctr_256_test_iv);
			message = hw_result;
			msglen = sizeof(aes_ctr_256_test_msg);
			sw_result = aes_ctr_256_test_msg;
			ret = hal_crypto_aes_ctr_init(key, keylen); CHK(ret);
			ret = hal_crypto_aes_ctr_decrypt(message, msglen, iv, ivlen, hw_result);
			break;
		case TEST_CIPHER_AES_GCTR:
			key = aes_gctr_256_test_key;
            keylen = sizeof(aes_gctr_256_test_key);
            iv = aes_gctr_256_test_iv;
            ivlen = sizeof(aes_gctr_256_test_iv);
            message = hw_result;
            msglen = sizeof(aes_gctr_256_test_msg);
            sw_result = aes_gctr_256_test_msg;
            ret = hal_crypto_aes_gctr_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_gctr_decrypt(message, msglen, iv, hw_result);
            break;
		case TEST_CIPHER_AES_GMAC:
			// dec only
            key = aes_gmac_256_test_key;
            keylen = sizeof(aes_gmac_256_test_key);
            iv = aes_gmac_256_test_iv;
            ivlen = sizeof(aes_gmac_256_test_iv);
            aad = aes_gmac_256_test_aad;
            aadlen = sizeof(aes_gmac_256_test_aad);
            message = aes_gmac_256_test_res;
            msglen = sizeof(aes_gmac_256_test_res);
            sw_result = NULL;
            sw_tag = aes_gmac_256_test_tag;
            ret = hal_crypto_aes_gmac_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_gmac_process(message, msglen, iv, aad, aadlen, hw_tag);
            break;
		case TEST_CIPHER_AES_GHASH:
            // dec only
            key = aes_ghash_test_key;
            keylen = sizeof(aes_ghash_test_key);
            iv = NULL;
            ivlen = 0;
            message = aes_ghash_test_msg;
            msglen = sizeof(aes_ghash_test_msg);
            sw_result = NULL;
            sw_tag = aes_ghash_test_tag;
            ret = hal_crypto_aes_ghash_init(key, keylen); CHK(ret);
            ret = hal_crypto_aes_ghash_process(message, msglen, hw_tag);
			break;
		case TEST_CIPHER_AES_GCM:
			key = aes_gcm_256_test_key;
			keylen = sizeof(aes_gcm_256_test_key);
			iv = aes_gcm_256_test_iv;
			ivlen = sizeof(aes_gcm_256_test_iv);
			aad = aes_gcm_256_test_aad;
			aadlen = sizeof(aes_gcm_256_test_aad);
			message = hw_result;
			msglen = sizeof(aes_gcm_256_test_msg);
			sw_result = aes_gcm_256_test_msg;
			sw_tag = aes_gcm_256_test_tag;
			ret = hal_crypto_aes_gcm_init(key, keylen); CHK(ret);
			ret = hal_crypto_aes_gcm_decrypt(message, msglen, iv, aad, aadlen, hw_result, hw_tag);
			break;
		default:
			__err_printf("vec_algo(%08x): no this vec_algo\n", vec_algo);
			break;
	}

	if (ret == SUCCESS) {
        if (vec_algo == TEST_CIPHER_AES_GMAC || vec_algo == TEST_CIPHER_AES_GHASH) {
            // compare tag only
            if (rtlc_memcmp(hw_tag, sw_tag, sizeof(sw_tag)) == 0) {
                    __info_printf("vector AES256 dec test success, vec_algo = %08x\n", vec_algo);
                    return SUCCESS;
                } else {
                    __info_printf("vector AES256 dec tag test fail, vec_algo = %08x\n", vec_algo);
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)aad, aadlen, "aad: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_tag, 32, "hw_tag: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_tag, 32, "sw_tag: ");
                    return FAIL;
                }
        }
		if (rtlc_memcmp(hw_result, sw_result, msglen) == 0) {
			if (vec_algo == TEST_CIPHER_AES_GCM) {
				if (rtlc_memcmp(hw_tag, sw_tag, sizeof(sw_tag)) == 0) {
					__info_printf("vector AES dec test success, vec_algo = %08x\n", vec_algo);
					return SUCCESS;
				} else {
                    __info_printf("vector AES256 dec tag test fail, vec_algo = %08x\n", vec_algo);
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)aad, aadlen, "aad: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_tag, 32, "hw_tag: ");
                    __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_tag, 32, "sw_tag: ");
                    return FAIL;
                }
			} else {
				__info_printf("vector AES256 dec test success, vec_algo = %08x\n", vec_algo);
				return SUCCESS;
			}
		} else {
			__err_printf("vector AES256 dec test fail, vec_algo = %08x\n", vec_algo);
			__dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
            return FAIL;
		}
	} else {
		__err_printf("Crypto Engine vector AES256 dec compute fail, ret = %d\n", ret);
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)iv, ivlen, "iv: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_result, msglen, "hw_result: ");
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_result, msglen, "sw_result: ");
        return FAIL;
	}

	//return FAIL;
}

s32 vector_cipher_test(
	IN u8 vec_mode,
	IN u8 vec_algo
)
{
	s32 ret = FAIL;

	switch (vec_mode) {
		case TEST_CIPHER_DES:
			ret = vector_cipher_des_test(vec_algo);
			break;
		case TEST_CIPHER_3DES:
			ret = vector_cipher_3des_test(vec_algo);
			break;
		case TEST_CIPHER_AES:
			ret = vector_cipher_aes128_test(vec_algo); CHK(ret);
            __info_printf("vector_cipher_aes128_test done\n");
			ret = vector_cipher_aes192_test(vec_algo); CHK(ret);
            __info_printf("vector_cipher_aes192_test done\n");
			ret = vector_cipher_aes256_test(vec_algo); CHK(ret);
            __info_printf("vector_cipher_aes256_test done\n");
			break;
		default:
			__err_printf("vec_mode(%08x): no this vec_mode\n", vec_mode);
	}

	return ret;
}

u8 auth_key_in[max_auth_keylen] __attribute__((aligned(byte_align)));
u8 auth_msg_in[byte_align + max_auth_msglen + byte_align] __attribute__((aligned(byte_align)));
u8 auth_msg_in_p[max_seq_update_num][byte_align + max_auth_msglen + byte_align] __attribute__((aligned(byte_align)));
u8 hw_auth_msg_out[max_auth_digestlen] __attribute__((aligned(byte_align)));

u32 throughput_auth_test(
    IN u8 thr_mode,
    IN u8 thr_algo,
    IN u32 thr_loop
)
{
    s32 ret = FAIL;
    u32 i, j, cnt_i;
    u32 startCount = 0, endCount = 0, totalCount = 0;

    u8 *message = NULL;
    u32 msglen = 0;
    u8 *key = NULL;
    u32 keylen = 0;
    u8 *message_p[max_seq_update_num];
    u32 msglen_p[max_seq_update_num] = {0};
    u8 *hw_digest = NULL;

    // test input setting
    switch (thr_mode) {
        case TEST_AUTH_HASH_NORMAL:
            keylen = 0;
            msglen = (u32)thr_auth_msglen;
            message = auth_msg_in;
            //rtlc_memset(message, 0x5a, msglen);
            break;
        case TEST_AUTH_HASH_SEQUENTIAL:
            keylen = 0;
            for (j = 0; j < thr_auth_seqnum; j++) {
                msglen_p[j] = (u32)thr_auth_msglen_p;
                message_p[j] = auth_msg_in_p[j];
                //rtlc_memset(message_p[j], 0x5a, msglen_p[j]);
            }
            break;
        case TEST_AUTH_HMAC_NORMAL:
            keylen = (u32)thr_auth_keylen;
            key = auth_key_in;
            //rtlc_memset(key, 0x5a, keylen);
            msglen = (u32)thr_auth_msglen;
            message = auth_msg_in;
            //rtlc_memset(message, 0x5a, msglen);
            break;
        case TEST_AUTH_HMAC_SEQUENTIAL:
            keylen = (u32)thr_auth_keylen;
            key = auth_key_in;
            //rtlc_memset(key, 0x5a, keylen);
            for (j = 0; j < thr_auth_seqnum; j++) {
                msglen_p[j] = (u32)thr_auth_msglen_p;
                message_p[j] = auth_msg_in_p[j];
                //rtlc_memset(message_p[j], 0x5a, msglen_p[j]);
            }
            break;
        default:
            __err_printf("thr_mode(%08x): no this thr_mode\n", thr_mode);
            break;
    }

    hw_digest = hw_auth_msg_out;

    __info_printf("msglen = %d\n", msglen);
    __info_printf("keylen = %d\n", keylen);
    __info_printf("seqnum = %d\n", thr_auth_seqnum);
    __info_printf("msglen_p = %d\n", msglen_p[0]);

    // test start

    // reset counter
    cnt_i = read_sysreg(PMCR_EL0) | PMCR_EL0_P | PMCR_EL0_C;
    write_sysreg(cnt_i, PMCR_EL0);
    // enable
    cnt_i = PMCR_EL0_E | read_sysreg(PMCR_EL0);
    write_sysreg(cnt_i, PMCR_EL0);
    startCount = read_sysreg(PMCCNTR_EL0);
    // counter enabble
    write_sysreg(PMCNTENSET_EL0_C, PMCNTENSET_EL0);

    switch (thr_mode) {
        case TEST_AUTH_HASH_NORMAL:
            if (thr_algo == TEST_AUTH_MD5) {
                for (i = 0; i < thr_loop; i++) {
                    ret = hal_crypto_md5(message, msglen, hw_digest); CHK(ret);
                }
            } else if (thr_algo == TEST_AUTH_SHA1){
                for (i = 0; i < thr_loop; i++) {
                    ret = hal_crypto_sha1(message, msglen, hw_digest); CHK(ret);
                }
            } else if (thr_algo == TEST_AUTH_SHA2_224){
                for (i = 0; i < thr_loop; i++) {
                    ret = hal_crypto_sha2_224(message, msglen, hw_digest); CHK(ret);
                }
            } else if (thr_algo == TEST_AUTH_SHA2_256){
                for (i = 0; i < thr_loop; i++) {
                    ret = hal_crypto_sha2_256(message, msglen, hw_digest); CHK(ret);
                }
            } else {
                __err_printf("thr_algo(%08x): no this thr_algo\n", thr_algo);
            }
            break;
        case TEST_AUTH_HASH_SEQUENTIAL:
            if (thr_algo == TEST_AUTH_MD5) {
                for (i = 0; i < thr_loop; i++) {
                    ret = hal_crypto_md5_init(); CHK(ret);
                    for (j = 0; j < thr_auth_seqnum; j++) {
                        ret = hal_crypto_md5_update(message_p[j], msglen_p[j]); CHK(ret);
                    }
                    ret = hal_crypto_md5_final(hw_digest); CHK(ret);
                }
            } else if (thr_algo == TEST_AUTH_SHA1){
                for (i = 0; i < thr_loop; i++) {
                    ret = hal_crypto_sha1_init(); CHK(ret);
                    for (j = 0; j < thr_auth_seqnum; j++) {
                        ret = hal_crypto_sha1_update(message_p[j], msglen_p[j]); CHK(ret);
                    }
                    ret = hal_crypto_sha1_final(hw_digest); CHK(ret);
                }
            } else if (thr_algo == TEST_AUTH_SHA2_224){
                for (i = 0; i < thr_loop; i++) {
                    ret = hal_crypto_sha2_224_init(); CHK(ret);
                    for (j = 0; j < thr_auth_seqnum; j++) {
                        ret = hal_crypto_sha2_224_update(message_p[j], msglen_p[j]); CHK(ret);
                    }
                    ret = hal_crypto_sha2_224_final(hw_digest); CHK(ret);
                }
            } else if (thr_algo == TEST_AUTH_SHA2_256){
                for (i = 0; i < thr_loop; i++) {
                    ret = hal_crypto_sha2_256_init(); CHK(ret);
                    for (j = 0; j < thr_auth_seqnum; j++) {
                        ret = hal_crypto_sha2_256_update(message_p[j], msglen_p[j]); CHK(ret);
                    }
                    ret = hal_crypto_sha2_256_final(hw_digest); CHK(ret);
                }
            } else {
                __err_printf("thr_algo(%08x): no this thr_algo\n", thr_algo);
            }
            break;
        case TEST_AUTH_HMAC_NORMAL:
            if (thr_algo == TEST_AUTH_HMACMD5) {
                for (i = 0; i < thr_loop; i++) {
                    ret = hal_crypto_hmac_md5(message, msglen, key, keylen, hw_digest); CHK(ret);
                }
            } else if (thr_algo == TEST_AUTH_HMACSHA1){
                for (i = 0; i < thr_loop; i++) {
                    ret = hal_crypto_hmac_sha1(message, msglen, key, keylen, hw_digest); CHK(ret);
                }
            } else if (thr_algo == TEST_AUTH_HMACSHA2_224){
                for (i = 0; i < thr_loop; i++) {
                    ret = hal_crypto_hmac_sha2_224(message, msglen, key, keylen, hw_digest); CHK(ret);
                }
            } else if (thr_algo == TEST_AUTH_HMACSHA2_256){
                for (i = 0; i < thr_loop; i++) {
                    ret = hal_crypto_hmac_sha2_256(message, msglen, key, keylen, hw_digest); CHK(ret);
                }
            } else {
                __err_printf("thr_algo(%08x): no this thr_algo\n", thr_algo);
            }
            break;
        case TEST_AUTH_HMAC_SEQUENTIAL:
            if (thr_algo == TEST_AUTH_HMACMD5) {
                for (i = 0; i < thr_loop; i++) {
                    ret = hal_crypto_hmac_md5_init(key, keylen); CHK(ret);
                    for (j = 0; j < thr_auth_seqnum; j++) {
                        ret = hal_crypto_hmac_md5_update(message_p[j], msglen_p[j]); CHK(ret);
                    }
                    ret = hal_crypto_hmac_md5_final(hw_digest); CHK(ret);
                }
            } else if (thr_algo == TEST_AUTH_HMACSHA1){
                for (i = 0; i < thr_loop; i++) {
                    ret = hal_crypto_hmac_sha1_init(key, keylen); CHK(ret);
                    for (j = 0; j < thr_auth_seqnum; j++) {
                        ret = hal_crypto_hmac_sha1_update(message_p[j], msglen_p[j]); CHK(ret);
                    }
                    ret = hal_crypto_hmac_sha1_final(hw_digest); CHK(ret);
                }
            } else if (thr_algo == TEST_AUTH_HMACSHA2_224){
                for (i = 0; i < thr_loop; i++) {
                    ret = hal_crypto_hmac_sha2_224_init(key, keylen); CHK(ret);
                    for (j = 0; j < thr_auth_seqnum; j++) {
                        ret = hal_crypto_hmac_sha2_224_update(message_p[j], msglen_p[j]); CHK(ret);
                    }
                    ret = hal_crypto_hmac_sha2_224_final(hw_digest); CHK(ret);
                }
            } else if (thr_algo == TEST_AUTH_HMACSHA2_256){
                for (i = 0; i < thr_loop; i++) {
                    ret = hal_crypto_hmac_sha2_256_init(key, keylen); CHK(ret);
                    for (j = 0; j < thr_auth_seqnum; j++) {
                        ret = hal_crypto_hmac_sha2_256_update(message_p[j], msglen_p[j]); CHK(ret);
                    }
                    ret = hal_crypto_hmac_sha2_256_final(hw_digest); CHK(ret);
                }
            } else {
                __err_printf("thr_algo(%08x): no this thr_algo\n", thr_algo);
            }
            break;
        default:
            __err_printf("thr_mode(%08x): no this thr_mode\n", thr_mode);
            break;
    }

    endCount = read_sysreg(PMCCNTR_EL0);
    __info_printf("(startCount, endCount) = (%x, %x)\n", startCount, endCount);
    totalCount = endCount - startCount;
    __info_printf("totalCount = %x\n", totalCount);

    return totalCount;
}

s32 random_auth_test(
    IN u8 ran_mode,
    IN u8 ran_algo
)
{
    s32 ret = FAIL;
    u32 i, j;
    u8 *key = NULL;
    // u8 auth_key_in[128];
    u32 keylen = 0;
    u8 *message = NULL;
    u8 *message_p[max_seq_update_num];
    u32 src_pos = 0, dst_pos = 0, update_num = 0;
    u32 msglen = 0, msglen_p[max_seq_update_num];
    u8 *hw_digest = NULL;
    u8 *sw_digest = NULL;
    u32 digestlen = 0;

    mbedtls_md5_context md5_ctx;
    mbedtls_sha1_context sha1_ctx;
    mbedtls_sha256_context sha2_ctx;

    mbedtls_md_context_t md_hmac_md5_con;
    mbedtls_md_context_t md_hmac_sha1_con;
    mbedtls_md_context_t md_hmac_sha224_con;
    mbedtls_md_context_t md_hmac_sha256_con;

    // generate random key
    if (ran_algo == TEST_AUTH_HMACMD5 ||
        ran_algo == TEST_AUTH_HMACSHA1 ||
        ran_algo == TEST_AUTH_HMACSHA2_224 ||
        ran_algo == TEST_AUTH_HMACSHA2_256) {
        keylen = rtlc_random() % 64 + 1;
        for (i = 0; i < keylen; i++) {
            auth_key_in[i] = rtlc_random() & 0xFF;
        }
        //__dbg_printf("keylen = %d\n", keylen);
        //__dbg_mem_dump(dbg_mem_dump_en, (size_t)auth_key_in, keylen, "auth_key_in: ");
    }

    // generate random message for non-alignment case
    src_pos = rtlc_random() % byte_align;
    if (ran_mode == TEST_AUTH_HASH_NORMAL ||
        ran_mode == TEST_AUTH_HMAC_NORMAL) {
        msglen = rtlc_random() % (max_auth_msglen) + 1;
        //message = &auth_msg_in[0][src_pos];
        for (i = 0; i < msglen; i++) {
            auth_msg_in[src_pos + i] = rtlc_random() & 0xFF;
            //message[i] = rtlc_random() & 0xFF;
        }
        __dbg_printf("msglen = %d\n", msglen);
        //__dbg_mem_dump(dbg_mem_dump_en, (size_t)auth_msg_in, msglen, "auth_msg_in: ");
    } else if (ran_mode == TEST_AUTH_HASH_SEQUENTIAL ||
        ran_mode == TEST_AUTH_HMAC_SEQUENTIAL) {
        update_num = rtlc_random() % (max_seq_update_num) + 1;
        __dbg_printf("update_num = %d\n", update_num);
        for (j = 0; j < update_num; j++) {
            msglen_p[j] = rtlc_random() % (max_auth_msglen) + 1;
            for (i = 0; i < msglen_p[j]; i++) {
                auth_msg_in_p[j][src_pos + i] = rtlc_random() & 0xFF;
            }
            __dbg_printf("msglen_p[%d] = %d\n", j, msglen_p[j]);
            //__dbg_mem_dump(dbg_mem_dump_en, (size_t)auth_msg_in_p[0][j], msglen_p[j], "msglen_p[j]: ");
        }
    }

   u8 *message_src = NULL;

   if(addr_test_flag && src_addr){
        memset( (void *)src_addr, 0x0, byte_align + max_auth_msglen + byte_align);
        message_src =  (u8 *)(src_addr+ src_pos);
        memcpy(message_src, &auth_msg_in[src_pos], msglen );
    }

    // prepare hw digest for non-alignment case
    dst_pos = rtlc_random() % byte_align;
	if(addr_test_flag && dst_addr){
		hw_digest =  (u8 *)(dst_addr+ dst_pos);;
	}else{
		hw_digest = &hw_auth_msg_out[dst_pos];
	}
    __dbg_printf("src_pos = %d, dst_pos = %d\r\n", src_pos, dst_pos);

    // hw & sw digest computation
    switch (ran_mode) {
        case TEST_AUTH_HASH_NORMAL:
            if (ran_algo == TEST_AUTH_MD5) {
                key = NULL;
                keylen = 0;
				if(addr_test_flag && src_addr){
					message = message_src;
				}else{
					message = &auth_msg_in[src_pos];
				}
                //msglen = ;
                digestlen = 16;
                // hw computation
                ret = hal_crypto_md5(message, msglen, hw_digest);
                // sw computation
                void *ctx = &md5_ctx;
                mbedtls_md5_init(ctx);
                mbedtls_md5_starts(ctx);
                mbedtls_md5_update(ctx, message, msglen);
                mbedtls_md5_finish(ctx, sw_digest);
                mbedtls_md5_free(ctx);
                //rtl_crypto_auth_swvrf(CRYPTO_AUTH_MD5, key, keylen, message, msglen, sw_digest, digestlen);
            } else if (ran_algo == TEST_AUTH_SHA1) {
                key = NULL;
                keylen = 0;
				if(addr_test_flag && src_addr){
					message = message_src;
				}else{
					message = &auth_msg_in[src_pos];
				}
                //msglen = ;
                digestlen = 20;
                // hw computation
                ret = hal_crypto_sha1(message, msglen, hw_digest);
                // sw computation
                void *ctx = &sha1_ctx;
                mbedtls_sha1_init(ctx);
                mbedtls_sha1_starts(ctx);
                mbedtls_sha1_update(ctx, message, msglen);
                mbedtls_sha1_finish(ctx, sw_digest);
                mbedtls_sha1_free(ctx);
                //rtl_crypto_auth_swvrf(CRYPTO_AUTH_SHA1, key, keylen, message, msglen, sw_digest, digestlen);
            } else if (ran_algo == TEST_AUTH_SHA2_224) {
                key = NULL;
                keylen = 0;
                //message = &auth_msg_in[src_pos];
				if(addr_test_flag && src_addr){
					message = message_src;
				}else{
					message = &auth_msg_in[src_pos];
				}
                //msglen = ;
                digestlen = 28;
                // hw computation
                ret = hal_crypto_sha2_224(message, msglen, hw_digest);
                // sw computation
                void *ctx = &sha2_ctx;
                mbedtls_sha256_init(ctx);
                mbedtls_sha256_starts(ctx, (digestlen == 28) ? 1 : 0);
                mbedtls_sha256_update(ctx, message, msglen);
                mbedtls_sha256_finish(ctx, sw_digest);
                mbedtls_sha256_free(ctx);
                //rtl_crypto_auth_swvrf(CRYPTO_AUTH_SHA2_224, key, keylen, message, msglen, sw_digest, digestlen);
            } else if (ran_algo == TEST_AUTH_SHA2_256) {
                key = NULL;
                keylen = 0;
                //message = &auth_msg_in[src_pos];
				if(addr_test_flag && src_addr){
					message = message_src;
				}else{
					message = &auth_msg_in[src_pos];
				}
                //msglen = ;
                digestlen = 32;
                // hw computation
                ret = hal_crypto_sha2_256(message, msglen, hw_digest);
                // sw computation
                void *ctx = &sha2_ctx;
                mbedtls_sha256_init(ctx);
                mbedtls_sha256_starts(ctx, (digestlen == 28) ? 1 : 0);
                mbedtls_sha256_update(ctx, message, msglen);
                mbedtls_sha256_finish(ctx, sw_digest);
                mbedtls_sha256_free(ctx);
                //rtl_crypto_auth_swvrf(CRYPTO_AUTH_SHA2_256, key, keylen, message, msglen, sw_digest, digestlen);
            } else {
                __err_printf("ran_algo(%08x): no this ran_algo\n", ran_algo);
            }
            break;
        case TEST_AUTH_HASH_SEQUENTIAL:
            if (ran_algo == TEST_AUTH_MD5) {
                key = NULL;
                keylen = 0;
                for (j = 0; j < update_num; j++) {
                    message_p[j] = &(auth_msg_in_p[j][src_pos]);
                    //msglen_p[j] = ;
                }
                digestlen = 16;
                // hw computation
                ret = hal_crypto_md5_init(); CHK(ret);
                for (j = 0; j < update_num; j++) {
                    ret = hal_crypto_md5_update(message_p[j], msglen_p[j]); CHK(ret);
                }
                ret = hal_crypto_md5_final(hw_digest);
                // sw computation
                void *ctx = &md5_ctx;
                mbedtls_md5_init(ctx);
                mbedtls_md5_starts(ctx);
                for (j = 0; j < update_num; j++) {
                    mbedtls_md5_update(ctx, message_p[j], msglen_p[j]);
                }
                mbedtls_md5_finish(ctx, sw_digest);
                mbedtls_md5_free(ctx);
            } else if (ran_algo == TEST_AUTH_SHA1) {
                key = NULL;
                keylen = 0;
                for (j = 0; j < update_num; j++) {
                    message_p[j] = &(auth_msg_in_p[j][src_pos]);
                    //msglen_p[j] = ;
                }
                digestlen = 20;
                // hw computation
                ret = hal_crypto_sha1_init(); CHK(ret);
                for (j = 0; j < update_num; j++) {
                    ret = hal_crypto_sha1_update(message_p[j], msglen_p[j]); CHK(ret);
                }
                ret = hal_crypto_sha1_final(hw_digest);
                // sw computation
                void *ctx = &sha1_ctx;
                mbedtls_sha1_init(ctx);
                mbedtls_sha1_starts(ctx);
                for (j = 0; j < update_num; j++) {
                    mbedtls_sha1_update(ctx, message_p[j], msglen_p[j]);
                }
                mbedtls_sha1_finish(ctx, sw_digest);
                mbedtls_sha1_free(ctx);
            } else if (ran_algo == TEST_AUTH_SHA2_224) {
                key = NULL;
                keylen = 0;
                for (j = 0; j < update_num; j++) {
                    message_p[j] = &(auth_msg_in_p[j][src_pos]);
                    //msglen_p[j] = ;
                }
                digestlen = 28;
                // hw computation
                ret = hal_crypto_sha2_224_init(); CHK(ret);
                for (j = 0; j < update_num; j++) {
                    ret = hal_crypto_sha2_224_update(message_p[j], msglen_p[j]); CHK(ret);
                }
                ret = hal_crypto_sha2_224_final(hw_digest);
                // sw computation
                void *ctx = &sha2_ctx;
                mbedtls_sha256_init(ctx);
                mbedtls_sha256_starts(ctx, (digestlen == 28) ? 1 : 0);
                for (j = 0; j < update_num; j++) {
                    mbedtls_sha256_update(ctx, message_p[j], msglen_p[j]);
                }
                mbedtls_sha256_finish(ctx, sw_digest);
                mbedtls_sha256_free(ctx);
            } else if (ran_algo == TEST_AUTH_SHA2_256) {
                key = NULL;
                keylen = 0;
                for (j = 0; j < update_num; j++) {
                    message_p[j] = &(auth_msg_in_p[j][src_pos]);
                    //msglen_p[j] = ;
                }
                digestlen = 32;
                // hw computation
                ret = hal_crypto_sha2_256_init(); CHK(ret);
                for (j = 0; j < update_num; j++) {
                    ret = hal_crypto_sha2_256_update(message_p[j], msglen_p[j]); CHK(ret);
                }
                ret = hal_crypto_sha2_256_final(hw_digest);
                // sw computation
                void *ctx = &sha2_ctx;
                mbedtls_sha256_init(ctx);
                mbedtls_sha256_starts(ctx, (digestlen == 28) ? 1 : 0);
                for (j = 0; j < update_num; j++) {
                    mbedtls_sha256_update(ctx, message_p[j], msglen_p[j]);
                }
                mbedtls_sha256_finish(ctx, sw_digest);
                mbedtls_sha256_free(ctx);
            } else {
                __err_printf("ran_algo(%08x): no this ran_algo\n", ran_algo);
            }
            break;
        case TEST_AUTH_HMAC_NORMAL:
            if (ran_algo == TEST_AUTH_HMACMD5) {
                key = auth_key_in;
                //keylen = ;
                //message = &auth_msg_in[src_pos];
				if(addr_test_flag && src_addr){
					message = message_src;
				}else{
					message = &auth_msg_in[src_pos];
				}
                //msglen = ;
                digestlen = 16;
                // hw computation
                ret = hal_crypto_hmac_md5(message, msglen, key, keylen, hw_digest);
                // sw computation
                void *ctx = &md_hmac_md5_con;
                mbedtls_hmac_init(ctx, CRYPTO_AUTH_HMAC_MD5);
                mbedtls_md_hmac_starts(ctx, key, keylen);
                mbedtls_md_hmac_update(ctx, message, msglen);
                mbedtls_md_hmac_finish(ctx, sw_digest);
                //rtl_crypto_swvrf_init();
                //rtl_crypto_auth_swvrf(CRYPTO_AUTH_HMAC_MD5, key, keylen, message, msglen, sw_digest, digestlen);
            } else if (ran_algo == TEST_AUTH_HMACSHA1) {
                key = auth_key_in;
                //keylen = ;
                //message = &auth_msg_in[src_pos];
				if(addr_test_flag && src_addr){
					message = message_src;
				}else{
					message = &auth_msg_in[src_pos];
				}
                //msglen = ;
                digestlen = 20;
                // hw computation
                ret = hal_crypto_hmac_sha1(message, msglen, key, keylen, hw_digest);
                // sw computation
                void *ctx = &md_hmac_sha1_con;
                mbedtls_hmac_init(ctx, CRYPTO_AUTH_HMAC_SHA1);
                mbedtls_md_hmac_starts(ctx, key, keylen);
                mbedtls_md_hmac_update(ctx, message, msglen);
                mbedtls_md_hmac_finish(ctx, sw_digest);
                //rtl_crypto_swvrf_init();
                //rtl_crypto_auth_swvrf(CRYPTO_AUTH_HMAC_SHA1, key, keylen, message, msglen, sw_digest, digestlen);
            } else if (ran_algo == TEST_AUTH_HMACSHA2_224) {
                key = auth_key_in;
                //keylen = ;
                //message = &auth_msg_in[src_pos];
				if(addr_test_flag && src_addr){
					message = message_src;
				}else{
					message = &auth_msg_in[src_pos];
				}
                //msglen = ;
                digestlen = 28;
                // hw computation
                ret = hal_crypto_hmac_sha2_224(message, msglen, key, keylen, hw_digest);
                // sw computation
                void *ctx = &md_hmac_sha224_con;
                mbedtls_hmac_init(ctx, CRYPTO_AUTH_HMAC_SHA2_224);
                mbedtls_md_hmac_starts(ctx, key, keylen);
                mbedtls_md_hmac_update(ctx, message, msglen);
                mbedtls_md_hmac_finish(ctx, sw_digest);
                //rtl_crypto_swvrf_init();
                //rtl_crypto_auth_swvrf(CRYPTO_AUTH_HMAC_SHA2_224, key, keylen, message, msglen, sw_digest, digestlen);
            } else if (ran_algo == TEST_AUTH_HMACSHA2_256) {
                key = auth_key_in;
                //keylen = ;
                //message = &auth_msg_in[src_pos];
				if(addr_test_flag && src_addr){
					message = message_src;
				}else{
					message = &auth_msg_in[src_pos];
				}
                //msglen = ;
                digestlen = 32;
                // hw computation
                ret = hal_crypto_hmac_sha2_256(message, msglen, key, keylen, hw_digest);
                // sw computation
                void *ctx = &md_hmac_sha256_con;
                mbedtls_hmac_init(ctx, CRYPTO_AUTH_HMAC_SHA2_256);
                mbedtls_md_hmac_starts(ctx, key, keylen);
                mbedtls_md_hmac_update(ctx, message, msglen);
                mbedtls_md_hmac_finish(ctx, sw_digest);
                //rtl_crypto_swvrf_init();
                //rtl_crypto_auth_swvrf(CRYPTO_AUTH_HMAC_SHA2_256, key, keylen, message, msglen, sw_digest, digestlen);
            } else {
                __err_printf("ran_algo(%08x): no this ran_algo\n", ran_algo);
            }
            break;
        case TEST_AUTH_HMAC_SEQUENTIAL:
            if (ran_algo == TEST_AUTH_HMACMD5) {
                key = auth_key_in;
                //keylen = ;
                for (j = 0; j < update_num; j++) {
                    message_p[j] = &(auth_msg_in_p[j][src_pos]);
                    //msglen_p[j] = ;
                }
                digestlen = 16;
                // hw computation
                ret = hal_crypto_hmac_md5_init(key, keylen); CHK(ret);
                for (j = 0; j < update_num; j++) {
                    ret = hal_crypto_hmac_md5_update(message_p[j], msglen_p[j]); CHK(ret);
                }
                ret = hal_crypto_hmac_md5_final(hw_digest);
                // sw computation
                void *ctx = &md_hmac_md5_con;
                mbedtls_hmac_init(ctx, CRYPTO_AUTH_HMAC_MD5);
                mbedtls_md_hmac_starts(ctx, key, keylen);
                for (j = 0; j < update_num; j++) {
                    mbedtls_md_hmac_update(ctx, message_p[j], msglen_p[j]);
                }
                mbedtls_md_hmac_finish(ctx, sw_digest);
            } else if (ran_algo == TEST_AUTH_HMACSHA1) {
                key = auth_key_in;
                //keylen = ;
                for (j = 0; j < update_num; j++) {
                    message_p[j] = &(auth_msg_in_p[j][src_pos]);
                    //msglen_p[j] = ;
                }
                digestlen = 20;
                // hw computation
                ret = hal_crypto_hmac_sha1_init(key, keylen); CHK(ret);
                for (j = 0; j < update_num; j++) {
                    ret = hal_crypto_hmac_sha1_update(message_p[j], msglen_p[j]); CHK(ret);
                }
                ret = hal_crypto_hmac_sha1_final(hw_digest);
                // sw computation
                void *ctx = &md_hmac_sha1_con;
                mbedtls_hmac_init(ctx, CRYPTO_AUTH_HMAC_SHA1);
                mbedtls_md_hmac_starts(ctx, key, keylen);
                for (j = 0; j < update_num; j++) {
                    mbedtls_md_hmac_update(ctx, message_p[j], msglen_p[j]);
                }
                mbedtls_md_hmac_finish(ctx, sw_digest);
            } else if (ran_algo == TEST_AUTH_HMACSHA2_224) {
                key = auth_key_in;
                //keylen = ;
                for (j = 0; j < update_num; j++) {
                    message_p[j] = &(auth_msg_in_p[j][src_pos]);
                    //msglen_p[j] = ;
                }
                digestlen = 28;
                // hw computation
                ret = hal_crypto_hmac_sha2_224_init(key, keylen); CHK(ret);
                for (j = 0; j < update_num; j++) {
                    ret = hal_crypto_hmac_sha2_224_update(message_p[j], msglen_p[j]); CHK(ret);
                }
                ret = hal_crypto_hmac_sha2_224_final(hw_digest);
                // sw computation
                void *ctx = &md_hmac_sha224_con;
                mbedtls_hmac_init(ctx, CRYPTO_AUTH_HMAC_SHA2_224);
                mbedtls_md_hmac_starts(ctx, key, keylen);
                for (j = 0; j < update_num; j++) {
                    mbedtls_md_hmac_update(ctx, message_p[j], msglen_p[j]);
                }
                mbedtls_md_hmac_finish(ctx, sw_digest);
            } else if (ran_algo == TEST_AUTH_HMACSHA2_256) {
                key = auth_key_in;
                //keylen = ;
                for (j = 0; j < update_num; j++) {
                    message_p[j] = &(auth_msg_in_p[j][src_pos]);
                    //msglen_p[j] = ;
                }
                digestlen = 32;
                // hw computation
                ret = hal_crypto_hmac_sha2_256_init(key, keylen); CHK(ret);
                for (j = 0; j < update_num; j++) {
                    ret = hal_crypto_hmac_sha2_256_update(message_p[j], msglen_p[j]); CHK(ret);
                }
                ret = hal_crypto_hmac_sha2_256_final(hw_digest);
                // sw computation
                void *ctx = &md_hmac_sha256_con;
                mbedtls_hmac_init(ctx, CRYPTO_AUTH_HMAC_SHA2_256);
                mbedtls_md_hmac_starts(ctx, key, keylen);
                for (j = 0; j < update_num; j++) {
                    mbedtls_md_hmac_update(ctx, message_p[j], msglen_p[j]);
                }
                mbedtls_md_hmac_finish(ctx, sw_digest);;
            } else {
                __err_printf("ran_algo(%08x): no this ran_algo\n", ran_algo);
            }
            break;
        default:
            __err_printf("ran_mode(%08x): no this ran_mode\n", ran_mode);
            break;
    }

    if (ret == SUCCESS) {
        if (rtlc_memcmp(hw_digest, sw_digest, digestlen) == 0) {
            __dbg_printf("random auth test success, ran_mode = %08x, ran_algo = %08x\n", ran_mode, ran_algo);
            return SUCCESS;
        } else {
            __err_printf("random auth test fail, ran_mode = %08x, ran_algo = %08x\n", ran_mode, ran_algo);
        }
    } else {
        __err_printf("Crypto Engine random auth compute fail, ret = %d\n", ret);
    }

    // print err info when test fail
    __err_printf("keylen = %d\n", keylen);
    __dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
    if (ran_mode == TEST_AUTH_HASH_NORMAL || ran_mode == TEST_AUTH_HMAC_NORMAL) {
        __err_printf("msglen = %d\n", msglen);
        __dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
    } else if (ran_mode == TEST_AUTH_HASH_SEQUENTIAL || ran_mode == TEST_AUTH_HMAC_SEQUENTIAL) {
        __err_printf("update_num = %d\n", update_num);
        for (j = 0; j < update_num; j++) {
            __err_printf("msglen_p[%d] = %d\n", j, msglen_p[j]);
            __dbg_mem_dump(dbg_mem_dump_en, (size_t)message_p[j], msglen_p[j], "message_p[]: ");
        }
    }
    __dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_digest, digestlen, "hw_digest: ");
    __dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_digest, digestlen, "sw_digest: ");

    return FAIL;
}

s32 vector_auth_test(
	IN u8 vec_mode,
	IN u8 vec_algo
)
{
	s32 ret = FAIL;
	u8 *key = NULL;
	u32 keylen = 0;
	u8 *message = NULL, *message_1 = NULL, *message_2 = NULL;
	u32 msglen = 0, msglen_1 = 0, msglen_2 = 0;
	u8 hw_digest[32], *sw_digest = NULL;
	u32 digestlen = 0;

	switch (vec_mode) {
		case TEST_AUTH_HASH_NULL:
			if (vec_algo == TEST_AUTH_MD5) {
				key = NULL;
				keylen = 0;
				message = md5_test_msg0;
				msglen = sizeof(md5_test_msg0);
				digestlen = 16;
				sw_digest = md5_test_res0;
				ret = hal_crypto_md5(message, msglen, hw_digest);
			} else if (vec_algo == TEST_AUTH_SHA1){
				key = NULL;
				keylen = 0;
				message = sha1_test_msg0;
				msglen = sizeof(sha1_test_msg0);
				digestlen = 20;
				sw_digest = sha1_test_res0;
				ret = hal_crypto_sha1(message, msglen, hw_digest);
			} else if (vec_algo == TEST_AUTH_SHA2_224){
				key = NULL;
				keylen = 0;
				message = sha2_224_test_msg0;
				msglen = sizeof(sha2_224_test_msg0);
				digestlen = 28;
				sw_digest = sha2_224_test_res0;
				ret = hal_crypto_sha2_224(message, msglen, hw_digest);
			} else if (vec_algo == TEST_AUTH_SHA2_256){
				key = NULL;
				keylen = 0;
				message = sha2_256_test_msg0;
				msglen = sizeof(sha2_256_test_msg0);
				digestlen = 32;
				sw_digest = sha2_256_test_res0;
				ret = hal_crypto_sha2_256(message, msglen, hw_digest);
			} else {
				__err_printf("vec_algo(%08x): no this vec_algo\n", vec_algo);
			}
			break;
		case TEST_AUTH_HASH_NORMAL:
			if (vec_algo == TEST_AUTH_MD5) {
				key = NULL;
				keylen = 0;
				message = md5_test_msg1;
				msglen = sizeof(md5_test_msg1);
				digestlen = 16;
				sw_digest = md5_test_res1;
				ret = hal_crypto_md5(message, msglen, hw_digest);
			} else if (vec_algo == TEST_AUTH_SHA1){
				key = NULL;
				keylen = 0;
				message = sha1_test_msg1;
				msglen = sizeof(sha1_test_msg1);
				digestlen = 20;
				sw_digest = sha1_test_res1;
				ret = hal_crypto_sha1(message, msglen, hw_digest);
			} else if (vec_algo == TEST_AUTH_SHA2_224){
				key = NULL;
				keylen = 0;
				message = sha2_224_test_msg1;
				msglen = sizeof(sha2_224_test_msg1);
				digestlen = 28;
				sw_digest = sha2_224_test_res1;
				ret = hal_crypto_sha2_224(message, msglen, hw_digest);
			} else if (vec_algo == TEST_AUTH_SHA2_256){
				key = NULL;
				keylen = 0;
				message = sha2_256_test_msg1;
				msglen = sizeof(sha2_256_test_msg1);
				digestlen = 32;
				sw_digest = sha2_256_test_res1;
				ret = hal_crypto_sha2_256(message, msglen, hw_digest);
			} else {
				__err_printf("vec_algo(%08x): no this vec_algo\n", vec_algo);
			}
			break;
		case TEST_AUTH_HASH_SEQUENTIAL:
			if (vec_algo == TEST_AUTH_MD5) {
				key = NULL;
				keylen = 0;
				message_1 = md5_seq_test_msg_p1;
				message_2 = md5_seq_test_msg_p2;
				msglen_1 = sizeof(md5_seq_test_msg_p1);
				msglen_2 = sizeof(md5_seq_test_msg_p2);
				digestlen = 16;
				sw_digest = md5_seq_test_res;
				ret = hal_crypto_md5_init(); CHK(ret);
				ret = hal_crypto_md5_update(message_1, msglen_1); CHK(ret);
				ret = hal_crypto_md5_update(message_2, msglen_2); CHK(ret);
				ret = hal_crypto_md5_final(hw_digest);
			} else if (vec_algo == TEST_AUTH_SHA1){
				key = NULL;
				keylen = 0;
				message_1 = sha1_seq_test_msg_p1;
				message_2 = sha1_seq_test_msg_p2;
				msglen_1 = sizeof(sha1_seq_test_msg_p1);
				msglen_2 = sizeof(sha1_seq_test_msg_p2);
				digestlen = 20;
				sw_digest = sha1_seq_test_res;
				ret = hal_crypto_sha1_init(); CHK(ret);
				ret = hal_crypto_sha1_update(message_1, msglen_1); CHK(ret);
				ret = hal_crypto_sha1_update(message_2, msglen_2); CHK(ret);
				ret = hal_crypto_sha1_final(hw_digest);
			} else if (vec_algo == TEST_AUTH_SHA2_224){
				key = NULL;
				keylen = 0;
				message_1 = sha2_224_seq_test_msg_p1;
				message_2 = sha2_224_seq_test_msg_p2;
				msglen_1 = sizeof(sha2_224_seq_test_msg_p1);
				msglen_2 = sizeof(sha2_224_seq_test_msg_p2);
				digestlen = 28;
				sw_digest = sha2_224_seq_test_res;
				ret = hal_crypto_sha2_224_init(); CHK(ret);
				ret = hal_crypto_sha2_224_update(message_1, msglen_1); CHK(ret);
				ret = hal_crypto_sha2_224_update(message_2, msglen_2); CHK(ret);
				ret = hal_crypto_sha2_224_final(hw_digest);
			} else if (vec_algo == TEST_AUTH_SHA2_256){
				key = NULL;
				keylen = 0;
				message_1 = sha2_256_seq_test_msg_p1;
				message_2 = sha2_256_seq_test_msg_p2;
				msglen_1 = sizeof(sha2_256_seq_test_msg_p1);
				msglen_2 = sizeof(sha2_256_seq_test_msg_p2);
				digestlen = 32;
				sw_digest = sha2_256_seq_test_res;
				ret = hal_crypto_sha2_256_init(); CHK(ret);
				ret = hal_crypto_sha2_256_update(message_1, msglen_1); CHK(ret);
				ret = hal_crypto_sha2_256_update(message_2, msglen_2); CHK(ret);
				ret = hal_crypto_sha2_256_final(hw_digest);
			} else {
				__err_printf("vec_algo(%08x): no this vec_algo\n", vec_algo);
			}
			break;
		case TEST_AUTH_HMAC_NORMAL:
			if (vec_algo == TEST_AUTH_HMACMD5) {
				key = hmac_test_key;
				keylen = sizeof(hmac_test_key);
				message = hmac_md5_test_msg1;
				msglen = sizeof(hmac_md5_test_msg1);
				digestlen = 16;
				sw_digest = hmac_md5_test_res1;
				ret = hal_crypto_hmac_md5(message, msglen, key, keylen, hw_digest);
			} else if (vec_algo == TEST_AUTH_HMACSHA1){
				key = hmac_test_key;
				keylen = sizeof(hmac_test_key);
				message = hmac_sha1_test_msg1;
				msglen = sizeof(hmac_sha1_test_msg1);
				digestlen = 20;
				sw_digest = hmac_sha1_test_res1;
				ret = hal_crypto_hmac_sha1(message, msglen, key, keylen, hw_digest);
			} else if (vec_algo == TEST_AUTH_HMACSHA2_224){
				key = hmac_test_key;
				keylen = sizeof(hmac_test_key);
				message = hmac_sha2_224_test_msg1;
				msglen = sizeof(hmac_sha2_224_test_msg1);
				digestlen = 28;
				sw_digest = hmac_sha2_224_test_res1;
				ret = hal_crypto_hmac_sha2_224(message, msglen, key, keylen, hw_digest);
			} else if (vec_algo == TEST_AUTH_HMACSHA2_256){
				key = hmac_test_key;
				keylen = sizeof(hmac_test_key);
				message = hmac_sha2_256_test_msg1;
				msglen = sizeof(hmac_sha2_256_test_msg1);
				digestlen = 32;
				sw_digest = hmac_sha2_256_test_res1;
				ret = hal_crypto_hmac_sha2_256(message, msglen, key, keylen, hw_digest);
			} else {
				__err_printf("vec_algo(%08x): no this vec_algo\n", vec_algo);
			}
			break;
		case TEST_AUTH_HMAC_SEQUENTIAL:
			if (vec_algo == TEST_AUTH_HMACMD5) {
				key = hmac_test_key;
				keylen = sizeof(hmac_test_key);
				message_1 = hmac_md5_seq_test_msg_p1;
				message_2 = hmac_md5_seq_test_msg_p2;
				msglen_1 = sizeof(hmac_md5_seq_test_msg_p1);
				msglen_2 = sizeof(hmac_md5_seq_test_msg_p2);
				digestlen = 16;
				sw_digest = hmac_md5_seq_test_res;
				ret = hal_crypto_hmac_md5_init(key, keylen); CHK(ret);
				ret = hal_crypto_hmac_md5_update(message_1, msglen_1); CHK(ret);
				ret = hal_crypto_hmac_md5_update(message_2, msglen_2); CHK(ret);
				ret = hal_crypto_hmac_md5_final(hw_digest);
			} else if (vec_algo == TEST_AUTH_HMACSHA1){
				key = hmac_test_key;
				keylen = sizeof(hmac_test_key);
				message_1 = hmac_sha1_seq_test_msg_p1;
				message_2 = hmac_sha1_seq_test_msg_p2;
				msglen_1 = sizeof(hmac_sha1_seq_test_msg_p1);
				msglen_2 = sizeof(hmac_sha1_seq_test_msg_p2);
				digestlen = 20;
				sw_digest = hmac_sha1_seq_test_res;
				ret = hal_crypto_hmac_sha1_init(key, keylen); CHK(ret);
				ret = hal_crypto_hmac_sha1_update(message_1, msglen_1); CHK(ret);
				ret = hal_crypto_hmac_sha1_update(message_2, msglen_2); CHK(ret);
				ret = hal_crypto_hmac_sha1_final(hw_digest);
			} else if (vec_algo == TEST_AUTH_HMACSHA2_224){
				key = hmac_test_key;
				keylen = sizeof(hmac_test_key);
				message_1 = hmac_sha2_224_seq_test_msg_p1;
				message_2 = hmac_sha2_224_seq_test_msg_p2;
				msglen_1 = sizeof(hmac_sha2_224_seq_test_msg_p1);
				msglen_2 = sizeof(hmac_sha2_224_seq_test_msg_p2);
				digestlen = 28;
				sw_digest = hmac_sha2_224_seq_test_res;
				ret = hal_crypto_hmac_sha2_224_init(key, keylen); CHK(ret);
				ret = hal_crypto_hmac_sha2_224_update(message_1, msglen_1); CHK(ret);
				ret = hal_crypto_hmac_sha2_224_update(message_2, msglen_2); CHK(ret);
				ret = hal_crypto_hmac_sha2_224_final(hw_digest);
			} else if (vec_algo == TEST_AUTH_HMACSHA2_256){
				key = hmac_test_key;
				keylen = sizeof(hmac_test_key);
				message_1 = hmac_sha2_256_seq_test_msg_p1;
				message_2 = hmac_sha2_256_seq_test_msg_p2;
				msglen_1 = sizeof(hmac_sha2_256_seq_test_msg_p1);
				msglen_2 = sizeof(hmac_sha2_256_seq_test_msg_p2);
				digestlen = 32;
				sw_digest = hmac_sha2_256_seq_test_res;
				ret = hal_crypto_hmac_sha2_256_init(key, keylen); CHK(ret);
				ret = hal_crypto_hmac_sha2_256_update(message_1, msglen_1); CHK(ret);
				ret = hal_crypto_hmac_sha2_256_update(message_2, msglen_2); CHK(ret);
				ret = hal_crypto_hmac_sha2_256_final(hw_digest);
			} else {
				__err_printf("vec_algo(%08x): no this vec_algo\n", vec_algo);
			}
			break;
		default:
			__err_printf("vec_mode(%08x): no this vec_mode\n", vec_mode);
			break;
	}

	if (ret == SUCCESS) {
		if (rtlc_memcmp(hw_digest, sw_digest, digestlen) == 0) {
			__info_printf("vector auth test success, vec_mode = %02x, vec_algo = %02x\n", vec_mode, vec_algo);
			return SUCCESS;
		} else {
			__err_printf("vector auth test fail, vec_mode = %08x, vec_algo = %08x\n", vec_mode, vec_algo);
		}
	} else {
		__err_printf("Crypto Engine vector auth compute fail, ret = %d\n", ret);
	}

	// print dbg info when test fail
	__dbg_mem_dump(dbg_mem_dump_en, (size_t)key, keylen, "key: ");
	__dbg_mem_dump(dbg_mem_dump_en, (size_t)message, msglen, "message: ");
	__dbg_mem_dump(dbg_mem_dump_en, (size_t)hw_digest, digestlen, "hw_digest: ");
	__dbg_mem_dump(dbg_mem_dump_en, (size_t)sw_digest, digestlen, "sw_digest: ");

	return FAIL;
}

void cycle_cnt_test(
    IN u32 thr_loop
)
{
    u32 cnt_i;

    // reset counter
    cnt_i = read_sysreg(PMCR_EL0) | PMCR_EL0_P | PMCR_EL0_C;
    write_sysreg(cnt_i, PMCR_EL0);
    // enable
    cnt_i = PMCR_EL0_E | read_sysreg(PMCR_EL0);
    write_sysreg(cnt_i, PMCR_EL0);
    // counter enabble
    write_sysreg(PMCNTENSET_EL0_C, PMCNTENSET_EL0);

    // suggestion: FPGA uses 500000000 cycles, ASIC uses 2000000000 cycles
    cnt_i = read_sysreg(PMCCNTR_EL0);
    __info_printf("pmu start @ %d cycles\n", cnt_i);
    while (cnt_i < thr_loop) {
        cnt_i = read_sysreg(PMCCNTR_EL0);
    }
    __info_printf("pmu delay %d cycles\n", thr_loop);
}

void rtl_crypto_usage(
	IN char *cmd
)
{
	if (cmd == NULL) {
		__info_printf("usage: crypto <init|info|test|debug|vector|random|interrupt|throughput>\n");
	} else if (strcmp(cmd, "init") == 0) {
		//
	} else if (strcmp(cmd, "info") == 0) {
		//
	} else if (strcmp(cmd, "test") == 0) {
		//
	} else if (strcmp(cmd, "debug") == 0) {
		//
	} else if (strcmp(cmd, "vector") == 0) {
		//
	} else if (strcmp(cmd, "random") == 0) {
		//
	} else if (strcmp(cmd, "interrupt") == 0) {
		//
	} else if (strcmp(cmd, "throughput") == 0) {
		//
	} else {
		__info_printf("no this cmd: %s\n", cmd);
		__info_printf("usage: crypto <init|info|test|debug|vector|random|interrupt|throughput>\n");
	}
}

s32 rtl_crypto_test(
	IN u16 argc,
	IN const char *argv[]
)
{
	s32 ret = _FALSE;
	u32 i;

	if (argc < 4) {
        rtl_crypto_usage(NULL);
        return _FALSE;
    }

    if (strcmp(argv[0], "init") == 0) {
		__dbg_printf("doing init\n");
		ret = hal_crypto_engine_init();
		if (ret == SUCCESS ) {
			__info_printf("Crypto Engine init success\n");
			return _TRUE;
		} else {
			__info_printf("Crypto Engine init fail, ret = %d\n", ret);
			return _FALSE;
		}
	} else if (strcmp(argv[0], "info") == 0) {
		__info_printf("Not suppport!\n");
	} else if (strcmp(argv[0], "test") == 0) {
		__info_printf("Not suppport!\n");
	} else if (strcmp(argv[0], "debug") == 0) {
		__info_printf("Not suppport!\n");
	} else if (strcmp(argv[0], "vector") == 0) {
		u32 vec_mode, vec_algo, vec_loop;

//		vec_mode = Strtoul((u8*)(argv[1]), (u8 **)NULL, 10);
//		vec_algo = Strtoul((u8*)(argv[2]), (u8 **)NULL, 10);
//		vec_loop = Strtoul((u8*)(argv[3]), (u8 **)NULL, 10);

		vec_mode= simple_strtoul(argv[1], NULL, 10);
		vec_algo= simple_strtoul(argv[2], NULL, 10);
		vec_loop= simple_strtoul(argv[3], NULL, 10);

		for (i = 0; i < vec_loop; i++) {
			if (vec_mode == TEST_AUTH_HASH_NULL ||
				vec_mode == TEST_AUTH_HASH_NORMAL ||
				vec_mode == TEST_AUTH_HASH_SEQUENTIAL ||
				vec_mode == TEST_AUTH_HMAC_NORMAL ||
				vec_mode == TEST_AUTH_HMAC_SEQUENTIAL) {
				ret = vector_auth_test(vec_mode, vec_algo);
			} else if (vec_mode == TEST_CIPHER_DES ||
				vec_mode == TEST_CIPHER_3DES ||
				vec_mode == TEST_CIPHER_AES) {
				ret = vector_cipher_test(vec_mode, vec_algo);
			} else if (vec_mode == TEST_MIX) {
				// ret = vector_mix_test(vec_mode, vec_algo);
			} else {
				__err_printf("no this vec_mode: %08x\n", vec_mode);
			}
			__info_printf("vec_loop(%d): vector test done, ret = %d\n", vec_loop, ret);
		}
	} else if (strcmp(argv[0], "random") == 0) {
		u32 ran_mode, ran_algo, ran_loop;

		ran_mode = Strtoul((u8*)(argv[1]), (u8 **)NULL, 10);
		ran_algo = Strtoul((u8*)(argv[2]), (u8 **)NULL, 10);
		ran_loop = Strtoul((u8*)(argv[3]), (u8 **)NULL, 10);

		for (i = 0; i < ran_loop; i++) {
			if (ran_mode == TEST_AUTH_HASH_NULL ||
				ran_mode == TEST_AUTH_HASH_NORMAL ||
				ran_mode == TEST_AUTH_HASH_SEQUENTIAL ||
				ran_mode == TEST_AUTH_HMAC_NORMAL ||
				ran_mode == TEST_AUTH_HMAC_SEQUENTIAL) {
				if ((ret = random_auth_test(ran_mode, ran_algo)) != SUCCESS) {
                    break;
                }
			} else if (ran_mode == TEST_CIPHER_DES ||
				ran_mode == TEST_CIPHER_3DES ||
				ran_mode == TEST_CIPHER_AES) {
				if ((ret = random_cipher_test(ran_mode, ran_algo)) != SUCCESS) {
                    break;
                }
			} else if (ran_mode == TEST_MIX) {
				if ((ret = random_mix_test(ran_mode, ran_algo)) != SUCCESS ) {
                    break;
                }
			} else {
				__err_printf("no this ran_mode: %08x\n", ran_mode);
			}
		}
        __info_printf("ran_loop(%d): random test done, ret = %d\n", i, ret);

		if (ret == SUCCESS && i == ran_loop) {
			__info_printf("random test (mode, algo) = (%08x, %08x) success\n", ran_mode, ran_algo);
			return _TRUE;
		} else {
			__info_printf("random test (mode, algo) = (%08x, %08x) fail, ret = %d\n", ran_mode, ran_algo, ret);
			return _FALSE;
		}
	} else if (strcmp(argv[0], "storage") == 0) {
        u8 storage_mode, storage_algo, storage_loop;

        storage_mode = Strtoul((u8*)(argv[1]), (u8 **)NULL, 10);
        storage_algo = Strtoul((u8*)(argv[2]), (u8 **)NULL, 10);
        storage_loop = Strtoul((u8*)(argv[3]), (u8 **)NULL, 10);

        for (i = 0; i < storage_loop; i++) {
            if (storage_mode == TEST_KEYSTORAGE) {
                //storage_test(storage_mode, storage_algo);
                if ((ret = key_storage_test(storage_algo)) != SUCCESS) {
                    break;
                }
            } else if (storage_mode == TEST_KEYPADSTORAGE) {
                if ((ret = keypad_storage_test(storage_algo)) != SUCCESS) {
                    break;
                }
            } else {
                __err_printf("no this storage_mode: %08x\n", storage_mode);
            }
        }
        __info_printf("storage_loop(%d): storage test done, ret = %d\n", storage_loop, ret);

        if (ret == SUCCESS && i == storage_loop) {
            __info_printf("storage test (mode, algo) = (%08x, %08x) success\n", storage_mode, storage_algo);
            return _TRUE;
        } else {
            __info_printf("storage test (mode, algo) = (%08x, %08x) fail, ret = %d\n", storage_mode, storage_algo, ret);
            return _FALSE;
        }

    } else if (strcmp(argv[0], "interrupt") == 0) {
		//
	} else if (strcmp(argv[0], "throughput") == 0) {
        u8 thr_mode, thr_algo;
        u32 thr_loop;
        u32 totalTime128 = 0, totalTime192 = 0, totalTime256 = 0;
        u32 totalTime = 0;

        thr_mode = Strtoul((u8*)(argv[1]), (u8 **)NULL, 10);
        thr_algo = Strtoul((u8*)(argv[2]), (u8 **)NULL, 10);
        thr_loop = Strtoul((u8*)(argv[3]), (u8 **)NULL, 10);

        if (thr_mode == TEST_MODE_DEFAULT) {
            cycle_cnt_test(thr_loop);
        } else if (thr_mode == TEST_AUTH_HASH_NULL ||
            thr_mode == TEST_AUTH_HASH_NORMAL ||
            thr_mode == TEST_AUTH_HASH_SEQUENTIAL ||
            thr_mode == TEST_AUTH_HMAC_NORMAL ||
            thr_mode == TEST_AUTH_HMAC_SEQUENTIAL) {
            totalTime = throughput_auth_test(thr_mode, thr_algo, thr_loop);
        } else if (thr_mode == TEST_CIPHER_DES ||
            thr_mode == TEST_CIPHER_3DES ||
            thr_mode == TEST_CIPHER_AES) {
            totalTime = throughput_cipher_test(thr_mode, thr_algo, thr_loop);
        } else if (thr_mode == TEST_MIX) {
            totalTime128 = throughput_mix_test(thr_mode, thr_algo, thr_loop, thr_mix_cipher_keylen128);
            totalTime192 = throughput_mix_test(thr_mode, thr_algo, thr_loop, thr_mix_cipher_keylen192);
            totalTime256 = throughput_mix_test(thr_mode, thr_algo, thr_loop, thr_mix_cipher_keylen256);
            totalTime = totalTime128 + totalTime192 + totalTime256;
        } else {
            __err_printf("no this thr_mode: %08x\n", thr_mode);
        }
        __info_printf("thr_loop(%d): throughput test done, totalTime = %d\n", thr_loop, totalTime);
	}  else if (strcmp(argv[0], "addr") == 0) {
			addr_test_flag = simple_strtoul(argv[1], NULL, 16);
			if(simple_strtoul(argv[2], NULL, 16) != 0){
				src_addr = simple_strtoul(argv[2], NULL, 16);
			}
			if(simple_strtoul(argv[3], NULL, 16) != 0){
				dst_addr= simple_strtoul(argv[3], NULL, 16);
			}
			__info_printf("addr:  flag(0x%x), src_addr(0x%llx), dst_addr(0x%llx)\n", addr_test_flag, src_addr, dst_addr);
	} else {
		rtl_crypto_usage(NULL);
	}

	return _TRUE;
}

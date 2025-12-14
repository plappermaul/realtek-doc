

base path: bootloader/uboot/common/socCmd/hal_peripheral_lib/base/mbedtls/mbedtls-2.2.1

mbedtls_test.c 
include/mbedtls_config.h


bootloader/uboot/common/socCmd/hal_peripheral_lib/base/Makefile

if select ecdsa
148 #obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/rsa.o
149 obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/ecdsa.o
150 obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/ecp_curves.o
151 obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/ecp.o

160 CFLAGS +=  -DMBEDTLS_CONFIG_FILE="<mbedtls_config.h>" \
161          -DMBEDTLS_KEY_ALG_ID=MBEDTLS_ECDSA -DMBEDTLS_RSA_LENGTH=256



if select RSA
148 obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/rsa.o
149 #obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/ecdsa.o
150 #obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/ecp_curves.o
151 #obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/ecp.o

160 CFLAGS +=  -DMBEDTLS_CONFIG_FILE="<mbedtls_config.h>" \
161          -DMBEDTLS_KEY_ALG_ID=MBEDTLS_RSA -DMBEDTLS_RSA_LENGTH=256



MBEDTLS_KEY_ALG_ID= MBEDTLS_ECDSA or MBEDTLS_RSA

if test ecdsa
select one in include/mbedtls_config.h
MBEDTLS_ECP_DP_SECP256R1_ENABLED
MBEDTLS_ECP_DP_SECP384R1_ENABLED
MBEDTLS_ECP_DP_SECP521R1_ENABLED

if test rsa

bootloader/uboot/common/socCmd/hal_peripheral_lib/base/Makefile
-DMBEDTLS_RSA_LENGTH=256
-DMBEDTLS_RSA_LENGTH=384
-DMBEDTLS_RSA_LENGTH=512


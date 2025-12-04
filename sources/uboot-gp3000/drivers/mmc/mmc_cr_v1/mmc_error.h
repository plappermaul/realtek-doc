/************************************************************************
 *
 *  error_type.h
 *
 *  Defines for Phoenix ROM code error types
 *
 ************************************************************************/

#ifndef ERROR_TYPE_H
#define ERROR_TYPE_H

/************************************************************************
 *  Definition
 ************************************************************************/
#define ERR_KEY_SIGNATURE_NOT_FOUND	0xf

#define ERR_HWSETTING_NOT_FOUND		0x11
#define ERR_HWSETTING_FAILED		0x12
#define ERR_HWSETTING_NOT_FINISH	0x13
#define ERR_HWSETTING_VERIFY_FAIL	0x14
#define ERR_HWSETTING_LENGTH		0x15

#define ERR_BOOTCODE_NOT_FOUND		0x20
#define ERR_BOOTCODE_FAILED		0x21
#define ERR_BOOTCODE_INVALID_SIZE	0x22
#define ERR_FSBL_INVALID_SIZE	0x23

#define ERR_MCP_ERROR			0x30
#define	ERR_SHA256_ERROR			0x31
#define ERR_RSA_SIG_NOT_MATCH		0x32
#define ERR_KEY_SIG_NOT_MATCH		0x3d
#define ERR_BOOTCODE_NOT_MATCH		0x3e
#define ERR_NOT_MATCH			    0x3f
#define ERR_FSBL_SIG_NOT_MATCH	    0x3b

#define ERR_NF_RESET_FAIL           0x40
#define ERR_NF_READ_ID_FAIL         0x41
#define ERR_NF_IDENTIFY_FAIL        0x42
#define ERR_NF_READ_ECC_FAIL        0x43
#define ERR_NF_ECC_IDENTIFY_FAIL        0x44

#define ERR_EMMC_INIT_FAIL          0x50    /* emmc initial flow fail */
#define ERR_EMMC_HOST_FAIL          0x51    /* mmc host reset fail */
#define ERR_EMMC_CMD1_FAIL          0x52    /* mmc cmd1 fail present initial process fail */
#define ERR_EMMC_DMA_ERROR          0x53    /* 0xb8010404 b:1 */
#define ERR_EMMC_CMD_ERR_RSP        0x54    /* 0xb8010414 b:2 at respone cmd */
#define ERR_EMMC_CMD_ERR_STR        0x55    /* 0xb8010414 b:2 at stream cmd*/
#define ERR_EMMC_CMD_TOUT_RSP       0x56    /* respone cmd timeout */
#define ERR_EMMC_CMD_TOUT_STR       0x57    /* stream cmd timeout */
#define ERR_EMMC_STATUS_FAIL        0x58    /* emmc wait ststus fail*/
#define ERR_EMMC_BUS_WIDTH          0x59    /* emmc change bit width fail*/
#define ERR_EMMC_SRAM_DMA_TIME      0x5A    /* */
#define ERR_EMMC_XXX_XXB            0x5B    /* */
#define ERR_EMMC_XXX_XXC            0x5C    /* */
#define ERR_EMMC_XXX_XXD            0x5D    /* */
#define ERR_EMMC_XXX_XXE            0x5E    /* */
#define ERR_EMMC_XXX_XXF            0x5F    /* */
#define ERR_EMMC_SEND_STATUS_RETRY_FAIL 0x60
#define ERR_EMMC_SEND_RW_CMD_RETRY_FAIL 0x61
#define ERR_EMMC_CMD3_FAIL          0x63    /* mmc cmd3 fail present initial process fail */
#define ERR_EMMC_CMD7_FAIL          0x64    /* mmc cmd4 fail present initial process fail */


#define EMMC_MAX_CMD_SEND_RETRY_CNT 20
#define EMMC_MAX_STOP_CMD_RETRY_CNT 10

#define ERR_SDCARD_INIT_FAIL          0x70    /* sdcard initial flow fail */
#define ERR_SDCARD_HOST_FAIL          0x71    /* sdcard host reset fail */
#define ERR_SDCARD_CMD1_FAIL          0x72    /* sdcard cmd1 fail present initial process fail */
#define ERR_SDCARD_DMA_ERROR          0x73    /* 0xb8010404 b:1 */
#define ERR_SDCARD_CMD_ERR_RSP        0x74    /* 0xb8010414 b:2 at respone cmd */
#define ERR_SDCARD_CMD_ERR_STR        0x75    /* 0xb8010414 b:2 at stream cmd*/
#define ERR_SDCARD_CMD_TOUT_RSP       0x76    /* respone cmd timeout */
#define ERR_SDCARD_CMD_TOUT_STR       0x77    /* stream cmd timeout */
#define ERR_SDCARD_STATUS_FAIL        0x78    /* sdcard wait ststus fail*/
#define ERR_SDCARD_BUS_WIDTH          0x79    /* sdcard change bit width fail*/
#define ERR_SDCARD_SRAM_DMA_TIME      0x7A    /* */
#define ERR_SDCARD_XXX_XXB            0x7B    /* */
#define ERR_SDCARD_XXX_XXC            0x7C    /* */
#define ERR_SDCARD_XXX_XXD            0x7D    /* */
#define ERR_SDCARD_XXX_XXE            0x7E    /* */
#define ERR_SDCARD_XXX_XXF            0x7F    /* */
#define ERR_SDCARD_SEND_STATUS_RETRY_FAIL 0x80
#define ERR_SDCARD_SEND_RW_CMD_RETRY_FAIL 0x81
#define ERR_SDCARD_CMD3_FAIL          0x83    /* sdcard cmd3 fail present initial process fail */
#define ERR_SDCARD_CMD7_FAIL          0x84    /* sdcard cmd4 fail present initial process fail */
#define ERR_SDCARD_ACMD41_APPFAIL          0x85    /* sdcard acmd41 app fail present initial process fail */
#define ERR_SDCARD_ACMD41_FAIL          0x86    /* sdcard acmd41 fail present initial process fail */
#define ERR_SDCARD_CMD8_FAIL          0x87    /* sdcard cmd8 fail present initial process fail */

#define SDCARD_MAX_CMD_SEND_RETRY_CNT 20
#define SDCARD_MAX_STOP_CMD_RETRY_CNT 10


#define ERR_WAKE_FROM_SUSPEND		0x8f
#define ERR_INVALID_ADDRESS		0x90
#define ERR_TIMEOUT			0x91
#define ERR_UNKNOWN_TYPE		0x92
#define ERR_INVALID_PARAM		0x93
#define ERR_ALLOC_FAILED		0x94
#define ERR_FUNC_NOT_AVAILABLE      0x95
#define ERR_AES_MODE_NOT_SUPPORT	0x96
#define ERR_WRITE_SRAM_FAIL			0x97
#define ERR_UART_BUFFER_FULL		0x9e
#define ERR_UNEXPECTED_RETURN		0x9f

#define ERR_DEFAULT			0xfe	/* default value (not assigned yet) */
#endif /* #ifndef ERROR_TYPE_H */

/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 40647 $
 * $Date: 2013-07-01 15:36:16 +0800 (?±ä?, 01 ä¸ƒæ? 2013) $
 *
 * Purpose : Main function of the EPON OAM protocol stack user application
 *           It create two additional threads for packet Rx and state control
 *
 * Feature : Start point of Europa. Use individual threads
 *           for packet Rx and state control
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include<sys/time.h>

#include <rtk/ldd.h>
#include <rtk/init.h>
#include <rtk/ponmac.h>
#include <rtk/switch.h>
#include <common/util/rt_util.h>

//#include "epon_oam_config.h"
//#include "epon_oam_err.h"
//#include "epon_oam_msgq.h"
#include "europa_cli.h"
#include "europa_8290c.h"
#include "europa_8291.h"

#include <osal/time.h>
#include <math.h>

#if CONFIG_GPON_VERSION < 2
#include <rtk/gpon.h>
#else
#include <module/gpon/gpon.h>
#endif

/* 
 * Symbol Definition 
 */
//#define RTL8290C_FT_TEST
#undef RTL8290C_FT_TEST
    
//#define RTL8290B_CAL_TIME
#undef RTL8290B_CAL_TIME
    
#define EUROPA_CMD_VERSION    "1.0.26"

#define EUROPA_CLI_BRANCH     0x01
#define EUROPA_CLI_LEAF       0x02
#define EUROPA_CLI_END        0x03

#define EUROPA_PARAMETER_SIZE 0x1000
#define EUROPA_BLOCK_SIZE          0x100
#define EUROPA_FILE_SIZE      0x1000
#define EUROPA_FILE_LOCATION       "/var/config/europa.data"

#define EUROPA_STRING_SIZE 256


// shift bits for RX power coefficients
#define RX_A_SHIFT 30
#define RX_B_SHIFT 13
#define RX_C_SHIFT 12

// shift bits for TX power coefficients
#define TX_A_SHIFT 8
#define TX_B_SHIFT 5

#define RSSI_CURR_BREAKDOWN_TH  5000

#define EUROPA_RSSI_CURR_SIZE         100
#define EUROPA_RSSI_CURR_OFFSET       10
#define EUROPA_RSSI_CURR_LOOP       (EUROPA_RSSI_CURR_SIZE-EUROPA_RSSI_CURR_OFFSET)
#define EUROPA_RSSI_CURR_START      (EUROPA_RSSI_CURR_OFFSET/2)
#define EUROPA_RSSI_CURR_END        (EUROPA_RSSI_CURR_OFFSET/2+EUROPA_RSSI_CURR_LOOP)

#define LASER_LUT_SZ 304
//For 8290
//#define IBCONV(b) ((b)*1024/80)>>2
//For 8290B
#define IBCONV(b) ((b)*1024/100)>>2
#define IMCONV(m) ((m)*1024/100)>>2

//#define EUROPA_DEBUG_RXPOWER
#undef EUROPA_DEBUG_RXPOWER

//#define EUROPA_I2C_NOWAIT
#undef EUROPA_I2C_NOWAIT


// ========== LUT gen ========== +
typedef struct LASER_LUT_T
{
    int32 temp;
    double bias;
    double mod;
} laserlut_data_t;

#define MAX_LASER_LUT_DATA_NUM 20

laserlut_data_t gLaserLutData[MAX_LASER_LUT_DATA_NUM];
laserlut_data_t gLaserLut[151];  /* -40C ~ 110C LUT */




typedef struct europa_cli_tree_s {
    unsigned char cmd[16];
    unsigned char type;
    union {
        struct europa_cli_tree_s *pSubTree;
        int (*cli_proc)(int argc, char *argv[], FILE *fp);
    } u;
} europa_cli_tree_t;


static unsigned char default_a0_reg[0x100] = {
0x02, 0x04, 0x0B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0x0C, 0x00, 0x14, 0xC8,
0x00, 0x00, 0x00, 0x00, 0x52, 0x45, 0x41, 0x4C, 0x54, 0x45, 0x4B, 0x20, 0x20, 0x20, 0x20, 0x20,
0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x52, 0x54, 0x4C, 0x38, 0x32, 0x39, 0x30, 0x20,
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x30, 0x30, 0x30, 0x31, 0x05, 0x1E, 0x00, 0xFF, 
0x00, 0x06, 0x00, 0x00, 0x76, 0x65, 0x6E, 0x64, 0x6F, 0x72, 0x70, 0x61, 0x72, 0x74, 0x6E, 0x75,
0x6D, 0x62, 0x65, 0x72, 0x32, 0x30, 0x31, 0x34, 0x30, 0x31, 0x32, 0x33, 0x68, 0x80, 0x02, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static unsigned char default_a2_reg[0x100]={
0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF,/* 0x94, 0x70,*/0x8e, 0x94, 0x6D, 0x60, 0x8C, 0xA0, 0x75, 0x30, 
0x75, 0x30, 0x05, 0xDC, 0x61, 0xA8, 0x07, 0xD0, 0x00, 0x00, 0x0F, 0x8D, 0x00, 0x0A, 0x0C, 0x5A, 
0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x80, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 
0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static unsigned char default_a4_reg[] = {
0x7d,
0x72, 0x89, 0xe1, 0xe2, 0x8e, 0x32, 0x9b, 0x90, 0x00, 0x49, 0x9f, 0xff, 0x23, 0x04, 0x78, 0x7f, 
0xff, 0x00, 0x02, 0x05, 0x00, 0x00, 0x01, 0xf6, 0xce, 0x90, 0xc0, 0x00, 0x00, 0x38, 0x24, 0x40, 
0x40, 0x00, 0x01, 0x13, 0xbc, 0x1e, 0x33, 0x10, 0xfb, 0x18, 0xe4, 0x80, 0xe0, 0x01, 0xb0, 0x44, 
0x40, 0xac, 0x03, 0x00, 0xf4, 0x00, 0x20, 0x40, 0x00, 0x7c, 0x20, 0x40, 0xfc, 0x30, 0x10, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xff, 0xfc, 0x12, 0x0a, 0x8b, 0x04, 0x01, 0x33, 0x74, 0xa5, 0xa0, 0xe0, 
0x00, 0x00, 0x00, 0x00, 0x81, 0x30, 0x00, 0x15, 0x00, 0xff, 0x1f, 0x01, 0x00, 0xff, 0x00, 0x32, 
0xff, 0x01, 0xff, 0xff, 0x05, 0x00, 0x60, 0x80, 0x00, 0x08, 0x10, 0xa0, 0xec, 0xb0, 0xbf, 0xfe, 
0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x10, 0x00, 0x00, 0x00, 0xff, 0xa0, 0xc0
};

static unsigned char default_apd_lut[] = {
0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B,
0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23,
0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24 
};

static unsigned char default_laser_lut[] = {
0x0A, 0x20, 0x0A, 0x20, 0x0A, 0x20, 0x0A, 0x20, 0x0A, 0x20, 0x0A, 0x20, 0x0A, 0x20, 0x0A, 0x20,
0x0A, 0x20, 0x0A, 0x20, 0x0C, 0x20, 0x0C, 0x20, 0x0C, 0x20, 0x0C, 0x20, 0x0C, 0x20, 0x0C, 0x20,
0x0C, 0x20, 0x0C, 0x20, 0x0C, 0x20, 0x0C, 0x20, 0x0E, 0x20, 0x0E, 0x20, 0x0E, 0x20, 0x0E, 0x20,
0x0E, 0x20, 0x0E, 0x20, 0x0E, 0x20, 0x0E, 0x20, 0x0E, 0x20, 0x0E, 0x20, 0x10, 0x20, 0x10, 0x20,
0x10, 0x20, 0x10, 0x20, 0x10, 0x20, 0x10, 0x20, 0x10, 0x20, 0x10, 0x20, 0x10, 0x20, 0x10, 0x20,
0x12, 0x20, 0x12, 0x20, 0x12, 0x20, 0x12, 0x20, 0x12, 0x20, 0x12, 0x20, 0x12, 0x20, 0x12, 0x20,
0x12, 0x20, 0x12, 0x20, 0x14, 0x32, 0x14, 0x32, 0x14, 0x32, 0x14, 0x32, 0x14, 0x32, 0x14, 0x32,
0x14, 0x32, 0x14, 0x32, 0x14, 0x32, 0x14, 0x32, 0x18, 0x34, 0x18, 0x34, 0x18, 0x34, 0x18, 0x34,
0x18, 0x34, 0x18, 0x34, 0x18, 0x34, 0x18, 0x34, 0x18, 0x34, 0x18, 0x34, 0x1C, 0x38, 0x1C, 0x38,
0x1C, 0x38, 0x1C, 0x38, 0x1C, 0x38, 0x1C, 0x38, 0x1C, 0x38, 0x1C, 0x38, 0x1C, 0x38, 0x1C, 0x38,
0x24, 0x40, 0x24, 0x40, 0x24, 0x40, 0x24, 0x40, 0x24, 0x40, 0x24, 0x40, 0x24, 0x40, 0x24, 0x40,
0x24, 0x40, 0x24, 0x40, 0x2C, 0x4C, 0x2C, 0x4C, 0x2C, 0x4C, 0x2C, 0x4C, 0x2C, 0x4C, 0x2C, 0x4C,
0x2C, 0x4C, 0x2C, 0x4C, 0x2C, 0x4C, 0x2C, 0x4C, 0x30, 0x54, 0x30, 0x54, 0x30, 0x54, 0x30, 0x54,
0x30, 0x54, 0x30, 0x54, 0x30, 0x54, 0x30, 0x54, 0x30, 0x54, 0x30, 0x54, 0x40, 0x5C, 0x40, 0x5C,
0x40, 0x5C, 0x40, 0x5C, 0x40, 0x5C, 0x40, 0x5C, 0x40, 0x5C, 0x40, 0x5C, 0x40, 0x5C, 0x40, 0x5C,
0x50, 0x60, 0x50, 0x60, 0x50, 0x60, 0x50, 0x60, 0x50, 0x60, 0x70, 0x68, 0x70, 0x68, 0x70, 0x68,
0x70, 0x68, 0x70, 0x68, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C,
0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C,
0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C
};

//P0, P1, Paverge, and others 
static unsigned char default_param[]= {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x26, 0x50, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00
};

static unsigned char rtl8290b_a0_reg[0x100] = {
0x02, 0x04, 0x0B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0x0C, 0x00, 0x14, 0xC8,
0x00, 0x00, 0x00, 0x00, 0x52, 0x45, 0x41, 0x4C, 0x54, 0x45, 0x4B, 0x20, 0x20, 0x20, 0x20, 0x20,
0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x52, 0x54, 0x4C, 0x38, 0x32, 0x39, 0x30, 0x20,
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x30, 0x30, 0x30, 0x31, 0x05, 0x1E, 0x00, 0xFF, 
0x00, 0x06, 0x00, 0x00, 0x76, 0x65, 0x6E, 0x64, 0x6F, 0x72, 0x70, 0x61, 0x72, 0x74, 0x6E, 0x75,
0x6D, 0x62, 0x65, 0x72, 0x32, 0x30, 0x31, 0x34, 0x30, 0x31, 0x32, 0x33, 0x68, 0x80, 0x02, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static unsigned char rtl8290b_a2_reg[0x100]={
0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF,/* 0x94, 0x70,*/0x8e, 0x94, 0x6D, 0x60, 0x8C, 0xA0, 0x75, 0x30, 
0x75, 0x30, 0x05, 0xDC, 0x61, 0xA8, 0x07, 0xD0, 0x00, 0x00, 0x0F, 0x8D, 0x00, 0x0A, 0x0C, 0x5A, 
0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x80, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 
0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

//20220623: Change W8 to 0x1
static unsigned char rtl8290b_a4_reg[] = {
/*The 1st value is length of rtl8290b_a4_reg[]*/0x85,
0x02, 0x89, 0xA1, 0xFE, 0x8E, 0xB2, 0x9B, 0x90, 0x01, 0x49, 0x9F, 0xFF, 0x23, 0x04, 0x78, 0x7F,
0xFF, 0x00, 0xE2, 0x05, 0x00, 0x00, 0x01, 0xF6, 0xCE, 0x90, 0xC0, 0x00, 0x00, 0x38, 0x24, 0x40,
0x40, 0x00, 0x01, 0x08, 0xBA, 0x1E, 0x72, 0xC0, 0x0B, 0x1A, 0xE0, 0x80, 0xE0, 0x01, 0xB0, 0x44,
0x40, 0x80, 0x00, 0x08, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x55, 0x80, 0x20, 0xFD, 0xFD,
0x01, 0x16, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x01, 0x81, 0x00, 0x00, 0x3F,
0x00, 0x00, 0x00, 0x00, 0x81, 0x30, 0x00, 0x34, 0x00, 0xFF, 0xFF, 0x01, 0x00, 0xFF, 0x00, 0x32,
0xFF, 0x03, 0xFF, 0x07, 0x00, 0x00, 0xA0, 0xC0, 0x00, 0x08, 0x10, 0xE0, 0xE0, 0xE0, 0xBF, 0xFE, 
0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x02
};

static unsigned char rtl8290b_apd_lut[] = {
0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B,
0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23,
0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24 
};

static unsigned char rtl8290b_laser_lut[] = {
0x0A, 0x20, 0x0A, 0x20, 0x0A, 0x20, 0x0A, 0x20, 0x0A, 0x20, 0x0A, 0x20, 0x0A, 0x20, 0x0A, 0x20,
0x0A, 0x20, 0x0A, 0x20, 0x0C, 0x20, 0x0C, 0x20, 0x0C, 0x20, 0x0C, 0x20, 0x0C, 0x20, 0x0C, 0x20,
0x0C, 0x20, 0x0C, 0x20, 0x0C, 0x20, 0x0C, 0x20, 0x0E, 0x20, 0x0E, 0x20, 0x0E, 0x20, 0x0E, 0x20,
0x0E, 0x20, 0x0E, 0x20, 0x0E, 0x20, 0x0E, 0x20, 0x0E, 0x20, 0x0E, 0x20, 0x10, 0x20, 0x10, 0x20,
0x10, 0x20, 0x10, 0x20, 0x10, 0x20, 0x10, 0x20, 0x10, 0x20, 0x10, 0x20, 0x10, 0x20, 0x10, 0x20,
0x12, 0x20, 0x12, 0x20, 0x12, 0x20, 0x12, 0x20, 0x12, 0x20, 0x12, 0x20, 0x12, 0x20, 0x12, 0x20,
0x12, 0x20, 0x12, 0x20, 0x14, 0x32, 0x14, 0x32, 0x14, 0x32, 0x14, 0x32, 0x14, 0x32, 0x14, 0x32,
0x14, 0x32, 0x14, 0x32, 0x14, 0x32, 0x14, 0x32, 0x18, 0x34, 0x18, 0x34, 0x18, 0x34, 0x18, 0x34,
0x18, 0x34, 0x18, 0x34, 0x18, 0x34, 0x18, 0x34, 0x18, 0x34, 0x18, 0x34, 0x1C, 0x38, 0x1C, 0x38,
0x1C, 0x38, 0x1C, 0x38, 0x1C, 0x38, 0x1C, 0x38, 0x1C, 0x38, 0x1C, 0x38, 0x1C, 0x38, 0x1C, 0x38,
0x24, 0x40, 0x24, 0x40, 0x24, 0x40, 0x24, 0x40, 0x24, 0x40, 0x24, 0x40, 0x24, 0x40, 0x24, 0x40,
0x24, 0x40, 0x24, 0x40, 0x2C, 0x4C, 0x2C, 0x4C, 0x2C, 0x4C, 0x2C, 0x4C, 0x2C, 0x4C, 0x2C, 0x4C,
0x2C, 0x4C, 0x2C, 0x4C, 0x2C, 0x4C, 0x2C, 0x4C, 0x30, 0x54, 0x30, 0x54, 0x30, 0x54, 0x30, 0x54,
0x30, 0x54, 0x30, 0x54, 0x30, 0x54, 0x30, 0x54, 0x30, 0x54, 0x30, 0x54, 0x40, 0x5C, 0x40, 0x5C,
0x40, 0x5C, 0x40, 0x5C, 0x40, 0x5C, 0x40, 0x5C, 0x40, 0x5C, 0x40, 0x5C, 0x40, 0x5C, 0x40, 0x5C,
0x50, 0x60, 0x50, 0x60, 0x50, 0x60, 0x50, 0x60, 0x50, 0x60, 0x70, 0x68, 0x70, 0x68, 0x70, 0x68,
0x70, 0x68, 0x70, 0x68, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C,
0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C,
0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C, 0x88, 0x6C
};

//P0, P1, Paverge, and others, The start address is RTL8290B_DCL_P0_ADDR (1346)
static unsigned char rtl8290b_default_param[]= {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x26, 0x50, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x05, 0x04, 0x01, 0x01, /*1405*/
0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x08, 0x00, 0x0E, 0x00, 0x00, 0x04, 0x02, 0x01, 0x00, /*1421*/
0x02, 0x00, 0x01, 0x00, 0x40, 0x07, 0x00, 0x05, 0x01, 0x05, 0x05, 0x01, 0x01, 0x88, 0xCC, 0x4D, /*1437*/
0x2A, 0x00, 0x01, 0x01, 0x01, 0x01, 0x03, 0x01, 0x02, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, /*1453*/
0x04, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x40, 0x03, 0x02, 0x00, 0x00, 0x03, 0x01, 0x01, 0x00, /*1469*/
0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x08, 0x45, 0x08, 0x20, 0x00, 0x00, /*1485*/
0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x03, 0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, /*1501*/
0x04, 0x00, 0x64, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*1517*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*1533*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

/*  
 * Data Declaration  
 */
//static int msgQId;
static europa_cli_tree_t cliRootInit[] = {
    {
        "flash",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_flash_init }
    },
    {
        "empty",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_param_open }
    },
    {
        "chip",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_chip_init }
    },    
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
};

static europa_cli_tree_t cliRootGetChip[] = {
    {
        "vmpdcal",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_vmpdCal_get }
    },    
    {
        "txpwr",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_txpower_get }
    },
    {
        "rxpwr",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_rxpower_get }
    },       
    {
        "txbias",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_txbias_get }
    },    
    {
        "txmod",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_txmod_get }
    },
    {
        "rssiv0",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_rssiv0_get }
    },    
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
};

static europa_cli_tree_t cliRootGetFlash[] = {
    {
        "param",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_flash_get }
    },       
    {
        "p0p1",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_P0P1_get }
    },    
    {
        "vn", /* A0/20~35 Vendor Name */
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_VendorName_get }
    },         
    {
        "pn", /* A0/40~55 Vendor Part Number */
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_VendorPN_get }
    },         
    {
        "rev", /* A0/56~59 Vendor Rev */
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_VendorRev_get }
    }, 
    {
        "sn", /* A0/68~83 Vendor Serial Number */
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_VendorSN_get }
    },           
    {
        "datecode", /* A0/84~91 Date Codev */
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_DateCode_get }
    }, 
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
};

static europa_cli_tree_t cliRootCalLaserlut[] = {
    {
        "data",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_laserlut }
    },       
    {
        "clear",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_gen_laserlut2_clear }
    },    
    {
        "calc", 
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_gen_laserlut2_calc }
    },         
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
};



static europa_cli_tree_t cliRootDumpFlash[] = {
    {
        "all",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_flash_getAll }
    },    
    {
        "a0",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_flash_a0_get }
    }, 
    {
        "a2",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_flash_a2_get }
    }, 
    {
        "a4",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_flash_a4_get }
    }, 
    {
        "apdlut",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_flash_apd_lut_get }
    },
    {
        "laserlut",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_flash_laser_lut_get }
    },    
    {
        "param",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_flash_param_get }
    },
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
};


static europa_cli_tree_t cliRootDump[] = {
    {
        "flash",
        EUROPA_CLI_BRANCH,
        { cliRootDumpFlash }
    },
    {
        "page",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_register_page_get }
    },     
    {
        "a0",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_ddmi_a0_getAll }
    }, 
    {
        "a2",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_ddmi_a2_getAll }
    }, 
    {
        "a4",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_a4_getAll }
    }, 
    {
        "a5",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_a5_getAll }
    },
    {
        "all",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_register_getAll }
    },    
    {
        "efuse",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_efuse_dump }
    },
    {
        "sram",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_sram_dump }
    },    
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
};

static europa_cli_tree_t cliRootCalEpon[] = { 
    {
        "er",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_epon_er }
    },    
    {
        "trim",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_epon_trim }
    },
    {
        "cmpd",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_epon_cmpd }
    }, 
    {
        "lpf",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_epon_lpf }
    },     
    {
        "apc-cross",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_epon_apc_cross }
    },
    {
        "comperc",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_epon_erc_comp }
    },    
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
};

static europa_cli_tree_t cliRootCal[] = { 
    {
        "init",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_init }
    },
    {
        "load",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_load }
    },  
    {
        "save",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_save }
    },  
    {
        "refresh",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_refresh }
    }, 
    {
        "txddmi",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_tx_ddmi }
    }, 
    {
        "rxddmi",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_rx_ddmi }
    },
    {
        "qreg",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_qreg_x1_y1_x2_y2_x3_y3 }
    },     
    {
        "power",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_power }
    }, 
    {
        "er",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_er }
    },    
    {
        "trim",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_trim }
    }, 
#if 0 
    {
        "los",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_los }
    },    
#endif   
    {
        "hyst",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_hyst }
    },  
    {
        "cmpd",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_cmpd }
    }, 
    {
        "lpf",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_lpf }
    },       
    {
        "cross",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_cross }
    },
    {
        "toffset",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_toffset }
    },  
    {
        "tscale",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_tscale }
    },  
    {
        "vbr",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_vbr }
    }, 
    {
        "laserlut",
        EUROPA_CLI_BRANCH,
        { cliRootCalLaserlut }
    },     
    {
        "rxpol",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_rxpol }
    },
    {
        "lospol",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_rxlospol }
    },
    {
        "rxlos",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_rxlos }
    },     
    {
        "rxsd",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_rxsd }
    },    
    {
        "autopwr",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_autopwr }
    },     
    {
        "rxtoffset",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_rxtoffset }
    },    
    {
        "comperc",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_erc_comp }
    },     
    {
        "txes",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_txes }
    },     
    {
        "ibmax",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_ibias_max }
    },      
    {
        "immax",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_imod_max }
    },    
    {
        "ibmin",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_ibias_min }
    },    
    {
        "level",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_level }
    },  
    {
        "ben",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_ben }
    }, 
    {
        "epon",
        EUROPA_CLI_BRANCH,
        { cliRootCalEpon }
    },  
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
};

static europa_cli_tree_t cliRootEpon[] = { 
    {
        "prbs",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_epon_prbs }
    },  
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
};

static europa_cli_tree_t cliRootGpon[] = { 
    {
        "prbs",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_cal_prbs }
    }, 
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
};


static europa_cli_tree_t cliRootGenLaserlut2[] = {  
    {
        "data",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_gen_laserlut2_data }
    }, 
    {
        "clear",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_gen_laserlut2_clear }
    },    
    {
        "calc", 
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_gen_laserlut2_calc }
    },    
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
};



static europa_cli_tree_t cliRootGen[] = {  
    {
        "laserlut",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_gen_laserlut }
    }, 
    {
        "laserlut2",
        EUROPA_CLI_BRANCH,
        { cliRootGenLaserlut2 }
    },    
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
};


static europa_cli_tree_t cliRootGet[] = {
    {
        "reg",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_register_get }
    },    
    {
        "loopmode",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_loopmode_get }
    },  
    {
        "temperature",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_temperature_get }
    },
    {
        "vdd",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_vdd_get }
    },    
    {
        "txpwr",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_txpower_get }
    },
    {
        "rxpwr",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_rxpower_get }
    },
    {
        "rxpower2",
        EUROPA_CLI_LEAF,
        { .cli_proc = rtl8290c_cli_rxpower2_get }
    },
#if 0  
    {
        "rxtrans",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_rxtrans_get }
    },    
    {
        "rxshft",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_param_shift_get }
    },
#endif    
    {
        "txparam",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_txparam_get }
    },
    {
        "rxparam",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_rxparam_get }
    },    
    {
        "txbias",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_txbias_get }
    },    
    {
        "txmod",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_txmod_get }
    },
    {
        "rssiv0",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_rssiv0_get }
    },
    {
        "rssi-v02",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_rssiv0_2_get }
    },    
    {
        "rssi-voltage",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_rssi_voltage5_get }
    },    
    {
        "txif",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_txif_get }
    },    
    {
        "driver-version",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_driver_version_get }
    },  
    {
        "laserinfo",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_laserinfo_get }
    },     
    {
        "laserlut",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_laserlut_get }
    },    
    {
        "apdlut",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_apd_lut_get }
    },    
    {
        "resistor",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_rssi_resistor_get }
    },     
    {
        "booster",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_booster_get }
    },    
    {
        "apcdig",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_apcdig_get }
    },
    {
        "sdadc",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_sdadc_code_get }
    },  
    {
        "efuse",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_efuse_get }
    }, 
    {
        "dac",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_dac_get }
    },
    {
        "txinfo",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_txinfo_get }
    },    
    {
        "test",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_test_get }
    },    
    {
        "flash",
        EUROPA_CLI_BRANCH,
        { cliRootGetFlash }
    },    
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }


#if 0    
    {
        "chip",
        EUROPA_CLI_BRANCH,
        { cliRootGetChip }
    },
    {
        "param",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_flash_get }
    },
    {
        "all",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_all_get }
    },       
    {
        "ddmia0",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_ddmi_a0_get }
    },    
    {
        "ddmia2",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_ddmi_a2_get }
    },
    {
        "a4",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_a4_get }
    },       
    {
        "p0p1",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_P0P1_get }
    },    
    {
        "vn", /* A0/20~35 Vendor Name */
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_VendorName_get }
    },         
    {
        "pn", /* A0/40~55 Vendor Part Number */
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_VendorPN_get }
    },         
    {
        "rev", /* A0/56~59 Vendor Rev */
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_VendorRev_get }
    },         
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
#endif    
};
static europa_cli_tree_t cliRootSetFlash[] = {
    {
        "param",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_flash_set }
    },
    {
        "long",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_long_set }
    },    
    {
        "ddmia0",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_ddmi_a0_set }
    },    
    {
        "ddmia2",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_ddmi_a2_set }
    },
    {
        "a4",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_a4_set }
    },    
    {
        "apdlut",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_apd_lut_set }
    },
    {
        "laserlut",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_laser_lut_set }
    },    
    {
        "p0",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_P0_set }
    },       
    {
        "p1",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_P1_set }
    },         
    {
        "vn", /* A0/20~35 Vendor Name */
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_VendorName_set }
    },         
    {
        "pn", /* A0/40~55 Vendor Part Number */
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_VendorPN_set }
    },         
    {
        "rev", /* A0/56~59 Vendor Rev */
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_VendorRev_set }
    },          
    {
        "sn", /* A0/68~83 Vendor Serial Number */
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_VendorSN_set }
    },           
    {
        "datecode", /* A0/84~91 Date Codev */
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_DateCode_set }
    },     
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
};

static europa_cli_tree_t cliRootSet[] = {
    {
        "flash",
        EUROPA_CLI_BRANCH,
        { cliRootSetFlash }
    },
    {
        "reg",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_register_set }
    },    
    {
        "loopmode",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_loopmode_set }
    },
    {
        "txparam",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_txparam_set }
    },
    {
        "rxparam",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_rxparam_set }
    },     
    {
        "txbias",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_txbias_set }
    },    
    {
        "txmod",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_txmod_set }
    }, 
    {
        "txif",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_txif_set }
    },   
    {
        "laserlut",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_laserlut_set }
    },    
    {
        "apdlut",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_apdlut_set }
    },  
    {
        "resistor",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_rssi_resistor_set }
    },
    {
        "efuse",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_efuse_set }
    }, 
    {
        "dac",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_dac_set }
    },
    {
        "booster",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_booster_set }
    },    
    {
        "fsutest",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_fsuTest_set }
    }, 
    {
        "short",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_shotBurst_set }
    },     
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
};

static europa_cli_tree_t cliRootUpdate[] = {  
    {
        "flash",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_update_flash }
    },    
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
};

static europa_cli_tree_t cliRoot[] = {
    {
        "init",
        EUROPA_CLI_BRANCH,
        {cliRootInit }
    },
    {
        "reset", 
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_reset }
    },    
    {
        "set",
        EUROPA_CLI_BRANCH,
        { cliRootSet }
    },       
    {
        "get",
        EUROPA_CLI_BRANCH,
        { cliRootGet }
    },
    {
        "cal",
        EUROPA_CLI_BRANCH,
        { cliRootCal }
    }, 
    {
        "gen",
        EUROPA_CLI_BRANCH,
        { cliRootGen }
    },    
    {
        "dump",
        EUROPA_CLI_BRANCH,
        { cliRootDump }
    },    
    {
        "clear",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_param_clear }
    },
    {
        "close",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_param_close }
    },    
    {
        "delete",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_delete }
    },  
    {
        "gpon",
        EUROPA_CLI_BRANCH,
        { cliRootGpon }
    },    
    {
        "epon",
        EUROPA_CLI_BRANCH,
        { cliRootEpon }
    },
    {
        "update",
        EUROPA_CLI_BRANCH,
        { cliRootUpdate }
    }, 
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }       
};

uint32 vmpd_cal;

/* 
 * Macro Definition 
 */

/*  
 * Function Declaration  
 */

/* ------------------------------------------------------------------------- */
/* Internal APIs */


int ascii_to_hex(char ch) 
{ 
    char ch_tmp; 
    int hex_val = -1; 


    ch_tmp = tolower(ch); 

    if ((ch_tmp >= '0') && (ch_tmp <= '9')) 
    { 
        hex_val = ch_tmp - '0'; 
    } 
    else if ((ch_tmp >= 'a') && (ch_tmp <= 'f')) 
    { 
        hex_val = ch_tmp - 'a' + 10; 
    } 

    return hex_val; 
} 

int str_to_hex(char *hex_str) 
{ 
    int i, len; 
    int hex_tmp; 
    uint32 hex_val; 

    len = strlen(hex_str); 
    if(len>=8)
    {
        printf("\rInput Overflow! hexlen = %d (Should < 8). \n", len);    
        return -1;
    }
    hex_val = 0; 
    for (i=0; i<len;i++) 
    { 
        if ((hex_val&0xf0000000)!=0)
        {
            return -1;
        }    
        hex_tmp = ascii_to_hex(hex_str[i]); 
    
        if (hex_tmp < 0) 
            return -1;
    
        hex_val	= hex_val&0xfffffff;
        hex_val = ((hex_val)<<4) + hex_tmp;
    } 
    return hex_val; 
}

int _vlaue_translate(char *str_value)
{
    char *ptr_tmp;
    int value;

    ptr_tmp = str_value;

    if ((*ptr_tmp == '0') && (*(ptr_tmp+1) == 'x')) 
    {
       ptr_tmp+=2;
       value = str_to_hex(ptr_tmp);
    }
    else
    {
        value = atoi(str_value); 
    }

    return value;
}

static void _europa_rxbubble_sort(europa_rxpwr_t arr[], uint32 num)
{
    uint32  i, j;
    //uint8 k;
    europa_rxpwr_t temp;

    //printk(KERN_EMERG "\nUnsorted Data:");
    //for (k = 0; k < num; k++) {
    //  printk(KERN_EMERG " %d ", iarr[k]);
   //}

    for (i = 1; i < num; i++)
    {
        for (j = 0; j < num - 1; j++)
        {
            if (arr[j].rssi_v > arr[j + 1].rssi_v)
            {
                //temp = arr[j];
                memcpy(&temp, &arr[j], sizeof(europa_rxpwr_t));
                //arr[j] = arr[j + 1];
                memcpy(&arr[j], &arr[j+1], sizeof(europa_rxpwr_t));                
                //arr[j + 1] = temp;
                memcpy(&arr[j+1], &temp, sizeof(europa_rxpwr_t));                
            }
        }
    }

   // printk("\nAfter pass %d : ", i);
    //for (k = 0; k < num; k++)
    //{
         //printk(" %d ", iarr[k]);
    //}

}


static int32 europa_i2c_write(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 data)
{
    int32   ret;

#if defined(CONFIG_COMMON_RT_API)
    ret = rt_i2c_write(i2cPort, devID, regAddr, data);
#else
    uint32 chipId, rev, subType;

    if((ret = rtk_switch_version_get(&chipId, &rev, &subType)) != 0)
    {
        return ret;
    }

#ifdef EUROPA_I2C_NOWAIT        
    if (RTL9601B_CHIP_ID == chipId)
    {
        ret = rtk_i2c_noWait_write(i2cPort, devID, regAddr, data);
    }
    else
    {
        ret = rtk_i2c_write(i2cPort, devID, regAddr, data);
    }
#else
    ret = rtk_i2c_write(i2cPort, devID, regAddr, data);    
#endif



#endif

    return ret;
}

static int32 europa_i2c_read(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData)
{
    int32   ret;

#if defined(CONFIG_COMMON_RT_API)
        ret = rt_i2c_read(i2cPort, devID, regAddr, pData);
#else
        ret = rtk_i2c_read(i2cPort, devID, regAddr, pData);
#endif

    return ret;
}

int _europa_ldd_parser(void)
{
    uint8 tmp;
    uint32 value1, value2, tmp2;
    int ret, mode;    
    uint32 devId = 5;

    mode = 0;
    value1 = 0;
    value2 = 0; 
    
    europa_i2c_read(EUROPA_I2C_PORT, 0x55, 0x90, &value1);
    europa_i2c_read(EUROPA_I2C_PORT, 0x55, 0x91, &value2); 
    value1 = (value1<<8)|(value2&0xFF);     
    if (value1 != 0x8290)
    {     
        if (rtk_ldd_parameter2_get(devId, 0x680, &tmp2)==0)
        {
            value2 = (uint32)tmp2;
        }
        if (rtk_ldd_parameter2_get(devId, 0x681, &tmp2)==0)
        {
            value2 = (value2<<8)|(uint32)tmp2;    
        }    
    }

    if (value1 == 0x8290)
    {
        mode = EUROPA_LDD_8290B_MODE;        
    }
    else if (value2 == 0x8290)
    {
        mode = EUROPA_LDD_8290C_MODE;        
    }
    else if (value2 == 0x8291)
    {
        mode = EUROPA_LDD_8291_MODE;        
    }    
    else
    {
        mode = EUROPA_LDD_UNKNOWN_MODE;   
    }
    //printf("\rvalue1 = 0x%x, value2 = 0x%x, mode = %d\n", value1, value2, mode);    
    
    return mode;
}


static int _europa_gpon_init(void)
{
    uint32 chipId, rev, subType;  
    int ret;

    if((ret = rtk_switch_version_get(&chipId, &rev, &subType)) != 0)
    {
        return ret;
    }

    if (RTL9607C_CHIP_ID == chipId)
    {
        if (rev > CHIP_REV_ID_B)
        {
            rtk_ponmac_mode_t mode;    
                
            if((ret = rtk_ponmac_mode_get(&mode)) != 0)
            {
                return ret;
            }

            //printf("PON Mode = %d", mode);

            if (mode == 0) //GPON mode
            {
                system("diag gpon init");
            }
            else if (mode == 1) //EPON mode
            {
                //20220329: 9607CVB init patch
                system("diag reg set 0x270 0x1f");
                system("diag reg set 0x40038 0x900");
                system("diag reg set 0x4003c 0x20");
                system("diag reg set 0x4072c 0x1800");
                system("diag reg set 0x405e8 0x2e2");
                system("diag reg set 0x405dc 0x2000");
                system("diag reg set 0x40708 0x2e30");
                system("diag reg set 0x405e0 0x23c2");
                system("diag reg set 0x405e4 0x6f42");
                system("diag reg set 0x405b8 0x2607");
                system("diag reg set 0x405bc 0x8090");
                system("diag reg set 0x40588 0xc41d");
                system("diag reg set 0x405b4 0x9150");
                system("diag reg set 0x40594 0xa4");
                system("diag reg set 0x40598 0x8000");
                system("diag reg set 0x405a4 0x187C");
                system("diag reg set 0x40714 0xc5b0");
                system("diag reg set 0x405F8 0x3689");
                system("diag reg set 0x405FC 0xf4a");
                system("diag reg set 0x40710 0xc081");
                system("diag reg set 0x270 0x08"); 
            }
            else
            {
                printf("PON Mode Error( mode = %d)!!!", mode);                
            }            
        }
        else
        {
            system("diag reg set 0x40030 0xf30");
            system("diag reg set 0x40508 0x0");
            system("diag reg set 0x270 0x1f");
            system("diag reg set 0x40038 0x900");
            system("diag reg set 0x4003c 0x20");
            system("diag reg set 0x405c4 0x2a89");
            system("diag reg set 0x405e0 0x7BC0");
            system("diag reg set 0x405e4 0xEf40");
            system("diag reg set 0x405d4 0xE5AC");
            system("diag reg set 0x405a4 0x1800");
            system("diag reg set 0x40598 0x8002");
            system("diag reg set 0x40588 0x231D");
            system("diag reg set 0x405A0 0x1245");
            system("diag reg set 0x405b0 0x11");
            system("diag reg set 0x405b4 0x9150");
            system("diag reg set 0x405b8 0x2107");
            system("diag reg set 0x40580 0x80");
             system("diag reg set 0x270 0x8");
            system("diag reg set 0x40508 0x3000");
        }
    }
    else if (RTL9602C_CHIP_ID == chipId)
    {
        system("diag reg set 0x1D0 0x1f");
        system("diag reg set 0x225d8 0x29");
        system("diag reg set 0x225ac 0xB000");
        system("diag reg set 0x225b0 0x4848");
        system("diag reg set 0x22738 0x80C5");
        system("diag reg set 0x22504 0x30");
        system("diag reg set 0x22038 0x900");
        system("diag reg set 0x2203c 0x0");
        system("diag reg set 0x1D0 0x8");
        system("diag reg set 0x22508 0x3000");
    }
    else if (RTL9603CVD_CHIP_ID == chipId)
    {
#if 1 //20210707 IB too large
        system("diag reg set 0x200 0x1f");
        system("diag reg set 0x40038 0x900");
        system("diag reg set 0x4058c 0x1929");
        system("diag reg set 0x405A4 0x2a89");
        system("diag reg set 0x405c4 0x3032");
        system("diag reg set 0x405d0 0x8dad");
        system("diag reg set 0x405d4 0x13A4");
        system("diag reg set 0x405e8 0x822d");
        system("diag reg set 0x405ec 0x23A4");
        system("diag reg set 0x200 0x8");
#endif
        system("diag gpon init");
    }
#if defined(CONFIG_LUNA_G3_SERIES)    
    //For Cortina ARM-based 9607DQ series
    else if (RTL8277C_CHIP_ID == chipId)
    {
        system("killall -9 monitord omci_app eponoamd");
        system("diag rt_ponmisc set force-prbs off");
        system("diag rt_ponmisc set mode 255 speed 0");        
        system("diag rt_ponmisc set mode 0 speed 0");
        system("diag rt_gpon init");
        system("diag rt_gpon activate init-state o1");
    }
#endif
    else
    {
        printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
    }

    return 0;
}


static int _europa_epon_init(void)
{
    uint32 chipId, rev, subType;  
    int ret;

    if((ret = rtk_switch_version_get(&chipId, &rev, &subType)) != 0)
    {
        return ret;
    }

    if (RTL9607C_CHIP_ID == chipId)
    {
        if (rev > CHIP_REV_ID_B)
        {
            //20220329: 9607CVB init patch
            system("diag reg set 0x270 0x1f");    
            system("diag reg set 0x40c00 0x1140");
            system("diag reg set 0x40808 0x71e0");
            system("diag reg set 0x40038 0x900");
            system("diag reg set 0x407ac 0x1800");
            system("diag reg set 0x405e8 0x2e2");
            system("diag reg set 0x405dc 0x2000");
            system("diag reg set 0x40788 0x2e30");
            system("diag reg set 0x405e0 0x6BC0");
            system("diag reg set 0x405b8 0x2607");
            system("diag reg set 0x405bc 0x8090");
            system("diag reg set 0x40588 0xc81d");
            system("diag reg set 0x405b4 0x9150");
            system("diag reg set 0x40594 0xa4");
            system("diag reg set 0x40598 0x8000");
            system("diag reg set 0x405a4 0x187C");
            system("diag reg set 0x405ac 0x94e0");
            system("diag reg set 0x40794 0xca70");
            system("diag reg set 0x405F8 0x3689");
            system("diag reg set 0x405FC 0xf4a");
            system("diag reg set 0x270 0xC");
            
            //system("diag epon init");             
        }
        else
        {
            system("diag reg set 0x270 0x1f");    
            system("diag reg set 0x40c00 0x1140");
            system("diag reg set 0x40808 0x71e0");
            system("diag reg set 0x40038 0x900");
            system("diag reg set 0x405c4 0x2a89");
            system("diag reg set 0x405e0 0x7BC0");
            system("diag reg set 0x405e4 0xEf40");
            system("diag reg set 0x405d4 0xFDF0");
            system("diag reg set 0x40588 0x231D");
            system("diag reg set 0x405a4 0x1800");
            system("diag reg set 0x40598 0x8002");
            system("diag reg set 0x405A0 0x1245");
            system("diag reg set 0x405b0 0x11");
            system("diag reg set 0x405b4 0x9150");
            system("diag reg set 0x405b8 0x2107");
            system("diag reg set 0x40580 0x80");
            system("diag reg set 0x270 0xC");
        }
    }
    else if (RTL9602C_CHIP_ID == chipId)
    {
        system("diag reg set 0x1D0 0x1f");
        system("diag reg set 0x22c00 0x1140");
        system("diag reg set 0x22808 0x71e0");
        system("diag reg set 0x225d8 0x29");
        system("diag reg set 0x225ac 0xB000");
        system("diag reg set 0x225b0 0x4848");
        system("diag reg set 0x227b8 0x80C5");
        system("diag reg set 0x22038 0x900");
        system("diag reg set 0x1D0 0xC");        
    }
    else if (RTL9603CVD_CHIP_ID == chipId)    
    {
        //system("diag reg set 0x200 0x1f");
        system("diag reg set 0x40038 0x900");
        system("diag reg set 0x40c00 0x1140");
        system("diag reg set 0x4058c 0x1929");
        system("diag reg set 0x405A4 0x2a89");
        system("diag reg set 0x405c4 0x3032");
        system("diag reg set 0x405d0 0x8dad");
        system("diag reg set 0x405d4 0x13a4");
        system("diag reg set 0x405e8 0x822d");
        system("diag reg set 0x405ec 0x23a4");
        system("diag reg set 0x200 0xC");

        system("diag epon init");        
    }
#if defined(CONFIG_LUNA_G3_SERIES)    
    //For Cortina ARM-based 9607DQ series
    else if (RTL8277C_CHIP_ID == chipId)
    {
        system("killall -9 monitord omci_app eponoamd");    
        system("diag rt_ponmisc set force-prbs off");
        system("diag rt_ponmisc set mode 255 speed 0");        
        system("diag rt_ponmisc set mode 1 speed 0");
        system("diag rt_epon init");        
    }
#endif    
    else
    {
        printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
    }

    return 0;
}

void _europa_cal_short_data_set(uint8 *ptr_data, uint32 length, uint32 value)
{
    uint8  i, temp8;
    uint32 temp32;

    if ((length==0)||(length>4))
    {
         printf("Data Length Error!!!!!!!\n");
         return;
    }
    temp8 = 0;
    temp32 = value;
    for(i=0;i<length;i++)
    {
        *ptr_data = (uint8)(temp32>>(8*((length-1)-i)));
       // printf("%s %d: 0x%x!!!!!!!\n", __FUNCTION__, i, *ptr_data);
        ptr_data++;
    }
    return;
}


double _europa_dBm2uw(double dBm_pwr)
{
    double uW_pwr;

    uW_pwr = 1000*pow(10,dBm_pwr/10);
    
    return uW_pwr;
}

void _europa_cal_short_data_get(uint8 *ptr_data, uint32 length, uint32 *value)
{
    uint8  i, temp8;
    uint32 temp32;

    if ((length==0)||(length>4))
    {
         printf("Data Length Error!!!!!!!\n");
         return;
    }
    temp32 = 0;
    for(i=0;i<length;i++)
    {
        temp8 = *ptr_data;
        temp32 = temp32 | ((uint32)temp8<<(8*((length-1)-i)));
        ptr_data++;
    }

    //printf("%s = 0x%x!!!!!!!\n", __FUNCTION__, temp32);

    *value = temp32;
    return;
}


void _europa_cal_flash_data_get(uint32 address, uint32 length, uint32 *value)
{
    FILE *fp;
    uint8 *init_data, *ptr_data;
    int ret=0;
    
    fp = fopen(RTL8290B_FILE_LOCATION,"rb");
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n");
        return;
    }

    init_data = (uint8 *)malloc(RTL8290B_PARAMETER_SIZE*sizeof(uint8));

    ret=fseek(fp, 0, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp); 
        free(init_data);
        return;
    }
    memset(init_data, 0x0, sizeof(uint8)*RTL8290B_PARAMETER_SIZE);
    ret=fread(init_data, 1, RTL8290B_PARAMETER_SIZE, fp);
    //if (ret!=0)
    //{
    //    fclose(fp); 
    //    free(init_data);
    //    return;
    //}
    ptr_data = init_data;

    ptr_data = ptr_data + address;
    _europa_cal_short_data_get(ptr_data, length, value);

    free(init_data);
    fclose(fp);

    return;
}

void _europa_cal_flash_data_set(uint32 address, uint32 length, uint32 value)
{
    FILE *fp;
    int wfd = -1;
    uint8 *init_data, *ptr_data;
    uint32 i;
    uint8 output;
    int ret=0;

    fp = fopen(RTL8290B_FILE_LOCATION,"rb");
    if (NULL ==fp)
    {
        printf("Open %s error!!!!!!!\n", RTL8290B_FILE_LOCATION);
        return;
    }

    init_data = (uint8 *)malloc(RTL8290B_PARAMETER_SIZE*sizeof(uint8));

    ret=fseek(fp, 0, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp); 
        free(init_data);
        return;
    }
    memset(init_data, 0x0, sizeof(uint8)*RTL8290B_PARAMETER_SIZE);
    ret=fread(init_data, 1, RTL8290B_PARAMETER_SIZE, fp);
    //if (ret!=0)
    //{
    //    fclose(fp); 
    //    free(init_data);
    //    return;
    //}
    fclose(fp);

    ptr_data = init_data;
    ptr_data = ptr_data + address;
    _europa_cal_short_data_set(ptr_data, length, value);

    fp = fopen(RTL8290B_FILE_LOCATION,"wb");
    if (NULL ==fp)
    {
        printf("Open %s error!!!!!!!\n", RTL8290B_FILE_LOCATION);
        free(init_data);        
        return;
    }
    ret=fseek(fp, 0, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp); 
        free(init_data);
        return;
    }
    fwrite(init_data,sizeof(char), RTL8290B_PARAMETER_SIZE,fp);
        
    fflush(fp); 
    wfd = fileno(fp);
    if(wfd != -1)
    {
        fsync(wfd);
    }
        
    fclose(fp);
    free(init_data);
    return;
}

void _europa_cal_flash_array_set(uint32 address, uint32 length, uint8 *data)
{
    FILE *fp;
    int wfd = -1;
    uint8 *init_data, *ptr_data;
    uint32 i;
    uint8 output;
    int ret=0;

    fp = fopen(RTL8290B_FILE_LOCATION,"rb");
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n");
        return;
    }

    init_data = (uint8 *)malloc(RTL8290B_PARAMETER_SIZE*sizeof(uint8));

    ret=fseek(fp, 0, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp); 
        free(init_data);
        return;
    }
    memset(init_data, 0x0, sizeof(uint8)*RTL8290B_PARAMETER_SIZE);
    ret=fread(init_data, 1, RTL8290B_PARAMETER_SIZE, fp);
    //if (ret!=0)
    //{
    //    fclose(fp); 
    //    free(init_data);
    //    return;
    //}
    fclose(fp);

    ptr_data = init_data;
    ptr_data = ptr_data + address;

    memcpy(ptr_data, data, length);

    fp = fopen(RTL8290B_FILE_LOCATION,"wb");
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n");
        free(init_data);        
        return;
    }
    ret=fseek(fp, 0, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp); 
        free(init_data);
        return;
    }
    
    fwrite(init_data,sizeof(char), EUROPA_PARAMETER_SIZE,fp);

    fflush(fp); 
    wfd = fileno(fp);
    if(wfd != -1)
    {
        fsync(wfd);
    }
    
    fclose(fp);
    free(init_data);
    return;
}


void _europa_cal_flash_array_get(uint32 address, uint32 length, uint8 *data)
{
    FILE *fp;
    uint8 *init_data, *ptr_data;
    uint32 i;
    uint8 output;
    int ret = 0;

    fp = fopen(RTL8290B_FILE_LOCATION,"rb");
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n");
        return;
    }

    init_data = (uint8 *)malloc(RTL8290B_PARAMETER_SIZE*sizeof(uint8));

    ret=fseek(fp, 0, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp); 
        free(init_data);
        return ret;
    }
    memset(init_data, 0x0, sizeof(uint8)*RTL8290B_PARAMETER_SIZE);
    ret = fread(init_data, 1, RTL8290B_PARAMETER_SIZE, fp);
    //if (ret!=0)
    //{
    //    fclose(fp); 
    //    free(init_data);
    //    return ret;
    //}
    fclose(fp);

    ptr_data = init_data;
    ptr_data = ptr_data + address;

    memcpy(data, ptr_data, sizeof(uint8)*length);

    free(init_data);
    return;
}


/* given (X1,Y1) (X2,Y2), using linear fitting to get
 * coefficients a and b of the equation y = a x + b
 *
 * if (X2,Y2 = (0, 0), thne use y = a x only.
 *
 *    Xn are MPD current in nA
 *    Yn are known Rx power in nW */
uint32 _europa_cal_lin_regression(
    uint32  X1,
    uint32  Y1,
    uint32  X2,
    uint32  Y2,
    double    *A,
    double    *B)
{
    double x1, x2, x3, y1, y2, y3;
    double a, b, c;

    x1 = (double)X1/1000; /* nA to uA */
    y1 = (double)Y1/1000;
    x2 = (double)X2/1000;
    y2 = (double)Y2/1000; /* nW to uW */

    //
    // TODO : validation the input value ?
    //

    if (0==x2 && 0==y2)   /* single point data */
    {
       // avoid divide by zero
       if (0==x1)
       {
          printf("%s: (%f,%f) (%f, %f) fail.\n",  __FUNCTION__,
              x1, y2, x2, y2);
      *A = 0;
      *B = 0;
      return 1;
       }
       a = y1 / x1;
       b = 0;
    }
    else
    {
       // avoid divide by zero
       if (0==(x2-x1))
       {
          printf("%s: (%f,%f) (%f, %f) fail.\n",  __FUNCTION__,
              x1, y2, x2, y2);
      *A = 0;
      *B = 0;
      return 1;
       }
       a = (y2 - y1) / (x2 - x1);
       b = y2 - a * x2;
    }

    *A = a;
    *B = b;

    return 0;
}

/* given (X1,Y1) (X2,Y2) (X3,Y3), using polynomial fitting to get
 * coefficients a, b and c of the equation y = a x^2 + b x + c
 *    Xn are RSSI current in nA
 *    Yn are known Rx power in nW */
uint32 _europa_cal_regression(
    uint32  X1,
    uint32  Y1,
    uint32  X2,
    uint32  Y2,
    uint32  X3,
    uint32  Y3,
    double    *A,
    double    *B,
    double    *C)
{
    double x1, x2, x3, y1, y2, y3;
    double a, b, c;

    x1 = (double)X1/1000; /* nA to uA */
    y1 = (double)Y1/1000;
    x2 = (double)X2/1000;
    y2 = (double)Y2/1000; /* nW to uW */
    x3 = (double)X3/1000;
    y3 = (double)Y3/1000;

    //
    // TODO : validation the input value ?
    //

    a = (((y1*x2 - y2*x1)/(x1 - x2)) - ((y2*x3 - y3*x2)/(x2 - x3)) )/(x1*x2 - x2*x3);
    c = (((y1*x2 - y2*x1)/(x1 - x2)*x3) - ((y2*x3 - y3*x2)/(x2 - x3)*x1))/(x1 - x3);
    b = (y1 - a*x1*x1-c)/x1;


    *A = a;
    *B = b;
    *C = c;

    return 0;
}


/* given (X1,Y1) (X2,Y2) (X3,Y3), using polynomial fitting to get
 * coefficients a, b and c of the equation y = a x^2 + b x + c
 *    Xn are RSSI current in nA
 *    Yn are known Rx power in nW */
uint32 _europa_cal_quad_regression(
    uint32  X1,
    uint32  Y1,
    uint32  X2,
    uint32  Y2,
    uint32  X3,
    uint32  Y3,
    double    *A,
    double    *B,
    double    *C)
{
    double x1, x2, x3, y1, y2, y3;
    double a, b, c;

    x1 = (double)X1/1000; /* nA to uA */
    y1 = (double)Y1/1000;
    x2 = (double)X2/1000;
    y2 = (double)Y2/1000; /* nW to uW */
    x3 = (double)X3/1000;
    y3 = (double)Y3/1000;

    //
    // TODO : validation the input value ?
    //

    a = (((y1*x2 - y2*x1)/(x1 - x2)) - ((y2*x3 - y3*x2)/(x2 - x3)) )/(x1*x2 - x2*x3);
    c = (((y1*x2 - y2*x1)/(x1 - x2)*x3) - ((y2*x3 - y3*x2)/(x2 - x3)*x1))/(x1 - x3);
    b = (y1 - a*x1*x1-c)/x1;


    *A = a;
    *B = b;
    *C = c;

    return 0;
}


uint32 _europa_cal_get_mpd_current(void)
{
    int ret;
    uint32 i_mpd, mpd1;
    uint32 loopcnt, i;
    uint64 sum, sum_mpd;

    loopcnt = 10;

    sum = 0;
    sum_mpd = 0;

    for (i=0;i<loopcnt;i++)
    {
        ret = rtk_ldd_tx_power_get(0, &mpd1, &i_mpd);
        if(ret)
        {
            printf("Get MPD current fail!!! (%d)\n", ret);
            return -1;
        }
        sum += i_mpd;
        sum_mpd += mpd1;
    }

    i_mpd = sum/loopcnt;
    mpd1 = sum_mpd/loopcnt;

    return i_mpd*1000; /* uA to nA */
}

int _rssi_compare(const void *a, const void *b)
{
      uint32 c = *(uint32 *)a;
      uint32 d = *(uint32 *)b;
      if(c < d) {return -1;}
      else if (c == d) {return 0;}
      else return 1;
}

/*
 * Get RSSI Current (nA)
 */
uint32 _europa_cal_get_rssi_current(void)
{
    int ret;
    uint32 rssi_v0, i_rssi, v_rssi;
    uint32 loopcnt, i, cnt, j;
    uint64 sum_i, sum_v;
    uint64 sum2_i, sum2_v;
    uint32 i_rssi_2, i_rssi_3, i_rssi_4;
    uint32 w83_data;

    uint32 curr[EUROPA_RSSI_CURR_SIZE];

    loopcnt = EUROPA_RSSI_CURR_SIZE;

    _europa_cal_flash_data_get(RTL8290B_RSSI_V0_ADDR, 4, &rssi_v0);
    {
        if (0==rssi_v0)
        {
            printf("Invalid RSSI-V0, run get rssi-v0 first!\n");
            return -1;
        }
    }
    
    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);

    osal_time_mdelay(500);

#if 0
    sum_i = 0;
    sum_v = 0;
    for (i=0;i<loopcnt;i++)
    {
        sum2_i = 0;
        sum2_v = 0;
        for (j=0;j<5;j++)
        {
        ret = rtk_ldd_rx_power_get(rssi_v0, &v_rssi, &i_rssi);
        if(ret)
        {
            printf("Get RSSI current fail!!! (%d)\n", ret);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
            return -1;
        }
            sum2_i += i_rssi;
            sum2_v += v_rssi;
            //printf("Loop [%d]: I_RSSI = %d nA, V_RSSI = %d uV RSSI_V0 = %d\n", j, i_rssi, v_rssi, rssi_v0);        
        }
        i_rssi = sum2_i/5;
        v_rssi = sum2_v/5;
        printf("Loop[%d]: I_RSSI = %d nA, V_RSSI = %d uV RSSI_V0 = %d\n", i, i_rssi, v_rssi, rssi_v0);
        
        sum_i += i_rssi;
        sum_v += v_rssi;
    }

    i_rssi = sum_i/loopcnt;
    v_rssi = sum_v/loopcnt;
#endif

#if 0
    sum_i = 0;
    sum_v = 0;
    
    for (i=0;i<loopcnt;i++)
    {
        //printf("Loop %d: \n", i);
        ret = rtk_ldd_rx_power_get(rssi_v0, &v_rssi, &i_rssi);
        if(ret)
        {
            printf("Get RX power fail!!! (%d)\n", ret);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);                  
            return -1;
        }
        sum_i += i_rssi;
        sum_v += v_rssi;
    }
    
    i_rssi = sum_i/loopcnt;
    v_rssi = sum_v/loopcnt;  
#endif

    memset(curr, 0, sizeof(uint32)*EUROPA_RSSI_CURR_SIZE);
    for (i=0;i<loopcnt;i++)
    {
        //printf("Loop %d: \n", i);
        ret = rtk_ldd_rx_power_get(rssi_v0, &v_rssi, &i_rssi);
        if(ret)
        {
            printf("Get RX power fail!!! (%d)\n", ret);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);                  
            return -1;
        }
        curr[i] = i_rssi;
    }

    qsort((void *)curr, 100, sizeof(curr[0]), _rssi_compare);
    
    sum_i = 0;    
    for (i=EUROPA_RSSI_CURR_START;i<EUROPA_RSSI_CURR_END;i++)
    {
        //printf("curr[%d] = %d\n", i, curr[i]);
        sum_i += curr[i];        
    }
    i_rssi = sum_i/EUROPA_RSSI_CURR_LOOP;

    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);

    printf("Get RX power I_RSSI = %d nA, V_RSSI = %d uV RSSI_V0 = %d, (%d), (%d)\n", i_rssi, v_rssi, rssi_v0, curr[5], curr[94]);
    //printf("Get RX power I_RSSI = %d nA, I_RSSI2 = %d nA RSSI_V0 = %d\n", i_rssi, v_rssi, rssi_v0);

    return i_rssi;

}


//// FIXME  booster API are chip-dependent, should move to RTL8290_API ++
/* Convert Vout voltage to DAC value
 *
 *  Vout: desired output voltage (mV)
 *  Rfb1: resistor (K ohms)
 *  Rfb2: resistor (K ohms)
 */
uint32 _europa_cal_booster_Vout2Dac(uint32 Vout, uint32 Rfb1, uint32 Rfb2)
{
   uint32 dac_code;
   uint32 Vref = 1200; /* 1.2V */
   uint32 Vdefault;

   Vdefault = Vref*(1+Rfb1/Rfb2) + Rfb1*12;

   /* avoid overflow if Vout is lower than Vdefault */
   if (Vout<Vdefault)
   {
    dac_code = 0;
   }
   else
   {
    //dac_code = (Vout*128)/(Rfb1*100) - (Vref*128)/(Rfb1*100) - (Vref*128)/(Rfb2*100) - 12*128/100;
    dac_code = (Vout-Vref)*128/(Rfb1*100) - (Vref*128)/(Rfb2*100) - 12*128/100;
   }

   return dac_code;
}

/* Convert DAC value to Vout voltage
 *
 *  Rfb1: resistor (K ohms)
 *  Rfb2: resistor (K ohms)
 */
uint32 _europa_cal_booster_Dac2Vout(uint32 dac_code, uint32 Rfb1, uint32 Rfb2)
{
   uint32 Vout;
   uint32 Vref = 1200; /* 1.2V */

   Vout = Vref + Vref*Rfb1/Rfb2 + Rfb1*12 + (Rfb1*dac_code*78125)/100000;

   return Vout;
}

static int _europa_param_init()
{
    FILE *fp;
    int wfd = -1;
    int length, start_addr, file_len;
    unsigned int input;
    unsigned int i;
    int ret=0;
           
    fp = fopen(EUROPA_FILE_LOCATION, "wb");  

    if (NULL ==fp)
    {
        printf("Create file in /var/config/ error!!!!!!!\n"); 
        return -1;           
    } 
    
    ret=fseek(fp, 0, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp); 
        return ret;
    }
    fwrite(default_a0_reg, sizeof(unsigned char), sizeof(default_a0_reg),fp);
    fwrite(default_a2_reg, sizeof(unsigned char), sizeof(default_a2_reg),fp);
    fwrite(default_a4_reg, sizeof(unsigned char), sizeof(default_a4_reg),fp);    
    ret=fseek(fp, 0x300, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp); 
        return ret;
    }
    fwrite(default_apd_lut, sizeof(unsigned char), sizeof(default_apd_lut),fp);    
    ret=fseek(fp, 0x400, SEEK_SET);
	if (ret!=0)
    {
        fclose(fp); 
        return ret;
    }
    fwrite(default_laser_lut, sizeof(unsigned char), sizeof(default_laser_lut),fp);    
    fwrite(default_param, sizeof(unsigned char), sizeof(default_param),fp);  
    
    fflush(fp); 
    wfd = fileno(fp);
    if(wfd != -1)
    {
        fsync(wfd);
    }
    
    fclose(fp);
    printf("Create File Success!!!\n"); 

    return 0;    
}

static int _rtl8290b_param_to_flash_init(void)
{
    FILE *fp;
    int wfd = -1;
    int length, start_addr, file_len;
    unsigned int input;
    unsigned int i;
    int ret=0;
           
    fp = fopen(RTL8290B_FILE_LOCATION, "wb");  

    if (NULL ==fp)
    {
        printf("Create file in /var/config/ error!!!!!!!\n"); 
        return -1;           
    } 
    
    ret=fseek(fp, 0, SEEK_SET); 
    if (ret!=0)
    {
        fclose(fp); 
        return ret;
    }
    fwrite(rtl8290b_a0_reg, sizeof(unsigned char), sizeof(rtl8290b_a0_reg),fp);
    fwrite(rtl8290b_a2_reg, sizeof(unsigned char), sizeof(rtl8290b_a2_reg),fp);
    fwrite(rtl8290b_a4_reg, sizeof(unsigned char), sizeof(rtl8290b_a4_reg),fp);    
    ret=fseek(fp, 0x300, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp); 
        return ret;
    }
    fwrite(rtl8290b_apd_lut, sizeof(unsigned char), sizeof(rtl8290b_apd_lut),fp);    
    ret=fseek(fp, 0x400, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp); 
        return ret;
    }
    fwrite(rtl8290b_laser_lut, sizeof(unsigned char), sizeof(rtl8290b_laser_lut),fp);    
    fwrite(rtl8290b_default_param, sizeof(unsigned char), sizeof(rtl8290b_default_param),fp);   

    fflush(fp); 
    wfd = fileno(fp);
    if(wfd != -1)
    {
        fsync(wfd);
    }
    
    fclose(fp);
    printf("Create File Success!!!\n"); 

    return 0;    
}
void _europa_ibim_low(void)
{
    rtk_ldd_loop_mode_t loop_mode;
    uint32 data, w70_data;

    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 70, &w70_data); 
    data = w70_data&(~(0x18));
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 70, data);     

    loop_mode = LDD_LOOP_DOL;        
    rtk_ldd_loopMode_set(loop_mode);
    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 54, 0x04);      
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 55, 0x04);
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 56, 0); 
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 61, 0x42);
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 61, 0xC2);
    
    loop_mode = LDD_LOOP_DCL;        
    rtk_ldd_loopMode_set(loop_mode);

    return;
}

void _europa_ibim_recover(void)
{
    uint32 addr, value;
    uint32 ib, im, ibmin_en;
    rtk_ldd_loop_mode_t loop_mode;
    uint32 data, w70_data;

    loop_mode = LDD_LOOP_DOL;        
    rtk_ldd_loopMode_set(loop_mode);

    addr = RTL8290B_APC_IBIAS_INIT_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
    _europa_cal_flash_data_get(addr, 2, &ib); 
    printf("IBIAS = 0x%x\n", ib);
    addr = RTL8290B_APC_IMOD_INIT_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
    _europa_cal_flash_data_get(addr, 2, &im); 
    printf("IMOD = 0x%x\n", im);
    value = ((im&0xF)<<4)|(ib&0xF);
    
    addr = RTL8290B_APC_BIAS_MIN_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
    _europa_cal_flash_data_get(addr, 1, &ibmin_en); 
    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 54, (ib&0xFF0)>>4);    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 55, (im&0xFF0)>>4);
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 56, value); 
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 61, 0x42);
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 61, 0xC2);

    loop_mode = LDD_LOOP_DCL;        
    rtk_ldd_loopMode_set(loop_mode);
    
    if (ibmin_en == 0x1)
    {
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 70, &w70_data); 
        data = w70_data|(0x18);
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 70, data);     
    }    
    
    return;
}


void _europa_ibim_info(uint32 number)
{
    int ret;
    uint32 bias;
    uint32 mod;
    uint16 tempK;
    int32 tempC;

    uint32 data;
    int8 offset;

    double ib, im;

    bias = 0;
    ret = rtk_ldd_tx_bias_get(&bias);
    if(ret)
    {
        printf("Get TX Bias Fail!!! (%d)\n", ret);
        return;
    }

    ib = ((double)bias*2)/1000;  /* 2uA to mA */

    mod = 0;
    ret = rtk_ldd_tx_mod_get(&mod);
    if(ret)
    {
        printf("Get TX Mod Fail!!! (%d)\n", ret);
        return;
    }

    im = ((double)mod*2)/1000;  /* 2uA to mA */

    printf("\nNo.%d: Bias=%2.2f(0x%x) Modulation=%2.2f(0x%x)\n", number, ib, bias, im, mod);

#if 0
    tempK = 0;
    ret = rtk_ldd_temperature_get(&tempK);
    if(ret)
    {
        printf("Get Temperature Fail!!! (%d)\n", ret);
        return -1;
    }

    tempC = (tempK*100-27315)/100; /* degK to degC */


    _europa_cal_flash_data_get(RTL8290B_TEMP_OFFSET_ADDR, 1, &data);   
    offset = (int8)(data);

    tempC = tempC - offset;
    printf("BOSA Temperature = %dC \n", tempC);



    if(argc < 2)
    {
        printf("\n[%4d] Bias=%2.2f Modulation=%2.2f\n", 
            tempC, ib, im);
    }
    else
    {
        uint32 impd;
        uint8 loopmode;

        impd = _europa_cal_get_mpd_current();

        impd = impd / 1000;   /* nA to uA */

        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 124, &loopmode);
        
        loopmode = (loopmode&0xC0)>>6;

        printf("\n[%4d] Bias=%2.2f(0x%x) Modulation=%2.2f(0x%x) Impd=%4d loopmode=%d\n", 
        tempC, ib, bias, im, mod, impd, loopmode);
    }
#endif
    return;
}   




void _europa_ibim_set(void)
{
    int ret;
    uint32 bias;
    uint32 mod;

    uint32 addr , value;
    uint32 ib, im;
    uint16 tempK;
    uint32 regData1 , regData2;

    ret = rtk_ldd_tx_power_get(0x8290B1, &regData1, &regData2);
    //printf("\nregData1 = 0x%x  regData2 = 0x%x\n", regData1, regData2);
    if ((regData2&0x1)==0)
    {
         return;
    }

    bias = 0;
    ret = rtk_ldd_tx_bias_get(&bias);
    if(ret)
    {
        printf("Get TX Bias Fail!!! (%d)\n", ret);
        return;
    }

    ib = bias*2;  /* 2uA to uA */

    mod = 0;
    ret = rtk_ldd_tx_mod_get(&mod);
    if(ret)
    {
        printf("Get TX Mod Fail!!! (%d)\n", ret);
        return;
    }

    im = mod*2;  /* 2uA to uA */

    ret = rtk_ldd_temperature_get(&tempK);
    if(ret)
    {
        printf("Get Temperature Fail!!! (%d)\n", ret);
        return;
    }    

    //printf("\nSave Bias=%d uA (0x%x) Modulation=%d uA (0x%x) Temp = %d\n", ib, ib, im, im, tempK);

    addr = RTL8290B_CAL_IBIAS_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
    _europa_cal_flash_data_set(addr, 4, ib);

    addr = RTL8290B_CAL_IMOD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
    _europa_cal_flash_data_set(addr, 4, im);

    addr = RTL8290B_TX_TEMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
    _europa_cal_flash_data_set(addr, 2, tempK);

    return;
}   

void _europa_epon_ibim_set(void)
{
    int ret;
    uint32 bias;
    uint32 mod;

    uint32 addr , value;
    uint32 ib, im;
    uint32 regData1 , regData2;

    ret = rtk_ldd_tx_power_get(0x8290B2, &regData1, &regData2);
    //printf("\nregData1 = 0x%x  regData2 = 0x%x\n", regData1, regData2);    
    if ((regData2&0x800)==0)
    {
         return;
    }

    bias = 0;
    ret = rtk_ldd_tx_bias_get(&bias);
    if(ret)
    {
        printf("Get TX Bias Fail!!! (%d)\n", ret);
        return;
    }

    ib = bias*2;  /* 2uA to uA */

    mod = 0;
    ret = rtk_ldd_tx_mod_get(&mod);
    if(ret)
    {
        printf("Get TX Mod Fail!!! (%d)\n", ret);
        return;
    }

    im = mod*2;  /* 2uA to uA */

    //printf("\nSave EPON Bias=%d uA (0x%x) Modulation=%d uA (0x%x)\n", ib, ib, im, im);

    addr = RTL8290B_CAL_EPON_IBIAS_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
    _europa_cal_flash_data_set(addr, 4, ib);

    addr = RTL8290B_CAL_EPON_IMOD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
    _europa_cal_flash_data_set(addr, 4, im);

    return;
}   


static int _europa_rxpwr_get(uint32 rssi_v0, uint32 *rssi_i, int32 *rssi_v)
{
    int32 ret = -1;
    uint32 i;
    uint64 sum_i, sum_v;
    europa_rxpwr_t *p_rx_arr;
    uint32 v_rssi, i_rssi;

    p_rx_arr = (europa_rxpwr_t *)malloc(60*sizeof(europa_rxpwr_t));
    memset(p_rx_arr, 0, sizeof(europa_rxpwr_t)*60);

    for (i=0;i<60;i++)
    {
        ret = rtk_ldd_rx_power_get(rssi_v0, &v_rssi, &i_rssi);
        if(ret)
        {
            printf("Get RX power fail!!! (%d)\n", ret);
            free(p_rx_arr);
            return -1;
        }
        p_rx_arr[i].rssi_i = i_rssi;
        p_rx_arr[i].rssi_v = v_rssi;        
    }

    _europa_rxbubble_sort(p_rx_arr, 60);

    sum_i = 0;
    sum_v = 0;
    for (i=0;i<50;i++)
    {
        //printf("Loop %d: \n", i);
        sum_i += p_rx_arr[i+5].rssi_i;
        sum_v += p_rx_arr[i+5].rssi_v;
        //printf("Sort:i %d  %d \n", i, p_rx_arr[i+5].rssi_i, p_rx_arr[i+5].rssi_v);            
    }
    
    i_rssi = sum_i/50;
    v_rssi = sum_v/50;

    *rssi_i = i_rssi;
    *rssi_v = v_rssi;
    free(p_rx_arr);

    return 0;
}

static int _europa_rx_parameter_get(europa_rxparam_t *ldd_rxparam)
{
    int ret;
    rtk_ldd_cfg_t ldd_cfg;

    memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
    ldd_cfg.rssi_v0 = 0x8290B;
    ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
    if(ret)
    {
        printf("Get parameter Fail(%d)\n", ret);           
        return -1;
    }
    
    ldd_rxparam->rssi_v0 = ldd_cfg.rssi_v0;
    ldd_rxparam->vdd_v0 = ldd_cfg.vdd_code;
    ldd_rxparam->gnd_v0 = ldd_cfg.gnd_code;
    ldd_rxparam->half_v0 = ldd_cfg.half_vdd_code;
    ldd_rxparam->rssi_r1 = ldd_cfg.tx_bias;
    ldd_rxparam->rssi_r2 = ldd_cfg.tx_mod;    
    ldd_rxparam->rssi_k = (int)ldd_cfg.rssi_code;    
    ldd_rxparam->rx_a   = (int)ldd_cfg.ldo_code;
    ldd_rxparam->rx_b   = (int)ldd_cfg.rssi_voltage;
    ldd_rxparam->rx_c   = (int)ldd_cfg.rssi_voltage2;

    printf("Current RX power RSSI V0 = %d\n", ldd_cfg.rssi_v0);
    printf("Current RX power VDD V0 = %d\n", ldd_cfg.vdd_code);
    printf("Current RX power GND V0 = %d\n", ldd_cfg.gnd_code);
    printf("Current RX power Half VDD V0 = %d\n", ldd_cfg.half_vdd_code);
    printf("Current RX power R1 = %d\n", ldd_cfg.tx_bias); 
    printf("Current RX power R2 = %d\n", ldd_cfg.tx_mod); 
    printf("Current RX power RSSI-K = %d\n", (int)ldd_cfg.rssi_code);     
    printf("Current RX power rx_a = %d\n", ldd_rxparam->rx_a);   
    printf("Current RX power rx_b = %d\n", ldd_rxparam->rx_b); 
    printf("Current RX power rx_c = %d\n", ldd_rxparam->rx_c); 
    
    return 0;
}

static int _europa_rssi_voltage_get(europa_rxparam_t *ldd_rxparam)
{
    int ret;
    uint32 rssi_voltage1, rssi_voltage2;
    uint32 loopcnt1, i;
    uint32 sum, sum_volt2;
    rtk_ldd_cfg_t ldd_cfg;
    uint32 vdd_v0, gnd_v0, half_v0;
    int rx_a, rx_b, rx_c;    
    int rssi_k;
    uint32 rssi_v0;
    uint64 sum_half, sum_v, sum_v2, sum_rssi, sum_ldo, sum_rset, sum_gnd, sum_vdd, sum_i, sum_i2;
    uint32 rssi_r1, rssi_r2;    
    uint32 avg_half, avg_v2, avg_rssi, avg_ldo, avg_rset, avg_gnd, avg_vdd, avg_i, avg_i2;
    double tmp, tmp2;  
    uint32 rx_power, rx_power2;
    
    rssi_v0 = ldd_rxparam->rssi_v0;
    vdd_v0  = ldd_rxparam->vdd_v0;
    gnd_v0  = ldd_rxparam->gnd_v0;
    half_v0 = ldd_rxparam->half_v0;
    rssi_r1 = ldd_rxparam->rssi_r1;
    rssi_r2 = ldd_rxparam->rssi_r2;
    rssi_k  = ldd_rxparam->rssi_k;
    rx_a    = ldd_rxparam->rx_a;
    rx_b    = ldd_rxparam->rx_b;
    rx_c    = ldd_rxparam->rx_c;    
    
    //printf("_europa_rssi_voltage_get RSSI V0 = %d\n", rssi_v0);
    //printf("_europa_rssi_voltage_get VDD V0 = %d\n", vdd_v0);
    //printf("_europa_rssi_voltage_get GND V0 = %d\n", gnd_v0);
    //printf("_europa_rssi_voltage_get Half VDD V0 = %d\n", half_v0);
    //printf("_europa_rssi_voltage_get R1 = %d\n", rssi_r1); 
    //printf("_europa_rssi_voltage_get R2 = %d\n", rssi_r2); 
    //printf("_europa_rssi_voltage_get RSSI-K = %d\n", rssi_k);     
    //printf("_europa_rssi_voltage_get rx_a = %d\n", rx_a);   
    //printf("_europa_rssi_voltage_get rx_b = %d\n", rx_b); 
    //printf("_europa_rssi_voltage_get rx_c = %d\n", rx_c); 

    loopcnt1 = 50;
    
    rssi_voltage1 = 0;
    rssi_voltage2 = 0;      
    sum_half = 0;
    sum_v2   = 0;    
    sum_rssi = 0;
    sum_ldo  = 0;
    sum_rset = 0;
    sum_gnd  = 0;
    sum_vdd  = 0;
    sum_v    = 0;     
    sum_i    = 0;
    sum_i2   = 0;         
    for (i=0;i<loopcnt1 ;i++)
    {
        //printf("Loop %d: total = %d \n", i, loopcnt+offset);   
        memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
        ldd_cfg.vdd_code = vdd_v0;
        ldd_cfg.gnd_code = gnd_v0;    
        ldd_cfg.half_vdd_code = half_v0;  
        //enable debug mode 
        ldd_cfg.state = 0xF1;      
        ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
        if(ret)
        {
            printf("Get RSSI Voltage fail!!! (%d)\n", ret);         
            return -1;
        }
        //printf("%8d %8d %8d %8d %8d %8d %8d %8d\n", ldd_cfg.rssi_code, ldd_cfg.gnd_code, ldd_cfg.vdd_code, ldd_cfg.ldo_code, ldd_cfg.half_vdd_code, ldd_cfg.rset_code, ldd_cfg.rssi_voltage, ldd_cfg.rssi_voltage2);        
        //printf("%8d %8d %8d %8d\n", ldd_cfg.rssi_voltage, ldd_cfg.rssi_voltage2, ldd_cfg.tx_bias, ldd_cfg.tx_mod);          
        sum_half   += ldd_cfg.half_vdd_code;
        sum_rssi   += ldd_cfg.rssi_code;    
        sum_gnd    += ldd_cfg.gnd_code;
        sum_vdd    += ldd_cfg.vdd_code;
        sum_rset   += ldd_cfg.rset_code;
        sum_ldo    += ldd_cfg.ldo_code;
        sum_v2     += ldd_cfg.rssi_voltage2;
        sum_v      += ldd_cfg.rssi_voltage;
        sum_i      += ldd_cfg.tx_bias;
        sum_i2     += ldd_cfg.tx_mod;            
    }
    rssi_voltage2 = sum_v2/loopcnt1;
    rssi_voltage1 = sum_v/loopcnt1;        
    avg_half = sum_half/loopcnt1;
    avg_rssi = sum_rssi/loopcnt1;
    avg_ldo = sum_ldo/loopcnt1;
    avg_rset = sum_rset/loopcnt1; 
    avg_gnd = sum_gnd/loopcnt1;
    avg_vdd = sum_vdd/loopcnt1;
    avg_i = sum_i/loopcnt1;
    avg_i2 = sum_i2/loopcnt1;        

    memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
    ldd_cfg.rssi_v0 = 0x8290;    
    ldd_cfg.rssi_voltage2  = avg_i;
    ldd_cfg.tx_bias        = (uint32)rx_a;
    ldd_cfg.tx_mod           = (uint32)rx_b;
    ldd_cfg.driver_version = (uint32)rx_c;
    ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
    if(ret)
    {
        printf("Re-calculation fail!!! (%d)\n", ret);           
        return -1;
    }
    
    rx_power = ldd_cfg.rssi_voltage;
    tmp = __log10((double)(rx_power&0xFFFF)*1/10000)*10;    
    //printf("IRSSI = %d    RX power(0x%x): %f    ", avg_i, ldd_cfg.rssi_voltage, tmp);    

    memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
    ldd_cfg.rssi_v0 = 0x8290;    
    ldd_cfg.rssi_voltage2  = avg_i2;
    ldd_cfg.tx_bias        = (uint32)rx_a;
    ldd_cfg.tx_mod           = (uint32)rx_b;
    ldd_cfg.driver_version = (uint32)rx_c;
    ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
    if(ret)
    {
        printf("Re-calculation fail!!! (%d)\n", ret);           
        return -1;
    }
    
    rx_power2 = ldd_cfg.rssi_voltage;
    tmp2 = __log10((double)(rx_power2&0xFFFF)*1/10000)*10;    
    //printf("IRSSI = %d    RX power(0x%x): %f    ", avg_i2, ldd_cfg.rssi_voltage, tmp2);  

    printf(" %8d %8d %8d %8d %8d %8d %8d %5d %5d %8f(%3d)%8f(%3d)\n", avg_rssi, avg_gnd, avg_vdd, avg_ldo, avg_half, rssi_voltage1, rssi_voltage2, avg_i, avg_i2, tmp, rx_power, tmp2, rx_power2);    


    //printf("Average: %8d %8d %8d %8d %8d %8d %8d %8d %8d\n", avg_rssi, avg_gnd, avg_vdd, avg_ldo, avg_half, rssi_voltage1, rssi_voltage2, avg_i, avg_i2);           
    
    return 0;
}  

static int _europa_rssi_voltage2_get(europa_rxparam_t *ldd_rxparam)
{
    int ret;
    uint32 rssi_voltage1, rssi_voltage2;
    uint32 loopcnt1, i;
    uint32 sum, sum_volt2;
    rtk_ldd_cfg_t ldd_cfg;
    uint32 vdd_v0, gnd_v0, half_v0;
    int rx_a, rx_b, rx_c;    
    int rssi_k, rssi_k2;
    uint32 rssi_v0, rssi_v0_2;
    uint64 sum_half, sum_v, sum_v2, sum_rssi, sum_rssi2, sum_ldo, sum_rset, sum_gnd, sum_vdd, sum_i, sum_i2;
    uint32 rssi_r1, rssi_r2;    
    uint32 avg_half, avg_v2, avg_rssi, avg_rssi2, avg_ldo, avg_rset, avg_gnd, avg_vdd, avg_i, avg_i2;
    double tmp, tmp2;  
    uint32 rx_power, rx_power2;
    uint16 tempK1,tempK2;
    int32 tempC1,tempC2;
    
    rssi_v0 = ldd_rxparam->rssi_v0;
    vdd_v0  = ldd_rxparam->vdd_v0;
    gnd_v0  = ldd_rxparam->gnd_v0;
    half_v0 = ldd_rxparam->half_v0;
    rssi_r1 = ldd_rxparam->rssi_r1;
    rssi_r2 = ldd_rxparam->rssi_r2;
    rssi_k  = ldd_rxparam->rssi_k;
    rx_a    = ldd_rxparam->rx_a;
    rx_b    = ldd_rxparam->rx_b;
    rx_c    = ldd_rxparam->rx_c;  

    rssi_v0_2 = ldd_rxparam->rssi_v0_2;
    rssi_k2  = ldd_rxparam->rssi_k_2;    
    
    loopcnt1 = 50;
    
    rssi_voltage1 = 0;
    rssi_voltage2 = 0;      
    sum_half = 0;
    sum_v2   = 0;    
    sum_rssi = 0;
    sum_ldo  = 0;
    sum_rset = 0;
    sum_gnd  = 0;
    sum_vdd  = 0;
    sum_v    = 0;     
    sum_i    = 0;
    sum_i2   = 0;         
    sum_rssi2 = 0;    

    tempK1 = 0;
    ret = rtk_ldd_temperature_get(&tempK1);
    if(ret)
    {
        printf("Get Temperature Fail!!! (%d)\n", ret);
        return -1;
    }
    tempC1 = tempK1-273; /* degK to degC */
    
    for (i=0;i<loopcnt1 ;i++)
    {
        //printf("Loop %d: total = %d \n", i, loopcnt+offset);   
        memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
        ldd_cfg.vdd_code = vdd_v0;
        ldd_cfg.gnd_code = gnd_v0;    
        ldd_cfg.half_vdd_code = half_v0;  
        //enable debug mode 
        //ldd_cfg.state = 0xF2;    
        ldd_cfg.state = 0xF3;         
        ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
        if(ret)
        {
            printf("Get RSSI Voltage fail!!! (%d)\n", ret);         
            return -1;
        }
        //printf("%8d %8d \n", ldd_cfg.rssi_code, ldd_cfg.driver_version);        
        //printf("%8d %8d %8d %8d\n", ldd_cfg.rssi_voltage, ldd_cfg.rssi_voltage2, ldd_cfg.tx_bias, ldd_cfg.tx_mod);          
        sum_half   += ldd_cfg.half_vdd_code;
        sum_rssi   += ldd_cfg.rssi_code;    
        sum_gnd    += ldd_cfg.gnd_code;
        sum_vdd    += ldd_cfg.vdd_code;
        sum_rset   += ldd_cfg.rset_code;
        sum_ldo    += ldd_cfg.ldo_code;
        sum_v2     += ldd_cfg.rssi_voltage2;
        sum_v      += ldd_cfg.rssi_voltage;
        sum_i      += ldd_cfg.tx_bias;
        sum_i2     += ldd_cfg.tx_mod;  
        sum_rssi2   += ldd_cfg.driver_version;           
    }
    
    tempK2 = 0;
    ret = rtk_ldd_temperature_get(&tempK2);
    if(ret)
    {
        printf("Get Temperature Fail!!! (%d)\n", ret);
        return -1;
    }
    tempC2 = tempK2-273; /* degK to degC */    

    
    rssi_voltage2 = sum_v2/loopcnt1;
    rssi_voltage1 = sum_v/loopcnt1;        
    avg_half = sum_half/loopcnt1;
    avg_rssi = sum_rssi/loopcnt1;
    avg_rssi2 = sum_rssi2/loopcnt1;    
    avg_ldo = sum_ldo/loopcnt1;
    avg_rset = sum_rset/loopcnt1; 
    avg_gnd = sum_gnd/loopcnt1;
    avg_vdd = sum_vdd/loopcnt1;
    avg_i = sum_i/loopcnt1;
    avg_i2 = sum_i2/loopcnt1;        

    memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
    ldd_cfg.rssi_v0 = 0x8290;    
    ldd_cfg.rssi_voltage2  = avg_i;
    ldd_cfg.tx_bias        = (uint32)rx_a;
    ldd_cfg.tx_mod           = (uint32)rx_b;
    ldd_cfg.driver_version = (uint32)rx_c;
    ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
    if(ret)
    {
        printf("Re-calculation fail!!! (%d)\n", ret);           
        return -1;
    }
    
    rx_power = ldd_cfg.rssi_voltage;
    tmp = __log10((double)(rx_power&0xFFFF)*1/10000)*10;    
    //printf("IRSSI = %d    RX power(0x%x): %f    ", avg_i, ldd_cfg.rssi_voltage, tmp);    

    memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
    ldd_cfg.rssi_v0 = 0x8290;    
    ldd_cfg.rssi_voltage2  = avg_i2;
    ldd_cfg.tx_bias        = (uint32)rx_a;
    ldd_cfg.tx_mod           = (uint32)rx_b;
    ldd_cfg.driver_version = (uint32)rx_c;
    ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
    if(ret)
    {
        printf("Re-calculation fail!!! (%d)\n", ret);           
        return -1;
    }
    
    rx_power2 = ldd_cfg.rssi_voltage;
    tmp2 = __log10((double)(rx_power2&0xFFFF)*1/10000)*10;    
    //printf("IRSSI = %d    RX power(0x%x): %f    ", avg_i2, ldd_cfg.rssi_voltage, tmp2);  

    printf("%8d %8d %8d %8d %8d %8d %8d %8d %5d %5d %9f(%3d)%9f(%3d)%3d %3d\n", avg_rssi, avg_rssi2, avg_gnd, avg_vdd, avg_ldo, avg_half, rssi_voltage1, rssi_voltage2, avg_i, avg_i2, tmp, rx_power, tmp2, rx_power2,tempC1,tempC2);    


    //printf("Average: %8d %8d %8d %8d %8d %8d %8d %8d %8d\n", avg_rssi, avg_gnd, avg_vdd, avg_ldo, avg_half, rssi_voltage1, rssi_voltage2, avg_i, avg_i2);           
    
    return 0;
}  



static int _europa_rx_test_get(europa_rxparam_t *ldd_rxparam)
{
    int ret;
    uint32 rssi_voltage1, rssi_voltage2;
    uint32 loopcnt1, i;
    uint32 sum, sum_volt2;
    rtk_ldd_cfg_t ldd_cfg;
    uint32 vdd_v0, gnd_v0, half_v0;
    int rx_a, rx_b, rx_c;    
    int rssi_k;
    uint32 rssi_v0;
    uint64 sum_half, sum_v, sum_v2, sum_rssi, sum_ldo, sum_rset, sum_gnd, sum_vdd, sum_i, sum_i2;
    uint32 rssi_r1, rssi_r2;    
    uint32 avg_half, avg_v2, avg_rssi, avg_ldo, avg_rset, avg_gnd, avg_vdd, avg_i, avg_i2;
    double tmp, tmp2;  
    uint32 rx_power, rx_power2;
    uint64 sum_ldo_tx, sum_ldo_apc, sum_ldo_rxla, sum_ldo_lv;
    uint32 avg_ldo_tx, avg_ldo_apc, avg_ldo_rxla, avg_ldo_lv;
    uint32 ldo_tx_code, ldo_apc_code, ldo_rxla_code, ldo_lv_code, rset_code;
    uint64 sum_rssi2;
    uint32 avg_rssi2;
#ifdef RTL8290B_CAL_TIME
    struct  timeval     tv_tmp1, tv_tmp2;
    struct      timezone     tz;
    double Duration; 
#endif
    uint64 sum_v3, sum_i3;
    uint32 avg_i3, avg_v3;
    double tmp3;  
    
    rssi_v0 = ldd_rxparam->rssi_v0;
    vdd_v0  = ldd_rxparam->vdd_v0;
    gnd_v0  = ldd_rxparam->gnd_v0;
    half_v0 = ldd_rxparam->half_v0;
    rssi_r1 = ldd_rxparam->rssi_r1;
    rssi_r2 = ldd_rxparam->rssi_r2;
    rssi_k  = ldd_rxparam->rssi_k;
    rx_a    = ldd_rxparam->rx_a;
    rx_b    = ldd_rxparam->rx_b;
    rx_c    = ldd_rxparam->rx_c;    
    
    //loopcnt1 = 50;
    loopcnt1 = ldd_rxparam->loopcnt;
    
    rssi_voltage1 = 0;
    rssi_voltage2 = 0;      
    sum_half = 0;
    sum_v2   = 0;    
    sum_rssi = 0;
    sum_ldo  = 0;
    sum_rset = 0;
    sum_gnd  = 0;
    sum_vdd  = 0;
    sum_v    = 0;     
    sum_i    = 0;
    sum_i2   = 0;  
    sum_ldo_tx   = 0;
    sum_ldo_apc  = 0;
    sum_ldo_rxla = 0;
    sum_ldo_lv   = 0;
    sum_rset     = 0;
    sum_rssi2 = 0;   
    sum_v3    = 0;     
    sum_i3    = 0;
    
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp1,&tz); 
#endif    
    for (i=0;i<loopcnt1 ;i++)
    {
        //printf("Loop %d: total = %d \n", i, loopcnt+offset);   
        memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
        ldd_cfg.vdd_code = vdd_v0;
        ldd_cfg.gnd_code = gnd_v0;    
        ldd_cfg.half_vdd_code = half_v0;  
        ldd_cfg.state = ldd_rxparam->mode;
        ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
        if(ret)
        {
            printf("Get RSSI Voltage fail!!! (%d)\n", ret);         
            return -1;
        }

        if (ldd_rxparam->message==1)
        {
            if ((ldd_rxparam->mode == 0xF4)||(ldd_rxparam->mode == 0xF5))
            {
                printf("%8d %8d %8d %8d %8d %8d %8d %8d %5d %5d %5d\n", ldd_cfg.rssi_code, ldd_cfg.gnd_code, ldd_cfg.vdd_code, ldd_cfg.ldo_code, ldd_cfg.half_vdd_code, 
                ldd_cfg.rssi_voltage, ldd_cfg.rssi_voltage2, ldd_cfg.rx_ldo_code, ldd_cfg.tx_bias, ldd_cfg.tx_mod, ldd_cfg.adc_gnd_code);
            }
            else
            {
                printf("%8d %8d %8d %8d %8d %8d %8d %8d %8d %5d %5d\n", ldd_cfg.rssi_code, ldd_cfg.driver_version, ldd_cfg.gnd_code, ldd_cfg.vdd_code, ldd_cfg.ldo_code, ldd_cfg.half_vdd_code, ldd_cfg.rset_code, ldd_cfg.rssi_voltage, ldd_cfg.rssi_voltage2, ldd_cfg.tx_bias, ldd_cfg.tx_mod);     
            }
        }


        if ((ldd_rxparam->mode == 0xF4)||(ldd_rxparam->mode == 0xF5))
        {
            sum_rssi   += ldd_cfg.rssi_code; 
            sum_gnd    += ldd_cfg.gnd_code;
            sum_vdd    += ldd_cfg.vdd_code;
            sum_ldo    += ldd_cfg.ldo_code;
            sum_half   += ldd_cfg.half_vdd_code;
            sum_v      += ldd_cfg.rssi_voltage;  
            sum_v2     += ldd_cfg.rssi_voltage2;
            sum_v3     += ldd_cfg.rx_ldo_code;
            sum_i      += ldd_cfg.tx_bias;
            sum_i2     += ldd_cfg.tx_mod;  
            sum_i3     += ldd_cfg.adc_gnd_code;    
        }
        else
        {
        sum_half   += ldd_cfg.half_vdd_code;
        sum_rssi   += ldd_cfg.rssi_code;    
        sum_gnd    += ldd_cfg.gnd_code;
        sum_vdd    += ldd_cfg.vdd_code;
        sum_rset   += ldd_cfg.rset_code;
        sum_ldo    += ldd_cfg.ldo_code;
        sum_v2     += ldd_cfg.rssi_voltage2;
        sum_v      += ldd_cfg.rssi_voltage;
        sum_i      += ldd_cfg.tx_bias;
        sum_i2     += ldd_cfg.tx_mod;  
        sum_rssi2   += ldd_cfg.driver_version;   
        }
#if 0
        _europa_sdadc_code_get(EUROPA_SDADC_LDO_TX_CODE, &ldo_tx_code);
        _europa_sdadc_code_get(EUROPA_SDADC_LDO_APC_CODE, &ldo_apc_code);
        _europa_sdadc_code_get(EUROPA_SDADC_LDO_RXLA_CODE, &ldo_rxla_code);
        //_europa_sdadc_code_get(EUROPA_SDADC_LDO_LV_CODE, &ldo_lv_code);
        //_europa_sdadc_code_get(EUROPA_SDADC_RSET1_CODE, &rset_code);            
        //printf(" %8d %8d %8d %8d %8d %8d\n", ldo_tx_code, ldo_apc_code, ldo_rxla_code, ldo_lv_code, rset_code);        
        sum_ldo_tx   += ldo_tx_code;
        sum_ldo_apc  += ldo_apc_code;     
        sum_ldo_rxla += ldo_rxla_code;
        //sum_ldo_lv   += ldo_lv_code;
        //sum_rset     += rset_code;
#endif
        
    }

    if ((ldd_rxparam->mode == 0xF4)||(ldd_rxparam->mode == 0xF5))
    {
        avg_rssi = sum_rssi/loopcnt1; 
        avg_gnd = sum_gnd/loopcnt1;
        avg_vdd = sum_vdd/loopcnt1;
        avg_ldo = sum_ldo/loopcnt1;
        avg_half = sum_half/loopcnt1;
        rssi_voltage1 = sum_v/loopcnt1;
        rssi_voltage2 = sum_v2/loopcnt1;
        avg_v3 = sum_v3/loopcnt1;       
        avg_i = sum_i/loopcnt1;
        avg_i2 = sum_i2/loopcnt1;         
        avg_i3 = sum_i3/loopcnt1;    
    }
    else
    {
    rssi_voltage2 = sum_v2/loopcnt1;
    rssi_voltage1 = sum_v/loopcnt1;        
    avg_half = sum_half/loopcnt1;
    avg_rssi = sum_rssi/loopcnt1;
    avg_ldo = sum_ldo/loopcnt1;
    avg_rset = sum_rset/loopcnt1; 
    avg_gnd = sum_gnd/loopcnt1;
    avg_vdd = sum_vdd/loopcnt1;
    avg_i = sum_i/loopcnt1;
    avg_i2 = sum_i2/loopcnt1;        
    avg_rssi2 = sum_rssi2/loopcnt1;    
    }
    
    memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
    ldd_cfg.rssi_v0 = 0x8290;    
    ldd_cfg.rssi_voltage2  = avg_i;
    ldd_cfg.tx_bias        = (uint32)rx_a;
    ldd_cfg.tx_mod           = (uint32)rx_b;
    ldd_cfg.driver_version = (uint32)rx_c;
    ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
    if(ret)
    {
        printf("Re-calculation fail!!! (%d)\n", ret);           
        return -1;
    }
    rx_power = ldd_cfg.rssi_voltage;
    tmp = __log10((double)(rx_power&0xFFFF)*1/10000)*10;    
    //printf("IRSSI = %d    RX power(0x%x): %f    ", avg_i, ldd_cfg.rssi_voltage, tmp);    

    memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
    ldd_cfg.rssi_v0 = 0x8290;    
    ldd_cfg.rssi_voltage2  = avg_i2;
    ldd_cfg.tx_bias        = (uint32)rx_a;
    ldd_cfg.tx_mod           = (uint32)rx_b;
    ldd_cfg.driver_version = (uint32)rx_c;
    ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
    if(ret)
    {
        printf("Re-calculation fail!!! (%d)\n", ret);           
        return -1;
    }
    rx_power2 = ldd_cfg.rssi_voltage;
    tmp2 = __log10((double)(rx_power2&0xFFFF)*1/10000)*10;  
    
    if ((ldd_rxparam->mode == 0xF4)||(ldd_rxparam->mode == 0xF5))
    {
        memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
        ldd_cfg.rssi_v0 = 0x8290;     
        ldd_cfg.rssi_voltage2  = avg_i3;
        ldd_cfg.tx_bias        = (uint32)rx_a;
        ldd_cfg.tx_mod             = (uint32)rx_b;
        ldd_cfg.driver_version = (uint32)rx_c;
        ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
        if(ret)
        {
            printf("Re-calculation fail!!! (%d)\n", ret);            
            return -1;
        }    
        rx_power2 = ldd_cfg.rssi_voltage;
        tmp3 = __log10((double)(rx_power2&0xFFFF)*1/10000)*10;    
    }
    
    //printf("IRSSI = %d    RX power(0x%x): %f    ", avg_i2, ldd_cfg.rssi_voltage, tmp2);    

    //printf("Avg: %8d %8d %8d %8d %8d %8d %8d %5d %5d %8f(%3d)%8f(%3d)\n", avg_rssi, avg_gnd, avg_vdd, avg_ldo, avg_half, rssi_voltage1, rssi_voltage2, avg_i, avg_i2, tmp, rx_power, tmp2, rx_power2);    

    //printf("Average: %8d %8d %8d %8d %8d %8d %8d %8d %8d\n", avg_rssi, avg_gnd, avg_vdd, avg_ldo, avg_half, rssi_voltage1, rssi_voltage2, avg_i, avg_i2);           
#ifdef RTL8290B_CAL_TIME
    printf("Avg: %8d %8d %8d %8d %8d %8d %8d %8d %8d %5d %5d  %f\n", avg_rssi, avg_rssi2, avg_gnd, avg_vdd, avg_ldo, avg_half, avg_rset, rssi_voltage1, rssi_voltage2, avg_i, avg_i2, Duration);

    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  _europa_rx_test_get Duration = %f ms\n", Duration);
#else
    if ((ldd_rxparam->mode == 0xF4)||(ldd_rxparam->mode == 0xF5))
    {
        printf("Avg: %8d %8d %8d %8d %8d %8d %8d %8d %5d %5d %5d %f %f %f \n", avg_rssi, avg_gnd, avg_vdd, avg_ldo, avg_half, rssi_voltage1, rssi_voltage2, avg_v3, avg_i, avg_i2, avg_i3, tmp, tmp2, tmp3);
    }
    else
    {
    printf("Avg: %8d %8d %8d %8d %8d %8d %8d %8d %8d %5d %5d  \n", avg_rssi, avg_rssi2, avg_gnd, avg_vdd, avg_ldo, avg_half, avg_rset, rssi_voltage1, rssi_voltage2, avg_i, avg_i2);
    }
#endif 
    
    return 0;
}  



static int europa_cli_delete(
    int argc,
    char *argv[],
    FILE *fp)
{
#ifdef RTL8290B_CAL_TIME
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif

    if(argc >=2)
    {
        if(!strcmp(argv[1], "europa"))
        {
            printf("%s europa_cli_delete!!!!!!!\n", EUROPA_FILE_LOCATION);         
            unlink(EUROPA_FILE_LOCATION);  
        }
        else if(!strcmp(argv[1], "8290b"))
        {
            printf("%s europa_cli_delete!!!!!!!\n", RTL8290B_FILE_LOCATION);         
            unlink(RTL8290B_FILE_LOCATION); 
         
        }
        else if(!strcmp(argv[1], "8290c"))
        {
            uint32 devId = 5; 
        
            if(argc >= 3)
            {
                devId = atoi(argv[2]); 
            }
            _rtl8290c_flash_delete(devId);
        }        
        else if(!strcmp(argv[1], "8291"))
        {
            uint32 devId = 5; 
        
            if(argc >= 3)
            {
                devId = atoi(argv[2]); 
            }
            _rtl8291_flash_delete(devId);
        } 
        else
        {
            printf("Chip %s error! Chip should be europa/8290b/8290c/8291. \n", argv[1]);
            printf("Warnning!!! This command will remove all the flash data for europa/8290b/8290c/8291.\n");

        }
    }    
    else 
    {
        printf("%s <chip:europa/8290b/8290c/8291> \n", argv[0]);
        return -1;
    } 
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_delete Duration = %f ms\n", Duration);
#endif

    return 0;
}

static int europa_cli_flash_init(
    int argc,
    char *argv[],
    FILE *fp)
{
#ifdef RTL8290B_CAL_TIME
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 

    
    gettimeofday(&tv_tmp1,&tz); 
#endif
    if(argc >=2)
    {

        if(!strcmp(argv[1], "europa"))
        {
    
            fp = fopen(EUROPA_FILE_LOCATION,"rb");  
            
            if (NULL ==fp)
            {
                printf("%s does not exist. Create a new file in flash with default initial value.\n", EUROPA_FILE_LOCATION);       
                _europa_param_init(); 
            }    
            else
            {
                printf("%s is already exist. So do nothing and return...\n", EUROPA_FILE_LOCATION);
                printf("If you want to modify flash data, please use set to change it.\n");    
                printf("Or delete the flash data and do init again to restore the default value.\n");                
                fclose(fp);
        
            }
        }
        else if(!strcmp(argv[1], "8290b"))
        {
            fp = fopen(RTL8290B_FILE_LOCATION,"rb");  
            
            if (NULL ==fp)
            {
                printf("%s does not exist. Create a new file in flash with default initial value.\n", RTL8290B_FILE_LOCATION);       
                //printf("Load RTL8290B data to flash\n"); 
                _rtl8290b_param_to_flash_init();  
            }    
            else
            {
                printf("%s is already exist. So do nothing and return...\n", RTL8290B_FILE_LOCATION); 
                printf("If you want to modify flash data, please use set to change it.\n");    
                printf("Or delete the flash data and do init again to restore the default value.\n");                    
                fclose(fp);                
            }           
        }
        else if(!strcmp(argv[1], "8290c"))
        {
            uint32 devId = 5; 
            
            if(argc >= 3)
            {
                devId = atoi(argv[2]); 
            }
            
            if (devId == 0x5)
                fp = fopen(RTL8290C_DEVICE_0_FILE, "rb");  
            else if (devId == 0x6)
                fp = fopen(RTL8290C_DEVICE_1_FILE, "rb");  
            else if (devId == 0x7)
                fp = fopen(RTL8290C_DEVICE_2_FILE, "rb");  
            else if (devId == 0x8)
                fp = fopen(RTL8290C_DEVICE_3_FILE, "rb");  
            else if (devId == 0x9)
                fp = fopen(RTL8290C_DEVICE_4_FILE, "rb");  
            else
            {
                printf("devId = 0x%x, no such devId in RTL8290C.\n", devId);     
                return -1;    
            }

            if (NULL ==fp)
            {
                printf("RTL8290C_DEVICE_%d_FILE does not exist. Create a new file in flash with default initial value.\n", devId-5);       
                //printf("Load RTL8290B data to flash\n"); 
                _rtl8290c_flash_init(devId);  
            }    
            else
            {
                printf("RTL8290C_DEVICE_%d_FILE is already exist. So do nothing and return...\n", devId-5); 
                printf("If you want to modify flash data, please use set to change it.\n");    
                printf("Or delete the flash data and do init again to restore the default value.\n");                    
                fclose(fp);                
            }           
        }
        else if(!strcmp(argv[1], "8291"))
        {
            uint32 devId = 5; 

            if(argc >= 3)
            {
                devId = atoi(argv[2]); 
            }
        
            if (devId == 0x5)
                fp = fopen(RTL8291_DEVICE_0_FILE, "rb");  
            else if (devId == 0x6)
                fp = fopen(RTL8291_DEVICE_1_FILE, "rb");  
            else if (devId == 0x7)
                fp = fopen(RTL8291_DEVICE_2_FILE, "rb");  
            else if (devId == 0x8)
                fp = fopen(RTL8291_DEVICE_3_FILE, "rb");  
            else if (devId == 0x9)
                fp = fopen(RTL8291_DEVICE_4_FILE, "rb");  
            else
            {
                printf("devId = 0x%x, no such devId in RTL8291.\n", devId);     
                return -1;    
            }
            
            if (NULL ==fp)
            {
                printf("RTL8291_DEVICE_%d_FILE does not exist. Create a new file in flash with default initial value.\n", devId-5);       
                //printf("Load RTL8290B data to flash\n"); 
                _rtl8291_flash_init(devId);  
            }    
            else
            {
                printf("RTL8291_DEVICE_%d_FILE is already exist. So do nothing and return...\n", devId-5); 
                printf("If you want to modify flash data, please use set to change it.\n");    
                printf("Or delete the flash data and do init again to restore the default value.\n");                    
                fclose(fp);                
            }           
        }     
        else
        {
            printf("Chip %s error! Chip should be europa/8290b/8290c/8291. \n", argv[1]);

        }
    }    
    else 
    {
        printf("%s <chip:europa/8290b/8290c/8291> \n", argv[0]);
        return -1;
    } 
    
#ifdef RTL8290B_CAL_TIME   
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_flash_init Duration = %f ms\n", Duration);
#endif
    
    return 0;
}

static int europa_cli_param_open(
    int argc,
    char *argv[],
    FILE *fp)
{
    char *ptr; 
    unsigned int i; 

    fp = fopen(EUROPA_FILE_LOCATION,"rb");  

    if (NULL ==fp)
    {
        printf("europa.data does not exist. Create an empty file.\n");       
        fp = fopen(EUROPA_FILE_LOCATION,"wb");
        if (NULL ==fp)
        {
            printf("Create file in /var/config/ error!!!!!!!\n"); 
            return -1;           
        }            
    }    
    else
    {
        printf("File in /var/config/ is already exist!!!!!!!\n");  

    }
    fclose(fp);   

    return 0;
}

static int europa_cli_param_close(
    int argc,
    char *argv[],
    FILE *fp)
{
    
#if 0   
    fp = fopen("/var/config/europa.data","wb");
        
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n"); 
        return -1;           
    }        
    
    printf("europa_cli_param_close!!!!!!!\n");    
    
    fclose(fp);
#endif
    return 0;
}

static int europa_cli_param_clear(
    int argc,
    char *argv[],
    FILE *fp)
{
    //int length, start_addr;    
    //char *ptr; 
           
    printf("europa_cli_param_clear!!!!!!!\n");    
    
    fp = fopen(EUROPA_FILE_LOCATION, "wb");
    
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n"); 
        return -1;           
    }        
#if 0    
    if(argc >= 3)
    {
        start_addr = atoi(argv[1]); 
        length = atoi(argv[2]);
        
        printf("europa_cli_param_clear: length = %d, start address = %d\n", length, start_addr);       
              
        if ((length < 0) || (start_addr < 0))
        {
              printf("europa_cli_param_clear: length  or start address error!!!\n"); 
            return -1;
        }
        
        /* set the file pointer to start address */
        fseek(fp, start_addr, SEEK_SET); 
        ptr = malloc(length);
        
        /*Set all to 0*/
        memset(ptr, 0x0, sizeof(char)*length);
        fwrite(ptr,sizeof(char), length, fp);
    
        free(ptr);      

    }
    else if (argc<3)
    {
        printf("%s <start address> <length> \n", argv[0]);
        return -1;
    }      
#endif

    fclose(fp);
        
    printf("Clear Success!!!\n"); 

    return 0;
}

static int _europa_load_data(
    unsigned char *ptr,
    FILE *fp,
    int *file_len)
{
    int i, output;
    int ret=0;
    
    fp = fopen(EUROPA_FILE_LOCATION, "rb");  
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n"); 
        return -1;           
    }        
    ret=fseek(fp, 0, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp); 
        return ret;
    }
    memset(ptr, 0x0, sizeof(char)*EUROPA_PARAMETER_SIZE);  
    for(i = 0; i < EUROPA_PARAMETER_SIZE; i++)
    {
        output = fgetc(fp);
        *file_len = i;                    
        if (output == EOF)
        {
            printf("Original file length = %d\n", *file_len);
            break;    
        }
        else
        {
            *ptr = (unsigned char)(output&0xff);
            //printf("temp[%d] = 0x%x\n", i, *ptr);
        }
        ptr++;       
    }        
#if 0                     
        for(i = 0; i < EUROPA_PARAMETER_SIZE; i++)
        {
            file_len = i;
            if (feof(fp))
            {
                printf("file length = %d\n", i);
                break;  
            }
            else
            {            
                input=fgetc(fp);
                temp[i] = input&0xff;
                printf("temp[%d] = %x\n", i, temp[i]);
            }
        } 
            
#endif        
    fclose(fp); 
    
    return 0;   
}

static int _rtl8290b_load_data(
    unsigned char *ptr,
    FILE *fp,
    int *file_len)
{
    int i, output;
    int ret=0;

    fp = fopen(RTL8290B_FILE_LOCATION, "rb");  
    if (NULL ==fp)
    {
        printf("Open file %s error!!!!!!!\n", RTL8290B_FILE_LOCATION); 
        return -1;           
    }        
    ret=fseek(fp, 0, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp); 
        return ret;
    }
    memset(ptr, 0x0, sizeof(char)*RTL8290B_PARAMETER_SIZE);  
    for(i = 0; i < RTL8290B_PARAMETER_SIZE; i++)
    {
        output = fgetc(fp);
        *file_len = i;                    
        if (output == EOF)
        {
            printf("Original file length = %d\n", *file_len);
            break;    
        }
        else
        {
            *ptr = (unsigned char)(output&0xff);
            //printf("temp[%d] = 0x%x\n", i, *ptr);
        }
        ptr++;       
    }        
#if 0                     
        for(i = 0; i < EUROPA_PARAMETER_SIZE; i++)
        {
            file_len = i;
            if (feof(fp))
            {
                printf("file length = %d\n", i);
                break;  
            }
            else
            {            
                input=fgetc(fp);
                temp[i] = input&0xff;
                printf("temp[%d] = %x\n", i, temp[i]);
            }
        } 
            
#endif        
    fclose(fp); 
    
    return 0;    
}

static int europa_cli_flash_set(
    int argc,
    char *argv[],
    FILE *fp)
{
    //FILE *fp;
    int wfd = -1;
    int length, start_addr, file_len, value;
    unsigned int input;
    unsigned int i;
    unsigned char temp[RTL8290B_PARAMETER_SIZE];
    int offset;
    int ret=0;
    
    if(argc >= 4)
    {
        if(!strcmp(argv[1], "europa"))
        {
            start_addr = atoi(argv[2]);
            length = atoi(argv[3]); 
            
            printf("europa_cli_param_set: start address = %d length = %d\n", start_addr, length);        
                  
            if ((length < 0) || (start_addr < 0) || ((length+start_addr)>=RTL8290B_PARAMETER_SIZE))
            {
                printf("europa_cli_param_set: length    or start address error!!!\n"); 
                return -1;
            }
            if ((length+start_addr)>EUROPA_PARAMETER_SIZE)
            {
                printf("europa_cli_param_set: full size should less than 4KB!!!\n"); 
                return -1;
            }         
            
            _europa_load_data(temp,fp,&file_len);  
               
            printf("Please input europa parameters with length %d:\n", length); 
            
            for(i = start_addr; i < (length+start_addr); i++)
            {
                 ret=scanf("%x",&input);
                 if (ret<0)
                 {
                     return -1;
                 }
                 temp[i] = input&0xFF;
            }
            
            if (file_len<(length+start_addr))
                file_len = length+start_addr;
            
            fp = fopen(EUROPA_FILE_LOCATION, "wb");  
            
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", EUROPA_FILE_LOCATION); 
                return -1;             
            } 
            
            ret=fseek(fp, 0, SEEK_SET); 
            if (ret!=0)
            {
                fclose(fp); 
                return ret;
            }
            fwrite(temp,sizeof(char), file_len,fp);

            fflush(fp); 
            wfd = fileno(fp);
            if(wfd != -1)
            {
                fsync(wfd);
            }
    
            fclose(fp);
            printf("Input Success!!!\n"); 

        }
        else if(!strcmp(argv[1], "8290b"))
        {
            start_addr = _vlaue_translate(argv[2]);
            length = _vlaue_translate(argv[3]); 

            if ((length < 0) || (start_addr < 0) || ((length+start_addr)>=RTL8290B_PARAMETER_SIZE))
            {
                  printf("europa_cli_param_set: length    or start address error!!!\n"); 
                return -1;
            }
            if ((length+start_addr)>EUROPA_PARAMETER_SIZE)
            {
                  printf("europa_cli_param_set: full size should less than 4KB!!!\n"); 
                return -1;
            }         
            
            _rtl8290b_load_data(temp,fp,&file_len);  
               
            if(argc == 5)
            {
                if (length <= 4)
                {
                    value = _vlaue_translate(argv[4]);                    
                    printf("europa_cli_param_set: start address = %d length = %d value = 0x%x size of argv[4] = %d\n", start_addr, length, value, sizeof(argv[4]));
                    //memcpy(&temp[start_addr], &value, length);                
                    offset = length;
                    for(i=start_addr;i<(start_addr+length);i++)
                    {
                        offset--;
                        temp[i] = (value&(0xFF<<(offset*8)))>>(offset*8);                
                        //printf("temp[%d] = 0x%x offset = %d\n", i, temp[i], offset);
                    }
                }
                else
                {
                    printf("Input length error! In this mode, length should be 1, 2, or 4. \n");                
                }
            }
            else
            {         
                printf("europa_cli_param_set: start address = %d length = %d\n", start_addr, length);                                  
                printf("Please input europa parameters with length %d:\n", length); 
            
                for(i = start_addr; i < (length+start_addr); i++)
                {
                     ret = scanf("%x",&input);
                     if (ret<0)
                     { 
                         return ret;
                     }  
                     temp[i] = input&0xFF;
                }
            } 
            
            if (file_len<(length+start_addr))
                file_len = length+start_addr;
            
            fp = fopen(RTL8290B_FILE_LOCATION, "wb");  
            
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", RTL8290B_FILE_LOCATION); 
                return -1;             
            }             
            
            ret=fseek(fp, 0, SEEK_SET);
            if (ret!=0)
            {
                fclose(fp); 
                return ret;
            }
            fwrite(temp,sizeof(char), file_len,fp);

            fflush(fp); 
            wfd = fileno(fp);
            if(wfd != -1)
            {
                fsync(wfd);
            }
            
            fclose(fp);
            printf("Input Success!!!\n");          
        }
        else if(!strcmp(argv[1], "8290c"))
        {
            uint32 devId = 5;  
            
            start_addr = _vlaue_translate(argv[2]);
            length = _vlaue_translate(argv[3]); 

            if ((length < 0) || (start_addr < 0) || ((length+start_addr)>=RTL8290C_PARAMETER_SIZE))
            {
                printf("europa_cli_param_set: length or start address error!!!\n"); 
                return -1;
            }
            if ((length+start_addr)>RTL8290C_PARAMETER_SIZE)
            {
                printf("europa_cli_param_set: full size should less than 4KB!!!\n"); 
                return -1;
            }         
            
            _rtl8290c_load_data(temp,devId,&file_len);  
               
            if(argc == 5)
            {
                if (length <= 4)
                {
                    value = _vlaue_translate(argv[4]);                    
                    printf("europa_cli_param_set: start address = %d length = %d value = 0x%x size of argv[4] = %d\n", start_addr, length, value, sizeof(argv[4]));
                    //memcpy(&temp[start_addr], &value, length);                
                    offset = length;
                    for(i=start_addr;i<(start_addr+length);i++)
                    {
                        offset--;
                        temp[i] = (value&(0xFF<<(offset*8)))>>(offset*8);                
                        //printf("temp[%d] = 0x%x offset = %d\n", i, temp[i], offset);
                    }
                }
                else
                {
                    printf("Input length error! In this mode, length should be 1, 2, or 4. \n");                
                }
            }
            else
            {         
                printf("europa_cli_param_set: start address = %d length = %d\n", start_addr, length);                                  
                printf("Please input europa parameters with length %d:\n", length); 
            
                for(i = start_addr; i < (length+start_addr); i++)
                {
                     ret=scanf("%x",&input);
                     if (ret<0)
                     { 
                         return ret;
                     }                     
                     temp[i] = input&0xFF;
                }
            } 
            
            if (file_len<(length+start_addr))
                file_len = length+start_addr;
            
            _rtl8290c_save_data(temp,devId,&file_len);  

            printf("Input Success!!!\n");          
        }        
        else if(!strcmp(argv[1], "8291"))
        {
            uint32 devId = 5;  
            
            start_addr = _vlaue_translate(argv[2]);
            length = _vlaue_translate(argv[3]); 

            if ((length < 0) || (start_addr < 0) || ((length+start_addr)>=RTL8291_PARAMETER_SIZE))
            {
                printf("europa_cli_param_set: length or start address error!!!\n"); 
                return -1;
            }
            if ((length+start_addr)>RTL8291_PARAMETER_SIZE)
            {
                printf("europa_cli_param_set: full size should less than 4KB!!!\n"); 
                return -1;
            }         
            
            _rtl8291_load_data(temp,devId,&file_len);  
               
            if(argc == 5)
            {
                if (length <= 4)
                {
                    value = _vlaue_translate(argv[4]);                    
                    printf("europa_cli_param_set: start address = %d length = %d value = 0x%x size of argv[4] = %d\n", start_addr, length, value, sizeof(argv[4]));
                    //memcpy(&temp[start_addr], &value, length);                
                    offset = length;
                    for(i=start_addr;i<(start_addr+length);i++)
                    {
                        offset--;
                        temp[i] = (value&(0xFF<<(offset*8)))>>(offset*8);                
                        //printf("temp[%d] = 0x%x offset = %d\n", i, temp[i], offset);
                    }
                }
                else
                {
                    printf("Input length error! In this mode, length should be 1, 2, or 4. \n");                
                }
            }
            else
            {         
                printf("europa_cli_param_set: start address = %d length = %d\n", start_addr, length);                                  
                printf("Please input europa parameters with length %d:\n", length); 
            
                for(i = start_addr; i < (length+start_addr); i++)
                {
                     ret=scanf("%x",&input);
                     if (ret<0)
                     { 
                         return ret;
                     }
                     temp[i] = input&0xFF;
                }
            } 
            
            if (file_len<(length+start_addr))
                file_len = length+start_addr;
            
            _rtl8291_save_data(temp,devId,&file_len);  

            printf("Input Success!!!\n");          
        }        

        else
        {
            printf("Chip %s error! Chip should be europa/8290b/8290c/8291. \n", argv[1]);

        }
    }
    else if (argc<4)
    {
        printf("%s <chip:europa/8290b/8290c/8291> <start address> <length> \n", argv[0]);
        return -1;
    }  

    return 0;    
}

static int europa_cli_long_set(
    int argc,
    char *argv[],
    FILE *fp)
{
    //FILE *fp;
    int wfd = -1;    
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];
    int ret=0;
    if(argc >= 2)
    {
        start_addr = atoi(argv[1]);
        
        printf("europa_cli_long_set: start address = %d \n", start_addr);       
              
        if ((start_addr < 0) ||(start_addr>EUROPA_PARAMETER_SIZE))
        {
              printf("europa_cli_long_set: start address error!!!\n"); 
            return -1;
        }
#if 0        
        fp = fopen("/var/config/europa.data","rb");  
        if (NULL ==fp)
        {
            printf("Open file in /var/config/ error!!!!!!!\n"); 
            return -1;           
        }        
        fseek(fp, 0, SEEK_SET);  
        memset(temp, 0x0, sizeof(char)*EUROPA_PARAMETER_SIZE);  
        for(i = 0; i < EUROPA_PARAMETER_SIZE; i++)
        {
            input = fgetc(fp);
            file_len = i;                    
            if (input == EOF)
            {
                printf("file length = %d\n", file_len);
                break;    
            }
            else
            {
                temp[i] = input&0xff;
                printf("temp[%d] = 0x%x\n", i, temp[i]);
            }        
        }        
#if 0                     
        for(i = 0; i < EUROPA_PARAMETER_SIZE; i++)
        {
            file_len = i;
            if (feof(fp))
            {
               printf("file length = %d\n", i);
               break;  
            }                 
            else
            {            
                input=fgetc(fp);
                temp[i] = input&0xff;
                printf("temp[%d] = %x\n", i, temp[i]);
            }
        }          
            
#endif        
        fclose(fp);           
#endif

        _europa_load_data(temp,fp,&file_len);  
                      
        printf("The maximum file size is 4096. If you want to end the data inupt , please key in abcd.  \n");              
        printf("Please insert data:\n");       
       
        length = 0;
        for(i = start_addr; i < EUROPA_PARAMETER_SIZE; i++)
        {
            ret=scanf("%x",&input);
            if (ret!=0)
            {
                return -1;
            }
            if (0xabcd != input)
            { 
                length++;               
                temp[i] = input&0xFF;
            }
            else
            {
                     break;         
            }
        }
        printf("Input europa parameters with length %d:\n", length);         
 
        if (file_len<(length+start_addr))
            file_len = length+start_addr;
 

        fp = fopen(EUROPA_FILE_LOCATION, "wb");  
        if (NULL ==fp)
        {
            printf("Open file in /var/config/ error!!!!!!!\n"); 
            return -1;           
        }        
 
        ret=fseek(fp, 0, SEEK_SET);
        if (ret!=0)
        {
            fclose(fp); 
            return ret;
        }
        fwrite(temp,sizeof(char), file_len,fp);
 
        fflush(fp); 
        wfd = fileno(fp);
        if(wfd != -1)
        {
            fsync(wfd);
        }
        
        fclose(fp);
        printf("europa long update success!!!\n"); 

    }
    else if (argc<3)
    {
        printf("%s <start address> \n", argv[0]);
        return -1;
    }  

    return 0;    
}

static int _europa_cli_block_set( FILE *fp, int start_addr, unsigned int size)
{
    int wfd = -1;
    int file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];
    int ret=0;
    
    _europa_load_data(temp,fp,&file_len);  
           
    printf("The maximum file size is %d. If you want to end the data inupt less than maximum , please key in abcd.  \n", size);              
    printf("Please insert data:\n");       
       
    length = 0;
    for(i = start_addr; i < (start_addr + size); i++)
    {
        ret=scanf("%x",&input);
        if (ret!=0)
        {
            return -1;
        }
        if (0xabcd != input)
        { 
            length++;               
            temp[i] = input&0xFF;
        }
        else
        {
            break;         
        }
    }      
    printf("Input Europa  data with length %d:\n", length);         
 
    if (file_len<(length+start_addr))
    {
        file_len = length+start_addr;
    }

    fp = fopen(EUROPA_FILE_LOCATION, "wb");  
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n"); 
        return -1;           
    } 
      
    ret=fseek(fp, 0, SEEK_SET); 
    if (ret!=0)
    {
        fclose(fp); 
        return ret;
    }    
    fwrite(temp,sizeof(char), file_len,fp);
 
    fflush(fp); 
    wfd = fileno(fp);
    if(wfd != -1)
    {
        fsync(wfd);

    }
    
    fclose(fp);
        
    return 0;    
}


/* 
   set ASCII string type parameters in A0
   eg. A0/20~35 : Vendor Name
       A0/40~55 : Vendor Part Name
       A0/56~59 : Vendor Rev */
static int _europa_cli_string_set(FILE *fp, 
                          int start_addr, 
                  unsigned int size,
                  char *str)
{
    int ret;
    uint32 temp;
    uint32 europa_id;
    uint32 version;

    //printf("%s( %d, %d, \"%s\" )\n", __FUNCTION__, start_addr, size, str);

    europa_id = 0;
    europa_i2c_read(EUROPA_I2C_PORT, 0x55, 0x90, &temp);
    europa_id = (0xFF&temp)<<8;
    europa_i2c_read(EUROPA_I2C_PORT, 0x55, 0x91, &temp);
    europa_id = europa_id|(0xFF&temp);

    if (europa_id != 0x8290)
    {
        printf("Chip ID = 0x%x, ID FAIL!!! \n", europa_id); 
        return -1;  
    }

    europa_i2c_read(EUROPA_I2C_PORT, 0x55, 0x94, &version);

    if (version == 0)
    {
        fp = fopen(EUROPA_FILE_LOCATION, "rb+");  

    }
    else if (version == 1)
    {
        fp = fopen(RTL8290B_FILE_LOCATION, "rb+");  
    }
    else
    {
        printf("Chip Ver = 0x%x, Ver FAIL!!! \n", version); 
        return -1;  
    }

    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n"); 
        return -1;           
    } 
 
    ret = fseek(fp, start_addr, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp);
        return -1;
    }
    ret = fwrite((void*)str, sizeof(char), size, fp);

    fclose(fp);
        
    return 0;    
}

/* 
   get ASCII string type parameters in A0
       A0/56~59 : Vendor Rev */
static int _europa_cli_string_get(FILE *fp, 
                          int start_addr, 
                  unsigned int size,
                  char *str)
{
    uint32 temp;
    uint32 europa_id;
    uint32 version;
    int ret=0;

    europa_id = 0;
    europa_i2c_read(EUROPA_I2C_PORT, 0x55, 0x90, &temp);
    europa_id = (0xFF&temp)<<8;
    europa_i2c_read(EUROPA_I2C_PORT, 0x55, 0x91, &temp);
    europa_id = europa_id|(0xFF&temp);

    if (europa_id != 0x8290)
    {
        printf("Chip ID = 0x%x, ID FAIL!!! \n", europa_id); 
        return -1;  
    }

    europa_i2c_read(EUROPA_I2C_PORT, 0x55, 0x94, &version);

    if (version == 0)
    {
    fp = fopen(EUROPA_FILE_LOCATION, "rb");  

    }
    else if (version == 1)
    {
        fp = fopen(RTL8290B_FILE_LOCATION, "rb");  
    }
    else
    {
        printf("Chip Ver = 0x%x, Ver FAIL!!! \n", version); 
        return -1;  
    }
    
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n"); 
        return -1;           
    } 
 
    ret = fseek(fp, start_addr, SEEK_SET); 
    if (ret!=0)
    {
        fclose(fp);
        return -1;
    }
    ret=fread((void*)str, sizeof(char), size, fp);
    //if (ret!=0)
    //{
    //    fclose(fp);
    //    return -1;
    //} 
    fclose(fp);
        
    return 0;    
}

static int europa_cli_ddmi_a0_set(
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];

    start_addr = 0;
        
    printf("europa_cli_ddmi_a0_set: start address = %d \n", start_addr);       
              
     _europa_cli_block_set( fp, start_addr, EUROPA_BLOCK_SIZE);
     
    printf("europa DDMI A0 update success!!!\n");  

    return 0;    
}

static int europa_cli_ddmi_a2_set(
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];

    start_addr = 0x100;
        
    printf("europa_cli_ddmi_a2_set: start address = %d \n", start_addr);       
              
     _europa_cli_block_set( fp, start_addr, EUROPA_BLOCK_SIZE);
     
    printf("europa DDMI A2 update success!!!\n");  

    return 0;    
}

static int europa_cli_a4_set(
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];

    start_addr = 0x200;
        
    printf("europa_cli_ddmi_a4_set: start address = %d \n", start_addr);       
              
     _europa_cli_block_set( fp, start_addr, EUROPA_BLOCK_SIZE);

    printf("europa DDMI A4 update success!!!\n");  

    return 0;    
}

static int europa_cli_apd_lut_set(
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE == chip_mode)
    {                  
        start_addr = 0x300;
        _europa_cli_block_set(fp, start_addr, EUROPA_BLOCK_SIZE);
    }
    else if (EUROPA_LDD_8290C_MODE == chip_mode)
    {
        uint32 devId = 5;    
        start_addr = RTL8290C_APDLUT_ADDR_BASE;
        _8290c_cli_flashBlock_set(devId, start_addr, RTL8290C_FLASH_PAGE_SIZE);
    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        uint32 devId = 5;    
        start_addr = RTL8291_APDLUT_ADDR_BASE;
        _8291_cli_flashBlock_set(devId, start_addr, RTL8291_FLASH_PAGE_SIZE);
    }    
    else
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;         
    }
     
    printf("europa APD lookup table update success!!!\n");  

    return 0;    
}

static int europa_cli_laser_lut_set(
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];

    start_addr = 0x400;
        
    printf("europa_cli_laser_lut_set: start address = %d \n", start_addr);       
              
     _europa_cli_block_set( fp, start_addr, 304);
     
    printf("europa Laser lookup table update success!!!\n");  

    return 0;    
}

static int europa_cli_P0_set(
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];

    start_addr = 1346;
        
    printf("europa_cli_P0_set: start address = %d \n", start_addr);       
              
     _europa_cli_block_set( fp, start_addr, 1);
     
    printf("europa P0 update success!!!\n");  

    return 0;    
}

static int europa_cli_P1_set(
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];

    start_addr = 1347;
        
    printf("europa_cli_P1_set: start address = %d \n", start_addr);       
              
     _europa_cli_block_set( fp, start_addr, 1);
     
    printf("europa P1 update success!!!\n");  

    return 0;    
}

static int europa_cli_VendorName_set(
    int argc,
    char *argv[],
    FILE *fp)
{
#define VENDOR_NAME_SZ 16
    int start_addr, length;
    char buf[VENDOR_NAME_SZ+1];
    char temp[EUROPA_STRING_SIZE];
    int chip_mode, ret=0;

    chip_mode = _europa_ldd_parser();

    start_addr = 20;
    
    memset(buf, ' ', sizeof(buf));
    buf[VENDOR_NAME_SZ] = '\0';
    
    if (1==argc)
    {
        printf("Vendor Name : ");
        ret=scanf("%16s", temp);
        if (ret!=0)
        {
            return -1;
        }
    }
    else
    {
        //CID166916
        //strcpy(temp, argv[1]);
        printf("Input Error ! ");
        return -1;
    }
                
    //CID166359
    //length = (strlen(temp)>=VENDOR_NAME_SZ) ? VENDOR_NAME_SZ : strlen(temp);
    if (strlen(temp)>VENDOR_NAME_SZ)
    {
        printf("Input Error ! ");
        return -1;
    }
    //else
    //{
    //    length = strlen(temp);
    //}
    //memcpy(buf, temp, length);

    memcpy(buf, temp, VENDOR_NAME_SZ);
    
    printf("%s: start address = %d \n", __FUNCTION__, start_addr);    
        
    if (EUROPA_LDD_8290B_MODE == chip_mode)
    {                  
        _europa_cli_string_set( fp, start_addr, VENDOR_NAME_SZ, buf);
    }
    else if (EUROPA_LDD_8290C_MODE == chip_mode)
    {
        uint32 devId = 5;
        _8290c_cli_string_set(devId, start_addr, VENDOR_NAME_SZ, buf);
    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        uint32 devId = 5;
        _8291_cli_string_set(devId, start_addr, VENDOR_NAME_SZ, buf);
    }	
    else
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;         
    }

    printf("europa VendorName update success!!!\n");

    return 0;    
#undef VENDOR_NAME_SZ
}

static int europa_cli_VendorPN_set(
    int argc,
    char *argv[],
    FILE *fp)
{
#define VENDOR_PN_SZ 16
    int start_addr, length;
    unsigned char buf[VENDOR_PN_SZ+1];
    char temp[EUROPA_STRING_SIZE];
    int chip_mode, ret=0;

    chip_mode = _europa_ldd_parser();

    start_addr = 40;

    memset(buf, ' ', sizeof(buf));
    buf[VENDOR_PN_SZ] = '\0';

    if (1==argc)
    {
        printf("Part Number: ");
        ret=scanf("%16s", temp);
        if (ret!=0)
        {
            return -1;
        }
    }
    else
    {
        //CID167039
        //strcpy(temp, argv[1]);
        printf("Input Error ! ");
        return -1;
    }
            
    //CID166350
    //    length = (strlen(temp)>=VENDOR_PN_SZ) ? VENDOR_PN_SZ : strlen(temp);
    if (strlen(temp)>VENDOR_PN_SZ)
    {
        printf("Input Error ! ");
        return -1;
    }
    //else
    //{
    //    length = strlen(temp);
    //}
    //memcpy(buf, temp, length);
    
    memcpy(buf, temp, VENDOR_PN_SZ);
    
    printf("%s: start address = %d \n", __FUNCTION__, start_addr);       
              
    if (EUROPA_LDD_8290B_MODE == chip_mode)
    {                  
        _europa_cli_string_set( fp, start_addr, VENDOR_PN_SZ, buf);
    }
    else if (EUROPA_LDD_8290C_MODE == chip_mode)
    {
        uint32 devId = 5;
        _8290c_cli_string_set(devId, start_addr, VENDOR_PN_SZ, buf);
    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        uint32 devId = 5;
        _8291_cli_string_set(devId, start_addr, VENDOR_PN_SZ, buf);
    }	
    else
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;         
    }

    printf("europa VendorPartNumber update success!!!\n");  

    return 0;    
#undef VENDOR_PN_SZ
}

static int europa_cli_VendorRev_set(
    int argc,
    char *argv[],
    FILE *fp)
{
#define VENDOR_REV_SZ 4
    int start_addr, length;
    unsigned char buf[VENDOR_REV_SZ+1];
    char temp[EUROPA_STRING_SIZE];
    int chip_mode, ret=0;

    chip_mode = _europa_ldd_parser();

    start_addr = 56;

    memset(buf, ' ', sizeof(buf));
    buf[VENDOR_REV_SZ] = '\0';

    if (1==argc)
    {
        printf("Rev: ");
        ret=scanf("%4s", temp);
        if (ret!=0)
        {
            return -1;
        }           
    }
    else
    {
        //CID165131
        //strcpy(temp, argv[1]);
        printf("Input Error ! ");
        return -1;
    }
        
    //CID166363
    //length = (strlen(temp)>=VENDOR_REV_SZ) ? VENDOR_REV_SZ : strlen(temp);
    if (strlen(temp)>VENDOR_REV_SZ)
    {
        printf("Input Error ! ");
        return -1;
    }
    //else
    //{
    //    length = strlen(temp);
    //}
    //memcpy(buf, temp, length);

    memcpy(buf, temp, VENDOR_REV_SZ);

    printf("%s: start address = %d \n", __FUNCTION__, start_addr);       
              
    if (EUROPA_LDD_8290B_MODE == chip_mode)
    {                  
        _europa_cli_string_set( fp, start_addr, VENDOR_REV_SZ, buf);
    }
    else if (EUROPA_LDD_8290C_MODE == chip_mode)
    {
        uint32 devId = 5;
        _8290c_cli_string_set(devId, start_addr, VENDOR_REV_SZ, buf);
    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        uint32 devId = 5;
        _8291_cli_string_set(devId, start_addr, VENDOR_REV_SZ, buf);
    }	
    else
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;         
    }
   
    printf("europa VendorRev update success!!!\n");  

    return 0;    
#undef VENDOR_REV_SZ
}

static int europa_cli_VendorSN_set(
    int argc,
    char *argv[],
    FILE *fp)
{
#define VENDOR_SN_SZ 16
    int start_addr, length;
    unsigned char buf[VENDOR_SN_SZ+1];
    char temp[EUROPA_STRING_SIZE];
    int chip_mode, ret=0;

    chip_mode = _europa_ldd_parser();

    start_addr = 68;

    memset(buf, ' ', sizeof(buf));
    buf[VENDOR_SN_SZ] = '\0';

    if (1==argc)
    {
        printf("Serial Number: ");
        ret=scanf("%16s", temp);
        if (ret!=0)
        {
            return -1;
        }
    }
    else
    {
        //CID165736
        //strcpy(temp, argv[1]);
        printf("Input Error ! ");
        return -1;
    }
        
    //CID165123
    //length = (strlen(temp)>=VENDOR_SN_SZ) ? VENDOR_SN_SZ : strlen(temp);
    if (strlen(temp)>VENDOR_SN_SZ)
    {
        printf("Input Error ! ");
        return -1;
    }
    //else
    //{
    //    length = strlen(temp);
    //}
    //memcpy(buf, temp, length);

    memcpy(buf, temp, VENDOR_SN_SZ);
        
    printf("%s: start address = %d \n", __FUNCTION__, start_addr);       
    
    if (EUROPA_LDD_8290B_MODE == chip_mode)
    {                  
        _europa_cli_string_set( fp, start_addr, VENDOR_SN_SZ, buf);
    }
    else if (EUROPA_LDD_8290C_MODE == chip_mode)
    {
        uint32 devId = 5;
        _8290c_cli_string_set(devId, start_addr, VENDOR_SN_SZ, buf);
    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        uint32 devId = 5;
        _8291_cli_string_set(devId, start_addr, VENDOR_SN_SZ, buf);
    }	
    else
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;         
    }

    printf("europa VendorSerialNumber update success!!!\n");  

    return 0;    
#undef VENDOR_SN_SZ
}

static int europa_cli_DateCode_set(
    int argc,
    char *argv[],
    FILE *fp)
{
#define DATE_CODE_SZ 8
    int start_addr, length;
    unsigned char buf[DATE_CODE_SZ+1];
    char temp[EUROPA_STRING_SIZE];
    int chip_mode,ret=0;

    chip_mode = _europa_ldd_parser();

    start_addr = 84;

    memset(buf, ' ', sizeof(buf));
    buf[DATE_CODE_SZ] = '\0';

    if (1==argc)
    {
        printf("Date Code: ");
        ret=scanf("%8s", temp);
        if (ret!=0)
        {
            return -1;
        }
    }
    else
    {
       //CID164593
       //strcpy(temp, argv[1]);
       printf("Input Error ! ");
       return -1;
    }
    
    //CID164322
    //length = (strlen(temp)>=DATE_CODE_SZ) ? DATE_CODE_SZ : strlen(temp);
    if (strlen(temp)>DATE_CODE_SZ)
    {
        printf("Input Error ! ");
        return -1;
    }
    //else
    //{
    //    length = strlen(temp);
    //}
    //memcpy(buf, temp, length);

    memcpy(buf, temp, DATE_CODE_SZ);    
        
    printf("%s: start address = %d \n", __FUNCTION__, start_addr);       
              
    if (EUROPA_LDD_8290B_MODE == chip_mode)
    {                  
        _europa_cli_string_set( fp, start_addr, DATE_CODE_SZ, buf);
    }
    else if (EUROPA_LDD_8290C_MODE == chip_mode)
    {
        uint32 devId = 5;
        _8290c_cli_string_set(devId, start_addr, DATE_CODE_SZ, buf);
    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        uint32 devId = 5;
        _8291_cli_string_set(devId, start_addr, DATE_CODE_SZ, buf);
    }	
    else
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;         
    }     
    printf("europa Date Code update success!!!\n");  

    return 0;    
#undef DATE_CODE_SZ
}

static int europa_cli_all_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    //FILE *fp;
    int length, start_addr;
    int output,i;
    int ret=0;

    if(argc >=2)
    {

        if(!strcmp(argv[1], "europa"))
        {
    
            printf("europa flash data:\n");       
                     
            fp = fopen(EUROPA_FILE_LOCATION, "rb");  
    
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", EUROPA_FILE_LOCATION); 
                return -1;           
            }
            
            /* set the file pointer to start address */
            ret=fseek(fp, 0, SEEK_SET); 
            if (ret!=0)
            {
                fclose(fp); 
                return ret;
            }            
            for(i=0;i<EUROPA_PARAMETER_SIZE;i++)
            {
                  output = fgetc(fp);
                if (output == EOF)
                {
                    break;    
                }
                else
                {
                    printf("0x%02x ", output);
                    if(i%8 == 7)
                        printf("   ");
                    if(i%16 == 15)
                        printf("\n");                    
                }
            }
            printf("\n");    
            fclose(fp);       
            printf("\nOutput Success!!!\n");
        }
        else if(!strcmp(argv[1], "8290b"))
        {
            printf("RTL8290B flash data:\n");       
                     
            fp = fopen(RTL8290B_FILE_LOCATION, "rb");  
    
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", RTL8290B_FILE_LOCATION); 
                return -1;           
            }
            
            /* set the file pointer to start address */
            ret=fseek(fp, 0, SEEK_SET); 
            if (ret!=0)
            {
                fclose(fp); 
                return ret;
            }            
            for(i=0;i<RTL8290B_PARAMETER_SIZE;i++)
            {
                  output = fgetc(fp);
                if (output == EOF)
                {
                    break;    
                }
                else
                {
                    printf("0x%02x ", output);
                    if(i%8 == 7)
                        printf("   ");
                    if(i%16 == 15)
                        printf("\n");                    
                }
            }
            printf("\n");    
            fclose(fp);       
            printf("\nOutput Success!!!\n");          
        }
        else
        {
            printf("Chip %s error! Chip should be europa or 8290b. \n", argv[1]);

        }
    }    
    else 
    {
        printf("%s <chip:europa or 8290b> \n", argv[0]);
        return -1;
    } 


#if 0
    if(argc >= 1)
    {  
        printf("europa_cli_all_get:\n");       
                 
        fp = fopen(EUROPA_FILE_LOCATION, "rb");  

        if (NULL ==fp)
        {
            printf("Open file in /var/config/ error!!!!!!!\n"); 
            return -1;           
        }
        
        /* set the file pointer to start address */
        fseek(fp, 0, SEEK_SET); 
        
        for(i=0;i<EUROPA_PARAMETER_SIZE;i++)
        {
              output = fgetc(fp);
            if (output == EOF)
            {
                break;    
            }
            else
            {
                printf("0x%02x ", output);
                if(i%8 == 7)
                    printf("   ");
                if(i%16 == 15)
                    printf("\n");                    
            }
        }
        printf("\n");    
        fclose(fp);       
        printf("\nOutput Success!!!\n"); 

    }
#endif 

    return 0; 
}

static int europa_cli_flash_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    //FILE *fp;
    int length, start_addr;
    int output,i;
    int ret=0;
  
    if(argc >= 4)
    {
        if(!strcmp(argv[1], "europa"))
        {
            start_addr = atoi(argv[2]); 
            length = atoi(argv[3]);
            
            printf("europa_cli_param_get: length = %d, start address = %d\n", length, start_addr);         
                  
            if ((length < 0) || (start_addr < 0))
            {
                  printf("europa_cli_param_get: length    or start address error!!!\n"); 
                return -1;
            }
            
            fp = fopen(EUROPA_FILE_LOCATION, "rb");  
            
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", EUROPA_FILE_LOCATION); 
                return -1;             
            }
            
            /* set the file pointer to start address */
            ret=fseek(fp, start_addr, SEEK_SET); 
            if (ret!=0)
            {
                fclose(fp);
                return ret;
            }
            for(i=0;i<length;i++)
            {
                output = fgetc(fp);
                if (output == EOF)
                {
                    break;      
                }
                else
                {
                    printf("0x%02x ", output);
                    if(i%8 == 7)
                        printf("   ");
                    if(i%16 == 15)
                        printf("\n");                     
                }
            }
            
            fclose(fp);
            printf("\n");     
            printf("\nOutput Success!!!\n");

        }
        else if(!strcmp(argv[1], "8290b"))
        {

            start_addr = atoi(argv[2]); 
            length = atoi(argv[3]);
       
            printf("RTL8290B: length = %d, start address = %d\n", length, start_addr);       
                  
            if ((length < 0) || (start_addr < 0))
            {
                  printf("RTL8290B: length  or start address error!!!\n"); 
                return -1;
            }
            
            fp = fopen(RTL8290B_FILE_LOCATION, "rb");  
    
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", RTL8290B_FILE_LOCATION); 
                return -1;           
            }
            
            /* set the file pointer to start address */
            ret=fseek(fp, start_addr, SEEK_SET); 
            if (ret!=0)
            {
                fclose(fp); 
                return ret;
            }           
            for(i=0;i<length;i++)
            {
                  output = fgetc(fp);
                if (output == EOF)
                {
                    break;    
                }
                else
                {
                    printf("0x%02x ", output);
                    if(i%8 == 7)
                        printf("   ");
                    if(i%16 == 15)
                        printf("\n");                    
                }
            }
    
            fclose(fp);
            printf("\n");    
            printf("\nOutput Success!!!\n"); 
        
        }
        else if(!strcmp(argv[1], "8290c"))
        {
            uint32 devId = 5;  
            
            start_addr = _vlaue_translate(argv[2]);
            length = _vlaue_translate(argv[3]); 

            printf("RTL8290C: length = %d, start address = %d\n", length, start_addr); 

            if ((length < 0) || (start_addr < 0) || ((length+start_addr)>=RTL8290C_PARAMETER_SIZE))
            {
                printf("europa_cli_param_set: length or start address error!!!\n"); 
                return -1;
            }
            if ((length+start_addr)>RTL8290C_PARAMETER_SIZE)
            {
                printf("europa_cli_param_set: full size should less than 4KB!!!\n"); 
                return -1;
            }         

            fp = fopen(RTL8290C_DEVICE_0_FILE, "rb");              
    
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", RTL8290C_DEVICE_0_FILE); 
                return -1;           
            }
            
            /* set the file pointer to start address */
            ret=fseek(fp, start_addr, SEEK_SET); 
            if (ret!=0)
            {
                fclose(fp); 
                return ret;
            }           
            for(i=0;i<length;i++)
            {
                output = fgetc(fp);
                if (output == EOF)
                {
                    break;    
                }
                else
                {
                    printf("0x%02x ", output);
                    if(i%8 == 7)
                        printf("   ");
                    if(i%16 == 15)
                        printf("\n");                    
                }
            }
    
            fclose(fp);
            printf("\n");    
            printf("\nOutput Success!!!\n"); 
        
        }        
        else if(!strcmp(argv[1], "8291"))
        {
            uint32 devId = 5;  
            
            start_addr = _vlaue_translate(argv[2]);
            length = _vlaue_translate(argv[3]); 

            printf("RTL8291: length = %d, start address = %d\n", length, start_addr); 

            if ((length < 0) || (start_addr < 0) || ((length+start_addr)>=RTL8290C_PARAMETER_SIZE))
            {
                printf("europa_cli_param_set: length or start address error!!!\n"); 
                return -1;
            }
            if ((length+start_addr)>RTL8291_PARAMETER_SIZE)
            {
                printf("europa_cli_param_set: full size should less than 4KB!!!\n"); 
                return -1;
            }         

            fp = fopen(RTL8291_DEVICE_0_FILE, "rb");              
    
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", RTL8291_DEVICE_0_FILE); 
                return -1;           
            }
            
            /* set the file pointer to start address */
            fseek(fp, start_addr, SEEK_SET); 
            
            for(i=0;i<length;i++)
            {
                output = fgetc(fp);
                if (output == EOF)
                {
                    break;    
                }
        else
        {
                    printf("0x%02x ", output);
                    if(i%8 == 7)
                        printf("   ");
                    if(i%16 == 15)
                        printf("\n");                    
                }
            }
    
            fclose(fp);
            printf("\n");    
            printf("\nOutput Success!!!\n"); 
        
        }    
        else
        {
            printf("Chip %s error! Chip should be europa/8290b/8290c/8291. \n", argv[1]);

        }
    }
    else if (argc<4)
    {
        printf("%s <chip: europa/8290b/8290c/8291> <start address> <length>\n", argv[0]);
        return -1;
    }  

    return 0; 
}

static int _europa_cli_block_get(FILE *fp, int start_addr, int length)
{
    int output,i;
    int ret=0;
                      
    fp = fopen(EUROPA_FILE_LOCATION, "rb");  

    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n"); 
        return -1;           
    }

    /* set the file pointer to start address */
    ret=fseek(fp, start_addr, SEEK_SET); 
    if (ret!=0)
    {
        fclose(fp);
        return ret;
    }        
    for(i=0;i<length;i++)
    {
        output = fgetc(fp);
        if (output == EOF)
        {
            break;    
        }
        else
        {
            printf("0x%02x ", output);
            if(i%8 == 7)
                printf("   ");
            if(i%16 == 15)
                printf("\n");                    
        }
    }

    fclose(fp);
    printf("\n");    

    return 0; 
}


static int europa_cli_ddmi_a0_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];

    start_addr = 0;
        
    printf("europa_cli_ddmi_a0_get: start address = %d \n", start_addr);       
              
     _europa_cli_block_get( fp, start_addr, EUROPA_BLOCK_SIZE);
     
    printf("europa DDMI A0 get success!!!\n");  

    return 0;    
}

static int europa_cli_ddmi_a2_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];
    
    start_addr = 0x100;
            
    printf("europa_cli_ddmi_a2_get: start address = %d \n", start_addr);       
                  
     _europa_cli_block_get( fp, start_addr, EUROPA_BLOCK_SIZE);
         
    printf("europa DDMI A2 get success!!!\n");    

    return 0;    
}

static int europa_cli_a4_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];

    start_addr = 0x200;
        
    printf("europa_cli_ddmi_a4_get: start address = %d \n", start_addr);       
              
     _europa_cli_block_get( fp, start_addr, EUROPA_BLOCK_SIZE);

    printf("europa DDMI A4 get success!!!\n");  

    return 0;    
}

static int europa_cli_apd_lut_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret = 0;
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if (EUROPA_LDD_8290B_MODE == chip_mode)
    {                  
    start_addr = 0x300;
    
    printf("europa_cli_apd_lut_get: start address = %d \n", start_addr);     
    
     _europa_cli_block_get( fp, start_addr, EUROPA_BLOCK_SIZE);
        
    printf("europa APD lookup table get success!!!\n");  

    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        uint32 devId = 5;    
        ret = rtl8291_cli_apdlut_get(devId, argc, argv, fp);
        return ret;
    }    
    else
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;         
    }

    return 0;    
}

static int europa_cli_laser_lut_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];

    start_addr = 0x400;
        
    printf("europa_cli_laser_lut_get: start address = %d \n", start_addr);       
              
     _europa_cli_block_get( fp, start_addr, 304);
     
    printf("europa Laser lookup table get success!!!\n");  

    return 0;    
}

static int europa_cli_P0P1_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];

    start_addr = 1346;
        
    printf("europa_cli_P0P1_get: start address = %d \n", start_addr);       
              
     _europa_cli_block_get( fp, start_addr, 2);
     
    printf("europa P0 P1 get  success!!!\n");  

    return 0;    
}

static int europa_cli_VendorName_get(
    int argc,
    char *argv[],
    FILE *fp)
{
#define VENDOR_NAME_SZ 16
    int start_addr;
    char buf[VENDOR_NAME_SZ+1];
    int chip_mode;
    int ret = 0;

    chip_mode = _europa_ldd_parser();

    start_addr = 20;
  
    if (EUROPA_LDD_8290B_MODE == chip_mode)
    {                  
        ret = _europa_cli_string_get( fp, start_addr, VENDOR_NAME_SZ, buf);
    }
    else if (EUROPA_LDD_8290C_MODE == chip_mode)
    {
        uint32 devId = 5;
        ret = _8290c_cli_string_get(devId, start_addr, VENDOR_NAME_SZ, buf);
    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        uint32 devId = 5;
        ret = _8291_cli_string_get(devId, start_addr, VENDOR_NAME_SZ, buf);
    }	
    else
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;         
    }
    
    buf[VENDOR_NAME_SZ]='\0';

    printf("%s: start address [%d] = \"%s\" \n", __FUNCTION__, start_addr, buf);
    return ret;    
#undef VENDOR_NAME_SZ
}

static int europa_cli_VendorPN_get(
    int argc,
    char *argv[],
    FILE *fp)
{
#define VENDOR_PN_SZ 16
    int start_addr;
    char buf[VENDOR_PN_SZ+1];
    int chip_mode;
    int ret = 0;

    chip_mode = _europa_ldd_parser();

    start_addr = 40;

    if (EUROPA_LDD_8290B_MODE == chip_mode)
    {                  
        ret = _europa_cli_string_get( fp, start_addr, VENDOR_PN_SZ, buf);
    }
    else if (EUROPA_LDD_8290C_MODE == chip_mode)
    {
    uint32 devId = 5;
        ret = _8290c_cli_string_get(devId, start_addr, VENDOR_PN_SZ, buf);
    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        uint32 devId = 5;
        ret = _8291_cli_string_get(devId, start_addr, VENDOR_PN_SZ, buf);
    }	
    else
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;         
    }

    buf[VENDOR_PN_SZ]='\0';

    printf("%s: start address [%d] = \"%s\" \n", __FUNCTION__, start_addr, buf);
    return ret;    
#undef VENDOR_PN_SZ
}

static int europa_cli_VendorRev_get(
    int argc,
    char *argv[],
    FILE *fp)
{
#define VENDOR_REV_SZ 4
    int start_addr;
    char buf[VENDOR_REV_SZ+1];
    int chip_mode;
    int ret = 0;

    chip_mode = _europa_ldd_parser();

    start_addr = 56;

    if (EUROPA_LDD_8290B_MODE == chip_mode)
    {                  
        ret = _europa_cli_string_get( fp, start_addr, VENDOR_REV_SZ, buf);
    }
    else if (EUROPA_LDD_8290C_MODE == chip_mode)
    {
        uint32 devId = 5;
        ret = _8290c_cli_string_get(devId, start_addr, VENDOR_REV_SZ, buf);
    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        uint32 devId = 5;
        ret = _8291_cli_string_get(devId, start_addr, VENDOR_REV_SZ, buf);
    }	
    else
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;         
    }
    
    buf[VENDOR_REV_SZ]='\0';

    printf("%s: start address [%d] = \"%s\" \n", __FUNCTION__, start_addr, buf);
    return ret;    

#undef VENDOR_REV_SZ
}

static int europa_cli_VendorSN_get(
    int argc,
    char *argv[],
    FILE *fp)
{
#define VENDOR_SN_SZ 16
    int start_addr;
    char buf[VENDOR_SN_SZ+1];
    int chip_mode;
    int ret = 0;

    chip_mode = _europa_ldd_parser();

    start_addr = 68;

    if (EUROPA_LDD_8290B_MODE == chip_mode)
    {                  
        ret = _europa_cli_string_get( fp, start_addr, VENDOR_SN_SZ, buf);
    }
    else if (EUROPA_LDD_8290C_MODE == chip_mode)
    {
        uint32 devId = 5;
        ret = _8290c_cli_string_get(devId, start_addr, VENDOR_SN_SZ, buf);
    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        uint32 devId = 5;
        ret = _8291_cli_string_get(devId, start_addr, VENDOR_SN_SZ, buf);
    }	
    else
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;         
    }
    
    buf[VENDOR_SN_SZ]='\0';

    printf("%s: start address [%d] = \"%s\" \n", __FUNCTION__, start_addr, buf);
    return ret;    
#undef VENDOR_SN_SZ
}

static int europa_cli_DateCode_get(
    int argc,
    char *argv[],
    FILE *fp)
{
#define DATE_CODE_SZ 8
    int start_addr;
    char buf[DATE_CODE_SZ+1];
    int chip_mode;
    int ret = 0;

    chip_mode = _europa_ldd_parser();

    start_addr = 84;

    
    if (EUROPA_LDD_8290B_MODE == chip_mode)
    {                  
        ret = _europa_cli_string_get( fp, start_addr, DATE_CODE_SZ, buf);
    }
    else if (EUROPA_LDD_8290C_MODE == chip_mode)
    {
        uint32 devId = 5;
        ret = _8290c_cli_string_get(devId, start_addr, DATE_CODE_SZ, buf);
    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        uint32 devId = 5;
        ret = _8291_cli_string_get(devId, start_addr, DATE_CODE_SZ, buf);
    }	
    else
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;         
    }
    
    buf[DATE_CODE_SZ]='\0';

    printf("%s: start address [%d] = \"%s\" \n", __FUNCTION__, start_addr, buf);
    return ret;    
#undef DATE_CODE_SZ
}


static int europa_cli_txpower_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 v_mpd;
    //uint32 vcal;
    int ret;
    uint64 sum_i, sum_v;
    int tx_a, tx_b, tx_c;
    uint32 mpd_v0, i_mpd, mpd1;
    int32 loopcnt, i;
    uint64 sum, sum_mpd;
    int32 tx_power;
    uint32 parm;
    uint32 w83_data;
    double temp_f;
    uint32 devId = 5;
    int chip_mode;
    uint32 r_mpd=0, code_250u=0, i_cal=0;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {
        if(argc >= 2)
        {
            loopcnt = _vlaue_translate(argv[1]); 
            if (loopcnt<0)
            {
                printf("Input Error!!!\n");
                return -1;
            }
            if(argc >= 5)
            {           
                r_mpd = _vlaue_translate(argv[2]);
                code_250u = _vlaue_translate(argv[3]);
                i_cal = _vlaue_translate(argv[4]);
            }
            else
            {
                r_mpd = 0;
                code_250u = 0;
                i_cal = 0;
            }
            
            ret = rtl8290c_cli_txpower_get(devId, loopcnt, r_mpd, code_250u, i_cal);
            if(ret)
            {
                printf("rtl8290c_cli_txpower_get fail!!! (%d)\n", ret);
                return -1;
            }
        }
        else
        {
            printf("%s <loop count> \n", argv[0]);
            printf("Or \n");
            printf("%s <loop count> <R MPD> <Code 250u> <I Cal>\n", argv[0]);
            return -1;
        }        
    }
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {
        if(argc >= 2)
        {
            loopcnt = _vlaue_translate(argv[1]);
            if (loopcnt<0)
            {
                printf("Input Error!!!\n");
                return -1;
            }
			r_mpd = 0;
            code_250u = 0;
            i_cal = 0;
            if(argc >= 4)
            {           
                r_mpd = _vlaue_translate(argv[2]);
                code_250u = _vlaue_translate(argv[3]);
                //i_cal = _vlaue_translate(argv[4]);
            }

            ret = rtl8291_cli_txpower_get(devId, loopcnt, r_mpd, code_250u, i_cal);
            if(ret)
            {
                printf("rtl8290c_cli_txpower_get fail!!! (%d)\n", ret);
                return -1;
            }
        }
        else
        {
            printf("%s <loop count> \n", argv[0]);
            printf("Or \n");
            printf("%s <loop count> <R MPD> <Code 250u>\n", argv[0]);
            return -1;
        }        
    }	
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
    if(argc >= 2)
    { 
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);    
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);
        osal_time_mdelay(500);    
        _europa_cal_flash_data_get(RTL8290B_TX_A_ADDR, 4, &tx_a);
        _europa_cal_flash_data_get(RTL8290B_TX_B_ADDR, 4, &tx_b);
        _europa_cal_flash_data_get(RTL8290B_TX_C_ADDR, 4, &tx_c);
        _europa_cal_flash_data_get(RTL8290B_MPD0_ADDR, 4, &mpd_v0);
        printf("Start to get MPD in original mode... \n");
        //printf("MPD0 = 0x%x \n", mpd_v0);
        loopcnt = _vlaue_translate(argv[1]);  
        if (loopcnt<0)
        {
            printf("loopcnt should >0 \n");
            return -1;
        }
        
        sum = 0;
        sum_mpd = 0;
        for (i=0;i<loopcnt;i++)
        {
            //printf("Loop %d: \n", i);
            ret = rtk_ldd_tx_power_get(mpd_v0, &mpd1, &i_mpd);
            if(ret)
            {
                printf("Get TX power fail!!! (%d)\n", ret);
                return -1;
            }
            sum += i_mpd;
            sum_mpd += mpd1;
        }
        
        i_mpd = sum/loopcnt;
        mpd1 = sum_mpd/loopcnt;
        
#if 0
        tx_power = (i_mpd*ldd_param.tx_a)>>TX_A_SHIFT +
                   (ldd_param.tx_b)>>TX_B_SHIFT;
#endif
        if(0==i_mpd || 0==tx_a)
        {
            printf("Invalid MPD current or DDMI coefficients!!!\n");
            temp_f = 0;
        }
        else if (0 == tx_b)
        {
            /* tx_b=0 means tx ddmi is calibrated at single point */
            temp_f = (double)i_mpd*(double)tx_a/(1<<TX_A_SHIFT);
        }
        else
        {
            temp_f = (double)i_mpd*(double)tx_a/(1<<TX_A_SHIFT) + \
            (double)(tx_b)/(1<<TX_B_SHIFT);
        }
         
        tx_power = (0.0<temp_f) ? (uint32)(temp_f*1000) : 0; /* nW */
            
        printf("Get TX power MPD1 = %d, I_MPD = %d uA\n", mpd1, i_mpd);
        printf("TX Power = %d nW\n", tx_power);

        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
        
        //vcal = _vlaue_translate(argv[1]);      
        //rtk_ldd_tx_power_get(vcal, &v_mpd, &i_mpd);        
        //rtk_ldd_tx_power_get(0, &v_mpd, &i_mpd);
        //printf("%s: v_mpd_cal = [0x%x], v_mpd = [0x%x],  i_mpd =  [0x%x]\n", __FUNCTION__, vcal, v_mpd, i_mpd); 
        //printf("%s: v_mpd = [0x%x],  i_mpd =  [0x%x]\n", __FUNCTION__, v_mpd, i_mpd); 
        //printf("%s: i_mpd =  %d uA\n", __FUNCTION__, i_mpd);           
    }
    else
    {
        printf("%s <loop count> \n", argv[0]);
        return -1;
    }
    }
    else
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;
    }
    
    return 0;    
}

static int europa_cli_vmpdCal_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE == chip_mode)
    {                  
        rtk_ldd_mpd0_get(0, &vmpd_cal);
        printf("%s:  v_mpd_cal = [0x%x]\n", __FUNCTION__, vmpd_cal);
    }
    else
    {
        printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
        return -1;         
    }


    return 0;    
}


static int europa_cli_rxpower_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 rssi_v0;
    uint32 v_rssi, i_rssi;
    uint32 output,i;
    uint64 sum_i, sum_v;
    int ret, loopcnt;
    int rx_a, rx_b, rx_c;
    double temp_f, temp_f2;
    uint32 rx_power;
    uint32 w83_data;
    uint32 vdd_v0, gnd_v0, half_v0;
    int rssi_k;

    europa_rxpwr_t *p_rx_arr;
    int32 offset, diff, addr;
    int16 temp0, temp_0_offset;
    uint32 value;
    uint64 sum_i2, sum_v2;
    uint32 v_rssi2, i_rssi2;

    rtk_ldd_cfg_t ldd_cfg;
    europa_rxparam_t ldd_rxparam;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {
        if(argc >= 2)
        {  
            loopcnt = _vlaue_translate(argv[1]); 
            ret = rtl8290c_cli_rxpower_get(devId, loopcnt);
            if(ret)
            {
                printf("rtl8290c_cli_rxpower_get fail!!! (%d)\n", ret);
                return -1;
            }
        }
        else
        {
            printf("%s <loop count> \n", argv[0]);
            return -1;
        }
    }
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {
        if(argc >= 2)
        {  
            loopcnt = _vlaue_translate(argv[1]); 
            ret = rtl8291_cli_rxpower_get(devId, loopcnt);
            if(ret)
            {
                printf("rtl8290c_cli_rxpower_get fail!!! (%d)\n", ret);
                return -1;
            }
        }
        else
        {
            printf("%s <loop count> \n", argv[0]);
            return -1;
        }
    }	
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
    if(argc >= 2)
    {     
        loopcnt = _vlaue_translate(argv[1]);  

        if (100 < loopcnt)
        {
            printf("Loop Count should less than 100. \n");
            return -1;
        }
        
        _europa_cal_flash_data_get(RTL8290B_RX_A_ADDR, 4, &rx_a);
        _europa_cal_flash_data_get(RTL8290B_RX_B_ADDR, 4, &rx_b);
        _europa_cal_flash_data_get(RTL8290B_RX_C_ADDR, 4, &rx_c);
        _europa_cal_flash_data_get(RTL8290B_RSSI_V0_ADDR, 4, &rssi_v0);
        _europa_cal_flash_data_get(RTL8290B_GND_V0_ADDR, 4, &gnd_v0);
        _europa_cal_flash_data_get(RTL8290B_VDD_V0_ADDR, 4, &vdd_v0);
        _europa_cal_flash_data_get(RTL8290B_HALF_VDD_V0_ADDR, 4, &half_v0);
        _europa_cal_flash_data_get(RTL8290B_RX_K_ADDR, 4, &rssi_k);
        
        addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_TEMP0_OFFSET_ADDR;    
        _europa_cal_flash_data_get(addr, 2, &value);
        temp0 = (int16)value;
        addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_TEMP0_OFF_OFFSET_ADDR;    
        _europa_cal_flash_data_get(addr, 2, &value);
        temp_0_offset = (int16)value;        

        printf("Europa RX power TEMP_0 = %d\n", temp0);         
        printf("Europa RX power TEMP_0_OFFSET = %d\n", temp_0_offset);     
        
#ifdef EUROPA_DEBUG_RXPOWER        
        printf("Europa RX power RSSI V0 = %d\n", rssi_v0);
        printf("Europa RX power VDD V0 = %d\n", vdd_v0);
        printf("Europa RX power GND V0 = %d\n", gnd_v0);
        printf("Europa RX power Half VDD V0 = %d\n", half_v0);
        printf("Europa RX power RSSI K = %d\n", rssi_k);        
#endif

            // tempest - validate rssi_v0 first
        {
            if (0==rssi_v0)
            {
                printf("Invalid RSSI-V0, run get rssi-v0 first!\n");
                return -1;
            }
        }
            
        memset(&ldd_rxparam, 0, sizeof(europa_rxparam_t));
        _europa_rx_parameter_get(&ldd_rxparam);

        offset = 10;
        //p_rx_arr = (europa_rxpwr_t *)malloc((loopcnt+offset)*sizeof(europa_rxpwr_t));
        //memset(p_rx_arr, 0,sizeof(europa_rxpwr_t)*(loopcnt+offset));
        p_rx_arr = (europa_rxpwr_t *)malloc(sizeof(europa_rxpwr_t)*110);
        memset(p_rx_arr, 0,sizeof(europa_rxpwr_t)*110);
        
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);    
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);

        osal_time_mdelay(500);
            
        //printf("Parameter list: \nrssi_code gnd_code vdd_code ldo_code half_vdd_code rset_code adc_gnd_code rx_ldo_code rssi_volt1 rssi_volt2 i_rssi1 i_rssi2, vdd_v0 gnd_v0 half_vdd_v0\n");
        
        sum_i2 = 0;
        sum_v2 = 0;
        i_rssi2 = 0;
        v_rssi2 = 0;

        for (i=0;i<loopcnt+offset;i++)
        {
            ret = rtk_ldd_rx_power_get(rssi_v0, &v_rssi, &i_rssi);
            if(ret)
            {
                printf("Get RX power fail!!! (%d)\n", ret);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
                free(p_rx_arr);
                return -1;
            }
            p_rx_arr[i].rssi_i = i_rssi;
            p_rx_arr[i].rssi_v = v_rssi;    

            sum_i2 += i_rssi;
            sum_v2 += v_rssi;            
            //printf("Original:i %d  %d \n", i, i_rssi, v_rssi);
            
        }

        i_rssi2 = sum_i2/(loopcnt+offset);
        v_rssi2 = sum_v2/(loopcnt+offset);
    
        _europa_rxbubble_sort(p_rx_arr, loopcnt+offset);
        
        sum_i = 0;
        sum_v = 0;
        
        for (i=0;i<loopcnt;i++)
        {
            //printf("Loop %d: \n", i);

            sum_i += p_rx_arr[i+5].rssi_i;
            sum_v += p_rx_arr[i+5].rssi_v;
            //printf("Sort:i %d  %d \n", i, p_rx_arr[i+5].rssi_i, p_rx_arr[i+5].rssi_v);            
        }
        
        i_rssi = sum_i/loopcnt;
        v_rssi = sum_v/loopcnt;

#ifdef EUROPA_DEBUG_RXPOWER
        for (i=0;i<5;i++)
        {
            diff = abs((int32)p_rx_arr[i].rssi_i-(int32)i_rssi);
            printf("p_rx_arr[%d].rssi_i = %d, p_rx_arr[%d].rssi_v = %d\n", i, p_rx_arr[i].rssi_i, i, p_rx_arr[i].rssi_v);        
            if (diff > (i_rssi*2/10))
            {
                printf("ERROR: p_rx_arr[%d].rssi_i = %d, AVG = %d\n", i, p_rx_arr[i].rssi_i, i_rssi);
            }            
        }

        for (i=0;i<5;i++)
        {
            diff = abs((int32)p_rx_arr[i+loopcnt+5].rssi_i-(int32)i_rssi);
            printf("p_rx_arr[%d].rssi_i = %d, p_rx_arr[%d].rssi_v = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_i, i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v);
            if (diff > (i_rssi*2/10))
            {
                printf("ERROR: p_rx_arr[%d].rssi_i = %d, AVG = = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_i, i_rssi);
            }
        }
#endif        
        free(p_rx_arr);

#if 0        
        sum_i = 0;
        sum_v = 0;
        
        for (i=0;i<loopcnt;i++)
        {
            //printf("Loop %d: \n", i);
            ret = rtk_ldd_rx_power_get(rssi_v0, &v_rssi, &i_rssi);
            if(ret)
            {
                printf("Get RX power fail!!! (%d)\n", ret);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);                
                return -1;
            }
            sum_i += i_rssi;
            sum_v += v_rssi;
        }
        
        i_rssi = sum_i/loopcnt;
        v_rssi = sum_v/loopcnt;
#endif

#if 0       
        printf("Get RX power I_RSSI = %d nA, V_RSSI = %d uV RSSI_V0 = %d\n", i_rssi, v_rssi, rssi_v0);
        
                //ldd_param.rssi_voltage= v_rssi;
                //ldd_param.rssi_i= i_rssi;
        
                //diag_util_printf("RX coeff: %x %x %x\n", ldd_param.rx_a, ldd_param.rx_b, ldd_param.rx_c);
        
    /* note that i_rrsi is nA */
#if 0
        temp_a = (ldd_param.rx_a*(i_rssi*i_rssi)/1000000)>>RX_A_SHIFT;
        temp_b = (ldd_param.rx_b*i_rssi/1000)>>RX_B_SHIFT;
        temp_c = (ldd_param.rx_c)>>RX_C_SHIFT;
        rx_power = temp_a+temp_b+temp_c;
#endif
        if(0==i_rssi || 0==rx_a || 0==rx_b || 0==rx_c )
        {
           printf("Invalid RSSI current or DDMI coefficients!!!\n");
           temp_f = 0;
        }
        else
        {
           temp_f = ((double)rx_a/(1<<RX_A_SHIFT))*((double)i_rssi*i_rssi/1000000) + \
                    ((double)rx_b/(1<<RX_B_SHIFT))*((double)i_rssi/1000) + \
                    ((double)rx_c/(1<<RX_C_SHIFT));
        }
        
        rx_power = (0.0<temp_f) ? (uint32)(temp_f*1000) : 0; /* nW */
        printf("rx_a = %d, rx_b = %d, rx_c = %d\n", rx_a, rx_b, rx_c);        
        printf("RX power = %d nW\n", rx_power);
#endif

#if 1       
                printf("Get RX power I_RSSI = %d nA, V_RSSI = %d uV RSSI_V0 = %d (0x%x)\n", i_rssi, v_rssi, rssi_v0, rssi_v0);
                //printf("Get RX power I_RSSI = %d nA, I_RSSI2 = %d nA RSSI_V0 = %d (0x%x)\n", i_rssi, v_rssi, rssi_v0, rssi_v0);
        
                //ldd_param.rssi_voltage= v_rssi;
                //ldd_param.rssi_i= i_rssi;
        
                //diag_util_printf("RX coeff: %x %x %x\n", ldd_param.rx_a, ldd_param.rx_b, ldd_param.rx_c);
        
    /* note that i_rrsi is nA */
#if 0
                temp_a = (ldd_param.rx_a*(i_rssi*i_rssi)/1000000)>>RX_A_SHIFT;
                temp_b = (ldd_param.rx_b*i_rssi/1000)>>RX_B_SHIFT;
                temp_c = (ldd_param.rx_c)>>RX_C_SHIFT;
                rx_power = temp_a+temp_b+temp_c;
#endif
                //if(0==i_rssi || 0==rx_a || 0==rx_b || 0==rx_c )
                if(0==i_rssi || 0==rx_b )                
                {
                   printf("Invalid RSSI current or DDMI coefficients!!!\n");
                   temp_f = 0;
                }
                else
                {
                   temp_f = ((double)rx_a/(1<<RX_A_SHIFT))*((double)i_rssi*i_rssi/1000000) + \
                            ((double)rx_b/(1<<RX_B_SHIFT))*((double)i_rssi/1000) + \
                            ((double)rx_c/(1<<RX_C_SHIFT));                   
                }
                
                rx_power = (0.0<temp_f) ? (uint32)(temp_f*1000) : 0; /* nW */              

                temp_f2  = __log10((double)((rx_power/100)&0xFFFF)*1/10000)*10;                  
                printf("rx_a = %d, rx_b = %d, rx_c = %d\n", rx_a, rx_b, rx_c);          
                printf("RX power = %d nW (%f dBm)\n", rx_power, temp_f2);
#endif
       
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);        
    }
    else
    {
        printf("%s <loop count> \n", argv[0]);
        return -1;
    }
    }
    else
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;
    }
    
    return 0;    
}

static int europa_cli_txbias_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 bias;

    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {
        ret = rtl8290c_cli_txbias_get(devId);
        if(ret)
        {
            printf("rtl8290c_cli_txbias_get Fail!!! (%d)\n", ret);
            return -1;
        }        

    }
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {
        ret = rtl8291_cli_txbias_get(devId);
        if(ret)
        {
            printf("rtl8291_cli_txbias_get Fail!!! (%d)\n", ret);
            return -1;
        }        

    }	
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
        bias = 0;
    
        ret = rtk_ldd_tx_bias_get(&bias);
        if(ret)
        {
            printf("Get TX Bias Fail!!! (%d)\n", ret);
            return -1;
        }
    
        //printf("%s: txbias = [0x%x] \n", __FUNCTION__, bias);
        printf("TX Bias = 0x%x(2uA), %duA\n", bias, bias*2);
    }
    else
    {
        printf("LDD Chip Not Supported!!!\n");
    }
    
    return 0;    
}

static int europa_cli_txmod_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 mod;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {
        ret = rtl8290c_cli_txmod_get(devId);
        if(ret)
        {
            printf("Get TX MOD Fail!!! (%d)\n", ret);
            return -1;
        }        
    }
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {
        ret = rtl8291_cli_txmod_get(devId);
        if(ret)
        {
            printf("Get TX MOD Fail!!! (%d)\n", ret);
            return -1;
        }        
    }	
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
        mod = 0;   
        ret = rtk_ldd_tx_mod_get(&mod);
        if(ret)
        {
            printf("Get TX Modulation Fail!!! (%d)\n", ret);
            return -1;
        }
    
        printf("TX Modulation = 0x%x(2uA), %duA\n", mod, mod*2);
    }
    else
    {
        printf("LDD Chip Not Supported!!!\n");
    }
    
    return 0;    
}


int _europa_sdadc_code_get(europa_sdadc_code_t sdadc_code, uint32 *code_data)
{
    uint32 regData1, regData2, regData3;
    uint32 temp32, W18_data, data2;

    switch (sdadc_code)
    {
        case EUROPA_SDADC_LDO_TX_CODE:
            W18_data = 0x02;                  
            break;

        case EUROPA_SDADC_LDO_APC_CODE:
            W18_data = 0x22;                        
            break;

        case EUROPA_SDADC_LDO_RXLA_CODE:
            W18_data = 0x42;                
            break;
    
        case EUROPA_SDADC_LDO_LV_CODE:
            W18_data = 0x62;                    
            break;

        case EUROPA_SDADC_HALF_VDD_CODE:
            W18_data = 0x82;                    
            break;

        case EUROPA_SDADC_GND_CODE:
            W18_data = 0xA2;                
            break;

        case EUROPA_SDADC_RSSI_CODE:
            W18_data = 0xC2;                
            break;
            
      case EUROPA_SDADC_RSET1_CODE:
            W18_data = 0xE2;                
            break;
           
        default:
            return 0;
    }

    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 18, W18_data);    

    osal_time_mdelay(10); 

    temp32 = 0;
    data2 = W18_data | 0x08;
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 18, data2);        

    europa_i2c_read(EUROPA_I2C_PORT, 0x55, 14, &regData1);        
    europa_i2c_read(EUROPA_I2C_PORT, 0x55, 15, &regData2);    
    europa_i2c_read(EUROPA_I2C_PORT, 0x55, 16, &regData3);    
    temp32 =((regData1 & 0xff) << 16) | ((regData2 & 0xff) << 8) | (regData3);
    *code_data = temp32;

    return 0;
}



/*
 * debug europa get rssi-voltage <UINT:loop_count>
 */
static int europa_cli_rssi_voltage_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 rssi_voltage1, rssi_voltage2;
    int32 loopcnt, i;
    uint32 sum, sum_volt2;
    rtk_ldd_cfg_t ldd_cfg;
    uint32 vdd_v0, gnd_v0, half_v0;
    int rx_a, rx_b, rx_c;    
    int rssi_k;
    uint32 rssi_v0;    
    uint32 w83_data;

    uint64 sum_half, sum_v2;
    europa_rxpwr_t *p_rx_arr;
    int32 offset, diff;

    uint32 v_rssi, i_rssi;    
    uint64 sum_i, sum_v;
    uint64 temp64;
    uint32 temp32;
    uint32 addr;
    uint32 value;
     uint32 rssi_r1, rssi_r2;    
    uint32 dbg_en;
#ifdef RTL8290B_CAL_TIME
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
#endif
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if(argc < 3)
    { 
        printf("%s <dbg_en: 1 or 0> <loop count> \n", argv[0]);
        return -1;
    }

    dbg_en  = _vlaue_translate(argv[1]);
    loopcnt = _vlaue_translate(argv[2]);

    if ((100 < loopcnt)||(0 > loopcnt))
    {
        printf("Loop Count should less than 100. \n");
        return -1;
    }

    _europa_cal_flash_data_get(RTL8290B_RX_A_ADDR, 4, &rx_a);
    _europa_cal_flash_data_get(RTL8290B_RX_B_ADDR, 4, &rx_b);
    _europa_cal_flash_data_get(RTL8290B_RX_C_ADDR, 4, &rx_c);
    _europa_cal_flash_data_get(RTL8290B_RSSI_V0_ADDR, 4, &rssi_v0);
    _europa_cal_flash_data_get(RTL8290B_GND_V0_ADDR, 4, &gnd_v0);
    _europa_cal_flash_data_get(RTL8290B_VDD_V0_ADDR, 4, &vdd_v0);
    _europa_cal_flash_data_get(RTL8290B_HALF_VDD_V0_ADDR, 4, &half_v0);
    _europa_cal_flash_data_get(RTL8290B_RX_K_ADDR, 4, &rssi_k);

    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_RX_RSSI_R1_OFFSET_ADDR;
    _europa_cal_flash_data_get(addr, 2, &value);    
    if (value <100)
    {
        rssi_r1 = 1000;
    }
    else
    {
        rssi_r1 = value*10;
    }

    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_RX_RSSI_R2_OFFSET_ADDR;
    _europa_cal_flash_data_get(addr, 2, &value);    
    if (value <100)
    {
        rssi_r2 = 1000;
    }
    else
    {
        rssi_r2 = value*10;
    }

    printf("Europa RX power RSSI V0 = %d\n", rssi_v0);
    printf("Europa RX power VDD V0 = %d\n", vdd_v0);
    printf("Europa RX power GND V0 = %d\n", gnd_v0);
    printf("Europa RX power Half VDD V0 = %d\n", half_v0);
    printf("Europa RX power RSSI K = %d\n", rssi_k);
    printf("rx_a = %d, rx_b = %d, rx_c = %d\n", rx_a, rx_b, rx_c);     
    printf("vdd_v0 = 0x%x gnd_v0 = 0x%x half_v0 = 0x%x \n", vdd_v0, gnd_v0, half_v0);
    printf("rssi_r1 = %d rssi_r2 = %d \n", rssi_r1, rssi_r2);

    //offset = 10;
    //p_rx_arr = (europa_rxpwr_t *)malloc(loopcnt+offset);    
    //memset(p_rx_arr, 0, sizeof(europa_rxpwr_t)*(loopcnt+offset));
    p_rx_arr = (europa_rxpwr_t *)malloc(sizeof(europa_rxpwr_t)*(loopcnt));    
    memset(p_rx_arr, 0, sizeof(europa_rxpwr_t)*(loopcnt));

    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);
    
    printf("Parameter list: \nrssi_code gnd_code vdd_code ldo_code half_vdd_code rset_code rssi_voltage rssi_voltage2\n");
    rssi_voltage1 = 0;
    rssi_voltage2 = 0;    
    sum_half= 0;
    sum_v2= 0;    

    osal_time_mdelay(500); 
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp1,&tz);    
#endif       
    for (i=0;i<loopcnt;i++)
    {
        //printf("Loop %d: total = %d \n", i, loopcnt+offset);   
        memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
        ldd_cfg.vdd_code = vdd_v0;
        ldd_cfg.gnd_code = gnd_v0;    
        ldd_cfg.half_vdd_code = half_v0;  
        //enable debug mode 
        ldd_cfg.state = 0xF1;
        ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
        if(ret)
        {
            printf("Get RSSI Voltage fail!!! (%d)\n", ret);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);  
            free(p_rx_arr);            
            return -1;
        }
        //printf("%8d %8d %8d %8d %8d %8d %8d %8d\n", ldd_cfg.rssi_code, ldd_cfg.gnd_code, ldd_cfg.vdd_code, ldd_cfg.ldo_code, ldd_cfg.half_vdd_code, ldd_cfg.rset_code, ldd_cfg.rssi_voltage, ldd_cfg.rssi_voltage2);        
        p_rx_arr[i].half_vdd_code = ldd_cfg.half_vdd_code;
        p_rx_arr[i].rssi_v = ldd_cfg.rssi_voltage2;      
    }
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);  
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);    
    printf("[TIME]:  europa_cli_rssiv0_get Duration = %f ms\n", Duration);
#endif
    //_europa_rxbubble_sort(p_rx_arr, loopcnt+offset);

    for (i=0;i<loopcnt;i++)
    {
        //printf("Loop %d: \n", i);  
        sum_half += p_rx_arr[i].half_vdd_code;
        sum_v2 += p_rx_arr[i].rssi_v;
        //printf("Sort:i %d  %d \n", i, p_rx_arr[i+5].rssi_i, p_rx_arr[i+5].rssi_v);            
    }
    
    //i_rssi = sum_i/loopcnt;
    rssi_voltage2 = sum_v2/loopcnt;
    printf("Average rssi_v = %d\n", rssi_voltage2);

    if (rssi_voltage2 < rssi_v0)
    {
        i_rssi = 0;        
    }    
    else
    {
        //The unit is nA
        //ra = 1000;
        //rb = 1000;
        //ra = 4000;
        //rb = 4000;        
        //*i_rssi = 1000*(volt_rssi- rssi_v0)*(ra+rb)/(ra*rb);   
        temp64 = abs(rssi_voltage2 - rssi_v0);
        temp64 = temp64*(rssi_r1+rssi_r2)*1000;
        temp32 = rssi_r1*rssi_r2;
        temp64 = temp64/temp32;
        i_rssi  = (uint32)temp64;        
    }
    
    printf("Average i_rssi = %d\n", i_rssi); 

    //for (i=0;i<5;i++)
    //{
    //    diff = abs((int32)p_rx_arr[i].rssi_v-(int32)rssi_voltage2);
    //    printf("[%d] rssi_v = %d, half_vdd_code = 0x%x, diff = %d\n", i, p_rx_arr[i].rssi_v, p_rx_arr[i].half_vdd_code, diff);        
    //    if (diff > (rssi_voltage2*2/10))
    //    {
    //        printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = %d\n", i, p_rx_arr[i].rssi_v, rssi_voltage2);
    //    }            
    //}
    
    //for (i=0;i<5;i++)
    //{
    //    diff = abs((int32)p_rx_arr[i+loopcnt+5].rssi_v-(int32)rssi_voltage2);
    //    printf("[%d] rssi_v = %d, half_vdd_code = 0x%x, diff = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, p_rx_arr[i+loopcnt+5].half_vdd_code, diff);
    //    if (diff > (rssi_voltage2*2/10))
    //    {
    //        printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, rssi_voltage2);
    //    }
    //}
    
    free(p_rx_arr);

#if 0    
    printf("Parameter list: \nrssi_code gnd_code vdd_code ldo_code half_vdd_code rset_code rssi_voltage rssi_voltage2\n");
    rssi_voltage1 = 0;
    rssi_voltage2 = 0;    
    sum = 0;
    sum_volt2 = 0;    
    for (i=0;i<loopcnt;i++)
    {
        //printf("Loop %d: \n", i);
        memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
        ldd_cfg.vdd_code = vdd_v0;
        ldd_cfg.gnd_code = gnd_v0;    
        ldd_cfg.half_vdd_code = half_v0;  
        //enable debug mode 
        ldd_cfg.state = 0xF1;      
        ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
        if(ret)
        {
            printf("Get RSSI Voltage fail!!! (%d)\n", ret);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);            
            free(p_rx_arr);            
            return -1;
        }
        printf("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", ldd_cfg.rssi_code, ldd_cfg.gnd_code, ldd_cfg.vdd_code, ldd_cfg.ldo_code, ldd_cfg.half_vdd_code, ldd_cfg.rset_code, ldd_cfg.rssi_voltage, ldd_cfg.rssi_voltage2);        
        sum       += ldd_cfg.rssi_voltage;
        sum_volt2 += ldd_cfg.rssi_voltage2;        
    }
#endif
    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);

#if 0
    rssi_voltage1 = sum/loopcnt;
    rssi_voltage2 = sum_volt2/loopcnt;

    //printf("Get RSSI Voltage = %d, RSSI Voltage2 = %d\n", rssi_voltage1, rssi_voltage2);
#endif

    printf("RSSI VOLTAGE FINISH\n");

    return 0;
}  

static int europa_cli_rssi_voltage2_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 rssi_voltage1, rssi_voltage2;
    int32 loopcnt1, loopcnt2, i, j;
    uint32 sum, sum_volt2;
    rtk_ldd_cfg_t ldd_cfg;
    uint32 vdd_v0, gnd_v0, half_v0;
    int rx_a, rx_b, rx_c;    
    int rssi_k;
    uint32 rssi_v0;    
    uint32 w83_data;
    uint64 sum_half, sum_v2, sum_rssi, sum_ldo, sum_rset, sum_gnd, sum_vdd;
    int32 offset, diff;

    uint32 v_rssi, i_rssi;    
    uint64 sum_i, sum_v;
    uint64 temp64;
    uint32 temp32;
    uint32 addr;
    uint32 value;
    uint32 rssi_r1, rssi_r2;    
    uint32 avg_half, avg_v2, avg_rssi, avg_ldo, avg_rset, avg_gnd, avg_vdd;

    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if(argc < 3)
    { 
        printf("%s <loop1 count> <loop2 count> \n", argv[0]);
        return -1;
    }

    loopcnt1 = _vlaue_translate(argv[1]);
    loopcnt2 = _vlaue_translate(argv[2]);

    if ((0 > loopcnt1) || (0 > loopcnt2) )
    {
        printf("Loop Count should > 0. \n");
        return -1;
    }

    _europa_cal_flash_data_get(RTL8290B_RX_A_ADDR, 4, &rx_a);
    _europa_cal_flash_data_get(RTL8290B_RX_B_ADDR, 4, &rx_b);
    _europa_cal_flash_data_get(RTL8290B_RX_C_ADDR, 4, &rx_c);
    _europa_cal_flash_data_get(RTL8290B_RSSI_V0_ADDR, 4, &rssi_v0);
    _europa_cal_flash_data_get(RTL8290B_GND_V0_ADDR, 4, &gnd_v0);
    _europa_cal_flash_data_get(RTL8290B_VDD_V0_ADDR, 4, &vdd_v0);
    _europa_cal_flash_data_get(RTL8290B_HALF_VDD_V0_ADDR, 4, &half_v0);
    _europa_cal_flash_data_get(RTL8290B_RX_K_ADDR, 4, &rssi_k);

    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_RX_RSSI_R1_OFFSET_ADDR;
    _europa_cal_flash_data_get(addr, 2, &value);    
    if (value <100)
    {
        rssi_r1 = 1000;
    }
    else
    {
        rssi_r1 = value*10;
    }

    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_RX_RSSI_R2_OFFSET_ADDR;
    _europa_cal_flash_data_get(addr, 2, &value);    
    if (value <100)
    {
        rssi_r2 = 1000;
    }
    else
    {
        rssi_r2 = value*10;
    }

    printf("Europa RX power RSSI V0 = %d\n", rssi_v0);
    printf("Europa RX power VDD V0 = %d\n", vdd_v0);
    printf("Europa RX power GND V0 = %d\n", gnd_v0);
    printf("Europa RX power Half VDD V0 = %d\n", half_v0);
    printf("Europa RX power RSSI K = %d\n", rssi_k);
    printf("rx_a = %d, rx_b = %d, rx_c = %d\n", rx_a, rx_b, rx_c);     
    printf("vdd_v0 = 0x%x gnd_v0 = 0x%x half_v0 = 0x%x \n", vdd_v0, gnd_v0, half_v0);
    printf("rssi_r1 = %d rssi_r2 = %d \n", rssi_r1, rssi_r2);

    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);
   
    printf("Parameter list: \nrssi_code gnd_code vdd_code ldo_code half_vdd_code rset_code rssi_voltage rssi_voltage2\n");
    rssi_voltage1 = 0;
    rssi_voltage2 = 0;    
    sum_half= 0;
    sum_v2= 0;  
    sum_rssi = 0;
    sum_ldo = 0;
    sum_rset = 0;
    sum_gnd = 0;
    sum_vdd = 0;

    osal_time_mdelay(500); 
    
    for (j=0;j<loopcnt2;j++)
    {
        rssi_voltage1 = 0;
        rssi_voltage2 = 0;      
        sum_half = 0;
        sum_v2   = 0;    
        sum_rssi = 0;
        sum_ldo  = 0;
        sum_rset = 0;
        sum_gnd  = 0;
        sum_vdd  = 0;
        sum_v    = 0;        
        for (i=0;i<loopcnt1 ;i++)
        {
            //printf("Loop %d: total = %d \n", i, loopcnt+offset);   
            memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
            ldd_cfg.vdd_code = vdd_v0;
            ldd_cfg.gnd_code = gnd_v0;    
            ldd_cfg.half_vdd_code = half_v0;  
            //enable debug mode 
            ldd_cfg.state = 0xF1;      
            ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
            if(ret)
            {
                printf("Get RSSI Voltage fail!!! (%d)\n", ret);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);           
                return -1;
            }
            printf("%8d %8d %8d %8d %8d %8d %8d %8d\n", ldd_cfg.rssi_code, ldd_cfg.gnd_code, ldd_cfg.vdd_code, ldd_cfg.ldo_code, ldd_cfg.half_vdd_code, ldd_cfg.rset_code, ldd_cfg.rssi_voltage, ldd_cfg.rssi_voltage2);        
            sum_half   += ldd_cfg.half_vdd_code;
            sum_rssi   += ldd_cfg.rssi_code;    
            sum_gnd    += ldd_cfg.gnd_code;
            sum_vdd    += ldd_cfg.vdd_code;
            sum_rset   += ldd_cfg.rset_code;
            sum_ldo    += ldd_cfg.ldo_code;
            sum_v2     += ldd_cfg.rssi_voltage2;
            sum_v      += ldd_cfg.rssi_voltage;
        }
        rssi_voltage2 = sum_v2/loopcnt1;
        rssi_voltage1 = sum_v/loopcnt1;        
        avg_half = sum_half/loopcnt1;
        avg_rssi = sum_rssi/loopcnt1;
        avg_ldo = sum_ldo/loopcnt1;
        avg_rset = sum_rset/loopcnt1; 
        avg_gnd = sum_gnd/loopcnt1;
        avg_vdd = sum_vdd/loopcnt1;
        printf("Average: %8d %8d %8d %8d %8d %8d %8d %8d\n", avg_rssi, avg_gnd, avg_vdd, avg_ldo, avg_half, avg_rset, rssi_voltage1, rssi_voltage2);        
    }   


    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);

    printf("RSSI VOLTAGE FINISH\n");

    return 0;
}  

static int europa_cli_rssi_voltage3_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 rssi_voltage1, rssi_voltage2;
    int loopcnt1, loopcnt2, i, j;
    uint32 sum, sum_volt2;
    rtk_ldd_cfg_t ldd_cfg;
    uint32 vdd_v0, gnd_v0, half_v0;
    int rx_a, rx_b, rx_c;    
    int rssi_k;
    uint32 rssi_v0;    
    uint32 w83_data;
    uint64 sum_half, sum_v2, sum_rssi, sum_ldo, sum_rset, sum_gnd, sum_vdd;
    int32 offset, diff;

    uint32 v_rssi, i_rssi;    
    uint64 sum_i, sum_v;
    uint64 temp64;
    uint32 temp32;
    uint32 addr;
    uint32 value;
    uint32 rssi_r1, rssi_r2;    
    uint32 avg_half, avg_v2, avg_rssi, avg_ldo, avg_rset, avg_gnd, avg_vdd;

    uint64 sum_ldo_tx, sum_ldo_apc, sum_ldo_rxla, sum_ldo_lv, sum_sd_half, sum_sd_gnd;
    uint32 avg_ldo_tx, avg_ldo_apc, avg_ldo_rxla, avg_ldo_lv, avg_sd_half, avg_sd_gnd;
    uint32 ldo_tx_code, ldo_apc_code, ldo_rxla_code, ldo_lv_code, sd_half_code, sd_gnd_code;

    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if(argc < 3)
    { 
        printf("%s <loop1 count> <loop2 count> \n", argv[0]);
        return -1;
    }

    loopcnt1 = _vlaue_translate(argv[1]);
    loopcnt2 = _vlaue_translate(argv[2]);

    if ((0 > loopcnt1) || (0 > loopcnt2) )
    {
        printf("Loop Count should > 0. \n");
        return -1;
    }

    _europa_cal_flash_data_get(RTL8290B_RX_A_ADDR, 4, &rx_a);
    _europa_cal_flash_data_get(RTL8290B_RX_B_ADDR, 4, &rx_b);
    _europa_cal_flash_data_get(RTL8290B_RX_C_ADDR, 4, &rx_c);
    _europa_cal_flash_data_get(RTL8290B_RSSI_V0_ADDR, 4, &rssi_v0);
    _europa_cal_flash_data_get(RTL8290B_GND_V0_ADDR, 4, &gnd_v0);
    _europa_cal_flash_data_get(RTL8290B_VDD_V0_ADDR, 4, &vdd_v0);
    _europa_cal_flash_data_get(RTL8290B_HALF_VDD_V0_ADDR, 4, &half_v0);
    _europa_cal_flash_data_get(RTL8290B_RX_K_ADDR, 4, &rssi_k);

    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_RX_RSSI_R1_OFFSET_ADDR;
    _europa_cal_flash_data_get(addr, 2, &value);    
    if (value <100)
    {
        rssi_r1 = 1000;
    }
    else
    {
        rssi_r1 = value*10;
    }

    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_RX_RSSI_R2_OFFSET_ADDR;
    _europa_cal_flash_data_get(addr, 2, &value);    
    if (value <100)
    {
        rssi_r2 = 1000;
    }
    else
    {
        rssi_r2 = value*10;
    }

    printf("Europa RX power RSSI V0 = %d\n", rssi_v0);
    printf("Europa RX power VDD V0 = %d\n", vdd_v0);
    printf("Europa RX power GND V0 = %d\n", gnd_v0);
    printf("Europa RX power Half VDD V0 = %d\n", half_v0);
    printf("Europa RX power RSSI K = %d\n", rssi_k);
    printf("rx_a = %d, rx_b = %d, rx_c = %d\n", rx_a, rx_b, rx_c);     
    printf("vdd_v0 = 0x%x gnd_v0 = 0x%x half_v0 = 0x%x \n", vdd_v0, gnd_v0, half_v0);
    printf("rssi_r1 = %d rssi_r2 = %d \n", rssi_r1, rssi_r2);

    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);
   
    printf("Parameter list: \nrssi_code gnd_code vdd_code ldo_code half_vdd_code rset_code rssi_voltage rssi_voltage2 ldo_tx_code ldo_apc_code ldo_rxla_code ldo_lv_code sd_half_code sd_gnd_code\n");
    rssi_voltage1 = 0;
    rssi_voltage2 = 0;    
    sum_half= 0;
    sum_v2= 0;  
    sum_rssi = 0;
    sum_ldo = 0;
    sum_rset = 0;
    sum_gnd = 0;
    sum_vdd = 0;

    osal_time_mdelay(500); 
    
    for (j=0;j<loopcnt2;j++)
    {
        rssi_voltage1 = 0;
        rssi_voltage2 = 0;      
        sum_half = 0;
        sum_v2   = 0;    
        sum_rssi = 0;
        sum_ldo  = 0;
        sum_rset = 0;
        sum_gnd  = 0;
        sum_vdd  = 0;
        sum_v    = 0; 

        sum_ldo_tx   = 0;
        sum_ldo_apc  = 0;
        sum_ldo_rxla = 0;
        sum_ldo_lv   = 0;
        sum_sd_half  = 0;
        sum_sd_gnd   = 0;        
        for (i=0;i<loopcnt1 ;i++)
        {
#if 1    
            //printf("Loop %d: total = %d \n", i, loopcnt+offset);   
            memset(&ldd_cfg, 0, sizeof(rtk_ldd_cfg_t));
            ldd_cfg.vdd_code = vdd_v0;
            ldd_cfg.gnd_code = gnd_v0;    
            ldd_cfg.half_vdd_code = half_v0;  
            //enable debug mode 
            ldd_cfg.state = 0xF1;      
            ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
            if(ret)
            {
                printf("Get RSSI Voltage fail!!! (%d)\n", ret);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);           
                return -1;
            }
            printf("%8d %8d %8d %8d %8d %8d %8d %8d", ldd_cfg.rssi_code, ldd_cfg.gnd_code, ldd_cfg.vdd_code, ldd_cfg.ldo_code, ldd_cfg.half_vdd_code, ldd_cfg.rset_code, ldd_cfg.rssi_voltage, ldd_cfg.rssi_voltage2);        
            sum_half   += ldd_cfg.half_vdd_code;
            sum_rssi   += ldd_cfg.rssi_code;    
            sum_gnd    += ldd_cfg.gnd_code;
            sum_vdd    += ldd_cfg.vdd_code;
            sum_rset   += ldd_cfg.rset_code;
            sum_ldo    += ldd_cfg.ldo_code;
            sum_v2     += ldd_cfg.rssi_voltage2;
            sum_v      += ldd_cfg.rssi_voltage;
#endif

            _europa_sdadc_code_get(EUROPA_SDADC_LDO_TX_CODE, &ldo_tx_code);
            _europa_sdadc_code_get(EUROPA_SDADC_LDO_APC_CODE, &ldo_apc_code);
            _europa_sdadc_code_get(EUROPA_SDADC_LDO_RXLA_CODE, &ldo_rxla_code);
            _europa_sdadc_code_get(EUROPA_SDADC_LDO_LV_CODE, &ldo_lv_code);
            _europa_sdadc_code_get(EUROPA_SDADC_HALF_VDD_CODE, &sd_half_code);
            _europa_sdadc_code_get(EUROPA_SDADC_GND_CODE, &sd_gnd_code);            
            printf("%8d %8d %8d %8d %8d %8d\n", ldo_tx_code, ldo_apc_code, ldo_rxla_code, ldo_lv_code, sd_half_code, sd_gnd_code);        
            sum_ldo_tx   += ldo_tx_code;
            sum_ldo_apc  += ldo_apc_code;    
            sum_ldo_rxla += ldo_rxla_code;
            sum_ldo_lv   += ldo_lv_code;
            sum_sd_half  += sd_half_code;
            sum_sd_gnd   += sd_gnd_code;
        }
#if 1        
        rssi_voltage2 = sum_v2/loopcnt1;
        rssi_voltage1 = sum_v/loopcnt1;        
        avg_half = sum_half/loopcnt1;
        avg_rssi = sum_rssi/loopcnt1;
        avg_ldo = sum_ldo/loopcnt1;
        avg_rset = sum_rset/loopcnt1; 
        avg_gnd = sum_gnd/loopcnt1;
        avg_vdd = sum_vdd/loopcnt1;
        printf("Average: %8d %8d %8d %8d %8d %8d %8d %8d", avg_rssi, avg_gnd, avg_vdd, avg_ldo, avg_half, avg_rset, rssi_voltage1, rssi_voltage2);        
#endif
        avg_ldo_tx   = sum_ldo_tx/loopcnt1;
        avg_ldo_apc  = sum_ldo_apc/loopcnt1;
        avg_ldo_rxla = sum_ldo_rxla/loopcnt1;
        avg_ldo_lv   = sum_ldo_lv/loopcnt1; 
        avg_sd_half  = sum_sd_half/loopcnt1;        
        avg_sd_gnd   = sum_sd_gnd/loopcnt1;
        printf(" %8d %8d %8d %8d %8d %8d\n", avg_ldo_tx, avg_ldo_apc, avg_ldo_rxla, avg_ldo_lv, avg_sd_half, avg_sd_gnd);
    }   

    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);

    printf("SDADC CODE FINISH\n");

    return 0;
}

static int europa_cli_rssi_voltage4_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 w83_data;
    uint32 loopcnt, i;
    europa_rxparam_t ldd_rxparam;
    int value;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if(argc < 2)
    { 
        printf("%s <loop count>\n", argv[0]);
        return -1;
    }

    value = _vlaue_translate(argv[1]);
    if (value<=0)
    {
        printf("<loop count> should be > 0\n");
        return -1;        
    }
    loopcnt = (uint32)value;

    memset(&ldd_rxparam, 0, sizeof(europa_rxparam_t));
    _europa_rx_parameter_get(&ldd_rxparam);

    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);
   
    printf("Parameter list: \nrssi_code gnd_code vdd_code ldo_code half_vdd_code rssi_v rssi_v2 rssi_i rssi_i2\n");

    osal_time_mdelay(500); 
    
    for (i=0;i<loopcnt;i++)
    {
        _europa_rssi_voltage_get(&ldd_rxparam);
    }   

    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);

    printf("RSSI VOLTAGE FINISH\n");

    return 0;
} 

static int europa_cli_rssi_voltage5_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 w83_data;
    uint32 loopcnt, i;
    europa_rxparam_t ldd_rxparam;
    int32 rssi_k_2;
    uint32 rssi_v0_2;
    int32 addr;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if(argc < 2)
    { 
        printf("%s <loop count>\n", argv[0]);
        return -1;
    }

    loopcnt = _vlaue_translate(argv[1]);

    memset(&ldd_rxparam, 0, sizeof(europa_rxparam_t));
    _europa_rx_parameter_get(&ldd_rxparam);

    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_RSSI_V0_2_OFFSET_ADDR;
    _europa_cal_flash_data_get(addr, 4, &rssi_v0_2);
    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_RSSI_K2_OFFSET_ADDR;    
    _europa_cal_flash_data_get(addr, 4, &rssi_k_2);
    ldd_rxparam.rssi_v0_2 = rssi_v0_2;
    ldd_rxparam.rssi_k_2 = rssi_k_2;    

    printf("Current RX power RSSI V0_2 = %d\n", ldd_rxparam.rssi_v0_2);
    printf("Current RX power RSSI-K_2 = %d\n", ldd_rxparam.rssi_k_2);  


    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);
   
    printf("Parameter list: \nrssi_code rssi_code2 gnd_code vdd_code ldo_code half_vdd_code rssi_v rssi_v2 rssi_i rssi_i2\n");

    osal_time_mdelay(500); 
    
    for (i=0;i<loopcnt;i++)
    {
        _europa_rssi_voltage2_get(&ldd_rxparam);
    }   

    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);

    printf("RSSI VOLTAGE FINISH\n");

    return 0;
} 

void _europa_rx_mode_sel(int32 *pMode)
{
    int32 rx_power_mode;
    uint32 addr, value;
    uint32 rssi_v0, rssi_k, rssi_sel;

    _europa_cal_flash_data_get(RTL8290B_RSSI_V0_ADDR, 4, &rssi_v0);    
    _europa_cal_flash_data_get(RTL8290B_RX_K_ADDR, 4, &rssi_k); 
    addr = RTL8290B_RSSI_CODE_SEL_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
    _europa_cal_flash_data_get(addr, 2, &rssi_sel); 

    //20211109 RX MODE definition:
    //RX_MODE0, Use Old RSSI Code with Original algorithm
    //RX_MODE1, Use Old RSSI Code with RSSI-K Algorithm
    //RX_MODE2, Use New RSSI Code with Original algorithm
    //RX_MODE3, Use New RSSI Code with RSSI-K Algorithm    

    rx_power_mode = 1; 
    
    if ((rssi_v0 == 0)&&(rssi_sel == 0))
    {
        rx_power_mode = 3;
        addr = RTL8290B_RSSI_CODE_SEL_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        value = 1;
        _europa_cal_flash_data_set(addr, 2, value);
        *pMode = rx_power_mode;        
        return;
    }
    
    if (rssi_sel == 0) //rssi_v0 != 0 
    {
        if (rssi_k == 0)
        {
            rx_power_mode = 0;
        }
        else
        {
            rx_power_mode = 1;
        }
    }
    else if (rssi_sel == 1)
    {
        if (rssi_k == 0)
        {
            rx_power_mode = 2;
        }
        else
        {
            rx_power_mode = 3;
        }
    }
    *pMode = rx_power_mode;        
    
    return;
}

static int europa_cli_rssiv0_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 rssi_v0, rssi_gnd, rssi_vdd, rssi_half0, rssi_half1, rssi_v0_ori;
    uint32 loopcnt, i;
    uint64 sum, sum_gnd, sum_vdd, sum_half, sum_rssi, sum_ori;
    uint32 index;
    uint32 w83_data, w80_data, w70_data, data;
    rtk_ldd_cfg_t ldd_cfg;
    rtk_ldd_loop_mode_t mode;
    uint32 Imod, ori_Imod;
    uint32 rssi_code1, rssi_code0;
    double temp1, temp2, temp3;
    int32 temp32, rssi_k;
    int32 rx_power_mode;
    uint32 rssi_sel;

    europa_rxpwr_t *p_rx_arr;
    int32 offset, diff;

    uint32 addr;
    int16 temp_0, temp_0_offset;
    uint16 temp_now;
    uint32 w59_data, w5a_data, w68_data;

#ifdef RTL8290B_CAL_TIME
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
#endif
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if(argc < 2)
    { 
        printf("%s <loop count> \n", argv[0]);
        return -1;
    }

    loopcnt = _vlaue_translate(argv[1]);

    if (100 < loopcnt)
    {
         printf("Loop Count should less than 100. \n");
         return -1;
    }
    
    printf("europa_cli_rssiv0_get:\n"); 

    
#ifdef RTL8290B_CAL_TIME    
    gettimeofday(&tv_tmp1,&tz); 
 #endif   
    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);
    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x59, &w59_data);    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, 0xFF);
    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x5A, &w5a_data);    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, 0xFF);

    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x68, &w68_data);  

    ret = rtk_ldd_temperature_get(&temp_now);
    if(ret)
    {
        printf("Get Temperature Fail!!! (%d)\n", ret);
        return -1;
    }    

    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_TEMP0_OFFSET_ADDR;    
    _europa_cal_flash_data_set(addr, 2, (uint32)temp_now);    

    //Set temp 0 offset to 20
    temp_0_offset = 20;
    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_TEMP0_OFF_OFFSET_ADDR;        
    _europa_cal_flash_data_set(addr, 2, (uint32)temp_0_offset);    

    rx_power_mode = 1;

    _europa_cal_flash_data_get(RTL8290B_RSSI_V0_ADDR, 4, &rssi_v0);    
    _europa_cal_flash_data_get(RTL8290B_GND_V0_ADDR, 4, &rssi_gnd);     
    _europa_cal_flash_data_get(RTL8290B_VDD_V0_ADDR, 4, &rssi_vdd); 
    _europa_cal_flash_data_get(RTL8290B_HALF_VDD_V0_ADDR, 4, &rssi_half0); 
    _europa_cal_flash_data_get(RTL8290B_RX_K_ADDR, 4, &rssi_k); 

    addr = RTL8290B_RSSI_CODE_SEL_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
    _europa_cal_flash_data_get(addr, 2, &rssi_sel); 

    if (rssi_sel == 0) //rssi_v0 != 0 
    {
        if (rssi_v0 == 0)
        {
            rx_power_mode = 3;
            addr = RTL8290B_RSSI_CODE_SEL_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
            _europa_cal_flash_data_set(addr, 2, 1);   
        }
        else
        {
            if (rssi_k == 0)
            {
                rx_power_mode = 0;
            }
            else
            {
                rx_power_mode = 1;
            }
        }
    }
    else if (rssi_sel == 1)
    {
        if (rssi_k == 0)
        {
            rx_power_mode = 2;
        }
        else
        {
            rx_power_mode = 3;
        }
    }

    //_europa_rx_mode_sel(&rx_power_mode);
    
    printf("RX Power Mode = %d\n", rx_power_mode); 
    
    if (rx_power_mode == 0)
    {

        rssi_v0 = 0;
        sum = 0;
        
        for (i=0;i<loopcnt;i++)
        {
            //printf("Loop %d: \n", i);
            ret = rtk_ldd_rssiV0_get(&rssi_v0);
            if(ret)
            {
                printf("Get RSSI V0 fail!!! (%d)\n", ret);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, w59_data);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, w5a_data);                
                return -1;
            }
            sum += rssi_v0;
            //printf("Loop %d: rssi V0 = 0x%x \n", i, rssi_v0);
        }
         
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, w59_data);
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, w5a_data);    
        
        rssi_v0 = sum/loopcnt;
         
        _europa_cal_flash_data_set(RTL8290B_RSSI_V0_ADDR, 4, rssi_v0);          
        
        printf("Get RSSI V0 = %d\n", rssi_v0);
        
    }
    else if (rx_power_mode == 2)
    {

        offset = 10;
        //Maximum is 100+10=110
        p_rx_arr = (europa_rxpwr_t *)malloc(sizeof(europa_rxpwr_t)*110);     
        memset(p_rx_arr, 0,sizeof(europa_rxpwr_t)*110);    
        for (i=0;i<(loopcnt+offset);i++)
        {        
            memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
            ldd_cfg.state = 4;
            ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
            if(ret)
            {
                printf("rtk_ldd_rssiVoltage_get fail!!! (%d)\n", ret);                
                free(p_rx_arr);                    
                return -1;
            }
            p_rx_arr[i].rssi_i = ldd_cfg.rssi_voltage;              
            p_rx_arr[i].rssi_v = ldd_cfg.rssi_voltage2;
            p_rx_arr[i].rssi_code = ldd_cfg.rssi_code;              
            p_rx_arr[i].gnd_code = ldd_cfg.gnd_code;
            p_rx_arr[i].vdd_code = ldd_cfg.vdd_code;  
            p_rx_arr[i].half_vdd_code = ldd_cfg.half_vdd_code;              
            p_rx_arr[i].rssi_code2 = ldd_cfg.driver_version;                
            if ((w68_data&0x40)!=0)
            {
                printf("%8d (0x%x) %8d (0x%x) %8d (0x%x) %8d (0x%x) %8d (0x%x) \n", 
                ldd_cfg.rssi_code, ldd_cfg.rssi_code, ldd_cfg.half_vdd_code, ldd_cfg.half_vdd_code,
                ldd_cfg.vdd_code, ldd_cfg.vdd_code, ldd_cfg.gnd_code, ldd_cfg.gnd_code, 
                ldd_cfg.rssi_voltage2, ldd_cfg.rssi_voltage2); 
            }
                    
        }
        
        //_europa_rxbubble_sort(p_rx_arr, loopcnt+offset);
        _europa_rxbubble_sort(p_rx_arr, 110);
            
        rssi_v0 = 0;
        rssi_gnd = 0;
        rssi_vdd = 0;
        rssi_half0 = 0;
        rssi_code0 = 0;
        
        sum_ori = 0;
        sum = 0;
        sum_gnd = 0;
        sum_vdd = 0;
        sum_half = 0;  
        sum_rssi = 0;  
        //printf("Parameter list:\nrssi_code gnd_code vdd_code half_vdd_code rssi_voltage\n");
                 
        for (i=5;i<loopcnt+5;i++)
        {    
            sum_ori  += p_rx_arr[i].rssi_i;
            sum      += p_rx_arr[i].rssi_v;
            sum_rssi += p_rx_arr[i].rssi_code;
            sum_gnd  += p_rx_arr[i].gnd_code;
            sum_vdd  += p_rx_arr[i].vdd_code;     
            sum_half += p_rx_arr[i].half_vdd_code;                    
                    //printf("Loop %d: rssi V0 = 0x%x \n", i, p_rx_arr[i].rssi_v);
        }
        rssi_v0_ori = sum_ori/loopcnt;
        rssi_v0 = sum/loopcnt;
        rssi_code0 = sum_rssi/loopcnt;
        rssi_gnd = sum_gnd/loopcnt;
        rssi_vdd = sum_vdd/loopcnt;
        rssi_half0 = sum_half/loopcnt;           
        
#ifdef EUROPA_DEBUG_RXPOWER
        printf("Parameter list:\nrssi_voltage gnd_code vdd_code half_vdd_code diff \n");
        for (i=0;i<5;i++)
        {
            diff = abs((int32)p_rx_arr[i].rssi_v-(int32)rssi_v0);
            printf("[%d]: %d 0x%x 0x%x 0x%x diff = %d\n", i, p_rx_arr[i].rssi_v, p_rx_arr[i].gnd_code, p_rx_arr[i].vdd_code, p_rx_arr[i].half_vdd_code, diff);          
            if (diff > (rssi_v0*2/10))
            {
                printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = %d\n", i, p_rx_arr[i].rssi_v, rssi_v0);
            }             
        }
            
        for (i=0;i<5;i++)
        {
            diff = abs((int32)p_rx_arr[i+loopcnt+5].rssi_v-(int32)rssi_v0);
            printf("[%d]: %d 0x%x 0x%x 0x%x diff = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, p_rx_arr[i+loopcnt+5].gnd_code, p_rx_arr[i+loopcnt+5].vdd_code, p_rx_arr[i+loopcnt+5].half_vdd_code, diff);
            if (diff > (rssi_v0*2/10))
            {
                printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, rssi_v0);
            }
        }
#endif                    
        _europa_cal_flash_data_set(RTL8290B_RSSI_V0_ADDR, 4, rssi_v0);          
        addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_RSSI_V0_2_OFFSET_ADDR;          
        _europa_cal_flash_data_set(addr, 4, rssi_v0);       
        printf("Get RSSI V0 = %d\n", rssi_v0);
        free(p_rx_arr);         
    }    
    else if ((rx_power_mode == 1)||(rx_power_mode == 3))
    {
        //ldd_param->state == 0: Normal mode, output one rssi voltage (Can be changed by SD suggestion)
        //ldd_param->state == 1: Use Old algorithm(from rtl8290
        //ldd_param->state == 2: Use New algorithm(with RSSI-K)
        //ldd_param->state == 3: use  R17~R19 to replace original RSSI Code, other's like New algorithm(with RSSI-K)
        //ldd_param->state == 4: use  R17~R19 to replace original RSSI Code, other's like Old algorithm(from rtl8290

    
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 70, &w70_data); 
        data = w70_data &(~(0x20));     
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 70, data);        
    
        offset = 10;
        //p_rx_arr = (europa_rxpwr_t *)malloc((loopcnt+offset)*sizeof(europa_rxpwr_t));    
        //memset(p_rx_arr, 0,sizeof(europa_rxpwr_t)*(loopcnt+offset));
        p_rx_arr = (europa_rxpwr_t *)malloc(sizeof(europa_rxpwr_t)*110);    
        memset(p_rx_arr, 0,sizeof(europa_rxpwr_t)*110);

        //Step1. set prbs off
        //printf("RSSI V0: PRBS close\n");
        //if ((ret = reg_write(0xBB040098, 0)) != 0)/*disable PRBS*/ 
        //{
        //    return ret;
        //}
    
        //Step2. disable TEMP_INT (A4/89=0xFF), set DOL loop
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 89, 0xFF);
    
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x7C, &w80_data);        
        mode = LDD_LOOP_DOL;
        ret = rtk_ldd_loopMode_set(mode);
        if(ret)
        {
            printf("Set Loop Mode Fail!!! (%d)\n", ret);
        }    
        
        //Step3. set Imod=5mA 
        ret = rtk_ldd_tx_mod_get(&ori_Imod);
        if(ret)
        {
            printf("rtk_ldd_tx_mod_get Fail!!! (%d)\n", ret);
        }
        
        Imod = (5 * 1000 / 2); /* convert mA to 2uA unit */
        ret = rtk_ldd_tx_mod_set(Imod);
        if(ret)
        {
            printf("Set TX Mod = %d Fail!!! (%d)\n", Imod, ret);
        }

        if ((w68_data&0x40)!=0)
        {
            ret = rtk_ldd_tx_mod_get(&Imod);   
            if(ret)
            {
                printf("rtk_ldd_tx_mod_get Fail!!! (%d)\n", ret);
            }            
            printf("Get TX Mod = %d \n", Imod);
        }
        
        osal_time_mdelay(500); 
    
        //Step4. get Crssi0, Chalf_vdd0, save Cgnd0, Cvdd0, calculate RSSIV0    
        for (i=0;i<(loopcnt+offset);i++)
        {       
            memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
            if (rx_power_mode == 1)
            {
                ldd_cfg.state = 2;
            }
            else
            {
                 ldd_cfg.state = 3;
            }
            ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
            if(ret)
            {
                printf("rtk_ldd_rssiVoltage_get fail!!! (%d)\n", ret);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, w59_data);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, w5a_data);                
                free(p_rx_arr);                    
                return -1;
            }
            p_rx_arr[i].rssi_i = ldd_cfg.rssi_voltage;            
            p_rx_arr[i].rssi_v = ldd_cfg.rssi_voltage2;
            p_rx_arr[i].rssi_code = ldd_cfg.rssi_code;            
            p_rx_arr[i].gnd_code = ldd_cfg.gnd_code;
            p_rx_arr[i].vdd_code = ldd_cfg.vdd_code;  
            p_rx_arr[i].half_vdd_code = ldd_cfg.half_vdd_code;            
            p_rx_arr[i].rssi_code2 = ldd_cfg.driver_version;                
            if ((w68_data&0x40)!=0)
            {
                printf("%8d (0x%x) %8d (0x%x) %8d (0x%x) %8d (0x%x) %8d (0x%x) \n", 
                ldd_cfg.rssi_code, ldd_cfg.rssi_code, ldd_cfg.half_vdd_code, ldd_cfg.half_vdd_code,
                ldd_cfg.vdd_code, ldd_cfg.vdd_code, ldd_cfg.gnd_code, ldd_cfg.gnd_code, 
                ldd_cfg.rssi_voltage2, ldd_cfg.rssi_voltage2); 
            }
            
        }

        _europa_rxbubble_sort(p_rx_arr, loopcnt+offset);
    
        rssi_v0 = 0;
        rssi_gnd = 0;
        rssi_vdd = 0;
        rssi_half0 = 0;
        rssi_code0 = 0;

        sum_ori = 0;
        sum = 0;
        sum_gnd = 0;
        sum_vdd = 0;
        sum_half = 0;  
        sum_rssi = 0;  
        //printf("Parameter list:\nrssi_code gnd_code vdd_code half_vdd_code rssi_voltage\n");
         
        for (i=5;i<loopcnt+5;i++)
        {   
            sum_ori  += p_rx_arr[i].rssi_i;
            sum      += p_rx_arr[i].rssi_v;
            sum_rssi += p_rx_arr[i].rssi_code;
            sum_gnd  += p_rx_arr[i].gnd_code;
            sum_vdd  += p_rx_arr[i].vdd_code;    
            sum_half += p_rx_arr[i].half_vdd_code;                  
            //printf("Loop %d: rssi V0 = 0x%x \n", i, p_rx_arr[i].rssi_v);
        }
        rssi_v0_ori = sum_ori/loopcnt;
        rssi_v0 = sum/loopcnt;
        rssi_code0 = sum_rssi/loopcnt;
        rssi_gnd = sum_gnd/loopcnt;
        rssi_vdd = sum_vdd/loopcnt;
        rssi_half0 = sum_half/loopcnt;         

#ifdef EUROPA_DEBUG_RXPOWER
        printf("Parameter list:\nrssi_voltage gnd_code vdd_code half_vdd_code diff \n");
        for (i=0;i<5;i++)
        {
            diff = abs((int32)p_rx_arr[i].rssi_v-(int32)rssi_v0);
            printf("[%d]: %d 0x%x 0x%x 0x%x diff = %d\n", i, p_rx_arr[i].rssi_v, p_rx_arr[i].gnd_code, p_rx_arr[i].vdd_code, p_rx_arr[i].half_vdd_code, diff);        
            if (diff > (rssi_v0*2/10))
            {
                printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = %d\n", i, p_rx_arr[i].rssi_v, rssi_v0);
            }            
        }
        
        for (i=0;i<5;i++)
        {
            diff = abs((int32)p_rx_arr[i+loopcnt+5].rssi_v-(int32)rssi_v0);
             printf("[%d]: %d 0x%x 0x%x 0x%x diff = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, p_rx_arr[i+loopcnt+5].gnd_code, p_rx_arr[i+loopcnt+5].vdd_code, p_rx_arr[i+loopcnt+5].half_vdd_code, diff);
            if (diff > (rssi_v0*2/10))
            {
                printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, rssi_v0);
            }
        }
#endif
                
        _europa_cal_flash_data_set(RTL8290B_RSSI_V0_ADDR, 4, rssi_v0);    
        _europa_cal_flash_data_set(RTL8290B_GND_V0_ADDR, 4, rssi_gnd);     
        _europa_cal_flash_data_set(RTL8290B_VDD_V0_ADDR, 4, rssi_vdd); 
        _europa_cal_flash_data_set(RTL8290B_HALF_VDD_V0_ADDR, 4, rssi_half0);  
        if (rx_power_mode == 3)
        {
            addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_RSSI_V0_2_OFFSET_ADDR;          
            _europa_cal_flash_data_set(addr, 4, rssi_v0); 
        } 
        
        printf("Get RSSI V0 = %d\n", rssi_v0);

#ifdef EUROPA_DEBUG_RXPOWER    
        //printf("Get RSSI V0 = %d GND Code = 0x%x, VDD Code = 0x%x, RSSI Code0 = 0x%x, Half VDD Code0 = 0x%x\n", rssi_v0, rssi_gnd, rssi_vdd, rssi_code0, rssi_half0);
        printf("Get RSSI V0 = %d\n", rssi_v0);
#endif

        //Step5. set Imod=100mA 
        Imod = (100 * 1000 / 2); /* convert mA to 2uA unit */
        ret = rtk_ldd_tx_mod_set(Imod);
        if(ret)
        {
            printf("Set TX Mod = %d Fail!!! (%d)\n", Imod, ret);
        }
        
        if ((w68_data&0x40)!=0)
        {
            rtk_ldd_tx_mod_get(&Imod);        
            printf("Set TX Mod = %d \n", Imod);
        }    
        
        osal_time_mdelay(10); 

        for (i=0;i<(loopcnt+offset);i++)
        {        
            memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
            if (rx_power_mode == 1)
            {
                ldd_cfg.state = 2;
            }
            else
            {
                 ldd_cfg.state = 3;
            }
            ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
            if(ret)
            {
                printf("rtk_ldd_rssiVoltage_get fail!!! (%d)\n", ret);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, w59_data);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, w5a_data);                
                free(p_rx_arr);                 
                return -1;
            }
            p_rx_arr[i].rssi_i = ldd_cfg.rssi_voltage;            
            p_rx_arr[i].rssi_v = ldd_cfg.rssi_voltage2;
            p_rx_arr[i].rssi_code = ldd_cfg.rssi_code;            
            p_rx_arr[i].gnd_code = ldd_cfg.gnd_code;
            p_rx_arr[i].vdd_code = ldd_cfg.vdd_code;  
            p_rx_arr[i].half_vdd_code = ldd_cfg.half_vdd_code;            
            p_rx_arr[i].rssi_code2 = ldd_cfg.driver_version;               
            if ((w68_data&0x40)!=0)
            {
                printf("%8d (0x%x) %8d (0x%x) %8d (0x%x) %8d (0x%x) %8d (0x%x) \n", 
                ldd_cfg.rssi_code, ldd_cfg.rssi_code, ldd_cfg.half_vdd_code, ldd_cfg.half_vdd_code,
                ldd_cfg.vdd_code, ldd_cfg.vdd_code, ldd_cfg.gnd_code, ldd_cfg.gnd_code, 
                ldd_cfg.rssi_voltage2, ldd_cfg.rssi_voltage2); 
            }        
        }

        _europa_rxbubble_sort(p_rx_arr, loopcnt+offset);

        rssi_v0 = 0;
        rssi_code1 = 0;    
        rssi_gnd = 0;
        rssi_vdd = 0;
        rssi_half1 = 0;
        
        sum = 0;
        sum_rssi = 0;
        sum_gnd = 0;
        sum_vdd = 0;
        sum_half = 0;
        
//printf("Parameter list:\nrssi_code gnd_code vdd_code half_vdd_code rssi_voltage\n");
        for (i=5;i<loopcnt+5;i++)
        {
            sum      += p_rx_arr[i].rssi_v;        
            sum_rssi += p_rx_arr[i].rssi_code;
            sum_gnd  += p_rx_arr[i].gnd_code;
            sum_vdd  += p_rx_arr[i].vdd_code;      
            sum_half += p_rx_arr[i].half_vdd_code;              
        } 

        rssi_v0 = sum/loopcnt;
        rssi_code1 = sum_rssi/loopcnt;
        rssi_gnd = sum_gnd/loopcnt;
        rssi_vdd = sum_vdd/loopcnt;
        rssi_half1 = sum_half/loopcnt;          
#ifdef EUROPA_DEBUG_RXPOWER    
        printf("Parameter list:\nrssi_voltage rssi_code gnd_code vdd_code half_vdd_code diff \n");
        for (i=0;i<5;i++)
        {
            diff = abs((int32)p_rx_arr[i].rssi_v-(int32)rssi_v0);
            printf("[%d]: %d 0x%x 0x%x 0x%x 0x%x diff = %d\n", i, p_rx_arr[i].rssi_v, p_rx_arr[i].rssi_code, p_rx_arr[i].gnd_code, p_rx_arr[i].vdd_code, p_rx_arr[i].half_vdd_code, diff);        
            if (diff > (rssi_v0*2/10))
            {
                printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = %d\n", i, p_rx_arr[i].rssi_v, rssi_v0);
            }            
        }
        
        for (i=0;i<5;i++)
        {
            diff = abs((int32)p_rx_arr[i+loopcnt+5].rssi_v-(int32)rssi_v0);
            printf("[%d]: %d 0x%x 0x%x 0x%x 0x%x diff = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, p_rx_arr[i+loopcnt+5].rssi_code, p_rx_arr[i+loopcnt+5].gnd_code, p_rx_arr[i+loopcnt+5].vdd_code, p_rx_arr[i+loopcnt+5].half_vdd_code, diff);
            if (diff > (rssi_v0*2/10))
            {
                printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, rssi_v0);
            }
        }
#endif        
        
        //printf("Get RSSI Code1 = %d Half VDD Code1 = 0x%x\n", rssi_code1, rssi_half1);
    
        //Step7. calculate k = -1* (Crssi1 - Crssi0)/ (Chalf_vdd1 - Chalf_vdd0), save k   
        //The value of k can be minus or plus, and  is floating point, so multiple 1000. When used in RX power, divide 1000
        temp1 = -1*((double)rssi_code1 - (double)rssi_code0);
        //printf("temp1 = %f \n", temp1);    
        temp2 = (double)rssi_half1 - (double)rssi_half0;
        temp3 = temp1/temp2;
        rssi_k = (int)(temp3*1000);
        //printf("temp2 = %f temp3 = %f\n", temp2, temp3);

        printf("rssi_code1 = %d rssi_half1 = %d rssi_code0 = %d  rssi_half0 = %d\n", rssi_code1, rssi_half1, rssi_code0, rssi_half0);        
        _europa_cal_flash_data_set(RTL8290B_RX_K_ADDR, 4, rssi_k);  
        if (rx_power_mode == 3)
        {
            addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_RSSI_K2_OFFSET_ADDR;          
            _europa_cal_flash_data_set(addr, 4, rssi_k); 
        }        
    
        //Step8. restore loop mode, TEMP_INT ((A4/89=0xFE)
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x7C, w80_data);    
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 89, 0xFE);
        ret = rtk_ldd_tx_mod_set(ori_Imod);
        if(ret)
        {
            printf("Set TX Mod = %d Fail!!! (%d)\n", ori_Imod, ret);
        }
        
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, w59_data);
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, w5a_data);                 
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 70, w70_data);
        
        printf("Get RSSI K (x1000) = %d\n", rssi_k);            
        free(p_rx_arr);         
    }
    else
    {
        printf("RX Power Mode  = %d, error!!! \n", rx_power_mode);   
    }
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_rssiv0_get Duration = %f ms\n", Duration);
#endif       
    return 0;     
}

#if 0
static int europa_cli_rssiv0_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 rssi_v0, rssi_gnd, rssi_vdd, rssi_half0, rssi_half1, rssi_v0_ori;
    uint32 loopcnt, i;
    uint64 sum, sum_gnd, sum_vdd, sum_half, sum_rssi, sum_ori;
    uint32 index;
    uint32 w83_data, w80_data, w70_data, data;
    rtk_ldd_cfg_t ldd_cfg;
    rtk_ldd_loop_mode_t mode;
    uint32 Imod, ori_Imod;
    uint32 rssi_code1, rssi_code0;
    double temp1, temp2, temp3;
    int32 temp32, rssi_k;
    int32 rx_power_mode;

    europa_rxpwr_t *p_rx_arr;
    int32 offset, diff;

    uint32 addr;
    int16 temp_0, temp_0_offset;
    uint16 temp_now;
    uint32 w59_data, w5a_data, w68_data;

    uint32 rssi_code21, rssi_code20;
    int32 rssi_k2;
    uint64 sum_rssi2;
#ifdef RTL8290B_CAL_TIME
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
#endif
    
    if(argc < 2)
    { 
        printf("%s <loop count> \n", argv[0]);
        return -1;
    }

    loopcnt = _vlaue_translate(argv[1]);

    if (100 < loopcnt)
    {
         printf("Loop Count should less than 100. \n");
         return -1;
    }
    
    printf("europa_cli_rssiv0_get:\n"); 

    
#ifdef RTL8290B_CAL_TIME    
    gettimeofday(&tv_tmp1,&tz); 
 #endif   
    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);
    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x59, &w59_data);    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, 0xFF);
    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x5A, &w5a_data);    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, 0xFF);

    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x68, &w68_data);  

    ret = rtk_ldd_temperature_get(&temp_now);
    if(ret)
    {
        printf("Get Temperature Fail!!! (%d)\n", ret);
        return -1;
    }    

    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_TEMP0_OFFSET_ADDR;    
    _europa_cal_flash_data_set(addr, 2, (uint32)temp_now);    

    //Set temp 0 offset to 20
    temp_0_offset = 20;
    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_TEMP0_OFF_OFFSET_ADDR;        
    _europa_cal_flash_data_set(addr, 2, (uint32)temp_0_offset);    

    rx_power_mode = 1;

    _europa_cal_flash_data_get(RTL8290B_RSSI_V0_ADDR, 4, &rssi_v0);    
    _europa_cal_flash_data_get(RTL8290B_GND_V0_ADDR, 4, &rssi_gnd);     
    _europa_cal_flash_data_get(RTL8290B_VDD_V0_ADDR, 4, &rssi_vdd); 
    _europa_cal_flash_data_get(RTL8290B_HALF_VDD_V0_ADDR, 4, &rssi_half0); 
    _europa_cal_flash_data_get(RTL8290B_RX_K_ADDR, 4, &rssi_k); 

    
    if (rssi_v0 == 0)
    {
        rx_power_mode = 1;
    }
    else
    {
        if ((rssi_k == 0)&&(rssi_gnd == 0)&&(rssi_vdd == 0)&&(rssi_half0 == 0))
        {
            rx_power_mode = 0;
        }
        else
        {
            rx_power_mode = 1;
        }
    }
    
    if (rx_power_mode == 0)
    {

        rssi_v0 = 0;
        sum = 0;
        
        for (i=0;i<loopcnt;i++)
        {
            //printf("Loop %d: \n", i);
            ret = rtk_ldd_rssiV0_get(&rssi_v0);
            if(ret)
            {
                printf("Get RSSI V0 fail!!! (%d)\n", ret);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, w59_data);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, w5a_data);                
                return -1;
            }
            sum += rssi_v0;
            //printf("Loop %d: rssi V0 = 0x%x \n", i, rssi_v0);
        }
         
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, w59_data);
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, w5a_data);    
        
        rssi_v0 = sum/loopcnt;
         
        _europa_cal_flash_data_set(RTL8290B_RSSI_V0_ADDR, 4, rssi_v0);          
        
        printf("Get RSSI V0 = %d\n", rssi_v0);
        
    }
    else if (rx_power_mode == 1)
    { 
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 70, &w70_data); 
        data = w70_data &(~(0x20));     
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 70, data);        
    
        offset = 10;
        p_rx_arr = (europa_rxpwr_t *)malloc((loopcnt+offset)*sizeof(europa_rxpwr_t));    
        memset(p_rx_arr, 0,sizeof(europa_rxpwr_t)*(loopcnt+offset));

        //Step1. set prbs off
        //printf("RSSI V0: PRBS close\n");
        //if ((ret = reg_write(0xBB040098, 0)) != 0)/*disable PRBS*/ 
        //{
        //    return ret;
        //}
    
        //Step2. disable TEMP_INT (A4/89=0xFF), set DOL loop
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 89, 0xFF);
    
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x7C, &w80_data);        
        mode = LDD_LOOP_DOL;
        ret = rtk_ldd_loopMode_set(mode);
        if(ret)
        {
            printf("Set Loop Mode Fail!!! (%d)\n", ret);
        }    
        
        //Step3. set Imod=5mA 
        ret = rtk_ldd_tx_mod_get(&ori_Imod);
        if(ret)
        {
            printf("rtk_ldd_tx_mod_get Fail!!! (%d)\n", ret);
        }
        
        Imod = (5 * 1000 / 2); /* convert mA to 2uA unit */
        ret = rtk_ldd_tx_mod_set(Imod);
        if(ret)
        {
            printf("Set TX Mod = %d Fail!!! (%d)\n", Imod, ret);
        }

        if ((w68_data&0x40)!=0)
        {
            ret = rtk_ldd_tx_mod_get(&Imod);   
            if(ret)
            {
                printf("rtk_ldd_tx_mod_get Fail!!! (%d)\n", ret);
            }            
            printf("Get TX Mod = %d \n", Imod);
        }
        
        osal_time_mdelay(500); 
    
        //Step4. get Crssi0, Chalf_vdd0, save Cgnd0, Cvdd0, calculate RSSIV0    
        for (i=0;i<(loopcnt+offset);i++)
        {       
            memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
            ldd_cfg.state = 0;
            ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
            if(ret)
            {
                printf("rtk_ldd_rssiVoltage_get fail!!! (%d)\n", ret);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, w59_data);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, w5a_data);                
                free(p_rx_arr);                    
                return -1;
            }
            p_rx_arr[i].rssi_i = ldd_cfg.rssi_voltage;            
            p_rx_arr[i].rssi_v = ldd_cfg.rssi_voltage2;
            p_rx_arr[i].rssi_code = ldd_cfg.rssi_code;            
            p_rx_arr[i].gnd_code = ldd_cfg.gnd_code;
            p_rx_arr[i].vdd_code = ldd_cfg.vdd_code;  
            p_rx_arr[i].half_vdd_code = ldd_cfg.half_vdd_code;            
            p_rx_arr[i].rssi_code2 = ldd_cfg.driver_version;                
            if ((w68_data&0x40)!=0)
            {
                printf("%8d (0x%x) %8d (0x%x) %8d (0x%x) %8d (0x%x) %8d (0x%x) \n", 
                ldd_cfg.rssi_code, ldd_cfg.rssi_code, ldd_cfg.half_vdd_code, ldd_cfg.half_vdd_code,
                ldd_cfg.vdd_code, ldd_cfg.vdd_code, ldd_cfg.gnd_code, ldd_cfg.gnd_code, 
                ldd_cfg.rssi_voltage2, ldd_cfg.rssi_voltage2); 
            }
            
        }

        _europa_rxbubble_sort(p_rx_arr, loopcnt+offset);
    
        rssi_v0 = 0;
        rssi_gnd = 0;
        rssi_vdd = 0;
        rssi_half0 = 0;
        rssi_code0 = 0;

        sum_ori = 0;
        sum = 0;
        sum_gnd = 0;
        sum_vdd = 0;
        sum_half = 0;  
        sum_rssi = 0;
        sum_rssi2 = 0;
    
        //printf("Parameter list:\nrssi_code gnd_code vdd_code half_vdd_code rssi_voltage\n");
         
        for (i=5;i<loopcnt+5;i++)
        {   
            sum_ori  += p_rx_arr[i].rssi_i;
            sum      += p_rx_arr[i].rssi_v;
            sum_rssi += p_rx_arr[i].rssi_code;
            sum_gnd  += p_rx_arr[i].gnd_code;
            sum_vdd  += p_rx_arr[i].vdd_code;    
            sum_half += p_rx_arr[i].half_vdd_code;        
            sum_rssi2 += p_rx_arr[i].rssi_code2;            
            //printf("Loop %d: rssi V0 = 0x%x \n", i, p_rx_arr[i].rssi_v);
        }
        rssi_v0_ori = sum_ori/loopcnt;
        rssi_v0 = sum/loopcnt;
        rssi_code0 = sum_rssi/loopcnt;
        rssi_gnd = sum_gnd/loopcnt;
        rssi_vdd = sum_vdd/loopcnt;
        rssi_half0 = sum_half/loopcnt;     
        rssi_code20 = sum_rssi2/loopcnt;        

#ifdef EUROPA_DEBUG_RXPOWER
        printf("Parameter list:\nrssi_voltage gnd_code vdd_code half_vdd_code diff \n");
        for (i=0;i<5;i++)
        {
            diff = abs((int32)p_rx_arr[i].rssi_v-(int32)rssi_v0);
            printf("[%d]: %d 0x%x 0x%x 0x%x diff = %d\n", i, p_rx_arr[i].rssi_v, p_rx_arr[i].gnd_code, p_rx_arr[i].vdd_code, p_rx_arr[i].half_vdd_code, diff);        
            if (diff > (rssi_v0*2/10))
            {
                printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = %d\n", i, p_rx_arr[i].rssi_v, rssi_v0);
            }            
        }
        
        for (i=0;i<5;i++)
        {
            diff = abs((int32)p_rx_arr[i+loopcnt+5].rssi_v-(int32)rssi_v0);
             printf("[%d]: %d 0x%x 0x%x 0x%x diff = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, p_rx_arr[i+loopcnt+5].gnd_code, p_rx_arr[i+loopcnt+5].vdd_code, p_rx_arr[i+loopcnt+5].half_vdd_code, diff);
            if (diff > (rssi_v0*2/10))
            {
                printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, rssi_v0);
            }
        }
#endif
        
#if 0      
        rssi_v0 = 0;
        rssi_gnd = 0;
        rssi_vdd = 0;
        rssi_half0 = 0;
        rssi_code0 = 0;

        sum_ori = 0;
        sum = 0;
        sum_gnd = 0;
        sum_vdd = 0;
        sum_half = 0;  
        sum_rssi = 0;
    
        //printf("Parameter list:\nrssi_code gnd_code vdd_code half_vdd_code rssi_voltage\n");
         
        for (i=0;i<loopcnt;i++)
        {
        
            memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
            ldd_cfg.state = 0;
            ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
            if(ret)
            {
                printf("rtk_ldd_rssiVoltage_get fail!!! (%d)\n", ret);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
                return -1;
            }
            //printf("0x%x  0x%x 0x%x 0x%x      0x%x \n", ldd_cfg.rssi_code, ldd_cfg.gnd_code, ldd_cfg.vdd_code, ldd_cfg.half_vdd_code, ldd_cfg.rssi_voltage2);        
            sum_ori += ldd_cfg.rssi_voltage;
            sum += ldd_cfg.rssi_voltage2;
            sum_rssi += ldd_cfg.rssi_code;
            sum_gnd += ldd_cfg.gnd_code;
            sum_vdd += ldd_cfg.vdd_code;    
            sum_half += ldd_cfg.half_vdd_code;        
            //printf("Loop %d: rssi V0 = 0x%x \n", i, rssi_v0);
        }
        rssi_v0_ori = sum_ori/loopcnt;
        rssi_v0 = sum/loopcnt;
        rssi_code0 = sum_rssi/loopcnt;
        rssi_gnd = sum_gnd/loopcnt;
        rssi_vdd = sum_vdd/loopcnt;
        rssi_half0 = sum_half/loopcnt;
#endif         
        _europa_cal_flash_data_set(RTL8290B_RSSI_V0_ADDR, 4, rssi_v0);    
        _europa_cal_flash_data_set(RTL8290B_GND_V0_ADDR, 4, rssi_gnd);     
        _europa_cal_flash_data_set(RTL8290B_VDD_V0_ADDR, 4, rssi_vdd); 
        _europa_cal_flash_data_set(RTL8290B_HALF_VDD_V0_ADDR, 4, rssi_half0);  
    
        addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_RSSI_V0_2_OFFSET_ADDR;         
        _europa_cal_flash_data_set(addr, 4, rssi_v0_ori);   


#ifdef EUROPA_DEBUG_RXPOWER    
        //printf("Get RSSI V0 = %d GND Code = 0x%x, VDD Code = 0x%x, RSSI Code0 = 0x%x, Half VDD Code0 = 0x%x\n", rssi_v0, rssi_gnd, rssi_vdd, rssi_code0, rssi_half0);
        printf("Get RSSI V0 = %d, Original RSSI V0 = %d\n", rssi_v0, rssi_v0_ori);
#else
        printf("Get RSSI V0 = %d, RSSI V0-2 = %d\n", rssi_v0, rssi_v0_ori);
#endif

    
        //Step5. set Imod=100mA 
        Imod = (100 * 1000 / 2); /* convert mA to 2uA unit */
        ret = rtk_ldd_tx_mod_set(Imod);
        if(ret)
        {
            printf("Set TX Mod = %d Fail!!! (%d)\n", Imod, ret);
        }
        
        if ((w68_data&0x40)!=0)
        {
            rtk_ldd_tx_mod_get(&Imod);        
            printf("Set TX Mod = %d \n", Imod);
        }    
        
        osal_time_mdelay(10); 

        for (i=0;i<(loopcnt+offset);i++)
        {        
            memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
            ldd_cfg.state = 0;
            ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
            if(ret)
            {
                printf("rtk_ldd_rssiVoltage_get fail!!! (%d)\n", ret);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, w59_data);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, w5a_data);                
                free(p_rx_arr);                 
                return -1;
            }
            p_rx_arr[i].rssi_i = ldd_cfg.rssi_voltage;            
            p_rx_arr[i].rssi_v = ldd_cfg.rssi_voltage2;
            p_rx_arr[i].rssi_code = ldd_cfg.rssi_code;            
            p_rx_arr[i].gnd_code = ldd_cfg.gnd_code;
            p_rx_arr[i].vdd_code = ldd_cfg.vdd_code;  
            p_rx_arr[i].half_vdd_code = ldd_cfg.half_vdd_code;            
            p_rx_arr[i].rssi_code2 = ldd_cfg.driver_version;               
            if ((w68_data&0x40)!=0)
            {
                printf("%8d (0x%x) %8d (0x%x) %8d (0x%x) %8d (0x%x) %8d (0x%x) \n", 
                ldd_cfg.rssi_code, ldd_cfg.rssi_code, ldd_cfg.half_vdd_code, ldd_cfg.half_vdd_code,
                ldd_cfg.vdd_code, ldd_cfg.vdd_code, ldd_cfg.gnd_code, ldd_cfg.gnd_code, 
                ldd_cfg.rssi_voltage2, ldd_cfg.rssi_voltage2); 
            }        
        }

        _europa_rxbubble_sort(p_rx_arr, loopcnt+offset);

        rssi_v0 = 0;
        rssi_code1 = 0;    
        rssi_gnd = 0;
        rssi_vdd = 0;
        rssi_half1 = 0;
        
        sum = 0;
        sum_rssi = 0;
        sum_gnd = 0;
        sum_vdd = 0;
        sum_half = 0;
        sum_rssi2 = 0;
        
//printf("Parameter list:\nrssi_code gnd_code vdd_code half_vdd_code rssi_voltage\n");
        for (i=5;i<loopcnt+5;i++)
        {
            sum      += p_rx_arr[i].rssi_v;        
            sum_rssi += p_rx_arr[i].rssi_code;
            sum_gnd  += p_rx_arr[i].gnd_code;
            sum_vdd  += p_rx_arr[i].vdd_code;      
            sum_half += p_rx_arr[i].half_vdd_code;    
            sum_rssi2 += p_rx_arr[i].rssi_code2;            
        } 

        rssi_v0 = sum/loopcnt;
        rssi_code1 = sum_rssi/loopcnt;
        rssi_gnd = sum_gnd/loopcnt;
        rssi_vdd = sum_vdd/loopcnt;
        rssi_half1 = sum_half/loopcnt;   
        rssi_code21 = sum_rssi2/loopcnt;        
#ifdef EUROPA_DEBUG_RXPOWER    
        printf("Parameter list:\nrssi_voltage rssi_code gnd_code vdd_code half_vdd_code diff \n");
        for (i=0;i<5;i++)
        {
            diff = abs((int32)p_rx_arr[i].rssi_v-(int32)rssi_v0);
            printf("[%d]: %d 0x%x 0x%x 0x%x 0x%x diff = %d\n", i, p_rx_arr[i].rssi_v, p_rx_arr[i].rssi_code, p_rx_arr[i].gnd_code, p_rx_arr[i].vdd_code, p_rx_arr[i].half_vdd_code, diff);        
            if (diff > (rssi_v0*2/10))
            {
                printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = %d\n", i, p_rx_arr[i].rssi_v, rssi_v0);
            }            
        }
        
        for (i=0;i<5;i++)
        {
            diff = abs((int32)p_rx_arr[i+loopcnt+5].rssi_v-(int32)rssi_v0);
            printf("[%d]: %d 0x%x 0x%x 0x%x 0x%x diff = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, p_rx_arr[i+loopcnt+5].rssi_code, p_rx_arr[i+loopcnt+5].gnd_code, p_rx_arr[i+loopcnt+5].vdd_code, p_rx_arr[i+loopcnt+5].half_vdd_code, diff);
            if (diff > (rssi_v0*2/10))
            {
                printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, rssi_v0);
            }
        }
#endif        
        
#if 0    
        rssi_v0 = 0;
        rssi_code1 = 0;    
        rssi_gnd = 0;
        rssi_vdd = 0;
        rssi_half1 = 0;
        
        sum = 0;
        sum_rssi = 0;
        sum_gnd = 0;
        sum_vdd = 0;
        sum_half = 0;       
        //printf("Parameter list: rssi_code gnd_code vdd_code ldo_code half_vdd_code rset_code rssi_voltage rssi_voltage2\n");
              
        for (i=0;i<loopcnt;i++)
        {    
            memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
            if(ret)
            {
                printf("rtk_ldd_rssiVoltage_get fail!!! (%d)\n", ret);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
                free(p_rx_arr);                    
                return -1;
            }
            //printf("0x%x  0x%x 0x%x 0x%x      0x%x \n", ldd_cfg.rssi_code, ldd_cfg.gnd_code, ldd_cfg.vdd_code, ldd_cfg.half_vdd_code, ldd_cfg.rssi_voltage2);         
            //sum += ldd_cfg.rssi_code;
            sum_rssi += ldd_cfg.rssi_code;        
            sum_gnd += ldd_cfg.gnd_code;
            sum_vdd += ldd_cfg.vdd_code;    
            sum_half += ldd_cfg.half_vdd_code;        
                //printf("Loop %d: rssi V0 = 0x%x \n", i, rssi_v0);
        }
        rssi_code1 = sum_rssi/loopcnt;
        rssi_gnd = sum_gnd/loopcnt;
        rssi_vdd = sum_vdd/loopcnt;
        rssi_half1 = sum_half/loopcnt;
#endif

        //printf("Get RSSI Code1 = %d Half VDD Code1 = 0x%x\n", rssi_code1, rssi_half1);
    
        //Step7. calculate k = -1* (Crssi1 - Crssi0)/ (Chalf_vdd1 - Chalf_vdd0), save k   
        //The value of k can be minus or plus, and  is floating point, so multiple 1000. When used in RX power, divide 1000
        temp1 = -1*((double)rssi_code1 - (double)rssi_code0);
        //printf("temp1 = %f \n", temp1);    
        temp2 = (double)rssi_half1 - (double)rssi_half0;
        temp3 = temp1/temp2;
        rssi_k = (int)(temp3*1000);
        //printf("temp2 = %f temp3 = %f\n", temp2, temp3);
        _europa_cal_flash_data_set(RTL8290B_RX_K_ADDR, 4, rssi_k);  
    
        temp1 = -1*((double)rssi_code21 - (double)rssi_code20);
        temp2 = (double)rssi_half1 - (double)rssi_half0;
        temp3 = temp1/temp2;
        rssi_k2 = (int)(temp3*1000);        
        addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_RSSI_K2_OFFSET_ADDR;         
        _europa_cal_flash_data_set(addr, 4, rssi_k2);         
    
        //Step8. restore loop mode, TEMP_INT ((A4/89=0xFE)
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x7C, w80_data);    
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 89, 0xFE);
        ret = rtk_ldd_tx_mod_set(ori_Imod);
        if(ret)
        {
            printf("Set TX Mod = %d Fail!!! (%d)\n", ori_Imod, ret);
        }
        
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, w59_data);
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, w5a_data);        

        free(p_rx_arr);    
            
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 70, w70_data);
        
        printf("Get RSSI K (x1000) = %d, RSSI K2 (x1000) = %d \n", rssi_k, rssi_k2);            
    }
    else
    {
        printf("RX Power Mode  = %d, error!!! \n", rx_power_mode);   
    }
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_rssiv0_get Duration = %f ms\n", Duration);
#endif       
    return 0;     
}
#endif

static int europa_cli_rssiv0_2_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 rssi_v0, rssi_gnd, rssi_vdd, rssi_half0, rssi_half1, rssi_v0_ori;
    uint32 loopcnt, i;
    uint64 sum, sum_gnd, sum_vdd, sum_half, sum_rssi, sum_ori;
    uint32 index;
    uint32 w83_data, w80_data, w70_data, data;
    rtk_ldd_cfg_t ldd_cfg;
    rtk_ldd_loop_mode_t mode;
    uint32 Imod, ori_Imod;
    uint32 rssi_code1, rssi_code0;
    double temp1, temp2, temp3;
    int32 temp32, rssi_k;
    int32 rx_power_mode;

    europa_rxpwr_t *p_rx_arr;
    int32 offset, diff;

    uint32 addr;
    int16 temp_0, temp_0_offset;
    uint16 temp_now;
    uint32 w59_data, w5a_data, w68_data;

    uint32 rssi_code21, rssi_code20;
    int32 rssi_k2;
    uint64 sum_rssi2;
#ifdef RTL8290B_CAL_TIME
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
#endif
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if(argc < 2)
    { 
        printf("%s <loop count> \n", argv[0]);
        return -1;
    }

    loopcnt = _vlaue_translate(argv[1]);

    if (100 < loopcnt)
    {
         printf("Loop Count should less than 100. \n");
         return -1;
    }

    printf("europa_cli_rssiv0_2_get:\n"); 

#ifdef RTL8290B_CAL_TIME    
    gettimeofday(&tv_tmp1,&tz); 
 #endif   
    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);
    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x59, &w59_data);    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, 0xFF);
    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x5A, &w5a_data);    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, 0xFF);

    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x68, &w68_data);  

    ret = rtk_ldd_temperature_get(&temp_now);
    if(ret)
    {
        printf("Get Temperature Fail!!! (%d)\n", ret);
        return -1;
    }    

    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_TEMP0_OFFSET_ADDR;    
    _europa_cal_flash_data_set(addr, 2, (uint32)temp_now);    

    //Set temp 0 offset to 20
    temp_0_offset = 20;
    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_TEMP0_OFF_OFFSET_ADDR;        
    _europa_cal_flash_data_set(addr, 2, (uint32)temp_0_offset);    

    rx_power_mode = 1;

    _europa_cal_flash_data_get(RTL8290B_RSSI_V0_ADDR, 4, &rssi_v0);    
    _europa_cal_flash_data_get(RTL8290B_GND_V0_ADDR, 4, &rssi_gnd);     
    _europa_cal_flash_data_get(RTL8290B_VDD_V0_ADDR, 4, &rssi_vdd); 
    _europa_cal_flash_data_get(RTL8290B_HALF_VDD_V0_ADDR, 4, &rssi_half0); 
    _europa_cal_flash_data_get(RTL8290B_RX_K_ADDR, 4, &rssi_k); 

    
    if (rssi_v0 == 0)
    {
        rx_power_mode = 1;
    }
    else
    {
        if ((rssi_k == 0)&&(rssi_gnd == 0)&&(rssi_vdd == 0)&&(rssi_half0 == 0))
        {
            rx_power_mode = 0;
        }
        else
        {
            rx_power_mode = 1;
        }
    }
    
    if (rx_power_mode == 0)
    {

        rssi_v0 = 0;
        sum = 0;
        
        for (i=0;i<loopcnt;i++)
        {
            //printf("Loop %d: \n", i);
            ret = rtk_ldd_rssiV0_get(&rssi_v0);
            if(ret)
            {
                printf("Get RSSI V0 fail!!! (%d)\n", ret);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, w59_data);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, w5a_data);                
                return -1;
            }
            sum += rssi_v0;
            //printf("Loop %d: rssi V0 = 0x%x \n", i, rssi_v0);
        }
         
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, w59_data);
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, w5a_data);    
        
        rssi_v0 = sum/loopcnt;
         
        _europa_cal_flash_data_set(RTL8290B_RSSI_V0_ADDR, 4, rssi_v0);          
        
        printf("Get RSSI V0 = %d\n", rssi_v0);
        
    }
    else if (rx_power_mode == 1)
    { 
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 70, &w70_data); 
        data = w70_data &(~(0x20));     
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 70, data);        
    
        offset = 10;
        //p_rx_arr = (europa_rxpwr_t *)malloc((loopcnt+offset)*sizeof(europa_rxpwr_t));    
        //memset(p_rx_arr, 0,sizeof(europa_rxpwr_t)*(loopcnt+offset));
        p_rx_arr = (europa_rxpwr_t *)malloc(sizeof(europa_rxpwr_t)*110);    
        memset(p_rx_arr, 0,sizeof(europa_rxpwr_t)*110);

        //Step1. set prbs off
        //printf("RSSI V0: PRBS close\n");
        //if ((ret = reg_write(0xBB040098, 0)) != 0)/*disable PRBS*/ 
        //{
        //    return ret;
        //}
    
        //Step2. disable TEMP_INT (A4/89=0xFF), set DOL loop
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 89, 0xFF);
    
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x7C, &w80_data);        
        mode = LDD_LOOP_DOL;
        ret = rtk_ldd_loopMode_set(mode);
        if(ret)
        {
            printf("Set Loop Mode Fail!!! (%d)\n", ret);
        }    
        
        //Step3. set Imod=5mA 
        ret = rtk_ldd_tx_mod_get(&ori_Imod);
        if(ret)
        {
            printf("rtk_ldd_tx_mod_get Fail!!! (%d)\n", ret);
        }
        
        Imod = (5 * 1000 / 2); /* convert mA to 2uA unit */
        ret = rtk_ldd_tx_mod_set(Imod);
        if(ret)
        {
            printf("Set TX Mod = %d Fail!!! (%d)\n", Imod, ret);
        }

        if ((w68_data&0x40)!=0)
        {
            rtk_ldd_tx_mod_get(&Imod);        
            printf("Set TX Mod = %d \n", Imod);
        }

        osal_time_mdelay(500); 
    
        //Step4. get Crssi0, Chalf_vdd0, save Cgnd0, Cvdd0, calculate RSSIV0    
        for (i=0;i<(loopcnt+offset);i++)
        {       
            memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
            ldd_cfg.state = 0xF2;
            ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
            if(ret)
            {
                printf("rtk_ldd_rssiVoltage_get fail!!! (%d)\n", ret);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, w59_data);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, w5a_data);                
                free(p_rx_arr);                    
                return -1;
            }

            p_rx_arr[i].rssi_i = ldd_cfg.rssi_voltage;            
            p_rx_arr[i].rssi_v = ldd_cfg.rssi_voltage2;
            p_rx_arr[i].rssi_code = ldd_cfg.rssi_code;            
            p_rx_arr[i].gnd_code = ldd_cfg.gnd_code;
            p_rx_arr[i].vdd_code = ldd_cfg.vdd_code;  
            p_rx_arr[i].half_vdd_code = ldd_cfg.half_vdd_code;            
            p_rx_arr[i].rssi_code2 = ldd_cfg.driver_version;                
            //if ((w68_data&0x40)!=0)
            //{
                printf("%8d (0x%x) %8d (0x%x) %8d (0x%x) %8d (0x%x) %8d (0x%x) %8d (0x%x) %8d (0x%x)\n", 
                ldd_cfg.rssi_code, ldd_cfg.rssi_code, ldd_cfg.driver_version, ldd_cfg.driver_version,
                ldd_cfg.half_vdd_code, ldd_cfg.half_vdd_code, ldd_cfg.vdd_code, ldd_cfg.vdd_code, 
                ldd_cfg.gnd_code, ldd_cfg.gnd_code, ldd_cfg.rssi_voltage, ldd_cfg.rssi_voltage, 
                ldd_cfg.rssi_voltage2, ldd_cfg.rssi_voltage2); 
            //}
            
        }

        _europa_rxbubble_sort(p_rx_arr, loopcnt+offset);
    
        rssi_v0 = 0;
        rssi_gnd = 0;
        rssi_vdd = 0;
        rssi_half0 = 0;
        rssi_code0 = 0;

        sum_ori = 0;
        sum = 0;
        sum_gnd = 0;
        sum_vdd = 0;
        sum_half = 0;  
        sum_rssi = 0;
        sum_rssi2 = 0;
    
        //printf("Parameter list:\nrssi_code gnd_code vdd_code half_vdd_code rssi_voltage\n");
         
        for (i=5;i<loopcnt+5;i++)
        {   
            sum_ori  += p_rx_arr[i].rssi_i;
            sum      += p_rx_arr[i].rssi_v;
            sum_rssi += p_rx_arr[i].rssi_code;
            sum_gnd  += p_rx_arr[i].gnd_code;
            sum_vdd  += p_rx_arr[i].vdd_code;    
            sum_half += p_rx_arr[i].half_vdd_code;        
            sum_rssi2 += p_rx_arr[i].rssi_code2;            
            //printf("Loop %d: rssi V0 = 0x%x \n", i, p_rx_arr[i].rssi_v);
        }
        rssi_v0_ori = sum_ori/loopcnt;
        rssi_v0 = sum/loopcnt;
        rssi_code0 = sum_rssi/loopcnt;
        rssi_gnd = sum_gnd/loopcnt;
        rssi_vdd = sum_vdd/loopcnt;
        rssi_half0 = sum_half/loopcnt;     
        rssi_code20 = sum_rssi2/loopcnt;        

#ifdef EUROPA_DEBUG_RXPOWER
        printf("Parameter list:\nrssi_voltage gnd_code vdd_code half_vdd_code diff \n");
        for (i=0;i<5;i++)
        {
            diff = abs((int32)p_rx_arr[i].rssi_v-(int32)rssi_v0);
            printf("[%d]: %d 0x%x 0x%x 0x%x diff = %d\n", i, p_rx_arr[i].rssi_v, p_rx_arr[i].gnd_code, p_rx_arr[i].vdd_code, p_rx_arr[i].half_vdd_code, diff);        
            if (diff > (rssi_v0*2/10))
            {
                printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = %d\n", i, p_rx_arr[i].rssi_v, rssi_v0);
            }            
        }
        
        for (i=0;i<5;i++)
        {
            diff = abs((int32)p_rx_arr[i+loopcnt+5].rssi_v-(int32)rssi_v0);
             printf("[%d]: %d 0x%x 0x%x 0x%x diff = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, p_rx_arr[i+loopcnt+5].gnd_code, p_rx_arr[i+loopcnt+5].vdd_code, p_rx_arr[i+loopcnt+5].half_vdd_code, diff);
            if (diff > (rssi_v0*2/10))
            {
                printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, rssi_v0);
            }
        }
#endif 

        _europa_cal_flash_data_set(RTL8290B_RSSI_V0_ADDR, 4, rssi_v0_ori);    
        _europa_cal_flash_data_set(RTL8290B_GND_V0_ADDR, 4, rssi_gnd);     
        _europa_cal_flash_data_set(RTL8290B_VDD_V0_ADDR, 4, rssi_vdd); 
        _europa_cal_flash_data_set(RTL8290B_HALF_VDD_V0_ADDR, 4, rssi_half0);  
    
        addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_RSSI_V0_2_OFFSET_ADDR;         
        _europa_cal_flash_data_set(addr, 4, rssi_v0);   


#ifdef EUROPA_DEBUG_RXPOWER    
        //printf("Get RSSI V0 = %d GND Code = 0x%x, VDD Code = 0x%x, RSSI Code0 = 0x%x, Half VDD Code0 = 0x%x\n", rssi_v0, rssi_gnd, rssi_vdd, rssi_code0, rssi_half0);
        printf("Get RSSI V0 = %d, Original RSSI V0 = %d\n", rssi_v0_ori, rssi_v0);
#else
        printf("Get RSSI V0 = %d, RSSI V0-2 = %d\n", rssi_v0_ori, rssi_v0);
#endif

    
        //Step5. set Imod=100mA 
        Imod = (100 * 1000 / 2); /* convert mA to 2uA unit */
        ret = rtk_ldd_tx_mod_set(Imod);
        if(ret)
        {
            printf("Set TX Mod = %d Fail!!! (%d)\n", Imod, ret);
        }
        
        if ((w68_data&0x40)!=0)
        {
            rtk_ldd_tx_mod_get(&Imod);        
            printf("Set TX Mod = %d \n", Imod);
        }    
        
        osal_time_mdelay(10); 

        for (i=0;i<(loopcnt+offset);i++)
        {        
            memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
            ldd_cfg.state = 0xF2;
            ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
            if(ret)
            {
                printf("rtk_ldd_rssiVoltage_get fail!!! (%d)\n", ret);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, w59_data);
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, w5a_data);                
                free(p_rx_arr);                 
                return -1;
            }
            p_rx_arr[i].rssi_i = ldd_cfg.rssi_voltage;            
            p_rx_arr[i].rssi_v = ldd_cfg.rssi_voltage2;
            p_rx_arr[i].rssi_code = ldd_cfg.rssi_code;            
            p_rx_arr[i].gnd_code = ldd_cfg.gnd_code;
            p_rx_arr[i].vdd_code = ldd_cfg.vdd_code;  
            p_rx_arr[i].half_vdd_code = ldd_cfg.half_vdd_code;            
            p_rx_arr[i].rssi_code2 = ldd_cfg.driver_version;               
            //if ((w68_data&0x40)!=0)
            //{
                printf("%8d (0x%x) %8d (0x%x) %8d (0x%x) %8d (0x%x) %8d (0x%x) %8d (0x%x) %8d (0x%x)\n", 
                ldd_cfg.rssi_code, ldd_cfg.rssi_code, ldd_cfg.driver_version, ldd_cfg.driver_version,
                ldd_cfg.half_vdd_code, ldd_cfg.half_vdd_code, ldd_cfg.vdd_code, ldd_cfg.vdd_code, 
                ldd_cfg.gnd_code, ldd_cfg.gnd_code, ldd_cfg.rssi_voltage, ldd_cfg.rssi_voltage, 
                ldd_cfg.rssi_voltage2, ldd_cfg.rssi_voltage2); 
            //}        
        }

        _europa_rxbubble_sort(p_rx_arr, loopcnt+offset);

        rssi_v0 = 0;
        rssi_code1 = 0;    
        rssi_gnd = 0;
        rssi_vdd = 0;
        rssi_half1 = 0;
        
        sum = 0;
        sum_rssi = 0;
        sum_gnd = 0;
        sum_vdd = 0;
        sum_half = 0;
        sum_rssi2 = 0;
        
//printf("Parameter list:\nrssi_code gnd_code vdd_code half_vdd_code rssi_voltage\n");
        for (i=5;i<loopcnt+5;i++)
        {
            sum      += p_rx_arr[i].rssi_v;        
            sum_rssi += p_rx_arr[i].rssi_code;
            sum_gnd  += p_rx_arr[i].gnd_code;
            sum_vdd  += p_rx_arr[i].vdd_code;      
            sum_half += p_rx_arr[i].half_vdd_code;    
            sum_rssi2 += p_rx_arr[i].rssi_code2;            
        } 

        rssi_v0 = sum/loopcnt;
        rssi_code1 = sum_rssi/loopcnt;
        rssi_gnd = sum_gnd/loopcnt;
        rssi_vdd = sum_vdd/loopcnt;
        rssi_half1 = sum_half/loopcnt;   
        rssi_code21 = sum_rssi2/loopcnt;        
#ifdef EUROPA_DEBUG_RXPOWER    
        printf("Parameter list:\nrssi_voltage rssi_code gnd_code vdd_code half_vdd_code diff \n");
        for (i=0;i<5;i++)
        {
            diff = abs((int32)p_rx_arr[i].rssi_v-(int32)rssi_v0);
            printf("[%d]: %d 0x%x 0x%x 0x%x 0x%x diff = %d\n", i, p_rx_arr[i].rssi_v, p_rx_arr[i].rssi_code, p_rx_arr[i].gnd_code, p_rx_arr[i].vdd_code, p_rx_arr[i].half_vdd_code, diff);        
            if (diff > (rssi_v0*2/10))
            {
                printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = %d\n", i, p_rx_arr[i].rssi_v, rssi_v0);
            }            
        }
        
        for (i=0;i<5;i++)
        {
            diff = abs((int32)p_rx_arr[i+loopcnt+5].rssi_v-(int32)rssi_v0);
            printf("[%d]: %d 0x%x 0x%x 0x%x 0x%x diff = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, p_rx_arr[i+loopcnt+5].rssi_code, p_rx_arr[i+loopcnt+5].gnd_code, p_rx_arr[i+loopcnt+5].vdd_code, p_rx_arr[i+loopcnt+5].half_vdd_code, diff);
            if (diff > (rssi_v0*2/10))
            {
                printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, rssi_v0);
            }
        }
#endif        

        //printf("Get RSSI Code1 = %d Half VDD Code1 = 0x%x\n", rssi_code1, rssi_half1);
    
        //Step7. calculate k = -1* (Crssi1 - Crssi0)/ (Chalf_vdd1 - Chalf_vdd0), save k   
        //The value of k can be minus or plus, and  is floating point, so multiple 1000. When used in RX power, divide 1000
        temp1 = -1*((double)rssi_code1 - (double)rssi_code0);
        //printf("temp1 = %f \n", temp1);    
        temp2 = (double)rssi_half1 - (double)rssi_half0;
        temp3 = temp1/temp2;
        rssi_k = (int)(temp3*1000);
        //printf("temp2 = %f temp3 = %f\n", temp2, temp3);
        _europa_cal_flash_data_set(RTL8290B_RX_K_ADDR, 4, rssi_k);  
    
        temp1 = -1*((double)rssi_code21 - (double)rssi_code20);
        temp2 = (double)rssi_half1 - (double)rssi_half0;
        temp3 = temp1/temp2;
        rssi_k2 = (int)(temp3*1000);        
        addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_RSSI_K2_OFFSET_ADDR;         
        _europa_cal_flash_data_set(addr, 4, rssi_k2);         
    
        //Step8. restore loop mode, TEMP_INT ((A4/89=0xFE)
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x7C, w80_data);    
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 89, 0xFE);
        ret = rtk_ldd_tx_mod_set(ori_Imod);
        if(ret)
        {
            printf("Set TX Mod = %d Fail!!! (%d)\n", ori_Imod, ret);
        }
        
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, w59_data);
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, w5a_data);        

        free(p_rx_arr);    
            
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 70, w70_data);
        
        printf("Get RSSI K (x1000) = %d, RSSI K2 (x1000) = %d \n", rssi_k, rssi_k2);            
    }
    //else
    //{
    //   printf("RX Power Mode  = %d, error!!! \n", rx_power_mode);   
    //}
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_rssiv0_get Duration = %f ms\n", Duration);
#endif       
    return 0;     
}

static int _europa_cli_flashBlock_get(FILE *fp, int start_addr, int length)
{
    int output,i;
    int ret=0;
                    
    /* set the file pointer to start address */
    ret=fseek(fp, start_addr, SEEK_SET); 
    if (ret!=0)
    {
        return ret;
    }
    for(i=0;i<length;i++)
    {
        output = fgetc(fp);
        if (output == EOF)
        {
            break;    
        }
        else
        {
            if(i%16 == 0)
            {
                printf("0x%04x ", start_addr +i);
            }

            printf("0x%02x ", output);
            if(i%8 == 7)
                printf("   ");
            if(i%16 == 15)
                printf("\n");                    
        }
    }

    fclose(fp);
    printf("\n");    

    return 0; 
}



static int europa_cli_flash_a0_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];
  
    printf("europa DDMI A0:\n");  

    if(argc >=2)
    {

        if(!strcmp(argv[1], "europa"))
        {
    
            fp = fopen(EUROPA_FILE_LOCATION,"rb");  
            
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", EUROPA_FILE_LOCATION); 
                return -1;           
            }
            printf("%s DDMI A0 flash data: \n", EUROPA_FILE_LOCATION);   
            start_addr = 0;        
            
            printf("Start Address: 0x%x, Size = 0x%x  \n", start_addr, EUROPA_BLOCK_SIZE); 
                 
            _europa_cli_flashBlock_get(fp, start_addr, EUROPA_BLOCK_SIZE);     
        }
        else if(!strcmp(argv[1], "8290b"))
        {
            fp = fopen(RTL8290B_FILE_LOCATION,"rb");  
            
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", RTL8290B_FILE_LOCATION); 
                return -1;           
            }  
            printf("%s DDMI A0 flash data: \n", RTL8290B_FILE_LOCATION);  
            start_addr = 0;        
    
            printf("Start Address: 0x%x, Size = 0x%x  \n", start_addr, EUROPA_BLOCK_SIZE); 
                 
            _europa_cli_flashBlock_get(fp, start_addr, EUROPA_BLOCK_SIZE);               
        }
        else if(!strcmp(argv[1], "8290c"))
        {
            uint32 devId = 5;        
            printf("RTL8290C DDMI A0 flash data: \n");  
            start_addr = 0;        
            printf("Start Address: 0x%x, Size = 0x%x  \n", start_addr, EUROPA_BLOCK_SIZE); 
            _8290c_cli_flashBlock_get(devId, start_addr, EUROPA_BLOCK_SIZE); 
        }        
        else
        {
            printf("Chip %s error! Chip should be europa or 8290b. \n", argv[1]);
            return -1;            
        }
    }    
    else 
    {
        printf("%s <chip:europa or 8290b> \n", argv[0]);
        return -1;
    } 
    
    return 0;    
}

static int europa_cli_flash_a2_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    printf("europa DDMI A2:\n");  

    if(argc >=2)
    {

        if(!strcmp(argv[1], "europa"))
        {
    
            fp = fopen(EUROPA_FILE_LOCATION,"rb");  
            
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", EUROPA_FILE_LOCATION); 
                return -1;           
            }
            printf("%s DDMI A2 flash data: \n", EUROPA_FILE_LOCATION);        
        }
        else if(!strcmp(argv[1], "8290b"))
        {
            fp = fopen(RTL8290B_FILE_LOCATION,"rb");  
            
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", RTL8290B_FILE_LOCATION); 
                return -1;           
            }  
            printf("%s DDMI A2 flash data: \n", RTL8290B_FILE_LOCATION);                
        }
        else
        {
            printf("Chip %s error! Chip should be europa or 8290b. \n", argv[1]);
            return -1;            
        }

        start_addr = 0x100;     
        
        printf("Start Address: 0x%x, Size = 0x%x  \n", start_addr, EUROPA_BLOCK_SIZE); 
              
        _europa_cli_flashBlock_get(fp, start_addr, EUROPA_BLOCK_SIZE);        
    }    
    else 
    {
        printf("%s <chip:europa or 8290b> \n", argv[0]);
        return -1;
    } 
    
    return 0;    

}

static int europa_cli_flash_a4_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }
  
    printf("europa A4 flash:\n");  

    if(argc >=2)
    {

        if(!strcmp(argv[1], "europa"))
        {
    
            fp = fopen(EUROPA_FILE_LOCATION,"rb");  
            
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", EUROPA_FILE_LOCATION); 
                return -1;           
            }
            printf("%s A4 flash data: \n", EUROPA_FILE_LOCATION);        
        }
        else if(!strcmp(argv[1], "8290b"))
        {
            fp = fopen(RTL8290B_FILE_LOCATION,"rb");  
            
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", RTL8290B_FILE_LOCATION); 
                return -1;           
            }  
            printf("%s DDMI A4 flash data: \n", RTL8290B_FILE_LOCATION);                
        }
        else
        {
            printf("Chip %s error! Chip should be europa or 8290b. \n", argv[1]);
            return -1;            
        }

        start_addr = 0x200;        

        printf("Start Address: 0x%x, Size = 0x%x  \n", start_addr, EUROPA_BLOCK_SIZE); 
        
        _europa_cli_flashBlock_get(fp, start_addr, EUROPA_BLOCK_SIZE);        
    }    
    else 
    {
        printf("%s <chip:europa or 8290b> \n", argv[0]);
        return -1;
    } 
    
    return 0;    

}

static int europa_cli_flash_apd_lut_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];

    printf("europa APD LUT:\n");  
    
    if(argc >=2)
    {
    
        if(!strcmp(argv[1], "europa"))
        {
    
            fp = fopen(EUROPA_FILE_LOCATION,"rb");    
            
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", EUROPA_FILE_LOCATION); 
                return -1;             
            }
            printf("%s APD LUT flash data: \n", EUROPA_FILE_LOCATION);  
            start_addr = 0x300;   
            printf("Start Address: 0x%x, Size = 0x%x  \n", start_addr, EUROPA_BLOCK_SIZE); 
            _europa_cli_flashBlock_get(fp, start_addr, EUROPA_BLOCK_SIZE);              
        }
        else if(!strcmp(argv[1], "8290b"))
        {
            fp = fopen(RTL8290B_FILE_LOCATION,"rb");  
            
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", RTL8290B_FILE_LOCATION); 
                return -1;             
            }  
            printf("%s APD LUT flash data: \n", RTL8290B_FILE_LOCATION);
            start_addr = 0x300;   
            printf("Start Address: 0x%x, Size = 0x%x  \n", start_addr, EUROPA_BLOCK_SIZE); 
            _europa_cli_flashBlock_get(fp, start_addr, EUROPA_BLOCK_SIZE); 
        }
        else if(!strcmp(argv[1], "8290c"))
        {
            uint32 devId = 5; 
            printf("RTL8290C APD LUT flash data: \n");            
            start_addr = RTL8290C_APDLUT_ADDR_BASE;          
            printf("Start Address: 0x%x, Size = 0x%x  \n", start_addr, RTL8290C_FLASH_PAGE_SIZE); 
            _8290c_cli_flashBlock_get(devId, start_addr, RTL8290C_FLASH_PAGE_SIZE);     
        }        
        else
        {
            printf("Chip %s error! Chip should be europa/8290b/8290c. \n", argv[1]);
            return -1;            
        }
          
    }     
    else 
    {
        printf("%s <chip:europa/8290b/8290c> \n", argv[0]);
        return -1;
    } 
    
    return 0;     
 
}

static int europa_cli_flash_laser_lut_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];

    printf("europa Laser LUT:\n");  
    
    if(argc >=2)
    {
    
        if(!strcmp(argv[1], "europa"))
        {
    
            fp = fopen(EUROPA_FILE_LOCATION,"rb");    
            
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", EUROPA_FILE_LOCATION); 
                return -1;             
            }
            printf("%s Laser LUT flash data: \n", EUROPA_FILE_LOCATION); 
            start_addr = 0x400; 
            printf("Start Address: 0x%x, Size = 0x%x  \n", start_addr, 304);   
            _europa_cli_flashBlock_get(fp, start_addr, 304);              
        }
        else if(!strcmp(argv[1], "8290b"))
        {
            fp = fopen(RTL8290B_FILE_LOCATION,"rb");  
            
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", RTL8290B_FILE_LOCATION); 
                return -1;             
            }  
            printf("%s Laser LUT flash data: \n", RTL8290B_FILE_LOCATION);
            start_addr = 0x400; 
            printf("Start Address: 0x%x, Size = 0x%x  \n", start_addr, 304);   
            _europa_cli_flashBlock_get(fp, start_addr, 304); 
        }
        else if(!strcmp(argv[1], "8290c"))
        {
            uint32 devId = 5; 
            printf("RTL8290C APD LUT flash data: \n");            
            start_addr = RTL8290C_TXLUT_ADDR_BASE;          
            printf("Start Address: 0x%x, Size = 0x%x  \n", start_addr, RTL8290C_FLASH_PAGE_SIZE); 
            _8290c_cli_flashBlock_get(devId, start_addr, RTL8290C_FLASH_PAGE_SIZE);     
        }
        else
        {
            printf("Chip %s error! Chip should be europa/8290b/8290c.. \n", argv[1]);
            return -1;            
        }      
    }     
    else 
    {
        printf("%s <chip:europa/8290b/8290c.> \n", argv[0]);
        return -1;
    } 
    
    return 0;    
}


static int europa_cli_flash_param_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }
  
    if(argc >=2)
    {
    
        if(!strcmp(argv[1], "europa"))
        {
    
            fp = fopen(EUROPA_FILE_LOCATION,"rb");    
            
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", EUROPA_FILE_LOCATION); 
                return -1;             
            }
            printf("%s parameter data: \n", EUROPA_FILE_LOCATION);        
        }
        else if(!strcmp(argv[1], "8290b"))
        {
            fp = fopen(RTL8290B_FILE_LOCATION,"rb");  
            
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", RTL8290B_FILE_LOCATION); 
                return -1;             
            }  
            printf("%s parameter data: \n", RTL8290B_FILE_LOCATION);                
        }
        else
        {
            printf("Chip %s error! Chip should be europa or 8290b. \n", argv[1]);
            return -1;            
        }        
        start_addr = RTL8290B_DCL_P0_ADDR;   

        printf("Start Address: 0x%x, Size = 0x%x  \n", start_addr, EUROPA_BLOCK_SIZE);         
              
        _europa_cli_flashBlock_get(fp, start_addr, EUROPA_BLOCK_SIZE);        
    }     
    else 
    {
        printf("%s <chip:europa or 8290b> \n", argv[0]);
        return -1;
    } 
    
    return 0; 

}

static int europa_cli_flash_getAll(
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[EUROPA_PARAMETER_SIZE];

    printf("Europa All Flash data:\n");  
    
    if(argc >=2)
    {
    
        if(!strcmp(argv[1], "europa"))
        {
    
            fp = fopen(EUROPA_FILE_LOCATION,"rb");    
            
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", EUROPA_FILE_LOCATION); 
                return -1;             
            }
            printf("%s Flash data: \n", EUROPA_FILE_LOCATION);        
        }
        else if(!strcmp(argv[1], "8290b"))
        {
            fp = fopen(RTL8290B_FILE_LOCATION,"rb");  
            
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", RTL8290B_FILE_LOCATION); 
                return -1;             
            }  
            printf("%s Flash data: \n", RTL8290B_FILE_LOCATION);                
        }
        else if(!strcmp(argv[1], "8290c"))
        {
            fp = fopen(RTL8290C_DEVICE_0_FILE,"rb");  
            
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", RTL8290C_DEVICE_0_FILE); 
                return -1;             
            }  
            printf("%s Flash data: \n", RTL8290C_DEVICE_0_FILE);                
        }        
        else if(!strcmp(argv[1], "8291"))
        {
            fp = fopen(RTL8291_DEVICE_0_FILE,"rb");  
            
            if (NULL ==fp)
            {
                printf("Open file %s error!!!!!!!\n", RTL8291_DEVICE_0_FILE); 
                return -1;             
            }  
            printf("%s Flash data: \n", RTL8291_DEVICE_0_FILE);                
        } 
        else
        {
            printf("Chip %s error! Chip should be europa/8290b/8290c. \n", argv[1]);
            return -1;            
        }        
        start_addr = 0;   

        printf("Start Address: 0x%x, Size = 0x%x  \n", start_addr, EUROPA_BLOCK_SIZE*10);         
              
        _europa_cli_flashBlock_get(fp, start_addr, EUROPA_BLOCK_SIZE*10);        
    }     
    else 
    {
        printf("%s <chip: europa/8290b/8290c/8291> \n", argv[0]);
        return -1;
    } 
    
    return 0; 

}

static int europa_cli_ddmi_a0_getAll(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint8 *init_data, *ptr_data;
    uint32 i, length, offset;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    //rtk_ldd_i2c_init(I2C_PORT_0);

    /*Dump DDMI A0*/
    //printf("Dump DDMI A0!!!!!!!\n", ret);
    length = 0xFF;
    offset = 0;

    init_data = (uint8 *)malloc(length*sizeof(uint8));
    if (NULL==init_data)
    {
        printf("Memory Allocation Fail!!!!!!!\n");
        return -1;
    }
    ptr_data = init_data;
    ret = rtk_ldd_parameter_get(length, offset, ptr_data);
    printf("\n");
    free(init_data);
    if(ret)
    {
        printf(" Dump registers failed (%d)\n", ret);
        return -1;
    }  

    return 0;    
}

static int europa_cli_ddmi_a2_getAll(
    int argc,
    char *argv[],
    FILE *fp)
{

    int ret;
    uint8 *init_data, *ptr_data;
    uint32 i, length, offset;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    //rtk_ldd_i2c_init(I2C_PORT_0);

    /*Dump DDMI A2*/
    //printf("Dump DDMI A2!!!!!!!\n", ret);
    length = 0xFF;
    offset = 0x100;

    init_data = (uint8 *)malloc(length*sizeof(uint8));
    if (NULL==init_data)
    {
        printf("Memory Allocation Fail!!!!!!!\n");
        return -1;
    }
    ptr_data = init_data;
    ret = rtk_ldd_parameter_get(length, offset, ptr_data);
    printf("\n");
    free(init_data);
    if(ret)
    {
        printf(" Dump registers failed (%d)\n", ret);
        return -1;
    }  

    return 0;     
}

static int europa_cli_a4_getAll(
    int argc,
    char *argv[],
    FILE *fp)
{

    int ret;
    uint8 *init_data, *ptr_data;
    uint32 i, length, offset;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    //rtk_ldd_i2c_init(I2C_PORT_0);

    /*Dump A4*/
    //printf("Dump A4!!!!!!!\n", ret);
    length = 0xFF;
    offset = 0x200;

    init_data = (uint8 *)malloc(length*sizeof(uint8));
    if (!init_data)
    {
        printf("Memory Allocation Fail!!!!!!!\n");
        return -1;
    }
    ptr_data = init_data;
    ret = rtk_ldd_parameter_get(length, offset, ptr_data);
    printf("\n");
    free(init_data);
    if(ret)
    {
        printf(" Dump registers failed (%d)\n", ret);
        return -1;
    }  

    return 0;    
}


static int europa_cli_a5_getAll(
    int argc,
    char *argv[],
    FILE *fp)
{

    int ret;
    uint8 *init_data, *ptr_data;
    uint32 i, length, offset;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    //rtk_ldd_i2c_init(I2C_PORT_0);

    /*Dump A5*/
    //printf("Dump A5!!!!!!!\n", ret);
    length = 0xFF;
    offset = 0x300;

    init_data = (uint8 *)malloc(length*sizeof(uint8));
    if (!init_data)
    {
        printf("Memory Allocation Fail!!!!!!!\n");
        return -1;
    }
    ptr_data = init_data;
    ret = rtk_ldd_parameter_get(length, offset, ptr_data);
    printf("\n");
    free(init_data);
    if(ret)
    {
        printf(" Dump registers failed (%d)\n", ret);
        return -1;
    }  

    return 0;    
}

static int europa_cli_register_getAll(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint8 *init_data, *ptr_data;
    uint32 i, length, offset;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    //rtk_ldd_i2c_init(I2C_PORT_0);

    /*Dump All*/
    printf("Dump All!!!!!!!\n");
    length = 0x3FF;
    offset = 0;


    init_data = (uint8 *)malloc(length*sizeof(uint8));
    if (!init_data)
    {
        printf("Memory Allocation Fail!!!!!!!\n");
        return -1;
    }
    ptr_data = init_data;
    ret = rtk_ldd_parameter_get(length, offset, ptr_data);
    printf("\n");
    free(init_data);
    if(ret)
    {
        printf(" Dump registers failed (%d)\n", ret);
        return -1;
    }  

    return 0;   
}

static int europa_cli_register_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    int32 addr;
    uint8 value;
    uint32 devId = 5, value2;
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
        if(argc >= 2)
        { 
            addr = _vlaue_translate(argv[1]);          
            ret = rtk_ldd_parameter_get(1, addr, &value);
            printf("\n");
            if(ret)
            {
                printf("get register failed (%d)\n", ret);
                return -1;
            } 
            printf("addr = 0x%x, data = 0x%x\n", addr, value);   
        }
        else
        {
            printf("%s <address> \n", argv[0]);
            printf("address is from 0~0x3FF\n");
            return -1;
        }
    }
    else if (EUROPA_LDD_8290C_MODE == chip_mode)
    {
        ret = rtl8290c_cli_register_get(devId, argc, argv, fp);
        if(ret)
        {
            printf("rtl8290c_cli_register_get failed (%d)\n", ret);
            return -1;
        }
    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        if(0 == strcmp(argv[0], "reg"))
        {
            ret = rtl8291_cli_register_get(devId, argc, argv, fp);
            if(ret)
            {
                printf("rtl8291_cli_register_get failed (%d)\n", ret);
                return -1;
            }
        }
        else if (0 == strcmp(argv[0], "regbit"))
        {
            ret = rtl8291_cli_regBit_get(devId, argc, argv, fp);
            if(ret)
            {
                printf("rtl8291_cli_regBit_get failed (%d)\n", ret);
                return -1;
            }
        }
        else if (0 == strcmp(argv[0], "regbits"))
        {
            ret = rtl8291_cli_regBits_get(devId, argc, argv, fp);
            if(ret)
            {
                printf("rtl8291_cli_regBits_get failed (%d)\n", ret);
                return -1;
            }
        }
        else
        {
            printf("Command Error!!!\n");
    
        }
    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        if(0 == strcmp(argv[0], "reg"))
        {
            ret = rtl8291_cli_register_get(devId, argc, argv, fp);
            if(ret)
            {
                printf("rtl8291_cli_register_get failed (%d)\n", ret);
                return -1;
            }
        }
        else if (0 == strcmp(argv[0], "regbit"))
        {
            ret = rtl8291_cli_regBit_get(devId, argc, argv, fp);
            if(ret)
            {
                printf("rtl8291_cli_regBit_get failed (%d)\n", ret);
                return -1;
            }
        }
        else if (0 == strcmp(argv[0], "regbits"))
        {
            ret = rtl8291_cli_regBits_get(devId, argc, argv, fp);
            if(ret)
            {
                printf("rtl8291_cli_regBits_get failed (%d)\n", ret);
                return -1;
            }
        }
        else
        {
            printf("Command Error!!!\n");
    
        }
    }    
    else
    {
        printf("LDD Chip Not Supported!!!\n");
    }

    return 0;    
}

static int europa_cli_register_set(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    int32 addr, data;
    uint8 value;
    uint32 devId = 5;
    int chip_mode;
    uint32 regData;

    chip_mode = _europa_ldd_parser();

    if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
        if(argc >= 3)
        { 
            addr = _vlaue_translate(argv[1]); 
            data = _vlaue_translate(argv[2]); 
            if (data >0xFF)
            {
                printf("dsta should be 0~0xFF\n");
                
            }
            value = (uint8)(data&0xFF);            
            ret = rtk_ldd_parameter_set(1, addr, &value);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_parameter_set failed (%d)\n", ret);
                return -1;
            }          
        }
        else
        {
            printf("%s <address> <data> \n", argv[0]);
            printf("address is from 0~0x3FF\n");
            return -1;
        }
    }    
    else if (EUROPA_LDD_8290C_MODE == chip_mode)
    {
        ret = rtl8290c_cli_register_set(devId, argc, argv, fp);
        if(ret)
        {
            printf("rtl8290c_cli_register_set failed (%d)\n", ret);
            return -1;
        }
    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        if(0 == strcmp(argv[0], "reg"))
        {
            ret = rtl8291_cli_register_set(devId, argc, argv, fp);
            if(ret)
            {
                printf("rtl8291_cli_register_set failed (%d)\n", ret);
                return -1;
            }
        }
        else if (0 == strcmp(argv[0], "regbit"))
        {
            ret = rtl8291_cli_regBit_set(devId, argc, argv, fp);
            if(ret)
            {
                printf("rtl8291_cli_regBit_set failed (%d)\n", ret);
                return -1;
            }
        }
        else if (0 == strcmp(argv[0], "regbits"))
        {
            ret = rtl8291_cli_regBits_set(devId, argc, argv, fp);
            if(ret)
            {
                printf("rtl8291_cli_regBits_set failed (%d)\n", ret);
                return -1;
            }
        }
        else
        {
            printf("Command Error!!!\n");
    
        }
    }    
    else
    {
        printf("LDD Chip Not Supported!!!\n");
    }
    
    return 0;    
}

static int europa_cli_register_page_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 regData;
    uint32 i, length, offset;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
        printf("LDD Chip Not Supported!!!\n");

    }
    else if (EUROPA_LDD_8290C_MODE == chip_mode)
    {
        offset = 0;
        length = 0;
        if(argc < 2)
        {
            printf("%s a0/a2l/a2p0/a2p1/wb/rb/dig/txlut/apdlut/rsvlut1/rsvlut2/digext/ind/info \n", argv[0]);
            printf("EX: %s info\n", argv[0]);
            return -1;
        }
        else
        {
            if(0 == strcmp(argv[1], "a0"))
            {
                offset = 0;
                length = 0x100;
            }
            else if(0 == strcmp(argv[1], "a2l"))
            {
                offset = 0x100;
                length = 0x80;
            }
            else if(0 == strcmp(argv[1], "a2p0"))
            {
                offset = 0x180;
                length = 0x80;
            }
            else if(0 == strcmp(argv[1], "a2p1"))
            {
                offset = 0x200;
                length = 0x80;
            }
            else if(0 == strcmp(argv[1], "wb"))
            {
                offset = 0x280;
                length = 0x80;
            }
            else if(0 == strcmp(argv[1], "rb"))
            {
                offset = 0x300;
                length = 0x80;
            }
            else if(0 == strcmp(argv[1], "dig"))
            {
                offset = 0x380;
                length = 0x80;
            }
            else if(0 == strcmp(argv[1], "txlut"))
            {
                offset = 0x400;
                length = 0x80;
            }
            else if(0 == strcmp(argv[1], "apdlut"))
            {
                offset = 0x480;
                length = 0x80;
            }
            else if(0 == strcmp(argv[1], "rsvlut1"))
            {
                offset = 0x500;
                length = 0x80;
            }
            else if(0 == strcmp(argv[1], "rsvlut2"))
            {
                offset = 0x580;
                length = 0x80;
            }
            else if(0 == strcmp(argv[1], "digext"))
            {
                offset = 0x700;
                length = 0x80;
            }
            else if(0 == strcmp(argv[1], "ind"))
            {
                offset = 0x600;
                length = 0x80;
            }        
            else if(0 == strcmp(argv[1], "info"))
            {
                offset = 0x680;
                length = 0x80;
            }
            else
            {
                printf("%s a0/a2l/a2p0/a2p1/wb/rb/dig/txlut/apdlut/rsvlut1/rsvlut2/digext/ind/info \n", argv[0]);
                printf("EX: %s info\n", argv[0]);
                return -1;
            }
        }

        for(i=0;i<length;i++)
        {
            ret = rtk_ldd_parameter2_get(devId, offset+i, &regData);
            if(ret)
            {
                printf(" Dump registers failed (%d)\n", ret);
                return -1;
            } 
            
            if(i%16 == 0)
            {
                printf("0x%04x ", offset+i);
            }
                
            printf("0x%02x ", regData);
            if(i%8 == 7)
                printf("   ");
            if(i%16 == 15)
                printf("\n");                 
        }
        
        printf("\n");
    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {  
        ret = rtl8291_cli_register_page_get(devId, argc, argv, fp);
        return ret;    
    }    
    else
    {
        printf("LDD Chip Not Supported!!!\n");
    }

    return 0;   
}



/*
 * debug europa get temperature
 */
 static int europa_cli_temperature_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint16 tempK;
    uint32 parm;
    int32 tempC;
    int8 offset;

    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {
        tempK = 0;
    
        ret = rtl8290c_cli_temperature_get(devId);
        if(ret)
        {
            printf("rtl8290c_cli_temperature_get Fail!!! (%d)\n", ret);
            return -1;
        }  
    }
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {
        ret = rtl8291_cli_temperature_get(devId, argc, argv);
        return ret;
    }
    else if(EUROPA_LDD_8290B_MODE == chip_mode)
    {
        tempK = 0;
    
        ret = rtk_ldd_temperature_get(&tempK);
        if(ret)
        {
            printf("Get Temperature Fail!!! (%d)\n", ret);
            return -1;
        }
    
        //diag_util_printf("Temperature = %dK\n", temp);
        //tempC = (tempK*100-27315)/100; /* degK to degC */
        tempC = tempK-273; /* degK to degC */
        printf("Temperature=%dK (%dC)\n", tempK, tempC);
    
        //rtk_ldd_flash_data_get(EUROPA_TEMP_OFFSET_ADDR, 1, &ldd_param.temp_offset);
        _europa_cal_flash_data_get(RTL8290B_TEMP_OFFSET_ADDR, 1, &parm);       
        offset = (int8)(parm);
    
        tempC = tempC - offset;
        printf("BOSA Temperature = %dC (offset=%d)\n", tempC, offset);
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);       
    }
    
    return 0;
} 

/*
 * debug europa get vdd
 */
static int europa_cli_vdd_get(
    int argc,
    char *argv[],
    FILE *fp)

{
    int ret;
    uint32 vdd;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {
        ret = rtl8290c_cli_vdd_get(devId);
        if(ret)
        {
            printf("rtl8290c_cli_vdd_get Fail!!! (%d)\n", ret);
            return -1;
        }    
    }
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {
        ret = rtl8291_cli_vdd_get(devId);
        return ret;    
    }
    else if(EUROPA_LDD_8290B_MODE == chip_mode)
    {    
        vdd = 0;
    
        ret = rtk_ldd_vdd_get(&vdd);
        if(ret)
        {
            printf("Get VDD Voltage fail!!! (%d)\n", ret);
            return -1;
        }
    
        printf("VDD Voltage = %d uV\n", vdd);
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);       
    }
    
    return 0;
}   
/*
 * debug europa set rx-param ( a | b | c ) <UINT:value>
 */
static int europa_cli_rxparam_set(
        int argc,
        char *argv[],
        FILE *fp)

{
    uint32 addr,length;
    int32 para;
    float  r_off;
    uint32 devId = 5;
    int chip_mode;
    int ret;
    
    chip_mode = _europa_ldd_parser();
    
    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {
        ret = rtl8290c_cli_rxparam_set(devId, argc, argv, fp);
        return ret;
    }
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {
        ret = rtl8291_cli_rxparam_set(devId, argc, argv, fp);
        return ret;
    }
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {
        ret = rtl8291_cli_rxparam_set(devId, argc, argv, fp);
        return ret;
    }
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {    
        if(argc < 3)
        { 
            printf("%s <a/b/c/offset> <value>\n", argv[0]);
            return -1;    
        }
    
        if(0 == strcmp(argv[1], "a")) 
        {
            addr = RTL8290B_RX_A_ADDR;
            para = _vlaue_translate(argv[2]);
            length = 4;
            printf("RX parameter a = %d\n",  para);        
        }
        else if(0 == strcmp(argv[1], "b")) 
        {
            addr = RTL8290B_RX_B_ADDR;
            para = _vlaue_translate(argv[2]);
            length = 4;
            printf("RX parameter b = %d\n",  para);                
        }
        else if(0 == strcmp(argv[1], "c")) 
        {
            addr = RTL8290B_RX_C_ADDR;
            para = _vlaue_translate(argv[2]);
            length = 4;
            printf("RX parameter c = %d\n",  para);        
        }
        else if(0 == strcmp(argv[1], "offset")) 
        {
            addr = RTL8290B_RXDDMI_OFFSET_OFFSET_ADDR+RTL8290B_PARAM_ADDR_BASE;
            r_off = atof(argv[2]);
            para = (int)(r_off*100);
            length = 2;    
            printf("RX Offset = %fdBm (%d)\n",  r_off, para);        
        }    
        else
        {
            printf("no such mode %s\n", argv[1]);
            return -1;
        }
    
        _europa_cal_flash_data_set(addr, length, para);
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);       
    }

    return 0;
}  

/*
 * debug europa get rx-param
 */
static int europa_cli_rxparam_get(
        int argc,
        char *argv[],
        FILE *fp)

{
    uint32 addr, para1;
    europa_rxparam_t ldd_rxparam;
    float r_off;
    int16 data;
    uint32 devId = 5;
    int chip_mode;
    int ret; 
    
    chip_mode = _europa_ldd_parser();
    
    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {
        ret = rtl8290c_cli_rxparam_get(devId, argc, argv, fp);
        return ret;
    }
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {
        ret = rtl8291_cli_rxparam_get(devId, argc, argv, fp);
        return ret;
    }    
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
        addr = RTL8290B_RX_A_ADDR;
        _europa_cal_flash_data_get(addr, 4, &para1);
        printf("RX parameter a: flash = %d\n",  para1);
        addr = RTL8290B_RX_B_ADDR;
        _europa_cal_flash_data_get(addr, 4, &para1);
        printf("RX parameter b: flash = %d\n", para1);
        addr = RTL8290B_RX_C_ADDR;
        _europa_cal_flash_data_get(addr, 4, &para1);
        printf("RX parameter c: flash = %d\n", para1);
    
        memset(&ldd_rxparam, 0, sizeof(europa_rxparam_t));
        _europa_rx_parameter_get(&ldd_rxparam);
    
        addr = RTL8290B_RXDDMI_OFFSET_OFFSET_ADDR+RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 2, &para1);
        data = (int16)para1;
        r_off = 0;
        if (para1!=0)
        {
            r_off = (float)(data)/100;
        }
        printf("RX Offset = %fdBm (%d)\n",  r_off, data);        
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);       
    }

    return 0;
}    /* end of cparser_cmd_debug_europa_get_rx_param */

/*
 * debug europa set tx-param ( a | b | c ) <INT:value>
 */
static int europa_cli_txparam_set(
        int argc,
        char *argv[],
        FILE *fp)

{
    uint32 addr;
    int32 para;
    uint32 devId = 5;
    int chip_mode;
    int ret;
    
    chip_mode = _europa_ldd_parser();
    
    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {
        ret = rtl8290c_cli_txparam_set(devId, argc, argv, fp);
        return ret;
    }
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {
        ret = rtl8291_cli_txparam_set(devId, argc, argv, fp);
        return ret;
    }    
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
        if(argc < 3)
        { 
            printf("%s <a/b/c> <value>\n", argv[0]);
            return -1;    
        }
    
        if(0 == strcmp(argv[1], "a")) 
        {
            addr = RTL8290B_TX_A_ADDR;
            para = _vlaue_translate(argv[2]);
            printf("TX parameter a = %d\n",  para);        
        }
        else if(0 == strcmp(argv[1], "b")) 
        {
            addr = RTL8290B_TX_B_ADDR;
            para = _vlaue_translate(argv[2]);
            printf("TX parameter b = %d\n",  para);                
        }
        else if(0 == strcmp(argv[1], "c")) 
        {
            addr = RTL8290B_TX_C_ADDR;
            para = _vlaue_translate(argv[2]);
            printf("TX parameter c = %d\n",  para);        
        }
        else
        {
            printf("no such mode %s\n", argv[1]);
            return -1;
        }
    
        _europa_cal_flash_data_set(addr, 4, para);
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);       
    }

    return 0;

}  

/*
 * debug europa get tx-param
 */
static int europa_cli_txparam_get(
        int argc,
        char *argv[],
        FILE *fp)

{
    uint32 addr, para1;
    uint32 devId = 5;
    int chip_mode;
    int ret;
    
    chip_mode = _europa_ldd_parser();
    
    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {
        ret = rtl8290c_cli_txparam_get(devId, argc, argv, fp);
        return ret;
    }
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {
        ret = rtl8291_cli_txparam_get(devId, argc, argv, fp);
        return ret;
    }    
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
        addr = RTL8290B_TX_A_ADDR;
        _europa_cal_flash_data_get(addr, 4, &para1);
        printf("TX parameter a: flash = %d\n",  para1);
        addr = RTL8290B_TX_B_ADDR;
        _europa_cal_flash_data_get(addr, 4, &para1);
        printf("TX parameter b: flash = %d\n", para1);
        addr = RTL8290B_TX_C_ADDR;
        _europa_cal_flash_data_get(addr, 4, &para1);
        printf("TX parameter c: flash = %d\n", para1);
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);       
    }

    return 0;
}    /* end of cparser_cmd_debug_europa_get_tx_param */

void _europa_cal_data_add(uint32 addr, uint32 data)
{
    FILE *fp;
    int wfd = -1, ret = 0;    
    uint8 temp8, i;
    uint8 *init_data, *ptr_data;
    uint32 temp32;

    init_data = (uint8 *)malloc(EUROPA_CAL_FILE_SIZE*sizeof(uint8));
    if (NULL == init_data)
    {
        printf("Memory Allocation Fail!!!!!!!\n");
        return;
    } 

    fp = fopen(EUROPA_CAL_FILE_LOCATION,"rb");
    if (NULL ==fp)
    {
        printf("Open %s error!!!!!!!\n", EUROPA_CAL_FILE_LOCATION);
        free(init_data); 
        return;
    }

    ret=fread(init_data, 1, EUROPA_CAL_FILE_SIZE, fp);
    //if (ret!=EUROPA_CAL_FILE_SIZE)
    //{
        //fclose(fp); 
        //free(init_data);
		//printf("fread ret = %d, EUROPA_CAL_FILE_SIZE = %d\n", ret, EUROPA_CAL_FILE_SIZE);
        //return;
    //}
    //printf("Test gLaserLutData !!!!!!!\n");
    ret = fseek(fp, EUROPA_CAL_LASERLUT_BASE_ADDR, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp); 
        free(init_data);
        printf("fseek 2 error!!!!!!!\n");
        return;
    }
    memset(gLaserLutData, 0x0, sizeof(gLaserLutData));
    ret=fread(gLaserLutData, 1, sizeof(gLaserLutData), fp);
    //if (ret>sizeof(gLaserLutData))
    //{
        //fclose(fp); 
        //free(init_data);
        //printf("fread ret = %d, SIZE = %d\n", ret, sizeof(gLaserLutData));
        //return;
    //}
    fclose(fp);
    //printf("Test gLaserLutData End!!!!!!!\n");

    ptr_data = init_data + addr;
    
    temp8 = 0;
    temp32 = data;
    for(i=0;i<4;i++)
    {
        *ptr_data = (uint8)(temp32>>(8*((4-1)-i)));
        //printf("%s %d: 0x%x!!!!!!!\n", __FUNCTION__, i, *ptr_data);
        ptr_data++;
    }

    fp = fopen(EUROPA_CAL_FILE_LOCATION, "wb");         
    if (NULL ==fp)
    {
        printf("Open %s error!!!!!!!\n", EUROPA_CAL_FILE_LOCATION);
        free(init_data);         
        return;             
    }
    ret=fwrite(init_data,sizeof(uint8), EUROPA_CAL_FILE_SIZE, fp);
    //if (ret!=EUROPA_CAL_FILE_SIZE)
    //{
    //    printf("fwrite ret = %d, size = %d\n", ret, EUROPA_CAL_FILE_SIZE);
    //} 
    
    fflush(fp); 
    wfd = fileno(fp);
    if(wfd != -1)
    {
        fsync(wfd);
    }
    
    fclose(fp);
    free(init_data);    
}

void _europa_cal_data_get(uint32 addr, uint32 *pdata)
{
    FILE *fp;
    uint8 i, tmp[4];
    uint32 temp32;
    int ret=0;

    fp = fopen(EUROPA_CAL_FILE_LOCATION,"rb");
    if (NULL ==fp)
    {
        printf("Open %s error!!!!!!!\n", EUROPA_CAL_FILE_LOCATION);
        return;
    }
 
    ret=fseek(fp, addr, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp);    
        printf("fseek error!!!!!!!\n");	
        return;
    }
    memset(tmp, 0x0, sizeof(uint8)*4);
    ret = fread(tmp, sizeof(uint8), 4, fp);
    //if (ret!=(sizeof(uint8)*4))
    //{
    //    return;
    //}    
    temp32 = 0;
    for(i=0;i<4;i++)
    {
        temp32 = temp32 | ((uint32)tmp[i]<<(8*((4-1)-i)));    
    }
    //printf("value = 0x%08X\n", temp32);    
    *pdata = temp32;    
    fclose(fp);
    
}

#if 0
void _europa_cal_data_save_to_flash(uint32 addr, uint32 data)
{
    FILE *fp;
    uint8 temp8, i;
    uint8 *init_data, *ptr_data;
    uint32 temp32;
    int file_len;

    init_data = (uint8 *)malloc(RTL8290B_PARAMETER_SIZE);
    _rtl8290b_load_data(init_data,fp,&file_len);  

    ptr_data = init_data + addr;
    
    temp8 = 0;
    temp32 = data;
    for(i=0;i<4;i++)
    {
        *ptr_data = (uint8)(temp32>>(8*((4-1)-i)));
        printf("%s %d: 0x%x!!!!!!!\n", __FUNCTION__, i, *ptr_data);
        ptr_data++;
    }

    fp = fopen(RTL8290B_FILE_LOCATION, "wb");         
    if (NULL ==fp)
    {
        printf("Open %s error!!!!!!!\n", RTL8290B_FILE_LOCATION);
        return;             
    }
    fseek(fp, 0, SEEK_SET); 
    fwrite(init_data,sizeof(char), file_len,fp);
    fclose(fp);
    free(init_data);    
}
#endif

static int europa_cli_efuse_dump(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret=0;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8291_MODE == chip_mode)
    {
        ret = rtl8291_cli_efuse_dump(devId, argc, argv, fp);
        if(ret)
        {
            printf("rtl8291_cli_efuse_dump Fail!!! (%d)\n", ret);
            return -1;
        }    
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);       
    }
    
    return 0;
}

static int europa_cli_sram_dump(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret=0;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8291_MODE == chip_mode)
    {
        ret = rtl8291_cli_sram_dump(devId, argc, argv, fp);
        if(ret)
        {
            printf("rtl8291_cli_sram_dump Fail!!! (%d)\n", ret);
            return -1;
        }    
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);       
    }
    
    return 0;
}

static int europa_cli_cal_tx_ddmi(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    int wfd = -1;
    int32 addr;
    int32 tx_a, tx_b;
    uint8 value, temp8, i;
    uint8 *init_data, *ptr_data;
    uint32 temp32;
    uint32 tx_pwr[2];
    uint32 tx_mpd[2]; 
    double    a,b;    
    uint32 tx_iavg;
    uint16 iavg_value;
#ifdef RTL8290B_CAL_TIME
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {     
        ret = rtl8290c_cli_cal_tx_ddmi(devId, argc, argv, fp);
        return ret;
    } 
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {     
        ret = rtl8291_cli_cal_tx_ddmi(devId, argc, argv, fp);
        return ret;
    }     
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {

    if(argc >= 2)
    { 
        if(0 == strcmp(argv[1], "0"))
        {
            printf("reset cal data.\n");
#if 0            
            fp = fopen(EUROPA_CAL_FILE_LOCATION, "wb");          
            if (NULL ==fp)
            {
                printf("Open %s error!!!!!!!\n", EUROPA_CAL_FILE_LOCATION);
                return -1;             
            }        
            init_data = (uint8 *)malloc(EUROPA_CAL_FILE_SIZE);
            memset(init_data, 0x0, sizeof(char)*EUROPA_CAL_FILE_SIZE);    
            fseek(fp, 0, SEEK_SET); 
            fwrite(init_data, sizeof(char), EUROPA_CAL_FILE_SIZE,fp);       
            fclose(fp);
            free(init_data);
#endif
            fp = fopen(EUROPA_CAL_FILE_LOCATION,"rb");
            if (NULL ==fp)
            {
                printf("%s does not exist. Create an empty file.\n", EUROPA_CAL_FILE_LOCATION);       
                fp = fopen(EUROPA_CAL_FILE_LOCATION,"wb");
                if (NULL ==fp)
                {
                    printf("Create %s error!!!!!!!\n", EUROPA_CAL_FILE_LOCATION); 
                    return -1;  
                }
                fclose(fp);
                return -1;
            }  
            
            init_data = (uint8 *)malloc(EUROPA_CAL_FILE_SIZE*sizeof(uint8));
            memset(init_data, 0x0, sizeof(char)*EUROPA_CAL_FILE_SIZE);    
            ret=fseek(fp, 0, SEEK_SET);
            if (ret!=0)
            {
                fclose(fp); 
                free(init_data);
                return ret;
            }
            ret=fread(init_data, sizeof(char), EUROPA_CAL_FILE_SIZE, fp);
            //if (ret!=0)
            //{
            //    fclose(fp); 
            //    free(init_data);
            //    return ret;
            //}
            fclose(fp);

            //Clear TX DDMI parameter
            memset(init_data, 0x0, sizeof(char)*EUROPA_CAL_RXPWR1_ADDR); 

            fp = fopen(EUROPA_CAL_FILE_LOCATION, "wb");          
            if (NULL ==fp)
            {
                printf("Open %s error!!!!!!!\n", EUROPA_CAL_FILE_LOCATION);
                free(init_data);            
                return -1;             
            }  
            ret=fseek(fp, 0, SEEK_SET);
            if (ret!=0)
            {
                fclose(fp); 
                free(init_data);
                return ret;
            }
            fwrite(init_data, sizeof(char), EUROPA_CAL_FILE_SIZE,fp);           
            free(init_data);
            
            fflush(fp); 
            wfd = fileno(fp);
            if(wfd != -1)
            {
                fsync(wfd);
            }            
                    
            fclose(fp);            
#if 0            
            //tx_pwr[0] = tx_pwr[1] = 0;
            //tx_mpd[0] = tx_mpd[1] = 0;
            setenv("TXPWR1","0",1);
            setenv("TXPWR2","0",1);
            setenv("TXMPD1","0",1);
            setenv("TXMPD2","0",1);    
#endif            
        }
        else if(0 == strcmp(argv[1], "calc"))
        {
            printf("calculate cal data.\n");
            _europa_cal_data_get(EUROPA_CAL_TXPWR1_ADDR, &tx_pwr[0]);
            _europa_cal_data_get(EUROPA_CAL_TXMPD1_ADDR, &tx_mpd[0]);            
            _europa_cal_data_get(EUROPA_CAL_TXPWR2_ADDR, &tx_pwr[1]);
            _europa_cal_data_get(EUROPA_CAL_TXMPD2_ADDR, &tx_mpd[1]);  
            _europa_cal_data_get(EUROPA_CAL_TX_IAVG_ADDR, &tx_iavg); 
      
            printf("Data1: Power=%6dnW Impd=%6dnA\n",
                tx_pwr[0], tx_mpd[0]);
            printf("Data2: Power=%6dnW Impd=%6dnA\n",
                tx_pwr[1], tx_mpd[1]); 
        
            printf("Iavg = 0x%x\n", tx_iavg);             
        
            printf("---- regression ...\n");
               _europa_cal_lin_regression(tx_mpd[0], tx_pwr[0], tx_mpd[1], tx_pwr[1],
                       &a, &b);
            printf("  a = %f\n", a);
            printf("  b = %f\n", b);
        
            printf("---- shifting...\n");
            a = a*(1<<TX_A_SHIFT);
            b = b*(1<<TX_B_SHIFT);
            printf("  a = %f\n", a);
            printf("  b = %f\n", b);
        
            printf("---- casting...\n");
            tx_a = (int32) a;
            tx_b = (int32) b;
            printf("  a = 0x%08x\n", tx_a);
            printf("  b = 0x%08x\n", tx_b);
        
            // save immediately
            //rtk_ldd_flash_data_set(EUROPA_TX_A_ADDR, 4, (uint32)ldd_param.tx_a);
            //rtk_ldd_flash_data_set(EUROPA_TX_B_ADDR, 4, (uint32)ldd_param.tx_b);
            //rtk_ldd_flash_data_set(EUROPA_TX_C_ADDR, 4, (uint32)ldd_param.tx_c);
            _europa_cal_flash_data_set(RTL8290B_TX_A_ADDR, 4, (uint32)tx_a);
            _europa_cal_flash_data_set(RTL8290B_TX_B_ADDR, 4, (uint32)tx_b);                        
            //20200113: For Short Burst
            _europa_cal_flash_data_set(RTL8290B_MPD1_ADDR, 4, tx_mpd[0]); 
            
            addr = RTL8290B_CAL_TX_IAVG_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
            _europa_cal_flash_data_set((uint32)addr, 1, (tx_iavg&0xFF));        
            
#if 0        
                        printf("TXPWR1=%sn \n",getenv("TXPWR1"));
                        printf("TXPWR2=%sn \n",getenv("TXPWR2"));
                        printf("TXMPD1=%sn \n",getenv("TXMPD1"));
                        printf("TXMPD2=%sn \n",getenv("TXMPD2"));
#endif
        }
        else if(0 == strcmp(argv[1], "1"))
        {      
            if (argc >= 3)
            {
                printf("Set TXPWR1 data.\n"); 
                tx_pwr[0] = _vlaue_translate(argv[2]);        
                _europa_cal_data_add(EUROPA_CAL_TXPWR1_ADDR, tx_pwr[0]);                       
                tx_mpd[0] = _europa_cal_get_mpd_current();
                _europa_cal_data_add(EUROPA_CAL_TXMPD1_ADDR, tx_mpd[0]);           
                printf("Data1: Power=%6dnW Impd=%6dnA\n",
                   tx_pwr[0], tx_mpd[0]);

                rtk_ldd_apcIavg_get(&iavg_value);
                tx_iavg = (uint32)iavg_value;
                _europa_cal_data_add(EUROPA_CAL_TX_IAVG_ADDR, tx_iavg); 
                printf("Iavg power = 0x%x\n", tx_iavg);     
#if 0        
                setenv("TXPWR1",argv[2],1);
                //setenv("TXMPD1","AA",1);    
                ret = system("export TXMPD1=AA");
                
                printf("ret = %d \n",ret);
                printf("TXPWR1=%s \n",getenv("TXPWR1"));
                printf("TXPWR2=%s \n",getenv("TXPWR2"));
                printf("TXMPD1=%s \n",getenv("TXMPD1"));
                printf("TXMPD2=%s \n",getenv("TXMPD2"));
#endif                                
               }
            else
            {
                printf("%s %s <tx_pwr1> \n", argv[0], argv[1]);
            }
        }
        else if(0 == strcmp(argv[1], "2"))
        {      
            if (argc >= 3)
            {
                printf("Set TXPWR2 data.\n"); 
                tx_pwr[1] = _vlaue_translate(argv[2]);        
                _europa_cal_data_add(EUROPA_CAL_TXPWR2_ADDR, tx_pwr[1]);                       
                tx_mpd[1] = _europa_cal_get_mpd_current();
                _europa_cal_data_add(EUROPA_CAL_TXMPD2_ADDR, tx_mpd[1]);           
                printf("Data2: Power=%6dnW Impd=%6dnA\n",
                   tx_pwr[1], tx_mpd[1]);
        
#if 0           
                setenv("TXPWR2",argv[2],1);
                setenv("TXMPD2","BB",1);

                printf("TXPWR1=%s \n",getenv("TXPWR1"));
                printf("TXPWR2=%s \n",getenv("TXPWR2"));
                printf("TXMPD1=%s \n",getenv("TXMPD1"));
                printf("TXMPD2=%s \n",getenv("TXMPD2"));
#endif
                
#if 0            
               tx_pwr[1] = *value_ptr;
               tx_mpd[1] = _ldd_get_mpd_current();
        
               printf("Data1: Power=%6dnW Impd=%6dnA\n",
                   tx_pwr[0], tx_mpd[0]);
               printf("Data2: Power=%6dnW Impd=%6dnA\n",
                   tx_pwr[1], tx_mpd[1]); 
#endif               
            }
            else
            {
                printf("%s %s <tx_pwr2> \n", argv[0], argv[1]);
            }            
        }  
    }
    else
    {
        printf("%s <case> \n", argv[0]);
        printf("case can be 0, 1, 2, and calc\n");
        return -1;
    }

#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]: europa_cli_cal_tx_ddmi Duration = %f ms\n", Duration);
#endif
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }

    
    return 0;    
}

static int europa_cli_cal_rx_ddmi(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    int wfd = -1;    
    int32 addr;
    int32 rx_a, rx_b, rx_c;
    uint8 value, temp8, i;
    uint8 *init_data, *ptr_data;
    uint32 temp32;
    uint32 rx_pwr[3];
    uint32 rx_rssi[3]; 
    double a,b,c;      
#ifdef RTL8290B_CAL_TIME
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif   
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {     
        ret = rtl8290c_cli_cal_rx_ddmi(devId, argc, argv, fp);
        return ret;
    } 
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {     
        ret = rtl8291_cli_cal_rx_ddmi(devId, argc, argv, fp);
        return ret;
    }     
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
    if(argc >= 2)
    { 
        if(0 == strcmp(argv[1], "0"))
        {   
            printf("reset cal data.\n");

#if 0            
            fp = fopen(EUROPA_CAL_FILE_LOCATION, "wb");          
            if (NULL ==fp)
            {
                printf("Open %s error!!!!!!!\n", EUROPA_CAL_FILE_LOCATION);
                return -1;               
            }         
            init_data = (uint8 *)malloc(EUROPA_CAL_FILE_SIZE);
            memset(init_data, 0x0, sizeof(char)*EUROPA_CAL_FILE_SIZE);      
            fseek(fp, 0, SEEK_SET); 
            fwrite(init_data, sizeof(char), EUROPA_CAL_FILE_SIZE,fp);        
            fclose(fp);
            free(init_data);
#else

            //printf("reset cal data. (rb) \n");

            fp = fopen(EUROPA_CAL_FILE_LOCATION,"rb");
            if (NULL ==fp)
            {
                printf("%s does not exist. Create an empty file.\n", EUROPA_CAL_FILE_LOCATION);       
                fp = fopen(EUROPA_CAL_FILE_LOCATION,"wb");
                if (NULL ==fp)
                {
                    printf("Create %s error!!!!!!!\n", EUROPA_CAL_FILE_LOCATION); 
                    return -1;    
                }
                fclose(fp);
                return -1;
            }

            init_data = (uint8 *)malloc(EUROPA_CAL_FILE_SIZE*sizeof(uint8));
            memset(init_data, 0x0, sizeof(char)*EUROPA_CAL_FILE_SIZE);    
            ret=fseek(fp, 0, SEEK_SET);
            if (ret!=0)
            {
                fclose(fp); 
                free(init_data);
                return ret;
            }
            ret=fread(init_data, sizeof(char), EUROPA_CAL_FILE_SIZE, fp);
            //if (ret!=0)
            //{
            //    fclose(fp); 
            //    free(init_data);
            //    return ret;
            //}
            fclose(fp);

            //Clear RX DDMI parameter
            ptr_data = init_data + EUROPA_CAL_RXPWR1_ADDR;
            memset(ptr_data, 0x0, sizeof(char)*4*6); 

            fp = fopen(EUROPA_CAL_FILE_LOCATION, "wb");          
            if (NULL ==fp)
            {
                printf("Open %s error!!!!!!!\n", EUROPA_CAL_FILE_LOCATION);
                free(init_data);            
                return -1;             
            }  
            ret=fseek(fp, 0, SEEK_SET);
            if (ret!=0)
            {
                fclose(fp); 
                free(init_data);
                return ret;
            }
            fwrite(init_data, sizeof(char), EUROPA_CAL_FILE_SIZE,fp);           
            free(init_data);
      
            fflush(fp); 
            wfd = fileno(fp);
            if(wfd != -1)
            {
                fsync(wfd);
            }
  
            //printf("\n%s: %d\n",__FUNCTION__, __LINE__); 
#if 0
            addr = EUROPA_CAL_LASERLUT_BASE_ADDR;
            fseek(fp, addr, SEEK_SET);
            memset(gLaserLutData, 0x0, sizeof(gLaserLutData));
            fread(gLaserLutData, 1, sizeof(gLaserLutData), fp);
            
            printf("\n%s: %s\n",__FUNCTION__, __LINE__); 
        
            addr += sizeof(gLaserLutData);      
            fseek(fp, sizeof(gLaserLutData), SEEK_SET);
            memset(gLaserLut, 0x0, sizeof(gLaserLut));
            fread(gLaserLut, 1, sizeof(gLaserLut), fp); 
#endif    
            fclose(fp);
#endif
            
                          
        }
        else if(0 == strcmp(argv[1], "calc"))
        {
            printf("calculate cal data.\n");
            _europa_cal_data_get(EUROPA_CAL_RXPWR1_ADDR, &rx_pwr[0]);
            _europa_cal_data_get(EUROPA_CAL_RXRSSI1_ADDR, &rx_rssi[0]);             
            _europa_cal_data_get(EUROPA_CAL_RXPWR2_ADDR, &rx_pwr[1]);
            _europa_cal_data_get(EUROPA_CAL_RXRSSI2_ADDR, &rx_rssi[1]); 
            _europa_cal_data_get(EUROPA_CAL_RXPWR3_ADDR, &rx_pwr[2]);
            _europa_cal_data_get(EUROPA_CAL_RXRSSI3_ADDR, &rx_rssi[2]);                
            
            if ((rx_rssi[0]!=0)&&(rx_rssi[1]!=0)&&(rx_rssi[2]!=0))
            {        
                printf("---- regression ...\n");
                _europa_cal_regression(rx_rssi[0], rx_pwr[0],
                             rx_rssi[1], rx_pwr[1],
                             rx_rssi[2], rx_pwr[2], &a, &b, &c);
                printf("    a = %f\n", a);
                printf("    b = %f\n", b);
                printf("    c = %f\n", c);
                 
                printf("---- shifting...\n");
                a = a*(1<<RX_A_SHIFT);
                b = b*(1<<RX_B_SHIFT);
                c = c*(1<<RX_C_SHIFT);
                printf("    a = %f\n", a);
                printf("    b = %f\n", b);
                printf("    c = %f\n", c);
                    
                printf("---- casting...\n");
                rx_a = (int32) a;
                rx_b = (int32) b;
                rx_c = (int32) c;
                printf("    a = 0x%08x\n", rx_a);
                printf("    b = 0x%08x\n", rx_b);
                printf("    c = 0x%08x\n", rx_c);
                    
                // save immediately
                _europa_cal_flash_data_set(RTL8290B_RX_A_ADDR, 4, (uint32)rx_a);
                _europa_cal_flash_data_set(RTL8290B_RX_B_ADDR, 4, (uint32)rx_b);  
                _europa_cal_flash_data_set(RTL8290B_RX_C_ADDR, 4, (uint32)rx_c); 
            }
            else
            {
                printf("Data1: Power=%6dnW Irssi=%6dnA\n",
                       rx_pwr[0], rx_rssi[0]);             
                printf("Data2: Power=%6dnW Irssi=%6dnA\n",
                       rx_pwr[1], rx_rssi[1]);           
                    
                printf("---- regression ...\n");
                _europa_cal_lin_regression(rx_rssi[0], rx_pwr[0], rx_rssi[1], rx_pwr[1],
                                       &b, &c);
                printf("  b = %f\n", b);
                printf("  c = %f\n", c);
                        
                printf("---- shifting...\n");
                b = b*(1<<RX_B_SHIFT);
                c = c*(1<<RX_C_SHIFT);
                printf("  b = %f\n", b);
                printf("  c = %f\n", c);
                        
                printf("---- casting...\n");
                rx_a = 0;                
                rx_b = (int32) b;
                rx_c = (int32) c;
                printf("  b = 0x%08x\n", rx_b);
                printf("  c = 0x%08x\n", rx_c);
                        
                // save immediately
                _europa_cal_flash_data_set(RTL8290B_RX_A_ADDR, 4, (uint32)rx_a);
                _europa_cal_flash_data_set(RTL8290B_RX_B_ADDR, 4, (uint32)rx_b);  
                _europa_cal_flash_data_set(RTL8290B_RX_C_ADDR, 4, (uint32)rx_c); 

            }
        }
        else if(0 == strcmp(argv[1], "1"))
        {       
            if (argc >= 3)
            {
                printf("Set RXPWR1 data.\n"); 
                rx_pwr[0] = _vlaue_translate(argv[2]); 
                _europa_cal_data_add(EUROPA_CAL_RXPWR1_ADDR, rx_pwr[0]);                   
                rx_rssi[0] = _europa_cal_get_rssi_current();
                _europa_cal_data_add(EUROPA_CAL_RXRSSI1_ADDR, rx_rssi[0]);                   
                    
                printf("Data1: Power=%6dnW Irssi=%6dnA\n",
                        rx_pwr[0], rx_rssi[0]);                               
            }
            else
            {
                    printf("%s %s <rx_pwr1> \n", argv[0], argv[1]);
            }
        }
        else if(0 == strcmp(argv[1], "2"))
        {       
            if (argc >= 3)
            {
                printf("Set RXPWR2 data.\n"); 
                rx_pwr[1] = _vlaue_translate(argv[2]); 
                _europa_cal_data_add(EUROPA_CAL_RXPWR2_ADDR, rx_pwr[1]);                   
                rx_rssi[1] = _europa_cal_get_rssi_current();
                _europa_cal_data_add(EUROPA_CAL_RXRSSI2_ADDR, rx_rssi[1]);                  
                printf("Data2: Power=%6dnW Irssi=%6dnA\n",
                       rx_pwr[1], rx_rssi[1]);                        
            }
            else
            {
                printf("%s %s <rx_pwr2> \n", argv[0], argv[1]);
            }             
        } 
        else if(0 == strcmp(argv[1], "3"))
        {       
            if (argc >= 3)
            {
                printf("Set RXPWR3 data.\n"); 
                rx_pwr[2] = _vlaue_translate(argv[2]); 
                _europa_cal_data_add(EUROPA_CAL_RXPWR3_ADDR, rx_pwr[2]);                   
                rx_rssi[2] = _europa_cal_get_rssi_current();
                _europa_cal_data_add(EUROPA_CAL_RXRSSI3_ADDR, rx_rssi[2]);                  
                printf("Data3: Power=%6dnW Irssi=%6dnA\n",
                       rx_pwr[2], rx_rssi[2]);                        
            }
            else
            {
                printf("%s %s <rx_pwr3> \n", argv[0], argv[1]);
            }             
        }          
        else if(0 == strcmp(argv[1], "4"))
        {       
            if (argc >= 3)
            {
                printf("Set RXPWR1 data. (Offline)\n"); 
                rx_pwr[0] = _vlaue_translate(argv[2]); 
                _europa_cal_data_add(EUROPA_CAL_RXPWR1_ADDR, rx_pwr[0]);                   
                //rx_rssi[0] = _europa_cal_get_rssi_current();
                rx_rssi[0] = _vlaue_translate(argv[3]);                 
                _europa_cal_data_add(EUROPA_CAL_RXRSSI1_ADDR, rx_rssi[0]);                                  
                printf("Data1: Power=%6dnW Irssi=%6dnA\n",
                        rx_pwr[0], rx_rssi[0]);                               
            }
            else
            {
                    printf("%s %s <rx_pwr1> \n", argv[0], argv[1]);
            }
        }
        else if(0 == strcmp(argv[1], "5"))
        {       
            if (argc >= 3)
            {
                printf("Set RXPWR2 data. (Offline)\n"); 
                rx_pwr[1] = _vlaue_translate(argv[2]); 
                _europa_cal_data_add(EUROPA_CAL_RXPWR2_ADDR, rx_pwr[1]);                   
                //rx_rssi[1] = _europa_cal_get_rssi_current();
                rx_rssi[1] = _vlaue_translate(argv[3]);                     
                _europa_cal_data_add(EUROPA_CAL_RXRSSI2_ADDR, rx_rssi[1]);                  
                printf("Data2: Power=%6dnW Irssi=%6dnA\n",
                       rx_pwr[1], rx_rssi[1]);                        
            }
            else
            {
                printf("%s %s <rx_pwr2> \n", argv[0], argv[1]);
            }             
        } 
        else if(0 == strcmp(argv[1], "6"))
        {       
            if (argc >= 3)
            {
                printf("Set RXPWR3 data. (Offline)\n"); 
                rx_pwr[2] = _vlaue_translate(argv[2]); 
                _europa_cal_data_add(EUROPA_CAL_RXPWR3_ADDR, rx_pwr[2]);                   
                //rx_rssi[2] = _europa_cal_get_rssi_current();
                rx_rssi[2] = _vlaue_translate(argv[3]);                     
                _europa_cal_data_add(EUROPA_CAL_RXRSSI3_ADDR, rx_rssi[2]);                  
                printf("Data3: Power=%6dnW Irssi=%6dnA\n",
                       rx_pwr[2], rx_rssi[2]);                        
            }
            else
            {
                printf("%s %s <rx_pwr3> \n", argv[0], argv[1]);
            }             
        }        
        else if(0 == strcmp(argv[1], "7"))
        {
            printf("calculate cal data.\n");
            _europa_cal_data_get(EUROPA_CAL_RXPWR1_ADDR, &rx_pwr[0]);
            _europa_cal_data_get(EUROPA_CAL_RXRSSI1_ADDR, &rx_rssi[0]);             
            _europa_cal_data_get(EUROPA_CAL_RXPWR2_ADDR, &rx_pwr[1]);
            _europa_cal_data_get(EUROPA_CAL_RXRSSI2_ADDR, &rx_rssi[1]); 
            _europa_cal_data_get(EUROPA_CAL_RXPWR3_ADDR, &rx_pwr[2]);
            _europa_cal_data_get(EUROPA_CAL_RXRSSI3_ADDR, &rx_rssi[2]);                
            
            printf("---- regression ...\n");
            _europa_cal_regression(rx_rssi[0], rx_pwr[0],
                         rx_rssi[1], rx_pwr[1],
                         rx_rssi[2], rx_pwr[2], &a, &b, &c);
            printf("    a = %f\n", a);
            printf("    b = %f\n", b);
            printf("    c = %f\n", c);
                
            printf("---- shifting...\n");
            a = a*(1<<RX_A_SHIFT);
            b = b*(1<<RX_B_SHIFT);
            c = c*(1<<RX_C_SHIFT);
            printf("    a = %f\n", a);
            printf("    b = %f\n", b);
            printf("    c = %f\n", c);
                
            printf("---- casting...\n");
            rx_a = (int32) a;
            rx_b = (int32) b;
            rx_c = (int32) c;
            printf("    a = 0x%08x\n", rx_a);
            printf("    b = 0x%08x\n", rx_b);
            printf("    c = 0x%08x\n", rx_c);
                     
        }        
    }
    else
    {
        printf("%s <case> \n", argv[0]);
        printf("case can be 0, 1, 2, 3 and calc\n");
        return -1;
    }
#ifdef RTL8290B_CAL_TIME    
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]: europa_cli_cal_rx_ddmi Duration = %f ms\n", Duration);
#endif
    } 
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }
    
    return 0; 

}

static int europa_cli_cal_power(
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value;      
    int ret;
    double measured_pwr, target_pwr;
    double measured_uW, target_uW;
    double current_iavg, target_iavg, w58_iavg, w58_target;
    uint32 data_w58_original, data_w58_target, data_temp;
    uint32 devId = 5;
    int chip_mode;
#ifdef RTL8290B_CAL_TIME    
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {     
        ret = rtl8290c_cli_cal_power(devId, argc, argv, fp);
        return ret;
    } 
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {     
        ret = rtl8291_cli_cal_power(devId, argc, argv, fp);
        return ret;
    }     
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
    if(argc < 2)
    {
        addr = RTL8290B_APC_IAVG_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_get(addr, 1, &value);  
        printf("Get iavg power = 0x%x\n", value);
    
        //printf("%s auto <measured pwr> <target pwr> \n", argv[0]);   
        //printf("Or %s <value>\n", argv[0]);         
        //return -1;
    }
    else if (argc == 2)
    {
        if ((ret=strcmp(argv[1], "auto")) != 0 )
        { 
            value = _vlaue_translate(argv[1]);   
            addr = RTL8290B_APC_IAVG_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
            _europa_cal_flash_data_set(addr, 1, value);
            printf("Set iavg power = 0x%x\n", value);     
            rtk_ldd_apcIavg_set((uint16)value);        
        }
        else
        { 
            printf("%s auto <measured pwr> <target pwr> \n", argv[0]);   
        }
    }
    else if(argc >= 4)
    {
        if ((ret=strcmp(argv[1], "auto")) == 0 )
        {

            measured_pwr = atof(argv[2]);     
            target_pwr = atof(argv[3]); 
            
            //printf("measured pwr = %lf dBm,  target pwr = %lf dBm  \n", measured_pwr, target_pwr);           
            
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 58, &data_w58_original); 
            //printf("W58 = 0x%x \n", data_w58_original);        
            w58_iavg = (double)data_w58_original/32;
            current_iavg = 25*pow(2,w58_iavg);
            //printf("current_iavg = %lf \n", current_iavg);        
            measured_uW = _europa_dBm2uw(measured_pwr);
            target_uW = _europa_dBm2uw(target_pwr);    
            //printf("measured pwr = %lf uW,    target pwr = %lf uW  \n", measured_uW, target_uW); 
            target_iavg = current_iavg*(target_uW/measured_uW);
            //printf("target_iavg = %lf \n", target_iavg);  
            w58_target = 32*log10(target_iavg/25)/log10(2);
            
            data_w58_target = (uint32)w58_target;
            
            printf("taget W58 = 0x%x\n", data_w58_target); 
            
            if ((data_w58_target < 0x20)||(data_w58_target > 0xC0))
            {    
                printf("Overflow! Taget value should be 0x20~0xC0.\n");     
            }
            else
            {
                if (data_w58_target<(data_w58_original-0x10))
                {
                   //To avoid DCL lost lock, use while loop to do step-by-step.
                   data_temp = data_w58_original; 
                   while((data_temp-0x10)>data_w58_target)
                   {
                       data_temp-=0x10;
                       europa_i2c_write(EUROPA_I2C_PORT, 0x54, 58, data_temp);
                       //printf("data_temp = 0x%x\n", data_temp);                            
                       osal_time_mdelay(20);    
                   }           
                }
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 58, data_w58_target); 
                addr = RTL8290B_APC_IAVG_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
                _europa_cal_flash_data_set(addr, 1, data_w58_target);
                printf("Set iavg power = 0x%x\n", data_w58_target);                    
            }                
        }
        else
        {
            printf("%s auto <measured pwr> <target pwr> \n", argv[0]);   

        }

    }
    else
    { 
        printf("%s auto <measured pwr> <target pwr> \n", argv[0]);   
        printf("Or %s <value>\n", argv[0]); 

    }
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }
    
#ifdef RTL8290B_CAL_TIME    
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]: europa_cli_cal_power Duration = %f ms\n", Duration);
#endif
    return 0; 
}

static int europa_cli_cal_er(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 addr, value; 
    uint32 devId = 5;    
    int chip_mode;    
#ifdef RTL8290B_CAL_TIME   
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    { 
        ret = rtl8290c_cli_cal_er(devId, argc, argv, fp);
        return ret;
    }
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    { 
        ret = rtl8291_cli_cal_er(devId, argc, argv, fp);
        return ret;
    }    
    else  if(EUROPA_LDD_8290B_MODE == chip_mode)
    { 
    if(argc >= 2)
    { 
        value = _vlaue_translate(argv[1]);   
        if (value>0xF)
        {
            printf("Input should be 0~0xF\n");       
            return -1;
        }
        addr = RTL8290B_APC_ER_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        printf("Set er = 0x%x\n", value);   
        rtk_ldd_apcEr_set((uint16)value);        
        _europa_ibim_set();    
    }
    else
    { 
        addr = RTL8290B_APC_ER_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_get(addr, 1, &value);  
        printf("er = 0x%x\n", value);

    }
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }
    
#ifdef RTL8290B_CAL_TIME    
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]: europa_cli_cal_er Duration = %f ms\n", Duration);
#endif
    return 0; 
}

static int europa_cli_cal_trim(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 addr, value; 
    uint32 devId = 5;    
    int chip_mode;    
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {     
        ret = rtl8290c_cli_cal_trim(devId, argc, argv, fp);
        return ret;
    } 
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
    if(argc >= 2)
    { 
        value = _vlaue_translate(argv[1]);   
        if (value>0x1F)
        {
            printf("Input should be 0~0x1F\n");       
            return -1;
        }
        addr = RTL8290B_APC_ER_TRIM_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        printf("Set trim = 0x%x\n", value);      
        rtk_ldd_apcErTrim_set((uint16)value);                    
        _europa_ibim_set();        
    }
    else
    { 
        addr = RTL8290B_APC_ER_TRIM_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_get(addr, 1, &value);  
        printf("trim = 0x%x\n", value);
    }
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }    
#ifdef RTL8290B_CAL_TIME    
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]: europa_cli_cal_trim Duration = %f ms\n", Duration);
#endif    
    return 0; 
}

static int europa_cli_cal_los(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 addr, value;
    uint16 data;
    uint32 devId = 5;    
    int chip_mode;    
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif 

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {     
        ret = rtl8290c_cli_cal_los(devId, argc, argv, fp);
        return ret;
    } 
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {     
        ret = rtl8291_cli_cal_los(devId, argc, argv, fp);
        return ret;
    } 
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {

    if(argc >= 2)
    { 
        value = _vlaue_translate(argv[1]);   
        addr = RTL8290B_RXLOS_REF_DAC_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        printf("Set los = 0x%x\n", value);     
        rtk_ldd_rxlosRefDac_set((uint16)(value&0x7F));        
    }
    else
    { 
        addr = RTL8290B_RXLOS_REF_DAC_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_get(addr, 1, &value);  
        printf("los = 0x%x\n", value);
        rtk_ldd_rxlosRefDac_get(&data);
        printf("data = 0x%x\n", data);
    }
       }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }    
 #ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_los Duration = %f ms\n", Duration);
#endif
   
    return 0; 
}

static int europa_cli_cal_hyst(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 addr, value;
    uint32 devId = 5;    
    int chip_mode;
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {     
        ret = rtl8290c_cli_cal_hyst(devId, argc, argv, fp);
        return ret;
    } 
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {     
        ret = rtl8291_cli_cal_hyst(devId, argc, argv, fp);
        return ret;
    } 
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {

    if(argc >= 2)
    { 
        value = _vlaue_translate(argv[1]);   
        addr = RTL8290B_RXLOS_HYST_SEL_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        printf("Set hyst = 0x%x\n", value);   
        rtk_ldd_rxlosHystSel_set((uint16)value);                 
    }
    else
    { 
        addr = RTL8290B_RXLOS_HYST_SEL_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_get(addr, 1, &value);  
        printf("hyst = 0x%x\n", value);

    }
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }    
 #ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_hyst Duration = %f ms\n", Duration);
#endif
   
    return 0; 
}


static int europa_cli_cal_cmpd(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 addr, value, value2;      
    uint32 bias, mod, cmpd;
    uint32 chipId, rev, subType;
    uint32 ib, im; 
    uint32 devId = 5;    
    int chip_mode;
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {     
        ret = rtl8290c_cli_cal_cmpd(devId, argc, argv, fp);
        return ret;
    } 
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {     
        ret = rtl8291_cli_cal_cmpd(devId, argc, argv, fp);
        return ret;
    } 
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {

    if(argc >= 2)
    { 

        if((ret = rtk_switch_version_get(&chipId, &rev, &subType)) != 0)
        {
            return ret;
        }
    
        if(0 == strcmp(argv[1], "check"))
        {  
            _europa_ibim_low();
    
            if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
            {
                system("diag reg set 0x40a30 0x000"); 
                system("diag register set 0x40540 0x30");                
            }
            else if (RTL9602C_CHIP_ID == chipId)
            {
                system("diag reg set 0x022A30 0x000"); 
                system("diag register set 0x22540 0x30");  
            }
#if defined(CONFIG_LUNA_G3_SERIES)    
            //For Cortina ARM-based 9607DQ series
            else if (RTL8277C_CHIP_ID == chipId)
            {
                system("rt_ponmisc set force-laser-on state enable");
            }
#endif
            else
            {
                printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
            }

            osal_time_mdelay(100);

            bias = 0;
            mod = 0;
            cmpd = 0;
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 60, &value);   
            cmpd = value&0x7;        
            while((bias>=mod)&&(cmpd !=0))
            {
                ret = rtk_ldd_tx_bias_get(&bias);
                if(ret)
                {
                    printf("Get TX Bias Fail!!! (%d)\n", ret);
                    return -1;
                }
                        

                ret = rtk_ldd_tx_mod_get(&mod);
                if(ret)
                {
                    printf("Get TX Mod Fail!!! (%d)\n", ret);
                    return -1;
                }
                
                //printf("cmpd = 0x%x, ib = 0x%x, im = 0x%x\n", cmpd, bias, mod);

                if (bias>=mod)
                {
                    cmpd = cmpd-1;
                    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 60, &value);   
                    value = (value&0xF8)|cmpd;
                    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 60, value); 
                    osal_time_mdelay(150);                        
                }
            }

            addr = RTL8290B_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
            _europa_cal_flash_data_set(addr, 1, cmpd);            
            printf("cmpd = 0x%x, ib = 0x%x, im = 0x%x\n", cmpd, bias, mod);

            if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
            {
                //EPON PRBS OFF
                system("diag register set 0x40a30 0"); 
                //GPON PRBS OFF
                system("diag register set 0x40098 0"); 
                //BEN OFF
                system("diag register set 0x360a0 0");
                /*force BEN=0*/                
                system("diag register set 0x40540 0");
            }
            else if (RTL9602C_CHIP_ID == chipId)
            {
                //EPON PRBS OFF           
                system("diag register set 0x22A30 0");
                //GPON PRBS OFF
                system("diag register set 0x22098 0"); 
                //BEN OFF
                system("diag register set 0x360a0 0");    
                /*force BEN=0*/                
                system("diag register set 0x22540 0");
            }
#if defined(CONFIG_LUNA_G3_SERIES)
            //For Cortina ARM-based 9607DQ series
            else if (RTL8277C_CHIP_ID == chipId)
            {
                system("rt_ponmisc set force-laser-on state disable");
            }
#endif
            else
            {
                printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
            } 

            _europa_ibim_recover();  
            
        }
        else
        {    
            value = _vlaue_translate(argv[1]); 
            if (value > 0x7)
            {
                printf("Value = 0x%x > 0x7\n", value);
                return -1; 
            }
            
            addr = RTL8290B_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
            _europa_cal_flash_data_set(addr, 1, value);
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 60, &value2);   
            value2 = (value2&0xF8)|value;
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 60, value2);             
            printf("Set cmpd = 0x%x (0x%x)\n", value, value2);
        }
    }
    else
    { 
        addr = RTL8290B_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_get(addr, 1, &value);  
        printf("cmpd = 0x%x\n", value);
    }
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }    
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_cmpd Duration = %f ms\n", Duration);
#endif
    
    return 0; 
}

static int europa_cli_cal_lpf(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 addr, value, value2;
    uint32 devId = 5;    
    int chip_mode;
 #ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {     
        ret = rtl8290c_cli_cal_lpf(devId, argc, argv, fp);
        return ret;
    } 
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {     
        ret = rtl8291_cli_cal_lpf(devId, argc, argv, fp);
        return ret;
    }
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
    if(argc >= 2)
    { 
        value = _vlaue_translate(argv[1]);   
        if (value > 0x3)
        {
            printf("Value = 0x%x > 0x3\n", value);
            return -1; 
        }
    
        addr = RTL8290B_APC_LPF_BW_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 64, &value2);    
        value2 = (value2&0xFC)|value;
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 64, value2);            
        printf("Set LPF = 0x%x\n", value);                         
    }
    else
    { 
        addr = RTL8290B_APC_LPF_BW_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_get(addr, 1, &value);  
        printf("LPF = 0x%x\n", value);
    }
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }
    
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_lpf Duration = %f ms\n", Duration);
#endif
    
    return 0; 
}

static int europa_cli_txif_set(
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value;      
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }
    
    if(argc >= 2)
    { 
        value = _vlaue_translate(argv[1]);   
        addr = RTL8290B_TX_INTERFACE_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        printf("Set TX interface = 0x%x\n", value);                         
    }
    else
    { 
        printf("%s <value>\n", argv[0]);    
        return -1; 
    }
    
    return 0; 
}

static int europa_cli_txif_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value;      
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }   
    addr = RTL8290B_TX_INTERFACE_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
    _europa_cal_flash_data_get(addr, 1, &value);  
    printf("TX interface = 0x%x\n", value);

    return 0; 
}

static int europa_cli_cal_cross(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 addr, value, value2, value3;      
    uint16 data1, data2;
    rtk_enable_t enable;
    uint32 devId = 5;    
    int chip_mode;
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif   

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {     
        ret = rtl8290c_cli_cal_cross(devId, argc, argv, fp);
        return ret;
    } 
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {     
        ret = rtl8291_cli_cal_cross(devId, argc, argv, fp);
        return ret;
    }     
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
    if(argc >= 2)
    { 
        if(0 == strcmp(argv[1], "show"))
        {
            addr = RTL8290B_TX_CROSS_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
            _europa_cal_flash_data_get(addr, 1, &value); 
            printf("tx cross enable = 0x%x\n", value);
            addr = RTL8290B_TX_CROSS_SIGN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
            _europa_cal_flash_data_get(addr, 1, &value); 
            printf("tx cross sign = 0x%x\n", value);
            addr = RTL8290B_TX_CROSS_STR_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
            _europa_cal_flash_data_get(addr, 1, &value); 
            printf("tx cross str = 0x%x\n", value);
            rtk_ldd_txCross_get(&enable, &data1, &data2);              
            printf("Current tx cross enable = 0x%x, sign = 0x%x, str = 0x%x\n", enable, data1, data2);            
        }
        else if(0 == strcmp(argv[1], "disable"))
        {        
            value = 0;     
            addr = RTL8290B_TX_CROSS_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
            _europa_cal_flash_data_set(addr, 1, value);
            printf("Set TX cross disable\n");
            rtk_ldd_txCross_get(&value, &value2, &value3);    
            rtk_ldd_txCross_set(0, value2, value3);    
        }
        else if(0 == strcmp(argv[1], "up"))
        {
            if(argc >= 3) 
            {
                value = 0;
                addr = RTL8290B_TX_CROSS_SIGN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
                _europa_cal_flash_data_set(addr, 1, value); 
                printf("tx cross sign = 0x%x\n", value);
                value = 1;          
                addr = RTL8290B_TX_CROSS_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
                _europa_cal_flash_data_set(addr, 1, value); 
                printf("tx cross enable = 0x%x\n", value);
                value = _vlaue_translate(argv[2]);           
                addr = RTL8290B_TX_CROSS_STR_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
                _europa_cal_flash_data_set(addr, 1, value); 
                printf("tx cross str = 0x%x\n", value);  
                rtk_ldd_txCross_set(1, 0, value);            
            }
            else
            {
                printf("%s <up> <value>\n", argv[0]);
                return -1; 
            } 
            
        }
        else if(0 == strcmp(argv[1], "down"))
        {        
            if(argc >= 3) 
            {
                value = 1;
                addr = RTL8290B_TX_CROSS_SIGN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
                _europa_cal_flash_data_set(addr, 1, value); 
                printf("tx cross sign = 0x%x\n", value);
                value = 1;          
                addr = RTL8290B_TX_CROSS_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
                _europa_cal_flash_data_set(addr, 1, value); 
                printf("tx cross enable = 0x%x\n", value);
                value = _vlaue_translate(argv[2]);           
                addr = RTL8290B_TX_CROSS_STR_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
                _europa_cal_flash_data_set(addr, 1, value); 
                printf("tx cross str = 0x%x\n", value);  
                rtk_ldd_txCross_set(1, 1, value);                    
            }
            else
            {
                printf("%s <down> <value>\n", argv[0]);
                return -1; 
            }

        }
        else
        {
            printf("%s <up/down> <value>\n", argv[0]);
            printf("%s <show/enable>\n", argv[0]);
            return -1;  
        }  
       
    }
    else
    { 

        printf("%s <up/down> <value>\n", argv[0]);
        printf("%s <show/enable>\n", argv[0]);
        return -1; 
    }
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }    
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_rxlos Duration = %f ms\n", Duration);
#endif
    
    return 0; 
}

static int europa_cli_cal_toffset(
    int argc,
    char *argv[],
    FILE *fp)
{
    int32 ret, temp;
    int8 offset;   
    uint32 devId = 5;    
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {     
        ret = rtl8290c_cli_cal_toffset(devId, argc, argv, fp);
        return ret;
    } 
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {     
        ret = rtl8291_cli_cal_toffset(devId, argc, argv, fp);
        return ret;
    }    
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
    if(argc >= 3)
    { 
        /* optional parameter means assign offset directly */
        printf("Ignore temperature input.\n");
        offset = _vlaue_translate(argv[2]); 
        printf("Temperature offset=%d.\n", offset);    
        _europa_cal_flash_data_set(RTL8290B_TEMP_OFFSET_ADDR, 1, offset&0xFF);          
    }
    else if (argc == 2)
    {
        uint16 tempK;
        int32 tempC;

        temp = _vlaue_translate(argv[1]);         
        ret = rtk_ldd_temperature_get(&tempK);
        if(ret)
        {
            printf("Get Temperature Fail!!! (%d)\n", ret);
            return -1;
        }
        
        tempC = (tempK*100-27315)/100; /* degK to degC */
        printf("Temperature=%dK (%dC)\n", tempK, tempC);
        
        offset = (int8)(tempC - temp);

        _europa_cal_flash_data_set(RTL8290B_TEMP_OFFSET_ADDR, 1, offset&0xFF);          

    }
    else
    {
        printf("%s <INT:temperature> <INT:offset> \n", argv[0]);      
        return -1;
    }
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    } 

    return 0;    
}

static int europa_cli_cal_tscale(
    int argc,
    char *argv[],
    FILE *fp)
{
    int32 ret, temp;
    uint8 scale;   
    uint32 devId = 5;    
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {     
        ret = rtl8290c_cli_cal_tscale(devId, argc, argv, fp);
        return ret;
    } 
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {

    if (argc >= 2)
    {
        scale = _vlaue_translate(argv[1]); 

        if (0<scale && 8>scale)
        {
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 91, scale);
            printf("Temperature scale=%d.\n", scale);
            
            //_europa_cal_flash_data_set(RTL8290B_TEMP_INTR_SCALE_ADDR, 1, scale);                
        }
        else
        {
           printf("Invalid temperature scale=%d.\n", scale);
           return -1;
        }
    }
    else
    {
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 91, &scale);
            printf("Read temperature scale=%d.\n", scale);
            
            //_europa_cal_flash_data_get(RTL8290B_TEMP_INTR_SCALE_ADDR, 1,&scale);  ;
    }
    }
    return 0;    
}

static int europa_cli_cal_rxpol(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 addr, value; 
    uint32 devId = 5;    
    int chip_mode;    
#ifdef RTL8290B_CAL_TIME   
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    { 
        ret = rtl8290c_cli_cal_rxpol(devId, argc, argv, fp);
        return ret;
    }
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    { 
        ret = rtl8291_cli_cal_rxpol(devId, argc, argv, fp);
        return ret;
    }    
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }
    
#ifdef RTL8290B_CAL_TIME    
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]: europa_cli_cal_rxpol Duration = %f ms\n", Duration);
#endif
    return 0; 
}

static int europa_cli_cal_rxlospol(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 addr, value; 
    uint32 devId = 5;    
    int chip_mode;    
#ifdef RTL8290B_CAL_TIME   
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    { 
        ret = rtl8290c_cli_cal_rxlospol(devId, argc, argv, fp);
        return ret;
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }
    
#ifdef RTL8290B_CAL_TIME    
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]: europa_cli_cal_rxlospol Duration = %f ms\n", Duration);
#endif
    return 0; 
}

static int europa_cli_txbias_set(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    double curr;
    uint32 Ibias;
    uint32 devId = 5;    
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    { 
        ret = rtl8290c_cli_txbias_set(devId, argc, argv, fp);
        return ret;
    }
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    { 
        ret = rtl8291_cli_txbias_set(devId, argc, argv, fp);
        return ret;
    }    
    else  if(EUROPA_LDD_8290B_MODE == chip_mode)
    {
    if(argc >= 2)
    { 
        curr = atof(argv[1]);    

        if (100.0 < curr)
        {
            printf("Invalid bias current %f\n", curr);
            return -1;
        }
    
        Ibias = (uint32)(curr * 1000 / 2); /* convert mA to 2uA unit */
    
        printf("Input current = %f, Ibias = 0x%x\n", curr, Ibias);

        ret = rtk_ldd_tx_bias_set(Ibias);
        if(ret)
        {
            printf("Set TX Bias Fail!!! (%d)\n", ret);
            return -1;
        }
    
        return 0;

    }
    else
    {
        printf("%s <tx bias> \n", argv[0]);
        printf("The unit is mA \n");        
        return -1;
    }
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }
        
    return 0;    
}


static int europa_cli_txmod_set(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    double curr;
    uint32 Imod;
    uint32 mod_sys;
    uint32 devId = 5;    
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    { 
        ret = rtl8290c_cli_txmod_set(devId, argc, argv, fp);
        return ret;
    }
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    { 
        ret = rtl8291_cli_txmod_set(devId, argc, argv, fp);
        return ret;
    }    
    else  if(EUROPA_LDD_8290B_MODE == chip_mode)
    {
    if(argc >= 2)
    { 
        curr = atof(argv[1]);    

        if (100.0 < curr)
        {
            printf("Invalid mod current %f\n", curr);
            return -1;
        }
    
        Imod = (uint32)(curr * 1000 / 2); /* convert mA to 2uA unit */
    
        ret = rtk_ldd_tx_mod_set(Imod);
        if(ret)
        {
            printf("Set TX Mod Fail!!! (%d)\n", ret);
            return -1;
        }

        return 0;

    }
    else
    {
        printf("%s <tx mod> \n", argv[0]);
        printf("The unit is mA \n");        
        return -1;
    }
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }
    
    return 0;    

}

static int europa_cli_cal_qreg_x1_y1_x2_y2_x3_y3(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 x1, x2, x3, y1, y2, y3;
    double a, b, c;


    if(argc >= 7)
    { 
        x1 = _vlaue_translate(argv[1]);
        x2 = _vlaue_translate(argv[2]);
        x3 = _vlaue_translate(argv[3]);
        y1 = _vlaue_translate(argv[4]);
        y2 = _vlaue_translate(argv[5]);
        y3 = _vlaue_translate(argv[6]);

        _europa_cal_quad_regression(x1, y1, x2, y2, x3, y3, &a, &b, &c);
        
        printf("---- regression test ----\n");
        printf("    parameter a = %lf\n", a);
        printf("    parameter b = %lf\n", b);
        printf("    parameter c = %lf\n", c);
    }
    else
    {
        printf("%s <UINT:x1> <UINT:y1> <UINT:x2> <UINT:y2> <UINT:x3> <UINT:y3> \n", argv[0]);   
        return -1;
    }

    return 0;    

}

static int europa_cli_cal_vbr(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret = 0;
    uint32 rfb1, rfb2, vstart, vend;
    
    uint16 tempK;
    int32 tempC;

    uint32 parm;

    uint32 rssi_v0, i_rssi, v_rssi;
    uint32 loopcnt, i, cnt1, cnt2;
    uint64 sum_i, sum_v, sum_w;

    uint32 init_dac, max_dac;
    uint32 orig_dac, dac;

    uint32 start_v, end_v, Vbr;
    uint32 w83_data;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        ret = rtl8291_cli_cal_vbr(devId, argc, argv, fp);
        return ret;
    }    
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if(argc >= 5)
    { 
        /* === get parameter */    
        rfb1   = _vlaue_translate(argv[1]);
        rfb2   = _vlaue_translate(argv[2]);
        vstart = _vlaue_translate(argv[3]);
        vend   = _vlaue_translate(argv[4]);

        start_v = vstart * 1000;  /* V to mV */
        end_v = vend * 1000;
        
        _europa_cal_flash_data_get(RTL8290B_RSSI_V0_ADDR, 4, &rssi_v0);

        if (0==rssi_v0)
        {
            printf("Invalid RSSI-V0, run get rssi-v0 first!\n");
            return -1;
        }

        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);    
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);

        osal_time_mdelay(500);
        
        // get orig A4/100, restore the value after Vbr test
        //rtk_ldd_parameter_get(1, EUROPA_REG_DAC_HB_ADDR, &orig_dac);
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 100, &orig_dac);        
        
        /* === detect Vbr */
        init_dac = _europa_cal_booster_Vout2Dac(start_v, rfb1, rfb2);
        max_dac = _europa_cal_booster_Vout2Dac(end_v, rfb1, rfb2);
        
        loopcnt = 1;
        i_rssi = 0;
        dac = init_dac;
        
        printf("Detecting Vbr from %dmV to %dmV...\n", start_v, end_v);
        
        
        while((i_rssi<RSSI_CURR_BREAKDOWN_TH)&&(dac<=max_dac))
        {
            sum_i = 0;
            cnt1 = loopcnt;
            cnt2 = 0;
        
            //rtk_ldd_parameter_set(1, EUROPA_REG_DAC_HB_ADDR, &dac);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 100, dac);    
            
            while ((cnt1>0) &&(cnt2<loopcnt))
            {
                ret = rtk_ldd_rx_power_get(rssi_v0, &v_rssi, &i_rssi);
                if(ret)
                {
                    printf("Get RX power fail!!! (%d)\n", ret);
                    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);                    
                    return -1;
                }
        
                if (v_rssi>rssi_v0)
                {
                    cnt1--;
                    sum_i += i_rssi;
                }
                else
                {
                    cnt2++;
                }
            }
        
            //diag_util_printf("Break Down Test: APD value = 0x%x, cnt1 = %d, cnt2    = %d\n", dac, cnt1, cnt2);
            if (cnt2>=loopcnt)
            {
                dac++;
            }
            else if (cnt1==0)
            {
                i_rssi = sum_i/loopcnt;
                printf("     DAC=0x%02x, I_RSSI=%d\n", dac, i_rssi);
                if (i_rssi >RSSI_CURR_BREAKDOWN_TH)
                {
                    break;
                }
                else
                {
                    dac++;
                }
            }
            else
            {
                printf("Break Down Test Error!!!!\n");
                break;
            }
        }

        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
        
        printf("restore orig DAC=0x%x\n", orig_dac);
        //rtk_ldd_parameter_set(1, EUROPA_REG_DAC_HB_ADDR, &orig_dac);
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 100, orig_dac);    
        
        printf("\nTest Result: DAC=0x%x, I_RSSI=%d\n", dac, i_rssi);
        
        /* === calculate Vout value */
        if (RSSI_CURR_BREAKDOWN_TH > i_rssi)
        {
             printf("!!! FAILE TO DETECT BREAKDOWN VOLTAGE !!!\n");
        
             Vbr = 0;
        }
        else
        {
             Vbr = _europa_cal_booster_Dac2Vout(dac, rfb1, rfb2);
        }
        
        
        /* === get temperature */
        tempK = 0;
        
        ret = rtk_ldd_temperature_get(&tempK);
        if(ret)
        {
            printf("Get Temperature Fail!!! (%d)\n", ret);
            return -1;
        }
        
        tempC = (tempK*100-27315)/100; /* degK to degC */
            //printf("Temperature=%dK (%dC)\n", tempK, tempC);
            
        printf("\nVbr=%d (mV) @ T=%dC (DAC=0x%02x)\n", Vbr, tempC, dac);

    }
    else
    {
        printf("%s <UINT:Rfb1> <UINT:Rfb2> <UINT:Vstart> <UINT:Vend> \n", argv[0]);   
        return -1;
    }

    return 0;    

}

static int europa_cli_cal_rxlos(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 i,data1, data2, los_flag, flag2;
    uint32 w39_data, w49_data, w38_data;
    uint32 addr, value, value2;
    uint32 w83_data;
    uint16 data;
    uint32 devId = 5;    
    int chip_mode;    
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 

    gettimeofday(&tv_tmp1,&tz); 
#endif   

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {     
        ret = rtl8290c_cli_cal_rxlos(devId, argc, argv, fp);
        return ret;
    } 
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {     
        ret = rtl8291_cli_cal_rxlos(devId, argc, argv, fp);
        return ret;
    }     
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {

    if(argc < 2)
    { 
        addr = RTL8290B_RXLOS_REF_DAC_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_get(addr, 1, &value);  
        addr = RTL8290B_RXLOS_RANGE_SEL_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_get(addr, 1, &value2);          
        printf("los = 0x%x, range = 0x%x\n", value, value2);
        rtk_ldd_rxlosRefDac_get(&data);
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 38, &w38_data);    
        printf("current los = 0x%x, range = 0x%x\n", data, ((w38_data&0x8)>>3));    
    }
    else if (argc >= 2)
    {
        if ((ret=strcmp(argv[1], "auto")) == 0 )
        {
            printf("Please plug the proper LOS optical power to ONU...\n");
            //printf("Wait 3 sec...\n");    
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);    
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);

            //osal_time_mdelay(3000);
    
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 49, &w49_data);
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 39, &w39_data);
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 38, &w38_data);    
            
            data1 = 0x10;
            w39_data = (w39_data&0x80)|data1;
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 39, w39_data);

            data = (w49_data&0xFC)|0x3;
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 49, data);

            w38_data = w38_data&0xF7;
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 38, w38_data);            
            
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 49, &value);
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &value2);            
            //printf("Start: W49 = 0x%x, W83 = 0x%x\n", value, value2);            
            
            los_flag = 0;
            //printf("Start: data1 = 0x%x, los_flag = 0x%x\n", data1, los_flag);
            while ((data1<0x7F)&&(los_flag==0))
            {
                flag2 = 0; 
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 39, &w39_data);            
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 38, &w38_data);                
                for (i=0; i<100; i++)
                {
                    europa_i2c_read(EUROPA_I2C_PORT, 0x55, 131, &data2);
                    //printf("i = %d, Get A5/131 = 0x%x\n", i, data2);
                    if (data2&0x4)
                    {
                        //printf("A5/131[2] == 1\n");
                        flag2 = 1;
                        break;
                    }    
                }
                //printf("W38 = 0x%x, W39 = 0x%x, range = 0x%x, Status2 = 0x%x\n", w38_data, w39_data, ((w38_data&0x8)>>3), data2);                        
                if (flag2 != 0)
                {
                    data1++;
                    w39_data = (w39_data&0x80)|data1;
                    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 39, w39_data);
                }
                else
                {
                    los_flag = 1;
                    //printf("LOS Flag = 1, W38 = 0x%x, W39 = 0x%x\n", w38_data, w39_data);
                }  

                if (data1>=0x7F)
                {
                    w38_data = 0;
                    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 38, &w38_data);
                    if ((w38_data&0x08)==0)
                    {
                        w38_data = w38_data|0x8;
                        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 38, w38_data);
    
                        data1=0;
                        w39_data = (w39_data&0x80)|data1;
                        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 39, w39_data);                        
                    }
                    else
                    {                
                        data1++;
                        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 49, w49_data);
                        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);                        
                        printf("RX LOS Fail, data = 0x%x, W38 = 0x%x\n", data1, w38_data);
                        return 0;
                    }
                }   
            }

            addr = RTL8290B_RXLOS_REF_DAC_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;     
            _europa_cal_flash_data_set(addr, 1, data1);

            addr = RTL8290B_RXLOS_RANGE_SEL_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;       
            _europa_cal_flash_data_set(addr, 1, ((w38_data&0x8)>>3));
            
            printf("RX LOS Finish. range = 0x%x, data1 = 0x%x\n",((w38_data&0x8)>>3), data1);

            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 49, w49_data);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);

            //europa_i2c_read(EUROPA_I2C_PORT, 0x54, 49, &value);
            //europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &value2);            
            //printf("Recover: W49 = 0x%x, W83 = 0x%x\n", value, value2);


        }
        else if ((ret=strcmp(argv[1], "range")) == 0 )
        {
            if(argc == 2)
            { 
                addr = RTL8290B_RXLOS_REF_DAC_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;     
                _europa_cal_flash_data_get(addr, 1, &value);  
                addr = RTL8290B_RXLOS_RANGE_SEL_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
                _europa_cal_flash_data_get(addr, 1, &value2);           
                printf("los = 0x%x, range = 0x%x\n", value, value2);
                rtk_ldd_rxlosRefDac_get(&data);
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 38, &w38_data);      
                printf("current los = 0x%x, range = 0x%x\n", data, ((w38_data&0x8)>>3));    
                return 0; 
            }

            value = _vlaue_translate(argv[2]); 
            if(value>1)
            {
                printf("%s range <value: 1 or 0> \n", argv[0]);       
                return -1;
            }
            addr = RTL8290B_RXLOS_RANGE_SEL_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;     
            _europa_cal_flash_data_set(addr, 1, value);
            w38_data = 0;
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 38, &w38_data);
            w38_data = (w38_data&0xF7)|((value&0x1)<<3);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 38, w38_data);    
        }        
        else
        {  
            value = _vlaue_translate(argv[1]);   
            if(value>0x7F)
            {
                printf("%s <value: less than 0x7F> \n", argv[0]);       
                return -1;
            }            
            addr = RTL8290B_RXLOS_REF_DAC_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
            _europa_cal_flash_data_set(addr, 1, value);
            printf("Set los = 0x%x\n", value);     
            rtk_ldd_rxlosRefDac_set((uint16)(value&0x7F));        
        }
    }
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }
    
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_rxlos Duration = %f ms\n", Duration);
#endif
    return 0;    

}

static int europa_cli_cal_rxsd(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 i,data1, data2, los_flag, flag2;
    uint32 w39_data, w49_data, w38_data;
    uint32 addr, value, value2;
    uint32 w83_data;
    uint16 data;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if(argc < 2)
    { 
        addr = RTL8290B_RXLOS_REF_DAC_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_get(addr, 1, &value);  
        addr = RTL8290B_RXLOS_RANGE_SEL_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_get(addr, 1, &value2);          
        printf("los = 0x%x, range = 0x%x\n", value, value2);
        rtk_ldd_rxlosRefDac_get(&data);
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 38, &w38_data);    
        printf("current los = 0x%x, range = 0x%x\n", data, ((w38_data&0x8)>>3));    
    }
    else if (argc >= 2)
    {
        if ((ret=strcmp(argv[1], "auto")) == 0 )
        {
            printf("Please plug the proper LOS De-Assert optical power to ONU...\n");
            //printf("Wait 3 sec...\n");    
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);    
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);
            
            //osal_time_mdelay(3000);
            
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 49, &w49_data);
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 39, &w39_data);
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 38, &w38_data);    
            
            data1 = 0x10;
            w39_data = (w39_data&0x80)|data1;
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 39, w39_data);
            
            data = (w49_data&0xFC)|0x2;
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 49, data);

            w38_data = w38_data&0xF7;
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 38, w38_data);       

            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 49, &value);
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &value2);            
            printf("Start: W49 = 0x%x, W83 = 0x%x\n", value, value2);            
            
            los_flag = 0;
            //printf("Start: data1 = 0x%x, los_flag = 0x%x\n", data1, los_flag);
            while ((data1<0x7F)&&(los_flag==0))
            {
                flag2 = 0; 
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 39, &w39_data);  
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 38, &w38_data);                
                for (i=0; i<100; i++)
                {
                    europa_i2c_read(EUROPA_I2C_PORT, 0x55, 131, &data2);
                    //printf("i = %d, Get A5/131 = 0x%x\n", i, data2);
                    if (data2&0x4)
                    {
                          //printf("A5/131[2] == 1\n");
                        flag2 = 1;
                        break;
                    }     
                }
                //printf("W38 = 0x%x, W39 = 0x%x, range = 0x%x, Status2 = 0x%x\n", w38_data, w39_data, ((w38_data&0x8)>>3), data2);                        
                if (flag2 != 0)
                {
                    data1++;
                    w39_data = (w39_data&0x80)|data1;
                    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 39, w39_data);
                }
                else
                {
                    los_flag = 1;
                    //printf("LOS Flag = 1, W38 = 0x%x, W39 = 0x%x\n", w38_data, w39_data);
                }  
            
                if (data1>=0x7F)
                {
                    w38_data = 0;
                    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 38, &w38_data);
                    if ((w38_data&0x08)==0)
                    {
                        w38_data = w38_data|0x8;
                        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 38, w38_data);
    
                        data1=0;
                        w39_data = (w39_data&0x80)|data1;
                        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 39, w39_data);                        
                    }
                    else
                    {                
                        data1++;
                        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 49, w49_data);
                        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);                        
                        printf("RX LOS Fail, data = 0x%x, W38 = 0x%x\n", data1, w38_data);
                        return 0;
                    }
                }     
            }
            
            addr = RTL8290B_RXLOS_REF_DAC_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;      
            _europa_cal_flash_data_set(addr, 1, data1);

            addr = RTL8290B_RXLOS_RANGE_SEL_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
            _europa_cal_flash_data_set(addr, 1, ((w38_data&0x8)>>3));

            printf("RX LOS Finish. range = 0x%x, data1 = 0x%x\n",((w38_data&0x8)>>3), data1);
            
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 49, w49_data);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);

            //europa_i2c_read(EUROPA_I2C_PORT, 0x54, 49, &value);
            //europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &value2);            
            //printf("Recover: W49 = 0x%x, W83 = 0x%x\n", value, value2);


        }
        else if ((ret=strcmp(argv[1], "range")) == 0 )
        {
            if(argc == 2)
            { 
                addr = RTL8290B_RXLOS_REF_DAC_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;     
                _europa_cal_flash_data_get(addr, 1, &value);  
                addr = RTL8290B_RXLOS_RANGE_SEL_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
                _europa_cal_flash_data_get(addr, 1, &value2);           
                printf("los = 0x%x, range = 0x%x\n", value, value2);
                rtk_ldd_rxlosRefDac_get(&data);
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 38, &w38_data);      
                printf("current los = 0x%x, range = 0x%x\n", data, ((w38_data&0x8)>>3));    
                return 0; 
            }

            value = _vlaue_translate(argv[2]); 
            if(value>1)
            {
                printf("%s range <value: 1 or 0> \n", argv[0]);       
                return -1;
            }
            addr = RTL8290B_RXLOS_RANGE_SEL_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;     
            _europa_cal_flash_data_set(addr, 1, value);
            w38_data = 0;
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 38, &w38_data);
            w38_data = (w38_data&0xF7)|((value&0x1)<<3);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 38, w38_data);    
        }        
        else
        {  
            value = _vlaue_translate(argv[1]);   
            if(value>0x7F)
            {
                printf("%s <value: less than 0x7F> \n", argv[0]);       
                return -1;
            }            
            addr = RTL8290B_RXLOS_REF_DAC_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
            _europa_cal_flash_data_set(addr, 1, value);
            printf("Set los = 0x%x\n", value);     
            rtk_ldd_rxlosRefDac_set((uint16)(value&0x7F));        
        }
    }
    
    return 0;    

}

static int europa_cli_cal_autopwr(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    double measured_pwr, target_pwr;
    double measured_uW, target_uW;
    double current_iavg, target_iavg, w58_iavg, w58_target;
    uint32 data_w58,addr;
    uint32 devId = 5;
    int chip_mode;    
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif   

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if(argc < 3)
    {
        printf("%s <measured pwr> <target pwr> \n", argv[0]);      
        return -1;
    }
 
    measured_pwr = atof(argv[1]);    
    target_pwr = atof(argv[2]); 

    printf("measured pwr = %lf dBm,  target pwr = %lf dBm  \n", measured_pwr, target_pwr);         
    
    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 58, &data_w58); 
    printf("W58 = 0x%x \n", data_w58);      
    w58_iavg = data_w58/32;
    current_iavg = 25*pow(2,w58_iavg);
    printf("current_iavg = %lf \n", current_iavg);      
    measured_uW = _europa_dBm2uw(measured_pwr);
    target_uW = _europa_dBm2uw(target_pwr);    
    printf("measured pwr = %lf uW,  target pwr = %lf uW  \n", measured_uW, target_uW); 
    target_iavg = current_iavg*(target_uW/measured_uW);
    printf("target_iavg = %lf \n", target_iavg);  
    w58_target = 32*log10(target_iavg/25)/log10(2);

    data_w58 = (uint32)w58_target;

    printf("taget W58 = 0x%x\n", data_w58); 

    if (data_w58 > 0x20)
    {
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 58, data_w58); 
        addr = RTL8290B_APC_IAVG_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, data_w58);
        printf("Set iavg power = 0x%x\n", data_w58);     
    }
    else
    {
        printf("Taget value should be 0x20~0x7F\n"); 
    }
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_autopwr Duration = %f ms\n", Duration);
#endif
    
    return 0;    

}

static int europa_cli_cal_rxtoffset(
    int argc,
    char *argv[],
    FILE *fp)
{
    int32 ret;
    int32 offset, addr;
    uint32 temp;
    uint32 devId = 5;
    int chip_mode;
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif   

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if (argc <= 1)
    {

        addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_TEMP0_OFFSET_ADDR;    
        _europa_cal_flash_data_get(addr, 2, &temp);            
        printf("RX T = %d\n", (int16)temp);    

        addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_TEMP0_OFF_OFFSET_ADDR;    
        _europa_cal_flash_data_get(addr, 2, &temp);            
        printf("RX T-OFFSET = %d\n", (int16)temp);    

        return 0;
    }    
    
    offset = _vlaue_translate(argv[1]);  

    printf("Set RX T-OFFSET = %d\n",  offset);

    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_TEMP0_OFF_OFFSET_ADDR;    
    _europa_cal_flash_data_set(addr, 2, (int16)offset);
    
    _europa_cal_flash_data_get(addr, 2, &temp);

    printf("temp = %d, temp = %d\n", temp, (int16)temp);    
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_rxtoffset Duration = %f ms\n", Duration);
#endif

    return 0;    
}

#if 0
static int europa_cli_cal_erc_comp(
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value, value2, step;
    int  cal_value;      

    if(argc >= 2)
    { 
        if(0 == strcmp(argv[1], "show"))
        {
            addr = RTL8290B_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
            _europa_cal_flash_data_get(addr, 1, &value); 
            printf("ERC comp enable = 0x%x\n", value);
        }
        else if(0 == strcmp(argv[1], "disable"))
        {        
            value = 0;     
            addr = RTL8290B_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
            _europa_cal_flash_data_set(addr, 1, value);

            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 67, &value);        
            value = value|0x80;      
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 67, value);    
            
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 124, &value2);    
            value = value2&0xF7;
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 124, value);    
            value = value2|0x08;
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 124, value);
            
            printf("Set ERC comp disable\n");  
        }
#if 0        
        else if(0 == strcmp(argv[1], "test"))
        {        
            value = _vlaue_translate(argv[2]);
            value2 = _vlaue_translate(argv[3]);

        
            printf("value = 0x%x (%d) value2 = 0x%x (%d) \n", value, value, value2, value2);     
            
            printf("Do ERC ...\n");       
            if (value2 > 0x7F)
            {
                printf("Value2 over flow!\n", value2); 
            }
                
            cal_value = (int)(value2&0x7F);        
            
            if ((value&0x01)==1)
            {
                cal_value = 0 - cal_value;
            } 
            
            printf("Before: cal_value = %d \n", cal_value); 
            
            cal_value = cal_value - 15*3;

            printf("after: cal_value = %d \n", cal_value); 

            if (cal_value<=-127)
            {
                cal_value = -127;
            }
            
            value = abs(cal_value);
            if (cal_value > 0)
            {
                value = 0x80 | value;
            }

            printf("after: W69 = 0x%x \n", value);             
            
            printf("Test ERC comp end\n");  
        }        
#endif        
        else
        {
            step = _vlaue_translate(argv[1]);

            if ((step<=0)||(step>3))
            {
                printf("%s <show/value/disable>\n", argv[0]);
                printf("<value> should be 1~3\n");                
                return -1;
            }
        
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 67, &value);        
            value = value|0x80;      
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 67, value);    
            
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 124, &value2);    
            value = value2&0xF7;
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 124, value);    
            value = value2|0x08;
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 124, value);
            osal_time_mdelay(10);
        
            addr = RTL8290B_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
            _europa_cal_flash_data_set(addr, 1, step);

            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 77, 0xD0);         
            osal_time_mdelay(10);
            europa_i2c_read(EUROPA_I2C_PORT, 0x55, 29, &value);                

            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 77, 0xC0);                 
            osal_time_mdelay(10);
            europa_i2c_read(EUROPA_I2C_PORT, 0x55, 29, &value2); 
            
            //printf("value = 0x%x (%d) value2 = 0x%x (%d) \n", value, value, value2, value2);     
            
            printf("Do ERC ...\n");       
            if (value2 > 0x7F)
            {
                printf("Value2 over flow!\n", value2); 
            }
                
            cal_value = (int)(value2&0x7F);        
            
            if ((value&0x01)==0)
            {
                cal_value = 0 - cal_value;
            } 
            
            //printf("Before: cal_value = %d \n", cal_value); 
            
            cal_value = cal_value - 15*step;

            //printf("after: cal_value = %d \n", cal_value); 

            if (cal_value<=-127)
            {
                cal_value = -127;
            }
            
            value = abs(cal_value);
            if (cal_value > 0)
            {
                value = 0x80 | value;
            }

            //printf("after: W69 = 0x%x \n", value); 

            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 67, 0x82);    
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 69, value);    
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 67, 0xA2);    

            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 69, &value); 
            //printf("Read W69 = 0x%x \n", value);             
            
            printf("Set ERC comp enable\n");             
        }  
       
    }
    else
    { 

        printf("%s <show/value/disable>\n", argv[0]);
        printf("<value> should be 1~3\n");                
        return -1; 
    }
    
    return 0; 
}
#endif

static int europa_cli_cal_erc_comp(
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value, value2;
    int  cal_value, step;
    uint32 devId = 5;
    int chip_mode;
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif   

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if((argc < 1)||(argc >=3))
    { 

        printf("%s <value>\n", argv[0]);
        printf("<value> should be 0~3. 0 for disable.\n");                
        return -1; 
    }
    else
    { 
        if(argc == 1)
        {
            addr = RTL8290B_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
            _europa_cal_flash_data_get(addr, 1, &value); 
            printf("ERC comp enable = 0x%x\n", value);
        }
        else 
        { 
            step = _vlaue_translate(argv[1]);
            
            if ((step<0)||(step>3))
            {
                printf("%s <value>\n", argv[0]);
                printf("<value> should be 0~3. 0 for disable.\n");
                return -1;
            }
            

            if (step ==0)
            {        
                value = 0;     
                addr = RTL8290B_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
                _europa_cal_flash_data_set(addr, 1, value);
    
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 67, &value);        
                value = value|0x80;      
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 67, value);    
                
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 124, &value2);    
                value = value2&0xF7;
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 124, value);    
                value = value2|0x08;
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 124, value);
                
                printf("Set ERC comp disable\n");  
            }        
            else
            {
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 67, &value);        
                value = value|0x80;      
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 67, value);    
                
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 124, &value2);    
                value = value2&0xF7;
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 124, value);    
                value = value2|0x08;
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 124, value);
                osal_time_mdelay(10);
            
                addr = RTL8290B_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
                _europa_cal_flash_data_set(addr, 1, step);
    
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 77, 0xD0);         
                osal_time_mdelay(10);
                europa_i2c_read(EUROPA_I2C_PORT, 0x55, 29, &value);                
    
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 77, 0xC0);                 
                osal_time_mdelay(10);
                europa_i2c_read(EUROPA_I2C_PORT, 0x55, 29, &value2); 
                
                //printf("value = 0x%x (%d) value2 = 0x%x (%d) \n", value, value, value2, value2);     
                
                printf("Do ERC ...\n");       
                if (value2 > 0x7F)
                {
                    printf("Value2 = 0x%x overflow!\n", value2); 
                }
                    
                cal_value = (int)(value2&0x7F);        
                
                if ((value&0x01)==0)
                {
                    cal_value = 0 - cal_value;
                } 
                
                //printf("Before: cal_value = %d \n", cal_value); 
                
                cal_value = cal_value - 15*step;
    
                //printf("after: cal_value = %d \n", cal_value); 
    
                if (cal_value<=-127)
                {
                    cal_value = -127;
                }
                
                value = abs(cal_value);
                if (cal_value > 0)
                {
                    value = 0x80 | value;
                }
    
                //printf("after: W69 = 0x%x \n", value); 
    
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 67, 0x82);    
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 69, value);    
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 67, 0xA2);    
    
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 69, &value); 
                //printf("Read W69 = 0x%x \n", value);             
                
                printf("Set ERC comp enable\n");             
            } 
        }     
    }
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_erc_comp Duration = %f ms\n", Duration);
#endif
    
    return 0; 
}

static int europa_cli_cal_txes(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 addr, value; 
    uint32 devId = 5;    
    int chip_mode;    
#ifdef RTL8290B_CAL_TIME   
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8291_MODE == chip_mode)
    { 
        ret = rtl8291_cli_cal_txes(devId, argc, argv, fp);
        return ret;
    }   
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }
    
#ifdef RTL8290B_CAL_TIME    
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]: europa_cli_cal_txes Duration = %f ms\n", Duration);
#endif
    return 0; 
}

static int europa_cli_cal_prbs(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 chipId, rev, subType;
    uint32 addr, value, value2, value3, ib, im;
    uint32 devId = 5;    
    int chip_mode;
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif   

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {     
        ret = rtl8290c_cli_cal_prbs(devId, argc, argv, fp);
    }
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
    if((argc < 2)||(argc > 3))
    { 
        //system("diag gpon get ");
        printf("%s <7/15/23/31/off>\n", argv[0]);
        return -1; 
    }

    if((ret = rtk_switch_version_get(&chipId, &rev, &subType)) != 0)
    {
        return ret;
    }

    if(argc >= 2)
    { 
        if((ret = _europa_gpon_init()) != 0)
        {
            return ret;
        }
    
        if(0 == strcmp(argv[1], "off"))
        { 
#if 0    
            system("diag gpon set force-prbs off");   
#else
            if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
            {
                //EPON PRBS OFF
                system("diag register set 0x40a30 0"); 
                //GPON PRBS OFF
                system("diag register set 0x40098 0"); 
                //BEN OFF
                system("diag register set 0x360a0 0");
                /*force BEN=0*/                
                system("diag register set 0x40540 0");                     
            }
            else if (RTL9602C_CHIP_ID == chipId)
            {
                //EPON PRBS OFF           
                system("diag register set 0x22A30 0");
                //GPON PRBS OFF
                system("diag register set 0x22098 0"); 
                //BEN OFF
                system("diag register set 0x360a0 0");    
                /*force BEN=0*/                
                system("diag register set 0x22540 0"); 
            }
#if defined(CONFIG_LUNA_G3_SERIES)    
            //For Cortina ARM-based 9607DQ series
            else if (RTL8277C_CHIP_ID == chipId)
            {
                system("diag rt_ponmisc set force-prbs off");
            } 
#endif
            else
            {
                printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
            }
#endif
        
            _europa_ibim_recover();

        }
        else
        {

            _europa_ibim_low();
    
            if(0 == strcmp(argv[1], "7"))
            {
#if 0            
                system("diag gpon set force-prbs prbs7");
#else
                if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
                {
                    system("diag register set 0x40094 0x407c"); 
                    system("diag register set 0x40098 1"); 
                }
                else if (RTL9602C_CHIP_ID == chipId)
                {
                    system("diag register set 0x22094 0x407c");
                    system("diag register set 0x22098 0x1");    
                }
#if defined(CONFIG_LUNA_G3_SERIES)    
                //For Cortina ARM-based 9607DQ series
                else if (RTL8277C_CHIP_ID == chipId)
                {
                    system("diag rt_ponmisc set force-prbs prbs07");
                }
#endif                
                else
                {
                    printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
                }
#endif 
           }
            else if(0 == strcmp(argv[1], "15"))
            {        
#if 0            
                system("diag gpon set force-prbs prbs15");

#else
                if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
                {
                    system("diag register set 0x40094 0x408d"); 
                    system("diag register set 0x40098 1"); 
                }
                else if (RTL9602C_CHIP_ID == chipId)
                {
                    system("diag register set 0x22094 0x408d");
                    system("diag register set 0x22098 0x1"); 
                }
#if defined(CONFIG_LUNA_G3_SERIES)    
                //For Cortina ARM-based 9607DQ series
                else if (RTL8277C_CHIP_ID == chipId)
                {
                    system("diag rt_ponmisc set force-prbs prbs15");
                }
#endif
                else
                {
                    printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
                }
#endif

            }
            else if(0 == strcmp(argv[1], "23"))
            {         
#if 0            
                system("diag gpon set force-prbs prbs23");   
#else
                if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
                {
                    system("diag register set 0x40094 0x409e"); 
                    system("diag register set 0x40098 1"); 
                }
                else if (RTL9602C_CHIP_ID == chipId)
                {
                    system("diag register set 0x22094 0x409e");
                    system("diag register set 0x22098 0x1"); 
                }
#if defined(CONFIG_LUNA_G3_SERIES)    
                //For Cortina ARM-based 9607DQ series
                else if (RTL8277C_CHIP_ID == chipId)
                {
                    system("diag rt_ponmisc set force-prbs prbs23");
                }
#endif
                else
                {
                    printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
                }
#endif
            }
            else if(0 == strcmp(argv[1], "31"))
            {        
#if 0            
                system("diag gpon set force-prbs prbs31");   
#else
                if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
                {
                    system("diag register set 0x40094 0x40af"); 
                    system("diag register set 0x40098 1"); 
                }
                else if (RTL9602C_CHIP_ID == chipId)
                {
                    system("diag register set 0x22094 0x40af");
                    system("diag register set 0x22098 0x1");
                }
#if defined(CONFIG_LUNA_G3_SERIES)    
                //For Cortina ARM-based 9607DQ series
                else if (RTL8277C_CHIP_ID == chipId)
                {
                    system("diag rt_ponmisc set force-prbs prbs31");
                }
#endif
                else
                {
                    printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
                }
#endif            
            }
            else
            {
                printf("%s <7/15/23/31/off>\n", argv[0]);
                return -1; 
            }    
#if 0            
            /*force BEN=1*/
            if (RTL9607C_CHIP_ID == chipId)
            {
                system("diag reg set 0x40540 0x30");                    
            }
            else if (RTL9602C_CHIP_ID == chipId)
            {   
                system("diag reg set 0x022540 0x30");                     
            }
            else
            {
                printf("Unknow Chip ID, do nothing...\n");
            }    
#endif
        }

    }
    }
    else 
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;
    }    
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_prbs Duration = %f ms\n", Duration);
#endif
    
    return 0; 
}

static int europa_cli_cal_epon_er(
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value;  
    rtk_ponmac_mode_t pon_mode;    
    int32 ret;
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif   

    pon_mode=PONMAC_MODE_GPON;
    if((ret = rtk_ponmac_mode_get(&pon_mode)) != 0)
    {
        printf("europa_cli_cal_epon_er: get pon mode fail!\n");
        return ret;
    }

    printf("pon mode = %d \n", pon_mode);

#if 0
    if(pon_mode == PONMAC_MODE_GPON)
    {
        printf("Device is GPON mode! Please switch to EPON mode\n");
        return -1;
    }
    else if(pon_mode >= PONMAC_MODE_FIBER_1G)
    {
        printf("Device is Fiber mode (0x%x).\n", pon_mode);
    }
#endif

    //Check  Dual PON Mode. If 0, set Dual PON Mode to 1
    value = 0;       
    addr = RTL8290B_DUAL_PON_MODE_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
    _europa_cal_flash_data_get(addr, 1, &value);
    if (value == 0)
    {
        value = 1;        
        _europa_cal_flash_data_set(addr, 1, value);
        
        //Set original er/trim/cmpd to epon er/trim/cmpd.
        addr = RTL8290B_APC_ER_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_ER_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        
        addr = RTL8290B_APC_ER_TRIM_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_ER_TRIM_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        
        addr = RTL8290B_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);          

        value = 0;
        addr = RTL8290B_EPON_APC_LPF_BW_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;     
        _europa_cal_flash_data_set(addr, 1, value);

        addr = RTL8290B_APC_CROSS_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_CROSS_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;  
        _europa_cal_flash_data_set(addr, 1, value);

        addr = RTL8290B_APC_CROSS_STR_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_CROSS_STR_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;  
        _europa_cal_flash_data_set(addr, 1, value);        

        addr = RTL8290B_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);         
    }
    
    if(argc >= 2)
    {   
        value = _vlaue_translate(argv[1]); 
        if (value>0xF)
        {
            printf("Input should be 0~0xF\n");       
            return -1;
        }
        addr = RTL8290B_EPON_APC_ER_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        printf("Set EPON er = 0x%x\n", value);   
        rtk_ldd_apcEr_set((uint16)value);        
        _europa_epon_ibim_set();            
    }
    else
    { 
        addr = RTL8290B_EPON_APC_ER_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_get(addr, 1, &value);  
        printf("EPON er = 0x%x\n", value);

    }
 #ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_epon_er Duration = %f ms\n", Duration);
#endif
    
    return 0; 
}

static int europa_cli_cal_epon_trim(
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value;
    rtk_ponmac_mode_t pon_mode;    
    int32 ret;
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    pon_mode=PONMAC_MODE_GPON;
    if((ret = rtk_ponmac_mode_get(&pon_mode)) != 0)
    {
        printf("europa_cli_cal_epon_trim: get pon mode fail!\n");
        return ret;
    }

    printf("pon mode = %d \n", pon_mode);

#if 0
    if(pon_mode == PONMAC_MODE_GPON)
    {
        printf("Device is GPON mode! Please switch to EPON mode\n");
        return -1;        
    }
    else if(pon_mode >= PONMAC_MODE_FIBER_1G)
    {
        printf("Device is Fiber mode (0x%x).\n", pon_mode);
    }
#endif
    
    //Check  Dual PON Mode. If 0, set Dual PON Mode to 1
    value = 0;       
    addr = RTL8290B_DUAL_PON_MODE_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
    _europa_cal_flash_data_get(addr, 1, &value);
    if (value == 0)
    {
        value = 1;        
        _europa_cal_flash_data_set(addr, 1, value);
        
        //Set original er/trim/cmpd to epon er/trim/cmpd.
        addr = RTL8290B_APC_ER_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_ER_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        
        addr = RTL8290B_APC_ER_TRIM_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_ER_TRIM_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
    
        addr = RTL8290B_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);          

        value = 0;
        addr = RTL8290B_EPON_APC_LPF_BW_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;     
        _europa_cal_flash_data_set(addr, 1, value);

        addr = RTL8290B_APC_CROSS_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_CROSS_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;  
        _europa_cal_flash_data_set(addr, 1, value);

        addr = RTL8290B_APC_CROSS_STR_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_CROSS_STR_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;  
        _europa_cal_flash_data_set(addr, 1, value);    

        addr = RTL8290B_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);         
    }
        
    if(argc >= 2)
    {       
        value = _vlaue_translate(argv[1]);   
        if (value>0x1F)
        {
            printf("Input should be 0~0x1F\n");       
            return -1;
        }
        addr = RTL8290B_EPON_APC_ER_TRIM_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        printf("Set EPON trim = 0x%x\n", value);      
        rtk_ldd_apcErTrim_set((uint16)value);                    
        _europa_epon_ibim_set();               
    }
    else
    { 
        addr = RTL8290B_EPON_APC_ER_TRIM_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_get(addr, 1, &value);  
        printf("EPON trim = 0x%x\n", value);
    }
 #ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_epon_trim Duration = %f ms\n", Duration);
#endif
    
    return 0; 
}

static int europa_cli_cal_epon_prbs(
    int argc,
    char *argv[],
    FILE *fp)
{
    int32   ret;
    uint32 chipId, rev, subType;
    uint32 addr, value, ib, im;   
    uint32 devId = 5;    
    int chip_mode;
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {     
        ret = rtl8290c_cli_cal_epon_prbs(devId, argc, argv, fp);
    }
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
    if((argc < 2)||(argc > 3))
    { 
        //system("diag gpon get ");
        printf("%s <7/7r/9/11/15/off/benon/benoff>\n", argv[0]);
        return -1; 
    }

    if((ret = rtk_switch_version_get(&chipId, &rev, &subType)) != 0)
    {
        return ret;
    }

    //system("diag debug pon mode set epon"); 
    if((ret = _europa_epon_init()) != 0)
    {
        return ret;
    }

    //Set EPON LPF to 40MHz
    value = 0;   
    addr = RTL8290B_EPON_APC_LPF_BW_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
    _europa_cal_flash_data_get(addr, 1, &value);
    if (value != 0)
    {
        value = 0;      
        _europa_cal_flash_data_set(addr, 1, value);
    }
    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 64, &value);    
    value = (value&0xFC);//Set EPON LPF to 40MHz
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 64, value);            
    printf("Set W64 = 0x%x\n", value); 
    
    //Set Dual PON Mode to 1
    //value = 0;     
    //addr = RTL8290B_DUAL_PON_MODE_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
    ///_europa_cal_flash_data_get(addr, 1, &value);
    //if (value == 0)
    //{
    //    value = 1;        
    //    _europa_cal_flash_data_set(addr, 1, value);
    //}

    if(argc >= 2)
    { 
        if(0 == strcmp(argv[1], "off"))
        {
            if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
            {
                //EPON PRBS OFF
                system("diag register set 0x40a30 0"); 
                //GPON PRBS OFF
                system("diag register set 0x40098 0");
                //BEN OFF
                system("diag register set 0x360a0 0");
                /*force BEN=0*/                
                system("diag register set 0x40540 0");                     
            }
            else if (RTL9602C_CHIP_ID == chipId)
            {
                //EPON PRBS OFF           
                system("diag register set 0x022A30 0");
                //GPON PRBS OFF
                system("diag register set 0x22098 0"); 
                //BEN OFF
                system("diag register set 0x360a0 0");                
                /*force BEN=0*/                
                system("diag register set 0x22540 0");                 
            }
#if defined(CONFIG_LUNA_G3_SERIES)    
            //For Cortina ARM-based 9607DQ series
            else if (RTL8277C_CHIP_ID == chipId)
            {
                system("diag rt_ponmisc set force-prbs off");
            }
#endif
            else
            {
                printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
            }

            _europa_ibim_recover();

        }
        else if(0 == strcmp(argv[1], "benon"))
        {  
            _europa_ibim_low();

            if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
            {
                system("diag register set 0x40540 0x30");
            }
            else if (RTL9602C_CHIP_ID == chipId)
            {
                system("diag register set 0x22540 0x30");
            }
#if defined(CONFIG_LUNA_G3_SERIES)    
            //For Cortina ARM-based 9607DQ series
            else if (RTL8277C_CHIP_ID == chipId)
            {
                system("rt_ponmisc set force-laser-on state enable");
            }
#endif
            else
            {
                printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
            }    
        }
        else if(0 == strcmp(argv[1], "benoff"))
        {  
            if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
            {
                //EPON PRBS OFF
                system("diag register set 0x40a30 0"); 
                //GPON PRBS OFF
                system("diag register set 0x40098 0");
                //BEN OFF
                system("diag register set 0x360a0 0");
                /*force BEN=0*/                
                system("diag register set 0x40540 0"); 
            }
            else if (RTL9602C_CHIP_ID == chipId)
            {
                //EPON PRBS OFF           
                system("diag register set 0x022A30 0");
                //GPON PRBS OFF
                system("diag register set 0x22098 0"); 
                //BEN OFF
                system("diag register set 0x360a0 0");
                /*force BEN=0*/                
                system("diag register set 0x22540 0");
            }
#if defined(CONFIG_LUNA_G3_SERIES)
            //For Cortina ARM-based 9607DQ series
            else if (RTL8277C_CHIP_ID == chipId)
            {
                system("rt_ponmisc set force-laser-on state disable");
            }
#endif
            else
            {
                printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
            }    
            
            _europa_ibim_recover(); 
            
        }    
        else
        {
            _europa_ibim_low();
    
            if(0 == strcmp(argv[1], "7"))
            {
                if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
                {
                    system("diag reg set 0x40a2C 0x0000");
                    system("diag reg set 0x40a30 0x803");                    
                }
                else if (RTL9602C_CHIP_ID == chipId)
                {
                    system("diag reg set 0x022A2C 0x0000");                 
                    system("diag reg set 0x022A30 0x803");                     
                }
#if defined(CONFIG_LUNA_G3_SERIES)
                //For Cortina ARM-based 9607DQ series
                else if (RTL8277C_CHIP_ID == chipId)
                {
                    system("diag rt_ponmisc set force-prbs prbs07");
                }
#endif
                else
                {
                    printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
                }            
            }
            else if(0 == strcmp(argv[1], "7r"))
            {
                if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
                {
                    system("diag reg set 0x40a2C 0x2000");                  
                    system("diag reg set 0x40a30 0x803");
                }
                else if (RTL9602C_CHIP_ID == chipId)
                {
                    system("diag reg set 0x022A2C 0x2000");                 
                    system("diag reg set 0x022A30 0x803");    
                }
#if defined(CONFIG_LUNA_G3_SERIES)
                //For Cortina ARM-based 9607DQ series
                else if (RTL8277C_CHIP_ID == chipId)
                {
                    system("diag rt_ponmisc set force-prbs prbs07");
                }
#endif
                else
                {
                    printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
                }            
            }
            else if(0 == strcmp(argv[1], "9"))
            {         
                if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
                {
                    system("diag reg set 0x40a2C 0x0100");                  
                    system("diag reg set 0x40a30 0x803");
                }
                else if (RTL9602C_CHIP_ID == chipId)
                {
                    system("diag reg set 0x022A2C 0x0100");
                    system("diag reg set 0x022A30 0x803");    
                }
#if defined(CONFIG_LUNA_G3_SERIES)
                //For Cortina ARM-based 9607DQ series
                else if (RTL8277C_CHIP_ID == chipId)
                {
                    system("diag rt_ponmisc set force-prbs prbs09");
                }
#endif
                else
                {
                    printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
                }            
            }
            else if(0 == strcmp(argv[1], "11"))
            {        
                if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
                {
                    system("diag reg set 0x40a2C 0x0200");                  
                    system("diag reg set 0x40a30 0x803"); 
                }
                else if (RTL9602C_CHIP_ID == chipId)
                {
                    system("diag reg set 0x022A2C 0x0200");                  
                    system("diag reg set 0x022A30 0x803");
                }
#if defined(CONFIG_LUNA_G3_SERIES)                //For Cortina ARM-based 9607DQ series
                else if (RTL8277C_CHIP_ID == chipId)
                {
                    system("diag rt_ponmisc set force-prbs prbs11");
                }
#endif
                else
                {
                    printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
                }                
            }
            else if(0 == strcmp(argv[1], "15"))
            {        
                if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
                {
                    system("diag reg set 0x40a2C 0x0300");  
                    system("diag reg set 0x40a30 0x803"); 
                }
                else if (RTL9602C_CHIP_ID == chipId)
                {
                    system("diag reg set 0x022A2C 0x0300");  
                    system("diag reg set 0x022A30 0x803");
                }
#if defined(CONFIG_LUNA_G3_SERIES)
                //For Cortina ARM-based 9607DQ series
                else if (RTL8277C_CHIP_ID == chipId)
                {
                    system("diag rt_ponmisc set force-prbs prbs15");
                }
#endif
                else
                {
                    printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
                }
            }
            else
            {
                printf("%s <7/9/11/15/off/benon/benoff>\n", argv[0]);
                return -1; 
            }

            /*force BEN=1*/
            if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
            {
                system("diag reg set 0x40540 0x30");                    
            }
            else if (RTL9602C_CHIP_ID == chipId)
            {   
                system("diag reg set 0x022540 0x30");                     
            }
#if defined(CONFIG_LUNA_G3_SERIES)
            //For Cortina ARM-based 9607DQ series
            else if (RTL8277C_CHIP_ID == chipId)
            {
                //do nothing
            }
#endif
            else
            {
                printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
            }            
        }

    }
    }
    else 
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;
    }

#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_epon_prbs Duration = %f ms\n", Duration);
#endif
    
    return 0; 
}

static int europa_cli_cal_epon_cmpd(
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value, value2;      
    rtk_ponmac_mode_t pon_mode;    
    int32 ret;
    uint32 bias, mod, cmpd;
    uint32 chipId, rev, subType;
    uint32 ib, im;
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    pon_mode=PONMAC_MODE_GPON;
    if((ret = rtk_ponmac_mode_get(&pon_mode)) != 0)
    {
        printf("europa_cli_cal_epon_cmpd: get pon mode fail!\n");
        return ret;
    }

    printf("pon mode = %d \n", pon_mode);
    
#if 0
    if(pon_mode == PONMAC_MODE_GPON)
    {
        printf("Device is GPON mode! Please switch to EPON mode\n");
        return -1;
    }
    else if(pon_mode >= PONMAC_MODE_FIBER_1G)
    {
        printf("Device is Fiber mode (0x%x).\n", pon_mode);
    }
#endif

    //Check  Dual PON Mode. If 0, set Dual PON Mode to 1
    value = 0;       
    addr = RTL8290B_DUAL_PON_MODE_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
    _europa_cal_flash_data_get(addr, 1, &value);
    if (value == 0)
    {
        value = 1;        
        _europa_cal_flash_data_set(addr, 1, value);
        
        //Set original er/trim/cmpd to epon er/trim/cmpd.
        addr = RTL8290B_APC_ER_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_ER_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        
        addr = RTL8290B_APC_ER_TRIM_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_ER_TRIM_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        
        addr = RTL8290B_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);          

        value = 0;
        addr = RTL8290B_EPON_APC_LPF_BW_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;     
        _europa_cal_flash_data_set(addr, 1, value);

        addr = RTL8290B_APC_CROSS_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_CROSS_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;  
        _europa_cal_flash_data_set(addr, 1, value);

        addr = RTL8290B_APC_CROSS_STR_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_CROSS_STR_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;  
        _europa_cal_flash_data_set(addr, 1, value);        

        addr = RTL8290B_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);         
    }
    
    if(argc >= 2)
    { 
        if((ret = rtk_switch_version_get(&chipId, &rev, &subType)) != 0)
        {
            return ret;
        }

        if(0 == strcmp(argv[1], "check"))
        {
               
            _europa_ibim_low();
               
            if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
            {
                system("diag reg set 0x40a30 0x000"); 
                system("diag register set 0x40540 0x30");                
            }
            else if (RTL9602C_CHIP_ID == chipId)
            {
                system("diag reg set 0x022A30 0x000"); 
                system("diag register set 0x22540 0x30");                 
            }
#if defined(CONFIG_LUNA_G3_SERIES)
            //For Cortina ARM-based 9607DQ series
            else if (RTL8277C_CHIP_ID == chipId)
            {
                system("rt_ponmisc set force-laser-on state enable");
            }
#endif
            else
            {
                printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
            }
               
            osal_time_mdelay(100);
               
            bias = 0;
            mod = 0;
            cmpd = 0;
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 60, &value);     
            cmpd = value&0x7;        
            while((bias>=mod)&&(cmpd !=0))
            {
                ret = rtk_ldd_tx_bias_get(&bias);
                if(ret)
                {
                    printf("Get TX Bias Fail!!! (%d)\n", ret);
                    return -1;
                }
                           
               
                ret = rtk_ldd_tx_mod_get(&mod);
                if(ret)
                {
                    printf("Get TX Mod Fail!!! (%d)\n", ret);
                    return -1;
                }
                   
                //printf("cmpd = 0x%x, ib = 0x%x, im = 0x%x\n", cmpd, bias, mod);
               
                if (bias>=mod)
                {
                    cmpd = cmpd-1;
                    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 60, &value);     
                    value = (value&0xF8)|cmpd;
                    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 60, value); 
                    osal_time_mdelay(150);                         
                }
            }
               
            addr = RTL8290B_EPON_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
            _europa_cal_flash_data_set(addr, 1, cmpd);             
            printf("cmpd = 0x%x, ib = 0x%x, im = 0x%x\n", cmpd, bias, mod);
           
           
            if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
            {
                //EPON PRBS OFF
                system("diag register set 0x40a30 0"); 
                //GPON PRBS OFF
                system("diag register set 0x40098 0"); 
                //BEN OFF
                system("diag register set 0x360a0 0");
                /*force BEN=0*/                
                system("diag register set 0x40540 0");
            }
            else if (RTL9602C_CHIP_ID == chipId)
            {
                //EPON PRBS OFF           
                system("diag register set 0x22A30 0");
                //GPON PRBS OFF
                system("diag register set 0x22098 0"); 
                //BEN OFF
                system("diag register set 0x360a0 0");    
                /*force BEN=0*/                
                system("diag register set 0x22540 0");
            }
#if defined(CONFIG_LUNA_G3_SERIES)
            //For Cortina ARM-based 9607DQ series
            else if (RTL8277C_CHIP_ID == chipId)
            {
                system("rt_ponmisc set force-laser-on state disable");
            }
#endif
            else
            {
                printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
            } 

            _europa_ibim_recover();  
        
        }
        else
        {
            value = _vlaue_translate(argv[1]); 
            if (value > 0x7)
            {
                printf("Value = 0x%x > 0x7\n", value);
                return -1; 
            }
        
            addr = RTL8290B_EPON_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
            _europa_cal_flash_data_set(addr, 1, value);
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 60, &value2);   
            value2 = (value2&0xF8)|value;
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 60, value2);        
            printf("Set EPON cmpd = 0x%x (0x%x)\n", value, value2);   
        }
    }
    else
    { 
        addr = RTL8290B_EPON_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_get(addr, 1, &value);  
        printf("EPON cmpd = 0x%x\n", value);
    }
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_epon_cmpd Duration = %f ms\n", Duration);
#endif
    
    return 0; 
}


static int europa_cli_cal_epon_lpf(
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value, value2;      
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }
    
    //Check  Dual PON Mode. If 0, set Dual PON Mode to 1
    value = 0;       
    addr = RTL8290B_DUAL_PON_MODE_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
    _europa_cal_flash_data_get(addr, 1, &value);
    if (value == 0)
    {
        value = 1;        
        _europa_cal_flash_data_set(addr, 1, value);
        
        //Set original er/trim/cmpd to epon er/trim/cmpd.
        addr = RTL8290B_APC_ER_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_ER_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        
        addr = RTL8290B_APC_ER_TRIM_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_ER_TRIM_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        
        addr = RTL8290B_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);  

        value = 0;
        addr = RTL8290B_EPON_APC_LPF_BW_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;     
        _europa_cal_flash_data_set(addr, 1, value);

        addr = RTL8290B_APC_CROSS_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_CROSS_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;  
        _europa_cal_flash_data_set(addr, 1, value);

        addr = RTL8290B_APC_CROSS_STR_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_CROSS_STR_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;  
        _europa_cal_flash_data_set(addr, 1, value);    

        addr = RTL8290B_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);         
    }

    if(argc >= 2)
    { 
        value = _vlaue_translate(argv[1]);   
        if (value > 0x3)
        {
            printf("Value = 0x%x > 0x3\n", value);
            return -1; 
        }
    
        addr = RTL8290B_EPON_APC_LPF_BW_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 64, &value2);    
        value2 = (value2&0xFC)|value;
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 64, value2);            
        printf("Set EPON LPF = 0x%x\n", value);                         
    }
    else
    { 
        addr = RTL8290B_EPON_APC_LPF_BW_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_get(addr, 1, &value);  
        printf("EPON LPF = 0x%x\n", value);
    }
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_epon_lpf Duration = %f ms\n", Duration);
#endif
    
    return 0; 
}

static int europa_cli_cal_epon_apc_cross(
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value, value2;      
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }
    
    //Check  Dual PON Mode. If 0, set Dual PON Mode to 1
    value = 0;       
    addr = RTL8290B_DUAL_PON_MODE_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
    _europa_cal_flash_data_get(addr, 1, &value);
    if (value == 0)
    {
        value = 1;        
        _europa_cal_flash_data_set(addr, 1, value);
        
        //Set original er/trim/cmpd to epon er/trim/cmpd.
        addr = RTL8290B_APC_ER_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_ER_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        
        addr = RTL8290B_APC_ER_TRIM_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_ER_TRIM_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        
        addr = RTL8290B_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);  

        value = 0;
        addr = RTL8290B_EPON_APC_LPF_BW_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;     
        _europa_cal_flash_data_set(addr, 1, value);

        addr = RTL8290B_APC_CROSS_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_CROSS_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;  
        _europa_cal_flash_data_set(addr, 1, value);

        addr = RTL8290B_APC_CROSS_STR_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_CROSS_STR_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;  
        _europa_cal_flash_data_set(addr, 1, value);    

        addr = RTL8290B_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);         
    }
 
    if(argc == 1)
    { 
        //Flash APC Cross config
        addr = RTL8290B_EPON_APC_CROSS_EN_OFFSET_ADDR+ RTL8290B_PARAM_ADDR_BASE; 
        _europa_cal_flash_data_get(addr, 1, &value); 
        printf("EPON APC Cross enable = 0x%x\n", value);
        addr = RTL8290B_EPON_APC_CROSS_STR_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
        _europa_cal_flash_data_get(addr, 1, &value); 
        printf("EPON APC Cross str = 0x%x\n", value);


        //Current APC Cross config  
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x83, &value);       
        printf("Now APC Cross str = 0x%x\n", value);
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x84, &value2);
        if ((value2&0x80) == 0)
        {
             printf("Now APC Cross enable is disable.\n");
        }
        else
        {
             printf("Now APC Cross enable is enable.\n");    
        }
    } 
    else if(argc == 2)
    { 
        if(0 == strcmp(argv[1], "enable"))
        {        
            value = 1;     
            addr = RTL8290B_EPON_APC_CROSS_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
            _europa_cal_flash_data_set(addr, 1, value);
            printf("Set EPON APC Cross enable\n");

            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x84, &value2);
            value2 = (value2|0x80);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x84, value2);    
        }
        else if(0 == strcmp(argv[1], "disable"))
        {        
            value = 0;     
            addr = RTL8290B_EPON_APC_CROSS_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
            _europa_cal_flash_data_set(addr, 1, value);
            printf("Set EPON APC Cross disable\n");

            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x84, &value2);
            value2 = (value2&0x7F);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x84, value2);     
        }        
        else
        {
            value = _vlaue_translate(argv[1]);    
            if (value > 0xFF)
            {
                printf("EPON APC cross str = 0x%x. It should be less than 0xFF.\n", value);
                return -1;                  
            }
    
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x83, value);
            printf("EPON APC cross str = 0x%x\n", value);  
            addr = RTL8290B_EPON_APC_CROSS_STR_OFFSET_ADDR+ RTL8290B_PARAM_ADDR_BASE; 
            _europa_cal_flash_data_set(addr, 1, value);         
        }         
    }
    else
    { 

        printf("%s <value>/enable/disable\n", argv[0]);       
        return -1;
    }
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_epon_apc_cross Duration = %f ms\n", Duration);
#endif
    
    return 0; 
}

static int europa_cli_cal_epon_erc_comp(
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value, value2;
    int  cal_value;      
    rtk_ponmac_mode_t pon_mode;    
    int ret, step;
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    pon_mode=PONMAC_MODE_GPON;
    if((ret = rtk_ponmac_mode_get(&pon_mode)) != 0)
    {
        printf("europa_cli_cal_epon_erc_comp: get pon mode fail!\n");
        return ret;
    }

    printf("pon mode = %d \n", pon_mode);
        
#if 0
    if(pon_mode == PONMAC_MODE_GPON)
    {
        printf("Device is GPON mode! Please switch to EPON mode\n");
        return -1;
    }
    else if(pon_mode >= PONMAC_MODE_FIBER_1G)
    {
        printf("Device is Fiber mode (0x%x).\n", pon_mode);
    }
#endif


    //Check  Dual PON Mode. If 0, set Dual PON Mode to 1
    value = 0;       
    addr = RTL8290B_DUAL_PON_MODE_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
    _europa_cal_flash_data_get(addr, 1, &value);
    if (value == 0)
    {
        value = 1;        
        _europa_cal_flash_data_set(addr, 1, value);
        
        //Set original er/trim/cmpd to epon er/trim/cmpd.
        addr = RTL8290B_APC_ER_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_ER_SET_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        
        addr = RTL8290B_APC_ER_TRIM_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_ER_TRIM_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);
        
        addr = RTL8290B_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);  

        value = 0;
        addr = RTL8290B_EPON_APC_LPF_BW_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;     
        _europa_cal_flash_data_set(addr, 1, value);

        addr = RTL8290B_APC_CROSS_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_CROSS_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;  
        _europa_cal_flash_data_set(addr, 1, value);

        addr = RTL8290B_APC_CROSS_STR_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_APC_CROSS_STR_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;  
        _europa_cal_flash_data_set(addr, 1, value);    

        addr = RTL8290B_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 1, &value);
        addr = RTL8290B_EPON_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_set(addr, 1, value);         
    }

    if((argc < 1)||(argc >=3))
    { 

        printf("%s <value>\n", argv[0]);
        printf("<value> should be 0~3. 0 for disable.\n");                
        return -1; 
    }
    else
    { 
        if(argc == 1)
        {
            addr = RTL8290B_EPON_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
            _europa_cal_flash_data_get(addr, 1, &value); 
            printf("ERC comp enable = 0x%x\n", value);
        }
        else 
        { 
            step = _vlaue_translate(argv[1]);
            
            if ((step<0)||(step>3))
            {
                printf("%s <value>\n", argv[0]);
                printf("<value> should be 0~3. 0 for disable.\n");
                return -1;
            }
            

            if (step ==0)
            {        
                value = 0;     
                addr = RTL8290B_EPON_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
                _europa_cal_flash_data_set(addr, 1, value);
    
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 67, &value);        
                value = value|0x80;      
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 67, value);    
                
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 124, &value2);    
                value = value2&0xF7;
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 124, value);    
                value = value2|0x08;
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 124, value);

                printf("Set ERC comp disable\n");  
            }        
            else
            {
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 67, &value);        
                value = value|0x80;      
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 67, value);    
                
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 124, &value2);    
                value = value2&0xF7;
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 124, value);    
                value = value2|0x08;
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 124, value);
                osal_time_mdelay(10);
            
                addr = RTL8290B_EPON_ERC_COMP_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
                _europa_cal_flash_data_set(addr, 1, step);
    
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 77, 0xD0);         
                osal_time_mdelay(10);
                europa_i2c_read(EUROPA_I2C_PORT, 0x55, 29, &value);                
    
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 77, 0xC0);                 
                osal_time_mdelay(10);
                europa_i2c_read(EUROPA_I2C_PORT, 0x55, 29, &value2); 
                
                //printf("value = 0x%x (%d) value2 = 0x%x (%d) \n", value, value, value2, value2);     
                
                printf("Do ERC ...\n");       
                if (value2 > 0x7F)
                {
                    printf("Value2 = 0x%x overflow!\n", value2); 
                }
                    
                cal_value = (int)(value2&0x7F);        
                
                if ((value&0x01)==0)
                {
                    cal_value = 0 - cal_value;
                } 
                
                //printf("Before: cal_value = %d \n", cal_value); 
                
                cal_value = cal_value - 15*step;
    
                //printf("after: cal_value = %d \n", cal_value); 
    
                if (cal_value<=-127)
                {
                    cal_value = -127;
                }
                
                value = abs(cal_value);
                if (cal_value > 0)
                {
                    value = 0x80 | value;
                }
    
                //printf("after: W69 = 0x%x \n", value); 
    
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 67, 0x82);    
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 69, value);    
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 67, 0xA2);    
    
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 69, &value); 
                //printf("Read W69 = 0x%x \n", value);             
                
                printf("Set ERC comp enable\n");             
            } 
        }     
    }
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_epon_erc_comp Duration = %f ms\n", Duration);
#endif
    
    return 0; 
}


static int europa_cli_cal_ibias_max(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    double curr;
    uint32 Ibias;
    uint32 addr, value, value2, value3, ibmax, iben; 
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if(argc == 1)
    { 
        //Flash IBNAX config
        addr = RTL8290B_APC_BIAS_MAX_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
        _europa_cal_flash_data_get(addr, 1, &ibmax); 
        curr = (double)(ibmax<<4)*100/4096;
        printf("IBIAS_MAX = %f mA (0x%x)\n", curr, ibmax);
        addr = RTL8290B_APC_BIAS_MAX_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
        _europa_cal_flash_data_get(addr, 1, &iben); 
        if (iben == 0)
        {
             printf("IBIAS_MAX_EN is disable.\n");
        }
        else if (iben == 0x1)
        {
             printf("IBIAS_MAX_EN is enable.\n");    
        }
        else
        {
            printf("IBIAS_MAX_EN = 0x%x, error.\n", iben);    
        }

        //Current IBMAX config  
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x48, &value);
        curr = (double)(value<<4)*100/4096;        
        printf("Now IBIAS MAX = %f mA (0x%x)\n", curr, value);
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value2);
        if ((value2&0x10) == 0)
        {
             printf("Now IBIAS_MAX_EN is disable.\n");
        }
        else
        {
             printf("Now IBIAS_MAX_EN is enable.\n");    
        }
    }    

    if(argc == 2)
    { 
       if(0 == strcmp(argv[1], "enable"))
       {

            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x48, &ibmax);
            if (ibmax == 0)
            {
                printf("IBIAS_MAX = 0x%x. It should not be 0.\n", ibmax);
                return -1;
            }
            
           europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
           value = (value|0x10);
           europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);           
           
           addr = RTL8290B_APC_BIAS_MAX_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
           _europa_cal_flash_data_set(addr, 1, 0x01);        
            printf("IBIAS_MAX_EN is enable.\n");            
       }
       else if (0 == strcmp(argv[1], "disable"))
       {
           europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
           value = (value&0xEF);
           europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);
 
            addr = RTL8290B_APC_BIAS_MAX_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
            _europa_cal_flash_data_set(addr, 1, 0);        
            printf("IBIAS_MAX_EN is disable.\n");            
        }
        else if (0 == strcmp(argv[1], "clear"))
        {
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
            value = (value&0xEF);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);
 
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
            value = (value&0xFD);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);
            
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x48, 0);
            
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
            value = (value|0x02);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);
            
            addr = RTL8290B_APC_BIAS_MAX_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
             _europa_cal_flash_data_set(addr, 1, 0); 
            addr = RTL8290B_APC_BIAS_MAX_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
            _europa_cal_flash_data_set(addr, 1, 0);        
            printf("IBIAS_MAX_EN & IBIAS_MAX are clear.\n");                
       }
       else
       {
            curr = atof(argv[1]);    

            if ((100.0 < curr)||(6.25>curr))
            {
                printf("Invalid bias max %f. It should less than 100mA and more than 6.25mA\n", curr);
                return -1;
            }
            else if (100.0 == curr)
            {
                ibmax = 0xFF;
            }
            else
            {
                ibmax = (uint32)(curr * 4096 / 100); //unit is 100mA/4096
                ibmax = (ibmax&0xFF0)>>4;          
            }
            
             printf("Input current = %f mA, Ibmax = 0x%x\n", curr, ibmax);
 
             europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
             value = (value&0xFD);
             europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);
 
             europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x48, ibmax);
 
             europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
             value = (value|0x02);
             europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);
 
             addr = RTL8290B_APC_BIAS_MAX_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
             _europa_cal_flash_data_set(addr, 1, ibmax);        
        }
    }
    else
    {
        printf("%s <txbias max value>/enable/disable/clear \n", argv[0]);
        printf("The unit is mA \n");        
        return -1;
    }

    return 0;    

}

static int europa_cli_cal_imod_max(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    double curr;
    uint32 addr, value, value2, immax, imen; 
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if(argc == 1)
    { 
        //Flash IBNAX config
        addr = RTL8290B_APC_MOD_MAX_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
        _europa_cal_flash_data_get(addr, 1, &immax); 
        curr = (double)(immax<<4)*100/4096;
        printf("IMOD MAX = %f mA (0x%x)\n", curr, immax);
        addr = RTL8290B_APC_MOD_MAX_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
        _europa_cal_flash_data_get(addr, 1, &imen); 
        if (imen == 0)
        {
             printf("IMOD_EN is disable.\n");
        }
        else if (imen == 0x1)
        {
             printf("IMOD_EN is enable.\n");    
        }
        else
        {
            printf("IMOD_EN = 0x%x, error.\n", imen);    
        }

        //Current IBMAX config  
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x47, &value);
        curr = (double)(value<<4)*100/4096;        
        printf("Now IMOD MAX = %f mA (0x%x)\n", curr, value);
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value2);
        if ((value2&0x20) == 0)
        {
             printf("Now IMOD_EN is disable.\n");
        }
        else
        {
             printf("Now IMOD_EN is enable.\n");    
        }
    }    

    if(argc == 2)
    { 
        if(0 == strcmp(argv[1], "enable"))
        {

            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x47, &immax);
            if (immax == 0)
            {
                printf("IMOD MAX = 0x%x. It should not be 0.\n", immax);
                return -1;
            }
      
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
            value = (value|0x20);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);  
           
            addr = RTL8290B_APC_MOD_MAX_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
            _europa_cal_flash_data_set(addr, 1, 0x01);       
            printf("IMOD_MAX_EN is enable.\n");            
        }
        else if (0 == strcmp(argv[1], "disable"))
        {
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
            value = (value&0xDF);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);
 
            addr = RTL8290B_APC_MOD_MAX_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
            _europa_cal_flash_data_set(addr, 1, 0); 
            printf("IMOD_MAX_EN is disable.\n");            
        }
        else if (0 == strcmp(argv[1], "clear"))
        {
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
            value = (value&0xDF);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);
 
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
            value = (value&0xFB);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);
            
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x47, 0);
            
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
            value = (value|0x04);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);
            
            addr = RTL8290B_APC_MOD_MAX_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
             _europa_cal_flash_data_set(addr, 1, 0); 
            addr = RTL8290B_APC_MOD_MAX_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
            _europa_cal_flash_data_set(addr, 1, 0); 
            printf("MOD_MAX_EN & MOD_MAX are clear.\n");                
        }       
        else
        {
             curr = atof(argv[1]);    
 
             if ((100.0 < curr)||(6.25>curr))
             {
                 printf("Invalid mod max %f. It should less than 100mA and more than 6.25mA\n", curr);
                 return -1;
             }
             else if (100.0 == curr)
             {
                 immax = 0xFF;
             }
             else
             {
                 immax = (uint32)(curr * 4096 / 100); //unit is 100mA/4096
                 immax = (immax&0xFF0)>>4;          
             }
             
             printf("Input current = %f mA, Immax = 0x%x\n", curr, immax);
 
             europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
             value = (value&0xFB);
             europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);
 
             europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x47, immax);
 
             europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
             value = (value|0x04);
             europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);
 
             addr = RTL8290B_APC_MOD_MAX_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
             _europa_cal_flash_data_set(addr, 1, immax);         
         }
    }
    else
    {
        printf("%s <txmod max value>/enable/disable/clear \n", argv[0]);
        printf("The unit is mA \n");        
        return -1;
    }

    return 0;    

}


static int europa_cli_cal_ibias_min(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    double curr;
    uint32 addr, value, value2, ibmin, iben; 
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    //BIAS_MIN<9:2> in BIAS<11:0>, LSB=0.097mA, max=24mA
    
    if(argc == 1)
    { 
        //Flash IBNAX config
        addr = RTL8290B_APC_BIAS_MIN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
        _europa_cal_flash_data_get(addr, 1, &ibmin); 
        curr = (double)(ibmin<<2)*100/4096;
        printf("IBIAS MIN = %f mA (0x%x)\n", curr, ibmin);
        addr = RTL8290B_APC_BIAS_MIN_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
        _europa_cal_flash_data_get(addr, 1, &iben); 
        if (iben == 0)
        {
             printf("IBIAS_MIN_EN is disable.\n");
        }
        else if (iben == 0x1)
        {
             printf("IBIAS_MIN_EN is enable.\n");    
        }
        else
        {
            printf("IBIAS_MIN_EN = 0x%x, error.\n", iben);    
        }

        //Current IBMIN config  
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x49, &value);
        curr = (double)(value<<2)*100/4096;        
        printf("Now IBIAS_MIN = %f mA (0x%x)\n", curr, value);
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value2);
        if ((value2&0x08) == 0)
        {
             printf("Now IBIAS_MIN_EN is disable.\n");
        }
        else
        {
             printf("Now IBIAS_MIN_EN is enable.\n");    
        }
    }    

    if(argc == 2)
    { 

        if(0 == strcmp(argv[1], "enable"))
        {

            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x49, &ibmin);
            if (ibmin == 0)
            {
                printf("IBIAS_MIN = 0x%x. It should not be 0.\n", ibmin);
                return -1;
            }
      
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
            value = (value|0x08);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);  
           
            addr = RTL8290B_APC_BIAS_MIN_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
            _europa_cal_flash_data_set(addr, 1, 0x01);       
            printf("IBIAS_MIN_EN is enable.\n");            
        }
        else if (0 == strcmp(argv[1], "disable"))
        {
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
            value = (value&0xF7);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);
 
            addr = RTL8290B_APC_BIAS_MIN_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
            _europa_cal_flash_data_set(addr, 1, 0); 
            printf("IBIAS_MIN_EN is disable.\n");            
        }
        else if (0 == strcmp(argv[1], "clear"))
        {
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
            value = (value&0xF7);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);
 
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
            value = (value&0xFE);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);
            
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x49, 0);
            
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
            value = (value|0x01);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);
            
            addr = RTL8290B_APC_BIAS_MIN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
             _europa_cal_flash_data_set(addr, 1, 0); 
            addr = RTL8290B_APC_BIAS_MIN_EN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
            _europa_cal_flash_data_set(addr, 1, 0); 
            printf("IBIAS_MIN_EN & IBIAS_MIN are clear.\n");                
        }       
        else
        {
             curr = atof(argv[1]);    
 
             if ((25.0 < curr)||(0.097>curr))
             {
                 printf("Invalid ibias mon %f. It should less than 24mA and more than 0.097mA\n", curr);
                 return -1;
             }
             else if (25.0 == curr)
             {
                 ibmin = 0xFF;
             }
             else
             {
                 ibmin = (uint32)(curr * 4096 / 100); //unit is 100mA/4096
                 ibmin = (ibmin&0x3FC)>>2;          
             }
             
             printf("Input current = %f mA, Immax = 0x%x\n", curr, ibmin);
 
             europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
             value = (value&0xFE);
             europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);
 
             europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x49, ibmin);
 
             europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x46, &value);
             value = (value|0x01);
             europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x46, value);
 
             addr = RTL8290B_APC_BIAS_MIN_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE; 
             _europa_cal_flash_data_set(addr, 1, ibmin);         
         }
    }
    else
    {
        printf("%s <txbias min value>/enable/disable/clear \n", argv[0]);
        printf("The unit is mA \n");        
        return -1;
    }

    return 0;    

}

static int europa_cli_cal_level(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint16 dBm2uW[21]= {10,  12,  15,  19,  25,  31,  39,  50,  63,  79, 
                        100, 125, 158, 199, 251, 316, 398, 501, 630, 794,
                        1000};
    int high_value, low_value, en_flag;
    int high_idx, low_idx;
    uint16 high_pwr, low_pwr;
    uint32 data, value, addr;
    double pwr_dB;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if((argc < 4)&&(argc >=2))
    {
        printf("%s <high bound, unit is dBm> <low bound, unit is dBm> <0:disable, 1:enable> \n", argv[0]);      
        return -1;
    }
    
    if(argc == 1)
    { 
        addr = RTL8290B_SOFTWARE_POWER_LEVEL_ENABLE_OFFSET_ADDR+ RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_get(addr, 1, &value);  
        printf("Enable = 0x%x\n", value);

        addr = RTL8290B_SOFTWARE_POWER_LEVEL_HIGH_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;
        _europa_cal_flash_data_get(addr, 2, &value);
        pwr_dB = __log10(((double)(value&0xFFFF)*1/10000))*10;     
        printf("High power = %d dBm (%d)\n", (int)pwr_dB, value);
        
        addr = RTL8290B_SOFTWARE_POWER_LEVEL_LOW_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;  
        _europa_cal_flash_data_get(addr, 2, &value);
        pwr_dB = __log10(((double)(value&0xFFFF)*1/10000))*10;             
        printf("Low power = %d dBm (%d)\n", (int)pwr_dB, value);
        
        return 0;        
    }

    high_value = _vlaue_translate(argv[1]);    
    low_value  = _vlaue_translate(argv[2]);    
    en_flag    = _vlaue_translate(argv[3]);        
    

    if ((high_value > -10)||(high_value < -30)||(low_value > -10) || (low_value < -30)||(high_value < low_value))
    {
        printf("High Threshold should be -10 ~ -30 \n");
        printf("Low Threshold should be -10 ~ -30 \n"); 
        printf("High Threshold should be larger thanLow Threshold.\n");         
        return -1;    
    } 
    
    high_idx = high_value + 30;
    low_idx  = low_value  + 30;    

    high_pwr = dBm2uW[high_idx];    
    low_pwr = dBm2uW[low_idx];

    printf("High value = %d idx = %d, Low value = %d idx = %d  en = %d\n", high_value, high_idx, low_value, low_idx, en_flag); 

    addr = RTL8290B_SOFTWARE_POWER_LEVEL_HIGH_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;  
    data = (uint16)high_pwr;
    _europa_cal_flash_data_set(addr, 2, data);
    printf("Set high power = 0x%x\n", data);
    
    addr = RTL8290B_SOFTWARE_POWER_LEVEL_LOW_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;  
    data = (uint16)low_pwr;
    _europa_cal_flash_data_set(addr, 2, data);
    printf("Set low power = 0x%x\n", data);

    addr = RTL8290B_SOFTWARE_POWER_LEVEL_ENABLE_OFFSET_ADDR+ RTL8290B_PARAM_ADDR_BASE;      
    _europa_cal_flash_data_set(addr, 1, en_flag);  
    printf("Enable = 0x%x\n", en_flag);
    
    return 0;    

}

static int europa_cli_cal_ben(
    int argc,
    char *argv[],
    FILE *fp)
{
    int32   ret;
    uint32 chipId, rev, subType;
    uint32 addr, value, ib, im;      
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if((argc < 2)||(argc > 3))
    { 
        //system("diag gpon get ");
        printf("%s <on/off>\n", argv[0]);
        return -1; 
    }

    if((ret = rtk_switch_version_get(&chipId, &rev, &subType)) != 0)
    {
        return ret;
    }

    if(argc >= 2)
    { 
        if(0 == strcmp(argv[1], "on"))
        {  
            if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
            {
                //20220719, recover BEN config after PON init...
                system("diag register set 0x40540 0"); 
                system("diag register get 0x40540");
            }
            else if (RTL9602C_CHIP_ID == chipId)
            {
                system("diag register set 0x22540 0x30");
            }
#if defined(CONFIG_LUNA_G3_SERIES)
            //For Cortina ARM-based 9607DQ series
            else if (RTL8277C_CHIP_ID == chipId)
            {
                system("rt_ponmisc set force-laser-on state enable");
            }
#endif
            else
            {
                printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
            }    
        }
        else if(0 == strcmp(argv[1], "off"))
        {  
            if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
            {
                //20220718, It is OK, but needs to reset after PON sedes init.
                system("diag register set 0x40540 0x1C"); 
                system("diag register get 0x40540");                             
            }
            else if (RTL9602C_CHIP_ID == chipId)
            {
                //EPON PRBS OFF           
                //system("diag register set 0x022A30 0");
                //GPON PRBS OFF
                //system("diag register set 0x22098 0"); 
                //BEN OFF
                //system("diag register set 0x360a0 0");
                /*force BEN=0*/                
                system("diag register set 0x22540 0");
            }
#if defined(CONFIG_LUNA_G3_SERIES)
            //For Cortina ARM-based 9607DQ series
            else if (RTL8277C_CHIP_ID == chipId)
            {
                system("rt_ponmisc set force-laser-on state disable");
            }
#endif
            else
            {
                printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
            }        
        }    
    }
    
    return 0; 
}


static int europa_cli_driver_version_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 ver;
    int ret;
    
    ver = 0;
    ret = rtk_ldd_driver_version_get(&ver);
    if (ret !=0)
    {
        ver = 0;
    }

    printf("LDD Driver Version %d.%d.%d\n",
            LDD_DRIVER_VERSION_MAJOR(ver),
            LDD_DRIVER_VERSION_MINOR(ver),
            LDD_DRIVER_VERSION_PATCH(ver));
    printf("Command Line Versio:n %s\n", EUROPA_CMD_VERSION);
    printf("Command Line Version(RTL8290C): %s\n", RTL8290C_CMD_VERSION);   
    printf("Command Line Version(RTL8291): %s\n", RTL8291_CMD_VERSION);   
    return 0; 
}

void _gen_laserlut(uint32 bias, uint32 mod, int32 temperature, double sb0, double sb1, double sm0, double sm1)
{
    int ret;
    uint32  ib0, im0;
    double  ftmp;

    uint8 lut[LASER_LUT_SZ] = { 0 };
    int t, t0;

    ib0 = bias;
    im0 = mod;
    t0  = temperature;
        
    printf("Ib=%d Im=%d T0=%d SB0=%f SB1=%f SM0=%f SM1=%f\n",
            ib0, im0, t0, sb0, sb1, sm0, sm1);
        
    for (t=-40; t<=110; t++)
    {
       if (t == t0)
       {
           lut[(t+40)*2] = IBCONV(ib0);
           lut[(t+40)*2+1] = IMCONV(im0);
       }
       else if (t < t0)
       {
           ftmp = ib0 - (t0 - t)*sb0;
           if (3.0 > ftmp)
              ftmp=3.0;
           lut[(t+40)*2] = IBCONV((int)ftmp);
        
           ftmp = im0 - (t0 - t)*sm0;
           if (12.0 > ftmp)
               ftmp=12.0;
           lut[(t+40)*2+1] = IMCONV((int)ftmp);
       }
       else if (t > t0)
       {
           ftmp = ib0 + (t - t0)*sb1;
           if (ftmp>50.0)
           ftmp=50.0;
           lut[(t+40)*2] = IBCONV((int)ftmp);
        
           ftmp = im0 + (t - t0)*sm1;
           if (ftmp>80.0)
              ftmp=80.0;
           lut[(t+40)*2+1] = IMCONV((int)ftmp);
       }
    }
        
    _europa_cal_flash_array_set(RTL8290B_LASER_LUT_ADDR, LASER_LUT_SZ, lut);
        
    for (t=-40; t<=110; t++)
    {
       printf("%2x %2x", lut[(t+40)*2], lut[(t+40)*2+1]);
       printf(" ");
        
       if ((t+1)%8 == 0)
       {
          printf("\n");
       }
    }
    printf("\n");

    return;    
}

int my_round(double d)
{
    return (int) ( (0.0<d) ? (d+0.5) : (d-0.5) );
}
void __laserlut_lin_interpolation(laserlut_data_t sdata[], int num, laserlut_data_t output[])
{
   int i;
   int data_index;

   int start, end;
   double slope_b, slope_m;
   int offset;

   data_index = 0;

   for (data_index=0; data_index < (num-1); data_index++)
   {
      start = sdata[data_index].temp+40;
      end = sdata[data_index+1].temp+40;
      offset = end - start;

      /* fill sampled data */
      output[start].bias = sdata[data_index].bias;
      output[start].mod  = sdata[data_index].mod;
      output[end].bias = sdata[data_index+1].bias;
      output[end].mod  = sdata[data_index+1].mod;

      /* calculate slope */
      slope_b = (sdata[data_index+1].bias - sdata[data_index].bias)/offset;
      slope_m = (sdata[data_index+1].mod  - sdata[data_index].mod )/offset;

      printf("%d~%d: offset=%d slopb_b=%f slope_m=%f\n",
              sdata[data_index].temp, sdata[data_index+1].temp,
              offset, slope_b, slope_m);

      /* interpolation */
      for (i=start; i<(end-1); i++)
      {
          output[i+1].bias = output[i].bias + slope_b;
          output[i+1].mod  = output[i].mod  + slope_m;
      printf("[%d]: %f %f\n", i+1-40, output[i+1].bias, output[i+1].mod);
      }
   }

   /* extra-polation for high temp */
   start = sdata[num-1].temp+40;
   end = 110+40;
   for (i=start; i<end; i++)
   {
      output[i+1].bias = output[i].bias + slope_b;
      output[i+1].mod  = output[i].mod  + slope_m;
      printf("[%d]: %f %f\n", i+1-40, output[i+1].bias, output[i+1].mod);
   }
}


void __laserlut_fixup(laserlut_data_t data[], laserlut_data_t output[], int chop)
{
   int start, end;
   int i;

   /* in case the lowest point is higher than 0 degC */
   if (0 < data[0].temp)
   {
       for (i=0; i<data[0].temp; i++)
       {
           output[i+40].bias = data[0].bias;
           output[i+40].mod  = data[0].mod;
       }
   }

   /* 0~-40, use data from 0C */
   start = -40;
   end = 0;
   for (i=(start+40); i<(end+40); i++)
   {
       output[i].bias = output[end+40].bias;
       output[i].mod  = output[end+40].mod;
   }

   /* 100~110, use data from 100C */
   start = chop;
   end = 110;
   for (i=(start+40); i<=(end+40); i++)
   {
       output[i].bias = output[start+40].bias;
       output[i].mod  = output[start+40].mod;
   }
}

void __laserlut_data_add(int index, int32_t temp, double bias, double mod)
{
    index = index - 1;

    if (index > 0 || index <= MAX_LASER_LUT_DATA_NUM)
    {
        gLaserLutData[index].temp = temp;
    gLaserLutData[index].bias = bias;
    gLaserLutData[index].mod = mod;
    }
}


void __laserlut_data_dump()
{
    int i;

    for (i=0; i<MAX_LASER_LUT_DATA_NUM; i++)
    {
       if (0.0==gLaserLutData[i].bias || 0.0==gLaserLutData[i].mod)
       {
           break;
       }
       else
       {
           printf("  [%2d]  %3d  %2.2f  %2.2f\n",
           i+1, gLaserLutData[i].temp,
           gLaserLutData[i].bias, gLaserLutData[i].mod); 
       }
    }
}

int __laserlut_data_num_entry()
{
    int i;
    int num;

    for (i=0, num=0; i<MAX_LASER_LUT_DATA_NUM; i++)
    {
       if (0.0==gLaserLutData[i].bias || 0.0==gLaserLutData[i].mod)
       {
           break;
       }
       else
       {
       num++;
       }
    }

    printf("Num Data entry : %d\n", num);

    return num;
}


void __laserlut_data_clear()
{
    int i;

    for (i=0; i<MAX_LASER_LUT_DATA_NUM; i++)
    {
    gLaserLutData[i].temp = 0;
    gLaserLutData[i].bias = 0.0;
    gLaserLutData[i].mod  = 0.0;
    }
}

void __laserlut_data_load(void)
{
    FILE *fp;
    int ret=0;

    fp = fopen(EUROPA_CAL_FILE_LOCATION,"rb");
    if (NULL ==fp)
    {
        printf("%s does not exist. Create an empty file.\n", EUROPA_CAL_FILE_LOCATION);       
        fp = fopen(EUROPA_CAL_FILE_LOCATION,"wb");
        if (NULL ==fp)
        {
            printf("Create %s error!!!!!!!\n", EUROPA_CAL_FILE_LOCATION); 
            return;    
        }
        fclose(fp);
        return;
    }
    //printf("Test gLaserLutData !!!!!!!\n");
    ret=fseek(fp, EUROPA_CAL_LASERLUT_BASE_ADDR, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp); 
        return;
    }
    memset(gLaserLutData, 0x0, sizeof(gLaserLutData));
    ret=fread(gLaserLutData, 1, sizeof(gLaserLutData), fp);    
    //if (ret!=0)
    //{
    //    fclose(fp);
    //    return;
    //}

    ret=fseek(fp, EUROPA_CAL_LASERLUT_BASE_ADDR+sizeof(gLaserLutData), SEEK_SET);
    if (ret!=0)
    {
        fclose(fp);
        return;
    }    
    memset(gLaserLut, 0x0, sizeof(gLaserLut));
    ret=fread(gLaserLut, 1, sizeof(gLaserLut), fp); 
    //if (ret!=0)
    //{
    //    fclose(fp);
    //    return;
    //}    
    fclose(fp);
    
    //printf("\n%s: %d\n",__FUNCTION__, __LINE__); 

    return;    
}

void __laserlut_data_save()
{
    FILE *fp;
    int wfd = -1;    
    uint8 temp8, i;
    uint8 *init_data, *ptr_data;
    uint32 temp32;
    int ret=0;

    fp = fopen(EUROPA_CAL_FILE_LOCATION,"rb");
    if (NULL ==fp)
    {
        printf("Open %s error!!!!!!!\n", EUROPA_CAL_FILE_LOCATION);
        return;
    }
    
    init_data = (uint8 *)malloc(EUROPA_CAL_FILE_SIZE*sizeof(uint8));    
    if (!init_data)
    {
        printf("Memory Allocation Fail!!!!!!!\n");
        fclose(fp);        
        return;
    } 
    ret = fread(init_data, 1, EUROPA_CAL_FILE_SIZE, fp);
    //if (ret!=0)
    //{
    //    fclose(fp);
    //    return;
    //}
    fclose(fp);

    ptr_data = init_data + EUROPA_CAL_LASERLUT_BASE_ADDR; 
    memcpy(ptr_data, gLaserLutData, sizeof(gLaserLutData));

    ptr_data += sizeof(gLaserLutData); 
    memcpy(ptr_data, gLaserLut, sizeof(gLaserLut));

    fp = fopen(EUROPA_CAL_FILE_LOCATION, "wb");         
    if (NULL ==fp)
    {
        printf("Open %s error!!!!!!!\n", EUROPA_CAL_FILE_LOCATION);
        free(init_data);         
        return;             
    }
    fwrite(init_data,sizeof(uint8), EUROPA_CAL_FILE_SIZE, fp);
    
    fflush(fp); 
    wfd = fileno(fp);
    if(wfd != -1)
    {
        fsync(wfd);
    }
    
    fclose(fp);
    free(init_data); 

    //printf("\n%s: %d\n",__FUNCTION__, __LINE__); 

}

/*
 *  init RSSI-V0
 */
uint32 _europa_cal_init(void)
{
    int ret;
    uint32 rssi_v0;
    uint32 mpd0;
    uint32 i;
    uint64 sum;
    uint32 index;
    uint32 w83_data;
    uint8 reg;

    printf("\n *** Initialize for self-calibration ***\n\n"
      " ==> Please keep dark condition, NO optical Tx/Rx power!\n\n");
    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);

    osal_time_mdelay(500); 


    /* get RSSI-V0 - ensure dark condition */
    rssi_v0 = 0;
    sum = 0;

    for (i=0;i<100;i++)
    {
        ret = rtk_ldd_rssiV0_get(&rssi_v0);
        if(ret)
        {
            printf("Get RSSI V0 fail!!! (%d)\n", ret);
            europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
            return -1;
        }
        sum += rssi_v0;
    }
    rssi_v0 = sum/100;
    
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);

    printf("RSSI V0 = %d\n", rssi_v0);

    _europa_cal_flash_data_set(RTL8290B_RSSI_V0_ADDR, 4, rssi_v0);

    return 0;
}

/*
 *  load parameters which will not init by driver (i.e not registers)
 */
uint32 _europa_cal_load(void)
{
   uint8 reg;
   uint32 parm;
   uint32 mpd0, rssi_v0;

   printf("Loading parameters...\n");

   /* MPD0 */
   _europa_cal_flash_data_get(RTL8290B_MPD0_ADDR, 4, &mpd0);
   _europa_cal_data_add(EUROPA_CAL_MPD0_ADDR, mpd0);


   /* RSSI-V0*/
   _europa_cal_flash_data_get(RTL8290B_RSSI_V0_ADDR, 4, &rssi_v0);
   _europa_cal_data_add(EUROPA_CAL_RSSIV0_ADDR, rssi_v0);   

   /* TX DDMI - slope & offset */
   {
     _europa_cal_flash_data_get(RTL8290B_TX_A_ADDR, 4, &parm);
     _europa_cal_data_add(EUROPA_CAL_TX_A_ADDR, parm); 
     _europa_cal_flash_data_get(RTL8290B_TX_B_ADDR, 4, &parm);
     _europa_cal_data_add(EUROPA_CAL_TX_B_ADDR, parm); 
     _europa_cal_flash_data_get(RTL8290B_TX_C_ADDR, 4, &parm);
     _europa_cal_data_add(EUROPA_CAL_TX_C_ADDR, parm); 
   }


   /* RX DDMI - slope & offset */
   {
     _europa_cal_flash_data_get(RTL8290B_RX_A_ADDR, 4, &parm);
     _europa_cal_data_add(EUROPA_CAL_RX_A_ADDR, parm); 
     _europa_cal_flash_data_get(RTL8290B_RX_B_ADDR, 4, &parm);
     _europa_cal_data_add(EUROPA_CAL_RX_B_ADDR, parm); 
     _europa_cal_flash_data_get(RTL8290B_RX_C_ADDR, 4, &parm);
     _europa_cal_data_add(EUROPA_CAL_RX_B_ADDR, parm); 
   }


   /* DDMI - Temperature Offset */
   {
      _europa_cal_flash_data_get(RTL8290B_TEMP_OFFSET_ADDR, 1, &parm);
      _europa_cal_data_add(EUROPA_CAL_TOFFSET_ADDR, parm); 
   }

#if 0
   /* Temperature interrupt scale */
   rtk_ldd_flash_data_get(EUROPA_TEMP_INTR_SCALE_ADDR, 1, &ldd_param.temp_intr_scale);


   /* RX SD - LOS assert/de-assert threshold */
   rtk_ldd_flash_data_get(EUROPA_RX_TH_ADDR, 1, &ldd_param.rx_th);
   rtk_ldd_flash_data_get(EUROPA_RX_DETH_ADDR, 1, &ldd_param.rx_deth);
#endif

   printf("done.\n");

   return 0;
}


/*
 *  save parameters
 */
uint32 _europa_cal_save(void)
{
   uint8 reg;

   printf("Saving parameters...\n");

#if 0
   /* TX - P0_target */
   rtk_ldd_parameter_get(1, EUROPA_REG_W58_ADDR, &reg);
   _europa_cal_flash_data_set(EUROPA_DCL_P0_ADDR, 1, reg);

   /* TX - P1_target */
   rtk_ldd_parameter_get(1, EUROPA_REG_W59_ADDR, &reg);
   _europa_cal_flash_data_set(EUROPA_DCL_P1_ADDR, 1, reg);

   /* TX - Pavg_target */
   rtk_ldd_parameter_get(1, EUROPA_REG_W61_ADDR, &reg);
   _europa_cal_flash_data_set(EUROPA_DCL_PAVG_ADDR, 1, reg);

   /* TX - DAC scale */
   rtk_ldd_parameter_get(1, EUROPA_REG_W57_ADDR, &reg);
   reg = (reg>>EUROPA_REG_W57_APCDIG_DAC_BIAS_HALF_OFFSET) & 0x01;
   reg = reg ^ 1; // in flash, 1=FULL SCALE, but in chip, 1=HALF SCALE
   _europa_cal_flash_data_set(EUROPA_W57_APCDIG_DAC_BIAS_ADDR, 1, reg);

   /* TX - Crossing */
   rtk_ldd_parameter_get(1, EUROPA_REG_W48_ADDR, &reg);
   _europa_cal_flash_data_set(RTL8290B_CROSS_ADDR, 1, reg);


   /* TX DDMI - slope & offset */
   _europa_cal_flash_data_set(RTL8290B_TX_A_ADDR, 4, (uint32)ldd_param.tx_a);
   _europa_cal_flash_data_set(RTL8290B_TX_B_ADDR, 4, (uint32)ldd_param.tx_b);
   _europa_cal_flash_data_set(RTL8290B_TX_C_ADDR, 4, (uint32)ldd_param.tx_c);


   /* RX DDMI - slope & offset */
   _europa_cal_flash_data_set(RTL8290B_RX_A_ADDR, 4, (uint32)ldd_param.rx_a);
   _europa_cal_flash_data_set(RTL8290B_RX_B_ADDR, 4, (uint32)ldd_param.rx_b);
   _europa_cal_flash_data_set(RTL8290B_RX_C_ADDR, 4, (uint32)ldd_param.rx_c);


   /* DDMI - Temperature Offset */
   _europa_cal_flash_data_set(RTL8290B_TEMP_OFFSET_ADDR, 1, (uint32)ldd_param.temp_offset);

   /* Temperature interrupt scale */
   rtk_ldd_parameter_get(1, EUROPA_REG_TEMP_INT_OFFSET_ADDR, &ldd_param.temp_intr_scale);
   _europa_cal_flash_data_set(RTL8290B_TEMP_INTR_SCALE_ADDR, 1, ldd_param.temp_intr_scale);


   /* RX SD - LOS assert/de-assert threshold */
   rtk_ldd_parameter_get(1, EUROPA_REG_RX_TH_ADDR, &ldd_param.rx_th);
   _europa_cal_flash_data_set(RTL8290B_RX_TH_ADDR, 1, ldd_param.rx_th);

   rtk_ldd_parameter_get(1, EUROPA_REG_RX_DE_TH_ADDR, &ldd_param.rx_deth);
   _europa_cal_flash_data_set(RTL8290B_RX_DETH_ADDR, 1, ldd_param.rx_deth);

   printf("done.\n");
#endif

   return 0;
}

/*
 *  refresh parameters
 */
uint32 _europa_cal_refresh(void)
{
   int ret;
   uint32 w83_data;

   europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);     
   europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);

   printf("Refreshing parameters...\n");

   ret = rtk_ldd_config_refresh();
   if(ret)
   {
      printf("Refresh failed!!! (%d)\n", ret);
      return -1;
   }
 
   europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data); 
   printf("done.\n");

   return 0;
}

static int europa_cli_gen_laserlut(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32  ib0, im0;
    double  sb0, sb1, sm0, sm1;
    double  ftmp;

    uint8 lut[LASER_LUT_SZ] = { 0 };
    int t, t0;

    if(argc >= 8)
    { 
        ib0 = _vlaue_translate(argv[1]);
        im0 = _vlaue_translate(argv[2]);
        t0  = _vlaue_translate(argv[3]);
        
        sb0 = atof(argv[4]);  
        sb1 = atof(argv[5]); 
        sm0 = atof(argv[6]); 
        sm1 = atof(argv[7]); 
        
        printf("Ib=%d Im=%d T0=%d SB0=%f SB1=%f SM0=%f SM1=%f\n",
                ib0, im0, t0, sb0, sb1, sm0, sm1);
        
        _gen_laserlut(ib0, im0, t0, sb0, sb1, sm0, sm1);

    }
    else
    {
        printf("%s <UINT:IB0> <UINT:IM0> <UINT:T0> <FLOAT:SB0> <FLOAT:SB1> <FLOAT:SM0> <FLOAT:SM1> \n", argv[0]);   
        return -1;
    }

    return 0;    

}

static int europa_cli_laserlut_set(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32    bias, mod;
    double    sb0, sb1, sm0, sm1;
    int32     temperature;
    
    uint16 tempK;
    
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    {     
        rtl8290c_cli_laserlut_set(devId);
        return 0; 
    }
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    {     
        rtl8291_cli_laserlut_set(devId);
        return 0; 
    }
    else
    {
        ret = rtk_ldd_tx_bias_get(&bias);
        if(ret)
        {
            printf("Get Bias Fail!!! (%d)\n", ret);
            return -1;
        }
        bias = (bias * 2)/1000;  /* 2uA to mA */
        
        ret = rtk_ldd_tx_mod_get(&mod);
        if(ret)
        {
            printf("Get Mod Fail!!! (%d)\n", ret);
                return -1;
        }
        mod = (mod * 2)/1000;  /* 2uA to mA */
        
        tempK = 0;
        ret = rtk_ldd_temperature_get(&tempK);
        if(ret)
        {
            printf("Get Temperature Fail!!! (%d)\n", ret); 
            return -1;
        }
        temperature = (int32)(tempK*100-27315)/100; /* degK to degC */
            
        sb0 = 0.06;
        sb1 = 0.12;
        sm0 = 0.03;
        sm1 = 0.10;
        
        _gen_laserlut(bias, mod, temperature, sb0, sb1, sm0, sm1);
    }
    return 0; 
}

static int europa_cli_laserlut_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    uint8 lut[LASER_LUT_SZ];
    int32  i, j, num;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    _europa_cal_flash_array_get(RTL8290B_LASER_LUT_ADDR, LASER_LUT_SZ, lut);

    printf("Temperature  &  DAC value\n ");

    for (i=0;i<10;i++)
    {
        if (i<9)
        {
            num =16;
        }
        else
        {
            num = 7;
        }

        printf("Temperature : ");
        for(j=0;j<num;j++)
        {
            printf("%4d  ", j+i*16-40);
        }
        printf("\n ");
        printf("BIAS/MOD    : ");
        for(j=0;j<num;j++)
        {
            printf("%02x/%02x ", lut[(j+i*16)*2], lut[(j+i*16)*2+1]);
        }
        printf("\n\n ");
    }

    return 0; 
}

static int europa_cli_laserlut_dump(
    int argc,
    char *argv[],
    FILE *fp)
{
    uint8 lut[LASER_LUT_SZ];
    int32 t;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    _europa_cal_flash_array_get(RTL8290B_LASER_LUT_ADDR, LASER_LUT_SZ, lut);

    printf("LaserLUT\n");

    for (t=-40; t<=110; t++)
    {
       printf("%02x %02x", lut[(t+40)*2], lut[(t+40)*2+1]);
       printf(" ");

       if ((t+1)%8 == 0)
       {
          printf("\n");
       }
    }
    printf("\n");

    return 0;

}

/*
* debug europa get laserinfo { <UINT:extra> }
*/
static int europa_cli_laserinfo_get(
        int argc,
        char *argv[],
        FILE *fp)

{
    int ret;
    uint32 bias;
    uint32 mod;
    uint16 tempK;
    int32 tempC;

    uint32 data;
    int8 offset;

    double ib, im;
    uint32 devId = 5, value2;
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if (EUROPA_LDD_8290C_MODE == chip_mode)
    {
        ret = rtl8290c_cli_laserinfo_get(devId);
        if(ret)
        {
            printf("rtl8290c_cli_laserinfo_get Fail!!! (%d)\n", ret);
            return -1;
        }    
    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        ret = rtl8291_cli_laserinfo_get(devId);
        return ret; 
    }
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
        bias = 0;
        ret = rtk_ldd_tx_bias_get(&bias);
        if(ret)
        {
            printf("Get TX Bias Fail!!! (%d)\n", ret);
            return -1;
        }
    
        ib = ((double)bias*2)/1000;  /* 2uA to mA */
    
        mod = 0;
        ret = rtk_ldd_tx_mod_get(&mod);
        if(ret)
        {
            printf("Get TX Mod Fail!!! (%d)\n", ret);
            return -1;
        }
    
        im = ((double)mod*2)/1000;  /* 2uA to mA */
    
    
        tempK = 0;
        ret = rtk_ldd_temperature_get(&tempK);
        if(ret)
        {
            printf("Get Temperature Fail!!! (%d)\n", ret);
            return -1;
        }
    
        tempC = (tempK*100-27315)/100; /* degK to degC */
    
    
        _europa_cal_flash_data_get(RTL8290B_TEMP_OFFSET_ADDR, 1, &data);   
        offset = (int8)(data);
    
        tempC = tempC - offset;
        printf("BOSA Temperature = %dC \n", tempC);
    
    
    
        if(argc < 2)
        {
            printf("\n[%4d] Bias=%2.2f Modulation=%2.2f\n", 
                tempC, ib, im);
        }
        else
        {
            uint32 impd;
            uint8 loopmode;
    
            impd = _europa_cal_get_mpd_current();
    
            impd = impd / 1000;   /* nA to uA */
    
            europa_i2c_read(EUROPA_I2C_PORT, 0x54, 124, &loopmode);
            
            loopmode = (loopmode&0xC0)>>6;
    
            printf("\n[%4d] Bias=%2.2f Modulation=%2.2f Impd=%4d loopmode=%d\n", 
            tempC, ib, im, impd, loopmode);
        }
    }
    else 
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;
    }

    
    return 0;
}    /* end of cparser_cmd_debug_europa_get_laserinfo_extra */


/* +++++++++++++++ LUT generation ++++++++++++++++++ */

/*
 * debug europa generate laserlut2 data <UINT:index> <INT:temp> <FLOAT:bias> <FLOAT:mod>
 */
static int europa_cli_gen_laserlut2_data(
    int argc,
    char *argv[],
    FILE *fp)

{
    int32 index;
    int32  temp;
    double bias, mod;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if (argc < 5)
    {
        printf("%s <UINT:index> <INT:temp> <FLOAT:bias> <FLOAT:mod> \n", argv[0]);   
        return -1;
    }

    __laserlut_data_load();

    index = _vlaue_translate(argv[1]);
    if (0==index)
    {
        __laserlut_data_clear();
        return 0;
    }
    else if (index < 0)
    {
        printf("Invalid index\n");
        return -1;
    }

    temp = _vlaue_translate(argv[2]);

    if ((-40 > temp) || (110 < temp))
    {
        printf("Invalid temp\n");
        return -1;
    }

    bias = atof(argv[3]);

    if (0.0 == bias)
    {
        printf("Invalid bias\n");
        return -1;
    }

    mod = atof(argv[4]);

    if (0.0 == mod)
    {
        printf("Invalid mod\n");
        return -1;
    }
    
    __laserlut_data_add(index, temp, bias, mod);

    printf("\nEntery added. [%d] Temp=%3d Bias=%2.2f Mod=%2.2f\n", 
            index, temp, bias, mod);

    __laserlut_data_dump();

    __laserlut_data_save();    

    return 0;
}    /* end of cparser_cmd_debug_europa_generate_laserlut2_data_index_temp_bias_mod */


/*
 * debug europa generate laserlut2 clear
 */
static int europa_cli_gen_laserlut2_clear(
    int argc,
    char *argv[],
    FILE *fp)

{
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    __laserlut_data_load();

    __laserlut_data_clear();
    
    __laserlut_data_save(); 

    return 0;
}  


/*
 * debug europa generate laserlut2 calc { <UINT:Tchop> }
 */
static int europa_cli_gen_laserlut2_calc(
    int argc,
    char *argv[],
    FILE *fp)

{
    uint32 Tchop;
    int num;

    uint8 lut[LASER_LUT_SZ] = { 0 };
    int t;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if (argc < 2)
    {
        printf("%s <UINT:Tchop> \n", argv[0]); 
        Tchop = 100;        

    }
    else
    {
        Tchop = _vlaue_translate(argv[1]);
    }

    printf("Tchop = %d \n", Tchop); 

    __laserlut_data_load();

    num = __laserlut_data_num_entry();

    __laserlut_lin_interpolation(gLaserLutData, num, gLaserLut);

    __laserlut_fixup(gLaserLutData, gLaserLut, Tchop);


    for (t=-40; t<=110; t++)
    {
       lut[(t+40)*2] = IBCONV((uint32)my_round(gLaserLut[t+40].bias));
       lut[(t+40)*2+1] = IMCONV((uint32)my_round(gLaserLut[t+40].mod));
    }

    _europa_cal_flash_array_set(RTL8290B_LASER_LUT_ADDR, LASER_LUT_SZ, lut);

    for (t=-40; t<=110; t++)
    {
       printf("%02x %02x", lut[(t+40)*2], lut[(t+40)*2+1]);
       printf(" ");

       if ((t+1)%8 == 0)
       {
          printf("\n");
       }
    }
    printf("\n");
    
    __laserlut_data_save();

    return 0;
}   

/*
 * debug europa cal laserlut data <UINT:index>
 */
static int europa_cli_cal_laserlut(
    int argc,
    char *argv[],
    FILE *fp)

{
    int ret;
    int32 index;
    uint32 bias;
    uint32 mod;
    uint16 tempK;
    int32 tempC;

    double ib, im;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if (argc<2)
    {
        printf("%s <UINT:index> \n", argv[0]);
        return -1;
    }

    __laserlut_data_load();

    //printf("\n%s: %d\n",__FUNCTION__, __LINE__); 

    index = _vlaue_translate(argv[1]);
    if (0==index)
    {
        __laserlut_data_clear();
        return 0;
    }
    else if (index < 0)
    {
        printf("Invalid index\n");
        return -1;
    }
    
    bias = 0;
    ret = rtk_ldd_tx_bias_get(&bias);
    if(ret)
    {
        printf("Get TX Bias Fail!!! (%d)\n", ret);
        return -1;
    }

    ib = ((double)bias*2)/1000;  /* 2uA to mA */
    //printf("\n%s: %d\n",__FUNCTION__, __LINE__); 

    mod = 0;
    ret = rtk_ldd_tx_mod_get(&mod);
    if(ret)
    {
        printf("Get TX Mod Fail!!! (%d)\n", ret);
        return -1;
    }

    im = ((double)mod*2)/1000;  /* 2uA to mA */

    //printf("\n%s: %d\n",__FUNCTION__, __LINE__); 

    tempK = 0;
    ret = rtk_ldd_temperature_get(&tempK);
    if(ret)
    {
        printf("Get Temperature Fail!!! (%d)\n", ret);
        return -1;
    }

    tempC = (tempK*100-27315)/100; /* degK to degC */


    __laserlut_data_add(index, tempC, ib, im);

    printf("\nEntery added. [%d] Temp=%3d Bias=%2.2f Mod=%2.2f\n", 
            index, tempC, ib, im);

    __laserlut_data_dump();
    __laserlut_data_save(); 
    
    return 0;
}  

static int europa_cli_cal_init(
    int argc,
    char *argv[],
    FILE *fp)

{
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    _europa_cal_init();
    
    return 0;
}    

static int europa_cli_cal_load(
    int argc,
    char *argv[],
    FILE *fp)

{
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    _europa_cal_load();

    return 0;
}   

static int europa_cli_cal_save(
    int argc,
    char *argv[],
    FILE *fp)

{
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    _europa_cal_save();

    return 0;
}    

static int europa_cli_cal_refresh(
    int argc,
    char *argv[],
    FILE *fp)

{
    uint32 devId = 5;
    int chip_mode;
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif

    chip_mode = _europa_ldd_parser();

    if (EUROPA_LDD_8290C_MODE == chip_mode)
    {
        rtl8290c_cli_cal_refresh(devId, argc, argv, fp);    
    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        rtl8291_cli_cal_refresh(devId, argc, argv, fp);    
    }	
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
        _europa_cal_refresh();
    }
    else 
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;
    }

 #ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_refresh Duration = %f ms\n", Duration);
#endif
    
    return 0;
}    /* end of cparser_cmd_debug_europa_cal_init_load_save */


static int europa_cli_loopmode_set(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 loop_mode, value, tx_sd_threshold;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if (EUROPA_LDD_8290C_MODE == chip_mode)
    {
        ret = rtl8290c_cli_loopmode_set(devId, argc, argv, fp);
        if(ret)
        {
            printf("rtl8290c_cli_loopmode_set Fail!!! (%d)\n", ret);
            return -1;
        }
        return 0;        
    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        ret = rtl8291_cli_loopmode_set(devId, argc, argv, fp);
        if(ret)
        {
            printf("rtl8291_cli_loopmode_set Fail!!! (%d)\n", ret);
            return -1;
        }
        return 0;
    }
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {
        if(argc >= 2)
        { 
            if(0 == strcmp(argv[1], "dol"))
            {
                loop_mode = LDD_LOOP_DOL;
                printf("Change to DOL mode\n");
    
            }
            else if(0 == strcmp(argv[1], "dcl"))
            {
                loop_mode = LDD_LOOP_DCL;
                printf("Change to DCL mode\n");
    
            }
            else if(0 == strcmp(argv[1], "scl-bias"))
            {
                loop_mode = LDD_LOOP_SCL_BIAS;
                printf("Change to SCL BIAS mode\n");
            }
            else if(0 == strcmp(argv[1], "scl-mod"))
            {
                loop_mode = LDD_LOOP_SCL_MOD;
                printf("Change to SCL MOD mode\n");
            }
            else
            {
                printf("%s <dol/dcl/scl-bias/scl-mod> \n", argv[0]);        
                return -1;
            }
                
            rtk_ldd_loopMode_set(loop_mode);
    
            _europa_cal_flash_data_set(RTL8290B_APC_LOOP_MODE_OFFSET_ADDR+RTL8290B_PARAM_ADDR_BASE, 1, loop_mode);           
    
            return 0;
    
        }
        else
        {
            printf("%s <dol/dcl/scl-bias/scl-mod> \n", argv[0]);        
            return -1;
        }
    }
    else 
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;
    }

    return 0;    

}

static int europa_cli_loopmode_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 mode, value, tx_sd_threshold;
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if (EUROPA_LDD_8290C_MODE == chip_mode)
    {
        ret = rtl8290c_cli_loopmode_get(devId, argc, argv, fp);
        if(ret)
        {
            printf("rtl8290c_cli_loopmode_get Fail!!! (%d)\n", ret);
            return -1;
        }
        return 0;
    }
    else if (EUROPA_LDD_8291_MODE == chip_mode)
    {
        ret = rtl8291_cli_loopmode_get(devId, argc, argv, fp);
        if(ret)
        {
            printf("rtl8291_cli_loopmode_get Fail!!! (%d)\n", ret);
            return -1;
        }
        return 0;
    }
    else if (EUROPA_LDD_8290B_MODE == chip_mode)
    {

        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 124, &mode);
    
        mode = (mode&0xC0)>>6;
    
        if (mode == 0)
        {
           printf("Loop mode is DOL mode\n");
    
        }
        else if(mode == 0x3)
        {
            printf("Loop mode is DCL mode\n");
        }
        else if(mode == 0x2)
        {
            printf("Loop mode is SCL MOD mode\n");
        }
        else if(mode == 0x1)
        {
            printf("Loop mode is SCL BIAS mode\n");
        }
        else
        {
            printf("Loop mode = 0x%x \n", mode);        
        }
    
        _europa_cal_flash_data_get(RTL8290B_APC_LOOP_MODE_OFFSET_ADDR+RTL8290B_PARAM_ADDR_BASE, 1, &value);           
    
        printf("Loop mode in flash = 0x%x \n", value);    
    #ifdef RTL8290B_CAL_TIME
        gettimeofday(&tv_tmp2,&tz);    
        Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
        printf("[TIME]:  europa_cli_loopmode_get Duration = %f ms\n", Duration);
    #endif
    }
    else 
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;
    }

    return 0;    

}


static int europa_cli_chip_init(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 loop_mode, value, tx_sd_threshold;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if (EUROPA_LDD_8290C_MODE == chip_mode)
    {  
        printf("Init RTL8290C...\n");
        rtk_ldd_chip_init2(devId);
        return 0; 
    }
    
    if (EUROPA_LDD_8291_MODE == chip_mode)
    {  
        printf("Init RTL8291...\n");
        rtk_ldd_chip_init2(devId);
        return 0; 
    }

    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if(argc >= 2)
    { 
        if(0 == strcmp(argv[1], "dol"))
        {
            loop_mode = LDD_LOOP_DOL;
            printf("Change to DOL mode\n");

        }
        else if(0 == strcmp(argv[1], "dcl"))
        {
            loop_mode = LDD_LOOP_DCL;
            printf("Change to DCL mode\n");

        }
        else if(0 == strcmp(argv[1], "scl-bias"))
        {
            loop_mode = LDD_LOOP_SCL_BIAS;
            printf("Change to SCL BIAS mode\n");
        }
        else if(0 == strcmp(argv[1], "scl-mod"))
        {
            loop_mode = LDD_LOOP_SCL_MOD;
            printf("Change to SCL MOD mode\n");
        }
        else
        {
            printf("%s <dol/dcl/scl-bias/scl-mod> \n", argv[0]);        
            return -1;
        }
            
        rtk_ldd_loopMode_set(loop_mode);

        _europa_cal_flash_data_set(RTL8290B_APC_LOOP_MODE_OFFSET_ADDR+RTL8290B_PARAM_ADDR_BASE, 1, loop_mode);           

        rtk_ldd_chip_init();

        return 0;

    }
    else
    {
        printf("%s <dol/dcl/scl-bias/scl-mod> \n", argv[0]);        
        return -1;
    }

    return 0;    

}


static int europa_cli_reset(
    int argc,
    char *argv[],
    FILE *fp)

{
    int ret;
    uint32 devId = 5;    
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    { 
        ret = rtl8290c_cli_reset(devId, argc, argv, fp);
        return ret;
    }
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    { 
        ret = rtl8291_cli_reset(devId, argc, argv, fp);
        return ret;
    }    
    else  if(EUROPA_LDD_8290B_MODE == chip_mode)
    {
    if(argc < 2)
    { 
        printf("%s <chip/digital/8051/analog> \n", argv[0]);
        return -1;
    }

    if(0 == strcmp(argv[1], "chip"))
    {
        /*Europa Reset Total Chip*/
        printf("Reset Total Chip\n");
        ret = rtk_ldd_reset(LDD_RESET_CHIP);
    }
    else if(0 == strcmp(argv[1], "digital"))
    {
        /*Europa Reset Digital*/
        printf("Reset Digital\n");
        ret = rtk_ldd_reset(LDD_RESET_DIGITAL)  ;
    }
    else if(0 == strcmp(argv[1], "8051"))
    {
        /*Europa Reset 8051*/
        printf("Reset 8051\n");
        ret = rtk_ldd_reset(LDD_RESET_8051)  ;
    }
    else if(0 == strcmp(argv[1], "analog"))
    {
        /*Europa Reset Analog*/
        printf("Reset Analog\n");
        ret = rtk_ldd_reset(LDD_RESET_ANALOG)  ;
    }
    else
    {
        printf("no such mode\n");
        return -1;
    }
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }

    return 0;
}    /* end of cparser_cmd_debug_europa_reset_chip_digital_8051_analog */

// * debug europa set apdlut ( factor | offset ) <UINT:rfb1> <UINT:rfb2> <UINT:vbr> <UINT:temp> <UINT:value> <UINT:c1> <UINT:c2>
//cparser_result_t
//cparser_cmd_debug_europa_set_apdlut_factor_offset_rfb1_rfb2_vbr_temp_value_c1_c2(
//    cparser_context_t *context,
//    uint32_t  *rfb1_ptr,
//    uint32_t  *rfb2_ptr,
//    uint32_t  *vbr_ptr,
//    uint32_t  *temp_ptr,
//    uint32_t  *value_ptr,
//    uint32_t  *c1_ptr,
//    uint32_t  *c2_ptr)
static int europa_cli_apdlut_set(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret = 0;
    uint8 lut[151];
    uint32 factor, offset;
    uint32 vbr;
    uint32 dac, dac2;
    uint32 c1, c2;
    int32 pos, pos2, i, j, num;
    uint32 slope;
    uint8 *init_data;
    uint32 rfb1, rfb2;
    int64 vapd;
    int32 dac_int;
    uint32 v;
    int temp;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8291_MODE == chip_mode)
    {  
        ret = rtl8291_cli_apdlut_set(devId, argc, argv, fp);
        return ret;
    }

    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if((argc < 9)&&(argc !=2))
    {
        printf("%s factor/offset <UINT:rfb1> <UINT:rfb2> <UINT:vbr> <INT:temp> <UINT:value> <UINT:c1> <UINT:c2> \n", argv[0]);
        printf("%s enable/disable\n", argv[0]); 
        return -1;
    }

    if(0 == strcmp(argv[1], "enable"))
    {
        rtk_ldd_calibration_state_set(ENABLED);
        return 0;        
    }
    else if(0 == strcmp(argv[1], "disable"))
    {
        rtk_ldd_calibration_state_set(DISABLED);
        return 0;        
    }

    rfb1  = _vlaue_translate(argv[2]);
    rfb2  = _vlaue_translate(argv[3]);
    vbr   = _vlaue_translate(argv[4]);        
    temp  = _vlaue_translate(argv[5]);
    if ((temp<-40)||(temp>110))
    {
        printf("Input temp Error!!! temp should be -40 ~ 110C.\n");
        return -1;
    }
    c1    = _vlaue_translate(argv[7]);
    c2    = _vlaue_translate(argv[8]);        

    if(0 == strcmp(argv[1], "factor"))
    {
        factor = _vlaue_translate(argv[6]);
        //CID166109
        vapd = (int64)vbr*factor/1000;
    }
    else if(0 == strcmp(argv[1], "offset"))
    {
        offset = _vlaue_translate(argv[6]);
        vapd = vbr-offset; /* offset is in mV also */
    }
    else
    {
        printf("no such mode %s\n", argv[1]);
        return -1;
    }


    //dac = (vapd -27600000)/156250;
    //Vout  = 1200 + 1200 * (Rfb1/Rfb2) + Rfb1 * 12 + (Rfb1 * DAC_CODE * 78125) / 100000
    dac_int = (vapd - 1200 - 1200*rfb1/rfb2 - rfb1*12)*100000/78125/rfb1;

    if ((dac_int>255)||(dac_int<0))
    {
        printf("DAC over 255!!!\n");
        return -1;
    }
    dac = dac_int;
    pos = temp + 40;
    if ((pos>150)||(pos<0))
    {
        printf("pos over flow!!!\n");
        return -1;
    }
    lut[pos] = dac;

    //For -40~temp
    if (pos>0)
    {
    pos2 = pos -1;
    slope= c1;
    while(pos2>=0)
    {
        //dac2 = dac - (pos-pos2)*slope/1000;
        v = _europa_cal_booster_Dac2Vout(dac, rfb1, rfb2);
    v = v - (pos-pos2)*slope;
        dac2 = _europa_cal_booster_Vout2Dac(v, rfb1, rfb2);

        if (dac2>0)
        {
           lut[pos2] = dac2;
        }
        else
        {
           lut[pos2] = 0;
        }
     pos2--;
    }
    }

    //For temp~110
    if (pos<150)
    {
    pos2 = pos + 1;
    slope= c2;
    while(pos2<150)
    {
        //dac2 = dac + (pos2-pos)*slope/1000;
        v = _europa_cal_booster_Dac2Vout(dac, rfb1, rfb2);
    v = v + (pos2-pos)*slope;
        dac2 = _europa_cal_booster_Vout2Dac(v, rfb1, rfb2);

        if (dac2>255)
        {
           lut[pos2] = 255;
        }
        else
        {
           lut[pos2] = dac2;
        }
     pos2++;
    }
    }

    printf("Temperature  &  DAC value\n ");

    for (i=0;i<10;i++)
    {
        if (i<9)
    {
            num =16;
        }
        else
        {
            num = 7;
        }

        printf("Temperature: ");
        for(j=0;j<num;j++)
        {
            printf("%4d ", j+i*16-40);
        }
        printf("\n ");
        printf("DAC:          ");
        for(j=0;j<num;j++)
        {
            printf("0x%02x ", lut[j+i*16]);
        }
        printf("\n\n ");
    }

    _europa_cal_flash_array_set(RTL8290B_APD_LUT_ADDR, 151, lut);

    return 0;
}


static int europa_cli_rssi_resistor_set(
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value;      
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if(argc < 3)
    { 
        printf("%s <rssi r1> <rssi r2> \n", argv[0]);        
        return -1;
    }

    printf("Set RSSI Resistor:\n"); 
    
    value = _vlaue_translate(argv[1]);   
    addr = RTL8290B_RX_RSSI_R1_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
    _europa_cal_flash_data_set(addr, 2, value);
    printf("Set R1 = %d\n", value);     
       
    value = _vlaue_translate(argv[2]);   
    addr = RTL8290B_RX_RSSI_R2_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
    _europa_cal_flash_data_set(addr, 2, value);
    printf("Set R2 = %d\n", value);   
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_rssi_resistor_set Duration = %f ms\n", Duration);
#endif
    
    return 0; 
}

static int europa_cli_rssi_resistor_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value;  
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }
    
    printf("Get RSSI Resistor:\n");     
  
    addr = RTL8290B_RX_RSSI_R1_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
    _europa_cal_flash_data_get(addr, 2, &value);
    printf("Get R1 = %d\n", value);  

    addr = RTL8290B_RX_RSSI_R2_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
    _europa_cal_flash_data_get(addr, 2, &value);
    printf("Get R2 = %d\n", value);  
    
    return 0; 
}


/*
 * europacli set booster <UINT:Vout> <UINT:Rfb1> <UINT:Rfb2>
 */
static int europa_cli_booster_set(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret = 0;
    uint32 Vout;
    uint32 Rfb1;
    uint32 Rfb2;
    uint32 dac_code;

    uint8 dac_reg;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8291_MODE == chip_mode)
    {  
        ret = rtl8291_cli_booster_set(devId, argc, argv, fp);
        return ret;
    }

    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if((argc < 4)&&(argc != 2))
    { 
        printf("%s <UINT:Vout> <UINT:Rfb1> <UINT:Rfb2> \n", argv[0]);        
        return -1;
    }

    printf("Set booster:\n"); 
    
    Vout = _vlaue_translate(argv[1]);     /* mV */   
    if (30000>Vout || 80000<Vout)
    {
        printf("Not support voltage %d(mV)\n", Vout);
        return -1;
    }
    
    if (argc == 2)
    {
        Rfb1 = 200;
        Rfb2 = 10;        
    }
    else
    {
        Rfb1 = _vlaue_translate(argv[2]);    
        Rfb2 = _vlaue_translate(argv[3]);
    }

    dac_code = _europa_cal_booster_Vout2Dac(Vout, Rfb1, Rfb2);

    if (0xff<dac_code)
    {
        printf("Booster DAC out-of-range\n");
        return 0;
    }
    else
    {
        dac_reg = (uint8) dac_code;
    }

    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x64, dac_reg);  

    printf("Booster DAC set 0x%02x (Vout=%d mV, Rfb1 = %d, Rfb2 = %d)\n", dac_code, Vout, Rfb1, Rfb2);
    
    return 0;
}    /* end of europa set booster*/


/*
 * debug europa get booster <UINT:Rfb1> <UINT:Rfb2>
 */
static int europa_cli_booster_get(
    int argc,
    char *argv[],
    FILE *fp)

{
    int ret = 0;
    uint32 Vout;
    uint32 Rfb1;
    uint32 Rfb2;

    uint32 dac_reg;
    int32 value1, value2;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8291_MODE == chip_mode)
    {  
        ret = rtl8291_cli_booster_get(devId, argc, argv, fp);
        return ret;
    }
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if((argc < 3)&&(argc != 1))
    { 
        printf("%s <UINT:Rfb1> <UINT:Rfb2> \n", argv[0]);        
        return -1;
    }

    if(argc ==1)
    {
        Rfb1 = 200;
        Rfb2 = 10;
    }
    else
    {
        value1 = _vlaue_translate(argv[1]);   
        value2 = _vlaue_translate(argv[2]);
        if ((value1<0)||(value2<=0))
        {
            printf("%s <UINT:Rfb1> <UINT:Rfb2> should > 0 \n", argv[0]);
            return -1;
        }
        Rfb1 = (uint32)value1;   
        Rfb2 = (uint32)value2; 
    }

    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x64, &dac_reg);  
    
    Vout = _europa_cal_booster_Dac2Vout(dac_reg, Rfb1, Rfb2);

    printf("Booster DAC is 0x%02x (Vout=%d mV, Rfb1 = %d , Rfb2 = %d)\n", dac_reg, Vout, Rfb1, Rfb2);

    return 0;
}    /* end of europa_cli_booster_get */



static int europa_cli_apcdig_get(
    int argc,
    char *argv[],
    FILE *fp)

{
    uint32 Vout;
    uint32 w77_data;
    uint32 loopcnt;
    uint32 data1, data2;
    uint32 i;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if(argc < 3)
    { 
        printf("%s <W77 value> <loop count> \n", argv[0]);        
        return -1;
    }


    w77_data = _vlaue_translate(argv[1]);   
    loopcnt = _vlaue_translate(argv[2]);

    if (w77_data > 0xFF)
    {
        printf("W77 = 0x%x, Out of Range!!!! \n", w77_data);        
        return -1;
    }

    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 14, &data1);  
    
    if ((data1&0x80) != 0)
    {
        printf("Warning!!!  A4/14 bit 7 = 1, \n");        
    }

    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x4D, w77_data);  
    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x4D, &data1);      
    printf("Write W77 to 0x%x, W77 is 0x%x.... \n", w77_data, data1); 

    printf("Read R29 for %d: \n", loopcnt);
       
    for (i=0;i<loopcnt;i++)
    {
        europa_i2c_read(EUROPA_I2C_PORT, 0x55, 0x1D, &data2);

        printf(" 0x%2x ", data2);
        
        if(i%8 == 7)
            printf("   ");
        if(i%16 == 15)
            printf("\n"); 

    } 
    
    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x4D, &data1);   
    printf("Read R29 finish. W77 = 0x%2x\n", data1);   

    return 0;
} 

static int europa_cli_shotBurst_set(
    int argc,
    char *argv[],
    FILE *fp)
{
    int32 ret = -1;
    uint8  laser_data[16];    
    uint32 l_delay, gap_delay, loop_num, log_num;
    uint8 *ptr_data;
    uint32 w61_data, w62_data;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }
    
    if(argc < 4)
    {      
        printf("%s <laser delay: ns> <gap delay: ns> <loop num> <log gap>\n", argv[0]);          
        return -1;
    }

    l_delay = _vlaue_translate(argv[1]);    
    //printf("Laser ON/OFF loop start: cnt = %d\n", cnt);  
    laser_data[0] = (l_delay&0xFF);
    laser_data[1] = ((l_delay&0xFF00)>>8);
    laser_data[2] = ((l_delay&0xFF0000)>>16);
    laser_data[3] = ((l_delay&0xFF000000)>>24);
    
    gap_delay = _vlaue_translate(argv[2]);
    laser_data[4] = (gap_delay&0xFF);
    laser_data[5] = ((gap_delay&0xFF00)>>8);
    laser_data[6] = ((gap_delay&0xFF0000)>>16);
    laser_data[7] = ((gap_delay&0xFF000000)>>24);
    
    loop_num = _vlaue_translate(argv[3]);     
    //printf("Laser ON/OFF loop start: cnt = %d\n", cnt);  
    laser_data[8] = (loop_num&0xFF);
    laser_data[9] = ((loop_num&0xFF00)>>8);
    laser_data[10] = ((loop_num&0xFF0000)>>16);
    laser_data[11] = ((loop_num&0xFF000000)>>24);
    
    log_num = _vlaue_translate(argv[4]);
    laser_data[12] = (log_num&0xFF);
    laser_data[13] = ((log_num&0xFF00)>>8);
    laser_data[14] = ((log_num&0xFF0000)>>16);
    laser_data[15] = ((log_num&0xFF000000)>>24);

    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 61, &w61_data);   
    printf("W61 = 0x%2x\n", w61_data); 

    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 62, &w62_data);   
    printf("W62 = 0x%2x\n", w62_data); 


    system("diag gpon set force-prbs prbs23");
    osal_time_mdelay(1000);
    system("diag gpon set force-prbs off");
    _europa_ibim_info(0);
    
    osal_time_mdelay(1000);

    ptr_data = laser_data; 
    ret = rtk_ldd_api_test(ptr_data);
    if(ret)
    {
        printf("rtk_ldd_api_test fail!!! (%d)\n", ret);
        return -1;
    }

    return 0;    
}

static int europa_cli_sdadc_code_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 w83_data;
    uint32 loopcnt1, loopcnt2, i;
    europa_rxparam_t ldd_rxparam;
    int32 mode;
    int32 rssi_k_2;
    uint32 rssi_v0_2;
    int32 addr;
    int32 rssi_mode;
    uint32 devId = 5;    
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290B_MODE == chip_mode)
    {    
        if(argc < 5)
        { 
            printf("%s <mode: 1 for all, 0 for only average> <rssi voltage mode> <loopcnt 1> <loopcnt2>\n", argv[0]);
            return -1;
        }
    
        //rssi_mode == 0: Normal mode, output one rssi voltage (Can be changed by SD suggestion)
        //rssi_mode == 1: Use Old algorithm(from rtl8290
        //rssi_mode == 2: Use New algorithm(with RSSI-K)
        //rssi_mode == 3: use  R17~R19 to replace original RSSI Code, other's like New algorithm(with RSSI-K)
        //rssi_mode == 4: use  R17~R19 to replace original RSSI Code, other's like Old algorithm(from rtl8290
    
        //rssi_mode == 0xF0: debug mode, dump all    
        //rssi_mode == 0xF1: Use Old algorithm(from rtl8290) and New algorithm, rssi_voltage is old, rssi_voltage2 is new
        //rssi_mode == 0xF2: list  R17~R19 and original RSSI Code, rssi voltage to compare (RSSI V0 also included)
    
        mode     = _vlaue_translate(argv[1]);
        rssi_mode = _vlaue_translate(argv[2]);    
        loopcnt1  = _vlaue_translate(argv[3]);
        loopcnt2  = _vlaue_translate(argv[4]);
    
        memset(&ldd_rxparam, 0, sizeof(europa_rxparam_t));
        _europa_rx_parameter_get(&ldd_rxparam);
        ldd_rxparam.loopcnt = loopcnt1;
        ldd_rxparam.mode = rssi_mode;
        ldd_rxparam.message = mode;
    
        addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_RSSI_V0_2_OFFSET_ADDR;
        _europa_cal_flash_data_get(addr, 4, &rssi_v0_2);
        addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_RSSI_K2_OFFSET_ADDR;    
        _europa_cal_flash_data_get(addr, 4, &rssi_k_2);
        ldd_rxparam.rssi_v0_2 = rssi_v0_2;
        ldd_rxparam.rssi_k_2 = rssi_k_2;
    
        printf("Current RX power RSSI V0 2 = %d\n", rssi_v0_2);
        printf("Current RX power RSSI-K 2 = %d\n", rssi_k_2); 
    
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);    
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);
       
        //if ((ldd_rxparam.mode==0) || (ldd_rxparam.mode==1)) 
        //    printf("Parameter list: \nrssi_code gnd_code vdd_code ldo_code half_vdd_code rset_code rssi_v rssi_v2 rssi_i rssi_i2 rssi_code2\n");
        //else if (ldd_rxparam.mode==3) 
        //    printf("Parameter list: \nrssi_code rssi_code2 gnd_code vdd_code half_vdd_code rssi_v rssi_v2 rssi_i rssi_i2\n"); 
        if ((ldd_rxparam.mode == 0xF4)||(ldd_rxparam.mode == 0xF5))
        {
            printf("Parameter list: \nrssi_code gnd_code vdd_code ldo_code half_vdd_code rssi_v1 rssi_v2 rssi_v3 rssi_i rssi_i2 rssi_i3\n");
        }
        else
        {
            printf("Parameter list: \nrssi_code rssi_code2 gnd_code vdd_code ldo_code half_vdd_code rset_code rssi_v rssi_v2 rssi_i rssi_i2 rssi_code2\n");
        }
        osal_time_mdelay(500); 
        
        for (i=0;i<loopcnt2;i++)
        {
            _europa_rx_test_get(&ldd_rxparam);
        }   
    
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
    }
    else if(EUROPA_LDD_8290C_MODE == chip_mode)
    {
        uint32 sdadc_type=0, ext_type=0;
        uint32 sdadc_code=0;
        int ret=0;
        int tmp_cnt=0;
        const char* SdadcStr[18] = {
            "CODE_TEM",
            "CODE_CALV",
            "CODE_PS",
            "CODE_TXAPC",
            "CODE_LDO",
            "CODE_RSSIINP_SEL",
            "CODE_RSSIINP",
            "CODE_RSSIINN_SEL",
            "CODE_RSSIINN",
            "CODE_RSSIFL_SEL",
            "CODE_RSSIFL",
            "CODE_VLASERN",
            "CODE_ADCIN",
            "CODE_PS_TX",
            "CODE_PS_RX",
            "CODE_VBG",
            "CODE_CALG", 
            "CODE_TXAPC_RSEL"
            };
    
        if(argc < 3)
        { 
            printf("%s <type: all/tem/calv/ps/rssiinp/rssiinn/rssifl/vlasern/adcin/pstx/psrx/vbg/calg/txapc> <loopcnt>\n", argv[0]);
            return -1;
        }

        //CID960622
        tmp_cnt = _vlaue_translate(argv[2]);
        if (tmp_cnt<0)
        {
            printf("Inpurt loopcnt error!!!\n");
            return -1;
        }
        else
        {
            loopcnt1 = (uint32)tmp_cnt;
        }

        if(0 == strcmp(argv[1], "all"))
        {
            printf("Parameter list: \ntem calv ps txpac ldo rssiinp-sel rssiinp rssiinn-sel rssiinn rssifl-sel rssifl vlasern adcin ps_tx ps_rx vbg calg txapc\n");

            for (i=0;i<loopcnt1;i++)
            {
                printf("\n");            
                for (sdadc_type=RTL8290C_CODE_TEM;sdadc_type<RTL8290C_SDADC_CODE_END;sdadc_type++)
                {
                    ret = rtk_ldd_sdadc_code2_get(devId, sdadc_type, &sdadc_code);
                    if (ret!=0)
                    {
                        printf(" rtk_ldd_sdadc_code2_get ERROR!!!!, ret = %d", ret);
                        return -1;
                    }
                    printf("  %8d", sdadc_code);                     
                }
            }
        }
        else
        {
            if(0 == strcmp(argv[1], "tem"))
            {
                sdadc_type=RTL8290C_CODE_TEM;            
            }
            else if(0 == strcmp(argv[1], "calv"))
            {
                sdadc_type=RTL8290C_CODE_CALV;            
            }
            else if(0 == strcmp(argv[1], "ps"))
            {
                sdadc_type=RTL8290C_CODE_PS;            
            }            
            else if(0 == strcmp(argv[1], "rssiinp"))
            {
                sdadc_type=RTL8290C_CODE_RSSIINP_SEL;            
            }
            else if(0 == strcmp(argv[1], "rssiinn"))
            {
                sdadc_type=RTL8290C_CODE_RSSIINN_SEL;            
            }
            else if(0 == strcmp(argv[1], "rssifl"))
            {
                sdadc_type=RTL8290C_CODE_RSSIFL_SEL;            
            }
            else if(0 == strcmp(argv[1], "vlasern"))
            {
                sdadc_type=RTL8290C_CODE_VLASERN;            
            }
            else if(0 == strcmp(argv[1], "adcin"))
            {
                sdadc_type=RTL8290C_CODE_ADCIN;            
            }
            else if(0 == strcmp(argv[1], "pstx"))
            {
                sdadc_type=RTL8290C_CODE_PS_TX;            
            }
            else if(0 == strcmp(argv[1], "psrx"))
            {
                sdadc_type=RTL8290C_CODE_PS_RX;            
            }
            else if(0 == strcmp(argv[1], "vbg"))
            {
                sdadc_type=RTL8290C_CODE_VBG;            
            }
            else if(0 == strcmp(argv[1], "ldo"))
            {
                sdadc_type=RTL8290C_CODE_LDO;            
            }
            else if(0 == strcmp(argv[1], "calg"))
            {
                sdadc_type=RTL8290C_CODE_CALG;            
            }
            else if(0 == strcmp(argv[1], "txapc"))
            {
                sdadc_type=RTL8290C_CODE_TXAPC;            
            }
            else 
            {
                printf("No such type!!!\n");
                printf("%s <type: all/tem/calv/ps/rssiinp/rssiinn/rssifl/vlasern/adcin/pstx/psrx/vbg/calg/txapc> <loopcnt>\n", argv[0]);
                return -1;
            }

            printf("Parameter list:%s (%d)\n", argv[1], sdadc_type);

            for (i=0;i<loopcnt1;i++)
            {
                printf("\n");            
                ret = rtk_ldd_sdadc_code2_get(devId, sdadc_type, &sdadc_code);
                if (ret!=0)
                {
                    printf(" rtk_ldd_sdadc_code2_get ERROR!!!!, ret = %d", ret);
                    return -1;
                }                
                printf("%s: %8d    ", SdadcStr[sdadc_type], sdadc_code); 
                if ((sdadc_type==RTL8290C_CODE_RSSIFL_SEL)||(sdadc_type==RTL8290C_CODE_RSSIINP_SEL)||(sdadc_type==RTL8290C_CODE_RSSIINN_SEL))
                {
                    ext_type = sdadc_type+1;
                    rtk_ldd_sdadc_code2_get(devId, ext_type, &sdadc_code);
                    printf("%s: %8d",SdadcStr[ext_type], sdadc_code); 
                }
                else if (sdadc_type==RTL8290C_CODE_TXAPC)
                {
                    sdadc_code = 0;
                    ext_type = RTL8290C_CODE_TXAPC_RSEL;
                    ret = rtk_ldd_sdadc_code2_get(devId, ext_type, &sdadc_code);
                    if (ret!=0)
                    {
                        printf(" rtk_ldd_sdadc_code2_get ERROR!!!!, ret = %d", ret);
                        return -1;
                    } 
                    printf("%s:, %8d", SdadcStr[ext_type], sdadc_code); 
                }                
            }            

        } 
    }
    else 
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;
    }

    printf("\nSDADC CODE FINISH");

    return 0;
} 

static int europa_cli_txinfo_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int32 ret = -1;
    int i, loopcnt;        
    uint32 reload = 0;
    uint32 bias;
    uint32 mod;
    uint16 tempK;
    int32 tempC;
    uint32 data, addr, exec_cnt;
    int8 offset;
    double ib, im;
    rtk_gpon_fsm_status_t state; 
    rtk_ldd_cfg_t ldd_cfg;    
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if(argc < 3)
    { 
        //printf("%s <loop cnt> <delay1: ms> <delay2: ms> \n", argv[0]);        
        printf("%s <1:reload europa drv; 0: no reload> <loop cnt>\n", argv[0]);        
        return -1;
    }

    reload = _vlaue_translate(argv[1]);
   
    loopcnt = _vlaue_translate(argv[2]);
    
    if(loopcnt<0)
    {
        printf("loopcnt should > 0\n");
        return -1;
    }
    
    if (reload !=0)
    {
        system("rmmod europa_drv");
        osal_time_mdelay(500); 
        system("insert_europa.sh");    
    }
    
    _europa_cal_flash_data_get(RTL8290B_TEMP_OFFSET_ADDR, 1, &data);   
    offset = (int8)(data);        
       
    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_IBIM_WORKAROUND_OFFSET_ADDR;
    _europa_cal_flash_data_get(addr, 4, &data); 
    printf("TX Patch  = 0x%x\n", data);

    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_CAL_IBIAS_OFFSET_ADDR;
    _europa_cal_flash_data_get(addr, 4, &data); 
    printf("Cal IBIAS  = 0x%x\n", data);

#if 0
    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_CAL_IMOD_OFFSET_ADDR;
    _europa_cal_flash_data_get(addr, 4, &data); 
    printf("Cal IMOD  = 0x%x\n", data);

    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_CAL_EPON_IBIAS_OFFSET_ADDR;
    _europa_cal_flash_data_get(addr, 4, &data); 
    printf("Cal EPON IBIAS  = 0x%x\n", data);

    addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_CAL_EPON_IMOD_OFFSET_ADDR;
    _europa_cal_flash_data_get(addr, 4, &data); 
    printf("Cal EPON IMOD  = 0x%x\n", data);       
#endif 

    for (i=0;i<loopcnt;i++)
    {
            bias = 0;
            ret = rtk_ldd_tx_bias_get(&bias);
            if(ret)
            {
                printf("Get TX Bias Fail!!! (%d)\n", ret);
                return -1;
            }
            ib = ((double)bias*2)/1000;  /* 2uA to mA */
    
            mod = 0;
            ret = rtk_ldd_tx_mod_get(&mod);
            if(ret)
            {
                printf("Get TX Mod Fail!!! (%d)\n", ret);
                return -1;
            }
            im = ((double)mod*2)/1000;  /* 2uA to mA */
    
            tempK = 0;
            ret = rtk_ldd_temperature_get(&tempK);
            if(ret)
            {
                printf("Get Temperature Fail!!! (%d)\n", ret);
                return -1;
            }
            tempC = (tempK*100-27315)/100; /* degK to degC */
            tempC = tempC - offset;

            ret = rtk_gpon_ponStatus_get(&state);
            if(ret)
            {
                printf("Get PON Status Fail!!! (%d)\n", ret);
                //return -1;
            }
            
            memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
            ldd_cfg.rssi_v0 = 0x8290D;
            ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
            exec_cnt = ldd_cfg.gnd_code;
            
            printf("\n[%8d] ONU[O%d] TempC[%4d] Bias=%2.2f Modulation=%2.2f\n", exec_cnt, state, tempC, ib, im);

            osal_time_mdelay(1000); 

    }

    
    return 0; 
}


static void _europa_rssiv0_get(europa_rxpwr_t *p_rx_arr, uint32 *pValue_v0, int32 *pValue_k)
{
    int ret;
    uint32 rssi_v0, rssi_gnd, rssi_vdd, rssi_half0, rssi_half1, rssi_v0_ori;
    uint32 loopcnt, i;
    uint64 sum, sum_gnd, sum_vdd, sum_half, sum_rssi, sum_ori;
    uint32 index;
    rtk_ldd_cfg_t ldd_cfg;
    rtk_ldd_loop_mode_t mode;
    uint32 Imod, ori_Imod;
    uint32 rssi_code1, rssi_code0;
    double temp1, temp2, temp3;
    int32 temp32, rssi_k;
    int32 rx_power_mode;
    int32 offset, diff;
    uint32 w80_data;
    uint32 rssi_gnd0, rssi_vdd0, rssi_gnd1, rssi_vdd1;


    offset = 10;
    loopcnt = 50;        
    //p_rx_arr = (europa_rxpwr_t *)malloc(loopcnt+offset);    
    memset(p_rx_arr, 0,sizeof(europa_rxpwr_t)*(loopcnt+offset));

    //Step1. set prbs off
    //printf("RSSI V0: PRBS close\n");
    //if ((ret = reg_write(0xBB040098, 0)) != 0)/*disable PRBS*/ 
    //{
    //    return ret;
    //}
    
    //Step2. disable TEMP_INT (A4/89=0xFF), set DOL loop
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 89, 0xFF);   
    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x7C, &w80_data);        
    mode = LDD_LOOP_DOL;
    ret = rtk_ldd_loopMode_set(mode);
    if(ret)
    {
        printf("Set Loop Mode Fail!!! (%d)\n", ret);
    }  
    
    //Step3. set Imod=5mA 
    ret = rtk_ldd_tx_mod_get(&ori_Imod);
    if(ret)
    {
        printf("rtk_ldd_tx_mod_get Fail!!! (%d)\n", ret);
    }
        
    Imod = (5 * 1000 / 2); /* convert mA to 2uA unit */
    ret = rtk_ldd_tx_mod_set(Imod);
    if(ret)
    {
        printf("Set TX Mod = %d Fail!!! (%d)\n", Imod, ret);
    }
    osal_time_mdelay(500); 
    
    //Step4. get Crssi0, Chalf_vdd0, save Cgnd0, Cvdd0, calculate RSSIV0    
    for (i=0;i<(loopcnt+offset);i++)
    {       
        memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
        ldd_cfg.state = 0;
        ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
        if(ret)
        {
            printf("rtk_ldd_rssiVoltage_get fail!!! (%d)\n", ret);             
            return;
        }
        p_rx_arr[i].rssi_i = ldd_cfg.rssi_voltage;            
        p_rx_arr[i].rssi_v = ldd_cfg.rssi_voltage2;
        p_rx_arr[i].rssi_code = ldd_cfg.rssi_code;            
        p_rx_arr[i].gnd_code = ldd_cfg.gnd_code;
        p_rx_arr[i].vdd_code = ldd_cfg.vdd_code;  
        p_rx_arr[i].half_vdd_code = ldd_cfg.half_vdd_code;            
    }

    _europa_rxbubble_sort(p_rx_arr, loopcnt+offset);
    
    rssi_v0 = 0;
    rssi_gnd = 0;
    rssi_vdd = 0;
    rssi_half0 = 0;
    rssi_code0 = 0;

    sum_ori = 0;
    sum = 0;
    sum_gnd = 0;
    sum_vdd = 0;
    sum_half = 0;  
    sum_rssi = 0;
    
    //printf("Parameter list:\nrssi_code gnd_code vdd_code half_vdd_code rssi_voltage\n");
         
    for (i=5;i<loopcnt+5;i++)
    {   
        sum_ori  += p_rx_arr[i].rssi_i;
        sum      += p_rx_arr[i].rssi_v;
        sum_rssi += p_rx_arr[i].rssi_code;
        sum_gnd  += p_rx_arr[i].gnd_code;
        sum_vdd  += p_rx_arr[i].vdd_code;    
        sum_half += p_rx_arr[i].half_vdd_code;        
            //printf("Loop %d: rssi V0 = 0x%x \n", i, p_rx_arr[i].rssi_v);
    }
    rssi_v0_ori = sum_ori/loopcnt;
    rssi_v0 = sum/loopcnt;
    rssi_code0 = sum_rssi/loopcnt;
    rssi_gnd = sum_gnd/loopcnt;
    rssi_vdd = sum_vdd/loopcnt;
    rssi_half0 = sum_half/loopcnt;     
    rssi_gnd0 = rssi_gnd;
    rssi_vdd0 = rssi_vdd;
    //printf("%8d    %8d    %8d    %8d\n", rssi_gnd, rssi_vdd, rssi_code0, rssi_half0);
    
#ifdef EUROPA_DEBUG_RXPOWER
    printf("Parameter list:\nrssi_voltage gnd_code vdd_code half_vdd_code diff \n");
    for (i=0;i<5;i++)
    {
        diff = abs((int32)p_rx_arr[i].rssi_v-(int32)rssi_v0);
        printf("[%d]: %d 0x%x 0x%x 0x%x diff = %d\n", i, p_rx_arr[i].rssi_v, p_rx_arr[i].gnd_code, p_rx_arr[i].vdd_code, p_rx_arr[i].half_vdd_code, diff);        
        if (diff > (rssi_v0*2/10))
        {
            printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = %d\n", i, p_rx_arr[i].rssi_v, rssi_v0);
        }            
    }
        
    for (i=0;i<5;i++)
    {
        diff = abs((int32)p_rx_arr[i+loopcnt+5].rssi_v-(int32)rssi_v0);
        printf("[%d]: %d 0x%x 0x%x 0x%x diff = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, p_rx_arr[i+loopcnt+5].gnd_code, p_rx_arr[i+loopcnt+5].vdd_code, p_rx_arr[i+loopcnt+5].half_vdd_code, diff);
        if (diff > (rssi_v0*2/10))
        {
            printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, rssi_v0);
        }
    }
#endif
        

#ifdef EUROPA_DEBUG_RXPOWER    
    //printf("Get RSSI V0 = %d GND Code = 0x%x, VDD Code = 0x%x, RSSI Code0 = 0x%x, Half VDD Code0 = 0x%x\n", rssi_v0, rssi_gnd, rssi_vdd, rssi_code0, rssi_half0);
    printf("Get RSSI V0 = %d, Original RSSI V0 = %d\n", rssi_v0, rssi_v0_ori);
#else
    //printf("Get RSSI V0 = %d  \n  ", rssi_v0);
#endif

    *pValue_v0 = rssi_v0;

    
    //Step5. set Imod=100mA 
    //Step5. set Imod=50mA 
    Imod = (50 * 1000 / 2); /* convert mA to 2uA unit */
    ret = rtk_ldd_tx_mod_set(Imod);
    if(ret)
    {
        printf("Set TX Mod = %d Fail!!! (%d)\n", Imod, ret);
    }
    osal_time_mdelay(10); 

    for (i=0;i<(loopcnt+offset);i++)
    {        
        memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
        ldd_cfg.state = 0;
        ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
        if(ret)
        {
            printf("rtk_ldd_rssiVoltage_get fail!!! (%d)\n", ret);
            return;
        }
        p_rx_arr[i].rssi_i = ldd_cfg.rssi_voltage;            
        p_rx_arr[i].rssi_v = ldd_cfg.rssi_voltage2;
        p_rx_arr[i].rssi_code = ldd_cfg.rssi_code;            
        p_rx_arr[i].gnd_code = ldd_cfg.gnd_code;
        p_rx_arr[i].vdd_code = ldd_cfg.vdd_code;  
        p_rx_arr[i].half_vdd_code = ldd_cfg.half_vdd_code;            
    }

    _europa_rxbubble_sort(p_rx_arr, loopcnt+offset);

    rssi_v0 = 0;
    rssi_code1 = 0;    
    rssi_gnd = 0;
    rssi_vdd = 0;
    rssi_half1 = 0;
        
    sum = 0;
    sum_rssi = 0;
    sum_gnd = 0;
    sum_vdd = 0;
    sum_half = 0;
        
//printf("Parameter list:\nrssi_code gnd_code vdd_code half_vdd_code rssi_voltage\n");
    for (i=5;i<loopcnt+5;i++)
    {
        sum      += p_rx_arr[i].rssi_v;        
        sum_rssi += p_rx_arr[i].rssi_code;
        sum_gnd  += p_rx_arr[i].gnd_code;
        sum_vdd  += p_rx_arr[i].vdd_code;      
        sum_half += p_rx_arr[i].half_vdd_code;    
    } 

    rssi_v0 = sum/loopcnt;
    rssi_code1 = sum_rssi/loopcnt;
    rssi_gnd = sum_gnd/loopcnt;
    rssi_vdd = sum_vdd/loopcnt;
    rssi_half1 = sum_half/loopcnt; 
    rssi_gnd1 = rssi_gnd;
    rssi_vdd1 = rssi_vdd;    
    //printf("%8d    %8d    %8d    %8d\n", rssi_gnd, rssi_vdd, rssi_code1, rssi_half1);    
        
#ifdef EUROPA_DEBUG_RXPOWER    
        printf("Parameter list:\nrssi_voltage rssi_code gnd_code vdd_code half_vdd_code diff \n");
        for (i=0;i<5;i++)
        {
            diff = abs((int32)p_rx_arr[i].rssi_v-(int32)rssi_v0);
            printf("[%d]: %d 0x%x 0x%x 0x%x 0x%x diff = %d\n", i, p_rx_arr[i].rssi_v, p_rx_arr[i].rssi_code, p_rx_arr[i].gnd_code, p_rx_arr[i].vdd_code, p_rx_arr[i].half_vdd_code, diff);        
            if (diff > (rssi_v0*2/10))
            {
                printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = %d\n", i, p_rx_arr[i].rssi_v, rssi_v0);
            }            
        }
        
        for (i=0;i<5;i++)
        {
            diff = abs((int32)p_rx_arr[i+loopcnt+5].rssi_v-(int32)rssi_v0);
            printf("[%d]: %d 0x%x 0x%x 0x%x 0x%x diff = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, p_rx_arr[i+loopcnt+5].rssi_code, p_rx_arr[i+loopcnt+5].gnd_code, p_rx_arr[i+loopcnt+5].vdd_code, p_rx_arr[i+loopcnt+5].half_vdd_code, diff);
            if (diff > (rssi_v0*2/10))
            {
                printf("ERROR: p_rx_arr[%d].rssi_v = %d, AVG = = %d\n", i+loopcnt+5, p_rx_arr[i+loopcnt+5].rssi_v, rssi_v0);
            }
        }
#endif        
        
        //printf("Get RSSI Code1 = %d Half VDD Code1 = 0x%x\n", rssi_code1, rssi_half1);
    
        //Step7. calculate k = -1* (Crssi1 - Crssi0)/ (Chalf_vdd1 - Chalf_vdd0), save k   
        //The value of k can be minus or plus, and  is floating point, so multiple 1000. When used in RX power, divide 1000
        temp1 = -1*((double)rssi_code1 - (double)rssi_code0);
        //printf("temp1 = %f \n", temp1);    
        temp2 = (double)rssi_half1 - (double)rssi_half0;
        temp3 = temp1/temp2;
        rssi_k = (int)(temp3*1000);
        //printf("temp2 = %f temp3 = %f\n", temp2, temp3);
    
        //Step8. restore loop mode, TEMP_INT ((A4/89=0xFE)
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x7C, w80_data);    
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 89, 0xFE);
        ret = rtk_ldd_tx_mod_set(ori_Imod);
        if(ret)
        {
            printf("Set TX Mod = %d Fail!!! (%d)\n", ori_Imod, ret);
        }
                    
        printf("%8d   %8d  %8d   %8d  %8d   %8d  %8d   %8d  %8d   %8d\n", *pValue_v0, rssi_gnd0, rssi_gnd1, rssi_vdd0, rssi_vdd1, rssi_code0, rssi_code1, rssi_half0, rssi_half1, rssi_k);

        //printf("RSSI K (x1000) = %d \n", rssi_k);
         *pValue_k = rssi_k;   
        
    return;     
}

static void _europa_rssiv0_get2(uint32 loopcnt, uint32 *pValue_v0, int32 *pValue_k)
{
    int ret;
    uint32 rssi_v0, rssi_gnd, rssi_vdd, rssi_half0, rssi_half1, rssi_v0_ori;
    uint32 i;
    uint64 sum, sum_gnd, sum_vdd, sum_half, sum_rssi, sum_ori;
    uint32 index;
    rtk_ldd_cfg_t ldd_cfg;
    rtk_ldd_loop_mode_t mode;
    uint32 Imod, ori_Imod;
    uint32 rssi_code1, rssi_code0;
    double temp1, temp2, temp3;
    int32 temp32, rssi_k;
    int32 rx_power_mode;
    int32 offset, diff;
    uint32 w80_data;
    
    //Step2. disable TEMP_INT (A4/89=0xFF), set DOL loop
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 89, 0xFF);   
    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x7C, &w80_data);        
    mode = LDD_LOOP_DOL;
    ret = rtk_ldd_loopMode_set(mode);
    if(ret)
    {
        printf("Set Loop Mode Fail!!! (%d)\n", ret);
    }  
    
    //Step3. set Imod=5mA 
    ret = rtk_ldd_tx_mod_get(&ori_Imod);
    if(ret)
    {
        printf("rtk_ldd_tx_mod_get Fail!!! (%d)\n", ret);
    }
        
    Imod = (5 * 1000 / 2); /* convert mA to 2uA unit */
    ret = rtk_ldd_tx_mod_set(Imod);
    if(ret)
    {
        printf("Set TX Mod = %d Fail!!! (%d)\n", Imod, ret);
    }
    osal_time_mdelay(500); 
    
    //Step4. get Crssi0, Chalf_vdd0, save Cgnd0, Cvdd0, calculate RSSIV0    
    rssi_v0 = 0;
    rssi_gnd = 0;
    rssi_vdd = 0;
    rssi_half0 = 0;
    rssi_code0 = 0;

    sum_ori = 0;
    sum = 0;
    sum_gnd = 0;
    sum_vdd = 0;
    sum_half = 0;  
    sum_rssi = 0;
    for (i=0;i<loopcnt;i++)
    {       
        memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
        ldd_cfg.state = 0xF1;
        ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
        if(ret)
        {
            printf("rtk_ldd_rssiVoltage_get fail!!! (%d)\n", ret);             
            return;
        }
        sum_ori  += ldd_cfg.rssi_voltage;            
        sum      += ldd_cfg.rssi_voltage2;
        sum_rssi += ldd_cfg.rssi_code;            
        sum_gnd  += ldd_cfg.gnd_code;
        sum_vdd  += ldd_cfg.vdd_code;  
        sum_half += ldd_cfg.half_vdd_code;            
    }

    rssi_v0_ori = sum_ori/loopcnt;
    rssi_v0 = sum/loopcnt;
    rssi_code0 = sum_rssi/loopcnt;
    rssi_gnd = sum_gnd/loopcnt;
    rssi_vdd = sum_vdd/loopcnt;
    rssi_half0 = sum_half/loopcnt;  

    printf("Get RSSI V0 = %d GND Code = 0x%x, VDD Code = 0x%x, RSSI Code0 = 0x%x, Half VDD Code0 = 0x%x\n", rssi_v0, rssi_gnd, rssi_vdd, rssi_code0, rssi_half0);
   
    *pValue_v0 = rssi_v0;

    
    //Step5. set Imod=100mA 
    Imod = (100 * 1000 / 2); /* convert mA to 2uA unit */
    ret = rtk_ldd_tx_mod_set(Imod);
    if(ret)
    {
        printf("Set TX Mod = %d Fail!!! (%d)\n", Imod, ret);
    }
    osal_time_mdelay(10); 

    rssi_v0 = 0;
    rssi_code1 = 0;    
    rssi_gnd = 0;
    rssi_vdd = 0;
    rssi_half1 = 0;
        
    sum = 0;
    sum_rssi = 0;
    sum_gnd = 0;
    sum_vdd = 0;
    sum_half = 0;
    for (i=0;i<loopcnt;i++)
    {        
        memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
        ldd_cfg.state = 0xF1;
        ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
        if(ret)
        {
            printf("rtk_ldd_rssiVoltage_get fail!!! (%d)\n", ret);        
            return -1;
        }
        sum_ori  += ldd_cfg.rssi_voltage;            
        sum      += ldd_cfg.rssi_voltage2;
        sum_rssi += ldd_cfg.rssi_code;            
        sum_gnd  += ldd_cfg.gnd_code;
        sum_vdd  += ldd_cfg.vdd_code;  
        sum_half += ldd_cfg.half_vdd_code;            
    }
    
    rssi_v0 = sum/loopcnt;
    rssi_code1 = sum_rssi/loopcnt;
    rssi_gnd = sum_gnd/loopcnt;
    rssi_vdd = sum_vdd/loopcnt;
    rssi_half1 = sum_half/loopcnt; 

    printf("Get RSSI V0 = %d GND Code = 0x%x, VDD Code = 0x%x, RSSI Code1 = 0x%x, Half VDD Code1 = 0x%x\n", rssi_v0, rssi_gnd, rssi_vdd, rssi_code1, rssi_half1);    
              
        
        //printf("Get RSSI Code1 = %d Half VDD Code1 = 0x%x\n", rssi_code1, rssi_half1);
    
        //Step7. calculate k = -1* (Crssi1 - Crssi0)/ (Chalf_vdd1 - Chalf_vdd0), save k   
        //The value of k can be minus or plus, and  is floating point, so multiple 1000. When used in RX power, divide 1000
        temp1 = -1*((double)rssi_code1 - (double)rssi_code0);
        //printf("temp1 = %f \n", temp1);    
        temp2 = (double)rssi_half1 - (double)rssi_half0;
        temp3 = temp1/temp2;
        rssi_k = (int)(temp3*1000);
        //printf("temp2 = %f temp3 = %f\n", temp2, temp3);
    
        //Step8. restore loop mode, TEMP_INT ((A4/89=0xFE)
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x7C, w80_data);    
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 89, 0xFE);
        ret = rtk_ldd_tx_mod_set(ori_Imod);
        if(ret)
        {
            printf("Set TX Mod = %d Fail!!! (%d)\n", ori_Imod, ret);
        }
                    
        //printf("RSSI K (x1000) = %d \n", rssi_k);
         *pValue_k = rssi_k;   
        
    return;     
}

static void _europa_rssiv0_2_get(uint32 loopcnt, uint32 *pValue_v0, int32 *pValue_k)
{
    int ret;
    uint32 rssi_v0, rssi_gnd, rssi_vdd, rssi_half0, rssi_half1, rssi_v0_ori;
    uint32 i;
    uint64 sum, sum_gnd, sum_vdd, sum_half, sum_rssi, sum_ori;
    uint32 index;
    rtk_ldd_cfg_t ldd_cfg;
    rtk_ldd_loop_mode_t mode;
    uint32 Imod, ori_Imod;
    uint32 rssi_code1, rssi_code0;
    double temp1, temp2, temp3;
    int32 temp32, rssi_k;
    int32 rx_power_mode;
    int32 offset, diff;
    uint32 w80_data;
    
    //Step2. disable TEMP_INT (A4/89=0xFF), set DOL loop
    europa_i2c_write(EUROPA_I2C_PORT, 0x54, 89, 0xFF);   
    europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x7C, &w80_data);        
    mode = LDD_LOOP_DOL;
    ret = rtk_ldd_loopMode_set(mode);
    if(ret)
    {
        printf("Set Loop Mode Fail!!! (%d)\n", ret);
    }  
    
    //Step3. set Imod=5mA 
    ret = rtk_ldd_tx_mod_get(&ori_Imod);
    if(ret)
    {
        printf("rtk_ldd_tx_mod_get Fail!!! (%d)\n", ret);
    }
        
    Imod = (5 * 1000 / 2); /* convert mA to 2uA unit */
    ret = rtk_ldd_tx_mod_set(Imod);
    if(ret)
    {
        printf("Set TX Mod = %d Fail!!! (%d)\n", Imod, ret);
    }
    osal_time_mdelay(500); 
    
    //Step4. get Crssi0, Chalf_vdd0, save Cgnd0, Cvdd0, calculate RSSIV0    
    rssi_v0 = 0;
    rssi_gnd = 0;
    rssi_vdd = 0;
    rssi_half0 = 0;
    rssi_code0 = 0;

    sum_ori = 0;
    sum = 0;
    sum_gnd = 0;
    sum_vdd = 0;
    sum_half = 0;  
    sum_rssi = 0;
    for (i=0;i<loopcnt;i++)
    {       
        memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
        ldd_cfg.state = 0xF1;
        ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
        if(ret)
        {
            printf("rtk_ldd_rssiVoltage_get fail!!! (%d)\n", ret);             
            return;
        }
        sum_ori  += ldd_cfg.rssi_voltage;            
        sum      += ldd_cfg.rssi_voltage2;
        sum_rssi += ldd_cfg.rssi_code;            
        sum_gnd  += ldd_cfg.gnd_code;
        sum_vdd  += ldd_cfg.vdd_code;  
        sum_half += ldd_cfg.half_vdd_code;            
    }

    rssi_v0_ori = sum_ori/loopcnt;
    rssi_v0 = sum/loopcnt;
    rssi_code0 = sum_rssi/loopcnt;
    rssi_gnd = sum_gnd/loopcnt;
    rssi_vdd = sum_vdd/loopcnt;
    rssi_half0 = sum_half/loopcnt;  

    printf("Get RSSI V0 = %d GND Code = 0x%x, VDD Code = 0x%x, RSSI Code0 = 0x%x, Half VDD Code0 = 0x%x\n", rssi_v0, rssi_gnd, rssi_vdd, rssi_code0, rssi_half0);
   
    *pValue_v0 = rssi_v0;

    
    //Step5. set Imod=100mA 
    Imod = (100 * 1000 / 2); /* convert mA to 2uA unit */
    ret = rtk_ldd_tx_mod_set(Imod);
    if(ret)
    {
        printf("Set TX Mod = %d Fail!!! (%d)\n", Imod, ret);
    }
    osal_time_mdelay(10); 

    rssi_v0 = 0;
    rssi_code1 = 0;    
    rssi_gnd = 0;
    rssi_vdd = 0;
    rssi_half1 = 0;
        
    sum = 0;
    sum_rssi = 0;
    sum_gnd = 0;
    sum_vdd = 0;
    sum_half = 0;
    for (i=0;i<loopcnt;i++)
    {        
        memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
        ldd_cfg.state = 0xF1;
        ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
        if(ret)
        {
            printf("rtk_ldd_rssiVoltage_get fail!!! (%d)\n", ret);        
            return -1;
        }
        sum_ori  += ldd_cfg.rssi_voltage;            
        sum      += ldd_cfg.rssi_voltage2;
        sum_rssi += ldd_cfg.rssi_code;            
        sum_gnd  += ldd_cfg.gnd_code;
        sum_vdd  += ldd_cfg.vdd_code;  
        sum_half += ldd_cfg.half_vdd_code;            
    }
    
    rssi_v0 = sum/loopcnt;
    rssi_code1 = sum_rssi/loopcnt;
    rssi_gnd = sum_gnd/loopcnt;
    rssi_vdd = sum_vdd/loopcnt;
    rssi_half1 = sum_half/loopcnt; 

    printf("Get RSSI V0 = %d GND Code = 0x%x, VDD Code = 0x%x, RSSI Code1 = 0x%x, Half VDD Code1 = 0x%x\n", rssi_v0, rssi_gnd, rssi_vdd, rssi_code1, rssi_half1);    
              
        
        //printf("Get RSSI Code1 = %d Half VDD Code1 = 0x%x\n", rssi_code1, rssi_half1);
    
        //Step7. calculate k = -1* (Crssi1 - Crssi0)/ (Chalf_vdd1 - Chalf_vdd0), save k   
        //The value of k can be minus or plus, and  is floating point, so multiple 1000. When used in RX power, divide 1000
        temp1 = -1*((double)rssi_code1 - (double)rssi_code0);
        //printf("temp1 = %f \n", temp1);    
        temp2 = (double)rssi_half1 - (double)rssi_half0;
        temp3 = temp1/temp2;
        rssi_k = (int)(temp3*1000);
        //printf("temp2 = %f temp3 = %f\n", temp2, temp3);
    
        //Step8. restore loop mode, TEMP_INT ((A4/89=0xFE)
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x7C, w80_data);    
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 89, 0xFE);
        ret = rtk_ldd_tx_mod_set(ori_Imod);
        if(ret)
        {
            printf("Set TX Mod = %d Fail!!! (%d)\n", ori_Imod, ret);
        }
                    
        //printf("RSSI K (x1000) = %d \n", rssi_k);
         *pValue_k = rssi_k;   
        
    return;     
}

static void _europa_rx_mod_test(europa_rxpwr_t *p_rx_arr, uint32 input_mod, uint32 loopcnt)
{
    int ret;
    uint32 rssi_v0, rssi_gnd, rssi_vdd, rssi_half0, rssi_half1, rssi_v0_ori;
    //uint32 loopcnt, i;
    uint32 i;
    uint64 sum, sum_gnd, sum_vdd, sum_half, sum_rssi, sum_ori;
    uint32 index;
    rtk_ldd_cfg_t ldd_cfg;
    rtk_ldd_loop_mode_t mode;
    uint32 Imod, ori_Imod;
    uint32 rssi_code1, rssi_code0;
    double temp1, temp2, temp3;
    int32 temp32, rssi_k;
    int32 rx_power_mode;
    int32 offset, diff;
    uint32 w80_data;


    offset = 10;
    //loopcnt = 50;        
    //p_rx_arr = (europa_rxpwr_t *)malloc(loopcnt+offset);    
    memset(p_rx_arr, 0,sizeof(europa_rxpwr_t)*(loopcnt+offset));
    
    //Set Imod      
    Imod = (input_mod * 1000 / 2); /* convert mA to 2uA unit */
    ret = rtk_ldd_tx_mod_set(Imod);
    if(ret)
    {
        printf("Set TX Mod = %d Fail!!! (%d)\n", Imod, ret);
    }
    osal_time_mdelay(500); 
    
    //Step4. get Crssi0, Chalf_vdd0, save Cgnd0, Cvdd0, calculate RSSIV0    
    for (i=0;i<(loopcnt+offset);i++)
    {       
        memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
        ldd_cfg.state = 0xF1;
        ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
        if(ret)
        {
            printf("rtk_ldd_rssiVoltage_get fail!!! (%d)\n", ret);             
            return;
        }
        p_rx_arr[i].rssi_i = ldd_cfg.rssi_voltage;            
        p_rx_arr[i].rssi_v = ldd_cfg.rssi_voltage2;
        p_rx_arr[i].rssi_code = ldd_cfg.rssi_code;            
        p_rx_arr[i].gnd_code = ldd_cfg.gnd_code;
        p_rx_arr[i].vdd_code = ldd_cfg.vdd_code;  
        p_rx_arr[i].half_vdd_code = ldd_cfg.half_vdd_code;            
    }

    _europa_rxbubble_sort(p_rx_arr, loopcnt+offset);
    
    rssi_v0 = 0;
    rssi_gnd = 0;
    rssi_vdd = 0;
    rssi_half0 = 0;
    rssi_code0 = 0;

    sum_ori = 0;
    sum = 0;
    sum_gnd = 0;
    sum_vdd = 0;
    sum_half = 0;  
    sum_rssi = 0;
    
    //printf("Parameter list:\nrssi_code gnd_code vdd_code half_vdd_code rssi_voltage\n");
         
    for (i=5;i<loopcnt+5;i++)
    {   
        sum_ori  += p_rx_arr[i].rssi_i;
        sum      += p_rx_arr[i].rssi_v;
        sum_rssi += p_rx_arr[i].rssi_code;
        sum_gnd  += p_rx_arr[i].gnd_code;
        sum_vdd  += p_rx_arr[i].vdd_code;    
        sum_half += p_rx_arr[i].half_vdd_code;        
            //printf("Loop %d: rssi V0 = 0x%x \n", i, p_rx_arr[i].rssi_v);
    }
    rssi_v0_ori = sum_ori/loopcnt;
    rssi_v0 = sum/loopcnt;
    rssi_code0 = sum_rssi/loopcnt;
    rssi_gnd = sum_gnd/loopcnt;
    rssi_vdd = sum_vdd/loopcnt;
    rssi_half0 = sum_half/loopcnt;  

    printf("imod = %8d  rssiv0= %8d  gnd_code = %8d, VDD_Code = %8d, RSSI_Code = %8d, Half_VDD_Code0 = %8d\n", input_mod, rssi_v0, rssi_gnd, rssi_vdd, rssi_code0, rssi_half0);
          
    return;     
}


static int europa_cli_test_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    rtk_transceiver_data_t dataCfg, readableCfg;
    int32 ret = -1;
    uint32 delay1, delay2;
    uint32 temp32,regData1,regData2,regData3;
    uint32 gnd_code,pre_temp32; 
    rtk_enable_t enable;
    rtk_i2c_width_t width, awidth, dwidth;
    uint32 clock;
    uint32 rssi_v0;
    uint32 v_rssi, i_rssi, pre_i_rssi;
    uint64 sum_i, sum_v;
    int loopcnt,j, i, cnt;        
    double tmp, pre_tmp, diff;
    double tmp2, pre_tmp2, tmp3, pre_tmp3, tmp4, pre_tmp4, tmp5, pre_tmp5;    
    uint32 test_case;
    rtk_ldd_cfg_t ldd_cfg;
    int32 rx_a, rx_b, rx_c, itemp32;
    uint32 rx_power;
    uint32 vdd_v0, gnd_v0, half_v0, w83_data;
    int32 rssi_k;
    uint32 addr, value;    
    int16 temp_0, temp_0_offset;
    uint32 ra, rb; 
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }

    if(argc < 3)
    { 
        //printf("%s <loop cnt> <delay1: ms> <delay2: ms> \n", argv[0]);        
        printf("%s <case n> <loop cnt>\n", argv[0]);        
        return -1;
    }

    test_case = _vlaue_translate(argv[1]);

    printf("test_case: %d \n", test_case);    

    if (test_case == 1)
    {
        cnt = _vlaue_translate(argv[2]);
        printf("I2C Read Test: \n");
    
        for (i=0;i<cnt;i++)
        {
            europa_i2c_read(EUROPA_I2C_PORT, 0x55, 0x90, &temp32);
            europa_i2c_read(EUROPA_I2C_PORT, 0x55, 0x91, &pre_temp32);
            temp32 = ((temp32&0xFF)<<8)|(pre_temp32&0xFF);
            if (temp32!=0x8290)
            {
                printf("loop %d: temp32 = 0x%x != 0x8290\n", i,  temp32); 
                return 0;
            }
        }
    
        printf("I2C Read Test finish with loopcnt = %d \n", cnt);        
    }
    else if (test_case == 2)
    {  
        cnt = _vlaue_translate(argv[2]);
        
        europa_i2c_write(EUROPA_I2C_PORT, 0x50, 100, 0xA0);
        europa_i2c_write(EUROPA_I2C_PORT, 0x50, 101, 0xA1);
        europa_i2c_write(EUROPA_I2C_PORT, 0x50, 102, 0xA2);
        
        europa_i2c_read(EUROPA_I2C_PORT, 0x50, 100, &temp32);    
        printf("Test Start, A0/100 = 0x%x\n", temp32);
        europa_i2c_read(EUROPA_I2C_PORT, 0x50, 101, &temp32);    
        printf("Test Start, A0/101 = 0x%x\n", temp32);    
        europa_i2c_read(EUROPA_I2C_PORT, 0x50, 102, &temp32);    
        printf("Test Start, A0/102 = 0x%x\n", temp32);
        
        //Read GND code
        temp32 = regData1 = regData2 = regData3 = 0;    
        gnd_code = 0;    
        for (i=0;i<cnt;i++)
        {
            pre_temp32 = gnd_code;
            temp32 = regData1 = regData2 = regData3 = 0;
            europa_i2c_read(EUROPA_I2C_PORT, 0x50, 100, &regData3);    
            europa_i2c_read(EUROPA_I2C_PORT, 0x50, 101, &regData2);    
            europa_i2c_read(EUROPA_I2C_PORT, 0x50, 102, &regData1);    
            temp32 = ((regData1 & 0xff) << 16) | ((regData2 & 0xff) << 8) | (regData3);
            gnd_code = temp32;
            if ((pre_temp32!=temp32) && (pre_temp32!=0))
            {
                printf("loop %d: pre_temp32 = 0x%x temp32 = 0x%x\n", i, pre_temp32, temp32);  
            }
        }

    }
    else if (test_case == 3)
    {      
        uint64 sum_i2, sum_v2;

        printf("RX power test \n");
        _europa_cal_flash_data_get(RTL8290B_RX_A_ADDR, 4, &rx_a);
        _europa_cal_flash_data_get(RTL8290B_RX_B_ADDR, 4, &rx_b);
        _europa_cal_flash_data_get(RTL8290B_RX_C_ADDR, 4, &rx_c);
        _europa_cal_flash_data_get(RTL8290B_RSSI_V0_ADDR, 4, &rssi_v0);
        _europa_cal_flash_data_get(RTL8290B_GND_V0_ADDR, 4, &gnd_v0);
        _europa_cal_flash_data_get(RTL8290B_VDD_V0_ADDR, 4, &vdd_v0);
        _europa_cal_flash_data_get(RTL8290B_HALF_VDD_V0_ADDR, 4, &half_v0);
        _europa_cal_flash_data_get(RTL8290B_RX_K_ADDR, 4, &rssi_k);

        addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_TEMP0_OFFSET_ADDR;     
        _europa_cal_flash_data_get(addr, 2, &value);
        temp_0 = (int16)value;
        addr = RTL8290B_PARAM_ADDR_BASE + RTL8290B_TEMP0_OFF_OFFSET_ADDR;     
        _europa_cal_flash_data_get(addr, 2, &value);
        temp_0_offset = (int16)value;         
                     
        addr = RTL8290B_RX_RSSI_R1_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_get(addr, 2, &ra);      
        addr = RTL8290B_RX_RSSI_R2_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
        _europa_cal_flash_data_get(addr, 2, &rb);        
                     
        printf("Europa RX power RSSI V0 = %d\n", rssi_v0);
        printf("Europa RX power VDD V0 = %d\n", vdd_v0);
        printf("Europa RX power GND V0 = %d\n", gnd_v0);
        printf("Europa RX power Half VDD V0 = %d\n", half_v0);
        printf("Europa RX power RSSI K = %d\n", rssi_k);
        printf("Europa RX power rx_a = %d, rx_b = %d, rx_c = %d,\n", rx_a, rx_b, rx_c);
        printf("Europa RX power TEMP_0 = %d\n", temp_0);        
        printf("Europa RX power TEMP_0_OFFSET = %d\n", temp_0_offset);
        printf("Europa RX power Resistor A = %d\n", ra);
        printf("Europa RX power Resistor B = %d\n", rb);

        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);      
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);

       loopcnt =  _vlaue_translate(argv[2]);
       if (loopcnt<0)
       {
           printf("Loop count should >0\n");
           return -1;
       }       

       if (0==rssi_v0)
       {
           printf("Invalid RSSI-V0, run get rssi-v0 first!\n");
           return -1;
       }
       
       sum_i2 = 0;
       sum_v2 = 0;
       tmp = pre_tmp =0;
       for (i=0;i<loopcnt;i++)
       {
           pre_tmp = tmp;       
           _europa_rxpwr_get(rssi_v0, &i_rssi, &v_rssi);
           memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
           ldd_cfg.rssi_v0 = 0x8290;
           ldd_cfg.rssi_voltage2  = i_rssi;
           ldd_cfg.tx_bias        = (uint32)rx_a;
           ldd_cfg.tx_mod         = (uint32)rx_b;
           ldd_cfg.driver_version = (uint32)rx_c;
           ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
           if(ret)
           {
               printf("Re-calculation fail!!! (%d)\n", ret);          
               return -1;
           }
    
           rx_power = ldd_cfg.rssi_voltage;
           tmp = __log10((double)(rx_power&0xFFFF)*1/10000)*10;    
           printf("IRSSI = %d  RX power(0x%x): %f  ", i_rssi, ldd_cfg.rssi_voltage, tmp);  

           if(tmp!=0 && pre_tmp!=0)
           {
               diff = tmp - pre_tmp;
                printf("diff = %f ", diff);               
               if((diff>1)||(diff<-1))
               {
                    printf("(Over)");
               }           
           }
           printf("\n"); 

           sum_i2 += i_rssi;
           sum_v2 += v_rssi;
        }

        i_rssi = sum_i2/loopcnt;
        v_rssi = sum_v2/loopcnt;
       
        printf("I-RSSI AVG = %d   V-RSSI AVG = %d\n", i_rssi, v_rssi);            

        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);       

    }
    else if (test_case == 4)
    { 
        cnt = _vlaue_translate(argv[2]);    
        printf("PON TRANSCEIVER RX-POWER: \n");

        pre_tmp = tmp = 0;
        for (i=0;i<cnt;i++)
        {
            pre_tmp = tmp;
            ret = rtk_ponmac_transceiver_get(RTK_TRANSCEIVER_PARA_TYPE_RX_POWER, &dataCfg);
            if(ret)
            {
                printf("rtk_ponmac_transceiver_get fail!!! (%d)\n", ret);                 
                return -1;
            }
            tmp = __log10(((double)((dataCfg.buf[0] << 8) | dataCfg.buf[1])*1/10000))*10;    
            if(tmp!=pre_tmp)
            {
                diff = tmp - pre_tmp;
                printf("RTK_TRANSCEIVER_PARA_TYPE_RX_POWER = %f  previous = %f  diff = %f  (%d)\n", tmp, pre_tmp, diff, i);

                if(((diff>1)||(diff<-1))&&(i!=0))
                {
                     printf("Over 1dBm!!!!! STOP DDMI!!!!!\n");
                     europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);                       
                     return 0;
                }
            }
            osal_time_mdelay(1000);            
        }
    }
    else if (test_case == 5)
    { 
        uint32 vdd;    
        printf("PON TRANSCEIVER VOLTAGE: \n");
        cnt = _vlaue_translate(argv[2]);        
        pre_tmp = tmp =0;
        for (i=0;i<cnt;i++)
        {
            ret = rtk_ldd_config_refresh();
            if(ret)
            {
               printf("Refresh failed!!! (%d)\n", ret);
               return -1;
            }
            
            ret = rtk_ponmac_transceiver_get(RTK_TRANSCEIVER_PARA_TYPE_VOLTAGE, &dataCfg);
            if(ret)
            {
                printf("rtk_ponmac_transceiver_get fail!!! (%d)\n", ret);                 
                return -1;
            }
            _get_data_by_type(RTK_TRANSCEIVER_PARA_TYPE_VOLTAGE, &dataCfg, &readableCfg);
            printf("------------------------ Voltage(%d): %s ------------------------------\n", i, readableCfg.buf);
            tmp = atof(readableCfg.buf);
            if ( tmp < 3 )
            {
                printf("Voltage diff to large !!!!!!!!!!!!!! \n"); 
                vdd = 0;
                ret = rtk_ldd_vdd_get(&vdd);
                if(ret)
                {
                    printf("Get VDD Voltage fail!!! (%d)\n", ret);
                    return -1;
                }
                printf("VDD Voltage = %d uV\n", vdd);                
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);    
                return 0;
            }
            pre_tmp = tmp;
            osal_time_mdelay(10);            
        }
        
        //europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);
        //printf("Disable DDMI....\n");        
    }
    else if (test_case == 6)
    {
        europa_rxpwr_t *p_rx_arr;  
        uint32 rssi_v0, pre_v0;
        int32 rssi_k, pre_k;
        int32 diff_v0, diff_k;
        uint32 w83_data, w59_data, w5a_data, w70_data, data;
        
        printf("RSSI V0 VOLTAGE Test: \n");
        cnt = _vlaue_translate(argv[2]);
        if(cnt<0)
        {
            printf("cnt should > 0\n");
            return -1;
        }

        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);      
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x59, &w59_data);    
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, 0xFF);
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x5A, &w5a_data);    
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, 0xFF); 
        
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 70, &w70_data); 
        data = w70_data &(~(0x20));     
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 70, data);  
    
        p_rx_arr = (europa_rxpwr_t *)malloc(60*sizeof(europa_rxpwr_t));    

        printf("RSSI_V0 GND-Code0 GND-Code1 VDD-Code0 VDD-Code1 RSSI-Code0 RSSI-Code1 Half-VDD-Code0 Half-VDD-Code1 RSSI_K\n");
        //printf("RSSI_V0 RSSI_K(x1000) Diff_V0 Diff_K\n");
        rssi_v0 = 0;
        rssi_k = 0;
        pre_v0 = 0;
        pre_k = 0;
        diff_v0 = 0;
        diff_k = 0;
        for (i=0;i<cnt;i++)
        {
            pre_v0 = rssi_v0;
            pre_k = rssi_k;
            _europa_rssiv0_get(p_rx_arr, &rssi_v0, &rssi_k);
            diff_v0 = rssi_v0 - pre_v0;
            diff_k  = rssi_k - pre_k;
            //printf("%8d   %8d   %8d   %8d   %8d \n", i, rssi_v0, rssi_k, diff_v0, diff_k);
        }

        free(p_rx_arr);       
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 70, w70_data);        
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);     
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, w59_data);   
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, w5a_data);           
    }
    else if (test_case == 7)
    { 
        printf("RX-POWER Re-calculation: \n");

        memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
        ldd_cfg.rssi_v0 = 0x8290;
        itemp32 = _vlaue_translate(argv[2]);         
        rx_a = _vlaue_translate(argv[3]);
        rx_b = _vlaue_translate(argv[4]);
        rx_c = _vlaue_translate(argv[5]);    

        i_rssi = (uint32)itemp32;

        ldd_cfg.rssi_voltage2  = i_rssi;
        ldd_cfg.tx_bias        = (uint32)rx_a;
        ldd_cfg.tx_mod         = (uint32)rx_b;
        ldd_cfg.driver_version = (uint32)rx_c;
        ldd_cfg.state          = 1;        
        ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
        if(ret)
        {
            printf("Re-calculation fail!!! (%d)\n", ret);          
            return -1;
        }

        rx_power = ldd_cfg.rssi_voltage;
        tmp = __log10((double)(rx_power&0xFFFF)*1/10000)*10;    
        printf("RX power(0x%x): %f\n", ldd_cfg.rssi_voltage, tmp);           

    }
    else if (test_case == 8)
    {
        cnt = _vlaue_translate(argv[2]);    
        printf("PON TRANSCEIVER INFORMATION: \n");
        
        pre_tmp = tmp = tmp2 = 0;
        pre_tmp3 = tmp3 = tmp4 = tmp5 = 0;        
        for (i=0;i<cnt;i++)
        {
            pre_tmp = tmp;
            ret = rtk_ponmac_transceiver_get(RTK_TRANSCEIVER_PARA_TYPE_RX_POWER, &dataCfg);
            if(ret)
            {
                printf("rtk_ponmac_transceiver_get fail!!! (%d)\n", ret);                 
                return -1;
            }
            tmp = __log10(((double)((dataCfg.buf[0] << 8) | dataCfg.buf[1])*1/10000))*10;    
            if(tmp!=pre_tmp)
            {
                diff = tmp - pre_tmp;
                printf("RTK_TRANSCEIVER_PARA_TYPE_RX_POWER = %f  previous = %f  diff = %f  (%d)\n", tmp, pre_tmp, diff, i);

                if(((diff>1)||(diff<-1))&&(i!=0))
                {
                     printf("Over 1dBm!!!!! STOP DDMI!!!!!\n");
                     europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);                       
                     return 0;
                }
            }
            osal_time_mdelay(500); 
            
            pre_tmp2 = tmp2;
            ret = rtk_ponmac_transceiver_get(RTK_TRANSCEIVER_PARA_TYPE_TX_POWER, &dataCfg);
            if(ret)
            {
                printf("rtk_ponmac_transceiver_get fail!!! (%d)\n", ret);                 
                return -1;
            }
            tmp2 = __log10(((double)((dataCfg.buf[0] << 8) | dataCfg.buf[1])*1/10000))*10;    
            if(tmp2!=pre_tmp2)
            {
                diff = tmp2 - pre_tmp2;
                printf("RTK_TRANSCEIVER_PARA_TYPE_TX_POWER = %f  previous = %f  diff = %f  (%d)\n", tmp2, pre_tmp2, diff, i);
 
                if(((diff>1)||(diff<-1))&&(i!=0))
                {
                     printf("Over 1dBm!!!!! STOP DDMI!!!!!\n");
                     europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);                       
                     return 0;
                }
            }
            osal_time_mdelay(500); 
    
            pre_tmp3 = tmp3;
            ret = rtk_ponmac_transceiver_get(RTK_TRANSCEIVER_PARA_TYPE_VOLTAGE, &dataCfg);
            if(ret)
            {
                printf("rtk_ponmac_transceiver_get fail!!! (%d)\n", ret);                 
                return -1;
            }
            _get_data_by_type(RTK_TRANSCEIVER_PARA_TYPE_VOLTAGE, &dataCfg, &readableCfg);
            tmp3 = atof(readableCfg.buf);
            if ( tmp3!=pre_tmp3 )
            {
                diff = tmp3 - pre_tmp3;
                printf("RTK_TRANSCEIVER_PARA_TYPE_VOLTAGE = %f  previous = %f  diff = %f  (%d)\n", tmp3, pre_tmp3, diff, i);

                if(((diff>1)||(diff<-1))&&(i!=0))
                {
                     printf("Over 1V, STOP DDMI!!!!!\n");
                     europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);                       
                     return 0;
                }
            }
            osal_time_mdelay(500);            

            pre_tmp4 = tmp4;
            ret = rtk_ponmac_transceiver_get(RTK_TRANSCEIVER_PARA_TYPE_TEMPERATURE, &dataCfg);
            if(ret)
            {
                printf("rtk_ponmac_transceiver_get fail!!! (%d)\n", ret);                  
                return -1;
            }
            _get_data_by_type(RTK_TRANSCEIVER_PARA_TYPE_TEMPERATURE, &dataCfg, &readableCfg);
            tmp4 = atof(readableCfg.buf);
            if ( tmp4!=pre_tmp4 )
            {
                diff = tmp4 - pre_tmp4;
                printf("RTK_TRANSCEIVER_PARA_TYPE_TEMPERATURE = %f  previous = %f  diff = %f  (%d)\n", tmp4, pre_tmp4, diff, i);

                //if(((diff>1)||(diff<-1))&&(i!=0))
                //{
                //     printf("Over 1, STOP DDMI!!!!!\n");
                //     europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);                       
                //     return 0;
                //}
            }
            osal_time_mdelay(500);

            pre_tmp5 = tmp5;
            ret = rtk_ponmac_transceiver_get(RTK_TRANSCEIVER_PARA_TYPE_BIAS_CURRENT, &dataCfg);
            if(ret)
            {
                printf("rtk_ponmac_transceiver_get fail!!! (%d)\n", ret);                   
                return -1;
            }
            _get_data_by_type(RTK_TRANSCEIVER_PARA_TYPE_BIAS_CURRENT, &dataCfg, &readableCfg);
            tmp5 = atof(readableCfg.buf);
            if ( tmp5!=pre_tmp5 )
            {
                diff = tmp5 - pre_tmp5;
                printf("RTK_TRANSCEIVER_PARA_TYPE_BIAS_CURRENT = %f  previous = %f  diff = %f  (%d)\n", tmp5, pre_tmp5, diff, i);

                if(((diff>1)||(diff<-1))&&(i!=0))
                {
                     printf("Over 1, STOP DDMI!!!!!\n");
                     europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);                       
                     return 0;
                }
            }

            osal_time_mdelay(500);
        
        }
    }   
    else if (test_case == 9)
    {
        europa_rxpwr_t *p_rx_arr;  
        uint32 rssi_v0, pre_v0;
        int32 rssi_k, pre_k;
        int32 diff_v0, diff_k;
        uint32 w83_data, w59_data, w5a_data, w80_data;
        uint32 iput_mod, ori_Imod;
        rtk_ldd_loop_mode_t mode;    
        
        printf("RSSI V0 IMOD Test: \n");
        cnt = _vlaue_translate(argv[2]);
        if((cnt<0)||(cnt>100))
        {
            printf("cnt should be 0~100\n");
            return -1;
        }

        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);      
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x59, &w59_data);    
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, 0xFF);
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x5A, &w5a_data);    
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, 0xFF); 

        ret = rtk_ldd_tx_mod_get(&ori_Imod);
        if(ret)
        {
            printf("Get TX Mod = %d Fail!!! (%d)\n", ori_Imod, ret);
        }

        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x7C, &w80_data);        
        mode = LDD_LOOP_DOL;
        ret = rtk_ldd_loopMode_set(mode);
        if(ret)
        {
            printf("Set Loop Mode Fail!!! (%d)\n", ret);
        }  
        
        //p_rx_arr = (europa_rxpwr_t *)malloc(60*sizeof(europa_rxpwr_t));    
        //p_rx_arr = (europa_rxpwr_t *)malloc((cnt+10)*sizeof(europa_rxpwr_t));    
        p_rx_arr = (europa_rxpwr_t *)malloc(sizeof(europa_rxpwr_t)*110); 

        rssi_v0 = 0;
        rssi_k = 0;
        pre_v0 = 0;
        pre_k = 0;
        diff_v0 = 0;
        diff_k = 0;
        for (i=1;i<=20;i++)
        {
           iput_mod = i*5;
           _europa_rx_mod_test(p_rx_arr, iput_mod, cnt);
        }

        free(p_rx_arr);       
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);     
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, w59_data);   
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, w5a_data);   

        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x7C, w80_data);    
        ret = rtk_ldd_tx_mod_set(ori_Imod);
        if(ret)
        {
            printf("Set TX Mod = %d Fail!!! (%d)\n", ori_Imod, ret);
        }
        
    }
    else if (test_case == 10)
    {
        //europa_rxpwr_t *p_rx_arr;  
        uint32 rssi_v0, pre_v0;
        int32 rssi_k, pre_k;
        int32 diff_v0, diff_k;
        uint32 w83_data, w59_data, w5a_data;
        
        printf("RSSI V0 VOLTAGE Test 2: \n");
        cnt = _vlaue_translate(argv[2]);

        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);      
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x59, &w59_data);    
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, 0xFF);
        europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x5A, &w5a_data);    
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, 0xFF); 
    
        //p_rx_arr = (europa_rxpwr_t *)malloc(60*sizeof(europa_rxpwr_t));    

        printf("RSSI_V0 RSSI_K(x1000) Diff_V0 Diff_K\n");
        rssi_v0 = 0;
        rssi_k = 0;
        pre_v0 = 0;
        pre_k = 0;
        diff_v0 = 0;
        diff_k = 0;
        for (i=0;i<10;i++)
        {
            pre_v0 = rssi_v0;
            pre_k = rssi_k;
            _europa_rssiv0_get2(cnt, &rssi_v0, &rssi_k);
            diff_v0 = rssi_v0 - pre_v0;
            diff_k  = rssi_k - pre_k;
            printf("%8d   %8d   %8d   %8d   %8d \n", i, rssi_v0, rssi_k, diff_v0, diff_k);
        }

        //free(p_rx_arr);       
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);     
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x59, w59_data);   
        europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x5A, w5a_data);           
    }
    else if (test_case == 11)
    { 
        osal_time_mdelay(100);    
        memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
        ldd_cfg.rssi_v0 = 0x8290C;
        rx_a = _vlaue_translate(argv[2]);         
        ldd_cfg.vdd_code  = (uint32)rx_a;   
        ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
        if(ret)
        {
            printf("Re-calculation fail!!! (%d)\n", ret);          
            return -1;
        }
        printf("RX-POWER Offset Test: \n");        
        rx_power = ldd_cfg.vdd_code;
        tmp = __log10((double)(rx_power&0xFFFF)*1/10000)*10;    
        printf("Before:RX power(%d): %f\n", ldd_cfg.vdd_code, tmp); 
   
        rx_power = ldd_cfg.gnd_code;
        tmp = __log10((double)(rx_power&0xFFFF)*1/10000)*10;    
        printf("Offset:RX power(%d): %f\n", ldd_cfg.gnd_code, tmp);           

    }   
    return 0; 
}


static int europa_cli_fsuTest_set(
    int argc,
    char *argv[],
    FILE *fp)
{  
    int ret;
    //uint32 rssi_v0;
    uint32 loopcnt, i;
    uint8 *ptr_data = NULL;
    //uint64 sum;
    //uint32 index;
    //uint32 w83_data;
    uint32 regData1, regData2, regData3, half_vdd_code, rssi_code, ldo_code, temp32;
    rtk_ldd_cfg_t ldd_cfg;
    uint64 sum_rssi, sum_gnd, sum_vdd, sum_half_vdd, sum_rset, sum_ldo, sum_adc_gnd, sum_rx_ldo;
    uint32 avg_rssi, avg_gnd, avg_vdd, avg_half_vdd, avg_rset, avg_ldo, avg_adc_gnd, avg_rx_ldo;
    uint32 devId = 5;
    int chip_mode;

    chip_mode = _europa_ldd_parser();
    
    if (EUROPA_LDD_8290B_MODE != chip_mode)
    {  
        printf("chip_mode = 0x%x, This command is not supported.\n", chip_mode);
        return 0; 
    }
 
    if(argc < 2)
    { 
        printf("%s <loop count> \n", argv[0]);
         return -1;
    }
 
    //europa_i2c_read(EUROPA_I2C_PORT, 0x54, 83, &w83_data);    
    //europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, 0x10);
 
    //osal_time_mdelay(500); 
  
    loopcnt = _vlaue_translate(argv[1]); 
    //rssi_v0 = 0;
    //sum = 0;
 
    //printf("Parameter list: rssi_code gnd_code vdd_code ldo_code half_vdd_code rset_code adc_gnd_code rx_ldo_code\n");
#if 1 
    //for (i=0;i<loopcnt;i++)
    //{
        //printf("Loop %d: \n", i);
        ret = rtk_ldd_api_test(ptr_data);
        if(ret)
        {
            printf("rtk_ldd_api_test fail!!! (%d)\n", ret);
            //europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
            return -1;
        }
        //sum += rssi_v0;
        //printf("Loop %d: rssi V0 = 0x%x \n", i, rssi_v0);
    //}
#else
    sum_rssi = 0;
    sum_gnd = 0;
    sum_vdd = 0;
    sum_ldo = 0;
    sum_half_vdd = 0;
    sum_rset = 0;
    sum_adc_gnd = 0;
    sum_rx_ldo = 0;    
    for (i=0;i<loopcnt;i++)
    {
        //printf("Loop %d: \n", i);
          memset(&ldd_cfg, 0 , sizeof(rtk_ldd_cfg_t));
        ret = rtk_ldd_rssiVoltage_get(&ldd_cfg);
        if(ret)
        {
            printf("rtk_ldd_api_test fail!!! (%d)\n", ret);
  //europa_i2c_write(EUROPA_I2C_PORT, 0x54, 83, w83_data);
            return -1;
        }
    printf("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", ldd_cfg.rssi_code, ldd_cfg.gnd_code, ldd_cfg.vdd_code, ldd_cfg.ldo_code, ldd_cfg.half_vdd_code, ldd_cfg.rset_code, ldd_cfg.adc_gnd_code, ldd_cfg.rx_ldo_code);        
        sum_rssi += ldd_cfg.rssi_code;
        sum_gnd += ldd_cfg.gnd_code;
        sum_vdd += ldd_cfg.vdd_code;
        sum_ldo += ldd_cfg.ldo_code;
        sum_half_vdd += ldd_cfg.half_vdd_code;
        sum_rset += ldd_cfg.rset_code;
        sum_adc_gnd+= ldd_cfg.adc_gnd_code;
        sum_rx_ldo += ldd_cfg.rx_ldo_code;        
    }
    avg_rssi = sum_rssi/loopcnt;
    avg_gnd = sum_gnd/loopcnt;
    avg_vdd = sum_vdd/loopcnt;
    avg_ldo = sum_ldo/loopcnt;    
    avg_half_vdd = sum_half_vdd/loopcnt;
    avg_rset = sum_rset/loopcnt;    
    avg_adc_gnd = sum_adc_gnd/loopcnt;
    avg_rx_ldo = sum_rx_ldo/loopcnt;        
    printf("Average(%d): \n", loopcnt);    
    printf("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", avg_rssi, avg_gnd, avg_vdd, avg_ldo, avg_half_vdd, avg_rset, avg_adc_gnd, avg_rx_ldo);        
#endif

    return 0;    
#if 0
  //Read VDD/2 code
  //printk("before: jiffies = 0x%x\n", jiffies);
  temp32 = 0;
  europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x12, 0x82);  
  europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x12, 0x8A);  
  europa_i2c_read(EUROPA_I2C_PORT, 0x55, 14, &regData1); 
  europa_i2c_read(EUROPA_I2C_PORT, 0x55, 15, &regData2);  
  europa_i2c_read(EUROPA_I2C_PORT, 0x55, 16, &regData3);  
  temp32 = ((regData1 & 0xff) << 16) | ((regData2 & 0xff) << 8) | (regData3);
  half_vdd_code = temp32;
  //printk("jiffies = 0x%x, HZ = %d, half_vdd_code = 0x%x\n", jiffies, HZ, half_vdd_code);
  
  //Read RSSI code
  temp32 = 0;
  europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x12, 0xC2);  
  europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x12, 0xCA);  
  europa_i2c_read(EUROPA_I2C_PORT, 0x55, 14, &regData1); 
  europa_i2c_read(EUROPA_I2C_PORT, 0x55, 15, &regData2);  
  europa_i2c_read(EUROPA_I2C_PORT, 0x55, 16, &regData3);  
  temp32 =((regData1 & 0xff) << 16) | ((regData2 & 0xff) << 8) | (regData3);
  rssi_code = temp32;

  temp32 = 0;
  europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x12, 0x62);  
  europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x12, 0x6A);  
  europa_i2c_read(EUROPA_I2C_PORT, 0x55, 14, &regData1); 
  europa_i2c_read(EUROPA_I2C_PORT, 0x55, 15, &regData2);  
  europa_i2c_read(EUROPA_I2C_PORT, 0x55, 16, &regData3);  
  temp32 = ((regData1 & 0xff) << 16) | ((regData2 & 0xff) << 8) | (regData3);
  ldo_code = temp32;

  printf("No delay: half_vdd_code = 0x%x, rssi_code = 0x%x, ldo_code = 0x%x \n", half_vdd_code, rssi_code, ldo_code);

  
  temp32 = 0;
  europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x12, 0x82); 
  osal_time_mdelay(10);  
  europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x12, 0x8A);  
  europa_i2c_read(EUROPA_I2C_PORT, 0x55, 14, &regData1); 
  europa_i2c_read(EUROPA_I2C_PORT, 0x55, 15, &regData2);  
  europa_i2c_read(EUROPA_I2C_PORT, 0x55, 16, &regData3);  
  temp32 = ((regData1 & 0xff) << 16) | ((regData2 & 0xff) << 8) | (regData3);
  half_vdd_code = temp32;
  //printk("jiffies = 0x%x, HZ = %d, half_vdd_code = 0x%x\n", jiffies, HZ, half_vdd_code);
  
  //Read RSSI code
  temp32 = 0;
  europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x12, 0xC2);
  osal_time_mdelay(10);  
  europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x12, 0xCA);  
  europa_i2c_read(EUROPA_I2C_PORT, 0x55, 14, &regData1); 
  europa_i2c_read(EUROPA_I2C_PORT, 0x55, 15, &regData2);  
  europa_i2c_read(EUROPA_I2C_PORT, 0x55, 16, &regData3);  
  temp32 =((regData1 & 0xff) << 16) | ((regData2 & 0xff) << 8) | (regData3);
  rssi_code = temp32;

  temp32 = 0;
  europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x12, 0x62);
  osal_time_mdelay(10);  
  europa_i2c_write(EUROPA_I2C_PORT, 0x54, 0x12, 0x6A);  
  europa_i2c_read(EUROPA_I2C_PORT, 0x55, 14, &regData1); 
  europa_i2c_read(EUROPA_I2C_PORT, 0x55, 15, &regData2);  
  europa_i2c_read(EUROPA_I2C_PORT, 0x55, 16, &regData3);  
  temp32 = ((regData1 & 0xff) << 16) | ((regData2 & 0xff) << 8) | (regData3);
  ldo_code = temp32;

  printf("Delay 10ms: half_vdd_code = 0x%x, rssi_code = 0x%x, ldo_code = 0x%x \n", half_vdd_code, rssi_code, ldo_code);

 
  //rssi_v0 = sum/loopcnt;
  
  //_europa_cal_flash_data_set(RTL8290B_RSSI_V0_ADDR, 4, rssi_v0);        
 
  //printf("Get RSSI V0 = %d\n", rssi_v0);
 
  return 0;     
#endif

#if 0
    fp = fopen(RTL8290B_FILE_LOCATION,"rb");
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n");
        return -1;
    }
    init_data = (uint8 *)malloc(RTL8290B_PARAMETER_SIZE);
    if (!init_data)
    {
            printf("Memory Allocation Fail!!!!!!!\n");
            return -1;
    }
        
    fread(init_data, 1, RTL8290B_PARAMETER_SIZE, fp);
        
    ptr_data = init_data + 1400;
    ret = rtk_ldd_api_test(ptr_data); 

    free(init_data);
    
    return ret; 
#endif    
}

static int europa_cli_efuse_set(
    int argc,
    char *argv[],
    FILE *fp)

{
    int ret;
    uint32 devId = 5;    
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    { 
        ret = rtl8290c_cli_efuse_set(devId, argc, argv, fp);
        return ret;
    }
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    { 
        ret = rtl8291_cli_efuse_set(devId, argc, argv, fp);
        return ret;
    }
    else if(EUROPA_LDD_8290B_MODE == chip_mode)
    {
        printf("RTL8290B not support.\n");
        return -1;
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }

    return 0;
} 

static int europa_cli_efuse_get(
    int argc,
    char *argv[],
    FILE *fp)

{
    int ret;
    uint32 devId = 5;    
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE == chip_mode)
    { 
        ret = rtl8290c_cli_efuse_get(devId, argc, argv, fp);
        return ret;
    }
    else if(EUROPA_LDD_8291_MODE == chip_mode)
    { 
        ret = rtl8291_cli_efuse_get(devId, argc, argv, fp);
        return ret;
    }
    else if(EUROPA_LDD_8290B_MODE == chip_mode)
    {
        printf("RTL8290B not support.\n");
        return -1;
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }

    return 0;
}
    
static int europa_cli_dac_set(
    int argc,
    char *argv[],
    FILE *fp)

{
    int ret;
    uint32 devId = 5;    
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8291_MODE == chip_mode)
    { 
        ret = rtl8291_cli_dac_set(devId, argc, argv, fp);
        return ret;
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }

    return 0;
} 

static int europa_cli_dac_get(
    int argc,
    char *argv[],
    FILE *fp)

{
    int ret;
    uint32 devId = 5;    
    int chip_mode;

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8291_MODE == chip_mode)
    { 
        ret = rtl8291_cli_dac_get(devId);
        return ret;
    }
    else
    {
         printf("chip_mode = 0x%x, Chip mode not support.\n", chip_mode);
         return -1;
    }

    return 0;
} 

static int europa_cli_update_flash(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 devId = 5;

    if (argc >= 2)
    {
        if(!strcmp(argv[1], "8290c"))
        {
        
            ret = rtl8290c_cli_update_flash(devId);
            if(ret)
            {
                printf("rtl8290c_cli_update_flash Fail!!! (%d)\n", ret);
                return -1;
            }           
        }
        else if(!strcmp(argv[1], "8291"))
        {
        
            ret = rtl8291_cli_update_flash(devId);
            if(ret)
            {
                printf("rtl8291_cli_update_flash Fail!!! (%d)\n", ret);
                return -1;
            }           
        }		
        else
        {
            printf("Chip not support.\n");
            return -1;
        }
    }
    else
    {
        printf("%s 8290c\n", argv[0]);
    }
    
    return 0;
}

static int europa_cli_parse(
    int argc,
    char *argv[],
    europa_cli_tree_t *pCliTree, 
    FILE *fp)
{
    int inputLen, cmdLen;

    if(0 == argc)
    {
        /* No any argument for parsing */
        printf("available cmds:\n");
        while(pCliTree->type != EUROPA_CLI_END)
        {
            printf("%s\n", pCliTree->cmd);
            pCliTree += 1;
        }

        return -1;
    }

    if(NULL == pCliTree)
    {
        printf("incorrect command tree\n");
        return -2;
    }

    inputLen = strlen(argv[0]);
    while(pCliTree->type != EUROPA_CLI_END)
    {
        cmdLen = strlen(pCliTree->cmd);
        if(strncmp(argv[0], pCliTree->cmd, inputLen > cmdLen ? cmdLen : inputLen) == 0)
        {
            /* Search subtree or execute the command */
            if(pCliTree->type == EUROPA_CLI_BRANCH)
            {
                return europa_cli_parse(argc - 1, &argv[1], pCliTree->u.pSubTree, fp);
            }
            else if(pCliTree->type == EUROPA_CLI_LEAF)
            {
                if(NULL != pCliTree->u.cli_proc)
                {
                    return pCliTree->u.cli_proc(argc, argv, fp);
                }
                else
                {
                    printf("incorrect command callback\n");
                    return -3;
                }
            }
            else
            {
                printf("incorrect command type\n");
                return -3;
            }
        }
        pCliTree = pCliTree + 1;
    }

    printf("incorrect command\n");
    return -4;
}

int
main( int argc,
      char *argv[])
{
    int ret;
    int permits;
    //key_t msgQKey = 1568;
    //oam_cli_t cli;
    FILE *fp = NULL;
    

    //memset(&cli, 0x0, sizeof(cli));
    ret = europa_cli_parse(argc - 1, &argv[1], cliRoot, fp);


    return ret;
}


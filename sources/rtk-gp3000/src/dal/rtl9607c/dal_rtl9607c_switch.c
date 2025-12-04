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
 * $Revision: 46644 $
 * $Date: 2014-02-26 18:16:35 +0800 (????? 26 ??? 2014) $
 *
 * Purpose : Definition of Switch Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Switch parameter settings
 *           (2) Management address and vlan configuration.
 *
 */


/*
 * Include Files
 */
#include <ioal/mem32.h>
#include <rtk/switch.h>
#include <rtk/acl.h>
#include <rtk/qos.h>
#include <rtk/rate.h>
#include <rtk/l2.h>

#include <dal/rtl9607c/dal_rtl9607c.h>
#include <dal/rtl9607c/dal_rtl9607c_switch.h>
#include <dal/rtl9607c/dal_rtl9607c_flowctrl.h>
#include <dal/rtl9607c/dal_rtl9607c_acl.h>
#include <dal/rtl9607c/dal_rtl9607c_qos.h>
#include <dal/rtl9607c/dal_rtl9607c_l2.h>
#include <dal/rtl9607c/dal_rtl9607c_rate.h>
#include <dal/rtl9607c/dal_rtl9607c_port.h>
#include <dal/rtl9607c/dal_rtl9607c_tool.h>
#include <hal/common/miim.h>
#include <osal/time.h>
#include <hal/mac/mac_probe.h>
#ifdef CONFIG_SDK_KERNEL_LINUX
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/version.h>
#endif

/*
 * Symbol Definition
 */
#if !defined(CONFIG_SDK_KERNEL_LINUX)
#define dal_rtl9607c_tool_get_patch_info(x, y) (0)
#endif

#define PHY_K_PORT_NUM			4

/*thermal meter efuse calibration value*/
typedef struct dal_rtl9607c_thermal_data_s
{
    uint8  valid;
    uint8  positive;
    int32    intPart;
    int32 floatPart;
} dal_rtl9607c_thermal_data_t;

static dal_rtl9607c_thermal_data_t efuseThermalData;


typedef struct dal_rtl9607c_phy_data_s
{
    uint16  phy;
    uint16  addr;
    uint16  data;
} dal_rtl9607c_phy_data_t;

typedef enum rtl9607c_sys_ACL_idx_e
{
    RTL9607C_SYS_ACL_IDX_NIC = 0,
    RTL9607C_SYS_ACL_IDX_IGMP,
    RTL9607C_SYS_ACL_IDX_RLDP,
    RTL9607C_SYS_ACL_IDX_ARP_LAN,
    RTL9607C_SYS_ACL_IDX_LAN_ADDR,
    RTL9607C_SYS_ACL_IDX_RA,
    RTL9607C_SYS_ACL_IDX_ARP_WAN_UNTAG,
    RTL9607C_SYS_ACL_IDX_ARP_WAN_CTAG,
    RTL9607C_SYS_ACL_IDX_ARP_WAN_STAG,
    RTL9607C_SYS_ACL_IDX_WAN_ADDR,
    RTL9607C_SYS_ACL_IDX_MAX,
} rtl9607c_sys_ACL_idx_t;

/*
 * Data Declaration
 */
static uint32    switch_init = INIT_NOT_COMPLETED;

static dal_rtl9607c_phy_data_t  phyPatchArray[] = {
    /* Channel Estimation ver. 20170803 */
    {0x0000,0xa436,0x809a},
    {0x0000,0xa438,0x7db4},
    {0x0000,0xa436,0x809c},
    {0x0000,0xa438,0xb507},
    {0x0000,0xa436,0x809e},
    {0x0000,0xa438,0x6106},
    {0x0000,0xa436,0x80a0},
    {0x0000,0xa438,0x0980},
    {0x0000,0xa436,0x80a2},
    {0x0000,0xa438,0x6019},
    {0x0000,0xa436,0x80a4},
    {0x0000,0xa438,0x9a0c},
    {0x0001,0xa436,0x809a},
    {0x0001,0xa438,0x7db4},
    {0x0001,0xa436,0x809c},
    {0x0001,0xa438,0xb507},
    {0x0001,0xa436,0x809e},
    {0x0001,0xa438,0x6106},
    {0x0001,0xa436,0x80a0},
    {0x0001,0xa438,0x0980},
    {0x0001,0xa436,0x80a2},
    {0x0001,0xa438,0x6019},
    {0x0001,0xa436,0x80a4},
    {0x0001,0xa438,0x9a0c},
    {0x0002,0xa436,0x809a},
    {0x0002,0xa438,0x7db4},
    {0x0002,0xa436,0x809c},
    {0x0002,0xa438,0xb507},
    {0x0002,0xa436,0x809e},
    {0x0002,0xa438,0x6106},
    {0x0002,0xa436,0x80a0},
    {0x0002,0xa438,0x0980},
    {0x0002,0xa436,0x80a2},
    {0x0002,0xa438,0x6019},
    {0x0002,0xa436,0x80a4},
    {0x0002,0xa438,0x9a0c},
    {0x0003,0xa436,0x809a},
    {0x0003,0xa438,0x7db4},
    {0x0003,0xa436,0x809c},
    {0x0003,0xa438,0xb507},
    {0x0003,0xa436,0x809e},
    {0x0003,0xa438,0x6106},
    {0x0003,0xa436,0x80a0},
    {0x0003,0xa438,0x0980},
    {0x0003,0xa436,0x80a2},
    {0x0003,0xa438,0x6019},
    {0x0003,0xa436,0x80a4},
    {0x0003,0xa438,0x9a0c},
    {0x0004,0xa436,0x809a},
    {0x0004,0xa438,0x7db4},
    {0x0004,0xa436,0x809c},
    {0x0004,0xa438,0xb507},
    {0x0004,0xa436,0x809e},
    {0x0004,0xa438,0x6106},
    {0x0004,0xa436,0x80a0},
    {0x0004,0xa438,0x0980},
    {0x0004,0xa436,0x80a2},
    {0x0004,0xa438,0x6019},
    {0x0004,0xa436,0x80a4},
    {0x0004,0xa438,0x9a0c},
    {0x0000,0xa436,0x80ac},
    {0x0000,0xa438,0x1a84},
    {0x0000,0xa436,0x80ae},
    {0x0000,0xa438,0x3607},
    {0x0000,0xa436,0x80b0},
    {0x0000,0xa438,0x2106},
    {0x0000,0xa436,0x80b2},
    {0x0000,0xa438,0x0a6f},
    {0x0000,0xa436,0x80b4},
    {0x0000,0xa438,0x3d19},
    {0x0000,0xa436,0x80b6},
    {0x0000,0xa438,0x800c},
    {0x0001,0xa436,0x80ac},
    {0x0001,0xa438,0x1a84},
    {0x0001,0xa436,0x80ae},
    {0x0001,0xa438,0x3607},
    {0x0001,0xa436,0x80b0},
    {0x0001,0xa438,0x2106},
    {0x0001,0xa436,0x80b2},
    {0x0001,0xa438,0x0a6f},
    {0x0001,0xa436,0x80b4},
    {0x0001,0xa438,0x3d19},
    {0x0001,0xa436,0x80b6},
    {0x0001,0xa438,0x800c},
    {0x0002,0xa436,0x80ac},
    {0x0002,0xa438,0x1a84},
    {0x0002,0xa436,0x80ae},
    {0x0002,0xa438,0x3607},
    {0x0002,0xa436,0x80b0},
    {0x0002,0xa438,0x2106},
    {0x0002,0xa436,0x80b2},
    {0x0002,0xa438,0x0a6f},
    {0x0002,0xa436,0x80b4},
    {0x0002,0xa438,0x3d19},
    {0x0002,0xa436,0x80b6},
    {0x0002,0xa438,0x800c},
    {0x0003,0xa436,0x80ac},
    {0x0003,0xa438,0x1a84},
    {0x0003,0xa436,0x80ae},
    {0x0003,0xa438,0x3607},
    {0x0003,0xa436,0x80b0},
    {0x0003,0xa438,0x2106},
    {0x0003,0xa436,0x80b2},
    {0x0003,0xa438,0x0a6f},
    {0x0003,0xa436,0x80b4},
    {0x0003,0xa438,0x3d19},
    {0x0003,0xa436,0x80b6},
    {0x0003,0xa438,0x800c},
    {0x0004,0xa436,0x80ac},
    {0x0004,0xa438,0x1a84},
    {0x0004,0xa436,0x80ae},
    {0x0004,0xa438,0x3607},
    {0x0004,0xa436,0x80b0},
    {0x0004,0xa438,0x2106},
    {0x0004,0xa436,0x80b2},
    {0x0004,0xa438,0x0a6f},
    {0x0004,0xa436,0x80b4},
    {0x0004,0xa438,0x3d19},
    {0x0004,0xa436,0x80b6},
    {0x0004,0xa438,0x800c},
    {0x0000,0xa436,0x8088},
    {0x0000,0xa438,0xa08b},
    {0x0000,0xa436,0x808a},
    {0x0000,0xa438,0x4709},
    {0x0000,0xa436,0x808c},
    {0x0000,0xa438,0xab08},
    {0x0000,0xa436,0x808e},
    {0x0000,0xa438,0x20b4},
    {0x0000,0xa436,0x8090},
    {0x0000,0xa438,0xc41f},
    {0x0000,0xa436,0x8092},
    {0x0000,0xa438,0x801e},
    {0x0001,0xa436,0x8088},
    {0x0001,0xa438,0xa08b},
    {0x0001,0xa436,0x808a},
    {0x0001,0xa438,0x4709},
    {0x0001,0xa436,0x808c},
    {0x0001,0xa438,0xab08},
    {0x0001,0xa436,0x808e},
    {0x0001,0xa438,0x20b4},
    {0x0001,0xa436,0x8090},
    {0x0001,0xa438,0xc41f},
    {0x0001,0xa436,0x8092},
    {0x0001,0xa438,0x801e},
    {0x0002,0xa436,0x8088},
    {0x0002,0xa438,0xa08b},
    {0x0002,0xa436,0x808a},
    {0x0002,0xa438,0x4709},
    {0x0002,0xa436,0x808c},
    {0x0002,0xa438,0xab08},
    {0x0002,0xa436,0x808e},
    {0x0002,0xa438,0x20b4},
    {0x0002,0xa436,0x8090},
    {0x0002,0xa438,0xc41f},
    {0x0002,0xa436,0x8092},
    {0x0002,0xa438,0x801e},
    {0x0003,0xa436,0x8088},
    {0x0003,0xa438,0xa08b},
    {0x0003,0xa436,0x808a},
    {0x0003,0xa438,0x4709},
    {0x0003,0xa436,0x808c},
    {0x0003,0xa438,0xab08},
    {0x0003,0xa436,0x808e},
    {0x0003,0xa438,0x20b4},
    {0x0003,0xa436,0x8090},
    {0x0003,0xa438,0xc41f},
    {0x0003,0xa436,0x8092},
    {0x0003,0xa438,0x801e},
    {0x0004,0xa436,0x8088},
    {0x0004,0xa438,0xa08b},
    {0x0004,0xa436,0x808a},
    {0x0004,0xa438,0x4709},
    {0x0004,0xa436,0x808c},
    {0x0004,0xa438,0xab08},
    {0x0004,0xa436,0x808e},
    {0x0004,0xa438,0x20b4},
    {0x0004,0xa436,0x8090},
    {0x0004,0xa438,0xc41f},
    {0x0004,0xa436,0x8092},
    {0x0004,0xa438,0x801e},

    /* Green Table initial part ver. 20180425 */
    {0x0000,0xa436,0x8065},
    {0x0000,0xa438,0x1b33},
    {0x0000,0xa436,0x8069},
    {0x0000,0xa438,0x0822},
    {0x0000,0xa436,0x805d},
    {0x0000,0xa438,0x0911},
    {0x0000,0xa436,0x8061},
    {0x0000,0xa438,0x0820},
    {0x0000,0xa436,0x804d},
    {0x0000,0xa438,0x2444},
    {0x0000,0xa436,0x8051},
    {0x0000,0xa438,0x0823},
    {0x0001,0xa436,0x8065},
    {0x0001,0xa438,0x1b33},
    {0x0001,0xa436,0x8069},
    {0x0001,0xa438,0x0822},
    {0x0001,0xa436,0x805d},
    {0x0001,0xa438,0x0911},
    {0x0001,0xa436,0x8061},
    {0x0001,0xa438,0x0820},
    {0x0001,0xa436,0x804d},
    {0x0001,0xa438,0x2444},
    {0x0001,0xa436,0x8051},
    {0x0001,0xa438,0x0823},
    {0x0002,0xa436,0x8065},
    {0x0002,0xa438,0x1b33},
    {0x0002,0xa436,0x8069},
    {0x0002,0xa438,0x0822},
    {0x0002,0xa436,0x805d},
    {0x0002,0xa438,0x0911},
    {0x0002,0xa436,0x8061},
    {0x0002,0xa438,0x0820},
    {0x0002,0xa436,0x804d},
    {0x0002,0xa438,0x2444},
    {0x0002,0xa436,0x8051},
    {0x0002,0xa438,0x0823},
    {0x0003,0xa436,0x8065},
    {0x0003,0xa438,0x1b33},
    {0x0003,0xa436,0x8069},
    {0x0003,0xa438,0x0822},
    {0x0003,0xa436,0x805d},
    {0x0003,0xa438,0x0911},
    {0x0003,0xa436,0x8061},
    {0x0003,0xa438,0x0820},
    {0x0003,0xa436,0x804d},
    {0x0003,0xa438,0x2444},
    {0x0003,0xa436,0x8051},
    {0x0003,0xa438,0x0823},
    {0x0004,0xa436,0x8065},
    {0x0004,0xa438,0x1b33},
    {0x0004,0xa436,0x8069},
    {0x0004,0xa438,0x0822},
    {0x0004,0xa436,0x805d},
    {0x0004,0xa438,0x0911},
    {0x0004,0xa436,0x8061},
    {0x0004,0xa438,0x0820},
    {0x0004,0xa436,0x804d},
    {0x0004,0xa438,0x2444},
    {0x0004,0xa436,0x8051},
    {0x0004,0xa438,0x0823},

    /* RTCT Open/Mismatch threshold ver. 20180108 */
    {0x0000,0xa436,0x8160},
    {0x0000,0xa438,0x3727},
    {0x0000,0xa436,0x8162},
    {0x0000,0xa438,0xC487},
    {0x0000,0xa436,0x8164},
    {0x0000,0xa438,0x2F84},
    {0x0000,0xa436,0x8166},
    {0x0000,0xa438,0xECA5},
    {0x0001,0xa436,0x8160},
    {0x0001,0xa438,0x3727},
    {0x0001,0xa436,0x8162},
    {0x0001,0xa438,0xC487},
    {0x0001,0xa436,0x8164},
    {0x0001,0xa438,0x2F84},
    {0x0001,0xa436,0x8166},
    {0x0001,0xa438,0xECA5},
    {0x0002,0xa436,0x8160},
    {0x0002,0xa438,0x3727},
    {0x0002,0xa436,0x8162},
    {0x0002,0xa438,0xC487},
    {0x0002,0xa436,0x8164},
    {0x0002,0xa438,0x2F84},
    {0x0002,0xa436,0x8166},
    {0x0002,0xa438,0xECA5},
    {0x0003,0xa436,0x8160},
    {0x0003,0xa438,0x3727},
    {0x0003,0xa436,0x8162},
    {0x0003,0xa438,0xC487},
    {0x0003,0xa436,0x8164},
    {0x0003,0xa438,0x2F84},
    {0x0003,0xa436,0x8166},
    {0x0003,0xa438,0xECA5},
    {0x0004,0xa436,0x8160},
    {0x0004,0xa438,0x3727},
    {0x0004,0xa436,0x8162},
    {0x0004,0xa438,0xC487},
    {0x0004,0xa436,0x8164},
    {0x0004,0xa438,0x2F84},
    {0x0004,0xa436,0x8166},
    {0x0004,0xa438,0xECA5},
    {0x0000,0xa436,0x8174},
    {0x0000,0xa438,0x04BD},
    {0x0000,0xa436,0x8176},
    {0x0000,0xa438,0xE9E5},
    {0x0000,0xa436,0x8178},
    {0x0000,0xa438,0x5ED9},
    {0x0000,0xa436,0x817A},
    {0x0000,0xa438,0x348B},
    {0x0001,0xa436,0x8174},
    {0x0001,0xa438,0x04BD},
    {0x0001,0xa436,0x8176},
    {0x0001,0xa438,0xE9E5},
    {0x0001,0xa436,0x8178},
    {0x0001,0xa438,0x5ED9},
    {0x0001,0xa436,0x817A},
    {0x0001,0xa438,0x348B},
    {0x0002,0xa436,0x8174},
    {0x0002,0xa438,0x04BD},
    {0x0002,0xa436,0x8176},
    {0x0002,0xa438,0xE9E5},
    {0x0002,0xa436,0x8178},
    {0x0002,0xa438,0x5ED9},
    {0x0002,0xa436,0x817A},
    {0x0002,0xa438,0x348B},
    {0x0003,0xa436,0x8174},
    {0x0003,0xa438,0x04BD},
    {0x0003,0xa436,0x8176},
    {0x0003,0xa438,0xE9E5},
    {0x0003,0xa436,0x8178},
    {0x0003,0xa438,0x5ED9},
    {0x0003,0xa436,0x817A},
    {0x0003,0xa438,0x348B},
    {0x0004,0xa436,0x8174},
    {0x0004,0xa438,0x04BD},
    {0x0004,0xa436,0x8176},
    {0x0004,0xa438,0xE9E5},
    {0x0004,0xa436,0x8178},
    {0x0004,0xa438,0x5ED9},
    {0x0004,0xa436,0x817A},
    {0x0004,0xa438,0x348B},
    {0x0000,0xa436,0x8211},
    {0x0000,0xa438,0x13D6},
    {0x0000,0xa436,0x8213},
    {0x0000,0xa438,0xD5DE},
    {0x0000,0xa436,0x8215},
    {0x0000,0xa438,0x3FB3},
    {0x0000,0xa436,0x8217},
    {0x0000,0xa438,0xCF97},
    {0x0001,0xa436,0x8211},
    {0x0001,0xa438,0x13D6},
    {0x0001,0xa436,0x8213},
    {0x0001,0xa438,0xD5DE},
    {0x0001,0xa436,0x8215},
    {0x0001,0xa438,0x3FB3},
    {0x0001,0xa436,0x8217},
    {0x0001,0xa438,0xCF97},
    {0x0002,0xa436,0x8211},
    {0x0002,0xa438,0x13D6},
    {0x0002,0xa436,0x8213},
    {0x0002,0xa438,0xD5DE},
    {0x0002,0xa436,0x8215},
    {0x0002,0xa438,0x3FB3},
    {0x0002,0xa436,0x8217},
    {0x0002,0xa438,0xCF97},
    {0x0003,0xa436,0x8211},
    {0x0003,0xa438,0x13D6},
    {0x0003,0xa436,0x8213},
    {0x0003,0xa438,0xD5DE},
    {0x0003,0xa436,0x8215},
    {0x0003,0xa438,0x3FB3},
    {0x0003,0xa436,0x8217},
    {0x0003,0xa438,0xCF97},
    {0x0004,0xa436,0x8211},
    {0x0004,0xa438,0x13D6},
    {0x0004,0xa436,0x8213},
    {0x0004,0xa438,0xD5DE},
    {0x0004,0xa436,0x8215},
    {0x0004,0xa438,0x3FB3},
    {0x0004,0xa436,0x8217},
    {0x0004,0xa438,0xCF97}
};

/*
 * Function Declaration
 */

int32 dal_rtl9607c_switch_phyPatch(void);

int32 _rtl9607c_switch_macAddr_set(rtk_mac_t *pMacAddr);
int32 _rtl9607c_switch_macAddr_get(rtk_mac_t *pMacAddr);
#if defined(CONFIG_SDK_KERNEL_LINUX)
int32 _dal_phy_recovery_init(void);
#endif

static int32
_dal_rtl9607c_switch_ipEnable_set(void)
{
#if defined(FPGA_DEFINED)
#else
    int32  ret;
    uint32 value;
    uint32 chip, rev, subtype;

    /* Enable all necessary IPs in SoC */
    /* Enable PONPBO IP */
    if ((ret = ioal_socMem32_read(0xB800063C, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
        return ret;
    }
    value |= (1 << 5);
    if ((ret = ioal_socMem32_write(0xB800063C, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
        return ret;
    }

    if((ret = dal_rtl9607c_switch_version_get(&chip, &rev, &subtype)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    if(rev > CHIP_REV_ID_A)
    {
        /* Initialize switch PBO */
        /* Enable PONPBO IP */
        if ((ret = ioal_socMem32_read(0xB800063C, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
            return ret;
        }
        value |= (1 << 25);
        if ((ret = ioal_socMem32_write(0xB800063C, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PBO), "");
            return ret;
        }
    }
#endif
    return RT_ERR_OK;
}

static int32
_dal_rtl9607c_switch_ponAsicPhyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
{
    switch(portName)
    {
#if defined(CONFIG_SDK_KERNEL_LINUX)
    case RTK_PORT_UTP0:
    case RTK_PORT_UTP1:
    case RTK_PORT_UTP2:
    case RTK_PORT_UTP3:
    case RTK_PORT_UTP4:
    {
        int ret;
        uint32 i, valid, portCnt = portName - RTK_PORT_UTP0;
        uint32 capability = 0;

        if((ret = dal_rtl9607c_tool_get_capability(&capability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }

        /* Count how many valid port before portName */
        for(i = 0, valid = 0 ; i < 5 ; i ++)
        {
            if(!(capability & (1<<i)))
            {
                valid ++;
            }
            if(valid == (portCnt + 1))
            {
                *pPortId = i;
                return RT_ERR_OK;
            }
        }
        return RT_ERR_INPUT;
    }
#else
    case RTK_PORT_UTP0:
        *pPortId = 0;
        break;
    case RTK_PORT_UTP1:
        *pPortId = 1;
        break;
    case RTK_PORT_UTP2:
        *pPortId = 2;
        break;
    case RTK_PORT_UTP3:
        *pPortId = 3;
        break;
    case RTK_PORT_UTP4:
        *pPortId = 4;
        break;
#endif
    case RTK_PORT_PON:
    case RTK_PORT_UTP5:
        *pPortId = 5;
        break;
    case RTK_PORT_HSG0:
    case RTK_PORT_UTP6:
        *pPortId = 6;
        break;
    case RTK_PORT_HSG1:
    case RTK_PORT_CPU2:
    case RTK_PORT_UTP7:
        *pPortId = 7;
        break;
    case RTK_PORT_UTP8:
        *pPortId = 8;
        break;
    case RTK_PORT_CPU0:
        *pPortId = 9;
        break;
    case RTK_PORT_CPU1:
        *pPortId = 10;
        break;
    default:
        return RT_ERR_INPUT;
    }
        
    return RT_ERR_OK;
}

static int32
_dal_rtl9607c_switch_ponAsicLogicalPortName_get(int32 portId, rtk_switch_port_name_t *pPortName)
{

    switch(portId)
    {
#if defined(CONFIG_SDK_KERNEL_LINUX)
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    {
        int ret;
        uint32 i, valid;
        uint32 capability = 0;

        if((ret = dal_rtl9607c_tool_get_capability(&capability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }

        /* Find nth valid physical port */
        if((capability & (1<<portId)))
        {
            return RT_ERR_INPUT;
        }
        for(i = 0, valid = 0 ; i < portId ; i++)
        {
            if(!(capability & (1<<i)))
            {
                valid ++;
            }
        }
        *pPortName = valid + RTK_PORT_UTP0;
        break;
    }
#else
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        *pPortName = RTK_PORT_UTP0 + portId;
        break;
#endif
    case 5:
        *pPortName = RTK_PORT_PON | RTK_PORT_UTP5;
        break;
    case 6:
        *pPortName = RTK_PORT_UTP6;
        break;
    case 7:
        *pPortName = RTK_PORT_CPU2;
        break;
    case 8:
        *pPortName = RTK_PORT_UTP8;
        break;
    case 9:
        *pPortName = RTK_PORT_CPU0;
        break;
    case 10:
        *pPortName = RTK_PORT_CPU1;
        break;

    default:
        return RT_ERR_INPUT;
    }
        
    return RT_ERR_OK;
}


static int32
_dal_rtl9607c_switch_tickToken_init(void)
{
    int ret;
    uint32 wData, switchTick, switchToken;
    uint32 chip, rev, subtype;

    /*meter pon-tick-token configuration*/
    wData = 0x6e;
    if ((ret = reg_field_write(RTL9607C_PON_TB_CTRLr, RTL9607C_TICK_PERIODf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    wData = 0x95;
    if ((ret = reg_field_write(RTL9607C_PON_TB_CTRLr, RTL9607C_TKNf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    if((ret = dal_rtl9607c_switch_version_get(&chip, &rev, &subtype)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /*meter switch-tick-token configuration*/
    switchTick = 0x57;
    switchToken = 0xbd;
#ifdef CONFIG_SWITCH_SYS_CLOCK_TUNE
    if(rev > CHIP_REV_ID_A)
    {
        /* Overwrite the default value */
        switchTick = 0x16;
        switchToken = 0x3c;
    }
#endif
    if ((ret = reg_field_write(RTL9607C_METER_TB_CTRLr, RTL9607C_TICK_PERIODf, &switchTick)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9607C_METER_TB_CTRLr, RTL9607C_TKNf, &switchToken)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



static int32
_dal_rtl9607c_switch_green_enable(void)
{
    return RT_ERR_OK;
}

static int32
_dal_rtl9607c_switch_eee_enable(void)
{
    int32 ret;
    rtk_port_t port;
    uint32 wData;

    HAL_SCAN_ALL_PORT(port)
    {
        if(!HAL_IS_PHY_EXIST(port) || HAL_IS_PON_PORT(port))
        {
            continue;
        }

        /* Enable MAC EEE Tx/Rx function */
        wData = 0x1;
        if ((ret = reg_array_field_write(RTL9607C_EEE_EEEP_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9607C_EEE_PORT_TX_ENf, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        wData = 0x1;
        if ((ret = reg_array_field_write(RTL9607C_EEE_EEEP_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9607C_EEE_PORT_RX_ENf, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

static int32
_dal_rtl9607c_switch_green_patch(void)
{
    uint32 value;
    int32 ret;

    /* Additional green config */
    value = 0x00512C48;
    if ((ret = reg_write(RTL9607C_EEE_TX_TIMER_GIGA_CTRLr, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    value = 0x00512C8F;
    if ((ret = reg_write(RTL9607C_EEE_TX_TIMER_GELITE_CTRLr, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

static int32
_dal_rtl9607c_switch_eee_patch(void)
    {
    int ret;

    /* Additional EEE config */
    if((ret=rtl9607c_ocpInterPhy_write(0, 0xa436, 0x8047))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    if((ret=rtl9607c_ocpInterPhy_write(0, 0xa438, 0x2733))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    if((ret=rtl9607c_ocpInterPhy_write(1, 0xa436, 0x8047))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    if((ret=rtl9607c_ocpInterPhy_write(1, 0xa438, 0x2733))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    if((ret=rtl9607c_ocpInterPhy_write(2, 0xa436, 0x8047))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    if((ret=rtl9607c_ocpInterPhy_write(2, 0xa438, 0x2733))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    if((ret=rtl9607c_ocpInterPhy_write(3, 0xa436, 0x8047))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    if((ret=rtl9607c_ocpInterPhy_write(3, 0xa438, 0x2733))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    if((ret=rtl9607c_ocpInterPhy_write(4, 0xa436, 0x8047))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    if((ret=rtl9607c_ocpInterPhy_write(4, 0xa438, 0x2733))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

static int32
_dal_rtl9607c_switch_powerSaving_init(void)
{
    int ret;
    uint32 chip, rev, subtype;

    if((ret = dal_rtl9607c_switch_version_get(&chip, &rev, &subtype)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* === Green enable === */
    if ((ret = _dal_rtl9607c_switch_green_enable()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    switch(rev)
    {
    case CHIP_REV_ID_0:
    case CHIP_REV_ID_A:
    case CHIP_REV_ID_B:
        if ((ret = _dal_rtl9607c_switch_green_patch()) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        break;
    case CHIP_REV_ID_C:
    default:
        break;
    }
    
    /* === EEE enable === 
     * Always turn MAC side EEE function for all ports
     * Actual EEE enable/disable is controlled by PHY ability
     */
    if ((ret = _dal_rtl9607c_switch_eee_enable()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    switch(rev)
    {
    case CHIP_REV_ID_0:
    case CHIP_REV_ID_A:
    case CHIP_REV_ID_B:
        if ((ret = _dal_rtl9607c_switch_eee_patch()) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        break;
    case CHIP_REV_ID_C:
    default:
        break;
    }

    return RT_ERR_OK;
}

static int32 _dal_rtl9607c_switch_phyCmd_checkBusy(void)
{
    int32 ret;
    uint32 cnt = 0xffff;
    uint32 rData;

    while(cnt > 0)
    {
        if ((ret = reg_read(RTL9607C_GPHY_IND_RDr, &rData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
            return ret;
        }
        if(!(rData & (1<<16)))
        {
            return RT_ERR_OK;
        }

        cnt --;
    }

    return RT_ERR_TIMEOUT;
}

static int32
_dal_rtl9607c_switch_phyPower_set(rtk_enable_t enable)
{
    int ret;
    uint32 wData;
    uint32 rData;
    uint32 i;

    for(i = 0 ; i <= 5 ; i++)
    {
        if(!HAL_IS_PORT_EXIST(i))
        {
            continue;
        }

        /* Port i */
        wData = 0x20a400 | (i << 16);
        if ((ret = reg_write(RTL9607C_GPHY_IND_CMDr, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
            return ret;
        }

        /* Check busy */
        if ((ret = _dal_rtl9607c_switch_phyCmd_checkBusy()) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
            return ret;
        }        

        if ((ret = reg_read(RTL9607C_GPHY_IND_RDr, &rData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
            return ret;
        }

        if(enable)
        {
            wData = rData & 0xf7ff;
        }
        else
        {
            wData = rData | 0x0800;
        }
        if ((ret = reg_write(RTL9607C_GPHY_IND_WDr, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
            return ret;
        }
        wData = 0x60a400 | (i << 16);
        if ((ret = reg_write(RTL9607C_GPHY_IND_CMDr, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
            return ret;
        }

        /* Check busy */
        if ((ret = _dal_rtl9607c_switch_phyCmd_checkBusy()) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
            return ret;
        }        
    }

    return RT_ERR_OK;
}

#if defined(CONFIG_SWITCH_SYS_CLOCK_TUNE)
static int32
_dal_rtl9607c_switch_sysClock_set(void)
{
    int32  ret;
    uint32 value;
    uint32 chip, rev, subtype;

    if((ret = dal_rtl9607c_switch_version_get(&chip, &rev, &subtype)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    if(rev > CHIP_REV_ID_A)
    {
        /* Change to 14/17 */
        value = 1;
        if((ret = reg_field_write(RTL9607C_SYSCLK_TUNEr, RTL9607C_DIS_TUNEf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value = 1;
        if((ret = reg_field_write(RTL9607C_SYSCLK_TUNEr, RTL9607C_DIV_17f, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value = 0x1ef7d;
        if((ret = reg_field_write(RTL9607C_SYSCLK_TUNEr, RTL9607C_CLK_DIVISORf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value = 1;
        if((ret = reg_field_write(RTL9607C_SYSCLK_TUNEr, RTL9607C_LOAD_DIVISORf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value = 0;
        if((ret = reg_field_write(RTL9607C_SYSCLK_TUNEr, RTL9607C_LOAD_DIVISORf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value = 0;
        if((ret = reg_field_write(RTL9607C_SYSCLK_TUNEr, RTL9607C_DIS_TUNEf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
    }
    return RT_ERR_OK;
}
#endif

int32
dal_rtl9607c_switch_phyPatch(void)
{
    int i;
    int ret;
    dal_rtl9607c_phy_data_t *patchArray;
    uint32 patchSize;
    uint32 chip, rev, subtype;

    if((ret = dal_rtl9607c_switch_version_get(&chip, &rev, &subtype)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    patchArray = phyPatchArray;
    patchSize = sizeof(phyPatchArray)/sizeof(dal_rtl9607c_phy_data_t);

    /*start patch phy*/
    for(i=0 ; i<patchSize ; i++)
    {
        if((ret=rtl9607c_ocpInterPhy_write(patchArray[i].phy,
                              patchArray[i].addr,
                              patchArray[i].data))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

#define CAL_LONG_START_ENTRY 14
#define CAL_SHORT_START_ENTRY 19
#define CAL_NUM_PHY_PORT 5
#define CAL_PATCH_TIMEOUT 10000
#if defined(CONFIG_SDK_KERNEL_LINUX)
static dal_rtl9607c_phy_data_t  en70PatchArray[] = {
    /* En70 ver. 20171113 */
    {0x0, 0xa436, 0x83ed},
    {0x0, 0xa438, 0xaf83},
    {0x0, 0xa438, 0xf9af},
    {0x0, 0xa438, 0x8402},
    {0x0, 0xa438, 0xaf84},
    {0x0, 0xa438, 0x02af},
    {0x0, 0xa438, 0x8402},
    {0x0, 0xa438, 0x020c},
    {0x0, 0xa438, 0x3f02},
    {0x0, 0xa438, 0x8402},
    {0x0, 0xa438, 0xaf0c},
    {0x0, 0xa438, 0x39f8},
    {0x0, 0xa438, 0xf9fb},
    {0x0, 0xa438, 0xef79},
    {0x0, 0xa438, 0xbf84},
    {0x0, 0xa438, 0x5602},
    {0x0, 0xa438, 0x4997},
    {0x0, 0xa438, 0x0d11},
    {0x0, 0xa438, 0xf62f},
    {0x0, 0xa438, 0xef31},
    {0x0, 0xa438, 0xe087},
    {0x0, 0xa438, 0x770d},
    {0x0, 0xa438, 0x01f6},
    {0x0, 0xa438, 0x271b},
    {0x0, 0xa438, 0x03aa},
    {0x0, 0xa438, 0x1ae0},
    {0x0, 0xa438, 0x877c},
    {0x0, 0xa438, 0xe187},
    {0x0, 0xa438, 0x7dbf},
    {0x0, 0xa438, 0x8459},
    {0x0, 0xa438, 0x0249},
    {0x0, 0xa438, 0x53e0},
    {0x0, 0xa438, 0x877e},
    {0x0, 0xa438, 0xe187},
    {0x0, 0xa438, 0x7fbf},
    {0x0, 0xa438, 0x845c},
    {0x0, 0xa438, 0x0249},
    {0x0, 0xa438, 0x53ae},
    {0x0, 0xa438, 0x18e0},
    {0x0, 0xa438, 0x8778},
    {0x0, 0xa438, 0xe187},
    {0x0, 0xa438, 0x79bf},
    {0x0, 0xa438, 0x8459},
    {0x0, 0xa438, 0x0249},
    {0x0, 0xa438, 0x53e0},
    {0x0, 0xa438, 0x877a},
    {0x0, 0xa438, 0xe187},
    {0x0, 0xa438, 0x7bbf},
    {0x0, 0xa438, 0x845c},
    {0x0, 0xa438, 0x0249},
    {0x0, 0xa438, 0x53ef},
    {0x0, 0xa438, 0x97ff},
    {0x0, 0xa438, 0xfdfc},
    {0x0, 0xa438, 0x0470},
    {0x0, 0xa438, 0xa880},
    {0x0, 0xa438, 0xf0bc},
    {0x0, 0xa438, 0xdcf0},
    {0x0, 0xa438, 0xbcde},
    {0x0, 0xa436, 0xb818},
    {0x0, 0xa438, 0x0C36},
    {0x0, 0xa436, 0xb81a},
    {0x0, 0xa438, 0x3531},
    {0x0, 0xa436, 0xb81c},
    {0x0, 0xa438, 0x3531},
    {0x0, 0xa436, 0xb81e},
    {0x0, 0xa438, 0x000F},
    {0x0, 0xa436, 0xb832},
    {0x0, 0xa438, 0x0001}
};
#endif
static int32
_dal_rtl9607c_switch_phyEn70Cal(void)
{

#if defined(CONFIG_SDK_KERNEL_LINUX)
    int32 ret;
    uint32 i, j, port, cnt, patchSize, calValue;
    uint16 value;
    dal_rtl9607c_phy_data_t *patchArray;

    RT_LOG(LOG_DEBUG, (MOD_SWITCH|MOD_DAL), "switch en70 cal start\n");
    for(i = 0;i < CAL_NUM_PHY_PORT;i ++)
    {
        port = CAL_NUM_PHY_PORT - i - 1;

        /* Patch start */
        if((ret = rtl9607c_ocpInterPhy_read(port, 0xb820, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value |= 0x0010;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xb820, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }

        cnt = CAL_PATCH_TIMEOUT;
        while(cnt > 0)
        {
            if((ret = rtl9607c_ocpInterPhy_read(port, 0xb800, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
            if((value & 0x40) == 0x40)
            {
                break;
            }
            osal_time_mdelay(1);
            cnt --;
        }

        if((value & 0x40) != 0x40)
        {
            RT_ERR(RT_ERR_TIMEOUT, (MOD_SWITCH|MOD_DAL), "");
            return RT_ERR_TIMEOUT;
        }
        /* Ready for patch */

        value = 0x8028;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xA436, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value = 0x3700;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xA438, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }

        if((ret = rtl9607c_ocpInterPhy_read(port, 0xb82e, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value |= 0x0001;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xb82e, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }

        /* Patch code execution */
        patchArray = en70PatchArray;
        patchSize = sizeof(en70PatchArray)/sizeof(dal_rtl9607c_phy_data_t);
        for(j=0 ; j<patchSize ; j++)
        {
            if((ret=rtl9607c_ocpInterPhy_write(port,
                                  patchArray[j].addr,
                                  patchArray[j].data))!=RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
        }

        /* Length threshold */
        value = 0x8777;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xa436, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value = 0x4600;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xa438, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }

        /* Long length */
        if(dal_rtl9607c_tool_get_patch_info(i + CAL_LONG_START_ENTRY, &calValue) != 0)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        value = 0x8778;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xa436, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value = calValue;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xa438, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        
        /* Short length */
        if(dal_rtl9607c_tool_get_patch_info(i + CAL_SHORT_START_ENTRY, &calValue) != 0)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        value = 0x877A;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xa436, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value = calValue;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xa438, calValue)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value = 0x877C;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xa436, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xa438, calValue)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value = 0x877E;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xa436, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xa438, calValue)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }

        if((ret = rtl9607c_ocpInterPhy_read(port, 0xb82e, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value &= 0xfffe;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xb82e, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }

        /* Patch end */
        value = 0;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xb820, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
    }
#endif
    return RT_ERR_OK;
}


static int32
_dal_rtl9607c_switch_mdiAmpVoltage(void)
{
    int32 ret, port;
    uint32 wData;

    for(port = 0 ; port < CAL_NUM_PHY_PORT ; port ++)
    {
        wData = 0x2ff00;
        if ((ret = reg_array_write(RTL9607C_PHY_GDAC_IB_10Mr, REG_ARRAY_INDEX_NONE, port, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        wData = 0x2AA00;
        if ((ret = reg_array_write(RTL9607C_PHY_GDAC_IB_100Mr, REG_ARRAY_INDEX_NONE, port, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        wData = 0x2AA00;
        if ((ret = reg_array_write(RTL9607C_PHY_GDAC_IB_1000Mr, REG_ARRAY_INDEX_NONE, port, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
    }
    
    return RT_ERR_OK;
}

static dal_rtl9607c_phy_data_t voltagePatchArray[] = {
    /* Patch for all ports */
    {0, 0xa436, 0xA012},
    {0, 0xa438, 0x0000},
    {0, 0xa436, 0xA014},
    {0, 0xa438, 0x2c04},
    {0, 0xa438, 0x2c06},
    {0, 0xa438, 0x2c14},
    {0, 0xa438, 0x2c14},
    {0, 0xa438, 0xc114},
    {0, 0xa438, 0x25cb},
    {0, 0xa438, 0x410e},
    {0, 0xa438, 0x15aa},
    {0, 0xa438, 0x15f1},
    {0, 0xa438, 0xd501},
    {0, 0xa438, 0xa103},
    {0, 0xa438, 0x8203},
    {0, 0xa438, 0xd500},
    {0, 0xa438, 0x206a},
    {0, 0xa438, 0x15f7},
    {0, 0xa438, 0xd501},
    {0, 0xa438, 0x8103},
    {0, 0xa438, 0xa203},
    {0, 0xa438, 0xd500},
    {0, 0xa438, 0x206a},
    {0, 0xa436, 0xA01A},
    {0, 0xa438, 0x0000},
    {0, 0xa436, 0xA006},
    {0, 0xa438, 0x0fff},
    {0, 0xa436, 0xA004},
    {0, 0xa438, 0x0fff},
    {0, 0xa436, 0xA002},
    {0, 0xa438, 0x0065},
    {0, 0xa436, 0xA000},
    {0, 0xa438, 0x35ca},
};

static int32
_dal_rtl9607c_switch_phyVoltage(void)
{
    int32 ret;
    uint32 i, port, cnt, patchSize;
    uint16 value;
    dal_rtl9607c_phy_data_t *patchArray;

    RT_LOG(LOG_DEBUG, (MOD_SWITCH|MOD_DAL), "switch phyVoltage start\n");

    for(port = 0; port < CAL_NUM_PHY_PORT ; port ++)
    {
        /* Patch start */
        if((ret = rtl9607c_ocpInterPhy_read(port, 0xb820, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value |= 0x0010;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xb820, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }

        cnt = CAL_PATCH_TIMEOUT;
        while(cnt > 0)
        {
            if((ret = rtl9607c_ocpInterPhy_read(port, 0xb800, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
            if((value & 0x40) == 0x40)
            {
                break;
            }
            osal_time_mdelay(1);
            cnt --;
        }

        if((value & 0x40) != 0x40)
        {
            RT_ERR(RT_ERR_TIMEOUT, (MOD_SWITCH|MOD_DAL), "");
            return RT_ERR_TIMEOUT;
        }

        /* Ready for patch */
        value = 0x8028;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xA436, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value = 0x3700;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xA438, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }

        value = 0xb82e;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xa436, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value = 0x0001;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xa438, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }

        value = 0xb820;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xa436, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value = 0x0090;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xa438, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }

        /* Patch code execution */
        patchArray = voltagePatchArray;
        patchSize = sizeof(voltagePatchArray)/sizeof(dal_rtl9607c_phy_data_t);
        for(i=0 ; i<patchSize ; i++)
        {
            if((ret=rtl9607c_ocpInterPhy_write(port,
                                  patchArray[i].addr,
                                  patchArray[i].data))!=RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
        }

        value = 0xb820;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xa436, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value = 0x0000;
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xa438, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }

        cnt = CAL_PATCH_TIMEOUT;
        while(cnt > 0)
        {
            if((ret = rtl9607c_ocpInterPhy_read(port, 0xb800, &value)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
            if((value & 0x40) == 0)
            {
                break;
            }
            osal_time_mdelay(1);
            cnt --;
        }

        if((value & 0x40) != 0)
        {
            RT_ERR(RT_ERR_TIMEOUT, (MOD_SWITCH|MOD_DAL), "");
            return RT_ERR_TIMEOUT;
        }

        if((ret = rtl9607c_ocpInterPhy_read(port, 0xa432, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        value &= ~(1<<6);
        if((ret = rtl9607c_ocpInterPhy_write(port, 0xa432, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

static int32
_dal_rtl9607c_switch_phyCal(void)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)
    uint32 i;
    uint32 value=0, va, vb, vc, vd;
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_SWITCH|MOD_DAL), "switch cal start\n");
    for(i = 0;i < CAL_NUM_PHY_PORT;i ++)
    {
        if(dal_rtl9607c_tool_get_patch_info(i + CAL_LONG_START_ENTRY, &value) != 0)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        va = value & 0xf;
        vb = (value & 0xf0) >> 4;
        vc = (value & 0xf00) >> 8;
        vd = (value & 0xf000) >> 12;
        va = (va + 7) > 0xf ? 0xf : (va + 7);
        vb = (vb + 7) > 0xf ? 0xf : (vb + 7);
        vc = (vc + 7) > 0xf ? 0xf : (vc + 7);
        vd = (vd + 7) > 0xf ? 0xf : (vd + 7);
        value = va | (vb << 4) | (vc << 8) | (vd << 12);

        RT_LOG(LOG_DEBUG, (MOD_SWITCH|MOD_DAL), "write phy=%d, val=0x%x\n", CAL_NUM_PHY_PORT - i - 1, value);
        if((ret = rtl9607c_ocpInterPhy_write(CAL_NUM_PHY_PORT - i - 1, 0xbcdc, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        if((ret = rtl9607c_ocpInterPhy_write(CAL_NUM_PHY_PORT - i - 1, 0xbcde, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
    }
#endif
    return RT_ERR_OK;
}

#define PATCH_NUM_ENTRY 37
#define PATCH_START_ENTRY 38
#define PATCH_CMD_END_ENTRY 251 
static int32
_dal_rtl9607c_switch_default_patch(void)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)
    uint32 v1=0, v2=0, v3=0, v4=0, ptn1_no=0, ptn2_no=0, i, j;
    int32 ret;

    /* check need patch or not */
    if ((ret = reg_array_field_read(RTL9607C_EFUSE_BOND_RSLTr, 0, REG_ARRAY_INDEX_NONE, RTL9607C_EF_BOND_RSLTf, &v1)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    if((v1 & 0x1) == 0)
        return RT_ERR_OK;

    if((ret = dal_rtl9607c_tool_get_patch_info(PATCH_NUM_ENTRY, &ptn1_no))!=0)
        return RT_ERR_FAILED;
    osal_printf("switch default start\n");
    ptn1_no=ptn1_no&0xff;
    for(i=0; i<ptn1_no; i++)
    {
        if(dal_rtl9607c_tool_get_patch_info(PATCH_START_ENTRY+i*3, &v1)!=0)
            return RT_ERR_FAILED;
        if(dal_rtl9607c_tool_get_patch_info(PATCH_START_ENTRY+i*3+1, &v2)!=0)
            return RT_ERR_FAILED;
        if(dal_rtl9607c_tool_get_patch_info(PATCH_START_ENTRY+i*3+2, &v3)!=0)
            return RT_ERR_FAILED;
        if(v1==0 && v2==0 && v3==0)
            return RT_ERR_OK;
        if(v1==0xffff && v2==0xffff && v3==0xffff)
            continue;
        HAL_SCAN_ALL_PORT(j)
        {
            if((v1>>j)&0x1)
            {
                osal_printf("write phy=%d, reg=0x%x, val=0x%x\n", j, v2, v3);
                if((ret = rtl9607c_ocpInterPhy_write(j, v2, v3)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                    return ret;
                }
                
            }
        }
    }

    ptn2_no=(PATCH_CMD_END_ENTRY-PATCH_START_ENTRY+1-3*ptn1_no)/4;
    for(i=0; i<ptn2_no; i++)
    {
        if(dal_rtl9607c_tool_get_patch_info(PATCH_START_ENTRY+ptn1_no*3+i*4, &v1)!=0)
            return RT_ERR_FAILED;
        if(dal_rtl9607c_tool_get_patch_info(PATCH_START_ENTRY+ptn1_no*3+i*4+1, &v2)!=0)
            return RT_ERR_FAILED;
        if(dal_rtl9607c_tool_get_patch_info(PATCH_START_ENTRY+ptn1_no*3+i*4+2, &v3)!=0)
            return RT_ERR_FAILED;
        if(dal_rtl9607c_tool_get_patch_info(PATCH_START_ENTRY+ptn1_no*3+i*4+3, &v4)!=0)
            return RT_ERR_FAILED;
        if(v1==0 && v2==0 && v3==0 && v4==0)
            return RT_ERR_OK;
        if(v1==0xffff && v2==0xffff && v3==0xffff && v4==0xffff)
            continue;
        WRITE_MEM32((v1<<16)|v2, (v3<<16)|v4);
        osal_printf("write mem=0x%x, val=0x%x\n", (v1<<16)|v2, (v3<<16)|v4);
    }
#endif
    return RT_ERR_OK;
}

static int lan_eth_check_pcs_state(unsigned int port)
{
	int32 ret;
    uint32 i, miim_data=0;

#define MAX_NUM_OF_PCS_STATE_CHECK  0xff

	for(i = 0 ; i<MAX_NUM_OF_PCS_STATE_CHECK ; i++)
    {
        ret = rtl9607c_miim_read(port, 0xA60, 16, &miim_data);
		//osal_printf("\r\n %s-%d: ret=%d miim_data=0x%x", __func__, __LINE__, ret, miim_data);
        if(ret == RT_ERR_OK && 0x1 == (miim_data & (0xff)))
        {
            return 0;
        }
        osal_time_udelay(100);
    }

    /* Timeout */
    return 1;
}

#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
static int Lan_PHYCAL_check_apro_gen2(void)
{
	unsigned short data;
	rtk_port_t port;

	HAL_SCAN_ALL_ETH_PORT(port)
	{
		if (port >= PHY_K_PORT_NUM)
			continue;

		if(rtl9607c_ocpInterPhy_read(port, 0xbcfc, &data) == RT_ERR_OK)
			osal_printf("\n port %d ado=%x   \n",port, data);
		if(rtl9607c_ocpInterPhy_read(port, 0xbcdc, &data) == RT_ERR_OK)
			osal_printf(" port %d RCc=%x   \n",port, data);
		if(rtl9607c_ocpInterPhy_read(port, 0xbcde, &data) == RT_ERR_OK)
			osal_printf(" port %d RCe=%x   \n",port, data);
		if(rtl9607c_ocpInterPhy_read(port, 0xbce0, &data) == RT_ERR_OK)
			osal_printf(" port %d R0=%x   \n",port, data);
		if(rtl9607c_ocpInterPhy_read(port, 0xbce2, &data) == RT_ERR_OK)
			osal_printf(" port %d R2=%x   \n",port, data);
		if(rtl9607c_ocpInterPhy_read(port, 0xbcac, &data) == RT_ERR_OK)
			osal_printf(" port %d AMP=%x \n  ",port, data);
	}

	return 0;
}
#endif

static int32 Lan_OTP_read_apro_gen2(uint16 entry, uint32 *data)
{
	int32 ret;
	uint32 rData_h = 0, rData_l = 0;

    if ((ret = dal_rtl9607c_tool_get_patch_info(entry<<1, &rData_l)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
		osal_printf("entry=0x%x get FAIL.\n", entry);
        return ret;
	}
	if ((ret = dal_rtl9607c_tool_get_patch_info((entry<<1)+1, &rData_h)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
		osal_printf("entry=0x%x get FAIL.\n", entry);
        return ret;
	}
	*data = (rData_h<<8)+rData_l;
	return RT_ERR_OK;
}

static int32 Lan_PHYCAL_load_apro_gen2(void)
{
	int32 ret;
	rtk_port_t port = 0;
	unsigned short phy_dis_500M;
	uint32 tapbinrx_pm;
	uint32 val_OTP_GPHY_HW_ENABLE,val_OTP_GPHY_SW_CHECK,rData;
	uint32 gdac_read, gdac_read_high;

#define OTP_GPHY_HW_ENABLE			0x106
#define OTP_GPHY_SW_CHECK			0x107
#define OTP_PORT0_RC_CAL			0x110
#define OTP_PORT0_AMP_CAL			0x111
#define OTP_PORT0_ADO_CAL			0x112
#define OTP_PORT0_R_CAL				0x113

#define OTP_GDACIB_10M				0x120
#define OTP_GDACIB_100M				0x122
#define OTP_GDACIB_1000M			0x124

#define GDAC_PORT0_10M				0xBB01F068
#define GDAC_PORT1_10M				0xBB01F06C
#define GDAC_PORT2_10M				0xBB01F070
#define GDAC_PORT3_10M				0xBB01F074

#define GDAC_PORT0_100M				0xBB01F07C
#define GDAC_PORT1_100M				0xBB01F080
#define GDAC_PORT2_100M				0xBB01F084
#define GDAC_PORT3_100M				0xBB01F088

#define GDAC_PORT0_1000M			0xBB01F090
#define GDAC_PORT1_1000M			0xBB01F094
#define GDAC_PORT2_1000M			0xBB01F098
#define GDAC_PORT3_1000M			0xBB01F09C

#define GDACK_MASK					0xfffc0000

#define HW_ENABLE_GPHY_CAL_CMPLE	0x0010
#define HW_ENABLE_GPHY_SW_PATCH		0x0100
#define HW_ENABLE_GPHY_EN_ADO_CAL	0x0001
#define HW_ENABLE_GPHY_EN_RC_CAL	0x0002
#define HW_ENABLE_GPHY_EN_R_CAL		0x0004
#define HW_ENABLE_GPHY_EN_AMP_CAL	0x0008
#define HW_ENABLE_GPHY_EN_DIS_500M	0x0010
#define HW_ENABLE_EN_GDAC_IB_10M	0x0020
#define HW_ENABLE_EN_GDAC_IB_100M	0x0040
#define HW_ENABLE_EN_GDAC_IB_1000M	0x0080

	if ((ret = Lan_OTP_read_apro_gen2(OTP_GPHY_HW_ENABLE, &val_OTP_GPHY_HW_ENABLE)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
		osal_printf("OTP_GPHY_HW_ENABLE=0x%x get FAIL.\n", OTP_GPHY_HW_ENABLE);
        return ret;
	}

	if ((ret = Lan_OTP_read_apro_gen2(OTP_GPHY_SW_CHECK, &val_OTP_GPHY_SW_CHECK)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
		osal_printf("OTP_GPHY_SW_CHECK=0x%x get FAIL.\n", OTP_GPHY_SW_CHECK);
        return ret;
	}

#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
	osal_printf("hw_en=0x%x(entry=0x%x) sw_check=0x%x(entry=0x%x)\n",val_OTP_GPHY_HW_ENABLE,OTP_GPHY_HW_ENABLE,val_OTP_GPHY_SW_CHECK,OTP_GPHY_SW_CHECK);
#endif

	if ((val_OTP_GPHY_HW_ENABLE & HW_ENABLE_GPHY_CAL_CMPLE) !=HW_ENABLE_GPHY_CAL_CMPLE)
	{
		osal_printf("no gphy cal\n");
		return RT_ERR_FAILED;
	}

	if ((val_OTP_GPHY_HW_ENABLE & HW_ENABLE_GPHY_SW_PATCH) != 0)
	{
		osal_printf("sw_patch!=0\n");
		return RT_ERR_FAILED;
	}

	HAL_SCAN_ALL_ETH_PORT(port)
	{
		if (port >= PHY_K_PORT_NUM)
			continue;

		if((val_OTP_GPHY_SW_CHECK & HW_ENABLE_GPHY_EN_ADO_CAL) == HW_ENABLE_GPHY_EN_ADO_CAL)
		{
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
			osal_printf("enable OTP_PORT0_ADO_CAL\n");
#endif
			//adoadj=Giga_Eth_ReadPhyMdio_byOCP(port, 0xbcfc);
			if ((ret = Lan_OTP_read_apro_gen2(OTP_PORT0_ADO_CAL+port*4, &rData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
		        return ret;
			}
			if((ret = rtl9607c_ocpInterPhy_write(port, 0xbcfc, (uint16)rData) != RT_ERR_OK))
			{
			    return ret;
			}
			//osal_printf("port %d adok=%x\n",port,efuse_read_entry(OTP_PORT0_ADO_CAL+port*4) );
		}

		if((val_OTP_GPHY_SW_CHECK & HW_ENABLE_GPHY_EN_RC_CAL) == HW_ENABLE_GPHY_EN_RC_CAL)
		{
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
			osal_printf("enable HW_ENABLE_GPHY_EN_RC_CAL\n");
#endif
			//len_val=Giga_Eth_ReadPhyMdio_byOCP(port, 0xbcdc);
			//rlen_val=Giga_Eth_ReadPhyMdio_byOCP(port, 0xbcde);
			if ((ret = Lan_OTP_read_apro_gen2(OTP_PORT0_RC_CAL+port*4, &rData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
		        return ret;
			}
			if((ret = rtl9607c_ocpInterPhy_write(port, 0xbcdc, (uint16)rData)) != RT_ERR_OK)
			{
			    return ret;
			}
			if ((ret = Lan_OTP_read_apro_gen2(OTP_PORT0_RC_CAL+port*4, &rData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
		        return ret;
			}
			if((ret = rtl9607c_ocpInterPhy_write(port, 0xbcde, (uint16)rData)) != RT_ERR_OK)
			{
			    return ret;
			}
			//osal_printf("port %d rck=%x\n",port,efuse_read_entry(OTP_PORT0_RC_CAL+port*4) );
		}

		if((val_OTP_GPHY_SW_CHECK & HW_ENABLE_GPHY_EN_R_CAL) == HW_ENABLE_GPHY_EN_R_CAL)
		{
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
			osal_printf("enable HW_ENABLE_GPHY_EN_R_CAL\n");
#endif
			//tapbintx=Giga_Eth_ReadPhyMdio_byOCP(port, 0xbce0);
			//tapbinrx_pm=Giga_Eth_ReadPhyMdio_byOCP(port, 0xbce2);
			if ((ret = Lan_OTP_read_apro_gen2(OTP_PORT0_R_CAL+port*4, &tapbinrx_pm)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
		        return ret;
			}
#if 0
			/*for new R cal*/
			tapbinrx_pm|= (tapbinrx_pm << 4)|(tapbinrx_pm << 8)|(tapbinrx_pm << 12);
#endif
			if((ret = rtl9607c_ocpInterPhy_write(port, 0xbce0, (uint16)tapbinrx_pm)) != RT_ERR_OK)
			{
			    return ret;
			}
			if((ret = rtl9607c_ocpInterPhy_write(port, 0xbce2, (uint16)tapbinrx_pm)) != RT_ERR_OK)
			{
			    return ret;
			}
			//osal_printf("port %d tapbinrx_pm=%x\n",port,tapbinrx_pm);
		}

		if((val_OTP_GPHY_SW_CHECK & HW_ENABLE_GPHY_EN_AMP_CAL) == HW_ENABLE_GPHY_EN_AMP_CAL)
		{
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
			osal_printf("enable HW_ENABLE_GPHY_EN_AMP_CAL\n");
#endif
			//Giga_Eth_SetPhyMdio_byOCP(port, 0xbcac,ibadj);
			if ((ret = Lan_OTP_read_apro_gen2(OTP_PORT0_AMP_CAL+port*4, &rData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
		        return ret;
			}
			if((ret = rtl9607c_ocpInterPhy_write(port, 0xbcac, (uint16)rData)) != RT_ERR_OK)
			{
			    return ret;
			}
			//osal_printf("port %d amp=%x\n",port, rData);
		}

		//osal_printf("0xa4a2=%x\n", phy_dis_500M);
		if((val_OTP_GPHY_SW_CHECK & HW_ENABLE_GPHY_EN_DIS_500M) == HW_ENABLE_GPHY_EN_DIS_500M)
		{
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
			osal_printf("enable 	DIS_500M\n");
#endif
			if(rtl9607c_ocpInterPhy_read(port,0xa4a2, &phy_dis_500M) == RT_ERR_OK)
			{
			    if((ret = rtl9607c_ocpInterPhy_write(port, 0xa4a2, phy_dis_500M & 0xfeff)) != RT_ERR_OK)
			    {
			        return ret;
			    }
			}
			if(rtl9607c_ocpInterPhy_read(port,0xa428, &phy_dis_500M) == RT_ERR_OK)
			{
			    if((ret = rtl9607c_ocpInterPhy_write(port, 0xa428, phy_dis_500M & 0xfdff)) != RT_ERR_OK)
			    {
			        return ret;
			    }
			}
		}
		else
		{
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
			osal_printf("enable 	Not DIS_500M\n");
#endif
			if(rtl9607c_ocpInterPhy_read(port,0xa4a2, &phy_dis_500M) == RT_ERR_OK)
			{
			    if((ret = rtl9607c_ocpInterPhy_write(port, 0xa4a2, phy_dis_500M | 0x0100)) != RT_ERR_OK)
			    {
			        return ret;
			    }
			}
			if(rtl9607c_ocpInterPhy_read(port,0xa428, &phy_dis_500M) == RT_ERR_OK)
			{
			    if((ret = rtl9607c_ocpInterPhy_write(port, 0xa428, phy_dis_500M | 0x0200)) != RT_ERR_OK)
			    {
			        return ret;
			    }
			}
		}

#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
		if(rtl9607c_ocpInterPhy_read(port,0xa4a2, &phy_dis_500M) == RT_ERR_OK)
		{
			osal_printf("0xa4a2=%x\n", phy_dis_500M);
		}
		if(rtl9607c_ocpInterPhy_read(port,0xa428, &phy_dis_500M) == RT_ERR_OK)
		{
			osal_printf("0xa428=%x\n", phy_dis_500M);
		}
#endif

		//add for GDAC
		if ((val_OTP_GPHY_SW_CHECK & HW_ENABLE_EN_GDAC_IB_10M) == HW_ENABLE_EN_GDAC_IB_10M)
		{
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
			osal_printf("valid 10M patch\n");
#endif
			gdac_read=REG32(GDAC_PORT0_10M+port*4);
			gdac_read&=GDACK_MASK;
			if ((ret = Lan_OTP_read_apro_gen2(OTP_GDACIB_10M+1, &rData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
		        return ret;
			}
			gdac_read_high=rData<<16;
			if ((ret = Lan_OTP_read_apro_gen2(OTP_GDACIB_10M, &rData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
		        return ret;
			}
			gdac_read =gdac_read+ rData+ gdac_read_high;
			REG32(GDAC_PORT0_10M+port*4)=gdac_read;
			//osal_printf("port %d 10M patch	%x=%x\n",port,GDAC_PORT0_10M+port*4,REG32(GDAC_PORT0_10M+port*4));
		}

		if ((val_OTP_GPHY_SW_CHECK & HW_ENABLE_EN_GDAC_IB_100M) == HW_ENABLE_EN_GDAC_IB_100M)
		{
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
			osal_printf("valid 100M patch\n");
#endif
			gdac_read=REG32(GDAC_PORT0_100M+port*4);
			if ((ret = Lan_OTP_read_apro_gen2(OTP_GDACIB_100M+1, &rData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
		        return ret;
			}
			gdac_read_high=rData<<16;
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
			osal_printf("valid 100M patch gdac_read=%x\n",gdac_read);
#endif
			gdac_read&=GDACK_MASK;
			if ((ret = Lan_OTP_read_apro_gen2(OTP_GDACIB_100M, &rData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
		        return ret;
			}
			gdac_read =gdac_read+ rData+gdac_read_high;
			REG32(GDAC_PORT0_100M+port*4)=gdac_read;
			//osal_printf("port %d 100M patch %x=%x\n",port,GDAC_PORT0_100M+port*4,REG32(GDAC_PORT0_100M+port*4));
		}

		if ((val_OTP_GPHY_SW_CHECK & HW_ENABLE_EN_GDAC_IB_1000M) == HW_ENABLE_EN_GDAC_IB_1000M)
		{
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
			osal_printf("valid 1000M patch\n");
#endif
			gdac_read=REG32(GDAC_PORT0_1000M+port*4);
			if ((ret = Lan_OTP_read_apro_gen2(OTP_GDACIB_1000M+1, &rData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
		        return ret;
			}
			gdac_read_high=rData<<16;
			gdac_read&=GDACK_MASK;
			if ((ret = Lan_OTP_read_apro_gen2(OTP_GDACIB_1000M, &rData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
		        return ret;
			}
			gdac_read =gdac_read+ rData+gdac_read_high;
			REG32(GDAC_PORT0_1000M+port*4)=gdac_read;
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
			osal_printf("port %d 1000M patch %x=%x\n",port,GDAC_PORT0_1000M+port*4,REG32(GDAC_PORT0_1000M+port*4));
#endif
		}
		//end of GDAC
	}

	return RT_ERR_OK;
}

typedef struct lan_eth_miim_entry
{
	uint32 page;
	uint32 reg;
	uint32 msb;
	uint32 lsb;
	uint32 data;
} lan_eth_miim_entry_t;

// GPHY parameter release_V1.6 20220407
static lan_eth_miim_entry_t lanNctlPatchFrcEcOAproGen2Table[] = {
	{0xa43, 27, 15, 0, 0x8028}, // patch_key
	{0xa43, 28, 15, 0, 0x3100},
	{0xa43, 27, 15, 0, 0xb82e}, // patch_lock
	{0xa43, 28, 15, 0, 0x0001},
	{0xa43, 27, 15, 0, 0xa012}, // set write address = nctl_patch_addr (pg#a01 rg#17)
	{0xa43, 28, 15, 0, 0x0000}, // set nctl_patch_addr=0x0000 (pg#a01rg#17 = 0x0000)
	{0xa43, 27, 15, 0, 0xa014}, // set write address = nctl_patch_data (pg#a01 rg#18)
	{0xb82, 16, 7, 7, 0x1},     // disable auto memory increase
	{0xa43, 28, 15, 0, 0x2c04}, // set nctl_patch_data = 2c08 start to fill nctl sram
	{0xa43, 28, 15, 0, 0x2c07},
	{0xa43, 28, 15, 0, 0x2c0a},
	{0xa43, 28, 15, 0, 0x2c0a},
	{0xa43, 28, 15, 0, 0x3129},
	{0xa43, 28, 15, 0, 0x4b98},
	{0xa43, 28, 15, 0, 0x2b92},
	{0xa43, 28, 15, 0, 0x3129},
	{0xa43, 28, 15, 0, 0x4bc5},
	{0xa43, 28, 15, 0, 0x2bc2},
	{0xb82, 16, 7, 7, 0x0}, // enable auto memory increase
	{0xa43, 27, 15, 0, 0xa002}, // set write address = nctl_bp/bp_addr (pg#a00 rg#16)
	{0xa43, 28, 15, 0, 0x0bc0}, // pg#a00 rg#16 = 1bc0) nctl_bp_en0=1, nctl_bp_addr0=bc0
	{0xa43, 27, 15, 0, 0xa000}, // set write address = nctl_bp/bp_addr (pg#a00 rg#16)
	{0xa43, 28, 15, 0, 0x3b95}, // pg#a00 rg#16 = 1bc0) nctl_bp_en0=1, nctl_bp_addr0=bc0
	{0xa43, 27, 15, 0, 0xb82e},
	{0xa43, 28, 15, 0, 0x0000},
	{0xa43, 27, 15, 0, 0x8028},
	{0xa43, 28, 15, 0, 0x0000},
	{0xa43, 27, 15, 0, 0xb820}, // set write address = patch reg
	{0xa43, 28, 15, 0, 0x0000}
};

// GPHY parameter release_V1.6 20220920
static lan_eth_miim_entry_t lanNctlPatchFrcEcOTxCommonAproGen2Table[] = {
	{0xa43, 27, 15, 0, 0x8028}, // patch_key
	{0xa43, 28, 15, 0, 0x3100},
	{0xa43, 27, 15, 0, 0xb82e}, // patch_lock
	{0xa43, 28, 15, 0, 0x0001},
	{0xa43, 27, 15, 0, 0xa012}, // set write address = nctl_patch_addr (pg#a01 rg#17)
	{0xa43, 28, 15, 0, 0x0000}, // set nctl_patch_addr=0x0000 (pg#a01rg#17 = 0x0000)
	{0xa43, 27, 15, 0, 0xa014}, // set write address = nctl_patch_data (pg#a01 rg#18)
	{0xb82, 16, 7, 7, 0x1},     // disable auto memory increase
	{0xa43, 28, 15, 0, 0x2c04}, // set nctl_patch_data = 2c08 start to fill nctl sram
	{0xa43, 28, 15, 0, 0x2c07}, // ====memo : patch for ex_o_x2 . (begin)
	{0xa43, 28, 15, 0, 0x2c0a},
	{0xa43, 28, 15, 0, 0x2c0c}, // new update
	{0xa43, 28, 15, 0, 0x3129},
	{0xa43, 28, 15, 0, 0x4b98},
	{0xa43, 28, 15, 0, 0x2b92},
	{0xa43, 28, 15, 0, 0x3129},
	{0xa43, 28, 15, 0, 0x4bc5},
	{0xa43, 28, 15, 0, 0x2bc2}, // ====memo : patch for ex_o_x2 . (end)
	{0xa43, 28, 15, 0, 0xc114}, // ====memo : new add patch for RX common mode power patch (begin) 2022/09/15 version 2
	{0xa43, 28, 15, 0, 0x25b6},
	{0xa43, 28, 15, 0, 0x410e},
	{0xa43, 28, 15, 0, 0x1595},
	{0xa43, 28, 15, 0, 0x15dc},
	{0xa43, 28, 15, 0, 0xd501},
	{0xa43, 28, 15, 0, 0xa103},
	{0xa43, 28, 15, 0, 0x8203},
	{0xa43, 28, 15, 0, 0xd500},
	{0xa43, 28, 15, 0, 0x206c},
	{0xa43, 28, 15, 0, 0x15e2},
	{0xa43, 28, 15, 0, 0xd501},
	{0xa43, 28, 15, 0, 0x8103},
	{0xa43, 28, 15, 0, 0xa203},
	{0xa43, 28, 15, 0, 0xd500},
	{0xa43, 28, 15, 0, 0x206c}, //  ====memo : new add patch for RX common mode power patch (begin) 2022/09/15 version 2
	{0xb82, 16 ,7, 7, 0x0},     // enable auto memory increase
	{0xa43, 27, 15, 0, 0xa000}, // set write address = nctl_bp/bp_addr (pg#a00 rg#16)
	{0xa43, 28, 15, 0, 0xfb95}, // pg#a00 rg#16 = fb95) nctl_bp_en3/2/1/0=f, nctl_bp_addr0=b95
	{0xa43, 27, 15, 0, 0xa002}, // set write address = nctl_bp/bp_addr (pg#a00 rg#16)
	{0xa43, 28, 15, 0, 0x0bc0}, // pg#a00 rg#16 = 0bc0) nctl_bp_addr1=bc0
	{0xa43, 27, 15, 0, 0xA004},
	{0xa43, 28, 15, 0, 0x05b5},
	{0xa43, 27, 15, 0, 0xA006},
	{0xa43, 28, 15, 0, 0x0067},
	{0xa43, 27, 15, 0, 0xb82e},
	{0xa43, 28, 15, 0, 0x0000},
	{0xa43, 27, 15, 0, 0x8028},
	{0xa43, 28, 15, 0, 0x0000},
	{0xa43, 27, 15, 0, 0xb820}, // set write address = patch reg
	{0xa43, 28, 15, 0, 0x0000},
	// ---------------------------------
	// reg patch : (for phyID 0,1,2,3)
	// ---------------------------------
	// ctap_short_en (page 0xa43, reg 25, bit6) = 0
	{0xa43, 25, 6, 6, 0x0}
};

static lan_eth_miim_entry_t lanPatchAfeParameterAproGen2Table[] = {
	//{0xbcc, 16 ,13, 12, 0x2}, // 1.   vcm :          pg 0xbcc  / rg 16 / bit[13:12] =0x2
	{0xbcc, 16 ,13, 12, 0x1}, // 1.   vcm :          pg 0xbcc  / rg 16 / bit[13:12] =0x1  ;# 2022/04/07 new update
	{0xbca, 23 ,14, 12, 0x4}, // 2.   ref_ldo:       pg 0xbca  / rg 23 / bit[14:12] =0x4
	{0xbcc, 16 ,10, 8, 0x4}, // 3.   reg_ref:       pg 0xbcc  / rg 16 / bit[10:8]  =0x4
	{0xbca, 23 ,1, 0, 0x0}, // 4.   adc_sarduty:   pg 0xbca  / rg 23 / bit[1:0]   =0x0
	{0xbca, 23 ,8, 8, 0x0}, // 5.   adc_duty75:    pg 0xbca  / rg 23 / bit[8]     =0x0
	{0xbcc, 23 ,4, 4, 0x0}, // 6.   adc_ibset_dn:  pg 0xbcc  / rg 23 / bit[4]     =0x0
	//{0xbcd, 19 ,2, 0, 0x1}, // 7.   adc_ibset:     pg 0xbcd  / rg 19 / bit[2:0]   =0x1
	{0xbcd, 19 ,2, 0, 0x5} // 7.   adc_ibset:     pg 0xbcd  / rg 19 / bit[2:0]   =0x5  ;# 2022/04/07 new update
};

static lan_eth_miim_entry_t lanPatchInrxParameterAproGen2Table[] = {
	{0xA43, 27, 15, 0, 0x809b}, // Giga master sram addr = 0x809b
	//{0xA43, 28, 15, 11, 0x11}, // INRX delta_a modify default 0xD to 0x11.
	{0xA43, 28, 15, 11, 0x13}, // INRX delta_a modify default 0xD to 0x13.
	{0xA43, 27, 15, 0, 0x80ad}, // Giga slave sram addr = 0x80ad
	{0xA43, 28, 15, 11, 0x11} // INRX delta_a modify default 0xD to 0x11.
};

static int32 Lan_eth_miim_clear_write(rtk_port_t port, uint32 page, uint32 reg, uint32 msb, uint32 lsb, uint32 data)
{
	uint32 miim_data=0;
	uint32 miim_mask=0;
	int32 ret;

#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
	osal_printf("\r\n [W]%s-%d: page=0x%x, reg=%d, msb=%d, lsb=%d, data=0x%x", __func__, __LINE__, page, reg, msb, lsb, data);
#endif

	ret = rtl9607c_miim_read(port, page, reg, &miim_data);
	if (ret == RT_ERR_OK)
	{
		miim_mask = ((1<<(msb-lsb+1))-1)<<lsb;
		miim_data &= ~(miim_mask);
		miim_data |= (data<<lsb);
		ret = rtl9607c_miim_write(port, page, reg, miim_data);
		if (ret != RT_ERR_OK)
		{
			osal_printf("\r\n %s-%d: rtl9607c_miim_write ERROR!", __func__, __LINE__);
			return ret;
		}
	}
	else
	{
		osal_printf("\r\n %s-%d: rtl9607c_miim_read ERROR!", __func__, __LINE__);
		return ret;
	}

#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
	ret = rtl9607c_miim_read(port, page, reg, &miim_data);
	if (ret == RT_ERR_OK)
	{
		osal_printf("\r\n [R]%s-%d: page=0x%x, reg=%d, miim_data=0x%x", __func__, __LINE__, page, reg, miim_data);
	}
#endif

	return RT_ERR_OK;
}

static int lan_eth_check_patch_state(unsigned int port)
{
	int32 ret;
	uint32 i, miim_data=0;

#define MAX_NUM_OF_PATCH_STATE_CHECK  0xff

	for(i = 0 ; i<MAX_NUM_OF_PATCH_STATE_CHECK ; i++)
	{
		ret = rtl9607c_miim_read(port, 0xB80, 16, &miim_data);
		//osal_printf("\r\n %s-%d: ret=%d miim_data=0x%x", __func__, __LINE__, ret, miim_data);
		if(ret == RT_ERR_OK && (miim_data & (1<<6)))
		{
			return 0;
	}
		osal_time_udelay(100);
	}

	/* Timeout */
	return 1;
}

static int32 Lan_NCTL_patch_frc_ec_o_apro_gen2(rtk_port_t port)
{
	uint32 i, patchSize;
	lan_eth_miim_entry_t *patchArray;
	int32 ret;

	// set patch req
	ret = Lan_eth_miim_clear_write(port, 0xB82, 16, 4, 4, 0x1);
		if (ret != RT_ERR_OK)
		{
		goto ERROR;
	}

	// read until change from 0 to 1, wait patch state ready
	if ((ret = lan_eth_check_patch_state(port)))
	{
		osal_printf("\r\n %s-%d: lan_eth_check_patch_state timeout.", __func__, __LINE__);
		goto ERROR;
	}

	patchArray = lanNctlPatchFrcEcOAproGen2Table;
	patchSize = sizeof(lanNctlPatchFrcEcOAproGen2Table)/sizeof(lan_eth_miim_entry_t);
	for(i=0 ; i<patchSize ; i++)
	{
		if((ret=Lan_eth_miim_clear_write(port,
                              patchArray[i].page,
                              patchArray[i].reg,
                              patchArray[i].msb,
                              patchArray[i].lsb,
                              patchArray[i].data))!=RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
			goto ERROR;
		}
	}

	return RT_ERR_OK;

ERROR:
	return RT_ERR_NOT_FINISH;
}

static int32 Lan_NCTL_patch_frc_ec_o_tx_common_apro_gen2(rtk_port_t port)
{
	uint32 i, patchSize;
	lan_eth_miim_entry_t *patchArray;
	int32 ret;

	// set patch req
	ret = Lan_eth_miim_clear_write(port, 0xB82, 16, 4, 4, 0x1);
		if (ret != RT_ERR_OK)
		{
		goto ERROR;
	}

	// read until change from 0 to 1, wait patch state ready
	if ((ret = lan_eth_check_patch_state(port)))
	{
		osal_printf("\r\n %s-%d: lan_eth_check_patch_state timeout.", __func__, __LINE__);
		goto ERROR;
	}

	patchArray = lanNctlPatchFrcEcOTxCommonAproGen2Table;
	patchSize = sizeof(lanNctlPatchFrcEcOTxCommonAproGen2Table)/sizeof(lan_eth_miim_entry_t);
	for(i=0 ; i<patchSize ; i++)
	{
		if((ret=Lan_eth_miim_clear_write(port,
                              patchArray[i].page,
                              patchArray[i].reg,
                              patchArray[i].msb,
                              patchArray[i].lsb,
                              patchArray[i].data))!=RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
			goto ERROR;
		}
	}

	return RT_ERR_OK;

ERROR:
	return RT_ERR_NOT_FINISH;
}

static int32 Lan_patch_AFE_parameter_apro_gen2(rtk_port_t port)
{
    uint32 i, patchSize;
    lan_eth_miim_entry_t *patchArray;
    int32 ret;

    patchArray = lanPatchAfeParameterAproGen2Table;
    patchSize = sizeof(lanPatchAfeParameterAproGen2Table)/sizeof(lan_eth_miim_entry_t);
    for(i=0 ; i<patchSize ; i++)
	{
        if((ret=Lan_eth_miim_clear_write(port,
                              patchArray[i].page,
                              patchArray[i].reg,
                              patchArray[i].msb,
                              patchArray[i].lsb,
                              patchArray[i].data))!=RT_ERR_OK)
		{
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
			goto ERROR;
		}
	}

	return RT_ERR_OK;

ERROR:
	return RT_ERR_NOT_FINISH;
}

static int32 Lan_patch_INRX_parameter_apro_gen2(rtk_port_t port)
{
	uint32 i, patchSize;
    lan_eth_miim_entry_t *patchArray;
	int32 ret;

	patchArray = lanPatchInrxParameterAproGen2Table;
    patchSize = sizeof(lanPatchInrxParameterAproGen2Table)/sizeof(lan_eth_miim_entry_t);
    for(i=0 ; i<patchSize ; i++)
	{
        if((ret=Lan_eth_miim_clear_write(port,
                              patchArray[i].page,
                              patchArray[i].reg,
                              patchArray[i].msb,
                              patchArray[i].lsb,
                              patchArray[i].data))!=RT_ERR_OK)
		{
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
			goto ERROR;
		}
	}

	return RT_ERR_OK;

ERROR:
	return RT_ERR_NOT_FINISH;
}

static int32 Lan_eth_change_default_apro_gen2(void)
{
	int32 ret;
	rtk_port_t port;
	uint32 miim_data=0;

	// (A1) set SOC ready
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
	osal_printf("\r\n (A1) set SOC ready");
#endif
	REG32(0xB8000044) |=  (1<<0);

	// (A2) patch GPHY parameter  (in lock main state)
	// <A2-begin> set lock main state (note: if phy_status = EXT_INI, no need to do lock main. cannot write SUCCESS too.)
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
	osal_printf("\r\n Lan_eth_change_default_apro_gen2 enter");
	osal_printf("\r\n REG32(0xB8000044)=0x%x", REG32(0xB8000044));
	osal_printf("\r\n");
	osal_printf("\r\n (A2) patch GPHY parameter");
	osal_printf("\r\n <A2-begin> set lock main state");
#endif
	HAL_SCAN_ALL_ETH_PORT(port)
	{
		if (port >= PHY_K_PORT_NUM)
			continue;

#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
		if((ret = hal_miim_read(port, 0, 0, &miim_data)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
		}
		osal_printf("\r\n port=%d power=%s", port, miim_data&(1<<11)?"DOWN":"UP");
#endif

		ret = rtl9607c_miim_read(port, 0xA42, 16, &miim_data);
		if (ret == RT_ERR_OK)
		{
			// When Px_phy_status = 0x2 (EXT_INI), no need to set lock main, because we cannot set it successfully.
			if ((miim_data&0x7) != 0x2)
			{
					// set lock main
				ret = Lan_eth_miim_clear_write(port, 0xA46, 21, 1, 1, 0x1);
					if (ret != RT_ERR_OK)
					{
						goto ERROR;
					}

				// check & wait pcs_state = 0x1
				if((ret = lan_eth_check_pcs_state(port)))
				{
					osal_printf("\r\n %s-%d: lan_eth_check_pcs_state timeout.", __func__, __LINE__);
					goto ERROR;
				}
			}
		}
	}

	// (A2-0) enable ADC_bias_bootAutoK
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
	osal_printf("\r\n (A2-0) enable ADC_bias_bootAutoK");
#endif
#if 0
	HAL_SCAN_ALL_ETH_PORT(port)
	{
		if (port >= PHY_K_PORT_NUM)
			continue;

			// SRAM address = 0x8016
		ret = Lan_eth_miim_clear_write(port, 0xA43, 27, 15, 0, 0x8016);
			if (ret != RT_ERR_OK)
			{
			goto ERROR;
		}

			// REG: SRAM address 0x8016 ,bit[10] =1
		ret = Lan_eth_miim_clear_write(port, 0xA43, 28, 10, 10, 0x1);
			if (ret != RT_ERR_OK)
			{
				goto ERROR;
			}
		}
#endif

	// <A2-1> GPHY init setting
	// ## <1>,<2> : disable green , R-tune .
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
	osal_printf("\r\n <A2-1> GPHY init setting");
#endif
	HAL_SCAN_ALL_ETH_PORT(port)
	{
		if (port >= PHY_K_PORT_NUM)
			continue;

			// SRAM address = 0x8011
		ret = Lan_eth_miim_clear_write(port, 0xA43, 27, 15, 0, 0x8011);
			if (ret != RT_ERR_OK)
			{
				goto ERROR;
			}

		// sram_rg01[7]/0x8011 disable green,  sram_rg01[3]/0x8011 disable R-tune.
		ret = Lan_eth_miim_clear_write(port, 0xA43, 28, 15, 15, 0x0);
		if (ret != RT_ERR_OK)
		{
			goto ERROR;
		}

		// SRAM address = 0x8011
		ret = Lan_eth_miim_clear_write(port, 0xA43, 27, 15, 0, 0x8011);
			if (ret != RT_ERR_OK)
			{
				goto ERROR;
			}

		// sram_rg01[7]/0x8011 disable green,  sram_rg01[3]/0x8011 disable R-tune.
		ret = Lan_eth_miim_clear_write(port, 0xA43, 28, 11, 11, 0x0);
		if (ret != RT_ERR_OK)
		{
			goto ERROR;
		}

		// <3> set ec_o_en =1 , to enable echo cancellor ec_o_x2 autotune function
		//----------
		// 2022/04/07 : new add for 4-corner hi/low temp cycle test performance improve:
#if 0
		// <4> force set ec_o_x2 =1 , to force trigger ec_o_x2 .
		ret = Lan_NCTL_patch_frc_ec_o_apro_gen2(port);
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
		osal_printf("\r\n %s-%d: Lan_NCTL_patch_frc_ec_o_apro_gen2() (port=%d, %s)", __func__, __LINE__, port, (ret==RT_ERR_OK)?"SUCCESS":"FAIL");
#endif
		if(ret != RT_ERR_OK)
		{
			osal_printf("\r\n %s-%d: Lan_NCTL_patch_frc_ec_o_apro_gen2 ERROR! (port=%d)", __func__, __LINE__, port);
			goto ERROR;
		}
#else
		// <4> force set ec_o_x2 =1 , to force trigger ec_o_x2 .  AND 
		// <5> patch for discrete X'former application RX common mode voltage. 
		ret = Lan_NCTL_patch_frc_ec_o_tx_common_apro_gen2(port);
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
		osal_printf("\r\n %s-%d: Lan_NCTL_patch_frc_ec_o_tx_common_apro_gen2() (port=%d, %s)", __func__, __LINE__, port, (ret==RT_ERR_OK)?"SUCCESS":"FAIL");
#endif
		if(ret != RT_ERR_OK)
	{
			osal_printf("\r\n %s-%d: Lan_NCTL_patch_frc_ec_o_tx_common_apro_gen2 ERROR! (port=%d)", __func__, __LINE__, port);
			goto ERROR;
		}
#endif
	}

	// ##  <3> enable  echo cancellor ec_o_x2 autotune function
	HAL_SCAN_ALL_ETH_PORT(port)
	{
		if (port >= PHY_K_PORT_NUM)
			continue;

			// enable  echo cancellor ec_o_x2 autotune function 
		ret = Lan_eth_miim_clear_write(port, 0xA42, 22, 4, 4, 0x1);
			if (ret != RT_ERR_OK)
			{
			goto ERROR;
		}
	}

	// <A2-2> RL6831_patch_AFE_parameter
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
	osal_printf("\r\n <A2-2> RL6831_patch_AFE_parameter");
#endif
	HAL_SCAN_ALL_ETH_PORT(port)
	{
		if (port >= PHY_K_PORT_NUM)
			continue;

		ret = Lan_patch_AFE_parameter_apro_gen2(port);
			if (ret != RT_ERR_OK)
			{
				goto ERROR;

}
	}

	// <A2-3> RL6831_patch_MP_cal
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
	osal_printf("\r\n <A2-3> RL6831_patch_MP_cal");
#endif
	HAL_SCAN_ALL_ETH_PORT(port)
	{
		if (port >= PHY_K_PORT_NUM)
			continue;

		//AMP-cal:
		ret = Lan_eth_miim_clear_write(port, 0xBCA, 22, 15, 0, 0x9999);
			if (ret != RT_ERR_OK)
			{
			goto ERROR;
		}
#if 0
		// RC-cal :
		ret = Lan_eth_miim_clear_write(port, 0xBCD, 22, 15, 0, 0x8888);
			if (ret != RT_ERR_OK)
			{
			goto ERROR;
		}

			// rlen
		ret = Lan_eth_miim_clear_write(port, 0xBCD, 23, 15, 0, 0x8888);
			if (ret != RT_ERR_OK)
			{
			goto ERROR;
		}

		// ## R-cal  :
			// tapbin
		ret = Lan_eth_miim_clear_write(port, 0xBCE, 16, 15, 0, 0x8888);
			if (ret != RT_ERR_OK)
			{
			goto ERROR;
		}

			// tapbin_pm
		ret = Lan_eth_miim_clear_write(port, 0xBCE, 17, 15, 0, 0x8888);
			if (ret != RT_ERR_OK)
			{
			goto ERROR;
		}

		// ADC biase -cal:
			// adcioffset
		ret = Lan_eth_miim_clear_write(port, 0xBCF, 22, 15, 0, 0x8888);
			if (ret != RT_ERR_OK)
			{
			goto ERROR;
		}
#endif
	}

#if 0
	efuse_write_entry(0x106, 0x0010);
	efuse_write_entry(0x107, 0x001f);
	efuse_write_entry(0x110, 0x8888);
	efuse_write_entry(0x111, 0x9aaa);
	efuse_write_entry(0x112, 0x8698);
	efuse_write_entry(0x113, 0x0006);
	efuse_write_entry(0x114, 0x8888);
	efuse_write_entry(0x115, 0x98a9);
	efuse_write_entry(0x116, 0x8688);
	efuse_write_entry(0x117, 0x0006);
	efuse_write_entry(0x118, 0x8888);
	efuse_write_entry(0x119, 0x8998);
	efuse_write_entry(0x11a, 0x8889);
	efuse_write_entry(0x11b, 0x0006);
	efuse_write_entry(0x11c, 0x8888);
	efuse_write_entry(0x11d, 0x7889);
	efuse_write_entry(0x11e, 0x9888);
	efuse_write_entry(0x11f, 0x0006);
#endif

	// <A2-4> RL6831_patch_MP_cal_OTP_reload : c-code
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
	osal_printf("\r\n <A2-4> RL6831_patch_MP_cal_OTP_reload : c-code");
#endif
	ret=Lan_PHYCAL_load_apro_gen2();

#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
	if (ret == RT_ERR_OK)
		osal_printf("\r\n %s-%d: Lan_PHYCAL_load_apro SUCCESS!", __func__, __LINE__);
	else
		osal_printf("\r\n %s-%d: Lan_PHYCAL_load_apro FAIL!", __func__, __LINE__);

	Lan_PHYCAL_check_apro_gen2();
#endif

	// <A2-5> RL6831_patch_INRX_parameter
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
	osal_printf("\r\n <A2-5> RL6831_patch_INRX_parameter");
#endif
	HAL_SCAN_ALL_ETH_PORT(port)
	{
		if (port >= PHY_K_PORT_NUM)
			continue;

		ret = Lan_patch_INRX_parameter_apro_gen2(port);
			if (ret != RT_ERR_OK)
			{
			goto ERROR;
		}
	}

	// <A2-end> release lock main state
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
	osal_printf("\r\n <A2-end> release lock main state");
#endif
	HAL_SCAN_ALL_ETH_PORT(port)
	{
		if (port >= PHY_K_PORT_NUM)
			continue;

				// set lock main 
		ret = Lan_eth_miim_clear_write(port, 0xA46, 21, 1, 1, 0x0);
				if (ret != RT_ERR_OK)
				{
			goto ERROR;
		}
	}

	// (A3) set GPHY patch done bit
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
	osal_printf("\r\n (A3) set GPHY patch done bit");
#endif
	REG32(0xBB000114) |=  (1<<0);

#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
	osal_printf("\r\n REG32(0xBB000114)=0x%x", REG32(0xBB000114));
	osal_printf("\r\n");
#endif

	// (A4) all ports reset PHY and restart AN
#if defined(APRO_GEN2_PATCH_GPHY_CHECK)
	osal_printf("\r\n (A4) all ports reset PHY and restart AN");
#endif
	HAL_SCAN_ALL_ETH_PORT(port)
	{
		if (port >= PHY_K_PORT_NUM)
			continue;

			// set lock main
		ret = Lan_eth_miim_clear_write(port, 0xA40, 0, 15, 15, 0x1);
			if (ret != RT_ERR_OK)
			{
				goto ERROR;
			}
		ret = Lan_eth_miim_clear_write(port, 0xA40, 0, 12, 12, 0x1);
		if (ret != RT_ERR_OK)
		{
			goto ERROR;
		}
		ret = Lan_eth_miim_clear_write(port, 0xA40, 0, 9, 9, 0x1);
		if (ret != RT_ERR_OK)
	{
			goto ERROR;
		}
	}

	return RT_ERR_OK;

ERROR:
	return RT_ERR_NOT_FINISH;
}

static int32 _dal_rtl9607c_gen2_switch_phyCal(void)
{
	int ret;
	uint32 value;
	rtk_port_t port;

	/* Reset PHY */
	value = 1;
	if((ret = reg_field_write(RTL9607C_SOFTWARE_RSTr, RTL9607C_CMD_GPHY_RST_PSf, &value)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
		osal_printf("PHY reset fail\n");
	}
        osal_time_mdelay(20);
	for(port = 0;port <= PHY_K_PORT_NUM;port ++)
	{
		if(!HAL_IS_PHY_EXIST(port) || !HAL_IS_PORT_EXIST(port) || HAL_IS_PON_PORT(port))
		{
			continue;
		}

		if((ret = hal_miim_read(port, 0, 0, &value)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
		}
		if(0x1940 != value)
		{
			osal_printf("port %u reg 0 != 0x1940 0x%04x\n", port, value);
		}
	}

	/* Recovery patch (EXT_INI state) */
	if((ret=Lan_eth_change_default_apro_gen2())!=RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "Lan_eth_change_default_apro_gen2 patch fail, ret=0x%x\n",ret);
		return ret;
	}
        osal_time_mdelay(20);

	/* Set power down and enter LAN_ON state */
	HAL_SCAN_ALL_PORT(port)
	{
		if(!HAL_IS_PHY_EXIST(port))
		{
			continue;
		}
		if((ret = hal_miim_read(port, 0, 0, &value)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
		}
		value &= ~(1<<11);
		if((ret = hal_miim_write(port, 0, 0, value)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
		}
	}

	/* Clear fatal error flag */
	value = 0x1f;
	if((ret = reg_field_write(RTL9607C_CRASH_INTR_STS_0r, RTL9607C_PHY_FATAL_ERRf, &value)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
	}
	if((ret = reg_field_read(RTL9607C_CRASH_INTR_STS_0r, RTL9607C_PHY_FATAL_ERRf, &value)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
	}
	if(value)
	{
		osal_printf("PHY fatal error flag not cleared %u\n", value);
	}

	/* Recovery patch (LAN_ON state) */
	/* default patch */
	if((ret=_dal_rtl9607c_switch_default_patch())!=RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "default patch fail, ret=0x%x\n",ret);
		return ret;
	}
	/* power saving feature */
	if((ret=_dal_rtl9607c_switch_powerSaving_init())!=RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
		return ret;
	}

	return RT_ERR_OK;
}

static int32 _dal_rtl9607c_addition_phyPatch(void)
{
	int ret;
	uint32 value;
	rtk_port_t port;

	HAL_SCAN_ALL_PORT(port)
	{
		if(!HAL_IS_PHY_EXIST(port))
		{
			continue;
		}
		if((ret = hal_miim_read(port, 0xbcc, 16, &value)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
		}
		value &= ~(7<<8);
        value |= (5<<8);
		if((ret = hal_miim_write(port, 0xbcc, 16, value)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
		}
	}

    return RT_ERR_OK;
}

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      dal_rtl9607c_switch_init
 * Description:
 *      Initialize switch module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Module must be initialized before using all of APIs in this module
 */
int32
dal_rtl9607c_switch_init(void)
{
    int32 ret;
    uint32 wData;
    rtk_port_t port;
    uint32 chip, rev, subtype;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    switch_init = INIT_COMPLETED;

#if defined(CONFIG_SDK_KERNEL_LINUX)
    /* Init all necessary IPs before all initializations */
    _dal_rtl9607c_switch_ipEnable_set();
#endif

    /*flow control threshold and set buffer mode*/
    if((ret = rtl9607c_raw_flowctrl_patch(FLOWCTRL_PATCH_DEFAULT)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /*meter tick-token configuration*/
    if((ret = _dal_rtl9607c_switch_tickToken_init())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* Make sure all the ports are link down before any PHY related operation */
    if ((ret = _dal_rtl9607c_switch_phyPower_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /*set switch ready, phy patch done*/
    wData = 1;
    if ((ret = reg_field_write(RTL9607C_WRAP_GPHY_MISCr,RTL9607C_PATCH_PHY_DONEf,&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    if((ret = dal_rtl9607c_switch_version_get(&chip, &rev, &subtype)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#if defined(FPGA_DEFINED)

#else
#if defined(RTL_CYGWIN_EMULATE)

#else
    /*analog patch*/
    switch(rev)
    {
    case CHIP_REV_ID_B:
    /*phy patch*/
    if((ret=dal_rtl9607c_switch_phyPatch())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "phy patch fail\n");
        return ret;
    }
        /* phy en70 calibration patch */
        if((ret=_dal_rtl9607c_switch_phyEn70Cal())!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "phy en70 calibration fail, ret=0x%x\n",ret);
            return ret;
        }
        if((ret=_dal_rtl9607c_switch_mdiAmpVoltage())!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "phy mdi amp voltage fail, ret=0x%x\n",ret);
            return ret;
        }

        /* phy voltage patch - common_mode_patch_220908 */
        if((ret=_dal_rtl9607c_switch_phyVoltage())!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "phy voltage patch fail, ret=0x%x\n",ret);
            return ret;
        }
        break;
    case CHIP_REV_ID_0:
    case CHIP_REV_ID_A:
        /* phy calibration patch */
        if((ret=_dal_rtl9607c_switch_phyCal())!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "phy calibration fail, ret=0x%x\n",ret);
            return ret;
        }
        break;
    case CHIP_REV_ID_C:
	default:
		/* phy calibration patch */
        if((ret=_dal_rtl9607c_gen2_switch_phyCal())!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "phy calibration fail, ret=0x%x\n",ret);
            return ret;
        }

        /* Additional patches */
        if((ret=_dal_rtl9607c_addition_phyPatch())!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "phy additional patch, ret=0x%x\n",ret);
            return ret;
        }
        break;
    }

    /* default patch */
    if((ret=_dal_rtl9607c_switch_default_patch())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "default patch fail, ret=0x%x\n",ret);
        return ret;
    }

    /* power saving feature */
    if((ret=_dal_rtl9607c_switch_powerSaving_init())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#endif
#if defined(CONFIG_SWITCH_SYS_CLOCK_TUNE)
    /* switch clock tune */
    if((ret=_dal_rtl9607c_switch_sysClock_set())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#endif
    if(rev > CHIP_REV_ID_A)
    {
        /* Switch patch */
        wData = 1;
        if ((ret = reg_array_field_write(RTL9607C_DIVSYSCLK_TX_CTRLr, 0, REG_ARRAY_INDEX_NONE, RTL9607C_DLY_TXCOMf, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        if ((ret = reg_array_field_write(RTL9607C_DIVSYSCLK_TX_CTRLr, 1, REG_ARRAY_INDEX_NONE, RTL9607C_DLY_TXCOMf, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        if ((ret = reg_array_field_write(RTL9607C_DIVSYSCLK_TX_CTRLr, 2, REG_ARRAY_INDEX_NONE, RTL9607C_DLY_TXCOMf, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        if ((ret = reg_array_field_write(RTL9607C_DIVSYSCLK_TX_CTRLr, 3, REG_ARRAY_INDEX_NONE, RTL9607C_DLY_TXCOMf, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        if ((ret = reg_array_field_write(RTL9607C_DIVSYSCLK_TX_CTRLr, 4, REG_ARRAY_INDEX_NONE, RTL9607C_DLY_TXCOMf, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        if ((ret = reg_array_field_write(RTL9607C_DIVSYSCLK_TX_CTRLr, 8, REG_ARRAY_INDEX_NONE, RTL9607C_DLY_TXCOMf, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        if ((ret = reg_array_field_write(RTL9607C_DIVSYSCLK_TX_CTRLr, 9, REG_ARRAY_INDEX_NONE, RTL9607C_DLY_TXCOMf, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        if ((ret = reg_array_field_write(RTL9607C_DIVSYSCLK_TX_CTRLr, 10, REG_ARRAY_INDEX_NONE, RTL9607C_DLY_TXCOMf, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
    }
#if defined(CONFIG_SWITCH_INIT_LINKDOWN)
    /* Turn off phy power*/
    if ((ret = _dal_rtl9607c_switch_phyPower_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#else
    /*turn on phy power*/
    if ((ret = _dal_rtl9607c_switch_phyPower_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#endif

#endif
    wData = 1;
    if ((ret = reg_field_write(RTL9607C_CFG_UNHIOLr, RTL9607C_IPG_COMPENSATIONf, (uint32 *)&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    wData = 1;
    if ((ret = reg_field_write(RTL9607C_SDS_REG7r, RTL9607C_SP_CFG_NEG_CLKWR_A2Df, (uint32 *)&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }


    /*force CPU port accept small packet and enable padding*/
    HAL_SCAN_ALL_PORT(port)
    {
        if(HAL_IS_CPU_PORT(port))
        {
            wData = 1;
            if ((ret = reg_array_field_write(RTL9607C_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9607C_RX_SPCf, &wData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }

            wData = 1;
            if ((ret = reg_array_field_write(RTL9607C_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9607C_PADDING_ENf, &wData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
        }
    }
#if defined(CONFIG_SDK_KERNEL_LINUX)
    /*for thermal sensor init*/
    {
        uint32 temData,rData;
        /* set REG_A */
        wData = 0x081123d7;
        if ((ret = reg_field_write(RTL9607C_THERMAL_CTRL_5r, RTL9607C_REG_Af, (uint32 *)&wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        /* set REG_B */
        wData = 0x003718a3;
        if ((ret = reg_field_write(RTL9607C_THERMAL_CTRL_2r, RTL9607C_REG_Bf, (uint32 *)&wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        /*enable thermal meter*/
        #if 0
        wData = 0x1;
        if ((ret = reg_field_write(RTL9607C_THERMAL_CTRL_0r, RTL9607C_REG_PPOWf, (uint32 *)&wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        #endif
        wData = 0x3446c3a4;
        if ((ret = reg_write(RTL9607C_THERMAL_CTRL_0r, (uint32 *)&wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        

        /*get efuse calibration value*/
        if ((ret = dal_rtl9607c_tool_get_patch_info(253, &rData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }    
        
        //rData = 0x9fb1;
            
        if(rData&(1<<15))
        {
            efuseThermalData.valid = 1;
            if(rData&(1<<12))
            {
                efuseThermalData.positive=0;
            }
            else
            {
                efuseThermalData.positive=1;
            }

            /*get interger bit 11~0 2's complement*/
            rData = rData&0xfff;
            rData = ~rData+1;
            rData = rData&0xfff; 

            /*get interger bit 11~4*/
            temData = (rData & 0xff0)>>4;

            if(efuseThermalData.positive==0)
            {
                efuseThermalData.intPart = 0-(int16)(temData&0xff);
            }
            else
            {
                efuseThermalData.intPart = temData&0xff;
            }
            /*get float part*/
            efuseThermalData.floatPart=0;

            temData = rData & 0xf;

            if(temData&0x1)
                efuseThermalData.floatPart = efuseThermalData.floatPart+62500;
            if(temData&0x2)
                efuseThermalData.floatPart = efuseThermalData.floatPart+125000;
            if(temData&0x4)
                efuseThermalData.floatPart = efuseThermalData.floatPart+250000;
            if(temData&0x8)
                efuseThermalData.floatPart = efuseThermalData.floatPart+500000;



            if(efuseThermalData.positive==0)
            {
                efuseThermalData.floatPart = 0-efuseThermalData.floatPart;
            }
            //else
            //{
            //    efuseThermalData.floatPart = efuseThermalData.floatPart;
            //}
        }
        else
        {
            efuseThermalData.valid = 0;
        }
#if 0
        printf("valid:%d\n",efuseThermalData.valid);
        if(efuseThermalData.valid==1)
        {

            printf("positive:%d\n",efuseThermalData.positive);
            printf("intPart:%d\n",efuseThermalData.intPart);
            printf("floatPart:%d\n",efuseThermalData.floatPart);

        }
#endif
#if defined(CONFIG_SDK_KERNEL_LINUX)
    if(rev >= CHIP_REV_ID_C)
    {
        _dal_phy_recovery_init();
    }
#endif
    }/*for thermal sensor init*/
#endif

    return RT_ERR_OK;
} /* end of dal_rtl9607c_switch_init */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */


/* Function Name:
 *      dal_rtl9607c_switch_phyPortId_get
 * Description:
 *      Get physical port id from logical port name
 * Input:
 *      portName - logical port name
 * Output:
 *      pPortId  - pointer to the physical port id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Call RTK API the port ID must get from this API
 */
int32
dal_rtl9607c_switch_phyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
{
    dal_rtl9607c_switch_asic_type_t asicType;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPortId), RT_ERR_NULL_POINTER);

    /*get chip info to check port name mapping*/
    asicType = RTL9607C_ASIC_PON;

    switch(asicType)
    {
        case RTL9607C_ASIC_PON:
        default:
            return _dal_rtl9607c_switch_ponAsicPhyPortId_get(portName,pPortId);
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_switch_phyPortId_get */



/* Function Name:
 *      dal_rtl9607c_switch_logicalPort_get
 * Description:
 *      Get logical port name from physical port id
 * Input:
 *      portId  - physical port id
 * Output:
 *      pPortName - pointer to logical port name
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_rtl9607c_switch_logicalPort_get(int32 portId, rtk_switch_port_name_t *pPortName)
{
    dal_rtl9607c_switch_asic_type_t asicType;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortName), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(portId), RT_ERR_PORT_ID);

    /*get chip info to check port name mapping*/
    asicType = RTL9607C_ASIC_PON;

    switch(asicType)
    {
        case RTL9607C_ASIC_PON:
        default:
            return _dal_rtl9607c_switch_ponAsicLogicalPortName_get(portId,pPortName);
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_switch_logicalPort_get */



/* Function Name:
 *      dal_rtl9607c_switch_port2PortMask_set
 * Description:
 *      Set port id to the portlist
 * Input:
 *      pPortMask    - port mask
 *      portName     - logical port name
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Call RTK API the port mask must set by this API
 */
int32
dal_rtl9607c_switch_port2PortMask_set(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32 portId;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_rtl9607c_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;

    RTK_PORTMASK_PORT_SET(*pPortMask,portId);

    return RT_ERR_OK;
} /* end of dal_rtl9607c_switch_port2PortMask_set */



/* Function Name:
 *      dal_rtl9607c_switch_port2PortMask_clear
 * Description:
 *      Set port id to the portlist
 * Input:
 *      pPortMask    - port mask
 *      portName     - logical port name
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Call RTK API the port mask must set by this API
 */
int32
dal_rtl9607c_switch_port2PortMask_clear(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32   ret;
    int32 portId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_rtl9607c_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;

    RTK_PORTMASK_PORT_CLEAR(*pPortMask, portId);

    return RT_ERR_OK;
} /* end of dal_rtl9607c_switch_port2PortMask_clear */



/* Function Name:
 *      dal_rtl9607c_switch_portIdInMask_check
 * Description:
 *      Check if given port is in port list
 * Input:
 *      pPortMask    - port mask
 *      portName     - logical port name
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_rtl9607c_switch_portIdInMask_check(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32   ret;
    int32 portId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_rtl9607c_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;

    if(RTK_PORTMASK_IS_PORT_SET(*pPortMask,portId))
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;

} /* end of dal_rtl9607c_switch_portIdInMask_check */

#if 0
/* Function Name:
  *      dal_rtl9607c_switch_maxPktLenLinkSpeed_get
  * Description:
  *      Get the max packet length setting of the specific speed type
  * Input:
  *      speed - speed type
  * Output:
  *      pLen  - pointer to the max packet length
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_NULL_POINTER - input parameter may be null pointer
  *      RT_ERR_INPUT        - invalid enum speed type
  * Note:
  *      Max packet length setting speed type
  *      - MAXPKTLEN_LINK_SPEED_FE
  *      - MAXPKTLEN_LINK_SPEED_GE
  */
int32
dal_rtl9607c_switch_maxPktLenLinkSpeed_get(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen)
{
    int32   ret;
    uint32 regAddr;
    uint32 fieldIdx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((MAXPKTLEN_LINK_SPEED_END <= speed), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);

    if(speed == MAXPKTLEN_LINK_SPEED_FE)
    {
        regAddr =  RTL9607C_MAX_LENGTH_CFG0r;
        fieldIdx = RTL9607C_ACCEPT_MAX_LENTH_CFG0f;
    }
    else
    {
        regAddr =  RTL9607C_MAX_LENGTH_CFG1r;
        fieldIdx = RTL9607C_ACCEPT_MAX_LENTH_CFG1f;
    }

    if ((ret = reg_field_read(regAddr, fieldIdx, pLen)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_rtl9607c_switch_maxPktLenLinkSpeed_get */

/* Function Name:
  *      dal_rtl9607c_switch_maxPktLenLinkSpeed_set
  * Description:
  *      Set the max packet length of the specific speed type
  * Input:
  *      speed - speed type
  *      len   - max packet length
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_INPUT   - invalid enum speed type
  * Note:
  *      Max packet length setting speed type
  *      - MAXPKTLEN_LINK_SPEED_FE
  *      - MAXPKTLEN_LINK_SPEED_GE
  */
int32
dal_rtl9607c_switch_maxPktLenLinkSpeed_set(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len)
{
    rtk_port_t  port, max_port;
    int32   ret;
    uint32 regAddr;
    uint32 fieldIdx;
    uint32 portField;
    uint32 index;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((MAXPKTLEN_LINK_SPEED_END <= speed), RT_ERR_INPUT);
    RT_PARAM_CHK((RTL9607C_PACEKT_LENGTH_MAX < len), RT_ERR_INPUT);

    if(speed == MAXPKTLEN_LINK_SPEED_FE)
    {
        regAddr =  RTL9607C_MAX_LENGTH_CFG0r;
        fieldIdx = RTL9607C_ACCEPT_MAX_LENTH_CFG0f;
        portField = RTL9607C_MAX_LENGTH_10_100f;
        index = 0;
    }
    else
    {
        regAddr   = RTL9607C_MAX_LENGTH_CFG1r;
        fieldIdx  = RTL9607C_ACCEPT_MAX_LENTH_CFG1f;
        portField = RTL9607C_MAX_LENGTH_GIGAf;
        index = 1;
    }

    if ((ret = reg_field_write(regAddr, fieldIdx, &len)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    /*set all port index to currect length index*/
    max_port = HAL_GET_MAX_PORT();
    for (port = 0; port <= max_port; port++)
    {
        if (!HAL_IS_PORT_EXIST(port))
        {
            continue;
        }

        if ((ret = reg_array_field_write(RTL9607C_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, portField, &index)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
            return ret;
        }
    }
    return RT_ERR_OK;
} /* end of dal_rtl9607c_switch_maxPktLenLinkSpeed_set */
#endif

/* Module Name    : Switch     */
/* Sub-module Name: Management address and vlan configuration */
/* Function Name:
 *      _rtl9607c_switch_macAddr_set
 * Description:
 *      Set switch mac address configurations
 * Input:
 *      pMacAddr - Switch mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - Success
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_INPUT                - Invalid input parameter
 * Note:
 *      None
 */
int32 _rtl9607c_switch_macAddr_set(rtk_mac_t *pMacAddr)
{
    int32 ret;
    uint32 i, tmp[6];

    if(pMacAddr == NULL)
        return RT_ERR_NULL_POINTER;

    for (i=0;i<ETHER_ADDR_LEN;i++)
        tmp[i] = pMacAddr->octet[i];


    if ((ret = reg_field_write(RTL9607C_SWITCH_MACr, RTL9607C_SWITCH_MAC5f, &tmp[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9607C_SWITCH_MACr, RTL9607C_SWITCH_MAC4f, &tmp[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9607C_SWITCH_MACr, RTL9607C_SWITCH_MAC3f, &tmp[2])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9607C_SWITCH_MACr, RTL9607C_SWITCH_MAC2f, &tmp[3])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_field_write(RTL9607C_SWITCH_MACr, RTL9607C_SWITCH_MAC1f, &tmp[4])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_field_write(RTL9607C_SWITCH_MACr, RTL9607C_SWITCH_MAC0f, &tmp[5])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}/*end of _rtl9607c_switch_macAddr_set*/

/* Function Name:
 *      _rtl9607c_switch_macAddr_get
 * Description:
 *      Get switch mac address configurations
 * Input:
 *      pMacAddr - Switch mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - Success
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_INPUT                - Invalid input parameter
 * Note:
 *      None
 */
int32 _rtl9607c_switch_macAddr_get(rtk_mac_t *pMacAddr)
{
    int32 ret;
    uint32 i, tmp[6];

    if ((ret = reg_field_read(RTL9607C_SWITCH_MACr, RTL9607C_SWITCH_MAC5f, &tmp[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9607C_SWITCH_MACr, RTL9607C_SWITCH_MAC4f, &tmp[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9607C_SWITCH_MACr, RTL9607C_SWITCH_MAC3f, &tmp[2])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9607C_SWITCH_MACr, RTL9607C_SWITCH_MAC2f, &tmp[3])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_field_read(RTL9607C_SWITCH_MACr, RTL9607C_SWITCH_MAC1f, &tmp[4])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_field_read(RTL9607C_SWITCH_MACr, RTL9607C_SWITCH_MAC0f, &tmp[5])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    for (i=0;i<ETHER_ADDR_LEN;i++)
        pMacAddr->octet[i] = tmp[i];

    return RT_ERR_OK;
}/*end of _rtl9607c_switch_macAddr_get*/



/* Function Name:
 *      dal_rtl9607c_switch_mgmtMacAddr_get
 * Description:
 *      Get MAC address of switch.
 * Input:
 *      None
 * Output:
 *      pMac - pointer to MAC address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9607c_switch_mgmtMacAddr_get(rtk_mac_t *pMac)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    if ((ret = _rtl9607c_switch_macAddr_get(pMac)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_switch_mgmtMacAddr_get */

/* Function Name:
 *      dal_rtl9607c_switch_mgmtMacAddr_set
 * Description:
 *      Set MAC address of switch.
 * Input:
 *      pMac - MAC address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9607c_switch_mgmtMacAddr_set(rtk_mac_t *pMac)
{

    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    if((pMac->octet[0] & BITMASK_1B) == 1)
        return RT_ERR_INPUT;

    if ((ret = _rtl9607c_switch_macAddr_set(pMac)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_switch_mgmtMacAddr_set */


/* Function Name:
 *      dal_rtl9607c_switch_chip_reset
 * Description:
 *      Reset switch chip
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9607c_switch_chip_reset(void){

    int32   ret;
    uint32 resetValue = 1;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    if ((ret = reg_field_write(RTL9607C_SOFTWARE_RSTr,RTL9607C_CMD_CHIP_RST_PSf,&resetValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rtl9607c_switch_version_get
 * Description:
 *      Get chip version
 * Input:
 *      pChipId    - chip id
 *      pRev       - revision id
 *      pSubtype   - sub type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 */
int32
dal_rtl9607c_switch_version_get(uint32 *pChipId, uint32 *pRev, uint32 *pSubtype)
{
    int32  ret;
    static uint8 is_init = 0;
    static uint32 chipId;
    static uint32 chipRev;
    static uint32 chipSubtype;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pChipId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRev), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSubtype), RT_ERR_NULL_POINTER);

    if(0 == is_init)
    {
        if (drv_swcore_cid_get(&chipId, &chipRev) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }

    #if defined(CONFIG_SDK_KERNEL_LINUX)
        if ((ret = dal_rtl9607c_tool_get_chipSubType(&chipSubtype)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        if(CHIP_REV_ID_C == chipRev)
        {
            switch(chipSubtype)
            {
            case RTL9607C_CHIP_SUB_TYPE_RTL9603C_VA6:
                chipSubtype = RTL9607C_CHIP_SUB_TYPE_RTL9606C_VA6;
                break;
            default:
                break;
            }
        }
    #else
        chipSubtype = RTL9607C_CHIP_SUB_TYPE_RTL9607CP;
    #endif
        is_init = 1;
    }

    /* function body */
    *pChipId = chipId;
    *pRev = chipRev;
    *pSubtype = chipSubtype;

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_switch_version_get */

/* Function Name:
  *      dal_rtl9607c_switch_maxPktLenByPort_get
  * Description:
  *      Get the max packet length setting of specific port
  * Input:
  *      port - speed type
  * Output:
  *      pLen - pointer to the max packet length
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_NULL_POINTER - input parameter may be null pointer
  *      RT_ERR_INPUT        - invalid enum speed type
  * Note:
  */
int32
dal_rtl9607c_switch_maxPktLenByPort_get(rtk_port_t port, uint32 *pLen)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_array_field_read(RTL9607C_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9607C_ACCEPT_MAX_LENTHf, pLen)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    return RT_ERR_OK;
} /*end of dal_rtl9607c_switch_maxPktLenByPort_get*/

/* Function Name:
  *      dal_rtl9607c_switch_maxPktLenByPort_set
  * Description:
  *      Set the max packet length of specific port
  * Input:
  *      port  - port
  *      len   - max packet length
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_INPUT   - invalid enum speed type
  * Note:
  */
int32
dal_rtl9607c_switch_maxPktLenByPort_set(rtk_port_t port, uint32 len)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTL9607C_PACEKT_LENGTH_MAX < len), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_array_field_write(RTL9607C_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9607C_ACCEPT_MAX_LENTHf, &len)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    return RT_ERR_OK;
}/*end of dal_rtl9607c_switch_maxPktLenByPort_set*/

/* Function Name:
  *      dal_rtl9607c_switch_changeDuplex_get
  * Description:
  *      Get change duplex function state
  * Input:
  *      enable   - change duplex state
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_INPUT   - invalid enum speed type
  * Note:
  */
int32
dal_rtl9607c_switch_changeDuplex_get(rtk_enable_t *pState)
{
    int     ret;
    uint32  value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if((ret = reg_field_read(RTL9607C_CHANGE_DUPLEX_CTRLr, RTL9607C_CFG_CHG_DUP_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    *pState = value;

    return RT_ERR_OK;
} /* end of dal_rtl9607c_switch_changeDuplex_get */

/* Function Name:
  *      dal_rtl9607c_switch_changeDuplex_set
  * Description:
  *      Set change duplex function state
  * Input:
  *      enable   - change duplex state
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_INPUT   - invalid enum speed type
  * Note:
  * This function only apply to local N-way enable but link
  * partner in force mode. In that way, the local link status
  * will be 100Mb/half duplex. This function will change
  * local link status to 100Mb/full duplex under specific
  * condition.
  */
int32
dal_rtl9607c_switch_changeDuplex_set(rtk_enable_t state)
{
    int     ret;
    uint32  value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((state >= RTK_ENABLE_END), RT_ERR_NULL_POINTER);

    value = (ENABLED == state) ? 1 : 0;
    if((ret = reg_field_write(RTL9607C_CHANGE_DUPLEX_CTRLr, RTL9607C_CFG_CHG_DUP_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_switch_changeDuplex_set */

/* Function Name:
  *      dal_rtl9607c_switch_system_init
  * Description:
  *      Set system application initial
  * Input:
  *      mode   - initial mode
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  * Note:
  *     This function is used for SFU application system
  */
int32
dal_rtl9607c_switch_system_init(rtk_switch_system_mode_t *pMode)
{
    int32   ret,qid;
    rtk_acl_field_entry_t fieldSelect;
    rtk_acl_template_t    aclTemplate;
    rtk_qos_pri2queue_t pri2qid;
    rtk_acl_ingress_entry_t aclRule;
    rtk_acl_field_t aclField,aclField2,aclField3;
    rtk_qos_priSelWeight_t weightOfPriSel;
    rtk_portmask_t          allPmExcludeCpu;

    if(pMode->initDefault)
    {
    rt_log_printf("initDefaultinitDefaultinitDefaultinitDefault\n");
        if ((ret = dal_rtl9607c_acl_igrState_set(0, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            rt_log_printf("dal_rtl9607c_acl_igrState_set failed... 111\n");
            return ret;
        }

        if ((ret = dal_rtl9607c_acl_igrState_set(3, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            rt_log_printf("dal_rtl9607c_acl_igrState_set failed... 222\n");
            return ret;
        }

        /*make sure ACL priority is highest*/
        if ((ret = dal_rtl9607c_qos_priSelGroup_get(0, &weightOfPriSel)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            rt_log_printf("dal_rtl9607c_qos_priSelGroup_get failed... 111\n");
            return ret;
        }

        weightOfPriSel.weight_of_acl = HAL_PRI_SEL_WEIGHT_MAX();
        if ((ret = dal_rtl9607c_qos_priSelGroup_set(0, &weightOfPriSel)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            rt_log_printf("dal_rtl9607c_qos_priSelGroup_get failed... 222\n");
            return ret;
        }

        /*set CPU port used pri*/
        if ((ret = dal_rtl9607c_qos_portPriMap_set(3, 3)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            rt_log_printf("dal_rtl9607c_qos_portPriMap_set failed... 111\n");
            return ret;
        }

        pri2qid.pri2queue[7] = 7;
        pri2qid.pri2queue[6] = 2;
        pri2qid.pri2queue[5] = 2;
        pri2qid.pri2queue[4] = 2;
        pri2qid.pri2queue[3] = 2;
        pri2qid.pri2queue[2] = 2;
        pri2qid.pri2queue[1] = 1;
        pri2qid.pri2queue[0] = 0;

        if ((ret = dal_rtl9607c_qos_priMap_set(3, &pri2qid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            rt_log_printf("dal_rtl9607c_qos_priMap_set failed... 111\n");
            return ret;
        }

        /*set CPU port APR for Q0~Q6 rate control with meter 7, exclude OMCI/OAM QID 7 usage*/
        for(qid =0 ; qid <= 6; qid++)
        {
            if ((ret = dal_rtl9607c_rate_egrQueueBwCtrlEnable_set(9,qid,ENABLED)) != RT_ERR_OK)
            {
                return ret;
            }
            /*point to real meter index of CPU port*/
            if ((ret = dal_rtl9607c_rate_egrQueueBwCtrlMeterIdx_set(9,qid,7)) != RT_ERR_OK)
            {
                return ret;
            }
        }

        /*set meter 31(port 9 offset index 7) to to 5000 PPS*/
        if ((ret = dal_rtl9607c_rate_shareMeterMode_set(31, METER_MODE_PACKET_RATE)) != RT_ERR_OK)
        {
            return ret;
        }

        if ((ret = dal_rtl9607c_rate_shareMeter_set(31, 5000, ENABLED)) != RT_ERR_OK)
        {
            return ret;
        }

        /*set CPU egress port for drop tail only*/
        for(qid = 0 ; qid <= 7; qid++)
        {
            if ((ret = rtl9607c_raw_flowctrl_egressDropEnable_set(9,qid ,ENABLED)) != RT_ERR_OK)
            {
                rt_log_printf("rtl9607c_raw_flowctrl_egressDropEnable_set failed... 111\n");
                return ret;
            }
        }

        /*ACL user defined field 13/14 for LAN side unknown ARP*/
        fieldSelect.index = 2;
        fieldSelect.format = ACL_FORMAT_RAW;
        fieldSelect.offset = 40;
        if ((ret = dal_rtl9607c_acl_fieldSelect_set(&fieldSelect)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            rt_log_printf("dal_rtl9607c_acl_fieldSelect_set failed... 111\n");
            return ret;
        }

        fieldSelect.index = 3;
        fieldSelect.format = ACL_FORMAT_RAW;
        fieldSelect.offset = 38;
        if ((ret = dal_rtl9607c_acl_fieldSelect_set(&fieldSelect)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        /*ACL user defined field 11/12 for WAN side unknown ARP with Ctag | Stag*/
        fieldSelect.index = 0;
        fieldSelect.format = ACL_FORMAT_RAW;
        fieldSelect.offset = 44;
        if ((ret = dal_rtl9607c_acl_fieldSelect_set(&fieldSelect)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        fieldSelect.index = 1;
        fieldSelect.format = ACL_FORMAT_RAW;
        fieldSelect.offset = 42;
        if ((ret = dal_rtl9607c_acl_fieldSelect_set(&fieldSelect)) != RT_ERR_OK )
        {
            rt_log_printf("dal_rtl9607c_acl_fieldSelect_set failed... \n");
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        aclTemplate.index = 3;
        aclTemplate.fieldType[0] = ACL_FIELD_SMAC0;
        aclTemplate.fieldType[1] = ACL_FIELD_SMAC1;
        aclTemplate.fieldType[2] = ACL_FIELD_SMAC2;
        aclTemplate.fieldType[3] = ACL_FIELD_USER_DEFINED00;
        aclTemplate.fieldType[4] = ACL_FIELD_USER_DEFINED01;
        aclTemplate.fieldType[5] = ACL_FIELD_USER_DEFINED02;
        aclTemplate.fieldType[6] = ACL_FIELD_USER_DEFINED03;
        aclTemplate.fieldType[7] = ACL_FIELD_USER_DEFINED04;

        if ((ret = dal_rtl9607c_acl_template_set(&aclTemplate)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            rt_log_printf("dal_rtl9607c_acl_fieldSelect_set failed... \n");
            return ret;
        }

        aclTemplate.index = 2;
        aclTemplate.fieldType[0] = ACL_FIELD_DMAC0;
        aclTemplate.fieldType[1] = ACL_FIELD_DMAC1;
        aclTemplate.fieldType[2] = ACL_FIELD_DMAC2;
        aclTemplate.fieldType[3] = ACL_FIELD_IPV4_DIP0;
        aclTemplate.fieldType[4] = ACL_FIELD_IPV4_DIP1;
        aclTemplate.fieldType[5] = ACL_FIELD_ETHERTYPE;
        aclTemplate.fieldType[6] = ACL_FIELD_CTAG;
        aclTemplate.fieldType[7] = ACL_FIELD_USER_DEFINED04;

        if ((ret = dal_rtl9607c_acl_template_set(&aclTemplate)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        /*Broadcast, multicast and unicast flooding port mask exclude CPU port*/
        HAL_GET_ALL_PORTMASK(allPmExcludeCpu);
        RTK_PORTMASK_PORT_CLEAR(allPmExcludeCpu, HAL_GET_CPU_PORT());

        if ((ret = dal_rtl9607c_l2_lookupMissFloodPortMask_set(DLF_TYPE_BCAST, &allPmExcludeCpu)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        if ((ret = dal_rtl9607c_l2_lookupMissFloodPortMask_set(DLF_TYPE_UCAST, &allPmExcludeCpu)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        if ((ret = dal_rtl9607c_l2_lookupMissFloodPortMask_set(DLF_TYPE_IPMC, &allPmExcludeCpu)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        if ((ret = dal_rtl9607c_l2_lookupMissFloodPortMask_set(DLF_TYPE_IP6MC, &allPmExcludeCpu)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        if ((ret = dal_rtl9607c_l2_lookupMissFloodPortMask_set(DLF_TYPE_MCAST, &allPmExcludeCpu)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        pMode->sysUsedAclNum = RTL9607C_SYS_ACL_IDX_MAX;

    }

    /*ACL 1*/
    /*Trap IGMP priority for qid and set ACL Priority for NIC ring ID*/
    if (pMode->initIgmpSnooping)
    {
        memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
        memset(&aclField, 0x00, sizeof(rtk_acl_field_t));

        aclField.fieldType                      = ACL_FIELD_USER_DEFINED04;
        aclField.fieldUnion.pattern.data.value  = 0x0040;
        aclField.fieldUnion.pattern.data.mask   = 0x0040;
        aclField.next                           = NULL;

        aclRule.index                           = RTL9607C_SYS_ACL_IDX_IGMP;
        aclRule.pFieldHead                      = &aclField;
        aclRule.templateIdx                     = 3;
        aclRule.activePorts.bits[0]             = 0x1f;
        aclRule.valid                           = ENABLED;
        aclRule.act.enableAct[ACL_IGR_PRI_ACT]  = ENABLED;
        aclRule.act.priAct.act                  = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
        aclRule.act.priAct.aclPri               = 2;

        if ((ret = dal_rtl9607c_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }
    }

    /*ACL 2*/
    /*Trap RLDP pkt with priority 1*/
    if (pMode->initRldp)
    {
        memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
        memset(&aclField, 0x00, sizeof(rtk_acl_field_t));
        memset(&aclField2, 0x00, sizeof(rtk_acl_field_t));

        aclField.fieldType                          = ACL_FIELD_SMAC;
        osal_memcpy(&aclField.fieldUnion.mac.value.octet, pMode->macLan.octet, ETHER_ADDR_LEN);
        osal_memset(&aclField.fieldUnion.mac.mask.octet, 0xFF, ETHER_ADDR_LEN);
        aclField.next                               = &aclField2;

        aclField2.fieldType                         = ACL_FIELD_USER_DEFINED04;
        aclField2.fieldUnion.pattern.data.value     = 0x0400;
        aclField2.fieldUnion.pattern.data.mask      = 0x0400;
        aclField2.next                              = NULL;

        aclRule.index                               = RTL9607C_SYS_ACL_IDX_RLDP;
        aclRule.pFieldHead                          = &aclField;
        aclRule.templateIdx                         = 3;
        aclRule.activePorts.bits[0]                 = 0xf;
        aclRule.valid                               = ENABLED;
        aclRule.act.enableAct[ACL_IGR_FORWARD_ACT]  = ENABLED;
        aclRule.act.forwardAct.act                  = ACL_IGR_FORWARD_TRAP_ACT;
        aclRule.act.enableAct[ACL_IGR_PRI_ACT]      = ENABLED;
        aclRule.act.priAct.act                      = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
        aclRule.act.priAct.aclPri                   = 1;

        if ((ret = dal_rtl9607c_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }
    }

    /*ACL 3-4*/
    if (pMode->initLan)
    {
        /*Copy all ARP for lan interface to Q0*/
        memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
        memset(&aclField, 0x00, sizeof(rtk_acl_field_t));
        memset(&aclField2, 0x00, sizeof(rtk_acl_field_t));
        memset(&aclField3, 0x00, sizeof(rtk_acl_field_t));

        aclField.fieldType                          = ACL_FIELD_USER_DEFINED02;
        aclField.fieldUnion.pattern.data.value      = pMode->ipLan & 0xFFFF;
        aclField.fieldUnion.pattern.data.mask       = 0xFFFF;
        aclField.next                               = &aclField2;

        aclField2.fieldType                         = ACL_FIELD_USER_DEFINED03;
        aclField2.fieldUnion.pattern.data.value     = (pMode->ipLan >> 16) & 0xFFFF;
        aclField2.fieldUnion.pattern.data.mask      = 0xFFFF;
        aclField2.next                              = &aclField3;

        aclField3.fieldType                         = ACL_FIELD_USER_DEFINED04;
        aclField3.fieldUnion.pattern.data.value     = 0x0004;
        aclField3.fieldUnion.pattern.data.mask      = 0x0004;
        aclField3.next = NULL;

        aclRule.index                               = RTL9607C_SYS_ACL_IDX_ARP_LAN;
        aclRule.pFieldHead                          = &aclField;
        aclRule.templateIdx                         = 3;
        aclRule.activePorts.bits[0]                 = 0xf;
        aclRule.valid                               = ENABLED;
        aclRule.act.enableAct[ACL_IGR_FORWARD_ACT]  = ENABLED;
        aclRule.act.forwardAct.act                  = ACL_IGR_FORWARD_TRAP_ACT;
        aclRule.act.enableAct[ACL_IGR_PRI_ACT]      = ENABLED;
        aclRule.act.priAct.act                      = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
        aclRule.act.priAct.aclPri                   = 0;

        if ((ret = dal_rtl9607c_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        /*Assign Lan interface unicast for QID 2*/
        memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
        memset(&aclField, 0x00, sizeof(rtk_acl_field_t));

        aclField.fieldType                          = ACL_FIELD_DMAC;
        osal_memcpy(&aclField.fieldUnion.mac.value.octet, pMode->macLan.octet, ETHER_ADDR_LEN);
        osal_memset(&aclField.fieldUnion.mac.mask.octet, 0xFF, ETHER_ADDR_LEN);
        aclField.next                               = NULL;

        aclRule.index                               = RTL9607C_SYS_ACL_IDX_LAN_ADDR;
        aclRule.pFieldHead                          = &aclField;
        aclRule.templateIdx                         = 2;
        aclRule.activePorts.bits[0]                 = 0xf;
        aclRule.valid                               = ENABLED;
        aclRule.act.enableAct[ACL_IGR_PRI_ACT]      = ENABLED;
        aclRule.act.priAct.act                      = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
        aclRule.act.priAct.aclPri                   = 2;

        if ((ret = dal_rtl9607c_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
        memset(&aclField, 0x00, sizeof(rtk_acl_field_t));

        aclField.fieldType                          = ACL_FIELD_IPV6_DIP;
        aclField.fieldUnion.ip6.value.ipv6_addr[0]  = 0xFF;
        aclField.fieldUnion.ip6.value.ipv6_addr[1]  = 0x02;
        osal_memset(&aclField.fieldUnion.ip6.mask, 0xFF, 8);

        aclRule.index                               = RTL9607C_SYS_ACL_IDX_RA;
        aclRule.pFieldHead                          = &aclField;
        aclRule.templateIdx                         = 6;
        aclRule.activePorts.bits[0]                 = 0x20;
        aclRule.valid                               = ENABLED;
        aclRule.act.enableAct[ACL_IGR_FORWARD_ACT]  = ENABLED;
        aclRule.act.forwardAct.act                  = RTL9607C_DAL_ACL_IGR_FORWARD_REDIRECT_ACT;
        aclRule.act.forwardAct.portMask.bits[0]     = 0x12f;

        if ((ret = dal_rtl9607c_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }
    }

    /*ACL 5-8*/
    /*Copy all ARP for WAN interface to Q0*/
    if (pMode->initWan)
    {
        memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
        memset(&aclField, 0x00, sizeof(rtk_acl_field_t));
        memset(&aclField2, 0x00, sizeof(rtk_acl_field_t));
        memset(&aclField3, 0x00, sizeof(rtk_acl_field_t));

        aclField.fieldType                              = ACL_FIELD_USER_DEFINED02;
        aclField.fieldUnion.pattern.data.value          = pMode->ipWan & 0xFFFF;
        aclField.fieldUnion.pattern.data.mask           = 0xFFFF;
        aclField.next                                   = &aclField2;

        aclField2.fieldType                             = ACL_FIELD_USER_DEFINED03;
        aclField2.fieldUnion.pattern.data.value         = (pMode->ipWan >> 16) & 0xFFFF;
        aclField2.fieldUnion.pattern.data.mask          = 0xFFFF;
        aclField2.next                                  = &aclField3;

        aclField3.fieldType                             = ACL_FIELD_USER_DEFINED04;
        aclField3.fieldUnion.pattern.data.value         = 0x0004;
        aclField3.fieldUnion.pattern.data.mask          = 0x0004;
        aclField3.next                                  = NULL;

        aclRule.index                                   = RTL9607C_SYS_ACL_IDX_ARP_WAN_UNTAG;
        aclRule.pFieldHead                              = &aclField;
        aclRule.templateIdx                             = 3;
        aclRule.activePorts.bits[0]                     = 0x10;
        aclRule.valid                                   = ENABLED;
        aclRule.act.enableAct[ACL_IGR_FORWARD_ACT]      = ENABLED;
        aclRule.act.forwardAct.act                      = ACL_IGR_FORWARD_TRAP_ACT;
        aclRule.act.enableAct[ACL_IGR_PRI_ACT]          = ENABLED;
        aclRule.act.priAct.act                          = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
        aclRule.act.priAct.aclPri                       = 0;

        if ((ret = dal_rtl9607c_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
        memset(&aclField, 0x00, sizeof(rtk_acl_field_t));
        memset(&aclField2, 0x00, sizeof(rtk_acl_field_t));
        memset(&aclField3, 0x00, sizeof(rtk_acl_field_t));

        aclField.fieldType                              = ACL_FIELD_USER_DEFINED00;
        aclField.fieldUnion.pattern.data.value          = pMode->ipWan & 0xFFFF;
        aclField.fieldUnion.pattern.data.mask           = 0xFFFF;
        aclField.next                                   = &aclField2;

        aclField2.fieldType                             = ACL_FIELD_USER_DEFINED01;
        aclField2.fieldUnion.pattern.data.value         = (pMode->ipWan >> 16) & 0xFFFF;
        aclField2.fieldUnion.pattern.data.mask          = 0xFFFF;
        aclField2.next                                  = &aclField3;

        aclField3.fieldType                             = ACL_FIELD_USER_DEFINED04;
        aclField3.fieldUnion.pattern.data.value         = 0x0004;
        aclField3.fieldUnion.pattern.data.mask          = 0x0004;
        aclField3.next                                  = NULL;

        aclRule.index                                   = RTL9607C_SYS_ACL_IDX_ARP_WAN_CTAG;
        aclRule.pFieldHead                              = &aclField;
        aclRule.templateIdx                             = 3;

        aclRule.careTag.tags[ACL_CARE_TAG_CTAG].value   = ENABLED;
        aclRule.careTag.tags[ACL_CARE_TAG_CTAG].mask    = ENABLED;
        aclRule.careTag.tags[ACL_CARE_TAG_STAG].value   = DISABLED;
        aclRule.careTag.tags[ACL_CARE_TAG_STAG].mask    = ENABLED;

        aclRule.activePorts.bits[0]                     = 0x10;
        aclRule.valid                                   = ENABLED;
        aclRule.act.enableAct[ACL_IGR_FORWARD_ACT]      = ENABLED;
        aclRule.act.forwardAct.act                      = ACL_IGR_FORWARD_TRAP_ACT;
        aclRule.act.enableAct[ACL_IGR_PRI_ACT]          = ENABLED;
        aclRule.act.priAct.act                          = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
        aclRule.act.priAct.aclPri                       = 0;

        if ((ret = dal_rtl9607c_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }

        memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
        memset(&aclField, 0x00, sizeof(rtk_acl_field_t));
        memset(&aclField2, 0x00, sizeof(rtk_acl_field_t));
        memset(&aclField3, 0x00, sizeof(rtk_acl_field_t));

        aclField.fieldType                              = ACL_FIELD_USER_DEFINED00;
        aclField.fieldUnion.pattern.data.value          = pMode->ipWan & 0xFFFF;
        aclField.fieldUnion.pattern.data.mask           = 0xFFFF;
        aclField.next                                   = &aclField2;

        aclField2.fieldType                             = ACL_FIELD_USER_DEFINED01;
        aclField2.fieldUnion.pattern.data.value         = (pMode->ipWan >> 16) & 0xFFFF;
        aclField2.fieldUnion.pattern.data.mask          = 0xFFFF;
        aclField2.next                                  = &aclField3;

        aclField3.fieldType                             = ACL_FIELD_USER_DEFINED04;
        aclField3.fieldUnion.pattern.data.value         = 0x0004;
        aclField3.fieldUnion.pattern.data.mask          = 0x0004;
        aclField3.next                                  = NULL;

        aclRule.index                                   = RTL9607C_SYS_ACL_IDX_ARP_WAN_STAG;
        aclRule.pFieldHead                              = &aclField;
        aclRule.templateIdx                             = 3;

        aclRule.careTag.tags[ACL_CARE_TAG_CTAG].value   = DISABLED;
        aclRule.careTag.tags[ACL_CARE_TAG_CTAG].mask    = ENABLED;
        aclRule.careTag.tags[ACL_CARE_TAG_STAG].value   = ENABLED;
        aclRule.careTag.tags[ACL_CARE_TAG_STAG].mask    = ENABLED;

        aclRule.activePorts.bits[0]                     = 0x10;
        aclRule.valid                                   = ENABLED;
        aclRule.act.enableAct[ACL_IGR_FORWARD_ACT]      = ENABLED;
        aclRule.act.forwardAct.act                      = ACL_IGR_FORWARD_TRAP_ACT;
        aclRule.act.enableAct[ACL_IGR_PRI_ACT]          = ENABLED;
        aclRule.act.priAct.act                          = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
        aclRule.act.priAct.aclPri                       = 0;

        if ((ret = dal_rtl9607c_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        /*Assign Wan interface unicast for QID 2*/
        memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
        memset(&aclField, 0x00, sizeof(rtk_acl_field_t));

        aclField.fieldType                              = ACL_FIELD_DMAC;
        osal_memcpy(&aclField.fieldUnion.mac.value.octet, pMode->macWan.octet, ETHER_ADDR_LEN);
        osal_memset(&aclField.fieldUnion.mac.mask.octet, 0xFF, ETHER_ADDR_LEN);
        aclField.next                                   = NULL;

        aclRule.index                                   = RTL9607C_SYS_ACL_IDX_WAN_ADDR;
        aclRule.pFieldHead                              = &aclField;
        aclRule.templateIdx                             = 2;
        aclRule.activePorts.bits[0]                     = 0x10;
        aclRule.valid                                   = ENABLED;
        aclRule.act.enableAct[ACL_IGR_PRI_ACT]          = ENABLED;
        aclRule.act.priAct.act                          = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
        aclRule.act.priAct.aclPri                       = 2;

        if ((ret = dal_rtl9607c_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}/* end of dal_rtl9607c_switch_system_init */

/* Function Name:
  *      dal_dal9607c_switch_thermal_get
  * Description:
  *      Get soc thermal value
  * Input:
  *      none
  * Output:
  *       thermalIntger  - pointer to thermal value for integer part
  *       thermalDecimal - pointer to thermal value for Decimal part
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_NULL_POINTER - input parameter may be null pointer
  *      RT_ERR_INPUT        - invalid enum speed type
  * Note:
  */
int32
dal_dal9607c_switch_thermal_get(int32 *thermalIntger, int32 *thermalDecimal)
{
    int32   ret;
    uint32  rData,tmpData,isPossive;
    int32   tmpInter;
    int32  tmpFloat;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == thermalIntger), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == thermalDecimal), RT_ERR_NULL_POINTER);

    /* function body */
    /*get from sensor*/
    if ((ret = reg_field_read(RTL9607C_THERMAL_STS_0r, RTL9607C_TM_SENSOR_OUTf, (uint32 *)&rData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    /* bit 0~18
       bit 18   : sign
       bit 17~10: interger part
       bit 9~0  : float part
    */
    rData = rData & 0x7ffff; 

    if(rData & 0x40000)
    {
        /*using 2's complement*/
        isPossive = 0;
        rData = rData& 0x3ffff;
        rData=~rData +1;
        rData = rData& 0x3ffff;
    }
    else
    {
        isPossive = 1;
        rData = rData& 0x3ffff;
    }

    tmpData = (rData & 0x3fc00)>>10;

    /*int part*/
    if(isPossive==0)
    {
        tmpInter = 0 - (int)tmpData;    
    }
    else
    {
        tmpInter = tmpData;  
    }
    /*float part*/
    tmpFloat = 0;
    tmpData = rData & 0x3ff;
    if(tmpData&0x1)
        tmpFloat = tmpFloat+977;
    if(tmpData&0x2)
        tmpFloat = tmpFloat+1953;
    if(tmpData&0x4)
        tmpFloat = tmpFloat+3906;
    if(tmpData&0x8)
        tmpFloat = tmpFloat+7813;
    if(tmpData&0x10)
        tmpFloat = tmpFloat+15625;
    if(tmpData&0x20)
        tmpFloat = tmpFloat+31250;
    if(tmpData&0x40)
        tmpFloat = tmpFloat+62500;
    if(tmpData&0x80)
        tmpFloat = tmpFloat+125000;
    if(tmpData&0x100)
        tmpFloat = tmpFloat+250000;
    if(tmpData&0x200)
        tmpFloat = tmpFloat+500000;


    if(isPossive==0)
    {
        tmpFloat = 0 - (int)tmpFloat;
    }
    else
    {
        tmpFloat = tmpFloat;
    }


    #if 0
        osal_printf("\nthermal reg:%d. %d\n",tmpInter,tmpFloat);
    #endif


    if(efuseThermalData.valid==0)
    {
        *thermalIntger = tmpInter;
        *thermalDecimal = tmpFloat;
    }
    else
    {

        //osal_printf("\nefuse thermal reg:%d. %d",efuseThermalData.intPart,efuseThermalData.floatPart);


        *thermalIntger = tmpInter+efuseThermalData.intPart;
        *thermalDecimal = tmpFloat+efuseThermalData.floatPart;


        if(*thermalDecimal >=1000000)
        {
            *thermalIntger=*thermalIntger+1;
            *thermalDecimal = *thermalDecimal - 1000000;
        }
        if(*thermalDecimal <=-1000000)
        {
            *thermalIntger=*thermalIntger-1;
            *thermalDecimal = *thermalDecimal + 1000000;
        }
    }

    /*round off to the 1st decimal place*/
    if(*thermalDecimal>=0)
    {

        if((*thermalDecimal%100000)>=50000)
        {
            *thermalDecimal=(*thermalDecimal/100000)*100000+100000;
        }
        else
        {
            *thermalDecimal=(*thermalDecimal/100000)*100000;
        }
    }
    else
    {
        if((*thermalDecimal%100000)<(-50000))
            *thermalDecimal=(*thermalDecimal/100000)*100000-100000;
        else
            *thermalDecimal=(*thermalDecimal/100000)*100000;

    }

    return RT_ERR_OK;
}   /* end of dal_dal9607c_switch_thermal_get */

#ifdef CONFIG_SDK_KERNEL_LINUX
static struct task_struct *pPhyRecover;

struct proc_dir_entry *phy_recovery_proc_dir = NULL;
struct proc_dir_entry *phy_recovery_cfg_proc = NULL;

static char recoveryPatchVer[] = "1.1";
static uint32 stopRecovery = 1;
static uint32 recoveryCnt = 0;

static ssize_t phy_recovery_cfg_write(struct file *file, const char *buffer, size_t count, loff_t *data)
{
	unsigned char tmpBuf[16] = {0};
    unsigned int value;
	int len = (count > 15) ? 15 : count;

	if (buffer && !copy_from_user(tmpBuf, buffer, len))
	{
		value = simple_strtoul(tmpBuf, NULL, 10);
		stopRecovery = value;
		printk("change stopRecovery to %u\n", stopRecovery);

		return count;
	}
	return -EFAULT;
}

static int phy_recovery_cfg_read(struct seq_file *seq, void *v)
{
	seq_printf(seq, "phy recovery version %s\n", recoveryPatchVer);
	seq_printf(seq, "stopRecovery = %u\n", stopRecovery);
	seq_printf(seq, "recoveryCnt = %u\n", recoveryCnt);

	return 0;
}

static int phy_recovery_cfg_open_proc(struct inode *inode, struct file *file)
{
	return single_open(file, phy_recovery_cfg_read, NULL);
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
struct proc_ops phy_recovery_cfg_fop = {
    .proc_open           = phy_recovery_cfg_open_proc,
    .proc_write          = phy_recovery_cfg_write,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = single_release,
};
#else
struct file_operations phy_recovery_cfg_fop = {
	.open = phy_recovery_cfg_open_proc,
	.write = phy_recovery_cfg_write,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};
#endif

static bool __dal_rtl9607c_switch_gphyError_check(void)
{
    int32 ret;
    uint32 value, port;

    if((ret = reg_field_read(RTL9607C_CRASH_INTR_STS_0r, RTL9607C_PHY_FATAL_ERRf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return FALSE;
    }
    if(value)
    {
        osal_printf("PHY fatal error flag %u\n", value);
        return TRUE;
    }

    for(port = 0; port <= PHY_K_PORT_NUM; port ++)
    {
        if(!HAL_IS_PHY_EXIST(port) || !HAL_IS_PORT_EXIST(port) || HAL_IS_PON_PORT(port))
        {
            continue;
        }

        if((ret = hal_miim_read(port, 0xb90, 19, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return FALSE;
        }
        if(value)
        {
            osal_printf("PHY fatal error reg 19 value %u on port %d\n", value, port);
            return TRUE;
        }

        if((ret = hal_miim_read(port, 0xb90, 20, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return FALSE;
        }
        if(value)
        {
            osal_printf("PHY fatal error reg 20 value %u on port %d\n", value, port);
            return TRUE;
        }
    }

    return FALSE;
}

static int phy_recovery_thread(void *data)
{
    bool needRecovery = FALSE;

    while(!kthread_should_stop())
    {
        /* It is NOT possible to wake up earlier */
        set_current_state(TASK_UNINTERRUPTIBLE);
        schedule_timeout(10 * HZ / 100); /* Wake up every 100 ms*/

		if(stopRecovery)
		{
			continue;
		}

        /* Scan each port for recovery */
        needRecovery = __dal_rtl9607c_switch_gphyError_check();
        if(needRecovery)
        {
            _dal_rtl9607c_gen2_switch_phyCal();
            recoveryCnt ++;
        }
    }

    return 0;
}

int32 _dal_phy_recovery_init(void)
{
    if(NULL == pPhyRecover)
    {
        struct task_struct *pKthread;

        pKthread = kthread_create(phy_recovery_thread, NULL, "phy_recovery");
        if(IS_ERR(pKthread))
        {
            printk("%s:%d phy recovery init failed %ld!\n", __FILE__, __LINE__, PTR_ERR(pKthread));
        }
        else
        {
            pPhyRecover = pKthread;
            wake_up_process(pPhyRecover);
            printk("phy recovery init complete! version %s\n", recoveryPatchVer);

			/* Create proc file for debug usage */
		    if(NULL == phy_recovery_proc_dir)
		    {
		        phy_recovery_proc_dir = proc_mkdir("phy_recovery", NULL);
		    }
			if(phy_recovery_proc_dir)
			{
		    	phy_recovery_cfg_proc = proc_create("stop_recovery", 0, phy_recovery_proc_dir, &phy_recovery_cfg_fop);
	        }
        }
    }

    return RT_ERR_OK;
}
#endif


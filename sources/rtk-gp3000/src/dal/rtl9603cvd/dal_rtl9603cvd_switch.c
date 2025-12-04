/*
 * Copyright (C) 2018 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: $
 * $Date: $
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

#include <dal/rtl9603cvd/dal_rtl9603cvd.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd_switch.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd_flowctrl.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd_rate.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd_tool.h>
#include <hal/common/miim.h>
#include <osal/time.h>
#include <hal/mac/mac_probe.h>

/*
 * Symbol Definition
 */

#if !defined(CONFIG_SDK_KERNEL_LINUX)
#define dal_rtl9603cvd_tool_get_patch_info(x, y) (0)
#endif

typedef struct dal_rtl9603cvd_phy_data_s
{
    uint16  phy;
    uint16  addr;
    uint16  data;
} dal_rtl9603cvd_phy_data_t;

/*
 * Data Declaration
 */
static uint32    switch_init = INIT_NOT_COMPLETED;
static dal_rtl9603cvd_phy_data_t  phyPatchArray[] = {
    /* Channel Estimation ver. 20190507 */
    {0x0003,0xa436,0x809a},
    {0x0003,0xa438,0x5db4},
    {0x0003,0xa436,0x809c},
    {0x0003,0xa438,0xd007},
    {0x0003,0xa436,0x809e},
    {0x0003,0xa438,0xa106},
    {0x0003,0xa436,0x80a0},
    {0x0003,0xa438,0x0a98},
    {0x0003,0xa436,0x80a2},
    {0x0003,0xa438,0x9819},
    {0x0003,0xa436,0x80a4},
    {0x0003,0xa438,0xd30c},
    {0x0003,0xa436,0x80ac},
    {0x0003,0xa438,0x2a84},
    {0x0003,0xa436,0x80ae},
    {0x0003,0xa438,0x0608},
    {0x0003,0xa436,0x80b0},
    {0x0003,0xa438,0x2106},
    {0x0003,0xa436,0x80b2},
    {0x0003,0xa438,0x0a6f},
    {0x0003,0xa436,0x80b4},
    {0x0003,0xa438,0x3519},
    {0x0003,0xa436,0x80b6},
    {0x0003,0xa438,0xa00c},
    {0x0003,0xa436,0x8088},
    {0x0003,0xa438,0x80ab},
    {0x0003,0xa436,0x808a},
    {0x0003,0xa438,0x870a},
    {0x0003,0xa436,0x808c},
    {0x0003,0xa438,0x1b09},
    {0x0003,0xa436,0x808e},
    {0x0003,0xa438,0x20a4},
    {0x0003,0xa436,0x8090},
    {0x0003,0xa438,0xc41f},
    {0x0003,0xa436,0x8092},
    {0x0003,0xa438,0x801e},

    /* Green Table initial part ver. 20181228 */
    {0x0003,0xa436,0x806d},
    {0x0003,0xa438,0x2444},
    {0x0003,0xa436,0x8071},
    {0x0003,0xa438,0x0823},
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

    /* RTCT Open/ Mismatch threshold ver. 20190507 */
    /* Open threshold */
    {0x0003,0xa436,0x8160},
    {0x0003,0xa438,0x3D0E},
    {0x0003,0xa436,0x8162},
    {0x0003,0xa438,0xBCC3},
    {0x0003,0xa436,0x8164},
    {0x0003,0xa438,0x366E},
    {0x0003,0xa436,0x8166},
    {0x0003,0xa438,0xE9AA},
    /* First mismatch threshold */
    {0x0003,0xa436,0x8174},
    {0x0003,0xa438,0x04FF},
    {0x0003,0xa436,0x8176},
    {0x0003,0xa438,0xEB52},
    {0x0003,0xa436,0x8178},
    {0x0003,0xa438,0x5094},
    {0x0003,0xa436,0x817A},
    {0x0003,0xa438,0x5D84},
    /* Second mismatch threshold */
    {0x0003,0xa436,0x8211},
    {0x0003,0xa438,0x157A},
    {0x0003,0xa436,0x8213},
    {0x0003,0xa438,0xD1B4},
    {0x0003,0xa436,0x8215},
    {0x0003,0xa438,0x4701},
    {0x0003,0xa436,0x8217},
    {0x0003,0xa438,0xC95B},

    /* FEPHY RTCT ver. 20190507 */
    {0x0000,0x3e,0x0002},
    {0x0000,0x20,0x2CCA},
    {0x0000,0x22,0x1465},
    {0x0000,0x24,0x9CC6},
    {0x0000,0x26,0x610F},
    {0x0000,0x28,0x0E42},
    {0x0000,0x2A,0x823B},
    {0x0000,0x2C,0x6111},
    {0x0000,0x2E,0x006A},
    {0x0000,0x30,0x2240},
    {0x0000,0x32,0x095E},
    {0x0000,0x34,0x8267},
    {0x0001,0x3e,0x0002},
    {0x0001,0x20,0x2CCA},
    {0x0001,0x22,0x1465},
    {0x0001,0x24,0x9CC6},
    {0x0001,0x26,0x610F},
    {0x0001,0x28,0x0E42},
    {0x0001,0x2A,0x823B},
    {0x0001,0x2C,0x6111},
    {0x0001,0x2E,0x006A},
    {0x0001,0x30,0x2240},
    {0x0001,0x32,0x095E},
    {0x0001,0x34,0x8267},
    {0x0002,0x3e,0x0002},
    {0x0002,0x20,0x2CCA},
    {0x0002,0x22,0x1465},
    {0x0002,0x24,0x9CC6},
    {0x0002,0x26,0x610F},
    {0x0002,0x28,0x0E42},
    {0x0002,0x2A,0x823B},
    {0x0002,0x2C,0x6111},
    {0x0002,0x2E,0x006A},
    {0x0002,0x30,0x2240},
    {0x0002,0x32,0x075E},
    {0x0002,0x34,0x8267},

    /* 10M EEE initial part ver. 20190408 */
    {0x0000,0x003e,0x0001},
    {0x0000,0x0024,0x3f80},
    {0x0000,0x0020,0xf548},
    {0x0001,0x003e,0x0001},
    {0x0001,0x0024,0x3f70},
    {0x0001,0x0020,0xf548},
    {0x0002,0x003e,0x0001},
    {0x0002,0x0024,0x3f60},
    {0x0002,0x0020,0xf548},

    /* 10M EEE initial part ver. 20190408 */
    {0x0003,0xa436,0x8047},
    {0x0003,0xa438,0x2733},
};

/*
 * Function Declaration
 */
int32 dal_rtl9603cvd_switch_phyPatch(void);
int32 _rtl9603cvd_switch_macAddr_set(rtk_mac_t *pMacAddr);
int32 _rtl9603cvd_switch_macAddr_get(rtk_mac_t *pMacAddr);

#if defined(CONFIG_SDK_KERNEL_LINUX)
static int32
_dal_rtl9603cvd_switch_ipEnable_set(void)
{
#if defined(FPGA_DEFINED)
#else
    int32  ret;
    uint32 value;

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
#endif
    return RT_ERR_OK;
}
#endif

static int32
_dal_rtl9603cvd_switch_ponAsicPhyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
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

        if((ret = dal_rtl9603cvd_tool_get_capability(&capability)) != RT_ERR_OK)
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
#endif
    case RTK_PORT_PON:
        *pPortId = 4;
        break;
    case RTK_PORT_CPU:
        *pPortId = 5;
        break;
    default:
        return RT_ERR_INPUT;
    }
        
    return RT_ERR_OK;
}

static int32
_dal_rtl9603cvd_switch_ponAsicLogicalPortName_get(int32 portId, rtk_switch_port_name_t *pPortName)
{

    switch(portId)
    {
#if defined(CONFIG_SDK_KERNEL_LINUX)
    case 0:
    case 1:
    case 2:
    case 3:
    {
        int ret;
        uint32 i, valid;
        uint32 capability = 0;

        if((ret = dal_rtl9603cvd_tool_get_capability(&capability)) != RT_ERR_OK)
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
        *pPortName = RTK_PORT_UTP0 + portId;
        break;
#endif
    case 4:
        *pPortName = RTK_PORT_PON;
        break;
    case 5:
        *pPortName = RTK_PORT_CPU;
        break;
    default:
        return RT_ERR_INPUT;
    }
        
    return RT_ERR_OK;
}


static int32
_dal_rtl9603cvd_switch_tickToken_init(void)
{
    int ret;
    uint32 wData, switchTick, switchToken;
    uint32 chip, rev, subtype;

    /*meter pon-tick-token configuration*/
    wData = 0x6e;
    if ((ret = reg_field_write(RTL9603CVD_PON_TB_CTRLr, RTL9603CVD_TICK_PERIODf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    wData = 0x95;
    if ((ret = reg_field_write(RTL9603CVD_PON_TB_CTRLr, RTL9603CVD_TKNf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    if((ret = dal_rtl9603cvd_switch_version_get(&chip, &rev, &subtype)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /*meter switch-tick-token configuration*/
    switchTick = 0x2b;
    switchToken = 0xbd;
    if ((ret = reg_field_write(RTL9603CVD_METER_TB_CTRLr, RTL9603CVD_TICK_PERIODf, &switchTick)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9603CVD_METER_TB_CTRLr, RTL9603CVD_TKNf, &switchToken)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



static int32
_dal_rtl9603cvd_switch_green_enable(void)
{
    return RT_ERR_OK;
}

static int32
_dal_rtl9603cvd_switch_eee_enable(void)
{
    /* Default enabled, no need to change */
    return RT_ERR_OK;
}

static int32
_dal_rtl9603cvd_switch_powerSaving_init(void)
{
    int ret;
    uint32 wData;

    /* === Green enable === */
    if ((ret = _dal_rtl9603cvd_switch_green_enable()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* === EEE enable === */
    if ((ret = _dal_rtl9603cvd_switch_eee_enable()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    /* Additional EEE config */
    wData = 0x20000;
    if ((ret = reg_array_write(RTL9603CVD_PHY_GDAC_IB_10Mr, REG_ARRAY_INDEX_NONE, 3, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

static int32
_dal_rtl9603cvd_switch_phyPower_set(rtk_enable_t enable)
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
        if ((ret = reg_write(RTL9603CVD_GPHY_IND_CMDr, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
            return ret;
        }
        osal_time_mdelay(10);
        if ((ret = reg_read(RTL9603CVD_GPHY_IND_RDr, &rData)) != RT_ERR_OK)
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
        if ((ret = reg_write(RTL9603CVD_GPHY_IND_WDr, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
            return ret;
        }
        wData = 0x60a400 | (i << 16);
        if ((ret = reg_write(RTL9603CVD_GPHY_IND_CMDr, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
            return ret;
        }
        osal_time_mdelay(10);
    }

    return RT_ERR_OK;
}

int32
dal_rtl9603cvd_switch_phyPatch(void)
{
    int i;
    int ret;
    dal_rtl9603cvd_phy_data_t *patchArray;
    uint32 patchSize;

    patchArray = phyPatchArray;
    patchSize = sizeof(phyPatchArray)/sizeof(dal_rtl9603cvd_phy_data_t);

    /*start patch phy*/
    for(i=0 ; i<patchSize ; i++)
    {
        if((ret=rtl9603cvd_ocpInterPhy_write(patchArray[i].phy,
                              patchArray[i].addr,
                              patchArray[i].data))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

#define CAL_PATCH_TIMEOUT 10000

static dal_rtl9603cvd_phy_data_t  nCtrlPatchArray[] = {
    /* NCtrl ver. 20190509 */
    {3, 0xa436, 0xB820},
    {3, 0xa438, 0x0090},
    {3, 0xa436, 0xA012},
    {3, 0xa438, 0x0000},
    {3, 0xa436, 0xA014},
    {3, 0xa438, 0x2c04},
    {3, 0xa438, 0x2c06},
    {3, 0xa438, 0x2c06},
    {3, 0xa438, 0x2c06},
    {3, 0xa438, 0xd08b},
    {3, 0xa438, 0x2267},
    {3, 0xa436, 0xA01A},
    {3, 0xa438, 0x0000},
    {3, 0xa436, 0xA006},
    {3, 0xa438, 0x0fff},
    {3, 0xa436, 0xA004},
    {3, 0xa438, 0x0fff},
    {3, 0xa436, 0xA002},
    {3, 0xa438, 0x0fff},
    {3, 0xa436, 0xA000},
    {3, 0xa438, 0x1266},
    {3, 0xa436, 0xB820},
    {3, 0xa438, 0x0010},
};

static int32
_dal_rtl9603cvd_switch_phyNCtrl(void)
{
    int32 ret;
    uint32 i, port, cnt, patchSize;
    uint16 value, reg0_bak;
    dal_rtl9603cvd_phy_data_t *patchArray;

    RT_LOG(LOG_DEBUG, (MOD_SWITCH|MOD_DAL), "switch nctrl start\n");

    /* Only port 3 need patch this */
    port = 3;

    /* Make sure the port is link down */
    if((ret = rtl9603cvd_ocpInterPhy_read(port, 0xa400, &reg0_bak)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xa400, reg0_bak | 0x800)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* Check PHY state */
    if((ret = rtl9603cvd_ocpInterPhy_read(port, 0xa46a, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    if((value & 0x700) != 0x500)
    {
        osal_printf("NCtrl patch failed (PHY state = 0x%x)\n", (value & 0x500) >> 8);
        return RT_ERR_FAILED;
    }

    /* Patch start */
    if((ret = rtl9603cvd_ocpInterPhy_read(port, 0xb820, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    value |= 0x0010;
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xb820, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    cnt = CAL_PATCH_TIMEOUT;
    while(cnt > 0)
    {
        if((ret = rtl9603cvd_ocpInterPhy_read(port, 0xb800, &value)) != RT_ERR_OK)
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
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xA436, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    value = 0x0800;
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xA438, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    value = 0xb82e;
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xa436, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    value = 0x0001;
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xa438, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* Patch code execution */
    patchArray = nCtrlPatchArray;
    patchSize = sizeof(nCtrlPatchArray)/sizeof(dal_rtl9603cvd_phy_data_t);
    for(i=0 ; i<patchSize ; i++)
    {
        if((ret=rtl9603cvd_ocpInterPhy_write(port,
                              patchArray[i].addr,
                              patchArray[i].data))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
    }

    value = 0xb82e;
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xa436, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    value = 0x0000;
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xa438, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    value = 0x8028;
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xa436, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    value = 0x0000;
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xa438, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* Patch end */
    if((ret = rtl9603cvd_ocpInterPhy_read(port, 0xb820, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    value &= ~(0x0010);
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xb820, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* Restore reg0 */
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xa400, reg0_bak)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

static dal_rtl9603cvd_phy_data_t voltagePatchArray[] = {
    {3, 0xa436, 0xA012},
    {3, 0xa438, 0x0000},
    {3, 0xa436, 0xA014},
    {3, 0xa438, 0x2C04},
    {3, 0xa438, 0x2C06},
    {3, 0xa438, 0x2C08},
    {3, 0xa438, 0x2C16},
    {3, 0xa438, 0xd08b},
    {3, 0xa438, 0x2267},
    {3, 0xa438, 0xc114},
    {3, 0xa438, 0x25cb},
    {3, 0xa438, 0x410e},
    {3, 0xa438, 0x15aa},
    {3, 0xa438, 0x15f1},
    {3, 0xa438, 0xd501},
    {3, 0xa438, 0xa103},
    {3, 0xa438, 0x8203},
    {3, 0xa438, 0xd500},
    {3, 0xa438, 0x206a},
    {3, 0xa438, 0x15f7},
    {3, 0xa438, 0xd501},
    {3, 0xa438, 0x8103},
    {3, 0xa438, 0xa203},
    {3, 0xa438, 0xd500},
    {3, 0xa438, 0x206a},
    {3, 0xa436, 0xA004},
    {3, 0xa438, 0x0065},
    {3, 0xa436, 0xA002},
    {3, 0xa438, 0x05ca},
    {3, 0xa436, 0xA000},
    {3, 0xa438, 0x7266}
};

static int32
_dal_rtl9603cvd_switch_phyVoltage(void)
{
    int32 ret;
    uint32 i, port, cnt, patchSize;
    uint16 value;
    dal_rtl9603cvd_phy_data_t *patchArray;

    RT_LOG(LOG_DEBUG, (MOD_SWITCH|MOD_DAL), "switch phyVoltage start\n");

    /* Only port 3 need patch this */
    port = 3;

    /* Patch start */
    if((ret = rtl9603cvd_ocpInterPhy_read(port, 0xb820, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    value |= 0x0010;
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xb820, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    cnt = CAL_PATCH_TIMEOUT;
    while(cnt > 0)
    {
        if((ret = rtl9603cvd_ocpInterPhy_read(port, 0xb800, &value)) != RT_ERR_OK)
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
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xA436, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    value = 0x0800;
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xA438, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    value = 0xb820;
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xa436, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    value = 0x0090;
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xa438, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* Patch code execution */
    patchArray = voltagePatchArray;
    patchSize = sizeof(voltagePatchArray)/sizeof(dal_rtl9603cvd_phy_data_t);
    for(i=0 ; i<patchSize ; i++)
    {
        if((ret=rtl9603cvd_ocpInterPhy_write(port,
                              patchArray[i].addr,
                              patchArray[i].data))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
    }

    value = 0xb820;
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xa436, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    value = 0x0000;
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xa438, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    if((ret = rtl9603cvd_ocpInterPhy_read(port, 0xa432, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    value &= ~(1<<6);
    if((ret = rtl9603cvd_ocpInterPhy_write(port, 0xa432, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

#define PATCH_NUM_ENTRY 17
#define PATCH_START_ENTRY 18
#define PATCH_CMD_END_ENTRY 251 
static int32
_dal_rtl9603cvd_switch_default_patch(void)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)
    uint32 v1=0, v2=0, v3=0, v4=0, ptn1_no=0, ptn2_no=0, i, j;
    int32 ret;

    /* check need patch or not */
    if ((ret = reg_array_field_read(RTL9603CVD_EFUSE_BOND_RSLTr, 0, REG_ARRAY_INDEX_NONE, RTL9603CVD_EF_BOND_RSLTf, &v1)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    if((v1 & 0x1) == 0)
        return RT_ERR_OK;

    if((ret = dal_rtl9603cvd_tool_get_patch_info(PATCH_NUM_ENTRY, &ptn1_no))!=0)
        return RT_ERR_FAILED;
    osal_printf("switch default start\n");
    ptn1_no=ptn1_no&0xff;
    for(i=0; i<ptn1_no; i++)
    {
        if(dal_rtl9603cvd_tool_get_patch_info(PATCH_START_ENTRY+i*3, &v1)!=0)
            return RT_ERR_FAILED;
        if(dal_rtl9603cvd_tool_get_patch_info(PATCH_START_ENTRY+i*3+1, &v2)!=0)
            return RT_ERR_FAILED;
        if(dal_rtl9603cvd_tool_get_patch_info(PATCH_START_ENTRY+i*3+2, &v3)!=0)
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
                if((ret = rtl9603cvd_ocpInterPhy_write(j, v2, v3)) != RT_ERR_OK)
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
        if(dal_rtl9603cvd_tool_get_patch_info(PATCH_START_ENTRY+ptn1_no*3+i*4, &v1)!=0)
            return RT_ERR_FAILED;
        if(dal_rtl9603cvd_tool_get_patch_info(PATCH_START_ENTRY+ptn1_no*3+i*4+1, &v2)!=0)
            return RT_ERR_FAILED;
        if(dal_rtl9603cvd_tool_get_patch_info(PATCH_START_ENTRY+ptn1_no*3+i*4+2, &v3)!=0)
            return RT_ERR_FAILED;
        if(dal_rtl9603cvd_tool_get_patch_info(PATCH_START_ENTRY+ptn1_no*3+i*4+3, &v4)!=0)
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

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      dal_rtl9603cvd_switch_init
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
dal_rtl9603cvd_switch_init(void)
{
    int32 ret;
    uint32 wData;
    rtk_port_t port;
    uint32 chip, rev, subtype;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    switch_init = INIT_COMPLETED;

#if defined(CONFIG_SDK_KERNEL_LINUX)
    /* Init all necessary IPs before all initializations */
    _dal_rtl9603cvd_switch_ipEnable_set();
#endif

    /*flow control threshold and set buffer mode*/
    if((ret = rtl9603cvd_raw_flowctrl_patch(FLOWCTRL_PATCH_DEFAULT)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /*meter tick-token configuration*/
    if((ret = _dal_rtl9603cvd_switch_tickToken_init())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* Make sure all the ports are link down before any PHY related operation */
    if ((ret = _dal_rtl9603cvd_switch_phyPower_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /*set switch ready, phy patch done*/
    wData = 1;
    if ((ret = reg_field_write(RTL9603CVD_WRAP_GPHY_MISCr,RTL9603CVD_PATCH_PHY_DONEf,&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    if((ret = dal_rtl9603cvd_switch_version_get(&chip, &rev, &subtype)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#if defined(FPGA_DEFINED)

#else
    /* Disble FE PHY polling */
    wData = 1;
    if ((ret = reg_field_write(RTL9603CVD_FEPHY_POLLr, RTL9603CVD_CFG_FEPHY_STOP_POLLf, (uint32 *)&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#if defined(RTL_CYGWIN_EMULATE)

#else
    /*analog patch*/
    /*phy patch*/
    if((ret=dal_rtl9603cvd_switch_phyPatch())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "phy patch fail\n");
        return ret;
    }

    /* phy nctrl patch */
    if((ret=_dal_rtl9603cvd_switch_phyNCtrl())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "phy nctrl patch fail, ret=0x%x\n",ret);
        return ret;
    }

    /* phy voltage patch - common_mode_patch_220908 */
    if((ret=_dal_rtl9603cvd_switch_phyVoltage())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "phy voltage patch fail, ret=0x%x\n",ret);
        return ret;
    }

    /* default patch */
    if((ret=_dal_rtl9603cvd_switch_default_patch())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "default patch fail, ret=0x%x\n",ret);
        return ret;
    }

    /* power saving feature */
    if((ret=_dal_rtl9603cvd_switch_powerSaving_init())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#endif
    /* Enable FE PHY polling */
    wData = 0;
    if ((ret = reg_field_write(RTL9603CVD_FEPHY_POLLr, RTL9603CVD_CFG_FEPHY_STOP_POLLf, (uint32 *)&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#if defined(CONFIG_SWITCH_INIT_LINKDOWN)
    /* Turn off phy power*/
    if ((ret = _dal_rtl9603cvd_switch_phyPower_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#else
    /*turn on phy power*/
    if ((ret = _dal_rtl9603cvd_switch_phyPower_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#endif

#endif
    wData = 1;
    if ((ret = reg_field_write(RTL9603CVD_CFG_UNHIOLr, RTL9603CVD_IPG_COMPENSATIONf, (uint32 *)&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    wData = 1;
    if ((ret = reg_field_write(RTL9603CVD_SDS_REG7r, RTL9603CVD_SP_CFG_NEG_CLKWR_A2Df, (uint32 *)&wData)) != RT_ERR_OK)
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
            if ((ret = reg_array_field_write(RTL9603CVD_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_RX_SPCf, &wData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }    

            wData = 1;
            if ((ret = reg_array_field_write(RTL9603CVD_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_PADDING_ENf, &wData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }    
        }
        
        wData = 1;
        if ((ret = reg_array_field_write(RTL9603CVD_P_MISCr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_WAIT_64B_IP6HDf, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
    }        

    /*enable thermal meter*/
    wData = 0x1;
    if ((ret = reg_field_write(RTL9603CVD_THERMAL_CTRL_0r, RTL9603CVD_REG_PPOWf, (uint32 *)&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9603cvd_switch_init */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */


/* Function Name:
 *      dal_rtl9603cvd_switch_phyPortId_get
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
dal_rtl9603cvd_switch_phyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
{
    dal_rtl9603cvd_switch_asic_type_t asicType;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPortId), RT_ERR_NULL_POINTER);

    /*get chip info to check port name mapping*/
    asicType = RTL9603CVD_ASIC_PON;

    switch(asicType)
    {
        case RTL9603CVD_ASIC_PON:
        default:
            return _dal_rtl9603cvd_switch_ponAsicPhyPortId_get(portName,pPortId);
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9603cvd_switch_phyPortId_get */



/* Function Name:
 *      dal_rtl9603cvd_switch_logicalPort_get
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
dal_rtl9603cvd_switch_logicalPort_get(int32 portId, rtk_switch_port_name_t *pPortName)
{
    dal_rtl9603cvd_switch_asic_type_t asicType;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortName), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(portId), RT_ERR_PORT_ID);

    /*get chip info to check port name mapping*/
    asicType = RTL9603CVD_ASIC_PON;

    switch(asicType)
    {
        case RTL9603CVD_ASIC_PON:
        default:
            return _dal_rtl9603cvd_switch_ponAsicLogicalPortName_get(portId,pPortName);
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9603cvd_switch_logicalPort_get */



/* Function Name:
 *      dal_rtl9603cvd_switch_port2PortMask_set
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
dal_rtl9603cvd_switch_port2PortMask_set(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32 portId;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_rtl9603cvd_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;

    RTK_PORTMASK_PORT_SET(*pPortMask,portId);

    return RT_ERR_OK;
} /* end of dal_rtl9603cvd_switch_port2PortMask_set */



/* Function Name:
 *      dal_rtl9603cvd_switch_port2PortMask_clear
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
dal_rtl9603cvd_switch_port2PortMask_clear(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32   ret;
    int32 portId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_rtl9603cvd_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;

    RTK_PORTMASK_PORT_CLEAR(*pPortMask, portId);

    return RT_ERR_OK;
} /* end of dal_rtl9603cvd_switch_port2PortMask_clear */



/* Function Name:
 *      dal_rtl9603cvd_switch_portIdInMask_check
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
dal_rtl9603cvd_switch_portIdInMask_check(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32   ret;
    int32 portId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_rtl9603cvd_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;

    if(RTK_PORTMASK_IS_PORT_SET(*pPortMask,portId))
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;

} /* end of dal_rtl9603cvd_switch_portIdInMask_check */

#if 0
/* Function Name:
  *      dal_rtl9603cvd_switch_maxPktLenLinkSpeed_get
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
dal_rtl9603cvd_switch_maxPktLenLinkSpeed_get(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen)
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
        regAddr =  RTL9603CVD_MAX_LENGTH_CFG0r;
        fieldIdx = RTL9603CVD_ACCEPT_MAX_LENTH_CFG0f;
    }
    else
    {
        regAddr =  RTL9603CVD_MAX_LENGTH_CFG1r;
        fieldIdx = RTL9603CVD_ACCEPT_MAX_LENTH_CFG1f;
    }

    if ((ret = reg_field_read(regAddr, fieldIdx, pLen)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_rtl9603cvd_switch_maxPktLenLinkSpeed_get */

/* Function Name:
  *      dal_rtl9603cvd_switch_maxPktLenLinkSpeed_set
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
dal_rtl9603cvd_switch_maxPktLenLinkSpeed_set(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len)
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
    RT_PARAM_CHK((RTL9603CVD_PACEKT_LENGTH_MAX < len), RT_ERR_INPUT);

    if(speed == MAXPKTLEN_LINK_SPEED_FE)
    {
        regAddr =  RTL9603CVD_MAX_LENGTH_CFG0r;
        fieldIdx = RTL9603CVD_ACCEPT_MAX_LENTH_CFG0f;
        portField = RTL9603CVD_MAX_LENGTH_10_100f;
        index = 0;
    }
    else
    {
        regAddr   = RTL9603CVD_MAX_LENGTH_CFG1r;
        fieldIdx  = RTL9603CVD_ACCEPT_MAX_LENTH_CFG1f;
        portField = RTL9603CVD_MAX_LENGTH_GIGAf;
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

        if ((ret = reg_array_field_write(RTL9603CVD_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, portField, &index)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
            return ret;
        }
    }
    return RT_ERR_OK;
} /* end of dal_rtl9603cvd_switch_maxPktLenLinkSpeed_set */
#endif

/* Module Name    : Switch     */
/* Sub-module Name: Management address and vlan configuration */
/* Function Name:
 *      _rtl9603cvd_switch_macAddr_set
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
int32 _rtl9603cvd_switch_macAddr_set(rtk_mac_t *pMacAddr)
{
    int32 ret;
    uint32 i, tmp[6];

    if(pMacAddr == NULL)
        return RT_ERR_NULL_POINTER;

    for (i=0;i<ETHER_ADDR_LEN;i++)
        tmp[i] = pMacAddr->octet[i];


    if ((ret = reg_field_write(RTL9603CVD_SWITCH_MACr, RTL9603CVD_SWITCH_MAC5f, &tmp[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9603CVD_SWITCH_MACr, RTL9603CVD_SWITCH_MAC4f, &tmp[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9603CVD_SWITCH_MACr, RTL9603CVD_SWITCH_MAC3f, &tmp[2])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9603CVD_SWITCH_MACr, RTL9603CVD_SWITCH_MAC2f, &tmp[3])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_field_write(RTL9603CVD_SWITCH_MACr, RTL9603CVD_SWITCH_MAC1f, &tmp[4])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_field_write(RTL9603CVD_SWITCH_MACr, RTL9603CVD_SWITCH_MAC0f, &tmp[5])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}/*end of _rtl9603cvd_switch_macAddr_set*/

/* Function Name:
 *      _rtl9603cvd_switch_macAddr_get
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
int32 _rtl9603cvd_switch_macAddr_get(rtk_mac_t *pMacAddr)
{
    int32 ret;
    uint32 i, tmp[6];

    if ((ret = reg_field_read(RTL9603CVD_SWITCH_MACr, RTL9603CVD_SWITCH_MAC5f, &tmp[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9603CVD_SWITCH_MACr, RTL9603CVD_SWITCH_MAC4f, &tmp[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9603CVD_SWITCH_MACr, RTL9603CVD_SWITCH_MAC3f, &tmp[2])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9603CVD_SWITCH_MACr, RTL9603CVD_SWITCH_MAC2f, &tmp[3])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_field_read(RTL9603CVD_SWITCH_MACr, RTL9603CVD_SWITCH_MAC1f, &tmp[4])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_field_read(RTL9603CVD_SWITCH_MACr, RTL9603CVD_SWITCH_MAC0f, &tmp[5])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    for (i=0;i<ETHER_ADDR_LEN;i++)
        pMacAddr->octet[i] = tmp[i];

    return RT_ERR_OK;
}/*end of _rtl9603cvd_switch_macAddr_get*/



/* Function Name:
 *      dal_rtl9603cvd_switch_mgmtMacAddr_get
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
dal_rtl9603cvd_switch_mgmtMacAddr_get(rtk_mac_t *pMac)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    if ((ret = _rtl9603cvd_switch_macAddr_get(pMac)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9603cvd_switch_mgmtMacAddr_get */

/* Function Name:
 *      dal_rtl9603cvd_switch_mgmtMacAddr_set
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
dal_rtl9603cvd_switch_mgmtMacAddr_set(rtk_mac_t *pMac)
{

    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    if((pMac->octet[0] & BITMASK_1B) == 1)
        return RT_ERR_INPUT;

    if ((ret = _rtl9603cvd_switch_macAddr_set(pMac)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9603cvd_switch_mgmtMacAddr_set */


/* Function Name:
 *      dal_rtl9603cvd_switch_chip_reset
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
dal_rtl9603cvd_switch_chip_reset(void){

    int32   ret;
    uint32 resetValue = 1;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    if ((ret = reg_field_write(RTL9603CVD_SOFTWARE_RSTr,RTL9603CVD_CMD_CHIP_RST_PSf,&resetValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rtl9603cvd_switch_version_get
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
dal_rtl9603cvd_switch_version_get(uint32 *pChipId, uint32 *pRev, uint32 *pSubtype)
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
        if ((ret = dal_rtl9603cvd_tool_get_chipSubType(&chipSubtype)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
    #endif
        is_init = 1;
    }

    /* function body */
    *pChipId = chipId;
    *pRev = chipRev;
    *pSubtype = chipSubtype;

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_switch_version_get */

/* Function Name:
  *      dal_rtl9603cvd_switch_maxPktLenByPort_get
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
dal_rtl9603cvd_switch_maxPktLenByPort_get(rtk_port_t port, uint32 *pLen)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_array_field_read(RTL9603CVD_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_ACCEPT_MAX_LENTHf, pLen)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    return RT_ERR_OK;
} /*end of dal_rtl9603cvd_switch_maxPktLenByPort_get*/

/* Function Name:
  *      dal_rtl9603cvd_switch_maxPktLenByPort_set
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
dal_rtl9603cvd_switch_maxPktLenByPort_set(rtk_port_t port, uint32 len)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTL9603CVD_PACEKT_LENGTH_MAX < len), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_array_field_write(RTL9603CVD_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_ACCEPT_MAX_LENTHf, &len)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    return RT_ERR_OK;
}/*end of dal_rtl9603cvd_switch_maxPktLenByPort_set*/

/* Function Name:
  *      dal_rtl9603cvd_switch_changeDuplex_get
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
dal_rtl9603cvd_switch_changeDuplex_get(rtk_enable_t *pState)
{
    int     ret;
    uint32  value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if((ret = reg_field_read(RTL9603CVD_CHANGE_DUPLEX_CTRLr, RTL9603CVD_CFG_CHG_DUP_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    *pState = value;

    return RT_ERR_OK;
} /* end of dal_rtl9603cvd_switch_changeDuplex_get */

/* Function Name:
  *      dal_rtl9603cvd_switch_changeDuplex_set
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
dal_rtl9603cvd_switch_changeDuplex_set(rtk_enable_t state)
{
    int     ret;
    uint32  value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((state >= RTK_ENABLE_END), RT_ERR_NULL_POINTER);

    value = (ENABLED == state) ? 1 : 0;
    if((ret = reg_field_write(RTL9603CVD_CHANGE_DUPLEX_CTRLr, RTL9603CVD_CFG_CHG_DUP_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9603cvd_switch_changeDuplex_set */

/* Function Name:
  *      dal_rtl9603cvd_switch_system_init
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
dal_rtl9603cvd_switch_system_init(rtk_switch_system_mode_t *pMode)
{
#if 0
    int32   ret,qid;
#endif
    if(pMode->initDefault)
    {
#if 0
        /*set CPU port APR for Q0~Q6 rate control with meter 7, exclude OMCI/OAM QID 7 usage*/
        for(qid =0 ; qid <= 6; qid++)
        {
            if ((ret = dal_rtl9603cvd_rate_egrQueueBwCtrlEnable_set(9,qid,ENABLED)) != RT_ERR_OK)
            {
                return ret;
            }
            /*point to real meter index of CPU port*/
            if ((ret = dal_rtl9603cvd_rate_egrQueueBwCtrlMeterIdx_set(9,qid,7)) != RT_ERR_OK)
            {
                return ret;
            }
#endif
        }

#if 0
        /*set meter 31(port 9 offset index 7) to to 5000 PPS*/
        if ((ret = dal_rtl9603cvd_rate_shareMeterMode_set(31, METER_MODE_PACKET_RATE)) != RT_ERR_OK)
        {
            return ret;
        }

        if ((ret = dal_rtl9603cvd_rate_shareMeter_set(31, 5000, ENABLED)) != RT_ERR_OK)
        {
            return ret;
        }
        /*set CPU egress port for drop tail only*/
        for(qid = 0 ; qid <= 7; qid++)
        {
            if ((ret = rtl9603cvd_raw_flowctrl_egressDropEnable_set(9,qid ,ENABLED)) != RT_ERR_OK)
            {
                return ret;
            }
        }
    }
#endif
    
    return RT_ERR_OK;
}/* end of dal_rtl9603cvd_switch_system_init */


/* Function Name:
  *      dal_dal9603cvd_switch_thermal_get
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
dal_dal9603cvd_switch_thermal_get(int32 *thermalIntger, int32 *thermalDecimal)
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
    if ((ret = reg_field_read(RTL9603CVD_THERMAL_STS_0r, RTL9603CVD_TM_SENSOR_OUTf, (uint32 *)&rData)) != RT_ERR_OK)
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

       
    *thermalIntger = tmpInter;
    *thermalDecimal = tmpFloat;

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
}   /* end of dal_dal9603cvd_switch_thermal_get */



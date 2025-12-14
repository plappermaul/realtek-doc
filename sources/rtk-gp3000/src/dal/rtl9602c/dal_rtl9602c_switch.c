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
 * $Date: 2014-02-26 18:16:35 +0800 (?Ÿæ?ä¸? 26 äºŒæ? 2014) $
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

#include <dal/rtl9602c/dal_rtl9602c.h>
#include <dal/rtl9602c/dal_rtl9602c_switch.h>
#include <dal/rtl9602c/dal_rtl9602c_flowctrl.h>
#include <dal/rtl9602c/dal_rtl9602c_acl.h>
#include <dal/rtl9602c/dal_rtl9602c_qos.h>
#include <dal/rtl9602c/dal_rtl9602c_rate.h>
#include <dal/rtl9602c/dal_rtl9602c_l2.h>

#include <hal/common/miim.h>
#include <osal/time.h>
#include <hal/mac/mac_probe.h>
#include <hal/mac/drv.h>
#include <dal/rtl9602c/dal_rtl9602c_tool.h>
#if !defined(CONFIG_SDK_KERNEL_LINUX)
#define dal_rtl9602c_tool_get_patch_info(x, y) (0)
#define dal_rtl9602c_tool_get_csExtId(x) (0)
#endif

#include <soc/type.h>

/*
 * Symbol Definition
 */


typedef enum rtl9602c_sys_ACL_idx_e
{
    RTL9602C_SYS_ACL_IDX_NIC = 0,
    RTL9602C_SYS_ACL_IDX_IGMP,
    RTL9602C_SYS_ACL_IDX_RLDP,
    RTL9602C_SYS_ACL_IDX_ARP_LAN,
    RTL9602C_SYS_ACL_IDX_LAN_ADDR,
    RTL9602C_SYS_ACL_IDX_ARP_WAN_UNTAG,
    RTL9602C_SYS_ACL_IDX_ARP_WAN_CTAG,
    RTL9602C_SYS_ACL_IDX_ARP_WAN_STAG,
    RTL9602C_SYS_ACL_IDX_WAN_ADDR,
    RTL9602C_SYS_ACL_IDX_MAX,
} rtl9602c_sys_ACL_idx_t;



typedef struct dal_rtl9602c_phy_data_s
{
    uint16  phy;
    uint16  page;
    uint16  reg;
    uint16  data;
} dal_rtl9602c_phy_data_t;


/*
 * Data Declaration
 */
static uint32    switch_init = INIT_NOT_COMPLETED;

static uint32    switch_pon_max_pkt_len_index = 1;


#define RTL9602C_CHIP_SUBTYPE_INVALID 0xFFFF

static uint32  chipId;
static uint32  chipRev;
static uint32  chipSubtype = RTL9602C_CHIP_SUBTYPE_INVALID;

#if 0
static const dal_rtl9602c_phy_data_t  phyPatchArray_20160122[] = {
/* Port 0 */
{ 0, 0xa43, 16, 0x019C},
/* Port 1 */
{ 0, 0xbce, 18, 0x0000},
/* CHEST Master */
{ 1, 0xa43, 19, 0x809A},
{ 1, 0xa43, 20, 0x4ad4},
{ 1, 0xa43, 19, 0x809e},
{ 1, 0xa43, 20, 0x0506},
{ 1, 0xa43, 19, 0x809C},
{ 1, 0xa43, 20, 0xd107},
{ 1, 0xa43, 19, 0x80a4},
{ 1, 0xa43, 20, 0x930c},
{ 1, 0xa43, 19, 0x80a2},
{ 1, 0xa43, 20, 0x8b19},
/* CHEST Slave */
{ 1, 0xa43, 19, 0x80B0},
{ 1, 0xa43, 20, 0x8006},
{ 1, 0xa43, 19, 0x80AC},
{ 1, 0xa43, 20, 0x429c},
{ 1, 0xa43, 19, 0x80B4},
{ 1, 0xa43, 20, 0x5319},
{ 1, 0xa43, 19, 0x80B2},
{ 1, 0xa43, 20, 0xff7f},
/* CHEST 100M */
{ 1, 0xa43, 19, 0x808e},
{ 1, 0xa43, 20, 0x15a4},
{ 1, 0xa43, 19, 0x8089},
{ 1, 0xa43, 20, 0x6bf7},
{ 1, 0xa43, 19, 0x808a},
{ 1, 0xa43, 20, 0xf708},
{ 1, 0xa43, 19, 0x8092},
{ 1, 0xa43, 20, 0x401e},
{ 1, 0xa43, 19, 0x8090},
{ 1, 0xa43, 20, 0xe61f},
{ 1, 0xa43, 19, 0x8088},
{ 1, 0xa43, 20, 0xb06b}
};
#endif

static const dal_rtl9602c_phy_data_t  phyPatchArray_20160413[] = {
/* Port 0 FEPHY enaqble ALDPS */
{ 0, 0xa43, 0x10, 0x019C },
/* Port 1 GEPHY 10M tabin-redeem setting */
{ 1, 0xbce, 0x12, 0x0000 },
/* CHEST Master */
{ 1, 0xa43, 0x13, 0x809A },
{ 1, 0xa43, 0x14, 0x4ad4 },
{ 1, 0xa43, 0x13, 0x809A },
{ 1, 0xa43, 0x13, 0x809e },
{ 1, 0xa43, 0x14, 0x0506 },
{ 1, 0xa43, 0x13, 0x809e },
{ 1, 0xa43, 0x13, 0x809C },
{ 1, 0xa43, 0x14, 0xd107 },
{ 1, 0xa43, 0x13, 0x809C },
{ 1, 0xa43, 0x13, 0x80a4 },
{ 1, 0xa43, 0x14, 0x930c },
{ 1, 0xa43, 0x13, 0x80a4 },
{ 1, 0xa43, 0x13, 0x80a2 },
{ 1, 0xa43, 0x14, 0x8b19 },
{ 1, 0xa43, 0x13, 0x80a2 },
/* CHEST Slave */
{ 1, 0xa43, 0x13, 0x80B0 },
{ 1, 0xa43, 0x14, 0x8006 },
{ 1, 0xa43, 0x13, 0x80B0 },
{ 1, 0xa43, 0x13, 0x80AC },
{ 1, 0xa43, 0x14, 0x429c },
{ 1, 0xa43, 0x13, 0x80AC },
{ 1, 0xa43, 0x13, 0x80B4 },
{ 1, 0xa43, 0x14, 0x5319 },
{ 1, 0xa43, 0x13, 0x80B4 },
{ 1, 0xa43, 0x13, 0x80B2 },
{ 1, 0xa43, 0x14, 0xff7f },
{ 1, 0xa43, 0x13, 0x80B2 },
/* CHEST 100M */
{ 1, 0xa43, 0x13, 0x808e },
{ 1, 0xa43, 0x14, 0x15a4 },
{ 1, 0xa43, 0x13, 0x808e },
{ 1, 0xa43, 0x13, 0x8089 },
{ 1, 0xa43, 0x14, 0x6bf7 },
{ 1, 0xa43, 0x13, 0x8089 },
{ 1, 0xa43, 0x13, 0x808a },
{ 1, 0xa43, 0x14, 0xf708 },
{ 1, 0xa43, 0x13, 0x808a },
{ 1, 0xa43, 0x13, 0x8092 },
{ 1, 0xa43, 0x14, 0x401e },
{ 1, 0xa43, 0x13, 0x8092 },
{ 1, 0xa43, 0x13, 0x8090 },
{ 1, 0xa43, 0x14, 0xe61f },
{ 1, 0xa43, 0x13, 0x8090 },
{ 1, 0xa43, 0x13, 0x8088 },
{ 1, 0xa43, 0x14, 0xb06b },
{ 1, 0xa43, 0x13, 0x8088 },
/* 20160226 Enable 500M gigabit Full Viterbi for green table */
{ 1, 0xa43, 19, 0x8062 },
{ 1, 0xa43, 20, 0x2000 },
{ 1, 0xa43, 19, 0x806a },
{ 1, 0xa43, 20, 0x2300 },
{ 1, 0xa43, 19, 0x8072 },
{ 1, 0xa43, 20, 0x2300 },
/* 20180123 Disable port 1 GPHY 100M Green
 * Enable 100M Green IOL test may failed when adopt short cable to link partner at 100M speed
 * force 100M mode without link partner, did not have this issue.
 */
{ 1, 0xa43, 0x13, 0x804d },
{ 1, 0xa43, 0x14, 0x2444 }
};

/*
 * Function Declaration
 */

int32 dal_rtl9602c_switch_phyPatch(void);

int32 _rtl9602c_switch_macAddr_set(rtk_mac_t *pMacAddr);
int32 _rtl9602c_switch_macAddr_get(rtk_mac_t *pMacAddr);
int32 _dal_rtl9602c_switch_maxPktLen_swap(void);

static int32
_dal_rtl9602c_switch_ipEnable_set(void)
{
#if defined(FPGA_DEFINED)
#else
    int32  ret;
    uint32 value;
    //uint32 chip, rev, subtype;

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
#endif
    return RT_ERR_OK;
}

static int32
_dal_rtl9602c_switch_ponAsicPhyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
{
    switch(portName)
    {
        case RTK_PORT_UTP0:
            *pPortId = 0;
            break;
        case RTK_PORT_UTP1:
            *pPortId = 1;
            break;
        case RTK_PORT_PON:
        case RTK_PORT_UTP2:
            *pPortId = 2;
            break;
        case RTK_PORT_CPU:
            *pPortId = 3;
            break;

        default:
            return RT_ERR_INPUT;
    }
    return RT_ERR_OK;
}

static int32
_dal_rtl9602c_switch_SFPAsicPhyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
{
    switch(portName)
    {
        case RTK_PORT_UTP0:
            *pPortId = 0;
            break;
        case RTK_PORT_PON:
        case RTK_PORT_UTP2:
            *pPortId = 2;
            break;
        case RTK_PORT_CPU:
            *pPortId = 3;
            break;

        default:
            return RT_ERR_INPUT;
    }
    return RT_ERR_OK;
}

static int32
_dal_rtl9602c_switch_sfuHguAsicPhyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
{
    switch(portName)
    {
        case RTK_PORT_UTP0:
            *pPortId = 1;
            break;
        case RTK_PORT_PON:
        case RTK_PORT_UTP2:
            *pPortId = 2;
            break;
        case RTK_PORT_CPU:
            *pPortId = 3;
            break;

        default:
            return RT_ERR_INPUT;
    }
    return RT_ERR_OK;
}

static int32
_dal_rtl9602c_switch_SFPAsicLogicalPortName_get(int32 portId, rtk_switch_port_name_t *pPortName)
{
    switch(portId)
    {
        case 0:
            *pPortName = RTK_PORT_UTP0;
            break;
        case 2:
            *pPortName = RTK_PORT_PON | RTK_PORT_UTP2;
            break;
        case 3:
            *pPortName = RTK_PORT_CPU;
            break;

        default:
            return RT_ERR_INPUT;
    }
    return RT_ERR_OK;
}



static int32
_dal_rtl9602c_switch_ponAsicLogicalPortName_get(int32 portId, rtk_switch_port_name_t *pPortName)
{
    switch(portId)
    {
        case 0:
            *pPortName = RTK_PORT_UTP0;
            break;
        case 1:
            *pPortName = RTK_PORT_UTP1;
            break;
        case 2:
            *pPortName = RTK_PORT_PON | RTK_PORT_UTP2;
            break;
        case 3:
            *pPortName = RTK_PORT_CPU;
            break;

        default:
            return RT_ERR_INPUT;
    }
    return RT_ERR_OK;
}

static int32
_dal_rtl9602c_switch_sfuHguAsicLogicalPortName_get(int32 portId, rtk_switch_port_name_t *pPortName)
{
    switch(portId)
    {
        case 1:
            *pPortName = RTK_PORT_UTP0;
            break;
        case 2:
            *pPortName = RTK_PORT_PON | RTK_PORT_UTP2;
            break;
        case 3:
            *pPortName = RTK_PORT_CPU;
            break;

        default:
            return RT_ERR_INPUT;
    }
    return RT_ERR_OK;
}

static int32
_dal_rtl9602c_switch_tickToken_init(void)
{
    int ret;
    uint32 wData;

    /*meter pon-tick-token configuration*/
    wData = 0x6e;
    if ((ret = reg_field_write(RTL9602C_PON_TB_CTRLr, RTL9602C_TICK_PERIODf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    wData = 0x95;
    if ((ret = reg_field_write(RTL9602C_PON_TB_CTRLr, RTL9602C_TKNf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    /*meter switch-tick-token configuration*/
    wData = 43;
    if ((ret = reg_field_write(RTL9602C_METER_TB_CTRLr, RTL9602C_TICK_PERIODf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    wData = 189;
    if ((ret = reg_field_write(RTL9602C_METER_TB_CTRLr, RTL9602C_TKNf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



static int32
_dal_rtl9602c_switch_green_enable(void)
{
    return RT_ERR_OK;
}

static int32
_dal_rtl9602c_switch_eee_enable(void)
{
    return RT_ERR_OK;
}

static int32
_dal_rtl9602c_switch_powerSaving_init(void)
{
    int ret;

    /* === Green enable === */
    if ((ret = _dal_rtl9602c_switch_green_enable()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* === EEE enable === */
    if ((ret = _dal_rtl9602c_switch_eee_enable()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



int32
dal_rtl9602c_switch_phyPatch(void)
{
    int i;
    int ret;
    dal_rtl9602c_phy_data_t const * patchArray = phyPatchArray_20160413;
    uint32   patchSize;

    patchSize = sizeof(phyPatchArray_20160413)/sizeof(dal_rtl9602c_phy_data_t);

    /*start patch phy*/
    for( i=0 ;  i < patchSize ; i++ )
    {
        if((ret=rtl9602c_miim_write(
            patchArray[i].phy,
            patchArray[i].page,
            patchArray[i].reg,
            patchArray[i].data))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

static int32 _dal_rtl9602c_get_chip_version(void)
{
    int32   ret;

    if (drv_swcore_cid_get(&chipId, &chipRev) != RT_ERR_OK)
    {
 	    RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

#if defined(CONFIG_FORCE_SDK_RTL9601CSFP)
    chipSubtype = RTL9602C_CHIP_SUB_TYPE_RTL9601C;
#else
    #if defined(CONFIG_SDK_KERNEL_LINUX)
    {
        int32   ret;
        uint32  subType;

        if ((ret = dal_rtl9602c_tool_get_chipSubType(&subType)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }

        chipSubtype = ((subType & 0x7c) >> 2) | ((subType & 0x100) >> 8) << 5;
    }
    #endif
#endif

    return RT_ERR_OK;
}

#if defined(RTL_CYGWIN_EMULATE)

#else
#define EFUSE_NUM_ENTRY 11
#define EFUSE_START_ENTRY 12

static int32
_dal_rtl9602c_switch_default_patch(void)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)

    uint32 v1 = 0, v2 = 0, v3 = 0, v4 = 0, ptn1_no = 0, ptn2_no = 0, i, j;

    /* check need patch or not */
    if(dal_rtl9602c_tool_get_patch_info(0, &v1)!=0)
        return RT_ERR_FAILED;
	if(dal_rtl9602c_tool_get_patch_info(1, &v2)!=0)
        return RT_ERR_FAILED;
	if(dal_rtl9602c_tool_get_patch_info(2, &v3)!=0)
        return RT_ERR_FAILED;
    if(((v1&0x1)^(v2&0x1)^(v3&0x1))==0)
        return RT_ERR_OK;
    if(dal_rtl9602c_tool_get_patch_info(EFUSE_NUM_ENTRY, &ptn1_no)!=0)
        return RT_ERR_FAILED;

    ptn1_no=ptn1_no&0xff;
    for(i=0; i<ptn1_no; i++)
    {
        if(dal_rtl9602c_tool_get_patch_info(EFUSE_START_ENTRY+i*3, &v1)!=0)
            return RT_ERR_FAILED;
        if(dal_rtl9602c_tool_get_patch_info(EFUSE_START_ENTRY+i*3+1, &v2)!=0)
            return RT_ERR_FAILED;
        if(dal_rtl9602c_tool_get_patch_info(EFUSE_START_ENTRY+i*3+2, &v3)!=0)
            return RT_ERR_FAILED;
        if(v1==0 && v2==0 && v3==0)
            return RT_ERR_OK;
        if(v1==0xffff && v2==0xffff && v3==0xffff)
            continue;
        for(j=0; j<3; j++)
        {
            if((v1>>j)&0x1)
                rtl9602c_ocpInterPhy_write(j, v2, v3);
        }
    }

    ptn2_no=(255-EFUSE_START_ENTRY+1-3*ptn1_no)/4;
    for(i=0; i<ptn2_no; i++)
    {
        if(dal_rtl9602c_tool_get_patch_info(EFUSE_START_ENTRY+ptn1_no*3+i*4, &v1)!=0)
            return RT_ERR_FAILED;
        if(dal_rtl9602c_tool_get_patch_info(EFUSE_START_ENTRY+ptn1_no*3+i*4+1, &v2)!=0)
            return RT_ERR_FAILED;
        if(dal_rtl9602c_tool_get_patch_info(EFUSE_START_ENTRY+ptn1_no*3+i*4+2, &v3)!=0)
            return RT_ERR_FAILED;
        if(dal_rtl9602c_tool_get_patch_info(EFUSE_START_ENTRY+ptn1_no*3+i*4+3, &v4)!=0)
            return RT_ERR_FAILED;
        if(v1==0 && v2==0 && v3==0 && v4==0)
            return RT_ERR_OK;
        if(v1==0xffff && v2==0xffff && v3==0xffff && v4==0xffff)
            continue;
        WRITE_MEM32((v1<<16)|v2, (v3<<16)|v4);
    }
#endif
    return RT_ERR_OK;
}

static int32
_dal_rtl9602c_switch_phyPower_set(rtk_enable_t enable)
{
    int32 ret;
    uint32 i;
    uint32 wData;
    uint32 rData;

    for(i = 0 ; i < 2 ; i ++)
    {
        if(!HAL_IS_PORT_EXIST(i))
        {
            continue;
        }
        
        /* Turn on/off phy power */
        /* Port i */
        wData = 0x20a400 | (i << 16);
        if ((ret = reg_write(RTL9602C_GPHY_IND_CMDr, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
            return ret;
        }
        osal_time_mdelay(10);
        if ((ret = reg_read(RTL9602C_GPHY_IND_RDr, &rData)) != RT_ERR_OK)
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
        if ((ret = reg_write(RTL9602C_GPHY_IND_WDr, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
            return ret;
        }
        wData = 0x60a400 | (i << 16);
        if ((ret = reg_write(RTL9602C_GPHY_IND_CMDr, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
            return ret;
        }
        osal_time_mdelay(10);
    }

    return RT_ERR_OK;
}
#endif

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      dal_rtl9602c_switch_init
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
dal_rtl9602c_switch_init(void)
{
    int32   ret;
    uint32 wData;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    switch_init = INIT_COMPLETED;

#if defined(CONFIG_SDK_KERNEL_LINUX)
    /* Init all necessary IPs before all initializations */
    _dal_rtl9602c_switch_ipEnable_set();
#endif

    /*get chip id*/
    if((ret = _dal_rtl9602c_get_chip_version())!=RT_ERR_OK)
    {
 	    RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /*flow control threshold and set buffer mode*/
#if defined(CONFIG_GPON_FEATURE) || defined(CONFIG_EPON_FEATURE)
    /*set packet buffer size*/
#else
    /*set packet buffer size*/
#endif


    /*flow control threshold and set buffer mode*/
    if((ret = rtl9602c_raw_flowctrl_patch(FLOWCTRL_PATCH_DEFAULT)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /*meter tick-token configuration*/
    if((ret = _dal_rtl9602c_switch_tickToken_init())!=RT_ERR_OK)
    {
 	    RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* Switch WFQ token control */
    wData = 1;
    if ((ret = reg_field_write(RTL9602C_SCH_WFQ_TKN_CTRLr, RTL9602C_WFQ_TKN_CTRLf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    /* Change LINE_RATE_2500M.RATE to 0x3ffff */
    wData = 0x3ffff;
    if ((ret = reg_field_write(RTL9602C_LINE_RATE_2500Mr, RTL9602C_RATEf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

#if defined(FPGA_DEFINED)

#else

#if defined(RTL_CYGWIN_EMULATE)

#else
    /*analog patch*/
    /*phy patch*/
    if((ret=dal_rtl9602c_switch_phyPatch())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "phy patch fail\n");
        return ret;
    }

	/* default patch from efuse */
    if((ret=_dal_rtl9602c_switch_default_patch())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "default patch fail\n");
        return ret;
    }

    /* power saving feature */
    if((ret=_dal_rtl9602c_switch_powerSaving_init())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#endif /*defined(RTL_CYGWIN_EMULATE)*/

/* For LAN SDS feature, the PHY patch done will be executed after setting LAN SDS mode */
#if !defined(CONFIG_LAN_SDS_FEATURE)
    /*set switch ready, phy patch done*/
    wData = 1;
    if ((ret = reg_field_write(RTL9602C_WRAP_GPHY_MISCr,RTL9602C_PATCH_PHY_DONEf,&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#endif

#if defined(RTL_CYGWIN_EMULATE)

#else

#if defined(CONFIG_SWITCH_INIT_LINKDOWN)
    if ((ret = _dal_rtl9602c_switch_phyPower_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#else
    if ((ret = _dal_rtl9602c_switch_phyPower_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#endif
#endif /*defined(RTL_CYGWIN_EMULATE)*/
#endif /*defined(FPGA_DEFINED)*/
    wData = 1;
    if ((ret = reg_field_write(RTL9602C_CFG_UNHIOLr, RTL9602C_IPG_COMPENSATIONf, (uint32 *)&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }


    /* set CPU port rx small packet */
    wData = 1;
    if ((ret = reg_array_field_write(RTL9602C_P_MISCr, 3, REG_ARRAY_INDEX_NONE, RTL9602C_RX_SPCf, (uint32 *)&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }


    /*init max packet length index  PON: use index 0 / others: use index 1*/
    {
        uint32 max_port;
        uint32 port,data;

        max_port = HAL_GET_MAX_PORT();
        for (port = 0; port <= max_port; port++)
        {
            if (!HAL_IS_PORT_EXIST(port))
            {
                continue;
            }
    
            if(HAL_IS_PON_PORT(port))
            {/*keep pon port index*/
                data = switch_pon_max_pkt_len_index;  
            }
            else
            {
                if(switch_pon_max_pkt_len_index == 0)
                    data = 1;
                else
                    data = 0;    
            }
            
        	if((ret = reg_array_field_write(RTL9602C_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_MAX_LENGTH_GIGAf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
        	if((ret = reg_array_field_write(RTL9602C_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_MAX_LENGTH_10_100f, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }        
        }

    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_switch_init */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */


/* Function Name:
 *      dal_rtl9602c_switch_phyPortId_get
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
dal_rtl9602c_switch_phyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
{

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPortId), RT_ERR_NULL_POINTER);


    switch(chipSubtype)
    {
        case RTL9602C_CHIP_SUB_TYPE_RTL9601C:
        case RTL9602C_CHIP_SUB_TYPE_RTL9601C_VB:
            return _dal_rtl9602c_switch_SFPAsicPhyPortId_get(portName,pPortId);
            break;
        case RTL9602C_CHIP_SUB_TYPE_RTL9601D_VA3:
        case RTL9602C_CHIP_SUB_TYPE_RTL9601D_VA4:
        #if defined(CONFIG_9601D_SFU) || defined(CONFIG_9601D_HGU)
            return _dal_rtl9602c_switch_sfuHguAsicPhyPortId_get(portName,pPortId);
        #elif defined(CONFIG_9601D_SFP)
            return _dal_rtl9602c_switch_SFPAsicPhyPortId_get(portName,pPortId);
        #else
            return _dal_rtl9602c_switch_ponAsicPhyPortId_get(portName,pPortId);
        #endif
            break;
        case RTL9602C_CHIP_SUB_TYPE_RTL9602C_VA4:
        case RTL9602C_CHIP_SUB_TYPE_RTL9602C_VA5:
        case RTL9602C_CHIP_SUB_TYPE_RTL9602CP:
        default:
            return _dal_rtl9602c_switch_ponAsicPhyPortId_get(portName,pPortId);
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_switch_phyPortId_get */



/* Function Name:
 *      dal_rtl9602c_switch_logicalPort_get
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
dal_rtl9602c_switch_logicalPort_get(int32 portId, rtk_switch_port_name_t *pPortName)
{

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortName), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(portId), RT_ERR_PORT_ID);

    /*get chip info to check port name mapping*/
    switch(chipSubtype)
    {
        case RTL9602C_CHIP_SUB_TYPE_RTL9601C:
        case RTL9602C_CHIP_SUB_TYPE_RTL9601C_VB:
            return _dal_rtl9602c_switch_SFPAsicLogicalPortName_get(portId,pPortName);
            break;
        case RTL9602C_CHIP_SUB_TYPE_RTL9601D_VA3:
        case RTL9602C_CHIP_SUB_TYPE_RTL9601D_VA4:
        #if defined(CONFIG_9601D_SFU) || defined(CONFIG_9601D_HGU)
            return _dal_rtl9602c_switch_sfuHguAsicLogicalPortName_get(portId,pPortName);
        #elif defined(CONFIG_9601D_SFP)
            return _dal_rtl9602c_switch_SFPAsicLogicalPortName_get(portId,pPortName);
        #else
            return _dal_rtl9602c_switch_ponAsicLogicalPortName_get(portId,pPortName);
        #endif
            break;
        case RTL9602C_CHIP_SUB_TYPE_RTL9602C_VA4:
        case RTL9602C_CHIP_SUB_TYPE_RTL9602C_VA5:
        case RTL9602C_CHIP_SUB_TYPE_RTL9602CP:
        default:
            return _dal_rtl9602c_switch_ponAsicLogicalPortName_get(portId,pPortName);
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_switch_logicalPort_get */



/* Function Name:
 *      dal_rtl9602c_switch_port2PortMask_set
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
dal_rtl9602c_switch_port2PortMask_set(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32 portId;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_rtl9602c_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;

    RTK_PORTMASK_PORT_SET(*pPortMask,portId);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_switch_port2PortMask_set */



/* Function Name:
 *      dal_rtl9602c_switch_port2PortMask_clear
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
dal_rtl9602c_switch_port2PortMask_clear(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32   ret;
    int32 portId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_rtl9602c_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;

    RTK_PORTMASK_PORT_CLEAR(*pPortMask, portId);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_switch_port2PortMask_clear */



/* Function Name:
 *      dal_rtl9602c_switch_portIdInMask_check
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
dal_rtl9602c_switch_portIdInMask_check(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32   ret;
    int32 portId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_rtl9602c_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;

    if(RTK_PORTMASK_IS_PORT_SET(*pPortMask,portId))
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;

} /* end of dal_rtl9602c_switch_portIdInMask_check */


/* Function Name:
  *      dal_rtl9602c_switch_maxPktLenLinkSpeed_get
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
dal_rtl9602c_switch_maxPktLenLinkSpeed_get(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen)
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
	    regAddr =  RTL9602C_MAX_LENGTH_CFG0r;
        fieldIdx = RTL9602C_ACCEPT_MAX_LENTH_CFG0f;
    }
    else
    {
	    regAddr =  RTL9602C_MAX_LENGTH_CFG1r;
        fieldIdx = RTL9602C_ACCEPT_MAX_LENTH_CFG1f;
    }

    if ((ret = reg_field_read(regAddr, fieldIdx, pLen)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_rtl9602c_switch_maxPktLenLinkSpeed_get */

/* Function Name:
  *      dal_rtl9602c_switch_maxPktLenLinkSpeed_set
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
dal_rtl9602c_switch_maxPktLenLinkSpeed_set(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len)
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
    RT_PARAM_CHK((RTL9602C_PACEKT_LENGTH_MAX < len), RT_ERR_INPUT);

    if(speed == MAXPKTLEN_LINK_SPEED_FE)
    {
	    regAddr =  RTL9602C_MAX_LENGTH_CFG0r;
        fieldIdx = RTL9602C_ACCEPT_MAX_LENTH_CFG0f;
        portField = RTL9602C_MAX_LENGTH_10_100f;
        index = 0;
    }
    else
    {
	    regAddr   = RTL9602C_MAX_LENGTH_CFG1r;
        fieldIdx  = RTL9602C_ACCEPT_MAX_LENTH_CFG1f;
        portField = RTL9602C_MAX_LENGTH_GIGAf;
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

        if ((ret = reg_array_field_write(RTL9602C_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, portField, &index)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
            return ret;
        }
    }
    return RT_ERR_OK;
} /* end of dal_rtl9602c_switch_maxPktLenLinkSpeed_set */


/* Module Name    : Switch     */
/* Sub-module Name: Management address and vlan configuration */
/* Function Name:
 *      _rtl9602c_switch_macAddr_set
 * Description:
 *      Set switch mac address configurations
 * Input:
 *      pMacAddr - Switch mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_SMI  				- SMI access error
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 _rtl9602c_switch_macAddr_set(rtk_mac_t *pMacAddr)
{
    int32 ret;
    uint32 i, tmp[6];

    if(pMacAddr == NULL)
        return RT_ERR_NULL_POINTER;

    for (i=0;i<ETHER_ADDR_LEN;i++)
        tmp[i] = pMacAddr->octet[i];


    if ((ret = reg_field_write(RTL9602C_SWITCH_MACr, RTL9602C_SWITCH_MAC5f, &tmp[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9602C_SWITCH_MACr, RTL9602C_SWITCH_MAC4f, &tmp[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9602C_SWITCH_MACr, RTL9602C_SWITCH_MAC3f, &tmp[2])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9602C_SWITCH_MACr, RTL9602C_SWITCH_MAC2f, &tmp[3])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_field_write(RTL9602C_SWITCH_MACr, RTL9602C_SWITCH_MAC1f, &tmp[4])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_field_write(RTL9602C_SWITCH_MACr, RTL9602C_SWITCH_MAC0f, &tmp[5])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}/*end of _rtl9602c_switch_macAddr_set*/

/* Function Name:
 *      _rtl9602c_switch_macAddr_get
 * Description:
 *      Get switch mac address configurations
 * Input:
 *      pMacAddr - Switch mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_SMI  				- SMI access error
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 _rtl9602c_switch_macAddr_get(rtk_mac_t *pMacAddr)
{
    int32 ret;
    uint32 i, tmp[6];

    if ((ret = reg_field_read(RTL9602C_SWITCH_MACr, RTL9602C_SWITCH_MAC5f, &tmp[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9602C_SWITCH_MACr, RTL9602C_SWITCH_MAC4f, &tmp[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9602C_SWITCH_MACr, RTL9602C_SWITCH_MAC3f, &tmp[2])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9602C_SWITCH_MACr, RTL9602C_SWITCH_MAC2f, &tmp[3])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_field_read(RTL9602C_SWITCH_MACr, RTL9602C_SWITCH_MAC1f, &tmp[4])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_field_read(RTL9602C_SWITCH_MACr, RTL9602C_SWITCH_MAC0f, &tmp[5])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    for (i=0;i<ETHER_ADDR_LEN;i++)
        pMacAddr->octet[i] = tmp[i];

    return RT_ERR_OK;
}/*end of _rtl9602c_switch_macAddr_get*/



/* Function Name:
 *      dal_rtl9602c_switch_mgmtMacAddr_get
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
dal_rtl9602c_switch_mgmtMacAddr_get(rtk_mac_t *pMac)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    if ((ret = _rtl9602c_switch_macAddr_get(pMac)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_switch_mgmtMacAddr_get */

/* Function Name:
 *      dal_rtl9602c_switch_mgmtMacAddr_set
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
dal_rtl9602c_switch_mgmtMacAddr_set(rtk_mac_t *pMac)
{

    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    if((pMac->octet[0] & BITMASK_1B) == 1)
        return RT_ERR_INPUT;

    if ((ret = _rtl9602c_switch_macAddr_set(pMac)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_switch_mgmtMacAddr_set */


/* Function Name:
 *      dal_rtl9602c_switch_chip_reset
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
dal_rtl9602c_switch_chip_reset(void){

    int32   ret;
    uint32 resetValue = 1;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    if ((ret = reg_field_write(RTL9602C_SOFTWARE_RSTr,RTL9602C_CMD_CHIP_RST_PSf,&resetValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rtl9602c_switch_version_get
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
dal_rtl9602c_switch_version_get(uint32 *pChipId, uint32 *pRev, uint32 *pSubtype)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);


    /* parameter check */
    RT_PARAM_CHK((NULL == pChipId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRev), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSubtype), RT_ERR_NULL_POINTER);


    if(RTL9602C_CHIP_SUBTYPE_INVALID==chipSubtype)
    {
        _dal_rtl9602c_get_chip_version();
    }

    /* function body */
    *pChipId = chipId;
    *pRev = chipRev;
    *pSubtype = chipSubtype;

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_switch_version_get */

/* Function Name:
  *      dal_rtl9602c_switch_maxPktLenByPort_get
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
dal_rtl9602c_switch_maxPktLenByPort_get(rtk_port_t port, uint32 *pLen)
{
	int32   ret;
	uint32  reg;
	uint32  field;
	uint32  data;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);

	/* function body */

   	if((ret = reg_array_field_read(RTL9602C_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_MAX_LENGTH_GIGAf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

	/*get setting from index*/
	if(1 == data)
	{
		reg = RTL9602C_MAX_LENGTH_CFG1r;
		field = RTL9602C_ACCEPT_MAX_LENTH_CFG1f;
	}else{
		reg = RTL9602C_MAX_LENGTH_CFG0r;
		field = RTL9602C_ACCEPT_MAX_LENTH_CFG0f;
	}

	if((ret = reg_field_read(reg, field, pLen)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

	return RT_ERR_OK;
} /*end of dal_rtl9602c_switch_maxPktLenByPort_get*/




/* Function Name:
  *      _dal_rtl9602c_switch_maxPktLen_swap
  * Description:
  *      Set the max packet length just have 2 index, index 0 must greater than index 1
  *      this API will aotu swap the setting to follow this design
  * Input:
  *      None
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_INPUT   - invalid enum speed type
  * Note:
  */
int32
_dal_rtl9602c_switch_maxPktLen_swap(void)
{
	int32   ret;
	uint32  reg;
	uint32  field;
    uint32  lenCfg0;
    uint32  lenCfg1;
    uint32  data;
    rtk_port_t  port, max_port;

	reg = RTL9602C_MAX_LENGTH_CFG1r;
	field = RTL9602C_ACCEPT_MAX_LENTH_CFG1f;
	if((ret = reg_field_read(reg, field, &lenCfg1)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
	reg = RTL9602C_MAX_LENGTH_CFG0r;
	field = RTL9602C_ACCEPT_MAX_LENTH_CFG0f;
	if((ret = reg_field_read(reg, field, &lenCfg0)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    if ( lenCfg0 >= lenCfg1)
    {   /*do not need swap*/
    	return RT_ERR_OK;
    }

    /*swap length setting*/

    /*1. set max length to 0, prevent packet pass to lmimt port*/
	reg = RTL9602C_MAX_LENGTH_CFG0r;
	field = RTL9602C_ACCEPT_MAX_LENTH_CFG0f;
    data = 0;
    if((ret = reg_field_write(reg, field, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
	reg = RTL9602C_MAX_LENGTH_CFG1r;
	field = RTL9602C_ACCEPT_MAX_LENTH_CFG1f;
    data = 0;
    if((ret = reg_field_write(reg, field, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }


    /*swap per port setting*/
    max_port = HAL_GET_MAX_PORT();
    for (port = 0; port <= max_port; port++)
    {
        if (!HAL_IS_PORT_EXIST(port))
        {
            continue;
        }

    	if((ret = reg_array_field_read(RTL9602C_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_MAX_LENGTH_GIGAf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        /*swap index*/
        if( 0==data )
        {
            data = 1;
        }
        else
        {
            data = 0;
        }


        if(HAL_IS_PON_PORT(port))
        {/*keep pon port index*/
            switch_pon_max_pkt_len_index = data;
        }

    	if((ret = reg_array_field_write(RTL9602C_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_MAX_LENGTH_GIGAf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }
    	if((ret = reg_array_field_write(RTL9602C_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_MAX_LENGTH_10_100f, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }
    }


    /*swap index config*/
	reg = RTL9602C_MAX_LENGTH_CFG0r;
	field = RTL9602C_ACCEPT_MAX_LENTH_CFG0f;
    data = lenCfg1;
    if((ret = reg_field_write(reg, field, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
	reg = RTL9602C_MAX_LENGTH_CFG1r;
	field = RTL9602C_ACCEPT_MAX_LENTH_CFG1f;
    data = lenCfg0;
    if((ret = reg_field_write(reg, field, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
  *      dal_rtl9602c_switch_maxPktLenByPort_set
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
dal_rtl9602c_switch_maxPktLenByPort_set(rtk_port_t port, uint32 len)
{
	int32   ret;
	uint32  cfgVal;
	uint32  reg;
	uint32  field;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((RTL9602C_PACEKT_LENGTH_MAX < len), RT_ERR_INPUT);

	/* function body */

	/*If PON port, use MAX_LENGTH_CFG1. Other ports, use MAX_LENGTH_CFG0*/
	if(HAL_IS_PON_PORT(port))
	{
        if(1==switch_pon_max_pkt_len_index)
        {
    		reg = RTL9602C_MAX_LENGTH_CFG1r;
    		field = RTL9602C_ACCEPT_MAX_LENTH_CFG1f;
    		cfgVal = 1;
        }
        else
        {
    		reg = RTL9602C_MAX_LENGTH_CFG0r;
    		field = RTL9602C_ACCEPT_MAX_LENTH_CFG0f;
    		cfgVal = 0;
        }
	}else{
        if(1==switch_pon_max_pkt_len_index)
        {
    		reg = RTL9602C_MAX_LENGTH_CFG0r;
    		field = RTL9602C_ACCEPT_MAX_LENTH_CFG0f;
    		cfgVal = 0;
        }
        else
        {
    		reg = RTL9602C_MAX_LENGTH_CFG1r;
    		field = RTL9602C_ACCEPT_MAX_LENTH_CFG1f;
    		cfgVal = 1;
        }
	}

	if((ret = reg_field_write(reg, field, &len)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

	if((ret = reg_array_field_write(RTL9602C_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_MAX_LENGTH_GIGAf, &cfgVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
	if((ret = reg_array_field_write(RTL9602C_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_MAX_LENGTH_10_100f, &cfgVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

	if((ret = _dal_rtl9602c_switch_maxPktLen_swap()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

	return RT_ERR_OK;
}/*end of dal_rtl9602c_switch_maxPktLenByPort_set*/

/* Function Name:
  *      dal_rtl9602c_switch_changeDuplex_get
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
dal_rtl9602c_switch_changeDuplex_get(rtk_enable_t *pState)
{
    int     ret;
    uint32  value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

	if((ret = reg_field_read(RTL9602C_CHANGE_DUPLEX_CTRLr, RTL9602C_CFG_CHG_DUP_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    *pState = value;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_switch_changeDuplex_get */

/* Function Name:
  *      dal_rtl9602c_switch_changeDuplex_set
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
dal_rtl9602c_switch_changeDuplex_set(rtk_enable_t state)
{
    int     ret;
    uint32  value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((state >= RTK_ENABLE_END), RT_ERR_NULL_POINTER);

    value = (ENABLED == state) ? 1 : 0;
	if((ret = reg_field_write(RTL9602C_CHANGE_DUPLEX_CTRLr, RTL9602C_CFG_CHG_DUP_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_switch_changeDuplex_set */

/* Function Name:
  *      dal_rtl9602c_switch_system_init
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
  * 	This function is used for SFU application system
  */
int32
dal_rtl9602c_switch_system_init(rtk_switch_system_mode_t *pMode)
{
    int32	                ret, qid;
    rtk_acl_field_entry_t   fieldSelect;
    rtk_acl_template_t	    aclTemplate;
    rtk_qos_pri2queue_t     pri2qid;
    rtk_acl_ingress_entry_t aclRule;
    rtk_acl_field_t         aclField,aclField2, aclField3;
    rtk_qos_priSelWeight_t  weightOfPriSel;
    rtk_portmask_t          allPmExcludeCpu;

	if (pMode->initDefault)
	{
#if !defined(CONFIG_9601D_SFU)&&!defined(CONFIG_9601D_HGU)
        if ((ret = dal_rtl9602c_acl_igrState_set(0, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }
#endif
#if !defined(CONFIG_9601D_SFP)
        if(RTL9602C_CHIP_SUB_TYPE_RTL9601C != chipSubtype && RTL9602C_CHIP_SUB_TYPE_RTL9601C_VB != chipSubtype)
        {
            if ((ret = dal_rtl9602c_acl_igrState_set(1, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
        }
#endif        
        if ((ret = dal_rtl9602c_acl_igrState_set(3, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

		/*make sure ACL priority is highest*/
        if ((ret = dal_rtl9602c_qos_priSelGroup_get(0, &weightOfPriSel)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

		weightOfPriSel.weight_of_acl = HAL_PRI_SEL_WEIGHT_MAX();
        if ((ret = dal_rtl9602c_qos_priSelGroup_set(0, &weightOfPriSel)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

		/*set CPU port used pri*/
        if ((ret = dal_rtl9602c_qos_portPriMap_set(3, 3)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
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

		if ((ret = dal_rtl9602c_qos_priMap_set(3, &pri2qid)) != RT_ERR_OK)
		{
		    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
			return ret;
		}

		/*set CPU port APR for Q0~Q6 rate control with meter 15, exclude OMCI/OAM QID=7 usage*/
		for (qid = 0 ; qid <= 6; qid++)
		{
			if ((ret = dal_rtl9602c_rate_egrQueueBwCtrlEnable_set(3, qid, ENABLED)) != RT_ERR_OK)
			{
		        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
				return ret;
			}

			/*point to real meter index of CPU port*/
			if ((ret = dal_rtl9602c_rate_egrQueueBwCtrlMeterIdx_set(3, qid, 15)) != RT_ERR_OK)
			{
		        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
				return ret;
			}
		}

        /*set meter 15 to 5000 PPS*/
        if ((ret = dal_rtl9602c_rate_shareMeterMode_set(15, METER_MODE_PACKET_RATE)) != RT_ERR_OK)
        {
		    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        if ((ret = dal_rtl9602c_rate_shareMeter_set(15, 5000, ENABLED)) != RT_ERR_OK)
        {
		    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        /*set CPU egress port for drop tail only*/
        for (qid = 0 ; qid <= 7; qid++)
        {
            if ((ret = rtl9602c_raw_flowctrl_egressDropEnable_set(3, qid, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
        }

        /*ACL user defined field 13/14 for LAN side unknown ARP*/
        fieldSelect.index = 13;
        fieldSelect.format = ACL_FORMAT_RAW;
        fieldSelect.offset = 40;
        if ((ret = dal_rtl9602c_acl_fieldSelect_set(&fieldSelect)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        fieldSelect.index = 14;
        fieldSelect.format = ACL_FORMAT_RAW;
        fieldSelect.offset = 38;
        if ((ret = dal_rtl9602c_acl_fieldSelect_set(&fieldSelect)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        /*ACL user defined field 11/12 for WAN side unknown ARP with Ctag | Stag*/
        fieldSelect.index = 11;
        fieldSelect.format = ACL_FORMAT_RAW;
        fieldSelect.offset = 44;
        if ((ret = dal_rtl9602c_acl_fieldSelect_set(&fieldSelect)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        fieldSelect.index = 12;
        fieldSelect.format = ACL_FORMAT_RAW;
        fieldSelect.offset = 42;
        if ((ret = dal_rtl9602c_acl_fieldSelect_set(&fieldSelect)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        aclTemplate.index = 3;
        aclTemplate.fieldType[0] = ACL_FIELD_SMAC0;
        aclTemplate.fieldType[1] = ACL_FIELD_SMAC1;
        aclTemplate.fieldType[2] = ACL_FIELD_SMAC2;
        aclTemplate.fieldType[3] = ACL_FIELD_USER_DEFINED11;
        aclTemplate.fieldType[4] = ACL_FIELD_USER_DEFINED12;
        aclTemplate.fieldType[5] = ACL_FIELD_USER_DEFINED13;
        aclTemplate.fieldType[6] = ACL_FIELD_USER_DEFINED14;
        aclTemplate.fieldType[7] = ACL_FIELD_USER_DEFINED15;

        if ((ret = dal_rtl9602c_acl_template_set(&aclTemplate)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
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
        aclTemplate.fieldType[7] = ACL_FIELD_USER_DEFINED15;

		if ((ret = dal_rtl9602c_acl_template_set(&aclTemplate)) != RT_ERR_OK )
		{
		    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
			return ret;
		}

        /*Broadcast, multicast and unicast flooding port mask exclude CPU port*/
        HAL_GET_ALL_PORTMASK(allPmExcludeCpu);
        RTK_PORTMASK_PORT_CLEAR(allPmExcludeCpu, HAL_GET_CPU_PORT());

		if ((ret = dal_rtl9602c_l2_lookupMissFloodPortMask_set(DLF_TYPE_BCAST, &allPmExcludeCpu)) != RT_ERR_OK )
		{
		    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
			return ret;
		}

		if ((ret = dal_rtl9602c_l2_lookupMissFloodPortMask_set(DLF_TYPE_UCAST, &allPmExcludeCpu)) != RT_ERR_OK )
		{
		    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
			return ret;
		}

		if ((ret = dal_rtl9602c_l2_lookupMissFloodPortMask_set(DLF_TYPE_IPMC, &allPmExcludeCpu)) != RT_ERR_OK )
		{
		    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
			return ret;
		}

        if ((ret = dal_rtl9602c_l2_lookupMissFloodPortMask_set(DLF_TYPE_IP6MC, &allPmExcludeCpu)) != RT_ERR_OK )
		{
		    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
			return ret;
		}

        if ((ret = dal_rtl9602c_l2_lookupMissFloodPortMask_set(DLF_TYPE_MCAST, &allPmExcludeCpu)) != RT_ERR_OK )
		{
		    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
			return ret;
		}
        pMode->sysUsedAclNum = RTL9602C_SYS_ACL_IDX_MAX;
	}

    /*ACL 1*/
    /*Trap IGMP priority for qid and set ACL Priority for NIC ring ID*/
    if (pMode->initIgmpSnooping)
    {
        memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
        memset(&aclField, 0x00, sizeof(rtk_acl_field_t));

        aclField.fieldType                      = ACL_FIELD_USER_DEFINED15;
        aclField.fieldUnion.pattern.data.value  = 0x0040;
        aclField.fieldUnion.pattern.data.mask   = 0x0040;
        aclField.next                           = NULL;

        aclRule.index                           = RTL9602C_SYS_ACL_IDX_IGMP;
        aclRule.pFieldHead                      = &aclField;
        aclRule.templateIdx                     = 3;
        aclRule.activePorts.bits[0]             = 0x7;
        aclRule.valid                           = ENABLED;
        aclRule.act.enableAct[ACL_IGR_PRI_ACT]  = ENABLED;
        aclRule.act.priAct.act                  = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
        aclRule.act.priAct.aclPri               = 2;

        if ((ret = dal_rtl9602c_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
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

        aclField2.fieldType                         = ACL_FIELD_USER_DEFINED15;
        aclField2.fieldUnion.pattern.data.value     = 0x0400;
        aclField2.fieldUnion.pattern.data.mask      = 0x0400;
        aclField2.next                              = NULL;

        aclRule.index                               = RTL9602C_SYS_ACL_IDX_RLDP;
        aclRule.pFieldHead                          = &aclField;
        aclRule.templateIdx                         = 3;
        aclRule.activePorts.bits[0]                 = 0x3;
        aclRule.valid                               = ENABLED;
        aclRule.act.enableAct[ACL_IGR_FORWARD_ACT]  = ENABLED;
        aclRule.act.forwardAct.act                  = ACL_IGR_FORWARD_TRAP_ACT;
        aclRule.act.enableAct[ACL_IGR_PRI_ACT]      = ENABLED;
        aclRule.act.priAct.act                      = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
        aclRule.act.priAct.aclPri                   = 1;

        if ((ret = dal_rtl9602c_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
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

        aclField.fieldType                          = ACL_FIELD_USER_DEFINED13;
        aclField.fieldUnion.pattern.data.value      = pMode->ipLan & 0xFFFF;
        aclField.fieldUnion.pattern.data.mask       = 0xFFFF;
        aclField.next                               = &aclField2;

        aclField2.fieldType                         = ACL_FIELD_USER_DEFINED14;
        aclField2.fieldUnion.pattern.data.value     = (pMode->ipLan >> 16) & 0xFFFF;
        aclField2.fieldUnion.pattern.data.mask      = 0xFFFF;
        aclField2.next                              = &aclField3;

        aclField3.fieldType                         = ACL_FIELD_USER_DEFINED15;
        aclField3.fieldUnion.pattern.data.value     = 0x0004;
        aclField3.fieldUnion.pattern.data.mask      = 0x0004;
        aclField3.next = NULL;

        aclRule.index                               = RTL9602C_SYS_ACL_IDX_ARP_LAN;
        aclRule.pFieldHead                          = &aclField;
        aclRule.templateIdx                         = 3;
        aclRule.activePorts.bits[0]                 = 0x3;
        aclRule.valid                               = ENABLED;
        aclRule.act.enableAct[ACL_IGR_FORWARD_ACT]  = ENABLED;
        aclRule.act.forwardAct.act                  = ACL_IGR_FORWARD_TRAP_ACT;
        aclRule.act.enableAct[ACL_IGR_PRI_ACT]      = ENABLED;
        aclRule.act.priAct.act                      = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
        aclRule.act.priAct.aclPri                   = 0;

        if ((ret = dal_rtl9602c_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
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

        aclRule.index                               = RTL9602C_SYS_ACL_IDX_LAN_ADDR;
        aclRule.pFieldHead                          = &aclField;
        aclRule.templateIdx                         = 2;
        aclRule.activePorts.bits[0]                 = 0x3;
        aclRule.valid                               = ENABLED;
        aclRule.act.enableAct[ACL_IGR_PRI_ACT]      = ENABLED;
        aclRule.act.priAct.act                      = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
        aclRule.act.priAct.aclPri                   = 2;

        if ((ret = dal_rtl9602c_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
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

        aclField.fieldType                              = ACL_FIELD_USER_DEFINED13;
        aclField.fieldUnion.pattern.data.value          = pMode->ipWan & 0xFFFF;
        aclField.fieldUnion.pattern.data.mask           = 0xFFFF;
        aclField.next                                   = &aclField2;

        aclField2.fieldType                             = ACL_FIELD_USER_DEFINED14;
        aclField2.fieldUnion.pattern.data.value         = (pMode->ipWan >> 16) & 0xFFFF;
        aclField2.fieldUnion.pattern.data.mask          = 0xFFFF;
        aclField2.next                                  = &aclField3;

        aclField3.fieldType                             = ACL_FIELD_USER_DEFINED15;
        aclField3.fieldUnion.pattern.data.value         = 0x0004;
        aclField3.fieldUnion.pattern.data.mask          = 0x0004;
        aclField3.next                                  = NULL;

        aclRule.index                                   = RTL9602C_SYS_ACL_IDX_ARP_WAN_UNTAG;
        aclRule.pFieldHead                              = &aclField;
        aclRule.templateIdx                             = 3;
        aclRule.activePorts.bits[0]                     = 0x4;
        aclRule.valid                                   = ENABLED;
        aclRule.act.enableAct[ACL_IGR_FORWARD_ACT]      = ENABLED;
        aclRule.act.forwardAct.act                      = ACL_IGR_FORWARD_TRAP_ACT;
        aclRule.act.enableAct[ACL_IGR_PRI_ACT]          = ENABLED;
        aclRule.act.priAct.act                          = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
        aclRule.act.priAct.aclPri                       = 0;

        if ((ret = dal_rtl9602c_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
        memset(&aclField, 0x00, sizeof(rtk_acl_field_t));
        memset(&aclField2, 0x00, sizeof(rtk_acl_field_t));
        memset(&aclField3, 0x00, sizeof(rtk_acl_field_t));

        aclField.fieldType                              = ACL_FIELD_USER_DEFINED11;
        aclField.fieldUnion.pattern.data.value          = pMode->ipWan & 0xFFFF;
        aclField.fieldUnion.pattern.data.mask           = 0xFFFF;
        aclField.next                                   = &aclField2;

        aclField2.fieldType                             = ACL_FIELD_USER_DEFINED12;
        aclField2.fieldUnion.pattern.data.value         = (pMode->ipWan >> 16) & 0xFFFF;
        aclField2.fieldUnion.pattern.data.mask          = 0xFFFF;
        aclField2.next                                  = &aclField3;

        aclField3.fieldType                             = ACL_FIELD_USER_DEFINED15;
        aclField3.fieldUnion.pattern.data.value         = 0x0004;
        aclField3.fieldUnion.pattern.data.mask          = 0x0004;
        aclField3.next                                  = NULL;

        aclRule.index                                   = RTL9602C_SYS_ACL_IDX_ARP_WAN_CTAG;
        aclRule.pFieldHead                              = &aclField;
        aclRule.templateIdx                             = 3;

        aclRule.careTag.tags[ACL_CARE_TAG_CTAG].value   = ENABLED;
        aclRule.careTag.tags[ACL_CARE_TAG_CTAG].mask    = ENABLED;
        aclRule.careTag.tags[ACL_CARE_TAG_STAG].value   = DISABLED;
        aclRule.careTag.tags[ACL_CARE_TAG_STAG].mask    = ENABLED;

        aclRule.activePorts.bits[0]                     = 0x4;
        aclRule.valid                                   = ENABLED;
        aclRule.act.enableAct[ACL_IGR_FORWARD_ACT]      = ENABLED;
        aclRule.act.forwardAct.act                      = ACL_IGR_FORWARD_TRAP_ACT;
        aclRule.act.enableAct[ACL_IGR_PRI_ACT]          = ENABLED;
        aclRule.act.priAct.act                          = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
        aclRule.act.priAct.aclPri                       = 0;

        if ((ret = dal_rtl9602c_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }

        memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
        memset(&aclField, 0x00, sizeof(rtk_acl_field_t));
        memset(&aclField2, 0x00, sizeof(rtk_acl_field_t));
        memset(&aclField3, 0x00, sizeof(rtk_acl_field_t));

        aclField.fieldType                              = ACL_FIELD_USER_DEFINED11;
        aclField.fieldUnion.pattern.data.value          = pMode->ipWan & 0xFFFF;
        aclField.fieldUnion.pattern.data.mask           = 0xFFFF;
        aclField.next                                   = &aclField2;

        aclField2.fieldType                             = ACL_FIELD_USER_DEFINED12;
        aclField2.fieldUnion.pattern.data.value         = (pMode->ipWan >> 16) & 0xFFFF;
        aclField2.fieldUnion.pattern.data.mask          = 0xFFFF;
        aclField2.next                                  = &aclField3;

        aclField3.fieldType                             = ACL_FIELD_USER_DEFINED15;
        aclField3.fieldUnion.pattern.data.value         = 0x0004;
        aclField3.fieldUnion.pattern.data.mask          = 0x0004;
        aclField3.next                                  = NULL;

        aclRule.index                                   = RTL9602C_SYS_ACL_IDX_ARP_WAN_STAG;
        aclRule.pFieldHead                              = &aclField;
        aclRule.templateIdx                             = 3;

        aclRule.careTag.tags[ACL_CARE_TAG_CTAG].value   = DISABLED;
        aclRule.careTag.tags[ACL_CARE_TAG_CTAG].mask    = ENABLED;
        aclRule.careTag.tags[ACL_CARE_TAG_STAG].value   = ENABLED;
        aclRule.careTag.tags[ACL_CARE_TAG_STAG].mask    = ENABLED;

        aclRule.activePorts.bits[0]                     = 0x4;
        aclRule.valid                                   = ENABLED;
        aclRule.act.enableAct[ACL_IGR_FORWARD_ACT]      = ENABLED;
        aclRule.act.forwardAct.act                      = ACL_IGR_FORWARD_TRAP_ACT;
        aclRule.act.enableAct[ACL_IGR_PRI_ACT]          = ENABLED;
        aclRule.act.priAct.act                          = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
        aclRule.act.priAct.aclPri                       = 0;

        if ((ret = dal_rtl9602c_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
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

        aclRule.index                                   = RTL9602C_SYS_ACL_IDX_WAN_ADDR;
        aclRule.pFieldHead                              = &aclField;
        aclRule.templateIdx                             = 2;
        aclRule.activePorts.bits[0]                     = 0x4;
        aclRule.valid                                   = ENABLED;
        aclRule.act.enableAct[ACL_IGR_PRI_ACT]          = ENABLED;
        aclRule.act.priAct.act                          = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
        aclRule.act.priAct.aclPri                       = 2;

        if ((ret = dal_rtl9602c_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}


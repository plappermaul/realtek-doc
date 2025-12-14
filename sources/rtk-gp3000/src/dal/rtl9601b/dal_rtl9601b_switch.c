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
 * $Revision$
 * $Date$
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
#include <rtk/switch.h>
#include <rtk/acl.h>
#include <rtk/qos.h>
#include <rtk/rate.h>

#include <dal/rtl9601b/dal_rtl9601b.h>
#include <dal/rtl9601b/dal_rtl9601b_switch.h>
#include <dal/rtl9601b/dal_rtl9601b_flowctrl.h>
#include <dal/rtl9601b/dal_rtl9601b_port.h>
#include <dal/rtl9601b/dal_rtl9601b_acl.h>
#include <dal/rtl9601b/dal_rtl9601b_qos.h>
#include <dal/rtl9601b/dal_rtl9601b_rate.h>
#include <dal/rtl9601b/dal_rtl9601b_l2.h>
#include <dal/rtl9601b/dal_rtl9601b_acl.h>

#include <hal/common/miim.h>
#include <osal/time.h>
#include <hal/mac/mac_probe.h>

/*
 * Symbol Definition
 */
#define DAL_RTL9601B_BROADCAST_PHYID  (0x0010)


typedef enum rtl9601b_sys_ACL_idx_e
{
    RTL9601B_SYS_ACL_IDX_NIC = 0,
    RTL9601B_SYS_ACL_IDX_IGMP,
    RTL9601B_SYS_ACL_IDX_RLDP,
    RTL9601B_SYS_ACL_IDX_ARP_LAN,
    RTL9601B_SYS_ACL_IDX_LAN_ADDR,
    RTL9601B_SYS_ACL_IDX_ARP_WAN_UNTAG,
    RTL9601B_SYS_ACL_IDX_ARP_WAN_CTAG,
    RTL9601B_SYS_ACL_IDX_ARP_WAN_STAG,
    RTL9601B_SYS_ACL_IDX_WAN_ADDR,
    RTL9601B_SYS_ACL_IDX_MAX,
} rtl9601b_sys_ACL_idx_t;



typedef struct dal_rtl9601b_phy_data_s
{
    uint16  phy;
    uint16  addr;
    uint16  data;
} dal_rtl9601b_phy_data_t;


/*
 * Data Declaration
 */
static uint32    switch_init = INIT_NOT_COMPLETED;

static uint32  chipId;
static uint32  chipRev;
static uint32  chipSubtype;

static dal_rtl9601b_phy_data_t  phyPatchArray_0639[] = {
#if 1
/* -----------nc patch--------------- */
{0x0000,0xa436,0xB820},
{0x0000,0xa438,0x0090},
{0x0000,0xa436,0xA012},
{0x0000,0xa438,0x0000},
{0x0000,0xa436,0xA014},
{0x0000,0xa438,0x2c04},
{0x0000,0xa438,0x2c12},
{0x0000,0xa438,0x2c14},
{0x0000,0xa438,0x2cd7},
{0x0000,0xa438,0x8620},
{0x0000,0xa438,0xa480},
{0x0000,0xa438,0x609f},
{0x0000,0xa438,0x3084},
{0x0000,0xa438,0x58c2},
{0x0000,0xa438,0x2c06},
{0x0000,0xa438,0xd710},
{0x0000,0xa438,0x6096},
{0x0000,0xa438,0xd71e},
{0x0000,0xa438,0x7fa4},
{0x0000,0xa438,0x28c2},
{0x0000,0xa438,0x8480},
{0x0000,0xa438,0xa101},
{0x0000,0xa438,0x2a7d},
{0x0000,0xa438,0x8104},
{0x0000,0xa438,0x0800},
{0x0000,0xa438,0xd704},
{0x0000,0xa438,0x60c5},
{0x0000,0xa438,0xd700},
{0x0000,0xa438,0x33b9},
{0x0000,0xa438,0x0c24},
{0x0000,0xa438,0x64d8},
{0x0000,0xa438,0x251e},
{0x0000,0xa438,0x60a6},
{0x0000,0xa438,0xd700},
{0x0000,0xa438,0x404c},
{0x0000,0xa438,0xb401},
{0x0000,0xa438,0x2c78},
{0x0000,0xa438,0xd700},
{0x0000,0xa438,0x604c},
{0x0000,0xa438,0xb401},
{0x0000,0xa438,0x2c55},
{0x0000,0xa438,0xd302},
{0x0000,0xa438,0xd076},
{0x0000,0xa438,0xd188},
{0x0000,0xa438,0xd03b},
{0x0000,0xa438,0xd198},
{0x0000,0xa438,0x3220},
{0x0000,0xa438,0x3c34},
{0x0000,0xa438,0xd024},
{0x0000,0xa438,0xd18b},
{0x0000,0xa438,0xd012},
{0x0000,0xa438,0xd19b},
{0x0000,0xa438,0x40b1},
{0x0000,0xa438,0xd07a},
{0x0000,0xa438,0xd189},
{0x0000,0xa438,0xd03d},
{0x0000,0xa438,0xd199},
{0x0000,0xa438,0xb240},
{0x0000,0xa438,0xd709},
{0x0000,0xa438,0x3001},
{0x0000,0xa438,0x5c16},
{0x0000,0xa438,0xd700},
{0x0000,0xa438,0x7ba8},
{0x0000,0xa438,0x3127},
{0x0000,0xa438,0x1c16},
{0x0000,0xa438,0x604c},
{0x0000,0xa438,0xb401},
{0x0000,0xa438,0x2c55},
{0x0000,0xa438,0x418c},
{0x0000,0xa438,0xd709},
{0x0000,0xa438,0x3001},
{0x0000,0xa438,0x5c55},
{0x0000,0xa438,0xd700},
{0x0000,0xa438,0x6228},
{0x0000,0xa438,0x33c7},
{0x0000,0xa438,0x7c55},
{0x0000,0xa438,0x3129},
{0x0000,0xa438,0x051e},
{0x0000,0xa438,0xb401},
{0x0000,0xa438,0x2c78},
{0x0000,0xa438,0xd709},
{0x0000,0xa438,0x3001},
{0x0000,0xa438,0x5c78},
{0x0000,0xa438,0xd700},
{0x0000,0xa438,0x6528},
{0x0000,0xa438,0x33c7},
{0x0000,0xa438,0x1c78},
{0x0000,0xa438,0x3129},
{0x0000,0xa438,0x051e},
{0x0000,0xa438,0xb401},
{0x0000,0xa438,0xd700},
{0x0000,0xa438,0x4190},
{0x0000,0xa438,0xd709},
{0x0000,0xa438,0x3006},
{0x0000,0xa438,0x1c5b},
{0x0000,0xa438,0x40c7},
{0x0000,0xa438,0xd700},
{0x0000,0xa438,0x8380},
{0x0000,0xa438,0xd014},
{0x0000,0xa438,0xd1a3},
{0x0000,0xa438,0x401a},
{0x0000,0xa438,0xd502},
{0x0000,0xa438,0xa120},
{0x0000,0xa438,0xd701},
{0x0000,0xa438,0xd500},
{0x0000,0xa438,0xa301},
{0x0000,0xa438,0xd501},
{0x0000,0xa438,0x60e5},
{0x0000,0xa438,0xe00f},
{0x0000,0xa438,0x0609},
{0x0000,0xa438,0xd500},
{0x0000,0xa438,0xe00c},
{0x0000,0xa438,0x0b08},
{0x0000,0xa438,0x2c6e},
{0x0000,0xa438,0xa608},
{0x0000,0xa438,0xd502},
{0x0000,0xa438,0x8120},
{0x0000,0xa438,0xd700},
{0x0000,0xa438,0xd501},
{0x0000,0xa438,0x3061},
{0x0000,0xa438,0x1c77},
{0x0000,0xa438,0x6062},
{0x0000,0xa438,0x4040},
{0x0000,0xa438,0x2c77},
{0x0000,0xa438,0x2c93},
{0x0000,0xa438,0xd700},
{0x0000,0xa438,0x4190},
{0x0000,0xa438,0xd709},
{0x0000,0xa438,0x3006},
{0x0000,0xa438,0x1c7e},
{0x0000,0xa438,0x40c7},
{0x0000,0xa438,0xd700},
{0x0000,0xa438,0x8380},
{0x0000,0xa438,0xd014},
{0x0000,0xa438,0xd1a3},
{0x0000,0xa438,0x401a},
{0x0000,0xa438,0xd502},
{0x0000,0xa438,0xa120},
{0x0000,0xa438,0xd701},
{0x0000,0xa438,0xd500},
{0x0000,0xa438,0x8301},
{0x0000,0xa438,0xd501},
{0x0000,0xa438,0x60e5},
{0x0000,0xa438,0xe00f},
{0x0000,0xa438,0x0602},
{0x0000,0xa438,0xd500},
{0x0000,0xa438,0xe00c},
{0x0000,0xa438,0x0b0c},
{0x0000,0xa438,0x2c91},
{0x0000,0xa438,0x8608},
{0x0000,0xa438,0xd502},
{0x0000,0xa438,0x8120},
{0x0000,0xa438,0xd500},
{0x0000,0xa438,0xd700},
{0x0000,0xa438,0x4230},
{0x0000,0xa438,0xd704},
{0x0000,0xa438,0x40c5},
{0x0000,0xa438,0xd705},
{0x0000,0xa438,0x4090},
{0x0000,0xa438,0x9840},
{0x0000,0xa438,0xd71e},
{0x0000,0xa438,0x6007},
{0x0000,0xa438,0x9810},
{0x0000,0xa438,0xd71e},
{0x0000,0xa438,0x6005},
{0x0000,0xa438,0xd700},
{0x0000,0xa438,0xa208},
{0x0000,0xa438,0xd027},
{0x0000,0xa438,0xd1a3},
{0x0000,0xa438,0x401a},
{0x0000,0xa438,0x8208},
{0x0000,0xa438,0x4618},
{0x0000,0xa438,0xb404},
{0x0000,0xa438,0xd705},
{0x0000,0xa438,0x6052},
{0x0000,0xa438,0xb280},
{0x0000,0xa438,0xd700},
{0x0000,0xa438,0x4136},
{0x0000,0xa438,0xa920},
{0x0000,0xa438,0xb440},
{0x0000,0xa438,0x30e8},
{0x0000,0xa438,0x1cbd},
{0x0000,0xa438,0xd051},
{0x0000,0xa438,0xd1fb},
{0x0000,0xa438,0x9440},
{0x0000,0xa438,0x2cbd},
{0x0000,0xa438,0xd705},
{0x0000,0xa438,0x405f},
{0x0000,0xa438,0x8920},
{0x0000,0xa438,0xd700},
{0x0000,0xa438,0x30e8},
{0x0000,0xa438,0x5cbd},
{0x0000,0xa438,0xa920},
{0x0000,0xa438,0xb440},
{0x0000,0xa438,0xa330},
{0x0000,0xa438,0xd302},
{0x0000,0xa438,0xd076},
{0x0000,0xa438,0xd188},
{0x0000,0xa438,0xd03b},
{0x0000,0xa438,0xd198},
{0x0000,0xa438,0x3220},
{0x0000,0xa438,0x3cd6},
{0x0000,0xa438,0xd024},
{0x0000,0xa438,0xd18b},
{0x0000,0xa438,0xd012},
{0x0000,0xa438,0xd19b},
{0x0000,0xa438,0x41b1},
{0x0000,0xa438,0xd07a},
{0x0000,0xa438,0xd189},
{0x0000,0xa438,0xd03d},
{0x0000,0xa438,0xd199},
{0x0000,0xa438,0xd705},
{0x0000,0xa438,0x40f2},
{0x0000,0xa438,0xd70c},
{0x0000,0xa438,0x40a0},
{0x0000,0xa438,0xd0f4},
{0x0000,0xa438,0xd187},
{0x0000,0xa438,0xd07a},
{0x0000,0xa438,0xd197},
{0x0000,0xa438,0x251e},
{0x0000,0xa438,0xd090},
{0x0000,0xa438,0x2279},
{0x0000,0xa436,0xA01A},
{0x0000,0xa438,0x0000},
{0x0000,0xa436,0xA006},
{0x0000,0xa438,0x0278},
{0x0000,0xa436,0xA004},
{0x0000,0xa438,0x046e},
{0x0000,0xa436,0xA002},
{0x0000,0xa438,0x05f1},
{0x0000,0xa436,0xA000},
{0x0000,0xa438,0xfa72},
{0x0000,0xa436,0xB820},
{0x0000,0xa438,0x0000},
/* -----------amp_dn patch--------------- */
{0x0000,0xa436,0x8079},
{0x0000,0xa438,0x0208},
{0x0000,0xa436,0x8082},
{0x0000,0xa438,0x0208},
{0x0000,0xa436,0x808b},
{0x0000,0xa438,0x0208},
{0x0000,0xa436,0x8094},
{0x0000,0xa438,0x0208},
{0x0000,0xa436,0x809d},
{0x0000,0xa438,0x0208},
{0x0000,0xa436,0x80a6},
{0x0000,0xa438,0x0208},
{0x0000,0xa436,0x80af},
{0x0000,0xa438,0x0208},
/* -----------channel estimation patch--------------- */
{0x0000,0xa436,0x80ec},
{0x0000,0xa438,0x4684},
{0x0000,0xa436,0x810b},
{0x0000,0xa438,0x447c},
{0x0000,0xa436,0x80cd},
{0x0000,0xa438,0xa09b},
{0x0000,0xa436,0x80cf},
{0x0000,0xa438,0xc409},
{0x0000,0xa436,0x80d7},
{0x0000,0xa438,0x90ca},
{0x0000,0xa436,0x80d1},
{0x0000,0xa438,0xa4ab},
{0x0000,0xa436,0x80d5},
{0x0000,0xa438,0xa0e7},
/* -----------EEE patch--------------- */
{0x0000,0xbcec,0x1f}
#endif
};

static dal_rtl9601b_phy_data_t  phyPatchArray_6422[] = {
/* -----------turn off sqe----------- */
{0x0000,0xa4e0,0x1a}
};

/*
 * Function Declaration
 */

int32 dal_rtl9601b_switch_phyPatch(void);
static int32 dal_rtl9601b_switch_regulatorPatch(void);

int32 _rtl9601b_switch_macAddr_set(rtk_mac_t *pMacAddr);
int32 _rtl9601b_switch_macAddr_get(rtk_mac_t *pMacAddr);

static int32
_dal_rtl9601b_switch_ponAsicPhyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
{
    switch(portName)
    {
        case RTK_PORT_UTP0:
            *pPortId = 0;
            break;
        case RTK_PORT_PON:
        case RTK_PORT_UTP1:
        case RTK_PORT_FIBER:
            *pPortId = 1;
            break;
        case RTK_PORT_CPU:
            *pPortId = 2;
            break;

        default:
            return RT_ERR_INPUT;
    }
    return RT_ERR_OK;
}




static int32
_dal_rtl9601b_switch_ponAsicLogicalPortName_get(int32 portId, rtk_switch_port_name_t *pPortName)
{
    switch(portId)
    {
        case 0:
            *pPortName = RTK_PORT_UTP0;
            break;
        case 1:
            *pPortName = RTK_PORT_PON | RTK_PORT_FIBER | RTK_PORT_UTP1;
            break;
        case 2:
            *pPortName = RTK_PORT_CPU;
            break;

        default:
            return RT_ERR_INPUT;
    }
    return RT_ERR_OK;
}


static int32
_dal_rtl9601b_switch_tickToken_init(void)
{
    int ret;
    uint32 wData;

    /*meter pon-tick-token configuration*/
    wData = 110;
    if ((ret = reg_field_write(RTL9601B_PON_TB_CTRLr, RTL9601B_TICK_PERIODf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    wData = 149;
    if ((ret = reg_field_write(RTL9601B_PON_TB_CTRLr, RTL9601B_TKNf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#if 0
    /*meter switch-tick-token configuration*/
    wData = 53;
    if ((ret = reg_field_write(RTL9601B_METER_TB_CTRLr, RTL9601B_TICK_PERIODf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    wData = 58;
    if ((ret = reg_field_write(RTL9601B_METER_TB_CTRLr, RTL9601B_TKNf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#endif

    return RT_ERR_OK;
}



static int32
_dal_rtl9601b_switch_green_enable(void)
{
    return RT_ERR_OK;
}

static int32
_dal_rtl9601b_switch_eee_enable(void)
{
    return RT_ERR_OK;
}

static int32
_dal_rtl9601b_switch_powerSaving_init(void)
{
    int ret;

    /* === Green enable === */
    if ((ret = _dal_rtl9601b_switch_green_enable()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* === EEE enable === */
    if ((ret = _dal_rtl9601b_switch_eee_enable()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


static int32
dal_rtl9601b_switch_regulatorPatch(void)
{
    uint32 regData;
    int32 ret;

    if((RTL9601B_CHIP_ID == chipId) && (0 == chipRev))
    {
        /* patch switch regulator SDZ_L for efficiency */
        regData = 0x170000;
        if ((ret = reg_write(RTL9601B_SWR_CONTROLL_1r, &regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        regData = 0x5788;
        if ((ret = reg_write(RTL9601B_SWR_CONTROLL_0r, &regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        regData = 0xc;
        if ((ret = reg_write(RTL9601B_SWR_CONTROLL_2r, &regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        regData = 0xe;
        if ((ret = reg_write(RTL9601B_SWR_CONTROLL_2r, &regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        regData = 0x9;
        if ((ret = reg_write(RTL9601B_SWR_CONTROLL_2r, &regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        regData = 0xa;
        if ((ret = reg_write(RTL9601B_SWR_CONTROLL_2r, &regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
#if 0 /* patch removed by Wang Ray */
        regData = 0x160000;
        if ((ret = reg_write(RTL9601B_SWR_CONTROLL_1r, &regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        regData = 0x5e05;
        if ((ret = reg_write(RTL9601B_SWR_CONTROLL_0r, &regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        regData = 0xc;
        if ((ret = reg_write(RTL9601B_SWR_CONTROLL_2r, &regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        regData = 0xe;
        if ((ret = reg_write(RTL9601B_SWR_CONTROLL_2r, &regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        regData = 0x9;
        if ((ret = reg_write(RTL9601B_SWR_CONTROLL_2r, &regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        regData = 0xa;
        if ((ret = reg_write(RTL9601B_SWR_CONTROLL_2r, &regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
#endif
    }
    return RT_ERR_OK;
}

int32
dal_rtl9601b_switch_phyPatch(void)
{
    int i;
    int ret;
    dal_rtl9601b_phy_data_t *patchArray;
    uint32   patchSize;

    patchArray = phyPatchArray_0639;
    patchSize = sizeof(phyPatchArray_0639)/sizeof(dal_rtl9601b_phy_data_t);

    /* 0639 need to phy patch */
    if((RTL9601B_CHIP_ID == chipId) && (0 == chipRev))
    {
        patchArray = phyPatchArray_0639;
        patchSize = sizeof(phyPatchArray_0639)/sizeof(dal_rtl9601b_phy_data_t);

    }
    else
    {
        patchArray = phyPatchArray_6422;
        patchSize = sizeof(phyPatchArray_6422)/sizeof(phyPatchArray_6422);
    }

    /*start patch phy*/
    for(i=0 ; i<patchSize ; i++)
    {
        if((ret=rtl9601b_ocpInterPhy_write(patchArray[i].phy,
                              patchArray[i].addr,
                              patchArray[i].data))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;

        }
    }

    return RT_ERR_OK;
}


static int32 _dal_rtl9601b_get_chip_version(void)
{
    int32   ret;
    if (drv_swcore_cid_get(&chipId, &chipRev) != RT_ERR_OK)
    {
 	    RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* TBD */
    chipSubtype = 0;

    return RT_ERR_OK;
}


/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      dal_rtl9601b_switch_init
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
dal_rtl9601b_switch_init(void)
{
    int32   ret;
    uint32 wData;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    switch_init = INIT_COMPLETED;

    /*get chip id*/
    if((ret = _dal_rtl9601b_get_chip_version())!=RT_ERR_OK)
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
    if((ret = rtl9601b_raw_flowctrl_patch(FLOWCTRL_PATCH_DEFAULT)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /*meter tick-token configuration*/
    if((ret = _dal_rtl9601b_switch_tickToken_init())!=RT_ERR_OK)
    {
 	    RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }


    /* patch switch regulator */
    if((ret=dal_rtl9601b_switch_regulatorPatch())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "phy patch fail\n");
        return ret;
    }

#if defined(FPGA_DEFINED)

#else
    /*CFG_PHY_POLL_CMD set to 0x1100*/
    wData = 0x1100;
    if ((ret = reg_write(RTL9601B_CFG_PHY_POLL_CMDr,&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#if defined(RTL_CYGWIN_EMULATE)

#else
    /*analog patch*/
    /* patch phy only before patch phy done */
    if ((ret = reg_field_read(RTL9601B_WRAP_GPHY_MISCr,RTL9601B_PATCH_PHY_DONEf,&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    if(wData==0)
    {
        /*phy patch*/
        if((ret=dal_rtl9601b_switch_phyPatch())!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "phy patch fail\n");
            return ret;
        }
    }

    /* power saving feature */
    if((ret=_dal_rtl9601b_switch_powerSaving_init())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#endif

#if defined(CONFIG_SWITCH_INIT_LINKDOWN)
    {
        uint32 data;

        if((ret = dal_rtl9601b_port_phyReg_get(0, 0xbc0, 19, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "set phy power down fail\n");
            return ret;
        }
        data = data|0x10;
        if((ret = dal_rtl9601b_port_phyReg_set(0, 0xbc0, 19, data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "set phy power down fail\n");
            return ret;
        }
    }
#endif

/* For LAN SDS feature, the PHY patch done will be executed after setting LAN SDS mode */
#if !defined(CONFIG_LAN_SDS_FEATURE)
    /*set switch ready, phy patch done*/
    wData = 1;
    if ((ret = reg_field_write(RTL9601B_WRAP_GPHY_MISCr,RTL9601B_PATCH_PHY_DONEf,&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#endif

    /*CFG_PHY_POLL_CMD set to 0x1110*/
    wData = 0x1110;
    if ((ret = reg_write(RTL9601B_CFG_PHY_POLL_CMDr,&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* set CPU port max packet len to 2031 (2047-4(ctag)-4(stag)-8(pppoe)=2031) */
    wData = 2031;
    if ((ret = reg_array_field_write(RTL9601B_ACCEPT_MAX_LEN_CTRLr, HAL_GET_CPU_PORT(), REG_ARRAY_INDEX_NONE, RTL9601B_RX_MAX_LENGTH_GIGAf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(RTL9601B_ACCEPT_MAX_LEN_CTRLr, HAL_GET_CPU_PORT(), REG_ARRAY_INDEX_NONE, RTL9601B_RX_MAX_LENGTH_10_100f, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(RTL9601B_TX_MAX_LEN_CTRLr, HAL_GET_CPU_PORT(), REG_ARRAY_INDEX_NONE, RTL9601B_TX_MAX_LENGTH_GIGAf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(RTL9601B_TX_MAX_LEN_CTRLr, HAL_GET_CPU_PORT(), REG_ARRAY_INDEX_NONE, RTL9601B_TX_MAX_LENGTH_10_100f, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
#endif

    wData = 1;
    if ((ret = reg_field_write(RTL9601B_GLOBAL_MAC_L2_MISC_0r, RTL9601B_IPG_COMPENSATIONf, (uint32 *)&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* Patch for 10H collision detection */
    wData = 0;
    if ((ret = reg_field_write(RTL9601B_DIGITAL_INTERFACE_SELECTr, RTL9601B_ORG_COLf, (uint32 *)&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9601B_CFG_PCSXFr, RTL9601B_CFG_PCSXFf, (uint32 *)&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    wData &= ~(1<<2); /* CFG_PCSXF[4] */
    if ((ret = reg_field_write(RTL9601B_CFG_PCSXFr, RTL9601B_CFG_PCSXFf, (uint32 *)&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    wData = 1;
    if ((ret = reg_field_write(RTL9601B_CFG_PCSXFr, RTL9601B_COL_10Mf, (uint32 *)&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_init */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */


/* Function Name:
 *      dal_rtl9601b_switch_phyPortId_get
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
dal_rtl9601b_switch_phyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
{
    dal_rtl9601b_switch_asic_type_t asicType;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPortId), RT_ERR_NULL_POINTER);

    /*get chip info to check port name mapping*/
    asicType = RTL9601B_ASIC_PON;

    switch(asicType)
    {
        case RTL9601B_ASIC_PON:
        default:
            return _dal_rtl9601b_switch_ponAsicPhyPortId_get(portName,pPortId);
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_phyPortId_get */



/* Function Name:
 *      dal_rtl9601b_switch_logicalPort_get
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
dal_rtl9601b_switch_logicalPort_get(int32 portId, rtk_switch_port_name_t *pPortName)
{
    dal_rtl9601b_switch_asic_type_t asicType;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortName), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(portId), RT_ERR_PORT_ID);

    /*get chip info to check port name mapping*/
    asicType = RTL9601B_ASIC_PON;

    switch(asicType)
    {
        case RTL9601B_ASIC_PON:
        default:
            return _dal_rtl9601b_switch_ponAsicLogicalPortName_get(portId,pPortName);
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_logicalPort_get */



/* Function Name:
 *      dal_rtl9601b_switch_port2PortMask_set
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
dal_rtl9601b_switch_port2PortMask_set(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32 portId;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_rtl9601b_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;

    RTK_PORTMASK_PORT_SET(*pPortMask,portId);

    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_port2PortMask_set */



/* Function Name:
 *      dal_rtl9601b_switch_port2PortMask_clear
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
dal_rtl9601b_switch_port2PortMask_clear(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32   ret;
    int32 portId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_rtl9601b_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;

    RTK_PORTMASK_PORT_CLEAR(*pPortMask, portId);

    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_port2PortMask_clear */



/* Function Name:
 *      dal_rtl9601b_switch_portIdInMask_check
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
dal_rtl9601b_switch_portIdInMask_check(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32   ret;
    int32 portId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_rtl9601b_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;

    if(RTK_PORTMASK_IS_PORT_SET(*pPortMask,portId))
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;

} /* end of dal_rtl9601b_switch_portIdInMask_check */


/* Function Name:
  *      dal_rtl9601b_switch_maxPktLenLinkSpeed_get
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
dal_rtl9601b_switch_maxPktLenLinkSpeed_get(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen)
{
    int32  ret;
	uint32 regAddr;
    uint32 fieldIdx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((MAXPKTLEN_LINK_SPEED_END <= speed), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);

    regAddr =  RTL9601B_ACCEPT_MAX_LEN_CTRLr;
    if(speed == MAXPKTLEN_LINK_SPEED_FE)
    {
        fieldIdx = RTL9601B_RX_MAX_LENGTH_10_100f;
    }
    else
    {
        fieldIdx = RTL9601B_RX_MAX_LENGTH_GIGAf;
    }

    if ((ret = reg_field_read(regAddr, fieldIdx, pLen)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_maxPktLenLinkSpeed_get */

/* Function Name:
  *      dal_rtl9601b_switch_maxPktLenLinkSpeed_set
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
dal_rtl9601b_switch_maxPktLenLinkSpeed_set(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len)
{
    int32  ret;
	uint32 regAddr;
    uint32 fieldIdx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((MAXPKTLEN_LINK_SPEED_END <= speed), RT_ERR_INPUT);
    RT_PARAM_CHK((RTL9601B_PACEKT_LENGTH_MAX < len), RT_ERR_INPUT);

    regAddr =  RTL9601B_ACCEPT_MAX_LEN_CTRLr;
    if(speed == MAXPKTLEN_LINK_SPEED_FE)
    {
        fieldIdx = RTL9601B_RX_MAX_LENGTH_10_100f;
    }
    else
    {
        fieldIdx  = RTL9601B_RX_MAX_LENGTH_GIGAf;
    }

    if ((ret = reg_field_write(regAddr, fieldIdx, &len)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_maxPktLenLinkSpeed_set */


/* Module Name    : Switch     */
/* Sub-module Name: Management address and vlan configuration */
/* Function Name:
 *      _rtl9601b_switch_macAddr_set
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
int32 _rtl9601b_switch_macAddr_set(rtk_mac_t *pMacAddr)
{
    int32 ret;
    uint32 i, tmp[6];

    if(pMacAddr == NULL)
        return RT_ERR_NULL_POINTER;

    for (i=0;i<ETHER_ADDR_LEN;i++)
        tmp[i] = pMacAddr->octet[i];


    if ((ret = reg_field_write(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC5f, &tmp[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC4f, &tmp[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC3f, &tmp[2])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC2f, &tmp[3])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_field_write(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC1f, &tmp[4])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_field_write(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC0f, &tmp[5])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}/*end of _rtl9601b_switch_macAddr_set*/

/* Function Name:
 *      _rtl9601b_switch_macAddr_get
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
int32 _rtl9601b_switch_macAddr_get(rtk_mac_t *pMacAddr)
{
    int32 ret;
    uint32 i, tmp[6];

    if ((ret = reg_field_read(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC5f, &tmp[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC4f, &tmp[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC3f, &tmp[2])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC2f, &tmp[3])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_field_read(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC1f, &tmp[4])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_field_read(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC0f, &tmp[5])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    for (i=0;i<ETHER_ADDR_LEN;i++)
        pMacAddr->octet[i] = tmp[i];

    return RT_ERR_OK;
}/*end of _rtl9601b_raw_switch_macAddr_get*/



/* Function Name:
 *      dal_rtl9601b_switch_mgmtMacAddr_get
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
dal_rtl9601b_switch_mgmtMacAddr_get(rtk_mac_t *pMac)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    if ((ret = _rtl9601b_switch_macAddr_get(pMac)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_mgmtMacAddr_get */

/* Function Name:
 *      dal_rtl9601b_switch_mgmtMacAddr_set
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
dal_rtl9601b_switch_mgmtMacAddr_set(rtk_mac_t *pMac)
{

    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    if((pMac->octet[0] & BITMASK_1B) == 1)
        return RT_ERR_INPUT;

    if ((ret = _rtl9601b_switch_macAddr_set(pMac)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_mgmtMacAddr_set */


/* Function Name:
 *      dal_rtl9601b_switch_chip_reset
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
dal_rtl9601b_switch_chip_reset(void){

    int32   ret;
    uint32 resetValue = 1;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    if ((ret = reg_field_write(RTL9601B_SOFTWARE_RSTr,RTL9601B_CMD_CHIP_RST_PSf,&resetValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}

/* Function Name:
 *      dal_rtl9601b_switch_version_get
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
dal_rtl9601b_switch_version_get(uint32 *pChipId, uint32 *pRev, uint32 *pSubtype)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pChipId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRev), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSubtype), RT_ERR_NULL_POINTER);

    /* function body */
    *pChipId = chipId;
    *pRev = chipRev;
    *pSubtype = chipSubtype;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_switch_version_get */

/* Function Name:
  *      dal_rtl9601b_switch_maxPktLenByPort_get
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
dal_rtl9601b_switch_maxPktLenByPort_get(rtk_port_t port, uint32 *pLen)
{
	int32   ret;
	
	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);

	/* function body */

	if((ret = reg_array_field_read(RTL9601B_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_RX_MAX_LENGTH_GIGAf, pLen)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

	return RT_ERR_OK;
} /*end of dal_rtl9601b_switch_maxPktLenByPort_get*/

/* Function Name:
  *      dal_rtl9601b_switch_maxPktLenByPort_set
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
dal_rtl9601b_switch_maxPktLenByPort_set(rtk_port_t port, uint32 len)
{
	int32   ret;
	
	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((RTL9601B_PACEKT_LENGTH_MAX < len), RT_ERR_INPUT);

	/* function body */
	
	if((ret = reg_array_field_write(RTL9601B_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_RX_MAX_LENGTH_GIGAf, &len)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
	if((ret = reg_array_field_write(RTL9601B_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_RX_MAX_LENGTH_10_100f, &len)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
	if((ret = reg_array_field_write(RTL9601B_TX_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_TX_MAX_LENGTH_GIGAf, &len)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
	if((ret = reg_array_field_write(RTL9601B_TX_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9601B_TX_MAX_LENGTH_10_100f, &len)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
	
	return RT_ERR_OK;
}/*end of dal_rtl9601b_switch_maxPktLenByPort_set*/

/* Function Name:
  *      dal_rtl9601b_switch_changeDuplex_get
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
dal_rtl9601b_switch_changeDuplex_get(rtk_enable_t *pState)
{
    int     ret;
    uint32  value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

	if((ret = reg_field_read(RTL9601B_CHANGE_DUPLEX_CTRLr, RTL9601B_CFG_CHG_DUP_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    *pState = value;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_changeDuplex_get */

/* Function Name:
  *      dal_rtl9601b_switch_changeDuplex_set
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
dal_rtl9601b_switch_changeDuplex_set(rtk_enable_t state)
{
    int     ret;
    uint32  value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((state >= RTK_ENABLE_END), RT_ERR_NULL_POINTER);

    value = (ENABLED == state) ? 1 : 0;
	if((ret = reg_field_write(RTL9601B_CHANGE_DUPLEX_CTRLr, RTL9601B_CFG_CHG_DUP_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_changeDuplex_set */


/* Function Name:
  *      dal_rtl9601b_switch_system_init
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
  * 	This function is used for SFU/SFP application system
  */
int32
dal_rtl9601b_switch_system_init(rtk_switch_system_mode_t *pMode)
{
	int32	ret,qid;
	rtk_acl_field_entry_t fieldSelect;	
	rtk_acl_template_t	aclTemplate;	
    rtk_qos_pri2queue_t pri2qid;
	rtk_acl_ingress_entry_t aclRule;
    rtk_acl_field_t aclField,aclField2,aclField3;
	rtk_qos_priSelWeight_t weightOfPriSel;	
	
	if(pMode->initDefault)
	{
        if ((ret = dal_rtl9601b_acl_igrState_set(0, ENABLED)) != RT_ERR_OK)
        {
            return ret;
        }
		
        if ((ret = dal_rtl9601b_acl_igrState_set(2, ENABLED)) != RT_ERR_OK)
        {
            return ret;
        }
	
		/*make sure ACL priority is highest*/
		if ((ret = dal_rtl9601b_qos_priSelGroup_get(0, &weightOfPriSel)) != RT_ERR_OK)
		{
			return ret;
		}

		weightOfPriSel.weight_of_acl = HAL_PRI_SEL_WEIGHT_MAX();
		if ((ret = dal_rtl9601b_qos_priSelGroup_set(0, &weightOfPriSel)) != RT_ERR_OK)
		{
			return ret;
		}

	
		/*set CPU port used pri*/
		if ((ret = dal_rtl9601b_qos_portPriMap_set(2, 3)) != RT_ERR_OK)
		{
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

		if ((ret = dal_rtl9601b_qos_priMap_set(3, &pri2qid)) != RT_ERR_OK)
		{
			return ret;
		}

		
		/*set CPU port APR for Q0~Q6 rate control with meter 7, exclude OMCI/OAM QID 7 usage*/
		for(qid =0 ; qid <= 6; qid++)
		{
			if ((ret = dal_rtl9601b_rate_egrQueueBwCtrlEnable_set(2,qid,ENABLED)) != RT_ERR_OK)
			{
				return ret;
			}
			
			if ((ret = dal_rtl9601b_rate_egrQueueBwCtrlMeterIdx_set(2,qid,7)) != RT_ERR_OK)
			{
				return ret;
			}			
		}
		
		/*set meter 7 to 5000PPS*/
		if ((ret = dal_rtl9601b_rate_shareMeterMode_set(7,METER_MODE_PACKET_RATE)) != RT_ERR_OK)
		{
			return ret;
		}	
		
		if ((ret = dal_rtl9601b_rate_shareMeter_set(7,5000,ENABLED)) != RT_ERR_OK)
		{
			return ret;
		}	


		/*set CPU egress port for drop tail only*/
		for(qid = 0 ; qid <= 7; qid++)
		{
			if ((ret = rtl9601b_raw_flowctrl_egressDropEnable_set(2,qid ,ENABLED)) != RT_ERR_OK)
			{
				return ret;
			}				
		}


		/*ACL user defined field 13/14 for LAN side unknown ARP*/
		fieldSelect.index = 13;
		fieldSelect.format = ACL_FORMAT_RAW;
		fieldSelect.offset = 40;
	    if ((ret = dal_rtl9601b_acl_fieldSelect_set(&fieldSelect)) != RT_ERR_OK )
		{
			return ret;
		}	

		fieldSelect.index = 14;
		fieldSelect.format = ACL_FORMAT_RAW;
		fieldSelect.offset = 38;
		if ((ret = dal_rtl9601b_acl_fieldSelect_set(&fieldSelect)) != RT_ERR_OK )
		{
			return ret;
		}	

		/*ACL user defined field 11/12 for WAN side unknown ARP with Ctag | Stag*/
		fieldSelect.index = 11;
		fieldSelect.format = ACL_FORMAT_RAW;
		fieldSelect.offset = 44;
	    if ((ret = dal_rtl9601b_acl_fieldSelect_set(&fieldSelect)) != RT_ERR_OK )
		{
			return ret;
		}	
		
		fieldSelect.index = 12;
		fieldSelect.format = ACL_FORMAT_RAW;
		fieldSelect.offset = 42;
		if ((ret = dal_rtl9601b_acl_fieldSelect_set(&fieldSelect)) != RT_ERR_OK )
		{
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

		if ((ret = dal_rtl9601b_acl_template_set(&aclTemplate)) != RT_ERR_OK )
		{
			return ret;
		}	

		aclTemplate.index = 2;
		aclTemplate.fieldType[0] = ACL_FIELD_DMAC0;
		aclTemplate.fieldType[1] = ACL_FIELD_DMAC1;
		aclTemplate.fieldType[2] = ACL_FIELD_DMAC2;
		aclTemplate.fieldType[3] = ACL_FIELD_IPV4_DIP0;
		aclTemplate.fieldType[4] = ACL_FIELD_IPV4_DIP1;
		aclTemplate.fieldType[5] = ACL_FIELD_CTAG;
		aclTemplate.fieldType[6] = ACL_FIELD_STAG;
		aclTemplate.fieldType[7] = ACL_FIELD_USER_DEFINED15;
		
		if ((ret = dal_rtl9601b_acl_template_set(&aclTemplate)) != RT_ERR_OK )
		{
			return ret;
		}	


		/*Broadcast, multicast and unicast flooding port mask exclude CPU port*/
		if ((ret = rtl9601b_raw_l2_bcFlood_set(2, DISABLED)) != RT_ERR_OK )
		{
			return ret;
		}	
		
		if ((ret = rtl9601b_raw_l2_unknUcFlood_set(2, DISABLED)) != RT_ERR_OK )
		{
			return ret;
		}	
		
		if ((ret = rtl9601b_raw_l2_unknMcFlood_set(2, DISABLED)) != RT_ERR_OK )
		{
			return ret;
		}				
        pMode->sysUsedAclNum = RTL9601B_SYS_ACL_IDX_MAX;
	}
	
	/*ACL 1*/
	/*Trap IGMP priority for qid and set ACL Priority for NIC ring ID*/
	if(pMode->initIgmpSnooping)
	{
		memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
		memset(&aclField, 0x00, sizeof(rtk_acl_field_t));

		aclField.fieldType = ACL_FIELD_USER_DEFINED15;
		aclField.fieldUnion.pattern.data.value = 0x0040;
		aclField.fieldUnion.pattern.data.mask = 0x0040;
		aclField.next = NULL;
			
		aclRule.index = RTL9601B_SYS_ACL_IDX_IGMP;
		aclRule.pFieldHead = &aclField;
		aclRule.templateIdx = 3;
		aclRule.activePorts.bits[0] = 0x3;
		aclRule.valid = ENABLED;
		aclRule.act.enableAct[ACL_IGR_PRI_ACT]= ENABLED;
		aclRule.act.priAct.act = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
		aclRule.act.priAct.aclPri = 2;
		
		if ((ret = dal_rtl9601b_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
		{
			return ret;
		}		
	}	

	/*ACL 2*/
	/*Trap RLDP pkt with priority 1*/
	if(pMode->initRldp)
	{
		memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
		memset(&aclField, 0x00, sizeof(rtk_acl_field_t));
		memset(&aclField2, 0x00, sizeof(rtk_acl_field_t));

		aclField.fieldType = ACL_FIELD_SMAC;
	    osal_memcpy(&aclField.fieldUnion.mac.value.octet, pMode->macLan.octet, ETHER_ADDR_LEN);
	    osal_memset(&aclField.fieldUnion.mac.mask.octet, 0xFF, ETHER_ADDR_LEN);
		aclField.next = &aclField2;
	

		aclField2.fieldType = ACL_FIELD_USER_DEFINED15;
		aclField2.fieldUnion.pattern.data.value = 0x0400;
		aclField2.fieldUnion.pattern.data.mask = 0x0400;
		aclField2.next = NULL;


		aclRule.index = RTL9601B_SYS_ACL_IDX_RLDP;
		aclRule.pFieldHead = &aclField;
		aclRule.templateIdx = 3;
		aclRule.activePorts.bits[0] = 0x1;
		aclRule.valid = ENABLED;
		aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
		aclRule.act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;
		
		aclRule.act.enableAct[ACL_IGR_PRI_ACT]= ENABLED;
		aclRule.act.priAct.act = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
		aclRule.act.priAct.aclPri = 1;
		
		if ((ret = dal_rtl9601b_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
		{
			return ret;
		}		
	}	

	/*ACL 3-4*/
	if(pMode->initLan)
	{
		/*Copy all ARP for lan interface to Q0*/
		memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
		memset(&aclField, 0x00, sizeof(rtk_acl_field_t));
		memset(&aclField2, 0x00, sizeof(rtk_acl_field_t));
		memset(&aclField3, 0x00, sizeof(rtk_acl_field_t));

		aclField.fieldType = ACL_FIELD_USER_DEFINED13;
		aclField.fieldUnion.pattern.data.value = pMode->ipLan & 0xFFFF; 
		aclField.fieldUnion.pattern.data.mask = 0xFFFF;
		aclField.next = &aclField2;
		
		aclField2.fieldType = ACL_FIELD_USER_DEFINED14;
		aclField2.fieldUnion.pattern.data.value = (pMode->ipLan >> 16) & 0xFFFF;
		aclField2.fieldUnion.pattern.data.mask = 0xFFFF;
		aclField2.next = &aclField3;

		aclField3.fieldType = ACL_FIELD_USER_DEFINED15;
		aclField3.fieldUnion.pattern.data.value = 0x0004;
		aclField3.fieldUnion.pattern.data.mask = 0x0004;
		aclField3.next = NULL;
			
		aclRule.index = RTL9601B_SYS_ACL_IDX_ARP_LAN;
		aclRule.pFieldHead = &aclField;
		aclRule.templateIdx = 3;
		aclRule.activePorts.bits[0] = 0x1;
		aclRule.valid = ENABLED;
		aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
		aclRule.act.forwardAct.act = ACL_IGR_FORWARD_COPY_ACT;
		
		aclRule.act.enableAct[ACL_IGR_PRI_ACT]= ENABLED;
		aclRule.act.priAct.act = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
		aclRule.act.priAct.aclPri = 0;
		
		if ((ret = dal_rtl9601b_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
		{
			return ret;
		}		

		/*Assign Lan interface unicast for QID 2*/
		memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
		memset(&aclField, 0x00, sizeof(rtk_acl_field_t));

		aclField.fieldType = ACL_FIELD_DMAC;
	    osal_memcpy(&aclField.fieldUnion.mac.value.octet, pMode->macLan.octet, ETHER_ADDR_LEN);
	    osal_memset(&aclField.fieldUnion.mac.mask.octet, 0xFF, ETHER_ADDR_LEN);
		aclField.next = NULL;		
	
		aclRule.index = RTL9601B_SYS_ACL_IDX_LAN_ADDR;
		aclRule.pFieldHead = &aclField;
		aclRule.templateIdx = 2;
		aclRule.activePorts.bits[0] = 0x1;
		aclRule.valid = ENABLED;

		/*
		The followings setting is to fix ASIC classify issue
		[ASIC expected behavior]
		If the drop action of classify entry is enable, the function should be only active for PON port, 
		
		[ASIC real behavior]
		The destination to NON PON port(CPU) packet is also drop whether DUT MAC address is learned or unknown. 
		Ex: Set a classify rule of untagged packet to drop action, it will cause that user cannot Ping/telnet DUT
		
		[Solution]
		Modify ACL entry, let packet copy to CPU if destination MAC address is DUT MAC
		*/
		aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
		aclRule.act.forwardAct.act = ACL_IGR_FORWARD_COPY_ACT;
		
		aclRule.act.enableAct[ACL_IGR_PRI_ACT]= ENABLED;
		aclRule.act.priAct.act = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
		aclRule.act.priAct.aclPri = 2;
		
		if ((ret = dal_rtl9601b_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
		{
			return ret;
		}		
	}	

	/*ACL 5-8*/
	/*Copy all ARP for WAN interface to Q0*/
	if(pMode->initWan)
	{
		memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
		memset(&aclField, 0x00, sizeof(rtk_acl_field_t));
		memset(&aclField2, 0x00, sizeof(rtk_acl_field_t));
		memset(&aclField3, 0x00, sizeof(rtk_acl_field_t));

		aclField.fieldType = ACL_FIELD_USER_DEFINED13;
		aclField.fieldUnion.pattern.data.value = pMode->ipWan & 0xFFFF; 
		aclField.fieldUnion.pattern.data.mask = 0xFFFF;
		aclField.next = &aclField2;
		
		aclField2.fieldType = ACL_FIELD_USER_DEFINED14;
		aclField2.fieldUnion.pattern.data.value = (pMode->ipWan >> 16) & 0xFFFF;
		aclField2.fieldUnion.pattern.data.mask = 0xFFFF;
		aclField2.next = &aclField3;

		aclField3.fieldType = ACL_FIELD_USER_DEFINED15;
		aclField3.fieldUnion.pattern.data.value = 0x0004;
		aclField3.fieldUnion.pattern.data.mask = 0x0004;
		aclField3.next = NULL;
			
		aclRule.index = RTL9601B_SYS_ACL_IDX_ARP_WAN_UNTAG;
		aclRule.pFieldHead = &aclField;
		aclRule.templateIdx = 3;
		aclRule.activePorts.bits[0] = 0x2;
		aclRule.valid = ENABLED;
		aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
		aclRule.act.forwardAct.act = ACL_IGR_FORWARD_COPY_ACT;
		
		aclRule.act.enableAct[ACL_IGR_PRI_ACT]= ENABLED;
		aclRule.act.priAct.act = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
		aclRule.act.priAct.aclPri = 0;
		
		if ((ret = dal_rtl9601b_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
		{
			return ret;
		}	

		memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
		memset(&aclField, 0x00, sizeof(rtk_acl_field_t));
		memset(&aclField2, 0x00, sizeof(rtk_acl_field_t));
		memset(&aclField3, 0x00, sizeof(rtk_acl_field_t));

		aclField.fieldType = ACL_FIELD_USER_DEFINED11;
		aclField.fieldUnion.pattern.data.value = pMode->ipWan & 0xFFFF; 
		aclField.fieldUnion.pattern.data.mask = 0xFFFF;
		aclField.next = &aclField2;
		
		aclField2.fieldType = ACL_FIELD_USER_DEFINED12;
		aclField2.fieldUnion.pattern.data.value = (pMode->ipWan >> 16) & 0xFFFF;
		aclField2.fieldUnion.pattern.data.mask = 0xFFFF;
		aclField2.next = &aclField3;

		aclField3.fieldType = ACL_FIELD_USER_DEFINED15;
		aclField3.fieldUnion.pattern.data.value = 0x0004;
		aclField3.fieldUnion.pattern.data.mask = 0x0004;
		aclField3.next = NULL;

		
		aclRule.index = RTL9601B_SYS_ACL_IDX_ARP_WAN_CTAG;
		aclRule.pFieldHead = &aclField;
		aclRule.templateIdx = 3;

		aclRule.careTag.tags[ACL_CARE_TAG_CTAG].value = ENABLED;
		aclRule.careTag.tags[ACL_CARE_TAG_CTAG].mask = ENABLED;
		aclRule.careTag.tags[ACL_CARE_TAG_STAG].value = DISABLED;
		aclRule.careTag.tags[ACL_CARE_TAG_STAG].mask = ENABLED;

		aclRule.activePorts.bits[0] = 0x2;
		aclRule.valid = ENABLED;
		aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
		aclRule.act.forwardAct.act = ACL_IGR_FORWARD_COPY_ACT;
		aclRule.act.forwardAct.portMask.bits[0] = 0x40;
		
		aclRule.act.enableAct[ACL_IGR_PRI_ACT]= ENABLED;
		aclRule.act.priAct.act = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
		aclRule.act.priAct.aclPri = 0;
		
		if ((ret = dal_rtl9601b_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
		{
			return ret;
		}	

		memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
		memset(&aclField, 0x00, sizeof(rtk_acl_field_t));
		memset(&aclField2, 0x00, sizeof(rtk_acl_field_t));
		memset(&aclField3, 0x00, sizeof(rtk_acl_field_t));

		aclField.fieldType = ACL_FIELD_USER_DEFINED11;
		aclField.fieldUnion.pattern.data.value = pMode->ipWan & 0xFFFF; 
		aclField.fieldUnion.pattern.data.mask = 0xFFFF;
		aclField.next = &aclField2;
		
		aclField2.fieldType = ACL_FIELD_USER_DEFINED12;
		aclField2.fieldUnion.pattern.data.value = (pMode->ipWan >> 16) & 0xFFFF;
		aclField2.fieldUnion.pattern.data.mask = 0xFFFF;
		aclField2.next = &aclField3;

		aclField3.fieldType = ACL_FIELD_USER_DEFINED15;
		aclField3.fieldUnion.pattern.data.value = 0x0004;
		aclField3.fieldUnion.pattern.data.mask = 0x0004;
		aclField3.next = NULL;

		
		aclRule.index = RTL9601B_SYS_ACL_IDX_ARP_WAN_STAG;
		aclRule.pFieldHead = &aclField;
		aclRule.templateIdx = 3;

		aclRule.careTag.tags[ACL_CARE_TAG_CTAG].value = DISABLED;
		aclRule.careTag.tags[ACL_CARE_TAG_CTAG].mask = ENABLED;
		aclRule.careTag.tags[ACL_CARE_TAG_STAG].value = ENABLED;
		aclRule.careTag.tags[ACL_CARE_TAG_STAG].mask = ENABLED;

		aclRule.activePorts.bits[0] = 0x2;
		aclRule.valid = ENABLED;
		aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
		aclRule.act.forwardAct.act = ACL_IGR_FORWARD_COPY_ACT;
		aclRule.act.forwardAct.portMask.bits[0] = 0x40;
		
		aclRule.act.enableAct[ACL_IGR_PRI_ACT]= ENABLED;
		aclRule.act.priAct.act = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
		aclRule.act.priAct.aclPri = 0;
		
		if ((ret = dal_rtl9601b_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
		{
			return ret;
		}	

		
		/*Assign Wan interface unicast for QID 2*/
		memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
		memset(&aclField, 0x00, sizeof(rtk_acl_field_t));

		aclField.fieldType = ACL_FIELD_DMAC;
	    osal_memcpy(&aclField.fieldUnion.mac.value.octet, pMode->macWan.octet, ETHER_ADDR_LEN);
	    osal_memset(&aclField.fieldUnion.mac.mask.octet, 0xFF, ETHER_ADDR_LEN);
		aclField.next = NULL;		
	
		aclRule.index = RTL9601B_SYS_ACL_IDX_WAN_ADDR;
		aclRule.pFieldHead = &aclField;
		aclRule.templateIdx = 2;
		aclRule.activePorts.bits[0] = 0x2;
		aclRule.valid = ENABLED;
		
		aclRule.act.enableAct[ACL_IGR_PRI_ACT]= ENABLED;
		aclRule.act.priAct.act = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
		aclRule.act.priAct.aclPri = 2;
		
		if ((ret = dal_rtl9601b_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
		{
			return ret;
		}		
	}	

	/*ACL 55*/
	/*trap inter control DMAC 00-e0-4c-03-04-05 to CPU and assign priority to Q2*/
	if(pMode->init8696InterCtrl)
	{
		/*Assign Lan interface unicast for QID 2*/
		memset(&aclRule, 0x00, sizeof(rtk_acl_ingress_entry_t));
		memset(&aclField, 0x00, sizeof(rtk_acl_field_t));
		
		aclField.fieldType = ACL_FIELD_DMAC;
		aclField.fieldUnion.mac.value.octet[0] = 0x00;
		aclField.fieldUnion.mac.value.octet[1] = 0xE0;
		aclField.fieldUnion.mac.value.octet[2] = 0x4C;
		aclField.fieldUnion.mac.value.octet[3] = 0x03;
		aclField.fieldUnion.mac.value.octet[4] = 0x04;
		aclField.fieldUnion.mac.value.octet[5] = 0x05;
		osal_memset(&aclField.fieldUnion.mac.mask.octet, 0xFF, ETHER_ADDR_LEN);
		aclField.next = NULL;		
		
		aclRule.index = 55;
		aclRule.pFieldHead = &aclField;
		aclRule.templateIdx = 2;
		aclRule.activePorts.bits[0] = 0x1;
		aclRule.valid = ENABLED;
		
		aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
		aclRule.act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;

		aclRule.act.enableAct[ACL_IGR_PRI_ACT]= ENABLED;
		aclRule.act.priAct.act = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
		aclRule.act.priAct.aclPri = 2;
		
		if ((ret = dal_rtl9601b_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK )
		{
			return ret;
		}		
	}	
	
    return RT_ERR_OK;
}






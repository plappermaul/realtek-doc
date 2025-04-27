/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
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
 * Purpose : PHY 8218B/8218FB/8214FC Driver APIs.
 *
 * Feature : PHY 8218B/8218FB/8214FC Driver APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>
#include <hal/phy/phy_rtl8218b.h>
#include <hal/phy/phy_rtl8218b_patch.h>
#include <hal/mac/miim_common_drv.h>
#include <osal/time.h>
#include <osal/memory.h>
#include <soc/type.h>


/*
 * Symbol Definition
 */
#define PHY_MEDIA_LINKDOWN          (0)
#define PHY_MEDIA_LINKUP            (1)

/* Serial LED Configuration register definition */
#define PHY_8214FC_SERIAL_LED_CONFIG_PAGE                                0x280
#define PHY_8214FC_SERIAL_LED_CONFIG_REG                                 16
#define PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_OFFSET            8
#define PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_MASK              (0xF << PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_OFFSET)
#define PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_OFFSET       6
#define PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_MASK         (0x3 << PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_OFFSET)
#define PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_OFFSET       4
#define PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_MASK         (0x3 << PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_OFFSET)
#define PHY_8214FC_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_OFFSET          3
#define PHY_8214FC_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_MASK            (0x1 << PHY_8214FC_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_OFFSET)

/* Serial LED PER LED Control register definition */
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_MDI_OFFSET                    12
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_MDI_MASK                      (0xF << PHY_8214FC_SERIAL_LED_PER_LED_CTRL_MDI_OFFSET)
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_1000M_SPEED_OFFSET            11
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_1000M_SPEED_MASK              (0x1 << PHY_8214FC_SERIAL_LED_PER_LED_CTRL_1000M_SPEED_OFFSET)
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_100M_SPEED_OFFSET             10
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_100M_SPEED_MASK               (0x1 << PHY_8214FC_SERIAL_LED_PER_LED_CTRL_100M_SPEED_OFFSET)
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_10M_SPEED_OFFSET              9
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_10M_SPEED_MASK                (0x1 << PHY_8214FC_SERIAL_LED_PER_LED_CTRL_10M_SPEED_OFFSET)
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_1000M_ACT_OFFSET              7
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_1000M_ACT_MASK                (0x1 << PHY_8214FC_SERIAL_LED_PER_LED_CTRL_1000M_ACT_OFFSET)
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_100M_ACT_OFFSET               6
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_100M_ACT_MASK                 (0x1 << PHY_8214FC_SERIAL_LED_PER_LED_CTRL_100M_ACT_OFFSET)
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_10M_ACT_OFFSET                5
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_10M_ACT_MASK                  (0x1 << PHY_8214FC_SERIAL_LED_PER_LED_CTRL_10M_ACT_OFFSET)
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_DUPLEX_OFFSET                 3
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_DUPLEX_MASK                   (0x1 << PHY_8214FC_SERIAL_LED_PER_LED_CTRL_DUPLEX_OFFSET)
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_COLLISION_OFFSET              2
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_COLLISION_MASK                (0x1 << PHY_8214FC_SERIAL_LED_PER_LED_CTRL_COLLISION_OFFSET)
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_TX_ACT_OFFSET                 1
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_TX_ACT_MASK                   (0x1 << PHY_8214FC_SERIAL_LED_PER_LED_CTRL_TX_ACT_OFFSET)
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_RX_ACT_OFFSET                 0
#define PHY_8214FC_SERIAL_LED_PER_LED_CTRL_RX_ACT_MASK                   (0x1 << PHY_8214FC_SERIAL_LED_PER_LED_CTRL_RX_ACT_OFFSET)

#define PHY_8214FC_MAX_LED                               36
#define PHY_8214FC_MAX_MDI                               8


typedef struct hal_phy_info_s
{
    uint8   auto_1000f[RTK_MAX_NUM_OF_PORTS]; /* copper, reg[9].bit[9] shadow for patch down-speed mechanism */
} hal_phy_info_t;

typedef enum phy_8218b_regPtp_e
{
    REG_8218B_PTP_BASE              = 0x1600,
    REG_8218B_PTP_TIME_NSEC_L       = 0x1600,
    REG_8218B_PTP_TIME_NSEC_H       = 0x1601,
    REG_8218B_PTP_TIME_SEC_L        = 0x1602,
    REG_8218B_PTP_TIME_SEC_H        = 0x1603,
    REG_8218B_PTP_TIME_CFG_0        = 0x1604,
    REG_8218B_PTP_OTAG_TPID         = 0x1605,
    REG_8218B_PTP_ITAG_TPID         = 0x1606,
    REG_8218B_PTP_MAC_ADDR_L        = 0x1607,
    REG_8218B_PTP_MAC_ADDR_M        = 0x1608,
    REG_8218B_PTP_MAC_ADDR_H        = 0x1609,
    REG_8218B_PTP_TIME_NSEC_L_RO    = 0x160A,
    REG_8218B_PTP_TIME_NSEC_H_RO    = 0x160B,
    REG_8218B_PTP_TIME_SEC_L_RO     = 0x160C,
    REG_8218B_PTP_TIME_SEC_H_RO     = 0x160D,
    REG_8218B_PTP_TIME_CFG_1        = 0x160E,
    REG_8218B_PTP_TIME_INT_STS_P    = 0x160F,

    /* per-port register addr-offset: 0x10 */
    REG_8218B_PTP_TIME_TX_SID_PN    = 0x1610,
    REG_8218B_PTP_TIME_RX_SID_PN    = 0x1614,
    REG_8218B_PTP_TIME_NSEC_L_PN    = 0x1618,
    REG_8218B_PTP_TIME_NSEC_H_PN    = 0x1619,
    REG_8218B_PTP_TIME_SEC_L_PN     = 0x161A,
    REG_8218B_PTP_TIME_SEC_H_PN     = 0x161B,
    REG_8218B_PTP_TIME_CTRL_PN      = 0x161C,
} phy_8218b_regPtp_t;

typedef enum phy_8218b_regPtpField_e
{
    REG_FIELD_8218B_PTP_PHY_EN              = (0x1 << 8),

    REG_FIELD_8218B_PTP_CMD_EXEC            = (0x1 << 15),
    REG_FIELD_8218B_PTP_CMD_OP_MSK          = (0x3 << 12),
    REG_FIELD_8218B_PTP_CMD_OP_READ         = (0x0 << 12),
    REG_FIELD_8218B_PTP_CMD_OP_WRITE        = (0x1 << 12),
    REG_FIELD_8218B_PTP_CMD_OP_ADJ_INC      = (0x2 << 12),
    REG_FIELD_8218B_PTP_CMD_OP_ADJ_DEC      = (0x3 << 12),
    REG_FIELD_8218B_PTP_TIME_NSEC_H         = (0x7FF << 0),

    REG_FIELD_8218B_PTP_CFG_TIMER_EN_FRC    = (0x1 << 2),
    REG_FIELD_8218B_PTP_CFG_TIMER_1588_EN   = (0x1 << 1),

    REG_FIELD_8218B_PTP_TS_OFFLOAD_EN   = (0x1 << 8),
    REG_FIELD_8218B_PTP_INTR_EN   = (0xFF << 0),

} phy_8218b_regPtpField_t;


#define REG_8218B_PTP_OFFSET_PORT(port) ((port % 8) * 0x10)
#define REG_8218B_PTP_OFFSET_MSGTYPE(type) (type % 4)

#if (defined CONFIG_SDK_RTL8218B) || (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC)
static hal_phy_info_t   *pPhy_info[RTK_MAX_NUM_OF_UNIT];
static uint32           phyInfo_alloc[RTK_MAX_NUM_OF_UNIT];
#endif

#if (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC)
static uint32           sdsMode_log[RTK_MAX_NUM_OF_PORTS];
static uint32           portEbl_log[RTK_MAX_NUM_OF_PORTS];
#endif

uint8   phy_8214fc_useBasePatch[RTK_MAX_NUM_OF_UNIT][RTK_MAX_NUM_OF_PORTS] = { {0} };

#ifdef CONFIG_SDK_RTL8214FC
int32 phy_8214fc_sdsEyeParam_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam);

int32 phy_8214fc_sdsEyeParam_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam);
#endif  /* CONFIG_SDK_RTL8214FC */

/*
 * Data Declaration
 */
#if defined(CONFIG_SDK_RTL8218B)
rt_phyInfo_t phy_8218B_info =
{
    .phy_num    = PORT_NUM_IN_8218B,
    .eth_type   = HWP_GE,
    .isComboPhy = {0, 0, 0, 0, 0, 0, 0, 0},
    .flags      = RTK_PHYINFO_FLAG_NONE,

    .xGePhyLocalDuplexAbilityDev            = 0,
    .xGePhyLocalDuplexAbilityAddr           = 0,
    .xGePhyLocalDuplexAbilityBit            = 0,

    .xGePhyLocal1000MSpeedAbilityDev        = 0,
    .xGePhyLocal1000MSpeedAbilityAddr       = 0,
    .xGePhyLocal1000MSpeedAbilityBit        = 0,

    .xGePhyLinkPartner1000MSpeedAbilityDev  = 0,
    .xGePhyLinkPartner1000MSpeedAbilityAddr = 0,
    .xGePhyLinkPartner1000MSpeedAbilityBit  = 0,
};
#endif
#ifdef CONFIG_SDK_RTL8218FB
rt_phyInfo_t phy_8218FB_info =
{
    .phy_num    = PORT_NUM_IN_8218FB,
    .eth_type   = HWP_GE,
    .isComboPhy = {0, 0, 0, 0, 1, 1, 1, 1},
    .flags      = RTK_PHYINFO_FLAG_NONE,

    .xGePhyLocalDuplexAbilityDev            = 0,
    .xGePhyLocalDuplexAbilityAddr           = 0,
    .xGePhyLocalDuplexAbilityBit            = 0,

    .xGePhyLocal1000MSpeedAbilityDev        = 0,
    .xGePhyLocal1000MSpeedAbilityAddr       = 0,
    .xGePhyLocal1000MSpeedAbilityBit        = 0,

    .xGePhyLinkPartner1000MSpeedAbilityDev  = 0,
    .xGePhyLinkPartner1000MSpeedAbilityAddr = 0,
    .xGePhyLinkPartner1000MSpeedAbilityBit  = 0,
};
#endif  /* CONFIG_SDK_RTL8218FB */
#ifdef CONFIG_SDK_RTL8214FC
rt_phyInfo_t phy_8214FC_info =
{
    .phy_num    = PORT_NUM_IN_8214FC,
    .eth_type   = HWP_GE,
    .isComboPhy = {1, 1, 1, 1, 0, 0, 0, 0},
    .flags      = RTK_PHYINFO_FLAG_NONE,

    .xGePhyLocalDuplexAbilityDev            = 0,
    .xGePhyLocalDuplexAbilityAddr           = 0,
    .xGePhyLocalDuplexAbilityBit            = 0,

    .xGePhyLocal1000MSpeedAbilityDev        = 0,
    .xGePhyLocal1000MSpeedAbilityAddr       = 0,
    .xGePhyLocal1000MSpeedAbilityBit        = 0,

    .xGePhyLinkPartner1000MSpeedAbilityDev  = 0,
    .xGePhyLinkPartner1000MSpeedAbilityAddr = 0,
    .xGePhyLinkPartner1000MSpeedAbilityBit  = 0,
};
#endif  /* CONFIG_SDK_RTL8214FC */

int32 _phy_8218b_14fc_intMedia_get(uint32 unit, rtk_port_t port, rtk_port_media_t *media, uint32 *link_status);

#ifdef CONFIG_SDK_RTL8218FB
static int32 phy_8218b_port_is_combo_phy(uint32 unit, rtk_port_t port, rt_phyInfo_t *pPhyInfo);
#endif

#if defined(CONFIG_SDK_RTL8214FC) || defined(CONFIG_SDK_RTL8218FB)
int32 phy_8214fc_fiberLinkUp_handler1(uint32 unit, rtk_port_t port);
#endif  /* defined(CONFIG_SDK_RTL8214FC) || defined(CONFIG_SDK_RTL8218FB) */

void phy_patchBit_set(int unit, int port, int page, int reg, unsigned char endBit,
    unsigned char startBit, unsigned int inVal)
{
    unsigned char   len;
    unsigned char   i;
    unsigned int    val, mask;

    //osal_time_usleep(10);
    len = endBit - startBit + 1;

    if (16 == len)
        val = inVal;
    else
    {
        mask = 0;
        for (i = startBit; i <= endBit; ++i)
            mask |= (1 << i);

        hal_miim_park_read(unit, port, page, 0x1f, reg, &val);
        val &= ~(mask);
        val |= (inVal << startBit);
    }

    hal_miim_park_write(unit, port, page, 0x1f, reg, val);
}

int32 _phy_8218b_autoCrossOver_EEEwakeupTxTimer_set(uint32 unit, rtk_port_t port)
{
    rtk_enable_t    pollSts[PORT_NUM_IN_8218B], pollSts_tmp;
    rtk_port_t      pid;
    uint32          val, modelId, oriData[PORT_NUM_IN_8218B];
    uint32          romId, maxPage = HAL_MIIM_FIX_PAGE;
    int             ret, i;
    uint8           portNum;

    portNum = HWP_PHY_BASE_PHY_MAX(unit, port);

    if (0 != (port % portNum))
        return RT_ERR_OK;

    if (HWP_COMBO_PORT(unit, port))
    {
        if ((ret = hal_miim_read(unit, port, 0, 30, &oriData[port % portNum])) != RT_ERR_OK)
             return ret;

        if ((ret = hal_miim_write(unit, port, 0, 30, 1) != RT_ERR_OK))
            return ret;
    }

    val = 0x4;
    ret = hal_miim_write(unit, port, 0, 27, val);
    if(ret != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 0, 28, &romId)) != RT_ERR_OK)
         return ret;

    if ((ret = hal_miim_read(unit, port, 0, 3, &val)) != RT_ERR_OK)
         return ret;

    if (HWP_COMBO_PORT(unit, port))
    {
        if ((ret = hal_miim_write(unit, port, 0, 30, oriData[port % portNum]) != RT_ERR_OK))
            return ret;
    }

    modelId = (val & ModelNumber_MASK) >> ModelNumber_OFFSET;
    if (PHY_MODEL_ID_RTL8218B_INT == modelId)
    {
        if (romId != 1)
            return RT_ERR_OK;
    }
    else
    {
        if (romId > 2)
            return RT_ERR_OK;
    }

    for (i = 0; i < portNum; ++i)
    {
        int j;

        pid = port + i;

        hal_miim_pollingWa_lock(unit, pid, &pollSts[i]);

        if (HWP_COMBO_PORT(unit, pid))
        {
            if ((ret = hal_miim_read(unit, pid, 0, 30, &oriData[i])) != RT_ERR_OK)
                return ret;

            if ((ret = hal_miim_write(unit, pid, 0, 30, 1) != RT_ERR_OK))
                return ret;
        }

        /* patch request */
        if ((ret = hal_miim_write(unit, pid, 0xb82, 16, 0x10)) != RT_ERR_OK)
            return ret;

        osal_time_usleep(1000);

        /* patch ready */
        for (j = 0; j < 100; ++j)
        {
            if ((ret = hal_miim_read(unit, pid, 0xb80, 16, &val)) != RT_ERR_OK)
                return ret;

            if ((val & 0x40) != 0)
            {
                break;
            }
        }

        if ((val & 0x40) == 0)
        {
            osal_printf("port %u patch ready fail %x\n", pid, val);
        }

        /*  #patch key sram 0x8146 */
        phy_patchBit_set(unit, pid, maxPage, 0x001f, 15, 0, 0x0a43);
        /*  #patch key sram 0x8146 */
        phy_patchBit_set(unit, pid, maxPage, 0x001B, 15, 0, 0x8146);
        if (PHY_MODEL_ID_RTL8218B_INT == modelId)
        {
            phy_patchBit_set(unit, pid, maxPage, 0x001C, 15, 0, (0x7500 + romId));
        }
        else
        {
            phy_patchBit_set(unit, pid, maxPage, 0x001C, 15, 0, (0x7600 + romId));
        }

        /* #patch lock page 0xb82, reg.23.b0 */
        phy_patchBit_set(unit, pid, maxPage, 0x001B, 15, 0, 0xb82e);
        phy_patchBit_set(unit, pid, maxPage, 0x001C, 15, 0, 0x0001);

        phy_patchBit_set(unit, pid, maxPage, 0x001f, 15, 0, 0x0a43);

        phy_patchBit_set(unit, pid, maxPage, 0x1b, 15, 0, 0xB820);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0090);
        phy_patchBit_set(unit, pid, maxPage, 0x1b, 15, 0, 0xA012);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0000);
        phy_patchBit_set(unit, pid, maxPage, 0x1b, 15, 0, 0xA014);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2c04);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2c3f);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2c41);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2c41);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd70c);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x6111);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x6090);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd075);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd188);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2c13);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd076);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd188);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2c13);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x6090);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd077);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd188);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2c13);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd078);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd188);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd03b);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd198);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd700);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x3220);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x3c3e);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd024);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd18b);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd012);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd19b);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x3231);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x4c3e);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd70c);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x6212);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x6111);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x6090);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd07a);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd189);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2c3d);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd07b);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd189);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2c3d);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x6090);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd07c);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd189);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2c3d);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd07d);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd189);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2c3d);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x6111);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x6090);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd07e);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd189);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2c3d);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd07f);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd189);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2c3d);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x6090);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd080);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd189);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2c3d);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd081);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd189);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2511);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x251b);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd090);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2276);
        phy_patchBit_set(unit, pid, maxPage, 0x1b, 15, 0, 0xA01A);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0000);
        phy_patchBit_set(unit, pid, maxPage, 0x1b, 15, 0, 0xA006);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0fff);
        phy_patchBit_set(unit, pid, maxPage, 0x1b, 15, 0, 0xA004);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0fff);
        phy_patchBit_set(unit, pid, maxPage, 0x1b, 15, 0, 0xA002);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0275);
        phy_patchBit_set(unit, pid, maxPage, 0x1b, 15, 0, 0xA000);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x3504);
        phy_patchBit_set(unit, pid, maxPage, 0x1b, 15, 0, 0xB820);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0010);
        phy_patchBit_set(unit, pid, maxPage, 0x1b, 15, 0, 0x8373);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xaf83);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x7faf);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x849c);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xaf84);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x9caf);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x849c);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xe182);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x52ac);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2b12);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xe082);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x47ac);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2303);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xaf0c);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0f02);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x83a0);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0213);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xffaf);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0c0f);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0284);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x4d02);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x1658);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xaf0c);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0ff8);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xf9fa);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xef69);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xfbe0);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x8016);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xad23);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x02ae);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x03af);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x8446);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xbf3a);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x7402);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x3972);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xad28);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xf4e0);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x878a);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xac20);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xeef7);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x20e4);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x878a);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xe087);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x82e1);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x8783);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x1f01);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x9e77);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xee87);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x8800);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xee87);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x891f);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd200);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0284);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x6ebf);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x8784);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x1a92);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xdde0);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x8788);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x1b01);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xab03);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xe587);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x88e0);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x8789);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x1b01);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xaa03);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xe587);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x8912);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xa204);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xdfe0);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x8780);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xe187);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x881b);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x01ab);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x42e0);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x8789);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xe187);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x881b);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x10e0);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x8781);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x1b01);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xab33);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd100);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xbf84);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xa802);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x3934);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd101);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xbf84);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xab02);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x3934);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xbf87);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x90e0);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x8783);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0c02);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x1a90);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xef79);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xbf87);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x84d0);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0402);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x392d);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xe087);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x8310);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xe487);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x83e0);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x878a);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xf721);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xe487);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x8aff);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xef96);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xfefd);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xfc04);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xf8fa);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xef69);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xe080);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x16ad);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2312);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xe087);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x8aee);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x878a);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x00ad);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x2108);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xd100);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xbf84);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xab02);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x3934);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xef96);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xfefc);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x04f9);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xfaef);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x69a2);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0005);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xbf84);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x9cae);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x1aa2);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0105);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xbf84);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x9fae);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x12a2);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0205);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xbf84);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xa2ae);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0aa2);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0305);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xbf84);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xa5ae);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x02ae);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0302);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x3972);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xef96);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xfefd);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0495);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xa8e8);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x95a9);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xe895);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xaae8);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x95ab);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xe8bb);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xa502);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xcca5);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0200);
        phy_patchBit_set(unit, pid, maxPage, 0x1b, 15, 0, 0x8780);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x160c);
        phy_patchBit_set(unit, pid, maxPage, 0x1b, 15, 0, 0x8782);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0500);
        phy_patchBit_set(unit, pid, maxPage, 0x1b, 15, 0, 0xb818);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0bf9);
        phy_patchBit_set(unit, pid, maxPage, 0x1b, 15, 0, 0xb81a);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xfffd);
        phy_patchBit_set(unit, pid, maxPage, 0x1b, 15, 0, 0xb81c);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xfffd);
        phy_patchBit_set(unit, pid, maxPage, 0x1b, 15, 0, 0xb81e);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0xfffd);
        phy_patchBit_set(unit, pid, maxPage, 0x1b, 15, 0, 0xb820);
        phy_patchBit_set(unit, pid, maxPage, 0x1c, 15, 0, 0x0011);
        phy_patchBit_set(unit, pid, maxPage, 0x1f, 15, 0, 0x0000);

        /* ####clear patch_key & patch_lock##### */
        phy_patchBit_set(unit, pid, maxPage, 0x001B, 15, 0, 0x0000);
        phy_patchBit_set(unit, pid, maxPage, 0x001C, 15, 0, 0x0000);
        /*  #patch lock page 0xb82, reg.23.b0 */
        phy_patchBit_set(unit, pid, maxPage, 0x001f, 15, 0, 0x0b82);
        phy_patchBit_set(unit, pid, maxPage, 0x0017, 15, 0, 0x0000);
        phy_patchBit_set(unit, pid, maxPage, 0x001f, 15, 0, 0x0a43);
        /* #patch key sram 0x8146 */
        phy_patchBit_set(unit, pid, maxPage, 0x001B, 15, 0, 0x8146);
        phy_patchBit_set(unit, pid, maxPage, 0x001C, 15, 0, 0x0000);

        /* patch release */
        phy_patchBit_set(unit, pid, 0xb82, 16, 4, 4, 0);
        phy_patchBit_set(unit, pid, maxPage, 31, 15, 0, 0);

        if (HWP_COMBO_PORT(unit, pid))
        {
            hal_miim_write(unit, pid, 0, 30, oriData[i]);
        }

        hal_miim_pollingWa_unlock(unit, pid, pollSts[i]);
    }

    for (i = 0; i < portNum; ++i)
    {
        pid = port + i;

        phy_patchBit_set(unit, pid, maxPage, 0x001b, 15, 0, 0x8016);
        phy_patchBit_set(unit, pid, maxPage, 0x001c, 11, 11, 0x1);

        hal_miim_pollingWa_lock(unit, pid, &pollSts[i]);

        if (HWP_COMBO_PORT(unit, pid))
        {
            if ((ret = hal_miim_write(unit, pid, 0, 30, 1) != RT_ERR_OK))
                return ret;
        }

        hal_miim_pollingWa_unlock(unit, pid, pollSts[i]);
    }

    osal_time_usleep(20000);

    for (i = 0; i < portNum; ++i)
    {
        pid = port + i;

        hal_miim_pollingWa_lock(unit, pid, &pollSts_tmp);

        phy_patchBit_set(unit, pid, maxPage, 0x001f, 15, 0, 0x0a45);
        phy_patchBit_set(unit, pid, maxPage, 0x0016, 2, 0, i);
        phy_patchBit_set(unit, pid, maxPage, 31, 15, 0, 0);
    }

    phy_patchBit_set(unit, port + 3, maxPage, 0x001f, 15, 0, 0x0a45);
    phy_patchBit_set(unit, port + 3, maxPage, 0x0016, 2, 0, 2);
    phy_patchBit_set(unit, port + 3, maxPage, 31, 15, 0, 0);

    if (4 < portNum)
    {
        phy_patchBit_set(unit, port + 7, maxPage, 0x001f, 15, 0, 0x0a45);
        phy_patchBit_set(unit, port + 7, maxPage, 0x0016, 2, 0, 6);
        phy_patchBit_set(unit, port + 7, maxPage, 31, 15, 0, 0);
    }

    for (i = 0; i < portNum; ++i)
    {
        pid = port + i;

        if (HWP_COMBO_PORT(unit, pid))
        {
            hal_miim_write(unit, pid, 0, 30, oriData[i]);
        }

        hal_miim_pollingWa_unlock(unit, pid, pollSts[i]);
    }

    return RT_ERR_OK;
}

/*
 * Function Declaration
 */

#ifdef CONFIG_SDK_RTL8218B
/* Function Name:
 *      phy_8218Bdrv_ge_mapperInit
 * Description:
 *      Initialize PHY 8218B driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void phy_8218Bdrv_ge_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8218B;
    pPhydrv->fPhydrv_init = phy_8218b_init;
    pPhydrv->fPhydrv_enable_set = phy_8218b_enable_set;
    pPhydrv->fPhydrv_ieeeTestMode_set = phy_8218b_ieeeTestMode_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_media_get = phy_8218b_media_get;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_common_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_common_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8218b_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8218b_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_common_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_common_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8218b_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8218b_speed_set;
    pPhydrv->fPhydrv_greenEnable_get = phy_8218b_greenEnable_get;
    pPhydrv->fPhydrv_greenEnable_set = phy_8218b_greenEnable_set;
    pPhydrv->fPhydrv_eeeEnable_get = phy_8218b_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set = phy_8218b_eeeEnable_set;
    pPhydrv->fPhydrv_broadcastEnable_set = phy_8218b_broadcastEnable_set;
    pPhydrv->fPhydrv_broadcastID_set = phy_8218b_broadcastID_set;
    pPhydrv->fPhydrv_eeepEnable_get = phy_8218b_eeepEnable_get;
    pPhydrv->fPhydrv_eeepEnable_set = phy_8218b_eeepEnable_set;
    pPhydrv->fPhydrv_patch_set = phy_8218b_patch_set;
    pPhydrv->fPhydrv_downSpeedEnable_get = phy_8218b_downSpeedEnable_get;
    pPhydrv->fPhydrv_downSpeedEnable_set = phy_8218b_downSpeedEnable_set;
    pPhydrv->fPhydrv_reg_get = phy_common_reg_get;
    pPhydrv->fPhydrv_reg_set = phy_common_reg_set;
    pPhydrv->fPhydrv_reg_park_get = phy_common_reg_park_get;
    pPhydrv->fPhydrv_reg_park_set = phy_common_reg_park_set;
    pPhydrv->fPhydrv_reg_mmd_get = phy_common_reg_mmd_get;
    pPhydrv->fPhydrv_reg_mmd_set = phy_common_reg_mmd_set;
    pPhydrv->fPhydrv_reg_mmd_portmask_set = phy_common_reg_mmd_portmask_set;
    pPhydrv->fPhydrv_reg_extParkPage_get = phy_common_reg_extParkPage_get;
    pPhydrv->fPhydrv_reg_extParkPage_set = phy_common_reg_extParkPage_set;
    pPhydrv->fPhydrv_reg_extParkPage_portmask_set = phy_common_reg_extParkPage_portmask_set;
    pPhydrv->fPhydrv_masterSlave_get = phy_common_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set = phy_common_masterSlave_set;
    pPhydrv->fPhydrv_loopback_get = phy_common_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_common_loopback_set;
    pPhydrv->fPhydrv_rtctResult_get = phy_8218b_rtctResult_get;
    pPhydrv->fPhydrv_rtct_start = phy_8218b_rtct_start;
    pPhydrv->fPhydrv_crossOverMode_get = phy_8218b_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set = phy_8218b_crossOverMode_set;
    pPhydrv->fPhydrv_crossOverStatus_get = phy_8218b_crossOverStatus_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_get = phy_8218b_linkDownPowerSavingEnable_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_set = phy_8218b_linkDownPowerSavingEnable_set;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_get = phy_8218b_ptpSwitchMacAddr_get;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_set = phy_8218b_ptpSwitchMacAddr_set;
    pPhydrv->fPhydrv_ptpRefTime_get = phy_8218b_ptpRefTime_get;
    pPhydrv->fPhydrv_ptpRefTime_set = phy_8218b_ptpRefTime_set;
    pPhydrv->fPhydrv_ptpRefTimeAdjust_set = phy_8218b_ptpRefTimeAdjust_set;
    pPhydrv->fPhydrv_ptpRefTimeEnable_get = phy_8218b_ptpRefTimeEnable_get;
    pPhydrv->fPhydrv_ptpRefTimeEnable_set = phy_8218b_ptpRefTimeEnable_set;
    pPhydrv->fPhydrv_ptpEnable_get = phy_8218b_ptpEnable_get;
    pPhydrv->fPhydrv_ptpEnable_set = phy_8218b_ptpEnable_set;
    pPhydrv->fPhydrv_ptpRxTimestamp_get = phy_8218b_ptpRxTimestamp_get;
    pPhydrv->fPhydrv_ptpTxTimestamp_get = phy_8218b_ptpTxTimestamp_get;
    pPhydrv->fPhydrv_speedStatus_get = phy_common_speedStatusResReg_get;
    pPhydrv->fPhydrv_speedDuplexStatus_get = phy_common_speedDuplexStatusResReg_get;
    pPhydrv->fPhydrv_linkStatus_get = phy_common_linkStatus_get;
#endif

} /* end of phy_8218Bdrv_ge_mapperInit*/
#endif  /* CONFIG_SDK_RTL8218B */
#ifdef CONFIG_SDK_RTL8218FB
/* Add the RTL8218FB driver and hook same function pointer as RTL8218B right now.
 * If the RTL8218FB have the different configure, then will separate the APIs.
 */
/* Function Name:
 *      phy_8218FBdrv_ge_mapperInit
 * Description:
 *      Initialize PHY 8218FB driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void phy_8218FBdrv_ge_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8218FB;
    pPhydrv->fPhydrv_init = phy_8218b_init;
    pPhydrv->fPhydrv_enable_set = phy_8218b_enable_set;
    pPhydrv->fPhydrv_ieeeTestMode_set = phy_8218b_ieeeTestMode_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_media_get = phy_8218fb_media_get;
    pPhydrv->fPhydrv_media_set = phy_8218fb_media_set;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_common_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_common_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8218fb_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8218fb_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_common_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_common_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8218b_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8218fb_speed_set;
    pPhydrv->fPhydrv_greenEnable_get = phy_8218b_greenEnable_get;
    pPhydrv->fPhydrv_greenEnable_set = phy_8218b_greenEnable_set;
    pPhydrv->fPhydrv_eeeEnable_get = phy_8218b_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set = phy_8218b_eeeEnable_set;
    pPhydrv->fPhydrv_fiber_media_get = phy_8218fb_fiber_media_get;
    pPhydrv->fPhydrv_fiber_media_set = phy_8218fb_fiber_media_set;
    pPhydrv->fPhydrv_broadcastEnable_set = phy_8218b_broadcastEnable_set;
    pPhydrv->fPhydrv_broadcastID_set = phy_8218b_broadcastID_set;
    pPhydrv->fPhydrv_eeepEnable_get = phy_8218b_eeepEnable_get;
    pPhydrv->fPhydrv_eeepEnable_set = phy_8218b_eeepEnable_set;
    pPhydrv->fPhydrv_downSpeedEnable_get = phy_8218b_downSpeedEnable_get;
    pPhydrv->fPhydrv_downSpeedEnable_set = phy_8218b_downSpeedEnable_set;
    pPhydrv->fPhydrv_fiberDownSpeedEnable_get = phy_8218fb_fiberDownSpeedEnable_get;
    pPhydrv->fPhydrv_fiberDownSpeedEnable_set = phy_8218fb_fiberDownSpeedEnable_set;
    pPhydrv->fPhydrv_fiberNwayForceLinkEnable_get = phy_8218fb_fiberNwayForceLink_get;
    pPhydrv->fPhydrv_fiberNwayForceLinkEnable_set = phy_8218fb_fiberNwayForceLink_set;
    pPhydrv->fPhydrv_reg_get = phy_common_reg_get;
    pPhydrv->fPhydrv_reg_set = phy_common_reg_set;
    pPhydrv->fPhydrv_reg_park_get = phy_common_reg_park_get;
    pPhydrv->fPhydrv_reg_park_set = phy_common_reg_park_set;
    pPhydrv->fPhydrv_reg_mmd_get = phy_common_reg_mmd_get;
    pPhydrv->fPhydrv_reg_mmd_set = phy_common_reg_mmd_set;
    pPhydrv->fPhydrv_reg_mmd_portmask_set = phy_common_reg_mmd_portmask_set;
    pPhydrv->fPhydrv_reg_extParkPage_get = phy_common_reg_extParkPage_get;
    pPhydrv->fPhydrv_reg_extParkPage_set = phy_common_reg_extParkPage_set;
    pPhydrv->fPhydrv_reg_extParkPage_portmask_set = phy_common_reg_extParkPage_portmask_set;
    pPhydrv->fPhydrv_masterSlave_get = phy_common_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set = phy_common_masterSlave_set;
    pPhydrv->fPhydrv_loopback_get = phy_8218fb_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_8218fb_loopback_set;
    pPhydrv->fPhydrv_fiberTxDis_set = phy_8218fb_fiberTxDis_set;
    pPhydrv->fPhydrv_fiberTxDisPin_set = phy_8218fb_fiberTxDisPin_set;
    pPhydrv->fPhydrv_fiberRx_check = phy_8218fb_fiberRx_check;
    pPhydrv->fPhydrv_fiberRx_reset = phy_8218fb_fiberRx_reset;
    pPhydrv->fPhydrv_fiberRxEnable_get = phy_8218fb_fiberRxEnable_get;
    pPhydrv->fPhydrv_fiberRxEnable_set = phy_8218fb_fiberRxEnable_set;
    pPhydrv->fPhydrv_rtctResult_get = phy_8218b_rtctResult_get;
    pPhydrv->fPhydrv_rtct_start = phy_8218b_rtct_start;
    pPhydrv->fPhydrv_crossOverMode_get = phy_8218b_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set = phy_8218b_crossOverMode_set;
    pPhydrv->fPhydrv_crossOverStatus_get = phy_8218b_crossOverStatus_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_get = phy_8218b_linkDownPowerSavingEnable_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_set = phy_8218b_linkDownPowerSavingEnable_set;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_get = phy_8218b_ptpSwitchMacAddr_get;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_set = phy_8218b_ptpSwitchMacAddr_set;
    pPhydrv->fPhydrv_ptpRefTime_get = phy_8218b_ptpRefTime_get;
    pPhydrv->fPhydrv_ptpRefTime_set = phy_8218b_ptpRefTime_set;
    pPhydrv->fPhydrv_ptpRefTimeAdjust_set = phy_8218b_ptpRefTimeAdjust_set;
    pPhydrv->fPhydrv_ptpRefTimeEnable_get = phy_8218b_ptpRefTimeEnable_get;
    pPhydrv->fPhydrv_ptpRefTimeEnable_set = phy_8218b_ptpRefTimeEnable_set;
    pPhydrv->fPhydrv_ptpEnable_get = phy_8218b_ptpEnable_get;
    pPhydrv->fPhydrv_ptpEnable_set = phy_8218b_ptpEnable_set;
    pPhydrv->fPhydrv_ptpRxTimestamp_get = phy_8218b_ptpRxTimestamp_get;
    pPhydrv->fPhydrv_ptpTxTimestamp_get = phy_8218b_ptpTxTimestamp_get;
    pPhydrv->fPhydrv_speedStatus_get = phy_8218fb_speedStatus_get;
    pPhydrv->fPhydrv_speedDuplexStatus_get = phy_8218fb_speedDuplexStatus_get;
    pPhydrv->fPhydrv_linkStatus_get = phy_common_linkStatus_get;
#endif

} /* end of phy_8218FBdrv_ge_mapperInit*/

/* Function Name:
 *      phy_8218FBdrv_MP_ge_mapperInit
 * Description:
 *      Initialize PHY 8218FB MP driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void phy_8218FBdrv_MP_ge_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8218FB_MP;
    pPhydrv->fPhydrv_init = phy_8218fb_init;
    pPhydrv->fPhydrv_enable_set = phy_8218fb_enable_set;
    pPhydrv->fPhydrv_ieeeTestMode_set = phy_8218b_ieeeTestMode_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_media_get = phy_8218fb_media_get;
    pPhydrv->fPhydrv_media_set = phy_8218fb_media_set;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_8218fb_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_8218fb_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8218fb_MP_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8218fb_MP_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_8218fb_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_8218fb_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8218fb_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8218fb_MP_speed_set;
    pPhydrv->fPhydrv_greenEnable_get = phy_8218b_greenEnable_get;
    pPhydrv->fPhydrv_greenEnable_set = phy_8218b_greenEnable_set;
    pPhydrv->fPhydrv_eeeEnable_get = phy_8218b_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set = phy_8218b_eeeEnable_set;
    pPhydrv->fPhydrv_fiber_media_get = phy_8218fb_fiber_media_get;
    pPhydrv->fPhydrv_fiber_media_set = phy_8218fb_fiber_media_set;
    pPhydrv->fPhydrv_broadcastEnable_set = phy_8218b_broadcastEnable_set;
    pPhydrv->fPhydrv_broadcastID_set = phy_8218b_broadcastID_set;
    pPhydrv->fPhydrv_eeepEnable_get = phy_8218b_eeepEnable_get;
    pPhydrv->fPhydrv_eeepEnable_set = phy_8218b_eeepEnable_set;
    pPhydrv->fPhydrv_patch_set = phy_8218fb_mp_patch_set;
    pPhydrv->fPhydrv_downSpeedEnable_get = phy_8218b_downSpeedEnable_get;
    pPhydrv->fPhydrv_downSpeedEnable_set = phy_8218b_downSpeedEnable_set;
    pPhydrv->fPhydrv_fiberDownSpeedEnable_get = phy_8218fb_fiberDownSpeedEnable_get;
    pPhydrv->fPhydrv_fiberDownSpeedEnable_set = phy_8218fb_fiberDownSpeedEnable_set;
    pPhydrv->fPhydrv_fiberNwayForceLinkEnable_get = phy_8218fb_fiberNwayForceLink_get;
    pPhydrv->fPhydrv_fiberNwayForceLinkEnable_set = phy_8218fb_fiberNwayForceLink_set;
    pPhydrv->fPhydrv_reg_get = phy_common_reg_get;
    pPhydrv->fPhydrv_reg_set = phy_common_reg_set;
    pPhydrv->fPhydrv_reg_park_get = phy_common_reg_park_get;
    pPhydrv->fPhydrv_reg_park_set = phy_common_reg_park_set;
    pPhydrv->fPhydrv_reg_mmd_get = phy_common_reg_mmd_get;
    pPhydrv->fPhydrv_reg_mmd_set = phy_common_reg_mmd_set;
    pPhydrv->fPhydrv_reg_mmd_portmask_set = phy_common_reg_mmd_portmask_set;
    pPhydrv->fPhydrv_reg_extParkPage_get = phy_common_reg_extParkPage_get;
    pPhydrv->fPhydrv_reg_extParkPage_set = phy_common_reg_extParkPage_set;
    pPhydrv->fPhydrv_reg_extParkPage_portmask_set = phy_common_reg_extParkPage_portmask_set;
    pPhydrv->fPhydrv_masterSlave_get = phy_8218fb_MP_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set = phy_8218fb_MP_masterSlave_set;
    pPhydrv->fPhydrv_loopback_get = phy_8218fb_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_8218fb_loopback_set;
    pPhydrv->fPhydrv_fiberTxDis_set = phy_8218fb_fiberTxDis_set;
    pPhydrv->fPhydrv_fiberTxDisPin_set = phy_8218fb_fiberTxDisPin_set;
    pPhydrv->fPhydrv_fiberRx_check = phy_8218fb_fiberRx_check;
    pPhydrv->fPhydrv_fiberRx_reset = phy_8218fb_fiberRx_reset;
    pPhydrv->fPhydrv_fiberLinkUp_handler1 = phy_8214fc_fiberLinkUp_handler1;
    pPhydrv->fPhydrv_fiberRxEnable_get = phy_8218fb_fiberRxEnable_get;
    pPhydrv->fPhydrv_fiberRxEnable_set = phy_8218fb_fiberRxEnable_set;
    pPhydrv->fPhydrv_rtctResult_get = phy_8218b_rtctResult_get;
    pPhydrv->fPhydrv_rtct_start = phy_8218b_rtct_start;
    pPhydrv->fPhydrv_crossOverMode_get = phy_8218b_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set = phy_8218b_crossOverMode_set;
    pPhydrv->fPhydrv_crossOverStatus_get = phy_8218b_crossOverStatus_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_get = phy_8218b_linkDownPowerSavingEnable_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_set = phy_8218b_linkDownPowerSavingEnable_set;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_get = phy_8218b_ptpSwitchMacAddr_get;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_set = phy_8218b_ptpSwitchMacAddr_set;
    pPhydrv->fPhydrv_ptpRefTime_get = phy_8218b_ptpRefTime_get;
    pPhydrv->fPhydrv_ptpRefTime_set = phy_8218b_ptpRefTime_set;
    pPhydrv->fPhydrv_ptpRefTimeAdjust_set = phy_8218b_ptpRefTimeAdjust_set;
    pPhydrv->fPhydrv_ptpRefTimeEnable_get = phy_8218b_ptpRefTimeEnable_get;
    pPhydrv->fPhydrv_ptpRefTimeEnable_set = phy_8218b_ptpRefTimeEnable_set;
    pPhydrv->fPhydrv_ptpEnable_get = phy_8218b_ptpEnable_get;
    pPhydrv->fPhydrv_ptpEnable_set = phy_8218b_ptpEnable_set;
    pPhydrv->fPhydrv_ptpRxTimestamp_get = phy_8218b_ptpRxTimestamp_get;
    pPhydrv->fPhydrv_ptpTxTimestamp_get = phy_8218b_ptpTxTimestamp_get;
    pPhydrv->fPhydrv_speedStatus_get = phy_8218fb_speedStatus_get;
    pPhydrv->fPhydrv_speedDuplexStatus_get = phy_8218fb_speedDuplexStatus_get;
    pPhydrv->fPhydrv_linkStatus_get = phy_common_linkStatus_get;
#endif
} /* end of phy_8218FBdrv_MP_ge_mapperInit*/
#endif  /* CONFIG_SDK_RTL8218FB */
#ifdef CONFIG_SDK_RTL8214FC

/* Function Name:
 *      phy_8214fc_macIntfSerdesLinkStatus_get
 * Description:
 *      Get PHY's MAC side interface serdes link status
 * Input:
 *      unit    - unit ID
 *      port    - Base port ID of the PHY
 * Output:
 *      pStatus - link status of the SerDes
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8214fc_macIntfSerdesLinkStatus_get(uint32 unit, rtk_port_t port,
    rtk_phy_macIntfSdsLinkStatus_t *pStatus)
{
    int32       ret;
    uint32      data, blk;

    RT_PARAM_CHK(NULL == pStatus, RT_ERR_NULL_POINTER);

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    RT_ERR_CHK(hal_miim_read(unit, port, 0, 30, &blk), ret);
    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 8), ret);

    hal_miim_read(unit, port, 0x40F, 0x16, &data);
    hal_miim_read(unit, port, 0x40F, 0x16, &data);

    pStatus->sds_num = 1;
    pStatus->link_status[0] = ((data & 0xF0) == 0xF0) ? PORT_LINKUP : PORT_LINKDOWN;

    hal_miim_write(unit, port, 0, 30, blk);
    return RT_ERR_OK;
}   /* end of phy_8214fc_macIntfSerdesLinkStatus_get */

/* Add the RTL8214FC driver and hook same function pointer as RTL8218B right now.
 * If the RTL8214FC have the different configure, then will separate the APIs.
 */
/* Function Name:
 *      phy_8214FCdrv_ge_mapperInit
 * Description:
 *      Initialize PHY 8214FC driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void phy_8214FCdrv_ge_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8214FC;
    pPhydrv->fPhydrv_init = phy_8218b_init;
    pPhydrv->fPhydrv_enable_set = phy_8218b_enable_set;
    pPhydrv->fPhydrv_ieeeTestMode_set = phy_8218b_ieeeTestMode_set;
    pPhydrv->fPhydrv_ledMode_set = phy_8214fc_ledMode_set;
    pPhydrv->fPhydrv_ledCtrl_get = phy_8214fc_ledCtrl_get;
    pPhydrv->fPhydrv_ledCtrl_set = phy_8214fc_ledCtrl_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_media_get = phy_8214fc_media_get;
    pPhydrv->fPhydrv_media_set = phy_8214fc_media_set;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_common_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_common_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8218fb_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8218fb_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_common_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_common_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8218b_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8214fc_speed_set;
    pPhydrv->fPhydrv_greenEnable_get = phy_8218b_greenEnable_get;
    pPhydrv->fPhydrv_greenEnable_set = phy_8218b_greenEnable_set;
    pPhydrv->fPhydrv_eeeEnable_get = phy_8218b_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set = phy_8218b_eeeEnable_set;
    pPhydrv->fPhydrv_fiber_media_get = phy_8214fc_fiber_media_get;
    pPhydrv->fPhydrv_fiber_media_set = phy_8214fc_fiber_media_set;
    pPhydrv->fPhydrv_broadcastEnable_set = phy_8214fc_broadcastEnable_set;
    pPhydrv->fPhydrv_broadcastID_set = phy_8214fc_broadcastID_set;
    pPhydrv->fPhydrv_eeepEnable_get = phy_8218b_eeepEnable_get;
    pPhydrv->fPhydrv_eeepEnable_set = phy_8218b_eeepEnable_set;
    pPhydrv->fPhydrv_downSpeedEnable_get = phy_8218b_downSpeedEnable_get;
    pPhydrv->fPhydrv_downSpeedEnable_set = phy_8218b_downSpeedEnable_set;
    pPhydrv->fPhydrv_fiberDownSpeedEnable_get = phy_8214fc_fiberDownSpeedEnable_get;
    pPhydrv->fPhydrv_fiberDownSpeedEnable_set = phy_8214fc_fiberDownSpeedEnable_set;
    pPhydrv->fPhydrv_fiberNwayForceLinkEnable_get = phy_8214fc_fiberNwayForceLink_get;
    pPhydrv->fPhydrv_fiberNwayForceLinkEnable_set = phy_8214fc_fiberNwayForceLink_set;
    pPhydrv->fPhydrv_fiberOAMLoopBackEnable_set = phy_8214fc_fiberOAMLoopBack_set;
    pPhydrv->fPhydrv_reg_get = phy_common_reg_get;
    pPhydrv->fPhydrv_reg_set = phy_common_reg_set;
    pPhydrv->fPhydrv_reg_park_get = phy_common_reg_park_get;
    pPhydrv->fPhydrv_reg_park_set = phy_common_reg_park_set;
    pPhydrv->fPhydrv_reg_mmd_get = phy_common_reg_mmd_get;
    pPhydrv->fPhydrv_reg_mmd_set = phy_common_reg_mmd_set;
    pPhydrv->fPhydrv_reg_mmd_portmask_set = phy_common_reg_mmd_portmask_set;
    pPhydrv->fPhydrv_reg_extParkPage_get = phy_common_reg_extParkPage_get;
    pPhydrv->fPhydrv_reg_extParkPage_set = phy_common_reg_extParkPage_set;
    pPhydrv->fPhydrv_reg_extParkPage_portmask_set = phy_common_reg_extParkPage_portmask_set;
    pPhydrv->fPhydrv_masterSlave_get = phy_common_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set = phy_common_masterSlave_set;
    pPhydrv->fPhydrv_loopback_get = phy_8214fc_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_8214fc_loopback_set;
    pPhydrv->fPhydrv_fiberTxDis_set = phy_8214fc_fiberTxDis_set;
    pPhydrv->fPhydrv_fiberTxDisPin_set = phy_8214fc_fiberTxDisPin_set;
    pPhydrv->fPhydrv_fiberRx_check = phy_8214fc_fiberRx_check;
    pPhydrv->fPhydrv_fiberRx_reset = phy_8214fc_fiberRx_reset;
    pPhydrv->fPhydrv_fiberRxEnable_get = phy_8214fc_fiberRxEnable_get;
    pPhydrv->fPhydrv_fiberRxEnable_set = phy_8214fc_fiberRxEnable_set;
    pPhydrv->fPhydrv_rtctResult_get = phy_8218b_rtctResult_get;
    pPhydrv->fPhydrv_rtct_start = phy_8218b_rtct_start;
    pPhydrv->fPhydrv_crossOverMode_get = phy_8218b_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set = phy_8218b_crossOverMode_set;
    pPhydrv->fPhydrv_crossOverStatus_get = phy_8218b_crossOverStatus_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_get = phy_8218b_linkDownPowerSavingEnable_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_set = phy_8218b_linkDownPowerSavingEnable_set;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_get = phy_8218b_ptpSwitchMacAddr_get;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_set = phy_8218b_ptpSwitchMacAddr_set;
    pPhydrv->fPhydrv_ptpRefTime_get = phy_8218b_ptpRefTime_get;
    pPhydrv->fPhydrv_ptpRefTime_set = phy_8218b_ptpRefTime_set;
    pPhydrv->fPhydrv_ptpRefTimeAdjust_set = phy_8218b_ptpRefTimeAdjust_set;
    pPhydrv->fPhydrv_ptpRefTimeEnable_get = phy_8218b_ptpRefTimeEnable_get;
    pPhydrv->fPhydrv_ptpRefTimeEnable_set = phy_8218b_ptpRefTimeEnable_set;
    pPhydrv->fPhydrv_ptpEnable_get = phy_8218b_ptpEnable_get;
    pPhydrv->fPhydrv_ptpEnable_set = phy_8218b_ptpEnable_set;
    pPhydrv->fPhydrv_ptpRxTimestamp_get = phy_8218b_ptpRxTimestamp_get;
    pPhydrv->fPhydrv_ptpTxTimestamp_get = phy_8218b_ptpTxTimestamp_get;
    pPhydrv->fPhydrv_speedStatus_get = phy_8214fc_speedStatus_get;
    pPhydrv->fPhydrv_speedDuplexStatus_get = phy_8214fc_speedDuplexStatus_get;
    pPhydrv->fPhydrv_sdsEyeParam_get = phy_8214fc_sdsEyeParam_get;
    pPhydrv->fPhydrv_sdsEyeParam_set = phy_8214fc_sdsEyeParam_set;
    pPhydrv->fPhydrv_macIntfSerdesLinkStatus_get = phy_8214fc_macIntfSerdesLinkStatus_get;
    pPhydrv->fPhydrv_linkStatus_get = phy_common_linkStatus_get;
    pPhydrv->fPhydrv_intr_init = phy_8214fc_intr_init;
    pPhydrv->fPhydrv_intrEnable_get = phy_8214fc_intrEnable_get;
    pPhydrv->fPhydrv_intrEnable_set = phy_8214fc_intrEnable_set;
    pPhydrv->fPhydrv_intrStatus_get = phy_8214fc_intrStatus_get;
    pPhydrv->fPhydrv_intrMask_get = phy_8214fc_intrMask_get;
    pPhydrv->fPhydrv_intrMask_set = phy_8214fc_intrMask_set;
#endif
} /* end of phy_8214FCdrv_ge_mapperInit*/

/* Function Name:
 *      phy_8214FCdrv_MP_ge_mapperInit
 * Description:
 *      Initialize PHY 8214FC driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void phy_8214FCdrv_MP_ge_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8214FC_MP;
    pPhydrv->fPhydrv_init = phy_8214fc_init;
    pPhydrv->fPhydrv_enable_set = phy_8214fc_enable_set;
    pPhydrv->fPhydrv_ieeeTestMode_set = phy_8218b_ieeeTestMode_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_media_get = phy_8214fc_media_get;
    pPhydrv->fPhydrv_media_set = phy_8214fc_media_set;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_8214fc_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_8214fc_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8214fc_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8214fc_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_8214fc_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_8214fc_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8214fc_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8214fc_MP_speed_set;
    pPhydrv->fPhydrv_greenEnable_get = phy_8218b_greenEnable_get;
    pPhydrv->fPhydrv_greenEnable_set = phy_8218b_greenEnable_set;
    pPhydrv->fPhydrv_eeeEnable_get = phy_8218b_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set = phy_8218b_eeeEnable_set;
    pPhydrv->fPhydrv_fiber_media_get = phy_8214fc_fiber_media_get;
    pPhydrv->fPhydrv_fiber_media_set = phy_8214fc_fiber_media_set;
    pPhydrv->fPhydrv_broadcastEnable_set = phy_8214fc_broadcastEnable_set;
    pPhydrv->fPhydrv_broadcastID_set = phy_8214fc_broadcastID_set;
    pPhydrv->fPhydrv_eeepEnable_get = phy_8218b_eeepEnable_get;
    pPhydrv->fPhydrv_eeepEnable_set = phy_8218b_eeepEnable_set;
    pPhydrv->fPhydrv_patch_set = phy_8214fc_mp_patch_set;
    pPhydrv->fPhydrv_downSpeedEnable_get = phy_8218b_downSpeedEnable_get;
    pPhydrv->fPhydrv_downSpeedEnable_set = phy_8218b_downSpeedEnable_set;
    pPhydrv->fPhydrv_fiberDownSpeedEnable_get = phy_8214fc_fiberDownSpeedEnable_get;
    pPhydrv->fPhydrv_fiberDownSpeedEnable_set = phy_8214fc_fiberDownSpeedEnable_set;
    pPhydrv->fPhydrv_fiberNwayForceLinkEnable_get = phy_8214fc_fiberNwayForceLink_get;
    pPhydrv->fPhydrv_fiberNwayForceLinkEnable_set = phy_8214fc_fiberNwayForceLink_set;
    pPhydrv->fPhydrv_fiberOAMLoopBackEnable_set = phy_8214fc_fiberOAMLoopBack_set;
    pPhydrv->fPhydrv_reg_get = phy_common_reg_get;
    pPhydrv->fPhydrv_reg_set = phy_common_reg_set;
    pPhydrv->fPhydrv_reg_park_get = phy_common_reg_park_get;
    pPhydrv->fPhydrv_reg_park_set = phy_common_reg_park_set;
    pPhydrv->fPhydrv_reg_mmd_get = phy_common_reg_mmd_get;
    pPhydrv->fPhydrv_reg_mmd_set = phy_common_reg_mmd_set;
    pPhydrv->fPhydrv_reg_mmd_portmask_set = phy_common_reg_mmd_portmask_set;
    pPhydrv->fPhydrv_reg_extParkPage_get = phy_common_reg_extParkPage_get;
    pPhydrv->fPhydrv_reg_extParkPage_set = phy_common_reg_extParkPage_set;
    pPhydrv->fPhydrv_reg_extParkPage_portmask_set = phy_common_reg_extParkPage_portmask_set;
    pPhydrv->fPhydrv_masterSlave_get = phy_8214fc_MP_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set = phy_8214fc_MP_masterSlave_set;
    pPhydrv->fPhydrv_loopback_get = phy_8214fc_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_8214fc_loopback_set;
    pPhydrv->fPhydrv_fiberTxDis_set = phy_8214fc_fiberTxDis_set;
    pPhydrv->fPhydrv_fiberTxDisPin_set = phy_8214fc_fiberTxDisPin_set;
    pPhydrv->fPhydrv_fiberRx_check = phy_8214fc_fiberRx_check;
    pPhydrv->fPhydrv_fiberRx_reset = phy_8214fc_fiberRx_reset;
    pPhydrv->fPhydrv_fiberRxEnable_get = phy_8214fc_fiberRxEnable_get;
    pPhydrv->fPhydrv_fiberRxEnable_set = phy_8214fc_fiberRxEnable_set;
    pPhydrv->fPhydrv_fiberLinkUp_handler1 = phy_8214fc_fiberLinkUp_handler1;
    pPhydrv->fPhydrv_rtctResult_get = phy_8218b_rtctResult_get;
    pPhydrv->fPhydrv_rtct_start = phy_8218b_rtct_start;
    pPhydrv->fPhydrv_crossOverMode_get = phy_8218b_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set = phy_8218b_crossOverMode_set;
    pPhydrv->fPhydrv_crossOverStatus_get = phy_8218b_crossOverStatus_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_get = phy_8218b_linkDownPowerSavingEnable_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_set = phy_8218b_linkDownPowerSavingEnable_set;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_get = phy_8218b_ptpSwitchMacAddr_get;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_set = phy_8218b_ptpSwitchMacAddr_set;
    pPhydrv->fPhydrv_ptpRefTime_get = phy_8218b_ptpRefTime_get;
    pPhydrv->fPhydrv_ptpRefTime_set = phy_8218b_ptpRefTime_set;
    pPhydrv->fPhydrv_ptpRefTimeAdjust_set = phy_8218b_ptpRefTimeAdjust_set;
    pPhydrv->fPhydrv_ptpRefTimeEnable_get = phy_8218b_ptpRefTimeEnable_get;
    pPhydrv->fPhydrv_ptpRefTimeEnable_set = phy_8218b_ptpRefTimeEnable_set;
    pPhydrv->fPhydrv_ptpEnable_get = phy_8218b_ptpEnable_get;
    pPhydrv->fPhydrv_ptpEnable_set = phy_8218b_ptpEnable_set;
    pPhydrv->fPhydrv_ptpRxTimestamp_get = phy_8218b_ptpRxTimestamp_get;
    pPhydrv->fPhydrv_ptpTxTimestamp_get = phy_8218b_ptpTxTimestamp_get;
    pPhydrv->fPhydrv_speedStatus_get = phy_8214fc_speedStatus_get;
    pPhydrv->fPhydrv_speedDuplexStatus_get = phy_8214fc_speedDuplexStatus_get;
    pPhydrv->fPhydrv_sdsEyeParam_get = phy_8214fc_sdsEyeParam_get;
    pPhydrv->fPhydrv_sdsEyeParam_set = phy_8214fc_sdsEyeParam_set;
    pPhydrv->fPhydrv_macIntfSerdesLinkStatus_get = phy_8214fc_macIntfSerdesLinkStatus_get;
    pPhydrv->fPhydrv_linkStatus_get = phy_common_linkStatus_get;
    pPhydrv->fPhydrv_intr_init = phy_8214fc_intr_init;
    pPhydrv->fPhydrv_intrEnable_get = phy_8214fc_intrEnable_get;
    pPhydrv->fPhydrv_intrEnable_set = phy_8214fc_intrEnable_set;
    pPhydrv->fPhydrv_intrStatus_get = phy_8214fc_intrStatus_get;
    pPhydrv->fPhydrv_intrMask_get = phy_8214fc_intrMask_get;
    pPhydrv->fPhydrv_intrMask_set = phy_8214fc_intrMask_set;
#endif
} /* end of phy_8214FCdrv_MP_ge_mapperInit*/
#endif  /* CONFIG_SDK_RTL8214FC */
#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8218B)
/* Function Name:
 *      phy_8380drv_int_ge_mapperInit
 * Description:
 *      Initialize PHY 8380 driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void phy_8380drv_int_ge_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8380_INT_GE;
    pPhydrv->fPhydrv_init = phy_8218b_init;
    pPhydrv->fPhydrv_enable_set = phy_8380_int_enable_set;
    pPhydrv->fPhydrv_ieeeTestMode_set = phy_8218b_ieeeTestMode_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_media_get = phy_8218b_media_get;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_common_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_common_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8218b_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8218b_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_common_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_common_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8218b_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8218b_speed_set;
    pPhydrv->fPhydrv_greenEnable_get = phy_8218b_greenEnable_get;
    pPhydrv->fPhydrv_greenEnable_set = phy_8218b_greenEnable_set;
    pPhydrv->fPhydrv_eeeEnable_get = phy_8218b_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set = phy_8218b_eeeEnable_set;
    pPhydrv->fPhydrv_broadcastEnable_set = phy_8218b_broadcastEnable_set;
    pPhydrv->fPhydrv_broadcastID_set = phy_8218b_broadcastID_set;
    pPhydrv->fPhydrv_eeepEnable_get = phy_8218b_eeepEnable_get;
    pPhydrv->fPhydrv_eeepEnable_set = phy_8218b_eeepEnable_set;
    pPhydrv->fPhydrv_downSpeedEnable_get = phy_8218b_downSpeedEnable_get;
    pPhydrv->fPhydrv_downSpeedEnable_set = phy_8218b_downSpeedEnable_set;
    pPhydrv->fPhydrv_reg_get = phy_common_reg_get;
    pPhydrv->fPhydrv_reg_set = phy_common_reg_set;
    pPhydrv->fPhydrv_reg_park_get = phy_common_reg_park_get;
    pPhydrv->fPhydrv_reg_park_set = phy_common_reg_park_set;
    pPhydrv->fPhydrv_reg_mmd_get = phy_common_reg_mmd_get;
    pPhydrv->fPhydrv_reg_mmd_set = phy_common_reg_mmd_set;
    pPhydrv->fPhydrv_reg_mmd_portmask_set = phy_common_reg_mmd_portmask_set;
    pPhydrv->fPhydrv_reg_extParkPage_get = phy_common_reg_extParkPage_get;
    pPhydrv->fPhydrv_reg_extParkPage_set = phy_common_reg_extParkPage_set;
    pPhydrv->fPhydrv_reg_extParkPage_portmask_set = phy_common_reg_extParkPage_portmask_set;
    pPhydrv->fPhydrv_masterSlave_get = phy_common_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set = phy_common_masterSlave_set;
    pPhydrv->fPhydrv_loopback_get = phy_common_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_common_loopback_set;
    pPhydrv->fPhydrv_rtctResult_get = phy_int8380_rtctResult_get;   /* phy_8218b_rtctResult_get, */
    pPhydrv->fPhydrv_rtct_start = phy_8218b_rtct_start;
    pPhydrv->fPhydrv_crossOverMode_get = phy_8218b_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set = phy_8218b_crossOverMode_set;
    pPhydrv->fPhydrv_crossOverStatus_get = phy_8218b_crossOverStatus_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_get = phy_8218b_linkDownPowerSavingEnable_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_set = phy_8218b_linkDownPowerSavingEnable_set;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_get = phy_8218b_ptpSwitchMacAddr_get;
    pPhydrv->fPhydrv_ptpSwitchMacAddr_set = phy_8218b_ptpSwitchMacAddr_set;
    pPhydrv->fPhydrv_ptpRefTime_get = phy_8218b_ptpRefTime_get;
    pPhydrv->fPhydrv_ptpRefTime_set = phy_8218b_ptpRefTime_set;
    pPhydrv->fPhydrv_ptpRefTimeAdjust_set = phy_8218b_ptpRefTimeAdjust_set;
    pPhydrv->fPhydrv_ptpRefTimeEnable_get = phy_8218b_ptpRefTimeEnable_get;
    pPhydrv->fPhydrv_ptpRefTimeEnable_set = phy_8218b_ptpRefTimeEnable_set;
    pPhydrv->fPhydrv_ptpEnable_get = phy_8218b_ptpEnable_get;
    pPhydrv->fPhydrv_ptpEnable_set = phy_8218b_ptpEnable_set;
    pPhydrv->fPhydrv_ptpRxTimestamp_get = phy_8218b_ptpRxTimestamp_get;
    pPhydrv->fPhydrv_ptpTxTimestamp_get = phy_8218b_ptpTxTimestamp_get;
    pPhydrv->fPhydrv_linkStatus_get = phy_common_linkStatus_get;
#endif
} /* end of phy_8380drv_int_ge_mapperInit*/
#endif  /* defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8218B) */

#if (defined CONFIG_SDK_RTL8218B) || (defined CONFIG_SDK_RTL8218FB)

#if !defined(__BOOTLOADER__)
int32 _phy_8218b_tsOffloadEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);
int32 _phy_8218b_intrEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);
#endif

/* Function Name:
 *      phy_8218b_init
 * Description:
 *      Initialize PHY 8218.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218b_init(uint32 unit, rtk_port_t port)
{
    uint32  base_port = 0;
    uint32  val;
    int32   ret = RT_ERR_FAILED;
    rtk_port_media_t restore_media;

    if ((base_port = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "unit=%u,port=%u get base port ID fail", unit, port);
        return RT_ERR_PORT_ID;
    }

    /* Initialize the PHY software shadow */
    if (phyInfo_alloc[unit] == 0)
    {
        pPhy_info[unit] = (hal_phy_info_t *)osal_alloc(sizeof(hal_phy_info_t));
        if (NULL == pPhy_info[unit])
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "memory allocate failed");
            return RT_ERR_FAILED;
        }
        osal_memset(pPhy_info[unit], 0, sizeof(hal_phy_info_t));
        phyInfo_alloc[unit] = 1;
    }

    if ((ret = hal_miim_write(unit, base_port, 0xa42, 29, 0x0000)) != RT_ERR_OK)
        return ret;

    /*
     * 0b00: mean receive IPG 64 bit time
     * 0b01: mean receive IPG 32 bit time
     * For Marvell EEE IOP issue, Need to take care to RTL8218B
     */

    /*Update RTCT threshold*/

    if ((ret = phy_8218b_media_get(unit, port, &restore_media)) != RT_ERR_OK)
        return ret;

    if (restore_media != PORT_MEDIA_COPPER)
    {
        if ((ret = phy_8218b_media_set(unit, port, PORT_MEDIA_COPPER)) != RT_ERR_OK)
            return ret;
    }
     /* Set default value for reset checking in waMon */
     if ((ret = hal_miim_write(unit, base_port, 0, 27, 0x801e)) != RT_ERR_OK)
         return ret;
     if ((ret = hal_miim_write(unit, base_port, 0, 28, 0x1)) != RT_ERR_OK)
         return ret;

#if defined(CONFIG_SDK_RTL8380)
    /* For 838x & 833x, set related regs to enable serdes always linkup */
    if (HWP_8380_30_FAMILY(unit))
    {
        uint32 phy_data;

        /*18B or 14FC*/
        if((port %8) == 0)
        {
            phy_data = 8;
            ret = hal_miim_write(unit,  port,  0,  30, phy_data);
            if(ret != RT_ERR_OK)
                return ret;

            phy_data = 0x9703;
            ret = hal_miim_write(unit,  port,  0x404,  0x10, phy_data);
            if(ret != RT_ERR_OK)
                return ret;

            phy_data = 0x9403;
            ret = hal_miim_write(unit,  port,  0x424,  0x10, phy_data);
            if(ret != RT_ERR_OK)
                return ret;

            phy_data = 0;
            ret = hal_miim_write(unit,  port,  0,  30, phy_data);
            if(ret != RT_ERR_OK)
                return ret;
        }
    }

    /* For 838x & 833x, Set PKTGEN RXPATH is from MAC TX & Enable PKTGEN RX for Phy error counter */
    if (HWP_8380_30_FAMILY(unit))
    {
        uint32 phy_data;

        ret = hal_miim_read(unit,  port,  0xc80,  16, &phy_data);
        if(ret != RT_ERR_OK)
            return ret;

        phy_data &= ~0xF;
        phy_data |= 0x1<<2;
        phy_data |= 0x2<<0;
        ret = hal_miim_write(unit,  port,  0xc80,  16, phy_data);
        if(ret != RT_ERR_OK)
            return ret;
    }
#endif

    /* Read the chip copper Auto-1000F ability value and initial the shadow value */
    if ((ret = hal_miim_read(unit, port, 0xa42, 9, &val)) != RT_ERR_OK)
        return ret;
    /* store to shadow */
    (*pPhy_info[unit]).auto_1000f[port] = (val >> _1000Base_TFullDuplex_OFFSET) & 0x1;

    if (restore_media != PORT_MEDIA_COPPER)
    {
        if ((ret = phy_8218b_media_set(unit, port, restore_media)) != RT_ERR_OK)
            return ret;
    }

    phy_8218b_greenEnable_set(unit, port, DISABLED);

    if (0 == (port % PORT_NUM_IN_8218B))
    {
        if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8214FC)
        {
            if (phy_8214fc_useBasePatch[unit][port] != 1)   /* if base patch is not used, then use original one */
            {
                _phy_8218b_autoCrossOver_EEEwakeupTxTimer_set(unit, port);
            }
        }
        else
        {
            _phy_8218b_autoCrossOver_EEEwakeupTxTimer_set(unit, port);
        }
    }

  #if !defined(__BOOTLOADER__)
    _phy_8218b_tsOffloadEnable_set(unit, port, ENABLED);
    _phy_8218b_intrEnable_set(unit, port, ENABLED);
  #endif

    return RT_ERR_OK;
} /* end of phy_8218b_init */

/* Function Name:
 *      phy_8218b_enable_set
 * Description:
 *      Set PHY interface status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - admin configuration of PHY interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218b_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  phyData, phyData_restore;
    rtk_port_media_t media;

    if ((ret = phy_8218b_media_get(unit, port, &media)) != RT_ERR_OK)
    {
        return ret;
    }



    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData)) != RT_ERR_OK)
        return ret;


    if (DISABLED == enable)
    {
        phyData |= (1 << PowerDown_OFFSET);
    }else{
        if((phyData & PowerDown_MASK) != 0) /*Original Status is disable*/
        {
            phyData &= ~(PowerDown_MASK);
            /*E0015926*/
            if(media == PORT_MEDIA_COPPER)
            {
                phyData_restore = phyData;
                if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
                    return ret;

                phyData |= (1 << Reset_OFFSET);
                if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
                    return ret;
                while(1)
                {
                    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData)) != RT_ERR_OK)
                        return ret;
                    if((phyData & Reset_MASK) == 0)
                    {
                        phyData = phyData_restore;
                        break;
                    }
                }
            }
        }else{  /*Original Status is enable*/
            phyData &= ~(PowerDown_MASK);
        }
    }

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
} /* end of phy_8218b_enable_set */

#endif  /* (defined CONFIG_SDK_RTL8218B) || (defined CONFIG_SDK_RTL8218FB) */

/* Function Name:
 *      phy_8380_int_enable_set
 * Description:
 *      Set PHY interface status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - admin configuration of PHY interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8380_int_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  phyData, phyData_restore;
    rtk_port_media_t media;

    if ((ret = phy_8218b_media_get(unit, port, &media)) != RT_ERR_OK)
    {
        return ret;
    }


    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData)) != RT_ERR_OK)
        return ret;


    if (DISABLED == enable)
    {
        /* isolate first */
        phyData &= ~(Isolate_MASK);
        phyData |= (1 << Isolate_OFFSET);
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
            return ret;

        /* power down */
        phyData &= ~(PowerDown_MASK);
        phyData |= (1 << PowerDown_OFFSET);
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
            return ret;

        /* isolate release */
        phyData &= ~(Isolate_MASK);
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
            return ret;
    }
    else
    {
        if((phyData & PowerDown_MASK) != 0) /*Original Status is disable*/
        {
            phyData &= ~(PowerDown_MASK);

            /*E0015926*/
            if(media == PORT_MEDIA_COPPER)
            {
                phyData_restore = phyData;
                if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
                    return ret;

                phyData |= (1 << Reset_OFFSET);
                if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
                    return ret;
                while(1)
                {
                    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData)) != RT_ERR_OK)
                        return ret;
                    if((phyData & Reset_MASK) == 0)
                    {
                        phyData = phyData_restore;
                        break;
                    }
                }
            }
        }
        else
        {
            /*Original Status is enable*/
            phyData &= ~(PowerDown_MASK);
        }

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
            return ret;
    }
    return RT_ERR_OK;
} /* end of phy_8380_int_enable_set */


#if (defined CONFIG_SDK_RTL8214FC) || (defined CONFIG_SDK_RTL8218FB)
/*
 * Function Declaration
 */
/* Function Name:
 *      phy_8214fc_init
 * Description:
 *      Initialize PHY 8214fc.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214fc_init(uint32 unit, rtk_port_t port)
{
    uint32  base_port = 0;
    uint32  val;
    uint32 phy_data;
    int32   ret = RT_ERR_FAILED;

    if ((base_port = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "unit=%u,port=%u get base port ID fail", unit, port);
        return RT_ERR_PORT_ID;
    }

    /* Initialize the PHY software shadow */
    if (phyInfo_alloc[unit] == 0)
    {
        pPhy_info[unit] = (hal_phy_info_t *)osal_alloc(sizeof(hal_phy_info_t));
        if (NULL == pPhy_info[unit])
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "memory allocate failed");
            return RT_ERR_FAILED;
        }
        osal_memset(pPhy_info[unit], 0, sizeof(hal_phy_info_t));
        phyInfo_alloc[unit] = 1;
    }

    if ((ret = hal_miim_write(unit, base_port, 0xa42, 29, 0x0000)) != RT_ERR_OK)
        return ret;

    /*
     * 0b00: mean receive IPG 64 bit time
     * 0b01: mean receive IPG 32 bit time
     * For Marvell EEE IOP issue, Need to take care to RTL8218B
     */

    /*Update RTCT threshold*/

    if ((ret = phy_8214fc_media_set(unit, port, PORT_MEDIA_COPPER)) != RT_ERR_OK)
        return ret;

    /* Set default value for reset checking in waMon */
    if ((ret = hal_miim_write(unit, base_port, 0, 29, 0x8)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, base_port, 0x268, 16, &phy_data)) != RT_ERR_OK)
         return ret;

    phy_data &= ~(0xF<<12);
    phy_data |= (0xA<<12);

    if ((ret = hal_miim_write(unit, base_port, 0x268, 16, phy_data)) != RT_ERR_OK)
         return ret;

    if ((ret = hal_miim_write(unit, base_port, 0, 29, 0x0)) != RT_ERR_OK)
        return ret;

#if defined(CONFIG_SDK_RTL8380)
    /* For 838x & 833x, set related regs to enable serdes always linkup */
    if (HWP_8380_30_FAMILY(unit))
    {
        uint32 phy_data;

        /*18B or 14FC*/
        if((port %8) == 0)
        {
            phy_data = 8;
            ret = hal_miim_write(unit,  port,  0,  30, phy_data);
            if(ret != RT_ERR_OK)
                return ret;

            phy_data = 0x9703;
            ret = hal_miim_write(unit,  port,  0x404,  0x10, phy_data);
            if(ret != RT_ERR_OK)
                return ret;

            phy_data = 0x9403;
            ret = hal_miim_write(unit,  port,  0x424,  0x10, phy_data);
            if(ret != RT_ERR_OK)
                return ret;

            phy_data = 0;
            ret = hal_miim_write(unit,  port,  0,  30, phy_data);
            if(ret != RT_ERR_OK)
                return ret;
        }
    }

    /* For 838x & 833x, Set PKTGEN RXPATH is from MAC TX & Enable PKTGEN RX for Phy error counter */
    if (HWP_8380_30_FAMILY(unit))
    {
        uint32 phy_data;

        ret = hal_miim_read(unit,  port,  0xc80,  16, &phy_data);
        if(ret != RT_ERR_OK)
            return ret;

        phy_data &= ~0xF;
        phy_data |= 0x1<<2;
        phy_data |= 0x2<<0;
        ret = hal_miim_write(unit,  port,  0xc80,  16, phy_data);
        if(ret != RT_ERR_OK)
            return ret;
    }
#endif

    /* Read the chip copper Auto-1000F ability value and initial the shadow value */
    if ((ret = hal_miim_read(unit, port, 0xa42, 9, &val)) != RT_ERR_OK)
        return ret;
    /* store to shadow */
    (*pPhy_info[unit]).auto_1000f[port] = (val >> _1000Base_TFullDuplex_OFFSET) & 0x1;


    if ((ret = phy_8214fc_media_set(unit, port, PORT_MEDIA_COPPER)) != RT_ERR_OK)
        return ret;

    phy_8214fc_fiber_media_set(unit, port, PORT_FIBER_MEDIA_1000);
    phy_8214fc_fiberDownSpeedEnable_set(unit, port, DISABLED);

    phy_8218b_greenEnable_set(unit, port, DISABLED);

    if (0 == (port % PORT_NUM_IN_8214FC))
    {
        if (phy_8214fc_useBasePatch[unit][port] != 1) /* if base patch is not used, then use original one */
        {
            _phy_8218b_autoCrossOver_EEEwakeupTxTimer_set(unit, port);
        }
    }

    return RT_ERR_OK;
} /* end of phy_8214fc_init */
#endif  /* (defined CONFIG_SDK_RTL8214FC) || (defined CONFIG_SDK_RTL8218FB) */

#ifdef CONFIG_SDK_RTL8218FB
/*
 * Function Declaration
 */
/* Function Name:
 *      phy_8218fb_init
 * Description:
 *      Initialize PHY 8218fb.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218fb_init(uint32 unit, rtk_port_t port)
{
    if (phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info))
    {
        phy_8214fc_init(unit, port);
    }
    else
    {
        phy_8218b_init(unit, port);
    }

    return RT_ERR_OK;
} /* end of phy_8218fb_init */

/* Function Name:
 *      phy_8218fb_enable_set
 * Description:
 *      Set PHY interface status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - admin configuration of PHY interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218fb_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    if (phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info))
        return phy_8214fc_enable_set(unit, port, enable);
    else
        return phy_8218b_enable_set(unit, port, enable);

    return RT_ERR_OK;
} /* end of phy_8218fb_enable_set */

#endif

/* Function Name:
 *      _phy_8214fc_combo_media_get
 * Description:
 *      Get 8214FC & 8218FB combo port media type.
 * Input:
 *      unit        - unit id
 *      basePort    - base port id
 *      reg         - port media register
 * Output:
 *      pMedia - pointer buffer of phy media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      (1) media type is as following:
 *          - PORT_MEDIA_COPPER
 *          - PORT_MEDIA_FIBER
 */
int32
_phy_8214fc_combo_media_get(uint32 unit, rtk_port_t basePort, uint32 reg,
    rtk_port_media_t *pMedia)
{
    uint32  val;
    int32   ret = RT_ERR_FAILED, rv;

    RT_PARAM_CHK((NULL == pMedia), RT_ERR_NULL_POINTER);

    if ((ret = hal_miim_write(unit, basePort, 0xa42, 29, 0x0008)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, basePort, 0x266, reg, &val)) != RT_ERR_OK)
        goto ERR;

    /* bit[10]: 0b0 (auto mode), 0b1 (force mode)
     * bit[11]: 0b0 - prefer fiber media if bit[11]=0b0, force fiber media if bit[11]=0b1
     *          0b1 - prefer copper media if bit[11]=0b0, force copper media if bit[11]=0b1
     */
    if (0x400 == (val & 0x400))
    {
        if (0x800 == (val & 0x800))
            *pMedia = PORT_MEDIA_COPPER;
        else
            *pMedia = PORT_MEDIA_FIBER;
    }
    else
    {
        ret = RT_ERR_TYPE;
    }
ERR:
    if ((rv = hal_miim_write(unit, basePort, 0xa42, 29, 0x0000)) != RT_ERR_OK)
        return rv;

    return ret;
}   /* end of _phy_8214fc_combo_media_get */

#if (defined CONFIG_SDK_RTL8214FC) || (defined CONFIG_SDK_RTL8218FB)

/* Function Name:
 *      _phy_8214fc_combo_media_set
 * Description:
 *      Set 8214FC & 8218FB combo port media type.
 * Input:
 *      unit        - unit id
 *      basePort    - base port id
 *      port        - port id
 *      reg         - port media register
 *      media       - phy media type
 * Output:
 *      pMedia - pointer buffer of phy media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 * Note:
 *      (1) media type is as following:
 *          - PORT_MEDIA_COPPER
 *          - PORT_MEDIA_FIBER
 */
int32
_phy_8214fc_combo_media_set(uint32 unit, rtk_port_t basePort, rtk_port_t port,
    uint32 reg, rtk_port_media_t media)
{
    uint32  val;
    uint32  phyOriDataCopper, phyOriDataFiber;
    uint32  phyNewDataCopper, phyNewDataFiber;
    int32   ret = RT_ERR_FAILED, rv;

    /* only store the powerDown bit in combo port */
    /* In auto mode, link from fiber to copper cause to copper power up */
    /* copper */
    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0001)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 0xa40, 16, &phyOriDataCopper)) != RT_ERR_OK)
        goto ERR;

    /* fiber */
    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0003)) != RT_ERR_OK)
        goto ERR;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 16, &phyOriDataFiber)) != RT_ERR_OK)
        goto ERR;

    if ((ret = hal_miim_write(unit, basePort, 0xa42, 29, 0x0008)) != RT_ERR_OK)
        goto ERR;

    if ((ret = hal_miim_read(unit, basePort, 0x266, reg, &val)) != RT_ERR_OK)
        goto ERR;

    switch (media)
    {
        case PORT_MEDIA_COPPER:
            val |= (1<<10);
            val |= (1<<11);
            break;
        case PORT_MEDIA_FIBER:
            val |= (1<<10);
            val &= ~(1<<11);
            break;
        default:
            ret = RT_ERR_INPUT;
            goto ERR;
            break;
    }

    if ((ret = hal_miim_write(unit, basePort, 0x266, reg, val)) != RT_ERR_OK)
        goto ERR;

    if ((ret = hal_miim_write(unit, basePort, 0xa42, 29, 0x0000)) != RT_ERR_OK)
        goto ERR;

    /* only store the powerDown bit in combo port */
    /* In auto mode, link from fiber to copper cause to copper power up */
    /* Configure copper */
    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0001)) != RT_ERR_OK)
        goto ERR;

    if ((ret = hal_miim_read(unit, port, 0xa40, 16, &phyNewDataCopper)) != RT_ERR_OK)
        goto ERR;

    if ((phyOriDataCopper & PowerDown_MASK) != (phyNewDataCopper & PowerDown_MASK))
    {
        if (phyOriDataCopper & PowerDown_MASK)
            phyNewDataCopper |= (1 << PowerDown_OFFSET);
        else
            phyNewDataCopper &= ~(PowerDown_MASK);
    }

    if ((ret = hal_miim_write(unit, port, 0xa40, 16, phyNewDataCopper)) != RT_ERR_OK)
        goto ERR;

    /* Configure fiber */
    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0003)) != RT_ERR_OK)
        goto ERR;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 16, &phyNewDataFiber)) != RT_ERR_OK)
        goto ERR;

    if ((phyOriDataFiber & PowerDown_MASK) != (phyNewDataFiber & PowerDown_MASK))
    {
        if (phyOriDataFiber & PowerDown_MASK)
            phyNewDataFiber |= (1 << PowerDown_OFFSET);
        else
            phyNewDataFiber &= ~(PowerDown_MASK);
    }

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 16, phyNewDataFiber)) != RT_ERR_OK)
        goto ERR;

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 3)) != RT_ERR_OK)
        goto ERR;

    if ((ret = hal_miim_read(unit, port, 8, 20, &val)) != RT_ERR_OK)
        goto ERR;

    /* default setting */
    val &= ~(0xF << 12);

    if (PORT_MEDIA_COPPER == media)
        val |= (7 << 12);      /* force fiber port to 2.5G */
    else
    {
        if(ENABLED == portEbl_log[port])
        {
            val |= sdsMode_log[port] << 12;
        }
        else
        {
            val |= 7 << 12;   /* force fiber port to 2.5G */
        }
    }

    if ((ret = hal_miim_write(unit, port, 8, 20, val)) != RT_ERR_OK)
        goto ERR;

    if ((ret = hal_miim_read(unit, port, 0xC, 16, &val)) != RT_ERR_OK)
        goto ERR;

    /* default setting */
    val &= ~(0xFF);
    /* force fiber port tx */
    if (PORT_MEDIA_COPPER == media)
        val |= 0xEF;
    if ((ret = hal_miim_write(unit, port, 0xC, 16, val)) != RT_ERR_OK)
        goto ERR;

ERR:
    if ((rv = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0000)) != RT_ERR_OK)
        return rv;

    return ret;
}   /* end of _phy_8214fc_combo_media_set */
#endif  /* (defined CONFIG_SDK_RTL8214FC) || (defined CONFIG_SDK_RTL8218FB) */


#if (defined CONFIG_SDK_RTL8218B) || (defined CONFIG_SDK_RTL8218FB)
/* Function Name:
 *      phy_8218b_media_get
 * Description:
 *      Get PHY 8218B media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The media type is return PORT_MEDIA_COPPER.
 */
int32
phy_8218b_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    *pMedia = PORT_MEDIA_COPPER;
    return RT_ERR_OK;
} /* end of phy_8218b_media_get */

/* Function Name:
 *      phy_8218b_media_set
 * Description:
 *      Get PHY 8218B media type.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - phy media type
 * Output:
 *      None
 * Return:
 *      RT_ERR_CHIP_NOT_SUPPORTED - RTL8218B doesn't support media set
 * Note:
 *      (1) media type is as following:
 *          - PORT_MEDIA_COPPER
 *          - PORT_MEDIA_FIBER
 */
int32
phy_8218b_media_set(uint32 unit, rtk_port_t port, rtk_port_media_t media)
{

    return RT_ERR_PORT_NOT_SUPPORTED;

} /* end of phy_8218b_media_set */

/* Function Name:
 *      phy_8218b_greenEnable_get
 * Description:
 *      Get the status of link-up green feature of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - pointer to status of link-up green feature
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8218b is supported the per-port link-up green feature.
 */
int32
phy_8218b_greenEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  phyData, fixed_page;
    rtk_port_media_t    media;
    uint32              link_sts;

    fixed_page = 0;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if((PHY_MEDIA_LINKUP == link_sts) && (PORT_MEDIA_FIBER == media))
    {
        /* register is in copper page and can't access when fiber is linkup. */
        return RT_ERR_PHY_FIBER_LINKUP;
    }else{
        /* Configure the GPHY page to copper */
        if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0001)) != RT_ERR_OK)
            return ret;

        /* get value from CHIP*/
        if ((ret = hal_miim_write(unit, port, fixed_page, 27, 0x8011)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
            return ret;

        if ((phyData >> 9) & 0x1)
            *pEnable = ENABLED;
        else
            *pEnable = DISABLED;

        /* Configure the GPHY page to auto */
        if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0000)) != RT_ERR_OK)
            return ret;
    }
    return RT_ERR_OK;
} /* end of phy_8218b_greenEnable_get */

/* Function Name:
 *      phy_8218b_greenEnable_set
 * Description:
 *      Set the status of link-up green feature of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of link-up  green feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8218b is supported the per-port link-up green feature.
 */
int32
phy_8218b_greenEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  value, fixed_page;
    int32   ret = RT_ERR_FAILED;
    rtk_port_media_t    media;
    uint32              link_sts;
    uint32  phyData0, restore_phyData0, restore_flag;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if((PHY_MEDIA_LINKUP == link_sts) && (PORT_MEDIA_FIBER == media))
    {
        /* register is in copper page and can't access when fiber is linkup. */
        return RT_ERR_PHY_FIBER_LINKUP;
    }else{
        /* Configure the GPHY page to copper */
        if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0001)) != RT_ERR_OK)
            return ret;

        fixed_page = 0;
        restore_flag = 0;

        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
            return ret;

        restore_phyData0 = phyData0;

        /* If the port is link up, link down at first */
        if ((phyData0 & (PowerDown_MASK)) == 0)
        {
            restore_flag = 1;
            phyData0 |= (1 << PowerDown_OFFSET);
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
                return ret;
        }

        /* get value from CHIP*/
        if ((ret = hal_miim_write(unit, port, fixed_page, 27, 0x8011)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_read(unit, port, fixed_page, 28, &value)) != RT_ERR_OK)
            return ret;

        if (ENABLED == enable)
        {
            value |= (1 << 9);
            if ((ret = hal_miim_write(unit, port, fixed_page, 27, 0x8011)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, fixed_page, 28, value)) != RT_ERR_OK)
                return ret;
        }
        else
        {
            value &= ~(1 << 9);
            if ((ret = hal_miim_write(unit, port, fixed_page, 27, 0x8011)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, fixed_page, 28, value)) != RT_ERR_OK)
                return ret;
        }

        /* To restore Reg 0*/
        if (restore_flag == 1)
        {
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, restore_phyData0)) != RT_ERR_OK)
                return ret;
        }

        /* Configure the GPHY page to auto */
        if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0000)) != RT_ERR_OK)
            return ret;
    }
    return RT_ERR_OK;
} /* end of phy_8218b_greenEnable_set */

/* Function Name:
 *      phy_8218b_eeeEnable_get
 * Description:
 *      Get enable status of EEE function in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of EEE
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8218b_eeeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  phyData;
    uint32              eee_sts;
    rtk_port_media_t    media;
    uint32              link_sts;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if((PHY_MEDIA_LINKUP == link_sts) && (PORT_MEDIA_FIBER == media))
    {
        /* register is in copper page and can't access when fiber is linkup. */
        return RT_ERR_PHY_FIBER_LINKUP;
    }else{

        /* Configure the GPHY page to copper */
        if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0001)) != RT_ERR_OK)
            return ret;

        ret = hal_miim_mmd_read(unit, port, 7, 60, &phyData);
        if ((ret != RT_ERR_OK)&&(ret != RT_ERR_CHIP_NOT_SUPPORTED))
        {
            return ret;
        }

        eee_sts = REG32_FIELD_GET(phyData, 2, (0x1 << 2)); /* 1000M EEE ability */

        if (eee_sts)
            *pEnable = ENABLED;
        else
            *pEnable = DISABLED;

        /* Configure the GPHY page to auto */
        if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0000)) != RT_ERR_OK)
            return ret;
    }
    return RT_ERR_OK;
} /* end of phy_8218b_eeeEnable_get */

/* Function Name:
 *      phy_8218_eeeEnable_set
 * Description:
 *      Set enable status of EEE function in the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of EEE
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8218b_eeeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  phyData;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t     an_enable;
    rtk_port_media_t    media;
    uint32              link_sts;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if((PHY_MEDIA_LINKUP == link_sts) && (PORT_MEDIA_FIBER == media))
    {
        /* register is in copper page and can't access when fiber is linkup. */
        return RT_ERR_PHY_FIBER_LINKUP;
    }else{

        /* Configure the GPHY page to copper */
        if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0001)) != RT_ERR_OK)
            return ret;

        phy_common_autoNegoEnable_get(unit, port, &an_enable);

        /* get value from CHIP*/
        if ((ret = hal_miim_read(unit, port, 0xa43, 25, &phyData)) != RT_ERR_OK)
                return ret;

        #if 0 /* always configure to MAC mode EEE, the mode configuration could be aparted to another API in the future */
        if (ENABLED == enable)
        {
            phyData |= (1 << 5);
            if ((ret = hal_miim_write(unit, port, 0xa43, 25, phyData)) != RT_ERR_OK)
                return ret;
        }
        else
        #else
        {
            phyData &= ~(1 << 5);
            if ((ret = hal_miim_write(unit, port, 0xa43, 25, phyData)) != RT_ERR_OK)
                return ret;
        }
        #endif

        if(ENABLED == enable)
            phyData = 0x6; /*enable 100M/1000M EEE ability*/
        else
            phyData = 0x0; /*disable 100M/1000M EEE ability*/

        ret = hal_miim_mmd_write(unit, port, 7, 60, phyData);
        if ((ret != RT_ERR_OK)&&(ret != RT_ERR_CHIP_NOT_SUPPORTED))
        {
            return ret;
        }

        if ((ret = hal_miim_read(unit, port, 0xa42, 20, &phyData)) != RT_ERR_OK)
        {
            return ret;
        }

        if(ENABLED == enable)
            phyData |= 0x1 << 7; /*enable 500M EEE ability*/
        else
            phyData &= ~(0x1 << 7); /*disable 500M EEE ability*/

        if ((ret = hal_miim_write(unit, port, 0xa42, 20, phyData)) != RT_ERR_OK)
        {
            return ret;
        }

        /* Force re-autonegotiation if AN is on*/
        if (ENABLED == an_enable)
        {
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData)) != RT_ERR_OK)
                return ret;

            phyData = phyData & ~(RestartAutoNegotiation_MASK);
            phyData = phyData | (an_enable << RestartAutoNegotiation_OFFSET);

            if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
                return ret;
        }

        /* Configure the GPHY page to auto */
        if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0000)) != RT_ERR_OK)
            return ret;
    }
    return RT_ERR_OK;
} /* end of phy_8218b_eeeEnable_set */

#endif  /* (defined CONFIG_SDK_RTL8218B) || (defined CONFIG_SDK_RTL8218FB) */

#if defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8218FB) || defined(CONFIG_SDK_RTL8214FC)
/* Function Name:
 *      phy_8218b_auto_1000f_get
 * Description:
 *      Get PHY 8218B/8218FB/8214FC copper 1000f ability from shadow.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pAbility - pointer to copper 1000f ability
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. copper 1000f ability value is as following:
 *      - 0: not the ability
 *      - 1: have the ability
 */
int32
phy_8218b_auto_1000f_get(uint32 unit, rtk_port_t port, uint32 *pAbility)
{
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPhy_info[unit]), RT_ERR_NULL_POINTER);

    *pAbility = (*pPhy_info[unit]).auto_1000f[port];
    return RT_ERR_OK;
} /* end of phy_8218b_auto_1000f_get */

/* Function Name:
 *      phy_8218b_auto_1000f_set
 * Description:
 *      Set PHY 8218B/8218FB/8214FC copper 1000f ability to shadow.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      ability  - copper 1000f ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. copper 1000f ability value is as following:
 *      - 0: not the ability
 *      - 1: have the ability
 */
int32
phy_8218b_auto_1000f_set(uint32 unit, rtk_port_t port, uint32 ability)
{
    RT_PARAM_CHK((ability != 0) && (ability != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPhy_info[unit]), RT_ERR_NULL_POINTER);

    (*pPhy_info[unit]).auto_1000f[port] = ability;
    return RT_ERR_OK;
} /* end of phy_8218b_auto_1000f_set */
#endif  /* defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8218FB) || defined(CONFIG_SDK_RTL8214FC) */

/* Function Name:
 *      phy_8214fc_media_get
 * Description:
 *      Get PHY 8214FC media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      (1) media type is as following:
 *          - PORT_MEDIA_COPPER
 *          - PORT_MEDIA_FIBER
 */
int32
phy_8214fc_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    uint32  base_port = 0, reg = 0;
    int32   ret = RT_ERR_FAILED;

    RT_PARAM_CHK((NULL == pMedia), RT_ERR_NULL_POINTER);

    switch (port % PORT_NUM_IN_8214FC)
    {
        case 0:
            reg = 16;
            break;
        case 1:
            reg = 19;
            break;
        case 2:
            reg = 20;
            break;
        case 3:
            reg = 21;
            break;
        default:
            return RT_ERR_FAILED;
    }

    base_port = port - (port % PORT_NUM_IN_8214FC);
    if ((ret = _phy_8214fc_combo_media_get(unit, base_port, reg, pMedia)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8214fc_media_get */

#if (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC)

/* Function Name:
 *      phy_8214fc_media_set
 * Description:
 *      Get PHY 8214FC media type.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - phy media type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      (1) media type is as following:
 *          - PORT_MEDIA_COPPER
 *          - PORT_MEDIA_FIBER
 */
int32
phy_8214fc_media_set(uint32 unit, rtk_port_t port, rtk_port_media_t media)
{
    uint32  base_port = 0, reg = 0, phyPortId;
    int32   ret = RT_ERR_FAILED;

    if ((base_port = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

    if ((phyPortId = HWP_PHY_PORTID(unit, port)) >= MAX_PHY_NUM_OF_CHIP)
    {
        return RT_ERR_PORT_ID;
    }
    switch (phyPortId)
    {
        case 0:
            reg = 16;
            break;
        case 1:
            reg = 19;
            break;
        case 2:
            reg = 20;
            break;
        case 3:
            reg = 21;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = _phy_8214fc_combo_media_set(unit, base_port, port, reg, media)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8214fc_media_set */
#endif  /* (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC) */

#ifdef CONFIG_SDK_RTL8214FC
/* Function Name:
 *      phy_8214fc_dumpTop
 * Description:
 *      Dump Top infomrantion
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
void phy_8214fc_dumpTop(uint32 unit, rtk_port_t port)
{
    int rtl8214_phy0_macid = port - (port % 8);
    int page, reg;
    unsigned int val;
    int base_id;

    base_id = rtl8214_phy0_macid;
    osal_printf("port = %u\n", port);

    /*Switch to right TOP address*/
    hal_miim_write(unit,base_id, 0, 29, 0x0008);

    osal_printf("########################################################################\n");
    osal_printf("  Page  | Reg-16  Reg-17  Reg-18  Reg-19  Reg-20  Reg-21  Reg-22  Reg-23\n");
    osal_printf("------------------------------------------------------------------------\n");
    for (page = 0x260; page <= 0x278; page++)
    {
        hal_miim_write(unit,base_id, 0, 31, page);
        osal_printf(" 0x%04X | ", page);
        for (reg = 16; reg <= 23; reg++)
        {
            hal_miim_read(unit, base_id, page, reg, &val);
            if (reg == 23)
                osal_printf("0x%04X\n", val);
            else
                osal_printf("0x%04X  ", val);
        }
    }
    osal_printf("########################################################################\n");
    osal_printf("  Page  | Reg-16  Reg-17  Reg-18  Reg-19  Reg-20  Reg-21  Reg-22  Reg-23\n");
    osal_printf("------------------------------------------------------------------------\n");
    for (page = 0x460; page <= 0x469; page++)
    {
        hal_miim_write(unit,base_id, 0, 31, page);
        osal_printf(" 0x%04X | ", page);
        for (reg = 16; reg <= 23; reg++)
        {
            hal_miim_read(unit, base_id, page, reg, &val);
            //OSAL_PRINTF("### Page = 0x%x, Reg=0x%x, Val=0x%04x ###\n", page, reg, val);
            if (reg == 23)
                osal_printf("0x%04X\n", val);
            else
                osal_printf("0x%04X  ", val);
        }
    }
    osal_printf("########################################################################\n");
    osal_printf("  Page  | Reg-16  Reg-17  Reg-18  Reg-19  Reg-20  Reg-21  Reg-22  Reg-23\n");
    osal_printf("------------------------------------------------------------------------\n");
    for (page = 0x404; page <= 0x40f; page++)
    {
        hal_miim_write(unit,base_id, 0, 31, page);
        osal_printf(" 0x%04X | ", page);
        for (reg = 16; reg <= 23; reg++)
        {
            hal_miim_read(unit, base_id, page, reg, &val);
            //OSAL_PRINTF("### Page = 0x%x, Reg=0x%x, Val=0x%04x ###\n", page, reg, val);
            if (reg == 23)
                osal_printf("0x%04X\n", val);
            else
                osal_printf("0x%04X  ", val);
        }
    }
    osal_printf("########################################################################\n");
    osal_printf("  Page  | Reg-16  Reg-17  Reg-18  Reg-19  Reg-20  Reg-21  Reg-22  Reg-23\n");
    osal_printf("------------------------------------------------------------------------\n");
    for (page = 0x420; page <= 0x42f; page++)
    {
        hal_miim_write(unit,base_id, 0, 31, page);
        osal_printf(" 0x%04X | ", page);
        for (reg = 16; reg <= 23; reg++)
        {
            hal_miim_read(unit, base_id, page, reg, &val);
            //OSAL_PRINTF("### Page = 0x%x, Reg=0x%x, Val=0x%04x ###\n", page, reg, val);
            if (reg == 23)
                osal_printf("0x%04X\n", val);
            else
                osal_printf("0x%04X  ", val);
        }
    }
    osal_printf("########################################################################\n");

    hal_miim_write(unit,base_id, 0, 29, 0x0000);
    return;
}/* end of phy_8214fc_dumpTop */
#endif  /* CONFIG_SDK_RTL8214FC */

/* Function Name:
 *      phy_8214fc_fiberDownSpeedEnable_get
 * Description:
 *      Get fiber down speed status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of fiber down speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
phy_8214fc_fiberDownSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    uint32  phyData;
    int32   ret;

    if ((ret = hal_miim_write(unit, port, 0, 29, 3)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 8, 17, &phyData)) != RT_ERR_OK)
        goto ERR;

    if (phyData & (1 << 5))
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    if ((ret = hal_miim_write(unit, port, 0, 29, 0)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
ERR:
    hal_miim_write(unit, port, 0, 29, 0);
    return ret;
}

/* Function Name:
 *      phy_8214fc_fiberDownSpeedEnable_set
 * Description:
 *      Set fiber down speed status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of fiber down speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
phy_8214fc_fiberDownSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    uint32  phyData;
    int32   ret;

    if ((ret = hal_miim_write(unit, port, 0, 29, 3)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 8, 17, &phyData)) != RT_ERR_OK)
        goto ERR;

    if (ENABLED == enable)
        phyData |= (1 << 5);
    else
        phyData &= ~(1 << 5);

    if ((ret = hal_miim_write(unit, port, 8, 17, phyData)) != RT_ERR_OK)
        goto ERR;

    if ((ret = hal_miim_write(unit, port, 0, 29, 0)) != RT_ERR_OK)
        return ret;

    return ret;
ERR:
    hal_miim_write(unit, port, 0, 29, 0);
    return ret;
}

#if (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC)
/* Function Name:
 *      phy_8214fc_fiber_media_get
 * Description:
 *      Get PHY 8214FC fiber media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy fiber media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. fiber media type value is as following:
 *      - PORT_FIBER_MEDIA_1000
 *      - PORT_FIBER_MEDIA_AUTO
 *      - PORT_FIBER_MEDIA_100
 */
int32
phy_8214fc_fiber_media_get(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t *pMedia)
{
    int32   ret = RT_ERR_OK;

    RT_PARAM_CHK((NULL == pMedia), RT_ERR_NULL_POINTER);

    switch (sdsMode_log[port])
    {
        case 0:
            *pMedia = PORT_FIBER_MEDIA_AUTO;
            break;
        case 9:
            *pMedia = PORT_FIBER_MEDIA_1000;
            break;
        case 11:
            *pMedia = PORT_FIBER_MEDIA_100;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return ret;
} /* end of phy_8214fc_fiber_media_get */

/* Function Name:
 *      phy_8214fc_fiber_media_set
 * Description:
 *      Get PHY 8214FC fiber media type.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - phy media fiber type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      1. fiber media type value is as following:
 *      - PORT_FIBER_MEDIA_1000
 *      - PORT_FIBER_MEDIA_AUTO
 */
int32
phy_8214fc_fiber_media_set(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t media)
{
    uint32  val, config;
    int32   ret = RT_ERR_FAILED;

    if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0003)) != RT_ERR_OK)
        return ret;

    switch (media)
    {
        case PORT_FIBER_MEDIA_AUTO:
            config = 0;
            break;

        case PORT_FIBER_MEDIA_1000:
            config = 9;
            break;

        case PORT_FIBER_MEDIA_100:
            config = 11;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            goto ERR;
    }

    if (ENABLED == portEbl_log[port])
    {
        if ((ret = hal_miim_read(unit, port, 8, 0x14, &val)) != RT_ERR_OK)
            goto ERR;

        val &= ~(0xF << 12);
        val |= (config << 12);

        if ((ret = hal_miim_write(unit, port, 8, 0x14, val)) != RT_ERR_OK)
        goto ERR;
    }

    sdsMode_log[port] = config;

ERR:
    if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0000)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8214fc_fiber_media_set */

/* Function Name:
 *      phy_8214fc_enable_set
 * Description:
 *      Set PHY interface status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - admin configuration of PHY interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214fc_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtk_port_media_t    media;
    uint32              phyData0;
    uint32              page, reg;
    int32               ret;

    if ((ret = phy_8214fc_media_get(unit, port, &media)) != RT_ERR_OK)
        return ret;

    reg = PHY_INT_REG_BASE + PHY_CONTROL_REG;

    /* for fiber force and auto */
    if (PORT_MEDIA_COPPER != media)
    {
        page = PHY_PAGE_0;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0003)) != RT_ERR_OK)
            goto ERR;

        if ((ret = hal_miim_read(unit, port, page, reg, &phyData0)) != RT_ERR_OK)
            goto ERR;

        phyData0 &= ~(PowerDown_MASK);

        if ((ret = hal_miim_write(unit, port, page, reg, phyData0)) != RT_ERR_OK)
            goto ERR;

        if(DISABLED == enable)
        {
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 20, &phyData0)) != RT_ERR_OK)
                goto ERR;
            phyData0 &= ~(0xF000);
            phyData0 |= 0x7000; /*frc_sds_mode_en=1, frc_sds_mode=3*/
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 20, phyData0)) != RT_ERR_OK)
                goto ERR;
        }
        else
        {
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 20, &phyData0)) != RT_ERR_OK)
                goto ERR;
            phyData0 &= ~(0xF000);
            phyData0 |= sdsMode_log[port] << 12; /*back to original value*/
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 20, phyData0)) != RT_ERR_OK)
                goto ERR;
        }
    }

    /* for copper force and auto */
    if (PORT_MEDIA_FIBER != media)
    {
        page = 0xa40;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0001)) != RT_ERR_OK)
            goto ERR;

        if ((ret = hal_miim_read(unit, port, page, reg, &phyData0)) != RT_ERR_OK)
            goto ERR;

        phyData0 &= ~(PowerDown_MASK);
        if (DISABLED == enable)
            phyData0 |= (1 << PowerDown_OFFSET);

        if ((ret = hal_miim_write(unit, port, page, reg, phyData0)) != RT_ERR_OK)
            goto ERR;
    }

    portEbl_log[port] = enable;

ERR:
    hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0000);
    hal_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, 0x0000);
    return ret;
} /* end of phy_8214fc_enable_set */

#endif  /* (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC) */

/* Function Name:
 *      _phy_8218b_14fc_intMedia_get
 * Description:
 *      Get 8214fc internal media
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      media         - internal media
 *      link_status  - media link status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
_phy_8218b_14fc_intMedia_get(uint32 unit, rtk_port_t port, rtk_port_media_t *media, uint32 *link_status)
{
    hal_control_t   *pHalCtrl;
    uint32              phyData1;
    int32               ret;
    uint8               is_fiber_linkup = 0, is_copper_linkup = 0;


    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    /* get media */
    if ((ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_media_get(unit, port, media)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData1)) != RT_ERR_OK)
        goto ERR;
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData1)) != RT_ERR_OK)
        goto ERR;

    if (phyData1 & LinkStatus_MASK)
    {
        *link_status = PHY_MEDIA_LINKUP;
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_EXTENDED_STATUS_REG, &phyData1)) != RT_ERR_OK)
            goto ERR;
        if((phyData1 & _1000BaseX_FullDuplex_R15_MASK) || (phyData1 & _1000BaseX_HalfDuplex_R15_MASK))
        {
            is_fiber_linkup = PHY_MEDIA_LINKUP; /*Link up media is Fiber*/
        }
        if((phyData1 & _1000Base_TFullDuplex_R15_MASK) || (phyData1 & _1000Base_THalfDuplex_R15_MASK))
        {
            is_copper_linkup = PHY_MEDIA_LINKUP; /*Link up media is Copper*/
        }

    }else{
        is_fiber_linkup = PHY_MEDIA_LINKDOWN;
        is_copper_linkup = PHY_MEDIA_LINKDOWN;
        *link_status = PHY_MEDIA_LINKDOWN;
    }

    if (PORT_MEDIA_FIBER == *media || PORT_MEDIA_COPPER == *media)
        return RT_ERR_OK;

    /* nego media */
    if (is_copper_linkup == is_fiber_linkup)
    {
        *media = PORT_MEDIA_COPPER;
    }
    else if (is_fiber_linkup)
        *media = PORT_MEDIA_FIBER;
    else
        *media = PORT_MEDIA_COPPER;

    return RT_ERR_OK;
ERR:
    return ret;
}   /* end of _phy_8218b_14fc_intMedia_get */

#ifdef CONFIG_SDK_RTL8218FB
/* Function Name:
 *      phy_8218b_is_combo_phy_port
 * Description:
 *      Check the port is Combo or not
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      TRUE     - Yes, the port is combo phy
 *      FALSE    - No, the port is not combo phy
 * Note:
 *      None
 */
static int32
phy_8218b_port_is_combo_phy(uint32 unit, rtk_port_t port, rt_phyInfo_t *pPhyInfo)
{
    uint32      phyPortId; /* PHY port ID */
    uint32      basePort;

    if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return FALSE;
    }

    if ((phyPortId = HWP_PHY_PORTID(unit, port)) >= MAX_PHY_NUM_OF_CHIP)
    {
        return FALSE;
    }

    if (pPhyInfo->isComboPhy[phyPortId])
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif  /* CONFIG_SDK_RTL8218FB */

#if !defined(__BOOTLOADER__)
#if (defined CONFIG_SDK_RTL8218B) || (defined CONFIG_SDK_RTL8218FB)
/* Function Name:
 *      phy_8218b_autoNegoAbility_get
 * Description:
 *      Get ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218b_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret;
    uint32  phyData4;
    uint32  phyData9;
    rtk_enable_t     enable;

    phy_common_autoNegoEnable_get(unit, port, &enable);

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return ret;

    pAbility->FC = (phyData4 & Pause_R4_MASK) >> Pause_R4_OFFSET;
    pAbility->AsyFC = (phyData4 & AsymmetricPause_R4_MASK) >> AsymmetricPause_R4_OFFSET;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
        return ret;

    pAbility->Full_100= (phyData4 & _100Base_TX_FD_R4_MASK) >> _100Base_TX_FD_R4_OFFSET;
    pAbility->Half_100= (phyData4 & _100Base_TX_R4_MASK) >> _100Base_TX_R4_OFFSET;
    pAbility->Full_10= (phyData4 & _10Base_T_FD_R4_MASK) >> _10Base_T_FD_R4_OFFSET;
    pAbility->Half_10= (phyData4 & _10Base_T_R4_MASK) >> _10Base_T_R4_OFFSET;
    pAbility->Half_1000 = (phyData9 & _1000Base_THalfDuplex_MASK) >> _1000Base_THalfDuplex_OFFSET;
    pAbility->Full_1000 = (phyData9 & _1000Base_TFullDuplex_MASK) >> _1000Base_TFullDuplex_OFFSET;

    return ret;
} /* end of phy_8218b_autoNegoAbility_get */

/* Function Name:
 *      phy_8218b_autoNegoAbility_set
 * Description:
 *      Set ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      pAbility  - auto negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218b_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret;
    uint32  phyData0;
    uint32  phyData4;
    uint32  phyData9;
    rtk_enable_t     enable;

    phy_common_autoNegoEnable_get(unit, port, &enable);

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return ret;

    phyData4 = phyData4 & ~(Pause_R4_MASK | AsymmetricPause_R4_MASK);
    phyData4 = phyData4
            | (pAbility->FC << Pause_R4_OFFSET)
            | (pAbility->AsyFC << AsymmetricPause_R4_OFFSET);

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
        return ret;

    phyData4 = phyData4 &
            ~(_100Base_TX_FD_R4_MASK | _100Base_TX_R4_MASK | _10Base_T_FD_R4_MASK | _10Base_T_R4_MASK);
    phyData4 = phyData4
            | (pAbility->Full_100 << _100Base_TX_FD_R4_OFFSET)
            | (pAbility->Half_100 << _100Base_TX_R4_OFFSET)
            | (pAbility->Full_10 << _10Base_T_FD_R4_OFFSET)
            | (pAbility->Half_10 << _10Base_T_R4_OFFSET);

    phyData9 = phyData9 & ~(_1000Base_TFullDuplex_MASK | _1000Base_THalfDuplex_MASK);
    phyData9 = phyData9 | (pAbility->Full_1000 << _1000Base_TFullDuplex_OFFSET)
               | (pAbility->Half_1000 << _1000Base_THalfDuplex_OFFSET);

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, phyData4)) != RT_ERR_OK)
        return ret;


    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, phyData9)) != RT_ERR_OK)
        return ret;

    /* Force re-autonegotiation if AN is on*/
    if (ENABLED == enable)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
            return ret;

        phyData0 = phyData0 & ~(RestartAutoNegotiation_MASK);
        phyData0 = phyData0 | (enable << RestartAutoNegotiation_OFFSET);

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
            return ret;
    }

    return ret;
} /* end of phy_8218b_autoNegoAbility_set */

/* Function Name:
 *      phy_8218b_speed_get
 * Description:
 *      Get link speed status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218b_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    int32   ret;
    uint32  val;
    uint32  phyData0;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;
    if (val & LinkStatus_MASK)
    {
        /* get value from CHIP*/
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
            return ret;

        *pSpeed = ((phyData0 & SpeedSelection1_MASK) >> (SpeedSelection1_OFFSET -1))
                  | ((phyData0 & SpeedSelection0_MASK) >> SpeedSelection0_OFFSET);
        return RT_ERR_OK;
    }

    /* Link-down */
    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    *pSpeed = ((phyData0 & SpeedSelection1_MASK) >> (SpeedSelection1_OFFSET -1))
              | ((phyData0 & SpeedSelection0_MASK) >> SpeedSelection0_OFFSET);

    return ret;
} /* end of phy_8218b_speed_get */

/* Function Name:
 *      phy_8218b_speed_set
 * Description:
 *      Set speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - copper media chip is not supported Force-1000
 * Note:
 *      None
 */
int32
phy_8218b_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    int32   ret;
    uint32  phyData0;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    phyData0 = phyData0 & ~(SpeedSelection1_MASK | SpeedSelection0_MASK);
    phyData0 = phyData0 | (((speed & 2) << (SpeedSelection1_OFFSET - 1)) | ((speed & 1) << SpeedSelection0_OFFSET));

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8218b_speed_set */
#endif  /* (defined CONFIG_SDK_RTL8218B) || (defined CONFIG_SDK_RTL8218FB) */

#ifdef CONFIG_SDK_RTL8218FB
/* Function Name:
 *      phy_8218fb_speed_set
 * Description:
 *      Set speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - copper media chip is not supported Force-1000
 * Note:
 *      None
 */
int32
phy_8218fb_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    int32   ret;
    uint32  val;
    uint32  phyData0;


    RT_PARAM_CHK((phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info) == FALSE) && (speed == PORT_SPEED_1000M), RT_ERR_CHIP_NOT_SUPPORTED);

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;
    if (val & LinkStatus_MASK)
    {
        /* get value from CHIP*/
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
            return ret;

        phyData0 = phyData0 & ~(SpeedSelection1_MASK | SpeedSelection0_MASK);
        phyData0 = phyData0 | (((speed & 2) << (SpeedSelection1_OFFSET - 1)) | ((speed & 1) << SpeedSelection0_OFFSET));

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
            return ret;

        return RT_ERR_OK;
    }

    /* Link-down */

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    phyData0 = phyData0 & ~(SpeedSelection1_MASK | SpeedSelection0_MASK);
    phyData0 = phyData0 | (((speed & 2) << (SpeedSelection1_OFFSET - 1)) | ((speed & 1) << SpeedSelection0_OFFSET));

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8218fb_speed_set */
#endif  /* CONFIG_SDK_RTL8218FB */
#if (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC)
/* Function Name:
 *      phy_8214fc_speed_set
 * Description:
 *      Set speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - copper media chip is not supported Force-1000
 * Note:
 *      None
 */
int32
phy_8214fc_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    int32   ret;
    uint32  val;
    uint32  phyData0;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;
    if (val & LinkStatus_MASK)
    {
        /* get value from CHIP*/
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
            return ret;

        phyData0 = phyData0 & ~(SpeedSelection1_MASK | SpeedSelection0_MASK);
        phyData0 = phyData0 | (((speed & 2) << (SpeedSelection1_OFFSET - 1)) | ((speed & 1) << SpeedSelection0_OFFSET));

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
            return ret;

        return RT_ERR_OK;
    }

    /* Link-down */

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    phyData0 = phyData0 & ~(SpeedSelection1_MASK | SpeedSelection0_MASK);
    phyData0 = phyData0 | (((speed & 2) << (SpeedSelection1_OFFSET - 1)) | ((speed & 1) << SpeedSelection0_OFFSET));

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8214fc_speed_set */
#endif  /* (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC) */

#if (defined CONFIG_SDK_RTL8218B) || (defined CONFIG_SDK_RTL8218FB)

/* Function Name:
 *      phy_8218b_rtctResult_get
 * Description:
 *      Get test result of RTCT.
 * Input:
 *      unit        - unit id
 *      port        - the port for retriving RTCT test result
 * Output:
 *      pRtctResult - RTCT result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_RTCT_NOT_FINISH   - RTCT not finish. Need to wait a while.
 *      RT_ERR_TIMEOUT      - RTCT test timeout in this port.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The result unit is cm
 */
int32
phy_8218b_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData, fixed_page;
    uint32  speed;
    rtk_port_media_t    media;
    uint32              link_sts;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if((PHY_MEDIA_LINKUP == link_sts) && (PORT_MEDIA_FIBER == media))
    {
        /* register is in copper page and can't access when fiber is linkup. */
        return RT_ERR_PHY_FIBER_LINKUP;
    }else{
        /* Check the port is link up or not? */
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData)) != RT_ERR_OK)
            return ret;

        phy_common_speed_get(unit, port, &speed);

        if ((phyData & LinkStatus_MASK) && speed != PORT_SPEED_10M)
        {
            /* If the port is link up,
                     * return cable length from green function
                     */

            fixed_page = 0xa88;

            /* The Length is store in [7:0], and the unit is meter*/
            if ((ret = hal_miim_read(unit, port, fixed_page, 16, &phyData)) != RT_ERR_OK)
                return ret;

            osal_memset(pRtctResult, 0, sizeof(rtk_rtctResult_t));
            if (speed == PORT_SPEED_1000M) /* 1000M */
            {
                pRtctResult->linkType = PORT_SPEED_1000M;
                pRtctResult->un.ge_result.channelALen = (phyData & 0x00FF)*100;
                pRtctResult->un.ge_result.channelBLen = (phyData & 0x00FF)*100;
                pRtctResult->un.ge_result.channelCLen = (phyData & 0x00FF)*100;
                pRtctResult->un.ge_result.channelDLen = (phyData & 0x00FF)*100;
            }
            else /* 100M */
            {
                pRtctResult->linkType = PORT_SPEED_100M;
                pRtctResult->un.fe_result.rxLen = (phyData & 0x00FF)*100;
                pRtctResult->un.fe_result.txLen = (phyData & 0x00FF)*100;
            }
        }
        else
        {
            /* If the port is link down,
                      * return cable length from RTCT function
                      */
            /* Page 0xa42, Register 17
                      * bit[15]: cable test finished or not?
                      *             1: Finished
                      *             0: Not finished
                      */
            fixed_page = 0xa42;

            if ((ret = hal_miim_read(unit, port, fixed_page, 17, &phyData)) != RT_ERR_OK)
                return ret;

            if(((phyData >> RTCT_DONE) & 0x1) != 0x1)
                return RT_ERR_PHY_RTCT_NOT_FINISH;

            pRtctResult->linkType = PORT_SPEED_1000M;
            /* Length = (Index/64)*8ns*(0.2m/ns) = Index/80 (m) = (1.25) * Index (cm) */

            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_LEN_ADDR_A)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= 0x3FFF; /*[13:0] are valid*/
            pRtctResult->un.ge_result.channelALen = (phyData)*5/4;

            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_LEN_ADDR_B)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= 0x3FFF; /*[13:0] are valid*/
            pRtctResult->un.ge_result.channelBLen = (phyData)*5/4;

            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_LEN_ADDR_C)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= 0x3FFF; /*[13:0] are valid*/
            pRtctResult->un.ge_result.channelCLen = (phyData)*5/4;

            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_LEN_ADDR_D)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= 0x3FFF; /*[13:0] are valid*/
            pRtctResult->un.ge_result.channelDLen = (phyData)*5/4;

            /* === Channel A Status ===*/
            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_STATUS_ADDR_A)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;

            if(((phyData >> RTCT_STATUS_SHORT) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelAShort = 1;
            if(((phyData >> RTCT_STATUS_OPEN) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelAOpen = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_SHORT) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelAMismatch = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_OPEN) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelAMismatch |= 0x2;
            if(((phyData >> RTCT_STATUS_LINE_DRIVER) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelALinedriver = 1;

            /* === Channel B Status ===*/
            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_STATUS_ADDR_B)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;

            if(((phyData >> RTCT_STATUS_SHORT) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelBShort = 1;
            if(((phyData >> RTCT_STATUS_OPEN) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelBOpen = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_SHORT) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelBMismatch = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_OPEN) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelBMismatch |= 0x2;
            if(((phyData >> RTCT_STATUS_LINE_DRIVER) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelBLinedriver = 1;

            /* === Channel C Status ===*/
            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_STATUS_ADDR_C)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;

            if(((phyData >> RTCT_STATUS_SHORT) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelCShort = 1;
            if(((phyData >> RTCT_STATUS_OPEN) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelCOpen = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_SHORT) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelCMismatch = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_OPEN) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelCMismatch |= 0x2;
            if(((phyData >> RTCT_STATUS_LINE_DRIVER) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelCLinedriver = 1;

            /* === Channel D Status ===*/
            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_STATUS_ADDR_D)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;

            if(((phyData >> RTCT_STATUS_SHORT) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelDShort = 1;
            if(((phyData >> RTCT_STATUS_OPEN) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelDOpen = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_SHORT) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelDMismatch = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_OPEN) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelDMismatch |= 0x2;
            if(((phyData >> RTCT_STATUS_LINE_DRIVER) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelDLinedriver = 1;
        }
    }
    return ret;
} /* end of phy_8218b_rtctResult_get */

/* Function Name:
 *      phy_8218b_rtct_start
 * Description:
 *      Start PHY interface RTCT test of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - chip not supported
 * Note:
 *      None
 */
int32
phy_8218b_rtct_start(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData, fixed_page;
    uint32  speed;
    rtk_port_media_t    media;
    uint32              link_sts;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if((PHY_MEDIA_LINKUP == link_sts) && (PORT_MEDIA_FIBER == media))
    {
        /* register is in copper page and can't access when fiber is linkup. */
        return RT_ERR_PHY_FIBER_LINKUP;
    }else{
        /* Check the port is link up or not? */
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData)) != RT_ERR_OK)
            return ret;

        phy_common_speed_get(unit, port, &speed);

        if ((phyData & LinkStatus_MASK) && speed != PORT_SPEED_10M)
        {
            /* Configure the GPHY page to copper */
            if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0001)) != RT_ERR_OK)
                return ret;

            fixed_page = 0;

            /* get value from CHIP*/
            if ((ret = hal_miim_write(unit, port, fixed_page, 27, 0x8011)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;

            /* enable green feature */
            phyData |= (1 << 9);
            if ((ret = hal_miim_write(unit, port, fixed_page, 27, 0x8011)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, fixed_page, 28, phyData)) != RT_ERR_OK)
                return ret;

            /* Configure the GPHY page to auto */
            if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0000)) != RT_ERR_OK)
                return ret;
        }
        else
        {
            if ((ret = hal_miim_read(unit, port, 0xa42, 17, &phyData)) != RT_ERR_OK)
                return ret;

            phyData |= (1 << RTCT_ENABLE);
            phyData |= (1 << RTCT_CH_A);
            phyData |= (1 << RTCT_CH_B);
            phyData |= (1 << RTCT_CH_C);
            phyData |= (1 << RTCT_CH_D);

            if ((ret = hal_miim_write(unit, port, 0xa42, 17, phyData)) != RT_ERR_OK)
                return ret;
        }
    }
    return ret;
} /* end of phy_8218b_rtct_start */

/* Function Name:
 *      phy_8218b_crossOverMode_get
 * Description:
 *      Get cross over mode in the specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
phy_8218b_crossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData, force_mode, mdi;
    rtk_port_media_t    media;
    uint32              link_sts;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if((PHY_MEDIA_LINKUP == link_sts) && (PORT_MEDIA_FIBER == media))
    {
        /* register is in copper page and can't access when fiber is linkup. */
        return RT_ERR_PHY_FIBER_LINKUP;
    }else{
        /* Configure the GPHY page to copper */
        if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0001)) != RT_ERR_OK)
            return ret;

        /* get value from CHIP*/
        if ((ret = hal_miim_read(unit, port, 0xa43, 24, &phyData)) != RT_ERR_OK)
            return ret;

        if((phyData & (1 << 9)) != 0)
        {
            force_mode = 1;
            if((phyData & (1 << 8)) != 0)
                mdi = 1;
            else
                mdi = 0;
        }else{
            force_mode = 0;
        }

        if (force_mode)
        {
            if (mdi)
                *pMode = PORT_CROSSOVER_MODE_MDI;
            else
                *pMode = PORT_CROSSOVER_MODE_MDIX;
        }
        else
            *pMode = PORT_CROSSOVER_MODE_AUTO;

        /* Configure the GPHY page to auto */
        if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0000)) != RT_ERR_OK)
            return ret;
    }

    return RT_ERR_OK;
} /* end of phy_8218b_crossOverMode_get */

/* Function Name:
 *      phy_8218b_crossOverMode_set
 * Description:
 *      Set cross over mode in the specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      mode - cross over mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
phy_8218b_crossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData;
    rtk_port_media_t    media;
    uint32              link_sts;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if((PHY_MEDIA_LINKUP == link_sts) && (PORT_MEDIA_FIBER == media))
    {
        /* register is in copper page and can't access when fiber is linkup. */
        return RT_ERR_PHY_FIBER_LINKUP;
    }else{
        /* Configure the GPHY page to copper */
        if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0001)) != RT_ERR_OK)
            return ret;

        /* get value from CHIP*/
        if ((ret = hal_miim_read(unit, port, 0xa43, 24, &phyData)) != RT_ERR_OK)
            return ret;

        switch (mode)
        {
            case PORT_CROSSOVER_MODE_AUTO:
                phyData &= ~(1 << 9);
                break;
            case PORT_CROSSOVER_MODE_MDI:
                phyData |= (1 << 9);
                phyData |= (1 << 8);
                break;
            case PORT_CROSSOVER_MODE_MDIX:
                phyData |= (1 << 9);
                phyData &= ~(1 << 8);
                break;
            default:
                return RT_ERR_INPUT;
        }

        if ((ret = hal_miim_write(unit, port, 0xa43, 24, phyData)) != RT_ERR_OK)
            return ret;

        /* Configure the GPHY page to auto */
        if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0000)) != RT_ERR_OK)
            return ret;
    }
    return RT_ERR_OK;
} /* end of phy_8218_crossOverMode_set */

/* Function Name:
 *      phy_8218b_crossOverStatus_get
 * Description:
 *      Get cross over status in the specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PHY_FIBER_LINKUP - This feature is not supported in this mode
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_STATUS_MDI
 *      - PORT_CROSSOVER_STATUS_MDIX
 */
int32
phy_8218b_crossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData;
    rtk_port_media_t    media;
    uint32              link_sts;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if((PHY_MEDIA_LINKUP == link_sts) && (PORT_MEDIA_FIBER == media))
    {
        /* register is in copper page and can't access when fiber is linkup. */
        return RT_ERR_PHY_FIBER_LINKUP;
    }else{
        /* Configure the GPHY page to copper */
        if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0001)) != RT_ERR_OK)
            return ret;

        /* get value from CHIP*/
        if ((ret = hal_miim_read(unit, port, 0xa43, 26, &phyData)) != RT_ERR_OK)
            return ret;

        if((phyData & (1 << 1)) != 0)
        {
            *pStatus = PORT_CROSSOVER_STATUS_MDI;
        }else{
            *pStatus = PORT_CROSSOVER_STATUS_MDIX;
        }

        /* Configure the GPHY page to auto */
        if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0000)) != RT_ERR_OK)
            return ret;
    }

    return RT_ERR_OK;
} /* end of phy_8218b_crossOverStatus_get */

/* Function Name:
 *      phy_8218b_linkDownPowerSavingEnable_get
 * Description:
 *      Get the status of link-down power saving of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of link-down power saving
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8218b is supported the per-port link-down power saving
 */
int32
phy_8218b_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  value;
    int32   ret = RT_ERR_FAILED;
    rtk_port_media_t    media;
    uint32              link_sts;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if((PHY_MEDIA_LINKUP == link_sts) && (PORT_MEDIA_FIBER == media))
    {
        /* register is in copper page and can't access when fiber is linkup. */
        return RT_ERR_PHY_FIBER_LINKUP;
    }else{
        /* For Link-Down Power Saving (per-port) */
        if ((ret = hal_miim_read(unit, port, 0xa43, 24, &value)) != RT_ERR_OK)
            return ret;

        if (((value >> 2) & 0x1) == 0x1)
            *pEnable = ENABLED;
        else
            *pEnable = DISABLED;
    }
    return RT_ERR_OK;
} /* end of phy_8218b_linkDownPowerSavingEnable_get */

/* Function Name:
 *      phy_8218b_linkDownPowerSavingEnable_set
 * Description:
 *      Set the status of link-down power saving of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of link-down power saving
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8218b is supported the per-port link-down power saving
 */
int32
phy_8218b_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  value;
    int32   ret = RT_ERR_FAILED;
    rtk_port_media_t    media;
    uint32              link_sts;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if((PHY_MEDIA_LINKUP == link_sts) && (PORT_MEDIA_FIBER == media))
    {
        /* register is in copper page and can't access when fiber is linkup. */
        return RT_ERR_PHY_FIBER_LINKUP;
    }else{
        /* For Link-Down Power Saving (per-port) */
        if ((ret = hal_miim_read(unit, port, 0xa43, 24, &value)) != RT_ERR_OK)
            return ret;

        value &= ~(0x1 << 2);
        if (ENABLED == enable)
        {
            value |= (0x1 << 2);
        }
        if ((ret = hal_miim_write(unit, port, 0xa43, 24, value)) != RT_ERR_OK)
            return ret;
    }
    return RT_ERR_OK;
} /* end of phy_8218b_linkDownPowerSavingEnable_set */

/* Function Name:
 *      phy_8218b_gigaLiteEnable_get
 * Description:
 *      Get the status of Giga Lite of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of Giga Lite
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8218b is supported the per-port Giga Lite feature.
 */
int32
phy_8218b_gigaLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  value;
    int32   ret = RT_ERR_FAILED;
    rtk_port_media_t    media;
    uint32              link_sts;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if((PHY_MEDIA_LINKUP == link_sts) && (PORT_MEDIA_FIBER == media))
    {
        /* register is in copper page and can't access when fiber is linkup. */
        return RT_ERR_PHY_FIBER_LINKUP;
    }else{
        /* Configure the GPHY page to copper */
       if ((ret = hal_miim_write(unit, port, GIGA_LITE_CTRL_REG, 29, 0x0001)) != RT_ERR_OK)
            return ret;

        /* For Giga Lite (per-port) */
       if ((ret = hal_miim_read(unit, port, GIGA_LITE_CTRL_REG, 20, &value)) != RT_ERR_OK)
           return ret;

       if (((value >> 9) & 0x1) == 0x1)
           *pEnable = ENABLED;
       else
           *pEnable = DISABLED;

        /* Configure the GPHY page to auto */
       if ((ret = hal_miim_write(unit, port, GIGA_LITE_CTRL_REG, 29, 0x0000)) != RT_ERR_OK)
           return ret;
    }
    return RT_ERR_OK;
} /* end of phy_8218b_gigaLiteEnable_get */

/* Function Name:
 *      phy_8218b_gigaLiteEnable_set
 * Description:
 *      Set the status of Giga Lite of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of Giga Lite
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8218b is supported the per-port Giga Lite feature.
 */
int32
phy_8218b_gigaLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  value;
    int32   ret = RT_ERR_FAILED;
    rtk_port_media_t    media;
    uint32              link_sts;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if((PHY_MEDIA_LINKUP == link_sts) && (PORT_MEDIA_FIBER == media))
    {
        /* register is in copper page and can't access when fiber is linkup. */
        return RT_ERR_PHY_FIBER_LINKUP;
    }else{
        /* Configure the GPHY page to copper */
        if ((ret = hal_miim_write(unit, port, GIGA_LITE_CTRL_REG, 29, 0x0001)) != RT_ERR_OK)
            return ret;

        /* bit[2]: configure 2-pair auto-downspeed */
        /* bit[9]: configure retry speed down to 500M */
        if ((ret = hal_miim_read(unit, port, 0xa44, 17, &value)) != RT_ERR_OK)
            return ret;

        value &= ~(0x1 << 9);
        value &= ~(0x1 << 2);
        if (ENABLED == enable)
        {
            value |= (0x1 << 9);
            value |= (0x1 << 2);
        }
        if ((ret = hal_miim_write(unit, port, 0xa44, 17, value)) != RT_ERR_OK)
            return ret;


        /* For Giga Lite (per-port) */
        if ((ret = hal_miim_read(unit, port, GIGA_LITE_CTRL_REG, 20, &value)) != RT_ERR_OK)
            return ret;

        value &= ~(0x1 << 9);
        if (ENABLED == enable)
        {
            value |= (0x1 << 9);
        }

        if ((ret = hal_miim_write(unit, port, GIGA_LITE_CTRL_REG, 20, value)) != RT_ERR_OK)
            return ret;

        /* Do the restart N-WAY */
        if ((ret = hal_miim_read(unit, port, 0, 0, &value)) != RT_ERR_OK)
            return ret;
        value |= (0x1UL<<9);
        if ((ret = hal_miim_write(unit, port, 0, 0, value)) != RT_ERR_OK)
            return ret;


        /* Configure the GPHY page to auto */
        if ((ret = hal_miim_write(unit, port, GIGA_LITE_CTRL_REG, 29, 0x0000)) != RT_ERR_OK)
            return ret;
    }


    return RT_ERR_OK;
} /* end of phy_8218b_gigaLiteEnable_set */

#endif  /* (defined CONFIG_SDK_RTL8218B) || (defined CONFIG_SDK_RTL8218FB) */


#if (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC)
/* Function Name:
 *      phy_8218fb_media_get
 * Description:
 *      Get PHY 8218FB media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      (1) media type is as following:
 *          - PORT_MEDIA_COPPER
 *          - PORT_MEDIA_FIBER
 */
int32
phy_8218fb_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    uint32  base_port = 0, reg = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  phyPortId; /* PHY port ID */

    RT_PARAM_CHK((NULL == pMedia), RT_ERR_NULL_POINTER);
    if ((base_port = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

    if ((phyPortId = HWP_PHY_PORTID(unit, port)) >= MAX_PHY_NUM_OF_CHIP)
    {
        return RT_ERR_PORT_ID;
    }

    switch (phyPortId)
    {
        case 4:
            reg = 16;
            break;
        case 5:
            reg = 19;
            break;
        case 6:
            reg = 20;
            break;
        case 7:
            reg = 21;
            break;
        default:
            *pMedia = PORT_MEDIA_COPPER;
            return RT_ERR_OK;
    }


    if ((ret = _phy_8214fc_combo_media_get(unit, base_port, reg, pMedia)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8218fb_media_get */

/* Function Name:
 *      phy_8218fb_media_set
 * Description:
 *      Get PHY 8218FB media type.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - phy media type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      (1) media type is as following:
 *          - PORT_MEDIA_COPPER
 *          - PORT_MEDIA_FIBER
 */
int32
phy_8218fb_media_set(uint32 unit, rtk_port_t port, rtk_port_media_t media)
{
    uint32  base_port = 0, reg = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  phyPortId; /* PHY port ID */

    if ((base_port = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

    if ((phyPortId = HWP_PHY_PORTID(unit, port)) >= MAX_PHY_NUM_OF_CHIP)
    {
        return RT_ERR_PORT_ID;
    }

    switch (phyPortId)
    {
        case 4:
            reg = 16;
            break;
        case 5:
            reg = 19;
            break;
        case 6:
            reg = 20;
            break;
        case 7:
            reg = 21;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = _phy_8214fc_combo_media_set(unit, base_port, port, reg, media)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8218fb_media_set */
#endif  /* (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC) */

#if (defined CONFIG_SDK_RTL8218B) || (defined CONFIG_SDK_RTL8218FB)
/* Function Name:
 *      phy_8218b_broadcastEnable_set
 * Description:
 *      Set enable status of broadcast mode
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - broadcast enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218b_broadcastEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  base_port = 0;
    uint32  phyData;


    if ((base_port = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

    /* get value from CHIP*/
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 29, 0x0008)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 31, 0x0266)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_read(unit, base_port, 0xa42, 22, &phyData)) != RT_ERR_OK)
        return ret;
    if (enable)
        phyData |= (0x1 << (port-base_port+8));
    else
        phyData &= ~(0x1 << (port-base_port+8));
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 22, phyData)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 29, 0x0000)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8218b_broadcastEnable_set */

/* Function Name:
 *      phy_8218b_broadcastID_set
 * Description:
 *      Set broadcast ID
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      broadcastID   - broadcast ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218b_broadcastID_set(uint32 unit, rtk_port_t port, uint32 broadcastID)
{
    int32   ret;
    uint32  base_port = 0;
    uint32  phyData;

    if ((base_port = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

    /* get value from CHIP*/
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 29, 0x0008)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 31, 0x0266)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_read(unit, base_port, 0xa42, 22, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= ~(0x1f);
    phyData |= (broadcastID & 0x1f);
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 22, phyData)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 29, 0x0000)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8218b_broadcastID_set */
#endif  /* (defined CONFIG_SDK_RTL8218B) || (defined CONFIG_SDK_RTL8218FB) */

#ifdef CONFIG_SDK_RTL8214FC
/* Function Name:
 *      phy_8214fc_broadcastEnable_set
 * Description:
 *      Set enable status of broadcast mode
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - broadcast enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214fc_broadcastEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  base_port = 0;
    uint32  phyData;


    if ((base_port = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

    /* get value from CHIP*/
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 29, 0x0008)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 31, 0x0266)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_read(unit, base_port, 0xa42, 22, &phyData)) != RT_ERR_OK)
        return ret;
    if (enable)
        phyData |= (0x1 << (port-base_port+8));
    else
        phyData &= ~(0x1 << (port-base_port+8));
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 22, phyData)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 29, 0x0000)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8214fc_broadcastEnable_set */

/* Function Name:
 *      phy_8214fc_broadcastID_set
 * Description:
 *      Set broadcast ID
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      broadcastID   - broadcast ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214fc_broadcastID_set(uint32 unit, rtk_port_t port, uint32 broadcastID)
{
    int32   ret;
    uint32  base_port = 0;
    uint32  phyData;

    if ((base_port = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

    /* get value from CHIP*/
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 29, 0x0008)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 31, 0x0266)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_read(unit, base_port, 0xa42, 22, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= ~(0x1f);
    phyData |= (broadcastID & 0x1f);
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 22, phyData)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, base_port, 0xa42, 29, 0x0000)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8214fc_broadcastID_set */
#endif  /* CONFIG_SDK_RTL8214FC */
#if (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC)
/* Function Name:
 *      phy_8218fb_autoNegoAbility_get
 * Description:
 *      Get ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218fb_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret;
    uint32  val, phyData4, phyData9;
    rtk_port_media_t media;
    hal_control_t   *pHalCtrl;
    uint32  phyExtStatus15, is_fiber_linkup = 0, is_copper_linkup = 0;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;

    if (val & LinkStatus_MASK)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 15, &phyExtStatus15)) != RT_ERR_OK)
            return ret;
        is_fiber_linkup = (phyExtStatus15 >> 15) & 0x1;
        is_copper_linkup = (phyExtStatus15 >> 13) & 0x1;

        /* get value from CHIP*/
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
            return ret;

        if (is_fiber_linkup)
        {
            /* 1000Base-X Linkup */
            pAbility->FC = (phyData4 & _1000BaseX_Pause_R4_MASK) >> _1000BaseX_Pause_R4_OFFSET;
            pAbility->AsyFC = (phyData4 & _1000BaseX_AsymmetricPause_R4_MASK) >> _1000BaseX_AsymmetricPause_R4_OFFSET;
            pAbility->Half_10 = 0;
            pAbility->Full_10 = 0;
            pAbility->Half_100 = 0;
            pAbility->Full_100 = 0;
            pAbility->Half_1000 = (phyData4 & _1000BaseX_HalfDuplex_R4_MASK) >> _1000BaseX_HalfDuplex_R4_OFFSET;
            pAbility->Full_1000 = (phyData4 & _1000BaseX_FullDuplex_R4_MASK) >> _1000BaseX_FullDuplex_R4_OFFSET;
        }
        else if (is_copper_linkup)
        {
            pAbility->FC = (phyData4 & Pause_R4_MASK) >> Pause_R4_OFFSET;
            pAbility->AsyFC = (phyData4 & AsymmetricPause_R4_MASK) >> AsymmetricPause_R4_OFFSET;

            if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
                return ret;

            pAbility->Full_100= (phyData4 & _100Base_TX_FD_R4_MASK) >> _100Base_TX_FD_R4_OFFSET;
            pAbility->Half_100= (phyData4 & _100Base_TX_R4_MASK) >> _100Base_TX_R4_OFFSET;
            pAbility->Full_10= (phyData4 & _10Base_T_FD_R4_MASK) >> _10Base_T_FD_R4_OFFSET;
            pAbility->Half_10= (phyData4 & _10Base_T_R4_MASK) >> _10Base_T_R4_OFFSET;
            pAbility->Half_1000 = (phyData9 & _1000Base_THalfDuplex_MASK) >> _1000Base_THalfDuplex_OFFSET;
            pAbility->Full_1000 = (phyData9 & _1000Base_TFullDuplex_MASK) >> _1000Base_TFullDuplex_OFFSET;
        }
        return RT_ERR_OK;
    }

    /* Link-down */
    if ((ret = phy_8218fb_media_get(unit, port, &media)) != RT_ERR_OK)
        return ret;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return ret;

    if (PORT_MEDIA_FIBER == media)
    {
        pAbility->FC = (phyData4 & _1000BaseX_Pause_R4_MASK) >> _1000BaseX_Pause_R4_OFFSET;
        pAbility->AsyFC = (phyData4 & _1000BaseX_AsymmetricPause_R4_MASK) >> _1000BaseX_AsymmetricPause_R4_OFFSET;
        pAbility->Half_10 = 0;
        pAbility->Full_10 = 0;
        pAbility->Half_100 = 0;
        pAbility->Full_100 = 0;
        pAbility->Half_1000 = (phyData4 & _1000BaseX_HalfDuplex_R4_MASK) >> _1000BaseX_HalfDuplex_R4_OFFSET;
        pAbility->Full_1000 = (phyData4 & _1000BaseX_FullDuplex_R4_MASK) >> _1000BaseX_FullDuplex_R4_OFFSET;
    }
    else
    {
        pAbility->FC = (phyData4 & Pause_R4_MASK) >> Pause_R4_OFFSET;
        pAbility->AsyFC = (phyData4 & AsymmetricPause_R4_MASK) >> AsymmetricPause_R4_OFFSET;

        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
            return ret;

        pAbility->Full_100= (phyData4 & _100Base_TX_FD_R4_MASK) >> _100Base_TX_FD_R4_OFFSET;
        pAbility->Half_100= (phyData4 & _100Base_TX_R4_MASK) >> _100Base_TX_R4_OFFSET;
        pAbility->Full_10= (phyData4 & _10Base_T_FD_R4_MASK) >> _10Base_T_FD_R4_OFFSET;
        pAbility->Half_10= (phyData4 & _10Base_T_R4_MASK) >> _10Base_T_R4_OFFSET;
        pAbility->Half_1000 = (phyData9 & _1000Base_THalfDuplex_MASK) >> _1000Base_THalfDuplex_OFFSET;
        pAbility->Full_1000 = (phyData9 & _1000Base_TFullDuplex_MASK) >> _1000Base_TFullDuplex_OFFSET;
    }

    return RT_ERR_OK;
} /* end of phy_8218fb_autoNegoAbility_get */

/* Function Name:
 *      phy_8218fb_autoNegoAbility_set
 * Description:
 *      Set ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      pAbility  - auto negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218fb_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret;
    uint32  phyData0;
    uint32  phyData4;
    uint32  phyData9;
    uint32  val;
    rtk_enable_t     enable;
    rtk_port_media_t media;
    hal_control_t   *pHalCtrl;
    uint32  phyExtStatus15, is_fiber_linkup = 0, is_copper_linkup = 0;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;

    if (val & LinkStatus_MASK)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 15, &phyExtStatus15)) != RT_ERR_OK)
            return ret;
        is_fiber_linkup = (phyExtStatus15 >> 15) & 0x1;
        is_copper_linkup = (phyExtStatus15 >> 13) & 0x1;

        phy_common_autoNegoEnable_get(unit, port, &enable);

        /* get value to CHIP*/
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
            return ret;

        if (is_fiber_linkup)
        {
            /* 1000Base-X Linkup */
            phyData4 = phyData4 & ~(_1000BaseX_Pause_R4_MASK | _1000BaseX_AsymmetricPause_R4_MASK);
            phyData4 = phyData4
                    | (pAbility->FC << _1000BaseX_Pause_R4_OFFSET)
                    | (pAbility->AsyFC << _1000BaseX_AsymmetricPause_R4_OFFSET);
            phyData4 = phyData4 & ~(_1000BaseX_HalfDuplex_R4_MASK | _1000BaseX_FullDuplex_R4_MASK);
            phyData4 = phyData4 | (pAbility->Half_1000 << _1000BaseX_HalfDuplex_R4_OFFSET)
                    | (pAbility->Full_1000 << _1000BaseX_FullDuplex_R4_OFFSET);

            if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, phyData4)) != RT_ERR_OK)
                return ret;
        }
        else if (is_copper_linkup)
        {
            phyData4 = phyData4 & ~(Pause_R4_MASK | AsymmetricPause_R4_MASK);
            phyData4 = phyData4
                    | (pAbility->FC << Pause_R4_OFFSET)
                    | (pAbility->AsyFC << AsymmetricPause_R4_OFFSET);

            if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
                return ret;

            phyData4 = phyData4 &
                    ~(_100Base_TX_FD_R4_MASK | _100Base_TX_R4_MASK | _10Base_T_FD_R4_MASK | _10Base_T_R4_MASK);
            phyData4 = phyData4
                    | (pAbility->Full_100 << _100Base_TX_FD_R4_OFFSET)
                    | (pAbility->Half_100 << _100Base_TX_R4_OFFSET)
                    | (pAbility->Full_10 << _10Base_T_FD_R4_OFFSET)
                    | (pAbility->Half_10 << _10Base_T_R4_OFFSET);

            phyData9 = phyData9 & ~(_1000Base_TFullDuplex_MASK | _1000Base_THalfDuplex_MASK);
            phyData9 = phyData9 | (pAbility->Full_1000 << _1000Base_TFullDuplex_OFFSET)
                       | (pAbility->Half_1000 << _1000Base_THalfDuplex_OFFSET);

            if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, phyData4)) != RT_ERR_OK)
                return ret;

            if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, phyData9)) != RT_ERR_OK)
                return ret;
        }

        /* Force re-autonegotiation if AN is on */
        if (ENABLED == enable)
        {
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
                return ret;

            phyData0 = phyData0 & ~(RestartAutoNegotiation_MASK);
            phyData0 = phyData0 | (enable << RestartAutoNegotiation_OFFSET);

            if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
                return ret;
        }
        return RT_ERR_OK;
    }

    /* Link-down */
    if ((ret = phy_8218fb_media_get(unit, port, &media)) != RT_ERR_OK)
        return ret;

    phy_common_autoNegoEnable_get(unit, port, &enable);

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return ret;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return ret;

    if (PORT_MEDIA_FIBER == media)
    {
        phyData4 = phyData4 & ~(_1000BaseX_Pause_R4_MASK | _1000BaseX_AsymmetricPause_R4_MASK);
        phyData4 = phyData4
                | (pAbility->FC << _1000BaseX_Pause_R4_OFFSET)
                | (pAbility->AsyFC << _1000BaseX_AsymmetricPause_R4_OFFSET);
        phyData4 = phyData4 & ~(_1000BaseX_HalfDuplex_R4_MASK | _1000BaseX_FullDuplex_R4_MASK);
        phyData4 = phyData4 | (pAbility->Half_1000 << _1000BaseX_HalfDuplex_R4_OFFSET)
                | (pAbility->Full_1000 << _1000BaseX_FullDuplex_R4_OFFSET);

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, phyData4)) != RT_ERR_OK)
            return ret;

    }
    else if (PORT_MEDIA_COPPER == media)
    {
        phyData4 = phyData4 & ~(Pause_R4_MASK | AsymmetricPause_R4_MASK);
        phyData4 = phyData4
                | (pAbility->FC << Pause_R4_OFFSET)
                | (pAbility->AsyFC << AsymmetricPause_R4_OFFSET);

        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
            return ret;

        phyData4 = phyData4 &
                ~(_100Base_TX_FD_R4_MASK | _100Base_TX_R4_MASK | _10Base_T_FD_R4_MASK | _10Base_T_R4_MASK);
        phyData4 = phyData4
                | (pAbility->Full_100 << _100Base_TX_FD_R4_OFFSET)
                | (pAbility->Half_100 << _100Base_TX_R4_OFFSET)
                | (pAbility->Full_10 << _10Base_T_FD_R4_OFFSET)
                | (pAbility->Half_10 << _10Base_T_R4_OFFSET);

        phyData9 = phyData9 & ~(_1000Base_TFullDuplex_MASK | _1000Base_THalfDuplex_MASK);
        phyData9 = phyData9 | (pAbility->Full_1000 << _1000Base_TFullDuplex_OFFSET)
                   | (pAbility->Half_1000 << _1000Base_THalfDuplex_OFFSET);

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, phyData4)) != RT_ERR_OK)
            return ret;


        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, phyData9)) != RT_ERR_OK)
            return ret;
    }

    /* Force re-autonegotiation if AN is on*/
    if (ENABLED == enable)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
            return ret;

        phyData0 = phyData0 & ~(RestartAutoNegotiation_MASK);
        phyData0 = phyData0 | (enable << RestartAutoNegotiation_OFFSET);

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
            return ret;
    }

    return ret;
} /* end of phy_8218fb_autoNegoAbility_set */
#endif  /* (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC) */

#if (defined CONFIG_SDK_RTL8218B) || (defined CONFIG_SDK_RTL8218FB)
/* Function Name:
 *      phy_8218b_eeepEnable_get
 * Description:
 *      Get enable status of EEEP function in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of EEEP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8218b_eeepEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  phyData;
    rtk_port_media_t    media;
    uint32              link_sts;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if((PHY_MEDIA_LINKUP == link_sts) && (PORT_MEDIA_FIBER == media))
    {
        /* register is in copper page and can't access when fiber is linkup. */
        return RT_ERR_PHY_FIBER_LINKUP;
    }else{
        /* Configure the GPHY page to copper */
        if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0001)) != RT_ERR_OK)
            return ret;

        /* get value from CHIP*/
        if ((ret = hal_miim_read(unit, port, 0xa43, 17, &phyData)) != RT_ERR_OK)
            return ret;

        if (((phyData >> 9) & 0x1) == 0x1)
            *pEnable = ENABLED;
        else
            *pEnable = DISABLED;

        /* Configure the GPHY page to auto */
        if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0000)) != RT_ERR_OK)
            return ret;
    }
    return RT_ERR_OK;
} /* end of phy_8218b_eeepEnable_get */

/* Function Name:
 *      phy_8218_eeepEnable_set
 * Description:
 *      Set enable status of EEEP function in the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of EEEP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8218b_eeepEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  phyData;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t     an_enable;
    rtk_port_media_t    media;
    uint32              link_sts;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if((PHY_MEDIA_LINKUP == link_sts) && (PORT_MEDIA_FIBER == media))
    {
        /* register is in copper page and can't access when fiber is linkup. */
        return RT_ERR_PHY_FIBER_LINKUP;
    }else{

        /* Configure the GPHY page to copper */
        if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0001)) != RT_ERR_OK)
            return ret;

        phy_common_autoNegoEnable_get(unit, port, &an_enable);

        /* get value from CHIP*/
        if ((ret = hal_miim_read(unit, port, 0xa43, 17, &phyData)) != RT_ERR_OK)
                return ret;

        if (ENABLED == enable)
        {
            phyData |= (1 << 9);  /* enable eeep rx */
            phyData |= (1 << 10); /* enable eeep slave rx */
            if ((ret = hal_miim_write(unit, port, 0xa43, 17, phyData)) != RT_ERR_OK)
                return ret;
        }
        else
        {
            phyData &= ~(1 << 9);  /* disable eeep rx */
            phyData &= ~(1 << 10); /* disable eeep slave rx */
            if ((ret = hal_miim_write(unit, port, 0xa43, 17, phyData)) != RT_ERR_OK)
                return ret;
        }

        /* Force re-autonegotiation if AN is on*/
        if (ENABLED == an_enable)
        {
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData)) != RT_ERR_OK)
                return ret;

            phyData = phyData & ~(RestartAutoNegotiation_MASK);
            phyData = phyData | (an_enable << RestartAutoNegotiation_OFFSET);

            if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
                return ret;
        }

        /* Configure the GPHY page to auto */
        if ((ret = hal_miim_write(unit, port, 0xa42, 29, 0x0000)) != RT_ERR_OK)
            return ret;
    }
    return RT_ERR_OK;
} /* end of phy_8218b_eeepEnable_set */
#endif  /* (defined CONFIG_SDK_RTL8218B) || (defined CONFIG_SDK_RTL8218FB) */

#ifdef CONFIG_SDK_RTL8218FB
/* Function Name:
 *      phy_8218fb_fiber_media_get
 * Description:
 *      Get PHY 8218FB fiber media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy fiber media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. fiber media type value is as following:
 *      - PORT_FIBER_MEDIA_1000
 *      - PORT_FIBER_MEDIA_AUTO
 */
int32
phy_8218fb_fiber_media_get(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t *pMedia)
{
    RT_PARAM_CHK((NULL == pMedia), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((port % PORT_NUM_IN_8218FB) < 4, RT_ERR_CHIP_NOT_SUPPORTED);

    return phy_8214fc_fiber_media_get(unit, port, pMedia);
} /* end of phy_8218fb_fiber_media_get */

/* Function Name:
 *      phy_8218fb_fiber_media_set
 * Description:
 *      Set PHY 8218FB fiber media type.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - phy media fiber type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      1. fiber media type value is as following:
 *      - PORT_FIBER_MEDIA_1000
 *      - PORT_FIBER_MEDIA_AUTO
 *      - PORT_FIBER_MEDIA_100
 */
int32
phy_8218fb_fiber_media_set(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t media)
{
    RT_PARAM_CHK((port % PORT_NUM_IN_8218FB) < 4, RT_ERR_CHIP_NOT_SUPPORTED);

    return phy_8214fc_fiber_media_set(unit, port, media);
} /* end of phy_8218fb_fiber_media_set */
#endif  /* CONFIG_SDK_RTL8218FB */

#if (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC)
/* Function Name:
 *      phy_8214fc_autoNegoEnable_get
 * Description:
 *      Get 8214FC autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214fc_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtk_port_media_t    media;
    uint32              link_sts;
    uint32              phyData0, page, reg;
    int32               ret;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    reg = PHY_INT_REG_BASE + PHY_CONTROL_REG;

    if (PORT_MEDIA_FIBER == media)
    {
        page = PHY_PAGE_0;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0003)) != RT_ERR_OK)
            goto ERR;
    }
    else
    {
        page = 0xa40;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0001)) != RT_ERR_OK)
            goto ERR;
    }

    if ((ret = hal_miim_read(unit, port, page, reg, &phyData0)) != RT_ERR_OK)
        goto ERR;

    if (phyData0 & AutoNegotiationEnable_MASK)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

ERR:
    hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0000);
    hal_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, 0x0000);
    return ret;
} /* end of phy_8214fc_autoNegoEnable_get */

/* Function Name:
 *      phy_8214fc_autoNegoEnable_set
 * Description:
 *      Set 8214FC autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214fc_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtk_port_media_t    media;
    uint32              link_sts;
    uint32              phyData0, page, reg;
    int32               ret;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    reg = PHY_INT_REG_BASE + PHY_CONTROL_REG;

    if (PORT_MEDIA_FIBER == media)
    {
        page = PHY_PAGE_0;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0003)) != RT_ERR_OK)
            goto ERR;
    }
    else
    {
        page = 0xa40;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0001)) != RT_ERR_OK)
            goto ERR;
    }

    if ((ret = hal_miim_read(unit, port, page, reg, &phyData0)) != RT_ERR_OK)
        goto ERR;

    phyData0 = phyData0 & ~(AutoNegotiationEnable_MASK | RestartAutoNegotiation_MASK);
    phyData0 = phyData0 | ((enable << AutoNegotiationEnable_OFFSET) | (1 << RestartAutoNegotiation_OFFSET));

    if ((ret = hal_miim_write(unit, port, page, reg, phyData0)) != RT_ERR_OK)
        goto ERR;

ERR:
    hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0000);
    hal_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, 0x0000);
    return ret;
} /* end of phy_8214fc_autoNegoEnable_set */

/* Function Name:
 *      phy_8214fc_autoNegoAbility_get
 * Description:
 *      Get 8214FC ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214fc_autoNegoAbility_get(uint32 unit, rtk_port_t port,
    rtk_port_phy_ability_t *pAbility)
{
    rtk_port_media_t    media;
    uint32              link_sts;
    uint32              phyData4, phyData9;
    uint32              page, reg;
    int32               ret;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if (PORT_MEDIA_FIBER == media)
    {
        page = PHY_PAGE_0;
        reg = PHY_INT_REG_BASE + PHY_AN_ADVERTISEMENT_REG;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0003)) != RT_ERR_OK)
            goto ERR;
        if ((ret = hal_miim_read(unit, port, page, reg, &phyData4)) != RT_ERR_OK)
            goto ERR;

        /* 1000Base-X Linkup */
        pAbility->FC = (phyData4 & _1000BaseX_Pause_R4_MASK) >> _1000BaseX_Pause_R4_OFFSET;
        pAbility->AsyFC = (phyData4 & _1000BaseX_AsymmetricPause_R4_MASK) >> _1000BaseX_AsymmetricPause_R4_OFFSET;
        pAbility->Half_10 = 0;
        pAbility->Full_10 = 0;
        pAbility->Half_100 = 0;
        pAbility->Full_100 = 0;
        pAbility->Half_1000 = (phyData4 & _1000BaseX_HalfDuplex_R4_MASK) >> _1000BaseX_HalfDuplex_R4_OFFSET;
        pAbility->Full_1000 = (phyData4 & _1000BaseX_FullDuplex_R4_MASK) >> _1000BaseX_FullDuplex_R4_OFFSET;
    }
    else
    {
        page = 0xa40;
        reg = PHY_INT_REG_BASE + PHY_AN_ADVERTISEMENT_REG;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0001)) != RT_ERR_OK)
            goto ERR;
        if ((ret = hal_miim_read(unit, port, page, reg, &phyData4)) != RT_ERR_OK)
            goto ERR;

        page = 0xa41;
        reg = PHY_INT_REG_BASE + (PHY_1000_BASET_CONTROL_REG % PHY_INT_REG_NEXT);
        if ((ret = hal_miim_read(unit, port, page, reg, &phyData9)) != RT_ERR_OK)
            goto ERR;

        pAbility->FC = (phyData4 & Pause_R4_MASK) >> Pause_R4_OFFSET;
        pAbility->AsyFC = (phyData4 & AsymmetricPause_R4_MASK) >> AsymmetricPause_R4_OFFSET;
        pAbility->Full_100= (phyData4 & _100Base_TX_FD_R4_MASK) >> _100Base_TX_FD_R4_OFFSET;
        pAbility->Half_100= (phyData4 & _100Base_TX_R4_MASK) >> _100Base_TX_R4_OFFSET;
        pAbility->Full_10= (phyData4 & _10Base_T_FD_R4_MASK) >> _10Base_T_FD_R4_OFFSET;
        pAbility->Half_10= (phyData4 & _10Base_T_R4_MASK) >> _10Base_T_R4_OFFSET;
        pAbility->Half_1000 = (phyData9 & _1000Base_THalfDuplex_MASK) >> _1000Base_THalfDuplex_OFFSET;
        pAbility->Full_1000 = (phyData9 & _1000Base_TFullDuplex_MASK) >> _1000Base_TFullDuplex_OFFSET;
    }

ERR:
    hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0000);
    hal_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, 0x0000);
    return ret;
} /* end of phy_8214fc_autoNegoAbility_get */

/* Function Name:
 *      phy_8214fc_autoNegoAbility_set
 * Description:
 *      Set 8214FC ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      pAbility  - auto negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214fc_autoNegoAbility_set(uint32 unit, rtk_port_t port,
    rtk_port_phy_ability_t *pAbility)
{
    rtk_port_media_t    media;
    rtk_enable_t        enable;
    uint32              link_sts;
    uint32              phyData0, phyData4, phyData9;
    int32               ret, page, reg;

    if ((ret = phy_common_autoNegoEnable_get(unit, port, &enable)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if (PORT_MEDIA_FIBER == media)
    {
        page = PHY_PAGE_0;
        reg = PHY_INT_REG_BASE + PHY_AN_ADVERTISEMENT_REG;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0003)) != RT_ERR_OK)
            goto ERR;
        if ((ret = hal_miim_read(unit, port, page, reg, &phyData4)) != RT_ERR_OK)
            goto ERR;

        phyData4 = phyData4 & ~(_1000BaseX_Pause_R4_MASK | _1000BaseX_AsymmetricPause_R4_MASK);
        phyData4 = phyData4 | (pAbility->FC << _1000BaseX_Pause_R4_OFFSET)
                | (pAbility->AsyFC << _1000BaseX_AsymmetricPause_R4_OFFSET);
        phyData4 = phyData4 & ~(_1000BaseX_HalfDuplex_R4_MASK | _1000BaseX_FullDuplex_R4_MASK);
        phyData4 = phyData4 | (pAbility->Half_1000 << _1000BaseX_HalfDuplex_R4_OFFSET)
                | (pAbility->Full_1000 << _1000BaseX_FullDuplex_R4_OFFSET);

        if ((ret = hal_miim_write(unit, port, page, reg, phyData4)) != RT_ERR_OK)
            goto ERR;

        /* Force re-autonegotiation if AN is on */
        if (ENABLED == enable)
        {
            reg = PHY_INT_REG_BASE + PHY_CONTROL_REG;

            if ((ret = hal_miim_read(unit, port, page, reg, &phyData0)) != RT_ERR_OK)
                goto ERR;

            phyData0 = phyData0 & ~(RestartAutoNegotiation_MASK);
            phyData0 = phyData0 | (enable << RestartAutoNegotiation_OFFSET);

            if ((ret = hal_miim_write(unit, port, page, reg, phyData0)) != RT_ERR_OK)
                goto ERR;
        }
    }
    else
    {
        page = 0xa40;
        /* register 4 */
        reg = PHY_INT_REG_BASE + PHY_AN_ADVERTISEMENT_REG;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0001)) != RT_ERR_OK)
            goto ERR;
        if ((ret = hal_miim_read(unit, port, page, reg, &phyData4)) != RT_ERR_OK)
            goto ERR;

        phyData4 = phyData4 & ~(Pause_R4_MASK | AsymmetricPause_R4_MASK);
        phyData4 = phyData4 | (pAbility->FC << Pause_R4_OFFSET)
                | (pAbility->AsyFC << AsymmetricPause_R4_OFFSET);

        phyData4 = phyData4 &
                ~(_100Base_TX_FD_R4_MASK | _100Base_TX_R4_MASK | _10Base_T_FD_R4_MASK | _10Base_T_R4_MASK);
        phyData4 = phyData4
                | (pAbility->Full_100 << _100Base_TX_FD_R4_OFFSET)
                | (pAbility->Half_100 << _100Base_TX_R4_OFFSET)
                | (pAbility->Full_10 << _10Base_T_FD_R4_OFFSET)
                | (pAbility->Half_10 << _10Base_T_R4_OFFSET);

        if ((ret = hal_miim_write(unit, port, page, reg, phyData4)) != RT_ERR_OK)
            goto ERR;

        /* register 9 */
        page = 0xa41;
        reg = PHY_INT_REG_BASE + (PHY_1000_BASET_CONTROL_REG % PHY_INT_REG_NEXT);
        if ((ret = hal_miim_read(unit, port, page, reg, &phyData9)) != RT_ERR_OK)
            goto ERR;

        phyData9 = phyData9 & ~(_1000Base_TFullDuplex_MASK | _1000Base_THalfDuplex_MASK);
        phyData9 = phyData9 | (pAbility->Full_1000 << _1000Base_TFullDuplex_OFFSET)
                   | (pAbility->Half_1000 << _1000Base_THalfDuplex_OFFSET);

        if ((ret = hal_miim_write(unit, port, page, reg, phyData9)) != RT_ERR_OK)
            goto ERR;

        /* Force re-autonegotiation if AN is on */
        if (ENABLED == enable)
        {
            page = 0xa40;
            reg = PHY_INT_REG_BASE + PHY_CONTROL_REG;

            if ((ret = hal_miim_read(unit, port, page, reg, &phyData0)) != RT_ERR_OK)
                goto ERR;

            phyData0 = phyData0 & ~(RestartAutoNegotiation_MASK);
            phyData0 = phyData0 | (enable << RestartAutoNegotiation_OFFSET);

            if ((ret = hal_miim_write(unit, port, page, reg, phyData0)) != RT_ERR_OK)
                goto ERR;
        }
    }

ERR:
    hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0000);
    hal_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, 0x0000);
    return ret;
} /* end of phy_8214fc_autoNegoAbility_set */

/* Function Name:
 *      phy_8214fc_duplex_get
 * Description:
 *      Get 8214FC duplex mode status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pDuplex - pointer to PHY duplex mode status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214fc_duplex_get(uint32 unit, rtk_port_t port, uint32 *pDuplex)
{
    rtk_port_media_t    media;
    uint32              link_sts;
    uint32              phyData0;
    uint32              page, reg;
    int32               ret;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    reg = PHY_INT_REG_BASE + PHY_CONTROL_REG;

    if (PORT_MEDIA_FIBER == media)
    {
        page = PHY_PAGE_0;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0003)) != RT_ERR_OK)
            goto ERR;
    }
    else
    {
        page = 0xa40;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0001)) != RT_ERR_OK)
            goto ERR;
    }

    if ((ret = hal_miim_read(unit, port, page, reg, &phyData0)) != RT_ERR_OK)
        goto ERR;

    *pDuplex = (phyData0 & DuplexMode_MASK) >> DuplexMode_OFFSET;

ERR:
    hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0000);
    hal_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, 0x0000);
    return ret;
} /* end of phy_8214fc_duplex_get */

/* Function Name:
 *      phy_8214fc_duplex_set
 * Description:
 *      Set 8214FC duplex mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      duplex        - duplex mode of the port, full or half
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214fc_duplex_set(uint32 unit, rtk_port_t port, uint32 duplex)
{
    rtk_port_media_t    media;
    uint32              link_sts;
    uint32              phyData0;
    uint32              page, reg;
    int32               ret;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    reg = PHY_INT_REG_BASE + PHY_CONTROL_REG;

    if (PORT_MEDIA_FIBER == media)
    {
        page = PHY_PAGE_0;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0003)) != RT_ERR_OK)
            goto ERR;
    }
    else
    {
        page = 0xa40;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0001)) != RT_ERR_OK)
            goto ERR;
    }

    if ((ret = hal_miim_read(unit, port, page, reg, &phyData0)) != RT_ERR_OK)
        goto ERR;

    phyData0 = phyData0 & ~(DuplexMode_MASK);
    phyData0 = phyData0 | (duplex << DuplexMode_OFFSET);

    if ((ret = hal_miim_write(unit, port, page, reg, phyData0)) != RT_ERR_OK)
        goto ERR;

ERR:
    hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0000);
    hal_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, 0x0000);
    return ret;
} /* end of phy_8214fc_duplex_set */

/* Function Name:
 *      phy_8214fc_speed_get
 * Description:
 *      Get 8214FC MP link speed status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214fc_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    rtk_port_media_t    media;
    uint32              link_sts;
    uint32              phyData0;
    uint32              page, reg;
    int32               ret;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    reg = PHY_INT_REG_BASE + PHY_CONTROL_REG;

    if (PORT_MEDIA_FIBER == media)
    {
        page = PHY_PAGE_0;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0003)) != RT_ERR_OK)
            goto ERR;
    }
    else
    {
        page = 0xa40;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0001)) != RT_ERR_OK)
            goto ERR;
    }

    if ((ret = hal_miim_read(unit, port, page, reg, &phyData0)) != RT_ERR_OK)
        goto ERR;

    *pSpeed = ((phyData0 & SpeedSelection1_MASK) >> (SpeedSelection1_OFFSET -1))
            | ((phyData0 & SpeedSelection0_MASK) >> SpeedSelection0_OFFSET);

ERR:
    hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0000);
    hal_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, 0x0000);
    return ret;
} /* end of phy_8214fc_speed_get */

/* Function Name:
 *      phy_8214fc_speedStatus_get
 * Description:
 *      Get PHY operational link speed status
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY operational link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_8214fc_speedStatus_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    rtk_port_duplex_t duplex;

    return phy_8214fc_speedDuplexStatus_get(unit, port, pSpeed, &duplex);
}

/* Function Name:
 *      phy_8214fc_speedDuplexStatus_get
 * Description:
 *      Get PHY operational link speed-duplex status from proprietary register
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pSpeed - pointer to PHY operational link speed
 *      pDuplex - pointer to PHY operational link duplex
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_8214fc_speedDuplexStatus_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex)
{
    rtk_port_media_t    media;
    uint32              link_sts;
    uint32              phyData0;
    uint32              page, reg, spd;
    int32               ret;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if (PORT_MEDIA_FIBER == media)
    {
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0003)) != RT_ERR_OK)
            goto ERR;

        page = PHY_PAGE_0;
        reg = PHY_INT_REG_BASE + PHY_CONTROL_REG;
        /* fiber mode speed decision is also using 0.[6,13] */
        if ((ret = hal_miim_read(unit, port, page, reg, &phyData0)) != RT_ERR_OK)
            goto ERR;

        spd = ((phyData0 & SpeedSelection1_MASK) >> (SpeedSelection1_OFFSET -1))
                | ((phyData0 & SpeedSelection0_MASK) >> SpeedSelection0_OFFSET);
        switch(spd)
        {
          case 0x0:
            *pSpeed = PORT_SPEED_10M;
            break;
          case 0x1:
            *pSpeed = PORT_SPEED_100M;
            break;
          case 0x2:
            *pSpeed = PORT_SPEED_1000M;
            break;
          default:
            goto ERR;
        }

        *pDuplex = (phyData0 & DuplexMode_MASK) ? PORT_FULL_DUPLEX : PORT_HALF_DUPLEX;
    }
    else
    {
        if ((ret = phy_common_speedDuplexStatusResReg_get(unit, port, pSpeed, pDuplex)) != RT_ERR_OK)
            goto ERR;
    }

ERR:
    hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0000);
    hal_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, 0x0000);
    return ret;
}

/* Function Name:
 *      phy_8214fc_MP_speed_set
 * Description:
 *      Set 8214FC MP speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - copper media chip is not supported Force-1000
 * Note:
 *      None
 */
int32
phy_8214fc_MP_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    rtk_port_media_t    media;
    uint32              link_sts;
    uint32              phyData0;
    uint32              page, reg;
    int32               ret;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    reg = PHY_INT_REG_BASE + PHY_CONTROL_REG;

    if (PORT_MEDIA_FIBER == media)
    {
        page = PHY_PAGE_0;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0003)) != RT_ERR_OK)
            goto ERR;
    }
    else
    {
        page = 0xa40;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0001)) != RT_ERR_OK)
            goto ERR;
    }

    if ((ret = hal_miim_read(unit, port, page, reg, &phyData0)) != RT_ERR_OK)
        goto ERR;

    phyData0 = phyData0 & ~(SpeedSelection1_MASK | SpeedSelection0_MASK);
    phyData0 = phyData0 | (((speed & 2) << (SpeedSelection1_OFFSET - 1))
            | ((speed & 1) << SpeedSelection0_OFFSET));

    if ((ret = hal_miim_write(unit, port, page, reg, phyData0)) != RT_ERR_OK)
        goto ERR;

ERR:
    hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0000);
    hal_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, 0x0000);
    return ret;
} /* end of phy_8214fc_MP_speed_set */


#endif  /* (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC) */
#ifdef CONFIG_SDK_RTL8218FB
/* Function Name:
 *      phy_8218fb_autoNegoEnable_get
 * Description:
 *      Get 8218FB autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218fb_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    if (phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info))
        return phy_8214fc_autoNegoEnable_get(unit, port, pEnable);
    else
        return phy_common_autoNegoEnable_get(unit, port, pEnable);

    return RT_ERR_OK;
} /* end of phy_8218fb_autoNegoEnable_get */

/* Function Name:
 *      phy_8218fb_autoNegoEnable_set
 * Description:
 *      Set 8218FB autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218fb_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    if (phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info))
        return phy_8214fc_autoNegoEnable_set(unit, port, enable);
    else
        return phy_common_autoNegoEnable_set(unit, port, enable);

    return RT_ERR_OK;
} /* end of phy_8218fb_autoNegoEnable_set */

/* Function Name:
 *      phy_8218fb_MP_autoNegoAbility_get
 * Description:
 *      Get 8218FB ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218fb_MP_autoNegoAbility_get(uint32 unit, rtk_port_t port,
    rtk_port_phy_ability_t *pAbility)
{
    if (phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info))
        return phy_8214fc_autoNegoAbility_get(unit, port, pAbility);
    else
        return phy_8218b_autoNegoAbility_get(unit, port, pAbility);

    return RT_ERR_OK;
} /* end of phy_8218fb_MP_autoNegoAbility_get */

/* Function Name:
 *      phy_8218fb_MP_autoNegoAbility_set
 * Description:
 *      Set 8218FB ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      pAbility  - auto negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218fb_MP_autoNegoAbility_set(uint32 unit, rtk_port_t port,
    rtk_port_phy_ability_t *pAbility)
{
    if (phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info))
        return phy_8214fc_autoNegoAbility_set(unit, port, pAbility);
    else
        return phy_8218b_autoNegoAbility_set(unit, port, pAbility);

    return RT_ERR_OK;
} /* end of phy_8218fb_MP_autoNegoAbility_set */

/* Function Name:
 *      phy_8218fb_duplex_get
 * Description:
 *      Get 8218FB duplex mode status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pDuplex - pointer to PHY duplex mode status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218fb_duplex_get(uint32 unit, rtk_port_t port, uint32 *pDuplex)
{
    if (phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info))
        return phy_8214fc_duplex_get(unit, port, pDuplex);
    else
        return phy_common_duplex_get(unit, port, pDuplex);

    return RT_ERR_OK;
} /* end of phy_8218fb_duplex_get */

/* Function Name:
 *      phy_8218fb_duplex_set
 * Description:
 *      Set 8218FB duplex mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      duplex        - duplex mode of the port, full or half
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218fb_duplex_set(uint32 unit, rtk_port_t port, uint32 duplex)
{
    if (phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info))
        return phy_8214fc_duplex_set(unit, port, duplex);
    else
        return phy_common_duplex_set(unit, port, duplex);

    return RT_ERR_OK;
} /* end of phy_8218fb_duplex_set */

/* Function Name:
 *      phy_8218fb_speed_get
 * Description:
 *      Get 8218FB MP link speed status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218fb_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    if (phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info))
        return phy_8214fc_speed_get(unit, port, pSpeed);
    else
        return phy_8218b_speed_get(unit, port, pSpeed);

    return RT_ERR_OK;
} /* end of phy_8218fb_speed_get */

/* Function Name:
 *      phy_8218fb_MP_speed_set
 * Description:
 *      Set 8218FB MP speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - copper media chip is not supported Force-1000
 * Note:
 *      None
 */
int32
phy_8218fb_MP_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    if (phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info))
        return phy_8214fc_MP_speed_set(unit, port, speed);
    else
        return phy_8218b_speed_set(unit, port, speed);

    return RT_ERR_OK;
} /* end of phy_8218fb_MP_speed_set */

/* Function Name:
 *      phy_8218fb_speedStatus_get
 * Description:
 *      Get PHY operational link speed status
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY operational link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_8218fb_speedStatus_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    if (phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info))
        return phy_8214fc_speedStatus_get(unit, port, pSpeed);
    else
        return phy_common_speedStatusResReg_get(unit, port, pSpeed);
} /* end of phy_8218fb_speedStatus_get */

/* Function Name:
 *      phy_8218fb_speedDuplexStatus_get
 * Description:
 *      Get PHY operational link speed-duplex status from proprietary register
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY operational link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_8218fb_speedDuplexStatus_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex)
{
    if (phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info))
        return phy_8214fc_speedDuplexStatus_get(unit, port, pSpeed, pDuplex);
    else
        return phy_common_speedDuplexStatusResReg_get(unit, port, pSpeed, pDuplex);
} /* end of phy_8218fb_speedDuplexStatus_get */

#endif  /* CONFIG_SDK_RTL8218FB */

#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8218B)
/* Function Name:
 *      phy_int8380_rtctResult_get
 * Description:
 *      Get test result of RTCT.
 * Input:
 *      unit        - unit id
 *      port        - the port for retriving RTCT test result
 * Output:
 *      pRtctResult - RTCT result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_RTCT_NOT_FINISH   - RTCT not finish. Need to wait a while.
 *      RT_ERR_TIMEOUT      - RTCT test timeout in this port.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The result unit is cm
 */
int32
phy_int8380_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData, fixed_page;
    hal_control_t   *pHalCtrl;
    uint32 speed;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    /* Check the port is link up or not? */
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData)) != RT_ERR_OK)
        return ret;

    phy_common_speed_get(unit, port, &speed);

    if ((phyData & LinkStatus_MASK) && speed != PORT_SPEED_10M)
    {
        /* If the port is link up,
         * return cable length from green function
         */

        fixed_page = 0xa88;

        /* The Length is store in [7:0], and the unit is meter*/
        if ((ret = hal_miim_read(unit, port, fixed_page, 16, &phyData)) != RT_ERR_OK)
            return ret;

        osal_memset(pRtctResult, 0, sizeof(rtk_rtctResult_t));
        if (speed == PORT_SPEED_1000M) /* 1000M */
        {
            pRtctResult->linkType = PORT_SPEED_1000M;
            pRtctResult->un.ge_result.channelALen = (phyData & 0x00FF)*100;
            pRtctResult->un.ge_result.channelBLen = (phyData & 0x00FF)*100;
            pRtctResult->un.ge_result.channelCLen = (phyData & 0x00FF)*100;
            pRtctResult->un.ge_result.channelDLen = (phyData & 0x00FF)*100;
        }
        else  /* 100M */
        {
            pRtctResult->linkType = PORT_SPEED_100M;
            pRtctResult->un.fe_result.rxLen = (phyData & 0x00FF)*100;
            pRtctResult->un.fe_result.txLen = (phyData & 0x00FF)*100;
        }
    }
    else
    {
        /* If the port is link down,
         * return cable length from RTCT function
         */
        /* Page 0xa42, Register 17
         * bit[15]: cable test finished or not?
         *             1: Finished
         *             0: Not finished
         */
         fixed_page = 0xa42;

         if ((ret = hal_miim_read(unit, port, fixed_page, 17, &phyData)) != RT_ERR_OK)
            return ret;

         if(((phyData >> RTCT_DONE) & 0x1) != 0x1)
            return RT_ERR_PHY_RTCT_NOT_FINISH;

        if (HWP_GE_PORT(unit, port))
        {
            pRtctResult->linkType = PORT_SPEED_1000M;
            /* Length = (Index/64)*8ns*(0.2m/ns) = Index/80 (m) = (1.25) * Index (cm) */

            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_LEN_ADDR_A)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= 0x3FFF; /*[13:0] are valid*/
            pRtctResult->un.ge_result.channelALen = (phyData)*5/4;

            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_LEN_ADDR_B)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= 0x3FFF; /*[13:0] are valid*/
            pRtctResult->un.ge_result.channelBLen = (phyData)*5/4;

            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_LEN_ADDR_C)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= 0x3FFF; /*[13:0] are valid*/
            pRtctResult->un.ge_result.channelCLen = (phyData)*5/4;

            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_LEN_ADDR_D)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= 0x3FFF; /*[13:0] are valid*/
            pRtctResult->un.ge_result.channelDLen = (phyData)*5/4;

            /* === Channel A Status ===*/
            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_STATUS_ADDR_A)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;

            if(((phyData >> RTCT_STATUS_SHORT) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelAShort = 1;
            if(((phyData >> RTCT_STATUS_OPEN) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelAOpen = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_SHORT) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelAMismatch = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_OPEN) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelAMismatch |= 0x2;
            if(((phyData >> RTCT_STATUS_LINE_DRIVER) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelALinedriver = 1;

            /* === Channel B Status ===*/
            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_STATUS_ADDR_B)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;

            if(((phyData >> RTCT_STATUS_SHORT) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelBShort = 1;
            if(((phyData >> RTCT_STATUS_OPEN) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelBOpen = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_SHORT) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelBMismatch = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_OPEN) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelBMismatch |= 0x2;
            if(((phyData >> RTCT_STATUS_LINE_DRIVER) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelBLinedriver = 1;

            /* === Channel C Status ===*/
            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_STATUS_ADDR_C)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;

            if(((phyData >> RTCT_STATUS_SHORT) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelCShort = 1;
            if(((phyData >> RTCT_STATUS_OPEN) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelCOpen = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_SHORT) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelCMismatch = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_OPEN) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelCMismatch |= 0x2;
            if(((phyData >> RTCT_STATUS_LINE_DRIVER) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelCLinedriver = 1;

            /* === Channel D Status ===*/
            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_STATUS_ADDR_D)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;

            if(((phyData >> RTCT_STATUS_SHORT) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelDShort = 1;
            if(((phyData >> RTCT_STATUS_OPEN) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelDOpen = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_SHORT) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelDMismatch = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_OPEN) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelDMismatch |= 0x2;
            if(((phyData >> RTCT_STATUS_LINE_DRIVER) & 0x1) != 0x0)
                pRtctResult->un.ge_result.channelDLinedriver = 1;
        }
        else /* if (HWP_FE_PORT(unit, port)) */
        {
            pRtctResult->linkType = PORT_SPEED_100M;
            /* Length = (Index/64)*8ns*(0.2m/ns) = Index/80 (m) = (1.25) * Index (cm) */

            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_LEN_ADDR_A)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= 0x3FFF; /*[13:0] are valid*/
            pRtctResult->un.fe_result.rxLen = (phyData)*5/4;

            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_LEN_ADDR_B)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;
            phyData &= 0x3FFF; /*[13:0] are valid*/
            pRtctResult->un.fe_result.txLen = (phyData)*5/4;

            /* === Channel A Status ===*/
            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_STATUS_ADDR_A)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;

            if(((phyData >> RTCT_STATUS_SHORT) & 0x1) != 0x0)
                pRtctResult->un.fe_result.isRxShort = 1;
            if(((phyData >> RTCT_STATUS_OPEN) & 0x1) != 0x0)
                pRtctResult->un.fe_result.isRxOpen = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_SHORT) & 0x1) != 0x0)
                pRtctResult->un.fe_result.isRxMismatch = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_OPEN) & 0x1) != 0x0)
                pRtctResult->un.fe_result.isRxMismatch |= 0x2;
            if(((phyData >> RTCT_STATUS_LINE_DRIVER) & 0x1) != 0x0)
                pRtctResult->un.fe_result.isRxLinedriver = 1;

            /* === Channel B Status ===*/
            if ((ret = hal_miim_write(unit, port, fixed_page, 27, RTCT_STATUS_ADDR_B)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, fixed_page, 28, &phyData)) != RT_ERR_OK)
                return ret;

            if(((phyData >> RTCT_STATUS_SHORT) & 0x1) != 0x0)
                pRtctResult->un.fe_result.isTxShort = 1;
            if(((phyData >> RTCT_STATUS_OPEN) & 0x1) != 0x0)
                pRtctResult->un.fe_result.isTxOpen = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_SHORT) & 0x1) != 0x0)
                pRtctResult->un.fe_result.isTxMismatch = 1;
            if(((phyData >> RTCT_STATUS_MISSMATCH_OPEN) & 0x1) != 0x0)
                pRtctResult->un.fe_result.isTxMismatch |= 0x2;
            if(((phyData >> RTCT_STATUS_LINE_DRIVER) & 0x1) != 0x0)
                pRtctResult->un.fe_result.isTxLinedriver = 1;
        }
    }

    return ret;
} /* end of phy_int8380_rtctResult_get */
#endif  /* defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8218B) */

#ifdef CONFIG_SDK_RTL8218B
/* Function Name:
 *      phy_8218_patch_set
 * Description:
 *      Set patch to PHY.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8218b_patch_set(uint32 unit, rtk_port_t port)
{
    int32 ret;
    ret = sub_phy_8218b_patch_set(unit, port);
    return ret;
} /* end of phy_8218b_patch_set */
#endif  /* CONFIG_SDK_RTL8218B */
#ifdef CONFIG_SDK_RTL8214FC
/* Function Name:
 *      phy_8214fc_mp_patch_set
 * Description:
 *      Set patch to PHY.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8214fc_mp_patch_set(uint32 unit, rtk_port_t port)
{
    int32 ret;
    ret = sub_phy_8214fc_mp_patch_set(unit, port);
    return ret;
} /* end of phy_8214fc_mp_patch_set */
#endif  /* #ifdef CONFIG_SDK_RTL8214FC */
#ifdef CONFIG_SDK_RTL8218FB
/* Function Name:
 *      phy_8218fb_mp_patch_set
 * Description:
 *      Set patch to PHY.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8218fb_mp_patch_set(uint32 unit, rtk_port_t port)
{
    int32 ret;
    ret = sub_phy_8218fb_mp_patch_set(unit, port);
    return ret;
} /* end of phy_8218fb_mp_patch_set */
#endif  /* CONFIG_SDK_RTL8218FB */

#ifdef CONFIG_SDK_RTL8218FB
/* Function Name:
 *      phy_8218fb_fiberDownSpeedEnable_get
 * Description:
 *      Get fiber down speed status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of fiber down speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
phy_8218fb_fiberDownSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    RT_PARAM_CHK(phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info) == FALSE, RT_ERR_CHIP_NOT_SUPPORTED);

    return phy_8214fc_fiberDownSpeedEnable_get(unit, port, pEnable);
}

/* Function Name:
 *      phy_8218fb_fiberDownSpeedEnable_set
 * Description:
 *      Set fiber down speed status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of fiber down speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
phy_8218fb_fiberDownSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    RT_PARAM_CHK(phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info) == FALSE, RT_ERR_CHIP_NOT_SUPPORTED);

    return phy_8214fc_fiberDownSpeedEnable_set(unit, port, enable);
}
#endif  /* CONFIG_SDK_RTL8218FB */
#if (defined CONFIG_SDK_RTL8218B) || (defined CONFIG_SDK_RTL8218FB)
/* Function Name:
 *      phy_8218b_utpDownSpeedEnable_get
 * Description:
 *      Get UTP down speed 1000M --> 100M status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of UTP down speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
phy_8218b_utpDownSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    uint32  phyData;
    int32   ret;

    if ((ret = hal_miim_write(unit, port, 0, 30, 1)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 0xa44, 17, &phyData)) != RT_ERR_OK)
        goto ERR;

    if (phyData & (1 << 3))
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    if ((ret = hal_miim_write(unit, port, 0, 30, 0)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
ERR:
    hal_miim_write(unit, port, 0, 30, 0);
    return ret;
}

/* Function Name:
 *      phy_8218b_utpDownSpeedEnable_set
 * Description:
 *      Set UTP down speed 1000M --> 100M status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of UTP down speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
phy_8218b_utpDownSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    uint32  phyData;
    int32   ret;

    if ((ret = hal_miim_write(unit, port, 0, 30, 1)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 0xa44, 17, &phyData)) != RT_ERR_OK)
        goto ERR;

    /* DownSpeed to 100M*/
    phyData &= ~(1 << 5);

    if (ENABLED == enable)
        phyData |= (1 << 3);
    else
        phyData &= ~(1 << 3);

    if ((ret = hal_miim_write(unit, port, 0xa44, 17, phyData)) != RT_ERR_OK)
        goto ERR;

    if ((ret = hal_miim_write(unit, port, 0, 30, 0)) != RT_ERR_OK)
        return ret;

    return ret;
ERR:
    hal_miim_write(unit, port, 0, 30, 0);
    return ret;
}

/* Function Name:
 *      phy_8218b_downSpeedEnable_get
 * Description:
 *      Get UTP down speed 1000M --> 100M status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of down speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
phy_8218b_downSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    int32   ret;
    rtk_port_media_t    media;
    uint32              link_sts;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if(PORT_MEDIA_FIBER == media)
    {
        ret = phy_8214fc_fiberDownSpeedEnable_get(unit, port, pEnable);
    }else{
        ret = phy_8218b_utpDownSpeedEnable_get(unit, port, pEnable);
    }
    return ret;
}

/* Function Name:
 *      phy_8218b_downSpeedEnable_set
 * Description:
 *      Set UTP down speed 1000M --> 100M status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of down speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
phy_8218b_downSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    int32   ret;
    rtk_port_media_t    media;
    uint32              link_sts;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if(PORT_MEDIA_FIBER == media)
    {
        ret = phy_8214fc_fiberDownSpeedEnable_set(unit, port, enable);
    }else{
        ret = phy_8218b_utpDownSpeedEnable_set(unit, port, enable);
    }
    return ret;
}
#endif  /* (defined CONFIG_SDK_RTL8218B) || (defined CONFIG_SDK_RTL8218FB) */
#if (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC)
/* Function Name:
 *      phy_8214fc_fiberNwayForceLink_get
 * Description:
 *      When fiber port is configured N-way,
 *      which can link with link partner is configured force mode.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pEnable - fiber Nway force links status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214fc_fiberNwayForceLink_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    uint32  phyData;
    int32   ret;

    if ((ret = hal_miim_write(unit, port, 0, 29, 3)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 8, 20, &phyData)) != RT_ERR_OK)
        goto ERR;

    if ((phyData >> 2) & 0x1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    if ((ret = hal_miim_write(unit, port, 0, 29, 0)) != RT_ERR_OK)
        return ret;

    return ret;
ERR:
    hal_miim_write(unit, port, 0, 29, 0);
    return ret;
}   /* end of phy_8214fc_fiberNwayForceLink_get */

/* Function Name:
 *      phy_8214fc_fiberNwayForceLink_set
 * Description:
 *      When fiber port is configured N-way,
 *      which can link with link partner is configured force mode.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      enable - fiber Nway force links status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214fc_fiberNwayForceLink_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    uint32  phyData;
    int32   ret;

    if ((ret = hal_miim_write(unit, port, 0, 29, 3)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 8, 20, &phyData)) != RT_ERR_OK)
        goto ERR;

    if (ENABLED == enable)
        phyData |= (1 << 2);
    else
        phyData &= ~(1 << 2);

    if ((ret = hal_miim_write(unit, port, 8, 20, phyData)) != RT_ERR_OK)
        goto ERR;

    if ((ret = hal_miim_write(unit, port, 0, 29, 0)) != RT_ERR_OK)
        return ret;

    return ret;
ERR:
    hal_miim_write(unit, port, 0, 29, 0);
    return ret;
}   /* end of phy_8214fc_fiberNwayForceLink_set */
#endif  /* (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC) */
#ifdef CONFIG_SDK_RTL8218FB
/* Function Name:
 *      phy_8218fb_fiberNwayForceLink_get
 * Description:
 *      When fiber port is configured N-way,
 *      which can link with link partner is configured force mode.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pEnable - fiber Nway force links status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218fb_fiberNwayForceLink_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    RT_PARAM_CHK(phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info) == FALSE, RT_ERR_CHIP_NOT_SUPPORTED);

    return phy_8214fc_fiberNwayForceLink_get(unit, port, pEnable);
}   /* end of phy_8218fb_fiberNwayForceLink_get */

/* Function Name:
 *      phy_8218fb_fiberNwayForceLink_set
 * Description:
 *      When fiber port is configured N-way,
 *      which can link with link partner is configured force mode.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      enable - fiber Nway force links status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8218fb_fiberNwayForceLink_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    RT_PARAM_CHK(phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info) == FALSE, RT_ERR_CHIP_NOT_SUPPORTED);

    return phy_8214fc_fiberNwayForceLink_set(unit, port, enable);
}   /* end of phy_8218fb_fiberNwayForceLink_set */
#endif  /* CONFIG_SDK_RTL8218FB */
#ifdef CONFIG_SDK_RTL8214FC
/* Function Name:
 *      phy_8214fc_fiberOAMLoopBack_set
 * Description:
 *      Set Fiber-Port OAM Loopback feature,
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      enable - Fiber-Port OAM Loopback feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214fc_fiberOAMLoopBack_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    uint32  phyData;
    uint32  reg_val;
    int32    ret;

    /*Enable Loopback*/
    /*Backup*/
    if ((ret = hal_miim_read(unit, port, 0, 30, &phyData)) != RT_ERR_OK)
        return ret;

    reg_val = 0x03;
    if ((ret = hal_miim_write(unit, port, 0, 30, reg_val)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 8, 16, &reg_val)) != RT_ERR_OK)
        return ret;

    if(enable == ENABLED)
        reg_val |= (1UL<<4);
    else
        reg_val &= ~(1UL<<4);

    if ((ret = hal_miim_write(unit, port, 8, 16, reg_val)) != RT_ERR_OK)
        return ret;

    /*Restore*/
    if ((ret = hal_miim_write(unit, port, 0, 30, phyData)) != RT_ERR_OK)
        return ret;

    if(enable == ENABLED)
    {
        /*Delay until PHY Linkup*/
        osal_time_usleep(100 * 1000); /* delay 10mS */
    }

    return ret;
}   /* end of phy_8214fc_fiberOAMLoopBack_set */
#endif  /* CONFIG_SDK_RTL8214FC */
#if (defined CONFIG_SDK_RTL8218B) || (defined CONFIG_SDK_RTL8218FB)
/* Function Name:
 *      _phy_8218b_ptpReg_get
 * Description:
 *      Get PTP register data of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      reg_addr            - reg address
 * Output:
 *      pData               - pointer to the PHY reg data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      None
 */
static int32
_phy_8218b_ptpReg_get(uint32 unit, rtk_port_t port, uint32 reg_addr, uint32 *pData)
{
    int32 ret;
    rtk_port_t port_base;
    uint32 page, reg;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d, port=%d, reg_addr=0x%x, reg=0x%x",
           unit, port, reg_addr);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    port_base = port - (port % 8);
    page = (reg_addr - (reg_addr % 8)) / 8;
    reg = ((reg_addr - REG_8218B_PTP_BASE) % 8) + 16;

    if ((ret = hal_miim_write(unit, port_base, 0, 29, 8)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = hal_miim_read(unit, port_base, page, reg, pData)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = hal_miim_write(unit, port_base, 0, 29, 0)) != RT_ERR_OK)
    {
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pData=0x%x", *pData);

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_8218b_ptpReg_set
 * Description:
 *      Set PTP register data of the specific port
 * Input:
 *      unit               - unit id
 *      port               - port id
 *      reg_addr           - reg address
 *      reg_val            - reg value
 *      reg_msk            - reg value mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
_phy_8218b_ptpReg_set(uint32 unit, uint32 port, uint32 reg_addr, uint32 reg_val, uint32 reg_msk)
{
    int32 ret;
    rtk_port_t port_base;
    uint32 page, reg, ori_val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, port=%d, reg_addr=0x%x, reg_val=0x%x, reg_msk=0x%x",
            unit, port, reg_addr, reg_val, reg_msk);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    port_base = port - (port % 8);
    page = (reg_addr - (reg_addr % 8)) / 8;
    reg = ((reg_addr - REG_8218B_PTP_BASE) % 8) + 16;

    if ((ret = hal_miim_write(unit, port_base, 0, 29, 8)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((reg_msk & 0xFFFF) != 0xFFFF)
    {
        if ((ret = hal_miim_read(unit, port_base, page, reg, &ori_val)) != RT_ERR_OK)
        {
            return ret;
        }

        reg_val = (ori_val & ~reg_msk) | (reg_val & reg_msk);
    }
    if ((ret = hal_miim_write(unit, port_base, page, reg, reg_val)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = hal_miim_write(unit, port_base, 0, 29, 0)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8218b_ptpSwitchMacAddr_get
 * Description:
 *      Get the Switch MAC address setting of PHY of the specified port.
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      pSwitchMacAddr - point to the Switch MAC Address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8218b_ptpSwitchMacAddr_get(uint32 unit, rtk_port_t port, rtk_mac_t *pSwitchMacAddr)
{
    int32 ret;
    uint32 mac_h, mac_m, mac_l;

    if ((ret = _phy_8218b_ptpReg_get(unit, port, REG_8218B_PTP_MAC_ADDR_H, &mac_h)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = _phy_8218b_ptpReg_get(unit, port, REG_8218B_PTP_MAC_ADDR_M, &mac_m)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = _phy_8218b_ptpReg_get(unit, port, REG_8218B_PTP_MAC_ADDR_L, &mac_l)) != RT_ERR_OK)
    {
        return ret;
    }

    pSwitchMacAddr->octet[0] = (uint8)(mac_h >> 8);
    pSwitchMacAddr->octet[1] = (uint8)(mac_h & 0xFF);
    pSwitchMacAddr->octet[2] = (uint8)(mac_m >> 8);
    pSwitchMacAddr->octet[3] = (uint8)(mac_m & 0xFF);
    pSwitchMacAddr->octet[4] = (uint8)(mac_l >> 8);
    pSwitchMacAddr->octet[5] = (uint8)(mac_l & 0xFF);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218b_ptpSwitchMacAddr_set
 * Description:
 *      Set the Switch MAC address setting of PHY of the specified port.
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      pSwitchMacAddr - point to the Switch MAC Address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8218b_ptpSwitchMacAddr_set(uint32 unit, rtk_port_t port, rtk_mac_t *pSwitchMacAddr)
{
    int32 ret;
    uint32 mac_h, mac_m, mac_l;

    mac_h = (pSwitchMacAddr->octet[0] << 8) | (pSwitchMacAddr->octet[1]);
    mac_m = (pSwitchMacAddr->octet[2] << 8) | (pSwitchMacAddr->octet[3]);
    mac_l = (pSwitchMacAddr->octet[4] << 8) | (pSwitchMacAddr->octet[5]);

    if ((ret = _phy_8218b_ptpReg_set(unit, port, REG_8218B_PTP_MAC_ADDR_H, mac_h, 0xFFFF)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = _phy_8218b_ptpReg_set(unit, port, REG_8218B_PTP_MAC_ADDR_M, mac_m, 0xFFFF)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = _phy_8218b_ptpReg_set(unit, port, REG_8218B_PTP_MAC_ADDR_L, mac_l, 0xFFFF)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218b_ptpRefTime_get
 * Description:
 *      Get the reference time of PHY of the specified port.
 * Input:
 *      unit       - unit id
 * Output:
 *      pTimeStamp - pointer buffer of the reference time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8218b_ptpRefTime_get(uint32 unit, rtk_port_t port,
    rtk_time_timeStamp_t *pTimeStamp)
{
    int32 ret;
    uint32 sec_l, sec_h, nsec_l, nsec_h;
    uint32 reg_val, reg_msk;

    /* execute reading command, [15] = 0x1 executing, [13:12] = 0x0 read */
    reg_val = REG_FIELD_8218B_PTP_CMD_EXEC | REG_FIELD_8218B_PTP_CMD_OP_READ;
    reg_msk = REG_FIELD_8218B_PTP_CMD_EXEC | REG_FIELD_8218B_PTP_CMD_OP_MSK;

    if ((ret = _phy_8218b_ptpReg_set(unit, port, REG_8218B_PTP_TIME_NSEC_H, reg_val, reg_msk)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }
    do {
        if ((ret = _phy_8218b_ptpReg_get(unit, port, REG_8218B_PTP_TIME_NSEC_H, &reg_val)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
            return ret;
        }
    } while (reg_val & REG_FIELD_8218B_PTP_CMD_EXEC); /* busy watting */

    if ((ret = _phy_8218b_ptpReg_get(unit, port, REG_8218B_PTP_TIME_NSEC_L_RO, &nsec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }
    if ((ret = _phy_8218b_ptpReg_get(unit, port, REG_8218B_PTP_TIME_NSEC_H_RO, &nsec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }
    if ((ret = _phy_8218b_ptpReg_get(unit, port, REG_8218B_PTP_TIME_SEC_L_RO, &sec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }
    if ((ret = _phy_8218b_ptpReg_get(unit, port, REG_8218B_PTP_TIME_SEC_H_RO, &sec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }

    pTimeStamp->sec = (sec_h << 16) | (sec_l & 0xFFFF);
    pTimeStamp->nsec = (((nsec_h & 0x7FF) << 16) | (nsec_l & 0xFFFF)) << 3; /* convert 8nsec to nsec */

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218b_ptpRefTime_set
 * Description:
 *      Set the reference time of PHY of the specified port.
 * Input:
 *      unit      - unit id
 *      timeStamp - reference timestamp value
 *      exec        - 0 : do not execute, 1: execute
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8218b_ptpRefTime_set(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t timeStamp, uint32 exec)
{
    int32 ret;
    uint32 sec_l, sec_h, nsec_l, nsec_h;
    uint32 reg_val;

    /* adjust Timer of PHY */
    sec_l = (timeStamp.sec) & 0xFFFF;
    sec_h = (timeStamp.sec) >> 16;
    /* convert nsec to 8nsec */
    nsec_l = (timeStamp.nsec >> 3) & 0xFFFF;
    nsec_h = ((timeStamp.nsec >> 3) >> 16) & 0x07FF;

    /* execute writing command, [15] = 0x1 executing, [13:12] = 0x1 write */
    if(exec)
        nsec_h = nsec_h | REG_FIELD_8218B_PTP_CMD_EXEC | REG_FIELD_8218B_PTP_CMD_OP_WRITE;
    else
        nsec_h = nsec_h | REG_FIELD_8218B_PTP_CMD_OP_WRITE;

    if ((ret = _phy_8218b_ptpReg_set(unit, port, REG_8218B_PTP_TIME_SEC_L, sec_l, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }
    if ((ret = _phy_8218b_ptpReg_set(unit, port, REG_8218B_PTP_TIME_SEC_H, sec_h, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }
    if ((ret = _phy_8218b_ptpReg_set(unit, port, REG_8218B_PTP_TIME_NSEC_L, nsec_l, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }
    if ((ret = _phy_8218b_ptpReg_set(unit, port, REG_8218B_PTP_TIME_NSEC_H, nsec_h, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }

    /* busy watting */
    do {
        if ((ret = _phy_8218b_ptpReg_get(unit, port, REG_8218B_PTP_TIME_NSEC_H, &reg_val)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
            return ret;
        }
    } while (reg_val & REG_FIELD_8218B_PTP_CMD_EXEC);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218b_ptpRefTimeAdjust_set
 * Description:
 *      Adjust the reference time of PHY of the specified port.
 * Input:
 *      unit      - unit id
 *      port    - port id
 *      sign      - significant
 *      timeStamp - reference timestamp value
 *      exec        - 0 : do not execute, 1: execute
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      sign=0 for positive adjustment, sign=1 for negative adjustment.
 */
int32
phy_8218b_ptpRefTimeAdjust_set(uint32 unit, rtk_port_t port, uint32 sign, rtk_time_timeStamp_t timeStamp, uint32 exec)
{
    int32 ret;
    uint32 sec_l, sec_h, nsec_l, nsec_h;
    uint32 reg_val;

    /* adjust Timer of PHY */
    sec_l = (timeStamp.sec) & 0xFFFF;
    sec_h = (timeStamp.sec) >> 16;
    /* convert nsec to 8nsec */
    nsec_l = (timeStamp.nsec >> 3) & 0xFFFF;
    nsec_h = ((timeStamp.nsec >> 3) >> 16) & 0x07FF;

    /* execute adjusting command, [15] = 0x1 executing, [13:12] = 0x2 inc / 0x3 dec */
    if (sign == 0)
    {
        nsec_h |= REG_FIELD_8218B_PTP_CMD_OP_ADJ_INC;
    } else {
        nsec_h |= REG_FIELD_8218B_PTP_CMD_OP_ADJ_DEC;
    }

    if(exec)
        nsec_h = nsec_h | REG_FIELD_8218B_PTP_CMD_EXEC;

    if ((ret = _phy_8218b_ptpReg_set(unit, port, REG_8218B_PTP_TIME_SEC_L, sec_l, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }
    if ((ret = _phy_8218b_ptpReg_set(unit, port, REG_8218B_PTP_TIME_SEC_H, sec_h, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }
    if ((ret = _phy_8218b_ptpReg_set(unit, port, REG_8218B_PTP_TIME_NSEC_L, nsec_l, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }
    if ((ret = _phy_8218b_ptpReg_set(unit, port, REG_8218B_PTP_TIME_NSEC_H, nsec_h, 0xFFFF)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }

    /* busy watting */
    do {
        if ((ret = _phy_8218b_ptpReg_get(unit, port, REG_8218B_PTP_TIME_NSEC_H, &reg_val)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
            return ret;
        }
    } while (reg_val & REG_FIELD_8218B_PTP_CMD_EXEC);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218b_ptpRefTimeEnable_get
 * Description:
 *      Get the enable state of reference time of PHY of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8218b_ptpRefTimeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 reg_addr, reg_val, reg_msk;

    reg_addr = REG_8218B_PTP_TIME_CFG_0;
    reg_msk = (REG_FIELD_8218B_PTP_CFG_TIMER_EN_FRC | REG_FIELD_8218B_PTP_CFG_TIMER_1588_EN);

    if ((ret = _phy_8218b_ptpReg_get(unit, port, reg_addr, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }

    if (reg_val & reg_msk)
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218b_ptpRefTimeEnable_set
 * Description:
 *      Set the enable state of reference time of PHY of the specified port.
 * Input:
 *      unit   - unit id
 *      port    - port id
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8218b_ptpRefTimeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret;
    uint32 reg_addr, reg_val, reg_msk;

    reg_addr = REG_8218B_PTP_TIME_CFG_0;
    reg_val = (enable == ENABLED)? \
        (REG_FIELD_8218B_PTP_CFG_TIMER_EN_FRC | REG_FIELD_8218B_PTP_CFG_TIMER_1588_EN) : 0;
    reg_msk = (REG_FIELD_8218B_PTP_CFG_TIMER_EN_FRC | REG_FIELD_8218B_PTP_CFG_TIMER_1588_EN);

    if ((ret = _phy_8218b_ptpReg_set(unit, port, reg_addr, reg_val, reg_msk)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218b_ptpEnable_get
 * Description:
 *      Get PTP status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8218b_ptpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 reg_addr, reg_val;

    reg_addr = REG_8218B_PTP_TIME_CTRL_PN + REG_8218B_PTP_OFFSET_PORT(port);

    if ((ret = _phy_8218b_ptpReg_get(unit, port, reg_addr, &reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }

    if (reg_val & REG_FIELD_8218B_PTP_PHY_EN)
    {
        *pEnable = ENABLED;
    } else {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218b_ptpEnable_set
 * Description:
 *      Set PTP status of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8218b_ptpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret;
    uint32 reg_addr, reg_val;

    reg_addr = REG_8218B_PTP_TIME_CTRL_PN + REG_8218B_PTP_OFFSET_PORT(port);
    reg_val = (enable == ENABLED)? REG_FIELD_8218B_PTP_PHY_EN : 0;

    if ((ret = _phy_8218b_ptpReg_set(unit, port, reg_addr, reg_val, REG_FIELD_8218B_PTP_PHY_EN)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218b_ptpRxTimestamp_get
 * Description:
 *      Get PTP Rx timstamp according to the PTP identifier on the dedicated port from the specified device.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      identifier - indentifier of PTP packet
 * Output:
 *      pTimeStamp - pointer buffer of TIME timestamp
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8218b_ptpRxTimestamp_get(uint32 unit, rtk_port_t port,
    rtk_time_ptpIdentifier_t identifier, rtk_time_timeStamp_t *pTimeStamp)
{
    int32 ret;
    uint32 reg_addr, reg_val;
    uint32 sec_l, sec_h, nsec_l, nsec_h;

    reg_addr = REG_8218B_PTP_TIME_RX_SID_PN + \
               REG_8218B_PTP_OFFSET_PORT(port) + \
               REG_8218B_PTP_OFFSET_MSGTYPE(identifier.msgType);

    if ((ret = _phy_8218b_ptpReg_get(unit, port, reg_addr, &reg_val)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Input sequence ID NEED match currently sequence ID of chip */
    if (reg_val != identifier.sequenceId)
    {
        reg_addr = REG_8218B_PTP_TIME_CTRL_PN + REG_8218B_PTP_OFFSET_PORT(port);
        reg_val = 0x1 << (4 + REG_8218B_PTP_OFFSET_MSGTYPE(identifier.msgType));
        if ((ret = _phy_8218b_ptpReg_set(unit, port, reg_addr, reg_val, reg_val)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
            return ret;
        }

        return RT_ERR_INPUT;
    }

    /* Get Rx Timestamp */
    if ((ret = _phy_8218b_ptpReg_get(unit, port, \
        REG_8218B_PTP_TIME_SEC_L_PN + REG_8218B_PTP_OFFSET_PORT(port), &sec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }
    if ((ret = _phy_8218b_ptpReg_get(unit, port, \
        REG_8218B_PTP_TIME_SEC_H_PN + REG_8218B_PTP_OFFSET_PORT(port), &sec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }
    if ((ret = _phy_8218b_ptpReg_get(unit, port, \
        REG_8218B_PTP_TIME_NSEC_L_PN + REG_8218B_PTP_OFFSET_PORT(port), &nsec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }
    if ((ret = _phy_8218b_ptpReg_get(unit, port, \
        REG_8218B_PTP_TIME_NSEC_H_PN + REG_8218B_PTP_OFFSET_PORT(port), &nsec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }

    /* Clear the pending status */
    reg_addr = REG_8218B_PTP_TIME_CTRL_PN + REG_8218B_PTP_OFFSET_PORT(port);
    reg_val = 0x1 << (4 + REG_8218B_PTP_OFFSET_MSGTYPE(identifier.msgType));
    if ((ret = _phy_8218b_ptpReg_set(unit, port, reg_addr, reg_val, reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }

    pTimeStamp->sec = (sec_h << 16) | (sec_l & 0xFFFF);
    pTimeStamp->nsec = (((nsec_h & 0x7FF) << 16) | (nsec_l & 0xFFFF)) << 3; /* convert 8nsec to nsec */

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218b_ptpTxTimestamp_get
 * Description:
 *      Get PTP Tx timstamp according to the PTP identifier on the dedicated port from the specified device.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      identifier - indentifier of PTP packet
 * Output:
 *      pTimeStamp - pointer buffer of TIME timestamp
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_8218b_ptpTxTimestamp_get(uint32 unit, rtk_port_t port,
    rtk_time_ptpIdentifier_t identifier, rtk_time_timeStamp_t *pTimeStamp)
{
    int32 ret;
    uint32 reg_addr, reg_val;
    uint32 sec_l, sec_h, nsec_l, nsec_h;

    reg_addr = REG_8218B_PTP_TIME_TX_SID_PN + \
               REG_8218B_PTP_OFFSET_PORT(port) + \
               REG_8218B_PTP_OFFSET_MSGTYPE(identifier.msgType);

    if ((ret = _phy_8218b_ptpReg_get(unit, port, reg_addr, &reg_val)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Input sequence ID NEED match currently sequence ID of chip */
    if (reg_val != identifier.sequenceId)
    {
        /* Clear the pending status */
        reg_addr = REG_8218B_PTP_TIME_CTRL_PN + REG_8218B_PTP_OFFSET_PORT(port);
        reg_val = 0x1 << (REG_8218B_PTP_OFFSET_MSGTYPE(identifier.msgType));
        if ((ret = _phy_8218b_ptpReg_set(unit, port, reg_addr, reg_val, reg_val)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
            return ret;
        }

        return RT_ERR_NOT_FINISH;
    }

    /* Get Tx Timestamp */
    if ((ret = _phy_8218b_ptpReg_get(unit, port, \
        REG_8218B_PTP_TIME_SEC_L_PN + REG_8218B_PTP_OFFSET_PORT(port), &sec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }
    if ((ret = _phy_8218b_ptpReg_get(unit, port, \
        REG_8218B_PTP_TIME_SEC_H_PN + REG_8218B_PTP_OFFSET_PORT(port), &sec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }
    if ((ret = _phy_8218b_ptpReg_get(unit, port, \
        REG_8218B_PTP_TIME_NSEC_L_PN + REG_8218B_PTP_OFFSET_PORT(port), &nsec_l)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }
    if ((ret = _phy_8218b_ptpReg_get(unit, port, \
        REG_8218B_PTP_TIME_NSEC_H_PN + REG_8218B_PTP_OFFSET_PORT(port), &nsec_h)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }

    /* Clear the pending status */
    reg_addr = REG_8218B_PTP_TIME_CTRL_PN + REG_8218B_PTP_OFFSET_PORT(port);
    reg_val = 0x1 << (REG_8218B_PTP_OFFSET_MSGTYPE(identifier.msgType));
    if ((ret = _phy_8218b_ptpReg_set(unit, port, reg_addr, reg_val, reg_val)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }

    pTimeStamp->sec = (sec_h << 16) | (sec_l & 0xFFFF);
    pTimeStamp->nsec = (((nsec_h & 0x7FF) << 16) | (nsec_l & 0xFFFF)) << 3; /* convert 8nsec to nsec */

    return RT_ERR_OK;
}


/* Function Name:
 *      _phy_8218b_tsOffloadEnable_set
 * Description:
 *      Set TX timestamp when offload occurs enable status of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
_phy_8218b_tsOffloadEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret;
    uint32 reg_addr, reg_val;

    reg_addr = REG_8218B_PTP_TIME_CFG_1;

    reg_val = (enable == ENABLED)? REG_FIELD_8218B_PTP_TS_OFFLOAD_EN : 0;

    if ((ret = _phy_8218b_ptpReg_set(unit, port, reg_addr, reg_val, REG_FIELD_8218B_PTP_TS_OFFLOAD_EN)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      _phy_8218b_intrEnable_set
 * Description:
 *      Set PTP interrup enble status of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
_phy_8218b_intrEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret;
    uint32 reg_addr, reg_val;

    reg_addr = REG_8218B_PTP_TIME_CFG_1;

    reg_val = (enable == ENABLED)? REG_FIELD_8218B_PTP_INTR_EN : 0;

    if ((ret = _phy_8218b_ptpReg_set(unit, port, reg_addr, reg_val, REG_FIELD_8218B_PTP_INTR_EN)) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

#endif  /* (defined CONFIG_SDK_RTL8218B) || (defined CONFIG_SDK_RTL8218FB) */
#if (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC)
/* Function Name:
 *      phy_8214fc_MP_masterSlave_set
 * Description:
 *      Set PHY configuration of master/slave mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      masterSlave         - PHY master slave configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_INPUT         - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8214fc_MP_masterSlave_set(uint32 unit, rtk_port_t port, rtk_port_masterSlave_t masterSlave)
{
    rtk_port_media_t    media;
    uint32              phyData0;
    uint32              page, reg;
    int32               ret;

    if ((ret = phy_8214fc_media_get(unit, port, &media)) != RT_ERR_OK)
        return ret;

    reg = PHY_INT_REG_BASE + (PHY_1000_BASET_CONTROL_REG % PHY_INT_REG_NEXT);

    /* for fiber force and auto */
    if (PORT_MEDIA_COPPER != media)
    {
        page = PHY_PAGE_1;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0003)) != RT_ERR_OK)
            goto ERR;
        if ((ret = hal_miim_read(unit, port, page, reg, &phyData0)) != RT_ERR_OK)
            goto ERR;

        phyData0 &= ~ (0x3 << 11);
        switch(masterSlave)
        {
            case PORT_AUTO_MODE:
                phyData0 |= 0x0 << 11;
                break;
            case PORT_SLAVE_MODE:
                phyData0 |= 0x2 << 11;
                break;
            case PORT_MASTER_MODE:
                phyData0 |= 0x3 << 11;
                break;
            default:
                ret = RT_ERR_INPUT;
                goto ERR;
        }

        if ((ret = hal_miim_write(unit, port, page, reg, phyData0)) != RT_ERR_OK)
            goto ERR;
    }

    /* for copper force and auto */
    if (PORT_MEDIA_FIBER != media)
    {
        page = 0xa41;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0001)) != RT_ERR_OK)
            goto ERR;
        if ((ret = hal_miim_read(unit, port, page, reg, &phyData0)) != RT_ERR_OK)
            goto ERR;

        phyData0 &= ~ (0x3 << 11);
        switch(masterSlave)
        {
            case PORT_AUTO_MODE:
                phyData0 |= 0x0 << 11;
                break;
            case PORT_SLAVE_MODE:
                phyData0 |= 0x2 << 11;
                break;
            case PORT_MASTER_MODE:
                phyData0 |= 0x3 << 11;
                break;
            default:
                ret = RT_ERR_INPUT;
                goto ERR;
        }

        if ((ret = hal_miim_write(unit, port, page, reg, phyData0)) != RT_ERR_OK)
            return ret;
    }

ERR:
    hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0000);
    hal_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, 0x0000);
    return ret;
} /* end of phy_8214fc_MP_masterSlave_set */

/* Function Name:
 *      phy_8214fc_MP_masterSlave_get
 * Description:
 *      Get PHY configuration of master/slave mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pMasterSlaveCfg     - pointer to the PHY master slave configuration
 *      pMasterSlaveActual  - pointer to the PHY master slave actual link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      This function only works on giga/ 10g port to get its master/slave mode configuration.
 */
int32
phy_8214fc_MP_masterSlave_get(uint32 unit, rtk_port_t port, rtk_port_masterSlave_t *pMasterSlaveCfg, rtk_port_masterSlave_t *pMasterSlaveActual)
{
    rtk_port_media_t    media;
    uint32              phyData0;
    uint32              page, reg;
    int32               ret;

    if ((ret = phy_8214fc_media_get(unit, port, &media)) != RT_ERR_OK)
        return ret;

    reg = PHY_INT_REG_BASE + (PHY_1000_BASET_CONTROL_REG % PHY_INT_REG_NEXT);

    /* for fiber force and auto */
    if (PORT_MEDIA_COPPER != media)
    {
        page = PHY_PAGE_1;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0003)) != RT_ERR_OK)
            goto ERR;
        if ((ret = hal_miim_read(unit, port, page, reg, &phyData0)) != RT_ERR_OK)
            goto ERR;

        phyData0 = (phyData0 >> 11) & 0x3;
        switch(phyData0)
        {
            case 0x2:
                *pMasterSlaveCfg = PORT_SLAVE_MODE;
                break;
            case 0x3:
                *pMasterSlaveCfg = PORT_MASTER_MODE;
                break;
            default:
                *pMasterSlaveCfg = PORT_AUTO_MODE;
                break;
        }

        reg = PHY_INT_REG_BASE + (PHY_1000_BASET_STATUS_REG % PHY_INT_REG_NEXT);
        if ((ret = hal_miim_read(unit, port, page, reg, &phyData0)) != RT_ERR_OK)
            goto ERR;

        phyData0 = (phyData0 >> 14) & 0x1;
        switch(phyData0)
        {
            case 0x0:
                *pMasterSlaveActual = PORT_SLAVE_MODE;
                break;
            case 0x1:
                *pMasterSlaveActual = PORT_MASTER_MODE;
                break;
            default:
                *pMasterSlaveActual = PORT_SLAVE_MODE;
                break;
         }
         goto ERR;
    }

    /* for copper force and auto */
    if (PORT_MEDIA_FIBER != media)
    {
        page = 0xa41;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0001)) != RT_ERR_OK)
            goto ERR;
        if ((ret = hal_miim_read(unit, port, page, reg, &phyData0)) != RT_ERR_OK)
            goto ERR;

        phyData0 = (phyData0 >> 11) & 0x3;
        switch(phyData0)
        {
            case 0x2:
                *pMasterSlaveCfg = PORT_SLAVE_MODE;
                break;
            case 0x3:
                *pMasterSlaveCfg = PORT_MASTER_MODE;
                break;
            default:
                *pMasterSlaveCfg = PORT_AUTO_MODE;
                break;
        }

        reg = PHY_INT_REG_BASE + (PHY_1000_BASET_STATUS_REG % PHY_INT_REG_NEXT);
        if ((ret = hal_miim_read(unit, port, page, reg, &phyData0)) != RT_ERR_OK)
            goto ERR;

        phyData0 = (phyData0 >> 14) & 0x1;
        switch(phyData0)
        {
            case 0x0:
                *pMasterSlaveActual = PORT_SLAVE_MODE;
                break;
            case 0x1:
                *pMasterSlaveActual = PORT_MASTER_MODE;
                break;
            default:
                *pMasterSlaveActual = PORT_SLAVE_MODE;
                break;
         }
    }

ERR:
    hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0000);
    hal_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, 0x0000);
    return ret;
} /* end of phy_8214fc_MP_masterSlave_get */
#endif  /* (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC) */
#ifdef CONFIG_SDK_RTL8218FB
/* Function Name:
 *      phy_8218fb_MP_masterSlave_set
 * Description:
 *      Set PHY configuration of master/slave mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      masterSlave         - PHY master slave configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_INPUT         - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8218fb_MP_masterSlave_set(uint32 unit, rtk_port_t port, rtk_port_masterSlave_t masterSlave)
{
    if (phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info))
        return phy_8214fc_MP_masterSlave_set(unit, port, masterSlave);
    else
        return phy_common_masterSlave_set(unit, port, masterSlave);

    return RT_ERR_OK;
} /* end of phy_8218fb_MP_masterSlave_set */

/* Function Name:
 *      phy_8218fb_MP_masterSlave_get
 * Description:
 *      Get PHY configuration of master/slave mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pMasterSlaveCfg     - pointer to the PHY master slave configuration
 *      pMasterSlaveActual  - pointer to the PHY master slave actual link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      This function only works on giga/ 10g port to get its master/slave mode configuration.
 */
int32
phy_8218fb_MP_masterSlave_get(uint32 unit, rtk_port_t port, rtk_port_masterSlave_t *pMasterSlaveCfg, rtk_port_masterSlave_t *pMasterSlaveActual)
{
    if (phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info))
        return phy_8214fc_MP_masterSlave_get(unit, port, pMasterSlaveCfg, pMasterSlaveActual);
    else
        return phy_common_masterSlave_get(unit, port, pMasterSlaveCfg, pMasterSlaveActual);

    return RT_ERR_OK;

} /* end of phy_8218fb_MP_masterSlave_get */
#endif  /* CONFIG_SDK_RTL8218FB */

#if (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC)
/* Function Name:
 *      phy_8214fc_fiberInternalLoopBack_get
 * Description:
 *      Get Fiber-Port Internal Loopback feature,
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pEnable             - loopback mode status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214fc_fiberInternalLoopBack_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  phyData;
    uint32  reg_val, reg_val1;
    int32    ret;

    /*Backup*/
    if ((ret = hal_miim_read(unit, port, 0, 30, &phyData)) != RT_ERR_OK)
        return ret;

    reg_val = 0x03;
    if ((ret = hal_miim_write(unit, port, 0, 30, reg_val)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 0, 16, &reg_val1)) != RT_ERR_OK)
        return ret;

    if(reg_val1 & Loopback_MASK) //bit-14
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;


    /*Restore*/
    if ((ret = hal_miim_write(unit, port, 0, 30, phyData)) != RT_ERR_OK)
        return ret;

    reg_val = 0x0;
    if ((ret = hal_miim_write(unit, port, 0, 30, reg_val)) != RT_ERR_OK)
        return ret;


    return ret;
}   /* end of phy_8214fc_fiberInternalLoopBack_get */

/* Function Name:
 *      phy_8214fc_fiberInternalLoopBack_set
 * Description:
 *      Set Fiber-Port Internal Loopback feature,
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      enable - Fiber-Port Internal Loopback feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214fc_fiberInternalLoopBack_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    uint32  phyData;
    uint32  reg_val, reg_val1;
    int32    ret;

    /*Enable Loopback*/
    /*Backup*/
    if ((ret = hal_miim_read(unit, port, 0, 30, &phyData)) != RT_ERR_OK)
        return ret;

    reg_val = 0x03;
    if ((ret = hal_miim_write(unit, port, 0, 30, reg_val)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 8, 16, &reg_val)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 0, 16, &reg_val1)) != RT_ERR_OK)
        return ret;


    if(enable == ENABLED)
    {
        reg_val &= ~(1UL<<8);
        reg_val1 |= (1UL<<14);

    }else{
        reg_val |= (1UL<<8);
        reg_val1 &= ~(1UL<<14);
    }

    if ((ret = hal_miim_write(unit, port, 8, 16, reg_val)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_write(unit, port, 0, 16, reg_val1)) != RT_ERR_OK)
        return ret;


    /*Restore*/
    if ((ret = hal_miim_write(unit, port, 0, 30, phyData)) != RT_ERR_OK)
        return ret;

    if(enable == ENABLED)
    {
        /*Delay until PHY Linkup*/
        osal_time_usleep(100 * 1000); /* delay 10mS */
    }

    reg_val = 0x0;
    if ((ret = hal_miim_write(unit, port, 0, 30, reg_val)) != RT_ERR_OK)
        return ret;


    return ret;
}   /* end of phy_8214fc_fiberInternalLoopBack_set */


/* Function Name:
 *      phy_8214fc_loopback_get
 * Description:
 *      Get PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pEnable             - loopback mode status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8214fc_loopback_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtk_port_media_t    media;
    uint32              link_status;
    int32               ret;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_status)) != RT_ERR_OK)
        return ret;

    if (media == PORT_MEDIA_COPPER)
    {
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0001)) != RT_ERR_OK)
            goto ERR;

        ret = phy_common_loopback_get(unit, port, pEnable);
    }
    else if (media == PORT_MEDIA_FIBER)
    {
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0003)) != RT_ERR_OK)
            goto ERR;

        ret = phy_8214fc_fiberInternalLoopBack_get(unit, port, pEnable);
    }
    else
    {
        return RT_ERR_FAILED;
    }

ERR:
    hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0000);
    return ret;
}

/* Function Name:
 *      phy_8214fc_loopback_set
 * Description:
 *      Set PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      enable              - ENABLED: Enable loopback;
 *                            DISABLED: Disable loopback. PHY back to normal operation.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8214fc_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtk_port_media_t    media;
    uint32              link_status;
    int32               ret;

    if ((ret = _phy_8218b_14fc_intMedia_get(unit, port, &media, &link_status)) != RT_ERR_OK)
        return ret;

    if (media == PORT_MEDIA_COPPER)
    {
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0001)) != RT_ERR_OK)
            goto ERR;

        ret = phy_common_loopback_set(unit, port, enable);
    }
    else if (media == PORT_MEDIA_FIBER)
    {
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0003)) != RT_ERR_OK)
            goto ERR;

        ret = phy_8214fc_fiberInternalLoopBack_set(unit, port, enable);
    }
    else
    {
        return RT_ERR_FAILED;
    }

ERR:
    hal_miim_write(unit, port, PHY_PAGE_0, 29, 0x0000);
    return ret;
}
#endif  /* (defined CONFIG_SDK_RTL8218FB) || (defined CONFIG_SDK_RTL8214FC) */

#ifdef CONFIG_SDK_RTL8218FB
/* Function Name:
 *      phy_8218fb_loopback_get
 * Description:
 *      Get PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pEnable             - loopback mode status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8218fb_loopback_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32               ret;

    if (!phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info))
    {
        ret = phy_common_loopback_get(unit, port, pEnable);
        return ret;
    }
    else
    {
        ret = phy_8214fc_loopback_get(unit, port, pEnable);
        return ret;
    }
}

/* Function Name:
 *      phy_8218fb_loopback_set
 * Description:
 *      Set PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      enable              - ENABLED: Enable loopback;
 *                            DISABLED: Disable loopback. PHY back to normal operation.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8218fb_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32               ret;

    if (!phy_8218b_port_is_combo_phy(unit, port, &phy_8218FB_info))
    {
        ret = phy_common_loopback_set(unit, port, enable);
        return ret;
    }
    else
    {
        ret = phy_8214fc_loopback_set(unit, port, enable);
        return ret;
    }
}
#endif  /* CONFIG_SDK_RTL8218FB */
#if (defined CONFIG_SDK_RTL8214FC) || (defined CONFIG_SDK_RTL8218FB)
/* Function Name:
 *      _phy_8214fc_fiberTxDis_set
 * Description:
 *      Set 8214FC & 8218FB Tx disable signal of the specific port
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      phyPortNum  - number of port in PHY
 *      enable      - ENABLED: Enable Tx disable signal;
 *                    DISABLED: Disable Tx disable signal.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_8214fc_fiberTxDis_set(uint32 unit, rtk_port_t port, uint32 phyPortNum, rtk_enable_t enable)
{
    rtk_port_t  basePort;
    uint32      val;
    int32       ret;

    basePort= port - (port % phyPortNum);

    if ((ret = hal_miim_write(unit, basePort, PHY_PAGE_0, 30, 0x0008)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, basePort, 0x261, 0x13, &val)) != RT_ERR_OK)
        goto ERR;

    if (ENABLED == enable)
    {
        val |= (1 << (10 + (port % PORT_NUM_IN_8214FC)));
    }
    else
    {
        val &= ~(1 << (10 + (port % PORT_NUM_IN_8214FC)));
    }

    if ((ret = hal_miim_write(unit, basePort, 0x261, 0x13, val)) != RT_ERR_OK)
        goto ERR;

ERR:
    hal_miim_write(unit, basePort, PHY_PAGE_0, 30, 0x0000);

    return ret;
}

/* Function Name:
 *      phy_8214fc_fiberTxDis_set
 * Description:
 *      Set PHY Tx disable signal of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - ENABLED: Enable Tx disable signal;
 *                DISABLED: Disable Tx disable signal.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8214fc_fiberTxDis_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    return _phy_8214fc_fiberTxDis_set(unit, port, PORT_NUM_IN_8214FC, enable);
}

/* Function Name:
 *      _phy_8214fc_fiberTxDisPin_set
 * Description:
 *      Set 8214FC & 8218FB Tx disable signal GPO output
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      phyPortNum  - number of port in PHY
 *      data      - GPO pin value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_8214fc_fiberTxDisPin_set(uint32 unit, rtk_port_t port, uint32 phyPortNum, uint32 data)
{
    rtk_port_t  basePort;
    uint32      val;
    int32       ret;

    basePort= port - (port % phyPortNum);

    if ((ret = hal_miim_write(unit, basePort, PHY_PAGE_0, 30, 0x0008)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, basePort, 0x262, 0x13, &val)) != RT_ERR_OK)
        goto ERR;

    if (0 != data)
    {
        val |= (1 << (10 + (port % PORT_NUM_IN_8214FC)));
    }
    else
    {
        val &= ~(1 << (10 + (port % PORT_NUM_IN_8214FC)));
    }

    if ((ret = hal_miim_write(unit, basePort, 0x262, 0x13, val)) != RT_ERR_OK)
        goto ERR;

ERR:
    hal_miim_write(unit, basePort, PHY_PAGE_0, 30, 0x0000);

    return ret;
}

/* Function Name:
 *      phy_8214fc_fiberTxDisPin_set
 * Description:
 *      Set 8214FC & 8218FB Tx disable signal GPO output
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      data   - GPO pin value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8214fc_fiberTxDisPin_set(uint32 unit, rtk_port_t port, uint32 data)
{
    return _phy_8214fc_fiberTxDisPin_set(unit, port, PORT_NUM_IN_8214FC, data);
}

/* Function Name:
 *      phy_8214fc_fiberRx_check
 * Description:
 *      Check Fiber Rx status
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pStatus -Fiber RX status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8214fc_fiberRx_check(uint32 unit, rtk_port_t port, uint32 *pStatus)
{
    uint32  backup_reg30_data;
    uint32  error_count, chk_loop, reg_data, chk_error;

    chk_error = 0;

    *pStatus = PHY_FIBER_RX_STATUS_0;

     /*Backup Page 0 Reg 30*/
     hal_miim_read(unit, port, 0, 30, &backup_reg30_data);

     /*Write page 0 reg 30 = 0x3*/
     hal_miim_write(unit, port, 0, 30, 0x3);

     /*Write page 0xf reg 0x10 = 0x10*/
     hal_miim_write(unit, port, 0xf, 0x10, 0x10);

     /*Check Error counter three times*/
     for(chk_loop = 0; chk_loop < 3; chk_loop++)
     {
         /*Read Error counter*/
         hal_miim_read(unit, port, 0xf, 0x11, &error_count);
         if(error_count == 0xffff)
             chk_error++;
     }

     /*Error happen*/
     if(chk_error > 2)
     {
        *pStatus = PHY_FIBER_RX_STATUS_1;
     }
     else
     {
        chk_error = 0;

        for(chk_loop = 0; chk_loop < 3; ++chk_loop)
        {
            hal_miim_read(unit, port, 0xf, 0x16, &reg_data);
            if (0x100 == (reg_data & 0x100))
            {
                if (0x10 != (reg_data & 0x10))
                {
                    ++chk_error;
                }
            }
        }

        if (3 == chk_error)
        {
            *pStatus = PHY_FIBER_RX_STATUS_2;
        }
    }

     /*Restore Page 0 Reg 30*/
     hal_miim_write(unit, port, 0, 30, backup_reg30_data);

    return RT_ERR_OK;
}



/* Function Name:
 *      phy_8214fc_fiberRx_reset
 * Description:
 *      Reset Fiber Rx part
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8214fc_fiberRx_reset(uint32 unit, rtk_port_t port)
{
    uint32  backup_reg30_data, reg_data;

    /*Backup Page 0 Reg 30*/
    hal_miim_read(unit, port, 0, 30, &backup_reg30_data);

    hal_miim_write(unit, port, 0, 30, 0x3); /*Write page 0 reg 30 = 0x3*/

    hal_miim_read(unit, port, 0xc, 0x16, &reg_data);
    reg_data |= (1 << 15);
    hal_miim_write(unit, port, 0xc, 0x16, reg_data);
    osal_time_usleep(100 * 1000);
    hal_miim_read(unit, port, 0xc, 0x16, &reg_data);
    reg_data &= ~(1 << 15);
    hal_miim_write(unit, port, 0xc, 0x16, reg_data);

    /*Restore Page 0 Reg 30*/
    hal_miim_write(unit, port, 0, 30, backup_reg30_data);

    return RT_ERR_OK;
}

#endif  /* (defined CONFIG_SDK_RTL8214FC) || (defined CONFIG_SDK_RTL8218FB) */
#ifdef CONFIG_SDK_RTL8218FB
/* Function Name:
 *      phy_8218fb_fiberTxDis_set
 * Description:
 *      Set PHY Tx disable signal of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - ENABLED: Enable Tx disable signal;
 *                DISABLED: Disable Tx disable signal.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8218fb_fiberTxDis_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    RT_PARAM_CHK((port % PORT_NUM_IN_8218FB) < 4, RT_ERR_CHIP_NOT_SUPPORTED);

    return _phy_8214fc_fiberTxDis_set(unit, port, PORT_NUM_IN_8218FB, enable);
}

/* Function Name:
 *      phy_8218fb_fiberTxDisPin_set
 * Description:
 *      Set PHY Tx disable signal of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      data   - GPO pin value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8218fb_fiberTxDisPin_set(uint32 unit, rtk_port_t port, uint32 data)
{
    RT_PARAM_CHK((port % PORT_NUM_IN_8218FB) < 4, RT_ERR_CHIP_NOT_SUPPORTED);

    return _phy_8214fc_fiberTxDisPin_set(unit, port, PORT_NUM_IN_8218FB, data);
}

/* Function Name:
 *      phy_8218fb_fiberRx_check
 * Description:
 *      Check Fiber Rx status
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pStatus -Fiber RX status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8218fb_fiberRx_check(uint32 unit, rtk_port_t port, uint32 *pStatus)
{
    RT_PARAM_CHK((port % PORT_NUM_IN_8218FB) < 4, RT_ERR_CHIP_NOT_SUPPORTED);

    return phy_8214fc_fiberRx_check(unit, port, pStatus);

}

/* Function Name:
 *      phy_8218fb_fiberRx_reset
 * Description:
 *      Reset Fiber Rx part
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8218fb_fiberRx_reset(uint32 unit, rtk_port_t port)
{
    RT_PARAM_CHK((port % PORT_NUM_IN_8218FB) < 4, RT_ERR_CHIP_NOT_SUPPORTED);

    return phy_8214fc_fiberRx_reset(unit, port);
}
#endif  /* CONFIG_SDK_RTL8218FB */


/* Function Name:
 *      _phy_8214fc_fiberRxEnable_get
 * Description:
 *      Get 8214fc and 8218fb fiber Rx enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - fiber Rx enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
_phy_8214fc_fiberRxEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    uint32  val;
    int32   ret;

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 30, 0x3)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 0xc, 0x16, &val)) != RT_ERR_OK)
        goto ERR;

    if (val & (1 << 15))
    {
        *pEnable = DISABLED;
    }
    else
    {
        *pEnable = ENABLED;
    }

ERR:
    hal_miim_write(unit, port, PHY_PAGE_0, 30, 0x0);

    return ret;
}   /* end of _phy_8214fc_fiberRxEnable_get */

/* Function Name:
 *      _phy_8214fc_fiberRxEnable_set
 * Description:
 *      Set 8214fc and 8218fb fiber Rx enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - fiber Rx enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
_phy_8214fc_fiberRxEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    uint32  val;
    int32   ret;

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 30, 0x3)) != RT_ERR_OK)
        return ret;

    if (ENABLED == enable)
    {
        if ((ret = hal_miim_read(unit, port, 0xc, 0x16, &val)) != RT_ERR_OK)
            goto ERR;

        val &= ~(1 << 15);

        if ((ret = hal_miim_write(unit, port, 0xc, 0x16, val)) != RT_ERR_OK)
            goto ERR;
    }
    else
    {
        if ((ret = hal_miim_read(unit, port, 0xc, 0x16, &val)) != RT_ERR_OK)
            goto ERR;

        val |= (1 << 15);

        if ((ret = hal_miim_write(unit, port, 0xc, 0x16, val)) != RT_ERR_OK)
            goto ERR;

        if ((ret = hal_miim_read(unit, port, 0x8, 0x13, &val)) != RT_ERR_OK)
            goto ERR;

        val |= (1 << 6);

        if ((ret = hal_miim_write(unit, port, 0x8, 0x13, val)) != RT_ERR_OK)
            goto ERR;

        val &= ~(1 << 6);

        if ((ret = hal_miim_write(unit, port, 0x8, 0x13, val)) != RT_ERR_OK)
            goto ERR;
    }

ERR:
    hal_miim_write(unit, port, PHY_PAGE_0, 30, 0x0);

    return ret;
}   /* end of _phy_8214fc_fiberRxEnable_set */

/* Function Name:
 *      phy_8214fc_fiberRxEnable_get
 * Description:
 *      Get fiber Rx enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - fiber Rx enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
phy_8214fc_fiberRxEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    return _phy_8214fc_fiberRxEnable_get(unit, port, pEnable);
}   /* end of phy_8214fc_fiberRxEnable_get */

/* Function Name:
 *      phy_8214fc_fiberRxEnable_set
 * Description:
 *      Set fiber Rx enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - fiber Rx enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
phy_8214fc_fiberRxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    return _phy_8214fc_fiberRxEnable_set(unit, port, enable);
}   /* end of phy_8214fc_fiberRxEnable_set */

/* Function Name:
 *      phy_8218fb_fiberRxEnable_get
 * Description:
 *      Get fiber Rx enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - fiber Rx enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
phy_8218fb_fiberRxEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((port % PORT_NUM_IN_8218FB) < 4, RT_ERR_CHIP_NOT_SUPPORTED);

    return _phy_8214fc_fiberRxEnable_get(unit, port, pEnable);
}   /* end of phy_8218fb_fiberRxEnable_get */

/* Function Name:
 *      phy_8218fb_fiberRxEnable_set
 * Description:
 *      Set fiber Rx enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - fiber Rx enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
phy_8218fb_fiberRxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    RT_PARAM_CHK((port % PORT_NUM_IN_8218FB) < 4, RT_ERR_CHIP_NOT_SUPPORTED);

    return _phy_8214fc_fiberRxEnable_set(unit, port, enable);
}   /* end of phy_8218fb_fiberRxEnable_set */


#if defined(CONFIG_SDK_RTL8214FC) || defined(CONFIG_SDK_RTL8218FB)
/* Function Name:
 *      phy_8214fc_fiberLinkUp_handler1
 * Description:
 *      Handle fiber linkup
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 * Note:
 *      None
 */
int32 phy_8214fc_fiberLinkUp_handler1(uint32 unit, rtk_port_t port)
{
    uint32  val;

    hal_miim_write(unit, port, PHY_PAGE_0, 30, 3);
    hal_miim_read(unit, port, 0x8, 0x13, &val);
    val |= (1 << 6);
    hal_miim_write(unit, port, 0x8, 0x13, val);
    val &= ~(1 << 6);
    hal_miim_write(unit, port, 0x8, 0x13, val);
    hal_miim_write(unit, port, PHY_PAGE_0, 30, 0);

    return RT_ERR_OK;
}   /* end of phy_8214fc_fiberLinkUp_handler1 */
#endif  /* defined(CONFIG_SDK_RTL8214FC) || defined(CONFIG_SDK_RTL8218FB) */
#endif /* !defined(__BOOTLOADER__) */

#if defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8218FB) || defined(CONFIG_SDK_RTL8214FC)
void
_phy_8218b_ieeeTestModeAllPhyPort_disable(uint32 unit, rtk_port_t phyBasePort, uint32 phyPortCnt)
{
    uint32      phyPort;

    for (phyPort = 0; phyPort < phyPortCnt; phyPort++)
    {
        hal_miim_write(unit, phyBasePort+phyPort,0,9,0x0E00);  // Disable Test mode
    }
}


/* Function Name:
 *      phy_8218b_ieeeTestMode_set
 * Description:
 *      Set test mode for Giga PHY transmitter test
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      pTestMode->mode  - test mode 1 ~ 4 which is defined in IEEE 40.6.1.1.2
 *      pTestMode->channel  - Channel A, B, C, D, or none
 *      pTestMode->flags -
 *          RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS -
 *              apply the test on all ports of the PHY.
 *              To use this feature, the "port" parameter shall set to the first port of the PHY.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_NOT_ALLOWED - The operation is not allowed
 *      RT_ERR_PORT_NOT_SUPPORTED - test mode is not supported
 * Note:
 *      RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS only support in mode 4
 */
int32
phy_8218b_ieeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode)
{
    uint32      phyBasePort = HWP_PHY_BASE_MACID(unit, port);
    uint32      phyid = port - phyBasePort, phyPortCnt = 0;
    uint32      val, chn;

    osal_printf("RTL8218B/14FC/18FB Test Mode (PHYID: %u)\n", port);

    if ((pTestMode->flags & RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS) && (pTestMode->mode != 4))
    {
        osal_printf("mode %u not support all-phy-port operation.\n", pTestMode->mode);
        return RT_ERR_NOT_ALLOWED;
    }

    switch (HWP_PHY_MODEL_BY_PORT(unit, port))
    {
      case RTK_PHYTYPE_RTL8208L_INT:
      case RTK_PHYTYPE_RTL8218B:
      case RTK_PHYTYPE_RTL8218FB:
        phyPortCnt = PORT_NUM_IN_8218B;
        break;
      case RTK_PHYTYPE_RTL8214FC:
        phyPortCnt = PORT_NUM_IN_8214FC;
        break;
      default:
        osal_printf("port type not support.\n");
        return RT_ERR_PORT_NOT_SUPPORTED;
    }

    switch (pTestMode->mode)
    {
      case 1:
        {
            _phy_8218b_ieeeTestModeAllPhyPort_disable(unit, phyBasePort, phyPortCnt);

            hal_miim_write(unit, port,0xbc0,18,0x0000);
            hal_miim_write(unit, port,0xbc0,19,0x01c0);
            hal_miim_write(unit, port,0,9,0x2E00);  // Test mode 1
            return RT_ERR_OK;
        }
        break;

      case 2:
        {
            _phy_8218b_ieeeTestModeAllPhyPort_disable(unit, phyBasePort, phyPortCnt);

            hal_miim_write(unit, port,0xbc0,18,0x0000);
            hal_miim_write(unit, port,0xbc0,19,0x01c0);
            hal_miim_write(unit, port,0,9,0x4E00);  // Test mode 2
            return RT_ERR_OK;
        }
        break;

    case 3:
        {
            //0=A,1=B,2=C,3=D
            chn = pTestMode->channel - PHY_TESTMODE_CHANNEL_A;
            if (chn > 3)
            {
                osal_printf(" Error test channel: %u\n", chn);
                return RT_ERR_INPUT;
            }

            osal_printf(" test channel: %u\n", chn);

            if (phyid < 4)
            {
                hal_miim_write(unit, phyBasePort+1,0xbc0,18,0x0000);
                hal_miim_write(unit, phyBasePort+1,0xbc0,19,0x01c0);
                hal_miim_write(unit, phyBasePort+1, 2, 17, 0x4000 | ((phyid & 0x7) << 9));
                hal_miim_write(unit, phyBasePort+1, 2, 16, 0x1100 | ((chn & 0x3) << 9));
            } else {
                hal_miim_write(unit, phyBasePort+4,0xbc0,18,0x0000);
                hal_miim_write(unit, phyBasePort+4,0xbc0,19,0x01c0);
                hal_miim_write(unit, phyBasePort+4, 2, 17, 0x4000 | ((phyid & 0x7) << 9));
                hal_miim_write(unit, phyBasePort+4, 2, 16, 0x1100 | ((chn & 0x3) << 9));
            }


            _phy_8218b_ieeeTestModeAllPhyPort_disable(unit, phyBasePort, phyPortCnt);

            hal_miim_write(unit, port,0xbc0,18,0x0000);
            hal_miim_write(unit, port,0xbc0,19,0x01c0);
            hal_miim_write(unit, port,0,9,0x6E00);  // Test mode 3
            return RT_ERR_OK;
        }
        break;

    case 4:
        {
            uint32      phyPort;

            //0=A,1=B,2=C,3=D
            chn = pTestMode->channel - PHY_TESTMODE_CHANNEL_A;
            if (chn > 3)
            {
                osal_printf(" Error test channel: %u\n", chn);
                return RT_ERR_INPUT;
            }

            osal_printf(" test channel: %u\n", chn);

            if (!(pTestMode->flags & RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS))
            {
                /* single port test */
                for (phyPort = 0; phyPort < phyPortCnt; phyPort++)
                {
                    hal_miim_write(unit, phyBasePort+phyPort,0xbc0,18,0x0000);
                    hal_miim_write(unit, phyBasePort+phyPort,0xbc0,19,0x01c0);
                    hal_miim_write(unit, phyBasePort+phyPort,0,9,0x0E00);  // Disable Test mode
                }

                hal_miim_write(unit, port,0,9,0x8E00); // PHYReg wi 0 9 0x8E00, PHY0 Test mode 4
                val = 0x11 << chn;
                hal_miim_write(unit, port,0xbc0,18, val);
                hal_miim_write(unit, port,0xbc0,19,0x0150);

                return RT_ERR_OK;
            }
            else
            {
                osal_printf(" port: %u ~ %u\n", phyBasePort, (phyBasePort+phyPortCnt-1));
                for (phyPort = 0; phyPort < phyPortCnt; phyPort++)
                {
                    hal_miim_write(unit, phyBasePort+phyPort,0,9,0x8E00); // PHYReg wi 0 9 0x8E00, PHY0 Test mode 4
                    val = 0x11 << chn;
                    hal_miim_write(unit, phyBasePort+phyPort,0xbc0,18, val);
                    hal_miim_write(unit, phyBasePort+phyPort,0xbc0,19,0x0150);
                }
                return RT_ERR_OK;
            }
        }
        break;

    default:
        osal_printf("The mode (%u) is not suppoted.\n", pTestMode->mode);
        return RT_ERR_PORT_NOT_SUPPORTED;
    }

}
#endif
#ifdef CONFIG_SDK_RTL8214FC
/* Function Name:
 *      phy_8214fc_serdes_linkdown_chk
 * Description:
 *      check phy serdes linkdown status
 * Input:
 *      unit   - unit id
 *      port   - base mac id
 *      uint32 * status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32
phy_8214fc_serdes_linkdown_chk(uint32 unit, rtk_port_t port, uint32 * status)
{
    uint32  phy_data0, phy_data1,phy_data2;

    hal_miim_write(unit, port, 0, 30, 8);
    hal_miim_write(unit, port, 0x404, 0x10, 0x9703);
    hal_miim_write(unit, port, 0, 30, 0);

    hal_miim_write(unit, port, 0, 30, 8);
    hal_miim_read(unit, port, 0x40f, 0x16, &phy_data0);
    hal_miim_write(unit, port, 0, 30, 0);
    hal_miim_write(unit, port, 0, 30, 8);
    hal_miim_read(unit, port, 0x40f, 0x16, &phy_data1);
    hal_miim_write(unit, port, 0, 30, 0);
    hal_miim_write(unit, port, 0, 30, 8);
    hal_miim_read(unit, port, 0x40f, 0x16, &phy_data2);
    hal_miim_write(unit, port, 0, 30, 0);

    if((0x1ff == (phy_data1 & 0x1ff)) && (0x1ff == (phy_data2 & 0x1ff)))
    {
      *status = 0x0;
    }
    else
    {
      *status = 0x1;      /*phy serdes had been linkdown*/
    }
    return RT_ERR_OK;
}
/* Function Name:
 *      phy_8214fc_serdes_reset
 * Description:
 *      reset phy serdes
 * Input:
 *      unit   - unit id
 *      port   - base mac id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32
phy_8214fc_serdes_reset(uint32 unit, rtk_port_t port)
{
    hal_miim_write(unit, port, 0, 30, 8);
    hal_miim_write(unit, port, 0x404, 0x10, 0x9700);
    hal_miim_write(unit, port, 0x404, 0x10, 0x9703);
    hal_miim_write(unit, port, 0, 30, 0);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8214fc_sdsEyeParam_get
 * Description:
 *      Get SerDes eye parameters
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 *      sdsId   - SerDes ID of the PHY
 * Output:
 *      pEyeParam - eye parameter.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Only support main amp
 */
int32
phy_8214fc_sdsEyeParam_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam)
{
    int32   ret;
    uint32  data, blk;

    RT_PARAM_CHK(NULL == pEyeParam, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((port != HWP_PHY_BASE_MACID(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((sdsId != 0), RT_ERR_INPUT);

    RT_ERR_CHK(hal_miim_read(unit, port, 0, 30, &blk), ret);

    osal_memset(pEyeParam, 0, sizeof(rtk_sds_eyeParam_t));

    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 8), ret);

    /* main amp */
    hal_miim_read(unit, port, 0x465, 0x13, &data);
    pEyeParam->main_amp = data & 0x7FFF;
    hal_miim_write(unit, port, 0, 30, blk);

    pEyeParam->main_en = ENABLED;

    return RT_ERR_OK;
}   /* end of phy_8214fc_sdsEyeParam_get */

/* Function Name:
 *      phy_8214fc_sdsEyeParam_set
 * Description:
 *      Set SerDes eye parameters
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 *      sdsId   - SerDes ID of the PHY
 *      pEyeParam - eye parameter.
 *                  impedance is not supported for configure.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Only support main amp
 */
int32
phy_8214fc_sdsEyeParam_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam)
{
    int32   ret;
    uint32  blk;

    RT_PARAM_CHK(NULL == pEyeParam, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((port != HWP_PHY_BASE_MACID(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((sdsId != 0), RT_ERR_INPUT);
    RT_PARAM_CHK((ENABLED != pEyeParam->main_en), RT_ERR_INPUT);

    RT_ERR_CHK(hal_miim_read(unit, port, 0, 30, &blk), ret);

    RT_ERR_CHK(hal_miim_write(unit, port, 0, 30, 8), ret);
    /* main amp */
    phy_patchBit_set(unit, port, 0x465, 0x13, 14, 0, pEyeParam->main_amp);
    hal_miim_write(unit, port, 0, 30, blk);

    return RT_ERR_OK;
}   /* end of phy_8214fc_sdsEyeParam_set */


/* Function Name:
 *      _phy_8214fc_serialLedModePageReg_get
 * Description:
 *      Get page and register number of a specific LED mode register
 * Input:
 *      unit - unit id
 *      port - base port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
_phy_8214fc_serialLedModePageReg_get(uint32 led_id, uint32 *pPage, uint32 *pReg)
{
    uint32 ledModeReg_00_05[] = {18, 19, 20, 21, 22, 23 }; /* LED 00~05 */
    uint32 ledModeReg[] = { 16, 17, 18, 19, 20, 21, 22, 23 }; /* LED 00~06, 14~21, 22~29, 30~35 */

    if (led_id >= PHY_8214FC_MAX_LED)
    {
        return RT_ERR_INPUT;
    }

    if (led_id >= 30)
    {
        *pPage = 0x285;
        *pReg = ledModeReg[(led_id - 30)];
    }
    else if (led_id >= 22)
    {
        *pPage = 0x284;
        *pReg = ledModeReg[(led_id - 22)];
    }
    else if (led_id >= 14)
    {
        *pPage = 0x283;
        *pReg = ledModeReg[(led_id - 14)];
    }
    else if (led_id >= 6)
    {
        *pPage = 0x282;
        *pReg = ledModeReg[(led_id - 6)];
    }
    else
    {
        *pPage = 0x281;
        *pReg = ledModeReg_00_05[led_id];
    }
    return RT_ERR_OK;
}

int32
_phy_8214fc_ledModeData_set(rtk_phy_ledMode_t *pLedMode, uint32 *pData)
{
    uint32  val;

    if (pLedMode->mdi == RTK_PHY_LED_MODE_MDI_DISABLE)
    {
        val = 0xF;
    }
    else if (pLedMode->mdi <= 11)
    {
        val = pLedMode->mdi;
    }
    else
    {
        return RT_ERR_INPUT;
    }
    *pData = REG32_FIELD_SET(*pData, val, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_MDI_OFFSET, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_MDI_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_1000M_SPEED) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_1000M_SPEED_OFFSET, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_1000M_SPEED_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_100M_SPEED) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_100M_SPEED_OFFSET, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_100M_SPEED_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_10M_SPEED) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_10M_SPEED_OFFSET, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_10M_SPEED_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_1000M_ACTIVITY) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_1000M_ACT_OFFSET, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_1000M_ACT_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_100M_ACTIVITY) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_100M_ACT_OFFSET, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_100M_ACT_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_10M_ACTIVITY) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_10M_ACT_OFFSET, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_10M_ACT_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_DUPLEX) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_DUPLEX_OFFSET, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_DUPLEX_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_COLLISION) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_COLLISION_OFFSET, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_COLLISION_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_TX_ACTIVITY) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_TX_ACT_OFFSET, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_TX_ACT_MASK);

    val = (pLedMode->led_ind_status_sel & RTK_PHY_LED_IND_STATUS_SEL_RX_ACTIVITY) ? 1 : 0;
    *pData = REG32_FIELD_SET(*pData, val, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_RX_ACT_OFFSET, PHY_8214FC_SERIAL_LED_PER_LED_CTRL_RX_ACT_MASK);

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8214fc_ledMode_set
 * Description:
 *      Set LED mode for PHY control LED
 * Input:
 *      unit    - unit ID
 *      port    - port ID
 *      pLedMode - LED mode select
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      pLedMode->mdi = 0 ~ 7 specifies MDI0 ~ 7
 *      pLedMode->mdi = 8 ~ 11 specifies FX0 ~ 3
 */
int32
phy_8214fc_ledMode_set(uint32 unit, rtk_port_t port, rtk_phy_ledMode_t *pLedMode)
{
    uint32  page, reg, data, blk;
    int32   ret;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    if ((ret = _phy_8214fc_serialLedModePageReg_get(pLedMode->led_id, &page, &reg)) != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u invalid led index %u ", unit, port, pLedMode->led_id);
        return ret;
    }

    if ((pLedMode->mdi != RTK_PHY_LED_MODE_MDI_DISABLE) && (pLedMode->mdi >= PHY_8214FC_MAX_MDI))
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u invalid mdi %u ", unit, port, pLedMode->mdi);
        return RT_ERR_INPUT;
    }

    if ((ret = hal_miim_read(unit, port, 0, 30, &blk)) != RT_ERR_OK)
    {
        return ret;
    }
    hal_miim_write(unit, port, 0, 30, 8);
    hal_miim_read(unit, port, page, reg, &data);
    ret = _phy_8214fc_ledModeData_set(pLedMode, &data);
    hal_miim_write(unit, port, page, reg, data);
    hal_miim_write(unit, port, 0, 30, blk);

    return ret;

}

/* Function Name:
 *      phy_8214fc_ledCtrl_get
 * Description:
 *      Get configuration of LED for PHY control LED
 * Input:
 *      unit    - unit ID
 *      port    - port ID
 * Output:
 *      pLedCtrl - LED control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 */
int32
phy_8214fc_ledCtrl_get(uint32 unit, rtk_port_t port, rtk_phy_ledCtrl_t *pLedCtrl)
{
    int32       ret;
    uint32      data, blk, val;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    if ((ret = hal_miim_read(unit, port, 0, 30, &blk)) != RT_ERR_OK)
    {
        return ret;
    }
    hal_miim_write(unit, port, 0, 30, 8);
    hal_miim_read(unit, port, PHY_8214FC_SERIAL_LED_CONFIG_PAGE, PHY_8214FC_SERIAL_LED_CONFIG_REG, &data);
    hal_miim_write(unit, port, 0, 30, blk);

    val = REG32_FIELD_GET(data, PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_OFFSET, PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_MASK);
    switch(val)
    {
      case 0:
        pLedCtrl->blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_32MS;
        break;
      case 1:
        pLedCtrl->blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_64MS;
        break;
      case 2:
        pLedCtrl->blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_128MS;
        break;
      case 3:
        pLedCtrl->blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_256MS;
        break;
      case 4:
        pLedCtrl->blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_512MS;
        break;
      case 5:
        pLedCtrl->blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_1024MS;
        break;
      case 6:
        pLedCtrl->blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_48MS;
        break;
      case 7:
        pLedCtrl->blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_96MS;
        break;
      default:
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown blink_rate val %u ", unit, port, val);
        break;
    }

    val = REG32_FIELD_GET(data, PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_OFFSET, PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_MASK);
    switch(val)
    {
      case 0:
        pLedCtrl->burst_cycle = RTK_PHY_LED_CTRL_BURST_CYCLE_8MS;
        break;
      case 1:
        pLedCtrl->burst_cycle = RTK_PHY_LED_CTRL_BURST_CYCLE_16MS;
        break;
      case 2:
        pLedCtrl->burst_cycle = RTK_PHY_LED_CTRL_BURST_CYCLE_32MS;
        break;
      case 3:
        pLedCtrl->burst_cycle = RTK_PHY_LED_CTRL_BURST_CYCLE_64MS;
        break;
      default:
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown burst_cycle val %u ", unit, port, val);
        break;
    }

    val = REG32_FIELD_GET(data, PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_OFFSET, PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_MASK);
    switch(val)
    {
      case 0:
        pLedCtrl->clock_cycle = RTK_PHY_LED_CTRL_CLOCK_CYCLE_32NS;
        break;
      case 1:
        pLedCtrl->clock_cycle = RTK_PHY_LED_CTRL_CLOCK_CYCLE_64NS;
        break;
      case 2:
        pLedCtrl->clock_cycle = RTK_PHY_LED_CTRL_CLOCK_CYCLE_96NS;
        break;
      case 3:
        pLedCtrl->clock_cycle = RTK_PHY_LED_CTRL_CLOCK_CYCLE_192NS;
        break;
      default:
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown clock_cycle val %u ", unit, port, val);
        break;
    }

    val = REG32_FIELD_GET(data, PHY_8214FC_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_OFFSET, PHY_8214FC_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_MASK);
    switch(val)
    {
      case 0:
        pLedCtrl->active_low = RTK_PHY_LED_CTRL_ACTIVE_HIGH;
        break;
      case 1:
        pLedCtrl->active_low = RTK_PHY_LED_CTRL_ACTIVE_LOW;
        break;
      default:
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown active_low val %u ", unit, port, val);
        break;
    }

    return RT_ERR_OK;

}

/* Function Name:
 *      phy_8214fc_ledCtrl_set
 * Description:
 *      Configure LED for PHY control LED
 * Input:
 *      unit    - unit ID
 *      port    - port ID
 *      pLedCtrl - LED control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 */
int32
phy_8214fc_ledCtrl_set(uint32 unit, rtk_port_t port, rtk_phy_ledCtrl_t *pLedCtrl)
{
    int32       ret;
    uint32      data, blk, val_blink_rate, val_burst_cycle, val_clock_cycle, val_active_low;

    if (port != HWP_PHY_BASE_MACID(unit, port))
    {
        return RT_ERR_PORT_ID;
    }

    switch(pLedCtrl->blink_rate)
    {
      case RTK_PHY_LED_CTRL_BLINK_RATE_32MS:
        val_blink_rate = 0;
        break;
      case RTK_PHY_LED_CTRL_BLINK_RATE_64MS:
        val_blink_rate = 1;
        break;
      case RTK_PHY_LED_CTRL_BLINK_RATE_128MS:
        val_blink_rate = 2;
        break;
      case RTK_PHY_LED_CTRL_BLINK_RATE_256MS:
        val_blink_rate = 3;
        break;
      case RTK_PHY_LED_CTRL_BLINK_RATE_512MS:
        val_blink_rate = 4;
        break;
      case RTK_PHY_LED_CTRL_BLINK_RATE_1024MS:
        val_blink_rate = 5;
        break;
      case RTK_PHY_LED_CTRL_BLINK_RATE_48MS:
        val_blink_rate = 6;
        break;
      case RTK_PHY_LED_CTRL_BLINK_RATE_96MS:
        val_blink_rate = 7;
        break;
      default:
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown blink_rate cfg %u ", unit, port, pLedCtrl->blink_rate);
        return RT_ERR_INPUT;
    }


    switch(pLedCtrl->burst_cycle)
    {
      case RTK_PHY_LED_CTRL_BURST_CYCLE_8MS:
        val_burst_cycle = 0;
        break;
      case RTK_PHY_LED_CTRL_BURST_CYCLE_16MS:
        val_burst_cycle = 1;
        break;
      case RTK_PHY_LED_CTRL_BURST_CYCLE_32MS:
        val_burst_cycle = 2;
        break;
      case RTK_PHY_LED_CTRL_BURST_CYCLE_64MS:
        val_burst_cycle = 3;
        break;
      default:
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown burst_cycle cfg %u ", unit, port, pLedCtrl->burst_cycle);
        return RT_ERR_INPUT;
    }


    switch(pLedCtrl->clock_cycle)
    {
      case RTK_PHY_LED_CTRL_CLOCK_CYCLE_32NS:
        val_clock_cycle = 0;
        break;
      case RTK_PHY_LED_CTRL_CLOCK_CYCLE_64NS:
        val_clock_cycle = 1;
        break;
      case RTK_PHY_LED_CTRL_CLOCK_CYCLE_96NS:
        val_clock_cycle = 2;
        break;
      case RTK_PHY_LED_CTRL_CLOCK_CYCLE_192NS:
        val_clock_cycle = 3;
        break;
      default:
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown clock_cycle cfg %u ", unit, port, pLedCtrl->clock_cycle);
        return RT_ERR_INPUT;
    }

    switch(pLedCtrl->active_low)
    {
      case RTK_PHY_LED_CTRL_ACTIVE_HIGH:
        val_active_low = 0;
        break;
      case RTK_PHY_LED_CTRL_ACTIVE_LOW:
        val_active_low = 1;
        break;
      default:
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit %u port %u unknown active_low cfg %u ", unit, port, pLedCtrl->active_low);
        return RT_ERR_INPUT;
    }

    if ((ret = hal_miim_read(unit, port, 0, 30, &blk)) != RT_ERR_OK)
    {
        return ret;
    }

    hal_miim_write(unit, port, 0, 30, 8);
    hal_miim_read(unit, port, PHY_8214FC_SERIAL_LED_CONFIG_PAGE, PHY_8214FC_SERIAL_LED_CONFIG_REG, &data);
    data = REG32_FIELD_SET(data, val_blink_rate, PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_OFFSET, PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_BLINK_RATE_MASK);
    data = REG32_FIELD_SET(data, val_burst_cycle, PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_OFFSET, PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_LED_BURST_CYCLE_MASK);
    data = REG32_FIELD_SET(data, val_clock_cycle, PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_OFFSET, PHY_8214FC_SERIAL_LED_CONFIG_SERIAL_LED_CLOCK_CYCLE_MASK);
    data = REG32_FIELD_SET(data, val_active_low, PHY_8214FC_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_OFFSET, PHY_8214FC_SERIAL_LED_CONFIG_LED_SERI_ACTIVE_LOW_MASK);
    hal_miim_write(unit, port, PHY_8214FC_SERIAL_LED_CONFIG_PAGE, PHY_8214FC_SERIAL_LED_CONFIG_REG, data);
    hal_miim_write(unit, port, 0, 30, blk);

    return RT_ERR_OK;

}

/*
 * PHY Interrupts.
 */
int32
_phy_8214fc_intrANCompleteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32 data = 0;

    /* Get per PHY port AN Complete enable (page 0xa42 reg 0x12 bit 3). */
    hal_miim_read(unit, port, 0xa42, 0x12, &data);

    if (data & (0x1 << 3))
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
}

int32
_phy_8214fc_intrANCompleteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32 data = 0;
    uint32 value = 0;

    if (ENABLED == enable)
        value = 1;
    else
        value = 0;

    /* Set per PHY port AN Complete enable (page 0xa42 reg 0x12 bit 3). */
    hal_miim_read(unit, port, 0xa42, 0x12, &data);
    data = REG32_FIELD_SET(data, value, 3, (0x1 << 3));
    hal_miim_write(unit, port, 0xa42, 0x12, data);

    return RT_ERR_OK;
}

int32
_phy_8214fc_intrLinkChangeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32 data = 0;

    /* Get per PHY port Link Change enable (page 0xa42 reg 0x12 bit 4). */
    hal_miim_read(unit, port, 0xa42, 0x12, &data);

    if (data & (0x1 << 4))
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
}

int32
_phy_8214fc_intrLinkChangeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32 data = 0;
    uint32 value = 0;

    if (ENABLED == enable)
        value = 1;
    else
        value = 0;

    /* Set per PHY port Link Change enable (page 0xa42 reg 0x12 bit 4). */
    hal_miim_read(unit, port, 0xa42, 0x12, &data);
    data = REG32_FIELD_SET(data, value, 4, (0x1 << 4));
    hal_miim_write(unit, port, 0xa42, 0x12, data);

    return RT_ERR_OK;
}

int32
_phy_8214fc_intrAldpsStateChangeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32 data = 0;

    /* Get per PHY port ALDPS State Change enable (page 0xa42 reg 0x12 bit 9). */
    hal_miim_read(unit, port, 0xa42, 0x12, &data);

    if (data & (0x1 << 9))
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
}

int32
_phy_8214fc_intrAldpsStateChangeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32 data = 0;
    uint32 value = 0;

    if (ENABLED == enable)
        value = 1;
    else
        value = 0;

    /* Set per PHY port ALDPS State Change enable (page 0xa42 reg 0x12 bit 9). */
    hal_miim_read(unit, port, 0xa42, 0x12, &data);
    data = REG32_FIELD_SET(data, value, 9, (0x1 << 9));
    hal_miim_write(unit, port, 0xa42, 0x12, data);

    return RT_ERR_OK;
}

int32
_phy_8214fc_intrCommonStatus_get(uint32 unit, uint32 port, rtk_phy_intrStatusVal_t *pStatus)
{
    uint32 data = 0;
    uint32 blk = 0;

    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 1);

    pStatus->statusType = RTK_PHY_INTR_STATUS_TYPE_STATUS_BITMAP;
    pStatus->statusValue = 0;

    /*
     * Read common types of interrupts status, supporting following types,
     * a bit represents a interrupt type status of a port:
     * [3]: Auto-Negotiation Completed
     * [4]: Link Status Changed
     * [9]: ALDPS State Changes
     */
    hal_miim_read(unit, port, 0xa43, 0x15, &data);
    pStatus->statusValue |= (data & 0x8) ? (1 << RTK_PHY_INTR_STATUS_COMMON_AN_COMPLETE) : (0);
    pStatus->statusValue |= (data & 0x10) ? (1 << RTK_PHY_INTR_STATUS_COMMON_LINK_CHANGE) : (0);
    pStatus->statusValue |= (data & 0x200) ? (1 << RTK_PHY_INTR_STATUS_COMMON_ALDPS_STATE_CHANGE) : (0);

    hal_miim_write(unit, port, 0, 30, blk);
    return RT_ERR_OK;
}

int32
_phy_8214fc_intrIntpinCommonMask_get(uint32 unit, uint32 basePort, uint32 phyPort, uint32 *pMask)
{
    uint32 data = 0;

    /* Read the chip interrupt PIN IMR (page 0x267 reg 0x11). */
    hal_miim_read(unit, basePort, 0x267, 0x11, &data);
    data = REG32_FIELD_GET(data, phyPort, (0x1 << phyPort));

    if(0 == data)
        *pMask = 1; // data 0 means interrupt is enabled, set *pMask to 1
    else
        *pMask = 0;

    return RT_ERR_OK;
}

int32
_phy_8214fc_intrIntpinCommonMask_set(uint32 unit, uint32 basePort, uint32 phyPort, uint32 mask)
{
    uint32 data = 0;

    if ((mask != 0) && (mask != 1))
    {
        return RT_ERR_INPUT;
    }

    if(0 == mask)
        mask = 1;// set 1 to disable interrupt
    else
        mask = 0;

    /* Set the chip interrupt PIN IMR (page 0x267 reg 0x11). */
    hal_miim_read(unit, basePort, 0x267, 0x11, &data);
    data = REG32_FIELD_SET(data, mask, phyPort, (0x1 << phyPort));
    hal_miim_write(unit, basePort, 0x267, 0x11, data);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8214fc_intr_init
 * Description:
 *      Initialize the type of PHY interrupt function of the specified PHY chip.
 * Input:
 *      unit    - unit id
 *      port    - base mac ID number of the PHY
 *      phyIntr - PHY interrupt type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The port id is the PHY address (the corresponding MAC id of its 1st PHY port).
 */
int32
phy_8214fc_intr_init(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr)
{
    int32 ret = RT_ERR_OK;
    uint32 blk = 0;
    uint32 data = 0;

    /* Get the basePort (this PHY's phy port 0's MAC ID). */
    if (port == HWP_PHY_BASE_MACID(unit, port))
    {

        hal_miim_read(unit, port, 0xA42, 30, &blk);
        hal_miim_write(unit, port, 0xA42, 30, 8);

        hal_miim_write(unit, port, 0x267, 0x11, 0x3ff0);

        /*
         * Common part init.
         * Enable PHY Interrupt pin, bit8=1
         */
        hal_miim_read(unit, port, 0x266, 0x17, &data);
        data = REG32_FIELD_SET(data, 1, 8, (0x1 << 8));
        hal_miim_write(unit, port, 0x266, 0x17, data);

        hal_miim_write(unit, port, 0xA42, 30, blk);
    }

    hal_miim_write(unit, port, 0xA42, 30, 1);
    hal_miim_write(unit, port, 0xA42, 18, 0x0010);
    hal_miim_write(unit, port, 0xA42, 30, 0);

    hal_miim_write(unit, port, 0xA42, 30, 1);
    hal_miim_read(unit, port, 0xA43, 21, &data); //read to clear
    hal_miim_write(unit, port, 0xA42, 30, 3);
    hal_miim_read(unit, port, 0x6, 22, &data); //read to clear
    hal_miim_write(unit, port, 0xA42, 30, 0);

    return ret;
}

/* Function Name:
 *      phy_8214fc_intrEnable_get
 * Description:
 *      Get the type of PHY interrupt enable status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phyIntr - PHY interrupt type
 * Output:
 *      pEnable - pointer to status of interrupt enable
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8214fc_intrEnable_get(uint32 unit, rtk_port_t port, rtk_phy_intr_status_t phyIntr, rtk_enable_t *pEnable)
{
    int32 ret = RT_ERR_OK;
    uint32 blk = 0;

    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 1);

    switch (phyIntr)
    {
        case RTK_PHY_INTR_STATUS_COMMON_NWAY_NEXT_PAGE_RECV:
            ret = RT_ERR_PORT_NOT_SUPPORTED;
            break;
        case RTK_PHY_INTR_STATUS_COMMON_AN_COMPLETE:
            ret = _phy_8214fc_intrANCompleteEnable_get(unit, port, pEnable);
            break;
        case RTK_PHY_INTR_STATUS_COMMON_LINK_CHANGE:
            ret = _phy_8214fc_intrLinkChangeEnable_get(unit, port, pEnable);
            break;
        case RTK_PHY_INTR_STATUS_COMMON_ALDPS_STATE_CHANGE:
            ret = _phy_8214fc_intrAldpsStateChangeEnable_get(unit, port, pEnable);
            break;
        case RTK_PHY_INTR_STATUS_RLFD:
            ret = RT_ERR_PORT_NOT_SUPPORTED;
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}

/* Function Name:
 *      phy_8214fc_intrEnable_set
 * Description:
 *      Set the type of PHY interrupt enable status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phyIntr - PHY interrupt type
 *      enable  - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8214fc_intrEnable_set(uint32 unit, rtk_port_t port, rtk_phy_intr_status_t phyIntr, rtk_enable_t enable)
{
    int32 ret = RT_ERR_OK;
    uint32 blk = 0;

    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 1);

    switch (phyIntr)
    {
        case RTK_PHY_INTR_STATUS_COMMON_NWAY_NEXT_PAGE_RECV:
            ret = RT_ERR_PORT_NOT_SUPPORTED;
            break;
        case RTK_PHY_INTR_STATUS_COMMON_AN_COMPLETE:
            ret = _phy_8214fc_intrANCompleteEnable_set(unit, port, enable);
            break;
        case RTK_PHY_INTR_STATUS_COMMON_LINK_CHANGE:
            ret = _phy_8214fc_intrLinkChangeEnable_set(unit, port, enable);
            break;
        case RTK_PHY_INTR_STATUS_COMMON_ALDPS_STATE_CHANGE:
            ret = _phy_8214fc_intrAldpsStateChangeEnable_set(unit, port, enable);
            break;
        case RTK_PHY_INTR_STATUS_RLFD:
            ret = RT_ERR_PORT_NOT_SUPPORTED;
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    hal_miim_write(unit, port, 0, 30, blk);
    return ret;
}


/* Function Name:
 *      phy_8214fc_intrStatus_get
 * Description:
 *      Get specified PHY interrupt status.
 * Input:
 *      unit    - unit id
 *      port    - port id or base mac ID number of the PHY
 *      phyIntr - PHY interrupt type
 * Output:
 *      pStatus - interrupt triggered status for specified PHY interrupt
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The PHY interrupt status register is read-clear.
 */
int32
phy_8214fc_intrStatus_get(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, rtk_phy_intrStatusVal_t *pStatus)
{
    int32 ret = RT_ERR_OK;

    switch (phyIntr)
    {
        case RTK_PHY_INTR_COMMON:
            ret = _phy_8214fc_intrCommonStatus_get(unit, port, pStatus);
            break;
        case RTK_PHY_INTR_RLFD:
            ret = RT_ERR_PORT_NOT_SUPPORTED;
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    return ret;
}

/* Function Name:
 *      phy_8214fc_intrMask_get
 * Description:
 *      Get PHY interrupt mask status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phyIntr - PHY interrupt type
 * Output:
 *      pMask   - pointer to status of PHY interrupt mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8214fc_intrMask_get(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, uint32 *pMask)
{
    int32 ret = RT_ERR_OK;
    uint32 basePort = 0;
    uint32 phyPort = 0;
    uint32 blk = 0;

    /* Get the corresponding PHY chip indexed by basePort (this PHY's phy port 0's MAC ID). */
    if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

    phyPort = port - basePort;
    if (phyPort >= PORT_NUM_IN_8214FC)
    {
        return RT_ERR_PORT_ID;
    }

    hal_miim_read(unit, basePort, 0, 30, &blk);
    hal_miim_write(unit, basePort, 0, 30, 8);

    switch (phyIntr)
    {
        case RTK_PHY_INTR_COMMON:
            ret = _phy_8214fc_intrIntpinCommonMask_get(unit, basePort, phyPort, pMask);
            break;
        case RTK_PHY_INTR_RLFD:
            ret = RT_ERR_PORT_NOT_SUPPORTED;
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    hal_miim_write(unit, basePort, 0, 30, blk);
    return ret;
}

/* Function Name:
 *      phy_8214fc_intrMask_set
 * Description:
 *      Set PHY interrupt mask status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phyIntr - PHY interrupt type
 *      mask    - mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8214fc_intrMask_set(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, uint32 mask)
{
    int32 ret = RT_ERR_OK;
    uint32 basePort = 0;
    uint32 phyPort = 0;
    uint32 blk = 0;

    /* Get the corresponding PHY chip indexed by basePort (this PHY's phy port 0's MAC ID). */
    if ((basePort = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        return RT_ERR_PORT_ID;
    }

    phyPort = port - basePort;
    if (phyPort >= PORT_NUM_IN_8214FC)
    {
        return RT_ERR_PORT_ID;
    }

    hal_miim_read(unit, basePort, 0, 30, &blk);
    hal_miim_write(unit, basePort, 0, 30, 8);

    switch (phyIntr)
    {
        case RTK_PHY_INTR_COMMON:
            ret = _phy_8214fc_intrIntpinCommonMask_set(unit, basePort, phyPort, mask);
            break;
        case RTK_PHY_INTR_RLFD:
            ret = RT_ERR_PORT_NOT_SUPPORTED;
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    hal_miim_write(unit, basePort, 0, 30, blk);
    return ret;
}


#endif



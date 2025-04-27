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
 * $Revision: 76016 $
 * $Date: 2017-02-24 11:41:35 +0800 (Fri, 24 Feb 2017) $
 *
 * Purpose : Definition those waFunc APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Port
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <osal/thread.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/time.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/longan/rtk_longan_table_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/mac/serdes.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <hal/common/miim.h>
#include <hal/common/halctrl.h>
#include <dal/dal_common.h>
#include <private/drv/swcore/swcore_rtl9300.h>
#include <ioal/mem32.h>
#include <hal/phy/phy_rtl9300.h>
#include <dal/dal_waMon.h>
#include <dal/dal_phy.h>
#include <dal/longan/dal_longan_port.h>
#include <dal/longan/dal_longan_sds.h>
#include <dal/longan/dal_longan_waFunc.h>


/*Auto Recovery Debug Counter*/
extern uint32 phy_watchdog_cnt[];
extern uint32 pktBuf_watchdog_cnt[];
extern uint32 fiber_rx_watchdog_cnt[];
extern uint32 macSerdes_watchdog_cnt[];
extern uint32 phySerdes_watchdog_cnt[];
extern uint32 force_link_up[];
extern rtk_portmask_t unidir_en_portmask;

#define WA_STK_BASE_PORT                   24

/*
 * Data Declaration
 */
typedef enum rtk_auto_recover_err_e
{
    AUTO_RECOVER_ERR_NONE = 0,
    AUTO_RECOVER_ERR_SYS_DSC,
    AUTO_RECOVER_ERR_TOKEN,
    AUTO_RECOVER_ERR_RXPORT_DSC,
    AUTO_RECOVER_ERR_PINGPONG,
    AUTO_RECOVER_ERR_LD_TX_DSC,
    AUTO_RECOVER_ERR_END
} rtk_auto_recover_err_t;

/* Function Name:
 *      _dal_longan_swQueRst_set
 * Description:
 *      software queue reset
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
static int32 _dal_longan_swQueRst_set(uint32 unit)
{
    uint32 val0,val1;

    ioal_mem32_read(unit, 0x8fec, &val0);
    ioal_mem32_read(unit, 0x8ff0, &val1);

    ioal_mem32_write(unit, 0x8fec, 0xaaaaaaaa);
    ioal_mem32_write(unit, 0x8ff0, 0xaaaaaa);

    ioal_mem32_field_write(unit, RTL9300_RST_GLB_CTRL_0_ADDR, RTL9300_RST_GLB_CTRL_0_SW_Q_RST_OFFSET, RTL9300_RST_GLB_CTRL_0_SW_Q_RST_MASK, 1);
    osal_time_mdelay(50); /* delay 50mS */

    ioal_mem32_write(unit, 0x8fec, val0);
    ioal_mem32_write(unit, 0x8ff0, val1);

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_port_auto_recover_err_get
 * Description:
 *      get auto recovery type
 * Input:
 *      unit - unit id
 * Output:
 *      type - auto recovery type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
static int32 _dal_longan_port_auto_recover_err_get(uint32 unit, rtk_auto_recover_err_t *pType)
{
    uint32 errFlag;
    int32 ret;

    if ((ret = reg_field_read(unit, LONGAN_AUTO_RECOVER_EVENT_FLAG_STS_INGRESSr, LONGAN_GLB_SYS_DSC_STSf, &errFlag)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    if(1 == errFlag)
    {
        *pType = AUTO_RECOVER_ERR_SYS_DSC;
        return RT_ERR_OK;
    }

    if ((ret = reg_field_read(unit, LONGAN_AUTO_RECOVER_EVENT_FLAG_STS_INGRESSr, LONGAN_GLB_TOKEN_STSf, &errFlag)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    if(1 == errFlag)
    {
        *pType = AUTO_RECOVER_ERR_TOKEN;
        return RT_ERR_OK;
    }

    if ((ret = reg_field_read(unit, LONGAN_AUTO_RECOVER_EVENT_FLAG_STS_INGRESSr, LONGAN_GLB_RXPORT_DSC_STSf, &errFlag)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    if(1 == errFlag)
    {
        *pType = AUTO_RECOVER_ERR_RXPORT_DSC;
        return RT_ERR_OK;
    }

    if ((ret = reg_field_read(unit, LONGAN_AUTO_RECOVER_EVENT_FLAG_STS_EGRESSr, LONGAN_GLB_PINGPONG_STSf, &errFlag)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    if(1 == errFlag)
    {
        *pType = AUTO_RECOVER_ERR_PINGPONG;
        return RT_ERR_OK;
    }

    if ((ret = reg_field_read(unit, LONGAN_AUTO_RECOVER_EVENT_FLAG_STS_EGRESSr, LONGAN_GLB_LD_TX_DSC_STSf, &errFlag)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    if(1 == errFlag)
    {
        if ((ret = reg_read(unit, LONGAN_LD_TX_DSC_STSr, &errFlag)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
        errFlag = errFlag & (~unidir_en_portmask.bits[0]);
        if(errFlag != 0)
        {
            *pType = AUTO_RECOVER_ERR_LD_TX_DSC;
            return RT_ERR_OK;
        }
    }

    *pType = AUTO_RECOVER_ERR_NONE;

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_port_auto_recover_check
 * Description:
 *      system auto-recovery check
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is used to to check auto-recovery.
 */
static int32 _dal_longan_port_auto_recover_check(uint32 unit)
{
    rtk_auto_recover_err_t type;
    int32          ret;

    type = AUTO_RECOVER_ERR_NONE;

    if ((ret = _dal_longan_port_auto_recover_err_get(unit, &type)) != RT_ERR_OK)
        return ret;

    if(AUTO_RECOVER_ERR_NONE != type)
    {
        _dal_longan_swQueRst_set(unit);

       pktBuf_watchdog_cnt[unit]++;
       if(pktBuf_watchdog_cnt[unit] >= 64)
           pktBuf_watchdog_cnt[unit] = 0;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_port_counter_clr
 * Description:
 *      clear mac tx error counter
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
static int32 _dal_longan_port_counter_clr(uint32 unit, rtk_port_t port)
{
    int32    ret;
    uint32  value;

    value = 0xffffffff;
    if((ret = reg_array_write(unit, LONGAN_TXERR_CNTr, port, REG_ARRAY_INDEX_NONE, &value)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_port_counter_routine
 * Description:
 *      routine for mac tx error counter occur
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
static int32 _dal_longan_port_counter_routine(uint32 unit, rtk_port_t port)
{
    int32   ret;
    uint32 cnt;

    if((ret = reg_array_read(unit, LONGAN_TXERR_CNTr, port, REG_ARRAY_INDEX_NONE, &cnt)) != RT_ERR_OK)
        return ret;

    if(cnt > 100)
    {
        ioal_mem32_field_write(unit, RTL9300_RST_GLB_CTRL_0_ADDR, RTL9300_RST_GLB_CTRL_0_SW_Q_RST_OFFSET, RTL9300_RST_GLB_CTRL_0_SW_Q_RST_MASK, 1);
        osal_time_mdelay(50); /* delay 50mS */

        ret = _dal_longan_port_counter_clr(unit, port); /*Clear mac tx error counter*/
        if(ret != RT_ERR_OK)
            return ret;

       pktBuf_watchdog_cnt[unit]++;
       if(pktBuf_watchdog_cnt[unit] >= 64)
           pktBuf_watchdog_cnt[unit] = 0;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_port_counter_check
 * Description:
 *      mac tx error counter check
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is used to to check mac tx error counter.
 */
static uint8 link_pre_sts[RTK_MAX_NUM_OF_UNIT][28] = {{0}};
static int32 _dal_longan_port_counter_check(uint32 unit)
{
    int32          ret;
    uint32        reg_data;

    rtk_port_t  port;
    uint8          duplex;
    uint8          link_sts1;
    uint8          link_temp;

    HWP_PORT_TRAVS(unit, port)
    {
        if(!HWP_PHY_EXIST(unit, port))
            continue;

         if (HWP_IS_CPU_PORT(unit, port) || HWP_SERDES_PORT(unit, port))
            continue;

        /*Link status First Time Read*/
        if ((ret = reg_field_read(unit, LONGAN_MAC_LINK_STSr, LONGAN_LINK_STS_28_0f, &reg_data)) != RT_ERR_OK)
            return ret;


        /*Link status Second Time Read*/
        if ((ret = reg_field_read(unit, LONGAN_MAC_LINK_STSr, LONGAN_LINK_STS_28_0f, &reg_data)) != RT_ERR_OK)
            return ret;
        link_sts1 = (reg_data >> port) & 0x1;

        /*Link Duplex*/
        if ((ret = reg_field_read(unit, LONGAN_MAC_LINK_DUP_STSr, LONGAN_DUP_STS_28_0f, &reg_data)) != RT_ERR_OK)
            return ret;
        duplex = (reg_data >> port) & 0x1;

        link_temp = link_pre_sts[unit][port];
        /*Save link status*/
        link_pre_sts[unit][port] = link_sts1;

        /*Current Port Link down*/
        if(0 == link_sts1)
        {
            if(PORT_LINKUP == link_temp)
            {
                ret = _dal_longan_port_counter_clr(unit, port);
                if(ret != RT_ERR_OK)
                    return ret;
            }
        }
        else
        {
            if(PORT_FULL_DUPLEX == duplex)
            {
                if(PORT_LINKDOWN == link_temp)
                {
                    ret = _dal_longan_port_counter_clr(unit, port);
                    if(ret != RT_ERR_OK)
                        return ret;
                }
                else
                {
                    ret = _dal_longan_port_counter_routine(unit, port);
                    if(ret != RT_ERR_OK)
                        return ret;
                }
            }
        }
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_port_pktbuf_watchdog
 * Description:
 *      Monitor for packet buffer problem.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is monitor for detect packet buffer problem and recover it.
 */
int32
dal_longan_port_pktbuf_watchdog(uint32 unit)
{
    int32 ret;

    /* check Init status */
    if ((ret = dal_longan_port_init_check(unit)) != RT_ERR_OK)
        return ret;

    if ((ret = dal_longan_port_sem_lock(unit)) != RT_ERR_OK)
        return ret;

    if((ret = _dal_longan_port_auto_recover_check(unit))!= RT_ERR_OK)
    {
        if ((ret = dal_longan_port_sem_unlock(unit)) != RT_ERR_OK)
            return ret;
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if((ret = _dal_longan_port_counter_check(unit))!= RT_ERR_OK)
    {
        if ((ret = dal_longan_port_sem_unlock(unit)) != RT_ERR_OK)
            return ret;
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = dal_longan_port_sem_unlock(unit)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}/* end of dal_longan_port_pktbuf_watchdog */


/* Function Name:
 *      _dal_longan_port_serdes_linkSts_get
 * Description:
 *      Get serdes link status.
 * Input:
 *      unit - unit id
 *      sdsId - serdes id
 * Output:
 *      pLinkSts - serdes link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
static int32 _dal_longan_port_serdes_linkSts_get(uint32 unit, uint32 sdsId, uint32 *pLinkSts)
{
    uint32 reg_data0,reg_data1,reg_data2;
    int32  ret;

    reg_data0 = 0;
    reg_data1 = 0;
    reg_data2 = 0;

    if((RTL9301_CHIP_ID_24G == HWP_CHIP_ID(unit)) || (RTL9301_CHIP_ID == HWP_CHIP_ID(unit)))
    {
        if(2 == sdsId)
        {
            if((ret = hal_serdes_reg_get(unit, sdsId, 1, 30 , &reg_data0)) != RT_ERR_OK)
                return ret;

            if((ret = hal_serdes_reg_get(unit, sdsId, 1, 30 , &reg_data1)) != RT_ERR_OK)
                return ret;

            if((ret = hal_serdes_reg_get(unit, sdsId, 1, 30 , &reg_data2)) != RT_ERR_OK)
                return ret;

            if((0x1ff == (reg_data1 & 0x1ff)) && (0x1ff == (reg_data2 & 0x1ff)))
            {
                if((ret = hal_serdes_reg_get(unit, sdsId + 1, 1, 30 , &reg_data0)) != RT_ERR_OK)
                    return ret;

                if((ret = hal_serdes_reg_get(unit, sdsId + 1, 1, 30 , &reg_data1)) != RT_ERR_OK)
                    return ret;

                if((ret = hal_serdes_reg_get(unit, sdsId + 1, 1, 30 , &reg_data2)) != RT_ERR_OK)
                    return ret;
            }
        }
        else if(3 == sdsId)
        {
            if((ret = hal_serdes_reg_get(unit, 10, 1, 30 , &reg_data0)) != RT_ERR_OK)
                return ret;

            if((ret = hal_serdes_reg_get(unit, 10, 1, 30 , &reg_data1)) != RT_ERR_OK)
                return ret;

            if((ret = hal_serdes_reg_get(unit, 10, 1, 30 , &reg_data2)) != RT_ERR_OK)
                return ret;

            if((0x1ff == (reg_data1 & 0x1ff)) && (0x1ff == (reg_data2 & 0x1ff)))
            {
                if((ret = hal_serdes_reg_get(unit, 11, 1, 30 , &reg_data0)) != RT_ERR_OK)
                    return ret;

                if((ret = hal_serdes_reg_get(unit, 11, 1, 30 , &reg_data1)) != RT_ERR_OK)
                    return ret;

                if((ret = hal_serdes_reg_get(unit, 11, 1, 30 , &reg_data2)) != RT_ERR_OK)
                    return ret;
            }
        }
        else
        {
            if((ret = hal_serdes_reg_get(unit, sdsId, 1, 30 , &reg_data0)) != RT_ERR_OK)
                return ret;

            if((ret = hal_serdes_reg_get(unit, sdsId, 1, 30 , &reg_data1)) != RT_ERR_OK)
                return ret;

            if((ret = hal_serdes_reg_get(unit, sdsId, 1, 30 , &reg_data2)) != RT_ERR_OK)
                return ret;
        }
    }
    else if((RTL9303_CHIP_ID_8XG == HWP_CHIP_ID(unit)) || (RTL9303_CHIP_ID == HWP_CHIP_ID(unit)))
    {
        if((ret = hal_serdes_reg_get(unit, sdsId, 1, 30 , &reg_data0)) != RT_ERR_OK)
            return ret;

        if((ret = hal_serdes_reg_get(unit, sdsId, 1, 30 , &reg_data1)) != RT_ERR_OK)
            return ret;

        if((ret = hal_serdes_reg_get(unit, sdsId, 1, 30 , &reg_data2)) != RT_ERR_OK)
            return ret;
    }
    else
    {
        /*bypass check linkstatus*/
        *pLinkSts = 1;
        return RT_ERR_OK;
    }

    if((0x1ff == (reg_data1 & 0x1ff)) && (0x1ff == (reg_data2 & 0x1ff)))
        *pLinkSts = 1;
    else
        *pLinkSts = 0;

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_port_serdes_rxCali_check
 * Description:
 *      Monitor for serdes rx calibration.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is monitor serdes link down and recover it.
 */
static int32
_dal_longan_port_serdes_rxCali_check(uint32 unit)
{
    rtk_port_phySdsRxCaliStatus_t status;
    rt_serdesMode_t mode;
    rtk_port_t port;
    uint32 sdsId;
    uint32 is_evn_set;
    uint32 rxIdle;
    uint32 flag = 0;
    int32 ret;

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        sdsId = HWP_PORT_SDSID(unit, port);

        if((ret = dal_waMon_phyEsdRstEvn_get(unit, port, &is_evn_set))!= RT_ERR_OK)
            return ret;

        if((ret = dal_longan_sds_rxCaliStatus_get(unit, sdsId, &status))!= RT_ERR_OK)
            return ret;

        if((is_evn_set == 0) && (PHY_SDS_RXCALI_STATUS_OK == status))
            continue;

        if((RTK_PHYTYPE_SERDES == HWP_SDS_ID2PHYMODEL(unit, sdsId) && !HWP_CASCADE_PORT(unit, port)) ||
            (RTK_PHYTYPE_NONE == HWP_SDS_ID2PHYMODEL(unit, sdsId)))
            continue;

        if((ret = dal_longan_sds_mode_get(unit, sdsId, &mode))!= RT_ERR_OK)
            return ret;

        if((mode != RTK_MII_USXGMII_10GDXGMII)&&
            (mode != RTK_MII_USXGMII_10GSXGMII)&&
            (mode != RTK_MII_USXGMII_10GQXGMII)&&
            (mode != RTK_MII_QHSGMII) &&
            (mode != RTK_MII_10GR) &&
            (mode != RTK_MII_XSGMII))
            continue;

        if((RTK_PHYTYPE_RTL8218D == HWP_SDS_ID2PHYMODEL(unit, sdsId)) &&
        (HWP_SDS_MODE(unit, sdsId) == RTK_MII_XSGMII) && (sdsId == 2 || sdsId == 3) && (is_evn_set == 1))
        {
            flag = 1;
        }

        dal_longan_sds_10gRxIdle_get(unit, sdsId, &rxIdle);
        if(rxIdle == 0)
        {
            osal_time_mdelay(200);
            dal_longan_sds_rxCali(unit, sdsId, 0);
        }

        if(flag == 1)
        {
            osal_time_mdelay(50);
            dal_longan_sds_clk_routine(unit);
        }

        dal_waMon_phyEsdRstEvn_clear(unit,port);
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_stack_port_serdes_linkdown_check
 * Description:
 *      Monitor for stack port serdes link down .
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *
 */
static int32
_dal_longan_stack_port_serdes_linkdown_check(uint32 unit)
{
    uint32 port = 0;
    uint32 sdsId;
    rtk_sds_linkSts_t linkSts;

    HWP_SDS_TRAVS(unit, sdsId)
    {
        port = HWP_SDS_ID2MACID(unit, sdsId);
        if(HAL_STACK_PORT(unit, port))
        {
            osal_memset(&linkSts, 0, sizeof(rtk_sds_linkSts_t));
            dal_longan_sds_linkSts_get(unit, sdsId, &linkSts);
            dal_longan_sds_linkSts_get(unit, sdsId, &linkSts);
            dal_longan_sds_linkSts_get(unit, sdsId, &linkSts);

            if((0 == linkSts.sts)&&(force_link_up[port]))
            {
                phy_9300_linkChange_process(unit,  port, PORT_LINKDOWN);
            }
        }
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_port_serdes_linkdown_check
 * Description:
 *      Monitor for serdes link down problem.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is monitor serdes link down and recover it.
 */
static int32
_dal_longan_port_serdes_linkdown_check(uint32 unit)
{
    int32 ret;
    uint32 flag = 0;
    uint32 sdsId;
    uint32 linkSts = 1;

    HWP_SDS_TRAVS(unit, sdsId)
    {
        if((RTK_PHYTYPE_SERDES == HWP_SDS_ID2PHYMODEL(unit, sdsId)) ||
            (RTK_PHYTYPE_NONE == HWP_SDS_ID2PHYMODEL(unit, sdsId)))
            continue;

        if((RTK_PHYTYPE_RTL8295R ==HWP_SDS_ID2PHYMODEL(unit, sdsId))||(RTK_PHYTYPE_RTL8295R_C22 ==HWP_SDS_ID2PHYMODEL(unit, sdsId)))
            continue;

#if defined(CONFIG_SDK_PHY_CUST1)
        if (RTK_PHYTYPE_CUST1 == HWP_SDS_ID2PHYMODEL(unit, sdsId))
            continue;
#endif

        if((ret = _dal_longan_port_serdes_linkSts_get(unit, sdsId, &linkSts)) != RT_ERR_OK)
            return ret;


        if(0 == linkSts)
        {

           if((RTK_PHYTYPE_RTL8218D == HWP_SDS_ID2PHYMODEL(unit, sdsId)) &&
            (HWP_SDS_MODE(unit, sdsId) == RTK_MII_XSGMII) && (sdsId == 2 || sdsId == 3))
           {
                flag = 1;
           }

            hal_mac_serdes_rst(unit, sdsId);

           macSerdes_watchdog_cnt[unit]++;
           if(macSerdes_watchdog_cnt[unit] >= 64)
               macSerdes_watchdog_cnt[unit] = 0;
        }
    }

    if(flag == 1)
    {
        osal_time_mdelay(50);
        dal_longan_sds_clk_routine(unit);
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_port_serdes_watchdog
 * Description:
 *      Monitor for serdes link status.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is monitor serdes link down and recover it.
 */
int32
dal_longan_port_serdes_watchdog(uint32 unit)
{
    int32 ret;

    /* check Init status */
    if ((ret = dal_longan_port_init_check(unit)) != RT_ERR_OK)
        return ret;

    if ((ret = dal_longan_port_sem_lock(unit)) != RT_ERR_OK)
        return ret;

    if((ret = _dal_longan_port_serdes_rxCali_check(unit))!= RT_ERR_OK)
    {
        if ((ret = dal_longan_port_sem_unlock(unit)) != RT_ERR_OK)
            return ret;

        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* SS-822 */
    if((ret = _dal_longan_stack_port_serdes_linkdown_check(unit))!= RT_ERR_OK)
    {
        if ((ret = dal_longan_port_sem_unlock(unit)) != RT_ERR_OK)
            return ret;

        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if((ret = _dal_longan_port_serdes_linkdown_check(unit))!= RT_ERR_OK)
    {
        if ((ret = dal_longan_port_sem_unlock(unit)) != RT_ERR_OK)
            return ret;

        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if((ret = dal_longan_sds_linkFault_check(unit))!= RT_ERR_OK)
    {
        if ((ret = dal_longan_port_sem_unlock(unit)) != RT_ERR_OK)
            return ret;

        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = dal_longan_port_sem_unlock(unit)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}/* end of dal_longan_port_serdes_watchdog */

/* Function Name:
 *      dal_longan_port_watchdog_debug
 * Description:
 *      Set watchdog debug counter.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is used to set watchdog debug counter.
 */
int32
dal_longan_port_watchdog_debug(uint32 unit)
{
    uint32 reg_val;
    int32  ret;

    /* check Init status */
    if ((ret = dal_longan_port_init_check(unit)) != RT_ERR_OK)
        return ret;

    if ((ret = dal_longan_port_sem_lock(unit)) != RT_ERR_OK)
        return ret;

    ioal_mem32_read(unit, 0x7ae0, &reg_val);
    reg_val |= (phy_watchdog_cnt[unit] & 0x3F)<<2;
    reg_val |= (pktBuf_watchdog_cnt[unit] & 0x3F)<<8;
    reg_val |= (fiber_rx_watchdog_cnt[unit] & 0x3F)<<8;
    reg_val |= (macSerdes_watchdog_cnt[unit] & 0x3F)<<20;
    reg_val |= (phySerdes_watchdog_cnt[unit] & 0x3F)<<26;
    ioal_mem32_write(unit, 0x7ae0, reg_val);

    if ((ret = dal_longan_port_sem_unlock(unit)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}/* end of dal_longan_port_watchdog_debug */



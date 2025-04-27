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
 * $Revision: 100813 $
 * $Date: 2019-10-08 22:22:51 +0800 (周二, 08 10月 2019) $
 *
 * Purpose : mac driver service APIs in the SDK.
 *
 * Feature : mac driver service APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <common/util/rt_util_system.h>
#include <ioal/ioal_param.h>
#if defined(CONFIG_SDK_RTL8231) && defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
#include <private/drv/rtl8231/rtl8231.h>
#endif
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/allmem.h>

#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <hal/chipdef/longan/rtk_longan_table_struct.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#include <hal/mac/drv/drv.h>
#include <hal/mac/led/led_rtl9300.h>
#include <hal/mac/drv/drv_rtl9300.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/mac/mac_probe.h>
#include <soc/type.h>
#include <osal/time.h>
#include <ioal/mem32.h>
#include <hwp/hw_profile.h>
#include <dal/dal_construct.h>
#include <dal/longan/dal_longan_l3.h>


/*
 * Symbol Definition
 */
#define TABLE_TYPE_L3_HOST_ROUTE            (1)     /* Host Routing */
#define TABLE_TYPE_L3_PREFIX_ROUTE          (2)     /* Prefix Routing*/
#define TABLE_READ_RETRY_TIME               (512)

#define LONGAN_SMI_ACCESS_PHY_CTRL_1_FAIL_MASK     (0x1 << 25)

/*
 * Macro Declaration
 */
#define L3_ENTRY_IDX_TO_ADDR(_idx)          ((((_idx)/6)*8) + ((_idx)%6))
#define L3_ENTRY_ADDR_TO_IDX(_addr)         ((((_addr)/8)*6) + ((_addr)%8))


int32 _rtl9300_miim_write(uint32 unit, rtk_port_t port, uint32 page, uint32 phy_reg, uint32 data);

/* Function Name:
 *      _rtl9300_port_phyAddr_get
 * Description:
 *      Get PHY address of the specified MAC ID
 * Input:
 *      macId - mac(port) id
 * Output:
 *      pPhyAddr - PHY address
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_PORT_ID - invalid port ID
 * Note:
 *      None
 */
int32
_rtl9300_port_phyAddr_get(uint32 unit, rtk_port_t macId, uint32 *pPhyAddr)
{
    int32       ret;
    uint32      reg;
    uint32      data;
    uint32      port_field[]={
            LONGAN_PORT0_ADDRf, LONGAN_PORT1_ADDRf, LONGAN_PORT2_ADDRf, LONGAN_PORT3_ADDRf, LONGAN_PORT4_ADDRf,
            LONGAN_PORT5_ADDRf, LONGAN_PORT6_ADDRf, LONGAN_PORT7_ADDRf, LONGAN_PORT8_ADDRf, LONGAN_PORT9_ADDRf,
            LONGAN_PORT10_ADDRf,LONGAN_PORT11_ADDRf,LONGAN_PORT12_ADDRf,LONGAN_PORT13_ADDRf,LONGAN_PORT14_ADDRf,
            LONGAN_PORT15_ADDRf,LONGAN_PORT16_ADDRf,LONGAN_PORT17_ADDRf,LONGAN_PORT18_ADDRf,LONGAN_PORT19_ADDRf,
            LONGAN_PORT20_ADDRf,LONGAN_PORT21_ADDRf,LONGAN_PORT22_ADDRf,LONGAN_PORT23_ADDRf,LONGAN_PORT24_ADDRf,
            LONGAN_PORT25_ADDRf,LONGAN_PORT26_ADDRf,LONGAN_PORT27_ADDRf,
        };

    if (macId >= sizeof(port_field)/sizeof(uint32))
    {
        RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %u macId %u unable to get register field!", unit, macId);
        return RT_ERR_FAILED;
    }


    if((macId >= 0) && (macId <= 5))
    {
        reg = LONGAN_SMI_PORT0_5_ADDR_CTRLr;
    }
    else if((macId >= 6) && (macId <= 11))
    {
        reg = LONGAN_SMI_PORT6_11_ADDR_CTRLr;
    }
    else if((macId >= 12) && (macId <= 17))
    {
        reg = LONGAN_SMI_PORT12_17_ADDR_CTRLr;
    }
    else if((macId >= 18) && (macId <= 23))
    {
        reg = LONGAN_SMI_PORT18_23_ADDR_CTRLr;
    }
    else if((macId >= 24) && (macId <= 27))
    {
        reg = LONGAN_SMI_PORT24_27_ADDR_CTRLr;
    }
    else
    {
        return RT_ERR_PORT_ID;
    }

    if ((ret = reg_field_read(unit, reg, port_field[macId], &data)) != RT_ERR_OK)
    {
        return ret;
    }

    *pPhyAddr = data;
    return RT_ERR_OK;
}


/* Function Name:
 *      _rtl9300_port_phyAddr_set
 * Description:
 *      Set PHY address of the specified MAC ID
 * Input:
 *      macId - mac(port) id
 *      phyAddr - PHY address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_PORT_ID - invalid port ID
 * Note:
 *      None
 */
int32
_rtl9300_port_phyAddr_set(uint32 unit, rtk_port_t macId, uint32 phyAddr)
{
    int32       ret;
    uint32      reg;
    uint32      port_field[]={
            LONGAN_PORT0_ADDRf, LONGAN_PORT1_ADDRf, LONGAN_PORT2_ADDRf, LONGAN_PORT3_ADDRf, LONGAN_PORT4_ADDRf,
            LONGAN_PORT5_ADDRf, LONGAN_PORT6_ADDRf, LONGAN_PORT7_ADDRf, LONGAN_PORT8_ADDRf, LONGAN_PORT9_ADDRf,
            LONGAN_PORT10_ADDRf,LONGAN_PORT11_ADDRf,LONGAN_PORT12_ADDRf,LONGAN_PORT13_ADDRf,LONGAN_PORT14_ADDRf,
            LONGAN_PORT15_ADDRf,LONGAN_PORT16_ADDRf,LONGAN_PORT17_ADDRf,LONGAN_PORT18_ADDRf,LONGAN_PORT19_ADDRf,
            LONGAN_PORT20_ADDRf,LONGAN_PORT21_ADDRf,LONGAN_PORT22_ADDRf,LONGAN_PORT23_ADDRf,LONGAN_PORT24_ADDRf,
            LONGAN_PORT25_ADDRf,LONGAN_PORT26_ADDRf,LONGAN_PORT27_ADDRf,
        };

    if (macId >= sizeof(port_field)/sizeof(uint32))
    {
        RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %u macId %u unable to get register field!", unit, macId);
        return RT_ERR_FAILED;
    }

    if((macId >= 0) && (macId <= 5))
    {
        reg = LONGAN_SMI_PORT0_5_ADDR_CTRLr;
    }
    else if((macId >= 6) && (macId <= 11))
    {
        reg = LONGAN_SMI_PORT6_11_ADDR_CTRLr;
    }
    else if((macId >= 12) && (macId <= 17))
    {
        reg = LONGAN_SMI_PORT12_17_ADDR_CTRLr;
    }
    else if((macId >= 18) && (macId <= 23))
    {
        reg = LONGAN_SMI_PORT18_23_ADDR_CTRLr;
    }
    else if((macId >= 24) && (macId <= 27))
    {
        reg = LONGAN_SMI_PORT24_27_ADDR_CTRLr;
    }
    else
    {
        return RT_ERR_PORT_ID;
    }

    ret = reg_field_write(unit, reg, port_field[macId], &phyAddr);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %d macId %d SMI_PORTx_x_ADDR_CTRL set fail %d", unit, macId, ret);
    }

    return ret;
}

/* Function Name:
 *      _rtl9300_port_smiSel_set
 * Description:
 *      Set SMI Select for PHY accessing
 * Input:
 *      macId - mac(port) id
 *      smi - SMI ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_PORT_ID - invalid port ID
 * Note:
 *      None
 */
int32
_rtl9300_port_smiSel_set(uint32 unit, rtk_port_t macId, uint32 smi)
{
    int32       ret;
    uint32      reg;
    uint32      port_field[]={
            LONGAN_SMI_PORT0_POLLING_SELf, LONGAN_SMI_PORT1_POLLING_SELf, LONGAN_SMI_PORT2_POLLING_SELf, LONGAN_SMI_PORT3_POLLING_SELf, LONGAN_SMI_PORT4_POLLING_SELf,
            LONGAN_SMI_PORT5_POLLING_SELf, LONGAN_SMI_PORT6_POLLING_SELf, LONGAN_SMI_PORT7_POLLING_SELf, LONGAN_SMI_PORT8_POLLING_SELf, LONGAN_SMI_PORT9_POLLING_SELf,
            LONGAN_SMI_PORT10_POLLING_SELf,LONGAN_SMI_PORT11_POLLING_SELf,LONGAN_SMI_PORT12_POLLING_SELf,LONGAN_SMI_PORT13_POLLING_SELf,LONGAN_SMI_PORT14_POLLING_SELf,
            LONGAN_SMI_PORT15_POLLING_SELf,LONGAN_SMI_PORT16_POLLING_SELf,LONGAN_SMI_PORT17_POLLING_SELf,LONGAN_SMI_PORT18_POLLING_SELf,LONGAN_SMI_PORT19_POLLING_SELf,
            LONGAN_SMI_PORT20_POLLING_SELf,LONGAN_SMI_PORT21_POLLING_SELf,LONGAN_SMI_PORT22_POLLING_SELf,LONGAN_SMI_PORT23_POLLING_SELf,LONGAN_SMI_PORT24_POLLING_SELf,
            LONGAN_SMI_PORT25_POLLING_SELf,LONGAN_SMI_PORT26_POLLING_SELf,LONGAN_SMI_PORT27_POLLING_SELf,
        };

    if (smi > 3)
    {
        ret = RT_ERR_INPUT;
        RT_ERR(ret, (MOD_HAL), "unit %u macId %u smi %u", unit, macId, smi);
        return ret;
    }

    if((macId >= 0) && (macId <= 15))
    {
        reg = LONGAN_SMI_PORT0_15_POLLING_SELr;
    }
    else if((macId >= 16) && (macId <= 27))
    {
        reg = LONGAN_SMI_PORT16_27_POLLING_SELr;
    }
    else
    {
        return RT_ERR_PORT_ID;
    }

    ret = reg_field_write(unit, reg, port_field[macId], &smi);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %d macId %d SMI_PORTx_y_POLLING_SEL set %u fail 0x%x", unit, macId, smi, ret);
    }

    return ret;
}


/* Function Name:
 *      _rtl9300_extSmiAccCtrlRegField_set
 * Description:
 *      Set register fields of EXT_SMI_ACCESS_CTRL
 * Input:
 *      unit    - unit id
 *      phy_addr- PHY address
 *      phy_reg - PHY register
 *      data - phy data
 *      write - 1: write; 0: read
 *      cmd - 1: execute
 * Output:
 *      pBuf   - reigster data buffer
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
_rtl9300_extSmiAccCtrlRegField_set(uint32 unit, uint32 phy_addr, uint32 phy_reg, uint32 data, uint32 write, uint32 cmd, uint32 *pBuf)
{
    int32       ret;
    uint32      rw_op;

    if ((ret = reg_field_set(unit, LONGAN_EXT_SMI_ACCESS_CTRLr, LONGAN_PHYADDRf, &phy_addr, pBuf)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_set(unit, LONGAN_EXT_SMI_ACCESS_CTRLr, LONGAN_REGf, &phy_reg, pBuf)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_set(unit, LONGAN_EXT_SMI_ACCESS_CTRLr, LONGAN_DATAf, &data, pBuf)) != RT_ERR_OK)
    {
        return ret;
    }

    rw_op = (write) ? 1 : 0;
    if ((ret = reg_field_set(unit, LONGAN_EXT_SMI_ACCESS_CTRLr, LONGAN_RWOPf, &rw_op, pBuf)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_set(unit, LONGAN_EXT_SMI_ACCESS_CTRLr, LONGAN_CMDf, &cmd, pBuf)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      rtl9300_port_probe
 * Description:
 *      Probe the select port interface settings of the chip.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
rtl9300_port_probe(uint32 unit)
{
    return RT_ERR_OK;
} /* end of rtl9300_port_probe */


/* Function Name:
 *      _rtl9300_smiAddr_init
 * Description:
 *      Initialize SMI Address (PHY address) of specific port.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *
 */
int32
_rtl9300_smiAddr_init(uint32 unit)
{
    int32       ret;
    uint32      mac_id, phy_id, smi_id, val;

    /* disable MAC-poll-PHY */
    val = 0;
    if ((ret = reg_field_read(unit, LONGAN_SMI_POLL_CTRLr, LONGAN_SMI_POLL_MASK_27_0f, &val)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_HAL), "reaad smi poll mask fail 0x%x", ret);
    }
    for(mac_id = 0; mac_id <= 27; mac_id++)
    {
        val &= ~( 1 << mac_id);
    }
    if ((ret = reg_field_write(unit, LONGAN_SMI_POLL_CTRLr, LONGAN_SMI_POLL_MASK_27_0f, &val)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_HAL), "write smi poll mask fail 0x%x", ret);
    }
    osal_time_mdelay(50);


    for(mac_id = 0; mac_id <= 27; mac_id++)
    {
        phy_id = HWP_PHY_ADDR(unit,mac_id);
        if(phy_id == HWP_NONE)
            continue;

        ret = _rtl9300_port_phyAddr_set(unit, mac_id, phy_id);
        if (ret != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %u macId %u set phy addr %u fail %d", unit, mac_id, phy_id, ret);
        }

        if ((smi_id = HWP_PORT_SMI(unit, mac_id)) != HWP_NONE)
        {
            ret = _rtl9300_port_smiSel_set(unit, mac_id, smi_id);
            if (ret != RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %u macId %u set smi-sel %u fail %d", unit, mac_id, smi_id, ret);
            }
        }
    }/* end for */

    return RT_ERR_OK;
}


/* Function Name:
 *      _rtl9300_smiMacType_init
 * Description:
 *      Initialize MAC Port 27 PHY Type
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
_rtl9300_smiMacType_init(uint32 unit)
{
    int32        ret;
    uint32      macID;
    uint32      field,value;

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, macID)
    {
        if(HWP_PORT_ETH(unit,macID) == HWP_NONE)
        {
            RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %u macId %u set phy type skip", unit, macID);
            continue;
        }

        switch(macID)
        {
            case 0 ... 3:
                field = LONGAN_MAC_P3_P0_TYPEf;
                break;
            case 4 ... 7:
                field = LONGAN_MAC_P7_P4_TYPEf;
                break;
            case 8 ... 11:
                field = LONGAN_MAC_P11_P8_TYPEf;
                break;
            case 12 ... 15:
                field = LONGAN_MAC_P15_P12_TYPEf;
                break;
            case 16 ... 19:
                field = LONGAN_MAC_P19_P16_TYPEf;
                break;
            case 20 ... 23:
                field = LONGAN_MAC_P23_P20_TYPEf;
                break;
            case 24:
                field = LONGAN_MAC_P24_TYPEf;
                break;
            case 25:
                field = LONGAN_MAC_P25_TYPEf;
                break;
            case 26:
                field = LONGAN_MAC_P26_TYPEf;
                break;
            case 27:
                field = LONGAN_MAC_P27_TYPEf;
                break;
            default:
                return RT_ERR_FAILED;
        }

        switch(HWP_PORT_ETH(unit,macID))
        {
            case HWP_FE:
                value = 2;
                break;
            case HWP_GE:
                value = 3;
                break;
            case HWP_XGE:
            case HWP_5GE:
            case HWP_2_5GE:
                if(HWP_SERDES != HWP_PORT_MEDI(unit, macID))
                {
                    value = 1;
                }
                else
                {
                    value = 0;
                }
                break;
            case HWP_SXGE:
                value = 0;
                break;
            default:
                return RT_ERR_FAILED;
        }

        if ((ret = reg_field_write(unit, LONGAN_SMI_MAC_TYPE_CTRLr, field, &value)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %u macId %u set phy type %u fail %d", unit, macID, value, ret);
            return ret;
        }
    }
    return RT_ERR_OK;
}

int32 drv_reg_fieled_set(uint32 unit, uint32 reg, uint32 endBit,
    uint32 startBit, uint32 val)
{
    int32 ret;
    uint32  configVal, len, mask;
    uint32  i;

    len = endBit - startBit + 1;

    if (32 == len)
        configVal = val;
    else
    {
        mask = 0;
        for (i = startBit; i <= endBit; ++i)
            mask |= (1 << i);

        RT_ERR_CHK(ioal_mem32_read(unit, reg, &configVal), ret);
        configVal &= ~(mask);
        configVal |= (val << startBit);
    }
    //RT_LOG(LOG_EVENT, (MOD_DAL|MOD_PORT), "reg 0x%x val 0x%x",reg, configVal);
    return ioal_mem32_write(unit, reg, configVal);
}



/* Function Name:
 *      rtl9300_miim_portSmiMdxProto_set
 * Description:
 *      Configure MDC/MDIO protocol for an SMI interface
 * Input:
 *      unit - unit id
 *      port - port id
 *      proto  - protocol as Clause 22 or Clause 45
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_INPUT  - error smi id or proto
 * Note:
 *      None
 */
int32
rtl9300_miim_portSmiMdxProto_set(uint32 unit, rtk_port_t port, drv_smi_mdxProtoSel_t proto)
{
    int32       ret;
    uint32      field, val;
    uint32      smi_id;

    smi_id = HWP_PORT_SMI(unit, port);
    switch (smi_id)
    {
      case 0:
        field = LONGAN_SMI0_INTF_SELf;
        break;
      case 1:
        field = LONGAN_SMI1_INTF_SELf;
        break;
      case 2:
        field = LONGAN_SMI2_INTF_SELf;
        break;
      case 3:
        field = LONGAN_SMI3_INTF_SELf;
        break;
      case 4:
        if (proto == DRV_SMI_MDX_PROTO_C22)
        {
            return RT_ERR_OK;
        }
        else
        {
            return RT_ERR_FAILED;
        }
        break;
      default:
        ret = RT_ERR_INPUT;
        RT_ERR(ret, (MOD_HAL), "unit %u smi_id %u", unit, smi_id);
        return ret;
    }

    if (proto == DRV_SMI_MDX_PROTO_C22)
    {
        val = 0;
    }
    else if (proto == DRV_SMI_MDX_PROTO_C45)
    {
        val = 1;
    }
    else
    {
        return RT_ERR_INPUT;
    }

    ret = reg_field_write(unit, LONGAN_SMI_GLB_CTRLr, field, &val);
    if (ret != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit %u SMI%u_INTF_SELf set %u fail 0x%x", unit, smi_id, val, ret);
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      rtl9300_miim_portSmiMdxProto_get
 * Description:
 *      Configure MDC/MDIO protocol for an SMI interface
 * Input:
 *      unit - unit id
 *      port - port id
 *      proto  - protocol as Clause 22 or Clause 45
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_INPUT  - error smi id or proto
 * Note:
 *      None
 */
int32
rtl9300_miim_portSmiMdxProto_get(uint32 unit, rtk_port_t port, drv_smi_mdxProtoSel_t *pProto)
{
    int32       ret;
    uint32      field, val = 0;
    uint32      smi_id;

    smi_id = HWP_PORT_SMI(unit, port);
    switch (smi_id)
    {
      case 0:
        field = LONGAN_SMI0_INTF_SELf;
        break;
      case 1:
        field = LONGAN_SMI1_INTF_SELf;
        break;
      case 2:
        field = LONGAN_SMI2_INTF_SELf;
        break;
      case 3:
        field = LONGAN_SMI3_INTF_SELf;
        break;
      case 4:
        *pProto = DRV_SMI_MDX_PROTO_C22;
        return RT_ERR_OK;
      default:
        ret = RT_ERR_INPUT;
        RT_ERR(ret, (MOD_HAL), "unit %u smi_id %u", unit, smi_id);
        return ret;
    }

    ret = reg_field_read(unit, LONGAN_SMI_GLB_CTRLr, field, &val);
    if (ret != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit %u SMI%u_INTF_SEL set %u fail 0x%x", unit, smi_id, val, ret);
        return ret;
    }

    *pProto = (val == 0) ? DRV_SMI_MDX_PROTO_C22 : DRV_SMI_MDX_PROTO_C45;

    return RT_ERR_OK;

}


/* Function Name:
 *      rtl9300_init
 * Description:
 *      Initialize the specified settings of the chip.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
rtl9300_init(uint32 unit)
{
    int32           ret;


    {
        uint8 ledModeInitSkip = LEDMODEINITSKIP_NO;
#if !defined(__BOOTLOADER__)
        ioal_param_ledInitFlag_get(&ledModeInitSkip);
#endif
        if (LEDMODEINITSKIP_NO == ledModeInitSkip)
        {
            if ((ret = rtl9300_led_config(unit)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_INIT), "led config failed");
            }
        }
    }


    if ((ret = _rtl9300_smiAddr_init(unit)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INIT), "SMI Address init failed");
    }

    if ((ret = _rtl9300_smiMacType_init(unit)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INIT), "mac phy type init failed");
    }

    return RT_ERR_OK;
} /* end of rtl9300_init */


/* Function Name:
 *      rtl9300_miim_read
 * Description:
 *      Get PHY registers from rtl9300 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None.
 */
int32
rtl9300_miim_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      *pData)
{
    uint32 temp;
    uint32 val;
    uint32 parkPage;


    int32  ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, port=%d, page=0x%x, phy_reg=0x%x", unit, port, page, phy_reg);

    //RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_FIX_PAGE), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);


    PHY_SEM_LOCK(unit);

    if (page == HAL_MIIM_FIX_PAGE)
    {
        page = HAL_MIIM_PAGE_ID_MAX(unit);
    }
    else
    {
        if (page > HAL_MIIM_PAGE_ID_MAX(unit))
        {
            _rtl9300_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, page);
            page = HAL_MIIM_PAGE_ID_MAX(unit);
        }
    }

    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * INDATA[5:0] is the Port WHEN RWOP = 0b0
     */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_2r, LONGAN_INDATA_15_0f, &port, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    if ((ret = reg_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_2r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* initialize variable */
    temp = 0;

    /* Select register number to access */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_REG_ADDR_4_0f, &phy_reg, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select page number to access */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_MAIN_PAGE_11_0f, &page, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select parked page*/
    /*Always not Park, because 80 mac polling always send right page*/
    parkPage = 0x1f;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_PARK_PAGE_4_0f, &parkPage, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }


    /* Select PHY register type
     * 0b0: Normal register
     * 0b1: MMD register
     */
    val = 0;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_TYPEf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    val = 0;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_RWOPf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    val = 1;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_CMDf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, 0x1, LONGAN_SMI_ACCESS_PHY_CTRL_1_FAIL_MASK);

    /* get the read operation result to pData */
    if ((ret = reg_field_read(unit, LONGAN_SMI_ACCESS_PHY_CTRL_2r, LONGAN_DATA_15_0f, pData)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    PHY_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_HAL), "pData=0x%x", *pData);

    return RT_ERR_OK;
} /* end of rtl9300_miim_read */


/* Function Name:
 *      rtl9300_miim_write
 * Description:
 *      Set PHY registers in rtl9300 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
_rtl9300_miim_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      data)
{
    uint32  temp;
    uint32  val;
    uint32 parkPage;
    int32   ret = RT_ERR_FAILED;


    /* Select PHY to access */
    val = 1<<port;
    if ((ret = reg_field_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_0r, LONGAN_PHY_MASKf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * If RWOP = 0(read), then INDATA[15:0] = {Reserved & PORT_ID[5:0]}
     * If RWOP = 1(write), then INDATA[15:0] = DATA[15:0]
     */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_2r, LONGAN_INDATA_15_0f, &data, &temp)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_2r, &temp)) != RT_ERR_OK)
    {
        return ret;
    }

    /* initialize variable */
    temp = 0;

    /* Select register number to access */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_REG_ADDR_4_0f, &phy_reg, &temp)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Select page number to access */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_MAIN_PAGE_11_0f, &page, &temp)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Select parked page*/
    /*Always not Park, because 80 mac polling always send right page*/
    parkPage = 0x1f;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_PARK_PAGE_4_0f, &parkPage, &temp)) != RT_ERR_OK)
    {
        return ret;
    }


    /* Select PHY register type
     * 0b0: Normal register
     * 0b1: MMD register
     */
    val = 0;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_TYPEf, &val, &temp)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    val = 1;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_RWOPf, &val, &temp)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    val = 1;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_CMDf, &val, &temp)) != RT_ERR_OK)
    {
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, &temp)) != RT_ERR_OK)
    {
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    if ((ret = drv_phy_smiCmdComplete_wait(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, 0x1, LONGAN_SMI_ACCESS_PHY_CTRL_1_FAIL_MASK, ACCESS_PHY_TIMEOUT_TIME)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9300_miim_write */


/* Function Name:
 *      rtl9300_miim_write
 * Description:
 *      Set PHY registers in rtl9300 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9300_miim_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      data)
{
    int32   ret = RT_ERR_FAILED;


    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, page=0x%x, phy_reg=0x%x, data=0x%x", unit, port, page, phy_reg, data);
    //RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_FIX_PAGE), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    PHY_SEM_LOCK(unit);

    if (page == HAL_MIIM_FIX_PAGE)
    {
        page = HAL_MIIM_PAGE_ID_MAX(unit);
    }
    else
    {
        if (page > HAL_MIIM_PAGE_ID_MAX(unit))
        {
            _rtl9300_miim_write(unit, port, PHY_PAGE_0, PHY_PAGE_SELECTION_REG, page);
            page = HAL_MIIM_PAGE_ID_MAX(unit);
        }
    }

    ret = _rtl9300_miim_write(unit, port, page, phy_reg, data);
    PHY_SEM_UNLOCK(unit);

    return ret;
} /* end of rtl9300_miim_write */

/* Function Name:
 *      rtl9300_miim_park_read
 * Description:
 *      Get PHY registers from rtl9300 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      parkPage    - PHY park page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9300_miim_park_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      *pData)
{
    uint32 temp;
    uint32 val;

    int32  ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, port=%d, page=0x%x, phy_reg=0x%x", unit, port, page, phy_reg);

    //RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_FIX_PAGE), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((parkPage > 0x1F), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    if (page == HAL_MIIM_FIX_PAGE)
    {
        page = HAL_MIIM_PAGE_ID_MAX(unit);
    }
    else
    {
        RT_PARAM_CHK((page > HAL_MIIM_PAGE_ID_MAX(unit)), RT_ERR_PHY_PAGE_ID);
    }

    PHY_SEM_LOCK(unit);

    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * INDATA[5:0] is the Port WHEN RWOP = 0b0
     */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_2r, LONGAN_INDATA_15_0f, &port, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    if ((ret = reg_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_2r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* initialize variable */
    temp = 0;

    /* Select register number to access */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_REG_ADDR_4_0f, &phy_reg, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select page number to access */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_MAIN_PAGE_11_0f, &page, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select parked page*/
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_PARK_PAGE_4_0f, &parkPage, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }


    /* Select PHY register type
     * 0b0: Normal register
     * 0b1: MMD register
     */
    val = 0;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_TYPEf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    val = 0;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_RWOPf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    val = 1;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_CMDf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, 0x1, LONGAN_SMI_ACCESS_PHY_CTRL_1_FAIL_MASK);

    /* get the read operation result to pData */
    if ((ret = reg_field_read(unit, LONGAN_SMI_ACCESS_PHY_CTRL_2r, LONGAN_DATA_15_0f, pData)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    PHY_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_HAL), "pData=0x%x", *pData);

    return RT_ERR_OK;
} /* end of rtl9300_miim_park_read */


/* Function Name:
 *      rtl9300_miim_park_write
 * Description:
 *      Set PHY registers in rtl9300 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      page    - PHY page
 *      parkPage    - PHY park page
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9300_miim_park_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      data)
{
    uint32  temp;
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, page=0x%x, phy_reg=0x%x, data=0x%x", unit, port, page, phy_reg, data);
    //RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_FIX_PAGE), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((parkPage > 0x1F), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);
    if (page == HAL_MIIM_FIX_PAGE)
    {
        page = HAL_MIIM_PAGE_ID_MAX(unit);
    }
    else
    {
        RT_PARAM_CHK((page > HAL_MIIM_PAGE_ID_MAX(unit)), RT_ERR_PHY_PAGE_ID);
    }

    PHY_SEM_LOCK(unit);

    /* Select PHY to access */
    val = 1<<port;
    if ((ret = reg_field_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_0r, LONGAN_PHY_MASKf, &val)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * If RWOP = 0(read), then INDATA[15:0] = {Reserved & PORT_ID[5:0]}
     * If RWOP = 1(write), then INDATA[15:0] = DATA[15:0]
     */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_2r, LONGAN_INDATA_15_0f, &data, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    if ((ret = reg_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_2r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* initialize variable */
    temp = 0;

    /* Select register number to access */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_REG_ADDR_4_0f, &phy_reg, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select page number to access */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_MAIN_PAGE_11_0f, &page, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select parked page*/
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_PARK_PAGE_4_0f, &parkPage, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }


    /* Select PHY register type
     * 0b0: Normal register
     * 0b1: MMD register
     */
    val = 0;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_TYPEf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    val = 1;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_RWOPf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    val = 1;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_CMDf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, 0x1, LONGAN_SMI_ACCESS_PHY_CTRL_1_FAIL_MASK);

    PHY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of rtl9300_miim_park_write */

/* Function Name:
 *      rtl9300_miim_portmask_write
 * Description:
 *      Set PHY registers in those portmask of rtl9300 family chips.
 * Input:
 *      unit     - unit id
 *      portmask - portmask
 *      page     - PHY page
 *      phy_reg  - PHY register
 *      data     - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. portmask valid range is bit 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9300_miim_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          page,
    uint32          phy_reg,
    uint32          data)
{
    uint32  temp;
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, portmask=0x%x, page=0x%x, phy_reg=0x%x, data=0x%x", unit, portmask.bits[0], page, phy_reg, data);
    RT_PARAM_CHK((page >= PHY_PAGE_MAX), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    PHY_SEM_LOCK(unit);

    /* Select PHY to access */
    val = portmask.bits[0];
    if ((ret = reg_field_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_0r, LONGAN_PHY_MASKf, &val)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * If RWOP = 0(read), then INDATA[15:0] = {Reserved & PORT_ID[5:0]}
     * If RWOP = 1(write), then INDATA[15:0] = DATA[15:0]
     */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_2r, LONGAN_INDATA_15_0f, &data, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    if ((ret = reg_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_2r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* initialize variable */
    temp = 0;
    /* Select register number to access */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_REG_ADDR_4_0f, &phy_reg, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select page number to access */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_MAIN_PAGE_11_0f, &page, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select PHY register type
     * 0b0: Normal register
     * 0b1: MMD register
     */
    val = 0;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_TYPEf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    val = 1;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_RWOPf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    val = 1;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_CMDf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, 0x1, LONGAN_SMI_ACCESS_PHY_CTRL_1_FAIL_MASK);

    PHY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of rtl9300_miim_portmask_write */

/* Function Name:
 *      rtl9300_miim_broadcast_write
 * Description:
 *      Set PHY registers in rtl9300 family chips with broadcast mechanism.
 * Input:
 *      unit    - unit id
 *      page    - page id
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. page valid range is 0 ~ 31
 *      2. phy_reg valid range is 0 ~ 31
 */
int32
rtl9300_miim_broadcast_write(
    uint32      unit,
    uint32      page,
    uint32      phy_reg,
    uint32      data)
{
    rtk_port_t      port;
    uint32          found = FALSE;
    uint32          phyAddr;
    int32           ret;

    if (page == HAL_MIIM_FIX_PAGE)
    {
        page = HAL_MIIM_PAGE_ID_MAX(unit);
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if (HWP_ETHER_PORT(unit, port))
        {
            found = TRUE;
            break;
        }
    }
    if (found == FALSE)
        return RT_ERR_FAILED;

    if ((ret = _rtl9300_port_phyAddr_get(unit, port, &phyAddr)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %u macId %u unable to get phyAddr %d!", unit, port, ret);
        return ret;
    }

    /* set broadcast address */
    if ((ret = _rtl9300_port_phyAddr_set(unit, port, 0x1F)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %u macId %u unable to set broadcast fail %d!", unit, port, ret);
        return ret;
    }

    if ((ret = rtl9300_miim_write(unit, port, page, phy_reg, data)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %u macId %u unable to set broadcast fail %d!", unit, port, ret);
        goto RET;
    }

  RET:
    /* recover back the phyAddr */
    if (_rtl9300_port_phyAddr_set(unit, port, phyAddr) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %u macId %u unable to recover phyAddr %d!", unit, port, ret);
    }

    return ret;

} /* end of rtl9300_miim_broadcast_write */



/* Function Name:
 *      rtl9300_miim_read
 * Description:
 *      Get PHY registers from rtl8390 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mainPage    - main page id
 *      extPage     - extension page id
 *      parkPage    - parking page id
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9300_miim_extParkPage_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      mainPage,
    uint32      extPage,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      *pData)
{
    return RT_ERR_OK;
} /* end of rtl9300_miim_read */


/* Function Name:
 *      rtl9300_miim_write
 * Description:
 *      Set PHY registers in rtl8390 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mainPage    - main page id
 *      extPage     - extension page id
 *      parkPage    - parking page id
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9300_miim_extParkPage_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      mainPage,
    uint32      extPage,
    uint32      parkPage,
    uint32      phy_reg,
    uint32      data)
{
    return RT_ERR_OK;
} /* end of rtl9300_miim_write */

/* Function Name:
 *      rtl9300_miim_extParkPage_portmask_write
 * Description:
 *      Set PHY registers in those portmask of rtl8390 family chips.
 * Input:
 *      unit     - unit id
 *      portmask - portmask
 *      page     - PHY page
 *      phy_reg  - PHY register
 *      data     - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. portmask valid range is bit 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9300_miim_extParkPage_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          mainPage,
    uint32          extPage,
    uint32          parkPage,
    uint32          phy_reg,
    uint32          data)
{
    return RT_ERR_OK;
} /* end of rtl9300_miim_portmask_write */


/* Function Name:
 *      rtl9300_miim_mmd_read
 * Description:
 *      Get PHY registers from rtl9300 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mmdAddr - mmd device address
 *      mmdReg  - mmd reg id
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9300_miim_mmd_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      mmdAddr,
    uint32      mmdReg,
    uint32      *pData)
{
    uint32 temp;
    uint32 val;

    int32  ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, port=%d, mmdAddr=0x%x, mmdReg=0x%x, reg=0x%x",
           unit, port, mmdAddr, mmdReg);

    //RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    //RT_PARAM_CHK((mainPage > HAL_MIIM_PAGE_ID_MAX(unit)), RT_ERR_PHY_PAGE_ID);
    //RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    PHY_SEM_LOCK(unit);

    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * INDATA[5:0] is the Port WHEN RWOP = 0b0
     */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_2r, LONGAN_INDATA_15_0f, &port, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    if ((ret = reg_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_2r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* initialize variable */
    temp = 0;
    /* Select mmd device address to access */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_3r, LONGAN_MMD_DEVAD_4_0f, &mmdAddr, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select mmd register to access */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_3r, LONGAN_MMD_REG_15_0f, &mmdReg, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    if ((ret = reg_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_3r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select PHY register type
     * 0b0: Normal register
     * 0b1: MMD register
     */
    val = 1;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_TYPEf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    val = 0;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_RWOPf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    val = 1;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_CMDf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, 0x1, LONGAN_SMI_ACCESS_PHY_CTRL_1_FAIL_MASK);

    /* get the read operation result to pData */
    if ((ret = reg_field_read(unit, LONGAN_SMI_ACCESS_PHY_CTRL_2r, LONGAN_DATA_15_0f, pData)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    PHY_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_HAL), "pData=0x%x", *pData);

    return RT_ERR_OK;
} /* end of rtl9300_miim_mmd_read */


/* Function Name:
 *      rtl9300_miim_mmd_write
 * Description:
 *      Set PHY registers in rtl9300 family chips.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mmdAddr - mmd device address
 *      mmdReg  - mmd reg id
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9300_miim_mmd_write(
    uint32      unit,
    rtk_port_t  port,
    uint32      mmdAddr,
    uint32      mmdReg,
    uint32      data)
{
    uint32  temp;
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, port=%d, mmdAddr=0x%x, mmdReg=0x%x \
           data=0x%x", unit, port, mmdAddr, mmdReg, data);

    //RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    //RT_PARAM_CHK((page >= PHY_PAGE_MAX), RT_ERR_PHY_PAGE_ID);
    //RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    PHY_SEM_LOCK(unit);

    /* Select PHY to access */
    val = 1<<port;
    if ((ret = reg_field_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_0r, LONGAN_PHY_MASKf, &val)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * If RWOP = 0(read), then INDATA[15:0] = {Reserved & PORT_ID[5:0]}
     * If RWOP = 1(write), then INDATA[15:0] = DATA[15:0]
     */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_2r, LONGAN_INDATA_15_0f, &data, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    if ((ret = reg_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_2r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* initialize variable */
    temp = 0;
    /* Select mmd device address to access */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_3r, LONGAN_MMD_DEVAD_4_0f, &mmdAddr, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select mmd register to access */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_3r, LONGAN_MMD_REG_15_0f, &mmdReg, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    if ((ret = reg_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_3r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select PHY register type
     * 0b0: Normal register
     * 0b1: MMD register
     */
    val = 1;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_TYPEf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    val = 1;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_RWOPf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    val = 1;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_CMDf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, 0x1, LONGAN_SMI_ACCESS_PHY_CTRL_1_FAIL_MASK);

    PHY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of rtl9300_miim_mmd_write */

/* Function Name:
 *      rtl9300_miim_mmd_portmask_write
 * Description:
 *      Set PHY registers in those portmask of rtl9300 family chips.
 * Input:
 *      unit     - unit id
 *      portmask - portmask
 *      mmdAddr  - mmd device address
 *      mmdReg   - mmd reg id
 *      data     - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. portmask valid range is bit 0 ~ 27
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9300_miim_mmd_portmask_write(
    uint32          unit,
    rtk_portmask_t  portmask,
    uint32          mmdAddr,
    uint32          mmdReg,
    uint32          data)
{
    uint32  temp;
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, portmask=0x%x, mmdAddr=0x%x, mmdReg=0x%x \
           data=0x%x", unit, portmask.bits[0], mmdAddr, mmdReg, data);
    //RT_PARAM_CHK((page >= PHY_PAGE_MAX), RT_ERR_PHY_PAGE_ID);
    //RT_PARAM_CHK((phy_reg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    PHY_SEM_LOCK(unit);

    /* Select PHY to access */
    val = portmask.bits[0];
    if ((ret = reg_field_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_0r, LONGAN_PHY_MASKf, &val)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    /* initialize variable */
    temp = 0;

    /* Input parameters:
     * If RWOP = 0(read), then INDATA[15:0] = {Reserved & PORT_ID[5:0]}
     * If RWOP = 1(write), then INDATA[15:0] = DATA[15:0]
     */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_2r, LONGAN_INDATA_15_0f, &data, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    if ((ret = reg_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_2r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* initialize variable */
    temp = 0;
    /* Select mmd device address to access */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_3r, LONGAN_MMD_DEVAD_4_0f, &mmdAddr, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select mmd register to access */
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_3r, LONGAN_MMD_REG_15_0f, &mmdReg, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }
    if ((ret = reg_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_3r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Select PHY register type
     * 0b0: Normal register
     * 0b1: MMD register
     */
    val = 1;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_TYPEf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Read/Write operation
     * 0b0: read
     * 0b1: write
     */
    val = 1;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_RWOPf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* Request MAC to access PHY MII register
     * 0b0: complete access
     * 0b1: execute access
     * Note: When MAC completes access, it will clear this bit.
     */
    val = 1;
    if ((ret = reg_field_set(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, LONGAN_CMDf, &val, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* write register to active the read operation */
    if ((ret = reg_write(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, &temp)) != RT_ERR_OK)
    {
        PHY_SEM_UNLOCK(unit);
        return ret;
    }

    /* busy waiting until reg.bit[0] = 0b0 (MAC completes access) */
    PHY_WAIT_CMD_COMPLETE(unit, LONGAN_SMI_ACCESS_PHY_CTRL_1r, 0x1, LONGAN_SMI_ACCESS_PHY_CTRL_1_FAIL_MASK);

    PHY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of rtl9300_miim_mmd_portmask_write */

/* Function Name:
 *      rtl9300_table_read
 * Description:
 *      Read one specified table entry by table index.
 * Input:
 *      unit  - unit id
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *      None
 */
int32
rtl9300_table_read(
    uint32  unit,
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    uint32      reg_data, reg_value;
    uint32      busy;
    int32       ret = RT_ERR_FAILED;
    rtk_table_t *pTable = NULL;
    uint32      groupId;
    rtk_indirectCtrlGroup_t ctrlGroup[] = {
        INDIRECT_CTRL_GROUP_L2,
        INDIRECT_CTRL_GROUP_TABLE,
        INDIRECT_CTRL_GROUP_PKT_ENC,
        INDIRECT_CTRL_GROUP_EGR_CTRL,
        INDIRECT_CTRL_GROUP_HSB_CTRL,
        INDIRECT_CTRL_GROUP_HSA_CTRL };
    rtk_longan_reg_list_t ctrlReg[] = {
        LONGAN_TBL_ACCESS_L2_CTRLr,
        LONGAN_TBL_ACCESS_CTRL_0r,
        LONGAN_TBL_ACCESS_CTRL_1r,
        LONGAN_TBL_ACCESS_CTRL_2r,
        LONGAN_TBL_ACCESS_HSB_CTRLr,
        LONGAN_TBL_ACCESS_HSA_CTRLr };
    rtk_longan_reg_list_t dataReg[] = {
        LONGAN_TBL_ACCESS_L2_DATAr,
        LONGAN_TBL_ACCESS_DATA_0r,
        LONGAN_TBL_ACCESS_DATA_1r,
        LONGAN_TBL_ACCESS_DATA_2r,
        LONGAN_TBL_ACCESS_HSB_DATAr,
        LONGAN_TBL_ACCESS_HSA_DATAr };
    uint32      index;
    #if !defined(CONFIG_VIRTUAL_ARRAY_ONLY)
    uint32 retry = 0;
    #endif

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, table=%d, addr=0x%x", unit, table, addr);

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pTable = table_find(unit, table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch (table)
    {
    case LONGAN_L2_UCt:
    case LONGAN_L2_MCt:
    case LONGAN_L2_CAM_UCt:
    case LONGAN_L2_CAM_MCt:
    case LONGAN_MC_PORTMASKt:
        groupId = 0;
        break;

    case LONGAN_VLANt:
    case LONGAN_VLAN_IGR_CNVTt:
    case LONGAN_VLAN_MAC_BASEDt:
    case LONGAN_VLAN_IP_BASEDt:
    case LONGAN_METERt:
    case LONGAN_MSTIt:
    case LONGAN_LOGt:
    case LONGAN_VACLt:
    case LONGAN_IACLt:
    case LONGAN_PORT_ISO_CTRLt:
    case LONGAN_LAGt:
    case LONGAN_SRC_TRK_MAPt:
        groupId = 1;
        break;

    case LONGAN_L3_ROUTER_MACt:
    case LONGAN_L3_HOST_ROUTE_IPUCt:
    case LONGAN_L3_HOST_ROUTE_IPMCt:
    case LONGAN_L3_HOST_ROUTE_IP6UCt:
    case LONGAN_L3_HOST_ROUTE_IP6MCt:
    case LONGAN_L3_PREFIX_ROUTE_IPUCt:
    case LONGAN_L3_PREFIX_ROUTE_IPMCt:
    case LONGAN_L3_PREFIX_ROUTE_IP6UCt:
    case LONGAN_L3_PREFIX_ROUTE_IP6MCt:
    case LONGAN_L3_NEXTHOPt:
    case LONGAN_L3_EGR_INTF_LISTt:
    case LONGAN_L3_EGR_INTFt:
        groupId = 2;
        break;

    case LONGAN_UNTAGt:
    case LONGAN_VLAN_EGR_CNVTt:
    case LONGAN_L3_EGR_INTF_MACt:
    case LONGAN_REMARKt:
        groupId = 3;
        break;

    case LONGAN_HSBt:
        groupId = 4;
        break;

    case LONGAN_HSAt:
        groupId = 5;
        break;

    default:
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    MEM_SEM_LOCK(unit, ctrlGroup[groupId]);

    /* initialize variable */
    reg_data = 0;
    busy = 0;

    /* Command hardware to execute indirect table access
     * 0b0: not execute
     * 0b1: execute
     * Note: This bit is common used by software and hardware.
     *       When hardware completes the table access, it will clear this bit.
     */
    reg_value = 1;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], LONGAN_EXECf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Table access operation
     * 0b0: read
     * 0b1: write
     */
    reg_value = 0;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], LONGAN_CMDf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* access table type */
    reg_value = pTable->type;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], LONGAN_TBLf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Select access address of the table */
    if((ctrlGroup[groupId] == INDIRECT_CTRL_GROUP_PKT_ENC) && (pTable->type == TABLE_TYPE_L3_HOST_ROUTE))
    {
        /* In L3 HOST/PREFIX table, the 6th/7th entry of a row does NOT exist,
         * have to do the 6:8 mapping here.
         */
        reg_value = L3_ENTRY_IDX_TO_ADDR(addr);
    }
    else
    {
        reg_value = addr;
    }
    if ((ret = reg_field_set(unit, ctrlReg[groupId], LONGAN_ADDRf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(unit, ctrlReg[groupId], &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

#if !defined(CONFIG_VIRTUAL_ARRAY_ONLY)
    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(unit, ctrlReg[groupId], LONGAN_EXECf, &busy)) != RT_ERR_OK)
        {
            MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
            return ret;
        }

        if((ctrlGroup[groupId] == INDIRECT_CTRL_GROUP_PKT_ENC) && (pTable->type == TABLE_TYPE_L3_HOST_ROUTE))
        {
            retry++;
            if ((retry >= TABLE_READ_RETRY_TIME) && (busy))
            {
                MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
                return RT_ERR_BUSYWAIT_TIMEOUT;
            }
        }
    } while (busy);
#endif

    /* Read table data from indirect data register */
    for (index=0; index<(pTable->datareg_num); index++)
    {
        if (table == LONGAN_HSAt)
        {
            if ((ret = reg_array_read(unit, dataReg[groupId], REG_ARRAY_INDEX_NONE, pTable->datareg_num - 1 - index, pData + index)) != RT_ERR_OK)
            {
                MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
                return ret;
            }
        } else {
            if ((ret = reg_array_read(unit, dataReg[groupId], REG_ARRAY_INDEX_NONE, index, pData + index)) != RT_ERR_OK)
            {
                MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
                return ret;
            }
        }
    }
    MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);

    return RT_ERR_OK;
} /* end of rtl9300_table_read */


/* Function Name:
 *      rtl9300_table_write
 * Description:
 *      Write one specified table entry by table index.
 * Input:
 *      unit  - unit id
 *      table - table index
 *      addr  - entry address of the table
 *      pData - pointer buffer of table entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *      None
 */
int32
rtl9300_table_write(
    uint32  unit,
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    uint32      reg_data, reg_value;
    uint32      busy;
    int32       ret = RT_ERR_FAILED;
    rtk_table_t *pTable = NULL;
    uint32      groupId;
    rtk_indirectCtrlGroup_t ctrlGroup[] = {
        INDIRECT_CTRL_GROUP_L2,
        INDIRECT_CTRL_GROUP_TABLE,
        INDIRECT_CTRL_GROUP_PKT_ENC,
        INDIRECT_CTRL_GROUP_EGR_CTRL,
        INDIRECT_CTRL_GROUP_HSB_CTRL,
        INDIRECT_CTRL_GROUP_HSA_CTRL };
    rtk_longan_reg_list_t ctrlReg[] = {
        LONGAN_TBL_ACCESS_L2_CTRLr,
        LONGAN_TBL_ACCESS_CTRL_0r,
        LONGAN_TBL_ACCESS_CTRL_1r,
        LONGAN_TBL_ACCESS_CTRL_2r,
        LONGAN_TBL_ACCESS_HSB_CTRLr,
        LONGAN_TBL_ACCESS_HSA_CTRLr };
    rtk_longan_reg_list_t dataReg[] = {
        LONGAN_TBL_ACCESS_L2_DATAr,
        LONGAN_TBL_ACCESS_DATA_0r,
        LONGAN_TBL_ACCESS_DATA_1r,
        LONGAN_TBL_ACCESS_DATA_2r,
        LONGAN_TBL_ACCESS_HSB_DATAr,
        LONGAN_TBL_ACCESS_HSA_DATAr };
    uint32      index;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, table=%d, addr=0x%x", unit, table, addr);

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pTable = table_find(unit, table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);

    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch (table)
    {
    case LONGAN_L2_UCt:
    case LONGAN_L2_MCt:
    case LONGAN_L2_CAM_UCt:
    case LONGAN_L2_CAM_MCt:
    case LONGAN_MC_PORTMASKt:
        groupId = 0;
        break;

    case LONGAN_VLANt:
    case LONGAN_VLAN_IGR_CNVTt:
    case LONGAN_VLAN_MAC_BASEDt:
    case LONGAN_VLAN_IP_BASEDt:
    case LONGAN_METERt:
    case LONGAN_MSTIt:
    case LONGAN_LOGt:
    case LONGAN_VACLt:
    case LONGAN_IACLt:
    case LONGAN_PORT_ISO_CTRLt:
    case LONGAN_LAGt:
    case LONGAN_SRC_TRK_MAPt:
        groupId = 1;
        break;

    case LONGAN_L3_ROUTER_MACt:
    case LONGAN_L3_HOST_ROUTE_IPUCt:
    case LONGAN_L3_HOST_ROUTE_IPMCt:
    case LONGAN_L3_HOST_ROUTE_IP6UCt:
    case LONGAN_L3_HOST_ROUTE_IP6MCt:
    case LONGAN_L3_PREFIX_ROUTE_IPUCt:
    case LONGAN_L3_PREFIX_ROUTE_IPMCt:
    case LONGAN_L3_PREFIX_ROUTE_IP6UCt:
    case LONGAN_L3_PREFIX_ROUTE_IP6MCt:
    case LONGAN_L3_NEXTHOPt:
    case LONGAN_L3_EGR_INTF_LISTt:
    case LONGAN_L3_EGR_INTFt:
        groupId = 2;
        break;

    case LONGAN_UNTAGt:
    case LONGAN_VLAN_EGR_CNVTt:
    case LONGAN_L3_EGR_INTF_MACt:
    case LONGAN_REMARKt:
        groupId = 3;
        break;

    case LONGAN_HSBt:
        groupId = 4;
        break;

    case LONGAN_HSAt:
        groupId = 5;
        break;

    default:
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    MEM_SEM_LOCK(unit, ctrlGroup[groupId]);

    /* initialize variable */
    reg_data = 0;
    busy = 0;

    /* Write pre-configure table data to indirect data register */
    for (index=0; index<(pTable->datareg_num); index++)
    {
        if (table == LONGAN_HSAt)
        {
            if ((ret = reg_array_write(unit, dataReg[groupId], REG_ARRAY_INDEX_NONE, pTable->datareg_num - 1 - index, pData + index)) != RT_ERR_OK)
            {
                MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
                return ret;
            }
        } else {
            if ((ret = reg_array_write(unit, dataReg[groupId], REG_ARRAY_INDEX_NONE, index, pData + index)) != RT_ERR_OK)
            {
                MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
                return ret;
            }
        }
    }

    /* Command hardware to execute indirect table access
     * 0b0: not execute
     * 0b1: execute
     * Note: This bit is common used by software and hardware.
     *       When hardware completes the table access, it will clear this bit.
     */
    reg_value = 1;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], LONGAN_EXECf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Table access operation
     * 0b0: read
     * 0b1: write
     */
    reg_value = 1;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], LONGAN_CMDf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* access table type */
    reg_value = pTable->type;
    if ((ret = reg_field_set(unit, ctrlReg[groupId], LONGAN_TBLf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Select access address of the table */
     if((ctrlGroup[groupId] == INDIRECT_CTRL_GROUP_PKT_ENC) && (pTable->type == TABLE_TYPE_L3_HOST_ROUTE))
    {
        /* In L3 HOST/PREFIX table, the 6th/7th entry of a row does NOT exist,
         * have to do the 6:8 mapping here.
         */
        reg_value = L3_ENTRY_IDX_TO_ADDR(addr);
    }
    else
    {
        reg_value = addr;
    }
    if ((ret = reg_field_set(unit, ctrlReg[groupId], LONGAN_ADDRf, &reg_value, &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

    /* Write indirect control register to start the write operation */
    if ((ret = reg_write(unit, ctrlReg[groupId], &reg_data)) != RT_ERR_OK)
    {
        MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
        return ret;
    }

#if !defined(CONFIG_VIRTUAL_ARRAY_ONLY)
    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(unit, ctrlReg[groupId], LONGAN_EXECf, &busy)) != RT_ERR_OK)
        {
            MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);
            return ret;
        }
    } while (busy);
#endif

    MEM_SEM_UNLOCK(unit, ctrlGroup[groupId]);

    return RT_ERR_OK;
} /* end of rtl9300_table_write */

/* Function Name:
 *      rtl9300_miim_pollingEnable_get
 * Description:
 *      Get the mac polling PHY status of the specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pEnabled - pointer buffer of mac polling PHY status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
rtl9300_miim_pollingEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_enable_t    *pEnabled)
{
    uint32 val;
    int32  ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, port=%d", unit, port);

    RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnabled), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(unit, LONGAN_SMI_POLL_CTRLr, LONGAN_SMI_POLL_MASK_27_0f, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "");
        return ret;
    }

    if (((val >> port) & 0x1) == 1)
        (*pEnabled) = ENABLED;
    else
        (*pEnabled) = DISABLED;

    return RT_ERR_OK;
} /* end of rtl9300_miim_pollingEnable_get */

/* Function Name:
 *      rtl9300_miim_pollingEnable_set
 * Description:
 *      Set the mac polling PHY status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enabled - mac polling PHY status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
rtl9300_miim_pollingEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_enable_t    enable)
{
    uint32 val;
    int32  ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, port=%d, enable=%d", unit, port, enable);

    RT_PARAM_CHK((!HWP_ETHER_PORT(unit, port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable != DISABLED && enable != ENABLED), RT_ERR_INPUT);

    if ((ret = reg_field_read(unit, LONGAN_SMI_POLL_CTRLr, LONGAN_SMI_POLL_MASK_27_0f, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "");
        return ret;
    }

    if (enable)
        val |= (1 << port);
    else
        val &= ~(1 << port);

    if ((ret = reg_field_write(unit, LONGAN_SMI_POLL_CTRLr, LONGAN_SMI_POLL_MASK_27_0f, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9300_miim_pollingEnable_set */


/* Function Name:
 *      drv_rtl9300_sds2XsgmSds_get
 * Description:
 *      Get XSGMII SerDes mapping from SerDes ID
 *      Convert logic sds id to physical sds id
 *      Only page 0 ~ 3 needs this conversion
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      mappedSds - Mapped SerDes id
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
drv_rtl9300_sds2XsgmSds_get(uint32 unit, uint32 sds, uint32 *mappedSds)
{
    RT_PARAM_CHK((!HWP_SDS_EXIST(unit, sds)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == mappedSds), RT_ERR_NULL_POINTER);

    if (sds == 3)
        *mappedSds = 10;
    else
        *mappedSds = sds;

    return RT_ERR_OK;
}   /* end of drv_rtl9300_sds2XsgmSds_get */

/* Function Name:
 *      drv_rtl9300_sdsCmuPage_get
 * Description:
 *      Get SerDes CMU page
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      page - CMU page
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
drv_rtl9300_sdsCmuPage_get(uint32 unit, uint32 sds, uint32 *page)
{
    RT_PARAM_CHK((!HWP_SDS_EXIST(unit, sds)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == page), RT_ERR_NULL_POINTER);

    switch (HWP_SDS_MODE(unit, sds))
    {
        case RTK_MII_100BX_FIBER:
        case RTK_MII_1000BX_FIBER:
        case RTK_MII_SGMII:
            *page = 0x25;
            break;
        case RTK_MII_RSGMII:
            *page = 0x27;
            break;
        case RTK_MII_2500Base_X:
        case RTK_MII_HISGMII:
            *page = 0x29;
            break;
        case RTK_MII_QSGMII:
            *page = 0x2B;
            break;
        case RTK_MII_RXAUI_LITE:
        case RTK_MII_RXAUI_PLUS:
            *page = 0x2D;
            break;
        case RTK_MII_XSGMII:
        case RTK_MII_QHSGMII:
        case RTK_MII_USXGMII_10GSXGMII:
        case RTK_MII_USXGMII_10GDXGMII:
        case RTK_MII_USXGMII_10GQXGMII:
        case RTK_MII_10GR:
            *page = 0x2F;
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}   /* end of drv_rtl9310_sdsCmuPage_get */
/* Function Name:
 *      rtl9300_drv_serdes_write
 * Description:
 *      write serdes register.
 * Input:
 *      unit    - unit id
 *      sdsId    - serdes id
 *      page
 *      reg
 *      data
 * Output:
 *      None
 * Return:
 *      RT_ERR_TIMEOUT
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32
rtl9300_drv_serdes_write(uint32 unit, uint32 sdsId, uint32 page, uint32 reg, uint32 data)
{
    uint32 cmddata = 0;
    uint32 value = 0;
    uint32 i;

    value = data;
    reg_field_write(unit, LONGAN_SDS_INDACS_DATAr, LONGAN_SDS_DATAf, &value);
    cmddata = (sdsId<<2) + (page<<7) + (reg<<13) + 3;
    reg_write(unit, LONGAN_SDS_INDACS_CMDr, &cmddata);

    for (i = 0; i < 100; i++)
    {
        reg_field_read(unit, LONGAN_SDS_INDACS_CMDr, LONGAN_SDS_CMDf,&value);
        if (0 == value)
        {
            break;
        }
        osal_time_mdelay(1);
    }
    osal_time_udelay(10);
    if (i >= 100)
    {
        RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, sdsId=%d, sds register access fail!\n", unit, sdsId);
        return RT_ERR_TIMEOUT;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9300_drv_serdes_read
 * Description:
 *      read serdes register.
 * Input:
 *      unit    - unit id
 *      sdsId    - serdes id
 *      page
 *      reg
 *      pData
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_TIMEOUT
 *      RT_ERR_OK
 * Note:
 *      None
 */

int32
rtl9300_drv_serdes_read(uint32 unit, uint32 sdsId, uint32 page, uint32 reg, uint32 *pData)
{
    uint32 cmddata = 0;
    uint32 value;
    uint32 i;

    cmddata = (sdsId<<2) + (page<<7) + (reg<<13) +1;
    reg_write(unit, LONGAN_SDS_INDACS_CMDr, &cmddata);

    for (i = 0; i < 100; i++)
    {
        reg_field_read(unit, LONGAN_SDS_INDACS_CMDr, LONGAN_SDS_CMDf,&value);
        if (0 == value)
        {
            break;
        }
        osal_time_mdelay(1);
    }

    if (i >= 100)
    {
        RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, sdsId=%d, sds register access fail!\n", unit, sdsId);
        return RT_ERR_TIMEOUT;
    }

    osal_time_udelay(10);
    reg_field_read(unit, LONGAN_SDS_INDACS_DATAr, LONGAN_SDS_DATAf, &value);
    *pData = value & 0xffff;

    return RT_ERR_OK;
}


/* Function Name:
 *      rtl9300_serdes_rst
 * Description:
 *      Reset Serdes and original patch are kept.
 * Input:
 *      unit    - unit id
 *      sds_num    - serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_OUT_OF_RANGE - Serdes index is not support.
 * Note:
 *      None
 */
int32
rtl9300_serdes_rst(
    uint32  unit,
    uint32 sds_num)
{
    uint32  value;
    uint32 reg;
    uint32 field;
    reg_field_t sds_mode_sel[] =
    {
        {LONGAN_SDS_MODE_SEL_0r, LONGAN_SDS0_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_0r, LONGAN_SDS1_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_0r, LONGAN_SDS2_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_0r, LONGAN_SDS3_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_1r, LONGAN_SDS4_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_1r, LONGAN_SDS5_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_1r, LONGAN_SDS6_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_1r, LONGAN_SDS7_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_2r, LONGAN_SDS8_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_2r, LONGAN_SDS9_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_3r, LONGAN_SDS10_MODE_SELf},
        {LONGAN_SDS_MODE_SEL_3r, LONGAN_SDS11_MODE_SELf},
    };

    reg = sds_mode_sel[sds_num].reg;
    field = sds_mode_sel[sds_num].field;

    switch (HWP_SDS_MODE(unit, sds_num))
    {
        case RTK_MII_QSGMII:
            /* serdes off */
            value = 0x1f;
            reg_field_write(unit, reg, field, &value);
            /* serdes mode QSGMII */
            value = 0x6;
            reg_field_write(unit, reg, field, &value);
            break;

        case RTK_MII_10GR:
            /* serdes off */
            value = 0x1f;
            reg_field_write(unit, reg, field, &value);
            /* serdes mode 10G Base-R */
            value = 0x1a;
            reg_field_write(unit, reg, field, &value);
            break;
       case RTK_MII_XSGMII:
            /* serdes off */
            value = 0x1f;
            reg_field_write(unit, reg, field, &value);
            /* serdes mode XSGMII */
            value = 0x10;
            reg_field_write(unit, reg, field, &value);
            break;

        case RTK_MII_10GR1000BX_AUTO:
            /* serdes off */
            value = 0x1f;
            reg_field_write(unit, reg, field, &value);
            /* serdes mode 10R1000BX Auto */
            value = 0x1b;
            reg_field_write(unit, reg, field, &value);
            break;

        case RTK_MII_RXAUI_PLUS:
            /* serdes off */
            value = 0x1f;
            reg_field_write(unit, reg, field, &value);
            /* serdes mode RXAUI+ */
            value = 0x19;
            reg_field_write(unit, reg, field, &value);
            break;

        case RTK_MII_DISABLE:
            break;
        default:
            RT_LOG(LOG_DEBUG, (MOD_HAL), "unit %u serdes %u media not found\n", unit, sds_num);
            break;

    }

    return RT_ERR_OK;
}   /* end of rtl9300_serdes_rst */

/* Function Name:
 *      rtl9300_smi_read
 * Description:
 *      Extend SMI read
 * Input:
 *      unit    - unit id
 *      smi     - SMI id
 *      phy_addr- PHY address
 *      page    - PHY page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
rtl9300_smi_read(
    uint32      unit,
    uint32      smi,
    uint32      phy_addr,
    uint32      page,
    uint32      phy_reg,
    uint32      *pData)
{
    int32       ret;
    uint32      ctrlReg = 0;

    if (smi != 4)
    {
        return RT_ERR_INPUT;
    }

    PHY_SEM_LOCK(unit);

    if ((ret = reg_read(unit, LONGAN_EXT_SMI_ACCESS_CTRLr, &ctrlReg)) != RT_ERR_OK)
    {
        goto ERR_RET;
    }

    if ((ret = _rtl9300_extSmiAccCtrlRegField_set(unit, phy_addr, 0x1F, page/*DATA*/, 1/*RW*/, 1/*CMD*/, &ctrlReg)) != RT_ERR_OK)
    {
        goto ERR_RET;
    }
    PHY_WAIT_CMD_COMPLETE(unit, LONGAN_EXT_SMI_ACCESS_CTRLr, 0x1, 0x2);

    if ((ret = _rtl9300_extSmiAccCtrlRegField_set(unit, phy_addr, phy_reg, 0/*DATA*/, 0/*RW*/, 1/*CMD*/, &ctrlReg)) != RT_ERR_OK)
    {
        goto ERR_RET;
    }
    PHY_WAIT_CMD_COMPLETE(unit, LONGAN_EXT_SMI_ACCESS_CTRLr, 0x1, 0x2);

    if ((ret = reg_field_read(unit, LONGAN_EXT_SMI_ACCESS_CTRLr, LONGAN_DATAf, pData)) != RT_ERR_OK)
    {
        goto ERR_RET;
    }

    PHY_SEM_UNLOCK(unit);
    return RT_ERR_OK;

  ERR_RET:
    PHY_SEM_UNLOCK(unit);
    return ret;
}


/* Function Name:
 *      rtl9300_smi_write
 * Description:
 *      Extend SMI write
 * Input:
 *      unit    - unit id
 *      smi     - SMI id
 *      phy_addr- PHY address
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32
rtl9300_smi_write(
    uint32      unit,
    uint32      smi,
    uint32      phy_addr,
    uint32      page,
    uint32      phy_reg,
    uint32      data)
{
    int32       ret;
    uint32      ctrlReg = 0;

    if (smi != 4)
    {
        return RT_ERR_INPUT;
    }

    PHY_SEM_LOCK(unit);

    if ((ret = reg_read(unit, LONGAN_EXT_SMI_ACCESS_CTRLr, &ctrlReg)) != RT_ERR_OK)
    {
        goto ERR_RET;
    }

    if ((ret = _rtl9300_extSmiAccCtrlRegField_set(unit, phy_addr, 0x1F, page/*DATA*/, 1/*RW*/, 1/*CMD*/, &ctrlReg)) != RT_ERR_OK)
    {
        goto ERR_RET;
    }
    PHY_WAIT_CMD_COMPLETE(unit, LONGAN_EXT_SMI_ACCESS_CTRLr, 0x1, 0x2);

    if ((ret = _rtl9300_extSmiAccCtrlRegField_set(unit, phy_addr, phy_reg, data/*DATA*/, 1/*RW*/, 1/*CMD*/, &ctrlReg)) != RT_ERR_OK)
    {
        goto ERR_RET;
    }
    PHY_WAIT_CMD_COMPLETE(unit, LONGAN_EXT_SMI_ACCESS_CTRLr, 0x1, 0x2);

    PHY_SEM_UNLOCK(unit);
    return RT_ERR_OK;

  ERR_RET:
    PHY_SEM_UNLOCK(unit);
    return ret;
}



/* RTL9300 mac driver service APIs */
rt_macdrv_t rtl9300_macdrv =
{
    .fMdrv_init                             = rtl9300_init,
    .fMdrv_miim_read                        = rtl9300_miim_read,
    .fMdrv_miim_write                       = rtl9300_miim_write,
    .fMdrv_miim_park_read                   = rtl9300_miim_park_read,
    .fMdrv_miim_park_write                  = rtl9300_miim_park_write,
    .fMdrv_miim_broadcast_write             = rtl9300_miim_broadcast_write,
    .fMdrv_miim_extParkPage_read            = rtl9300_miim_extParkPage_read,
    .fMdrv_miim_extParkPage_write           = rtl9300_miim_extParkPage_write,
    .fMdrv_miim_extParkPage_portmask_write  = rtl9300_miim_extParkPage_portmask_write,
    .fMdrv_miim_mmd_read                    = rtl9300_miim_mmd_read,
    .fMdrv_miim_mmd_write                   = rtl9300_miim_mmd_write,
    .fMdrv_miim_mmd_portmask_write          = rtl9300_miim_mmd_portmask_write,
    .fMdrv_table_read                       = rtl9300_table_read,
    .fMdrv_table_write                      = rtl9300_table_write,
    .fMdrv_port_probe                       = rtl9300_port_probe,
    .fMdrv_miim_portmask_write              = rtl9300_miim_portmask_write,
    .fMdrv_miim_pollingEnable_get           = rtl9300_miim_pollingEnable_get,
    .fMdrv_miim_pollingEnable_set           = rtl9300_miim_pollingEnable_set,
    .fMdrv_mac_serdes_rst                   = rtl9300_serdes_rst,
    .fMdrv_mac_serdes_read                  = rtl9300_drv_serdes_read,
    .fMdrv_mac_serdes_write                 = rtl9300_drv_serdes_write,
    .fMdrv_smi_read                         = rtl9300_smi_read,
    .fMdrv_smi_write                        = rtl9300_smi_write,
    .fMdrv_miim_portSmiMdxProto_get         = rtl9300_miim_portSmiMdxProto_get,
    .fMdrv_miim_portSmiMdxProto_set         = rtl9300_miim_portSmiMdxProto_set,
}; /* end of rtl9300_macdrv */


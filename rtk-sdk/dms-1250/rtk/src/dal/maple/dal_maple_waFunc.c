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
#include <hal/chipdef/maple/rtk_maple_table_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/common/miim.h>
#include <hal/common/halctrl.h>
#include <dal/dal_common.h>
#include <private/drv/swcore/swcore_rtl8380.h>
#include <ioal/mem32.h>
#include <hal/phy/phy_rtl8380.h>
#include <dal/dal_waMon.h>
#include <dal/dal_phy.h>
#include <dal/maple/dal_maple_port.h>
#include <dal/maple/dal_maple_waFunc.h>

#if defined(CONFIG_SDK_RTL8214QF)
  #include <hal/phy/phy_rtl8295.h>
#endif

/*Auto Recovery Debug Counter*/
extern uint32 pktBuf_watchdog_cnt[];
extern uint32 macSerdes_watchdog_cnt[];
extern uint32 phy_watchdog_cnt[];

extern int32 dal_waMon_phyReconfig_portMaskSet(uint32 unit, rtk_port_t port);

/************************************ WATCH DOG For Recovery****************************************/

static int32
_dal_maple_swQueRst_set(uint32 unit)
{
    ioal_mem32_field_write(unit, RTL8380_RST_GLB_CTRL_0_ADDR, RTL8380_RST_GLB_CTRL_0_SW_Q_RST_OFFSET, RTL8380_RST_GLB_CTRL_0_SW_Q_RST_MASK, 1);
    osal_time_usleep(50 * 1000); /* delay 50mS */

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_maple_mac_serdes_rst
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
static int32
_dal_maple_mac_serdes_rst(uint32  unit, uint32 sds_num)
{
    int32 ret;
    uint32 val;

    /*rx reset*/
    ioal_mem32_read(unit, 0xF3A4+sds_num*0x100, &val);
    val |= 1UL<<9;
    ioal_mem32_write(unit, 0xF3A4+sds_num*0x100, val);
    val &= ~(1UL<<9);
    ioal_mem32_write(unit, 0xF3A4+sds_num*0x100, val);

    /*cmu reset*/
    val = 0x4040;
    ioal_mem32_write(unit, 0xF380+sds_num*0x100, val);
    val = 0x4740;
    ioal_mem32_write(unit, 0xF380+sds_num*0x100, val);
    val = 0x47c0;
    ioal_mem32_write(unit, 0xF380+sds_num*0x100, val);
    val = 0x4000;
    ioal_mem32_write(unit, 0xF380+sds_num*0x100, val);

    /*software reset*/
    val = 0x7146;
    ioal_mem32_write(unit, 0xE78C+sds_num*0x200, val);
    val = 0x7106;
    ioal_mem32_write(unit, 0xE78C+sds_num*0x200, val);

    /*tx & rx reset*/
    ioal_mem32_read(unit, 0xE780+sds_num*0x200, &val);
    val &= ~(3UL << 0);
    ioal_mem32_write(unit, 0xE780+sds_num*0x200, val);
    val |= (3UL << 0);
    ioal_mem32_write(unit, 0xE780+sds_num*0x200, val);

    ret = RT_ERR_OK;
    return ret;
}   /* end of _dal_maple_mac_serdes_rst */





/* Function Name:
 *      _dal_maple_port_phy_counter_clr_counter
 * Description:
 *      Phy counter monitor to clear phy counter.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is used to to clear phy counter of specified port.
 */
static int32 _dal_maple_port_phy_counter_clr_counter(uint32 unit, rtk_port_t port)
{
    int32 ret;
    uint32 phy_data;

    int32   phy_id;

    /* Get phyID */
    phy_drv_chk(unit, port, &phy_id);

    if (HWP_8380_FAMILY_ID(unit))
    {
        /*8218B & 8214FC & 8214C*/
        /*It is better to check PHYID*/
        phy_data = 0x70;
        ret = dal_phy_portReg_set(unit,  port,  0xc80,  17, phy_data);
        if(ret != RT_ERR_OK)
            return ret;
    }

    if (HWP_8330_FAMILY_ID(unit))
    {
        /*It is better to check PHYID*/
        if(port<=7)
        {
             /*8208*/
             /*While clear, phyid should be 0*/
            ret = dal_phy_portReg_get(unit,  0,  65,  20, &phy_data);
            if(ret != RT_ERR_OK)
                return ret;

            phy_data &= ~(1UL<<port);

            ret = dal_phy_portReg_set(unit,  0,  65,  20, phy_data);
            if(ret != RT_ERR_OK)
                return ret;

            ret = dal_phy_portReg_get(unit,  0,  65,  20, &phy_data);
            if(ret != RT_ERR_OK)
                return ret;

            phy_data |= (1UL<<port);

            ret = dal_phy_portReg_set(unit,  0,  65,  20, phy_data);
            if(ret != RT_ERR_OK)
                return ret;
        }
        else if((port>=8)&&(port<=15))
        {
            /*Internal phy*/
            phy_data = 0x70;
            ret = dal_phy_portReg_set(unit,  port,  0xc80,  17, phy_data);
            if(ret != RT_ERR_OK)
                return ret;
        }
        else if((port>=16)&&(port<=23))
        {
            /*08L*/
             /*While clear, phyid should be 16*/
            ret = dal_phy_portReg_get(unit,  16,  65,  20, &phy_data);
            if(ret != RT_ERR_OK)
                return ret;

            phy_data &= ~(1UL<<(port-16));

            ret = dal_phy_portReg_set(unit,  16,  65,  20, phy_data);
            if(ret != RT_ERR_OK)
                return ret;

            ret = dal_phy_portReg_get(unit,  16,  65,  20, &phy_data);
            if(ret != RT_ERR_OK)
                return ret;

            phy_data |= (1UL<<(port-16));

            ret = dal_phy_portReg_set(unit,  16,  65,  20, phy_data);
            if(ret != RT_ERR_OK)
                return ret;
        }
        else  if((port>=24)&&(port<=27))
        {
            if(phy_id == RT_PHYDRV_RTL8214FC_MP || phy_id == RT_PHYDRV_RTL8214C)
            {
                /*8218B & 8214FC*/
                /*It is better to check PHYID*/
                phy_data = 0x70;
                ret = dal_phy_portReg_set(unit,  port,  0xc80,  17, phy_data);
                if(ret != RT_ERR_OK)
                    return ret;
            }
            else if(phy_id == RT_PHYDRV_RTL8214B || phy_id == RT_PHYDRV_RTL8212B || phy_id == RT_PHYDRV_RTL8214FB)
            {
                /*8214B or 8212B*/
                phy_data = 0x4012;
                ret = dal_phy_portReg_set(unit,  port,  6,  1, phy_data);
                if(ret != RT_ERR_OK)
                    return ret;
            }
            else
            {
                return  RT_ERR_OK;
            }
        }
    }

    ret =  RT_ERR_OK;
    return ret;
}



/************************************PKTBUFFER WATCH DOG****************************************/
/************************************PKTBUFFER WATCH DOG--->PHY Counter Check********************/
/* Function Name:
 *      _dal_maple_port_phy_counter_frcclk_off
 * Description:
 *      Phy counter monitor force clock off.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is used to turn off force tx clk of specified port.
 */
static int32 _dal_maple_port_phy_counter_frcclk_off(uint32 unit, rtk_port_t port)
{
    int32 ret;
    uint32 phy_data;

    int32   phy_id;

    /* Get phyID */
    phy_drv_chk(unit, port, &phy_id);

    /*838x*/
    if (HWP_8380_FAMILY_ID(unit))
    {
        /*18b/18fb/14FC/14C*/
        ret = dal_phy_portReg_get(unit,  port,  0xc40,  21, &phy_data);
        if(ret != RT_ERR_OK)
            return ret;

        phy_data &= ~(1UL<<13);

        ret = dal_phy_portReg_set(unit,  port,  0xc40,  21, phy_data);
        if(ret != RT_ERR_OK)
            return ret;
    }

    /*833x*/
    if (HWP_8330_FAMILY_ID(unit))
    {
        /*18b internal phy*/
        if((port>=8)&&(port<=15))
        {  /*It is better to check PHYID*/
            ret = dal_phy_portReg_get(unit,  port,  0xc40,  21, &phy_data);
            if(ret != RT_ERR_OK)
                return ret;

            phy_data &= ~(1UL<<13);

            ret = dal_phy_portReg_set(unit,  port,  0xc40,  21, phy_data);
            if(ret != RT_ERR_OK)
                return ret;
        }
        if((port>=24)&&(port<=27))
        {
            if(phy_id == RT_PHYDRV_RTL8214FC_MP || phy_id == RT_PHYDRV_RTL8214C)
            {
                ret = dal_phy_portReg_get(unit,  port,  0xc40,  21, &phy_data);
                if(ret != RT_ERR_OK)
                    return ret;

                phy_data &= ~(1UL<<13);

                ret = dal_phy_portReg_set(unit,  port,  0xc40,  21, phy_data);
                if(ret != RT_ERR_OK)
                    return ret;
            }
        }
    }

    ret =  RT_ERR_OK;
    return ret;
}

/* Function Name:
 *      _dal_maple_port_phy_counter_monitor_begin
 * Description:
 *      Phy counter monitor force clock on.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is used to turn on force tx clk of specified port.
 */
static int32 _dal_maple_port_phy_counter_monitor_begin(uint32 unit, rtk_port_t port, uint8 speed)
{
    int32 ret;
    uint32 phy_data;
    int32   phy_id;

    /* Get phyID */
    phy_drv_chk(unit, port, &phy_id);

    /*838X*/
    if (HWP_8380_FAMILY_ID(unit))
    {
        if(speed == 0x1)
        {
            /*A: writephy  portID  reg_21[5:3]  page:0xc40  data:4 */
            ret = dal_phy_portReg_get(unit,  port,  0xc40,  21, &phy_data);
            if(ret != RT_ERR_OK)
                return ret;
            phy_data &= ~(0x7UL<<3);
            phy_data |= 0x4UL<<3;
            ret = dal_phy_portReg_set(unit,  port,  0xc40,  21, phy_data);
            if(ret != RT_ERR_OK)
                return ret;

           /* B: writephy  portID  reg_21[13]  page:0xc40  data:1 */
            ret = dal_phy_portReg_get(unit,  port,  0xc40,  21, &phy_data);
            if(ret != RT_ERR_OK)
                return ret;
            phy_data &= ~(0x1UL<<13);
            phy_data |= 0x1UL<<13;
            ret = dal_phy_portReg_set(unit,  port,  0xc40,  21, phy_data);
            if(ret != RT_ERR_OK)
                return ret;

            /* C:Clear Counter writephy  portID  reg_17  page:0xc80  data:0x70 */
            phy_data = 0x70;
            ret = dal_phy_portReg_set(unit,  port,  0xc80,  17, phy_data);
            if(ret != RT_ERR_OK)
                return ret;
        }
        else if(speed == 0x0)
        {
             /* A: writephy  portID  reg_21[5:3]  page:0xc40  data:5 */
            ret = dal_phy_portReg_get(unit,  port,  0xc40,  21, &phy_data);
            if(ret != RT_ERR_OK)
                return ret;
            phy_data &= ~(0x7UL<<3);
            phy_data |= 0x5UL<<3;
            ret = dal_phy_portReg_set(unit,  port,  0xc40,  21, phy_data);
            if(ret != RT_ERR_OK)
                return ret;

           /* B: writephy  portID  reg_21[13]  page:0xc40  data:1 */
            ret = dal_phy_portReg_get(unit,  port,  0xc40,  21, &phy_data);
            if(ret != RT_ERR_OK)
                return ret;
            phy_data &= ~(0x1UL<<13);
            phy_data |= 0x1UL<<13;
            ret = dal_phy_portReg_set(unit,  port,  0xc40,  21, phy_data);
            if(ret != RT_ERR_OK)
                return ret;

            /* C:Clear Counter writephy  portID  reg_17  page:0xc80  data:0x70 */
            phy_data = 0x70;
            ret = dal_phy_portReg_set(unit,  port,  0xc80,  17, phy_data);
            if(ret != RT_ERR_OK)
                return ret;
        }
        else
        {
            _dal_maple_port_phy_counter_frcclk_off(unit, port);
            _dal_maple_port_phy_counter_clr_counter(unit, port);
        }
    }

    /*833X*/
    if (HWP_8330_FAMILY_ID(unit))
    {
        /*Set internal PHY force clk*/
        if((port>=8)&&(port<=15))
        {
            if(speed == 0x1)
            {
                /*A: writephy  portID  reg_21[5:3]  page:0xc40  data:4 */
                ret = dal_phy_portReg_get(unit,  port,  0xc40,  21, &phy_data);
                if(ret != RT_ERR_OK)
                    return ret;
                phy_data &= ~(0x7UL<<3);
                phy_data |= 0x4UL<<3;
                ret = dal_phy_portReg_set(unit,  port,  0xc40,  21, phy_data);
                if(ret != RT_ERR_OK)
                    return ret;

               /* B: writephy  portID  reg_21[13]  page:0xc40  data:1 */
                ret = dal_phy_portReg_get(unit,  port,  0xc40,  21, &phy_data);
                if(ret != RT_ERR_OK)
                    return ret;
                phy_data &= ~(0x1UL<<13);
                phy_data |= 0x1UL<<13;
                ret = dal_phy_portReg_set(unit,  port,  0xc40,  21, phy_data);
                if(ret != RT_ERR_OK)
                    return ret;

            }
            else if(speed == 0x0)
            {
                 /* A: writephy  portID  reg_21[5:3]  page:0xc40  data:5 */
                ret = dal_phy_portReg_get(unit,  port,  0xc40,  21, &phy_data);
                if(ret != RT_ERR_OK)
                    return ret;
                phy_data &= ~(0x7UL<<3);
                phy_data |= 0x5UL<<3;
                ret = dal_phy_portReg_set(unit,  port,  0xc40,  21, phy_data);
                if(ret != RT_ERR_OK)
                    return ret;

               /* B: writephy  portID  reg_21[13]  page:0xc40  data:1 */
                ret = dal_phy_portReg_get(unit,  port,  0xc40,  21, &phy_data);
                if(ret != RT_ERR_OK)
                    return ret;
                phy_data &= ~(0x1UL<<13);
                phy_data |= 0x1UL<<13;
                ret = dal_phy_portReg_set(unit,  port,  0xc40,  21, phy_data);
                if(ret != RT_ERR_OK)
                    return ret;
            }

            ret = _dal_maple_port_phy_counter_clr_counter(unit, port);
            if(ret != RT_ERR_OK)
                return ret;
        }
        else if((port>=24)&&(port<=27))
        {
            if(phy_id == RT_PHYDRV_RTL8214FC_MP || phy_id == RT_PHYDRV_RTL8214C)
            {
                if(speed == 0x1)
                {
                    /*A: writephy  portID  reg_21[5:3]  page:0xc40  data:4 */
                    ret = dal_phy_portReg_get(unit,  port,  0xc40,  21, &phy_data);
                    if(ret != RT_ERR_OK)
                        return ret;
                    phy_data &= ~(0x7UL<<3);
                    phy_data |= 0x4UL<<3;
                    ret = dal_phy_portReg_set(unit,  port,  0xc40,  21, phy_data);
                    if(ret != RT_ERR_OK)
                        return ret;

                   /* B: writephy  portID  reg_21[13]  page:0xc40  data:1 */
                    ret = dal_phy_portReg_get(unit,  port,  0xc40,  21, &phy_data);
                    if(ret != RT_ERR_OK)
                        return ret;
                    phy_data &= ~(0x1UL<<13);
                    phy_data |= 0x1UL<<13;
                    ret = dal_phy_portReg_set(unit,  port,  0xc40,  21, phy_data);
                    if(ret != RT_ERR_OK)
                        return ret;

                    /* C:Clear Counter writephy  portID  reg_17  page:0xc80  data:0x70 */
                    phy_data = 0x70;
                    ret = dal_phy_portReg_set(unit,  port,  0xc80,  17, phy_data);
                    if(ret != RT_ERR_OK)
                        return ret;
                }
                else if(speed == 0x0)
                {
                     /* A: writephy  portID  reg_21[5:3]  page:0xc40  data:5 */
                    ret = dal_phy_portReg_get(unit,  port,  0xc40,  21, &phy_data);
                    if(ret != RT_ERR_OK)
                        return ret;
                    phy_data &= ~(0x7UL<<3);
                    phy_data |= 0x5UL<<3;
                    ret = dal_phy_portReg_set(unit,  port,  0xc40,  21, phy_data);
                    if(ret != RT_ERR_OK)
                        return ret;

                   /* B: writephy  portID  reg_21[13]  page:0xc40  data:1 */
                    ret = dal_phy_portReg_get(unit,  port,  0xc40,  21, &phy_data);
                    if(ret != RT_ERR_OK)
                        return ret;
                    phy_data &= ~(0x1UL<<13);
                    phy_data |= 0x1UL<<13;
                    ret = dal_phy_portReg_set(unit,  port,  0xc40,  21, phy_data);
                    if(ret != RT_ERR_OK)
                        return ret;

                    /* C:Clear Counter writephy  portID  reg_17  page:0xc80  data:0x70 */
                    phy_data = 0x70;
                    ret = dal_phy_portReg_set(unit,  port,  0xc80,  17, phy_data);
                    if(ret != RT_ERR_OK)
                        return ret;
                }
                else
                {
                    _dal_maple_port_phy_counter_frcclk_off(unit, port);
                    _dal_maple_port_phy_counter_clr_counter(unit, port);
                }
            }
            else
            {
               ret = _dal_maple_port_phy_counter_clr_counter(unit, port);
                if(ret != RT_ERR_OK)
                    return ret;
            }
        }
    }

    ret =  RT_ERR_OK;
    return ret;
}


/* Function Name:
 *      _dal_maple_port_phy_counter_monitor_end
 * Description:
 *      Phy counter monitor to clear phy counter.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is used to to clear phy counter of specified port.
 */
static int32 _dal_maple_port_phy_counter_monitor_end(uint32 unit, rtk_port_t port)
{
    int32 ret;
    uint32 phy_data;
    rtk_port_t port_id;

    int32   phy_id;

    /* Get phyID */
    phy_drv_chk(unit, port, &phy_id);

    if (HWP_8380_FAMILY_ID(unit))
    {
        phy_data = 0;

        /*18b/18fb/14FC/14C*/
        /*Read  PKTGEN RXERR CNT*/
        ret = dal_phy_portReg_get(unit,  port,  0xc81,  18, &phy_data);
        if(ret != RT_ERR_OK)
            return ret;

        /*If out of range means TX CRC Error*/
         if(phy_data > 100)
         {
            pktBuf_watchdog_cnt[unit]++;
            if(pktBuf_watchdog_cnt[unit] >= 64)
                pktBuf_watchdog_cnt[unit] = 0;

            _dal_maple_swQueRst_set(unit);

            /* Because of Queue Reset, Clear All port counters*/
            for(port_id = 0; port_id < 28; port_id++)
            {
                if(!HWP_PHY_EXIST(unit, port_id))
                    continue;

                 if (HWP_IS_CPU_PORT(unit, port_id) || HWP_SERDES_PORT(unit, port_id))
                    continue;

                    _dal_maple_port_phy_counter_clr_counter(unit, port_id);
            }
         }
    }

    if (HWP_8330_FAMILY_ID(unit))
    {
        phy_data = 0;

        /*It is better to check PHYID*/
        if((port<=7) || ((port>=16)&&(port<=23)))
        {
            /*Read Counter*/
            ret = dal_phy_portReg_get(unit,  port,  65,  24, &phy_data);
            if(ret != RT_ERR_OK)
                return ret;
        }
        else if((port>=8)&&(port<=15))
        {
            /*Read  PKTGEN RXERR CNT*/
            ret = dal_phy_portReg_get(unit,  port,  0xc81,  18, &phy_data);
            if(ret != RT_ERR_OK)
                return ret;
        }
        else if((port>=24)&&(port<=27))
        {
            if(phy_id == RT_PHYDRV_RTL8214FC_MP || phy_id == RT_PHYDRV_RTL8214C)
            {
                /*Read  PKTGEN RXERR CNT*/
                ret = dal_phy_portReg_get(unit,  port,  0xc81,  18, &phy_data);
                if(ret != RT_ERR_OK)
                    return ret;
            }
            else if(phy_id == RT_PHYDRV_RTL8214B || phy_id == RT_PHYDRV_RTL8212B || phy_id == RT_PHYDRV_RTL8214FB)
            {
                /*8214B or 8212B*/
                ret = dal_phy_portReg_get(unit,  port,  6,  9, &phy_data);
                if(ret != RT_ERR_OK)
                    return ret;
            }
            else
            {
                return RT_ERR_OK;
            }
        }

         if(phy_data > 100)
         {
            pktBuf_watchdog_cnt[unit]++;
            if(pktBuf_watchdog_cnt[unit] >= 64)
                pktBuf_watchdog_cnt[unit] = 0;

            _dal_maple_swQueRst_set(unit);

            /* Because of Queue Reset, Clear All port counters*/
            for(port_id = 0; port_id < 28; port_id++)
            {
                if(!HWP_PHY_EXIST(unit, port_id))
                    continue;

                 if (HWP_IS_CPU_PORT(unit, port_id) || HWP_SERDES_PORT(unit, port_id))
                    continue;

                    _dal_maple_port_phy_counter_clr_counter(unit, port_id);
            }
         }
    }

    ret =  RT_ERR_OK;
    return ret;
}


/* Function Name:
 *      _dal_maple_port_phy_counter_check
 * Description:
 *      Phy counter monitor phy counter.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is used to to check phy counter of specified port.
 */
static uint8 link_pre_sts[RTK_MAX_NUM_OF_UNIT][28];
static uint8 link_pre_spd[RTK_MAX_NUM_OF_UNIT][28];
static uint8 link_pre_dpx[RTK_MAX_NUM_OF_UNIT][28];
static uint8 link_pre_mda[RTK_MAX_NUM_OF_UNIT][28];
int32 _dal_maple_port_phy_counter_check(uint32 unit)
{
    int32 ret;
    rtk_port_t port;

    //uint32 phy_data;
    uint32 reg_val;

    uint8 link_sts0,link_sts1;
    uint32 speed_val;
    uint8 speed;
    uint8 duplex;
    uint32 link_media;

    uint8 link_temp;
    uint8 spd_temp;
    uint8 dpx_temp;
    uint8 media_temp;

    int32   phy_id;

     /*Solution:
        Note0: must care about linkmon&walmon race condition
        Note1: only confined to status linkup on full duplex state
        1: First time detected linkup, clear counter & force clk;
        2: Second time detected linkup, check counter whether out of range
            if Yes, do queue reset operation*/
    for(port = 0; port < 28; port++)
    {
        /* Get phyID */
        phy_drv_chk(unit, port, &phy_id);

        if(!HWP_PHY_EXIST(unit, port))
            continue;

         if (HWP_IS_CPU_PORT(unit, port) || HWP_SERDES_PORT(unit, port))
            continue;

        if(RT_PHYDRV_RTL8214QF == phy_id)
            continue;

        /*Link status First Time Read*/
        if ((ret = reg_field_read(unit, MAPLE_MAC_LINK_STSr, MAPLE_LINK_STS_27_0f, &reg_val)) != RT_ERR_OK)
            return ret;
        link_sts0 = (reg_val >> port) & 0x1;

        /*Link status Second Time Read*/
        if ((ret = reg_field_read(unit, MAPLE_MAC_LINK_STSr, MAPLE_LINK_STS_27_0f, &reg_val)) != RT_ERR_OK)
            return ret;
        link_sts1 = (reg_val >> port) & 0x1;

        /*Link Speed*/
        if ((ret = reg_array_field_read(unit, MAPLE_MAC_LINK_SPD_STSr, port, REG_ARRAY_INDEX_NONE, \
                    MAPLE_SPD_STS_27_0f,&speed_val)) != RT_ERR_OK)
            return ret;
        speed = speed_val & 0x3;

        /*Link Duplex*/
        if ((ret = reg_field_read(unit, MAPLE_MAC_LINK_DUP_STSr, MAPLE_DUP_STS_27_0f, &reg_val)) != RT_ERR_OK)
            return ret;
        duplex = (reg_val >> port) & 0x1;

        /*Link Media*/
        if ((ret = reg_field_read(unit, MAPLE_MAC_LINK_MEDIA_STSr, MAPLE_MEDIA_STS_27_0f, &reg_val)) != RT_ERR_OK)
            return ret;
        link_media = (reg_val >> port) & 0x1;

        /*Backup previous status*/
        link_temp =  link_pre_sts[unit][port];
        spd_temp = link_pre_spd[unit][port];
        dpx_temp = link_pre_dpx[unit][port];
        media_temp = link_pre_mda[unit][port];

         /*Save link status*/
         link_pre_sts[unit][port] = link_sts1;
        link_pre_spd[unit][port] = speed;
        link_pre_dpx[unit][port] = duplex;
        link_pre_mda[unit][port] = link_media;

        /*Current Port Link down*/
        if(link_sts1 == 0)
        {
            if(link_temp == 1)
            {
                ret = _dal_maple_port_phy_counter_frcclk_off(unit, port); /*Close Force Clock*/
                if(ret != RT_ERR_OK)
                    return ret;
            }
        }
        else/*Current Port Link up*/
        {
            /*If link status changes, taken as latch low*/
            if(link_temp == 0)
            {
                link_sts0 = 0;
            }
            else
            {
                 /*If speed or duplex or media change, taken as latch low*/
                if((spd_temp!= speed) || (dpx_temp != duplex) || (media_temp != link_media))
                {
                    link_sts0 = 0;
                }
            }

            if(link_sts0 == 0)
            {
                 /*Half Duplex or Fiber mode, donnot care*/
                 if((0 == duplex) || (0x1 == link_media))
                 {
                    ret = _dal_maple_port_phy_counter_frcclk_off(unit, port); /*Close Force Clock*/
                    if(ret != RT_ERR_OK)
                        return ret;
                 }
                 else/*Full Duplex & Copper*/
                 {
                    /*First time detected*/
                    ret = _dal_maple_port_phy_counter_monitor_begin(unit, port, speed);
                    if(ret != RT_ERR_OK)
                        return ret;
                 }
            }
            else
            {
                 if((0 != duplex) && (0x1 != link_media))
                 {
                    /*Second Time detected*/
                    ret = _dal_maple_port_phy_counter_monitor_end(unit, port);
                    if(ret != RT_ERR_OK)
                        return ret;
                 }
            }
        }
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      dal_maple_port_pktbuf_watchdog
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
dal_maple_port_pktbuf_watchdog(uint32 unit)
{
    int32 ret;

    /* check Init status */
    if ((ret = dal_maple_port_init_check(unit)) != RT_ERR_OK)
        return ret;

    if ((ret = dal_maple_port_sem_lock(unit)) != RT_ERR_OK)
        return ret;

    /***************************1: PHY  Counter Check Start********************/
    if((ret = _dal_maple_port_phy_counter_check(unit))!= RT_ERR_OK)
    {
        if ((ret = dal_maple_port_sem_unlock(unit)) != RT_ERR_OK)
            return ret;
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    /***************************1: PHY  Counter Check End*********************/

    if ((ret = dal_maple_port_sem_unlock(unit)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}/* end of dal_maple_port_pktbuf_watchdog */

/* Function Name:
 *      _dal_maple_phy_serdes_rst
 * Description:
 *      Reset PHY Serdes.
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
static int32
_dal_maple_phy_serdes_rst(uint32  unit, uint32 sds_num)
{
    int32 ret;
    uint32 port;
    uint32 macID;

    int32 phy_id;

    if(sds_num >= 6)
        return RT_ERR_FAILED;

    if((macID = HWP_SDS_ID2MACID(unit, sds_num)) == HWP_NONE)
        return RT_ERR_FAILED;

    if((port = HWP_PHY_BASE_MACID(unit, macID)) == HWP_NONE)
        return RT_ERR_FAILED;

    phy_drv_chk(unit, port, &phy_id);

    if((phy_id == RT_PHYDRV_RTL8214FC_MP) || (phy_id == RT_PHYDRV_RTL8218B))
    {
        /*Reset 18b or 14fc PHY Serdes*/
        dal_phy_portReg_set(unit,  port,  0x0000, 0x1e, 0x0008);
        dal_phy_portReg_set(unit,  port,  0x0464,  0x17, 0x84f5);
        dal_phy_portReg_set(unit,  port,  0x0464,  0x17, 0x04f5);

        dal_phy_portReg_set(unit,  port,  0x042D,  0x11, 0xC015);
        dal_phy_portReg_set(unit,  port,  0x042D,  0x11, 0xC014);

        dal_phy_portReg_set(unit,  port,  0x0467,  0x14, 0x1415);
        dal_phy_portReg_set(unit,  port,  0x0467,  0x14, 0x3C3D);
        dal_phy_portReg_set(unit,  port,  0x0467,  0x14, 0x3C3F);
        dal_phy_portReg_set(unit,  port,  0x0467,  0x14, 0x0000);

        dal_phy_portReg_set(unit,  port,  0x0261,  0x10, 0x6000);
        dal_phy_portReg_set(unit,  port,  0x0261,  0x10, 0x0000);

        dal_phy_portReg_set(unit,  port,  0x0404,  0x13, 0x7146);
        dal_phy_portReg_set(unit,  port,  0x0404,  0x13, 0x7106);

        dal_phy_portReg_set(unit,  port,  0x0424,  0x13, 0x7146);
        dal_phy_portReg_set(unit,  port,  0x0424,  0x13, 0x7106);

        dal_phy_portReg_set(unit,  port,  0x0000,  0x1e, 0x0000);
    }
    if(phy_id == RT_PHYDRV_RTL8214C)
    {
        /*Reset 14c PHY Serdes*/
        //#sds rx reset: reg_en_self=1-->0(page.21,rg02[7])
        dal_phy_portReg_set(unit,  port,  0x0000,  29, 0x6602);
        dal_phy_portReg_set(unit,  port,  0x0000,  30, 0xb682);
        dal_phy_portReg_set(unit,  port,  0x0000,  29, 0x6601);
        dal_phy_portReg_set(unit,  port,  0x0000,  30, 0x0422);
        dal_phy_portReg_set(unit,  port,  0x0000,  29, 0x6600);
        dal_phy_portReg_set(unit,  port,  0x0000,  30, 0x00C0);

        osal_time_usleep(10000);

        dal_phy_portReg_set(unit,  port,  0x0000,  29, 0x6602);
        dal_phy_portReg_set(unit,  port,  0x0000,  30, 0xb602);
        dal_phy_portReg_set(unit,  port,  0x0000,  29, 0x6601);
        dal_phy_portReg_set(unit,  port,  0x0000,  30, 0x0422);
        dal_phy_portReg_set(unit,  port,  0x0000,  29, 0x6600);
        dal_phy_portReg_set(unit,  port,  0x0000,  30, 0x00C0);

        //#digital soft reset: soft_rst=1-->0(page.0,sds_reg03[6])
        dal_phy_portReg_set(unit,  port,  0x0000,  29, 0x6602);
        dal_phy_portReg_set(unit,  port,  0x0000,  30, 0x7146);
        dal_phy_portReg_set(unit,  port,  0x0000,  29, 0x6601);
        dal_phy_portReg_set(unit,  port,  0x0000,  30, 0x0003);
        dal_phy_portReg_set(unit,  port,  0x0000,  29, 0x6600);
        dal_phy_portReg_set(unit,  port,  0x0000,  30, 0x00C0);

        osal_time_usleep(10000);

        dal_phy_portReg_set(unit,  port,  0x0000,  29, 0x6602);
        dal_phy_portReg_set(unit,  port,  0x0000,  30, 0x7106);
        dal_phy_portReg_set(unit,  port,  0x0000,  29, 0x6601);
        dal_phy_portReg_set(unit,  port,  0x0000,  30, 0x0003);
        dal_phy_portReg_set(unit,  port,  0x0000,  29, 0x6600);
        dal_phy_portReg_set(unit,  port,  0x0000,  30, 0x00C0);
    }
#if defined(CONFIG_SDK_RTL8214QF)
    if (phy_id == RT_PHYDRV_RTL8214QF)
    {
        _dal_phy_macIntfSerdes_reset(unit, port);
    }
#endif

    ret = RT_ERR_OK;
    return ret;
}   /* end of _dal_maple_phy_serdes_rst */
/************************************SERDES WATCH DOG****************************************/
/* Function Name:
 *      _dal_maple_port_serdes_status_mac_action
 * Description:
 *      When serdes link down or up mac do action.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */

/************************************SERDES WATCH DOG****************************************/
static void
_dal_maple_port_serdes_status_mac_action(uint32 unit)
{
    uint32 val = 0; 
    uint32 val1 = 0; 
    uint32 val2 = 0; 
    int32 port_idx = 0;
    uint32 reg_val = 0;
    static int32 force_done = 0;


    ioal_mem32_read(unit,0x5c,&reg_val); 
    for(port_idx = 24; port_idx <= 26; port_idx +=2)
    {        
        if((0x2 != (reg_val & 0x7))&&(24 == port_idx))
        {  
            if(1 == (force_done & 0x1))
            {
                ioal_mem32_read(unit, 0xa164, &val);
                val = (val & (0xfffffffe));
                ioal_mem32_write(unit, 0xa164,val);
                force_done = (force_done & 0x2);
            }
            continue; 
        }
        if((0x2 != ((reg_val >> 3) & 0x7))&&(26 == port_idx))
        {  
           if(1 == ((force_done & 0x2) >> 1))
           {
                ioal_mem32_read(unit, 0xa16c, &val);
                val = (val & (0xfffffffe));
                ioal_mem32_write(unit, 0xa16c,val);
                force_done = (force_done & 0x1);
           }
           continue; 
        }
        if(24 == port_idx)
        {
            ioal_mem32_read(unit, 0xf7f4, &val1);
            if(0x111 != (val1 & 0x1ff))
            {
                ioal_mem32_read(unit, 0xa164, &val2);
                val2 = (val2 | 0x1);
                val2 = (val2 & (0xfffffffd));
                ioal_mem32_write(unit, 0xa164,val2);
                force_done = (force_done | 0x1);
            }else
            {
                ioal_mem32_read(unit, 0xa164, &val2);
                val2 = (val2 & (0xfffffffe));
                ioal_mem32_write(unit, 0xa164,val2);
            }
        }else
        {
            ioal_mem32_read(unit, 0xf8f4, &val1);
            if(0x111 != (val1 & 0x1ff))
            {
                ioal_mem32_read(unit, 0xa16c, &val2);
                val2 = (val2 | 0x1);
                val2 = (val2 & (0xfffffffd));
                ioal_mem32_write(unit, 0xa16c,val2);
                force_done = (force_done | 0x2);
            }else
            {
                ioal_mem32_read(unit, 0xa16c, &val2);
                val2 = (val2 & (0xfffffffe));
                ioal_mem32_write(unit, 0xa16c,val2);
            }
        }
    }
    return;
}



/************************************SERDES WATCH DOG****************************************/
/* Function Name:
 *      _dal_maple_port_serdes_linkdown_check
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
static uint32 sds_rst_flag[]={0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static int32
_dal_maple_port_serdes_linkdown_check(uint32 unit)
{
    int32 ret;
    int32 sds_mde;
    uint32 val;
    uint32 value0,value1,value2;
    uint32 sds_idx;
    uint32 sds_sts[]={0xF3F4, 0xF4F4, 0xF5F4, 0xF6F4, 0xF7F4, 0xF8F4};

    if (HWP_8380_FAMILY_ID(unit))
    {
        /*Check link status*/
        for(sds_idx = 0; sds_idx<sizeof(sds_sts)/sizeof(uint32); sds_idx++)
        {
            /*Check Serdes use or not?*/
            if(RTK_PHYTYPE_NONE  == HWP_SDS_ID2PHYMODEL(unit, sds_idx))
                continue;

            if(RTK_PHYTYPE_SERDES  == HWP_SDS_ID2PHYMODEL(unit, sds_idx))
                continue;

            /*Check sds0-5 mode*/
            ioal_mem32_read(unit, 0x28, &val);
            if(sds_idx == 0x5)
            {
                continue;
            }
            else if(sds_idx == 0x4)
            {
                sds_mde = (val >> 5) & 0x1F;
            }
            else if(sds_idx == 0x3)
            {
                sds_mde = (val >> 10) & 0x1F;
            }
            else if(sds_idx == 0x2)
            {
                sds_mde = (val >> 15) & 0x1F;
            }
            else if(sds_idx == 0x1)
            {
                sds_mde = (val >> 20) & 0x1F;
            }
            else if(sds_idx == 0x0)
            {
                sds_mde = (val >> 25) & 0x1F;
            }
            else
            {
                sds_mde = 0x0;
            }
            /*Only process 5G-QSGMII*/
            if(sds_mde != 0x6)
                continue;

            /*5G Serdes mode*/
            ioal_mem32_read(unit, sds_sts[sds_idx], &value0);
            ioal_mem32_read(unit, sds_sts[sds_idx], &value1);
            ioal_mem32_read(unit, sds_sts[sds_idx], &value2);
            if((0x1FF == value1) && (0x1FF == value2))
            {
                /*Serdes linkup, work right!*/
                sds_rst_flag[sds_idx] = 0;
            }
            else
            {
                /*Serdes linkdown, work wrong!*/
                if(sds_rst_flag[sds_idx] == 0)
                {
                    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "[SERDES-WATCHDOG] --->Serdes Link Down--->5G-Serdes[%d]!\n", sds_idx);

                    /*First linkdown, work wrong!*/
                    _dal_maple_mac_serdes_rst(unit, sds_idx);
                    sds_rst_flag[sds_idx]++;
                    macSerdes_watchdog_cnt[unit]++;
                    if(macSerdes_watchdog_cnt[unit] >= 64)
                        macSerdes_watchdog_cnt[unit] = 0;
                }
                else
                {
                    sds_rst_flag[sds_idx]++;
                    if(sds_rst_flag[sds_idx] == 5)
                    {
                        /*Not expected case occurs, Reset PHY  & MAC serdes*/
                        sds_rst_flag[sds_idx] = 1;
                        _dal_maple_mac_serdes_rst(unit, sds_idx);
                        _dal_maple_phy_serdes_rst(unit, sds_idx);
                        macSerdes_watchdog_cnt[unit]++;
                        if(macSerdes_watchdog_cnt[unit] >= 64)
                            macSerdes_watchdog_cnt[unit] = 0;
                    }
                }
            }
        }
    }
    else if (HWP_8330_FAMILY_ID(unit))
    {
        /*Check link status*/
        for(sds_idx = 0; sds_idx<sizeof(sds_sts)/sizeof(uint32); sds_idx++)
        {
            /*Check Serdes use or not?*/
            if(RTK_PHYTYPE_NONE  == HWP_SDS_ID2PHYMODEL(unit, sds_idx))
                continue;

            if(RTK_PHYTYPE_SERDES  == HWP_SDS_ID2PHYMODEL(unit, sds_idx))
                continue;

           /*Check sds0-5 mode*/
            ioal_mem32_read(unit, 0x28, &val);
            if(sds_idx == 0x5)
            {
                /*2.5G Serdes mode-RSGMII*/
                sds_mde = (val >> 0) & 0x1F;
                if(0x1 != sds_mde)
                    continue;
            }
            else if(sds_idx == 0x4)
            {
                /*2.5G Serdes mode-RSGMII or 5G QSGMII*/
                sds_mde = (val >> 5) & 0x1F;
                if((0x1 != sds_mde) && (0x6 != sds_mde))
                    continue;
            }
            else if(sds_idx == 0x3)
            {
                /*2.5G Serdes mode-RS8MII*/
                sds_mde = (val >> 10) & 0x1F;
                if(0x9 != sds_mde)
                    continue;
            }
            else if(sds_idx == 0x2)
            {
                /*2.5G Serdes mode-RS8MII*/
                sds_mde = (val >> 15) & 0x1F;
                if(0x9 != sds_mde)
                    continue;
            }
            else if(sds_idx == 0x1)
            {
                /*2.5G Serdes mode-RS8MII*/
                sds_mde = (val >> 20) & 0x1F;
                if(0x9 != sds_mde)
                    continue;
            }
            else if(sds_idx == 0x0)
            {
                /*2.5G Serdes mode-RS8MII*/
                sds_mde = (val >> 25) & 0x1F;
                if(0x9 != sds_mde)
                    continue;
            }
            else
            {
                continue;
            }

            if(sds_idx<=3)
            {
                /*2.5G Serdes mode-RS8MII*/
                ioal_mem32_read(unit, sds_sts[sds_idx], &value0);
                ioal_mem32_read(unit, sds_sts[sds_idx], &value1);
                ioal_mem32_read(unit, sds_sts[sds_idx], &value2);
                if((0x1FF == value1) && (0x1FF == value2))
                {
                    /*Serdes linkup, work right!*/
                    sds_rst_flag[sds_idx] = 0;
                }
                else
                {
                    /*Serdes linkdown, work wrong!*/
                    if(sds_rst_flag[sds_idx] == 0)
                    {
                        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "[SERDES-WATCHDOG] --->Serdes Link Down--->2.5GRS8MII-Serdes[%d]!", sds_idx);
                        /*First linkdown, work wrong!*/
                        _dal_maple_mac_serdes_rst(unit, sds_idx);
                        sds_rst_flag[sds_idx]++;
                        macSerdes_watchdog_cnt[unit]++;
                        if(macSerdes_watchdog_cnt[unit] >= 64)
                            macSerdes_watchdog_cnt[unit] = 0;
                    }
                    else
                    {
                        sds_rst_flag[sds_idx]++;
                        if(sds_rst_flag[sds_idx] == 5)
                        {
                            sds_rst_flag[sds_idx] = 0;
                        }
                    }
                }
            }
            else if(sds_idx == 4)
            {
                if(0x6 == sds_mde)
                {
                    /*5G Serdes mode*/
                    ioal_mem32_read(unit, sds_sts[sds_idx], &value0);
                    ioal_mem32_read(unit, sds_sts[sds_idx], &value1);
                    ioal_mem32_read(unit, sds_sts[sds_idx], &value2);
                    if((0x1FF == value1) && (0x1FF == value2))
                    {
                        /*Serdes linkup, work right!*/
                        sds_rst_flag[sds_idx] = 0;
                    }
                    else
                    {
                        /*Serdes linkdown, work wrong!*/
                        if(sds_rst_flag[sds_idx] == 0)
                        {
                            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "[SERDES-WATCHDOG] --->Serdes Link Down--->5G-Serdes[%d]!\n", sds_idx);

                            /*First linkdown, work wrong!*/
                           _dal_maple_mac_serdes_rst(unit, sds_idx);
                            sds_rst_flag[sds_idx]++;
                            macSerdes_watchdog_cnt[unit]++;
                           if(macSerdes_watchdog_cnt[unit] >= 64)
                            macSerdes_watchdog_cnt[unit] = 0;
                        }
                        else
                        {
                            sds_rst_flag[sds_idx]++;
                            if(sds_rst_flag[sds_idx] == 5)
                            {
                                /*Not expected case occurs, Reset PHY  & MAC serdes*/
                                sds_rst_flag[sds_idx] = 1;
                               _dal_maple_mac_serdes_rst(unit, sds_idx);
                               /*Here suppose connecting 14FC*/
                               _dal_maple_phy_serdes_rst(unit, sds_idx);
                                  macSerdes_watchdog_cnt[unit]++;
                               if(macSerdes_watchdog_cnt[unit] >= 64)
                                macSerdes_watchdog_cnt[unit] = 0;
                            }
                        }
                    }
                }
                else
                {
                    /*2.5G Serdes mode-RSGMII*/
                    ioal_mem32_read(unit, sds_sts[sds_idx], &value0);
                    ioal_mem32_read(unit, sds_sts[sds_idx], &value1);
                    ioal_mem32_read(unit, sds_sts[sds_idx], &value2);
                    if((0x133 == value1) && (0x133 == value2))
                    {
                        /*Serdes linkup*/
                        sds_rst_flag[sds_idx] = 0;
                    }
                    else
                    {
                        /*Serdes linkdown, work wrong!*/
                        if(sds_rst_flag[sds_idx] == 0)
                        {
                            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "[SERDES-WATCHDOG] --->Serdes Link Down--->2.5GRSGMII-Serdes[%d]!", sds_idx);
                            /*First linkdown, work wrong!*/
                           _dal_maple_mac_serdes_rst(unit, sds_idx);
                            sds_rst_flag[sds_idx]++;
                            macSerdes_watchdog_cnt[unit]++;
                           if(macSerdes_watchdog_cnt[unit] >= 64)
                            macSerdes_watchdog_cnt[unit] = 0;
                        }
                        else
                        {
                            sds_rst_flag[sds_idx]++;
                            if(sds_rst_flag[sds_idx] == 5)
                            {
                                sds_rst_flag[sds_idx] = 0;
                            }
                        }
                    }
                }
            }
            else if((sds_idx == 5))
            {
                /*2.5G Serdes mode-RSGMII*/
                ioal_mem32_read(unit, sds_sts[sds_idx], &value0);
                ioal_mem32_read(unit, sds_sts[sds_idx], &value1);
                ioal_mem32_read(unit, sds_sts[sds_idx], &value2);
                if((0x133 == value1) && (0x133 == value2))
                {
                    /*Serdes linkup*/
                    sds_rst_flag[sds_idx] = 0;
                }
                else
                {
                    /*Serdes linkdown, work wrong!*/
                    if(sds_rst_flag[sds_idx] == 0)
                    {
                        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "[SERDES-WATCHDOG] --->Serdes Link Down--->2.5GRSGMII-Serdes[%d]!", sds_idx);
                        /*First linkdown, work wrong!*/
                        _dal_maple_mac_serdes_rst(unit, sds_idx);
                        sds_rst_flag[sds_idx]++;
                        macSerdes_watchdog_cnt[unit]++;
                        if(macSerdes_watchdog_cnt[unit] >= 64)
                            macSerdes_watchdog_cnt[unit] = 0;
                    }
                    else
                    {
                        sds_rst_flag[sds_idx]++;
                        if(sds_rst_flag[sds_idx] == 5)
                        {
                            sds_rst_flag[sds_idx] = 0;
                        }
                    }
                }
            }
        }
    }

   ret =  RT_ERR_OK;
    return ret;
}

/* Function Name:
 *      dal_maple_port_serdes_watchdog
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
dal_maple_port_serdes_watchdog(uint32 unit)
{
    int32 ret;

    /* check Init status */
    if ((ret = dal_maple_port_init_check(unit)) != RT_ERR_OK)
        return ret;

    if ((ret = dal_maple_port_sem_lock(unit)) != RT_ERR_OK)
        return ret;


    /***************************1: Serdes Link Down Start*********************/
    if((ret = _dal_maple_port_serdes_linkdown_check(unit))!= RT_ERR_OK)
    {
        if ((ret = dal_maple_port_sem_unlock(unit)) != RT_ERR_OK)
            return ret;

        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }
    /***************************1: Serdes Link Down End*********************/

    _dal_maple_port_serdes_status_mac_action(unit);
    
    if ((ret = dal_maple_port_sem_unlock(unit)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}/* end of dal_maple_port_serdes_watchdog */

/************************************WATCH DOG Debug Counter*******************************/
/* Function Name:
 *      dal_maple_port_watchdog_debug
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
dal_maple_port_watchdog_debug(uint32 unit)
{
    uint32 int_val;
    uint32 reg_val;
    int32  ret;

    /* check Init status */
    if ((ret = dal_maple_port_init_check(unit)) != RT_ERR_OK)
        return ret;

    if ((ret = dal_maple_port_sem_lock(unit)) != RT_ERR_OK)
        return ret;

    ioal_mem32_read(unit, 0x0058, &int_val);
    ioal_mem32_write(unit, 0x0058, 0x3);
    reg_val = (pktBuf_watchdog_cnt[unit] & 0x3F)<<8;
    reg_val |= (macSerdes_watchdog_cnt[unit] & 0x3F)<<20;
    reg_val |= (phy_watchdog_cnt[unit] & 0x3F)<<2;
    ioal_mem32_write(unit, 0xad60, reg_val);
    ioal_mem32_write(unit, 0x0058, int_val);

    if ((ret = dal_maple_port_sem_unlock(unit)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}/* end of dal_maple_port_watchdog_debug */

/************************************ComboPort Fiber Mode FC not work*******************************/
#if defined(CONFIG_SDK_WA_COMBO_FLOWCONTROL)
/* Function Name:
 *      _dal_maple_port_comboPort_fc_workaround
 * Description:
 *      Monitor for phy(combo port) flow control on fiber mode.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is Monitor for phy(combo port) flow control on fiber mode.
 */
int32 _dal_maple_port_comboPort_fc_workaround(uint32 unit)
{
    int32 ret;
    uint32 port_id;
    uint32 reg_val;
    uint32 speed_val;
    uint32 link_sts;
    uint32 link_media;
    uint32 link_speed;

    uint32 phy_data0;
    uint32 phy_data1;

    uint32 phy_data;

    uint32 tx_pause;
    uint32 rx_pause;

    uint32  reg_idx;
    uint32  temp;
    
    /*From Combo Port, right now portid range:24-27*/
    for(port_id = 0; port_id < 24; port_id++)
    {
        if(!HWP_PHY_EXIST(unit, port_id))
            continue;

        if (HWP_IS_CPU_PORT(unit, port_id) || HWP_SERDES_PORT(unit, port_id))
            continue;

        if(!(HWP_GE_PORT(unit, port_id)&&(HWP_COMBO_PORT(unit, port_id)||HWP_FIBER_PORT(unit, port_id))))
            continue;
        
        /*Link Status First*/
        if ((ret = reg_field_read(unit, MAPLE_MAC_LINK_STSr, MAPLE_LINK_STS_27_0f, &reg_val)) != RT_ERR_OK)
            return ret;

        /*Link Status Second*/
        if ((ret = reg_field_read(unit, MAPLE_MAC_LINK_STSr, MAPLE_LINK_STS_27_0f, &reg_val)) != RT_ERR_OK)
            return ret;
        link_sts = (reg_val >> port_id) & 0x1;

        if(0 == link_sts)
            continue;

        /*Page0 Reg0:BIT12--NWAY Enable*/
        ret = dal_phy_portReg_get(unit,  port_id,  0x0,  0, &phy_data);
        if(ret != RT_ERR_OK)
            return ret;
        phy_data = (phy_data>>12) & 0x1;

        if(0 == phy_data)
            continue;

        /*Link Media*/
        if ((ret = reg_field_read(unit, MAPLE_MAC_LINK_MEDIA_STSr, MAPLE_MEDIA_STS_27_0f, &reg_val)) != RT_ERR_OK)
            return ret;
        link_media = (reg_val >> port_id) & 0x1;

        /*Link Speed*/
        if ((ret = reg_array_field_read(unit, MAPLE_MAC_LINK_SPD_STSr, port_id, REG_ARRAY_INDEX_NONE, \
                    MAPLE_SPD_STS_27_0f,&speed_val)) != RT_ERR_OK)
            return ret;
        link_speed = speed_val & 0x3;

        /*Page0 Reg4*/
        ret = dal_phy_portReg_get(unit,  port_id,  0x0,  4, &phy_data0);
        if(ret != RT_ERR_OK)
            return ret;

        /*Page0 Reg5*/
        ret = dal_phy_portReg_get(unit,  port_id,  0x0,  5, &phy_data1);
        if(ret != RT_ERR_OK)
            return ret;


        /*If LINK-UP-Fiber-1000M*/
        if((0x1 == link_media) && (0x2 == link_speed))
        {
            /*TX Pause*/
            tx_pause = (~((phy_data0>>7) & 0x1)) & ((phy_data0>>8) & 0x1) & ((phy_data1>>7) & 0x1) & ((phy_data1>>8) & 0x1);
            tx_pause |= ((phy_data0>>7) & 0x1) & ((phy_data1>>7) & 0x1);
            /*RX Pause*/
            rx_pause = ((phy_data0>>7) & 0x1) & ((phy_data0>>8) & 0x1) & (~((phy_data1>>7) & 0x1)) & ((phy_data1>>8) & 0x1);
            rx_pause |= ((phy_data0>>7) & 0x1) & ((phy_data1>>7) & 0x1);
        }
        else
        {
            /*TX Pause*/
            tx_pause = (~((phy_data0>>10) & 0x1)) & ((phy_data0>>11) & 0x1) & ((phy_data1>>10) & 0x1) & ((phy_data1>>11) & 0x1);
            tx_pause |= ((phy_data0>>10) & 0x1) & ((phy_data1>>10) & 0x1);
            /*RX Pause*/
            rx_pause = ((phy_data0>>10) & 0x1) & ((phy_data0>>11) & 0x1) & (~((phy_data1>>10) & 0x1)) & ((phy_data1>>11) & 0x1);
            rx_pause |= ((phy_data0>>10) & 0x1) & ((phy_data1>>10) & 0x1);
        }

        /* Need to configure [MAC_FORCE_MODE_CTRL]*/
        reg_idx = MAPLE_MAC_FORCE_MODE_CTRLr;

        temp = 0x1;     /*Always set MAC FORCE Flow Control*/
        ret = reg_array_field_write(unit, reg_idx, port_id, REG_ARRAY_INDEX_NONE, MAPLE_MAC_FORCE_FC_ENf, &temp);
        if ((ret != RT_ERR_OK)&&(ret != RT_ERR_CHIP_NOT_SUPPORTED))
            return ret;

        temp = rx_pause;
        ret = reg_array_field_write(unit, reg_idx, port_id, REG_ARRAY_INDEX_NONE, MAPLE_RX_PAUSE_ENf, &temp);
        if ((ret != RT_ERR_OK)&&(ret != RT_ERR_CHIP_NOT_SUPPORTED))
            return ret;

        temp = tx_pause;
        ret = reg_array_field_write(unit, reg_idx, port_id, REG_ARRAY_INDEX_NONE, MAPLE_TX_PAUSE_ENf, &temp);
        if ((ret != RT_ERR_OK)&&(ret != RT_ERR_CHIP_NOT_SUPPORTED))
            return ret;

    }

    return RT_ERR_OK;
}



/* Function Name:
 *      dal_maple_port_comboPort_workaround
 * Description:
 *      Monitor for phy(combo port) flow control on fiber mode.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is Monitor for phy(combo port) flow control on fiber mode.
 */
int32
dal_maple_port_comboPort_workaround(uint32 unit)
{
    int32 ret;

    /* check Init status */
    if ((ret = dal_maple_port_init_check(unit)) != RT_ERR_OK)
        return ret;

    if ((ret = dal_maple_port_sem_lock(unit)) != RT_ERR_OK)
        return ret;

    if((ret = _dal_maple_port_comboPort_fc_workaround(unit))!= RT_ERR_OK)
    {
        if ((ret = dal_maple_port_sem_unlock(unit)) != RT_ERR_OK)
            return ret;
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if ((ret = dal_maple_port_sem_unlock(unit)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}/* end of dal_maple_port_comboPort_workaround */
#endif



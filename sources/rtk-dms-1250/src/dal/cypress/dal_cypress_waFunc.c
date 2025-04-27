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
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/common/miim.h>
#include <hal/common/halctrl.h>
#include <dal/dal_common.h>
#include <dal/dal_waMon.h>
#include <dal/cypress/dal_cypress_port.h>
#include <dal/cypress/dal_cypress_waFunc.h>
#include <dal/dal_phy.h>
#include <rtk/port.h>
#include <rtk/default.h>
#include <hal/phy/phy_rtl8390.h>
#include <ioal/mem32.h>

extern uint32 macSerdes_watchdog_cnt[RTK_MAX_NUM_OF_UNIT];


#if defined(CONFIG_SDK_WA_SERDES_WATCHDOG)
/* Function Name:
 *      dal_cypress_port_serdes_watchdog
 * Description:
 *      Monitor for serdes link statuse.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is monitor serdes link down and patch it.
 *      Protect PHY page by port semaphore.
 */
int32
dal_cypress_port_serdes_watchdog(uint32 unit)
{
    uint32  sdsReg[] = {0xA340, 0xA740, 0xAB40, 0xAF40, 0xB3F8, 0xB740, 0xBBF8};
    uint32  sdsIdx, sdsMode, sdsAddr, val;
    uint32  ofst;
    int32   ret;

    if (HWP_8390_FAMILY_ID(unit))
    {
        /* Check serdes interface mode */
        for (sdsIdx = 0; sdsIdx < (2 * (sizeof(sdsReg)/sizeof(uint32))); ++sdsIdx)
        {
            if ((ret = reg_array_field_read(unit, CYPRESS_MAC_SERDES_IF_CTRLr,
                    REG_ARRAY_INDEX_NONE, sdsIdx, CYPRESS_SERDES_SPD_SELf,
                    &sdsMode)) != RT_ERR_OK)
                return ret;

            /* QSGMII */
            if (6 == sdsMode)
            {
                /* check serdes link */
                sdsAddr = 0xA078 + (0x400 * (sdsIdx / 2)) + (0x100 * (sdsIdx % 2));

                ioal_mem32_read(unit, sdsAddr, &val);
                ioal_mem32_read(unit, sdsAddr, &val);

                if (0x1ff0000 != val)
                {
                    ++macSerdes_watchdog_cnt[unit];

                    /* CMU reset */
                    ofst = 0x400 * (sdsIdx / 2);
                    switch (sdsIdx)
                    {
                        case 8 ... 9:
                            sdsAddr = 0xb3f8;
                            break;
                        case 12 ... 13:
                            sdsAddr = 0xbbf8;
                            break;
                        default:
                            sdsAddr = 0xa3c0 + ofst;
                    }

                    ioal_mem32_read(unit, sdsAddr, &val);
                    val &= (~(0x3 << 20));
                    val |= (0x1 << 20);
                    ioal_mem32_write(unit, sdsAddr, val);

                    ioal_mem32_read(unit, sdsAddr, &val);
                    val &= (~(0x3 << 20));
                    val |= (0x3 << 20);
                    ioal_mem32_write(unit, sdsAddr, val);

                    ioal_mem32_read(unit, sdsAddr, &val);
                    val &= (~(0x3 << 20));
                    ioal_mem32_write(unit, sdsAddr, val);

                    /* RX reset */
                    sdsAddr = sdsReg[sdsIdx/2];
                    ioal_mem32_read(unit, sdsAddr, &val);

                    switch (sdsIdx)
                    {
                        case 8 ... 9:
                        case 12 ... 13:
                            val |= (0x5F << 20);
                            break;
                        default:
                            val |= (0xF5 << 24);
                    }

                    ioal_mem32_write(unit, sdsAddr, val);

                    ioal_mem32_read(unit, sdsAddr, &val);
                    switch (sdsIdx)
                    {
                        case 8 ... 9:
                        case 12 ... 13:
                            val &= ~(0xFF << 20);
                            break;
                        default:
                            val &= ~(0xFF << 24);
                    }
                    ioal_mem32_write(unit, sdsAddr, val);

                    /* digital */
                    sdsAddr = 0xa004 + ofst;
                    ioal_mem32_read(unit, sdsAddr, &val);
                    val &= (~(0x1 << 22));
                    val |= (0x1 << 22);
                    ioal_mem32_write(unit, sdsAddr, val);

                    ioal_mem32_read(unit, sdsAddr, &val);
                    val &= (~(0x1 << 22));
                    ioal_mem32_write(unit, sdsAddr, val);

                    sdsAddr += 0x100;
                    ioal_mem32_read(unit, sdsAddr, &val);
                    val &= (~(0x1 << 22));
                    val |= (0x1 << 22);
                    ioal_mem32_write(unit, sdsAddr, val);

                    ioal_mem32_read(unit, sdsAddr, &val);
                    val &= (~(0x1 << 22));
                    ioal_mem32_write(unit, sdsAddr, val);

                    if (0 == (sdsIdx % 2))
                    {
                        ++sdsIdx;
                    }
                }
            }
        }
    }

    return RT_ERR_OK;
}   /* end of dal_cypress_port_serdes_watchdog */
#endif  /* CONFIG_SDK_WA_SERDES_WATCHDOG */


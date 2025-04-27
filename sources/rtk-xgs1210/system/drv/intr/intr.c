/* Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 93380 $
 * $Date: 2018-11-08 17:49:37 +0800 (Thu, 08 Nov 2018) $
 *
 * Purpose : Definition of Interrupt control API
 *
 * Feature : The file includes the following modules
 *           (1) SWCORE
 *           (2) NIC
 *
 */

 /*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/debug/rt_log.h>
#include <private/drv/intr/intr.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#if defined(CONFIG_SDK_RTL8390)
#include <private/drv/swcore/swcore_rtl8390.h>
#include <private/drv/nic/nic_rtl8390.h>
#endif
#if defined(CONFIG_SDK_RTL8380)
#include <private/drv/swcore/swcore_rtl8380.h>
#include <private/drv/nic/nic_rtl8380.h>
#endif
#if defined(CONFIG_SDK_RTL9300)
#include <private/drv/swcore/swcore_rtl9300.h>
#include <private/drv/nic/nic_rtl9300.h>
#endif
#if defined(CONFIG_SDK_RTL9310)
#include <private/drv/swcore/swcore_rtl9310.h>
#include <private/drv/nic/nic_rtl9310.h>
#endif
#if defined(CONFIG_SDK_DRIVER_L2NTFY)
#include <drv/l2ntfy/l2ntfy.h>
#include <private/drv/l2ntfy/l2ntfy_rtl8390.h>
#endif
#include <common/util/rt_util_time.h>
#include <rtcore/rtcore.h>
#include <ioal/mem32.h>
#include <osal/print.h>
#include <drv/nic/nic.h>
#include <hwp/hw_profile.h>

/*
 * Symbol Definition
 */
typedef struct drv_intr_hdl_attr_s {
    uint32	    maskbit;
    drv_intr_hdl_f	fIntr_handler;
    uint32	    intr_data[2];
    uint8	    *pIntr_name;
} drv_intr_hdl_attr_t;

#if defined(CONFIG_SDK_RTL8390)
#define NUM_OF_R8390_INTR_HANDLER (sizeof(r8390_intr_handlers)/sizeof(drv_intr_hdl_attr_t))
#endif
#if defined(CONFIG_SDK_RTL8380)
#define NUM_OF_R8380_INTR_HANDLER (sizeof(r8380_intr_handlers)/sizeof(drv_intr_hdl_attr_t))
#endif
#if defined(CONFIG_SDK_RTL9300)
#define NUM_OF_R9300_INTR_HANDLER (sizeof(r9300_intr_handlers)/sizeof(drv_intr_hdl_attr_t))
#endif
#if defined(CONFIG_SDK_RTL9310)
#define NUM_OF_R9310_INTR_HANDLER (sizeof(r9310_intr_handlers)/sizeof(drv_intr_hdl_attr_t))
#endif
#define INTR_DB_SIZE     (sizeof(intr_db)/sizeof(cid_prefix_group_t))
/*
 * Data Declaration
 */
void drv_intr_link_stat_hdl(uint32 unit, void* data);
drv_intr_hdl_f fLink_stat_cb = NULL;
uint32 intr_count = 0;
#if defined(CONFIG_SDK_RTL8390)
static drv_intr_hdl_attr_t r8390_intr_handlers[] = {
    {RTL8390_ISR_GLB_SRC_ISR_GLB_LINK_CHG_MASK, drv_intr_link_stat_hdl, {0}, (uint8 *)"link change interrupt"},
};
#endif
#if defined(CONFIG_SDK_RTL8380)
static drv_intr_hdl_attr_t r8380_intr_handlers[] = {
    {(uint32)0x1, drv_intr_link_stat_hdl, {0}, (uint8 *)"link change interrupt"},
};
#endif
#if defined(CONFIG_SDK_RTL9300)
static drv_intr_hdl_attr_t r9300_intr_handlers[] = {
    {(uint32)RTL9300_ISR_GLB_ISR_GLB_LINK_CHG_MASK, drv_intr_link_stat_hdl, {0}, (uint8 *)"link change interrupt"},
};
#endif
#if defined(CONFIG_SDK_RTL9310)
static drv_intr_hdl_attr_t r9310_intr_handlers[] = {
    {(uint32)RTL9310_ISR_GLB_SRC_STS_ISR_GLB_LINK_CHG_MASK, drv_intr_link_stat_hdl, {0}, (uint8 *)"link change interrupt"},
};
#endif
const static cid_prefix_group_t intr_db[] =
{
#if defined(CONFIG_SDK_RTL8390)
    {RTL8390_FAMILY_ID, INTR_R8390},
    {RTL8350_FAMILY_ID, INTR_R8390},
#endif
#if defined(CONFIG_SDK_RTL8380)
    {RTL8380_FAMILY_ID, INTR_R8380},
    {RTL8330_FAMILY_ID, INTR_R8380},
#endif
#if defined(CONFIG_SDK_RTL9300)
    {RTL9300_FAMILY_ID, INTR_R9300},
#endif
#if defined(CONFIG_SDK_RTL9310)
    {RTL9310_FAMILY_ID, INTR_R9310},
#endif


};

#if defined(CONFIG_SDK_RTL8390)
static int32 _r8390_intr_enable_set(uint32 unit, drv_intr_source_t intr_source);
static void  _r8390_intr_swcore_handler(void *pParam);
#endif
#if defined(CONFIG_SDK_RTL8380)
static int32 _r8380_intr_enable_set(uint32 unit, drv_intr_source_t intr_source);
static void  _r8380_intr_swcore_handler(void *pParam);
#endif
#if defined(CONFIG_SDK_RTL9300)
static int32 _r9300_intr_enable_set(uint32 unit, drv_intr_source_t intr_source);
static void  _r9300_intr_swcore_handler(void *pParam);
#endif
#if defined(CONFIG_SDK_RTL9310)
static int32 _r9310_intr_enable_set(uint32 unit, drv_intr_source_t intr_source);
static void  _r9310_intr_swcore_handler(void *pParam);
#endif

drv_intr_mapper_operation_t intr_ops[INTR_CTRL_END] =
{
#if defined(CONFIG_SDK_RTL8390)
    {   /* INTR_R8390 */
        .enable_set = _r8390_intr_enable_set,
        .swcore_handler = _r8390_intr_swcore_handler,
    },
#endif
#if defined(CONFIG_SDK_RTL8380)
    {   /* INTR_R8380 */
        .enable_set = _r8380_intr_enable_set,
        .swcore_handler = _r8380_intr_swcore_handler,
    },
#endif
#if defined(CONFIG_SDK_RTL9300)
    {   /* INTR_R9300 */
        .enable_set = _r9300_intr_enable_set,
        .swcore_handler = _r9300_intr_swcore_handler,
    },
#endif
#if defined(CONFIG_SDK_RTL9310)
    {   /* INTR_R9310 */
        .enable_set = _r9310_intr_enable_set,
        .swcore_handler = _r9310_intr_swcore_handler,
    },
#endif
};

uint32 intr_if[RTK_MAX_NUM_OF_UNIT] = {CID_GROUP_NONE};
#if defined(CONFIG_SDK_LINKMON_POLLING_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
extern uint32 swLinkScanFlag;
#endif
/*
 * Function Declaration
 */


/* Function Name:
 *      drv_intr_init
 * Description:
 *      Initialize intr module of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Must initialize nic module before calling any nic APIs.
 */
int32
drv_intr_init(uint32 unit)
{
    uint32      i;
    uint32      cid;
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    RT_INIT_MSG("  Intr init (unit %u)\n",unit);

    cid = HWP_CHIP_ID(unit);

    intr_if[unit] = CID_GROUP_NONE;
    for (i = 0; i < INTR_DB_SIZE; i++)
    {
        if (CID_PREFIX_MATCH(intr_db[i], cid))
        {
            intr_if[unit] = intr_db[i].gid;
            return RT_ERR_OK;
        }
    }

    return RT_ERR_FAILED;
} /* end of drv_intr_init */

/* Function Name:
 *      drv_intr_enable_set
 * Description:
 *      Set the interrupt enable status of the specified source.
 * Input:
 *      unit     - unit id
 *      intr_source - interrupt source that is going to be enabled.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Note:
 *      Must initialize interrupt module before calling any nic APIs.
 */
int32
drv_intr_enable_set(uint32 unit, drv_intr_source_t intr_source)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    return INTR_CTRL(unit).enable_set(unit, intr_source);
} /* end of drv_intr_enable_set */

/* Function Name:
 *      drv_intr_swcore_handler
 * Description:
 *      Common swcore interrupt handler function.
 * Input:
 *      pParam - The argument passed to interrupt handler at interrupt time.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
drv_intr_swcore_handler(void *pParam)
{
    rtcore_ioctl_t *dio;

    dio = (rtcore_ioctl_t *)pParam;

    return INTR_CTRL(dio->data[0]).swcore_handler(pParam);
} /* end of drv_intr_swcore_handler */

/* Function Name:
 *      drv_intr_link_stat_register
 * Description:
 *      Register the callback function for handling link state change interrupt handler.
 * Input:
 *      fLinkStatCb - pointer to a handler of link state change
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
drv_intr_link_stat_register(drv_intr_hdl_f fLinkStatCb)
{
    RT_PARAM_CHK(NULL == fLinkStatCb, RT_ERR_NULL_POINTER);

    fLink_stat_cb = fLinkStatCb;

    return RT_ERR_OK;
} /* end of drv_intr_link_stat_register */

/* Function Name:
 *      drv_intr_link_stat_unregister
 * Description:
 *      Unregister the callback function for handling link state change interrupt handler.
 * Input:
 *      unit   - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Note:
 *      None
 */
int32
drv_intr_link_stat_unregister(uint32 unit)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    if (NULL != fLink_stat_cb)
    {
        fLink_stat_cb = NULL;
    }
    else
    {
        /* Handler is not existing */
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of drv_intr_link_stat_unregister */


void drv_intr_link_stat_hdl(uint32 unit, void* data)
{
    if (NULL != fLink_stat_cb)
        fLink_stat_cb(unit, data);
    return;
} /* end of drv_intr_link_stat_hdl */

#if defined(CONFIG_SDK_RTL8390)
/* Function Name:
 *      _r8390_intr_enable_set
 * Description:
 *      Enable interrupt with specified interrupt source of 8390 series
 * Input:
 *      unit        - unit id
 *      intr_source - interrupt source that is going to be enabled.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
static int32
_r8390_intr_enable_set(uint32 unit, drv_intr_source_t intr_source)
{
    RT_PARAM_CHK(intr_source >= INTR_SOURCE_END, RT_ERR_INPUT);

    switch (intr_source)
    {
        case LINK_CHANGE_INTR:
            //ioal_mem32_write(unit, RTL8390_IMR_PORT_LINK_STS_CHG_ADDR(0), 0xFFFFFFFF);
            //ioal_mem32_write(unit, RTL8390_IMR_PORT_LINK_STS_CHG_ADDR(32), 0xFFFFF);
            break;
        case MEDIA_CHANGE_INTR:
            break;
        case SPEED_CHANGE_INTR:
            break;
        case DUPLEX_CHANGE_INTR:
            break;
        default:
            break;
    }

    return RT_ERR_OK;
} /* end of _r8390_intr_enable_set */

/* Function Name:
 *      _r8390_intr_swcoreHandler
 * Description:
 *      Common interrupt handler function of 8390 series.
 * Input:
 *      pParam - The argument passed to interrupt handler at interrupt time.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
static void
_r8390_intr_swcore_handler(void *pParam)
{
    uint32  intr_status;
    uint32  unit, i;

    intr_count++;
    unit = ((rtcore_ioctl_t *)pParam)->data[0];
    intr_status = ((rtcore_ioctl_t *)pParam)->data[2];
#if defined(CONFIG_SDK_LINKMON_POLLING_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
    swLinkScanFlag = ((rtcore_ioctl_t *)pParam)->data[7];
#endif
    for (i = 0; i < NUM_OF_R8390_INTR_HANDLER; i++)
    {
        if (intr_status & r8390_intr_handlers[i].maskbit)
        {
            /* Bit found, dispatch interrupt */
            /* Assing interrupt status carried from kernel space */
            r8390_intr_handlers[i].intr_data[0] = ((rtcore_ioctl_t *)pParam)->data[3];
            r8390_intr_handlers[i].intr_data[1] = ((rtcore_ioctl_t *)pParam)->data[4];
            (r8390_intr_handlers[i].fIntr_handler)(unit, (void *)&r8390_intr_handlers[i].intr_data);
        }
    }

    /*
     * re-enable interrupts here.
     */
    /* link change interrupt */
    ioal_mem32_write(unit, RTL8390_IMR_PORT_LINK_STS_CHG_ADDR(0), 0xFFFFFFFF);
    ioal_mem32_write(unit, RTL8390_IMR_PORT_LINK_STS_CHG_ADDR(32), 0xFFFFF);
    return;
} /* end of _r8390_intr_swcore_handler */
#endif

#if defined(CONFIG_SDK_RTL8380)
/* Function Name:
 *      _r8380_intr_enable_set
 * Description:
 *      Enable interrupt with specified interrupt source of RTL8380 series
 * Input:
 *      unit        - unit id
 *      intr_source - interrupt source that is going to be enabled.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
static int32
_r8380_intr_enable_set(uint32 unit, drv_intr_source_t intr_source)
{
    RT_PARAM_CHK(intr_source >= INTR_SOURCE_END, RT_ERR_INPUT);

    switch (intr_source)
    {
        case LINK_CHANGE_INTR:
           //by cw ioal_mem32_write(unit, RTL8380_SWITCH_INTERRUPT_CONTROL0_ADDR, RTL8380_SWITCH_INTERRUPT_CONTROL0_LINK_STA_CHANGE_IE_MASK);
            break;
        case MEDIA_CHANGE_INTR:
            break;
        case SPEED_CHANGE_INTR:
            break;
        case DUPLEX_CHANGE_INTR:
            break;
        default:
            break;
    }

    return RT_ERR_OK;
} /* end of _r8380_intr_enable_set */

/* Function Name:
 *      _r8380_intr_swcoreHandler
 * Description:
 *      Common interrupt handler function of 8380 series.
 * Input:
 *      pParam - The argument passed to interrupt handler at interrupt time.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
static void
_r8380_intr_swcore_handler(void *pParam)
{
    uint32  intr_status;
    uint32  unit, i;

    intr_count++;
    unit = ((rtcore_ioctl_t *)pParam)->data[0];
    intr_status = ((rtcore_ioctl_t *)pParam)->data[2];
#if defined(CONFIG_SDK_LINKMON_POLLING_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
    swLinkScanFlag = ((rtcore_ioctl_t *)pParam)->data[7];
#endif
    for (i = 0; i < NUM_OF_R8380_INTR_HANDLER; i++)
    {
        if (intr_status & r8380_intr_handlers[i].maskbit)
        {
            /* Bit found, dispatch interrupt */
            /* Assing interrupt status carried from kernel space */
            r8380_intr_handlers[i].intr_data[0] = ((rtcore_ioctl_t *)pParam)->data[3];
            (r8380_intr_handlers[i].fIntr_handler)(unit, (void *)&r8380_intr_handlers[i].intr_data);
        }
    }

    /*
     * reenable interrupts here.
     */
   //by cw ioal_mem32_write(unit, RTL8380_SWITCH_INTERRUPT_CONTROL0_ADDR, RTL8380_SWITCH_INTERRUPT_CONTROL0_LINK_STA_CHANGE_IE_MASK);
    return;
} /* end of _r8380_intr_swcore_handler */
#endif

#if defined(CONFIG_SDK_RTL9300)
/* Function Name:
 *      _r9300_intr_enable_set
 * Description:
 *      Enable interrupt with specified interrupt source of RTL9300 series
 * Input:
 *      unit        - unit id
 *      intr_source - interrupt source that is going to be enabled.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
static int32
_r9300_intr_enable_set(uint32 unit, drv_intr_source_t intr_source)
{
    RT_PARAM_CHK(intr_source >= INTR_SOURCE_END, RT_ERR_INPUT);

    switch (intr_source)
    {
        case LINK_CHANGE_INTR:
           //by cw ioal_mem32_write(unit, RTL8380_SWITCH_INTERRUPT_CONTROL0_ADDR, RTL8380_SWITCH_INTERRUPT_CONTROL0_LINK_STA_CHANGE_IE_MASK);
            break;
        case MEDIA_CHANGE_INTR:
            break;
        case SPEED_CHANGE_INTR:
            break;
        case DUPLEX_CHANGE_INTR:
            break;
        default:
            break;
    }

    return RT_ERR_OK;
} /* end of _r9300_intr_enable_set */

/* Function Name:
 *      _r9300_intr_swcore_handler
 * Description:
 *      Common interrupt handler function of 9300 series.
 * Input:
 *      pParam - The argument passed to interrupt handler at interrupt time.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
static void
_r9300_intr_swcore_handler(void *pParam)
{
    uint32  intr_status;
    uint32  unit, i;

    intr_count++;
    unit = ((rtcore_ioctl_t *)pParam)->data[0];
    intr_status = ((rtcore_ioctl_t *)pParam)->data[2];
#if defined(CONFIG_SDK_LINKMON_POLLING_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
    swLinkScanFlag = ((rtcore_ioctl_t *)pParam)->data[7];
#endif
    for (i = 0; i < NUM_OF_R9300_INTR_HANDLER; i++)
    {
        if (intr_status & r9300_intr_handlers[i].maskbit)
        {
            /* Bit found, dispatch interrupt */
            /* Assing interrupt status carried from kernel space */
            r9300_intr_handlers[i].intr_data[0] = ((rtcore_ioctl_t *)pParam)->data[3];
            (r9300_intr_handlers[i].fIntr_handler)(unit, (void *)&r9300_intr_handlers[i].intr_data);
        }
    }

    /*
     * reenable interrupts here.
     */
   //by cw ioal_mem32_write(unit, RTL9300_SWITCH_INTERRUPT_CONTROL0_ADDR, RTL9300_SWITCH_INTERRUPT_CONTROL0_LINK_STA_CHANGE_IE_MASK);
    return;
} /* end of _r9300_intr_swcore_handler */
#endif

#if defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      _r9310_intr_enable_set
 * Description:
 *      Enable interrupt with specified interrupt source of RTL9310 series
 * Input:
 *      unit        - unit id
 *      intr_source - interrupt source that is going to be enabled.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
static int32
_r9310_intr_enable_set(uint32 unit, drv_intr_source_t intr_source)
{
    RT_PARAM_CHK(intr_source >= INTR_SOURCE_END, RT_ERR_INPUT);

    switch (intr_source)
    {
        case LINK_CHANGE_INTR:
            break;
        case MEDIA_CHANGE_INTR:
            break;
        case SPEED_CHANGE_INTR:
            break;
        case DUPLEX_CHANGE_INTR:
            break;
        default:
            break;
    }

    return RT_ERR_OK;
} /* end of _r9310_intr_enable_set */

/* Function Name:
 *      _r9310_intr_swcore_handler
 * Description:
 *      Common interrupt handler function of 9310 series.
 * Input:
 *      pParam - The argument passed to interrupt handler at interrupt time.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
static void
_r9310_intr_swcore_handler(void *pParam)
{
    uint32  intr_status;
    uint32  unit, i;

    intr_count++;
    unit = ((rtcore_ioctl_t *)pParam)->data[0];
    intr_status = ((rtcore_ioctl_t *)pParam)->data[2];
#if defined(CONFIG_SDK_LINKMON_POLLING_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
    swLinkScanFlag = ((rtcore_ioctl_t *)pParam)->data[7];
#endif
    for (i = 0; i < NUM_OF_R9310_INTR_HANDLER; i++)
    {
        if (intr_status & r9310_intr_handlers[i].maskbit)
        {
            /* Bit found, dispatch interrupt */
            /* Assing interrupt status carried from kernel space */
            r9310_intr_handlers[i].intr_data[0] = ((rtcore_ioctl_t *)pParam)->data[3];
            r9310_intr_handlers[i].intr_data[1] = ((rtcore_ioctl_t *)pParam)->data[4];
            (r9310_intr_handlers[i].fIntr_handler)(unit, (void *)&r9310_intr_handlers[i].intr_data);
        }
    }

    return;
}
#endif



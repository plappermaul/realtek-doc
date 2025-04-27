/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Monitor Link status of each port
 *
 * Feature :
 *
 */
#ifndef __DAL_LINKMON_H__
#define __DAL_LINKMON_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <dal/dal_mapper.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */


/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Module Name : */

/* Function Name:
 *      dal_linkMon_init
 * Description:
 *      Initial Link Monitor component
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 * Note:
 *      None
 */
extern int32
dal_linkMon_init(void);

/* Function Name:
 *      dal_linkMon_devInit
 * Description:
 *      Initial Link Monitor component for each device
 * Input:
 *      unit            - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 * Note:
 *      None
 */
extern int32
dal_linkMon_devInit(uint32 unit);


/* Function Name:
 *      dal_linkMon_register
 * Description:
 *      Register callback function for link change notification
 * Input:
 *      linkMon_callback    - callback function for link change
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_linkMon_register(rtk_port_linkMon_callback_t linkMon_callback);

/* Function Name:
 *      dal_linkMon_enable
 * Description:
 *      Enable link monitor thread
 * Input:
 *      scan_interval_us        - scan interval in us.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - scan interval is too small
 * Note:
 *      When enable link monitor thread, all link change interrupt will be handled by thread.
 */
extern int32
dal_linkMon_enable(uint32 scan_interval_us);

/* Function Name:
 *      dal_linkMon_disable
 * Description:
 *      Disable link scan thread
 * Input:
 *      None.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      When disable link monitor thread, all link change interrupt will be callback to upper layer.
 */
extern int32
dal_linkMon_disable(void);

/* Function Name:
 *      dal_linkMonPolling_stop
 * Description:
 *      Make linkmon polling thread stop/resume polling port status.
 *      polling.
 * Input:
 *      unit      - unit
 *      stop_bool - TRUE:stop / FALSE:resume
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - success
 *      RT_ERR_FAILED   - fail
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      A switch may have multiple units but only one linkmon module.
 *      This API takes effects on the linkmon polling thread.
 *
 */
extern int32
dal_linkMonPolling_stop(uint32 unit, uint32 stop_bool);





/* Function Name:
 *      dal_linkMon_unregister
 * Description:
 *      Unregister callback function for link change notification
 * Input:
 *      linkMon_callback    - callback function for link change
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_FAILED   - initialize fail
 * Note:
 *      None
 */
extern int32
dal_linkMon_unregister(rtk_port_linkMon_callback_t linkMon_callback);

/* Function Name:
 *      dal_linkMon_swScanPorts_set
 * Description:
 *      Configure portmask of software linkscan for certain unit
 * Input:
 *      unit                - callback function for link change
 *      pSwScan_portmask    - portmask for software scan
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - initialize success
 *      RT_ERR_FAILED       - initialize fail
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_linkMon_swScanPorts_set(uint32 unit, rtk_portmask_t *pSwScan_portmask);

/* Function Name:
 *      dal_linkMon_swScanPorts_get
 * Description:
 *      Get portmask of software linkscan for certain unit
 * Input:
 *      unit                - callback function for link change
 * Output:
 *      pSwScan_portmask    - portmask for software scan
 * Return:
 *      RT_ERR_OK           - initialize success
 *      RT_ERR_FAILED       - initialize fail
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_linkMon_swScanPorts_get(uint32 unit, rtk_portmask_t *pSwScan_portmask);

#endif /* __DAL_LINKMON_H__ */

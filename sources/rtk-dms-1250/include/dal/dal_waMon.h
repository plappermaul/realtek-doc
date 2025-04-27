/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Monitor workaround status of each port
 *
 * Feature :
 *
 */
#ifndef __DAL_WA_MON_H__
#define __DAL_WA_MON_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>

#if defined(CONFIG_SFLOW_PG_THREAD)
#include <rtk/flowctrl.h>
#include <rtk/mirror.h>
#endif


/*
 * Symbol Definition
 */
#ifndef __BOOTLOADER__
    #define CONFIG_SDK_WA_FIBER_RX_WATCHDOG 1
  #if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    #define CONFIG_SDK_WA_SERDES_WATCHDOG 1
  #endif

  #if defined(CONFIG_SDK_RTL8390)
    #define CONFIG_SDK_WA_SERDES_FIBER_LINKDOWN_WATCHDOG 1
  #endif

  #if defined(CONFIG_SDK_RTL8380)  || defined(CONFIG_SDK_RTL9300)
    #define CONFIG_SDK_WA_PKTBUF_WATCHDOG 1
  #endif
  #if defined(CONFIG_SDK_RTL8380)
    #define CONFIG_SDK_WA_COMBO_FLOWCONTROL 1
  #endif

  #if defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8214FB) ||  defined(CONFIG_SDK_RTL8214FC) || defined(CONFIG_SDK_RTL8214C) || defined(CONFIG_SDK_RTL8218FB) || defined(CONFIG_SDK_RTL8218D) || defined(CONFIG_SDK_RTL8218E) || defined(CONFIG_SDK_RTL8380)
    #define CONFIG_SDK_WA_PHY_WATCHDOG 1
  #endif

  #if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    #define CONFIG_SDK_WA_REMOTE_FAULT 1
  #endif
#endif

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
 *      dal_waMon_init
 * Description:
 *      Initial Workaround Monitor component
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *      None
 */
extern int32
dal_waMon_init(void);

/* Function Name:
 *      dal_waMon_enable
 * Description:
 *      Enable workaround monitor thread
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - scan interval is too small
 * Note:
 *      When enable workaround monitor thread
 *
 */
int32 dal_waMon_enable(void);

/* Function Name:
 *      dal_waMon_disable
 * Description:
 *      Disable workaround scan thread
 * Input:
 *      None.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      When disable workaround monitor thread
 */
extern int32
dal_waMon_disable(void);

/* Function Name:
 *      dal_waMon_phyReconfig_register
 * Description:
 *      Register callback function for PHY need to reconfigure notification
 * Input:
 *      phyReconfig_callback    - callback function for link change
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
extern int32
dal_waMon_phyReconfig_register(rtk_port_phyReconfig_callback_t phyReconfig_callback);


/* Function Name:
 *      dal_waMon_phyReconfig_unregister
 * Description:
 *      UnRegister callback function for PHY need to reconfigure notification
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *
 */
extern int32
dal_waMon_phyReconfig_unregister(void);


/* Function Name:
 *      dal_waMon_phyEsdRstEvn_set
 * Description:
 *      set the PHY ESD reset event
 * Input:
 *      unit            - unit
 *      base_mac_id     - PHY's base MAC id for indicating the PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *
 */
extern int32
dal_waMon_phyEsdRstEvn_set(uint32 unit, rtk_port_t base_mac_id);

/* Function Name:
 *      dal_waMon_phyEsdRstEvn_clear
 * Description:
 *      clear the PHY ESD reset event
 * Input:
 *      unit            - unit
 *      base_mac_id     - PHY's base MAC id for indicating the PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *
 */
extern int32
dal_waMon_phyEsdRstEvn_clear(uint32 unit, rtk_port_t base_mac_id);

/* Function Name:
 *      dal_waMon_phyEsdRstEvn_get
 * Description:
 *      get the event status
 * Input:
 *      unit            - unit
 *      base_mac_id     - PHY's base MAC id for indicating the PHY
 * Output:
 *      is_evn_set      - TRUE or FALSE
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *
 */
extern int32
dal_waMon_phyEsdRstEvn_get(uint32 unit, rtk_port_t base_mac_id, uint32 *is_evn_set);

/* Function Name:
 *      dal_waMon_8295rClkRecovChk_set
 * Description:
 *      set flag for checking 8295R clk
 * Input:
 *      unit            - unit
 *      port     - PHY's base MAC id for indicating the PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *
 */
extern int32
dal_waMon_8295rClkRecovChk_set(uint32 unit, rtk_port_t port);

#if defined(CONFIG_SFLOW_PG_THREAD)
/* Function Name:
 *      dal_waMon_pgMon_sflow_register
 * Description:
 *      Register to receive callbacks for when sFlow enabling status change due to used-page count exceed.
 * Input:
 *      stsChg_cb   - callback function for sFlow enabling status change
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
extern int32
dal_waMon_pgMon_sflow_register(sflowStsChg_cb_f stsChg_cb);

extern void
_dal_waMonPgMon_egrCpuQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);
#endif

#endif /* __DAL_WA_MON_H__ */

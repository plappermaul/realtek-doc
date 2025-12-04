/*
 * Copyright (C) 2019 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : Definition of PON misc API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) PON misc
 */

#ifndef __DAL_RT_RTL8277C_PONMISC_H__
#define __DAL_RT_RTL8277C_PONMISC_H__
/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/ponmac.h>
#include <rtk/rt/rt_ponmisc.h>
/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      dal_rt_rtl8277c_ponmisc_init
 * Description:
 *      Configure PON misc initial settings
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
dal_rt_rtl8277c_ponmisc_init(void);

/* Function Name:
 *      dal_rt_rtl8277c_ponmisc_mode_get
 * Description:
 *      Configure PON misc get settings
 * Input:
 *      None.
 * Output:
 *      pPonMode    - current running PON mode
 *      pPonSpeed   - current running PON speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_FEATURE_NOT_SUPPORTED
 *      RT_ERR_NULL_POINTER
 * Note:
 */
extern int32
dal_rt_rtl8277c_ponmisc_mode_get(rt_ponmisc_ponMode_t *pPonMode,rt_ponmisc_ponSpeed_t *pPonSpeed);

/* Function Name:
 *      dal_rt_rtl8277c_ponmisc_mode_set
 * Description:
 *      Configure PON mode and speed settings
 * Input:
 *      ponMode    - PON mode
 *      ponSpeed   - PON speed
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_FEATURE_NOT_SUPPORTED
 *      RT_ERR_INPUT
 * Note:
 */
extern int32
dal_rt_rtl8277c_ponmisc_mode_set(rt_ponmisc_ponMode_t ponMode, rt_ponmisc_ponSpeed_t ponSpeed);

/* Function Name:
 *      dal_rt_rtl8277c_ponmisc_ponQueue_mode_get
 * Description:
 *      get currect PON queue mode setting
 * Input:
 *      None.
 * Output:
 *      pPonQueueMode    - current running PON queue mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 */
extern int32
dal_rt_rtl8277c_ponmisc_ponQueue_mode_get(rt_ponmisc_ponQueueMode_t *pPonQueueMode);

/* Function Name:
 *      dal_rt_rtl8277c_ponmisc_tcont_llid_logic2phy_get
 * Description:
 *      Get physical to logic mapping of T-CONT/LLID
 * Input:
 *      logicId   - logic index of T-CONT/LLID
 * Output:
 *      pPhyId    - pointer to physical index of T-CONT/LLID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 */
extern int32
dal_rt_rtl8277c_ponmisc_tcont_llid_logic2phy_get(uint16 logicId,uint16 *pPhyId);

/* Function Name:
*      dal_rt_rtl8277c_ponmisc_burstPolarityReverse_get
* Description:
*      Get the status of PON burst polarity reverse
* Input:
*      none
* Output:
*      pPolarity       - pointer of burst polarity reverse status
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
extern int32
dal_rt_rtl8277c_ponmisc_burstPolarityReverse_get(rt_ponmisc_polarity_t *pPolarity);

/* Function Name:
*      dal_rt_rtl8277c_ponmisc_burstPolarityReverse_set
* Description:
*      Set the status of PON burst polarity reverse
* Input:
*      polarity        - the burst polarity reverse status
* Output:
*      none
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
extern int32
dal_rt_rtl8277c_ponmisc_burstPolarityReverse_set(rt_ponmisc_polarity_t polarity);

/* Function Name:
*      dal_rt_rtl8277c_ponmisc_rxlosPolarityReverse_get
* Description:
*      Get the status of PON rx_los polarity reverse
* Input:
*      none
* Output:
*      pPolarity       - pointer of rx_los polarity reverse status
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
extern int32
dal_rt_rtl8277c_ponmisc_rxlosPolarityReverse_get(rt_ponmisc_polarity_t *pPolarity);

/* Function Name:
*      dal_rt_rtl8277c_ponmisc_rxlosPolarityReverse_set
* Description:
*      Set the status of PON rx_los polarity reverse
* Input:
*      polarity        - the rx_los polarity reverse status
* Output:
*      none
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
extern int32
dal_rt_rtl8277c_ponmisc_rxlosPolarityReverse_set(rt_ponmisc_polarity_t polarity);

/* Function Name:
*      dal_rt_rtl8277c_ponmisc_forceLaserState_get
* Description:
*      Get Force Laser status
* Input:
*      none
* Output:
*      pStatus       - pointer of Force Laser status
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
extern int32
dal_rt_rtl8277c_ponmisc_forceLaserState_get(rt_ponmisc_laser_status_t *pStatus);

/* Function Name:
*      dal_rt_rtl8277c_ponmisc_forceLaserState_set
* Description:
*      Set Force Laser status
* Input:
*      status       - Force Laser status
* Output:
*      none
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
extern int32
dal_rt_rtl8277c_ponmisc_forceLaserState_set(rt_ponmisc_laser_status_t status);

/* Function Name:
*      dal_rt_rtl8277c_ponmisc_forcePRBS_get
* Description:
*      Get the PRBS config
* Input:
*      none
* Output:
*      pPrbsCfg       - pointer of PRBS config
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
extern int32
dal_rt_rtl8277c_ponmisc_forcePRBS_get(rt_ponmisc_prbs_t *pPrbsCfg);

/* Function Name:
*      dal_rt_rtl8277c_ponmisc_forcePRBS_set
* Description:
*      Set the PRBS config
* Input:
*      none
* Output:
*      prbsCfg       - PRBS config
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
extern int32
dal_rt_rtl8277c_ponmisc_forcePRBS_set(rt_ponmisc_prbs_t prbsCfg);

/* Function Name:
*      dal_rt_rtl8277c_ponmisc_forcePRBS_rx_get
* Description:
*      Get the PRBS RX config
* Input:
*      none
* Output:
*      pPrbsCfg       - pointer of PRBS RX config
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
extern int32
dal_rt_rtl8277c_ponmisc_forcePRBS_rx_get(rt_ponmisc_prbs_t *pPrbsCfg);

/* Function Name:
*      dal_rt_rtl8277c_ponmisc_forcePRBS_rx_set
* Description:
*      Set the PRBS RX config
* Input:
*      prbsCfg       - PRBS RX config
* Output:
*      none
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
extern int32
dal_rt_rtl8277c_ponmisc_forcePRBS_rx_set(rt_ponmisc_prbs_t prbsCfg);

/* Function Name:
*      dal_rt_rtl8277c_ponmisc_prbsRxStatus_get
* Description:
*      Get the PRBS RX config
* Input:
*      none
* Output:
*      pPrbsCfg       - pointer of PRBS RX config
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
extern int32
dal_rt_rtl8277c_ponmisc_prbsRxStatus_get(rt_ponmisc_prbsRxStatus_t *pPrbsRxStatus);

/* Function Name:
*      dal_rt_rtl8277c_ponmisc_selfRogue_cfg_get
* Description:
*      Get the self rogue detect config
* Input:
*      none
* Output:
*      pCfg       - pointer of self rogue detect config
* Return:
*      RT_ERR_OK            - OK
*      RT_ERR_FAILED        - failure
*      RT_ERR_NULL_POINTER  - input parameter may be null pointer
*      RT_ERR_OUT_OF_RANGE  - value out of range
* Note:
*
*/
extern int32
dal_rt_rtl8277c_ponmisc_selfRogue_cfg_get(rtk_enable_t *pCfg);

/* Function Name:
*      dal_rt_rtl8277c_ponmisc_selfRogue_cfg_set
* Description:
*      Set the self rogue detect config
* Input:
*      cfg       - self rogue detect config
* Output:
*      none
* Return:
*      RT_ERR_OK           - OK
*      RT_ERR_FAILED       - failure
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
extern int32
dal_rt_rtl8277c_ponmisc_selfRogue_cfg_set(rtk_enable_t cfg);

/* Function Name:
*      dal_rt_rtl8277c_ponmisc_selfRogue_count_get
* Description:
*      Get the self rogue detect counter
* Input:
*      none
* Output:
*      pCnt       - pointer of self rogue detect counter
* Return:
*      RT_ERR_OK            - OK
*      RT_ERR_FAILED        - failure
*      RT_ERR_NULL_POINTER  - input parameter may be null pointer
* Note:
*
*/
extern int32
dal_rt_rtl8277c_ponmisc_selfRogue_count_get(rt_ponmisc_rogue_cnt_t *pCnt);

/* Function Name:
*      dal_rt_rtl8277c_ponmisc_selfRogue_count_clear
* Description:
*      Clear the self rogue detect counter
* Input:
*      none
* Output:
*      none
* Return:
*      RT_ERR_OK           - OK
*      RT_ERR_FAILED       - failure
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
extern int32
dal_rt_rtl8277c_ponmisc_selfRogue_count_clear(void);

/* Function Name:
*      dal_rt_rtl8277c_ponmisc_dying_gasp_force_trigger
* Description:
*      force trigger dying gasp signal
* Input:
*      none
* Output:
*      none
* Return:
*      RT_ERR_OK           - OK
*      RT_ERR_FAILED       - failure
* Note:
*
*/
extern int32
dal_rt_rtl8277c_ponmisc_dying_gasp_force_trigger(void);
#endif

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
 * Purpose : Definition those public statistic APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) statistic counter reset
 *           2) statistic counter get
 *
 */
#ifndef __DAL_LONGAN_STAT_H__
#define __DAL_LONGAN_STAT_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/stat.h>
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

/* Module Name : STAT */


/* Function Name:
 *      dal_longan_statMapper_init
 * Description:
 *      Hook stat module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook stat module before calling any stat APIs.
 */
extern int32
dal_longan_statMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_longan_stat_init
 * Description:
 *      Initialize stat module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize stat module before calling any vlan APIs.
 */
extern int32
dal_longan_stat_init(uint32 unit);

/* Function Name:
 *      dal_longan_stat_global_reset
 * Description:
 *      Reset the global counters in the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      None
 */
extern int32
dal_longan_stat_global_reset(uint32 unit);

/* Function Name:
 *      dal_longan_stat_port_reset
 * Description:
 *      Reset the specified port counters in the specified device.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      None
 */
extern int32
dal_longan_stat_port_reset(uint32 unit, rtk_port_t port);

/* Function Name:
 *      dal_longan_stat_global_get
 * Description:
 *      Get one specified global counter in the specified device.
 * Input:
 *      unit     - unit id
 *      cntr_idx - specified global counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT                 - The module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL    - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_INVALID_GLOBAL_CNTR - Invalid Global Counter
 * Note:
 *      None
 */
extern int32
dal_longan_stat_global_get(uint32 unit, rtk_stat_global_type_t cntr_idx, uint64 *pCntr);

/* Function Name:
 *      dal_longan_stat_global_getAll
 * Description:
 *      Get all global counters in the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      pGlobal_cntrs - pointer buffer of global counter structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT                 - The module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_STAT_INVALID_GLOBAL_CNTR - Invalid Global Counter
 * Note:
 *      None
 */
extern int32
dal_longan_stat_global_getAll(uint32 unit, rtk_stat_global_cntr_t *pGlobal_cntrs);

/* Function Name:
 *      dal_longan_stat_port_get
 * Description:
 *      Get one specified port counter in the specified device.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      cntr_idx - specified port counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_PORT_ID                - invalid port id
 *      RT_ERR_NULL_POINTER           - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE           - input parameter out of range
 *      RT_ERR_STAT_INVALID_PORT_CNTR - Invalid Port Counter
 * Note:
 *      None
 */
extern int32
dal_longan_stat_port_get(uint32 unit, rtk_port_t port, rtk_stat_port_type_t cntr_idx, uint64 *pCntr);

/* Function Name:
 *      dal_longan_stat_port_getAll
 * Description:
 *      Get all counters of one specified port in the specified device.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pPort_cntrs - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_PORT_ID                - invalid port id
 *      RT_ERR_NULL_POINTER           - input parameter may be null pointer
 *      RT_ERR_STAT_INVALID_PORT_CNTR - Invalid Port Counter
 * Note:
 *      None
 */
extern int32
dal_longan_stat_port_getAll(uint32 unit, rtk_port_t port, rtk_stat_port_cntr_t *pPort_cntrs);

/* Function Name:
 *      dal_longan_stat_tagLenCntIncEnable_get
 * Description:
 *      Get RX/TX counter to include or exclude tag length in the specified device.
 * Input:
 *      unit        - unit id
 *      tagCnt_type - specified RX counter or TX counter
 * Output:
 *      pEnable     - pointer buffer of including/excluding tag length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_stat_tagLenCntIncEnable_get(uint32 unit, rtk_stat_tagCnt_type_t tagCnt_type, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_longan_stat_tagLenCntIncEnable_set
 * Description:
 *      Set RX/TX counter to include or exclude tag length in the specified device.
 * Input:
 *      unit        - unit id
 *      tagCnt_type - specified RX counter or TX counter
 *      enable      - include/exclude Tag length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_longan_stat_tagLenCntIncEnable_set(uint32 unit, rtk_stat_tagCnt_type_t tagCnt_type, rtk_enable_t enable);


/* Function Name:
 *      dal_longan_stat_stackHdrLenCntIncEnable_get
 * Description:
 *      Get RX/TX counter to include or exclude tag length in the specified device.
 * Input:
 *      unit        - unit id
 *      type        - specified RX counter or TX counter
 * Output:
 *      pEnable     - pointer buffer of including/excluding tag length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Stacking header length can be included or excluded to the counter through the API.
 */
extern int32
dal_longan_stat_stackHdrLenCntIncEnable_get(uint32 unit, rtk_stat_stackHdrCnt_type_t type, rtk_enable_t *pEnable);


/* Function Name:
 *      dal_longan_stat_stackHdrLenCntIncEnable_set
 * Description:
 *      Set RX/TX counter to include or exclude stacking header length in the specified device.
 * Input:
 *      unit        - unit id
 *      type        - specified RX counter or TX counter
 *      enable      - include/exclude Tag length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Stacking header length can be included or excluded to the counter through the API.
 */
int32
extern dal_longan_stat_stackHdrLenCntIncEnable_set(uint32 unit, rtk_stat_stackHdrCnt_type_t type, rtk_enable_t enable);


/* Function Name:
 *      dal_longan_stat_flexibleCntRange_get
 * Description:
 *      Get the flexible mib counter max/min boundary.
 * Input:
 *      unit        - unit id
 *      idx         - flexible mib counter set index
 * Output:
 *      pRange      - pointer buffer of the boundary value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Per flexible counter MAX/MIN boundary value can be up to max frame length.
 */
extern int32
dal_longan_stat_flexibleCntRange_get(uint32 unit, uint32 idx, rtk_stat_flexCntSet_t *pRange);


/* Function Name:
 *      rtk_stat_flexibleCntRange_set
 * Description:
 *      Set the flexible mib counter max/min boundary.
 * Input:
 *      unit        - unit id
 *      idx         - flexible mib counter set index
 *      pRange      - the pointer of the boundary value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Per flexible counter MAX/MIN boundary value can be up to max frame length.
 */
extern int32
dal_longan_stat_flexibleCntRange_set(uint32 unit, uint32 idx, rtk_stat_flexCntSet_t *pRange);

#endif /* __DAL_LONGAN_STAT_H__ */

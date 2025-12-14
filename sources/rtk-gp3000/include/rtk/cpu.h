/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
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
 * Purpose : Definition of CPU Tag API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) CPU tag functions set/get
 */

#ifndef __RTK_CPU_H__
#define __RTK_CPU_H__


/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
typedef enum
{
    CPU_GMAC_0 = 0,
    CPU_GMAC_1,
    CPU_GMAC_2,
    CPU_GMAC_3,
    CPU_GMAC_END
} rtk_cpu_gmac_t;

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_cpu_init
 * Description:
 *      Initialize cpu tag module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
extern int32
rtk_cpu_init(void);

/* Function Name:
 *      rtk_cpu_awarePortMask_set
 * Description:
 *      Set CPU awared port mask.
 * Input:
 *      port_mask   - CPU awared port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
extern int32
rtk_cpu_awarePortMask_set(rtk_portmask_t port_mask);

/* Function Name:
 *      dal_apollo_cpu_awarePortMask_get
 * Description:
 *      Get CPU awared port mask.
 * Input:
 *      pPort_mask   - the pointer of CPU awared port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
extern int32
rtk_cpu_awarePortMask_get(rtk_portmask_t *pPort_mask);

/* Function Name:
 *      rtk_cpu_tagFormat_set
 * Description:
 *      Set CPU tag format.
 * Input:
 *      mode    - CPU tag format mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
extern int32
rtk_cpu_tagFormat_set(rtk_cpu_tag_fmt_t mode);

/* Function Name:
 *      rtk_cpu_tagFormat_get
 * Description:
 *      Get CPU tag format.
 * Input:
 *      pMode    - the pointer of CPU tag format mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
extern int32
rtk_cpu_tagFormat_get(rtk_cpu_tag_fmt_t *pMode);

/* Function Name:
 *      rtk_cpu_trapInsertTag_set
 * Description:
 *      Set trap CPU insert tag state.
 * Input:
 *      state    - insert CPU tag state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
extern int32
rtk_cpu_trapInsertTag_set(rtk_enable_t state);

/* Function Name:
 *      rtk_cpu_trapInsertTag_get
 * Description:
 *      Get trap CPU insert tag state.
 * Input:
 *      pState    - the pointer of insert CPU tag state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
extern int32
rtk_cpu_trapInsertTag_get(rtk_enable_t *pState);

/* Function Name:
 *      rtk_cpu_tagAware_set
 * Description:
 *      Set CPU tag aware state.
 * Input:
 *      state    - CPU tag aware state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
extern int32
rtk_cpu_tagAware_set(rtk_enable_t state);

/* Function Name:
 *      rtk_cpu_tagAware_get
 * Description:
 *      Get CPU tag aware state.
 * Input:
 *      pState    - the pointer of CPU tag aware state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
extern int32
rtk_cpu_tagAware_get(rtk_enable_t *pState);

/* Function Name:
 *      rtk_cpu_trapInsertTagByPort_set
 * Description:
 *      Set trap CPU insert tag state.
 * Input:
 *      port     - CPU port ID
 *      state    - insert CPU tag state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
extern int32
rtk_cpu_trapInsertTagByPort_set(rtk_port_t port, rtk_enable_t state);

/* Function Name:
 *      rtk_cpu_trapInsertTagByPort_get
 * Description:
 *      Get trap CPU insert tag state.
 * Input:
 *      port     - CPU port ID
 *      pState   - the pointer of insert CPU tag state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
extern int32
rtk_cpu_trapInsertTagByPort_get(rtk_port_t port, rtk_enable_t *pState);

/* Function Name:
 *      rtk_cpu_tagAwareByPort_set
 * Description:
 *      Set CPU tag aware state.
 * Input:
 *      port     - CPU port ID
 *      state    - CPU tag aware state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
extern int32
rtk_cpu_tagAwareByPort_set(rtk_port_t port, rtk_enable_t state);

/* Function Name:
 *      rtk_cpu_tagAwareByPort_get
 * Description:
 *      Get CPU tag aware state.
 * Input:
 *      port     - CPU port ID
 *      pState   - the pointer of CPU tag aware state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
extern int32
rtk_cpu_tagAwareByPort_get(rtk_port_t port, rtk_enable_t *pState);

#endif /*#ifndef __RTK_CPU_H__*/

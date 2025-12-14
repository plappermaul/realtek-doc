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
 * $Revision: 46644 $
 * $Date: 2014-02-26 18:16:35 +0800 (?Ÿæ?ä¸? 26 äºŒæ? 2014) $
 *
 * Purpose : Definition of Switch Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Switch parameter settings
 *           (2) Management address and vlan configuration.
 *
 */


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <dal/luna_g3/dal_luna_g3.h>
#include <dal/luna_g3/dal_luna_g3_switch.h>
#include <osal/time.h>

#include <cortina-api/include/port.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32 switch_init = INIT_NOT_COMPLETED;

/*
 * Macro Declaration
 */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      dal_luna_g3_switch_init
 * Description:
 *      Initialize switch module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Module must be initialized before using all of APIs in this module
 */
int32
dal_luna_g3_switch_init(void)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    switch_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_luna_g3_switch_init */

/* Function Name:
 *      dal_luna_g3_switch_phyPortId_get
 * Description:
 *      Get physical port id from logical port name
 * Input:
 *      portName - logical port name
 * Output:
 *      pPortId  - pointer to the physical port id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Call RTK API the port ID must get from this API
 */
int32
dal_luna_g3_switch_phyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPortId), RT_ERR_NULL_POINTER);

    /*get chip info to check port name mapping*/
    switch(portName)
    {
        case RTK_PORT_UTP0:
            *pPortId = 0;
            break;
        case RTK_PORT_UTP1:
            *pPortId = 1;
            break;
        case RTK_PORT_UTP2:
            *pPortId = 2;
            break;
        case RTK_PORT_UTP3:
            *pPortId = 3;
            break;
        case RTK_PORT_UTP5:
            *pPortId = 5;
            break;
        case RTK_PORT_UTP6:
            *pPortId = 6;
            break;
        case RTK_PORT_UTP7:
        case RTK_PORT_PON:
            *pPortId = 7;
            break;
        case RTK_PORT_CPU0:
            *pPortId = 16;
            break;
        case RTK_PORT_CPU1:
            *pPortId = 17;
            break;
        case RTK_PORT_CPU2:
            *pPortId = 18;
            break;
        case RTK_PORT_CPU3:
            *pPortId = 19;
            break;
        case RTK_PORT_CPU4:
            *pPortId = 20;
            break;
        case RTK_PORT_CPU5:
            *pPortId = 21;
            break;
        case RTK_PORT_CPU6:
            *pPortId = 22;
            break;
        case RTK_PORT_CPU7:
            *pPortId = 23;
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_switch_phyPortId_get */

/* Function Name:
 *      dal_luna_g3_switch_version_get
 * Description:
 *      Get chip version
 * Input:
 *      pChipId    - chip id
 *      pRev       - revision id
 *      pSubtype   - sub type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 */

int32 drv_swcore_cid_get(uint32 *pChip_id, uint32 *pChip_rev_id);

int32
dal_luna_g3_switch_version_get(uint32 *pChipId, uint32 *pRev, uint32 *pSubtype)
{
    int32  ret;
    static uint8 is_init = 0;
    static uint32 chipId;
    static uint32 chipRev;
    static uint32 chipSubtype;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pChipId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRev), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSubtype), RT_ERR_NULL_POINTER);

    if(0 == is_init)
    {
        if (drv_swcore_cid_get(&chipId, &chipRev) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }

        chipSubtype = 1234;

        is_init = 1;
    }

    /* function body */
    *pChipId = chipId;
    *pRev = chipRev;
    *pSubtype = chipSubtype;

    return RT_ERR_OK;
}   /* end of dal_luna_g3_switch_version_get */

/* Function Name:
  *      dal_luna_g3_switch_maxPktLenByPort_get
  * Description:
  *      Get the max packet length setting of specific port
  * Input:
  *      port - speed type
  * Output:
  *      pLen - pointer to the max packet length
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_NULL_POINTER - input parameter may be null pointer
  *      RT_ERR_INPUT        - invalid enum speed type
  * Note:
  */
int32
dal_luna_g3_switch_maxPktLenByPort_get(rtk_port_t port, uint32 *pLen)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_uint32_t size=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = ca_port_max_frame_size_get(0,port_id,&size))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    *pLen = size;

    return RT_ERR_OK;
} /*end of dal_luna_g3_switch_maxPktLenByPort_get*/

/* Function Name:
  *      dal_luna_g3_switch_maxPktLenByPort_set
  * Description:
  *      Set the max packet length of specific port
  * Input:
  *      port  - port
  *      len   - max packet length
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_INPUT   - invalid enum speed type
  * Note:
  */
int32
dal_luna_g3_switch_maxPktLenByPort_set(rtk_port_t port, uint32 len)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_uint32_t size=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    
    /* function body */
    size = len;

    if((ret = ca_port_max_frame_size_set(0,port_id,size))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}/*end of dal_luna_g3_switch_maxPktLenByPort_set*/

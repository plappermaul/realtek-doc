/*
 * Copyright (C) 2013 Realtek Semiconductor Corp.
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
 * Purpose : RTK switch high-level API for RTL8367/RTL8367C
 * Feature : Here is a list of all functions and variables in CPU module.
 *
 */

#include <rtk_switch.h>
#include <rtk_error.h>
#include <cpu.h>
#include <linux/string.h>

#include <dal/dal_mgmt.h>

/* Function Name:
 *      rtk_cpu_enable_set
 * Description:
 *      Set CPU port function enable/disable.
 * Input:
 *      enable - CPU port function enable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameter.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can set CPU port function enable/disable.
 */
rtk_api_ret_t rtk_cpu_enable_set(rtk_enable_t enable)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->cpu_enable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->cpu_enable_set(enable);    
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_cpu_enable_get
 * Description:
 *      Get CPU port and its setting.
 * Input:
 *      None
 * Output:
 *      pEnable - CPU port function enable
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_L2_NO_CPU_PORT   - CPU port is not exist
 * Note:
 *      The API can get CPU port function enable/disable.
 */
rtk_api_ret_t rtk_cpu_enable_get(rtk_enable_t *pEnable)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->cpu_enable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->cpu_enable_get(pEnable);    
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_cpu_tagPort_set
 * Description:
 *      Set CPU port and CPU tag insert mode.
 * Input:
 *      port - Port id.
 *      mode - CPU tag insert for packets egress from CPU port.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameter.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can set CPU port and inserting proprietary CPU tag mode (Length/Type 0x8899)
 *      to the frame that transmitting to CPU port.
 *      The inset cpu tag mode is as following:
 *      - CPU_INSERT_TO_ALL
 *      - CPU_INSERT_TO_TRAPPING
 *      - CPU_INSERT_TO_NONE
 */
rtk_api_ret_t rtk_cpu_tagPort_set(rtk_port_t port, rtk_cpu_insert_t mode)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->cpu_tagPort_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->cpu_tagPort_set(port, mode);    
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_cpu_tagPort_get
 * Description:
 *      Get CPU port and CPU tag insert mode.
 * Input:
 *      None
 * Output:
 *      pPort - Port id.
 *      pMode - CPU tag insert for packets egress from CPU port, 0:all insert 1:Only for trapped packets 2:no insert.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_L2_NO_CPU_PORT   - CPU port is not exist
 * Note:
 *      The API can get configured CPU port and its setting.
 *      The inset cpu tag mode is as following:
 *      - CPU_INSERT_TO_ALL
 *      - CPU_INSERT_TO_TRAPPING
 *      - CPU_INSERT_TO_NONE
 */
rtk_api_ret_t rtk_cpu_tagPort_get(rtk_port_t *pPort, rtk_cpu_insert_t *pMode)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->cpu_tagPort_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->cpu_tagPort_get(pPort, pMode);    
    RTK_API_UNLOCK();

    return retVal;
}


/* Function Name:
 *      rtk_cpu_awarePort_set
 * Description:
 *      Set CPU aware port mask.
 * Input:
 *      portmask - Port mask.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_MASK      - Invalid port mask.
 * Note:
 *      The API can set configured CPU aware port mask.
 */
rtk_api_ret_t rtk_cpu_awarePort_set(rtk_portmask_t *pPortmask)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->cpu_awarePort_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->cpu_awarePort_set(pPortmask);    
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_cpu_awarePort_get
 * Description:
 *      Get CPU aware port mask.
 * Input:
 *      None
 * Output:
 *      pPortmask - Port mask.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      The API can get configured CPU aware port mask.
 */
rtk_api_ret_t rtk_cpu_awarePort_get(rtk_portmask_t *pPortmask)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->cpu_awarePort_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->cpu_awarePort_get(pPortmask);    
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_cpu_tagPosition_set
 * Description:
 *      Set CPU tag position.
 * Input:
 *      position - CPU tag position.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT      - Invalid input.
 * Note:
 *      The API can set CPU tag position.
 */
rtk_api_ret_t rtk_cpu_tagPosition_set(rtk_cpu_position_t position)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->cpu_tagPosition_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->cpu_tagPosition_set(position);    
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_cpu_tagPosition_get
 * Description:
 *      Get CPU tag position.
 * Input:
 *      None
 * Output:
 *      pPosition - CPU tag position.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT      - Invalid input.
 * Note:
 *      The API can get CPU tag position.
 */
rtk_api_ret_t rtk_cpu_tagPosition_get(rtk_cpu_position_t *pPosition)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->cpu_tagPosition_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->cpu_tagPosition_get(pPosition);    
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_cpu_tagLength_set
 * Description:
 *      Set CPU tag length.
 * Input:
 *      length - CPU tag length.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT      - Invalid input.
 * Note:
 *      The API can set CPU tag length.
 */
rtk_api_ret_t rtk_cpu_tagLength_set(rtk_cpu_tag_length_t length)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->cpu_tagLength_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->cpu_tagLength_set(length);    
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_cpu_tagLength_get
 * Description:
 *      Get CPU tag length.
 * Input:
 *      None
 * Output:
 *      pLength - CPU tag length.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT      - Invalid input.
 * Note:
 *      The API can get CPU tag length.
 */
rtk_api_ret_t rtk_cpu_tagLength_get(rtk_cpu_tag_length_t *pLength)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->cpu_tagLength_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->cpu_tagLength_get(pLength);    
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_cpu_priRemap_set
 * Description:
 *      Configure CPU priorities mapping to internal absolute priority.
 * Input:
 *      int_pri     - internal priority value.
 *      new_pri    - new internal priority value.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_VLAN_PRIORITY    - Invalid 1p priority.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      Priority of CPU tag assignment for internal asic priority, and it is used for queue usage and packet scheduling.
 */
rtk_api_ret_t rtk_cpu_priRemap_set(rtk_pri_t int_pri, rtk_pri_t new_pri)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->cpu_priRemap_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->cpu_priRemap_set(int_pri, new_pri);    
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_cpu_priRemap_get
 * Description:
 *      Configure CPU priorities mapping to internal absolute priority.
 * Input:
 *      int_pri     - internal priority value.
 * Output:
 *      pNew_pri    - new internal priority value.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_VLAN_PRIORITY    - Invalid 1p priority.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      Priority of CPU tag assignment for internal asic priority, and it is used for queue usage and packet scheduling.
 */
rtk_api_ret_t rtk_cpu_priRemap_get(rtk_pri_t int_pri, rtk_pri_t *pNew_pri)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->cpu_priRemap_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->cpu_priRemap_get(int_pri, pNew_pri);    
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_cpu_acceptLength_set
 * Description:
 *      Set CPU accept  length.
 * Input:
 *      length - CPU tag length.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT      - Invalid input.
 * Note:
 *      The API can set CPU accept length.
 */
rtk_api_ret_t rtk_cpu_acceptLength_set(rtk_cpu_rx_length_t length)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->cpu_acceptLength_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->cpu_acceptLength_set(length);    
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_cpu_acceptLength_get
 * Description:
 *      Get CPU accept length.
 * Input:
 *      None
 * Output:
 *      pLength - CPU tag length.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT      - Invalid input.
 * Note:
 *      The API can get CPU accept length.
 */
rtk_api_ret_t rtk_cpu_acceptLength_get(rtk_cpu_rx_length_t *pLength)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->cpu_acceptLength_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->cpu_acceptLength_get(pLength);    
    RTK_API_UNLOCK();

    return retVal;
}


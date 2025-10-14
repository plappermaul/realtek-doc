/*
 * Copyright (C) 2021 Realtek Semiconductor Corp.
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
 * Purpose : RTK switch high-level API
 * Feature : Here is a list of all functions and variables in GPIO module.
 *
 */

#include <rtk_switch.h>
#include <rtk_error.h>
#include <gpio.h>
#include <linux/string.h>

#include <dal/dal_mgmt.h>

/* Function Name:
 *      rtk_gpio_input_get
 * Description:
 *      Get gpio input
 * Input:
 *      pin 		- GPIO pin
 * Output:
 *      pInput 		- GPIO input
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE 	- input out of range.
 *      RT_ERR_NULL_POINTER 	- input parameter is null pointer.
 * Note:
 *      None
 */
rtk_api_ret_t rtk_gpio_input_get(rtk_uint32 pin, rtk_uint32 *pInput)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->gpio_input_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    retVal = RT_MAPPER->gpio_input_get(pin, pInput);
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_gpio_output_set
 * Description:
 *      Set GPIO output value.
 * Input:
 *      pin     - GPIO pin
 *      output  - 1 or 0
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK            - OK
 *      RT_ERR_FAILED        - Failed
 *      RT_ERR_SMI           - SMI access error
 *      RT_ERR_INPUT         - Invalid input parameter.
 *      RT_ERR_OUT_OF_RANGE  - input parameter out of range.
 * Note:
 *      The API can set GPIO pin output 1 or 0.
 */
rtk_api_ret_t rtk_gpio_output_set(rtk_uint32 pin, rtk_uint32 output)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->gpio_output_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    retVal = RT_MAPPER->gpio_output_set(pin, output);
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_gpio_output_get
 * Description:
 *      Get GPIO output.
 * Input:
 *      pin      - GPIO pin
 * Output:
 *      pOutput  - GPIO output
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range.
 *      RT_ERR_NULL_POINTER 	- input parameter is null pointer.
 * Note:
 *      The API can get GPIO output.
 */
rtk_api_ret_t rtk_gpio_output_get(rtk_uint32 pin, rtk_uint32 *pOutput)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->gpio_output_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    retVal = RT_MAPPER->gpio_output_get(pin, pOutput);
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_gpio_state_set
 * Description:
 *      Set GPIO control.
 * Input:
 *      pin     - GPIO pin
 *      state   - GPIO enable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK            - OK
 *      RT_ERR_FAILED        - Failed
 *      RT_ERR_SMI           - SMI access error
 *      RT_ERR_INPUT         - Invalid input parameter.
 *      RT_ERR_OUT_OF_RANGE  - input parameter out of range.
 *      RT_ERR_ENABLE        - invalid enable parameter .
 * Note:
 *      The API can set GPIO pin output 1 or 0.
 */
rtk_api_ret_t rtk_gpio_state_set(rtk_uint32 pin, rtk_enable_t state)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->gpio_state_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    retVal = RT_MAPPER->gpio_state_set(pin, state);
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_gpio_state_get
 * Description:
 *      Get GPIO enable state.
 * Input:
 *      pin      - GPIO pin
 * Output:
 *      pState   - GPIO state
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range.
 *      RT_ERR_NULL_POINTER 	- input parameter is null pointer.
 * Note:
 *      The API can get GPIO enable state.
 */
rtk_api_ret_t rtk_gpio_state_get(rtk_uint32 pin, rtk_enable_t *pState)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->gpio_state_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    retVal = RT_MAPPER->gpio_state_get(pin, pState);
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_gpio_mode_set
 * Description:
 *      Set GPIO mode.
 * Input:
 *      pin     - GPIO pin
 *      mode    - Output or input mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK            - OK
 *      RT_ERR_FAILED        - Failed
 *      RT_ERR_SMI           - SMI access error
 *      RT_ERR_INPUT         - Invalid input parameter.
 *      RT_ERR_OUT_OF_RANGE  - input parameter out of range.
 * Note:
 *      The API can set GPIO  to input or output mode.
 */
rtk_api_ret_t rtk_gpio_mode_set(rtk_uint32 pin, rtk_gpio_mode_t mode)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->gpio_mode_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    retVal = RT_MAPPER->gpio_mode_set(pin, mode);
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_gpio_mode_get
 * Description:
 *      Get GPIO mode.
 * Input:
 *      pin      - GPIO pin
 * Output:
 *      pMode    - GPIO mode
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range.
  *      RT_ERR_NULL_POINTER 	- input parameter is null pointer.
 * Note:
 *      The API can get GPIO mode.
 */
rtk_api_ret_t rtk_gpio_mode_get(rtk_uint32 pin, rtk_gpio_mode_t *pMode)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->gpio_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    retVal = RT_MAPPER->gpio_mode_get(pin, pMode);
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_gpio_aclEnClear_set
 * Description:
 *      Set GPIO acl clear.
 * Input:
 *      pin     - GPIO pin
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK            - OK
 *      RT_ERR_FAILED        - Failed
 *      RT_ERR_SMI           - SMI access error
 *      RT_ERR_INPUT         - Invalid input parameter.
 *      RT_ERR_OUT_OF_RANGE  - input parameter out of range.
 * Note:
 *      The API can set GPIO ACL clear.
 */
rtk_api_ret_t rtk_gpio_aclEnClear_set(rtk_uint32 pin)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->gpio_aclEnClear_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    retVal = RT_MAPPER->gpio_aclEnClear_set(pin);
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_gpio_aclEnClear_get
 * Description:
 *      Get GPIO acl clear.
 * Input:
 *      pin      - GPIO pin
 * Output:
 *      pAclEn   - GPIO acl enable
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range.
 *      RT_ERR_NULL_POINTER 	- input parameter is null pointer.
 * Note:
 *      The API can get GPIO acl enable clear.
 */
rtk_api_ret_t rtk_gpio_aclEnClear_get(rtk_uint32 pin, rtk_enable_t *pAclEn)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->gpio_aclEnClear_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    retVal = RT_MAPPER->gpio_aclEnClear_get(pin, pAclEn);
    RTK_API_UNLOCK();

    return retVal;
}


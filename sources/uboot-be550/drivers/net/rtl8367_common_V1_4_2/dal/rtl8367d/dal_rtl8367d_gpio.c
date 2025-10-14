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
 * $Revision$
 * $Date$
 *
 * Purpose : RTK switch high-level API for RTL8367/RTL8367D
 * Feature : Here is a list of all functions and variables in GPIO module.
 *
 */

#include <rtk_switch.h>
#include <rtk_error.h>
#include <dal/rtl8367d/rtl8367d_asicdrv.h>
#include <dal/rtl8367d/dal_rtl8367d_gpio.h>
#include <linux/string.h>

/* Function Name:
 *      dal_rtl8367d_gpio_input_get
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
rtk_api_ret_t dal_rtl8367d_gpio_input_get(rtk_uint32 pin, rtk_uint32 *pInput)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

	if(pin >= RTL8367D_GPIOPINNO)
		return RT_ERR_OUT_OF_RANGE;

    if(NULL == pInput)
        return RT_ERR_NULL_POINTER;
    
    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_GPIO_67C_I_X0 + (pin / 16), (pin % 16), pInput)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_gpio_output_set
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
rtk_api_ret_t dal_rtl8367d_gpio_output_set(rtk_uint32 pin, rtk_uint32 output)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

	if(pin >= RTL8367D_GPIOPINNO)
		return RT_ERR_OUT_OF_RANGE;

    if (output > 1)
        return RT_ERR_INPUT;

    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_GPIO_67C_O_X0 + (pin / 16), (pin % 16), output)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_gpio_output_get
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
rtk_api_ret_t dal_rtl8367d_gpio_output_get(rtk_uint32 pin, rtk_uint32 *pOutput)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

	if(pin >= RTL8367D_GPIOPINNO)
		return RT_ERR_OUT_OF_RANGE;

    if(NULL == pOutput)
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_GPIO_67C_O_X0 + (pin / 16), (pin % 16), pOutput)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_gpio_state_set
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
rtk_api_ret_t dal_rtl8367d_gpio_state_set(rtk_uint32 pin, rtk_enable_t state)
{
    rtk_api_ret_t retVal;
    rtk_uint32 gpioState;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(pin >= RTL8367D_GPIOPINNO)
        return RT_ERR_OUT_OF_RANGE;

    switch (state)
    {
        case DISABLED:
            gpioState = 0;
            break;
        case ENABLED:
            gpioState = 1;
            break;
        default:
            return RT_ERR_ENABLE;
    }
    
    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_GPIO_MODE_67C_X0 + (pin / 16), (pin % 16), gpioState)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_gpio_state_get
 * Description:
 *      Get GPIO enable state.
 * Input:
 *      pin      - GPIO pin
 * Output:
 *      pState   - GPIO enable
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
rtk_api_ret_t dal_rtl8367d_gpio_state_get(rtk_uint32 pin, rtk_enable_t *pState)
{
    rtk_api_ret_t retVal;
    rtk_uint32 gpioState;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

	if(pin >= RTL8367D_GPIOPINNO)
		return RT_ERR_OUT_OF_RANGE;

    if(NULL == pState)
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_GPIO_MODE_67C_X0 + (pin / 16), (pin % 16), &gpioState)) != RT_ERR_OK)
        return retVal;

    switch (gpioState)
    {
        case 0:
            *pState = DISABLED;
            break;
        case 1:
            *pState = ENABLED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_gpio_mode_set
 * Description:
 *      Set GPIO mode.
 * Input:
 *      pin     - GPIO pin
 *      mode    - 1 or 0
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
rtk_api_ret_t dal_rtl8367d_gpio_mode_set(rtk_uint32 pin, rtk_gpio_mode_t mode)
{
    rtk_api_ret_t retVal;
    rtk_uint32 gpioMode;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

	if(pin >= RTL8367D_GPIOPINNO)
		return RT_ERR_OUT_OF_RANGE;

    switch (mode)
    {
        case GPIO_MODE_OUTPUT:
            gpioMode = 0;
            break;
        case GPIO_MODE_INPUT:
            gpioMode = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }
    
    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_GPIO_67C_OE_X0 + (pin / 16), (pin % 16), gpioMode)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_gpio_mode_get
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
rtk_api_ret_t dal_rtl8367d_gpio_mode_get(rtk_uint32 pin, rtk_gpio_mode_t *pMode)
{
    rtk_api_ret_t retVal;
    rtk_uint32 gpioMode;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

	if(pin >= RTL8367D_GPIOPINNO)
		return RT_ERR_OUT_OF_RANGE;

    if(NULL == pMode)
        return RT_ERR_NULL_POINTER;
    
    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_GPIO_67C_OE_X0 + (pin / 16), (pin % 16), &gpioMode)) != RT_ERR_OK)
        return retVal;

    switch (gpioMode)
    {
        case 0:
            *pMode = GPIO_MODE_OUTPUT;
            break;
        case 1:
            *pMode = GPIO_MODE_INPUT;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_gpio_aclEnClear_set
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
rtk_api_ret_t dal_rtl8367d_gpio_aclEnClear_set(rtk_uint32 pin)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(pin >= RTL8367D_GPIOPINNO)
        return RT_ERR_OUT_OF_RANGE;
    
    /* ACL clear */
    if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_ACL_GPIO0 + (pin / 16),  (pin % 16), 1)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_gpio_aclEnClear_get
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
rtk_api_ret_t dal_rtl8367d_gpio_aclEnClear_get(rtk_uint32 pin, rtk_enable_t *pAclEn)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

	if(pin >= RTL8367D_GPIOPINNO)
		return RT_ERR_OUT_OF_RANGE;

    if(NULL == pAclEn)
        return RT_ERR_NULL_POINTER;
    
    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_ACL_GPIO0 + (pin / 16), (pin % 16), (rtk_uint32 *)pAclEn)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


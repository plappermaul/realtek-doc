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

#ifndef __RTK_API_GPIO_H__
#define __RTK_API_GPIO_H__

/*
 * Data Type Declaration
 */
typedef enum rtk_gpio_mode_e
{
    GPIO_MODE_OUTPUT = 0,
    GPIO_MODE_INPUT,
    GPIO_MODE_END
} rtk_gpio_mode_t;

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
extern rtk_api_ret_t rtk_gpio_input_get(rtk_uint32 pin, rtk_uint32 *pInput);

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
extern rtk_api_ret_t rtk_gpio_output_set(rtk_uint32 pin, rtk_uint32 output);

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
extern rtk_api_ret_t rtk_gpio_output_get(rtk_uint32 pin, rtk_uint32 *pOutput);

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
extern rtk_api_ret_t rtk_gpio_state_set(rtk_uint32 pin, rtk_enable_t state);

/* Function Name:
 *      rtk_gpio_state_get
 * Description:
 *      Get GPIO enable state.
 * Input:
 *      pin      - GPIO pin
 * Output:
 *      pState  - GPIO enable
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
extern rtk_api_ret_t rtk_gpio_state_get(rtk_uint32 pin, rtk_enable_t *pState);

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
extern rtk_api_ret_t rtk_gpio_mode_set(rtk_uint32 pin, rtk_gpio_mode_t mode);

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
extern rtk_api_ret_t rtk_gpio_mode_get(rtk_uint32 pin, rtk_gpio_mode_t *pMode);

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
extern rtk_api_ret_t rtk_gpio_aclEnClear_set(rtk_uint32 pin);

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
extern rtk_api_ret_t rtk_gpio_aclEnClear_get(rtk_uint32 pin, rtk_enable_t *pAclEn);


#endif /*__RTK_API_GPIO_H__*/
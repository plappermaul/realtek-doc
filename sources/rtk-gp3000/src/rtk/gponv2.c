/*
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 * *
 * $Revision$
 * $Date: 2012-08-07
 *
 * Purpose : GPON MAC register access APIs
 *
 * Feature : Provide the APIs to access GPON MAC
 *
 */

#include <common/rt_type.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <dal/dal_mgmt.h>
#include <rtk/gponv2.h>
#ifdef CONFIG_SOC_MODE
    #ifdef CONFIG_SDK_KERNEL_LINUX
        #include <linux/delay.h>
    #endif
#endif
#include <osal/lib.h>
#include <hal/common/halctrl.h>

/* Function Name:
 *      rtk_gpon_init
 * Description:
 *      gpon register level initial function
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_init(void)
{
    int32   ret;
    rtk_port_macAbility_t mac_ability;
    uint32 chipId, ver, subtype;

    /* function body */
    if (NULL == RT_MAPPER->gpon_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_init();
    RTK_API_UNLOCK();

    if((ret = rtk_switch_version_get(&chipId, &ver, &subtype)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set switch PON port mac force 1000F up */
    osal_memset(&mac_ability, 0x00, sizeof(rtk_port_macAbility_t));
    /* 9607C chip ver A, set to 500Mbps */
    if((chipId == RTL9607C_CHIP_ID) && (ver == CHIP_REV_ID_A))
    {
        mac_ability.speed       = PORT_SPEED_500M;
    }
    else
    {
        mac_ability.speed       = PORT_SPEED_1000M;
    }
    mac_ability.duplex          = PORT_FULL_DUPLEX;
    mac_ability.linkFib1g       = DISABLED;
    mac_ability.linkStatus      = PORT_LINKDOWN;
    mac_ability.txFc            = DISABLED;
    mac_ability.rxFc            = DISABLED;
    mac_ability.nwayAbility     = DISABLED;
    mac_ability.masterMod       = DISABLED;
    mac_ability.nwayFault       = DISABLED;
    mac_ability.lpi_100m        = DISABLED;
    mac_ability.lpi_giga        = DISABLED;

    if((ret = rtk_port_macForceAbility_set(HAL_GET_PON_PORT(), mac_ability)) != RT_ERR_OK)
    {
        return ret;
    }

#if 0 /* keep pn port link down until pon clock is steady */
    mac_ability.linkStatus      = PORT_LINKUP;
    if((ret = rtk_port_macForceAbility_set(HAL_GET_PON_PORT(), mac_ability)) != RT_ERR_OK)
    {
        return ret;
    }
#endif

    if((ret = rtk_port_macForceAbilityState_set(HAL_GET_PON_PORT(), ENABLED)) != RT_ERR_OK)
    {
        return ret;
    }

    return ret;
}   /* end of rtk_gpon_init */



/* Function Name:
 *      rtk_gpon_resetState_set
 * Description:
 *      reset gpon register
 * Input:
 *    state: enable for reset gpon register
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_resetState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_resetState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_resetState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_resetState_set */


/* Function Name:
 *      rtk_gpon_resetDoneState_get
 * Description:
 *      get the reset status
 * Input:
 *
 * Output:
 *      pState: point of reset status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_resetDoneState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_resetDoneState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_resetDoneState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_resetDoneState_get */

/* Function Name:
 *      rtk_gpon_version_get
 * Description:
 *      Read the gpon version
 * Input:
 *
 * Output:
 *      pVersion: point for get gpon version
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_macVersion_get(uint32 *pVersion)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_version_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_version_get(pVersion);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_version_get */

/* Function Name:
 *      rtk_gpon_test_get
 * Description:
 *      For test get gpon test data
 * Input:
 *
 * Output:
 *      pTestData: point for get test data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_test_get(uint32 *pTestData)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_test_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_test_get(pTestData);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_test_get */

/* Function Name:
 *      rtk_gpon_test_set
 * Description:
 *      For test set gpon test data
 * Input:
 *      testData: assign test data
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_test_set(uint32 testData)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_test_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_test_set(testData);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_test_set */

/* Function Name:
 *      rtk_gpon_topIntrMask_get
 * Description:
 *      Get GPON Top level interrupt mask
 * Input:
 *    topIntrType: type of top interrupt
 * Output:
 *      pState: point of get interrupt state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_topIntrMask_get(rtk_gpon_intrType_t topIntrType,rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_topIntrMask_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_topIntrMask_get(topIntrType, pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_topIntrMask_get */


/* Function Name:
 *      rtk_gpon_topIntrMask_set
 * Description:
 *      Set GPON Top level interrupt mask
 * Input:
 *    topIntrType: type of top interrupt
 *      state: set interrupt mask state
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_topIntrMask_set(rtk_gpon_intrType_t topIntrType,rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_topIntrMask_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_topIntrMask_set(topIntrType, state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_topIntrMask_set */




/* Function Name:
 *      rtk_gpon_topIntr_get
 * Description:
 *      Set GPON Top level interrupt state
 * Input:
 *    topIntrType: type of top interrupt
 * Output:
 *      pState: point for get  interrupt state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_topIntr_get(rtk_gpon_intrType_t topIntrType,rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_topIntr_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_topIntr_get(topIntrType, pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_topIntr_get */


/* Function Name:
 *      rtk_gpon_topIntr_disableAll
 * Description:
 *      Disable all of top interrupt for GPON
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_topIntr_disableAll(void)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_topIntr_disableAll)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_topIntr_disableAll();
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_topIntr_disableAll */

/*

page 0x01 */

/* Function Name:
 *      rtk_gpon_gtcDsIntr_get
 * Description:
 *      Get GTC DS interrupt
 * Input:
 *     gtcIntrType: type of gtc interrupt type
 * Output:
 *      pState: point for get gtc interrupt  state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_gtcDsIntr_get(rtk_gpon_gtcDsIntrType_t gtcIntrType,rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gtcDsIntr_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gtcDsIntr_get(gtcIntrType, pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gtcDsIntr_get */


/* Function Name:
 *      rtk_gpon_gtcDsIntrDlt_get
 * Description:
 *      Get GTC DS interrupt indicator
 * Input:
 *     gtcIntrType: type of gtc interrupt type
 * Output:
 *      pState: point for get gtc interrupt  state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_gtcDsIntrDlt_get(rtk_gpon_gtcDsIntrType_t gtcIntrType,rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gtcDsIntrDlt_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gtcDsIntrDlt_get(gtcIntrType, pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gtcDsIntrDlt_get */


/* Function Name:
 *      rtk_gpon_gtcDsIntrMask_get
 * Description:
 *      Get GTC DS Interrupt Mask state
 * Input:
 *       gtcIntrType: type of gtc interrupt type
 * Output:
 *      pState: point for get gtc interrupt  mask state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_gtcDsIntrMask_get(rtk_gpon_gtcDsIntrType_t gtcIntrType,rtk_enable_t  *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gtcDsIntrMask_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gtcDsIntrMask_get(gtcIntrType, pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gtcDsIntrMask_get */

/* Function Name:
 *      rtk_gpon_gtcDsIntrMask_set
 * Description:
 *      Set GTC DS Interrupt Mask state
 * Input:
 *     gtcIntrType: type of gtc interrupt type
 *       state: set gtc interrupt  mask state
 * Output:
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_gtcDsIntrMask_set(rtk_gpon_gtcDsIntrType_t gtcIntrType,rtk_enable_t  state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gtcDsIntrMask_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gtcDsIntrMask_set(gtcIntrType, state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gtcDsIntrMask_set */


/* Function Name:
 *      rtk_gpon_onuId_set
 * Description:
 *      Set GPON ONU ID
 * Input:
 *      onuId: value of ONUID
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_onuId_set(uint8 onuId)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_onuId_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_onuId_set(onuId);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_onuId_set */

/* Function Name:
 *      rtk_gpon_onuId_set
 * Description:
 *      Set GPON ONU ID
 * Input:
 *      onuId: value of ONUID
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_onuId_get(uint8 *pOnuId)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_onuId_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_onuId_get(pOnuId);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_onuId_get */

/* Function Name:
 *      rtk_gpon_onuState_set
 * Description:
 *      Set ONU State .
 * Input:
 *      onuState: onu state
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_onuState_set(rtk_gpon_onuState_t  onuState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_onuState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_onuState_set(onuState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_onuState_set */

/* Function Name:
 *      rtk_gpon_onuState_get
 * Description:
 *      Get ONU State
 * Input:
 *
 * Output:
 *      pOnuState: point for get onu state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_onuState_get(rtk_gpon_onuState_t  *pOnuState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_onuState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_onuState_get(pOnuState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_onuState_get */

/* Function Name:
 *      rtk_gpon_dsBwmapCrcCheckState_set
 * Description:
 *      Set DS Bandwidth map CRC check enable
 * Input:
 *      state: enable or disable DS Bandwidth map CRC check
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED     - Failed
 * Note:
 */
int32 rtk_gpon_dsBwmapCrcCheckState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsBwmapCrcCheckState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsBwmapCrcCheckState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsBwmapCrcCheckState_set */

/* Function Name:
 *      rtk_gpon_dsBwmapCrcCheckState_get
 * Description:
 *      Get DS Bandwidth map CRC check enable
 * Input:
 *
 * Output:
 *      pState: point for get enable or disable of DS Bandwidth map CRC check
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsBwmapCrcCheckState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsBwmapCrcCheckState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsBwmapCrcCheckState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsBwmapCrcCheckState_get */

/* Added in GPON_MAC_SWIO_v1.1 */

/* Function Name:
 *      rtk_gpon_dsBwmapFilterOnuIdState_set
 * Description:
 *      Set DS bandwidth map filter oun state
 * Input:
 *      state: state of DS bandwidth map filter oun
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsBwmapFilterOnuIdState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsBwmapFilterOnuIdState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsBwmapFilterOnuIdState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsBwmapFilterOnuIdState_set */


/* Function Name:
 *      rtk_gpon_dsBwmapFilterOnuIdState_get
 * Description:
 *      Get DS bandwidth map filter oun state
 * Input:
 *
 * Output:
 *      pState: point for get DS bandwidth map filter oun state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsBwmapFilterOnuIdState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsBwmapFilterOnuIdState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsBwmapFilterOnuIdState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_gpon_dsPlendStrictMode_set
 * Description:
 *      Set DS Plen Strict Mode
 * Input:
 *      state: state of DS Plen Strict Mode
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsPlendStrictMode_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsPlendStrictMode_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsPlendStrictMode_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsPlendStrictMode_set */

/* Function Name:
 *      rtk_gpon_dsPlendStrictMode_get
 * Description:
 *      Get DS Plen Strict Mode
 * Input:
 *
 * Output:
 *      pState: point of state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsPlendStrictMode_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsPlendStrictMode_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsPlendStrictMode_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsPlendStrictMode_get */

/* Function Name:
 *      rtk_gpon_dsScrambleState_set
 * Description:
 *      Set DS scramble
 * Input:
 *      state: state of DS scramble
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsScrambleState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsScrambleState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsScrambleState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsScrambleState_set */

/* Function Name:
 *      rtk_gpon_dsScrambleState_get
 * Description:
 *      Get DS scramble
 * Input:
 *
 * Output:
 *        pState: state of DS scramble
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsScrambleState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsScrambleState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsScrambleState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsScrambleState_get */

/* Function Name:
 *      rtk_gpon_dsFecBypass_set
 * Description:
 *      Set DS FEC bypass
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsFecBypass_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsFecBypass_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsFecBypass_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsFecBypass_set */

/* Function Name:
 *      rtk_gpon_dsFecBypass_get
 * Description:
 *      Get DS FEC bypass
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsFecBypass_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsFecBypass_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsFecBypass_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsFecBypass_get */

/* Function Name:
 *      rtk_gpon_dsFecThrd_set
 * Description:
 *      Set DS Fec threshhold
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsFecThrd_set(uint8 fecThrdValue)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsFecThrd_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsFecThrd_set(fecThrdValue);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsFecThrd_set */

/* Function Name:
 *      rtk_gpon_dsFecThrd_get
 * Description:
 *      Get DS Fec threshhold
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsFecThrd_get(uint8 *pFecThrdValue)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsFecThrd_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsFecThrd_get(pFecThrdValue);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsFecThrd_get */

/* Function Name:
 *      rtk_gpon_extraSnTxTimes_set
 * Description:
 *      Set extra serial number tx times
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_extraSnTxTimes_set(uint8 exSnTxTimes)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_extraSnTxTimes_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_extraSnTxTimes_set(exSnTxTimes);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_extraSnTxTimes_set */


/* Function Name:
 *      rtk_gpon_extraSnTxTimes_get
 * Description:
 *      Get extra serial number tx times
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_extraSnTxTimes_get(uint8 *pExSnTxTimes)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_extraSnTxTimes_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_extraSnTxTimes_get(pExSnTxTimes);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_extraSnTxTimes_get */

/* Function Name:
 *      rtk_gpon_topGemUsIntr_get
 * Description:
 *      Set DS PLOAM no message
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsPloamNomsg_set(uint8 ploamNoMsgValue)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsPloamNomsg_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsPloamNomsg_set(ploamNoMsgValue);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsPloamNomsg_set */


/* Function Name:
 *      rtk_gpon_dsPloamNomsg_get
 * Description:
 *      Get DS PLOAM no message
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsPloamNomsg_get(uint8 *pPloamNoMsgValue)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsPloamNomsg_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsPloamNomsg_get(pPloamNoMsgValue);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsPloamNomsg_get */

/* Function Name:
 *      rtk_gpon_dsPloamOnuIdFilterState_set
 * Description:
 *      Set DS PLOAM ONU ID Filter state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsPloamOnuIdFilterState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsPloamOnuIdFilterState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsPloamOnuIdFilterState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsPloamOnuIdFilterState_set */

/* Function Name:
 *      rtk_gpon_dsPloamOnuIdFilterState_get
 * Description:
 *      Get DS PLOAM ONUID Filter state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsPloamOnuIdFilterState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsPloamOnuIdFilterState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsPloamOnuIdFilterState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsPloamOnuIdFilterState_get */

/* Function Name:
 *      rtk_gpon_dsPloamBcAcceptState_set
 * Description:
 *      Set DS PLAOM Boardcast accept state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsPloamBcAcceptState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsPloamBcAcceptState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsPloamBcAcceptState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsPloamBcAcceptState_set */

/* Function Name:
 *      rtk_gpon_dsPloamBcAcceptState_get
 * Description:
 *    Get DS PLAOM Boardcast accept state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsPloamBcAcceptState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsPloamBcAcceptState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsPloamBcAcceptState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsPloamBcAcceptState_get */

/* Added in GPON_MAC_SWIO_v1.1 */


/* Function Name:
 *      rtk_gpon_dsPloamDropCrcState_set
 * Description:
 *      Set DS PLOAM Drop crc state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsPloamDropCrcState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsPloamDropCrcState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsPloamDropCrcState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsPloamDropCrcState_set */

/* Function Name:
 *      rtk_gpon_dsPloamDropCrcState_get
 * Description:
 *      Get DS PLOAM Drop crc state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsPloamDropCrcState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsPloamDropCrcState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsPloamDropCrcState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsPloamDropCrcState_get */




/* Function Name:
 *      rtk_gpon_cdrLosStatus_get
 * Description:
 *      Get CDR LOS status
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_cdrLosStatus_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_cdrLosStatus_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_cdrLosStatus_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_cdrLosStatus_get */




/* Function Name:
 *      rtk_gpon_optLosStatus_get
 * Description:
 *      GET OPT LOS Status
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_optLosStatus_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_optLosStatus_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_optLosStatus_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_optLosStatus_get */


/* Function Name:
 *      rtk_gpon_topGemUsIntr_get
 * Description:
 *      Set LOS configuration
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_losCfg_set(rtk_enable_t opten, int32 optpolar, rtk_enable_t cdren, int32 cdrpolar, rtk_enable_t filter)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_losCfg_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_losCfg_set(opten, optpolar, cdren, cdrpolar, filter);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_losCfg_set */

/* Function Name:
 *      rtk_gpon_topGemUsIntr_get
 * Description:
 *      Get LOS configuration
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_losCfg_get(int32 *opten, int32 *optpolar, int32 *cdren, int32 *cdrpolar, int32 *filter)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_losCfg_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_losCfg_get(opten, optpolar, cdren, cdrpolar, filter);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_losCfg_get */

/* Function Name:
 *      rtk_gpon_dsPloam_get
 * Description:
 *      Get DS PLOAM data
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsPloam_get(uint8 *pPloamData)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsPloam_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsPloam_get(pPloamData);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsPloam_get */

/* Function Name:
 *      rtk_gpon_dsPloam_getAll
 * Description:
 *      Get all DS PLOAM data from HW and store to SW buf
 * Input:
 *
 * Output:
 *      pPloambuf - ds ploam SW buf
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsPloam_getAll(gpon_ds_ploam_buf_t *pPloambuf)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsPloam_getAll)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsPloam_getAll(pPloambuf);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsPloam_getAll */

/* Function Name:
 *      rtk_gpon_usTcont_get
 * Description:
 *      Get Tcont allocate Id
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_usTcont_get(uint32 tcontId, uint32 *pAllocateId)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_tcont_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_tcont_get(tcontId, pAllocateId);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_usTcont_get */

/* Function Name:
 *      rtk_gpon_usTcont_set
 * Description:
 *      Set TCONT allocate Id
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_usTcont_set(uint32 tcontId, uint32 allocateId)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_tcont_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_tcont_set(tcontId, allocateId);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_usTcont_set */

/* Function Name:
 *      rtk_gpon_usTcont_del
 * Description:
 *      Delete TCONT
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_usTcont_del(uint32 tcontId)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_tcont_del)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_tcont_del(tcontId);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_usTcont_del */

/* Function Name:
 *      rtk_gpon_dsGemPort_get
 * Description:
 *      Get DS gem port configure
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPort_get(uint32 idx, rtk_gem_cfg_t *pGemCfg)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPort_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPort_get(idx, pGemCfg);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPort_get */

/* Function Name:
 *      rtk_gpon_dsGemPort_set
 * Description:
 *      Set DS gem port configure
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPort_set(uint32 idx, rtk_gem_cfg_t gemCfg)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPort_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPort_set(idx, gemCfg);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPort_set */

/* Function Name:
 *      rtk_gpon_dsGemPort_del
 * Description:
 *      Delete DS Gem pot
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPort_del(uint32 idx)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPort_del)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPort_del(idx);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPort_del */

/* Function Name:
 *      rtk_gpon_dsGemPortPktCnt_get
 * Description:
 *      Get DS Gem port packet counter
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortPktCnt_get(uint32 idx, uint32 *pktCnt)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortPktCnt_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortPktCnt_get(idx, pktCnt);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortPktCnt_get */


/* Function Name:
 *      rtk_gpon_dsGemPortByteCnt_get
 * Description:
 *      Get DS Gem port byte counter
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortByteCnt_get(uint32 idx, uint32 *byteCnt)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortByteCnt_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortByteCnt_get(idx, byteCnt);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortByteCnt_get */


/* Function Name:
 *      rtk_gpon_dsGemPortByteCnt_get
 * Description:
 *      Get DS Gem port misc counter
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsGtcMiscCnt_get(rtk_gpon_dsGtc_pmMiscType_t dsGtcPmMiscType, uint32 *miscCnt)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGtcMiscCnt_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGtcMiscCnt_get(dsGtcPmMiscType, miscCnt);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGtcMiscCnt_get */


/* Function Name:
 *      rtk_gpon_dsOmciPti_set
 * Description:
 *      Set DS OMCI PTI value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsOmciPti_set(uint32 mask, uint32 end)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsOmciPti_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsOmciPti_set(mask, end);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsOmciPti_set */


/* Function Name:
 *      rtk_gpon_dsOmciPti_get
 * Description:
 *      Get DS OMCI PTI value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsOmciPti_get(uint32 *pMask, uint32 *pEnd)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsOmciPti_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsOmciPti_get(pMask, pEnd);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsOmciPti_get */



/* Function Name:
 *      rtk_gpon_dsEthPti_set
 * Description:
 *      Set DS Ethernet PTI value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsEthPti_set(uint32 mask, uint32 end)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsEthPti_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsEthPti_set(mask, end);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsEthPti_set */

/* Function Name:
 *      rtk_gpon_dsEthPti_get
 * Description:
 *      Get DS Ethernet PTI value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dsEthPti_get(uint32 *pMask, uint32 *pEnd)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsEthPti_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsEthPti_get(pMask, pEnd);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsEthPti_get */


/* Function Name:
 *      rtk_gpon_aesKeySwitch_set
 * Description:
 *      Set AES key switch value(superframe value)
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_aesKeySwitch_set(uint32 superframe)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_aesKeySwitch_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_aesKeySwitch_set(superframe);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_aesKeySwitch_set */



/* Function Name:
 *      rtk_gpon_aesKeySwitch_get
 * Description:
 *      Get AES key switch value(superframe value)
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_aesKeySwitch_get(uint32 *pSuperframe)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_aesKeySwitch_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_aesKeySwitch_get(pSuperframe);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_aesKeySwitch_get */


/* Function Name:
 *      rtk_gpon_aesKeyWord_set
 * Description:
 *      Set AES keyword value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_aesKeyWord_set(uint8 *keyword)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_aesKeyWord_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_aesKeyWord_set(keyword);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_aesKeyWord_set */


/* Function Name:
 *      rtk_gpon_aesKeyWordActive_set
 * Description:
 *      Set AES keyword value for active key, this api is only set by initial.
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_aesKeyWordActive_set(uint8 *keyword)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_aesKeyWordActive_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_aesKeyWordActive_set(keyword);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_aesKeyWord_set */


/* Function Name:
 *      rtk_gpon_irq_get
 * Description:
 *      Get global irq status
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_irq_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_irq_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_irq_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_irq_get */

/* Function Name:
 *      rtk_gpon_dsGemPortEthRxCnt_get
 * Description:
 *      Get DS Gemport Ethernet RX counter
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortEthRxCnt_get(uint32 idx,uint32 *pEthRxCnt)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortEthRxCnt_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortEthRxCnt_get(idx, pEthRxCnt);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortEthRxCnt_get */


/* Function Name:
 *      rtk_gpon_dsGemPortEthFwdCnt_get
 * Description:
 *      Get GPON DS Gemport Ethernet Forward counter
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortEthFwdCnt_get(uint32 idx,uint32 *pEthFwdCnt)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortEthFwdCnt_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortEthFwdCnt_get(idx, pEthFwdCnt);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortEthFwdCnt_get */


/* Function Name:
 *      rtk_gpon_dsGemPortMiscCnt_get
 * Description:
 *      Get GPON DS Gemport MISC counter
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortMiscCnt_get(rtk_gpon_dsGem_pmMiscType_t idx,uint32 *pMiscCnt)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortMiscCnt_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortMiscCnt_get(idx, pMiscCnt);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortMiscCnt_get */


/* Function Name:
 *      rtk_gpon_dsGemPortFcsCheckState_get
 * Description:
 *      Get GPON DS Gemport FCS check state
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortFcsCheckState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortFcsCheckState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortFcsCheckState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortFcsCheckState_get */

/* Function Name:
 *      rtk_gpon_dsGemPortFcsCheckState_set
 * Description:
 *      Get GPON DS Gemport FCS check state
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortFcsCheckState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortFcsCheckState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortFcsCheckState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortFcsCheckState_set */


/* Function Name:
 *      rtk_gpon_dsGemPortBcPassState_set
 * Description:
 *      Set GPON DS Gemport Boardcast pass state
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortBcPassState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortBcPassState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortBcPassState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortBcPassState_set */

/* Function Name:
 *      rtk_gpon_dsGemPortBcPassState_get
 * Description:
 *      Get GPON DS Gemport Boardcast pass state
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortBcPassState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortBcPassState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortBcPassState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortBcPassState_get */


/* Function Name:
 *      rtk_gpon_dsGemPortNonMcPassState_set
 * Description:
 *      Set GPON DS Gemport None Multicast pass state
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortNonMcPassState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortNonMcPassState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortNonMcPassState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortNonMcPassState_set */

/* Function Name:
 *      rtk_gpon_dsGemPortNonMcPassState_get
 * Description:
 *      Get GPON DS Gemport None Multicast pass state
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortNonMcPassState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortNonMcPassState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortNonMcPassState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortNonMcPassState_get */


/* Function Name:
 *      rtk_gpon_dsGemPortMacFilterMode_set
 * Description:
 *      Set GPON DS Gemport MAC filter mode
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortMacFilterMode_set(rtk_gpon_macTable_exclude_mode_t macFilterMode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortMacFilterMode_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortMacFilterMode_set(macFilterMode);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortMacFilterMode_set */

/* Function Name:
 *      rtk_gpon_dsGemPortMacFilterMode_get
 * Description:
 *      Get GPON DS Gemport MAC filter mode
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortMacFilterMode_get(rtk_gpon_macTable_exclude_mode_t *pMacFilterMode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortMacFilterMode_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortMacFilterMode_get(pMacFilterMode);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortMacFilterMode_get */

/* Function Name:
 *      rtk_gpon_dsGemPortMacForceMode_set
 * Description:
 *      Set GPON DS Gemport MAC force mode
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortMacForceMode_set(rtk_gpon_ipVer_t ipVer,rtk_gpon_mc_force_mode_t macForceMode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortMacForceMode_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortMacForceMode_set(ipVer, macForceMode);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortMacForceMode_set */

/* Function Name:
 *      rtk_gpon_dsGemPortMacForceMode_get
 * Description:
 *      Get GPON DS Gemport MAC force mode
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortMacForceMode_get(rtk_gpon_ipVer_t ipVer,rtk_gpon_mc_force_mode_t *pMacForceMode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortMacForceMode_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortMacForceMode_get(ipVer, pMacForceMode);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortMacForceMode_get */


/* Function Name:
 *      rtk_gpon_dsGemPortMacEntry_set
 * Description:
 *      Set GPON DS Gemport MAC filter MAC entry
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortMacEntry_set(uint32 idx, rtk_mac_t mac)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortMacEntry_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortMacEntry_set(idx, mac);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortMacEntry_set */


/* Function Name:
 *      rtk_gpon_dsGemPortMacEntry_get
 * Description:
 *     Get GPON DS Gemport MAC filter MAC entry
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortMacEntry_get(uint32 idx, rtk_mac_t *pMac)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortMacEntry_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortMacEntry_get(idx, pMac);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortMacEntry_get */


/* Function Name:
 *      rtk_gpon_dsGemPortMacEntry_del
 * Description:
 *     Delete GPON DS Gemport MAC filter MAC entry
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortMacEntry_del(uint32 idx)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortMacEntry_del)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortMacEntry_del(idx);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortMacEntry_del */



/* Function Name:
 *      rtk_gpon_dsGemPortFrameTimeOut_set
 * Description:
 *      Set GPON DS Gemport Frame Time out value
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortFrameTimeOut_set(uint32 timeOutValue)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortFrameTimeOut_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortFrameTimeOut_set(timeOutValue);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortFrameTimeOut_set */


/* Function Name:
 *      rtk_gpon_dsGemPortFrameTimeOut_get
 * Description:
 *      Get GPON DS Gemport Frame Time out value
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_dsGemPortFrameTimeOut_get(uint32 *pTimeOutValue)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsGemPortFrameTimeOut_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsGemPortFrameTimeOut_get(pTimeOutValue);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dsGemPortFrameTimeOut_get */


/* Function Name:
 *      rtk_gpon_ipv6McAddrPtn_get
 * Description:
 *      Get GPON IPv6 MC address prefix
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
*/

int32 rtk_gpon_ipv4McAddrPtn_get(uint32 *pPrefix)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_ipv4McAddrPtn_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_ipv4McAddrPtn_get(pPrefix);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_ipv4McAddrPtn_get */


/* Function Name:
 *      rtk_gpon_ipv6McAddrPtn_set
 * Description:
 *      Set GPON IPv6 MC address prefix
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
*/

int32 rtk_gpon_ipv4McAddrPtn_set(uint32 prefix)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_ipv4McAddrPtn_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_ipv4McAddrPtn_set(prefix);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_ipv4McAddrPtn_set */

/* Function Name:
 *      rtk_gpon_ipv6McAddrPtn_get
 * Description:
 *      Get GPON IPv6 MC address prefix
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
*/

int32 rtk_gpon_ipv6McAddrPtn_get(uint32 *pPrefix)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_ipv6McAddrPtn_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_ipv6McAddrPtn_get(pPrefix);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_ipv6McAddrPtn_get */


/* Function Name:
 *      rtk_gpon_ipv6McAddrPtn_set
 * Description:
 *      Set GPON IPv6 MC address prefix
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
*/

int32 rtk_gpon_ipv6McAddrPtn_set(uint32 prefix)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_ipv6McAddrPtn_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_ipv6McAddrPtn_set(prefix);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_ipv6McAddrPtn_set */


/* page 0x05 */

/* Function Name:
 *      rtk_gpon_gtcUsIntr_get
 * Description:
 *      Get US GTC interrupt state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_gtcUsIntr_get(rtk_gpon_gtcUsIntrType_t gtcUsIntrType,rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gtcUsIntr_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gtcUsIntr_get(gtcUsIntrType, pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gtcUsIntr_get */


/* Function Name:
 *      rtk_gpon_gtcUsIntrDlt_get
 * Description:
 *      Get US GTC interrupt status state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_gtcUsIntrDlt_get(rtk_gpon_gtcUsIntrType_t gtcUsIntrType,rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gtcUsIntrDlt_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gtcUsIntrDlt_get(gtcUsIntrType, pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gtcUsIntrDlt_get */


/* Function Name:
 *      rtk_gpon_gtcUsIntrMask_get
 * Description:
 *      Get US GTC Interrupt mask
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_gtcUsIntrMask_get(rtk_gpon_gtcUsIntrType_t gtcUsIntrType,rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gtcUsIntrMask_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gtcUsIntrMask_get(gtcUsIntrType, pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gtcUsIntrMask_get */

/* Function Name:
 *      rtk_gpon_gtcUsIntrMask_set
 * Description:
 *      Set US Interrupt mask
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_gpon_gtcUsIntrMask_set(rtk_gpon_gtcUsIntrType_t gtcUsIntrType,rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gtcUsIntrMask_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gtcUsIntrMask_set(gtcUsIntrType, state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gtcUsIntrMask_set */


/* Function Name:
 *      rtk_gpon_forceLaser_set
 * Description:
 *      Set Force Laser status
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_gpon_forceLaser_set(rtk_gpon_laser_status_t laserStatus)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_forceLaser_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_forceLaser_set(laserStatus);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_forceLaser_set */

/* Function Name:
 *      rtk_gpon_forceLaser_get
 * Description:
 *      Get Force Laser status
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_forceLaser_get(rtk_gpon_laser_status_t *plaserStatus)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_forceLaser_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_forceLaser_get(plaserStatus);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_forceLaser_get */

/* Function Name:
 *      rtk_gpon_forcePRBS_set
 * Description:
 *      Set force PRBS status
 * Input:
 *      prbsCfg             - PRBS config
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_forcePRBS_set(rtk_gpon_prbs_t prbsCfg)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_forcePRBS_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_forcePRBS_set(prbsCfg);
    RTK_API_UNLOCK();

    return ret;
}   /* end of rtk_gpon_forcePRBS_set */

/* Function Name:
 *      rtk_gpon_forcePRBS_get
 * Description:
 *      Get force PRBS status
 * Input:
 *      pPrbsCfg            - pointer of PRBS config
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_forcePRBS_get(rtk_gpon_prbs_t *pPrbsCfg)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_forcePRBS_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_forcePRBS_get(pPrbsCfg);
    RTK_API_UNLOCK();

    return ret;
}   /* end of rtk_gpon_forcePRBS_get */

/* Function Name:
 *      rtk_gpon_ploamState_set
 * Description:
 *      Set PLOAM state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_gpon_ploamState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_ploamState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_ploamState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_ploamState_set */

/* Function Name:
 *      rtk_gpon_ploamState_get
 * Description:
 *      Get PLOAM State
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_ploamState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_ploamState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_ploamState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_ploamState_get */

/* Function Name:
 *      rtk_gpon_indNrmPloamState_set
 * Description:
 *      Set Ind normal PLOAM state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_gpon_indNrmPloamState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_indNrmPloamState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_indNrmPloamState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_indNrmPloamState_set */

/* Function Name:
 *      rtk_gpon_indNrmPloamState_get
 * Description:
 *     Get Ind normal PLOAM state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_indNrmPloamState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_indNrmPloamState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_indNrmPloamState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_indNrmPloamState_get */

/* Function Name:
 *      rtk_gpon_dbruState_set
 * Description:
 *      Set DBRu state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_gpon_dbruState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dbruState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dbruState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dbruState_set */

/* Function Name:
 *      rtk_gpon_dbruState_get
 * Description:
 *       Get DBRu state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dbruState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dbruState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dbruState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_dbruState_get */

/* Function Name:
 *      rtk_gpon_topGemUsIntr_get
 * Description:
 *      Set US scramble state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_gpon_usScrambleState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_usScrambleState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_usScrambleState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_usScrambleState_set */

/* Function Name:
 *      rtk_gpon_topGemUsIntr_get
 * Description:
 *      Get US scramble state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_usScrambleState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_usScrambleState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_usScrambleState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_usScrambleState_get */

/* Function Name:
 *      rtk_gpon_usBurstPolarity_set
 * Description:
 *      Set US burst polarity value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_gpon_usBurstPolarity_set(rtk_gpon_polarity_t polarityValue)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_usBurstPolarity_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_usBurstPolarity_set(polarityValue);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_usBurstPolarity_set */

/* Function Name:
 *      rtk_gpon_usBurstPolarity_get
 * Description:
 *      Get US burst polarity value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_usBurstPolarity_get(rtk_gpon_polarity_t *pPolarityValue)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_usBurstPolarity_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_usBurstPolarity_get(pPolarityValue);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_usBurstPolarity_get */

/* Function Name:
 *      rtk_gpon_eqd_set
 * Description:
 *      Set Eqd value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_gpon_eqd_set(uint32 value,int32 offset)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_eqd_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_eqd_set(value, offset);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_eqd_set */

/* Function Name:
 *      rtk_gpon_eqd_get
 * Description:
 *      Get Eqd value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_gpon_eqd_get(uint32 *pValue)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_eqd_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_eqd_get(pValue);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_eqd_get */

/* Function Name:
 *      rtk_gpon_laserTime_set
 * Description:
 *      Set laserTime value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_gpon_laserTime_set(uint8 on, uint8 off)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_laserTime_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_laserTime_set(on, off);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_laserTime_set */

/* Function Name:
 *      rtk_gpon_laserTime_get
 * Description:
 *      Get laser Time value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_laserTime_get(uint8 *on, uint8 *off)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_laserTime_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_laserTime_get(on, off);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_laserTime_get */

/* Function Name:
 *      rtk_gpon_burstOverhead_set
 * Description:
 *      Set BOH value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_gpon_burstOverhead_set(uint8 rep, uint8 len, uint8 size, uint8 *oh)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_burstOverhead_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_burstOverhead_set(rep, len, size, oh);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_burstOverhead_set */

/* Function Name:
 *      rtk_gpon_usPloam_set
 * Description:
 *      Set US PLOAM
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_usPloam_set(rtk_enable_t isUrgent, uint8 *ploamValue)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_usPloam_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_usPloam_set(isUrgent, ploamValue);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_usPloam_set */

/* Function Name:
 *      rtk_gpon_usAutoPloam_set
 * Description:
 *      Set US auto ploam
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_gpon_usAutoPloam_set(rtk_gpon_usAutoPloamType_t usAutoPloamType, uint8 *ploamValue)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_usAutoPloam_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_usAutoPloam_set(usAutoPloamType, ploamValue);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_usAutoPloam_set */

/* Function Name:
 *      rtk_gpon_usPloamCrcGenState_set
 * Description:
 *      Set us PLOAM CRC  State
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_gpon_usPloamCrcGenState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_usPloamCrcGenState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_usPloamCrcGenState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_usPloamCrcGenState_set */

/* Function Name:
 *      rtk_gpon_topGemUsIntr_get
 * Description:
 *     Get us PLOAM CRC state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_usPloamCrcGenState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_usPloamCrcGenState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_usPloamCrcGenState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_usPloamCrcGenState_get */

/* Function Name:
 *      rtk_gpon_usPloamOnuIdFilterState_set
 * Description:
 *      Set US PLOAM ONUID Filter State
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_gpon_usPloamOnuIdFilterState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_usPloamOnuIdFilterState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_usPloamOnuIdFilterState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_usPloamOnuIdFilterState_set */

/* Function Name:
 *      rtk_gpon_usPloamOnuIdFilter_get
 * Description:
 *      Get US PLOAM ONUID Filter state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_usPloamOnuIdFilter_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_usPloamOnuIdFilter_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_usPloamOnuIdFilter_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_usPloamOnuIdFilter_get */

/* Function Name:
 *      rtk_gpon_usPloamBuf_flush
 * Description:
 *      Flush us PLOAM buffer
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_gpon_usPloamBuf_flush(void)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_usPloamBuf_flush)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_usPloamBuf_flush();
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_usPloamBuf_flush */



/* Function Name:
 *      rtk_gpon_usGtcMiscCnt_get
 * Description:
 *      Get US GTC Misc conuter
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_usGtcMiscCnt_get(rtk_gpon_usGtc_pmMiscType_t pmMiscType, uint32 *pValue)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_usGtcMiscCnt_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_usGtcMiscCnt_get(pmMiscType, pValue);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_usGtcMiscCnt_get */

/* Function Name:
 *      rtk_gpon_usRdi_set
 * Description:
 *      Set RDI vlaue
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_gpon_usRdi_set(int32 value)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_rdi_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_rdi_set(value);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_rdi_set */

/* Function Name:
 *      rtk_gpon_usRdi_get
 * Description:
 *      Get RDI value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_usRdi_get(int32 *value)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_rdi_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_rdi_get(value);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_rdi_get */

/* Function Name:
 *      rtk_gpon_topGemUsIntr_get
 * Description:
 *      Set US smals start proc state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_gpon_usSmalSstartProcState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_usSmalSstartProcState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_usSmalSstartProcState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_usSmalSstartProcState_set */

/* Function Name:
 *      rtk_gpon_usSmalSstartProcState_get
 * Description:
 *      Get US smals start proc state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_usSmalSstartProcState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_usSmalSstartProcState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_usSmalSstartProcState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_usSmalSstartProcState_get */

/* Function Name:
 *      rtk_gpon_usSuppressLaserState_set
 * Description:
 *      Set US supper press laser state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_gpon_usSuppressLaserState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_usSuppressLaserState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_usSuppressLaserState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_usSuppressLaserState_set */

/* Function Name:
 *      rtk_gpon_usSuppressLaserState_get
 * Description:
 *      Get US supper press laser state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_usSuppressLaserState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_usSuppressLaserState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_usSuppressLaserState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_usSuppressLaserState_get */


/* page 0x06 */
/* Function Name:
 *      rtk_gpon_gemUsIntr_get
 * Description:
 *      Get US GEM interrupt state
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_gemUsIntr_get(rtk_gpon_gemUsIntrType_t gemUsIntrType,rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gemUsIntr_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gemUsIntr_get(gemUsIntrType, pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gemUsIntr_get */


/* Function Name:
 *      rtk_gpon_gemUsIntrMask_get
 * Description:
 *      Get US GEM interrutp mask state
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_gemUsIntrMask_get(rtk_gpon_gemUsIntrType_t gemUsIntrType,rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gemUsIntrMask_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gemUsIntrMask_get(gemUsIntrType, pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gemUsIntrMask_get */

/* Function Name:
 *      rtk_gpon_gemUsIntrMask_set
 * Description:
 *      Set US GEM interrutp mask state
 * Input:
 *      value       - interrupt mask value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32 rtk_gpon_gemUsIntrMask_set(rtk_gpon_gemUsIntrType_t gemUsIntrType,rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gemUsIntrMask_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gemUsIntrMask_set(gemUsIntrType, state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gemUsIntrMask_set */



/* Function Name:
 *      rtk_gpon_gemUsForceIdleState_set
 * Description:
 *      Turn on/off force to send IDLE GEM only.
 * Input:
 *      value       - on/off value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32 rtk_gpon_gemUsForceIdleState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gemUsForceIdleState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gemUsForceIdleState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gemUsForceIdleState_set */

/* Function Name:
 *      rtk_gpon_gemUsForceIdleState_get
 * Description:
 *      Read the setting of force IDLE GEM.
 * Input:
 *      None
 * Output:
 *      pValue      - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_gemUsForceIdleState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gemUsForceIdleState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gemUsForceIdleState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gemUsForceIdleState_get */

/* Function Name:
 *      rtk_gpon_gemUsPtiVector_set
 * Description:
 *      Set the PTI value vector mapping base on (OMCI,END_FRAG).
 * Input:
 *      pti_v0       - the PTI value base on (OMCI,END_FRAG)=(0,0)
 *      pti_v1       - the PTI value  base on (OMCI,END_FRAG)=(0,1)
 *      pti_v2       - the PTI value  base on (OMCI,END_FRAG)=(1,0)
 *      pti_v3       - the PTI value  base on (OMCI,END_FRAG)=(1,1)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32 rtk_gpon_gemUsPtiVector_set(uint8 pti_v0, uint8 pti_v1, uint8 pti_v2, uint8 pti_v3)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gemUsPtiVector_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gemUsPtiVector_set(pti_v0, pti_v1, pti_v2, pti_v3);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gemUsPtiVector_set */

/* Function Name:
 *      rtk_gpon_gemUsPtiVector_get
 * Description:
 *      Read the setting of force IDLE GEM.
 * Input:
 *      None
 * Output:
 *      pPti_v0             - the PTI vector 0
 *      pPti_v1             - the PTI vector 1
 *      pPti_v2             - the PTI vector 2
 *      pPti_v3             - the PTI vector 3
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_gemUsPtiVector_get(uint8 *pPti_v0, uint8 *pPti_v1, uint8 *pPti_v2, uint8 *pPti_v3)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gemUsPtiVector_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gemUsPtiVector_get(pPti_v0, pPti_v1, pPti_v2, pPti_v3);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gemUsPtiVector_get */


/* Function Name:
 *      rtk_gpon_gemUsEthCnt_get
 * Description:
 *      Read the ether counter for local port.
 * Input:
 *      local_idx           - local port index
 * Output:
 *      pEth_cntr           - counter value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 */
int32 rtk_gpon_gemUsEthCnt_get(uint32 idx, uint32 *pEthCntr)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gemUsEthCnt_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gemUsEthCnt_get(idx, pEthCntr);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gemUsEthCnt_get */

/* Function Name:
 *      rtk_gpon_gemUsGemCnt_read
 * Description:
 *      Read the ether counter for local port.
 * Input:
 *      local_idx           - local port index
 * Output:
 *      pGem_cntr           - counter value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 */
int32 rtk_gpon_gemUsGemCnt_get(uint32 idx, uint32 *pGemCntr)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gemUsGemCnt_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gemUsGemCnt_get(idx, pGemCntr);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gemUsGemCnt_get */

/* Function Name:
 *      rtk_gpon_gemUsPortCfg_set
 * Description:
 *      Set the mapping of local_idx and gem_port_id.
 * Input:
 *      cycle       - specify the cycle value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_gemUsPortCfg_set(uint32 idx, uint32 gemPortId)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gemUsPortCfg_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gemUsPortCfg_set(idx, gemPortId);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gemUsPortCfg_set */

/* Function Name:
 *      rtk_gpon_gemUsPortCfg_get
 * Description:
 *      Read the mapping of local_idx and gem_port_id.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 */
int32 rtk_gpon_gemUsPortCfg_get(uint32 idx, uint32 *pGemPortId)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gemUsPortCfg_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gemUsPortCfg_get(idx, pGemPortId);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gemUsPortCfg_get */

/* Function Name:
 *      rtk_gpon_gemUsDataByteCnt_get
 * Description:
 *      Read the data byte counter for local port.
 * Input:
 *      local_idx           - local port index
 * Output:
 *      pByte_cntr          - counter value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 */
int32 rtk_gpon_gemUsDataByteCnt_get(uint32 idx, uint64 *pByteCntr)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gemUsDataByteCnt_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gemUsDataByteCnt_get(idx, pByteCntr);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gemUsDataByteCnt_get */

/* Function Name:
 *      rtk_gpon_gemUsIdleByteCnt_read
 * Description:
 *      Read the IDLE byte counter for TCONT.
 * Input:
 *      local_idx           - local TCONT index
 * Output:
 *      pIdle_cntr          - counter value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 */
int32 rtk_gpon_gemUsIdleByteCnt_get(uint32 idx, uint64 *pIdleCntr)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gemUsIdleByteCnt_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gemUsIdleByteCnt_get(idx, pIdleCntr);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gemUsIdleByteCnt_get */


/* Function Name:
 *      rtk_gpon_dbruPeriod_set
 * Description:
 *      Read the data byte counter for local port.
 * Input:
 *      periodTime           - dbru period time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 */
int32 rtk_gpon_dbruPeriod_set(uint32 periodTime)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dbruPeriod_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dbruPeriod_set(periodTime);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_gpon_dbruPeriod_get
 * Description:
 *      Read the data byte counter for local port.
 * Input:
 *      None
 * Output:
 *      *pPeriodTime        - point of period time
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 */
int32 rtk_gpon_dbruPeriod_get(uint32 *pPeriodTime)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dbruPeriod_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dbruPeriod_get(pPeriodTime);
    RTK_API_UNLOCK();
    return ret;
}


/* Function Name:
 *      rtk_gpon_portMacForceMode_set
 * Description:
 *      Set MAC focre mode for PON port
 * Input:
 *       None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_portMacForceMode_set(rtk_port_linkStatus_t linkStatus)
{
    int32 ret;

    /* use pon port accept packet length to implement port link up/down */

    if(PORT_LINKUP == linkStatus)
    {
        if((ret = rtk_ponmac_maxPktLen_set(2031)) != RT_ERR_OK)
        {
            return ret;
        }
    }
    else
    {
        if((ret = rtk_ponmac_maxPktLen_set(0)) != RT_ERR_OK)
        {
            return ret;
        }
    }

#if 0
    rtk_switch_devInfo_t devInfo;

    if((ret = rtk_switch_deviceInfo_get(&devInfo))!=RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

    if(APOLLOMP_CHIP_ID == devInfo.chipId)
    { /* Now we use port-isolation for the purpose: if not O5, LAN/CPU can't send packet to pon port. */

        rtk_port_t port;
        rtk_port_isoConfig_t mode;
        rtk_portmask_t portmask;
        rtk_portmask_t extPortmask;

        HAL_SCAN_ALL_PORT(port)
        {
            if(HAL_IS_PON_PORT(port))
                continue;

            for(mode=RTK_PORT_ISO_CFG_0; mode<RTK_PORT_ISO_CFG_END; mode++)
            {
                if((ret = rtk_port_isolationEntry_get(mode, port, &portmask, &extPortmask)) != RT_ERR_OK)
                {
                    return ret;
                }

                if(PORT_LINKDOWN == linkStatus)
                {
                    RTK_PORTMASK_PORT_CLEAR(portmask,HAL_GET_PON_PORT());
                }
                else
                {
                    RTK_PORTMASK_PORT_SET(portmask,HAL_GET_PON_PORT());
                }

                if((ret = rtk_port_isolationEntry_set(mode, port, &portmask, &extPortmask)) != RT_ERR_OK)
                {
                    return ret;
                }
            }
        }
    }
    else
    {/* 9601B use port force link */
        rtk_port_macAbility_t mac_ability;

        /*set mac force */
        osal_memset(&mac_ability, 0x00, sizeof(rtk_port_macAbility_t));
        mac_ability.speed           = PORT_SPEED_1000M;
        mac_ability.duplex          = PORT_FULL_DUPLEX;
        mac_ability.linkFib1g       = DISABLED;
        mac_ability.linkStatus      = PORT_LINKDOWN;
        mac_ability.txFc              = DISABLED;
        mac_ability.rxFc            = DISABLED;
        mac_ability.nwayAbility     = DISABLED;
        mac_ability.masterMod       = DISABLED;
        mac_ability.nwayFault       = DISABLED;
        mac_ability.lpi_100m        = DISABLED;
        mac_ability.lpi_giga        = DISABLED;

    #ifdef CONFIG_SOC_MODE
        #ifdef CONFIG_SDK_KERNEL_LINUX
            /*add for avoid traffic will still in queue but link down will make packet stuck issue*/
            mdelay(1);
        #endif
    #endif

        if((ret = rtk_port_macForceAbility_set(HAL_GET_PON_PORT(), mac_ability)) != RT_ERR_OK)
        {
            return ret;
        }
        mac_ability.linkStatus      = linkStatus;
        if((ret = rtk_port_macForceAbility_set(HAL_GET_PON_PORT(), mac_ability)) != RT_ERR_OK)
        {
            return ret;
        }

        if((ret = rtk_port_macForceAbilityState_set(HAL_GET_PON_PORT(), ENABLED)) != RT_ERR_OK)
        {
            return ret;
        }
    }

#endif
    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_gpon_port_get
 * Description:
 *      Get GPON port id
 * Input:
 *       None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_port_get(rtk_port_t *ponPort)
{
    *ponPort = HAL_GET_PON_PORT();

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gpon_gtcDsIntrDlt_check
 * Description:
 *      Check GTC DS interrupt
 * Input:
 *     gtcIntrType: type of gtc interrupt type
 * Output:
 *      pState: point for get gtc interrupt  state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_gtcDsIntrDlt_check(rtk_gpon_gtcDsIntrType_t gtcDsIntrType,uint32 gtcDsIntrDltValue,rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gtcDsIntrDlt_check)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gtcDsIntrDlt_check(gtcDsIntrType, gtcDsIntrDltValue, pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gtcDsIntrDlt_check */


/* Function Name:
 *      rtk_gpon_gtcUsIntrDlt_check
 * Description:
 *      Check GTC US interrupt
 * Input:
 *     gtcIntrType: type of gtc interrupt type
 * Output:
 *      pState: point for get gtc interrupt  state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_gtcUsIntrDlt_check(rtk_gpon_gtcUsIntrType_t gtcUsIntrType,uint32 gtcUsIntrDltValue,rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gtcUsIntrDlt_check)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gtcUsIntrDlt_check(gtcUsIntrType, gtcUsIntrDltValue, pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gtcUsIntrDlt_check */


/* Function Name:
 *      rtk_gpon_gemUsIntrDlt_check
 * Description:
 *      Check GTC DS interrupt
 * Input:
 *     gtcIntrType: type of gtc interrupt type
 * Output:
 *      pState: point for get gtc interrupt  state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_gemUsIntrDlt_check(rtk_gpon_gemUsIntrType_t gemIntrType,uint32 gemUsIntrDltValue,rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_gemUsIntrDlt_check)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_gemUsIntrDlt_check(gemIntrType, gemUsIntrDltValue, pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_gemUsIntrDlt_check */


/* Function Name:
 *      rtk_gpon_usRogueOnt_set
 * Description:
 *      Config Rogue ONT
 * Input:
 *     rogOntState - enable or disable rogue ont
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_usRogueOnt_set(rtk_enable_t rogOntState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_rogueOnt_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_rogueOnt_set(rogOntState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_rogueOnt_set */


/* Function Name:
 *      rtk_gpon_drainOutDefaultQueue_set
 * Description:
 *      Drain Out GPON default Queue
 * Input:
 *     rogOntState - enable or disable rogue ont
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_drainOutDefaultQueue_set(void)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_drainOutDefaultQueue_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_drainOutDefaultQueue_set();
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_drainOutDefaultQueue_set */

/* Function Name:
 *      rtk_gpon_autoDisTx_set
 * Description:
 *      Enable or Disable auto dis_tx function for power saveing mode
 * Input:
 *     autoDisTxState - enable or disable auto dis_tx
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_autoDisTx_set(rtk_enable_t autoDisTxState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_autoDisTx_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_autoDisTx_set(autoDisTxState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_autoDisTx_set */

/* Function Name:
 *      rtk_gpon_scheInfo_get
 * Description:
 *      get GPON scheduler information.
 * Input:
 *      None
 * Output:
 *      pScheInfo   - pointer of pScheInfo information
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_scheInfo_get(rtk_gpon_schedule_info_t *pScheInfo)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_scheInfo_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_scheInfo_get(pScheInfo);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_gpon_dataPath_reset
 * Description:
 *      reset GPON data path.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_gpon_dataPath_reset(void)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dataPath_reset)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dataPath_reset();
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_gpon_dsOmciCnt_get
 * Description:
 *      Get GPON DS OMCI counter
 * Input:
 *      none
 * Output:
 *      usOmciCnt           - return value of OMCI counter
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_dsOmciCnt_get(rtk_gpon_ds_omci_t *dsOmciCnt)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dsOmciCnt_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dsOmciCnt_get(dsOmciCnt);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_gpon_usOmciCnt_get
 * Description:
 *      Get GPON US OMCI counter
 * Input:
 *      none
 * Output:
 *      usOmciCnt           - return value of OMCI counter
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_usOmciCnt_get(rtk_gpon_us_omci_t *usOmciCnt)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_usOmciCnt_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_usOmciCnt_get(usOmciCnt);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_gpon_usDbruBlockSize_get
 * Description:
 *      Get GPON DBRu block size
 * Input:
 *      none
 * Output:
 *      blockSize           - return value of block size
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_usDbruBlockSize_get(uint32 *pBlockSize)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dbruBlockSize_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dbruBlockSize_get(pBlockSize);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_gpon_usDbruBlockSize_set
 * Description:
 *      Set GPON DBRu block size
 * Input:
 *      blockSize           - config value of block size
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_usDbruBlockSize_set(uint32 blockSize)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_dbruBlockSize_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_dbruBlockSize_set(blockSize);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_gpon_flowctrl_adjust_byFlowNum
 * Description:
 *      Adjust pbo flowcontrol threshold by flow number
 * Input:
 *      flowNum     - flow number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_gpon_flowctrl_adjust_byFlowNum(uint32 flowNum)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_flowctrl_adjust_byFlowNum)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_flowctrl_adjust_byFlowNum(flowNum);
    RTK_API_UNLOCK();

    return ret;

}

/* Function Name:
 *      rtk_gpon_usLaserDefault_get
 * Description:
 *      get the default config of us laser on/off offset
 * Input:
 *      None
 * Output:
 *      pLaserOn
 *      pLaserOff
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_DRIVER_NOT_FOUND                 - no driver callback.
 * Note:
 *      None
 */
int32
rtk_gpon_usLaserDefault_get(uint8 *pLaserOn, uint8 *pLaserOff)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_usLaserDefault_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_usLaserDefault_get(pLaserOn, pLaserOff);
    RTK_API_UNLOCK();

    return ret;
}

/* Function Name:
 *      rtk_gpon_smallBwTcont_get
 * Description:
 *      get the bitmask of TCONT which is small bwmap.
 * Input:
 *      smallBw
 * Output:
 *      pTcontBitMask
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_DRIVER_NOT_FOUND                 - no driver callback.
 * Note:
 *      None
 */
int32
rtk_gpon_smallBwTcont_get(uint32 smallBw, uint32 *pTcontBitMask)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_smallBwTcont_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_smallBwTcont_get(smallBw, pTcontBitMask);
    RTK_API_UNLOCK();

    return ret;
}

/* Function Name:
 *      rtk_gpon_autoDG_set
 * Description:
 *      Set GPON auto dying gasp
 * Input:
 *      state           - config value of auto dying gasp
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_autoDG_set(uint32 state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_autoDG_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_autoDG_set(state);
    RTK_API_UNLOCK();

    return ret;
}   /* end of rtk_gpon_autoDG_set */

/* Function Name:
 *      rtk_gpon_autoDG_get
 * Description:
 *      Get GPON auto dying gasp
 * Input:
 *      pState           - pointer of config value of auto dying gasp
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_autoDG_get(uint32 *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_autoDG_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_autoDG_get(pState);
    RTK_API_UNLOCK();

    return ret;
}   /* end of rtk_gpon_autoDG_get */

/* Function Name:
 *      rtk_gpon_DgCnt_get
 * Description:
 *      Get GPON dying gasp counter
 * Input:
 *      pCnt           - pointer of GPON dying gasp counter
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_DgCnt_get(uint32 *pCnt)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_DgCnt_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_DgCnt_get(pCnt);
    RTK_API_UNLOCK();

    return ret;
}   /* end of rtk_gpon_DgCnt_get */



/* Function Name:
 *      rtk_gpon_DgThreshold_set
 * Description:
 *      Set GPON dying gasp threshold
 * Input:
 *      threshold           - config value of dying gasp threshold
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_DgThreshold_set(uint32 threshold)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_DgThreshold_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_DgThreshold_set(threshold);
    RTK_API_UNLOCK();

    return ret;
}   /* end of rtk_gpon_DgThreshold_set */


/* Function Name:
 *      rtk_gpon_DgThreshold_get
 * Description:
 *      Get GPON dying gasp threshold
 * Input:
 *      pThreshold          - pointer of config value of auto dying threshold
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 rtk_gpon_DgThreshold_get(uint32 *pThreshold)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_DgThreshold_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_DgThreshold_get(pThreshold);
    RTK_API_UNLOCK();

    return ret;
}   /* end of rtk_gpon_DgThreshold_get */


#ifdef CONFIG_LUNA_G3_SERIES
/* Function Name:
 *      rtk_gpon_password_set
 * Description:
 *      GPON MAC set Password.
 * Input:
 *      pPwd             - the pointer of Password
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
int32 rtk_gpon_password_set(rtk_gpon_password_t *pPwd)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpon_password_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpon_password_set(pPwd);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpon_password_set */
#endif

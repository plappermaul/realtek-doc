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
 * $Revision: 43348 $
 * $Date: 2013-10-02 11:32:20 +0800 (星期三, 02 十月 2013) $
 *
 * Purpose : Definition of Security API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) attack prevention
 */


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>

#include <dal/rtl9601b/dal_rtl9601b.h>
#include <rtk/led.h>
#include <dal/rtl9601b/dal_rtl9601b_led.h>
/*
 * Symbol Definition
 */

#define RTL9601B_PARALLEL_LED_MAX  4



typedef enum rtl9601b_led_operation_e
{
    RTL9601B_LED_OP_PARALLEL = 0,
    RTL9601B_LED_OP_SERIAL   = 1,
    RTL9601B_LED_OP_END,
}rtl9601b_led_operation_t;



typedef enum rtl9601b_led_active_e
{
    RTL9601B_LED_ACTIVE_HIGH = 0,
    RTL9601B_LED_ACTIVE_LOW  = 1,
    RTL9601B_LED_ACTIVE_END,
}rtl9601b_led_active_t;



typedef enum rtl9601b_led_enable_e
{
    RTL9601B_LED_DISABLED = 0,
    RTL9601B_LED_ENABLED  = 1,
    RTL9601B_LED_ENABLE_END,
}rtl9601b_led_enable_t;



typedef enum rtl9601b_led_blink_rate_e
{
    RTL9601B_LED_BLINKRATE_32MS  = 0,
    RTL9601B_LED_BLINKRATE_64MS  = 1,
    RTL9601B_LED_BLINKRATE_128MS = 2,
    RTL9601B_LED_BLINKRATE_256MS = 3,
    RTL9601B_LED_BLINKRATE_512MS = 4,
    RTL9601B_LED_BLINKRATE_1024MS= 5,
    RTL9601B_LED_BLINKRATE_48MS  = 6,
    RTL9601B_LED_BLINKRATE_96MS  = 7,
    RTL9601B_LED_BLINKRATE_END,
}rtl9601b_led_blink_rate_t;


/* led type */
typedef enum rtl9601b_led_type_e
{
    RTL9601B_LED_TYPE_FIBER0 = 0x00,
    RTL9601B_LED_TYPE_UTP0   = 0x01,
    RTL9601B_LED_TYPE_FIBER1 = 0x02,
    RTL9601B_LED_TYPE_PON    = 0x03,
    RTL9601B_LED_TYPE_END
} rtl9601b_led_type_t;



typedef enum rtl9601b_led_force_mode_e
{
    RTL9601B_LED_FORCE_OFF   = 0,
    RTL9601B_LED_FORCE_ON    = 1,
    RTL9601B_LED_FORCE_BLINK = 2,
    RTL9601B_LED_FORCE_END
}rtl9601b_led_force_mode_t;


/*
 * Data Declaration
 */
static uint32    led_init = {INIT_COMPLETED};



/*
 * Function Declaration
 */

/* Function Name:
 *      dal_rtl9601b_led_init
 * Description:
 *      Initialize led module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize led module before calling any led APIs.
 */
int32
dal_rtl9601b_led_init(void)
{
    int     ledIdx;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_LED),"%s",__FUNCTION__);

    led_init = INIT_COMPLETED;

    /*disable all parallel LED*/
    for(ledIdx = 0; ledIdx < RTL9601B_PARALLEL_LED_MAX ; ledIdx++)
    {
        if((ret = dal_rtl9601b_led_parallelEnable_set(ledIdx, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_LED | MOD_DAL), "");
            led_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_led_init */

/* Function Name:
 *      rtk_led_operation_set
 * Description:
 *      Get Led operation mode
 * Input:
 *      None
 * Output:
 *      pMode - LED operation mode.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can set Led operation mode.
 *      The modes that can be set are as following:
 *      - LED_OP_PARALLEL 17 led
 *      - LED_OP_SERIAL   32 led
 */
int32
dal_rtl9601b_led_operation_get(rtk_led_operation_t *pMode)
{

    *pMode = LED_OP_PARALLEL;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_led_operation_get */


/* Function Name:
 *      dal_rtl9601b_led_operation_set
 * Description:
 *      Set Led operation mode
 * Input:
 *      mode - LED operation mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can set Led operation mode.
 *      The modes that can be set are as following:
 *      - LED_OP_PARALLEL 4 led
 *      - LED_OP_SERIAL   not support
 */
int32
dal_rtl9601b_led_operation_set(rtk_led_operation_t mode)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_LED),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(led_init);

    /* parameter check */
    switch(mode)
    {
        case LED_OP_PARALLEL:
            ret = RT_ERR_OK;
            break;
        case LED_OP_SERIAL:
        default:
            return RT_ERR_INPUT;
            break;
    }

    return ret;
} /* end of dal_rtl9601b_led_operation_set */


/* Function Name:
 *      dal_rtl9601b_led_blinkRate_get
 * Description:
 *      Get LED blinking rate
 * Input:
 *      group      - led blinking group
 * Output:
 *      pBlinkRate - blinking rate.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      There are 8 types of LED blinking rates at 32ms, 48ms, 64ms, 96ms, 128ms, 256ms, 512ms, and 1024ms.
 */
int32
dal_rtl9601b_led_blinkRate_get(rtk_led_blinkGroup_t group, rtk_led_blink_rate_t *pBlinkRate)
{
    int32   ret;
    uint32  data;
    uint32  field;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_LED),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(led_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBlinkRate), RT_ERR_NULL_POINTER);

    switch(group)
    {
        case LED_BLINK_GROUP_PORT:
            field = RTL9601B_SEL_MAC_LED_RATEf;
            break;
        case LED_BLINK_GROUP_FORCE_MODE:
            field = RTL9601B_SEL_LED_FORCE_RATEf;
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    if ((ret = reg_field_read(RTL9601B_LED_BLINK_RATE_CFGr, field, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }
    switch(data)
    {
        case RTL9601B_LED_BLINKRATE_32MS:
            *pBlinkRate = LED_BLINKRATE_32MS;
            break;
        case RTL9601B_LED_BLINKRATE_64MS:
            *pBlinkRate = LED_BLINKRATE_64MS;
            break;
        case RTL9601B_LED_BLINKRATE_128MS:
            *pBlinkRate = LED_BLINKRATE_128MS;
            break;
        case RTL9601B_LED_BLINKRATE_256MS:
            *pBlinkRate = LED_BLINKRATE_256MS;
            break;
        case RTL9601B_LED_BLINKRATE_512MS:
            *pBlinkRate = LED_BLINKRATE_512MS;
            break;
        case RTL9601B_LED_BLINKRATE_1024MS:
            *pBlinkRate = LED_BLINKRATE_1024MS;
            break;
        case RTL9601B_LED_BLINKRATE_48MS:
            *pBlinkRate = LED_BLINKRATE_48MS;
            break;
        case RTL9601B_LED_BLINKRATE_96MS:
            *pBlinkRate = LED_BLINKRATE_96MS;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_led_blinkRate_get */

/* Function Name:
 *      dal_rtl9601b_led_blinkRate_set
 * Description:
 *      Get LED blinking rate
 * Input:
 *      group      - led blinking group
 *      blinkRate  - blinking rate.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      There are 8 types of LED blinking rates at 32ms, 48ms, 64ms, 96ms, 128ms, 256ms, 512ms, and 1024ms.
 */
int32
dal_rtl9601b_led_blinkRate_set(rtk_led_blinkGroup_t group, rtk_led_blink_rate_t blinkRate)
{
    int32   ret;
    uint32  data;
    uint32  field;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_LED),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(led_init);

    /* parameter check */
    switch(group)
    {
        case LED_BLINK_GROUP_PORT:
            field = RTL9601B_SEL_MAC_LED_RATEf;
            break;
        case LED_BLINK_GROUP_FORCE_MODE:
            field = RTL9601B_SEL_LED_FORCE_RATEf;
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    switch(blinkRate)
    {
        case LED_BLINKRATE_32MS:
            data = RTL9601B_LED_BLINKRATE_32MS;
            break;
        case LED_BLINKRATE_64MS:
            data = RTL9601B_LED_BLINKRATE_64MS;
            break;
        case LED_BLINKRATE_128MS:
            data = RTL9601B_LED_BLINKRATE_128MS;
            break;
        case LED_BLINKRATE_256MS:
            data = RTL9601B_LED_BLINKRATE_256MS;
            break;
        case LED_BLINKRATE_512MS:
            data = RTL9601B_LED_BLINKRATE_512MS;
            break;
        case LED_BLINKRATE_1024MS:
            data = RTL9601B_LED_BLINKRATE_1024MS;
            break;
        case LED_BLINKRATE_48MS:
            data = RTL9601B_LED_BLINKRATE_48MS;
            break;
        case LED_BLINKRATE_96MS:
            data = RTL9601B_LED_BLINKRATE_96MS;
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    if ((ret = reg_field_write(RTL9601B_LED_BLINK_RATE_CFGr, field, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_led_blinkRate_set */


static int32 _dal_rtl9601b_led_modeEnable_set(uint32 ledIdx, uint32 field, rtk_enable_t enable)
{
    int32   ret;
    uint32  data;

    if(ENABLED == enable)
        data = 1;
    else
        data = 0;

    if ((ret = reg_array_field_write(RTL9601B_DATA_LED_CFGr, REG_ARRAY_INDEX_NONE, ledIdx, field, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}


static int32 _dal_rtl9601b_led_modeEnable_get(uint32 ledIdx, uint32 field, rtk_enable_t *enable)
{
    int32   ret;
    uint32  data;

    if ((ret = reg_array_field_read(RTL9601B_DATA_LED_CFGr, REG_ARRAY_INDEX_NONE, ledIdx, field, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }
    if( RTL9601B_LED_DISABLED == data)
        *enable = DISABLED;
    else
        *enable = ENABLED;

    return RT_ERR_OK;
}


static int32 _dal_rtl9601b_led_ponSocConfig_set(uint32 ledIdx, rtk_led_type_t type, rtk_led_config_t *pConfig)
{
    int32   ret;
    uint32  field;

    switch(type)
    {
        case LED_TYPE_PON:
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    /*set force mode*/
    field = RTL9601B_CPU_FORCE_MODf;
    if ((ret = _dal_rtl9601b_led_modeEnable_set(ledIdx,field, pConfig->ledEnable[LED_CONFIG_FORCE_MODE])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }
    /*set PON link mode*/
    if(LED_TYPE_PON == type)
    {
        field = RTL9601B_SPD1000f;
        if ((ret = _dal_rtl9601b_led_modeEnable_set(ledIdx,field, pConfig->ledEnable[LED_CONFIG_PON_LINK])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_LED|MOD_DAL), "");
            return ret;
        }

        field = RTL9601B_TX_ACTf;
        if ((ret = _dal_rtl9601b_led_modeEnable_set(ledIdx,field, pConfig->ledEnable[LED_CONFIG_TX_ACT])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_LED|MOD_DAL), "");
            return ret;
        }

        field = RTL9601B_RX_ACTf;
        if ((ret = _dal_rtl9601b_led_modeEnable_set(ledIdx,field, pConfig->ledEnable[LED_CONFIG_RX_ACT])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_LED|MOD_DAL), "");
            return ret;
        }

        field = RTL9601B_SPD100_ACTf;
        if ((ret = _dal_rtl9601b_led_modeEnable_set(ledIdx,field, pConfig->ledEnable[LED_CONFIG_PON_ALARM])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_LED|MOD_DAL), "");
            return ret;
        }

        field = RTL9601B_SPD10_ACTf;
        if ((ret = _dal_rtl9601b_led_modeEnable_set(ledIdx,field, pConfig->ledEnable[LED_CONFIG_PON_WARNING])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_LED|MOD_DAL), "");
            return ret;
        }

    }

    return RT_ERR_OK;
}


static int32 _dal_rtl9601b_led_ponSocConfig_get(uint32 ledIdx, rtk_led_type_t type, rtk_led_config_t *pConfig)
{
    int32   ret;
    uint32  field;

    switch(type)
    {
        case LED_TYPE_PON:
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    memset(pConfig,0x0,sizeof(rtk_led_config_t));

    /*set force mode*/
    field = RTL9601B_CPU_FORCE_MODf;
    if ((ret = _dal_rtl9601b_led_modeEnable_get(ledIdx,field, &pConfig->ledEnable[LED_CONFIG_FORCE_MODE])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }
    /*set PON link mode*/
    if(LED_TYPE_PON == type)
    {
        field = RTL9601B_SPD1000f;
        if ((ret = _dal_rtl9601b_led_modeEnable_get(ledIdx,field, &pConfig->ledEnable[LED_CONFIG_PON_LINK])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_LED|MOD_DAL), "");
            return ret;
        }

        field = RTL9601B_TX_ACTf;
        if ((ret = _dal_rtl9601b_led_modeEnable_get(ledIdx,field, &pConfig->ledEnable[LED_CONFIG_TX_ACT])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_LED|MOD_DAL), "");
            return ret;
        }

        field = RTL9601B_RX_ACTf;
        if ((ret = _dal_rtl9601b_led_modeEnable_get(ledIdx,field, &pConfig->ledEnable[LED_CONFIG_RX_ACT])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_LED|MOD_DAL), "");
            return ret;
        }

        field = RTL9601B_SPD100_ACTf;
        if ((ret = _dal_rtl9601b_led_modeEnable_get(ledIdx,field, &pConfig->ledEnable[LED_CONFIG_PON_ALARM])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_LED|MOD_DAL), "");
            return ret;
        }

        field = RTL9601B_SPD10_ACTf;
        if ((ret = _dal_rtl9601b_led_modeEnable_get(ledIdx,field, &pConfig->ledEnable[LED_CONFIG_PON_WARNING])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_LED|MOD_DAL), "");
            return ret;
        }


    }

    return RT_ERR_OK;
}



static int32 _dal_rtl9601b_led_portConfig_set(uint32 ledIdx, rtk_led_type_t type, rtk_led_config_t *pConfig)
{
    int32   ret;
    uint32  field;

    switch(type)
    {
        case LED_TYPE_UTP0:
        case LED_TYPE_FIBER0:
        case LED_TYPE_FIBER1:
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    /*error check*/
    if(ENABLED == pConfig->ledEnable[LED_CONFIG_PON_LINK])
        return RT_ERR_INPUT;
    if(ENABLED == pConfig->ledEnable[LED_CONFIG_SOC_LINK_ACK])
        return RT_ERR_INPUT;

    if( (LED_TYPE_FIBER0 == type) || (LED_TYPE_FIBER1 == type) )
    {
        if(ENABLED == pConfig->ledEnable[LED_CONFIG_SPD10ACT])
            return RT_ERR_INPUT;
        if(ENABLED == pConfig->ledEnable[LED_CONFIG_SPD10])
            return RT_ERR_INPUT;
        if(ENABLED == pConfig->ledEnable[LED_CONFIG_COL])
            return RT_ERR_INPUT;
        if(ENABLED == pConfig->ledEnable[LED_CONFIG_DUP])
            return RT_ERR_INPUT;
    }


    /*set force mode*/
    field = RTL9601B_CPU_FORCE_MODf;
    if ((ret = _dal_rtl9601b_led_modeEnable_set(ledIdx, field, pConfig->ledEnable[LED_CONFIG_FORCE_MODE])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set speed 1000 mode*/
    field = RTL9601B_SPD1000f;
    if ((ret = _dal_rtl9601b_led_modeEnable_set(ledIdx, field, pConfig->ledEnable[LED_CONFIG_SPD1000])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set speed 500 mode*/
    field = RTL9601B_SPD500f;
    if ((ret = _dal_rtl9601b_led_modeEnable_set(ledIdx, field, pConfig->ledEnable[LED_CONFIG_SPD500])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }


    /*set speed 100 mode*/
    field = RTL9601B_SPD100f;
    if ((ret = _dal_rtl9601b_led_modeEnable_set(ledIdx, field, pConfig->ledEnable[LED_CONFIG_SPD100])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set speed 10 mode*/
    field = RTL9601B_SPD10f;
    if ((ret = _dal_rtl9601b_led_modeEnable_set(ledIdx, field, pConfig->ledEnable[LED_CONFIG_SPD10])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set duplex mode*/
    field = RTL9601B_DUPf;
    if ((ret = _dal_rtl9601b_led_modeEnable_set(ledIdx, field, pConfig->ledEnable[LED_CONFIG_DUP])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set speed 1000 Act mode*/
    field = RTL9601B_SPD1000_ACTf;
    if ((ret = _dal_rtl9601b_led_modeEnable_set(ledIdx, field, pConfig->ledEnable[LED_CONFIG_SPD1000ACT])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set speed 500 Act mode*/
    field = RTL9601B_SPD500_ACTf;
    if ((ret = _dal_rtl9601b_led_modeEnable_set(ledIdx, field, pConfig->ledEnable[LED_CONFIG_SPD500ACT])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }
    /*set speed 100 Act mode*/
    field = RTL9601B_SPD100_ACTf;
    if ((ret = _dal_rtl9601b_led_modeEnable_set(ledIdx, field, pConfig->ledEnable[LED_CONFIG_SPD100ACT])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set speed 10 Act mode*/
    field = RTL9601B_SPD10_ACTf;
    if ((ret = _dal_rtl9601b_led_modeEnable_set(ledIdx, field, pConfig->ledEnable[LED_CONFIG_SPD10ACT])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set Rx Act mode*/
    field = RTL9601B_RX_ACTf;
    if ((ret = _dal_rtl9601b_led_modeEnable_set(ledIdx, field, pConfig->ledEnable[LED_CONFIG_RX_ACT])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set Tx Act mode*/
    field = RTL9601B_TX_ACTf;
    if ((ret = _dal_rtl9601b_led_modeEnable_set(ledIdx, field, pConfig->ledEnable[LED_CONFIG_TX_ACT])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set Collision mode*/
    field = RTL9601B_COLf;
    if ((ret = _dal_rtl9601b_led_modeEnable_set(ledIdx, field, pConfig->ledEnable[LED_CONFIG_COL])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}




static int32 _dal_rtl9601b_led_portConfig_get(uint32 ledIdx, rtk_led_type_t type, rtk_led_config_t *pConfig)
{
    int32   ret;
    uint32  field;

    memset(pConfig,0x0,sizeof(rtk_led_config_t));


    switch(type)
    {
        case LED_TYPE_UTP0:
        case LED_TYPE_FIBER0:
        case LED_TYPE_FIBER1:

            break;
        default:
            return RT_ERR_INPUT;
            break;
    }


    /*get force mode*/
    field = RTL9601B_CPU_FORCE_MODf;
    if ((ret = _dal_rtl9601b_led_modeEnable_get(ledIdx, field, &pConfig->ledEnable[LED_CONFIG_FORCE_MODE])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set speed 1000 mode*/
    field = RTL9601B_SPD1000f;
    if ((ret = _dal_rtl9601b_led_modeEnable_get(ledIdx, field, &pConfig->ledEnable[LED_CONFIG_SPD1000])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set speed 500 mode*/
    field = RTL9601B_SPD500f;
    if ((ret = _dal_rtl9601b_led_modeEnable_get(ledIdx, field, &pConfig->ledEnable[LED_CONFIG_SPD500])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }


    /*set speed 100 mode*/
    field = RTL9601B_SPD100f;
    if ((ret = _dal_rtl9601b_led_modeEnable_get(ledIdx, field, &pConfig->ledEnable[LED_CONFIG_SPD100])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set speed 10 mode*/
    field = RTL9601B_SPD10f;
    if ((ret = _dal_rtl9601b_led_modeEnable_get(ledIdx, field, &pConfig->ledEnable[LED_CONFIG_SPD10])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set duplex mode*/
    field = RTL9601B_DUPf;
    if ((ret = _dal_rtl9601b_led_modeEnable_get(ledIdx, field, &pConfig->ledEnable[LED_CONFIG_DUP])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set speed 1000 Act mode*/
    field = RTL9601B_SPD1000_ACTf;
    if ((ret = _dal_rtl9601b_led_modeEnable_get(ledIdx, field, &pConfig->ledEnable[LED_CONFIG_SPD1000ACT])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set speed 500 Act mode*/
    field = RTL9601B_SPD500_ACTf;
    if ((ret = _dal_rtl9601b_led_modeEnable_get(ledIdx, field, &pConfig->ledEnable[LED_CONFIG_SPD500ACT])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set speed 100 Act mode*/
    field = RTL9601B_SPD100_ACTf;
    if ((ret = _dal_rtl9601b_led_modeEnable_get(ledIdx, field, &pConfig->ledEnable[LED_CONFIG_SPD100ACT])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set speed 10 Act mode*/
    field = RTL9601B_SPD10_ACTf;
    if ((ret = _dal_rtl9601b_led_modeEnable_get(ledIdx, field, &pConfig->ledEnable[LED_CONFIG_SPD10ACT])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set Rx Act mode*/
    field = RTL9601B_RX_ACTf;
    if ((ret = _dal_rtl9601b_led_modeEnable_get(ledIdx, field, &pConfig->ledEnable[LED_CONFIG_RX_ACT])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set Tx Act mode*/
    field = RTL9601B_TX_ACTf;
    if ((ret = _dal_rtl9601b_led_modeEnable_get(ledIdx, field, &pConfig->ledEnable[LED_CONFIG_TX_ACT])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    /*set Collision mode*/
    field = RTL9601B_COLf;
    if ((ret = _dal_rtl9601b_led_modeEnable_get(ledIdx, field, &pConfig->ledEnable[LED_CONFIG_COL])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9601b_led_Config_set
 * Description:
 *      Set per group Led to congiuration mode
 * Input:
 *      ledIdx  - LED index id.
 *      type    - LED type
 *      config  - LED configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32
dal_rtl9601b_led_config_set(uint32 ledIdx, rtk_led_type_t type, rtk_led_config_t *pConfig)
{
    int32   ret;
    uint32 ledMode;
    uint32 isPortMode = 0;
    uint32 isSoCPonMode = 0;
    int i;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_LED),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(led_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9601B_LED_INDEX_MAX < ledIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pConfig), RT_ERR_NULL_POINTER);


    for(i = 0; i<LED_CONFIG_END ; i++)
    {
        if(pConfig->ledEnable[i] >= RTK_ENABLE_END)
            return RT_ERR_INPUT;
    }


    /*rtl9601B do not support type none*/
    if(type == LED_TYPE_NONE)
        type = LED_TYPE_UTP0;

    switch(type)
    {
        case LED_TYPE_UTP0:
            ledMode = RTL9601B_LED_TYPE_UTP0;
            isPortMode = 1;
            break;

        case LED_TYPE_FIBER0:
            ledMode = RTL9601B_LED_TYPE_FIBER0;
            isPortMode = 1;
            break;
        case LED_TYPE_FIBER1:
            ledMode = RTL9601B_LED_TYPE_FIBER1;
            isPortMode = 1;
            break;

        case LED_TYPE_PON:
            ledMode = RTL9601B_LED_TYPE_PON;
            isSoCPonMode = 1;
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    if ((ret = reg_array_field_write(RTL9601B_DATA_LED_CFGr, REG_ARRAY_INDEX_NONE, ledIdx, RTL9601B_LED_CFGf, &ledMode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    if(isSoCPonMode)
    {
        if ((ret = _dal_rtl9601b_led_ponSocConfig_set(ledIdx, type, pConfig)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_LED|MOD_DAL), "");
            return ret;
        }
    }
    if(isPortMode)
    {
        if ((ret = _dal_rtl9601b_led_portConfig_set(ledIdx, type, pConfig)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_LED|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_led_config_set */

/* Function Name:
 *      dal_rtl9601b_led_groupConfig_set
 * Description:
 *      Set per group Led to congiuration mode
 * Input:
 *      ledIdx  - LED index id.
 * Output:
 *      pType    - LED type
 *      pConfig  - LED configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32
dal_rtl9601b_led_config_get(uint32 ledIdx, rtk_led_type_t *pType, rtk_led_config_t *pConfig)
{
    int32   ret;
    uint32 ledMode;
    uint32 isPortMode = 0;
    uint32 isSoCPonMode = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_LED),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(led_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9601B_LED_INDEX_MAX < ledIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pConfig), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9601B_DATA_LED_CFGr, REG_ARRAY_INDEX_NONE, ledIdx, RTL9601B_LED_CFGf, &ledMode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }


    switch(ledMode)
    {
        case RTL9601B_LED_TYPE_UTP0:
            *pType = LED_TYPE_UTP0;
            isPortMode = 1;
            break;
        case RTL9601B_LED_TYPE_FIBER0:
            *pType = LED_TYPE_FIBER0;
            isPortMode = 1;
            break;
        case RTL9601B_LED_TYPE_FIBER1:
            *pType = LED_TYPE_FIBER1;
            isPortMode = 1;
            break;

        case RTL9601B_LED_TYPE_PON:
            *pType = LED_TYPE_PON;
            isSoCPonMode = 1;
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }



    if(isSoCPonMode)
    {
        if ((ret = _dal_rtl9601b_led_ponSocConfig_get(ledIdx, *pType, pConfig)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_LED|MOD_DAL), "");
            return ret;
        }
    }
    if(isPortMode)
    {
        if ((ret = _dal_rtl9601b_led_portConfig_get(ledIdx, *pType, pConfig)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_LED|MOD_DAL), "");
            return ret;
        }
        pConfig->ledEnable[LED_CONFIG_SOC_LINK_ACK] = DISABLED;
        pConfig->ledEnable[LED_CONFIG_PON_LINK] = DISABLED;

    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_led_config_get */

/* Function Name:
 *      dal_rtl9601b_led_modeForce_get
 * Description:
 *      Get Led group to congiuration force mode
 * Input:
 *      ledIdx - LED index id.
 * Output:
 *      pMode  - LED force mode.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can get forced Led group mode.
 *      The force modes that can be set are as following:
 *      - LED_FORCE_BLINK,
 *      - LED_FORCE_OFF,
 *      - LED_FORCE_ON.
 *      For LED_OP_SERIAL the max led index is 31
 *      For LED_OP_PARALLEL the max led index is 16
 */
int32
dal_rtl9601b_led_modeForce_get(uint32 ledIdx, rtk_led_force_mode_t *pMode)
{
    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_LED),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(led_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9601B_LED_INDEX_MAX < ledIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9601B_LED_FORCE_VALUE_CFGr, REG_ARRAY_INDEX_NONE, ledIdx, RTL9601B_SEL_LED_FORCE_VALUEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }

    switch(data)
    {
        case RTL9601B_LED_FORCE_OFF:
            *pMode = LED_FORCE_OFF;
            break;
        case RTL9601B_LED_FORCE_ON:
            *pMode = LED_FORCE_ON;
            break;
        case RTL9601B_LED_FORCE_BLINK:
            *pMode = LED_FORCE_BLINK;
            break;
        default:
            return RT_ERR_FAILED;
            break;

    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_led_modeForce_get */

/* Function Name:
 *      dal_rtl9601b_led_modeForce_set
 * Description:
 *      Set Led group to congiuration force mode
 * Input:
 *      ledIdx - LED index id.
 *      mode  - LED force mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can get forced Led group mode.
 *      The force modes that can be set are as following:
 *      - LED_FORCE_NORMAL,
 *      - LED_FORCE_BLINK,
 *      - LED_FORCE_OFF,
 *      - LED_FORCE_ON.
 *      For LED_OP_SERIAL the max led index is 31
 *      For LED_OP_PARALLEL the max led index is 16
 */
int32
dal_rtl9601b_led_modeForce_set(uint32 ledIdx, rtk_led_force_mode_t mode)
{
    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_LED),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(led_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9601B_LED_INDEX_MAX < ledIdx), RT_ERR_INPUT);
    switch(mode)
    {
        case LED_FORCE_OFF:
            data = RTL9601B_LED_FORCE_OFF;
            break;
        case LED_FORCE_ON:
            data = RTL9601B_LED_FORCE_ON;
            break;
        case LED_FORCE_BLINK:
            data = RTL9601B_LED_FORCE_BLINK;
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    if ((ret = reg_array_field_write(RTL9601B_LED_FORCE_VALUE_CFGr, REG_ARRAY_INDEX_NONE, ledIdx, RTL9601B_SEL_LED_FORCE_VALUEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_LED|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_led_modeForce_set */



/* Function Name:
 *      dal_rtl9601b_led_parallelEnable_get
 * Description:
 *      Get Led group enable status for parallel mode
 * Input:
 *      ledIdx - LED index id.
 * Output:
 *      pState  - LED parallel enable status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32
dal_rtl9601b_led_parallelEnable_get(uint32 ledIdx, rtk_enable_t *pState)
{
    int32 ret;
    uint32 data;
    uint32 ledIoField;
    uint32 ledEnableField;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_LED), "ledIdx=%d",ledIdx);

    /* check Init status */
    RT_INIT_CHK(led_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9601B_PARALLEL_LED_MAX < ledIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    switch(ledIdx)
    {
        case 0:
            ledIoField     = RTL9601B_LED0_ENf;
            ledEnableField = RTL9601B_LED0_PARA_ENf;
            break;
        case 1:
            ledIoField = RTL9601B_LED1_ENf;
            ledEnableField = RTL9601B_LED1_PARA_ENf;
            break;
        case 2:
            ledIoField = RTL9601B_LED2_ENf;
            ledEnableField = RTL9601B_LED2_PARA_ENf;
            break;
        case 3:
            ledIoField = RTL9601B_LED3_ENf;
            ledEnableField = RTL9601B_LED3_PARA_ENf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    /* function body */
    if((ret = reg_field_read(RTL9601B_LED_ENr, ledEnableField, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return ret;
    }

    if(1 == data)
        *pState = ENABLED;
    else
        *pState = DISABLED;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_led_parallelEnable_get */

/* Function Name:
 *      dal_rtl9601b_led_parallelEnable_set
 * Description:
 *      Set Led group enable status for parallel mode
 * Input:
 *      ledIdx - LED index id.
 *      state  - LED parallel enable status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32
dal_rtl9601b_led_parallelEnable_set(uint32 ledIdx, rtk_enable_t state)
{
    int32 ret;
    uint32 data;
    uint32 ledIoField;
    uint32 ledEnableField;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_LED), "ledIdx=%d,state=%d",ledIdx, state);

    /* check Init status */
    RT_INIT_CHK(led_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9601B_PARALLEL_LED_MAX < ledIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    switch(ledIdx)
    {
        case 0:
            ledIoField     = RTL9601B_LED0_ENf;
            ledEnableField = RTL9601B_LED0_PARA_ENf;
            break;
        case 1:
            ledIoField     = RTL9601B_LED1_ENf;
            ledEnableField = RTL9601B_LED1_PARA_ENf;
            break;
        case 2:
            ledIoField     = RTL9601B_LED2_ENf;
            ledEnableField = RTL9601B_LED2_PARA_ENf;
            break;
        case 3:
            ledIoField     = RTL9601B_LED3_ENf;
            ledEnableField = RTL9601B_LED3_PARA_ENf;
            break;
        default:
            return RT_ERR_INPUT;
    }


    /* function body */
    if(ENABLED==state)
    {
        /*LED enable must disable jtag*/
        data = 1;
        if((ret = reg_field_write(RTL9601B_IO_MODE_ENr, RTL9601B_JTAG_DISf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_LED), "");
            return ret;
        }


        /*set LED_EN */
        data = 1;
        if((ret = reg_field_write(RTL9601B_LED_ENr, ledEnableField, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_LED), "");
            return ret;
        }

        /*set IO_LED_ENr*/
        data = 1;
        if((ret = reg_field_write(RTL9601B_IO_LED_ENr, ledIoField, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_LED), "");
            return ret;
        }
    }
    else
    {
        /*set LED_EN */
        data = 0;
        if((ret = reg_field_write(RTL9601B_LED_ENr, ledEnableField, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_LED), "");
            return ret;
        }

        /*set IO_LED_ENr*/
        data = 0;
        if((ret = reg_field_write(RTL9601B_IO_LED_ENr, ledIoField, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_LED), "");
            return ret;
        }
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_led_parallelEnable_set */


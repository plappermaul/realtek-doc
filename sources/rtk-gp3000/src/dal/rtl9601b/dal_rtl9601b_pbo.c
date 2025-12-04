/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : switch asic-level packet buffer offload API
 * Feature : packet buffer offload related functions
 *
 */

#include <dal/rtl9601b/dal_rtl9601b_pbo.h>
#include <ioal/mem32.h>
#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/kernel.h>
#endif

#if defined(FPGA_DEFINED)
#define PBO_USE_DRAM            0
#else
#define PBO_USE_DRAM            1
#endif

int32 rtl9601b_raw_pbo_dramUsage_init(void);

/* Function Name:
 *      rtl9601b_raw_pbo_stopAllThreshold_set
 * Description:
 *      Set pbo stop all threshold
 * Input:
 *      threshold         - stop all threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_stopAllThreshold_set(uint32 threshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < threshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9601B_PON_SID_STOP_THr, RTL9601B_THf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9601b_raw_pbo_stopAllThreshold_set */

/* Function Name:
 *      rtl9601b_raw_pbo_stoppAllThreshold_get
 * Description:
 *      Get pbo stop all threshold
 * Input:
 *      None
 * Output:
 *      pThreshold         - stop all threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_stopAllThreshold_get(uint32 *pThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9601B_PON_SID_STOP_THr, RTL9601B_THf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9601b_raw_pbo_stopAllThreshold_get */

/* Function Name:
 *      rtl9601b_raw_pbo_stopAllState_set
 * Description:
 *      Set pbo stop all threshold
 * Input:
 *      state         - stop all state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_stopAllState_set(rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    value = (enable == ENABLED) ? 1 : 0;
    if ((ret = reg_field_write(RTL9601B_PONIP_MODEr, RTL9601B_CFG_STOP_RXC_ENf, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9601b_raw_pbo_stopAllState_set */

/* Function Name:
 *      rtl9601b_raw_pbo_stopAllState_get
 * Description:
 *      Get pbo stop all threshold
 * Input:
 *      None
 * Output:
 *      pEnable         - stop all state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_stopAllState_get(rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((pEnable == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9601B_PONIP_MODEr, RTL9601B_CFG_STOP_RXC_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pEnable = ((value == 1) ? ENABLED : DISABLED);

    return RT_ERR_OK;
} /* end of rtl9601b_raw_pbo_stopAllState_get */

/* Function Name:
 *      rtl9601b_raw_pbo_ponMode_set
 * Description:
 *      Set pbo PON mode
 * Input:
 *      mode         - PON mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_ponMode_set(rtl9601b_pbo_ponMode_t mode)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((mode >= RTL9601B_PBO_PONMODE_END), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9601B_PONIP_MODEr, RTL9601B_CFG_EPON_MODEf, &mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9601b_raw_pbo_ponMode_set */

/* Function Name:
 *      rtl9601b_raw_pbo_ponMode_get
 * Description:
 *      Set pbo PON mode
 * Input:
 *      mode         - PON mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_ponMode_get(rtl9601b_pbo_ponMode_t *pMode)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((NULL == pMode), RT_ERR_INPUT);

    if ((ret = reg_field_read(RTL9601B_PONIP_MODEr, RTL9601B_CFG_EPON_MODEf, pMode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9601b_raw_pbo_ponMode_get */

/* Function Name:
 *      rtl9601b_raw_pbo_dscRunoutThreshold_set
 * Description:
 *      Set pbo descriptor run out threshold
 * Input:
 *      dram         - DRAM threshold
 *      sram         - SRAM threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_dscRunoutThreshold_set(uint32 dram, uint32 sram)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < dram), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < sram), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9601B_DSCRUNOUTr, RTL9601B_CFG_DRAM_RUNOUTf, &dram)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9601B_DSCRUNOUTr, RTL9601B_CFG_SRAM_RUNOUTf, &sram)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9601b_raw_pbo_dropAllThreshold_set */

/* Function Name:
 *      rtl9601b_raw_pbo_dscRunoutThreshold_get
 * Description:
 *      Get pbo descriptor run out threshold
 * Input:
 *      None
 * Output:
 *      pDram         - DRAM threshold
 *      pSram         - SRAM threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_dscRunoutThreshold_get(uint32 *pDram, uint32 *pSram)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pDram == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pSram == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9601B_DSCRUNOUTr, RTL9601B_CFG_DRAM_RUNOUTf, pDram)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9601B_DSCRUNOUTr, RTL9601B_CFG_SRAM_RUNOUTf, pSram)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9601b_raw_pbo_dropAllThreshold_get */

/* Function Name:
 *      rtl9601b_raw_pbo_globalThreshold_set
 * Description:
 *      Set global on/off threshold of pbo.
 * Input:
 *      onThreshold         - global on threshold
 *      offThreshold         - global off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_globalThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9601B_PON_SID_GLB_THr, RTL9601B_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9601B_PON_SID_GLB_THr, RTL9601B_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9601b_raw_pbo_globalThreshold_set */

/* Function Name:
 *      rtl9601b_raw_pbo_globalThreshold_get
 * Description:
 *      Get global on/off threshold of pbo.
 * Input:
 *      None
 * Output:
 *      pOnThreshold         - global on threshold
 *      pOffThreshold         - global off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_globalThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9601B_PON_SID_GLB_THr, RTL9601B_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9601B_PON_SID_GLB_THr, RTL9601B_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9601b_raw_pbo_globalThreshold_get */

/* Function Name:
 *      rtl9601b_raw_pbo_sidThreshold_set
 * Description:
 *      Set per SID on/off threshold of pbo.
 * Input:
 *      sid                 - SID
 *      onThreshold         - per SID on threshold
 *      offThreshold        - per SID off threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_sidThreshold_set(uint32 sid, uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() < sid), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(RTL9601B_PON_SID_RPV_THr, REG_ARRAY_INDEX_NONE, sid, RTL9601B_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9601B_PON_SID_RPV_THr, REG_ARRAY_INDEX_NONE, sid, RTL9601B_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9601b_raw_pbo_sidThreshold_set */

/* Function Name:
 *      rtl9601b_raw_pbo_sidThreshold_get
 * Description:
 *      Get per SID on/off threshold of pbo.
 * Input:
 *      None
 * Output:
 *      sid                 - SID
 *      pOnThreshold        - per SID on threshold
 *      pOffThreshold       - per SID off threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_sidThreshold_get(uint32 sid,uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() < sid), RT_ERR_INPUT);
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9601B_PON_SID_RPV_THr, REG_ARRAY_INDEX_NONE, sid, RTL9601B_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9601B_PON_SID_RPV_THr, REG_ARRAY_INDEX_NONE, sid, RTL9601B_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9601b_raw_pbo_sidThreshold_get */

/* Function Name:
 *      rtl9601b_raw_pbo_state_set
 * Description:
 *      Set PBO to enable or disable
 * Input:
 *      enable              - enable / disable PBO module
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_state_set(rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    value = (enable == ENABLED) ? 1 : 0;
    if ((ret = reg_field_write(RTL9601B_PONIP_MODEr, RTL9601B_CFG_PBUF_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9601b_raw_pbo_state_get
 * Description:
 *      Get current PBO state
 * Input:
 *      None
 * Output:
 *      pEnable              - PBO module is enable or disable
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_state_get(rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((pEnable == NULL), RT_ERR_INPUT);

    if ((ret = reg_field_read(RTL9601B_PONIP_MODEr, RTL9601B_CFG_PBUF_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pEnable = ((value == 1) ? ENABLED : DISABLED);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9601b_raw_pbo_status_get
 * Description:
 *      Get current PBO status
 * Input:
 *      None
 * Output:
 *      pEnable              - PBO module is ready or not
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_status_get(rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((pEnable == NULL), RT_ERR_INPUT);

    if ((ret = reg_field_read(RTL9601B_PON_IPSTSr, RTL9601B_PONIC_INITRDYf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pEnable = ((value == 1) ? ENABLED : DISABLED);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9601b_raw_pbo_usedPageCount_get
 * Description:
 *      Get current PBO used page count
 * Input:
 *      None
 * Output:
 *      pSram               - PBO SRAM used page count
 *      pDram               - PBO DRAM used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_usedPageCount_get(uint32 *pSram, uint32 *pDram)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((pSram == NULL), RT_ERR_INPUT);
    RT_PARAM_CHK((pDram == NULL), RT_ERR_INPUT);

    if ((ret = reg_field_read(RTL9601B_PON_DSC_USAGEr, RTL9601B_PONNIC_SRAM_USEDf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pSram = (value) & 0x1fff;

    if ((ret = reg_field_read(RTL9601B_PON_DSC_USAGEr, RTL9601B_PONNIC_DRAM_USEDf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pDram = (value) & 0x1fff;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9601b_raw_pbo_maxUsedPageCount_get
 * Description:
 *      Get maximum PBO used page count
 * Input:
 *      None
 * Output:
 *      pSram               - PBO SRAM maximum used page count
 *      pDram               - PBO DRAM maximum used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_maxUsedPageCount_get(uint32 *pSram, uint32 *pDram)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((pSram == NULL), RT_ERR_INPUT);
    RT_PARAM_CHK((pDram == NULL), RT_ERR_INPUT);

    if ((ret = reg_field_read(RTL9601B_PON_DSC_STSr, RTL9601B_SRAM_USED_MAXf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pSram = (value) & 0x1fff;

    if ((ret = reg_field_read(RTL9601B_PON_DSC_STSr, RTL9601B_DRAM_USED_MAXf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pDram = (value) & 0x1fff;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9601b_raw_pbo_flowcontrolStatus_get
 * Description:
 *      Get maximum PBO used page count
 * Input:
 *      sid                 - SID
 *      latched             - latched status or current status
 * Output:
 *      pSram               - PBO SRAM maximum used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_flowcontrolStatus_get(uint32 sid, rtl9601b_pbo_fc_status_t latched, rtk_enable_t *pStatus)
{
    int32  ret;
    uint32 reg;
    uint32 value;

    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() < sid), RT_ERR_INPUT);
    RT_PARAM_CHK((latched >= RTL9601B_PBO_FLOWCONTROL_STATUS_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pStatus == NULL), RT_ERR_INPUT);

    switch(latched)
    {
    case RTL9601B_PBO_FLOWCONTROL_STATUS_CURRENT:
        reg = RTL9601B_PONIP_SID_OVER_STSr;
        break;
    case RTL9601B_PBO_FLOWCONTROL_STATUS_LATCHED:
        reg = RTL9601B_PONIP_SID_OVER_LATCH_STSr;
        break;
    default:
        return RT_ERR_INPUT;
    }

    if ((ret = reg_array_field_read(reg, REG_ARRAY_INDEX_NONE, sid, RTL9601B_OVER_STSf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pStatus = value == 1 ? ENABLED : DISABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9601b_raw_pbo_flowcontrolLatchStatus_clear
 * Description:
 *      Clear the latched flowcontrol status
 * Input:
 *      sid                 - SID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_flowcontrolLatchStatus_clear(uint32 sid)
{
    int32  ret;
     uint32 value;

    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() < sid), RT_ERR_INPUT);

    value = 1;
    if ((ret = reg_array_field_write(RTL9601B_PONIP_SID_OVER_LATCH_STSr, REG_ARRAY_INDEX_NONE, sid, RTL9601B_OVER_STSf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9601b_raw_pbo_maxUsedPageCount_clear
 * Description:
 *      Clear maximum PBO used page count
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_maxUsedPageCount_clear(void)
{
    int32   ret;
    uint32  value;

    value = 1;
    if ((ret = reg_field_write(RTL9601B_PON_DSC_STSr, RTL9601B_CLR_RAM_USED_MAXf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9601b_raw_pbo_counterGroupMember_add
 * Description:
 *      Add the member to counter group
 * Input:
 *      group               - group id
 *      sid                 - sid to be added to group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_counterGroupMember_add(uint32 group, uint32 sid)
{
    int32   ret;
    uint32  value;

    RT_PARAM_CHK((group >= 4), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() < sid), RT_ERR_INPUT);

    value = 1;
    if ((ret = reg_array_field_write(RTL9601B_CNT_MASKr, group, sid, RTL9601B_SID_MASKf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9601b_raw_pbo_counterGroupMember_clear
 * Description:
 *      Clear all member of specific group
 * Input:
 *      group               - group id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_counterGroupMember_clear(uint32 group)
{
    int32   ret;
    uint32  i;
    uint32  value;

    RT_PARAM_CHK((group >= 4), RT_ERR_INPUT);

    for(i = 0;i < HAL_CLASSIFY_SID_NUM();i++)
    {
        value = 0;
        if ((ret = reg_array_field_write(RTL9601B_CNT_MASKr, group, i, RTL9601B_SID_MASKf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9601b_raw_pbo_counterGroupMember_get
 * Description:
 *      Check if the specifiec SID belong to the specific group
 * Input:
 *      group               - group id
 *      sid                 - sid to be checked
 * Output:
 *      pEnable             - if the SID belong to the group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_counterGroupMember_get(uint32 group, uint32 sid, rtk_enable_t *pEnabled)
{
    int32   ret;
    uint32  value = 0;

    RT_PARAM_CHK((group >= 4), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() < sid), RT_ERR_INPUT);
    RT_PARAM_CHK((pEnabled == NULL), RT_ERR_INPUT);

    if ((ret = reg_array_field_read(RTL9601B_CNT_MASKr, group, sid, RTL9601B_SID_MASKf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pEnabled = (value == 1) ? ENABLED : DISABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9601b_raw_pbo_counter_get
 * Description:
 *      Get PBO packet counters
 * Input:
 *      None
 * Output:
 *      pCnt                - PBO counter set
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_counter_get(rtl9601b_pbo_cnt_t *pCnt)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((pCnt == NULL), RT_ERR_INPUT);

    if ((ret = reg_read(RTL9601B_RX_SID_ERR_CNTr, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->rx_err_cnt = value;

    if ((ret = reg_read(RTL9601B_TX_EMPTY_EOBr, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->tx_empty_eob_cnt = value;

    if ((ret = reg_read(RTL9601B_RX_ERR_SIDIN_CNTr, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->rx_err_cnt = value;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9601b_raw_pbo_counter_get
 * Description:
 *      Get PBO packet counters
 * Input:
 *      group               - group counter set to be get
 * Output:
 *      pCnt                - PBO counter set
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_groupCounter_get(uint32 group, rtl9601b_pbo_group_cnt_t *pCnt)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((group >= 5), RT_ERR_INPUT);
    RT_PARAM_CHK((pCnt == NULL), RT_ERR_INPUT);

    if ((ret = reg_array_read(RTL9601B_RX_SID_GOOD_CNTr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->rx_good_cnt = value;

    if ((ret = reg_array_read(RTL9601B_RX_SID_BAD_CNTr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->rx_bad_cnt = value;

    if ((ret = reg_array_read(RTL9601B_TX_SID_CNTr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->tx_cnt = value;

    if ((ret = reg_array_read(RTL9601B_TX_SID_FRAG_CNTr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->tx_frag_cnt = value;

    if ((ret = reg_array_read(RTL9601B_RX_SIDIN_CNTr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->rx_in_cnt = value;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9601b_raw_pbo_counter_clear
 * Description:
 *      Clear PBO packet counters
 * Input:
 *      group               - group counter set to be clear
 * Output:
 *      pCnt                - PBO counter set
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_groupCounter_clear(uint32 group)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((group >= 5), RT_ERR_INPUT);

    value = 1;
    if ((ret = reg_array_write(RTL9601B_RX_SID_GOOD_CNTr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    value = 1;
    if ((ret = reg_array_write(RTL9601B_RX_SID_BAD_CNTr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    value = 1;
    if ((ret = reg_array_write(RTL9601B_TX_SID_CNTr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    value = 1;
    if ((ret = reg_array_write(RTL9601B_TX_SID_FRAG_CNTr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    value = 1;
    if ((ret = reg_array_write(RTL9601B_RX_SIDIN_CNTr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

#if PBO_USE_DRAM
int32 rtl9601b_raw_pbo_dramUsage_init(void)
{
    int ret;

#if defined(CONFIG_SDK_KERNEL_LINUX)
    char *ptr_start, *ptr_end;
    char cmdline[256];
    uint32 offset;
    extern char arcs_cmdline[];
#endif
    uint32 dram_cnt, sram_cnt;
    uint32 dram_runt, sram_runt;
    uint32 mem_size = 0;


#if defined(CONFIG_SDK_KERNEL_LINUX)
    #if defined(CONFIG_PBO_MEM_USAGE)
    mem_size = CONFIG_PBO_MEM_USAGE;
    #endif

    /* Retrive the kernel memory settings from u-boot */
    osal_memcpy(cmdline, arcs_cmdline, ((sizeof(cmdline) <= strlen(arcs_cmdline)) ? sizeof(cmdline) : strlen(arcs_cmdline)));
    ptr_start = strstr(cmdline, "mem=");

    #if defined(CONFIG_CMDLINE)
    if(NULL == ptr_start)
    {
        /* Try to use CONFIG_CMDLINE*/
        osal_memcpy(cmdline, CONFIG_CMDLINE, ((sizeof(cmdline) <= strlen(CONFIG_CMDLINE)) ? sizeof(cmdline) : strlen(CONFIG_CMDLINE)));
    }
    ptr_start = strstr(cmdline, "mem=");
    #endif

    if(NULL != ptr_start)
    {
        ptr_start = strchr(ptr_start, '=');
        if(NULL != ptr_start)
        {
            ptr_start += 1;
            ptr_end = strchr(ptr_start, 'M');
            if(NULL != ptr_end)
            {
                *ptr_end = '\0';
                offset = simple_strtol(ptr_start, NULL, 10);

                if(offset >= 256)
                {
                    /* memory size > 256MB? or parsing error?
                     * If this is the actual case, incrase the compare value
                     */
                    mem_size = 0;
                }
                else
                {
                    offset *= 1024 * 1024;

                    if ((ret = reg_field_write(RTL9601B_IP_MSTBASEr, RTL9601B_CFG_PON_MSTBASEf, &offset)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
                        return ret;
                    }
                }
            }
            else
            {
                mem_size = 0;
            }
        }
        else
        {
            mem_size = 0;
        }
    }
    else
    {
        mem_size = 0;
    }
#else
    /* No CONFIG_SDK_KERNEL_LINUX, don't use DRAM */
#endif

    /* Descriptor count for SRAM usage */
    sram_cnt = 0xff;
    if ((ret = reg_field_write(RTL9601B_PON_DSC_CFGr, RTL9601B_CFG_SRAM_NOf, &sram_cnt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* Descriptor count for DRAM usage */
    if(0 != mem_size)
    {
        dram_cnt = (mem_size * 1024) - 1;
    }
    else
    {
        dram_cnt = sram_cnt;
    }
    if ((ret = reg_field_write(RTL9601B_PON_DSC_CFGr, RTL9601B_CFG_RAM_NOf, &dram_cnt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* Runt count for SRAM */
    sram_runt = (sram_cnt + 1) - 0x10;
    if ((ret = reg_field_write(RTL9601B_DSCRUNOUTr, RTL9601B_CFG_SRAM_RUNOUTf, &sram_runt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* Runt count for DRAM */
    if(0 != mem_size)
    {
        dram_runt = dram_cnt - sram_cnt - 0x10;
    }
    else
    {
        dram_runt = 0;
    }
    if ((ret = reg_field_write(RTL9601B_DSCRUNOUTr, RTL9601B_CFG_DRAM_RUNOUTf, &dram_runt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}
#endif

/* Function Name:
 *      rtl9601b_raw_pbo_init
 * Description:
 *      Inititalize PBO
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9601b_raw_pbo_init(void)
{
    int32   ret;
    uint32  value;
#if PBO_USE_DRAM
#else
    uint32  dram_cnt, sram_cnt;
    uint32  dram_runt, sram_runt;
#endif
    value = 3;
    if ((ret = reg_field_write(RTL9601B_PON_US_FIFO_CTLr, RTL9601B_CFG_USFIFO_STARTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    value = 1;
    if ((ret = reg_field_write(RTL9601B_PON_US_FIFO_CTLr, RTL9601B_CFG_USFIFO_SPACEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

#if PBO_USE_DRAM
    if ((ret = rtl9601b_raw_pbo_dramUsage_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
#else
    #if defined(FPGA_DEFINED)
    #else
    sram_cnt = 0xff;
    dram_cnt = 0xff;
    sram_runt = 0xf0;
    dram_runt = 0;
    if ((ret = reg_field_write(RTL9601B_PON_DSC_CFGr, RTL9601B_CFG_SRAM_NOf, &sram_cnt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9601B_PON_DSC_CFGr, RTL9601B_CFG_RAM_NOf, &dram_cnt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* runt */
    if ((ret = reg_field_write(RTL9601B_DSCRUNOUTr, RTL9601B_CFG_SRAM_RUNOUTf, &sram_runt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9601B_DSCRUNOUTr, RTL9601B_CFG_DRAM_RUNOUTf, &dram_runt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    #endif
#endif

    /* Manual reset PBO */
    if ((ret = rtl9601b_raw_pbo_state_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    /* Disable PON NIC Tx/Rx */
    if((ret = ioal_socMem32_read(0xB8015434, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value &= ~(0x30);
    if((ret = ioal_socMem32_write(0xB8015434, value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = rtl9601b_raw_pbo_state_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 0x10;
    if ((ret = reg_field_write(RTL9601B_PON_EMPTY_EOB_CFGr, RTL9601B_CFG_EMPTY_EOB_BCf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* Initial PON NIC */
    if ((ret = ioal_ponNic_write(0xB8014040, 0x00000c05)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = ioal_ponNic_write(0xB801404c, 0x20000000)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = ioal_ponNic_write(0xB8015400, 0x00000023)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = ioal_ponNic_write(0xB80153f0, 0x00000000)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    /*PONNIC RX DMA unit set to 128bytes*/
    if((ret = ioal_socMem32_write(0xB8015434, 0x90100870))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}


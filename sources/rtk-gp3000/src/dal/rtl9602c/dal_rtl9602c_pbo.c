/*
 * Copyright (C) 2015 Realtek Semiconductor Corp.
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

#include <dal/rtl9602c/dal_rtl9602c_pbo.h>
#include <dal/rtl9602c/dal_rtl9602c_switch.h>
#include <ioal/mem32.h>
#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/kernel.h>
#endif

#define PBO_USE_DRAM            1
#define PBO_USE_DS_DRAM         1

#if defined(CONFIG_SDK_KERNEL_LINUX)
static rtl9602c_memUsage_t memUsage;
#endif

int32 rtl9602c_raw_pbo_memUsage_init(uint32 mode, uint32 usPageSize, uint32 dsPageSize);

/* Function Name:
 *      rtl9602c_raw_pbo_stopAllThreshold_set
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
int32 rtl9602c_raw_pbo_stopAllThreshold_set(uint32 threshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < threshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9602C_PON_SID_STOP_THr, RTL9602C_THf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9602c_raw_pbo_stopAllThreshold_set */

/* Function Name:
 *      rtl9602c_raw_pbo_stoppAllThreshold_get
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
int32 rtl9602c_raw_pbo_stopAllThreshold_get(uint32 *pThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_PON_SID_STOP_THr, RTL9602C_THf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9602c_raw_pbo_stopAllThreshold_get */

/* Function Name:
 *      rtl9602c_raw_pbo_globalThreshold_set
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
int32 rtl9602c_raw_pbo_globalThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9602C_PON_SID_GLB_THr, RTL9602C_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9602C_PON_SID_GLB_THr, RTL9602C_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9602c_raw_pbo_globalThreshold_set */

/* Function Name:
 *      rtl9602c_raw_pbo_globalThreshold_get
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
int32 rtl9602c_raw_pbo_globalThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_PON_SID_GLB_THr, RTL9602C_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9602C_PON_SID_GLB_THr, RTL9602C_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9602c_raw_pbo_globalThreshold_get */

/* Function Name:
 *      rtl9602c_raw_pbo_sidThreshold_set
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
int32 rtl9602c_raw_pbo_sidThreshold_set(uint32 sid, uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() < sid), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(RTL9602C_PON_SID_RPV_THr, REG_ARRAY_INDEX_NONE, sid, RTL9602C_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9602C_PON_SID_RPV_THr, REG_ARRAY_INDEX_NONE, sid, RTL9602C_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9602c_raw_pbo_sidThreshold_set */

/* Function Name:
 *      rtl9602c_raw_pbo_sidThreshold_get
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
int32 rtl9602c_raw_pbo_sidThreshold_get(uint32 sid,uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() < sid), RT_ERR_INPUT);
    RT_PARAM_CHK((pOnThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pOffThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_PON_SID_RPV_THr, REG_ARRAY_INDEX_NONE, sid, RTL9602C_ON_THf, pOnThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9602C_PON_SID_RPV_THr, REG_ARRAY_INDEX_NONE, sid, RTL9602C_OFF_THf, pOffThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9602c_raw_pbo_sidThreshold_get */

/* Function Name:
 *      rtl9602c_raw_pbo_usState_set
 * Description:
 *      Set PBO upstream to enable or disable
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
int32 rtl9602c_raw_pbo_usState_set(rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    value = (enable == ENABLED) ? 1 : 0;
    if ((ret = reg_field_write(RTL9602C_PONIP_CTL_USr, RTL9602C_CFG_PBUF_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_usState_get
 * Description:
 *      Get current PBO upstream state
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
int32 rtl9602c_raw_pbo_usState_get(rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((pEnable == NULL), RT_ERR_INPUT);

    if ((ret = reg_field_read(RTL9602C_PONIP_CTL_USr, RTL9602C_CFG_PBUF_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pEnable = ((value == 1) ? ENABLED : DISABLED);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsState_set
 * Description:
 *      Set PBO downstream to enable or disable
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
int32 rtl9602c_raw_pbo_dsState_set(rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    value = (enable == ENABLED) ? 1 : 0;
    if ((ret = reg_field_write(RTL9602C_PONIP_CTL_DSr, RTL9602C_CFG_PBUF_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsState_get
 * Description:
 *      Get current PBO downstream state
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
int32 rtl9602c_raw_pbo_dsState_get(rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((pEnable == NULL), RT_ERR_INPUT);

    if ((ret = reg_field_read(RTL9602C_PONIP_CTL_DSr, RTL9602C_CFG_PBUF_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pEnable = ((value == 1) ? ENABLED : DISABLED);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_usStatus_get
 * Description:
 *      Get current PBO upstream status
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
int32 rtl9602c_raw_pbo_usStatus_get(rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((pEnable == NULL), RT_ERR_INPUT);

    if ((ret = reg_field_read(RTL9602C_PON_IPSTS_USr, RTL9602C_PONIC_INITRDYf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pEnable = ((value == 1) ? ENABLED : DISABLED);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsStatus_get
 * Description:
 *      Get current PBO downstream status
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
int32 rtl9602c_raw_pbo_dsStatus_get(rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((pEnable == NULL), RT_ERR_INPUT);

    if ((ret = reg_field_read(RTL9602C_PON_IPSTS_DSr, RTL9602C_PONIC_INITRDYf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pEnable = ((value == 1) ? ENABLED : DISABLED);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_usDscRunoutThreshold_set
 * Description:
 *      Set pbo upstream descriptor run out threshold
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
int32 rtl9602c_raw_pbo_usDscRunoutThreshold_set(uint32 dram, uint32 sram)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < dram), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < sram), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9602C_DSCRUNOUT_USr, RTL9602C_CFG_DRAM_RUNOUTf, &dram)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9602C_DSCRUNOUT_USr, RTL9602C_CFG_SRAM_RUNOUTf, &sram)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9602c_raw_pbo_usDscRunoutThreshold_set */

/* Function Name:
 *      rtl9602c_raw_pbo_usDscRunoutThreshold_get
 * Description:
 *      Get pbo upstream descriptor run out threshold
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
int32 rtl9602c_raw_pbo_usDscRunoutThreshold_get(uint32 *pDram, uint32 *pSram)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pDram == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pSram == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_DSCRUNOUT_USr, RTL9602C_CFG_DRAM_RUNOUTf, pDram)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9602C_DSCRUNOUT_USr, RTL9602C_CFG_SRAM_RUNOUTf, pSram)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9602c_raw_pbo_usDscRunoutThreshold_get */

/* Function Name:
 *      rtl9602c_raw_pbo_dsDscRunoutThreshold_set
 * Description:
 *      Set pbo downstream descriptor run out threshold
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
int32 rtl9602c_raw_pbo_dsDscRunoutThreshold_set(uint32 dram, uint32 sram)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < dram), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < sram), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9602C_DSCRUNOUT_DSr, RTL9602C_CFG_DRAM_RUNOUTf, &dram)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9602C_DSCRUNOUT_DSr, RTL9602C_CFG_SRAM_RUNOUTf, &sram)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9602c_raw_pbo_dsDscRunoutThreshold_set */

/* Function Name:
 *      rtl9602c_raw_pbo_dsDscRunoutThreshold_get
 * Description:
 *      Get pbo downstream descriptor run out threshold
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
int32 rtl9602c_raw_pbo_dsDscRunoutThreshold_get(uint32 *pDram, uint32 *pSram)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pDram == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pSram == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_DSCRUNOUT_DSr, RTL9602C_CFG_DRAM_RUNOUTf, pDram)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9602C_DSCRUNOUT_DSr, RTL9602C_CFG_SRAM_RUNOUTf, pSram)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9602c_raw_pbo_dsDscRunoutThreshold_get */

/* Function Name:
 *      rtl9602c_raw_pbo_usUsedPageCount_get
 * Description:
 *      Get current PBO upstream used page count
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
int32 rtl9602c_raw_pbo_usUsedPageCount_get(uint32 *pSram, uint32 *pDram)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((pSram == NULL), RT_ERR_INPUT);
    RT_PARAM_CHK((pDram == NULL), RT_ERR_INPUT);

    if ((ret = reg_field_read(RTL9602C_PON_DSC_USAGE_USr, RTL9602C_PONNIC_SRAM_USEDf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pSram = value;

    if ((ret = reg_field_read(RTL9602C_PON_DSC_USAGE_USr, RTL9602C_PONNIC_DRAM_USEDf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pDram = value;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_usMaxUsedPageCount_get
 * Description:
 *      Get maximum PBO upstream used page count
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
int32 rtl9602c_raw_pbo_usMaxUsedPageCount_get(uint32 *pSram, uint32 *pDram)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((pSram == NULL), RT_ERR_INPUT);
    RT_PARAM_CHK((pDram == NULL), RT_ERR_INPUT);

    if ((ret = reg_field_read(RTL9602C_PON_DSC_STS_USr, RTL9602C_SRAM_USED_MAXf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pSram = value;

    if ((ret = reg_field_read(RTL9602C_PON_DSC_STS_USr, RTL9602C_DRAM_USED_MAXf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pDram = value;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_usMaxUsedPageCount_clear
 * Description:
 *      Clear maximum PBO upstream used page count
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
int32 rtl9602c_raw_pbo_usMaxUsedPageCount_clear(void)
{
    int32   ret;
    uint32  value;

    value = 1;
    if ((ret = reg_field_write(RTL9602C_PON_DSC_STS_USr, RTL9602C_CLR_RAM_USED_MAXf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsUsedPageCount_get
 * Description:
 *      Get current PBO downstream used page count
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
int32 rtl9602c_raw_pbo_dsUsedPageCount_get(uint32 *pSram, uint32 *pDram)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((pSram == NULL), RT_ERR_INPUT);
    RT_PARAM_CHK((pDram == NULL), RT_ERR_INPUT);

    if ((ret = reg_field_read(RTL9602C_PON_DSC_USAGE_DSr, RTL9602C_PONNIC_SRAM_USEDf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pSram = value;

    if ((ret = reg_field_read(RTL9602C_PON_DSC_USAGE_DSr, RTL9602C_PONNIC_DRAM_USEDf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pDram = value;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsMaxUsedPageCount_get
 * Description:
 *      Get maximum PBO upstream used page count
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
int32 rtl9602c_raw_pbo_dsMaxUsedPageCount_get(uint32 *pSram, uint32 *pDram)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((pSram == NULL), RT_ERR_INPUT);
    RT_PARAM_CHK((pDram == NULL), RT_ERR_INPUT);

    if ((ret = reg_field_read(RTL9602C_PON_DSC_STS_DSr, RTL9602C_SRAM_USED_MAXf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pSram = value;

    if ((ret = reg_field_read(RTL9602C_PON_DSC_STS_DSr, RTL9602C_DRAM_USED_MAXf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pDram = value;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsMaxUsedPageCount_clear
 * Description:
 *      Clear maximum PBO downstream used page count
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
int32 rtl9602c_raw_pbo_dsMaxUsedPageCount_clear(void)
{
    int32   ret;
    uint32  value;

    value = 1;
    if ((ret = reg_field_write(RTL9602C_PON_DSC_STS_DSr, RTL9602C_CLR_RAM_USED_MAXf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_flowcontrolStatus_get
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
int32 rtl9602c_raw_pbo_flowcontrolStatus_get(uint32 sid, rtl9602c_pbo_fc_status_t latched, rtk_enable_t *pStatus)
{
    int32  ret;
    uint32 reg;
    uint32 value;

    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() < sid), RT_ERR_INPUT);
    RT_PARAM_CHK((latched >= RTL9602C_PBO_FLOWCONTROL_STATUS_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pStatus == NULL), RT_ERR_INPUT);

    switch(latched)
    {
    case RTL9602C_PBO_FLOWCONTROL_STATUS_CURRENT:
        reg = RTL9602C_PONIP_SID_OVER_STSr;
        break;
    case RTL9602C_PBO_FLOWCONTROL_STATUS_LATCHED:
        reg = RTL9602C_PONIP_SID_OVER_LATCH_STSr;
        break;
    default:
        return RT_ERR_INPUT;
    }

    if ((ret = reg_array_field_read(reg, REG_ARRAY_INDEX_NONE, sid, RTL9602C_OVER_STSf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pStatus = value == 1 ? ENABLED : DISABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_flowcontrolLatchStatus_clear
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
int32 rtl9602c_raw_pbo_flowcontrolLatchStatus_clear(uint32 sid)
{
    int32  ret;
     uint32 value;

    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() < sid), RT_ERR_INPUT);

    value = 1;
    if ((ret = reg_array_field_write(RTL9602C_PONIP_SID_OVER_LATCH_STSr, REG_ARRAY_INDEX_NONE, sid, RTL9602C_OVER_STSf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_usCounterGroupMember_add
 * Description:
 *      Add member to counter group for PBO upstream
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
int32 rtl9602c_raw_pbo_usCounterGroupMember_add(uint32 group, uint32 sid)
{
    int32   ret;
    uint32  value;

    RT_PARAM_CHK((group >= 4), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() < sid), RT_ERR_INPUT);

    value = 1;
    if ((ret = reg_array_field_write(RTL9602C_CNT_MASK_USr, group, sid, RTL9602C_SID_MASKf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_usCounterGroupMember_clear
 * Description:
 *      Clear all member of specific group for PBO upstream
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
int32 rtl9602c_raw_pbo_usCounterGroupMember_clear(uint32 group)
{
    int32   ret;
    uint32  i;
    uint32  value;

    RT_PARAM_CHK((group >= 4), RT_ERR_INPUT);

    for(i = 0;i < HAL_CLASSIFY_SID_NUM();i++)
    {
        value = 0;
        if ((ret = reg_array_field_write(RTL9602C_CNT_MASK_USr, group, i, RTL9602C_SID_MASKf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_usCounterGroupMember_get
 * Description:
 *      Check if the specifiec SID belong to the specific group for PBO upstream
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
int32 rtl9602c_raw_pbo_usCounterGroupMember_get(uint32 group, uint32 sid, rtk_enable_t *pEnabled)
{
    int32   ret;
    uint32  value = 0;

    RT_PARAM_CHK((group >= 4), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() < sid), RT_ERR_INPUT);
    RT_PARAM_CHK((pEnabled == NULL), RT_ERR_INPUT);

    if ((ret = reg_array_field_read(RTL9602C_CNT_MASK_USr, group, sid, RTL9602C_SID_MASKf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pEnabled = (value == 1) ? ENABLED : DISABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_usCounter_get
 * Description:
 *      Get PBO packet counters for PBO upstream
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
int32 rtl9602c_raw_pbo_usCounter_get(rtl9602c_pbo_cnt_t *pCnt)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((pCnt == NULL), RT_ERR_INPUT);

    if ((ret = reg_read(RTL9602C_RX_ERR_CNT_USr, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->rx_err_cnt = value;

    if ((ret = reg_read(RTL9602C_TX_EMPTY_EOB_USr, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->tx_empty_eob_cnt = value;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_usCounter_clear
 * Description:
 *      Clear PBO packet counters for PBO upstream
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
int32 rtl9602c_raw_pbo_usCounter_clear(void)
{
    int32   ret;
    uint32  value;

    value = 1;
    if ((ret = reg_write(RTL9602C_RX_ERR_CNT_USr, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = reg_write(RTL9602C_TX_EMPTY_EOB_USr, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_usPonnicCounter_get
 * Description:
 *      Get PONNIC packet counters for PBO upstream
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
int32 rtl9602c_raw_pbo_usPonnicCounter_get(rtl9602c_pbo_ponnic_cnt_t *pCnt)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((pCnt == NULL), RT_ERR_INPUT);

    if ((ret = reg_field_read(RTL9602C_PKT_OK_CNT_USr, RTL9602C_TX_OK_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->pkt_ok_cnt_tx = value;
    if ((ret = reg_field_read(RTL9602C_PKT_OK_CNT_USr, RTL9602C_RX_OK_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->pkt_ok_cnt_rx = value;

    if ((ret = reg_field_read(RTL9602C_PKT_ERR_CNT_USr, RTL9602C_TX_ERR_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->pkt_err_cnt_tx = value;
    if ((ret = reg_field_read(RTL9602C_PKT_ERR_CNT_USr, RTL9602C_RX_ERR_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->pkt_err_cnt_rx = value;

    if ((ret = reg_field_read(RTL9602C_PKT_MISS_CNT_USr, RTL9602C_MISS_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->pkt_miss_cnt = value;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_usPonnicCounter_clear
 * Description:
 *      Clear PONNIC packet counters for PBO upstream
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
int32 rtl9602c_raw_pbo_usPonnicCounter_clear(void)
{
    int32   ret;
    uint32  value;

    value = 1;
    if ((ret = reg_field_write(RTL9602C_PKT_OK_CNT_USr, RTL9602C_TX_OK_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9602C_PKT_OK_CNT_USr, RTL9602C_RX_OK_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9602C_PKT_ERR_CNT_USr, RTL9602C_TX_ERR_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9602C_PKT_ERR_CNT_USr, RTL9602C_RX_ERR_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9602C_PKT_MISS_CNT_USr, RTL9602C_MISS_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_usGroupCounter_get
 * Description:
 *      Get PBO packet counters for PBO upstream
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
int32 rtl9602c_raw_pbo_usGroupCounter_get(uint32 group, rtl9602c_pbo_group_cnt_t *pCnt)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((group >= 5), RT_ERR_INPUT);
    RT_PARAM_CHK((pCnt == NULL), RT_ERR_INPUT);

    if ((ret = reg_array_read(RTL9602C_RX_SID_GOOD_CNT_USr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->rx_good_cnt = value;

    if ((ret = reg_array_read(RTL9602C_RX_SID_BAD_CNT_USr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->rx_bad_cnt = value;

    if ((ret = reg_array_read(RTL9602C_TX_SID_CNT_USr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->tx_cnt = value;

    if ((ret = reg_array_read(RTL9602C_TX_SID_FRAG_CNT_USr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->tx_frag_cnt = value;

    if ((ret = reg_array_read(RTL9602C_RX_SID_CNT_USr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->rx_cnt = value;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_usGroupCounter_clear
 * Description:
 *      Clear PBO packet counters for PBO upstream
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
int32 rtl9602c_raw_pbo_usGroupCounter_clear(uint32 group)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((group >= 5), RT_ERR_INPUT);

    value = 1;
    if ((ret = reg_array_write(RTL9602C_RX_SID_GOOD_CNT_USr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    value = 1;
    if ((ret = reg_array_write(RTL9602C_RX_SID_BAD_CNT_USr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    value = 1;
    if ((ret = reg_array_write(RTL9602C_TX_SID_CNT_USr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    value = 1;
    if ((ret = reg_array_write(RTL9602C_TX_SID_FRAG_CNT_USr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    value = 1;
    if ((ret = reg_array_write(RTL9602C_RX_SID_CNT_USr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsCounterGroupMember_add
 * Description:
 *      Add member to counter group for PBO downstream
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
int32 rtl9602c_raw_pbo_dsCounterGroupMember_add(uint32 group, uint32 sid)
{
    int32   ret;
    uint32  value;

    RT_PARAM_CHK((group >= 4), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() < sid), RT_ERR_INPUT);

    value = 1;
    if ((ret = reg_array_field_write(RTL9602C_CNT_MASK_DSr, group, sid, RTL9602C_SID_MASKf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsCounterGroupMember_clear
 * Description:
 *      Clear all member of specific group for PBO downstream
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
int32 rtl9602c_raw_pbo_dsCounterGroupMember_clear(uint32 group)
{
    int32   ret;
    uint32  i;
    uint32  value;

    RT_PARAM_CHK((group >= 4), RT_ERR_INPUT);

    for(i = 0;i < HAL_CLASSIFY_SID_NUM();i++)
    {
        value = 0;
        if ((ret = reg_array_field_write(RTL9602C_CNT_MASK_DSr, group, i, RTL9602C_SID_MASKf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsCounterGroupMember_get
 * Description:
 *      Check if the specifiec SID belong to the specific group for PBO downstream
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
int32 rtl9602c_raw_pbo_dsCounterGroupMember_get(uint32 group, uint32 sid, rtk_enable_t *pEnabled)
{
    int32   ret;
    uint32  value = 0;

    RT_PARAM_CHK((group >= 4), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() < sid), RT_ERR_INPUT);
    RT_PARAM_CHK((pEnabled == NULL), RT_ERR_INPUT);

    if ((ret = reg_array_field_read(RTL9602C_CNT_MASK_DSr, group, sid, RTL9602C_SID_MASKf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pEnabled = (value == 1) ? ENABLED : DISABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsCounter_get
 * Description:
 *      Get PBO packet counters for PBO downstream
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
int32 rtl9602c_raw_pbo_dsCounter_get(rtl9602c_pbo_cnt_t *pCnt)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((pCnt == NULL), RT_ERR_INPUT);

    if ((ret = reg_read(RTL9602C_RX_ERR_CNT_DSr, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->rx_err_cnt = value;
    if ((ret = reg_read(RTL9602C_RX_DROP_CNT_DSr, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->rx_drop_cnt = value;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsCounter_clear
 * Description:
 *      Clear PBO packet counters for PBO downstream
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
int32 rtl9602c_raw_pbo_dsCounter_clear(void)
{
    int ret;
    uint32  value;

    value = 1;
    if ((ret = reg_write(RTL9602C_RX_ERR_CNT_DSr, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 0;
    if ((ret = reg_write(RTL9602C_RX_DROP_CNT_DSr, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsPonnicCounter_get
 * Description:
 *      Get PONNIC packet counters for PBO downstream
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
int32 rtl9602c_raw_pbo_dsPonnicCounter_get(rtl9602c_pbo_ponnic_cnt_t *pCnt)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((pCnt == NULL), RT_ERR_INPUT);

    if ((ret = reg_field_read(RTL9602C_PKT_OK_CNT_DSr, RTL9602C_TX_OK_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->pkt_ok_cnt_tx = value;
    if ((ret = reg_field_read(RTL9602C_PKT_OK_CNT_DSr, RTL9602C_RX_OK_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->pkt_ok_cnt_rx = value;

    if ((ret = reg_field_read(RTL9602C_PKT_ERR_CNT_DSr, RTL9602C_TX_ERR_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->pkt_err_cnt_tx = value;
    if ((ret = reg_field_read(RTL9602C_PKT_ERR_CNT_DSr, RTL9602C_RX_ERR_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->pkt_err_cnt_rx = value;

    if ((ret = reg_field_read(RTL9602C_PKT_MISS_CNT_DSr, RTL9602C_MISS_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->pkt_miss_cnt = value;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsPonnicCounter_clear
 * Description:
 *      Clear PONNIC packet counters for PBO downstream
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
int32 rtl9602c_raw_pbo_dsPonnicCounter_clear(void)
{
    int32   ret;
    uint32  value;

    value = 1;
    if ((ret = reg_field_write(RTL9602C_PKT_OK_CNT_DSr, RTL9602C_TX_OK_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9602C_PKT_OK_CNT_DSr, RTL9602C_RX_OK_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9602C_PKT_ERR_CNT_DSr, RTL9602C_TX_ERR_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9602C_PKT_ERR_CNT_DSr, RTL9602C_RX_ERR_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9602C_PKT_MISS_CNT_DSr, RTL9602C_MISS_CNTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsGroupCounter_get
 * Description:
 *      Get PBO packet counters for PBO downstream
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
int32 rtl9602c_raw_pbo_dsGroupCounter_get(uint32 group, rtl9602c_pbo_group_cnt_t *pCnt)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((group >= 5), RT_ERR_INPUT);
    RT_PARAM_CHK((pCnt == NULL), RT_ERR_INPUT);

    if ((ret = reg_array_read(RTL9602C_RX_SID_GOOD_CNT_DSr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->rx_good_cnt = value;

    if ((ret = reg_array_read(RTL9602C_RX_SID_BAD_CNT_DSr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->rx_bad_cnt = value;

    if ((ret = reg_array_read(RTL9602C_RX_SID_FRAG_CNT_DSr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->rx_frag_cnt = value;

    if ((ret = reg_array_read(RTL9602C_TX_SID_CNT_DSr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    pCnt->tx_cnt = value;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsGroupCounter_clear
 * Description:
 *      Clear PBO packet counters for PBO downstream
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
int32 rtl9602c_raw_pbo_dsGroupCounter_clear(uint32 group)
{
    int32   ret;
    uint32  value;

    /*parameter check*/
    RT_PARAM_CHK((group >= 5), RT_ERR_INPUT);

    value = 1;
    if ((ret = reg_array_write(RTL9602C_RX_SID_GOOD_CNT_DSr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    value = 1;
    if ((ret = reg_array_write(RTL9602C_RX_SID_BAD_CNT_DSr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    value = 1;
    if ((ret = reg_array_write(RTL9602C_RX_SID_FRAG_CNT_DSr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    value = 1;
    if ((ret = reg_array_write(RTL9602C_TX_SID_CNT_DSr, REG_ARRAY_INDEX_NONE, group, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsSidToQueueMap_set
 * Description:
 *      Configure PBO SID to queue mapping for PBO downstream
 * Input:
 *      sid                 - SID to be set
 *      qType               - queue type to be assigned to 
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9602c_raw_pbo_dsSidToQueueMap_set(uint32 sid, rtl9602c_pbo_dsQueue_type_t qType)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() < sid), RT_ERR_INPUT);
    RT_PARAM_CHK((qType >= RTL9602C_PBO_DSQUEUE_TYPE_END), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(RTL9602C_PON_SID_Q_MAP_DSr, REG_ARRAY_INDEX_NONE, sid, RTL9602C_SID_MAPf, &qType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsSidToQueueMap_set
 * Description:
 *      Retrieve PBO SID of queue mapping for PBO downstream
 * Input:
 *      sid                 - SID to be get
 * Output:
 *      pQType              - queue type for the SID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9602c_raw_pbo_dsSidToQueueMap_get(uint32 sid, rtl9602c_pbo_dsQueue_type_t *pQType)
{
    int32   ret;
    uint32 value;

    /*parameter check*/
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() < sid), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pQType), RT_ERR_INPUT);

    if ((ret = reg_array_field_read(RTL9602C_PON_SID_Q_MAP_DSr, REG_ARRAY_INDEX_NONE, sid, RTL9602C_SID_MAPf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pQType = value;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsQueueQos_set
 * Description:
 *      Configure PBO downstream queue qos type
 * Input:
 *      qosType             - qos configuration between queues
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9602c_raw_pbo_dsQueueQos_set(rtl9602c_pbo_dsQueueQos_type_t qosType)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((qosType >= RTL9602C_PBO_DSQUEUEQOS_TYPE_END), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9602C_PON_Q_CONFIG_DSr, RTL9602C_Q_SCHEDULINGf, &qosType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsQueueQos_get
 * Description:
 *      Retrieve PBO downstream queue qos type
 * Input:
 *      none
 * Output:
 *      pQosType            - qos configuration between queues
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9602c_raw_pbo_dsQueueQos_get(rtl9602c_pbo_dsQueueQos_type_t *pQosType)
{
    int32   ret;
    uint32 value;

    /*parameter check*/
    RT_PARAM_CHK((NULL == pQosType), RT_ERR_INPUT);

    if ((ret = reg_field_read(RTL9602C_PON_Q_CONFIG_DSr, RTL9602C_Q_SCHEDULINGf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pQosType = value;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsFcIgnoreThreshold_set
 * Description:
 *      Configure PBO downstream queue qos type
 * Input:
 *      onThreshold         - threshold value to regain switch flow control
 *      offThreshold        - threshold value to ignore switch flow control
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9602c_raw_pbo_dsFcIgnoreThreshold_set(uint32 onThreshold, uint32 offThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < onThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_FLOWCTRL_THRESH_MAX() < offThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9602C_PON_FC_CONFIG_DSr, RTL9602C_FC_OFF_THf, &offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9602C_PON_FC_CONFIG_DSr, RTL9602C_FC_ON_THf, &onThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_pbo_dsFcIgnoreThreshold_get
 * Description:
 *      Configure PBO downstream queue qos type
 * Input:
 *      none
 * Output:
 *      *pOnThreshold       - threshold value to regain switch flow control
 *      *pOffThreshold      - threshold value to ignore switch flow control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtl9602c_raw_pbo_dsFcIgnoreThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold)
{
    int32   ret;
    uint32 onTh, offTh;

    /*parameter check*/
    RT_PARAM_CHK((NULL == pOnThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pOffThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_read(RTL9602C_PON_FC_CONFIG_DSr, RTL9602C_FC_OFF_THf, &offTh)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9602C_PON_FC_CONFIG_DSr, RTL9602C_FC_ON_THf, &onTh)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    *pOffThreshold = offTh;
    *pOnThreshold = onTh;

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_KERNEL_LINUX)
int32 rtl9602c_raw_pbo_memUsage_init(uint32 mode, uint32 usPageSize, uint32 dsPageSize)
{
    int ret;
    uint32 value;
    uint32 us_dram_cnt, us_sram_cnt;
    uint32 us_dram_runt, us_sram_runt;
#if defined(CONFIG_LUNA_PBO_UL_DRAM_OFFSET)
    uint32 us_mem_base = 0;
#endif
    uint32 us_mem_size = 0;
    uint32 ds_dram_cnt, ds_sram_cnt;
    uint32 ds_dram_runt, ds_sram_runt;
#if defined(CONFIG_LUNA_PBO_DL_DRAM_OFFSET)
    uint32 ds_mem_base = 0;
#endif
    uint32 ds_mem_size = 0;
    uint32 sidIdx;
    uint32 sysOnTh, sysOffTh;
    uint32 sidOnTh = RTL9602C_PBO_SIDONTH_DEFAULT / (usPageSize/128), sidOffTh = RTL9602C_PBO_SIDOFFTH_DEFAULT / (usPageSize/128);

#if defined(FPGA_DEFINED)
    /* For FPGA testing, there are only 8Mb x 2 simulated DRAM for upstream and downstream
     * The DSC runout for DRAM shoud reserved at least 32 pages for PONNIC internal 2KB buffer
     * Additional 8 pages are reserved for corner case
     */
    us_mem_size = 1024 * 1024;
    #if PBO_USE_DS_DRAM
    ds_mem_size = 1024 * 1024;
    #else
    ds_mem_size = 0;
    #endif
#elif !PBO_USE_DRAM
    /* No DRAM for use */
#elif defined(CONFIG_LUNA_RESERVE_DRAM_FOR_PBO)
    #if defined(CONFIG_LUNA_PBO_UL_DRAM_SIZE)
    us_mem_size = CONFIG_LUNA_PBO_UL_DRAM_SIZE;
    #endif
    #if defined(CONFIG_LUNA_PBO_DL_DRAM_SIZE)
    ds_mem_size = CONFIG_LUNA_PBO_DL_DRAM_SIZE;
    #endif
    #if defined(CONFIG_LUNA_PBO_UL_DRAM_OFFSET)
        #if defined(CONFIG_LUNA_MEMORY_AUTO_DETECTION)
        extern uint32 LUNA_PBO_UL_DRAM_OFFSET;
        us_mem_base = LUNA_PBO_UL_DRAM_OFFSET;
        #else
        us_mem_base = CONFIG_LUNA_PBO_UL_DRAM_OFFSET;
        #endif
    #endif
    #if defined(CONFIG_LUNA_PBO_DL_DRAM_OFFSET)
        #if defined(CONFIG_LUNA_MEMORY_AUTO_DETECTION)
        extern uint32 LUNA_PBO_DL_DRAM_OFFSET;
        ds_mem_base = LUNA_PBO_DL_DRAM_OFFSET;
        #else
        ds_mem_base = CONFIG_LUNA_PBO_DL_DRAM_OFFSET;
        #endif
    #endif

    if ((ret = reg_field_write(RTL9602C_IP_MSTBASE_USr, RTL9602C_CFG_PON_MSTBASEf, &us_mem_base)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9602C_IP_MSTBASE_DSr, RTL9602C_CFG_PON_MSTBASEf, &ds_mem_base)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
#else
    /* No CONFIG_LUNA_RESERVE_DRAM_FOR_PBO, don't use DRAM */
#endif

    /* Configure upstream/downstream SRAM count */
    us_sram_cnt = (128 * 128 / usPageSize) - 1;
    if ((ret = reg_field_write(RTL9602C_PON_DSC_CFG_USr, RTL9602C_CFG_SRAM_NOf, &us_sram_cnt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    ds_sram_cnt = (32 * 128 / dsPageSize) - 1;
    if ((ret = reg_field_write(RTL9602C_PON_DSC_CFG_DSr, RTL9602C_CFG_SRAM_NOf, &ds_sram_cnt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* Configure upstream/downstream DRAM count */
    if(0 != us_mem_size)
    {
        us_dram_cnt = us_mem_size / usPageSize;
        us_dram_cnt = (us_dram_cnt > RTL9602C_PBO_PAGE_CNT_US_MAX) ? (RTL9602C_PBO_PAGE_CNT_US_MAX - 1) : (us_dram_cnt - 1);
    }
    else
    {
        /* If no DRAM being used, the total descriptor number is SRAM number */
        us_dram_cnt = us_sram_cnt;
    }
    if ((ret = reg_field_write(RTL9602C_PON_DSC_CFG_USr, RTL9602C_CFG_RAM_NOf, &us_dram_cnt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if(0 != ds_mem_size)
    {
        ds_dram_cnt = ds_mem_size / dsPageSize;
        ds_dram_cnt = (ds_dram_cnt > RTL9602C_PBO_PAGE_CNT_DS_MAX) ? (RTL9602C_PBO_PAGE_CNT_DS_MAX - 1) : (ds_dram_cnt - 1);
    }
    else
    {
        /* If no DRAM being used, the total descriptor number is SRAM number */
        ds_dram_cnt = ds_sram_cnt;
    }
    if ((ret = reg_field_write(RTL9602C_PON_DSC_CFG_DSr, RTL9602C_CFG_RAM_NOf, &ds_dram_cnt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* Configure upstream/downstream SRAM DSC runout */
    us_sram_runt = us_sram_cnt - 1;
    if ((ret = reg_field_write(RTL9602C_DSCRUNOUT_USr, RTL9602C_CFG_SRAM_RUNOUTf, &us_sram_runt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    ds_sram_runt = ds_sram_cnt - 1;
    if ((ret = reg_field_write(RTL9602C_DSCRUNOUT_DSr, RTL9602C_CFG_SRAM_RUNOUTf, &ds_sram_runt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* Configure upstream/downstream DRAM DSC runout */
    if(0 == us_mem_size)
    {
        /* If no DRAM being used, the descriptor runout should be 0 */
        us_dram_runt = 0;
    }
    else
    {
        us_dram_runt = (us_mem_size / usPageSize) - (32 + 8) - (128 * 128 / usPageSize);
    }
    if ((ret = reg_field_write(RTL9602C_DSCRUNOUT_USr, RTL9602C_CFG_DRAM_RUNOUTf, &us_dram_runt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if(0 != ds_mem_size)
    {
        ds_dram_runt = (ds_mem_size / dsPageSize) - (32 + 8) - (32 * 128 / dsPageSize);
    }
    else
    {
        /* If no DRAM being used, the descriptor runout should be 0 */
        ds_dram_runt = 0;
    }
    if ((ret = reg_field_write(RTL9602C_DSCRUNOUT_DSr, RTL9602C_CFG_DRAM_RUNOUTf, &ds_dram_runt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if(0 == us_mem_size)
    {
        value = us_sram_runt - 1;
        if((ret = rtl9602c_raw_pbo_stopAllThreshold_set(value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }
        if((ret = rtl9602c_raw_pbo_globalThreshold_set(us_sram_runt - 1, us_sram_runt - 3)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }
    }
    else
    {
        value = us_dram_runt - (32 + 8);
        if((ret = rtl9602c_raw_pbo_stopAllThreshold_set(value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }

        switch(usPageSize)
        {
        case 128:
            sysOnTh = RTL9602C_PBO_SYS_CNT_128_MAX;
            break;
        case 256:
            sysOnTh = RTL9602C_PBO_SYS_CNT_256_MAX;
            break;
        case 512:
            sysOnTh = RTL9602C_PBO_SYS_CNT_512_MAX;
            break;
        default:
            return RT_ERR_INPUT;
        }
        if(sysOnTh > (us_dram_runt - (32 + 8) * 3))
        {
            sysOnTh = (us_dram_runt - (32 + 8) * 3);
        }
        if(PBO_MODE_GPON != mode)
        {
            sysOnTh -= 8 * sidOnTh;
        }
        sysOffTh = sysOnTh - (32 + 8) * 4;
        if((ret = rtl9602c_raw_pbo_globalThreshold_set(sysOnTh, sysOffTh)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }
    }
    /* Update per SID US flow control threshold */
    for(sidIdx = 0 ; sidIdx < HAL_CLASSIFY_SID_NUM() ; sidIdx ++)
    {
        if((ret = rtl9602c_raw_pbo_sidThreshold_set(sidIdx, sidOnTh, sidOffTh) != RT_ERR_OK))
        {
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }
    }

    if(0 == ds_mem_size)
    {
        if((ret = rtl9602c_raw_pbo_dsFcIgnoreThreshold_set(ds_sram_runt - (20 + 8), ds_sram_runt - 8)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }
    }
    else
    {
        if((ret = rtl9602c_raw_pbo_dsFcIgnoreThreshold_set(ds_sram_runt + ds_dram_runt - (32 + 32 + 100), ds_sram_runt + ds_dram_runt - (32 + 100))) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }
    }

#if defined(CONFIG_LUNA_PBO_UL_DRAM_OFFSET)
    memUsage.us_base = us_mem_base;
#endif
    memUsage.us_offset = us_mem_size;
    memUsage.us_page_size = usPageSize;
    memUsage.us_page_cnt = us_dram_cnt;

#if defined(CONFIG_LUNA_PBO_DL_DRAM_OFFSET)
    memUsage.ds_base = ds_mem_base;
#endif
    memUsage.ds_offset = ds_mem_size;
    memUsage.ds_page_size = dsPageSize;
    memUsage.ds_page_cnt = ds_dram_cnt;

    return RT_ERR_OK;
}
#endif

/* Function Name:
 *      dal_rtl9602c_pbo_init
 * Description:
 *      Inititalize PBO
 * Input:
 *      mode        - EPON/GPON mode
 *      usPageSize  - Upstream page size
 *      dsPageSize  - Downstream page size
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 dal_rtl9602c_pbo_init(rtk_pbo_initParam_t initParam)
{
    int32   ret;
    uint32  value;
    uint32  usPboSize,dsPboSize;
    uint32  usPonnicSize,dsPonnicSize;

    /*parameter check*/
    RT_PARAM_CHK((initParam.mode >= PBO_MODE_END), RT_ERR_INPUT);
    switch(initParam.usPageSize)
    {
    case 128:
        usPboSize = 0;
        usPonnicSize = 0;
        break;
    case 256:
        usPboSize = 1;
        usPonnicSize = 1;
        break;
    case 512:
        usPboSize = 2;
        usPonnicSize = 2;
        break;
    default:
        return RT_ERR_INPUT;
    }

    switch(initParam.dsPageSize)
    {
    case 128:
        dsPboSize = 0;
        dsPonnicSize = 0;
        break;
    case 256:
        dsPboSize = 1;
        dsPonnicSize = 1;
        break;
    case 512:
        dsPboSize = 2;
        dsPonnicSize = 2;
        break;
    default:
        return RT_ERR_INPUT;
    }

#if defined(CONFIG_SDK_KERNEL_LINUX)
    /* Disable PONNIC & PBO first */
    value = 0;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_USr, RTL9602C_GMII_RX_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 0;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_USr, RTL9602C_GMII_TX_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 0;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_DSr, RTL9602C_GMII_RX_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 0;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_DSr, RTL9602C_GMII_TX_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = rtl9602c_raw_pbo_usState_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = rtl9602c_raw_pbo_dsState_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = rtl9602c_raw_pbo_memUsage_init(initParam.mode, initParam.usPageSize, initParam.dsPageSize)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
#else
    return RT_ERR_NOT_ALLOWED;
#endif

#if 0//defined(FPGA_DEFINED)
    /* For FPGA testing, there are only 8Mb x 2 simulated DRAM for upstream and downstream
     * The DSC runout for DRAM shoud reserved at least 32 pages for PONNIC internal 2KB buffer
     * Additional 8 pages are reserved for corner case
     */
    /* Configure upstream/downstream SRAM count */
    value = (128 * 128 / usPageSize) - 1;
    if ((ret = reg_field_write(RTL9602C_PON_DSC_CFG_USr, RTL9602C_CFG_SRAM_NOf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = (32 * 128 / dsPageSize) - 1;
    if ((ret = reg_field_write(RTL9602C_PON_DSC_CFG_DSr, RTL9602C_CFG_SRAM_NOf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* Configure upstream/downstream DRAM count */
    value = (1024 * 1024 / usPageSize) - 1;
    if ((ret = reg_field_write(RTL9602C_PON_DSC_CFG_USr, RTL9602C_CFG_RAM_NOf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
#if PBO_USE_DS_DRAM
    value = (1024 * 1024 / dsPageSize) - 1;
#else
    /* If no DRAM being used, the total descriptor number is SRAM number */
    value = (32 * 128 / dsPageSize) - 1;
#endif
    if ((ret = reg_field_write(RTL9602C_PON_DSC_CFG_DSr, RTL9602C_CFG_RAM_NOf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* Configure upstream/downstream SRAM DSC runout */
    value = (128 * 128 / usPageSize) - 2;
    if ((ret = reg_field_write(RTL9602C_DSCRUNOUT_USr, RTL9602C_CFG_SRAM_RUNOUTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = (32 * 128 / dsPageSize) - 2;
    if ((ret = reg_field_write(RTL9602C_DSCRUNOUT_DSr, RTL9602C_CFG_SRAM_RUNOUTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* Configure upstream/downstream DRAM DSC runout */
    value = (1024 * 1024 / usPageSize) - (32 + 8) - (128 * 128 / usPageSize);
    if ((ret = reg_field_write(RTL9602C_DSCRUNOUT_USr, RTL9602C_CFG_DRAM_RUNOUTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
#if PBO_USE_DS_DRAM
    value = (1024 * 1024 / dsPageSize) - (32 + 8) - (32 * 128 / dsPageSize);
#else
    /* If no DRAM being used, the descriptor runout should be 0 */
    value = 0;
#endif
    if ((ret = reg_field_write(RTL9602C_DSCRUNOUT_DSr, RTL9602C_CFG_DRAM_RUNOUTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = (1024 * 1024 / usPageSize) - ((32 + 8) * 2) - (128 * 128 / usPageSize);
    if ((ret = rtl9602c_raw_pbo_stopAllThreshold_set(value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = rtl9602c_raw_pbo_globalThreshold_set((1024 * 1024 / usPageSize) - ((32 + 8) * 4) - (128 * 128 / usPageSize), (1024 * 1024 / usPageSize) - ((32 + 8) * 8) - (128 * 128 / usPageSize))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
#endif

    /* Configure GPON/EPON mode before enable PBO */
    if(initParam.mode == PBO_MODE_EPON)
    {
        value = 1;
    }
    else
    {
        value = 0;
    }
    if ((ret = reg_field_write(RTL9602C_PONIP_CTL_USr, RTL9602C_CFG_EPON_MODEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9602C_PONIP_CTL_DSr, RTL9602C_CFG_EPON_MODEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 1;
    if ((ret = reg_field_write(RTL9602C_PONIP_CTL_USr, RTL9602C_CFG_STOP_RXC_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 1;
    if ((ret = reg_field_write(RTL9602C_PON_US_FIFO_CTLr, RTL9602C_CFG_USFIFO_SPACEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 3;
    if ((ret = reg_field_write(RTL9602C_PON_US_FIFO_CTLr, RTL9602C_CFG_USFIFO_STARTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* Set upstream page size as configured */
    value = usPboSize;
    if ((ret = reg_field_write(RTL9602C_PON_DSC_CFG_USr, RTL9602C_CFG_PAGE_SIZEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    /* Set downstream page size as configured */
    value = dsPboSize;
    if ((ret = reg_field_write(RTL9602C_PON_DSC_CFG_DSr, RTL9602C_CFG_PAGE_SIZEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* Set PONNIC upstream page size as configured */
    value = usPonnicSize;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_1_USr, RTL9602C_R_RPAGE_SIZEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = usPonnicSize;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_1_USr, RTL9602C_R_TPAGE_SIZEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* Set PONNIC downstream page size as configured */
    value = dsPonnicSize;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_1_DSr, RTL9602C_R_RPAGE_SIZEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = dsPonnicSize;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_1_DSr, RTL9602C_R_TPAGE_SIZEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* Enable upstream/downstream PBO */
    if ((ret = rtl9602c_raw_pbo_usState_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = rtl9602c_raw_pbo_dsState_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* Configure PON NIC */

    if ((ret = reg_field_read(RTL9602C_PROBE_SELECT_USr, RTL9602C_R_DBG_FUNC_SELf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value |= (1 << 1);
    if ((ret = reg_field_write(RTL9602C_PROBE_SELECT_USr, RTL9602C_R_DBG_FUNC_SELf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9602C_PROBE_SELECT_DSr, RTL9602C_R_DBG_FUNC_SELf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value |= (1 << 1);
    if ((ret = reg_field_write(RTL9602C_PROBE_SELECT_DSr, RTL9602C_R_DBG_FUNC_SELf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* PONNIC Upstream */
    value = 1;
    if ((ret = reg_field_write(RTL9602C_CFG_USr, RTL9602C_E_EN_RFF_AFULLf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 1;
    if ((ret = reg_field_write(RTL9602C_CFG_USr, RTL9602C_EN_TX_STOPf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 1;
    if ((ret = reg_field_write(RTL9602C_CFG_USr, RTL9602C_EN_TXE_EXTRAf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* PONNIC Downstream */
    value = 1;
    if ((ret = reg_field_write(RTL9602C_CFG_DSr, RTL9602C_E_EN_RFF_AFULLf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 1;
    if ((ret = reg_field_write(RTL9602C_CFG_DSr, RTL9602C_EN_TX_STOPf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 1;
    if ((ret = reg_field_write(RTL9602C_CFG_DSr, RTL9602C_EN_TXE_EXTRAf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* PONNIC Upstream */
    value = 3;
    if ((ret = reg_field_write(RTL9602C_TX_CFG_USr, RTL9602C_IFGf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 1;
    if ((ret = reg_field_write(RTL9602C_TX_CFG_USr, RTL9602C_R_PREAMBLE_LENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 1;
    if ((ret = reg_field_write(RTL9602C_TX_CFG_USr, RTL9602C_R_TX_PADDINGf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* Accept CRC error for all US from switch */
    value = 1;
    if ((ret = reg_field_write(RTL9602C_RX_CFG_USr, RTL9602C_AERf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* PONNIC Downstream */
    value = 3;
    if ((ret = reg_field_write(RTL9602C_TX_CFG_DSr, RTL9602C_IFGf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 1;
    if ((ret = reg_field_write(RTL9602C_TX_CFG_DSr, RTL9602C_R_PREAMBLE_LENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 1;
    if ((ret = reg_field_write(RTL9602C_TX_CFG_DSr, RTL9602C_R_TX_PADDINGf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* Accept CRC error for all DS from EPON/GPON */
    value = 1;
    if ((ret = reg_field_write(RTL9602C_RX_CFG_DSr, RTL9602C_AERf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* PONNIC Upstream */
    value = 1;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_USr, RTL9602C_MAX_DMA_SEL_0f, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 0;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_USr, RTL9602C_MAX_DMA_SEL_1f, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 1;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_USr, RTL9602C_EARLY_TX_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 2;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_USr, RTL9602C_TX_FIFO_THRf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 2;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_USr, RTL9602C_RX_FIFO_THRf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 1;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_USr, RTL9602C_RX_MAX_DMA_SELf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 1;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_USr, RTL9602C_GMII_RX_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 1;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_USr, RTL9602C_GMII_TX_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* PONNIC Downstream */
    value = 1;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_DSr, RTL9602C_MAX_DMA_SEL_0f, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 0;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_DSr, RTL9602C_MAX_DMA_SEL_1f, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 1;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_DSr, RTL9602C_EARLY_TX_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 1;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_DSr, RTL9602C_TX_FIFO_THRf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 2;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_DSr, RTL9602C_RX_FIFO_THRf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 1;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_DSr, RTL9602C_RX_MAX_DMA_SELf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 1;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_DSr, RTL9602C_GMII_RX_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    value = 1;
    if ((ret = reg_field_write(RTL9602C_IO_CMD_0_DSr, RTL9602C_GMII_TX_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
}


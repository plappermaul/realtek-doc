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
 * $Revision: 46542 $
 * $Date: 2012-08-07
 *
 * Purpose : GPON MAC register access APIs
 *
 * Feature : Provide the APIs to access GPON MAC
 *
 */


#include <common/rt_type.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd.h>
#include <osal/lib.h>
#include <hal/common/miim.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd_gpon.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd_ponmac.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd_pbo.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd_switch.h>
#include <osal/time.h>
#include <ioal/mem32.h>
#include <rtk/gpio.h>
#include <rtk/ponmac.h>



#define GPON_NO_CHECK                               0
#define WAIT_ZERO                                   4096
#define WAIT_FOREVER                                (0xFFFFFFFF - 1)
#define WAIT_MAX_TIMES                              WAIT_FOREVER

#define GPON_REG_PTN_GTC_US_WRPROTECT_ENA           0xCC19
#define GPON_REG_PTN_GTC_US_WRPROTECT_DIS           0x0000
#define GPON_REG_EQD_FRAME_LEN                      (19440*8)

#define GPON_REG_TYPE_GTC_US_PLOAM_NRM              0x0
#define GPON_REG_TYPE_GTC_US_PLOAM_URG              0x1
#define GPON_REG_TYPE_GTC_US_PLOAM_DYINGGASP        0x5
#define GPON_REG_TYPE_GTC_US_PLOAM_SN               0x6
#define GPON_REG_TYPE_GTC_US_PLOAM_NOMSG            0x7

#define GPON_REG_BITS_INDIRECT_ACK                  15
#define GPON_REG_BITS_CAM_OP_IDX                    0
#define GPON_REG_BITS_CAM_OP_MODE                   8
#define GPON_REG_BITS_CAM_OP_HIT                    13
#define GPON_REG_BITS_CAM_OP_COMPLETE               14
#define GPON_REG_BITS_CAM_OP_REQ                    15

#define GPON_REG_STATUS_CLEAR                       0
#define GPON_REG_STATUS_SET                         1
#define GPON_REG_16BITS_LOW                         0
#define GPON_REG_16BITS_HIGH                        8

#define GPON_REG_OPERRATION_NO                      0
#define GPON_REG_OPERRATION_WRITE                   1
#define GPON_REG_OPERRATION_READ                    2
#define GPON_REG_OPERRATION_CLEAN                   3


static uint32 gpon_init = { INIT_NOT_COMPLETED } ;

typedef enum gpon_raw_IntrType_e{
    GPON_RAW_INTR_GEM_US =0,
    GPON_RAW_INTR_GTC_US,
    GPON_RAW_INTR_GEM_DS,
    GPON_RAW_INTR_AES_DEC,
    GPON_RAW_INTR_GTC_DS_CAP,
    GPON_RAW_INTR_GTC_DS,
    GPON_RAW_INTR_END
}gpon_raw_IntrType_t;


typedef enum gpon_raw_gtcDsIntrType_e{
    GPON_RAW_GTC_DS_INTR=0,
    GPON_RAW_PLM_BUF_REQ,
    GPON_RAW_RNG_REQ_HIS,
    GPON_RAW_SN_REQ_HIS,
    GPON_RAW_LOM_DLT,
    GPON_RAW_DS_FEC_STS_DLT,
    GPON_RAW_LOF_DLT,
    GPON_RAW_LOS_DLT,
    GPON_RAW_GTC_DS_INTR_END
}gpon_raw_gtcDsIntrType_t;

static uint32 gpon_intr_field[]={
    RTL9603CVD_GEM_US_INTRf,
    RTL9603CVD_GTC_US_INTRf,
    RTL9603CVD_GEM_DS_INTRf,
    RTL9603CVD_AES_DECRYPT_INTRf,
    RTL9603CVD_GTC_DS_CAP_INTRf,
    RTL9603CVD_GTC_DS_INTRf,
};

static uint32 gpon_intrMask_field[]={
    RTL9603CVD_GEM_US_Mf,
    RTL9603CVD_GTC_US_Mf,
    RTL9603CVD_GEM_DS_Mf,
    RTL9603CVD_AES_DECRYPT_Mf,
    RTL9603CVD_GTC_DS_CAP_Mf,
    RTL9603CVD_GTC_DS_Mf,
};


static uint32 gpon_gtcDsIntr_field[] = {
    0,
    0,
    0,
    0,
    RTL9603CVD_LOMf,
    RTL9603CVD_DS_FEC_STSf,
    RTL9603CVD_LOFf,
    RTL9603CVD_LOSf,
    0,
};

static uint32 gpon_gtcDsIntrIndicator_field[] = {
    RTL9603CVD_GTC_DS_INTRf,
    RTL9603CVD_PLM_BUF_REQf,
    RTL9603CVD_RNG_REQ_HISf,
    RTL9603CVD_SN_REQ_HISf,
    RTL9603CVD_LOM_DLTf,
    RTL9603CVD_DS_FEC_STA_DLTf,
    RTL9603CVD_LOF_DLTf,
    RTL9603CVD_LOS_DLTf,
    RTL9603CVD_PPS_DLTf,
};

static uint32 gpon_gtcDsIntrMask_field[] = {
    RTL9603CVD_GTC_DS_Mf,
    RTL9603CVD_PLM_BUF_Mf,
    RTL9603CVD_RNG_REQ_Mf,
    RTL9603CVD_SN_REQ_Mf,
    RTL9603CVD_LOM_Mf,
    RTL9603CVD_DS_FEC_STA_Mf,
    RTL9603CVD_LOF_Mf,
    RTL9603CVD_LOS_Mf,
    RTL9603CVD_PPS_Mf,
};

static uint32 gpon_gtcUsIntr_field[] = {
    0,
    0,
    0,
    0,
    0,
    RTL9603CVD_US_FEC_STSf,
    0,
};



static uint32 gpon_gtcUsIntrIndicator_field[] = {
    RTL9603CVD_GTC_US_INTRf,
    RTL9603CVD_OPTIC_SD_MISM_DLTf,
    RTL9603CVD_OPTIC_SD_TOOLONG_DLTf,
    RTL9603CVD_PLM_URG_EMPTY_DLTf,
    RTL9603CVD_PLM_NRM_EMPTY_DLTf,
    RTL9603CVD_US_FEC_STS_DLTf,
    RTL9603CVD_DG_MSG_TX_DLTf,
};

static uint32 gpon_gtcUsIntrMask_field[] = {
    0,
    RTL9603CVD_OPTIC_SD_MISM_Mf,
    RTL9603CVD_OPTIC_SD_TOOLONG_Mf,
    RTL9603CVD_PLM_URG_EMPTY_Mf,
    RTL9603CVD_PLM_NRM_EMPTY_Mf,
    RTL9603CVD_US_FEC_STS_Mf,
    RTL9603CVD_DG_MSG_TX_Mf,
};

static uint32 gpon_gemUsIntrIndicator_field[] = {
    RTL9603CVD_GEM_US_INTRf,
    RTL9603CVD_SD_VALID_LONG_DLTf,
    RTL9603CVD_SD_DIFF_HUGE_DLTf,
    RTL9603CVD_REQUEST_DELAY_DLTf,
    RTL9603CVD_BC_LESS6_DLTf,
    RTL9603CVD_ERR_PLI_DLTf,
    RTL9603CVD_BURST_TM_LARGER_GTC_DLTf,
    RTL9603CVD_BANK_TOO_MUCH_AT_END_DLTf,
    RTL9603CVD_BANK_REMAIN_AFRD_DLTf,
    RTL9603CVD_BANK_OVERFL_DLTf,
    RTL9603CVD_BANK_UNDERFL_DLTf
};

static uint32 gpon_gemUsIntrMask_field[] = {
    0,
    RTL9603CVD_SD_VALID_LONG_Mf,
    RTL9603CVD_SD_DIFF_HUGE_Mf,
    RTL9603CVD_REQUEST_DELAY_Mf,
    RTL9603CVD_BC_LESS6_Mf,
    RTL9603CVD_ERR_PLI_Mf,
    RTL9603CVD_BURST_TM_LARGER_GTC_Mf,
    RTL9603CVD_BANK_TOO_MUCH_AT_END_Mf,
    RTL9603CVD_BANK_REMAIN_AFRD_Mf,
    RTL9603CVD_BANK_OVERFL_Mf,
    RTL9603CVD_BANK_UNDERFL_Mf
};

static uint32 gpon_gemUsIntrStatus_field[] = {
    0,
    RTL9603CVD_SD_VALID_LONG_INDf,
    RTL9603CVD_SD_DIFF_HUGE_INDf,
    RTL9603CVD_REQUEST_DELAY_INDf,
    RTL9603CVD_BC_LESS6_INDf,
    RTL9603CVD_ERR_PLI_INDf,
    RTL9603CVD_BURST_TM_LARGER_GTC_INDf,
    RTL9603CVD_BANK_TOO_INDUCH_AT_END_INDf,
    RTL9603CVD_BANK_REMAIN_AFRD_INDf,
    RTL9603CVD_BANK_OVERFL_INDf,
    RTL9603CVD_BANK_UNDERFL_INDf
};

static uint32 sysOnTh, sysOffTh;

/*Defien local function*/

static int32
gpon_macfilterMode_cfg2raw(rtk_gpon_macTable_exclude_mode_t type, uint32 *raw)
{
    switch(type){
    case RTK_GPON_MACTBL_MODE_INCLUDE:
        *raw = 0;
    break;
    case RTK_GPON_MACTBL_MODE_EXCLUDE:
        *raw =1 ;
    break;
    default:
        return RT_ERR_FAILED;
    break;
    }
    return RT_ERR_OK;
}


static int32
gpon_macfilterMode_raw2cfg(uint32 raw, rtk_gpon_macTable_exclude_mode_t *cfg)
{
    switch(raw){
    case 0:
        *cfg = RTK_GPON_MACTBL_MODE_INCLUDE;
    break;
    case 1:
        *cfg = RTK_GPON_MACTBL_MODE_EXCLUDE;
    break;
    default:
        return RT_ERR_FAILED;
    break;
    }
    return RT_ERR_OK;
}


static int32
gpon_gtcUsBurstPolarity_cfg2raw(rtk_gpon_polarity_t polarity,uint32 *raw)
{
    switch(polarity){
    case GPON_POLARITY_LOW:
        *raw = 0;
    break;
    case GPON_POLARITY_HIGH:
        *raw = 1;
    break;
    default:
        return RT_ERR_FAILED;
    break;
    }
    return RT_ERR_OK;
}

static int32
gpon_usGtcMiscCntType_cfg2raw(rtk_gpon_usGtc_pmMiscType_t type,uint32 *raw)
{
    switch(type){
    case GPON_REG_USGTCPMMISC_PLOAM_BOH_TX:
        *raw = 0;
    break;
        case GPON_REG_USGTCPMMISC_GEM_DBRU_TX:
        *raw = 1;
    break;
        case GPON_REG_USGTCPMMISC_PLOAM_CPU_TX:
        *raw = 2;
    break;
        case GPON_REG_USGTCPMMISC_PLOAM_AUTO_TX:
        *raw = 3;
    break;
        case GPON_REG_USGTCPMMISC_GEM_BYTE_TX:
        *raw = 4;
    break;
    default:
        return RT_ERR_FAILED;
    break;
    }
    return RT_ERR_OK;
}


static void
gpon_delay(uint32 step)
{
    uint32 delay = step;
    while(delay)
    {
        delay--;
    }
}


static uint32
gpon_get_bit(uint32 value, uint32 bit)
{
    return ((value>>bit)&0x01);
}



static int32
gpon_indirect_wait(uint32 reg)
{
#if GPON_NO_CHECK /*defined(CONFIG_SDK_ASICDRV_TEST)*/ /* for init pass, scott */
    gpon_delay(WAIT_ZERO);
    return TRUE;
#else
    uint32 timeout;
    uint32 ind;
    int32  ret;

    for(timeout=0;timeout<=WAIT_MAX_TIMES;timeout++)
    {
        if((ret = reg_read(reg, &ind))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return FALSE;
        }
        if(gpon_get_bit(ind,GPON_REG_BITS_INDIRECT_ACK))
        {
            return TRUE;
        }
        /*osal_printf("apollo_raw_gpon_indirect_wait %d\n\r",timeout);*/
    }
    return FALSE;
#endif
}


static int32
gpon_cam_wait(uint32 reg, int32 waithit)
{
#if GPON_NO_CHECK /*defined(CONFIG_SDK_ASICDRV_TEST)*/ /* for init pass, scott */
    gpon_delay(WAIT_ZERO);
    return TRUE;
#else
    uint32 timeout;
    uint32 ind;
    int32  ret;

    for(timeout=0;timeout<=WAIT_MAX_TIMES;timeout++)
    {
        if((ret = reg_read(reg, &ind))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return FALSE;
        }
        if(gpon_get_bit(ind,GPON_REG_BITS_CAM_OP_COMPLETE))
        {
            if(!waithit || gpon_get_bit(ind,GPON_REG_BITS_CAM_OP_HIT))
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
        /*osal_printf("apollo_raw_gpon_cam_wait %d\n\r",timeout);*/
    }
      return FALSE;
#endif
}


static void
gpon_txPtn_write(uint32 fg_ptn, uint32 bg_ptn)
{
    uint32 tmp;
    int32  ret;

    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
    }

    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_TX_PATTERN_FGr, RTL9603CVD_TX_PATTERN_FGf, &fg_ptn))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
    }
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_TX_PATTERN_BGr, RTL9603CVD_TX_PATTERN_BGf, &bg_ptn))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
    }

    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
    }
}

static int32
gpon_txPtn_read(uint32 *pFg_ptn, uint32 *pBg_ptn)
{
    uint32 ret;

    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_US_TX_PATTERN_FGr, RTL9603CVD_TX_PATTERN_FGf, pFg_ptn)) != RT_ERR_OK)
        return ret;

    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_US_TX_PATTERN_BGr, RTL9603CVD_TX_PATTERN_BGf, pBg_ptn)) != RT_ERR_OK)
        return ret;

    return ret;
}

static void
gpon_txPtnMode_write(rtk_gpon_tx_ptn_mode_t fg_mode, rtk_gpon_tx_ptn_mode_t bg_mode)
{
    uint32 no_fg;
    uint32 data=0;
    uint32 tmp;
    int32  ret;

    if(fg_mode == RTK_GPON_TX_PTN_MODE_NONE)
    {
        no_fg = 1;
        fg_mode = 0;
    }
    else
    {
        no_fg = 0;
    }

    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_US_TX_PATTERN_CTLr, RTL9603CVD_TX_PATTERN_MODE_NO_FGf, &no_fg, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
    }
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_US_TX_PATTERN_CTLr, RTL9603CVD_TX_PATTERN_MODE_FGf, &fg_mode, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
    }
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_US_TX_PATTERN_CTLr, RTL9603CVD_TX_PATTERN_MODE_BGf, &bg_mode, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
    }

    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
    }

    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_TX_PATTERN_CTLr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
    }

    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
    }
}

static int32
gpon_txPtnMode_read(rtk_gpon_tx_ptn_mode_t *pFg_mode, rtk_gpon_tx_ptn_mode_t *pBg_mode)
{
    uint32 val, data;
    uint32 ret;

    if((ret = reg_read(RTL9603CVD_GPON_GTC_US_TX_PATTERN_CTLr, &data)) != RT_ERR_OK)
        return ret;

    if((ret = reg_field_get(RTL9603CVD_GPON_GTC_US_TX_PATTERN_CTLr, RTL9603CVD_TX_PATTERN_MODE_BGf, &val, &data)) != RT_ERR_OK)
        return ret;

    *pBg_mode = val;

    if((ret = reg_field_get(RTL9603CVD_GPON_GTC_US_TX_PATTERN_CTLr, RTL9603CVD_TX_PATTERN_MODE_NO_FGf, &val, &data)) != RT_ERR_OK)
        return ret;

    if(val == 1)
        *pFg_mode = RTK_GPON_TX_PTN_MODE_NONE;
    else
    {
        if((ret = reg_field_get(RTL9603CVD_GPON_GTC_US_TX_PATTERN_CTLr, RTL9603CVD_TX_PATTERN_MODE_FGf, &val, &data)) != RT_ERR_OK)
            return ret;
        *pFg_mode = val;
    }

    return ret;
}


/* Function Name:
 *      dal_rtl9603cvd_gpon_init
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
int32  dal_rtl9603cvd_gpon_init(void)
{
    int32 ret;
    uint32 data;
    int32 i;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    gpon_init = INIT_COMPLETED;

    /* parameter check */
    /* function body */
    if((ret = dal_rtl9603cvd_gpon_resetState_set(ENABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        gpon_init = INIT_NOT_COMPLETED;

        return RT_ERR_FAILED;
    }
    if((ret = dal_rtl9603cvd_gpon_resetState_set(DISABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        gpon_init = INIT_NOT_COMPLETED;

        return RT_ERR_FAILED;
    }
    data = 0;
    for(i=0;i<0xffff;i++)
    {
        if((ret = dal_rtl9603cvd_gpon_resetDoneState_get(&data))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            gpon_init = INIT_NOT_COMPLETED;

            return RT_ERR_GPON_INITIAL_FAIL;
        }
        if(1 == data)
            break;
    }
    if(0 == data)
    {
        osal_printf("gpon mac reset fail\n\r");
        gpon_init = INIT_NOT_COMPLETED;
        return RT_ERR_GPON_INITIAL_FAIL;
    }

#if !defined(FPGA_DEFINED)
    /* set NIC register, OMCI sid is 127 for 9603D */
    (*(volatile uint32*)(0xB8012050)) &= ~(0x7F << 8);
    (*(volatile uint32*)(0xB8012050)) |= (RTL9603CVD_GPON_OMCI_FLOW_ID << 8);
#endif

    /* set threshold for HEC error patch , GPON MAC page 6 offset 0x98 */
    data = 40;
    if((ret = reg_field_write(RTL9603CVD_GPON_GEM_US_EOB_MERGEr,RTL9603CVD_EOB_MERGE_THRESHf,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    /* adjust the min_delay1 and min_delay2 */
    data = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

#if HW_OLT_DISTANCE_ACCURATE
    data = 284;
#else
#if ZTE_OLT_DISTANCE_ACCURATE
    data = 298;
#else
    data = 290;
#endif
#endif

    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_MIN_DELAYr,RTL9603CVD_MIN_DELAY1f,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    data = 50;
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_MIN_DELAYr,RTL9603CVD_MIN_DELAY2f,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    data = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    /* DBRu period */
    if((ret = dal_rtl9603cvd_gpon_dbruPeriod_set(2))!=RT_ERR_OK)
    {
          RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
              return ret;
    }

    /* Dbru report block size set to 48 byte */
    if ((ret = dal_rtl9603cvd_gpon_dbruBlockSize_set(48)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }

    /* Because laser driver TX_SD delay, adjust missmatch threshold to 0xa0 */
    data = 0xa0;
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_OPTIC_SD_THr,RTL9603CVD_OPTIC_SD_MISM_THREHf,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    /* Because laser driver TX_SD delay, TX_SD maybe merge to next burst, adjust toolong threshold to 0x7fff */
    data = 0x7fff;
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_OPTIC_SD_THr,RTL9603CVD_OPTIC_SD_TOOLONG_THRESHf,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

#ifdef CONFIG_GPON_ROGUE_SELF_DETECT /* support GPON rogue ONU self detect, turn on TX_SD pin */
    data = 0x1;
    if((ret = reg_field_write(RTL9603CVD_IO_MODE_ENr,RTL9603CVD_OEM_ENf,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
#endif

    /* for GPON MAC power saving default use GEM mdoe */
    data = 0x1;
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_PWR_SAV_MODEr,RTL9603CVD_PWR_SAV_MODEf,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    /* set behind to 0x10, ahead to 0x100 */
    data = 0x10;
    if((ret = reg_field_write(RTL9603CVD_GPON_GEM_US_PWR_SAV_CFGr,RTL9603CVD_OPT_BEHIND_CYCLESf,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    data = 0x100;
    if((ret = reg_field_write(RTL9603CVD_GPON_GEM_US_PWR_SAV_CFGr,RTL9603CVD_OPT_AHEAD_CYCLESf,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = rtl9603cvd_raw_pbo_globalThreshold_get(&sysOnTh,&sysOffTh)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON|MOD_DAL), "");
        return ret;
    }

    data = 0x1;
    if((ret = reg_field_write(RTL9603CVD_PON_RATE_CTRLr,RTL9603CVD_EN_RATE_PONf,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    data = 0x4ffff;
    if((ret = reg_field_write(RTL9603CVD_PON_RATE_CTRLr,RTL9603CVD_RATEf,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    /* for gpon pps setting */
    data = 7999;
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_DS_PPS_INTRr,RTL9603CVD_PPS_PERIODf,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    /* set DG tx option for send only 3 times when dying gasp happened */
    data = 0x1;
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_PWR_SAV_MODEr,RTL9603CVD_DG_TX_OPTf,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_init */



/* Function Name:
 *      dal_rtl9603cvd_gpon_resetState_set
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
int32  dal_rtl9603cvd_gpon_resetState_set(rtk_enable_t state)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_RESETr,RTL9603CVD_SOFT_RSTf,&state))!=RT_ERR_OK)
    {
         RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
         return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_resetState_set */



/* Function Name:
 *      dal_rtl9603cvd_gpon_resetDoneState_get
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
int32 dal_rtl9603cvd_gpon_resetDoneState_get(rtk_enable_t *pState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_RESETr,RTL9603CVD_RST_DONEf,pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of uint32    dal_rtl9603cvd_gpon_resetDoneState_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_version_get
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
int32 dal_rtl9603cvd_gpon_version_get(uint32 *pVersion)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pVersion), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_VERSIONr,RTL9603CVD_VER_IDf,pVersion))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
          return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_version_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_test_get
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
int32 dal_rtl9603cvd_gpon_test_get(uint32 *pTestData)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTestData), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_read(RTL9603CVD_GPON_TESTr,pTestData))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
          return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_test_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_test_set
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
int32 dal_rtl9603cvd_gpon_test_set(uint32 testData)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "testData=%d",testData);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */

    /* function body */
    if((ret = reg_write(RTL9603CVD_GPON_TESTr,&testData))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
          return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_test_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_topIntrMask_get
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
int32 dal_rtl9603cvd_gpon_topIntrMask_get(rtk_gpon_intrType_t topIntrType,rtk_enable_t *pState)
{
    int32 ret=0;
    uint32 field;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "topIntrType=%d",topIntrType);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_INTR_END <=topIntrType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if(topIntrType!=GPON_INTR_ALL)
    {
        field = gpon_intrMask_field[topIntrType];
        if(field ==0 || (ret = reg_field_read(RTL9603CVD_GPON_INTR_MASKr,field,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
              return RT_ERR_FAILED;
        }
    }else
    {
        if((ret = reg_read(RTL9603CVD_GPON_INTR_MASKr,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
              return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_topIntrMask_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_topIntrMask_set
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
int32 dal_rtl9603cvd_gpon_topIntrMask_set(rtk_gpon_intrType_t topIntrType,rtk_enable_t state)
{
    int32 ret=0;
    uint32 regValue,field;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "topIntrType=%d,state=%d",topIntrType, state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_INTR_END <=topIntrType), RT_ERR_INPUT);

    /* function body */
    if(topIntrType!=GPON_INTR_ALL)
    {
        field = gpon_intrMask_field[topIntrType];
        if(field == 0 || (ret = reg_field_write(RTL9603CVD_GPON_INTR_MASKr,field,&state))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
              return RT_ERR_FAILED;
        }
    }else
    {
        regValue = (uint32)state;
        if((ret = reg_write(RTL9603CVD_GPON_INTR_MASKr,&regValue))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
              return RT_ERR_FAILED;
        }
    }
        return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_topIntrMask_set */




/* Function Name:
 *      dal_rtl9603cvd_gpon_topIntr_get
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
int32 dal_rtl9603cvd_gpon_topIntr_get(rtk_gpon_intrType_t topIntrType,rtk_enable_t *pState)
{
    int32 ret;
    uint32 field;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "topIntrType=%d",topIntrType);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_INTR_END <=topIntrType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if(topIntrType!=GPON_INTR_ALL)
    {
        field = gpon_intr_field[topIntrType];
        if((ret = reg_field_read(RTL9603CVD_GPON_INTR_STSr,field,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
              return RT_ERR_FAILED;
        }
    }else
    {
        if((ret = reg_read(RTL9603CVD_GPON_INTR_STSr,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
              return RT_ERR_FAILED;
        }
    }


    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_topIntr_get */


/*

page 0x01 */


/* Function Name:
 *      dal_rtl9603cvd_gpon_gtcDsIntrDlt_get
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
int32 dal_rtl9603cvd_gpon_gtcDsIntrDlt_get(rtk_gpon_gtcDsIntrType_t gtcIntrType,rtk_enable_t *pState)
{
    int32 ret=0;
    uint32 field;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "gtcIntrType=%d",gtcIntrType);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_GTC_DS_INTR_END <=gtcIntrType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if(gtcIntrType!=GPON_GTC_DS_INTR_ALL){

        field = gpon_gtcDsIntrIndicator_field[gtcIntrType];
        if(field==0 || (ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_INTR_DLTr,field,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }

    }else
    {
        if((ret = reg_read(RTL9603CVD_GPON_GTC_DS_INTR_DLTr,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }


    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_gtcDsIntr_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_gtcDsIntrDlt_check
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
int32 dal_rtl9603cvd_gpon_gtcDsIntrDlt_check(rtk_gpon_gtcDsIntrType_t gtcIntrType,uint32 gtcDsIntrDltValue,rtk_enable_t *pState)
{
    int32 ret=0;
    uint32 field;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "gtcIntrType=%d",gtcIntrType);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_GTC_DS_INTR_ALL <=gtcIntrType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */

    field = gpon_gtcDsIntrIndicator_field[gtcIntrType];
    if(field==0 || (ret = reg_field_get(RTL9603CVD_GPON_GTC_DS_INTR_DLTr,field,pState,&gtcDsIntrDltValue))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_gtcDsIntr_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_gtcDsIntr_get
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
int32 dal_rtl9603cvd_gpon_gtcDsIntr_get(rtk_gpon_gtcDsIntrType_t gtcIntrType,rtk_enable_t *pState)
{
    int32 ret=0;
    uint32 field;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "gtcIntrType=%d",gtcIntrType);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_GTC_DS_INTR_END <=gtcIntrType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if(gtcIntrType!=GPON_GTC_DS_INTR_ALL){

        field = gpon_gtcDsIntr_field[gtcIntrType];
        if(field==0 || (ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_INTR_STSr,field,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }

    }else
    {
        if((ret = reg_read(RTL9603CVD_GPON_GTC_DS_INTR_STSr,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }


    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_gtcDsIntr_get */



/* Function Name:
 *      dal_rtl9603cvd_gpon_gtcDsIntrMask_get
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
int32 dal_rtl9603cvd_gpon_gtcDsIntrMask_get(rtk_gpon_gtcDsIntrType_t gtcIntrType,rtk_enable_t  *pState)
{
    int32 ret=0;
    uint32 field;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "gtcIntrType=%d",gtcIntrType);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_GTC_DS_INTR_END <=gtcIntrType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if(gtcIntrType!=GPON_GTC_DS_INTR_ALL)
    {
        field = gpon_gtcDsIntrMask_field[gtcIntrType];
        if(field ==0 || (ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_INTR_MASKr,field,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
              return RT_ERR_FAILED;
        }
    }else
    {
        if((ret = reg_read(RTL9603CVD_GPON_GTC_DS_INTR_MASKr,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
              return RT_ERR_FAILED;
        }
    }
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_gtcDsIntrMask_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_gtcDsIntrMask_set
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
int32 dal_rtl9603cvd_gpon_gtcDsIntrMask_set(rtk_gpon_gtcDsIntrType_t gtcIntrType,rtk_enable_t  state)
{
    int32 ret=0;
    uint32 field,regValue;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "gtcIntrType=%d,state=%d",gtcIntrType, state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_GTC_DS_INTR_END <=gtcIntrType), RT_ERR_INPUT);

    /* function body */
    if(gtcIntrType!=GPON_GTC_DS_INTR_ALL)
    {
        field = gpon_gtcDsIntrMask_field[gtcIntrType];
        if(field == 0 || (ret = reg_field_write(RTL9603CVD_GPON_GTC_DS_INTR_MASKr,field,&state))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
              return RT_ERR_FAILED;
        }
    }else
    {
        regValue = (uint32) state;
        if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_INTR_MASKr,&regValue))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
              return RT_ERR_FAILED;
        }
    }
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_gtcDsIntrMask_set */


/* Function Name:
 *      dal_rtl9603cvd_gpon_topIntr_disableAll
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
int32  dal_rtl9603cvd_gpon_topIntr_disableAll(void)
{
    int32 ret;
    uint32 data,tmp;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */

    /* function body */
    if((ret = dal_rtl9603cvd_gpon_topIntrMask_set(GPON_INTR_ALL,DISABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = dal_rtl9603cvd_gpon_gtcDsIntrMask_set(GPON_GTC_DS_INTR_ALL,DISABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = dal_rtl9603cvd_gpon_gtcUsIntrMask_set(GPON_GTC_US_INTR_ALL,DISABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    data=0;
    tmp = GPON_REG_STATUS_SET;
    if((ret = reg_field_set(RTL9603CVD_GPON_INTR_MASKr, RTL9603CVD_GTC_DS_Mf, &tmp, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    //reg_field_set(RTL9603CVD_GPON_INTR_MASKr, RTL9603CVD_GEM_DS_Mf, &tmp, &data);
    if((ret = reg_field_set(RTL9603CVD_GPON_INTR_MASKr, RTL9603CVD_GTC_US_Mf, &tmp, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_write(RTL9603CVD_GPON_INTR_MASKr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_topIntr_disableAll */

/* Function Name:
 *      dal_rtl9603cvd_gpon_onuId_set
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
int32 dal_rtl9603cvd_gpon_onuId_set(uint8 onuId)
{
    int32 ret;
    uint32 tmp = onuId;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "ounId=%d",onuId);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    /*RT_PARAM_CHK((256 <=onuId), RT_ERR_INPUT);*/

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_DS_ONU_ID_STATUSr,RTL9603CVD_ONU_IDf,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_ONU_IDr,RTL9603CVD_ONU_IDf,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }


    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_onuId_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_onuId_get
 * Description:
 *      Get GPON ONU ID
 * Input:
 *      onuId: value of ONUID
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9603cvd_gpon_onuId_get(uint8 *pOnuId)
{
    int32 ret;
    uint32 tmp;
    
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL ==pOnuId), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_US_ONU_IDr,RTL9603CVD_ONU_IDf,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    *pOnuId = (uint8)tmp;

    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_onuId_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_onuState_set
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
int32 dal_rtl9603cvd_gpon_onuState_set(rtk_gpon_onuState_t  onuState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "onuState=%d",onuState);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_STATE_END <=onuState), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_DS_ONU_ID_STATUSr,RTL9603CVD_ONU_STATEf,&onuState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_onuState_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_onuState_get
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
int32 dal_rtl9603cvd_gpon_onuState_get(rtk_gpon_onuState_t  *pOnuState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pOnuState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_ONU_ID_STATUSr,RTL9603CVD_ONU_STATEf,pOnuState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_onuState_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsBwmapCrcCheckState_set
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
int32 dal_rtl9603cvd_gpon_dsBwmapCrcCheckState_set(rtk_enable_t state)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_DS_CFGr,RTL9603CVD_CHK_BWM_CRCf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_dsBwmapCrcCheckState_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsBwmapCrcCheckState_get
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
int32 dal_rtl9603cvd_gpon_dsBwmapCrcCheckState_get(rtk_enable_t *pState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_CFGr,RTL9603CVD_CHK_BWM_CRCf,pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_dsBwmapCrcCheckState_get */


/* Added in GPON_MAC_SWIO_v1.1 */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsBwmapFilterOnuIdState_set
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
int32 dal_rtl9603cvd_gpon_dsBwmapFilterOnuIdState_set(rtk_enable_t state)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_DS_CFGr,RTL9603CVD_BWM_FILT_ONUIDf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_dsBwmapFilterOnuIdState_set */


/* Function Name:
 *      dal_rtl9603cvd_gpon_dsBwmapFilterOnuIdState_get
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
int32 dal_rtl9603cvd_gpon_dsBwmapFilterOnuIdState_get(rtk_enable_t *pState)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK(( NULL == pState), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_CFGr,RTL9603CVD_BWM_FILT_ONUIDf,pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd*/


/* Function Name:
 *      dal_rtl9603cvd_gpon_dsPlendStrictMode_set
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
int32 dal_rtl9603cvd_gpon_dsPlendStrictMode_set(rtk_enable_t state)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_DS_CFGr,RTL9603CVD_PLEND_STRICT_MODEf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_dsPlendStrictMode_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsPlendStrictMode_get
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
int32 dal_rtl9603cvd_gpon_dsPlendStrictMode_get(rtk_enable_t *pState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_CFGr,RTL9603CVD_PLEND_STRICT_MODEf,pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_dsPlendStrictMode_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsScrambleState_set
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
int32 dal_rtl9603cvd_gpon_dsScrambleState_set(rtk_enable_t state)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    state = !state;
    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_DS_CFGr,RTL9603CVD_DESCRAM_DISf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_dsScrambleState_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsScrambleState_get
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
int32 dal_rtl9603cvd_gpon_dsScrambleState_get(rtk_enable_t *pState)
{
    int32 ret;
    uint32 state;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_CFGr,RTL9603CVD_DESCRAM_DISf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    *pState = !state;
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_dsScrambleState_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsFecBypass_set
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
int32 dal_rtl9603cvd_gpon_dsFecBypass_set(rtk_enable_t state)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_DS_CFGr,RTL9603CVD_FEC_CORRECT_DISf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_dsFecBypass_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsFecBypass_get
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
int32 dal_rtl9603cvd_gpon_dsFecBypass_get(rtk_enable_t *pState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_CFGr,RTL9603CVD_FEC_CORRECT_DISf,pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_dsFecBypass_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsFecThrd_set
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
int32 dal_rtl9603cvd_gpon_dsFecThrd_set(uint8 fecThrdValue)
{
    int32 ret;
    uint32 tmp = fecThrdValue;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "fecThrdValue=%d",fecThrdValue);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((8 <=fecThrdValue), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_DS_CFGr,RTL9603CVD_FEC_DET_THRSHf,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_dsFecThrd_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsFecThrd_get
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
int32 dal_rtl9603cvd_gpon_dsFecThrd_get(uint8 *pFecThrdValue)
{
    int32 ret;
    uint32 tmp;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pFecThrdValue), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_CFGr,RTL9603CVD_FEC_DET_THRSHf,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    *pFecThrdValue = (uint8)tmp;
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_dsFecThrd_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_extraSnTxTimes_set
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
int32 dal_rtl9603cvd_gpon_extraSnTxTimes_set(uint8 exSnTxTimes)
{
    int32 ret;
    uint32 tmp = exSnTxTimes;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "exSnTxTimes=%d",exSnTxTimes);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((4 <=exSnTxTimes), RT_ERR_INPUT);

    /* function body */
    if((ret=reg_field_write(RTL9603CVD_GPON_GTC_DS_CFGr,RTL9603CVD_EXTRA_SN_TXf,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_extraSnTxTimes_set */


/* Function Name:
 *      dal_rtl9603cvd_gpon_extraSnTxTimes_get
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
int32 dal_rtl9603cvd_gpon_extraSnTxTimes_get(uint8 *pExSnTxTimes)
{
    uint32 ret;
    uint32 tmp;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pExSnTxTimes), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret=reg_field_read(RTL9603CVD_GPON_GTC_DS_CFGr,RTL9603CVD_EXTRA_SN_TXf,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    *pExSnTxTimes = (uint8) tmp;
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_extraSnTxTimes_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_topGemUsIntr_get
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
int32 dal_rtl9603cvd_gpon_dsPloamNomsg_set(uint8 ploamNoMsgValue)
{
    int32 ret;
    uint32 tmp = ploamNoMsgValue;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "ploamNoMsgValue=%d",ploamNoMsgValue);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */

    /* function body */
    if((ret =  reg_field_write(RTL9603CVD_GPON_GTC_DS_PLOAM_CFGr,RTL9603CVD_PLM_DS_NOMSG_IDf,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_dsPloamNomsg_set */


/* Function Name:
 *      dal_rtl9603cvd_gpon_dsPloamNomsg_get
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
int32 dal_rtl9603cvd_gpon_dsPloamNomsg_get(uint8 *pPloamNoMsgValue)
{
    int32 ret;
    uint32 tmp;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPloamNoMsgValue), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret =  reg_field_read(RTL9603CVD_GPON_GTC_DS_PLOAM_CFGr,RTL9603CVD_PLM_DS_NOMSG_IDf,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    *pPloamNoMsgValue = (uint8)tmp;
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_dsPloamNomsg_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsPloamOnuIdFilterState_set
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
int32 dal_rtl9603cvd_gpon_dsPloamOnuIdFilterState_set(rtk_enable_t state)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_DS_PLOAM_CFGr,RTL9603CVD_PLM_DS_ONUID_FLT_ENf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_dsPloamOnuIdFilterState_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsPloamOnuIdFilterState_get
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
int32 dal_rtl9603cvd_gpon_dsPloamOnuIdFilterState_get(rtk_enable_t *pState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_PLOAM_CFGr,RTL9603CVD_PLM_DS_ONUID_FLT_ENf,pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_dsPloamOnuIdFilterState_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsPloamBcAcceptState_set
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
int32 dal_rtl9603cvd_gpon_dsPloamBcAcceptState_set(rtk_enable_t state)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_DS_PLOAM_CFGr,RTL9603CVD_PLM_BC_ACC_ENf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_dsPloamBcAcceptState_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsPloamBcAcceptState_get
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
int32 dal_rtl9603cvd_gpon_dsPloamBcAcceptState_get(rtk_enable_t *pState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_PLOAM_CFGr,RTL9603CVD_PLM_BC_ACC_ENf,pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_dsPloamBcAcceptState_get */

/* Added in GPON_MAC_SWIO_v1.1 */


/* Function Name:
 *      dal_rtl9603cvd_gpon_dsPloamDropCrcState_set
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
int32  dal_rtl9603cvd_gpon_dsPloamDropCrcState_set(rtk_enable_t state)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_DS_PLOAM_CFGr,RTL9603CVD_PLM_DROP_CRCEf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsPloamDropCrcState_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsPloamDropCrcState_get
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
int32 dal_rtl9603cvd_gpon_dsPloamDropCrcState_get(rtk_enable_t *pState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_PLOAM_CFGr,RTL9603CVD_PLM_DROP_CRCEf,pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_dsPloamDropCrcState_get */




/* Function Name:
 *      dal_rtl9603cvd_gpon_cdrLosStatus_get
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
int32 dal_rtl9603cvd_gpon_cdrLosStatus_get(rtk_enable_t *pState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret  =  reg_field_read(RTL9603CVD_GPON_GTC_DS_LOS_CFG_STSr, RTL9603CVD_CDR_LOS_SIGf, pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_cdrLosStatus_get */




/* Function Name:
 *      dal_rtl9603cvd_gpon_optLosStatus_get
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
int32 dal_rtl9603cvd_gpon_optLosStatus_get(rtk_enable_t *pState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret  =  reg_field_read(RTL9603CVD_GPON_GTC_DS_LOS_CFG_STSr, RTL9603CVD_OPTIC_LOS_SIGf, pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_optLosStatus_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_topGemUsIntr_get
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
int32  dal_rtl9603cvd_gpon_losCfg_set(rtk_enable_t opten, int32 optpolar, rtk_enable_t cdren, int32 cdrpolar, rtk_enable_t filter)
{
    int32 ret;
    uint32 value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "opten=%d,optpolar=%d,cdren=%d,cdrpolar=%d,filter=%d",opten, optpolar, cdren, cdrpolar, filter);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= opten), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <=optpolar), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= cdren), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <=cdrpolar), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= filter), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_read(RTL9603CVD_GPON_GTC_DS_LOS_CFG_STSr,&value))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_LOS_CFG_STSr,RTL9603CVD_OPTIC_LOS_ENf,&opten,&value))!= RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_LOS_CFG_STSr,RTL9603CVD_OPTIC_LOS_POLARf,&optpolar,&value))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_LOS_CFG_STSr,RTL9603CVD_CDR_LOS_ENf,&cdren,&value))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_LOS_CFG_STSr,RTL9603CVD_CDR_LOS_POLARf,&cdrpolar,&value))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_LOS_CFG_STSr,RTL9603CVD_LOS_FILTER_ENf,&filter,&value))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_LOS_CFG_STSr,&value))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_losCfg_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_topGemUsIntr_get
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
int32 dal_rtl9603cvd_gpon_losCfg_get(int32 *opten, int32 *optpolar, int32 *cdren, int32 *cdrpolar, int32 *filter)
{
    int32 ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == opten), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == optpolar), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == cdren), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == cdrpolar), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == filter), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_read(RTL9603CVD_GPON_GTC_DS_LOS_CFG_STSr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_get(RTL9603CVD_GPON_GTC_DS_LOS_CFG_STSr,RTL9603CVD_OPTIC_LOS_ENf,(uint32*)opten,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_get(RTL9603CVD_GPON_GTC_DS_LOS_CFG_STSr,RTL9603CVD_OPTIC_LOS_POLARf,(uint32*)optpolar,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_get(RTL9603CVD_GPON_GTC_DS_LOS_CFG_STSr,RTL9603CVD_CDR_LOS_ENf,(uint32*)cdren,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_get(RTL9603CVD_GPON_GTC_DS_LOS_CFG_STSr,RTL9603CVD_CDR_LOS_POLARf,(uint32*)cdrpolar,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_get(RTL9603CVD_GPON_GTC_DS_LOS_CFG_STSr,RTL9603CVD_LOS_FILTER_ENf,(uint32*)filter,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_losCfg_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsPloam_get
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
int32 dal_rtl9603cvd_gpon_dsPloam_get(uint8 *pPloamData)
{
    int32 ret;
    uint32 tmp,i,ploam,data;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPloamData), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_PLOAM_INDr,RTL9603CVD_PLM_BUF_EMPTYf,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if(tmp == GPON_REG_STATUS_CLEAR)
    {

        for(i=0;i<6;i++)
        {
            if((ret = reg_array_field_read(RTL9603CVD_GPON_GTC_DS_PLOAM_MSGr,REG_ARRAY_INDEX_NONE,i,RTL9603CVD_PLOAM_RDATAf,&ploam))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
                return RT_ERR_FAILED;
            }
            pPloamData[2*i+0] = (ploam>>GPON_REG_16BITS_HIGH)&0xFF;
            pPloamData[2*i+1] = (ploam>>GPON_REG_16BITS_LOW)&0xFF;
        }
        data = GPON_REG_STATUS_CLEAR;
            if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_PLOAM_INDr,&data))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
            }
        data = GPON_REG_STATUS_SET;
        if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_PLOAM_INDr,&data))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
            return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_PLOAM_QUEUE_EMPTY;
    }

}   /* end of   dal_rtl9603cvd_gpon_dsPloam_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsPloam_getAll
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
int32 dal_rtl9603cvd_gpon_dsPloam_getAll(gpon_ds_ploam_buf_t *pPloambuf)
{
    int32 ret;
    uint32 tmp,i,ploam,data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPloambuf), RT_ERR_NULL_POINTER);

    /* function body */
    /* read all ploam from HW until ds_ploam_buf(SW) full or HW empty */
    do { 
        /* ds_ploam_buf is full */
        if((pPloambuf->tail+1)%GPON_DS_PLOAM_BUF_MAX == pPloambuf->head)
        {
            break;
        }

        if((ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_PLOAM_INDr,RTL9603CVD_PLM_BUF_EMPTYf,&tmp))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        if(tmp == GPON_REG_STATUS_CLEAR) /* HW PLM BUF is not empty */
        {
            for(i=0;i<6;i++)
            {
                if((ret = reg_array_field_read(RTL9603CVD_GPON_GTC_DS_PLOAM_MSGr,REG_ARRAY_INDEX_NONE,i,RTL9603CVD_PLOAM_RDATAf,&ploam))!=RT_ERR_OK)
                {
                    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
                    return RT_ERR_FAILED;
                }
                pPloambuf->ploam_array[pPloambuf->tail][2*i+0] = (ploam>>GPON_REG_16BITS_HIGH)&0xFF;
                pPloambuf->ploam_array[pPloambuf->tail][2*i+1] = (ploam>>GPON_REG_16BITS_LOW)&0xFF;
            }
            data = GPON_REG_STATUS_CLEAR;
            if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_PLOAM_INDr,&data))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
                break;
            }
            data = GPON_REG_STATUS_SET;
            if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_PLOAM_INDr,&data))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
                break;;
            }
            pPloambuf->tail = (pPloambuf->tail+1)%GPON_DS_PLOAM_BUF_MAX;
        }
    } while (tmp == GPON_REG_STATUS_CLEAR);

    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_dsPloam_getAll */

/* Function Name:
 *      dal_rtl9603cvd_gpon_tcont_get
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
int32 dal_rtl9603cvd_gpon_tcont_get(uint32 tcontId, uint32 *pAllocateId)
{
    int32 ret;
    uint32 op = 0;
    uint32 tmp_val;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "tcontId=%d",tcontId);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_TCONT()<=tcontId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAllocateId), RT_ERR_NULL_POINTER);

    /* function body */

    tmp_val = GPON_REG_OPERRATION_READ;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr, RTL9603CVD_ALLOCID_OP_MODEf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    /* for 9603D, TCONT4 and TCONT7 switch, TCONT5 and TCONT11 switch. */
    switch(tcontId)
    {
        case 4:
            tmp_val = 7;
        break;
        case 5:
            tmp_val = 11;
        break;
        case 7:
            tmp_val = 4;
        break;
        case 11:
            tmp_val = 5;
        break;
        default:
            tmp_val = tcontId;
        break;
    }
    
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr, RTL9603CVD_ALLOCID_OP_IDXf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = GPON_REG_STATUS_CLEAR;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr, RTL9603CVD_ALLOCID_OP_REQf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr,&op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = GPON_REG_STATUS_SET;

    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr, RTL9603CVD_ALLOCID_OP_REQf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr,&op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if(gpon_cam_wait(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr,TRUE))
    {
        if((ret = reg_read(RTL9603CVD_GPON_GTC_DS_ALLOC_RDr,pAllocateId))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_tcont_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_tcont_set
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
int32 dal_rtl9603cvd_gpon_tcont_set(uint32 tcontId, uint32 allocateId)
{
    uint32 op = 0;
    uint32 tmp_val;
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "tcontId=%d,allocateId=%d",tcontId, allocateId);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_TCONT() <=tcontId), RT_ERR_INPUT);
    RT_PARAM_CHK((4096 <=allocateId), RT_ERR_INPUT);

    /* function body */
    /* check global tcont state, if enabled, limit tcont set for tcont 1-3 */

    tmp_val = GPON_REG_OPERRATION_WRITE;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr, RTL9603CVD_ALLOCID_OP_MODEf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    /* for 9603D, TCONT4 and TCONT7 switch, TCONT5 and TCONT11 switch. */
    switch(tcontId)
    {
        case 4:
            tmp_val = 7;
        break;
        case 5:
            tmp_val = 11;
        break;
        case 7:
            tmp_val = 4;
        break;
        case 11:
            tmp_val = 5;
        break;
        default:
            tmp_val = tcontId;
        break;
    }

    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr, RTL9603CVD_ALLOCID_OP_IDXf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = GPON_REG_STATUS_CLEAR;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr, RTL9603CVD_ALLOCID_OP_REQf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr,&op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_DS_ALLOC_WRr,RTL9603CVD_ALLOCID_OP_WDATAf,&allocateId))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = GPON_REG_STATUS_SET;

    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr, RTL9603CVD_ALLOCID_OP_REQf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr,&op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if(gpon_cam_wait(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr,FALSE))
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }

    return RT_ERR_OK;
}   /* end of   dal_rtl9603cvd_gpon_tcont_set */


/* Function Name:
 *      dal_rtl9603cvd_gpon_tcont_del
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
int32 dal_rtl9603cvd_gpon_tcont_del(uint32 tcontId)
{
    uint32 op = 0;
    uint32 tmp_val;
    int32 ret;
#if 0
    int32 i;
    rtk_ponmac_queue_t queue,defQueue;
#endif

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "tcontId=%d",tcontId);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_MAC_MAX_TCONT_NUM <= tcontId), RT_ERR_INPUT);

    /* function body */
    tmp_val = GPON_REG_OPERRATION_CLEAN;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr, RTL9603CVD_ALLOCID_OP_MODEf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    
    /* for 9603D, TCONT4 and TCONT7 switch, TCONT5 and TCONT11 switch. */
    switch(tcontId)
    {
        case 4:
            tmp_val = 7;
        break;
        case 5:
            tmp_val = 11;
        break;
        case 7:
            tmp_val = 4;
        break;
        case 11:
            tmp_val = 5;
        break;
        default:
            tmp_val = tcontId;
        break;
    }
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr, RTL9603CVD_ALLOCID_OP_IDXf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = GPON_REG_STATUS_CLEAR;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr, RTL9603CVD_ALLOCID_OP_REQf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr,&op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = GPON_REG_STATUS_SET;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr, RTL9603CVD_ALLOCID_OP_REQf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr,&op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if(gpon_cam_wait(RTL9603CVD_GPON_GTC_DS_ALLOC_INDr,FALSE))
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_tcont_del */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPort_get
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
int32 dal_rtl9603cvd_gpon_dsGemPort_get(uint32 idx, rtk_gem_cfg_t *pGemCfg)
{
    uint32 op = 0;
    uint32 tmp_val;
    int32  ret;
    uint32 cfg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "idx=%d",idx);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_FLOW()<=idx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pGemCfg), RT_ERR_NULL_POINTER);

    /* function body */
    tmp_val = GPON_REG_OPERRATION_READ;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_PORT_INDr, RTL9603CVD_PORTID_OP_MODEf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = idx;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_PORT_INDr, RTL9603CVD_PORTID_OP_IDXf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = GPON_REG_STATUS_CLEAR;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_PORT_INDr, RTL9603CVD_PORTID_OP_REQf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_PORT_INDr,&op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = GPON_REG_STATUS_SET;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_PORT_INDr, RTL9603CVD_PORTID_OP_REQf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_PORT_INDr,&op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if(gpon_cam_wait(RTL9603CVD_GPON_GTC_DS_PORT_INDr,TRUE))
    {
#if defined(CONFIG_SDK_ASICDRV_TEST)
        if((ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_PORT_WRr,RTL9603CVD_PORTID_OP_WDATAf,&pGemCfg->gemPortId))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
#else
        if((ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_PORT_RDr,RTL9603CVD_PORTID_OP_RDATAf,&pGemCfg->gemPortId))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
#endif
            if((ret = reg_array_field_read(RTL9603CVD_GPON_GTC_DS_TRAFFIC_CFGr,
                             REG_ARRAY_INDEX_NONE,
                             idx,
                             RTL9603CVD_TRAFFIC_TYPE_CFGf,
                             &cfg))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
            }
        pGemCfg->enAES = (cfg & (1<<4)) ? ENABLED : DISABLED;
        pGemCfg->isEthernet = (cfg & (1<<1) )? ENABLED: DISABLED;
        pGemCfg->isOMCI= (cfg & (1<<2)) ? ENABLED: DISABLED;
        pGemCfg->isMulticast= (cfg & (1<<0)) ? ENABLED: DISABLED;

        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPort_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPort_set
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
int32 dal_rtl9603cvd_gpon_dsGemPort_set(uint32 idx, rtk_gem_cfg_t gemCfg)
{
    uint32 op = 0;
    uint32 tmp_val;
    int32  ret;
    uint32 cfg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "idx=%u,gemPortId=%d,isMulticast=%d,isEthernet=%d,isOMCI=%d,enAES=%d",idx, gemCfg.gemPortId, gemCfg.isMulticast, gemCfg.isEthernet, gemCfg.isOMCI, gemCfg.enAES);

    /* check Init status */
    RT_INIT_CHK(gpon_init);
    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_FLOW() <=idx), RT_ERR_INPUT);
    RT_PARAM_CHK((4096 <=gemCfg.gemPortId), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END<=gemCfg.enAES), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END<=gemCfg.isEthernet), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END<=gemCfg.isMulticast), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END<=gemCfg.isOMCI), RT_ERR_INPUT);

    /* function body */
    tmp_val = GPON_REG_OPERRATION_WRITE;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_PORT_INDr, RTL9603CVD_PORTID_OP_MODEf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }

    tmp_val = idx;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_PORT_INDr, RTL9603CVD_PORTID_OP_IDXf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }

    tmp_val = GPON_REG_STATUS_CLEAR;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_PORT_INDr, RTL9603CVD_PORTID_OP_REQf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_PORT_INDr,&op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_DS_PORT_WRr,RTL9603CVD_PORTID_OP_WDATAf,&gemCfg.gemPortId))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }
    tmp_val = GPON_REG_STATUS_SET;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_PORT_INDr, RTL9603CVD_PORTID_OP_REQf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_PORT_INDr,&op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }

    if(gpon_cam_wait(RTL9603CVD_GPON_GTC_DS_PORT_INDr,FALSE))
    {
        cfg = 0 ;
        cfg = gemCfg.isMulticast | gemCfg.isEthernet << 1 | gemCfg.isOMCI << 2 | gemCfg.enAES <<4;

        if((ret = reg_array_field_write(RTL9603CVD_GPON_GTC_DS_TRAFFIC_CFGr,
                              REG_ARRAY_INDEX_NONE,
                              idx,
                              RTL9603CVD_TRAFFIC_TYPE_CFGf,
                              &cfg))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return ret;
        }

        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPort_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPort_del
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
int32 dal_rtl9603cvd_gpon_dsGemPort_del(uint32 idx)
{

    uint32 op = 0;
    uint32 tmp_val;
    int32  ret;
    uint32 cfg=0;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "idx=%d",idx);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_MAC_MAX_FLOW_NUM <=idx), RT_ERR_INPUT);

    /* function body */
    tmp_val = GPON_REG_OPERRATION_CLEAN;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_PORT_INDr, RTL9603CVD_PORTID_OP_MODEf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = idx;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_PORT_INDr, RTL9603CVD_PORTID_OP_IDXf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = GPON_REG_STATUS_CLEAR;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_PORT_INDr, RTL9603CVD_PORTID_OP_REQf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_PORT_INDr,&op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = GPON_REG_STATUS_SET;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_PORT_INDr, RTL9603CVD_PORTID_OP_REQf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_PORT_INDr,&op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if(gpon_cam_wait(RTL9603CVD_GPON_GTC_DS_PORT_INDr,FALSE))
    {
        if((ret = reg_array_field_write(RTL9603CVD_GPON_GTC_DS_TRAFFIC_CFGr,
                              REG_ARRAY_INDEX_NONE,
                              idx,
                              RTL9603CVD_TRAFFIC_TYPE_CFGf,
                              &cfg))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
            return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPort_del */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortPktCnt_get
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
int32 dal_rtl9603cvd_gpon_dsGemPortPktCnt_get(uint32 idx, uint32 *pktCnt)
{
    int32 ret,ind=0;
    uint32 tmp_val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "idx=%d",idx);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_FLOW() <=idx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pktCnt), RT_ERR_NULL_POINTER);

    /* function body */
    tmp_val = idx;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_PORT_CNTR_INDr, RTL9603CVD_GEM_CNTR_IDXf, &tmp_val, &ind))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = GPON_REG_STATUS_CLEAR;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_PORT_CNTR_INDr, RTL9603CVD_GEM_CNTR_RSELf, &tmp_val, &ind))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_PORT_CNTR_INDr,&ind))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if(gpon_indirect_wait(RTL9603CVD_GPON_GTC_DS_PORT_CNTR_INDr))
    {
        if((ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_PORT_CNTR_STATr,RTL9603CVD_GEM_CNTRf,pktCnt))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortPktCnt_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortByteCnt_get
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
int32 dal_rtl9603cvd_gpon_dsGemPortByteCnt_get(uint32 idx, uint32 *byteCnt)
{
    uint32 ind = 0;
    uint32 tmp_val;
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "idx=%d",idx);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_FLOW() <=idx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == byteCnt), RT_ERR_NULL_POINTER);

    /* function body */
    tmp_val = idx;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_PORT_CNTR_INDr, RTL9603CVD_GEM_CNTR_IDXf, &tmp_val, &ind))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = GPON_REG_STATUS_SET;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_PORT_CNTR_INDr, RTL9603CVD_GEM_CNTR_RSELf, &tmp_val, &ind))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret =reg_write(RTL9603CVD_GPON_GTC_DS_PORT_CNTR_INDr,&ind))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if(gpon_indirect_wait(RTL9603CVD_GPON_GTC_DS_PORT_CNTR_INDr))
    {
        if((ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_PORT_CNTR_STATr,RTL9603CVD_GEM_CNTRf,byteCnt))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortByteCnt_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGtcMiscCnt_get
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
int32 dal_rtl9603cvd_gpon_dsGtcMiscCnt_get(rtk_gpon_dsGtc_pmMiscType_t dsGtcPmMiscType, uint32 *miscCnt)
{
    int32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "dsGtcPmMiscType=%d",dsGtcPmMiscType);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_REG_DSGTCPMMISC_END <=dsGtcPmMiscType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == miscCnt), RT_ERR_NULL_POINTER);

    /* function body */

    switch(dsGtcPmMiscType)
    {
        case GPON_REG_DSGTCPMMISC_BIP_BLOCK_ERR:
            ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_MISC_CNTR_BIP_ERR_BLKr,RTL9603CVD_CNTR_DS_BIP_ERR_BLOCKf,miscCnt);
            break;
        case GPON_REG_DSGTCPMMISC_BIP_BITS_ERR:
            ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_MISC_CNTR_BIP_ERR_BITr,RTL9603CVD_CNTR_DS_BIP_ERR_BITSf,miscCnt);
            break;
        case GPON_REG_DSGTCPMMISC_FEC_CORRECTED_BITS:
            ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_MISC_CNTR_FEC_CORRECT_BITr,RTL9603CVD_CNTR_FEC_CORRECTED_BITSf,miscCnt);
            break;
        case GPON_REG_DSGTCPMMISC_FEC_CORRECTED_BYTES:
            ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_MISC_CNTR_FEC_CORRECT_BYTEr,RTL9603CVD_CNTR_FEC_CORRECTED_BYTESf,miscCnt);
            break;
        case GPON_REG_DSGTCPMMISC_FEC_CORRETED_CWS:
            ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_MISC_CNTR_FEC_CORRECT_CWr,RTL9603CVD_CNTR_FEC_CORRETED_CWf,miscCnt);
            break;
        case GPON_REG_DSGTCPMMISC_FEC_UNCOR_CWS:
            ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_MISC_CNTR_FEC_UNCOR_CWr,RTL9603CVD_CNTR_FEC_UNCORRETABLE_CWf,miscCnt);
            break;
        case GPON_REG_DSGTCPMMISC_LOM:
            ret = reg_read(RTL9603CVD_GPON_GTC_DS_MISC_CNTR_LOMr,miscCnt);
            break;
        case GPON_REG_DSGTCPMMISC_PLOAM_ACCEPT:
            ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_MISC_CNTR_PLOAM_ACPTr,RTL9603CVD_CNTR_PLOAMD_ACCEPTEDf,miscCnt);
            break;
        case GPON_REG_DSGTCPMMISC_PLOAM_FAIL:
            ret = reg_read(RTL9603CVD_GPON_GTC_DS_MISC_CNTR_PLOAM_FAILr,miscCnt);
            break;
        case GPON_REG_DSGTCPMMISC_BWMAP_FAIL:
            ret = reg_read(RTL9603CVD_GPON_GTC_DS_MISC_CNTR_BWM_FAILr,miscCnt);
            break;
        case GPON_REG_DSGTCPMMISC_BWMAP_INV:
            ret = reg_read(RTL9603CVD_GPON_GTC_DS_MISC_CNTR_BWM_INVr,miscCnt);
            break;
        case GPON_REG_DSGTCPMMISC_ACTIVE:
            ret = reg_read(RTL9603CVD_GPON_GTC_DS_MISC_CNTR_ACTIVEr,miscCnt);
            break;
        case GPON_REG_DSGTCPMMISC_BWMAP_ACPT:
            ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_MISC_CNTR_BWM_ACPTr,RTL9603CVD_CNTR_BWMAP_ACCPTEDf,miscCnt);
            break;
        case GPON_REG_DSGTCPMMISC_GEM_LOS:
            ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_MISC_CNTR_GEM_LOSr,RTL9603CVD_CNTR_GEM_LOSf,miscCnt);
            break;
        case GPON_REG_DSGTCPMMISC_HEC_COR:
            ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_MISC_CNTR_HEC_CORRECTr,RTL9603CVD_CNTR_HEC_CORRECTEDf,miscCnt);
            break;
        case GPON_REG_DSGTCPMMISC_GEM_IDLE:
            ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_MISC_CNTR_GEM_IDLEr,RTL9603CVD_CNTR_GEM_IDLEf,miscCnt);
            break;
        case GPON_REG_DSGTCPMMISC_GEM_FAIL:
            ret = reg_read(RTL9603CVD_GPON_GTC_DS_MISC_CNTR_GEM_FAILr,miscCnt);
            break;
        case GPON_REG_DSGTCPMMISC_RX_GEM_NON_IDLE:
            ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_MISC_CNTR_GEM_NON_IDLEr,RTL9603CVD_CNTR_RX_GEM_NON_IDLEf,miscCnt);
            break;
        case GPON_REG_DSGTCPMMISC_PLEND_CORRECTIONS:
            ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_MISC_CNTR_PLEN_CORRECTr,RTL9603CVD_CNTR_PLEND_CORRECTIONSf,miscCnt);
            break;
        default:
            ret = RT_ERR_OUT_OF_RANGE;
            break;
    }

    if(ret!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortMiscCnt_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_dsOmciPti_set
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
int32 dal_rtl9603cvd_gpon_dsOmciPti_set(uint32 mask, uint32 end)
{
    int32 ret;
    uint32 data=0;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "mask=%d,end=%d",mask, end);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* function body */
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_OMCI_PTIr, RTL9603CVD_OMCI_PTI_MASKf, &mask, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_OMCI_PTIr, RTL9603CVD_OMCI_END_PTIf, &end, &data))!=RT_ERR_OK)
    {
         RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_OMCI_PTIr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsOmciPti_set */


/* Function Name:
 *      dal_rtl9603cvd_gpon_dsOmciPti_get
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
int32 dal_rtl9603cvd_gpon_dsOmciPti_get(uint32 *pMask, uint32 *pEnd)
{
    uint32 data=0,value;
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEnd), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_read(RTL9603CVD_GPON_GTC_DS_OMCI_PTIr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_get(RTL9603CVD_GPON_GTC_DS_OMCI_PTIr, RTL9603CVD_OMCI_PTI_MASKf, &value, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    *pMask = value & 0x7;
    if((ret = reg_field_get(RTL9603CVD_GPON_GTC_DS_OMCI_PTIr, RTL9603CVD_OMCI_END_PTIf, &value, &data))!=RT_ERR_OK)
    {
         RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    *pEnd = value & 0x7;
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsOmciPti_get */



/* Function Name:
 *      dal_rtl9603cvd_gpon_dsEthPti_set
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
int32 dal_rtl9603cvd_gpon_dsEthPti_set(uint32 mask, uint32 end)
{
    int32 ret;
    uint32 data=0;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "mask=%d,end=%d",mask, end);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* function body */
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_ETH_PTIr, RTL9603CVD_ETH_PTI_MASKf, &mask, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_DS_ETH_PTIr, RTL9603CVD_ETH_END_PTIf, &end, &data))!=RT_ERR_OK)
    {
         RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_ETH_PTIr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsEthPti_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsEthPti_get
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
int32 dal_rtl9603cvd_gpon_dsEthPti_get(uint32 *pMask, uint32 *pEnd)
{
    int32 ret,data=0,value;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEnd), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_read(RTL9603CVD_GPON_GTC_DS_ETH_PTIr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_get(RTL9603CVD_GPON_GTC_DS_ETH_PTIr, RTL9603CVD_ETH_PTI_MASKf, &value, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    *pMask = value & 0x7;
    if((ret = reg_field_get(RTL9603CVD_GPON_GTC_DS_ETH_PTIr, RTL9603CVD_ETH_END_PTIf, &value, &data))!=RT_ERR_OK)
    {
         RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    *pEnd = value & 0x7;
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsEthPti_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_aesKeySwitch_set
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
int32 dal_rtl9603cvd_gpon_aesKeySwitch_set(uint32 superframe)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "superframe=%d",superframe);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_AES_KEY_SWITCH_TIMEr,RTL9603CVD_SWITCH_SUPERFRAMEf,&superframe))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_aesKeySwitch_set */



/* Function Name:
 *      dal_rtl9603cvd_gpon_aesKeySwitch_get
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
int32 dal_rtl9603cvd_gpon_aesKeySwitch_get(uint32 *pSuperframe)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSuperframe), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_AES_KEY_SWITCH_TIMEr,RTL9603CVD_SWITCH_SUPERFRAMEf,pSuperframe))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_aesKeySwitch_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_aesKeyWord_set
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
int32 dal_rtl9603cvd_gpon_aesKeyWord_set(uint8 *keyword)
{
    uint32 data = 0;
    uint32 tmp_val;
    uint32 idx,ind;
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == keyword), RT_ERR_NULL_POINTER);

    /* function body */
    tmp_val = GPON_REG_STATUS_CLEAR;
    if((ret = reg_field_set(RTL9603CVD_GPON_AES_KEY_SWITCH_REQr, RTL9603CVD_CFG_ACTIVE_KEYf, &tmp_val, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = GPON_REG_STATUS_CLEAR;
    if((ret = reg_field_set(RTL9603CVD_GPON_AES_KEY_SWITCH_REQr, RTL9603CVD_KEY_CFG_REQf, &tmp_val, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_AES_KEY_SWITCH_REQr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = GPON_REG_STATUS_SET;
    if((ret = reg_field_set(RTL9603CVD_GPON_AES_KEY_SWITCH_REQr, RTL9603CVD_KEY_CFG_REQf, &tmp_val, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_AES_KEY_SWITCH_REQr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    for(idx=0;idx<8;idx++)
    {
        data = (keyword[2*idx+0]<<GPON_REG_16BITS_HIGH)|(keyword[2*idx+1]<<GPON_REG_16BITS_LOW);
        if((ret = reg_field_write(RTL9603CVD_GPON_AES_WORD_DATAr,RTL9603CVD_KEY_DATAf,&data))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }

        ind = 0;
        tmp_val = idx;
        if((ret = reg_field_set(RTL9603CVD_GPON_AES_KEY_WORD_INDr, RTL9603CVD_KEY_WORD_IDXf, &tmp_val, &ind))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        tmp_val = GPON_REG_STATUS_CLEAR;
        if((ret = reg_field_set(RTL9603CVD_GPON_AES_KEY_WORD_INDr, RTL9603CVD_KEY_WR_REQf, &tmp_val, &ind))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        if((ret = reg_write(RTL9603CVD_GPON_AES_KEY_WORD_INDr,&ind))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }

        tmp_val = GPON_REG_STATUS_SET;
        if((ret = reg_field_set(RTL9603CVD_GPON_AES_KEY_WORD_INDr, RTL9603CVD_KEY_WR_REQf, &tmp_val, &ind))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        if((ret = reg_write(RTL9603CVD_GPON_AES_KEY_WORD_INDr,&ind))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }

        if(gpon_cam_wait(RTL9603CVD_GPON_AES_KEY_WORD_INDr,FALSE))
        {
            /*do onthing*/
        }

    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_aesKeyWord_set */


/* Function Name:
 *      dal_rtl9603cvd_gpon_aesKeyWordActive_set
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
int32 dal_rtl9603cvd_gpon_aesKeyWordActive_set(uint8 *keyword)
{
    uint32 data = 0;
    uint32 tmp_val;
    uint32 idx,ind;
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == keyword), RT_ERR_NULL_POINTER);

    /* function body */
    tmp_val = GPON_REG_STATUS_SET;
    if((ret = reg_field_set(RTL9603CVD_GPON_AES_KEY_SWITCH_REQr, RTL9603CVD_CFG_ACTIVE_KEYf, &tmp_val, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = GPON_REG_STATUS_CLEAR;
    if((ret = reg_field_set(RTL9603CVD_GPON_AES_KEY_SWITCH_REQr, RTL9603CVD_KEY_CFG_REQf, &tmp_val, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_AES_KEY_SWITCH_REQr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = GPON_REG_STATUS_SET;
    if((ret = reg_field_set(RTL9603CVD_GPON_AES_KEY_SWITCH_REQr, RTL9603CVD_KEY_CFG_REQf, &tmp_val, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_AES_KEY_SWITCH_REQr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    for(idx=0;idx<8;idx++)
    {
        data = (keyword[2*idx+0]<<GPON_REG_16BITS_HIGH)|(keyword[2*idx+1]<<GPON_REG_16BITS_LOW);
        if((ret = reg_field_write(RTL9603CVD_GPON_AES_WORD_DATAr,RTL9603CVD_KEY_DATAf,&data))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }

        ind = 0;
        tmp_val = idx;
        if((ret = reg_field_set(RTL9603CVD_GPON_AES_KEY_WORD_INDr, RTL9603CVD_KEY_WORD_IDXf, &tmp_val, &ind))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        tmp_val = GPON_REG_STATUS_CLEAR;
        if((ret = reg_field_set(RTL9603CVD_GPON_AES_KEY_WORD_INDr, RTL9603CVD_KEY_WR_REQf, &tmp_val, &ind))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        if((ret = reg_write(RTL9603CVD_GPON_AES_KEY_WORD_INDr,&ind))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }

        tmp_val = GPON_REG_STATUS_SET;
        if((ret = reg_field_set(RTL9603CVD_GPON_AES_KEY_WORD_INDr, RTL9603CVD_KEY_WR_REQf, &tmp_val, &ind))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        if((ret = reg_write(RTL9603CVD_GPON_AES_KEY_WORD_INDr,&ind))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }

        if(gpon_cam_wait(RTL9603CVD_GPON_AES_KEY_WORD_INDr,FALSE))
        {
            /*do onthing*/
        }

    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_aesKeyWordActive_set */


/* Function Name:
 *      dal_rtl9603cvd_gpon_irq_get
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
int32 dal_rtl9603cvd_gpon_irq_get(rtk_enable_t *pState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_INT_DLTr,RTL9603CVD_GPON_IRQf,pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_irq_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortEthRxCnt_get
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
int32 dal_rtl9603cvd_gpon_dsGemPortEthRxCnt_get(uint32 idx,uint32 *pEthRxCnt)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "idx=%d",idx);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_FLOW() <=idx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEthRxCnt), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_write(RTL9603CVD_GPON_GEM_DS_RX_CNTR_INDr,&idx))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if(gpon_indirect_wait(RTL9603CVD_GPON_GEM_DS_RX_CNTR_INDr))
    {
        if((ret = reg_field_read(RTL9603CVD_GPON_GEM_DS_RX_CNTR_STATr,RTL9603CVD_ETH_PKT_RXf,pEthRxCnt))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortEthRxCnt_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortEthFwdCnt_get
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
int32 dal_rtl9603cvd_gpon_dsGemPortEthFwdCnt_get(uint32 idx,uint32 *pEthFwdCnt)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "idx=%d",idx);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_FLOW() <=idx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEthFwdCnt), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_write(RTL9603CVD_GPON_GEM_DS_FWD_CNTR_INDr,&idx))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if(gpon_indirect_wait(RTL9603CVD_GPON_GEM_DS_FWD_CNTR_INDr))
    {
        if((ret = reg_field_read(RTL9603CVD_GPON_GEM_DS_FWD_CNTR_STATr,RTL9603CVD_ETH_PKT_FWDf,pEthFwdCnt))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortEthFwdCnt_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortMiscCnt_get
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
int32 dal_rtl9603cvd_gpon_dsGemPortMiscCnt_get(rtk_gpon_dsGem_pmMiscType_t idx,uint32 *pMiscCnt)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "idx=%d",idx);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_REG_DSGEMPMMISC_OMCI_RX <idx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMiscCnt), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GEM_DS_MISC_INDr,RTL9603CVD_MISC_CNTR_IDXf,&idx))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_read(RTL9603CVD_GPON_GEM_DS_MISC_CNTR_STATr,RTL9603CVD_MISC_CNTRf,pMiscCnt))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortMiscCnt_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortFcsCheckState_get
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
int32 dal_rtl9603cvd_gpon_dsGemPortFcsCheckState_get(rtk_enable_t *pState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GEM_DS_MC_CFGr,RTL9603CVD_FCS_CHK_ENf,pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortFcsCheckState_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortFcsCheckState_set
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
int32 dal_rtl9603cvd_gpon_dsGemPortFcsCheckState_set(rtk_enable_t state)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GEM_DS_MC_CFGr,RTL9603CVD_FCS_CHK_ENf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortFcsCheckState_set */


/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortBcPassState_set
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
int32 dal_rtl9603cvd_gpon_dsGemPortBcPassState_set(rtk_enable_t state)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GEM_DS_MC_CFGr,RTL9603CVD_BROADCAST_PASSf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortBcPassState_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortBcPassState_get
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
int32 dal_rtl9603cvd_gpon_dsGemPortBcPassState_get(rtk_enable_t *pState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GEM_DS_MC_CFGr,RTL9603CVD_BROADCAST_PASSf,pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortBcPassState_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortNonMcPassState_set
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
int32 dal_rtl9603cvd_gpon_dsGemPortNonMcPassState_set(rtk_enable_t state)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GEM_DS_MC_CFGr,RTL9603CVD_NON_MULTICAST_PASSf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortNonMcPassState_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortNonMcPassState_get
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
int32 dal_rtl9603cvd_gpon_dsGemPortNonMcPassState_get(rtk_enable_t *pState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GEM_DS_MC_CFGr,RTL9603CVD_NON_MULTICAST_PASSf,pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortNonMcPassState_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortMacFilterMode_set
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

int32 dal_rtl9603cvd_gpon_dsGemPortMacFilterMode_set(rtk_gpon_macTable_exclude_mode_t macFilterMode)
{
    int32 ret;
    uint32 raw;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "macFilterMode=%d",macFilterMode);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_GPON_MACTBL_MODE_END <=macFilterMode), RT_ERR_INPUT);

    if((ret = gpon_macfilterMode_cfg2raw(macFilterMode,&raw))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GEM_DS_MC_CFGr,RTL9603CVD_MC_EXCL_MODEf,&raw))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortMacFilterMode_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortMacFilterMode_get
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

int32 dal_rtl9603cvd_gpon_dsGemPortMacFilterMode_get(rtk_gpon_macTable_exclude_mode_t *pMacFilterMode)
{
    int32 ret;
    uint32 raw;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMacFilterMode), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GEM_DS_MC_CFGr,RTL9603CVD_MC_EXCL_MODEf,&raw))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = gpon_macfilterMode_raw2cfg(raw,pMacFilterMode))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortMacFilterMode_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortMacForceMode_set
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

int32 dal_rtl9603cvd_gpon_dsGemPortMacForceMode_set(rtk_gpon_ipVer_t ipVer ,rtk_gpon_mc_force_mode_t macForceMode)
{
    int32 ret;
    uint32 drop,pass;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "macForceMode=%d",macForceMode);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_GPON_MCFORCE_MODE_END <=macForceMode), RT_ERR_INPUT);

    /* function body */
    switch(macForceMode){
    case RTK_GPON_MCFORCE_MODE_PASS:
        drop = 0;
        pass = 1;
    break;
    case RTK_GPON_MCFORCE_MODE_DROP:
        drop = 1;
        pass = 0;
    break;
    case RTK_GPON_MCFORCE_MODE_NORMAL:
        drop = 0;
        pass = 0;
    break;
    default:
        return RT_ERR_FAILED;
    break;
    }

    switch(ipVer){
    case GPON_IP_VER_V4:
    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GEM_DS_MC_CFGr,RTL9603CVD_IPV4_MC_FORCE_DROPf,&drop))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_write(RTL9603CVD_GPON_GEM_DS_MC_CFGr,RTL9603CVD_IPV4_MC_FORCE_PASSf,&pass))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    break;
    case GPON_IP_VER_V6:
    if((ret = reg_field_write(RTL9603CVD_GPON_GEM_DS_MC_CFGr,RTL9603CVD_IPV6_MC_FORCE_DROPf,&drop))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_write(RTL9603CVD_GPON_GEM_DS_MC_CFGr,RTL9603CVD_IPV6_MC_FORCE_PASSf,&pass))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    break;
    default:
        return RT_ERR_FAILED;
    break;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortMacForceMode_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortMacForceMode_get
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
int32 dal_rtl9603cvd_gpon_dsGemPortMacForceMode_get(rtk_gpon_ipVer_t ipVer,rtk_gpon_mc_force_mode_t *pMacForceMode)
{
    int32 ret;
    uint32 drop,pass;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMacForceMode), RT_ERR_NULL_POINTER);

    switch(ipVer){
    case GPON_IP_VER_V4:
    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GEM_DS_MC_CFGr,RTL9603CVD_IPV4_MC_FORCE_DROPf,&drop))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_read(RTL9603CVD_GPON_GEM_DS_MC_CFGr,RTL9603CVD_IPV4_MC_FORCE_PASSf,&pass))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    break;
    case GPON_IP_VER_V6:
    if((ret = reg_field_read(RTL9603CVD_GPON_GEM_DS_MC_CFGr,RTL9603CVD_IPV6_MC_FORCE_DROPf,&drop))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_read(RTL9603CVD_GPON_GEM_DS_MC_CFGr,RTL9603CVD_IPV6_MC_FORCE_PASSf,&pass))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    break;
    default:
        return RT_ERR_FAILED;
    break;
    }
    /* function body */
    if(drop==0 && pass == 0)
    {
        *pMacForceMode = RTK_GPON_MCFORCE_MODE_NORMAL;
    }else
    if(drop==0 && pass == 1)
    {
        *pMacForceMode = RTK_GPON_MCFORCE_MODE_PASS;
    }else
    {
        *pMacForceMode = RTK_GPON_MCFORCE_MODE_DROP;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortMacForceMode_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortMacEntry_set
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
int32 dal_rtl9603cvd_gpon_dsGemPortMacEntry_set(uint32 idx, rtk_mac_t mac)
{
    uint32 op=0;
    uint32 tmp_val;
    uint32 ret;
    uint32 tmpMac;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "idx=%u,mac=%u:%u:%u:%u:%u:%u",idx, mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],mac.octet[5]);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    /*RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_FLOW() <=idx), RT_ERR_INPUT);*/
    /*assign mac without prefix*/
    tmpMac = (mac.octet[2]<<24) |(mac.octet[3]<<16)|(mac.octet[4]<<8)|(mac.octet[5]<<0);
    /* function body */
    tmp_val = GPON_REG_OPERRATION_WRITE;
    if((ret = reg_field_set(RTL9603CVD_GPON_GEM_DS_MC_INDr, RTL9603CVD_MC_ITEM_OP_MODEf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = idx;
    if((ret = reg_field_set(RTL9603CVD_GPON_GEM_DS_MC_INDr, RTL9603CVD_MC_ITEM_OP_IDXf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_write(RTL9603CVD_GPON_GEM_DS_MC_INDr,&op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_field_write(RTL9603CVD_GPON_GEM_DS_MC_WRr,RTL9603CVD_MC_ITEM_OP_WDATAf,&tmpMac))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = GPON_REG_STATUS_SET;
    if((ret = reg_field_set(RTL9603CVD_GPON_GEM_DS_MC_INDr, RTL9603CVD_MC_ITEM_OP_REQf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GEM_DS_MC_INDr,&op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if(gpon_cam_wait(RTL9603CVD_GPON_GEM_DS_MC_INDr,FALSE))
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortMacEntry_set */


/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortMacEntry_get
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
int32 dal_rtl9603cvd_gpon_dsGemPortMacEntry_get(uint32 idx, rtk_mac_t *pMac)
{
    uint32 op=0;
    uint32 tmp_val;
    int32 ret;
    uint32 tmpMac=0;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "idx=%d",idx);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    /*RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_FLOW() <=idx), RT_ERR_INPUT);*/
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    /* function body */
    tmp_val = GPON_REG_OPERRATION_READ;
    if((ret = reg_field_set(RTL9603CVD_GPON_GEM_DS_MC_INDr, RTL9603CVD_MC_ITEM_OP_MODEf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = idx;
    if((ret = reg_field_set(RTL9603CVD_GPON_GEM_DS_MC_INDr, RTL9603CVD_MC_ITEM_OP_IDXf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_write(RTL9603CVD_GPON_GEM_DS_MC_INDr,&op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_field_write(RTL9603CVD_GPON_GEM_DS_MC_WRr,RTL9603CVD_MC_ITEM_OP_WDATAf,&tmpMac))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = GPON_REG_STATUS_SET;
    if((ret = reg_field_set(RTL9603CVD_GPON_GEM_DS_MC_INDr, RTL9603CVD_MC_ITEM_OP_REQf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GEM_DS_MC_INDr,&op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if(gpon_cam_wait(RTL9603CVD_GPON_GEM_DS_MC_INDr,FALSE))
    {
#if defined (CONFIG_SDK_ASICDRV_TEST)
        if((ret= reg_field_read(RTL9603CVD_GPON_GEM_DS_MC_WRr,RTL9603CVD_MC_ITEM_OP_WDATAf,&tmpMac))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
#else
        if((ret = reg_field_read(RTL9603CVD_GPON_GEM_DS_MC_RDr,RTL9603CVD_MC_ITEM_OP_RDATAf,&tmpMac))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
#endif
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
    pMac->octet[0]=0x0;
        pMac->octet[1]=0x0;
    pMac->octet[2]=0x0;
        pMac->octet[3]=(tmpMac>>16)&0xFF;
        pMac->octet[4]=(tmpMac>>8)&0xFF;
        pMac->octet[5]=tmpMac&0xFF;
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortMacEntry_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortMacEntry_del
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
int32 dal_rtl9603cvd_gpon_dsGemPortMacEntry_del(uint32 idx)
{
    uint32 op=0;
    uint32 tmp_val;
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "idx=%d",idx);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    /*RT_PARAM_CHK((RTK_XXX <=idx), RT_ERR_INPUT);*/

    /* function body */
    tmp_val = GPON_REG_OPERRATION_CLEAN;
    if((ret = reg_field_set(RTL9603CVD_GPON_GEM_DS_MC_INDr, RTL9603CVD_MC_ITEM_OP_MODEf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = idx;
    if((ret = reg_field_set(RTL9603CVD_GPON_GEM_DS_MC_INDr, RTL9603CVD_MC_ITEM_OP_IDXf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_write(RTL9603CVD_GPON_GEM_DS_MC_INDr,&op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = GPON_REG_STATUS_SET;
    if((ret = reg_field_set(RTL9603CVD_GPON_GEM_DS_MC_INDr, RTL9603CVD_MC_ITEM_OP_REQf, &tmp_val, &op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GEM_DS_MC_INDr,&op))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if(gpon_cam_wait(RTL9603CVD_GPON_GEM_DS_MC_INDr,FALSE))
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortMacEntry_del */



/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortFrameTimeOut_set
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
int32 dal_rtl9603cvd_gpon_dsGemPortFrameTimeOut_set(uint32 timeOutValue)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "timeOutValue=%d",timeOutValue);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* function body */
    if((ret =   reg_field_write(RTL9603CVD_GPON_GEM_DS_FRM_TIMEOUTr, RTL9603CVD_ASSM_TIMEOUT_FRMf,&timeOutValue))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortFrameTimeOut_set */


/* Function Name:
 *      dal_rtl9603cvd_gpon_dsGemPortFrameTimeOut_get
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
int32 dal_rtl9603cvd_gpon_dsGemPortFrameTimeOut_get(uint32 *pTimeOutValue)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTimeOutValue), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret =   reg_field_read(RTL9603CVD_GPON_GEM_DS_FRM_TIMEOUTr, RTL9603CVD_ASSM_TIMEOUT_FRMf,pTimeOutValue))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsGemPortFrameTimeOut_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_ipv6McAddrPtn_get
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

int32 dal_rtl9603cvd_gpon_ipv4McAddrPtn_get(uint32 *pPrefix)
{
    int32 ret;
    uint32 pattern;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPrefix), RT_ERR_NULL_POINTER);

    /* function body */
        if((ret = reg_field_read(RTL9603CVD_GPON_GEM_DS_MC_ADDR_PTN_IPV4r, RTL9603CVD_IPV4_MC_MAC_PREFIXf,&pattern))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
        }
    *pPrefix = pattern&0xFFFFFF;

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_ipv4McAddrPtn_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_ipv6McAddrPtn_set
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

int32 dal_rtl9603cvd_gpon_ipv4McAddrPtn_set(uint32 prefix)
{
    int32 ret;
    uint32 pattern=prefix&0xFFFFFF;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GEM_DS_MC_ADDR_PTN_IPV4r, RTL9603CVD_IPV4_MC_MAC_PREFIXf,&pattern))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_ipv4McAddrPtn_set */


/* Function Name:
 *      dal_rtl9603cvd_gpon_ipv6McAddrPtn_get
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

int32 dal_rtl9603cvd_gpon_ipv6McAddrPtn_get(uint32 *pPrefix)
{
    int32 ret;
    uint32 pattern;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPrefix), RT_ERR_NULL_POINTER);

    /* function body */
        if((ret = reg_field_read(RTL9603CVD_GPON_GEM_DS_MC_ADDR_PTN_IPV6r, RTL9603CVD_IPV6_MC_MAC_PREFIXf,&pattern))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
        }
    *pPrefix = pattern&0xFFFF;

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_ipv6McAddrPtn_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_ipv6McAddrPtn_set
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

int32 dal_rtl9603cvd_gpon_ipv6McAddrPtn_set(uint32 prefix)
{
    int32 ret;
    uint32 pattern=prefix&0xFFFF;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GEM_DS_MC_ADDR_PTN_IPV6r, RTL9603CVD_IPV6_MC_MAC_PREFIXf,&pattern))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_ipv6McAddrPtn_set */


/* page 0x05 */

/* Function Name:
 *      dal_rtl9603cvd_gpon_gtcUsIntr_get
 * Description:
 *      Get US GTC interrupt indicator state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9603cvd_gpon_gtcUsIntr_get(rtk_gpon_gtcUsIntrType_t gtcUsIntrType,rtk_enable_t *pState)
{
    int32 ret=0;
    uint32 field;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "gtcUsIntrType=%d",gtcUsIntrType);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_GTC_US_INTR_END <=gtcUsIntrType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if(gtcUsIntrType == GPON_GTC_US_INTR_ALL)
    {
        if((ret = reg_read(RTL9603CVD_GPON_GTC_US_INTR_STSr,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }else{

        /* function body */
        field = gpon_gtcUsIntr_field[gtcUsIntrType];
        if(field == 0 || (ret=reg_field_read(RTL9603CVD_GPON_GTC_US_INTR_STSr,field,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gtcUsIntr_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_gtcUsIntrIndicator_get
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
int32 dal_rtl9603cvd_gpon_gtcUsIntrDlt_get(rtk_gpon_gtcUsIntrType_t gtcUsIntrType,rtk_enable_t *pState)
{
    int32 ret = 0 ;
    uint32 field;
    RT_PARAM_CHK((GPON_GTC_US_INTR_END <=gtcUsIntrType), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);
    if(gtcUsIntrType == GPON_GTC_US_INTR_ALL){
        if( (ret=reg_read(RTL9603CVD_GPON_GTC_US_INTR_DLTr,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }else{
        /* function body */
        field = gpon_gtcUsIntrIndicator_field[gtcUsIntrType];
        if(field == 0 || (ret=reg_field_read(RTL9603CVD_GPON_GTC_US_INTR_DLTr,field,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gtcUsIntr_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_gtcUsIntrDlt_check
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
int32 dal_rtl9603cvd_gpon_gtcUsIntrDlt_check(rtk_gpon_gtcUsIntrType_t gtcUsIntrType,uint32 gtcUsIntrDltValue,rtk_enable_t *pState)
{
    int32 ret = 0 ;
    uint32 field;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "gtcUsIntrType=%d,gtcUsIntrDltValue=%d",gtcUsIntrType, gtcUsIntrDltValue);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_GTC_US_INTR_ALL <=gtcUsIntrType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    field = gpon_gtcUsIntrIndicator_field[gtcUsIntrType];
    if(field == 0 || (ret=reg_field_get(RTL9603CVD_GPON_GTC_US_INTR_DLTr,field,pState,&gtcUsIntrDltValue))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gtcUsIntrDlt_check */


/* Function Name:
 *      dal_rtl9603cvd_gpon_gtcUsIntrMask_get
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
int32 dal_rtl9603cvd_gpon_gtcUsIntrMask_get(rtk_gpon_gtcUsIntrType_t gtcUsIntrType,rtk_enable_t *pState)
{
    int32 ret = 0;
    uint32 field;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "gtcUsIntrType=%d",gtcUsIntrType);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_GTC_US_INTR_END <=gtcUsIntrType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if(gtcUsIntrType == GPON_GTC_US_INTR_ALL)
    {
        if((ret = reg_read(RTL9603CVD_GPON_GTC_US_INTR_MASKr,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }else{
        field  = gpon_gtcUsIntrMask_field[gtcUsIntrType];
        /* function body */
        if(field == 0 || (ret=reg_field_read(RTL9603CVD_GPON_GTC_US_INTR_MASKr,field,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gtcUsIntrMask_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_gtcUsIntrMask_set
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
int32  dal_rtl9603cvd_gpon_gtcUsIntrMask_set(rtk_gpon_gtcUsIntrType_t gtcUsIntrType,rtk_enable_t state)
{
    int32 ret;
    uint32 regValue,field;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "gtcUsIntrType=%d,state=%d",gtcUsIntrType, state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_GTC_US_INTR_END <=gtcUsIntrType), RT_ERR_INPUT);

    /* function body */
    if(gtcUsIntrType == GPON_GTC_US_INTR_ALL)
    {
        regValue = (uint32) state;
        if((ret = reg_write(RTL9603CVD_GPON_GTC_DS_INTR_MASKr,&regValue))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
              return RT_ERR_FAILED;
        }
    }else{
        field  = gpon_gtcUsIntrMask_field[gtcUsIntrType];
        /* function body */
        if((ret=reg_field_write(RTL9603CVD_GPON_GTC_US_INTR_MASKr,field,&state))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gtcUsIntrMask_set */


/* Function Name:
 *      dal_rtl9603cvd_gpon_forceLaser_set
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
int32  dal_rtl9603cvd_gpon_forceLaser_set(rtk_gpon_laser_status_t laserStatus)
{
    int32 ret;
    uint32 data=0,operation;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "laserStatus=%d",laserStatus);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_GPON_LASER_STATUS_END <=laserStatus), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_read(RTL9603CVD_GPON_GTC_US_CFGr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if(laserStatus==RTK_GPON_LASER_STATUS_FORCE_ON)
    {
        operation = GPON_REG_STATUS_SET;
        if((ret = reg_field_set(RTL9603CVD_GPON_GTC_US_CFGr, RTL9603CVD_FS_LONf, &operation, &data))!=RT_ERR_OK)
        {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }
    else
    {
        operation = GPON_REG_STATUS_CLEAR;
        if((ret=reg_field_set(RTL9603CVD_GPON_GTC_US_CFGr, RTL9603CVD_FS_LONf, &operation, &data))!=RT_ERR_OK)
        {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }

    if(laserStatus==RTK_GPON_LASER_STATUS_FORCE_OFF)
    {
        operation = GPON_REG_STATUS_SET;
       if((ret =  reg_field_set(RTL9603CVD_GPON_GTC_US_CFGr, RTL9603CVD_FS_LOFFf, &operation, &data))!=RT_ERR_OK)
       {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
       }
    }
    else
    {
        operation = GPON_REG_STATUS_CLEAR;
        if((ret = reg_field_set(RTL9603CVD_GPON_GTC_US_CFGr, RTL9603CVD_FS_LOFFf, &operation, &data))!=RT_ERR_OK)
        {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }
    operation = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;

    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&operation))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_CFGr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    operation = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&operation))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_forceLaser_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_forceLaser_get
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
int32 dal_rtl9603cvd_gpon_forceLaser_get(rtk_gpon_laser_status_t *plaserStatus)
{
    int32 ret;
    uint32 data,value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == plaserStatus), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_read(RTL9603CVD_GPON_GTC_US_CFGr, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_field_get(RTL9603CVD_GPON_GTC_US_CFGr,RTL9603CVD_FS_LOFFf,&value,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if(value)
    {
        *plaserStatus = RTK_GPON_LASER_STATUS_FORCE_OFF;
        return RT_ERR_OK;
    }
    if((ret = reg_field_get(RTL9603CVD_GPON_GTC_US_CFGr,RTL9603CVD_FS_LONf,&value,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if(value)
    {
        *plaserStatus = RTK_GPON_LASER_STATUS_FORCE_ON;
        return RT_ERR_OK;
    }
    *plaserStatus = RTK_GPON_LASER_STATUS_NORMAL;

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_forceLaser_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_forcePRBS_set
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
int32 dal_rtl9603cvd_gpon_forcePRBS_set(rtk_gpon_prbs_t prbsCfg)
{
    int32   ret;
    uint32  data,val=0;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* function body */
    if (RTK_GPON_PRBS_OFF != prbsCfg)
    {
        switch (prbsCfg)
        {
            case RTK_GPON_PRBS_31:
                data = 0x40AF;
                break;
            case RTK_GPON_PRBS_23:
                data = 0x409E;
                break;
            case RTK_GPON_PRBS_15:
                data = 0x408D;
                break;
            case RTK_GPON_PRBS_7:
                data = 0x407C;
                break;
            default:
                return RT_ERR_OUT_OF_RANGE;
                break;
        }

        if (RT_ERR_OK != (ret = reg_field_write(RTL9603CVD_WSDS_DIG_19r, RTL9603CVD_CFG_PRBS_TYPE_SELf, &data)))
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x", ret);
            return ret;
        }
    }

    data = (RTK_GPON_PRBS_OFF == prbsCfg) ? 0 : 1;

    if (RT_ERR_OK != (ret = reg_field_write(RTL9603CVD_WSDS_DIG_1Ar, RTL9603CVD_CFG_PRBS_ENf, &data)))
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x", ret);
        return ret;
    }

	/* set force BEN  and TX clock reference */
	if(RTK_GPON_PRBS_OFF == prbsCfg)
	{
		/*disable force BEN*/
        data = 0;
        if ((ret = reg_field_write(RTL9603CVD_SDS_ANA_MISC_REG16r, RTL9603CVD_FRC_BEN_ONf, &data)) != RT_ERR_OK)
        {
            return ret;
        }
        data = 0;
        if ((ret = reg_field_write(RTL9603CVD_SDS_ANA_MISC_REG16r, RTL9603CVD_FRC_BEN_VALf, &data)) != RT_ERR_OK)
        {
            return ret;
        }
        /* set TX clock reference from PON */
        data = 0x1;
        if ((ret = reg_field_write(RTL9603CVD_SDS_ANA_GPON_REG32r, RTL9603CVD_REG_TX_CK_SELf, &data)) != RT_ERR_OK)
        {
            return ret;
        }
    }
    else
    {
        /*check ben polarity invert*/
        if ((ret = reg_field_read(RTL9603CVD_WSDS_DIG_18r,RTL9603CVD_CFG_FRC_BEN_INVf,&val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        if(val == 0) /*high active*/
            data = 1;
        else
            data = 0;
        /*force BEN value*/
        if ((ret = reg_field_write(RTL9603CVD_SDS_ANA_MISC_REG16r, RTL9603CVD_FRC_BEN_VALf, &data)) != RT_ERR_OK)
        {
            return ret;
        }
        /*enable force BEN*/
        data = 1;
        if ((ret = reg_field_write(RTL9603CVD_SDS_ANA_MISC_REG16r, RTL9603CVD_FRC_BEN_ONf, &data)) != RT_ERR_OK)
        {
            return ret;
        }
        /* set TX clock reference from local */
        data = 0x2;
        if ((ret = reg_field_write(RTL9603CVD_SDS_ANA_GPON_REG32r, RTL9603CVD_REG_TX_CK_SELf, &data)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_forcePRBS_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_forcePRBS_get
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
int32 dal_rtl9603cvd_gpon_forcePRBS_get(rtk_gpon_prbs_t *pPrbsCfg)
{
    int32   ret;
    uint32  data;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPrbsCfg), RT_ERR_NULL_POINTER);

    /* function body */
    if (RT_ERR_OK != (ret = reg_field_read(RTL9603CVD_WSDS_DIG_1Ar, RTL9603CVD_CFG_PRBS_ENf, &data)))
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x", ret);
        return RT_ERR_FAILED;
    }

    if (data & 0x1)
    {
        if (RT_ERR_OK != (ret = reg_field_read(RTL9603CVD_WSDS_DIG_19r, RTL9603CVD_CFG_PRBS_TYPE_SELf, &data)))
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x", ret);
            return RT_ERR_FAILED;
        }

        switch ((data & 0xF0)>>4)
        {
            case 0xA:
                *pPrbsCfg = RTK_GPON_PRBS_31;
                break;
            case 0x9:
                *pPrbsCfg = RTK_GPON_PRBS_23;
                break;
            case 0x8:
                *pPrbsCfg = RTK_GPON_PRBS_15;
                break;
            case 0x7:
                *pPrbsCfg = RTK_GPON_PRBS_7;
                break;
            default:
                return RT_ERR_OUT_OF_RANGE;
                break;
        }
    }
    else
    {
        *pPrbsCfg = RTK_GPON_PRBS_OFF;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_forcePRBS_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_ploamState_set
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
int32  dal_rtl9603cvd_gpon_ploamState_set(rtk_enable_t state)
{
    int32 ret;
    uint32 tmp;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */

    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    state = !state;
        if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_CFGr,RTL9603CVD_PLM_DISf,&state))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
        }

    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
        if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&tmp))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
        }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_ploamState_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_ploamState_get
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
int32 dal_rtl9603cvd_gpon_ploamState_get(rtk_enable_t *pState)
{
    int32 ret ;
    uint32 data=0;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_US_CFGr,RTL9603CVD_PLM_DISf,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    *pState = !data;
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_ploamState_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_indNrmPloamState_set
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
int32  dal_rtl9603cvd_gpon_indNrmPloamState_set(rtk_enable_t state)
{
    int32 ret;
    uint32 tmp;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

        if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_CFGr,RTL9603CVD_IND_NRM_PLMf,&state))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
        }

    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
        if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&tmp))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
        }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_indNrmPloamState_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_indNrmPloamState_get
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
int32 dal_rtl9603cvd_gpon_indNrmPloamState_get(rtk_enable_t *pState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_US_CFGr,RTL9603CVD_IND_NRM_PLMf,pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_indNrmPloamState_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_dbruState_set
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
int32  dal_rtl9603cvd_gpon_dbruState_set(rtk_enable_t state)
{
    int32 ret;
    uint32 tmp;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */

    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    state = !state;
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_CFGr,RTL9603CVD_DBRU_DISf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dbruState_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dbruState_get
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
int32 dal_rtl9603cvd_gpon_dbruState_get(rtk_enable_t *pState)
{
    int32 ret ;
    uint32 data;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_US_CFGr,RTL9603CVD_DBRU_DISf,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    *pState = !data;
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dbruState_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_topGemUsIntr_get
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
int32  dal_rtl9603cvd_gpon_usScrambleState_set(rtk_enable_t state)
{
    int32 ret;
    uint32 tmp;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    state = !state;
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_CFGr,RTL9603CVD_SCRM_DISf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_usScrambleState_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_topGemUsIntr_get
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
int32 dal_rtl9603cvd_gpon_usScrambleState_get(rtk_enable_t *pState)
{
    int32 ret;
    uint32 data;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_US_CFGr,RTL9603CVD_SCRM_DISf,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    *pState = !data;
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_usScrambleState_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_usBurstPolarity_set
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
int32  dal_rtl9603cvd_gpon_usBurstPolarity_set(rtk_gpon_polarity_t polarityValue)
{
    int32 ret;
    uint32 tmp;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "polarityValue=%d",polarityValue);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_POLARITY_END <=polarityValue), RT_ERR_INPUT);

    /* function body */
    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = gpon_gtcUsBurstPolarity_cfg2raw(polarityValue,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_CFGr,RTL9603CVD_US_BEN_POLARf,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_usBurstPolarity_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_usBurstPolarity_get
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
int32 dal_rtl9603cvd_gpon_usBurstPolarity_get(rtk_gpon_polarity_t *pPolarityValue)
{
    int32 ret;
    uint32 data;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPolarityValue), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_US_CFGr,RTL9603CVD_US_BEN_POLARf,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = gpon_gtcUsBurstPolarity_cfg2raw(data,pPolarityValue))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_usBurstPolarity_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_eqd_set
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
int32  dal_rtl9603cvd_gpon_eqd_set(uint32 value,int32 offset)
{
    uint32 data=0,min_delay1;
    uint32 eqd1,multi,intra;
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "value=%d,offset=%d",value, offset);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* function body */

    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_US_MIN_DELAYr,RTL9603CVD_MIN_DELAY1f,&min_delay1))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    eqd1 = value + (min_delay1*16*8) + offset;
    multi = (eqd1 /GPON_REG_EQD_FRAME_LEN);
    intra = (eqd1-multi*GPON_REG_EQD_FRAME_LEN);

    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_US_EQDr,RTL9603CVD_EQD1_MULTFRAMEf,&multi,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_US_EQDr,RTL9603CVD_EQD1_INFRAMEf,&intra,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_EQDr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_eqd_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_eqd_get
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
int32  dal_rtl9603cvd_gpon_eqd_get(uint32 *pValue)
{
    uint32 data=0,min_delay1;
    uint32 eqd1,multi,intra;
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_US_MIN_DELAYr,RTL9603CVD_MIN_DELAY1f,&min_delay1))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_read(RTL9603CVD_GPON_GTC_US_EQDr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_field_get(RTL9603CVD_GPON_GTC_US_EQDr,RTL9603CVD_EQD1_MULTFRAMEf,&multi,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_get(RTL9603CVD_GPON_GTC_US_EQDr,RTL9603CVD_EQD1_INFRAMEf,&intra,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    eqd1 = multi*GPON_REG_EQD_FRAME_LEN + intra;
    *pValue = eqd1 - (min_delay1*16*8);
    
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_eqd_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_laserTime_set
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
int32  dal_rtl9603cvd_gpon_laserTime_set(uint8 on, uint8 off)
{
    int32 ret;
    uint32 value,data=0;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "on=%d,off=%d",on, off);

    /* check Init status */
    RT_INIT_CHK(gpon_init);
    /*check */
    RT_PARAM_CHK((0x37 < on), RT_ERR_INPUT);
    RT_PARAM_CHK((0x37 < off), RT_ERR_INPUT);

    /* function body */
    value = on;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_US_LASERr,RTL9603CVD_LON_TIMEf,&value,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    value = off;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_US_LASERr,RTL9603CVD_LOFF_TIMEf,&value,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_LASERr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_laserTime_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_laserTime_get
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
int32 dal_rtl9603cvd_gpon_laserTime_get(uint8 *on, uint8 *off)
{
    uint32 data,value;
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == on), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == off), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_read(RTL9603CVD_GPON_GTC_US_LASERr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_field_get(RTL9603CVD_GPON_GTC_US_LASERr,RTL9603CVD_LON_TIMEf,&value,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    *on  = (uint8)value;
    if((ret = reg_field_get(RTL9603CVD_GPON_GTC_US_LASERr,RTL9603CVD_LOFF_TIMEf,&value,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    *off  = (uint8)value;
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_laserTime_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_burstOverhead_set
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
int32  dal_rtl9603cvd_gpon_burstOverhead_set(uint8 rep, uint8 len, uint8 size, uint8 *oh)
{
    uint8 i;
    uint32 data=0, tmp;
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "rep=%d,len=%d,size=%d",rep, len, size);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    /*RT_PARAM_CHK((RTK_XXX <=rep), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_XXX <=len), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_XXX <=size), RT_ERR_INPUT);
    */
    RT_PARAM_CHK((NULL == oh), RT_ERR_NULL_POINTER);

    /* function body */
    if (rep) {}

#if 1 /* change the repeat to the last byte before delimiter */
    tmp = (size-4)&0x0F;
#else
    tmp = rep&0x0F;
#endif
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_US_BOH_CFGr, RTL9603CVD_BOH_REPEATf, &tmp, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp = len;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_US_BOH_CFGr, RTL9603CVD_BOH_LENGTHf, &tmp, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_BOH_CFGr,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    for(i=0;i<size;i++)
    {
        data = oh[i];
        if((ret = reg_array_write(RTL9603CVD_GPON_GTC_US_BOH_DATAr, REG_ARRAY_INDEX_NONE, i, &data))!=RT_ERR_OK)
        {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_burstOverhead_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_usPloam_set
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
int32 dal_rtl9603cvd_gpon_usPloam_set(rtk_enable_t isUrgent, uint8 *ploamValue)
{
    int32 ret;
    uint32 data, ind, bit, tmp;
    uint8  i;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "isUrgent=%d",isUrgent);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= isUrgent), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == ploamValue), RT_ERR_NULL_POINTER);

    /* function body */
    ind = 0;
    if(isUrgent)
    {
            tmp = GPON_REG_TYPE_GTC_US_PLOAM_URG;
            if((ret = reg_field_set(RTL9603CVD_GPON_GTC_US_PLOAM_INDr, RTL9603CVD_PLM_TYPEf, &tmp, &ind))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
            }
            bit = RTL9603CVD_PLM_URG_FULLf;
    }
    else
    {
        tmp = GPON_REG_TYPE_GTC_US_PLOAM_NRM;
        if((ret = reg_field_set(RTL9603CVD_GPON_GTC_US_PLOAM_INDr, RTL9603CVD_PLM_TYPEf, &tmp, &ind))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        bit = RTL9603CVD_PLM_NRM_FULLf;
    }
    tmp = GPON_REG_STATUS_CLEAR;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_US_PLOAM_INDr, RTL9603CVD_PLM_ENQf, &tmp, &ind))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_PLOAM_INDr,&ind))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_US_PLOAM_INDr,bit,&data))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if(data==GPON_REG_STATUS_CLEAR)
    {
        for(i=0;i<6;i++)
        {
            data = (ploamValue[2*i+0]<<GPON_REG_16BITS_HIGH)|(ploamValue[2*i+1]<<GPON_REG_16BITS_LOW);
            if((ret = reg_array_write(RTL9603CVD_GPON_GTC_US_PLOAM_DATAr, REG_ARRAY_INDEX_NONE, i, &data))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
            }
        }
        tmp = GPON_REG_STATUS_SET;
        if((ret = reg_field_set(RTL9603CVD_GPON_GTC_US_PLOAM_INDr, RTL9603CVD_PLM_ENQf, &tmp, &ind))!=RT_ERR_OK)
        {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        if((ret = reg_write(RTL9603CVD_GPON_GTC_US_PLOAM_INDr,&ind))!=RT_ERR_OK)
        {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_PLOAM_QUEUE_FULL;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_usPloam_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_usAutoPloam_set
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
int32  dal_rtl9603cvd_gpon_usAutoPloam_set(rtk_gpon_usAutoPloamType_t usAutoPloamType, uint8 *ploamValue)
{
    int32  ret;
    uint32 data, ind, tmp;
        uint8 i;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "usAutoPloamType=%d",usAutoPloamType);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_REG_USAUTOPOLAM_END <=usAutoPloamType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == ploamValue), RT_ERR_NULL_POINTER);

    /* function body */
    ind = 0;
    if(usAutoPloamType==GPON_REG_USAUTOPLOAM_SN)
    {
        tmp = GPON_REG_TYPE_GTC_US_PLOAM_SN;

    }
    else if(usAutoPloamType==GPON_REG_USAUTOPLOAM_DYINGGASP)
    {
        tmp = GPON_REG_TYPE_GTC_US_PLOAM_DYINGGASP;
    }
    else
    {
        tmp = GPON_REG_TYPE_GTC_US_PLOAM_NOMSG;
    }
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_US_PLOAM_INDr, RTL9603CVD_PLM_TYPEf, &tmp, &ind))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }

    tmp = GPON_REG_STATUS_CLEAR;
    if((ret = reg_field_set(RTL9603CVD_GPON_GTC_US_PLOAM_INDr, RTL9603CVD_PLM_ENQf, &tmp, &ind))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_PLOAM_INDr,&ind))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }

    for(i=0;i<6;i++)
    {
        data = (ploamValue[2*i+0]<<GPON_REG_16BITS_HIGH)|(ploamValue[2*i+1]<<GPON_REG_16BITS_LOW);
        if((ret = reg_array_write(RTL9603CVD_GPON_GTC_US_PLOAM_DATAr, REG_ARRAY_INDEX_NONE, i, &data))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return ret;
        }
    }

    tmp = GPON_REG_STATUS_SET;
    reg_field_set(RTL9603CVD_GPON_GTC_US_PLOAM_INDr, RTL9603CVD_PLM_ENQf, &tmp, &ind);
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_PLOAM_INDr,&ind))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_usAutoPloam_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_usPloamCrcGenState_set
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
int32  dal_rtl9603cvd_gpon_usPloamCrcGenState_set(rtk_enable_t state)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_PLOAM_CFGr,RTL9603CVD_PLM_US_CRC_GEN_ENf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_usPloamCrcGenState_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_topGemUsIntr_get
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
int32 dal_rtl9603cvd_gpon_usPloamCrcGenState_get(rtk_enable_t *pState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_US_PLOAM_CFGr,RTL9603CVD_PLM_US_CRC_GEN_ENf,pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_usPloamCrcGenState_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_usPloamOnuIdFilterState_set
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
int32  dal_rtl9603cvd_gpon_usPloamOnuIdFilterState_set(rtk_enable_t state)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_PLOAM_CFGr,RTL9603CVD_PLM_US_ONUID_OVRD_ENf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_usPloamOnuIdFilterState_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_usPloamOnuIdFilter_get
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
int32 dal_rtl9603cvd_gpon_usPloamOnuIdFilter_get(rtk_enable_t *pState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_US_PLOAM_CFGr,RTL9603CVD_PLM_US_ONUID_OVRD_ENf,pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_usPloamOnuIdFilter_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_usPloamBuf_flush
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
int32  dal_rtl9603cvd_gpon_usPloamBuf_flush(void)
{
    int32 ret;
    uint32 value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */

    /* function body */
    value = 0;
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_PLOAM_CFGr,RTL9603CVD_PLM_FLUSH_BUFf,&value))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    value = 1;
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_PLOAM_CFGr,RTL9603CVD_PLM_FLUSH_BUFf,&value))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_usPloamBuf_flush */



/* Function Name:
 *      dal_rtl9603cvd_gpon_usGtcMiscCnt_get
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

int32 dal_rtl9603cvd_gpon_usGtcMiscCnt_get(rtk_gpon_usGtc_pmMiscType_t pmMiscType, uint32 *pValue)
{
    int32 ret;
    uint32 raw;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "pmMiscType=%d",pmMiscType);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_REG_USGTCPMMISC_END <=pmMiscType), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = gpon_usGtcMiscCntType_cfg2raw(pmMiscType,&raw))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_MISC_CNTR_IDXr,RTL9603CVD_MISC_IDXf,&raw))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_read(RTL9603CVD_GPON_GTC_US_MISC_CNTR_STATr,pValue))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_usGtcMiscCnt_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_rdi_set
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
int32  dal_rtl9603cvd_gpon_rdi_set(int32 value)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "value=%d",value);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <=value), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_RDIr,RTL9603CVD_ONU_RDIf,&value))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_rdi_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_rdi_get
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
int32 dal_rtl9603cvd_gpon_rdi_get(int32 *value)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == value), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_US_RDIr,RTL9603CVD_ONU_RDIf,value))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_rdi_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_topGemUsIntr_get
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
int32  dal_rtl9603cvd_gpon_usSmalSstartProcState_set(rtk_enable_t state)
{
    int32 ret;
    uint32 tmp;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_PROC_MODEr,RTL9603CVD_AUTO_PROC_SSTARTf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_usSmalSstartProcState_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_usSmalSstartProcState_get
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
int32 dal_rtl9603cvd_gpon_usSmalSstartProcState_get(rtk_enable_t *pState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_US_PROC_MODEr,RTL9603CVD_AUTO_PROC_SSTARTf,pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_usSmalSstartProcState_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_usSuppressLaserState_set
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
int32  dal_rtl9603cvd_gpon_usSuppressLaserState_set(rtk_enable_t state)
{
    uint32 tmp;
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_PROC_MODEr,RTL9603CVD_OPTIC_AUTO_SUPRESS_DISf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    if((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_usSuppressLaserState_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_usSuppressLaserState_get
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
int32 dal_rtl9603cvd_gpon_usSuppressLaserState_get(rtk_enable_t *pState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_US_PROC_MODEr,RTL9603CVD_OPTIC_AUTO_SUPRESS_DISf,pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_usSuppressLaserState_get */


/* page 0x06 */
/* Function Name:
 *      dal_rtl9603cvd_gpon_gemUsIntr_get
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
int32 dal_rtl9603cvd_gpon_gemUsIntr_get(rtk_gpon_gemUsIntrType_t gemUsIntrType,rtk_enable_t *pState)
{
    int32 ret = 0;
    uint32 field;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "gemUsIntrType=%d",gemUsIntrType);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_GEM_US_INTR_END <=gemUsIntrType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if(gemUsIntrType == GPON_GEM_US_INTR_ALL){
        if((ret = reg_read(RTL9603CVD_GPON_GEM_US_INTR_STSr,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }else{
        field = gpon_gemUsIntrStatus_field[gemUsIntrType];
        if(field ==0 || (ret = reg_field_read(RTL9603CVD_GPON_GEM_US_INTR_STSr,field,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gemUsIntr_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_gemUsIntrMask_get
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
int32 dal_rtl9603cvd_gpon_gemUsIntrDlt_get(rtk_gpon_gemUsIntrType_t gemUsIntrType,rtk_enable_t *pState)
{
    int32 ret;
    uint32 field;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "gemUsIntrType=%d",gemUsIntrType);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_GEM_US_INTR_END <=gemUsIntrType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if(gemUsIntrType == GPON_GEM_US_INTR_ALL){
        if((ret = reg_read(RTL9603CVD_GPON_GEM_US_INTR_DLTr,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }else{
        field = gpon_gemUsIntrIndicator_field[gemUsIntrType];
        if((ret = reg_field_read(RTL9603CVD_GPON_GEM_US_INTR_DLTr,field,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gemUsIntrMask_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_gemUsIntrDlt_check
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
int32 dal_rtl9603cvd_gpon_gemUsIntrDlt_check(rtk_gpon_gemUsIntrType_t gemIntrType,uint32 gemUsIntrDltValue,rtk_enable_t *pState)
{
    int32 ret;
    uint32 field;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "gemIntrType=%d,gemUsIntrDltValue=%d",gemIntrType, gemUsIntrDltValue);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_GEM_US_INTR_ALL <=gemIntrType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    field = gpon_gemUsIntrIndicator_field[gemIntrType];
    if((ret = reg_field_get(RTL9603CVD_GPON_GEM_US_INTR_DLTr,field,pState,&gemUsIntrDltValue))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gemUsIntrDlt_check */



/* Function Name:
 *      dal_rtl9603cvd_gpon_gemUsIntrMask_get
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
int32 dal_rtl9603cvd_gpon_gemUsIntrMask_get(rtk_gpon_gemUsIntrType_t gemUsIntrType,rtk_enable_t *pState)
{
    int32 ret;
    uint32 field;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "gemUsIntrType=%d",gemUsIntrType);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_GEM_US_INTR_END <=gemUsIntrType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if(gemUsIntrType == GPON_GEM_US_INTR_ALL){
        if((ret = reg_read(RTL9603CVD_GPON_GEM_US_INTR_MASKr,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }else{
        field = gpon_gemUsIntrMask_field[gemUsIntrType];
        if((ret = reg_field_read(RTL9603CVD_GPON_GEM_US_INTR_MASKr,field,pState))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gemUsIntrMask_get */



/* Function Name:
 *      dal_rtl9603cvd_gpon_gemUsIntrMask_set
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
int32 dal_rtl9603cvd_gpon_gemUsIntrMask_set(rtk_gpon_gemUsIntrType_t gemUsIntrType,rtk_enable_t state)
{
    int32 ret;
    uint32 regValue,field;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "gemUsIntrType=%d,state=%d",gemUsIntrType, state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_GEM_US_INTR_END <=gemUsIntrType), RT_ERR_INPUT);

    /* function body */
    if(gemUsIntrType == GPON_GEM_US_INTR_ALL)
    {
        regValue = (uint32) state;
        if((ret = reg_write(RTL9603CVD_GPON_GEM_US_INTR_MASKr,&regValue))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
              return RT_ERR_FAILED;
        }
    }else{
        field = gpon_gemUsIntrMask_field[gemUsIntrType];
        /* function body */
        if((ret=reg_field_write(RTL9603CVD_GPON_GEM_US_INTR_MASKr,field,&state))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gemUsMask_set */



/* Function Name:
 *      dal_rtl9603cvd_gpon_gemUsForceIdleState_set
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
int32 dal_rtl9603cvd_gpon_gemUsForceIdleState_set(rtk_enable_t state)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_field_write(RTL9603CVD_GPON_GEM_US_PTI_CFGr, RTL9603CVD_FS_GEM_IDLEf, &state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gemUsForceIdleState_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_gemUsForceIdleState_get
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
int32 dal_rtl9603cvd_gpon_gemUsForceIdleState_get(rtk_enable_t *pState)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GEM_US_PTI_CFGr, RTL9603CVD_FS_GEM_IDLEf, pState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gemUsForceIdleState_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_gemUsPtiVector_set
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
int32 dal_rtl9603cvd_gpon_gemUsPtiVector_set(uint8 pti_v0, uint8 pti_v1, uint8 pti_v2, uint8 pti_v3)
{
    uint32 data=0;
    int32  ret;
    uint32 pti_value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "pti_v0=%d,pti_v1=%d,pti_v2=%d,pti_v3=%d",pti_v0, pti_v1, pti_v2, pti_v3);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    /*RT_PARAM_CHK((RTK_XXX <=pti_v0), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_XXX <=pti_v1), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_XXX <=pti_v2), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_XXX <=pti_v3), RT_ERR_INPUT);
    */
    /* function body */
    pti_value = pti_v0&0x7;
    if((ret = reg_field_set(RTL9603CVD_GPON_GEM_US_PTI_CFGr, RTL9603CVD_PTI_VECTOR0f, &pti_value, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }

    pti_value = pti_v1&0x7;
    if((ret = reg_field_set(RTL9603CVD_GPON_GEM_US_PTI_CFGr, RTL9603CVD_PTI_VECTOR1f, &pti_value, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }

    pti_value = pti_v2&0x7;
    if((ret = reg_field_set(RTL9603CVD_GPON_GEM_US_PTI_CFGr, RTL9603CVD_PTI_VECTOR2f, &pti_value, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }

    pti_value = pti_v3&0x7;
    if((ret = reg_field_set(RTL9603CVD_GPON_GEM_US_PTI_CFGr, RTL9603CVD_PTI_VECTOR3f, &pti_value, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }

    if((ret = reg_write(RTL9603CVD_GPON_GEM_US_PTI_CFGr, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gemUsPtiVector_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_gemUsPtiVector_get
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
int32 dal_rtl9603cvd_gpon_gemUsPtiVector_get(uint8 *pPti_v0, uint8 *pPti_v1, uint8 *pPti_v2, uint8 *pPti_v3)
{
    uint32 data=0;
    int32  ret;
    uint32 pti_value;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPti_v0), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPti_v1), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPti_v2), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPti_v3), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_read(RTL9603CVD_GPON_GEM_US_PTI_CFGr, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }

    if((ret = reg_field_get(RTL9603CVD_GPON_GEM_US_PTI_CFGr, RTL9603CVD_PTI_VECTOR0f, &pti_value, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }
    *pPti_v0 = pti_value&0x7;
    if((ret = reg_field_get(RTL9603CVD_GPON_GEM_US_PTI_CFGr, RTL9603CVD_PTI_VECTOR1f, &pti_value, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }
    *pPti_v1 = pti_value&0x7;
    if((ret = reg_field_get(RTL9603CVD_GPON_GEM_US_PTI_CFGr, RTL9603CVD_PTI_VECTOR2f, &pti_value, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }
    *pPti_v2 = pti_value&0x7;
    if((ret = reg_field_get(RTL9603CVD_GPON_GEM_US_PTI_CFGr, RTL9603CVD_PTI_VECTOR3f, &pti_value, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }
    *pPti_v3 = pti_value&0x7;

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gemUsPtiVector_get */


/* Function Name:
 *      dal_rtl9603cvd_gpon_gemUsEthCnt_get
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
int32 dal_rtl9603cvd_gpon_gemUsEthCnt_get(uint32 idx, uint32 *pEthCntr)
{
    uint32 data;
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "idx=%d",idx);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_FLOW()<=idx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEthCntr), RT_ERR_NULL_POINTER);

    /* function body */

    data = 0x80 | (idx & 0x7F);

    if((ret = reg_field_write(RTL9603CVD_GPON_GEM_US_ETH_GEM_RX_CNTR_IDXr, RTL9603CVD_ETH_GEM_RX_IDXf, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if(gpon_indirect_wait(RTL9603CVD_GPON_GEM_US_ETH_GEM_RX_CNTR_IDXr))
    {
        if((ret =  reg_read(RTL9603CVD_GPON_GEM_US_ETH_GEM_RX_CNTR_STATr, pEthCntr))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gemUsEthCnt_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_gemUsGemCnt_read
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
int32 dal_rtl9603cvd_gpon_gemUsGemCnt_get(uint32 idx, uint32 *pGemCntr)
{
    uint32 data;
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "idx=%d",idx);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_FLOW() <=idx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pGemCntr), RT_ERR_NULL_POINTER);

    /* function body */
    data = (idx & 0x7F);

    if((ret  =reg_field_write(RTL9603CVD_GPON_GEM_US_ETH_GEM_RX_CNTR_IDXr, RTL9603CVD_ETH_GEM_RX_IDXf, &data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if(gpon_indirect_wait(RTL9603CVD_GPON_GEM_US_ETH_GEM_RX_CNTR_IDXr))
    {
        if((ret =  reg_read(RTL9603CVD_GPON_GEM_US_ETH_GEM_RX_CNTR_STATr, pGemCntr))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gemUsGemCnt_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_gemUsPortCfg_set
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
int32 dal_rtl9603cvd_gpon_gemUsPortCfg_set(uint32 idx, uint32 gemPortId)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "idx=%d,gemPortId=%d",idx, gemPortId);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_FLOW() <=idx), RT_ERR_INPUT);
    RT_PARAM_CHK((4096 <=gemPortId), RT_ERR_INPUT);

    /* function body */
    if((ret = reg_array_field_write(RTL9603CVD_GPON_GEM_US_PORT_MAPr,
                                 REG_ARRAY_INDEX_NONE,
                                 idx,
                                 RTL9603CVD_PORT_CFG_DATAf,
                                 &gemPortId))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gemUsPortCfg_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_gemUsPortCfg_get
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
int32 dal_rtl9603cvd_gpon_gemUsPortCfg_get(uint32 idx, uint32 *pGemPortId)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "idx=%d",idx);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_FLOW() <=idx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pGemPortId), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_array_field_read(RTL9603CVD_GPON_GEM_US_PORT_MAPr,
                                 REG_ARRAY_INDEX_NONE,
                                 idx,
                                 RTL9603CVD_PORT_CFG_DATAf,
                                 pGemPortId))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gemUsPortCfg_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_gemUsDataByteCnt_get
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
int32 dal_rtl9603cvd_gpon_gemUsDataByteCnt_get(uint32 idx, uint64 *pByteCntr)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "idx=%d",idx);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_FLOW() <=idx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pByteCntr), RT_ERR_NULL_POINTER);

    /* function body */

    if((ret = reg_array_read(RTL9603CVD_GPON_GEM_US_BYTE_STATr,
                     REG_ARRAY_INDEX_NONE,
                     idx,
                     (uint32 *)pByteCntr))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    *pByteCntr = ((*pByteCntr >> 32) & 0xFFFFFFFF) |
                    ((*pByteCntr & 0xFFFFFFFF) << 32);

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gemUsDataByteCnt_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_gemUsIdleByteCnt_read
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
int32 dal_rtl9603cvd_gpon_gemUsIdleByteCnt_get(uint32 idx, uint64 *pIdleCntr)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "idx=%d",idx);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_FLOW() <=idx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pIdleCntr), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_array_read(RTL9603CVD_TCONT_IDLE_BYTE_STATr,
                         REG_ARRAY_INDEX_NONE,
                         idx,
                         (uint32 *)pIdleCntr))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    *pIdleCntr = ((*pIdleCntr >> 32) & 0xFFFFFFFF) |
                  ((*pIdleCntr & 0xFFFFFFFF) << 32);

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_gemUsIdleByteCnt_get */



/* Function Name:
 *      dal_rtl9603cvd_gpon_dbruPeriod_set
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
int32 dal_rtl9603cvd_gpon_dbruPeriod_set(uint32 periodTime)
{
    int32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "periodTime=%d",periodTime);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((15<=periodTime), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_GPON_DPRU_RPT_PRDr, RTL9603CVD_RPT_PRDf, &periodTime)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9603cvd_gpon_dbruPeriod_get
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
int32 dal_rtl9603cvd_gpon_dbruPeriod_get(uint32 *pPeriodTime)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL==pPeriodTime), RT_ERR_INPUT);

    if ((ret = reg_field_read(RTL9603CVD_GPON_DPRU_RPT_PRDr, RTL9603CVD_RPT_PRDf, pPeriodTime)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9603cvd_gpon_rogueOnt_set
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
int32 dal_rtl9603cvd_gpon_rogueOnt_set(rtk_enable_t rogOntState)
{
    int32 ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "rogOntState=%d",rogOntState);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= rogOntState), RT_ERR_INPUT);

    /* function body */
    if(ENABLED == rogOntState)          //diagnose enable
    {
        /*osal_print("Rcv Ds ploam DiagNose enable %d \r\n",value);*/
        /* set PRBS selection */
        data = 0xa0;
        if ((ret = reg_field_write(RTL9603CVD_WSDS_DIG_19r, RTL9603CVD_CFG_PRBS_TYPE_SELf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
            return ret;
        }
        /* set PRBS enable */
        data = 0x1;
        if ((ret = reg_field_write(RTL9603CVD_WSDS_DIG_1Ar, RTL9603CVD_CFG_PRBS_ENf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
            return ret;
        }
    }
    else    //diagnose disable
    {
        /*osal_print("Rcv Ds ploam DiagNose enable %d \r\n",value);*/
        /* set PRBS disable */
        data = 0x0;
        if ((ret = reg_field_write(RTL9603CVD_WSDS_DIG_1Ar, RTL9603CVD_CFG_PRBS_ENf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
            return ret;
        }
        /* set PRBS selection */
        data = 0x0;
        if ((ret = reg_field_write(RTL9603CVD_WSDS_DIG_19r, RTL9603CVD_CFG_PRBS_TYPE_SELf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
            return ret;
        }
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_rogueOnt_set */


/* Function Name:
 *      dal_rtl9603cvd_gpon_drainOutDefaultQueue_set
 * Description:
 *      Drain Out GPON all Queue
 * Input:
 *     rogOntState - enable or disable rogue ont
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9603cvd_gpon_drainOutDefaultQueue_set(void)
{
    int32 ret;
 //   uint32 data;
    rtk_ponmac_queue_t drn_queue;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* drainout all queue */
    drn_queue.queueId = RTL9603CVD_TCONT_QUEUE_MAX;
    for(drn_queue.schedulerId=0; drn_queue.schedulerId<RTL9603CVD_GPON_TCONT_MAX; drn_queue.schedulerId++)
    {
        if((ret = dal_rtl9603cvd_ponmac_queueDrainOut_set(&drn_queue))!=RT_ERR_OK)
        {
             RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
                return ret;
        }
    }

#if 0
    /* clear DS BANK0/1 */
    data = 1;
    if((ret = reg_field_write(RTL9603CVD_PON_PORT_CTRLr, RTL9603CVD_CLR_DS_WRAP_BANK0f, &data))!=RT_ERR_OK)
    {
         RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
    }
    if((ret = reg_field_write(RTL9603CVD_PON_PORT_CTRLr, RTL9603CVD_CLR_DS_WRAP_BANK1f, &data))!=RT_ERR_OK)
    {
         RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
    }

    data = 0;
    if((ret = reg_field_write(RTL9603CVD_PON_PORT_CTRLr, RTL9603CVD_CLR_DS_WRAP_BANK0f, &data))!=RT_ERR_OK)
    {
         RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
    }
    if((ret = reg_field_write(RTL9603CVD_PON_PORT_CTRLr, RTL9603CVD_CLR_DS_WRAP_BANK1f, &data))!=RT_ERR_OK)
    {
         RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
    }
#endif
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_drainOutDefaultQueue_set */


/* Function Name:
 *      dal_rtl9603cvd_gpon_autoDisTx_set
 * Description:
 *      Enable or Disable auto disable Tx function
 * Input:
 *     autoDisTxState - enable or disable rogue ont
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9603cvd_gpon_autoDisTx_set(rtk_enable_t state)
{
    uint32 data;
    uint32 addr;
    int32 ret;

    addr = (5 << 12) | (0x9b)*4 | 0x700000;
    if(ENABLED == state)
    {
        data=0;
        if ((ret = ioal_mem32_write(addr, data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        /* ENABLE DIS_TX, TX_SD, ToD, GPONPPS */
        data=0X3;
        if ((ret = reg_field_write(RTL9603CVD_IO_MODE_ENr, RTL9603CVD_OEM_ENf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        data=0;
        if ((ret = reg_field_write(RTL9603CVD_WSDS_DIG_18r, RTL9603CVD_CFG_FRC_TX_DISABLE_OPTICf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        data=1;
        if ((ret = reg_field_write(RTL9603CVD_WSDS_DIG_18r, RTL9603CVD_TX_DISABLE_OPTIC_OEf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        #if 1
        if((ret = rtk_gpio_state_set(34,DISABLED))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }
        #else
        data=0;
        if ((ret = reg_array_field_write(RTL9603CVD_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 34, RTL9603CVD_EN_GPIOf,&data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }
        #endif
    }
    else
    {
        data=0;
        if ((ret = ioal_mem32_write(addr, data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        /* DISABLE DIS_TX, TX_SD, ToD, GPONPPS */
        data=0X0;
        if ((ret = reg_field_write(RTL9603CVD_IO_MODE_ENr, RTL9603CVD_OEM_ENf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        data=0;
        if ((ret = reg_field_write(RTL9603CVD_WSDS_DIG_18r, RTL9603CVD_CFG_FRC_TX_DISABLE_OPTICf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        data=0;
        if ((ret = reg_field_write(RTL9603CVD_WSDS_DIG_18r, RTL9603CVD_TX_DISABLE_OPTIC_OEf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }

        #if 1
        if((ret = rtk_gpio_state_set(34,DISABLED))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }
        #else
        data=1;
        if ((ret = reg_array_field_write(RTL9603CVD_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 34, RTL9603CVD_EN_GPIOf,&data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            return ret;
        }
        #endif

    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9603cvd_gpon_scheInfo_get
 * Description:
 *      Get GPON scheduler information
 * Input:
 *     None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32
dal_rtl9603cvd_gpon_scheInfo_get(rtk_gpon_schedule_info_t *pScheInfo)
{

    /* no check Init status for these infor are no need initial, and will be used before gpon_init */
    /*RT_INIT_CHK(gpon_init);*/

    /* parameter check */
    RT_PARAM_CHK((NULL == pScheInfo), RT_ERR_NULL_POINTER);

    pScheInfo->omcc_tcont = RTL9603CVD_GPON_OMCI_TCONT_ID;
    pScheInfo->omcc_queue = RTL9603CVD_GPON_OMCI_QUEUE_ID;
    pScheInfo->omcc_flow = RTL9603CVD_GPON_OMCI_FLOW_ID;
    pScheInfo->max_tcont = HAL_MAX_NUM_OF_GPON_TCONT();
    pScheInfo->max_tcon_queue  = HAL_PONMAC_TCONT_QUEUE_MAX();
    pScheInfo->max_pon_queue = HAL_MAX_NUM_OF_PON_QUEUE();
    pScheInfo->max_flow = HAL_MAX_NUM_OF_GPON_FLOW();

    return RT_ERR_OK;

}

/* Function Name:
 *      dal_rtl9603cvd_gpon_dataPath_reset
 * Description:
 *      reset GPON data path.
 * Input:
 *     None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9603cvd_gpon_dataPath_reset(void)
{
    int32 ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    data = 0;
    if ((ret = reg_field_write(RTL9603CVD_WSDS_DIG_00r, RTL9603CVD_CFG_SFT_RSTB_GPONf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }
    data = 1;
    if ((ret = reg_field_write(RTL9603CVD_WSDS_DIG_00r, RTL9603CVD_CFG_SFT_RSTB_GPONf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dataPath_reset */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dsOmciCnt_get
 * Description:
 *      Get GPON DS OMCI counter
 * Input:
 *      none
 * Output:
 *      dsOmciCnt           - return value of OMCI counter
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 dal_rtl9603cvd_gpon_dsOmciCnt_get(rtk_gpon_ds_omci_t *dsOmciCnt)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == dsOmciCnt), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_OMCI_RX_PKT_CNTr,RTL9603CVD_OMCIRXPKTCNTf,&dsOmciCnt->rx_omci))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_read(RTL9603CVD_OMCI_RX_BYTE_CNTr,RTL9603CVD_OMCIRXBYTECNTf,&dsOmciCnt->rx_omci_byte))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_read(RTL9603CVD_OMCI_DROP_PKT_CNTr,RTL9603CVD_OMCIDROPPKTCNTf,&dsOmciCnt->rx_omci_drop))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_read(RTL9603CVD_OMCI_CRC_ERROR_PKT_CNTr,RTL9603CVD_OMCICRCERRORPKTCNTf,&dsOmciCnt->rx_omci_crc_err))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dsOmciCnt_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_usOmciCnt_get
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
int32 dal_rtl9603cvd_gpon_usOmciCnt_get(rtk_gpon_us_omci_t *usOmciCnt)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == usOmciCnt), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_OMCI_TX_PKT_CNTr,RTL9603CVD_OMCITXPKTCNTf,&usOmciCnt->tx_omci))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if((ret = reg_field_read(RTL9603CVD_OMCI_TX_BYTE_CNTr,RTL9603CVD_OMCITXBYTECNTf,&usOmciCnt->tx_omci_byte))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_usOmciCnt_get */

/* Function Name:
 *      dal_rtl9603cvd_gponGtcDsTodSuperFrame_set
 * Description:
 *      Set ToD superframe counter
 * Input:
 *     superframe - superframe counter used for time of dsy synchronization
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9603cvd_gponGtcDsTodSuperFrame_set(uint32 superframe)
{
    int32 ret;
    uint32 state;
    uint32 data;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "superframe=%d",superframe);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /*Hardware should decode the entire superframe info inside the pkt.
    If Assert tod while decoding finished, it will cause some offset from 125us.
    therefore, the design will assert tod on the begging of next 125us frame.*/
    data = superframe - 1;
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_DS_TOD_SUPERFRAME_CTRLr,RTL9603CVD_TOD_SF_CNTRf,&data))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    state = 0;
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_DS_TOD_SUPERFRAME_CTRLr,RTL9603CVD_TOD_WR_REQf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    state = 1;
    if((ret = reg_field_write(RTL9603CVD_GPON_GTC_DS_TOD_SUPERFRAME_CTRLr,RTL9603CVD_TOD_WR_REQf,&state))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gponGtcDsTodSuperFrame_set */

/* Function Name:
 *      dal_rtl9603cvd_gponGtcDsTodSuperFrame_get
 * Description:
 *      Get ToD superframe counter
 * Input:
 *      none
 * Output:
 *      pSuperframe           - return value of superframe counter
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 dal_rtl9603cvd_gponGtcDsTodSuperFrame_get(uint32 *pSuperframe)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSuperframe), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_GTC_DS_TOD_SUPERFRAME_CTRLr,RTL9603CVD_TOD_SF_CNTRf,pSuperframe))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gponGtcDsTodSuperFrame_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dbruBlockSize_get
 * Description:
 *      Get GPON DBRu block size
 * Input:
 *      none
 * Output:
 *      pBlockSize          - return value of block size
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 dal_rtl9603cvd_gpon_dbruBlockSize_get(uint32 *pBlockSize)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBlockSize), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = reg_field_read(RTL9603CVD_GPON_DPRU_RPT_PRDr,RTL9603CVD_DBA_BLKSIZEf,pBlockSize))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dbruBlockSize_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_dbruBlockSize_set
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
int32 dal_rtl9603cvd_gpon_dbruBlockSize_set(uint32 blockSize)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((255 < blockSize), RT_ERR_INPUT);

    /* function body */
    if ((ret = reg_field_write(RTL9603CVD_GPON_DPRU_RPT_PRDr,RTL9603CVD_DBA_BLKSIZEf,&blockSize)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_dbruBlockSize_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_flowctrl_adjust_byFlowNum
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
dal_rtl9603cvd_gpon_flowctrl_adjust_byFlowNum(uint32 flowNum)
{
    int32   ret=RT_ERR_INPUT;
    int32  onThreshold, offThreshold;
    int32  sidOnTh, sidOffTh;

    if(flowNum > (RTL9603CVD_CLASSIFY_SID_MAX-1))
    {
        RT_ERR(ret, (MOD_GPON|MOD_DAL), "");
        return ret;
    }

    if((ret = rtl9603cvd_raw_pbo_sidThreshold_get(0,&sidOnTh,&sidOffTh)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON|MOD_DAL), "");
        return ret;
    }

    if(flowNum == 0)
        flowNum = 1;

    onThreshold = sysOnTh - (flowNum-1)*sidOnTh;
    offThreshold = sysOffTh - (flowNum-1)*sidOnTh;

    if(onThreshold < ((int32)(sysOnTh/4)))
    {
        onThreshold = (sysOnTh/4);
        offThreshold = (sysOnTh/4)-(sidOnTh-sidOffTh)*8;
    }

    /*osal_printf("dal_rtl9601b_ponmac_flowctrl_adjust_byFlowNum flownum=%d, sysOnTh=%d, sysOffTh=%d, sidOnTh=%d, sidOffTh=%d, onThreshold=%d, offThreshold=%d\n", 
                flowNum, sysOnTh, sysOffTh, sidOnTh, sidOffTh, onThreshold, offThreshold);*/

    if((ret = rtl9603cvd_raw_pbo_globalThreshold_set(onThreshold, offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}/* end of dal_rtl9603cvd_gpon_flowctrl_adjust_byFlowNum */

/* Function Name:
 *      dal_rtl9603cvd_gpon_autoDG_set
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
int32 dal_rtl9603cvd_gpon_autoDG_set(uint32 state)
{
    uint32 data;
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    if(state != 0 && state != 1)
        return RT_ERR_INPUT;

    data = 0xcc19;
    if ((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_CFGr, RTL9603CVD_ENA_AUTO_DGf, &state)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    data = 0x0;
    if ((ret = reg_write(RTL9603CVD_GPON_GTC_US_WRITE_PROTECTr, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_autoDG_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_autoDG_get
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
int32 dal_rtl9603cvd_gpon_autoDG_get(uint32 *pState)
{
    uint32 tmp_data;
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);
    
    if ((ret = reg_read(RTL9603CVD_GPON_GTC_US_CFGr, &tmp_data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_GPON_GTC_US_CFGr, RTL9603CVD_ENA_AUTO_DGf, pState)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_autoDG_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_DgCnt_get
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
int32 dal_rtl9603cvd_gpon_DgCnt_get(uint32 *pCnt)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pCnt), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9603CVD_GPON_GTC_US_DGr, RTL9603CVD_DG_MSG_TX_CNTf, pCnt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_DgCnt_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_DgThreshold_set
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
int32 dal_rtl9603cvd_gpon_DgThreshold_set(uint32 threshold)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    RT_PARAM_CHK((threshold > 15), RT_ERR_INPUT);

    if ((ret = reg_field_write(RTL9603CVD_GPON_GTC_US_DGr, RTL9603CVD_DG_MSG_TX_CNT_THRESHOLDf, &threshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_DgThreshold_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_DgThreshold_get
 * Description:
 *      Get GPON auto dying gasp
 * Input:
 *      pThreshold          - pointer of config value of auto dying threshold
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 dal_rtl9603cvd_gpon_DgThreshold_get(uint32 *pThreshold)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThreshold), RT_ERR_NULL_POINTER);
    
    if ((ret = reg_field_read(RTL9603CVD_GPON_GTC_US_DGr, RTL9603CVD_DG_MSG_TX_CNT_THRESHOLDf, pThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_DgThreshold_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_forcePRBS_rx_set
 * Description:
 *      Set force PRBS RX status
 * Input:
 *      prbsCfg             - PRBS config
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9603cvd_gpon_forcePRBS_rx_set(rtk_gpon_prbs_t prbsCfg)
{
    int32   ret;
    uint32  data;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* function body */
    if (RTK_GPON_PRBS_OFF != prbsCfg)
    {
        switch (prbsCfg)
        {
            case RTK_GPON_PRBS_31:
                data = 0x40AF;
                break;
            case RTK_GPON_PRBS_23:
                data = 0x409E;
                break;
            case RTK_GPON_PRBS_15:
                data = 0x408D;
                break;
            case RTK_GPON_PRBS_7:
                data = 0x407C;
                break;
            default:
                return RT_ERR_OUT_OF_RANGE;
                break;
        }

        if (RT_ERR_OK != (ret = reg_field_write(RTL9603CVD_WSDS_DIG_19r, RTL9603CVD_CFG_PRBS_TYPE_SELf, &data)))
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x", ret);
            return ret;
        }
    }

    data = (RTK_GPON_PRBS_OFF == prbsCfg) ? 0 : 1;

    if (RT_ERR_OK != (ret = reg_field_write(RTL9603CVD_WSDS_DIG_1Ar, RTL9603CVD_CFG_PRBS_ENf, &data)))
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x", ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_forcePRBS_rx_set */

/* Function Name:
 *      dal_rtl9603cvd_gpon_forcePRBS_rx_get
 * Description:
 *      Get force PRBS RX status
 * Input:
 *      pPrbsCfg            - pointer of PRBS config
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9603cvd_gpon_forcePRBS_rx_get(rtk_gpon_prbs_t *pPrbsCfg)
{
    int32   ret;
    uint32  data;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPrbsCfg), RT_ERR_NULL_POINTER);

    /* function body */
    if (RT_ERR_OK != (ret = reg_field_read(RTL9603CVD_WSDS_DIG_1Ar, RTL9603CVD_CFG_PRBS_ENf, &data)))
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x", ret);
        return RT_ERR_FAILED;
    }

    if (data & 0x1)
    {
        if (RT_ERR_OK != (ret = reg_field_read(RTL9603CVD_WSDS_DIG_19r, RTL9603CVD_CFG_PRBS_TYPE_SELf, &data)))
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x", ret);
            return RT_ERR_FAILED;
        }

        switch (data & 0xF)
        {
            case 0xF:
                *pPrbsCfg = RTK_GPON_PRBS_31;
                break;
            case 0xE:
                *pPrbsCfg = RTK_GPON_PRBS_23;
                break;
            case 0xD:
                *pPrbsCfg = RTK_GPON_PRBS_15;
                break;
            case 0xC:
                *pPrbsCfg = RTK_GPON_PRBS_7;
                break;
            default:
                return RT_ERR_OUT_OF_RANGE;
                break;
        }
    }
    else
    {
        *pPrbsCfg = RTK_GPON_PRBS_OFF;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_forcePRBS_rx_get */

/* Function Name:
 *      dal_rtl9603cvd_gpon_prbsRxStatus_get
 * Description:
 *      Get the PRBS RX Status
 * Input:
 *      pPrbsRxStatus       - pointer of PRBS RX Status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *  	RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 dal_rtl9603cvd_gpon_prbsRxStatus_get(rtk_gpon_prbsRxStatus_t *pPrbsRxStatus)
{
    int32   ret;
    uint32  data=0, los=0, lof=0;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPrbsRxStatus), RT_ERR_NULL_POINTER);

    /* function body */
    memset(pPrbsRxStatus, 0x0, sizeof(rtk_gpon_prbsRxStatus_t));

    if ((ret = reg_read(RTL9603CVD_GPON_GTC_DS_INTR_STSr,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_get(RTL9603CVD_GPON_GTC_DS_INTR_STSr,RTL9603CVD_LOSf,&los,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_get(RTL9603CVD_GPON_GTC_DS_INTR_STSr,RTL9603CVD_LOFf,&lof,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

	if(los == 1)
        pPrbsRxStatus->prbsRxState = RTK_GPON_PRBS_RX_LOS;
	else if(los == 0 && lof == 0)
        pPrbsRxStatus->prbsRxState = RTK_GPON_PRBS_RX_NOTAVAILABLE;
	else
        pPrbsRxStatus->prbsRxState = RTK_GPON_PRBS_RX_READY;


	if ((ret = reg_field_read(RTL9603CVD_WSDS_DIG_1Br,RTL9603CVD_CFG_PRBS_ERRORSf,&pPrbsRxStatus->prbsRxErrCnt)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_GPON|MOD_DAL), "");
		return ret;
	}

    /*clear prbs error counter*/
    if (RT_ERR_OK != (ret = reg_field_read(RTL9603CVD_WSDS_DIG_19r, RTL9603CVD_CFG_PRBS_TYPE_SELf, &data)))
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x", ret);
        return RT_ERR_FAILED;
    }
    data |= 0x8000;
	if (RT_ERR_OK != (ret = reg_field_write(RTL9603CVD_WSDS_DIG_19r, RTL9603CVD_CFG_PRBS_TYPE_SELf, &data)))
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x", ret);
        return RT_ERR_FAILED;
    }
    data &= ~0x8000;
    if (RT_ERR_OK != (ret = reg_field_write(RTL9603CVD_WSDS_DIG_19r, RTL9603CVD_CFG_PRBS_TYPE_SELf, &data)))
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x", ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9603cvd_gpon_prbsRxStatus_get */

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
 *
 *
 * $Revision$
 * $Date$
 *
 * Purpose : GPON MAC register access APIs
 *
 * Feature : Provide the APIs to access GPON MAC
 *
 */

#include <hal/mac/reg.h>
#include <common/rt_error.h>
#include <dal/apollo/gpon/gpon_defs.h>
#include <dal/apollomp/raw/apollomp_raw.h>
#include <dal/apollomp/raw/apollomp_raw_gpon.h>

#define GPON_NO_CHECK  0

#define GPON_REG_16BITS_LOW                     0
#define GPON_REG_16BITS_HIGH                    8

#define GPON_REG_32BITS_0                       0
#define GPON_REG_32BITS_1                       8
#define GPON_REG_32BITS_2                       16
#define GPON_REG_32BITS_3                       24

#define GPON_REG_STATUS_CLEAR                   0
#define GPON_REG_STATUS_SET                     1

#define GPON_REG_OPERRATION_NO                  0
#define GPON_REG_OPERRATION_WRITE               1
#define GPON_REG_OPERRATION_READ                2
#define GPON_REG_OPERRATION_CLEAN               3

#define GPON_REG_BITS_CAM_OP_IDX                0
#define GPON_REG_BITS_CAM_OP_MODE               8
#define GPON_REG_BITS_CAM_OP_HIT                13
#define GPON_REG_BITS_CAM_OP_COMPLETE           14
#define GPON_REG_BITS_CAM_OP_REQ                15

#define GPON_REG_BITS_INDIRECT_ACK              15

#define GPON_REG_EQD_FRAME_LEN                  (19440*8)

#define GPON_REG_PTN_GTC_US_WRPROTECT_ENA       0xCC19
#define GPON_REG_PTN_GTC_US_WRPROTECT_DIS       0x0000

#define GPON_REG_TYPE_GTC_US_PLOAM_NRM          0x0
#define GPON_REG_TYPE_GTC_US_PLOAM_URG          0x1
#define GPON_REG_TYPE_GTC_US_PLOAM_DYINGGASP    0x5
#define GPON_REG_TYPE_GTC_US_PLOAM_SN           0x6
#define GPON_REG_TYPE_GTC_US_PLOAM_NOMSG        0x7



#define WAIT_ZERO                               4096

static void apollo_raw_gpon_delay(uint32 step)
{
    uint32 delay = step;
    while(delay)
    {
        delay--;
    }
}


static uint32 gpon_get_bit(uint32 value, uint32 bit)
{
    return ((value>>bit)&0x01);
}

#define WAIT_FOREVER                            (0xFFFFFFFF - 1)
#define WAIT_MAX_TIMES                          WAIT_FOREVER

static int32 apollo_raw_gpon_indirect_wait(uint32 reg)
{
#if GPON_NO_CHECK /*defined(CONFIG_SDK_ASICDRV_TEST)*/ /* for init pass, scott */
    apollo_raw_gpon_delay(WAIT_ZERO);
    return TRUE;
#else
    uint32 timeout;
    uint32 ind;

    for(timeout=0;timeout<=WAIT_MAX_TIMES;timeout++)
    {
        reg_read(reg, &ind);
        if(gpon_get_bit(ind,GPON_REG_BITS_INDIRECT_ACK))
        {
            return TRUE;
        }
        /*osal_printf("apollo_raw_gpon_indirect_wait %d\n\r",timeout);*/
    }
    return FALSE;
#endif
}

static int32 apollo_raw_gpon_cam_wait(uint32 reg, int32 waithit)
{
#if GPON_NO_CHECK /*defined(CONFIG_SDK_ASICDRV_TEST)*/ /* for init pass, scott */
    apollo_raw_gpon_delay(WAIT_ZERO);
    return TRUE;
#else
    uint32 timeout;
    uint32 ind;

    for(timeout=0;timeout<=WAIT_MAX_TIMES;timeout++)
    {
        reg_read(reg, &ind);
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
        osal_printf("apollo_raw_gpon_cam_wait %d\n\r",timeout);
    }
      return FALSE;
#endif
}

void apollo_raw_gpon_intr_disableAll(void)
{
	uint32 data;
    uint32 tmp;

    data = GPON_REG_STATUS_CLEAR;

    /* close top int mask */
    reg_write(APOLLOMP_GPON_INTR_MASKr,&data);

    /* clear all mask */
    reg_write(APOLLOMP_GPON_GTC_DS_INTR_MASKr,&data);
#if 0 /* GEM DS interrupt is removed */
    reg_write(APOLLOMP_GPON_GEM_DS_INTR_MASKr,&data);
#endif
    reg_write(APOLLOMP_GPON_GTC_US_INTR_MASKr,&data);

    /* clear unused mask */
	data = GPON_REG_STATUS_CLEAR;
	reg_write(APOLLOMP_GPON_AES_INTR_MASKr, &data);

    /* open top int mask */
    data=0;
    tmp = GPON_REG_STATUS_SET;
    reg_field_set(APOLLOMP_GPON_INTR_MASKr, APOLLOMP_GTC_DS_Mf, &tmp, &data);
#if 0
    reg_field_set(APOLLOMP_GPON_INTR_MASKr, APOLLOMP_GEM_DS_Mf, &tmp, &data);
#endif
    reg_field_set(APOLLOMP_GPON_INTR_MASKr, APOLLOMP_GTC_US_Mf, &tmp, &data);
    reg_write(APOLLOMP_GPON_INTR_MASKr,&data);
}

int32 apollo_raw_gpon_top_gtcDsIntr_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_INTR_STSr, APOLLOMP_GTC_DS_INTRf, &value, &status);
    return value;
}

int32 apollo_raw_gpon_top_gemDsIntr_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_INTR_STSr, APOLLOMP_GEM_DS_INTRf, &value, &status);
    return value;
}

int32 apollo_raw_gpon_top_gtcUsIntr_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_INTR_STSr, APOLLOMP_GTC_US_INTRf, &value, &status);
    return value;
}

int32 apollo_raw_gpon_top_gemUsIntr_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_INTR_STSr, APOLLOMP_GEM_US_INTRf, &value, &status);
    return value;
}

void apollo_raw_gpon_reset_write(int32 reset)
{
	reg_write(APOLLOMP_GPON_RESETr,&reset);
}

int32 apollo_raw_gpon_restDone_wait(void)
{
#if GPON_NO_CHECK /*defined(CONFIG_SDK_ASICDRV_TEST)*/ /* for init pass, scott */
    apollo_raw_gpon_delay(WAIT_ZERO);
    return TRUE;
#else
    uint32 timeout;
    uint32 ind=0;

    for(timeout=0;timeout<=WAIT_MAX_TIMES;timeout++)
    {
		reg_field_read(APOLLOMP_GPON_RESETr,APOLLOMP_RST_DONEf,&ind);
        if(1 == ind)
        {
            return TRUE;
        }
        osal_printf("apollomp_raw_gpon_restDone_wait %d\n\r",timeout);
    }
    return FALSE;
#endif
}

uint32 apollo_raw_gpon_version_read(void)
{
	uint32 data=0;
	reg_read(APOLLOMP_GPON_VERSIONr, &data);
	return data;
}

uint32 apollo_raw_gpon_test_read(void)
{
	uint32 data=0;
	reg_read(APOLLOMP_GPON_TESTr, &data);
	return data;
}

void apollo_raw_gpon_test_write(uint32 value)
{
	reg_write(APOLLOMP_GPON_TESTr, &value);
}

int32 apollo_raw_gpon_top_mask_read(uint32 *pValue)
{
    return reg_read(APOLLOMP_GPON_INTR_MASKr, pValue);
}

void apollo_raw_gpon_top_mask_write(uint32 value)
{
    reg_write(APOLLOMP_GPON_INTR_MASKr,&value);
}

int32 apollo_raw_gpon_top_intr_read(uint32 *pValue)
{
    return reg_read(APOLLOMP_GPON_INTR_STSr, pValue);
}

int32 apollo_raw_gpon_gtcDs_intr_read(uint32 *pValue)
{
    return reg_read(APOLLOMP_GPON_GTC_DS_INTR_DLTr, pValue);
}

int32 apollo_raw_gpon_gtcDsIntr_snReq_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_GTC_DS_INTR_DLTr, APOLLOMP_SN_REQ_HISf, &value, &status);
    return value;
}

int32 apollo_raw_gpon_gtcDsIntr_rangReq_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_GTC_DS_INTR_DLTr, APOLLOMP_RNG_REQ_HISf, &value, &status);
    return value;
}

int32 apollo_raw_gpon_gtcDsIntr_ploam_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_GTC_DS_INTR_DLTr, APOLLOMP_PLM_BUF_REQf, &value, &status);
    return value;
}

int32 apollo_raw_gpon_gtcDsIntr_los_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_GTC_DS_INTR_DLTr, APOLLOMP_LOS_DLTf, &value, &status);
    return value;
}

int32 apollo_raw_gpon_gtcDsIntr_lof_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_GTC_DS_INTR_DLTr, APOLLOMP_LOF_DLTf, &value, &status);
    return value;
}

int32 apollo_raw_gpon_gtcDsIntr_lom_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_GTC_DS_INTR_DLTr, APOLLOMP_LOM_DLTf, &value, &status);
    return value;
}

int32 apollo_raw_gpon_gtcDsIntr_fec_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_GTC_DS_INTR_DLTr, APOLLOMP_DS_FEC_STA_DLTf, &value, &status);
    return value;
}

int32 apollo_raw_gpon_gtcDs_mask_read(uint32 *pValue)
{
    return reg_read(APOLLOMP_GPON_GTC_DS_INTR_MASKr, pValue);
}

void apollo_raw_gpon_gtcDs_mask_write(uint32 value)
{
    reg_write(APOLLOMP_GPON_GTC_DS_INTR_MASKr,&value);
}

void apollo_raw_gpon_gtcDsMask_ploam_write(int32 value)
{
	reg_field_write(APOLLOMP_GPON_GTC_DS_INTR_MASKr,APOLLOMP_PLM_BUF_Mf,&value);
}

void apollo_raw_gpon_gtcDsMask_rangReq_write(int32 value)
{
	reg_field_write(APOLLOMP_GPON_GTC_DS_INTR_MASKr,APOLLOMP_RNG_REQ_Mf,&value);
}

void apollo_raw_gpon_gtcDsMask_snReq_write(int32 value)
{
	reg_field_write(APOLLOMP_GPON_GTC_DS_INTR_MASKr,APOLLOMP_SN_REQ_Mf,&value);
}

void apollo_raw_gpon_gtcDsMask_los_write(int32 value)
{
	reg_field_write(APOLLOMP_GPON_GTC_DS_INTR_MASKr,APOLLOMP_LOS_Mf,&value);
}

void apollo_raw_gpon_gtcDsMask_lof_write(int32 value)
{
	reg_field_write(APOLLOMP_GPON_GTC_DS_INTR_MASKr,APOLLOMP_LOF_Mf,&value);
}

void apollo_raw_gpon_gtcDsMask_lom_write(int32 value)
{
	reg_field_write(APOLLOMP_GPON_GTC_DS_INTR_MASKr,APOLLOMP_LOM_Mf,&value);
}

void apollo_raw_gpon_gtcDsMask_fec_write(int32 value)
{
	reg_field_write(APOLLOMP_GPON_GTC_DS_INTR_MASKr,APOLLOMP_DS_FEC_STA_Mf,&value);
}

int32 apollo_raw_gpon_gtcDs_los_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GTC_DS_INTR_STSr,APOLLOMP_LOSf,pValue);
}

int32 apollo_raw_gpon_gtcDs_lof_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GTC_DS_INTR_STSr,APOLLOMP_LOFf,pValue);
}

int32 apollo_raw_gpon_gtcDs_lom_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GTC_DS_INTR_STSr,APOLLOMP_LOMf,pValue);
}

int32 apollo_raw_gpon_gtcDs_fec_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GTC_DS_INTR_STSr,APOLLOMP_DS_FEC_STSf,pValue);
}

void apollo_raw_gpon_onuId_write(uint8 value)
{
    uint32 tmp = value;

	reg_field_write(APOLLOMP_GPON_GTC_DS_ONU_ID_STATUSr,APOLLOMP_ONU_IDf,&tmp);

	reg_field_write(APOLLOMP_GPON_GTC_US_ONU_IDr,APOLLOMP_ONU_IDf,&tmp);
}

int apollo_raw_gpon_onuId_read(uint8 *pValue)
{
    uint32 tmp;
    int32 ret;

	ret = reg_field_read(APOLLOMP_GPON_GTC_DS_ONU_ID_STATUSr, APOLLOMP_ONU_IDf, &tmp);

    *pValue = tmp;

    return ret;
}

void apollo_raw_gpon_onuStatus_write(uint8 value)
{
    uint32 tmp = value;

	reg_field_write(APOLLOMP_GPON_GTC_DS_ONU_ID_STATUSr,APOLLOMP_ONU_STATEf,&tmp);
}

int apollo_raw_gpon_onuStatus_read(uint8 *pValue)
{
    uint32 tmp;
    int32 ret;

	ret = reg_field_read(APOLLOMP_GPON_GTC_DS_ONU_ID_STATUSr, APOLLOMP_ONU_STATEf, &tmp);

    *pValue = tmp;

    return ret;
}

void apollo_raw_gpon_dsBwmap_crcCheck_enable(int32 value)
{
	reg_field_write(APOLLOMP_GPON_GTC_DS_CFGr,APOLLOMP_CHK_BWM_CRCf,&value);
}

int32 apollo_raw_gpon_dsBwmap_crcCheck_enable_read(int32 *value)
{
    return reg_field_read(APOLLOMP_GPON_GTC_DS_CFGr,APOLLOMP_CHK_BWM_CRCf,value);
}

/* Added in GPON_MAC_SWIO_v1.1 */
void apollo_raw_gpon_dsBwmap_filterOnuId_write(int32 value)
{
    reg_field_write(APOLLOMP_GPON_GTC_DS_CFGr,APOLLOMP_BWM_FILT_ONUIDf,&value);
}

int32 apollo_raw_gpon_dsBwmap_filterOnuId_read(int32 *value)
{
    return reg_field_read(APOLLOMP_GPON_GTC_DS_CFGr,APOLLOMP_BWM_FILT_ONUIDf,value);
}

void apollo_raw_gpon_dsBwmap_plend_write(int32 value)
{

    reg_field_write(APOLLOMP_GPON_GTC_DS_CFGr,APOLLOMP_PLEND_STRICT_MODEf,&value);
}

int32 apollo_raw_gpon_dsBwmap_plend_read(int32 *value)
{
    return reg_field_read(APOLLOMP_GPON_GTC_DS_CFGr,APOLLOMP_PLEND_STRICT_MODEf,value);
}

void apollo_raw_gpon_dsScramble_enable(int32 value)
{
    uint32 tmp = !value;

	reg_field_write(APOLLOMP_GPON_GTC_DS_CFGr,APOLLOMP_DESCRAM_DISf,&tmp);
}

int32 apollo_raw_gpon_dsScramble_enable_read(int32 *value)
{
    uint32 data;
    int32 ret;
    ret = reg_field_read(APOLLOMP_GPON_GTC_DS_CFGr,APOLLOMP_DESCRAM_DISf,&data);
    *value = !data;
    return ret;
}

void apollo_raw_gpon_dsFec_bypass(int32 value)
{
	reg_field_write(APOLLOMP_GPON_GTC_DS_CFGr,APOLLOMP_FEC_CORRECT_DISf,&value);
}

int32 apollo_raw_gpon_dsFec_bypass_read(int32 *value)
{
    return reg_field_read(APOLLOMP_GPON_GTC_DS_CFGr,APOLLOMP_FEC_CORRECT_DISf,value);
}

void apollo_raw_gpon_dsFecThrd_write(uint8 value)
{
    uint32 tmp = value;

	reg_field_write(APOLLOMP_GPON_GTC_DS_CFGr,APOLLOMP_FEC_DET_THRSHf,&tmp);
}

int32 apollo_raw_gpon_dsFecThrd_read(uint8 *value)
{
    uint32 tmp ;
    int32 ret;

    ret = reg_field_read(APOLLOMP_GPON_GTC_DS_CFGr,APOLLOMP_FEC_DET_THRSHf,&tmp);

    *value = (uint8)tmp;
    return ret;
}

void apollo_raw_gpon_extraSN_write(uint8 value)
{
    uint32 tmp = value;

	reg_field_write(APOLLOMP_GPON_GTC_DS_CFGr,APOLLOMP_EXTRA_SN_TXf,&tmp);
}

void apollo_raw_gpon_dsPloam_nomsg_write(uint8 value)
{
    uint32 tmp=value;

    reg_field_write(APOLLOMP_GPON_GTC_DS_PLOAM_CFGr,APOLLOMP_PLM_DS_NOMSG_IDf,&tmp);
}

void apollo_raw_gpon_dsPloam_onuIdFilter_write(int32 value)
{
	reg_field_write(APOLLOMP_GPON_GTC_DS_PLOAM_CFGr,APOLLOMP_PLM_DS_ONUID_FLT_ENf,&value);
}

int32 apollo_raw_gpon_dsPloam_onuIdFilter_read(int32 *value)
{
    return reg_field_read(APOLLOMP_GPON_GTC_DS_PLOAM_CFGr,APOLLOMP_PLM_DS_ONUID_FLT_ENf,value);
}

void apollo_raw_gpon_dsPloam_bcAccept_write(int32 value)
{
	reg_field_write(APOLLOMP_GPON_GTC_DS_PLOAM_CFGr,APOLLOMP_PLM_BC_ACC_ENf,&value);
}

int32 apollo_raw_gpon_dsPloam_bcAccept_read(int32 *value)
{
    return reg_field_read(APOLLOMP_GPON_GTC_DS_PLOAM_CFGr,APOLLOMP_PLM_BC_ACC_ENf,value);
}

/* Added in GPON_MAC_SWIO_v1.1 */
void apollo_raw_gpon_dsPloam_dropCrc_write(int32 value)
{
    reg_field_write(APOLLOMP_GPON_GTC_DS_PLOAM_CFGr,APOLLOMP_PLM_DROP_CRCEf,&value);
}

int32 apollo_raw_gpon_dsPloam_dropCrc_read(int32 *value)
{
    return reg_field_read(APOLLOMP_GPON_GTC_DS_PLOAM_CFGr,APOLLOMP_PLM_DROP_CRCEf,value);
}

int32 apollo_raw_gpon_cdrLos_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GTC_DS_LOS_CFG_STSr, APOLLOMP_CDR_LOS_SIGf, pValue);
}

int32 apollo_raw_gpon_optLos_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GTC_DS_LOS_CFG_STSr, APOLLOMP_OPTIC_LOS_SIGf, pValue);
}

void apollo_raw_gpon_losCfg_write(int32 opten, int32 optpolar, int32 cdren, int32 cdrpolar, int32 filter)
{
    uint32 value;
	reg_read(APOLLOMP_GPON_GTC_DS_LOS_CFG_STSr,&value);
	reg_field_set(APOLLOMP_GPON_GTC_DS_LOS_CFG_STSr,APOLLOMP_OPTIC_LOS_ENf,&opten,&value);
	reg_field_set(APOLLOMP_GPON_GTC_DS_LOS_CFG_STSr,APOLLOMP_OPTIC_LOS_POLARf,&optpolar,&value);
	reg_field_set(APOLLOMP_GPON_GTC_DS_LOS_CFG_STSr,APOLLOMP_CDR_LOS_ENf,&cdren,&value);
	reg_field_set(APOLLOMP_GPON_GTC_DS_LOS_CFG_STSr,APOLLOMP_CDR_LOS_POLARf,&cdrpolar,&value);
	reg_field_set(APOLLOMP_GPON_GTC_DS_LOS_CFG_STSr,APOLLOMP_LOS_FILTER_ENf,&filter,&value);
	reg_write(APOLLOMP_GPON_GTC_DS_LOS_CFG_STSr,&value);
}

int32 apollo_raw_gpon_losCfg_read(int32 *opten, int32 *optpolar, int32 *cdren, int32 *cdrpolar, int32 *filter)
{
    uint32 data;
	reg_read(APOLLOMP_GPON_GTC_DS_LOS_CFG_STSr,&data);
	reg_field_get(APOLLOMP_GPON_GTC_DS_LOS_CFG_STSr,APOLLOMP_OPTIC_LOS_ENf,(uint32*)opten,&data);
	reg_field_get(APOLLOMP_GPON_GTC_DS_LOS_CFG_STSr,APOLLOMP_OPTIC_LOS_POLARf,(uint32*)optpolar,&data);
	reg_field_get(APOLLOMP_GPON_GTC_DS_LOS_CFG_STSr,APOLLOMP_CDR_LOS_ENf,(uint32*)cdren,&data);
	reg_field_get(APOLLOMP_GPON_GTC_DS_LOS_CFG_STSr,APOLLOMP_CDR_LOS_POLARf,(uint32*)cdrpolar,&data);
	reg_field_get(APOLLOMP_GPON_GTC_DS_LOS_CFG_STSr,APOLLOMP_LOS_FILTER_ENf,(uint32*)filter,&data);

    return RT_ERR_OK;
}

int32 apollo_raw_gpon_superframe_read(uint32 * value)
{
    reg_read(APOLLOMP_GPON_GTC_DS_SUPERFRAME_CNTr,value);
    return RT_ERR_OK;
}

int32 apollo_raw_gpon_dsPloam_read(uint8 *value)
{
    uint32 ploam,i;
    uint32 tmp,data;

    reg_field_read(APOLLOMP_GPON_GTC_DS_PLOAM_INDr,APOLLOMP_PLM_BUF_EMPTYf,&tmp);
    if(tmp==GPON_REG_STATUS_CLEAR)
    {
        for(i=0;i<6;i++)
        {
            reg_array_field_read(APOLLOMP_GPON_GTC_DS_PLOAM_MSGr,
                             REG_ARRAY_INDEX_NONE,
                             i,
                             APOLLOMP_PLOAM_RDATAf,
                             &ploam);
            value[2*i+0] = (ploam>>GPON_REG_16BITS_HIGH)&0xFF;
            value[2*i+1] = (ploam>>GPON_REG_16BITS_LOW)&0xFF;
        }
		data = GPON_REG_STATUS_CLEAR;
        reg_write(APOLLOMP_GPON_GTC_DS_PLOAM_INDr,&data);
		data = GPON_REG_STATUS_SET;
        reg_write(APOLLOMP_GPON_GTC_DS_PLOAM_INDr,&data);
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_PLOAM_QUEUE_EMPTY;
    }
}

int32 apollo_raw_gpon_tcont_read(uint32 idx, uint32 *value)
{
    uint32 op = 0;
    uint32 tmp_val;

    tmp_val = GPON_REG_OPERRATION_READ;
    reg_field_set(APOLLOMP_GPON_GTC_DS_ALLOC_INDr, APOLLOMP_ALLOCID_OP_MODEf, &tmp_val, &op);
    tmp_val = idx;
    reg_field_set(APOLLOMP_GPON_GTC_DS_ALLOC_INDr, APOLLOMP_ALLOCID_OP_IDXf, &tmp_val, &op);
    tmp_val = GPON_REG_STATUS_CLEAR;
    reg_field_set(APOLLOMP_GPON_GTC_DS_ALLOC_INDr, APOLLOMP_ALLOCID_OP_REQf, &tmp_val, &op);

    reg_write(APOLLOMP_GPON_GTC_DS_ALLOC_INDr,&op);

    tmp_val = GPON_REG_STATUS_SET;
    reg_field_set(APOLLOMP_GPON_GTC_DS_ALLOC_INDr, APOLLOMP_ALLOCID_OP_REQf, &tmp_val, &op);

    reg_write(APOLLOMP_GPON_GTC_DS_ALLOC_INDr,&op);

    if(apollo_raw_gpon_cam_wait(APOLLOMP_GPON_GTC_DS_ALLOC_INDr,TRUE))
    {
		reg_read(APOLLOMP_GPON_GTC_DS_ALLOC_RDr,value);
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
}

int32 apollo_raw_gpon_tcont_write(uint32 idx, uint32 value)
{
    uint32 op = 0;
    uint32 tmp_val;

    tmp_val = GPON_REG_OPERRATION_WRITE;
    reg_field_set(APOLLOMP_GPON_GTC_DS_ALLOC_INDr, APOLLOMP_ALLOCID_OP_MODEf, &tmp_val, &op);
    tmp_val = idx;
    reg_field_set(APOLLOMP_GPON_GTC_DS_ALLOC_INDr, APOLLOMP_ALLOCID_OP_IDXf, &tmp_val, &op);
    tmp_val = GPON_REG_STATUS_CLEAR;
    reg_field_set(APOLLOMP_GPON_GTC_DS_ALLOC_INDr, APOLLOMP_ALLOCID_OP_REQf, &tmp_val, &op);

    reg_write(APOLLOMP_GPON_GTC_DS_ALLOC_INDr,&op);

    reg_field_write(APOLLOMP_GPON_GTC_DS_ALLOC_WRr,APOLLOMP_ALLOCID_OP_WDATAf,&value);

	tmp_val = GPON_REG_STATUS_SET;
    reg_field_set(APOLLOMP_GPON_GTC_DS_ALLOC_INDr, APOLLOMP_ALLOCID_OP_REQf, &tmp_val, &op);

    reg_write(APOLLOMP_GPON_GTC_DS_ALLOC_INDr,&op);

    if(apollo_raw_gpon_cam_wait(APOLLOMP_GPON_GTC_DS_ALLOC_INDr,FALSE))
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
}

int32 apollo_raw_gpon_tcont_clean(uint32 idx)
{
    uint32 op = 0;
    uint32 tmp_val;

    tmp_val = GPON_REG_OPERRATION_CLEAN;
    reg_field_set(APOLLOMP_GPON_GTC_DS_ALLOC_INDr, APOLLOMP_ALLOCID_OP_MODEf, &tmp_val, &op);
    tmp_val = idx;
    reg_field_set(APOLLOMP_GPON_GTC_DS_ALLOC_INDr, APOLLOMP_ALLOCID_OP_IDXf, &tmp_val, &op);
    tmp_val = GPON_REG_STATUS_CLEAR;
    reg_field_set(APOLLOMP_GPON_GTC_DS_ALLOC_INDr, APOLLOMP_ALLOCID_OP_REQf, &tmp_val, &op);

    reg_write(APOLLOMP_GPON_GTC_DS_ALLOC_INDr,&op);

	tmp_val = GPON_REG_STATUS_SET;
    reg_field_set(APOLLOMP_GPON_GTC_DS_ALLOC_INDr, APOLLOMP_ALLOCID_OP_REQf, &tmp_val, &op);

    reg_write(APOLLOMP_GPON_GTC_DS_ALLOC_INDr,&op);

    if(apollo_raw_gpon_cam_wait(APOLLOMP_GPON_GTC_DS_ALLOC_INDr,FALSE))
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
}

int32 apollo_raw_gpon_dsPort_read(uint32 idx, uint32 *value, uint32 *cfg)
{
    uint32 op = 0;
    uint32 tmp_val;
    int32  ret;

    tmp_val = GPON_REG_OPERRATION_READ;
    reg_field_set(APOLLOMP_GPON_GTC_DS_PORT_INDr, APOLLOMP_PORTID_OP_MODEf, &tmp_val, &op);
    tmp_val = idx;
    reg_field_set(APOLLOMP_GPON_GTC_DS_PORT_INDr, APOLLOMP_PORTID_OP_IDXf, &tmp_val, &op);
    tmp_val = GPON_REG_STATUS_CLEAR;
    reg_field_set(APOLLOMP_GPON_GTC_DS_PORT_INDr, APOLLOMP_PORTID_OP_REQf, &tmp_val, &op);

    reg_write(APOLLOMP_GPON_GTC_DS_PORT_INDr,&op);

    tmp_val = GPON_REG_STATUS_SET;
    reg_field_set(APOLLOMP_GPON_GTC_DS_PORT_INDr, APOLLOMP_PORTID_OP_REQf, &tmp_val, &op);

    reg_write(APOLLOMP_GPON_GTC_DS_PORT_INDr,&op);

    if(apollo_raw_gpon_cam_wait(APOLLOMP_GPON_GTC_DS_PORT_INDr,TRUE))
    {
#if defined(CONFIG_SDK_ASICDRV_TEST)
		reg_field_read(APOLLOMP_GPON_GTC_DS_PORT_WRr,APOLLOMP_PORTID_OP_WDATAf,value);
#else
		reg_field_read(APOLLOMP_GPON_GTC_DS_PORT_RDr,APOLLOMP_PORTID_OP_RDATAf,value);
#endif
        ret = reg_array_field_read(APOLLOMP_GPON_GTC_DS_TRAFFIC_CFGr,
                             REG_ARRAY_INDEX_NONE,
                             idx,
                             APOLLOMP_TRAFFIC_TYPE_CFGf,
                             cfg);
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
}

int32 apollo_raw_gpon_dsPort_write(uint32 idx, uint32 value, uint32 cfg)
{
    uint32 op = 0;
    uint32 tmp_val;

    tmp_val = GPON_REG_OPERRATION_WRITE;
    reg_field_set(APOLLOMP_GPON_GTC_DS_PORT_INDr, APOLLOMP_PORTID_OP_MODEf, &tmp_val, &op);
    tmp_val = idx;
    reg_field_set(APOLLOMP_GPON_GTC_DS_PORT_INDr, APOLLOMP_PORTID_OP_IDXf, &tmp_val, &op);
    tmp_val = GPON_REG_STATUS_CLEAR;
    reg_field_set(APOLLOMP_GPON_GTC_DS_PORT_INDr, APOLLOMP_PORTID_OP_REQf, &tmp_val, &op);

    reg_write(APOLLOMP_GPON_GTC_DS_PORT_INDr,&op);

    reg_field_write(APOLLOMP_GPON_GTC_DS_PORT_WRr,APOLLOMP_PORTID_OP_WDATAf,&value);

    tmp_val = GPON_REG_STATUS_SET;
    reg_field_set(APOLLOMP_GPON_GTC_DS_PORT_INDr, APOLLOMP_PORTID_OP_REQf, &tmp_val, &op);

    reg_write(APOLLOMP_GPON_GTC_DS_PORT_INDr,&op);

    if(apollo_raw_gpon_cam_wait(APOLLOMP_GPON_GTC_DS_PORT_INDr,FALSE))
    {
        reg_array_field_write(APOLLOMP_GPON_GTC_DS_TRAFFIC_CFGr,
                              REG_ARRAY_INDEX_NONE,
                              idx,
                              APOLLOMP_TRAFFIC_TYPE_CFGf,
                              &cfg);

        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
}

int32 apollo_raw_gpon_dsPort_clean(uint32 idx)
{
    uint32 op = 0;
    uint32 tmp_val;
    uint32 cfg=0;

    tmp_val = GPON_REG_OPERRATION_CLEAN;
    reg_field_set(APOLLOMP_GPON_GTC_DS_PORT_INDr, APOLLOMP_PORTID_OP_MODEf, &tmp_val, &op);
    tmp_val = idx;
    reg_field_set(APOLLOMP_GPON_GTC_DS_PORT_INDr, APOLLOMP_PORTID_OP_IDXf, &tmp_val, &op);
    tmp_val = GPON_REG_STATUS_CLEAR;
    reg_field_set(APOLLOMP_GPON_GTC_DS_PORT_INDr, APOLLOMP_PORTID_OP_REQf, &tmp_val, &op);

    reg_write(APOLLOMP_GPON_GTC_DS_PORT_INDr,&op);

    tmp_val = GPON_REG_STATUS_SET;
    reg_field_set(APOLLOMP_GPON_GTC_DS_PORT_INDr, APOLLOMP_PORTID_OP_REQf, &tmp_val, &op);

    reg_write(APOLLOMP_GPON_GTC_DS_PORT_INDr,&op);

    if(apollo_raw_gpon_cam_wait(APOLLOMP_GPON_GTC_DS_PORT_INDr,FALSE))
    {
        reg_array_field_write(APOLLOMP_GPON_GTC_DS_TRAFFIC_CFGr,
                              REG_ARRAY_INDEX_NONE,
                              idx,
                              APOLLOMP_TRAFFIC_TYPE_CFGf,
                              &cfg);
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
}

int32 apollo_raw_gpon_dsPort_pktCnt_read(uint32 idx, uint32 *value)
{
    uint32 ind = 0;
    uint32 tmp_val;

    tmp_val = idx;
    reg_field_set(APOLLOMP_GPON_GTC_DS_PORT_CNTR_INDr, APOLLOMP_GEM_CNTR_IDXf, &tmp_val, &ind);
    tmp_val = GPON_REG_STATUS_CLEAR;
    reg_field_set(APOLLOMP_GPON_GTC_DS_PORT_CNTR_INDr, APOLLOMP_GEM_CNTR_RSELf, &tmp_val, &ind);

    reg_write(APOLLOMP_GPON_GTC_DS_PORT_CNTR_INDr,&ind);

    if(apollo_raw_gpon_indirect_wait(APOLLOMP_GPON_GTC_DS_PORT_CNTR_INDr))
    {
		return reg_field_read(APOLLOMP_GPON_GTC_DS_PORT_CNTR_STATr,APOLLOMP_GEM_CNTRf,value);
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
}

int32 apollo_raw_gpon_dsPort_byteCnt_read(uint32 idx, uint32 *value)
{
    uint32 ind = 0;
    uint32 tmp_val;

    tmp_val = idx;
    reg_field_set(APOLLOMP_GPON_GTC_DS_PORT_CNTR_INDr, APOLLOMP_GEM_CNTR_IDXf, &tmp_val, &ind);
    tmp_val = GPON_REG_STATUS_SET;
    reg_field_set(APOLLOMP_GPON_GTC_DS_PORT_CNTR_INDr, APOLLOMP_GEM_CNTR_RSELf, &tmp_val, &ind);

    reg_write(APOLLOMP_GPON_GTC_DS_PORT_CNTR_INDr,&ind);

    if(apollo_raw_gpon_indirect_wait(APOLLOMP_GPON_GTC_DS_PORT_CNTR_INDr))
    {
		return reg_field_read(APOLLOMP_GPON_GTC_DS_PORT_CNTR_STATr,APOLLOMP_GEM_CNTRf,value);
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
}

int32 apollo_raw_gpon_dsGtc_miscCnt_read(apollo_raw_gpon_dsGtc_pmMiscType_t idx, uint32 *pValue)
{
    int32 ret;

    switch(idx)
    {
        case GPON_REG_DSGTCPMMISC_BIP_BLOCK_ERR:
            ret = reg_field_read(APOLLOMP_GPON_GTC_DS_MISC_CNTR_BIP_ERR_BLKr,APOLLOMP_CNTR_DS_BIP_ERR_BLOCKf,pValue);
            break;
        case GPON_REG_DSGTCPMMISC_BIP_BITS_ERR:
            ret = reg_field_read(APOLLOMP_GPON_GTC_DS_MISC_CNTR_BIP_ERR_BITr,APOLLOMP_CNTR_DS_BIP_ERR_BITSf,pValue);
            break;
        case GPON_REG_DSGTCPMMISC_FEC_CORRECTED_BITS:
            ret = reg_field_read(APOLLOMP_GPON_GTC_DS_MISC_CNTR_FEC_CORRECT_BITr,APOLLOMP_CNTR_FEC_CORRECTED_BITSf,pValue);
            break;
        case GPON_REG_DSGTCPMMISC_FEC_CORRECTED_BYTES:
            ret = reg_field_read(APOLLOMP_GPON_GTC_DS_MISC_CNTR_FEC_CORRECT_BYTEr,APOLLOMP_CNTR_FEC_CORRECTED_BYTESf,pValue);
            break;
        case GPON_REG_DSGTCPMMISC_FEC_CORRETED_CWS:
            ret = reg_field_read(APOLLOMP_GPON_GTC_DS_MISC_CNTR_FEC_CORRECT_CWr,APOLLOMP_CNTR_FEC_CORRETED_CWf,pValue);
            break;
        case GPON_REG_DSGTCPMMISC_FEC_UNCOR_CWS:
            ret = reg_field_read(APOLLOMP_GPON_GTC_DS_MISC_CNTR_FEC_UNCOR_CWr,APOLLOMP_CNTR_FEC_UNCORRETABLE_CWf,pValue);
            break;
        case GPON_REG_DSGTCPMMISC_LOM:
            ret = reg_read(APOLLOMP_GPON_GTC_DS_MISC_CNTR_LOMr,pValue);
            break;
        case GPON_REG_DSGTCPMMISC_PLOAM_ACCEPT:
            ret = reg_field_read(APOLLOMP_GPON_GTC_DS_MISC_CNTR_PLOAM_ACPTr,APOLLOMP_CNTR_PLOAMD_ACCEPTEDf,pValue);
            break;
        case GPON_REG_DSGTCPMMISC_PLOAM_FAIL:
            ret = reg_read(APOLLOMP_GPON_GTC_DS_MISC_CNTR_PLOAM_FAILr,pValue);
            break;
        case GPON_REG_DSGTCPMMISC_BWMAP_FAIL:
            ret = reg_read(APOLLOMP_GPON_GTC_DS_MISC_CNTR_BWM_FAILr,pValue);
            break;
        case GPON_REG_DSGTCPMMISC_BWMAP_INV:
            ret = reg_read(APOLLOMP_GPON_GTC_DS_MISC_CNTR_BWM_INVr,pValue);
            break;
        case GPON_REG_DSGTCPMMISC_ACTIVE:
            ret = reg_read(APOLLOMP_GPON_GTC_DS_MISC_CNTR_ACTIVEr,pValue);
            break;
        case GPON_REG_DSGTCPMMISC_BWMAP_ACPT:
            ret = reg_field_read(APOLLOMP_GPON_GTC_DS_MISC_CNTR_BWM_ACPTr,APOLLOMP_CNTR_BWMAP_ACCPTEDf,pValue);
            break;
        case GPON_REG_DSGTCPMMISC_GEM_LOS:
            ret = reg_field_read(APOLLOMP_GPON_GTC_DS_MISC_CNTR_GEM_LOSr,APOLLOMP_CNTR_GEM_LOSf,pValue);
            break;
        case GPON_REG_DSGTCPMMISC_HEC_COR:
            ret = reg_field_read(APOLLOMP_GPON_GTC_DS_MISC_CNTR_HEC_CORRECTr,APOLLOMP_CNTR_HEC_CORRECTEDf,pValue);
            break;
        case GPON_REG_DSGTCPMMISC_GEM_IDLE:
            ret = reg_field_read(APOLLOMP_GPON_GTC_DS_MISC_CNTR_GEM_IDLEr,APOLLOMP_CNTR_GEM_IDLEf,pValue);
            break;
        case GPON_REG_DSGTCPMMISC_GEM_FAIL:
            ret = reg_read(APOLLOMP_GPON_GTC_DS_MISC_CNTR_GEM_FAILr,pValue);
            break;
        case GPON_REG_DSGTCPMMISC_RX_GEM_NON_IDLE:
            ret = reg_field_read(APOLLOMP_GPON_GTC_DS_MISC_CNTR_GEM_NON_IDLEr,APOLLOMP_CNTR_RX_GEM_NON_IDLEf,pValue);
            break;
        case GPON_REG_DSGTCPMMISC_PLEND_CORRECTIONS:
            ret = reg_field_read(APOLLOMP_GPON_GTC_DS_MISC_CNTR_PLEN_CORRECTr,APOLLOMP_CNTR_PLEND_CORRECTIONSf,pValue);
            break;
        default:
            ret = RT_ERR_OUT_OF_RANGE;
            break;
    }
    return ret;
}

void apollo_raw_gpon_dsOmci_pti_write(uint8 mask, uint8 end)
{
    uint32 data=0;
    uint32 value;

    value = mask;
    reg_field_set(APOLLOMP_GPON_GTC_DS_OMCI_PTIr, APOLLOMP_OMCI_PTI_MASKf, &value, &data);
    value = end;
    reg_field_set(APOLLOMP_GPON_GTC_DS_OMCI_PTIr, APOLLOMP_OMCI_END_PTIf, &value, &data);

    reg_write(APOLLOMP_GPON_GTC_DS_OMCI_PTIr,&data);
}

int32 apollo_raw_gpon_dsOmci_pti_read(uint8 *mask, uint8 *end)
{
    uint32 value, data;
    int32 ret;

    if((ret = reg_read(APOLLOMP_GPON_GTC_DS_OMCI_PTIr, &data))!=RT_ERR_OK)
        return ret;

    reg_field_get(APOLLOMP_GPON_GTC_DS_OMCI_PTIr,APOLLOMP_OMCI_PTI_MASKf,&value,&data);
    *mask = value&0x7;
    reg_field_get(APOLLOMP_GPON_GTC_DS_OMCI_PTIr,APOLLOMP_OMCI_END_PTIf,&value,&data);
    *end = value&0x7;

    return RT_ERR_OK;
}

void apollo_raw_gpon_dsEth_pti_write(uint8 mask, uint8 end)
{
    uint32 data=0;
    uint32 value;

    value = mask;
    reg_field_set(APOLLOMP_GPON_GTC_DS_ETH_PTIr, APOLLOMP_ETH_PTI_MASKf, &value, &data);
    value = end;
    reg_field_set(APOLLOMP_GPON_GTC_DS_ETH_PTIr, APOLLOMP_ETH_END_PTIf, &value, &data);

    reg_write(APOLLOMP_GPON_GTC_DS_ETH_PTIr,&data);
}

int32 apollo_raw_gpon_dsEth_pti_read(uint8 *mask, uint8 *end)
{
    uint32 value, data;
    int32 ret;

    if((ret = reg_read(APOLLOMP_GPON_GTC_DS_ETH_PTIr, &data))!=RT_ERR_OK)
        return ret;

    reg_field_get(APOLLOMP_GPON_GTC_DS_ETH_PTIr,APOLLOMP_ETH_PTI_MASKf,&value,&data);
    *mask = value&0x7;
    reg_field_get(APOLLOMP_GPON_GTC_DS_ETH_PTIr,APOLLOMP_ETH_END_PTIf,&value,&data);
    *end = value&0x7;

    return RT_ERR_OK;
}

void apollo_raw_gpon_aes_keyswitch_write(uint32 value)
{
    uint32 data;
    data = value;
    reg_field_write(APOLLOMP_GPON_AES_KEY_SWITCH_TIMEr,APOLLOMP_SWITCH_SUPERFRAMEf,&data);
}

void apollo_raw_gpon_aes_keyswitch_read(uint32 *pValue)
{
    uint32 data;

    reg_field_read(APOLLOMP_GPON_AES_KEY_SWITCH_TIMEr,APOLLOMP_SWITCH_SUPERFRAMEf,&data);
    *pValue = data;
}

int32 apollo_raw_gpon_aes_keyword_write(uint8 *value)
{
    uint32 data = 0;
    uint32 tmp_val;
    uint32 idx,ind;

    tmp_val = GPON_REG_STATUS_CLEAR;
    reg_field_set(APOLLOMP_GPON_AES_KEY_SWITCH_REQr, APOLLOMP_CFG_ACTIVE_KEYf, &tmp_val, &data);
    tmp_val = GPON_REG_STATUS_CLEAR;
    reg_field_set(APOLLOMP_GPON_AES_KEY_SWITCH_REQr, APOLLOMP_KEY_CFG_REQf, &tmp_val, &data);

    reg_write(APOLLOMP_GPON_AES_KEY_SWITCH_REQr,&data);

    tmp_val = GPON_REG_STATUS_SET;
    reg_field_set(APOLLOMP_GPON_AES_KEY_SWITCH_REQr, APOLLOMP_KEY_CFG_REQf, &tmp_val, &data);

    reg_write(APOLLOMP_GPON_AES_KEY_SWITCH_REQr,&data);

    for(idx=0;idx<8;idx++)
    {
        data = (value[2*idx+0]<<GPON_REG_16BITS_HIGH)|(value[2*idx+1]<<GPON_REG_16BITS_LOW);
        reg_field_write(APOLLOMP_GPON_AES_WORD_DATAr,APOLLOMP_KEY_DATAf,&data);

        ind = 0;
        tmp_val = idx;
        reg_field_set(APOLLOMP_GPON_AES_KEY_WORD_INDr, APOLLOMP_KEY_WORD_IDXf, &tmp_val, &ind);
        tmp_val = GPON_REG_STATUS_CLEAR;
        reg_field_set(APOLLOMP_GPON_AES_KEY_WORD_INDr, APOLLOMP_KEY_WR_REQf, &tmp_val, &ind);
        reg_write(APOLLOMP_GPON_AES_KEY_WORD_INDr,&ind);

        tmp_val = GPON_REG_STATUS_SET;
        reg_field_set(APOLLOMP_GPON_AES_KEY_WORD_INDr, APOLLOMP_KEY_WR_REQf, &tmp_val, &ind);
		reg_write(APOLLOMP_GPON_AES_KEY_WORD_INDr,&ind);

        if(apollo_raw_gpon_cam_wait(APOLLOMP_GPON_AES_KEY_WORD_INDr,FALSE))
        {
        }
        else
        {
        }
    }
    return RT_ERR_OK;
}

int32 apollo_raw_gpon_gemDs_intr_read(uint32 *pValue)
{
#if defined(OLD_FPGA_DEFINED)
    return reg_read(APOLLOMP_GPON_GEM_DS_INTR_DLTr, pValue);
#else
    if(pValue)
        return FALSE;

    return FALSE;
#endif
}

int32 apollo_raw_gpon_gemDsIntr_omci_check(uint32 status)
{
#if defined(OLD_FPGA_DEFINED)
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_GEM_DS_INTR_DLTr, APOLLOMP_OMCI_RX_REQf, &value, &status);
    return value;
#else
    if(status)
        return FALSE;
    return FALSE;
#endif
}

int32 apollo_raw_gpon_omci_read(uint8 *value)
{
#if defined(OLD_FPGA_DEFINED)
    uint32 omci,i;
    uint32 tmp,data;

    reg_field_read(APOLLOMP_GPON_GEM_DS_OMCI_INDr, APOLLOMP_OMCI_BUF_EMPTYf, &tmp);
    if(tmp==GPON_REG_STATUS_CLEAR)
    {
        for(i = 0; i<(RTK_GPON_OMCI_MSG_LEN / 4); i++)
        {
            reg_array_read(APOLLOMP_GPON_GEM_DS_OMCI_MSGr,
                             REG_ARRAY_INDEX_NONE,
                             i,
                             &omci);
            value[4 * i + 0] = (omci >> GPON_REG_32BITS_3) & 0xFF;
            value[4 * i + 1] = (omci >> GPON_REG_32BITS_2) & 0xFF;
            value[4 * i + 2] = (omci >> GPON_REG_32BITS_1) & 0xFF;
            value[4 * i + 3] = (omci >> GPON_REG_32BITS_0) & 0xFF;
        }
		data = GPON_REG_STATUS_CLEAR;
        reg_write(APOLLOMP_GPON_GEM_DS_OMCI_INDr, &data);
		data = GPON_REG_STATUS_SET;
        reg_write(APOLLOMP_GPON_GEM_DS_OMCI_INDr, &data);
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_OMCI_QUEUE_EMPTY;
    }
#else
    if(value)
        return RT_ERR_GPON_OMCI_QUEUE_EMPTY;

    return RT_ERR_GPON_OMCI_QUEUE_EMPTY;
#endif
}

int32 apollo_raw_gpon_dsPort_ethRxCnt_read(uint32 idx, uint32 *value)
{
    reg_write(APOLLOMP_GPON_GEM_DS_RX_CNTR_INDr,&idx);

    if(apollo_raw_gpon_indirect_wait(APOLLOMP_GPON_GEM_DS_RX_CNTR_INDr))
    {
		return reg_field_read(APOLLOMP_GPON_GEM_DS_RX_CNTR_STATr,APOLLOMP_ETH_PKT_RXf,value);
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
}

int32 apollo_raw_gpon_dsPort_ethFwdCnt_read(uint32 idx, uint32 *value)
{
    reg_write(APOLLOMP_GPON_GEM_DS_FWD_CNTR_INDr,&idx);

    if(apollo_raw_gpon_indirect_wait(APOLLOMP_GPON_GEM_DS_FWD_CNTR_INDr))
    {
		return reg_field_read(APOLLOMP_GPON_GEM_DS_FWD_CNTR_STATr,APOLLOMP_ETH_PKT_FWDf,value);
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
}

int32 apollo_raw_gpon_dsGem_miscCnt_read(apollo_raw_gpon_dsGem_pmMiscType_t idx, uint32 *pValue)
{
    int32 ret;

    RT_PARAM_CHK((idx>GPON_REG_DSGEMPMMISC_OMCI_RX),RT_ERR_OUT_OF_RANGE);
    ret = reg_field_write(APOLLOMP_GPON_GEM_DS_MISC_INDr,APOLLOMP_MISC_CNTR_IDXf,&idx);
    if(ret != RT_ERR_OK)
        return ret;

    return reg_field_read(APOLLOMP_GPON_GEM_DS_MISC_CNTR_STATr,APOLLOMP_MISC_CNTRf,pValue);
}

void apollo_raw_gpon_dsEth_fcsCheck_enable(int32 value)
{
	reg_field_write(APOLLOMP_GPON_GEM_DS_MC_CFGr,APOLLOMP_FCS_CHK_ENf,&value);
}

int32 apollo_raw_gpon_dsEth_fcsCheck_enable_read(uint32 *value)
{
    return reg_field_read(APOLLOMP_GPON_GEM_DS_MC_CFGr,APOLLOMP_FCS_CHK_ENf,value);
}

void apollo_raw_gpon_dsEth_bcPAss_write(int32 value)
{
    reg_field_write(APOLLOMP_GPON_GEM_DS_MC_CFGr,APOLLOMP_BROADCAST_PASSf,&value);
}

int32 apollo_raw_gpon_dsEth_bcPAss_read(int32 *value)
{
    return reg_field_read(APOLLOMP_GPON_GEM_DS_MC_CFGr,APOLLOMP_BROADCAST_PASSf,value);
}

void apollo_raw_gpon_dsEth_nonMcPass_write(int32 value)
{
    reg_field_write(APOLLOMP_GPON_GEM_DS_MC_CFGr,APOLLOMP_NON_MULTICAST_PASSf,&value);
}

int32 apollo_raw_gpon_dsEth_nonMcPass_read(int32 *value)
{
    return reg_field_read(APOLLOMP_GPON_GEM_DS_MC_CFGr,APOLLOMP_NON_MULTICAST_PASSf,value);
}

void apollo_raw_gpon_dsEth_macFilterMode_write(rtk_gpon_macTable_exclude_mode_t exclude)
{
    uint32 value;
    if(exclude == RTK_GPON_MACTBL_MODE_EXCLUDE)
        value = 1;
    else
        value =0;

    reg_field_write(APOLLOMP_GPON_GEM_DS_MC_CFGr,APOLLOMP_MC_EXCL_MODEf,&value);
}

int32 apollo_raw_gpon_dsEth_macFilterMode_read(rtk_gpon_macTable_exclude_mode_t *value)
{
    uint32 excl_mode;
    int32 ret;

    if( (ret = reg_field_read(APOLLOMP_GPON_GEM_DS_MC_CFGr,APOLLOMP_MC_EXCL_MODEf,&excl_mode)) != RT_ERR_OK )
        return ret;

    if(excl_mode == 0)
        *value = RTK_GPON_MACTBL_MODE_INCLUDE;
    else
        *value = RTK_GPON_MACTBL_MODE_EXCLUDE;
    return RT_ERR_OK;
}

void apollo_raw_gpon_dsEth_mcForceMode_write(rtk_gpon_mc_force_mode_t ipv4,
                                      rtk_gpon_mc_force_mode_t ipv6)
{
    uint32 drop_ipv4, pass_ipv4, drop_ipv6, pass_ipv6;
    uint32 data;

    switch(ipv4)
    {
        case RTK_GPON_MCFORCE_MODE_PASS:
            drop_ipv4 = 0;
            pass_ipv4 = 1;
        break;

        case RTK_GPON_MCFORCE_MODE_DROP:
            drop_ipv4 = 1;
            pass_ipv4 = 0;
        break;

        case RTK_GPON_MCFORCE_MODE_NORMAL:
        default:
            drop_ipv4 = 0;
            pass_ipv4 = 0;
        break;
    }
    switch(ipv6)
    {
        case RTK_GPON_MCFORCE_MODE_PASS:
            drop_ipv6 = 0;
            pass_ipv6 = 1;
        break;

        case RTK_GPON_MCFORCE_MODE_DROP:
            drop_ipv6 = 1;
            pass_ipv6 = 0;
        break;

        case RTK_GPON_MCFORCE_MODE_NORMAL:
        default:
            drop_ipv6 = 0;
            pass_ipv6 = 0;
        break;
    }

    reg_read(APOLLOMP_GPON_GEM_DS_MC_CFGr, &data);

    reg_field_set(APOLLOMP_GPON_GEM_DS_MC_CFGr,APOLLOMP_IPV4_MC_FORCE_PASSf,&pass_ipv4,&data);
    reg_field_set(APOLLOMP_GPON_GEM_DS_MC_CFGr,APOLLOMP_IPV4_MC_FORCE_DROPf,&drop_ipv4,&data);
    reg_field_set(APOLLOMP_GPON_GEM_DS_MC_CFGr,APOLLOMP_IPV6_MC_FORCE_PASSf,&pass_ipv6,&data);
    reg_field_set(APOLLOMP_GPON_GEM_DS_MC_CFGr,APOLLOMP_IPV6_MC_FORCE_DROPf,&drop_ipv6,&data);

    reg_write(APOLLOMP_GPON_GEM_DS_MC_CFGr,&data);

}

int32 apollo_raw_gpon_dsEth_mcForceMode_read(rtk_gpon_mc_force_mode_t *ipv4,
                                      rtk_gpon_mc_force_mode_t *ipv6)
{
    uint32 drop_ipv4, pass_ipv4, drop_ipv6, pass_ipv6;
    uint32 data;
    int32  ret;

    if((ret = reg_read(APOLLOMP_GPON_GEM_DS_MC_CFGr,&data)) != RT_ERR_OK )
        return ret;

    if((ret = reg_field_get(APOLLOMP_GPON_GEM_DS_MC_CFGr,APOLLOMP_IPV4_MC_FORCE_PASSf,&pass_ipv4,&data)) != RT_ERR_OK)
        return ret;
    if((ret = reg_field_get(APOLLOMP_GPON_GEM_DS_MC_CFGr,APOLLOMP_IPV4_MC_FORCE_DROPf,&drop_ipv4,&data)) != RT_ERR_OK)
        return ret;
    if((ret = reg_field_get(APOLLOMP_GPON_GEM_DS_MC_CFGr,APOLLOMP_IPV6_MC_FORCE_PASSf,&pass_ipv6,&data)) != RT_ERR_OK)
        return ret;
    if((ret = reg_field_get(APOLLOMP_GPON_GEM_DS_MC_CFGr,APOLLOMP_IPV6_MC_FORCE_DROPf,&drop_ipv6,&data)) != RT_ERR_OK)
        return ret;

    if((pass_ipv4 == 0) && (drop_ipv4 == 0))
        *ipv4 = RTK_GPON_MCFORCE_MODE_NORMAL;
    else if(pass_ipv4 == 1)
        *ipv4 = RTK_GPON_MCFORCE_MODE_PASS;
    else
        *ipv4 = RTK_GPON_MCFORCE_MODE_DROP;

    if((pass_ipv6 == 0) && (drop_ipv6 == 0))
        *ipv6 = RTK_GPON_MCFORCE_MODE_NORMAL;
    else if(pass_ipv6 == 1)
        *ipv6 = RTK_GPON_MCFORCE_MODE_PASS;
    else
        *ipv6 = RTK_GPON_MCFORCE_MODE_DROP;

    return RT_ERR_OK;
}

int32 apollo_raw_gpon_macEntry_read(uint32 idx, uint32 *value)
{
    uint32 op=0;
    uint32 tmp_val;

    tmp_val = GPON_REG_OPERRATION_READ;
    reg_field_set(APOLLOMP_GPON_GEM_DS_MC_INDr, APOLLOMP_MC_ITEM_OP_MODEf, &tmp_val, &op);
    tmp_val = idx;
    reg_field_set(APOLLOMP_GPON_GEM_DS_MC_INDr, APOLLOMP_MC_ITEM_OP_IDXf, &tmp_val, &op);
    reg_write(APOLLOMP_GPON_GEM_DS_MC_INDr,&op);

    tmp_val = GPON_REG_STATUS_SET;
    reg_field_set(APOLLOMP_GPON_GEM_DS_MC_INDr, APOLLOMP_MC_ITEM_OP_REQf, &tmp_val, &op);
    reg_write(APOLLOMP_GPON_GEM_DS_MC_INDr,&op);

    if(apollo_raw_gpon_cam_wait(APOLLOMP_GPON_GEM_DS_MC_INDr,TRUE))
    {
#if defined (CONFIG_SDK_ASICDRV_TEST)
		return reg_field_read(APOLLOMP_GPON_GEM_DS_MC_WRr,APOLLOMP_MC_ITEM_OP_WDATAf,value);
#else
		return reg_field_read(APOLLOMP_GPON_GEM_DS_MC_RDr,APOLLOMP_MC_ITEM_OP_RDATAf,value);
#endif
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
}

int32 apollo_raw_gpon_macEntry_write(uint32 idx, uint32 value)
{
    uint32 op=0;
    uint32 tmp_val;

    tmp_val = GPON_REG_OPERRATION_WRITE;
    reg_field_set(APOLLOMP_GPON_GEM_DS_MC_INDr, APOLLOMP_MC_ITEM_OP_MODEf, &tmp_val, &op);
    tmp_val = idx;
    reg_field_set(APOLLOMP_GPON_GEM_DS_MC_INDr, APOLLOMP_MC_ITEM_OP_IDXf, &tmp_val, &op);
    reg_write(APOLLOMP_GPON_GEM_DS_MC_INDr,&op);

    reg_field_write(APOLLOMP_GPON_GEM_DS_MC_WRr,APOLLOMP_MC_ITEM_OP_WDATAf,&value);

    tmp_val = GPON_REG_STATUS_SET;
    reg_field_set(APOLLOMP_GPON_GEM_DS_MC_INDr, APOLLOMP_MC_ITEM_OP_REQf, &tmp_val, &op);
    reg_write(APOLLOMP_GPON_GEM_DS_MC_INDr,&op);

    if(apollo_raw_gpon_cam_wait(APOLLOMP_GPON_GEM_DS_MC_INDr,FALSE))
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
}

int32 apollo_raw_gpon_macEntry_clean(uint32 idx)
{
    uint32 op=0;
    uint32 tmp_val;

    tmp_val = GPON_REG_OPERRATION_CLEAN;
    reg_field_set(APOLLOMP_GPON_GEM_DS_MC_INDr, APOLLOMP_MC_ITEM_OP_MODEf, &tmp_val, &op);
    tmp_val = idx;
    reg_field_set(APOLLOMP_GPON_GEM_DS_MC_INDr, APOLLOMP_MC_ITEM_OP_IDXf, &tmp_val, &op);
    reg_write(APOLLOMP_GPON_GEM_DS_MC_INDr,&op);

    tmp_val = GPON_REG_STATUS_SET;
    reg_field_set(APOLLOMP_GPON_GEM_DS_MC_INDr, APOLLOMP_MC_ITEM_OP_REQf, &tmp_val, &op);
    reg_write(APOLLOMP_GPON_GEM_DS_MC_INDr,&op);

    if(apollo_raw_gpon_cam_wait(APOLLOMP_GPON_GEM_DS_MC_INDr,FALSE))
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
}

void apollo_raw_gpon_assembleTimeout_write(uint8 value)
{
    uint32 tmp = value;
	reg_field_write(APOLLOMP_GPON_GEM_DS_FRM_TIMEOUTr, APOLLOMP_ASSM_TIMEOUT_FRMf,&tmp);
}

int32 apollo_raw_gpon_assembleTimeout_read(uint8 *value)
{
    uint32 tmp;
    int32 ret;
    ret=reg_field_read(APOLLOMP_GPON_GEM_DS_FRM_TIMEOUTr, APOLLOMP_ASSM_TIMEOUT_FRMf,&tmp);
    *value = (tmp&0x1F);
    return ret;
}

/* rename MAC_H24B_xxx to IPV4_MC_MAC_PREFIX in GPON_MAC_SWIO_r1.2 */
void apollo_raw_gpon_ipv4McAddrPtn_write(uint32 value)
{
    uint32 pattern=value&0xFFFFFF;
    reg_field_write(APOLLOMP_GPON_GEM_DS_MC_ADDR_PTN_IPV4r, APOLLOMP_IPV4_MC_MAC_PREFIXf,&pattern);
}

int32 apollo_raw_gpon_ipv4McAddrPtn_read(uint32 *value)
{
    uint32 pattern;
    int32  ret;

    ret = reg_field_read(APOLLOMP_GPON_GEM_DS_MC_ADDR_PTN_IPV4r, APOLLOMP_IPV4_MC_MAC_PREFIXf,&pattern);
    *value = pattern&0xFFFFFF;
    return ret;
}
/* add IPV6_MC_MAC_PREFIX in GPON_MAC_SWIO_r1.2 */
void apollo_raw_gpon_ipv6McAddrPtn_write(uint32 value)
{
    uint32 pattern=value&0xFFFF;
    reg_field_write(APOLLOMP_GPON_GEM_DS_MC_ADDR_PTN_IPV6r,APOLLOMP_IPV6_MC_MAC_PREFIXf,&pattern);
}

int32 apollo_raw_gpon_ipv6McAddrPtn_read(uint32 *value)
{
    uint32 pattern;
    int32  ret;

    ret = reg_field_read(APOLLOMP_GPON_GEM_DS_MC_ADDR_PTN_IPV6r,APOLLOMP_IPV6_MC_MAC_PREFIXf,&pattern);
    *value = pattern&0xFFFF;
    return ret;
}

#if 0 /* wellknown addr is removed in GPON_MAC_SWIO_r1.1 */
void apollo_raw_gpon_wellKnownAddr_write(uint32 value)
{
    apollo_raw_gpon_register_write(APOLLOMP_GPON_GEM_DS_WELLKWN_ADDRr,value);
}

int32 apollo_raw_gpon_wellKnownAddr_read(uint32 *value)
{
    return reg_field_read(APOLLOMP_GPON_GEM_DS_WELLKWN_ADDRr,APOLLOMP_WELL_KNOWN_ADDRf,value);
}
#endif

int32 apollo_raw_gpon_gtcUs_intr_read(uint32 *pValue)
{
    return reg_read(APOLLOMP_GPON_GTC_US_INTR_DLTr, pValue);
}

int32 apollo_raw_gpon_gtcUsIntr_fec_check(uint32 status)
{
    uint32 value;

    reg_field_get(APOLLOMP_GPON_GTC_US_INTR_DLTr, APOLLOMP_US_FEC_STS_DLTf, &value, &status);
    return value;
}

int32 apollo_raw_gpon_gtcUsIntr_urgPloamEmpty_check(uint32 status)
{
    uint32 value;

    reg_field_get(APOLLOMP_GPON_GTC_US_PLOAM_INDr, APOLLOMP_PLM_URG_EMPTYf, &value, &status);
    return value;
}

int32 apollo_raw_gpon_gtcUsIntr_nrmPloamEmpty_check(uint32 status)
{
    uint32 value;

    reg_field_get(APOLLOMP_GPON_GTC_US_PLOAM_INDr, APOLLOMP_PLM_NRM_EMPTYf, &value, &status);
    return value;
}

int32 apollo_raw_gpon_gtcUs_mask_read(uint32 *pValue)
{
    return reg_read(APOLLOMP_GPON_GTC_US_INTR_MASKr, pValue);
}

void apollo_raw_gpon_gtcUs_mask_write(uint32 value)
{
    reg_write(APOLLOMP_GPON_GTC_US_INTR_MASKr,&value);
}

void apollo_raw_gpon_gtcUsMask_fec_write(int32 value)
{
	reg_field_write(APOLLOMP_GPON_GTC_US_INTR_MASKr,APOLLOMP_US_FEC_STS_Mf,&value);
}

void apollo_raw_gpon_gtcUsMask_urgPloamEmpty_write(int32 value)
{
	reg_field_write(APOLLOMP_GPON_GTC_US_INTR_MASKr,APOLLOMP_PLM_URG_EMPTY_Mf,&value);
}

void apollo_raw_gpon_gtcUsMask_nrmPloamEmpty_write(int32 value)
{
	reg_field_write(APOLLOMP_GPON_GTC_US_INTR_MASKr,APOLLOMP_PLM_NRM_EMPTY_Mf,&value);
}

int32 apollo_raw_gpon_gtcUs_fec_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GTC_US_INTR_STSr,APOLLOMP_US_FEC_STSf,pValue);
}

int apollo_raw_gpon_usOnuId_read(uint8 *pValue)
{
    uint32 tmp;
    int32 ret;

	ret = reg_field_read(APOLLOMP_GPON_GTC_US_ONU_IDr, APOLLOMP_ONU_IDf, &tmp);

    *pValue = tmp;

    return ret;
}

void apollo_raw_gpon_forceLaser_write(rtk_gpon_laser_status_t value)
{
    uint32 data,tmp;

	reg_read(APOLLOMP_GPON_GTC_US_CFGr,&data);

	if(value==RTK_GPON_LASER_STATUS_FORCE_ON)
    {
        tmp = GPON_REG_STATUS_SET;
        reg_field_set(APOLLOMP_GPON_GTC_US_CFGr, APOLLOMP_FS_LONf, &tmp, &data);
    }
    else
    {
        tmp = GPON_REG_STATUS_CLEAR;
        reg_field_set(APOLLOMP_GPON_GTC_US_CFGr, APOLLOMP_FS_LONf, &tmp, &data);
    }

    if(value==RTK_GPON_LASER_STATUS_FORCE_OFF)
    {
        tmp = GPON_REG_STATUS_SET;
        reg_field_set(APOLLOMP_GPON_GTC_US_CFGr, APOLLOMP_FS_LOFFf, &tmp, &data);
    }
    else
    {
        tmp = GPON_REG_STATUS_CLEAR;
        reg_field_set(APOLLOMP_GPON_GTC_US_CFGr, APOLLOMP_FS_LOFFf, &tmp, &data);
    }
	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
    reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);

	reg_write(APOLLOMP_GPON_GTC_US_CFGr,&data);

	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);
}

int32 apollo_raw_gpon_forceLaser_read(rtk_gpon_laser_status_t *pValue)
{
    uint32 data,value;
    int32 ret;

    ret = reg_read(APOLLOMP_GPON_GTC_US_CFGr, &data);
    reg_field_get(APOLLOMP_GPON_GTC_US_CFGr,APOLLOMP_FS_LOFFf,&value,&data);
    if(value)
    {
        *pValue = RTK_GPON_LASER_STATUS_FORCE_OFF;
        return ret;
    }
    reg_field_get(APOLLOMP_GPON_GTC_US_CFGr,APOLLOMP_FS_LONf,&value,&data);
    if(value)
    {
        *pValue = RTK_GPON_LASER_STATUS_FORCE_ON;
        return ret;
    }
    *pValue = RTK_GPON_LASER_STATUS_NORMAL;

    return ret;
}

void apollo_raw_gpon_txPtnMode_write(rtk_gpon_tx_ptn_mode_t fg_mode, rtk_gpon_tx_ptn_mode_t bg_mode)
{
    uint32 no_fg;
    uint32 data=0;
    uint32 tmp;

    if(fg_mode == RTK_GPON_TX_PTN_MODE_NONE)
    {
        no_fg = 1;
        fg_mode = 0;
    }
    else
    {
        no_fg = 0;
    }

    reg_field_set(APOLLOMP_GPON_GTC_US_TX_PATTERN_CTLr, APOLLOMP_TX_PATTERN_MODE_NO_FGf, &no_fg, &data);
    reg_field_set(APOLLOMP_GPON_GTC_US_TX_PATTERN_CTLr, APOLLOMP_TX_PATTERN_MODE_FGf, &fg_mode, &data);
    reg_field_set(APOLLOMP_GPON_GTC_US_TX_PATTERN_CTLr, APOLLOMP_TX_PATTERN_MODE_BGf, &bg_mode, &data);

    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
    reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);

    reg_write(APOLLOMP_GPON_GTC_US_TX_PATTERN_CTLr,&data);

    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);
}

int32 apollo_raw_gpon_txPtnMode_read(rtk_gpon_tx_ptn_mode_t *pFg_mode, rtk_gpon_tx_ptn_mode_t *pBg_mode)
{
    uint32 val, data;
    uint32 ret;

    if((ret = reg_read(APOLLOMP_GPON_GTC_US_TX_PATTERN_CTLr, &data)) != RT_ERR_OK)
        return ret;

    if((ret = reg_field_get(APOLLOMP_GPON_GTC_US_TX_PATTERN_CTLr, APOLLOMP_TX_PATTERN_MODE_BGf, &val, &data)) != RT_ERR_OK)
        return ret;

    *pBg_mode = val;

    if((ret = reg_field_get(APOLLOMP_GPON_GTC_US_TX_PATTERN_CTLr, APOLLOMP_TX_PATTERN_MODE_NO_FGf, &val, &data)) != RT_ERR_OK)
        return ret;

    if(val == 1)
        *pFg_mode = RTK_GPON_TX_PTN_MODE_NONE;
    else
    {
        if((ret = reg_field_get(APOLLOMP_GPON_GTC_US_TX_PATTERN_CTLr, APOLLOMP_TX_PATTERN_MODE_FGf, &val, &data)) != RT_ERR_OK)
            return ret;
        *pFg_mode = val;
    }

    return ret;
}

void apollo_raw_gpon_txPtn_write(uint32 fg_ptn, uint32 bg_ptn)
{
    uint32 tmp;

    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
    reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);

    reg_field_write(APOLLOMP_GPON_GTC_US_TX_PATTERN_FGr, APOLLOMP_TX_PATTERN_FGf, &fg_ptn);
    reg_field_write(APOLLOMP_GPON_GTC_US_TX_PATTERN_BGr, APOLLOMP_TX_PATTERN_BGf, &bg_ptn);

    tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);
}

int32 apollo_raw_gpon_txPtn_read(uint32 *pFg_ptn, uint32 *pBg_ptn)
{
    uint32 ret;

    if((ret = reg_field_read(APOLLOMP_GPON_GTC_US_TX_PATTERN_FGr, APOLLOMP_TX_PATTERN_FGf, pFg_ptn)) != RT_ERR_OK)
        return ret;

    if((ret = reg_field_read(APOLLOMP_GPON_GTC_US_TX_PATTERN_BGr, APOLLOMP_TX_PATTERN_BGf, pBg_ptn)) != RT_ERR_OK)
        return ret;

    return ret;
}

void apollo_raw_gpon_RogueOnt_write(int32 value)
{
    if(1 == value)			//diagnose enable
    {
        /*osal_print("Rcv Ds ploam DiagNose enable %d \r\n",value);*/
        apollo_raw_gpon_txPtn_write(0x0, 0x55555555);
        apollo_raw_gpon_txPtnMode_write(RTK_GPON_TX_PTN_MODE_NORMAL, RTK_GPON_TX_PTN_MODE_CUSTOM);
    }
    else if(0 == value)		//diagnose disable
    {
        /*osal_print("Rcv Ds ploam DiagNose enable %d \r\n",value);*/
        apollo_raw_gpon_txPtn_write(0x0, 0x0);
        apollo_raw_gpon_txPtnMode_write(RTK_GPON_TX_PTN_MODE_NORMAL, RTK_GPON_TX_PTN_MODE_NORMAL);
    }
}

void apollo_raw_gpon_forceUsPrbs_write(int32 value)
{
    if(1 == value)			//diagnose enable
    {
        apollo_raw_gpon_txPtnMode_write(RTK_GPON_TX_PTN_MODE_NONE, RTK_GPON_TX_PTN_MODE_PRBS);
        apollo_raw_gpon_forceLaser_write(RTK_GPON_LASER_STATUS_FORCE_ON);
    }
    else if(0 == value)		//diagnose disable
    {
        apollo_raw_gpon_txPtnMode_write(RTK_GPON_TX_PTN_MODE_NORMAL, RTK_GPON_TX_PTN_MODE_NORMAL);
        apollo_raw_gpon_forceLaser_write(RTK_GPON_LASER_STATUS_NORMAL);
    }
}

#if 0
void apollo_raw_gpon_forceUsGemIdle_write(int32 value)
{
	uint32 tmp;

	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
	reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);

    reg_field_write(APOLLOMP_GPON_GTC_US_CFGr,APOLLOMP_FS_GEM_IDLEf,&value);

	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);
}

int32 apollo_raw_gpon_forceUsGemIdle_read(int32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GTC_US_CFGr,APOLLOMP_FS_GEM_IDLEf,pValue);
}

void apollo_raw_gpon_forceUsPrbs_write(int32 value)
{
	uint32 tmp;

	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
	reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);

    reg_field_write(APOLLOMP_GPON_GTC_US_CFGr,APOLLOMP_FS_PRBSf,&value);

	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);
}

int32 apollo_raw_gpon_forceUsPrbs_read(int32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GTC_US_CFGr,APOLLOMP_FS_PRBSf,pValue);
}
#endif

void apollo_raw_gpon_ploam_enable(int32 value)
{
    uint32 tmp;

	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
	reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);

	value = !value;
    reg_field_write(APOLLOMP_GPON_GTC_US_CFGr,APOLLOMP_PLM_DISf,&value);

	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);
}

int32 apollo_raw_gpon_ploam_enable_read(int32 *pValue)
{
    uint32 data;
    int32 ret;

    ret = reg_field_read(APOLLOMP_GPON_GTC_US_CFGr,APOLLOMP_PLM_DISf,&data);
    *pValue = !data;
    return ret;
}

void apollo_raw_gpon_ind_nrm_ploam_enable(int32 value)
{
    uint32 tmp;

	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
	reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);

    reg_field_write(APOLLOMP_GPON_GTC_US_CFGr,APOLLOMP_IND_NRM_PLMf,&value);

	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);
}

int32 apollo_raw_gpon_ind_nrm_ploam_enable_read(int32 *pValue)
{
    uint32 data;
    int32 ret;

    ret = reg_field_read(APOLLOMP_GPON_GTC_US_CFGr,APOLLOMP_IND_NRM_PLMf,&data);
    *pValue = data;
    return ret;
}

void apollo_raw_gpon_dbru_enable(int32 value)
{
    uint32 tmp;

	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
	reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);

	value = !value;
    reg_field_write(APOLLOMP_GPON_GTC_US_CFGr,APOLLOMP_DBRU_DISf,&value);

	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);
}

int32 apollo_raw_gpon_dbru_enable_read(int32 *pValue)
{
    uint32 data;
    int32 ret;

    ret = reg_field_read(APOLLOMP_GPON_GTC_US_CFGr,APOLLOMP_DBRU_DISf,&data);
    *pValue = !data;
    return ret;
}

void apollo_raw_gpon_usScramble_enable(int32 value)
{
	uint32 tmp;

	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
	reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);

	value = !value;
    reg_field_write(APOLLOMP_GPON_GTC_US_CFGr,APOLLOMP_SCRM_DISf,&value);

	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);
}

int32 apollo_raw_gpon_usScramble_enable_read(int32 *pValue)
{
    uint32 data;
    int32 ret;

    ret = reg_field_read(APOLLOMP_GPON_GTC_US_CFGr,APOLLOMP_SCRM_DISf,&data);
    *pValue = !data;
    return ret;
}

void apollo_raw_gpon_usBurstPolarity_write(int32 value)
{
    uint32 tmp;

	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
	reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);

    reg_field_write(APOLLOMP_GPON_GTC_US_CFGr,APOLLOMP_US_BEN_POLARf,&value);

	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);
}

int32 apollo_raw_gpon_usBurstPolarity_read(int32 *value)
{
    return reg_field_read(APOLLOMP_GPON_GTC_US_CFGr,APOLLOMP_US_BEN_POLARf,value);
}

extern gpon_drv_obj_t *g_gponmac_drv;

void apollo_raw_gpon_eqd_write(uint32 value)
{
    uint32 data=0,min_delay1;
    uint32 eqd1,multi,intra;

	reg_field_read(APOLLOMP_GPON_GTC_US_MIN_DELAYr,APOLLOMP_MIN_DELAY1f,&min_delay1);

    eqd1 = value + (min_delay1*16*8) + (g_gponmac_drv->dev->eqd_offset);
    multi = (eqd1 /GPON_REG_EQD_FRAME_LEN);
    intra = (eqd1-multi*GPON_REG_EQD_FRAME_LEN);

    reg_field_set(APOLLOMP_GPON_GTC_US_EQDr,APOLLOMP_EQD1_MULTFRAMEf,&multi,&data);
	reg_field_set(APOLLOMP_GPON_GTC_US_EQDr,APOLLOMP_EQD1_INFRAMEf,&intra,&data);

	reg_write(APOLLOMP_GPON_GTC_US_EQDr,&data);

    GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"set eqd(%d) offset(%d) eqd1(%d) multi(%d) intra(%d)",value,(g_gponmac_drv->dev->eqd_offset),eqd1,multi,intra);
}

int32 apollo_raw_gpon_eqd_read(uint32 *pMultiFrame, uint32 *pIntraFrame)
{
    uint32 data;
    int32 ret;

    ret =  reg_read(APOLLOMP_GPON_GTC_US_EQDr, &data);

    reg_field_get(APOLLOMP_GPON_GTC_US_EQDr, APOLLOMP_EQD1_MULTFRAMEf, pMultiFrame, &data);
    reg_field_get(APOLLOMP_GPON_GTC_US_EQDr, APOLLOMP_EQD1_INFRAMEf, pIntraFrame, &data);

    return ret;
}

void apollo_raw_gpon_laserTime_write(uint8 on, uint8 off)
{
	uint32 data=0, value;

	value = on;
	reg_field_set(APOLLOMP_GPON_GTC_US_LASERr,APOLLOMP_LON_TIMEf,&value,&data);
	value = off;
	reg_field_set(APOLLOMP_GPON_GTC_US_LASERr,APOLLOMP_LOFF_TIMEf,&value,&data);

	reg_write(APOLLOMP_GPON_GTC_US_LASERr,&data);
}

int32 apollo_raw_gpon_laserTime_read(uint8 *on, uint8 *off)
{
    uint32 data,value;
	int32  ret;

    ret = reg_read(APOLLOMP_GPON_GTC_US_LASERr,&data);

	reg_field_get(APOLLOMP_GPON_GTC_US_LASERr,APOLLOMP_LON_TIMEf,&value,&data);
    *on  = (uint8)value;
    reg_field_get(APOLLOMP_GPON_GTC_US_LASERr,APOLLOMP_LOFF_TIMEf,&value,&data);
    *off  = (uint8)value;

	return ret;
}

void apollo_raw_gpon_burstOverhead_write(uint8 rep, uint8 len, uint8 size, uint8 *oh)
{
    uint8 i;
    uint32 data=0, tmp;

    if (rep) {}

#if 1 /* change the repeat to the last byte before delimiter */
    tmp = (size-4)&0x0F;
#else
    tmp = rep&0x0F;
#endif
    reg_field_set(APOLLOMP_GPON_GTC_US_BOH_CFGr, APOLLOMP_BOH_REPEATf, &tmp, &data);

    tmp = len;
    reg_field_set(APOLLOMP_GPON_GTC_US_BOH_CFGr, APOLLOMP_BOH_LENGTHf, &tmp, &data);

    reg_write(APOLLOMP_GPON_GTC_US_BOH_CFGr,&data);

    for(i=0;i<size;i++)
    {
        data = oh[i];
        reg_array_write(APOLLOMP_GPON_GTC_US_BOH_DATAr, REG_ARRAY_INDEX_NONE, i, &data);
    }
}

int32 apollo_raw_gpon_usPloam_write(int32 urgent, uint8 *value)
{
    uint32 data, ind, bit, tmp;
    uint8  i;

    ind = 0;
    if(urgent)
    {
        tmp = GPON_REG_TYPE_GTC_US_PLOAM_URG;
        reg_field_set(APOLLOMP_GPON_GTC_US_PLOAM_INDr, APOLLOMP_PLM_TYPEf, &tmp, &ind);
        bit = APOLLOMP_PLM_URG_FULLf;
    }
    else
    {
        tmp = GPON_REG_TYPE_GTC_US_PLOAM_NRM;
        reg_field_set(APOLLOMP_GPON_GTC_US_PLOAM_INDr, APOLLOMP_PLM_TYPEf, &tmp, &ind);
        bit = APOLLOMP_PLM_NRM_FULLf;
    }
    tmp = GPON_REG_STATUS_CLEAR;
    reg_field_set(APOLLOMP_GPON_GTC_US_PLOAM_INDr, APOLLOMP_PLM_ENQf, &tmp, &ind);
    reg_write(APOLLOMP_GPON_GTC_US_PLOAM_INDr,&ind);

	reg_field_read(APOLLOMP_GPON_GTC_US_PLOAM_INDr,bit,&data);
    if(data==GPON_REG_STATUS_CLEAR)
    {
        for(i=0;i<6;i++)
        {
            data = (value[2*i+0]<<GPON_REG_16BITS_HIGH)|(value[2*i+1]<<GPON_REG_16BITS_LOW);
            reg_array_write(APOLLOMP_GPON_GTC_US_PLOAM_DATAr, REG_ARRAY_INDEX_NONE, i, &data);
        }
        tmp = GPON_REG_STATUS_SET;
        reg_field_set(APOLLOMP_GPON_GTC_US_PLOAM_INDr, APOLLOMP_PLM_ENQf, &tmp, &ind);
        reg_write(APOLLOMP_GPON_GTC_US_PLOAM_INDr,&ind);
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_GPON_PLOAM_QUEUE_FULL;
    }
}

void apollo_raw_gpon_usAutoPloam_write(apollo_raw_gpon_usAutoPloamType_t type, uint8 *value)
{
    uint32 data, ind, tmp;
    uint8 i;

    ind = 0;
    if(type==GPON_REG_USAUTOPLOAM_SN)
    {
        tmp = GPON_REG_TYPE_GTC_US_PLOAM_SN;

    }
    else if(type==GPON_REG_USAUTOPLOAM_DYINGGASP)
    {
        tmp = GPON_REG_TYPE_GTC_US_PLOAM_DYINGGASP;
    }
	else
    {
        tmp = GPON_REG_TYPE_GTC_US_PLOAM_NOMSG;
    }
    reg_field_set(APOLLOMP_GPON_GTC_US_PLOAM_INDr, APOLLOMP_PLM_TYPEf, &tmp, &ind);

    tmp = GPON_REG_STATUS_CLEAR;
    reg_field_set(APOLLOMP_GPON_GTC_US_PLOAM_INDr, APOLLOMP_PLM_ENQf, &tmp, &ind);

    reg_write(APOLLOMP_GPON_GTC_US_PLOAM_INDr,&ind);

    for(i=0;i<6;i++)
    {
        data = (value[2*i+0]<<GPON_REG_16BITS_HIGH)|(value[2*i+1]<<GPON_REG_16BITS_LOW);
        reg_array_write(APOLLOMP_GPON_GTC_US_PLOAM_DATAr, REG_ARRAY_INDEX_NONE, i, &data);
    }

    tmp = GPON_REG_STATUS_SET;
    reg_field_set(APOLLOMP_GPON_GTC_US_PLOAM_INDr, APOLLOMP_PLM_ENQf, &tmp, &ind);
    reg_write(APOLLOMP_GPON_GTC_US_PLOAM_INDr,&ind);
}

void apollo_raw_gpon_usPloam_crcGen_enable(int32 value)
{
	reg_field_write(APOLLOMP_GPON_GTC_US_PLOAM_CFGr,APOLLOMP_PLM_US_CRC_GEN_ENf,&value);
}

int32 apollo_raw_gpon_usPloam_crcGen_read(int32 *value)
{
    return reg_field_read(APOLLOMP_GPON_GTC_US_PLOAM_CFGr,APOLLOMP_PLM_US_CRC_GEN_ENf,value);
}

void apollo_raw_gpon_usPloam_onuIdFilter_write(int32 value)
{
	reg_field_write(APOLLOMP_GPON_GTC_US_PLOAM_CFGr,APOLLOMP_PLM_US_ONUID_OVRD_ENf,&value);
}

int32 apollo_raw_gpon_usPloam_onuIdFilter_read(int32 *value)
{
    return reg_field_read(APOLLOMP_GPON_GTC_US_PLOAM_CFGr,APOLLOMP_PLM_US_ONUID_OVRD_ENf,value);
}
#if 1
void apollo_raw_gpon_usPloam_buf_flush_write(int32 value)
{
	reg_field_write(APOLLOMP_GPON_GTC_US_PLOAM_CFGr,APOLLOMP_PLM_FLUSH_BUFf,&value);
}

int32 apollo_raw_gpon_usPloam_buf_flush_read(int32 *value)
{
    return reg_field_read(APOLLOMP_GPON_GTC_US_PLOAM_CFGr,APOLLOMP_PLM_FLUSH_BUFf,value);
}
#endif
int32 apollo_raw_gpon_usGtc_miscCnt_read(apollo_raw_gpon_usGtc_pmMiscType_t idx, uint32 *pValue)
{
    int32 ret;

    RT_PARAM_CHK((idx>GPON_REG_USGTCPMMISC_GEM_BYTE_TX),RT_ERR_OUT_OF_RANGE);
    ret = reg_field_write(APOLLOMP_GPON_GTC_US_MISC_CNTR_IDXr,APOLLOMP_MISC_IDXf,&idx);
    if(ret != RT_ERR_OK)
        return ret;

    return reg_read(APOLLOMP_GPON_GTC_US_MISC_CNTR_STATr,pValue);
}

void apollo_raw_gpon_rdi_write(int32 value)
{
	reg_field_write(APOLLOMP_GPON_GTC_US_RDIr,APOLLOMP_ONU_RDIf,&value);
}

int32 apollo_raw_gpon_rdi_read(int32 *value)
{
    return reg_field_read(APOLLOMP_GPON_GTC_US_RDIr,APOLLOMP_ONU_RDIf,value);
}

void apollo_raw_gpon_usSmalSstartProc_enable(int32 value)
{
	uint32 tmp;

	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
	reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);

    reg_field_write(APOLLOMP_GPON_GTC_US_PROC_MODEr,APOLLOMP_AUTO_PROC_SSTARTf,&value);

	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);
}

int32 apollo_raw_gpon_usSmalSstartProc_enable_read(int32 *value)
{
    return reg_field_read(APOLLOMP_GPON_GTC_US_PROC_MODEr,APOLLOMP_AUTO_PROC_SSTARTf,value);
}

void apollo_raw_gpon_usSuppressLaser_enable(int32 value)
{
	uint32 tmp;

	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_ENA;
	reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);

    reg_field_write(APOLLOMP_GPON_GTC_US_PROC_MODEr,APOLLOMP_OPTIC_AUTO_SUPRESS_DISf,&value);

	tmp = GPON_REG_PTN_GTC_US_WRPROTECT_DIS;
    reg_write(APOLLOMP_GPON_GTC_US_WRITE_PROTECTr,&tmp);
}

int32 apollo_raw_gpon_usSuppressLaser_enable_read(int32 *value)
{
    return reg_field_read(APOLLOMP_GPON_GTC_US_PROC_MODEr,APOLLOMP_OPTIC_AUTO_SUPRESS_DISf,value);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_intr_read
 * Description:
 *      Read GPON GEM upstream interrupt indicator register
 * Input:
 *      reg_addr 	- void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_intr_read(uint32 *pValue)
{
    return reg_read(APOLLOMP_GPON_GEM_US_INTR_DLTr, pValue);
}

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_sdLong_check
 * Description:
 *      Indicate the Signal Detect valid more than 125us.
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_sdLong_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_GEM_US_INTR_DLTr, APOLLOMP_SD_VALID_LONG_DLTf, &value, &status);

    return value;
}

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_sdDiff_check
 * Description:
 *      Indicate the Signal Detect and the TX Burst enable has
 *      the huge difference for the Transmit Optical Module.
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_sdDiff_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_GEM_US_INTR_DLTr, APOLLOMP_SD_DIFF_HUGE_DLTf, &value, &status);

    return value;
}

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_reqDelay_check
 * Description:
 *      Indicate the GTC_US need request the next 125 us packets but
 *      the SWITCH also is busy on the last 125 us.
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_reqDelay_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_GEM_US_INTR_DLTr, APOLLOMP_REQUEST_DELAY_DLTf, &value, &status);

    return value;
}

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_bcLess6_check
 * Description:
 *      Indicate the byte counter don't take less than 6 and
 *      as Empty TCONT by Switch
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_bcLess6_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_GEM_US_INTR_DLTr, APOLLOMP_BC_LESS6_DLTf, &value, &status);

    return value;
}

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_errPLI_check
 * Description:
 *      Indicate the PLI mismatching with the input cycle from the Switch.
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_errPLI_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_GEM_US_INTR_DLTr, APOLLOMP_ERR_PLI_DLTf, &value, &status);

    return value;
}

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_burstLarge_check
 * Description:
 *      Indicate the TM(SWITCH) has the larger burst bytes
 *      than the GTC indication. In this case, GEM module will
 *      insert IDLE until bank (125us) end.
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_burstLarge_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_GEM_US_INTR_DLTr, APOLLOMP_BURST_TM_LARGER_GTC_DLTf, &value, &status);

    return value;
}

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_bankTooMuch_check
 * Description:
 *      Indicate the 1k bytes Bank has too much data at the time
 *      of leave 800 cycles (bytes). It will force the signal buffer
 *      full indication to un-valid in order to let the Switch can
 *      send the next bank data. It means the Switch gives too many
 *      traffic from request and full at the last 800 cycles of band read.
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_bankTooMuch_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_GEM_US_INTR_DLTr, APOLLOMP_BANK_TOO_MUCH_AT_END_DLTf, &value, &status);

    return value;
}

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_bankRemain_check
 * Description:
 *      Indicate the Bank has the remained data after the 125us bank read.
 *      It means the Switch gives too many traffic from request.
 *      (Too many response times or too many bytes in one response, etc.)
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_bankRemain_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_GEM_US_INTR_DLTr, APOLLOMP_BANK_REMAIN_AFRD_DLTf, &value, &status);

    return value;
}

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_bankOverflow_check
 * Description:
 *      Indicate Bank overflow. It means the Switch doesn¡¦t have the
 *      true back press machine.
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_bankOverflow_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_GEM_US_INTR_DLTr, APOLLOMP_BANK_OVERFL_DLTf, &value, &status);

    return value;
}

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_bankUnderflow_check
 * Description:
 *      Indicate Bank underflow. It is as timeout of request to response
 *      of interface. Insert IDLE in this case.
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_bankUnderflow_check(uint32 status)
{
    uint32 value=0;

    reg_field_get(APOLLOMP_GPON_GEM_US_INTR_DLTr, APOLLOMP_BANK_UNDERFL_DLTf, &value, &status);

    return value;
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_mask_read
 * Description:
 *      Read the GEM upstream interrupt mask.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_mask_read(uint32 *pValue)
{
    return reg_read(APOLLOMP_GPON_GEM_US_INTR_MASKr, pValue);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_mask_write
 * Description:
 *      Write the GEM upstream interrupt mask.
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
int32 apollo_raw_gpon_gemUs_mask_write(uint32 value)
{
    return reg_write(APOLLOMP_GPON_GEM_US_INTR_MASKr,&value);
}

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_sdLong_write
 * Description:
 *      Turn on/off the SD long valid interrupt mask.
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
int32 apollo_raw_gpon_gemUsrMask_sdLong_write(int32 value)
{
    return reg_field_write(APOLLOMP_GPON_GEM_US_INTR_MASKr, APOLLOMP_SD_VALID_LONG_Mf, &value);
}

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_sdDiff_write
 * Description:
 *      Turn on/off the SD huge difference interrupt mask.
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
int32 apollo_raw_gpon_gemUsrMask_sdDiff_write(int32 value)
{
    return reg_field_write(APOLLOMP_GPON_GEM_US_INTR_MASKr, APOLLOMP_SD_DIFF_HUGE_Mf, &value);
}

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_reqDelay_write
 * Description:
 *      Turn on/off the request delay interrupt mask.
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
int32 apollo_raw_gpon_gemUsrMask_reqDelay_write(int32 value)
{
    return reg_field_write(APOLLOMP_GPON_GEM_US_INTR_MASKr, APOLLOMP_REQUEST_DELAY_Mf, &value);
}

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_bcLess6_write
 * Description:
 *      Turn on/off the 'byte count less than 6' interrupt mask.
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
int32 apollo_raw_gpon_gemUsrMask_bcLess6_write(int32 value)
{
    return reg_field_write(APOLLOMP_GPON_GEM_US_INTR_MASKr, APOLLOMP_BC_LESS6_Mf, &value);
}

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_errPLI_write
 * Description:
 *      Turn on/off the error PLT interrupt mask.
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
int32 apollo_raw_gpon_gemUsrMask_errPLI_write(int32 value)
{
    return reg_field_write(APOLLOMP_GPON_GEM_US_INTR_MASKr, APOLLOMP_ERR_PLI_Mf, &value);
}

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_burstLarge_write
 * Description:
 *      Turn on/off the burst large interrupt mask.
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
int32 apollo_raw_gpon_gemUsrMask_burstLarge_write(int32 value)
{
    return reg_field_write(APOLLOMP_GPON_GEM_US_INTR_MASKr, APOLLOMP_BURST_TM_LARGER_GTC_Mf, &value);
}

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_bankTooMuch_write
 * Description:
 *      Turn on/off the bank too much data interrupt mask.
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
int32 apollo_raw_gpon_gemUsrMask_bankTooMuch_write(int32 value)
{
    return reg_field_write(APOLLOMP_GPON_GEM_US_INTR_MASKr, APOLLOMP_BANK_TOO_MUCH_AT_END_Mf, &value);
}

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_bankRemain_write
 * Description:
 *      Turn on/off the bank remain data interrupt mask.
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
int32 apollo_raw_gpon_gemUsrMask_bankRemain_write(int32 value)
{
    return reg_field_write(APOLLOMP_GPON_GEM_US_INTR_MASKr, APOLLOMP_BANK_REMAIN_AFRD_Mf, &value);
}

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_bankOverflow_write
 * Description:
 *      Turn on/off the bank overflow interrupt mask.
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
int32 apollo_raw_gpon_gemUsrMask_bankOverflow_write(int32 value)
{
    return reg_field_write(APOLLOMP_GPON_GEM_US_INTR_MASKr, APOLLOMP_BANK_OVERFL_Mf, &value);
}

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_bankUnderflow_write
 * Description:
 *      Turn on/off the bank underflow interrupt mask.
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
int32 apollo_raw_gpon_gemUsrMask_bankUnderflow_write(int32 value)
{
    return reg_field_write(APOLLOMP_GPON_GEM_US_INTR_MASKr, APOLLOMP_BANK_UNDERFL_Mf, &value);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_sdLong_read
 * Description:
 *      None
 * Input:
 *      value       - interrupt mask value
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_sdLong_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GEM_US_INTR_STSr, APOLLOMP_SD_VALID_LONG_INDf, pValue);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_sdDiff_read
 * Description:
 *      Read the SD huge difference status.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_sdDiff_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GEM_US_INTR_STSr, APOLLOMP_SD_DIFF_HUGE_INDf, pValue);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_sdLong_read
 * Description:
 *      Read the request delay status.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_reqDelay_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GEM_US_INTR_STSr, APOLLOMP_REQUEST_DELAY_INDf, pValue);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_bcLess6_read
 * Description:
 *      Read the 'byte count less than 6' status.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_bcLess6_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GEM_US_INTR_STSr, APOLLOMP_BC_LESS6_INDf, pValue);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_errPLI_read
 * Description:
 *      Read the error PLI status.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_errPLI_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GEM_US_INTR_STSr, APOLLOMP_ERR_PLI_INDf, pValue);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_burstLarge_read
 * Description:
 *      Read the burst large status.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_burstLarge_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GEM_US_INTR_STSr, APOLLOMP_BURST_TM_LARGER_GTC_INDf, pValue);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_bankTooMuch_read
 * Description:
 *      Read the bank too much data status.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_bankTooMuch_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GEM_US_INTR_STSr, APOLLOMP_BANK_TOO_INDUCH_AT_END_INDf, pValue);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_bankRemain_read
 * Description:
 *      Read the bank remain data status.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_bankRemain_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GEM_US_INTR_STSr, APOLLOMP_BANK_REMAIN_AFRD_INDf, pValue);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_bankOverflow_read
 * Description:
 *      Read the bank overflow status.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_bankOverflow_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GEM_US_INTR_STSr, APOLLOMP_BANK_OVERFL_INDf, pValue);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_bankUnderflow_read
 * Description:
 *      Read the bank underflow status.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_bankUnderflow_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GEM_US_INTR_STSr, APOLLOMP_BANK_UNDERFL_INDf, pValue);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_forceIdle_write
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
int32 apollo_raw_gpon_gemUs_forceIdle_write(uint32 value)
{
    return reg_field_write(APOLLOMP_GPON_GEM_US_PTI_CFGr, APOLLOMP_FS_GEM_IDLEf, &value);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_forceIdle_read
 * Description:
 *      Read the setting of force IDLE GEM.
 * Input:
 *      None
 * Output:
 *      pValue      - return value of setting
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_forceIdle_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GEM_US_PTI_CFGr, APOLLOMP_FS_GEM_IDLEf,  pValue);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_PTI_vector_write
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
int32 apollo_raw_gpon_gemUs_PTI_vector_write(uint8 pti_v0, uint8 pti_v1, uint8 pti_v2, uint8 pti_v3)
{
    uint32 data=0;
    int32  ret;
    uint32 pti_value;

    pti_value = pti_v0&0x7;
    ret = reg_field_set(APOLLOMP_GPON_GEM_US_PTI_CFGr, APOLLOMP_PTI_VECTOR0f, &pti_value, &data);
    if (ret != RT_ERR_OK)
        return ret;

    pti_value = pti_v1&0x7;
    ret = reg_field_set(APOLLOMP_GPON_GEM_US_PTI_CFGr, APOLLOMP_PTI_VECTOR1f, &pti_value, &data);
    if (ret != RT_ERR_OK)
        return ret;

    pti_value = pti_v2&0x7;
    ret = reg_field_set(APOLLOMP_GPON_GEM_US_PTI_CFGr, APOLLOMP_PTI_VECTOR2f, &pti_value, &data);
    if (ret != RT_ERR_OK)
        return ret;

    pti_value = pti_v3&0x7;
    ret = reg_field_set(APOLLOMP_GPON_GEM_US_PTI_CFGr, APOLLOMP_PTI_VECTOR3f, &pti_value, &data);
    if (ret != RT_ERR_OK)
        return ret;

    return reg_write(APOLLOMP_GPON_GEM_US_PTI_CFGr, &data);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_PTI_vector_read
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
int32 apollo_raw_gpon_gemUs_PTI_vector_read(uint8 *pPti_v0, uint8 *pPti_v1, uint8 *pPti_v2, uint8 *pPti_v3)
{
    uint32 data=0;
    int32  ret;
    uint32 pti_value;

    ret = reg_read(APOLLOMP_GPON_GEM_US_PTI_CFGr, &data);
    if (ret != RT_ERR_OK)
        return ret;

    reg_field_get(APOLLOMP_GPON_GEM_US_PTI_CFGr, APOLLOMP_PTI_VECTOR0f, &pti_value, &data);
    *pPti_v0 = pti_value&0x7;
    reg_field_get(APOLLOMP_GPON_GEM_US_PTI_CFGr, APOLLOMP_PTI_VECTOR1f, &pti_value, &data);
    *pPti_v1 = pti_value&0x7;
    reg_field_get(APOLLOMP_GPON_GEM_US_PTI_CFGr, APOLLOMP_PTI_VECTOR2f, &pti_value, &data);
    *pPti_v2 = pti_value&0x7;
    reg_field_get(APOLLOMP_GPON_GEM_US_PTI_CFGr, APOLLOMP_PTI_VECTOR3f, &pti_value, &data);
    *pPti_v3 = pti_value&0x7;

    return ret;
}

#if 0 /* APOLLOMP_GPON_GEM_US_OPT_CFG is removed in GPON_MAC_SWIO_r1.1 */
/* Function Name:
 *      apollo_raw_gpon_gemUs_txDis_write
 * Description:
 *      Set the use of TX disable for power saving.
 * Input:
 *      use_tx_dis  - specify to use TX disable or not
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_txDis_write(uint32 use_tx_dis)
{
    return reg_field_write(APOLLOMP_GPON_GEM_US_OPT_CFGr, APOLLOMP_USE_TX_OPT_DISABLEf, &use_tx_dis);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_txDis_read
 * Description:
 *      Read the setting of TX disable for power saving.
 * Input:
 *      None
 * Output:
 *      pValue      - return value of setting
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_txDis_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GEM_US_OPT_CFGr, APOLLOMP_USE_TX_OPT_DISABLEf, pValue);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_optBehindCycle_write
 * Description:
 *      Set the optical behind cycle.
 * Input:
 *      cycle   - specify the cycle value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_optBehindCycle_write(uint32 cycle)
{
    return reg_field_write(APOLLOMP_GPON_GEM_US_OPT_CFGr, APOLLOMP_OPT_BEHIND_CYCLESf, &cycle);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_optBehindCycle_read
 * Description:
 *      Read the setting of optical behind cycle.
 * Input:
 *      None
 * Output:
 *      pValue      - return value of setting
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_optBehindCycle_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GEM_US_OPT_CFGr, APOLLOMP_OPT_BEHIND_CYCLESf, pValue);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_optAheadCycle_write
 * Description:
 *      Set the optical ahead cycle.
 * Input:
 *      cycle   - specify the cycle value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_optAheadCycle_write(uint32 cycle)
{
    return reg_field_write(APOLLOMP_GPON_GEM_US_OPT_CFGr, APOLLOMP_OPT_AHEAD_CYCLESf, &cycle);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_optAheadCycle_read
 * Description:
 *      Read the setting of optical ahead cycle.
 * Input:
 *      None
 * Output:
 *      pValue      - return value of setting
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_optAheadCycle_read(uint32 *pValue)
{
    reg_field_read(APOLLOMP_GPON_GEM_US_OPT_CFGr, APOLLOMP_OPT_AHEAD_CYCLESf, pValue);
}
#endif

#if 0 /* APOLLOMP_GPON_GEM_US_OPT_SD will move to GTC_US */
/* Function Name:
 *      apollo_raw_gpon_gemUs_txSd_read
 * Description:
 *      Indicate the TX Signal Detect vaild.
 * Input:
 *      None
 * Output:
 *      pValue      - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_txSd_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GEM_US_OPT_SDr, APOLLOMP_TX_OPT_SD_HVf, pValue);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_txSdPolar_write
 * Description:
 *      Set the palarity setting of TX Signal Detect vaild.
 * Input:
 *      value       - specify the polarity value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_txSdPolar_write(uint32 value)
{
    return reg_field_write(APOLLOMP_GPON_GEM_US_OPT_SDr, APOLLOMP_TX_OPT_SD_POLARf, &value);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_txSdPolar_read
 * Description:
 *      Read the palarity setting of TX Signal Detect vaild.
 * Input:
 *      None
 * Output:
 *      pValue      - return value of setting
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_txSdPolar_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GEM_US_OPT_SDr, APOLLOMP_TX_OPT_SD_POLARf, pValue);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_sdDiffCycle_write
 * Description:
 *      Set the setting of SD difference cycle.
 * Input:
 *      cycle       - specify the cycle value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_sdDiffCycle_write(uint32 cycle)
{
    return reg_field_write(APOLLOMP_GPON_GEM_US_OPT_SDr, APOLLOMP_SD_DIFF_CYCLESf, &cycle);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_sdDiffCycle_read
 * Description:
 *      Read the setting of SD difference cycle.
 * Input:
 *      None
 * Output:
 *      pValue      - return value of setting
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_sdDiffCycle_read(uint32 *pValue)
{
    return reg_field_read(APOLLOMP_GPON_GEM_US_OPT_SDr, APOLLOMP_SD_DIFF_CYCLESf, pValue);
}
#endif

/* Function Name:
 *      apollo_raw_gpon_gemUs_ethCnt_read
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
int32 apollo_raw_gpon_gemUs_ethCnt_read(uint32 local_idx, uint32 *pEth_cntr)
{
    uint32 data;

    data = 0x80 | (local_idx & 0x7F);

    reg_field_write(APOLLOMP_GPON_GEM_US_ETH_GEM_RX_CNTR_IDXr, APOLLOMP_ETH_GEM_RX_IDXf, &data);

    if(apollo_raw_gpon_indirect_wait(APOLLOMP_GPON_GEM_US_ETH_GEM_RX_CNTR_IDXr))
    {
		return reg_read(APOLLOMP_GPON_GEM_US_ETH_GEM_RX_CNTR_STATr, pEth_cntr);
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_gemCnt_read
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
int32 apollo_raw_gpon_gemUs_gemCnt_read(uint32 local_idx, uint32 *pGem_cntr)
{
    uint32 data;

    data = (local_idx & 0x7F);

    reg_field_write(APOLLOMP_GPON_GEM_US_ETH_GEM_RX_CNTR_IDXr, APOLLOMP_ETH_GEM_RX_IDXf, &data);

    if(apollo_raw_gpon_indirect_wait(APOLLOMP_GPON_GEM_US_ETH_GEM_RX_CNTR_IDXr))
    {
		return reg_read(APOLLOMP_GPON_GEM_US_ETH_GEM_RX_CNTR_STATr, pGem_cntr);
    }
    else
    {
        return RT_ERR_GPON_INVALID_HANDLE;
    }
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_portCfg_write
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
int32 apollo_raw_gpon_gemUs_portCfg_write(uint32 local_idx, uint32 gem_port_id)
{
    return reg_array_field_write(APOLLOMP_GPON_GEM_US_PORT_MAPr,
                                 REG_ARRAY_INDEX_NONE,
                                 local_idx,
                                 APOLLOMP_PORT_CFG_DATAf,
                                 &gem_port_id);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_portCfg_read
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
int32 apollo_raw_gpon_gemUs_portCfg_read(uint32 local_idx, uint32 *pGem_port_id)
{
    return reg_array_field_read(APOLLOMP_GPON_GEM_US_PORT_MAPr,
                                REG_ARRAY_INDEX_NONE,
                                local_idx,
                                APOLLOMP_PORT_CFG_DATAf,
                                pGem_port_id);
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_dataByteCnt_read
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
int32 apollo_raw_gpon_gemUs_dataByteCnt_read(uint32 local_idx, uint64 *pByte_cntr)
{
    int32 ret;

    ret = reg_array_read(APOLLOMP_GPON_GEM_US_BYTE_STATr,
                         REG_ARRAY_INDEX_NONE,
                         local_idx,
                         (uint32 *)pByte_cntr);
    *pByte_cntr = ((*pByte_cntr >> 32) & 0xFFFFFFFF) |
                  ((*pByte_cntr & 0xFFFFFFFF) << 32);

    return ret;
}

/* Function Name:
 *      apollo_raw_gpon_gemUs_idleByteCnt_read
 * Description:
 *      Read the IDLE byte counter for local TCONT.
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
int32 apollo_raw_gpon_gemUs_idleByteCnt_read(uint32 local_idx, uint64 *pIdle_cntr)
{
    int32 ret;

    ret = reg_array_read(APOLLOMP_TCONT_IDLE_BYTE_STATr,
                         REG_ARRAY_INDEX_NONE,
                         local_idx,
                         (uint32 *)pIdle_cntr);
    *pIdle_cntr = ((*pIdle_cntr >> 32) & 0xFFFFFFFF) |
                  ((*pIdle_cntr & 0xFFFFFFFF) << 32);

    return ret;
}



#if defined(OLD_FPGA_DEFINED)
#define PKT_GEN_W_OFFSET 0xC000
#define PKT_GEN_R_OFFSET 0x8000

int32 packet_gen_read(uint32 address, uint16 *pValue)
{
    io_mii_phy_reg_write(11, 0, PKT_GEN_R_OFFSET|(address&0xFFF));
    io_mii_phy_reg_read(11, 2, pValue);

    GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"packet_gen_read addr=0x%x, value=0x%04x\n\r", address, *pValue);
}

int32 packet_gen_write(uint32 address, uint16 data)
{
    uint16 value;
    io_mii_phy_reg_write(11, 1, data);
    io_mii_phy_reg_write(11, 0, PKT_GEN_W_OFFSET|(address&0xFFF));

    GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"packet_gen_write addr=0x%x, data=0x%04x\n\r", address, data);

    packet_gen_read(address, &value);
}




#define PKT_GEN_ADDR_BUF_BASE           0x0
#define PKT_GEN_ADDR_BUF_OFFSET         5

#define PKT_GEN_ADDR_TCONT_BASE         0x40
#define PKT_GEN_ADDR_TCONT_OFFSET       1
#define PKT_GEN_FIELD_TCONT_OFFSET              11
#define PKT_GEN_FIELD_BC_OFFSET                 0

#define PKT_GEN_ADDR_GEM_BASE           0x41
#define PKT_GEN_ADDR_GEM_OFFSET         1
#define PKT_GEN_FIELD_EN_OFFSET                 10
#define PKT_GEN_FIELD_OMCI_OFFSET               9
#define PKT_GEN_FIELD_GEM_OFFSET                0

#define PKT_GEN_ADDR_RAM_BUF_BASE_1     0x46
#define PKT_GEN_ADDR_RAM_BUF_BASE_2     0x47
#define PKT_GEN_ADDR_RAM_CTRL_BASE      0x48
#define PKT_GEN_FIELD_RAM_RES_OFFSET            8
#define PKT_GEN_FIELD_RAM_DELAY_OFFSET          0


#define PKT_GEN_REQ_DELAY               0x2

#define ITEM_BUF_ADDR(__item)       (PKT_GEN_ADDR_BUF_BASE   | (__item<<PKT_GEN_ADDR_BUF_OFFSET))
#define ITEM_TCONT_ADDR(__item)     (PKT_GEN_ADDR_TCONT_BASE | (__item<<PKT_GEN_ADDR_TCONT_OFFSET))
#define ITEM_GEM_ADDR(__item)       (PKT_GEN_ADDR_GEM_BASE   | (__item<<PKT_GEN_ADDR_GEM_OFFSET))


int32 apollo_raw_gpon_pktGen_cfg_write(uint32 item, uint32 tcont, uint32 len, uint32 gem, int32 omci)
{
    int32  enable;
    uint16 data;

    /* set TCONT */
    data = (((tcont&0x1F)<<PKT_GEN_FIELD_TCONT_OFFSET) |
            ((len&0x3FF)<<PKT_GEN_FIELD_BC_OFFSET));
    packet_gen_write(ITEM_TCONT_ADDR(item), data);

    /* set GEM */
    if(gem == 0xFF)
        enable = 0;
    else
        enable = 1;

    data = (((gem&0x7F)<<PKT_GEN_FIELD_GEM_OFFSET) |
            ((omci&0x1)<<PKT_GEN_FIELD_OMCI_OFFSET) |
            ((enable&0x1)<<PKT_GEN_FIELD_EN_OFFSET));
    packet_gen_write(ITEM_GEM_ADDR(item), data);

    GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"pktGen cfg: item=%d, tcont=%d, len=%d, gem=%d, omci=%d\n\r",item,tcont,len,gem,omci);

    return RT_ERR_OK;
}

int32 apollo_raw_gpon_pktGen_buf_write(uint32 item, uint8 *buf, uint32 len)
{
    int32 i;
    uint16 data;

    /* set buf */
    if((item == 0) || (item == 1))
    {
        for(i=0;i<len;i+=2)
        {
            data = (buf[i]) | (buf[i+1] << 8);
            packet_gen_write(((ITEM_BUF_ADDR(item))+(i/2)), data);
        }
    }
    else if(item == 2)
    {
        data = ((1<<PKT_GEN_FIELD_RAM_RES_OFFSET) |
                (PKT_GEN_REQ_DELAY<<PKT_GEN_FIELD_RAM_DELAY_OFFSET));
        packet_gen_write(PKT_GEN_ADDR_RAM_CTRL_BASE, data);

        for(i=0;i<len;i+=4)
        {
            data = (buf[i]<<8) | (buf[i+1]);
            packet_gen_write(PKT_GEN_ADDR_RAM_BUF_BASE_1, data);
            data = (buf[i+2]<<8) | (buf[i+3]);
            packet_gen_write(PKT_GEN_ADDR_RAM_BUF_BASE_2, data);
        }
    }

    GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"pktGen buf: \n\r");
    for(i=0;i<len;i++)
        GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"%02x-",buf[i]);

    return RT_ERR_OK;
}
#endif /* OLD_FPGA_DEFINED */

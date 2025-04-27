/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public MIRROR APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Port-based mirror
 *           2) Group-based mirror
 *           3) RSPAN
 *           4) Mirror-based SFLOW
 *           5) Port-based SFLOW
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/maple/rtk_maple_table_struct.h>
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/maple/dal_maple_mirror.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/mirror.h>
#include <ioal/mem32.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32               mirror_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         mirror_sem[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Declaration
 */
#define MIRROR_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(mirror_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_MIRROR|MOD_DAL), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)

#define MIRROR_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(mirror_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_MIRROR|MOD_DAL), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

#define MIRROT_PORTMASK_MEMBER_PORT_CHECK(portmask, port, result)\
    do{\
        result = RTK_PORTMASK_IS_PORT_SET(portmask, port); \
    } while(0)

/*
 * Function Declaration
 */

/* Module Name : Mirror */

/* Function Name:
 *      dal_maple_mirrorMapper_init
 * Description:
 *      Hook mirror module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook mirror module before calling any mirror APIs.
 */
int32
dal_maple_mirrorMapper_init(dal_mapper_t *pMapper)
{
    pMapper->mirror_init = dal_maple_mirror_init;
    pMapper->mirror_group_init = dal_maple_mirror_group_init;
    pMapper->mirror_group_get = dal_maple_mirror_group_get;
    pMapper->mirror_group_set = dal_maple_mirror_group_set;
    pMapper->mirror_rspanIgrMode_get = dal_maple_mirror_rspanIgrMode_get;
    pMapper->mirror_rspanIgrMode_set = dal_maple_mirror_rspanIgrMode_set;
    pMapper->mirror_rspanEgrMode_get = dal_maple_mirror_rspanEgrMode_get;
    pMapper->mirror_rspanEgrMode_set = dal_maple_mirror_rspanEgrMode_set;
    pMapper->mirror_rspanTag_get = dal_maple_mirror_rspanTag_get;
    pMapper->mirror_rspanTag_set = dal_maple_mirror_rspanTag_set;
    pMapper->mirror_sflowMirrorSampleRate_get = dal_maple_mirror_sflowMirrorSampleRate_get;
    pMapper->mirror_sflowMirrorSampleRate_set = dal_maple_mirror_sflowMirrorSampleRate_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_mirror_init
 * Description:
 *      Initialize the mirroring database.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize Mirror module before calling any Mirror APIs.
 */
int32
dal_maple_mirror_init(uint32 unit)
{
    int32   ret;
    uint32  value = 0;
    uint32  mirror_id, max_mirror_id;

    RT_LOG(LOG_DEBUG, (MOD_MIRROR|MOD_DAL), "unit=%d", unit);

    RT_INIT_REENTRY_CHK(mirror_init[unit]);
    mirror_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    mirror_sem[unit] = osal_sem_mutex_create();
    if (0 == mirror_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_MIRROR|MOD_DAL), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    mirror_init[unit] = INIT_COMPLETED;

    /* Initial mirror cross vlan enable, and flowbased mirror ignore portmask setting */
    value = 1;
    max_mirror_id = HAL_MAX_NUM_OF_MIRROR(unit);
    for (mirror_id = 0; mirror_id < max_mirror_id; mirror_id++)
    {
        /* set value to CHIP*/
        if ((ret = reg_array_field_write(unit, MAPLE_MIR_CTRLr
                            , REG_ARRAY_INDEX_NONE, mirror_id, MAPLE_CROSS_VLANf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
            return ret;
        }

        if ((ret = reg_array_field_write(unit, MAPLE_MIR_CTRLr
                            , REG_ARRAY_INDEX_NONE, mirror_id, MAPLE_FLOW_BASED_PMSK_IGNOREf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
            return ret;
        }
    }


    return RT_ERR_OK;
} /* end of dal_maple_mirror_init */


/* Module Name    : Mirror             */
/* Sub-module Name: Group-based mirror */

/* Function Name:
 *      dal_maple_mirror_group_init
 * Description:
 *      Initialization mirror group entry.
 * Input:
 *      unit         - unit id
 *      pMirrorEntry - mirror entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_mirror_group_init(
    uint32              unit,
    rtk_mirror_entry_t  *pMirrorEntry)
{
    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMirrorEntry), RT_ERR_NULL_POINTER);

    pMirrorEntry->mirror_enable = DISABLED;
    pMirrorEntry->oper_of_igr_and_egr_ports = 0;
    pMirrorEntry->mirror_orginalPkt = ENABLED;
    pMirrorEntry->mirroring_port_egrMode = FORWARD_ALL_PKTS;

    /* Added for RTL8380 */
    pMirrorEntry->cross_vlan = ENABLED; /* enable mirror cross vlan */
    pMirrorEntry->flowBasedOnly = DISABLED;
    pMirrorEntry->duplicate_fltr = 0;
    pMirrorEntry->self_flter = 0;
    pMirrorEntry->mir_mode = 0;
    pMirrorEntry->mir_qid = 0;
    pMirrorEntry->mir_qid_en = 0;
    pMirrorEntry->flowBased_pmsk_ignore = 1;    /* flowbased mirror ignore portmask */

    return RT_ERR_OK;
} /* end of dal_maple_mirror_group_init */

/* Function Name:
 *      dal_maple_mirror_group_get
 * Description:
 *      Get mirror group entry.
 * Input:
 *      unit         - unit id
 *      mirror_id    - mirror id
 * Output:
 *      pMirrorEntry - mirror entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MIRROR_ID    - invalid mirror id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_mirror_group_get(
    uint32              unit,
    uint32              mirror_id,
    rtk_mirror_entry_t  *pMirrorEntry)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, pMirrorEntry=%x"
            , unit, pMirrorEntry);

    /* parameter check */
    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);
    RT_PARAM_CHK((NULL == pMirrorEntry), RT_ERR_NULL_POINTER);

    MIRROR_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_MIR_CTRLr,  REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_MIR_ENf, &pMirrorEntry->mirror_enable)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_MIR_CTRLr,  REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_MIR_DST_Pf, &pMirrorEntry->mirroring_port)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_MIR_SPM_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_SPM_0f, &pMirrorEntry->mirrored_igrPorts.bits[0])) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_MIR_DPM_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_DPM_0f, &pMirrorEntry->mirrored_egrPorts.bits[0])) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_MIR_OPf, &pMirrorEntry->oper_of_igr_and_egr_ports)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_MIR_SELf, &pMirrorEntry->mirror_orginalPkt)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_DST_P_ISOf, &pMirrorEntry->mirroring_port_egrMode)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_CROSS_VLANf, &pMirrorEntry->cross_vlan)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_FLOW_BASED_ONLYf, &pMirrorEntry->flowBasedOnly)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_DUPLICATE_FLTRf, &pMirrorEntry->duplicate_fltr)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_SELF_FLTERf, &pMirrorEntry->self_flter)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_MIR_MODE_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_MIR_MODEf, &pMirrorEntry->mir_mode)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_MIR_QID_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_MIR_QIDf, &pMirrorEntry->mir_qid)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_MIR_QID_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_MIR_QID_ENf, &pMirrorEntry->mir_qid_en)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_FLOW_BASED_PMSK_IGNOREf, &pMirrorEntry->flowBased_pmsk_ignore)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_mirror_group_get */

/* Function Name:
 *      dal_maple_mirror_group_set
 * Description:
 *      Set mirror group entry.
 * Input:
 *      unit         - unit id
 *      mirror_id    - mirror id
 *      pMirrorEntry - mirror entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT  - The module is not initial
 *      RT_ERR_MIRROR_ID - invalid mirror id
 *      RT_ERR_PORT_ID   - invalid mirroring port id
 *      RT_ERR_PORT_MASK - invalid mirrored ingress or egress portmask
 *      RT_ERR_INPUT     - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_mirror_group_set(
    uint32              unit,
    uint32              mirror_id,
    rtk_mirror_entry_t  *pMirrorEntry)
{
    uint32  val;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, pMirrorEntry=%x"
            , unit, pMirrorEntry);

    /* parameter check */
    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);
    RT_PARAM_CHK((NULL == pMirrorEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, pMirrorEntry->mirroring_port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(pMirrorEntry->oper_of_igr_and_egr_ports >= MIRROR_OP_END, RT_ERR_INPUT);
    RT_PARAM_CHK(pMirrorEntry->mirror_orginalPkt >= RTK_ENABLE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(pMirrorEntry->mirror_enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    /*Added cross vlan, flow based only and dst_p_iso check for RTL8380*/
    RT_PARAM_CHK(pMirrorEntry->cross_vlan >= RTK_ENABLE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(pMirrorEntry->flowBasedOnly >= RTK_ENABLE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(pMirrorEntry->mirroring_port_egrMode >= MIRROR_EGR_MODE_END, RT_ERR_INPUT);

    /*Check the Mirroring port is included in Igr/Egr portmask setting or NOT*/
    MIRROT_PORTMASK_MEMBER_PORT_CHECK(pMirrorEntry->mirrored_egrPorts,pMirrorEntry->mirroring_port,ret);
    RT_PARAM_CHK(ret, RT_ERR_MIRROR_DP_IN_SPM_DPM);
    MIRROT_PORTMASK_MEMBER_PORT_CHECK(pMirrorEntry->mirrored_igrPorts,pMirrorEntry->mirroring_port,ret);
    RT_PARAM_CHK(ret, RT_ERR_MIRROR_DP_IN_SPM_DPM);

    MIRROR_SEM_LOCK(unit);

    /* set value to CHIP*/
    val = pMirrorEntry->mirror_enable;
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_MIR_ENf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->mirroring_port;
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_MIR_DST_Pf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->mirrored_igrPorts.bits[0];
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_SPM_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_SPM_0f, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->mirrored_egrPorts.bits[0];
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_DPM_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_DPM_0f, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->oper_of_igr_and_egr_ports;
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_MIR_OPf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->mirror_orginalPkt;
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_MIR_SELf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->mirroring_port_egrMode;
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_DST_P_ISOf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    /*Added for cross vlan and flow based for RTL8380*/
    val = pMirrorEntry->cross_vlan;
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_CROSS_VLANf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->flowBasedOnly;
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_FLOW_BASED_ONLYf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->duplicate_fltr;
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_DUPLICATE_FLTRf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->self_flter;
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_SELF_FLTERf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->mir_qid_en;
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_MIR_QID_ENf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->mir_qid;
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_QID_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_MIR_QIDf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_MIR_QIDf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->mir_qid_en;
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_QID_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_MIR_QID_ENf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_MIR_QID_ENf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->mir_mode;
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_MODE_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_MIR_MODEf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->flowBased_pmsk_ignore;
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                MAPLE_FLOW_BASED_PMSK_IGNOREf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_mirror_group_set */


/* Module Name    : Mirror */
/* Sub-module Name: RSPAN  */

/* Function Name:
 *      dal_maple_mirror_rspanIgrMode_get
 * Description:
 *      Get ingress mode of RSPAN on specified mirror group.
 * Input:
 *      unit      - unit id
 *      mirror_id - mirror id
 * Output:
 *      pIgrMode  - pointer to ingress mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MIRROR_ID    - invalid mirror id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Ingress mode is as following:
 *      - RSPAN_IGR_HANDLE_RSPAN_TAG
 *      - RSPAN_IGR_IGNORE_RSPAN_TAG
 */
int32
dal_maple_mirror_rspanIgrMode_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanIgrMode_t *pIgrMode)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, mirror id=%d, pIgrMode=%x"
            , unit, mirror_id, pIgrMode);

    /* parameter check */
    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);
    RT_PARAM_CHK((NULL == pIgrMode), RT_ERR_NULL_POINTER);

    MIRROR_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_MIR_RSPAN_RX_TAG_EN_CTRLr, REG_ARRAY_INDEX_NONE,
        mirror_id, MAPLE_RSPAN_RX_TAG_ENf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "ret=%x", ret);
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    switch (value)
    {
        case 0:
            *pIgrMode = RSPAN_IGR_IGNORE_RSPAN_TAG;
            break;

        case 1:
            *pIgrMode = RSPAN_IGR_HANDLE_RSPAN_TAG;
            break;

        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_maple_mirror_rspanIgrMode_get */

/* Function Name:
 *      dal_maple_mirror_rspanIgrMode_set
 * Description:
 *      Set ingress mode of RSPAN on specified mirror group.
 * Input:
 *      unit      - unit id
 *      mirror_id - mirror id
 *      igrMode   - ingress mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT  - The module is not initial
 *      RT_ERR_MIRROR_ID - invalid mirror id
 *      RT_ERR_INPUT     - invalid input parameter
 * Note:
 *      Ingress mode is as following:
 *      - RSPAN_IGR_HANDLE_RSPAN_TAG
 *      - RSPAN_IGR_IGNORE_RSPAN_TAG
 */
int32
dal_maple_mirror_rspanIgrMode_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanIgrMode_t igrMode)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, mirror id=%d, igrMode=%u"
            , unit, mirror_id, igrMode);

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);
    RT_PARAM_CHK(igrMode >= RSPAN_IGR_END, RT_ERR_INPUT);

    switch (igrMode)
    {
        case RSPAN_IGR_IGNORE_RSPAN_TAG:
            value = 0;
            break;
        case RSPAN_IGR_HANDLE_RSPAN_TAG:
            value = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }

    MIRROR_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_RSPAN_RX_TAG_EN_CTRLr, REG_ARRAY_INDEX_NONE,
        mirror_id, MAPLE_RSPAN_RX_TAG_ENf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "ret=%x", ret);
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_mirror_rspanIgrMode_set */

/* Function Name:
 *      dal_maple_mirror_rspanEgrMode_get
 * Description:
 *      Get egress mode of RSPAN on specified mirror group.
 * Input:
 *      unit      - unit id
 *      mirror_id - mirror id
 * Output:
 *      pEgrMode  - pointer to egress mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MIRROR_ID    - invalid mirror id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Egress mode is as following:
 *      - RSPAN_EGR_REMOVE_TAG
 *      - RSPAN_EGR_ADD_TAG
 *      - RSPAN_EGR_NO_MODIFY
 */
int32
dal_maple_mirror_rspanEgrMode_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanEgrMode_t *pEgrMode)
{
    int32   ret;
    uint32  addTag, removeTag;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, mirror_id=%d, pEgrMode=%x"
            , unit, mirror_id, pEgrMode);

    /* parameter check */
    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);
    RT_PARAM_CHK((NULL == pEgrMode), RT_ERR_NULL_POINTER);

    MIRROR_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_MIR_RSPAN_TX_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, MAPLE_RSPAN_TX_TAG_ADDf, &addTag)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "ret=%x", ret);
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_MIR_RSPAN_RX_TAG_RM_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, MAPLE_RSPAN_RX_TAG_RMf, &removeTag)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "ret=%x", ret);
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    switch ((addTag << 1)|removeTag)
    {
        case 0:
            *pEgrMode = RSPAN_EGR_NO_MODIFY;
            break;
        case 1:
            *pEgrMode = RSPAN_EGR_REMOVE_TAG;
            break;
        case 2:
            *pEgrMode = RSPAN_EGR_ADD_TAG;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_maple_mirror_rspanEgrMode_get */

/* Function Name:
 *      dal_maple_mirror_rspanEgrMode_set
 * Description:
 *      Set egress mode of RSPAN on specified mirror group.
 * Input:
 *      unit      - unit id
 *      mirror_id - mirror id
 *      egrMode   - egress mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT  - The module is not initial
 *      RT_ERR_MIRROR_ID - invalid mirror id
 *      RT_ERR_INPUT     - invalid input parameter
 * Note:
 *      Egress mode is as following:
 *      - RSPAN_EGR_REMOVE_TAG
 *      - RSPAN_EGR_ADD_TAG
 *      - RSPAN_EGR_NO_MODIFY
 */
int32
dal_maple_mirror_rspanEgrMode_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanEgrMode_t egrMode)
{
    int32   ret;
    uint32  addTag, removeTag;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, mirror id=%d, egrMode=%u"
            , unit, mirror_id, egrMode);

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);
    RT_PARAM_CHK((egrMode >= RSPAN_EGR_END), RT_ERR_INPUT);

    switch (egrMode)
    {
        case RSPAN_EGR_NO_MODIFY:
            addTag = 0;
            removeTag = 0;
            break;
        case RSPAN_EGR_REMOVE_TAG:
            addTag = 0;
            removeTag = 1;
            break;
        case RSPAN_EGR_ADD_TAG:
            addTag = 1;
            removeTag = 0;
            break;
        default:
            return RT_ERR_INPUT;
    }

    MIRROR_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_RSPAN_TX_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, MAPLE_RSPAN_TX_TAG_ADDf, &addTag)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "ret=%x", ret);
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_MIR_RSPAN_RX_TAG_RM_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, MAPLE_RSPAN_RX_TAG_RMf, &removeTag)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "ret=%x", ret);
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_mirror_rspanEgrMode_set */

/* Function Name:
 *      dal_maple_mirror_rspanTag_get
 * Description:
 *      Get content of RSPAN tag on specified mirror group.
 * Input:
 *      unit      - unit id
 *      mirror_id - mirror id
 * Output:
 *      pTag      - pointer to content of RSPAN tag
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MIRROR_ID    - invalid mirror id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_mirror_rspanTag_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanTag_t *pTag)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, mirror_id=%d"
            , unit, mirror_id);

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTag), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);

    MIRROR_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, MAPLE_RSPAN_TAG_TPIDf, &(pTag->tpid))) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, MAPLE_RSPAN_TAG_PRIf, &(pTag->pri))) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, MAPLE_RSPAN_TAG_CFIf, &(pTag->cfi))) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, MAPLE_RSPAN_TAG_VIDf, &(pTag->vid))) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_mirror_rspanTag_get */

/* Function Name:
 *      dal_maple_mirror_rspanTag_set
 * Description:
 *      Set content of RSPAN tag on specified mirroring group.
 * Input:
 *      unit      - unit id
 *      mirror_id - mirror id
 *      pTag      - content of RSPAN tag
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MIRROR_ID    - invalid mirror id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_mirror_rspanTag_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanTag_t *pTag)
{
    uint32  val;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTag), RT_ERR_NULL_POINTER);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, mirror id=%d, pTag=%x"
            , unit, mirror_id, pTag);
    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);
    RT_PARAM_CHK((pTag->tpid > RTK_ETHERTYPE_MAX), RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(pTag->vid > RTK_VLAN_ID_MAX, RT_ERR_VLAN_VID);
    RT_PARAM_CHK(RTK_DOT1P_PRIORITY_MAX < pTag->pri, RT_ERR_PRIORITY);
    RT_PARAM_CHK(pTag->cfi > RTK_DOT1P_CFI_MAX, RT_ERR_INPUT);

    MIRROR_SEM_LOCK(unit);

    /* programming value on CHIP*/
    val = pTag->tpid;
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, MAPLE_RSPAN_TAG_TPIDf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_MIR_RSPAN_VLAN_CTRL_MACr
                        , REG_ARRAY_INDEX_NONE, mirror_id, MAPLE_RSPAN_TAG_TPIDf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pTag->pri;
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, MAPLE_RSPAN_TAG_PRIf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pTag->cfi;
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, MAPLE_RSPAN_TAG_CFIf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pTag->vid;
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, MAPLE_RSPAN_TAG_VIDf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if((ret = reg_array_field_write(unit, MAPLE_MIR_RSPAN_VLAN_CTRL_MACr
                        , REG_ARRAY_INDEX_NONE, mirror_id, MAPLE_RSPAN_TAG_VIDf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_mirror_rspanTag_set */


/* Module Name    : Mirror             */
/* Sub-module Name: Mirror-based SFLOW */

/* Function Name:
 *      dal_maple_mirror_sflowMirrorSampleRate_get
 * Description:
 *      Get sampling rate of specified mirror group.
 * Input:
 *      unit      - unit id
 *      mirror_id - mirror id
 * Output:
 *      pRate     - pointer to sampling rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MIRROR_ID    - invalid mirror id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_mirror_sflowMirrorSampleRate_get(uint32 unit, uint32 mirror_id, uint32 *pRate)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, mirror_id=%d"
            , unit, mirror_id);

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    MIRROR_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, MAPLE_MIR_SAMPLE_RATE_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, MAPLE_SAMPLE_RATEf, pRate)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }
    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_mirror_sflowMirrorSampleRate_get */

/* Function Name:
 *      dal_maple_mirror_sflowMirrorSampleRate_set
 * Description:
 *      Set sampling rate of specified mirror group.
 * Input:
 *      unit      - unit id
 *      mirror_id - mirror id
 *      rate      - sampling rate
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT  - The module is not initial
 *      RT_ERR_MIRROR_ID - invalid mirror id
 *      RT_ERR_INPUT     - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_mirror_sflowMirrorSampleRate_set(uint32 unit, uint32 mirror_id, uint32 rate)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, mirror_id=%d, rate=%u"
            , unit, mirror_id, rate);

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);
    RT_PARAM_CHK(rate > HAL_SFLOW_RATE_MAX(unit), RT_ERR_OUT_OF_RANGE);

    MIRROR_SEM_LOCK(unit);
    val = rate;
    if ((ret = reg_array_field_write(unit, MAPLE_MIR_SAMPLE_RATE_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id, MAPLE_SAMPLE_RATEf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }
    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_mirror_sflowMirrorSampleRate_set */

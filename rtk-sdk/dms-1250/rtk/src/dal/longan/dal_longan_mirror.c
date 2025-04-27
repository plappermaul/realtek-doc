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
#include <hal/chipdef/longan/rtk_longan_table_struct.h>
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/longan/dal_longan_mirror.h>
#include <dal/longan/dal_longan_stack.h>
#include <rtk/default.h>
#include <rtk/mirror.h>


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
        result = RTK_PORTMASK_IS_PORT_SET(portmask, port);  \
    } while(0)

#define WAIT_LOCAL_MIRROR_TRUNK_REFRESH_DONE(val) \
do{ \
    if ((ret = reg_field_read(unit, \
                          LONGAN_TRK_LOCAL_TBL_REFRESHr, \
                          LONGAN_TRK_LOCAL_TBL_REFRESHf, \
                          &val)) != RT_ERR_OK) \
    { \
        MIRROR_SEM_UNLOCK(unit); \
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), ""); \
        return ret; \
    } \
} while(0 != val)

#define LOCAL_MIRROR_TRUNK_REFRESH() \
do{ \
    uint32 doRefresh=1; \
    if ((ret = reg_field_write(unit, \
                          LONGAN_TRK_LOCAL_TBL_REFRESHr, \
                          LONGAN_TRK_LOCAL_TBL_REFRESHf, \
                          &doRefresh)) != RT_ERR_OK) \
    { \
        MIRROR_SEM_UNLOCK(unit); \
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), ""); \
        return ret; \
    } \
} while(0)


/*
 * Function Declaration
 */

/* Function Name:
 *      dal_longan_mirrorMapper_init
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
dal_longan_mirrorMapper_init(dal_mapper_t *pMapper)
{
    pMapper->mirror_init = dal_longan_mirror_init;
    pMapper->mirror_group_init = dal_longan_mirror_group_init;
    pMapper->mirror_group_get = dal_longan_mirror_group_get;
    pMapper->mirror_group_set = dal_longan_mirror_group_set;
    pMapper->mirror_rspanEgrMode_get = dal_longan_mirror_rspanEgrMode_get;
    pMapper->mirror_rspanEgrMode_set = dal_longan_mirror_rspanEgrMode_set;
    pMapper->mirror_rspanTag_get = dal_longan_mirror_rspanTag_get;
    pMapper->mirror_rspanTag_set = dal_longan_mirror_rspanTag_set;
    pMapper->mirror_sflowMirrorSampleRate_get = dal_longan_mirror_sflowMirrorSampleRate_get;
    pMapper->mirror_sflowMirrorSampleRate_set = dal_longan_mirror_sflowMirrorSampleRate_set;
    pMapper->mirror_egrQueue_get = dal_longan_mirror_egrQueue_get;
    pMapper->mirror_egrQueue_set = dal_longan_mirror_egrQueue_set;

    pMapper->mirror_sflowPortIgrSampleRate_get = dal_longan_mirror_sflowPortIgrSampleRate_get;
    pMapper->mirror_sflowPortIgrSampleRate_set = dal_longan_mirror_sflowPortIgrSampleRate_set;
    pMapper->mirror_sflowPortEgrSampleRate_get = dal_longan_mirror_sflowPortEgrSampleRate_get;
    pMapper->mirror_sflowPortEgrSampleRate_set = dal_longan_mirror_sflowPortEgrSampleRate_set;
    pMapper->mirror_sflowSampleCtrl_get = dal_longan_mirror_sflowSampleCtrl_get;
    pMapper->mirror_sflowSampleCtrl_set = dal_longan_mirror_sflowSampleCtrl_set;
    pMapper->mirror_sflowSampleTarget_get = dal_longan_mirror_sflowSampleTarget_get;
    pMapper->mirror_sflowSampleTarget_set = dal_longan_mirror_sflowSampleTarget_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_mirror_init
 * Description:
 *      Initialize the mirroring database.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must initialize Mirror module before calling any Mirror APIs.
 */
int32
dal_longan_mirror_init(uint32 unit)
{
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

    return RT_ERR_OK;
} /* end of dal_longan_mirror_init */


/* Function Name:
 *      dal_longan_mirror_group_init
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
dal_longan_mirror_group_init(
    uint32              unit,
    rtk_mirror_entry_t  *pMirrorEntry)
{
    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMirrorEntry), RT_ERR_NULL_POINTER);

    /* Disable all mirroring entries */
    pMirrorEntry->mirror_type = DISABLED;

    return RT_ERR_OK;
} /* end of dal_longan_mirror_group_init */

/* Function Name:
 *      dal_longan_mirror_group_get
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
dal_longan_mirror_group_get(
    uint32              unit,
    uint32              mirror_id,
    rtk_mirror_entry_t  *pMirrorEntry)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, pMirrorEntry=%x"
            , unit, pMirrorEntry);

    /* parameter check */
    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);
    RT_PARAM_CHK((NULL == pMirrorEntry), RT_ERR_NULL_POINTER);

    MIRROR_SEM_LOCK(unit);

    /* Mirror Type */
    if ((ret = reg_array_field_read(unit, LONGAN_MIR_CTRLr,  REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_MIR_TYPEf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if(0x0 == value)
        pMirrorEntry->mirror_type = DISABLE_MIRROR;
    else if(0x1 == value)
        pMirrorEntry->mirror_type = PORT_BASED_MIRROR;
    else if(0x2 == value)
        pMirrorEntry->mirror_type = RSPAN_BASED_MIRROR;
    else
        pMirrorEntry->mirror_type = FLOW_BASED_MIRROR;

    /* MTP Type */
    if ((ret = reg_array_field_read(unit, LONGAN_MIR_CTRLr,  REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_MTP_IS_TRKf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if(0x0 == value)
        pMirrorEntry->mtp_type = MTP_TYPE_NOT_TRK;
    else
        pMirrorEntry->mtp_type = MTP_TYPE_IS_TRK;

    /* MTP Port */
    if ((ret = reg_array_field_read(unit, LONGAN_MIR_CTRLr,  REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_MTP_PORTf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if (MTP_TYPE_NOT_TRK == pMirrorEntry->mtp_type)
    {
        pMirrorEntry->mirroring_devID = ((rtk_port_t)value >> 6) & 0xF;
        pMirrorEntry->mirroring_port = (rtk_port_t)value & 0x3F;
    }
    else
    {
        pMirrorEntry->mirroring_port = value;
    }

    /* SPM Portmask */
    if ((ret = reg_array_field_read(unit, LONGAN_MIR_SPM_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_SPM_28_0f, &pMirrorEntry->mirrored_igrPorts.bits[0])) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }


    /* DPM Portmask */
    if ((ret = reg_array_field_read(unit, LONGAN_MIR_DPM_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_DPM_28_0f, &pMirrorEntry->mirrored_egrPorts.bits[0])) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    /* Mirror Operation */
    if ((ret = reg_array_field_read(unit, LONGAN_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_MIR_OPf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if(0x0 == value)
        pMirrorEntry->oper_of_igr_and_egr_ports = MIRROR_OP_OR;
    else
        pMirrorEntry->oper_of_igr_and_egr_ports = MIRROR_OP_AND;

    /* Mirror Self Filter */
    if ((ret = reg_array_field_read(unit, LONGAN_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_MTP_SELF_FLTERf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if(0x0 == value)
        pMirrorEntry->self_flter = DISABLED;
    else
        pMirrorEntry->self_flter = ENABLED;


    /* Mirror Original or Modified */
    if ((ret = reg_array_field_read(unit, LONGAN_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_MIR_RX_TX_SELf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if(0x0 == value)
        pMirrorEntry->mirror_orginalPkt = DISABLED;
    else
        pMirrorEntry->mirror_orginalPkt = ENABLED;

    /* Mirror VLAN Mode */
    if ((ret = reg_array_field_read(unit, LONGAN_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_MIR_MODEf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }
    if(0x0 == value)
        pMirrorEntry->mir_mode = MIRROR_VLAN_MODEL_NORMAL;
    else
        pMirrorEntry->mir_mode = MIRROR_VLAN_MODEL_SPECIAL;


    /* Mirror TX Isolation */
    if ((ret = reg_array_field_read(unit, LONGAN_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_MTP_TX_ISOf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if(0x0 == value)
        pMirrorEntry->mirroring_port_egrMode = FORWARD_ALL_PKTS;
    else
        pMirrorEntry->mirroring_port_egrMode = FORWARD_MIRRORED_PKTS_ONLY;


    /* Mirror Traffic Selection */
    if ((ret = reg_array_field_read(unit, LONGAN_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_TRAFFIC_SELf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if(0x0 == value)
        pMirrorEntry->duplicate_fltr = DISABLED;
    else
        pMirrorEntry->duplicate_fltr = ENABLED;

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_mirror_group_get */

/* Function Name:
 *      dal_longan_mirror_group_set
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
dal_longan_mirror_group_set(
    uint32              unit,
    uint32              mirror_id,
    rtk_mirror_entry_t  *pMirrorEntry)
{
    uint32  value;
    uint32  val;
    int32   ret = RT_ERR_FAILED;
    uint32 myUnit = 0;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, pMirrorEntry=%x"
            , unit, pMirrorEntry);

    /* parameter check */
    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);
    RT_PARAM_CHK((NULL == pMirrorEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pMirrorEntry->mirror_type >= MIRROR_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(pMirrorEntry->mtp_type >= MTP_TYPE_END, RT_ERR_INPUT);

    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &(pMirrorEntry->mirrored_igrPorts)), RT_ERR_PORT_MASK);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &(pMirrorEntry->mirrored_egrPorts)), RT_ERR_PORT_MASK);

    if(pMirrorEntry->mtp_type == MTP_TYPE_NOT_TRK)
    {
        RT_PARAM_CHK(pMirrorEntry->mirroring_devID >= RTK_MAX_NUM_OF_UNIT, RT_ERR_UNIT_ID);
        RT_PARAM_CHK(pMirrorEntry->mirroring_port >= RTK_MAX_PORT_PER_UNIT, RT_ERR_PORT_ID);

        /*Check the Mirroring port is included in Igr/Egr portmask setting or NOT*/
        if ((ret = dal_longan_stack_devId_get(unit, &myUnit)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
            return ret;
        }

        if(myUnit == pMirrorEntry->mirroring_devID)
        {
            MIRROT_PORTMASK_MEMBER_PORT_CHECK(pMirrorEntry->mirrored_egrPorts,pMirrorEntry->mirroring_port,ret);
            RT_PARAM_CHK(ret, RT_ERR_MIRROR_DP_IN_SPM_DPM);
            MIRROT_PORTMASK_MEMBER_PORT_CHECK(pMirrorEntry->mirrored_igrPorts,pMirrorEntry->mirroring_port,ret);
            RT_PARAM_CHK(ret, RT_ERR_MIRROR_DP_IN_SPM_DPM);
        }
    }
    else
    {
        RT_PARAM_CHK(pMirrorEntry->mirroring_port >= HAL_MAX_NUM_OF_TRUNK(unit), RT_ERR_LA_STACK_TRUNK_ID);
    }

    RT_PARAM_CHK(pMirrorEntry->oper_of_igr_and_egr_ports >= MIRROR_OP_END, RT_ERR_INPUT);
    RT_PARAM_CHK(pMirrorEntry->self_flter >= RTK_ENABLE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(pMirrorEntry->mirror_orginalPkt >= RTK_ENABLE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(pMirrorEntry->mir_mode >= MIRROR_VLAN_MODEL_END, RT_ERR_INPUT);
    RT_PARAM_CHK(pMirrorEntry->mirroring_port_egrMode >= MIRROR_EGR_MODE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(pMirrorEntry->duplicate_fltr >= RTK_ENABLE_END, RT_ERR_INPUT);

    MIRROR_SEM_LOCK(unit);

    /* Mirror Type */
    if(pMirrorEntry->mirror_type == DISABLE_MIRROR)
        value = 0x0;
    else if(pMirrorEntry->mirror_type == PORT_BASED_MIRROR)
        value = 0x1;
    else if(pMirrorEntry->mirror_type == RSPAN_BASED_MIRROR)
        value = 0x2;
    else
        value = 0x3;

    if ((ret = reg_array_field_write(unit, LONGAN_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_MIR_TYPEf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    /* MTP Type */
    if(pMirrorEntry->mtp_type == MTP_TYPE_IS_TRK)
    {
        value = 0x1;
        if ((ret = reg_array_field_write(unit, LONGAN_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_MTP_IS_TRKf, &value)) != RT_ERR_OK)
        {
            MIRROR_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
            return ret;
        }

        /* MTP port means TRUNK ID */
        value = (uint32)pMirrorEntry->mirroring_port;
        if ((ret = reg_array_field_write(unit, LONGAN_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                    LONGAN_MTP_PORTf, &value)) != RT_ERR_OK)
        {
            MIRROR_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
            return ret;
        }
    }
    else
    {
        value = 0x0;
        if ((ret = reg_array_field_write(unit, LONGAN_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_MTP_IS_TRKf, &value)) != RT_ERR_OK)
        {
            MIRROR_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
            return ret;
        }

        /* MTP port means DEV+PORT */
        value = ((uint32)pMirrorEntry->mirroring_devID<<6) + (uint32)pMirrorEntry->mirroring_port;
        if ((ret = reg_array_field_write(unit, LONGAN_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                    LONGAN_MTP_PORTf, &value)) != RT_ERR_OK)
        {
            MIRROR_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
            return ret;
        }
    }

    /* SPM Portmask */
    value = pMirrorEntry->mirrored_igrPorts.bits[0];
    if ((ret = reg_array_field_write(unit, LONGAN_MIR_SPM_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_SPM_28_0f, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    /* DPM Portmask */
    value = pMirrorEntry->mirrored_egrPorts.bits[0];
    if ((ret = reg_array_field_write(unit, LONGAN_MIR_DPM_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_DPM_28_0f, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    /* Mirror Operation */
    if(pMirrorEntry->oper_of_igr_and_egr_ports == MIRROR_OP_OR)
         value = 0x0;
    else
         value = 0x1;

    if ((ret = reg_array_field_write(unit, LONGAN_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_MIR_OPf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    /* Mirror Self Filter */
    if(pMirrorEntry->self_flter == DISABLED)
         value = 0x0;
    else
         value = 0x1;

    if ((ret = reg_array_field_write(unit, LONGAN_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_MTP_SELF_FLTERf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    /* Mirror Original or Modified */
    if(pMirrorEntry->mirror_orginalPkt == DISABLED)
         value = 0x0;
    else
         value = 0x1;
    if ((ret = reg_array_field_write(unit, LONGAN_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_MIR_RX_TX_SELf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    /* Mirror VLAN Mode */
    if(pMirrorEntry->mir_mode == MIRROR_VLAN_MODEL_NORMAL)
         value = 0x0;
    else
         value = 0x1;
    if ((ret = reg_array_field_write(unit, LONGAN_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_MIR_MODEf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }
    /* Mirror TX Isolation */
    if(pMirrorEntry->mirroring_port_egrMode == FORWARD_ALL_PKTS)
         value = 0x0;
    else
         value = 0x1;

    if ((ret = reg_array_field_write(unit, LONGAN_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_MTP_TX_ISOf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    /* Mirror Traffic Selection */
    if(pMirrorEntry->duplicate_fltr == DISABLED)
         value = 0x0;
    else
         value = 0x1;

    if ((ret = reg_array_field_write(unit, LONGAN_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                LONGAN_TRAFFIC_SELf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    /* Local refresh */
    LOCAL_MIRROR_TRUNK_REFRESH() ;

    WAIT_LOCAL_MIRROR_TRUNK_REFRESH_DONE(val) ;

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_mirror_group_set */


/* Module Name    : Mirror */
/* Sub-module Name: RSPAN  */

/* Function Name:
 *      dal_longan_mirror_rspanEgrMode_get
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
dal_longan_mirror_rspanEgrMode_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanEgrMode_t *pEgrMode)
{
    int32   ret = RT_ERR_FAILED;
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
    if ((ret = reg_array_field_read(unit, LONGAN_MIR_RSPAN_TX_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, LONGAN_RSPAN_TX_TAG_ADDf, &addTag)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "ret=%x", ret);
        return ret;
    }

    if ((ret = reg_array_field_read(unit, LONGAN_MIR_RSPAN_RX_TAG_RM_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, LONGAN_RSPAN_RX_TAG_RMf, &removeTag)) != RT_ERR_OK)
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
} /* end of dal_longan_mirror_rspanEgrMode_get */

/* Function Name:
 *      dal_longan_mirror_rspanEgrMode_set
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
dal_longan_mirror_rspanEgrMode_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanEgrMode_t egrMode)
{
    int32   ret = RT_ERR_FAILED;
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
    if ((ret = reg_array_field_write(unit, LONGAN_MIR_RSPAN_TX_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, LONGAN_RSPAN_TX_TAG_ADDf, &addTag)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "ret=%x", ret);
        return ret;
    }

    if ((ret = reg_array_field_write(unit, LONGAN_MIR_RSPAN_RX_TAG_RM_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, LONGAN_RSPAN_RX_TAG_RMf, &removeTag)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "ret=%x", ret);
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_mirror_rspanEgrMode_set */

/* Function Name:
 *      dal_longan_mirror_rspanTag_get
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
dal_longan_mirror_rspanTag_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanTag_t *pTag)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, mirror_id=%d"
            , unit, mirror_id);

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTag), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);

    MIRROR_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, LONGAN_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, LONGAN_RSPAN_TAG_TPIDf, &(pTag->tpid))) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, LONGAN_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, LONGAN_RSPAN_TAG_PRIf, &(pTag->pri))) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, LONGAN_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, LONGAN_RSPAN_TAG_CFIf, &(pTag->cfi))) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, LONGAN_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, LONGAN_RSPAN_TAG_VIDf, &(pTag->vid))) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_mirror_rspanTag_get */

/* Function Name:
 *      dal_longan_mirror_rspanTag_set
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
dal_longan_mirror_rspanTag_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanTag_t *pTag)
{
    uint32  val;
    int32   ret = RT_ERR_FAILED;

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
    if ((ret = reg_array_field_write(unit, LONGAN_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, LONGAN_RSPAN_TAG_TPIDf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, LONGAN_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, LONGAN_RSPAN_TAG_TPIDf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pTag->pri;
    if ((ret = reg_array_field_write(unit, LONGAN_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, LONGAN_RSPAN_TAG_PRIf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pTag->cfi;
    if ((ret = reg_array_field_write(unit, LONGAN_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, LONGAN_RSPAN_TAG_CFIf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pTag->vid;
    if ((ret = reg_array_field_write(unit, LONGAN_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, LONGAN_RSPAN_TAG_VIDf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if((ret = reg_array_field_write(unit, LONGAN_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, LONGAN_RSPAN_TAG_VIDf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_mirror_rspanTag_set */


/* Module Name    : Mirror             */
/* Sub-module Name: Mirror-based SFLOW */

/* Function Name:
 *      dal_longan_mirror_sflowMirrorSampleRate_get
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
dal_longan_mirror_sflowMirrorSampleRate_get(uint32 unit, uint32 mirror_id, uint32 *pRate)
{
    int32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, mirror_id=%d"
            , unit, mirror_id);

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    MIRROR_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, LONGAN_MIR_SAMPLE_RATE_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, LONGAN_SAMPLE_RATEf, pRate)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }
    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_mirror_sflowMirrorSampleRate_get */

/* Function Name:
 *      dal_longan_mirror_sflowMirrorSampleRate_set
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
dal_longan_mirror_sflowMirrorSampleRate_set(uint32 unit, uint32 mirror_id, uint32 rate)
{
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, mirror_id=%d, rate=%u"
            , unit, mirror_id, rate);

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);
    RT_PARAM_CHK(rate > HAL_SFLOW_RATE_MAX(unit), RT_ERR_OUT_OF_RANGE);

    MIRROR_SEM_LOCK(unit);
    val = rate;
    if ((ret = reg_array_field_write(unit, LONGAN_MIR_SAMPLE_RATE_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id, LONGAN_SAMPLE_RATEf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }
    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_mirror_sflowMirrorSampleRate_set */



/*
 * Function Declaration
 *      dal_longan_mirror_egrQueue_get
 * Description:
 *      Get enable status and output queue ID of mirror packet.
 * Input:
 *      unit      - unit id
 * Output:
 *      pEnable    - pointer to mirror packet dedicated output queue ID enable status
 *      pQid          - pointer to mirror packet output queue ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_mirror_egrQueue_get(uint32 unit, rtk_enable_t *pEnable, rtk_qid_t *pQid)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pQid), RT_ERR_NULL_POINTER);

    MIRROR_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit,
                          LONGAN_MIR_QID_CTRLr,
                          LONGAN_MIR_QID_ENf,
                          pEnable)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_field_read(unit,
                          LONGAN_MIR_QID_CTRLr,
                          LONGAN_MIR_QIDf,
                          pQid)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_longan_mirror_egrQueue_set
 * Description:
 *      Set enable status and output queue ID of mirror packet.
 * Input:
 *      unit      - unit id
 *      enable   - mirror packet dedicated output queue ID enable status
 *      qid        - mirror packet output queue ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_QUEUE_ID     - Invalid queue ID
 * Note:
 *      (1) mirror packet would follow this queue configuration if the function enable status is enabled
 */
int32
dal_longan_mirror_egrQueue_set(uint32 unit, rtk_enable_t enable, rtk_qid_t qid)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, enable=%d, qid=%u"
            , unit, enable, qid);

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((qid >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);

    MIRROR_SEM_LOCK(unit);

    val = enable;

    if ((ret = reg_field_write(unit,
                          LONGAN_MIR_QID_CTRLr,
                          LONGAN_MIR_QID_ENf,
                          &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = qid;
    if ((ret = reg_field_write(unit,
                          LONGAN_MIR_QID_CTRLr,
                          LONGAN_MIR_QIDf,
                          &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Sub-module Name: Port-based SFLOW */

/* Function Name:
 *      dal_longan_mirror_sflowSampleCtrl_get
 * Description:
 *      Get sampling control status for simple hit ingress and egress
 *      which direction take hit at first.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pCtrl - pointer to sampling preference
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_mirror_sflowSampleCtrl_get(uint32 unit, rtk_sflowSampleCtrl_t *pCtrl)
{
    int32   ret;
    int32   val;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d", unit);

    RT_PARAM_CHK((NULL == pCtrl), RT_ERR_NULL_POINTER);

    MIRROR_SEM_LOCK(unit);
    ret = reg_field_read(unit, LONGAN_SFLOW_CTRLr, LONGAN_SMPL_SELf, (uint32 *)&val);
    if (RT_ERR_OK != ret)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if (0 == val)
        *pCtrl = SFLOW_CTRL_INGRESS;
    else if(1 == val)
        *pCtrl = SFLOW_CTRL_EGRESS;

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}    /* end of dal_longan_mirror_sflowSampleCtrl_get */

/* Function Name:
 *      dal_longan_mirror_sflowSampleCtrl_set
 * Description:
 *      Set sampling control status for simple hit ingress and egress
 *      which direction take hit at first.
 * Input:
 *      unit - unit id
 *      rate - status of sampling preference
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_mirror_sflowSampleCtrl_set(uint32 unit, rtk_sflowSampleCtrl_t ctrl)
{
    int32   ret;
    int32   val;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d ctrl=%d", unit, ctrl);

    RT_PARAM_CHK((SFLOW_CTRL_END <= ctrl), RT_ERR_INPUT);

    MIRROR_SEM_LOCK(unit);

    if (SFLOW_CTRL_INGRESS == ctrl)
        val = 0;
    else if(SFLOW_CTRL_EGRESS == ctrl)
        val = 1;

    ret = reg_field_write(unit, LONGAN_SFLOW_CTRLr, LONGAN_SMPL_SELf, (uint32 *)&val);
    if (RT_ERR_OK != ret)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }
    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}    /* end of dal_longan_mirror_sflowSampleCtrl_set */

/* Function Name:
 *      dal_longan_mirror_sflowPortIgrSampleRate_get
 * Description:
 *      Get sampling rate of ingress sampling on specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pRate - pointer to sampling rate of ingress sampling
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_mirror_sflowPortIgrSampleRate_get(uint32 unit, rtk_port_t port, uint32 *pRate)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, port=%d",unit, port);

    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    MIRROR_SEM_LOCK(unit);
    ret = reg_array_field_read(unit, LONGAN_SFLOW_PORT_RATE_CTRLr,
                               port, REG_ARRAY_INDEX_NONE,
                               LONGAN_IGR_RATEf, pRate);
    if (RT_ERR_OK != ret)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_mirror_sflowPortIgrSampleRate_get */

/* Function Name:
 *      dal_longan_mirror_sflowPortIgrSampleRate_set
 * Description:
 *      Set sampling rate of ingress sampling on specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      rate - sampling rate of ingress sampling
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 * Note:
 *      None
 */
int32
dal_longan_mirror_sflowPortIgrSampleRate_set(uint32 unit, rtk_port_t port, uint32 rate)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, port=%d, rate=%u",
            unit, port, rate);

    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(rate > HAL_SFLOW_RATE_MAX(unit), RT_ERR_OUT_OF_RANGE);

    MIRROR_SEM_LOCK(unit);

    /* set value to CHIP */
    ret = reg_array_field_write(unit, LONGAN_SFLOW_PORT_RATE_CTRLr,
                                port, REG_ARRAY_INDEX_NONE,
                                LONGAN_IGR_RATEf, &rate);
    if (RT_ERR_OK != ret)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_mirror_sflowPortIgrSampleRate_set */

/* Function Name:
 *      dal_longan_mirror_sflowPortEgrSampleRate_get
 * Description:
 *      Get sampling rate of egress sampling on specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pRate - pointer to sampling rate of egress sampling
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_mirror_sflowPortEgrSampleRate_get(uint32 unit, rtk_port_t port,
                                              uint32 *pRate)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, port=%d",unit, port);

    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    MIRROR_SEM_LOCK(unit);
    ret = reg_array_field_read(unit, LONGAN_SFLOW_PORT_RATE_CTRLr,
                               port, REG_ARRAY_INDEX_NONE,
                               LONGAN_EGR_RATEf, pRate);
    if (RT_ERR_OK != ret)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_mirror_sflowPortEgrSampleRate_get */

/* Function Name:
 *      dal_longan_mirror_sflowPortEgrSampleRate_set
 * Description:
 *      Set sampling rate of egress sampling on specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      rate - sampling rate of egress sampling
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 * Note:
 *      None
 */
int32
dal_longan_mirror_sflowPortEgrSampleRate_set(uint32 unit, rtk_port_t port,
                                              uint32 rate)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, port=%d, rate=%u",
            unit, port, rate);

    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(rate > HAL_SFLOW_RATE_MAX(unit), RT_ERR_OUT_OF_RANGE);

    MIRROR_SEM_LOCK(unit);

    /* set value to CHIP */
    ret = reg_array_field_write(unit, LONGAN_SFLOW_PORT_RATE_CTRLr,
                                port, REG_ARRAY_INDEX_NONE,
                                LONGAN_EGR_RATEf, &rate);
    if (RT_ERR_OK != ret)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_mirror_sflowPortEgrSampleRate_set */

/* Function Name:
 *      dal_longan_mirror_sflowSampleTarget_get
 * Description:
 *      Get information of sFlow sample packet to local or master CPU.
 * Input:
 *      unit    - unit id
 * Output:
 *      pTarget - pointer to the information of sFlow sample packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
int32
dal_longan_mirror_sflowSampleTarget_get(uint32 unit, rtk_sflow_sampleTarget_t *pTarget)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d",unit);

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTarget), RT_ERR_NULL_POINTER);

    /* function body */
    MIRROR_SEM_LOCK(unit);

    ret = reg_field_read(unit, LONGAN_SFLOW_CTRLr, LONGAN_CPU_SELf, &val);
    if (ret != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MPLS), "field read fail");
        return ret;
    }

    if (0 == val)
    {
        *pTarget = RTK_SFLOW_SAMPLE_LOCAL;
    }
    else
    {
        *pTarget = RTK_SFLOW_SAMPLE_MASTER;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_mirror_sflowSampleTarget_get */

/* Function Name:
 *      dal_longan_mirror_sflowSampleTarget_set
 * Description:
 *      Set information of MPLS trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 *      target  - the status of MPLS trap packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 */
int32
dal_longan_mirror_sflowSampleTarget_set(uint32 unit, rtk_sflow_sampleTarget_t target)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d,target=%d",unit, target);

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_SFLOW_SAMPLE_END <= target), RT_ERR_INPUT);

    /* function body */
    MIRROR_SEM_LOCK(unit);

    switch (target)
    {
        case RTK_SFLOW_SAMPLE_LOCAL:
            val = 0;
            break;
        case RTK_SFLOW_SAMPLE_MASTER:
            val = 1;
            break;
        default:
            MIRROR_SEM_UNLOCK(unit);
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_MPLS), "target %d is invalid.", target);
            return RT_ERR_INPUT;
    }

    ret = reg_field_write(unit, LONGAN_SFLOW_CTRLr, LONGAN_CPU_SELf, &val);
    if (ret != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MPLS), "field write fail");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_mirror_sflowSampleTarget_set */



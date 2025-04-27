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
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/cypress/dal_cypress_mirror.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/mirror.h>


/*
 * Symbol Definition
 */
typedef struct dal_cypress_mirror_info_s {
    uint8   sflowMirrorSample_enable[4];
    uint32  sflowMirrorSample_rate[4];
    uint8   sflowPortIgrSample_enable[RTK_MAX_NUM_OF_PORTS];
    uint32  sflowPortIgrSample_rate[RTK_MAX_NUM_OF_PORTS];
    uint8   sflowPortEgrSample_enable[RTK_MAX_NUM_OF_PORTS];
    uint32  sflowPortEgrSample_rate[RTK_MAX_NUM_OF_PORTS];
} dal_cypress_mirror_info_t;

/*
 * Data Declaration
 */
static uint32               mirror_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         mirror_sem[RTK_MAX_NUM_OF_UNIT];
static dal_cypress_mirror_info_t   *pMirror_info[RTK_MAX_NUM_OF_UNIT];

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


/*
 * Function Declaration
 */

/* Module Name : Mirror */

/* Function Name:
 *      dal_cypress_mirrorMapper_init
 * Description:
 *      Hook mirror module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook mirror module before calling any mirror APIs.
 */
int32
dal_cypress_mirrorMapper_init(dal_mapper_t *pMapper)
{
    pMapper->mirror_init = dal_cypress_mirror_init;
    pMapper->mirror_group_init = dal_cypress_mirror_group_init;
    pMapper->mirror_group_get = dal_cypress_mirror_group_get;
    pMapper->mirror_group_set = dal_cypress_mirror_group_set;
    pMapper->mirror_rspanIgrMode_get = dal_cypress_mirror_rspanIgrMode_get;
    pMapper->mirror_rspanIgrMode_set = dal_cypress_mirror_rspanIgrMode_set;
    pMapper->mirror_rspanEgrMode_get = dal_cypress_mirror_rspanEgrMode_get;
    pMapper->mirror_rspanEgrMode_set = dal_cypress_mirror_rspanEgrMode_set;
    pMapper->mirror_rspanTag_get = dal_cypress_mirror_rspanTag_get;
    pMapper->mirror_rspanTag_set = dal_cypress_mirror_rspanTag_set;
    pMapper->mirror_sflowMirrorSampleRate_get = dal_cypress_mirror_sflowMirrorSampleRate_get;
    pMapper->mirror_sflowMirrorSampleRate_set = dal_cypress_mirror_sflowMirrorSampleRate_set;
    pMapper->mirror_sflowPortIgrSampleRate_get = dal_cypress_mirror_sflowPortIgrSampleRate_get;
    pMapper->mirror_sflowPortIgrSampleRate_set = dal_cypress_mirror_sflowPortIgrSampleRate_set;
    pMapper->mirror_sflowPortEgrSampleRate_get = dal_cypress_mirror_sflowPortEgrSampleRate_get;
    pMapper->mirror_sflowPortEgrSampleRate_set = dal_cypress_mirror_sflowPortEgrSampleRate_set;
    pMapper->mirror_sflowSampleCtrl_get = dal_cypress_mirror_sflowSampleCtrl_get;
    pMapper->mirror_sflowSampleCtrl_set = dal_cypress_mirror_sflowSampleCtrl_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_mirror_init
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
dal_cypress_mirror_init(uint32 unit)
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

    pMirror_info[unit] = (dal_cypress_mirror_info_t *)osal_alloc(sizeof(dal_cypress_mirror_info_t));
    if (NULL == pMirror_info[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_MIRROR), "memory allocate failed");
        return RT_ERR_FAILED;
    }
    osal_memset(pMirror_info[unit], 0, sizeof(dal_cypress_mirror_info_t));

    mirror_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_cypress_mirror_init */


/* Module Name    : Mirror            */
/* Sub-module Name: Port-based mirror */

/* Module Name    : Mirror             */
/* Sub-module Name: Group-based mirror */

/* Function Name:
 *      dal_cypress_mirror_group_init
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_mirror_group_init(
    uint32              unit,
    rtk_mirror_entry_t  *pMirrorEntry)
{

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_PARAM_CHK((NULL == pMirrorEntry), RT_ERR_NULL_POINTER);

    pMirrorEntry->mirror_enable = DISABLED;
    pMirrorEntry->oper_of_igr_and_egr_ports = 0;
    pMirrorEntry->mirror_orginalPkt = ENABLED;
    pMirrorEntry->mirroring_port_egrMode = FORWARD_ALL_PKTS;

    return RT_ERR_OK;
} /* end of dal_cypress_mirror_group_init */

/* Function Name:
 *      dal_cypress_mirror_group_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_MIRROR_ID        - invalid mirror id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_mirror_group_get(
    uint32              unit,
    uint32              mirror_id,
    rtk_mirror_entry_t  *pMirrorEntry)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_PARAM_CHK((NULL == pMirrorEntry), RT_ERR_NULL_POINTER);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, pMirrorEntry=%x"
            , unit, pMirrorEntry);

    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);

    MIRROR_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_MIR_CTRLr,  REG_ARRAY_INDEX_NONE, mirror_id,
                CYPRESS_MIR_ENf, &pMirrorEntry->mirror_enable)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_MIR_CTRLr,  REG_ARRAY_INDEX_NONE, mirror_id,
                CYPRESS_MIR_DST_Pf, &pMirrorEntry->mirroring_port)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_MIR_SPM_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                CYPRESS_SPM_0f, &pMirrorEntry->mirrored_igrPorts.bits[0])) != RT_ERR_OK)    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_MIR_SPM_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                CYPRESS_SPM_1f, &pMirrorEntry->mirrored_igrPorts.bits[1])) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_MIR_DPM_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                CYPRESS_DPM_0f, &pMirrorEntry->mirrored_egrPorts.bits[0])) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_MIR_DPM_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                CYPRESS_DPM_1f, &pMirrorEntry->mirrored_egrPorts.bits[1])) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                CYPRESS_MIR_OPf, &pMirrorEntry->oper_of_igr_and_egr_ports)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                CYPRESS_MIR_SELf, &pMirrorEntry->mirror_orginalPkt)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                CYPRESS_DST_P_ISOf, &pMirrorEntry->mirroring_port_egrMode)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_mirror_group_get */

/* Function Name:
 *      dal_cypress_mirror_group_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_MIRROR_ID        - invalid mirror id
 *      RT_ERR_PORT_ID          - invalid mirroring port id
 *      RT_ERR_INPUT                - invalid input parameter
 *      RT_ERR_MIRROR_DP_IN_SPM_DPM        - mirroring port can not be in ingress or egress mirrored portmask of any mirroring set
 * Note:
 *      None
 */
int32
dal_cypress_mirror_group_set(
    uint32              unit,
    uint32              mirror_id,
    rtk_mirror_entry_t  *pMirrorEntry)
{
    uint32  val;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_PARAM_CHK((NULL == pMirrorEntry), RT_ERR_NULL_POINTER);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, pMirrorEntry=%x"
            , unit, pMirrorEntry);

    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, pMirrorEntry->mirroring_port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(pMirrorEntry->oper_of_igr_and_egr_ports >= MIRROR_OP_END, RT_ERR_INPUT);
    RT_PARAM_CHK(pMirrorEntry->mirror_orginalPkt >= RTK_ENABLE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(pMirrorEntry->mirror_enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    /*Check the Mirroring port is included in Igr/Egr portmask setting or NOT*/
    MIRROT_PORTMASK_MEMBER_PORT_CHECK(pMirrorEntry->mirrored_egrPorts,pMirrorEntry->mirroring_port,ret);
    RT_PARAM_CHK(ret, RT_ERR_MIRROR_DP_IN_SPM_DPM);
    MIRROT_PORTMASK_MEMBER_PORT_CHECK(pMirrorEntry->mirrored_igrPorts,pMirrorEntry->mirroring_port,ret);
    RT_PARAM_CHK(ret, RT_ERR_MIRROR_DP_IN_SPM_DPM);

    MIRROR_SEM_LOCK(unit);

    /* set value to CHIP*/
    val = pMirrorEntry->mirror_enable;
    if ((ret = reg_array_field_write(unit, CYPRESS_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                CYPRESS_MIR_ENf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->mirroring_port;
    if ((ret = reg_array_field_write(unit, CYPRESS_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                CYPRESS_MIR_DST_Pf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->mirrored_igrPorts.bits[0];
    if ((ret = reg_array_field_write(unit, CYPRESS_MIR_SPM_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                CYPRESS_SPM_0f, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->mirrored_igrPorts.bits[1];
    if ((ret = reg_array_field_write(unit, CYPRESS_MIR_SPM_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                CYPRESS_SPM_1f, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->mirrored_egrPorts.bits[0];
    if ((ret = reg_array_field_write(unit, CYPRESS_MIR_DPM_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                CYPRESS_DPM_0f, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->mirrored_egrPorts.bits[1];
    if ((ret = reg_array_field_write(unit, CYPRESS_MIR_DPM_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                CYPRESS_DPM_1f, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->oper_of_igr_and_egr_ports;
    if ((ret = reg_array_field_write(unit, CYPRESS_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                CYPRESS_MIR_OPf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->mirror_orginalPkt;
    if ((ret = reg_array_field_write(unit, CYPRESS_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                CYPRESS_MIR_SELf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pMirrorEntry->mirroring_port_egrMode;
    if ((ret = reg_array_field_write(unit, CYPRESS_MIR_CTRLr, REG_ARRAY_INDEX_NONE, mirror_id,
                CYPRESS_DST_P_ISOf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_mirror_group_set */



/* Module Name    : Mirror */
/* Sub-module Name: RSPAN  */

/* Function Name:
 *      dal_cypress_mirror_rspanIgrMode_get
 * Description:
 *      Get ingress mode of RSPAN on specified mirror group.
 * Input:
 *      unit        - unit id
 *      mirror_id   - mirror id
 * Output:
 *      pIgrMode - pointer to ingress mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_MIRROR_ID      - invalid mirror ID
 * Note:
 *      Ingress mode is as following:
 *      - RSPAN_IGR_HANDLE_RSPAN_TAG
 *      - RSPAN_IGR_IGNORE_RSPAN_TAG
 */
int32
dal_cypress_mirror_rspanIgrMode_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanIgrMode_t *pIgrMode)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_PARAM_CHK((NULL == pIgrMode), RT_ERR_NULL_POINTER);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, mirror id=%d, pIgrMode=%x"
            , unit, mirror_id, pIgrMode);

    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);

    MIRROR_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_MIR_RSPAN_RX_TAG_EN_CTRLr, REG_ARRAY_INDEX_NONE,
        mirror_id, CYPRESS_RSPAN_RX_TAG_ENf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
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
} /* end of dal_cypress_mirror_rspanIgrMode_get */

/* Function Name:
 *      dal_cypress_mirror_rspanIgrMode_set
 * Description:
 *      Set ingress mode of RSPAN on specified mirror group.
 * Input:
 *      unit    - unit id
 *      mirror_id    - mirror id
 *      igrMode - ingress mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_MIRROR_ID      - invalid mirror ID
 * Note:
 *      Ingress mode is as following:
 *      - RSPAN_IGR_HANDLE_RSPAN_TAG
 *      - RSPAN_IGR_IGNORE_RSPAN_TAG
 */
int32
dal_cypress_mirror_rspanIgrMode_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanIgrMode_t igrMode)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, mirror id=%d, igrMode=%u"
            , unit, mirror_id, igrMode);

    RT_PARAM_CHK(igrMode >= RSPAN_IGR_END, RT_ERR_INPUT);
    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);

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
    if ((ret = reg_array_field_write(unit, CYPRESS_MIR_RSPAN_RX_TAG_EN_CTRLr, REG_ARRAY_INDEX_NONE,
        mirror_id, CYPRESS_RSPAN_RX_TAG_ENf, &value)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_mirror_rspanIgrMode_set */

/* Function Name:
 *      dal_cypress_mirror_rspanEgrMode_get
 * Description:
 *      Get egress mode of RSPAN on specified mirror group.
 * Input:
 *      unit     - unit id
 *      mirror_id     - mirror id
 * Output:
 *      pEgrMode - pointer to egress mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_MIRROR_ID      - invalid mirror ID
 * Note:
 *      Ingress mode is as following:
 *      - RSPAN_EGR_REMOVE_TAG
 *      - RSPAN_EGR_ADD_TAG
 *      - RSPAN_EGR_NO_MODIFY
 */
int32
dal_cypress_mirror_rspanEgrMode_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanEgrMode_t *pEgrMode)
{
    int32   ret;
    uint32  addTag, removeTag;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_PARAM_CHK((NULL == pEgrMode), RT_ERR_NULL_POINTER);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, mirror_id=%d, pEgrMode=%x"
            , unit, mirror_id, pEgrMode);

    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);

    MIRROR_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_MIR_RSPAN_TX_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, CYPRESS_RSPAN_TX_TAG_ADDf, &addTag)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_MIR_RSPAN_RX_TAG_RM_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, CYPRESS_RSPAN_RX_TAG_RMf, &removeTag)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
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
} /* end of dal_cypress_mirror_rspanEgrMode_get */

/* Function Name:
 *      dal_cypress_mirror_rspanEgrMode_set
 * Description:
 *      Set egress mode of RSPAN on specified mirror group.
 * Input:
 *      unit    - unit id
 *      mirror_id    - mirror id
 *      egrMode - egress mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_MIRROR_ID      - invalid mirror ID
 * Note:
 *      Ingress mode is as following:
 *      - RSPAN_EGR_HANDLE_RSPAN_TAG
 *      - RSPAN_EGR_IGNORE_RSPAN_TAG
 */
int32
dal_cypress_mirror_rspanEgrMode_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanEgrMode_t egrMode)
{
    int32   ret;
    uint32  addTag, removeTag;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, mirror id=%d, egrMode=%u"
            , unit, mirror_id, egrMode);

    RT_PARAM_CHK(egrMode >= RSPAN_EGR_END, RT_ERR_INPUT);
    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);

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
    if ((ret = reg_array_field_write(unit, CYPRESS_MIR_RSPAN_TX_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, CYPRESS_RSPAN_TX_TAG_ADDf, &addTag)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, CYPRESS_MIR_RSPAN_RX_TAG_RM_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, CYPRESS_RSPAN_RX_TAG_RMf, &removeTag)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_mirror_rspanEgrMode_set */

/* Function Name:
 *      dal_cypress_mirror_rspanTag_get
 * Description:
 *      Get content of RSPAN tag on specified mirror group.
 * Input:
 *      unit    - unit id
 *      mirror_id    - mirror id
 * Output:
 *      pTag - pointer to content of RSPAN tag
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_MIRROR_ID      - invalid mirror ID
 * Note:
 *      None
 */
int32
dal_cypress_mirror_rspanTag_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanTag_t *pTag)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);
    RT_PARAM_CHK((NULL == pTag), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, mirror_id=%d, pTag=%x"
            , unit, mirror_id, pTag);

    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);

    MIRROR_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, CYPRESS_RSPAN_TAG_TPID_IDXf, &(pTag->tpidIdx))) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, CYPRESS_RSPAN_TAG_PRIf, &(pTag->pri))) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, CYPRESS_RSPAN_TAG_CFIf, &(pTag->cfi))) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, CYPRESS_RSPAN_TAG_VIDf, &(pTag->vid))) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_mirror_rspanTag_get */

/* Function Name:
 *      dal_cypress_mirror_rspanTag_set
 * Description:
 *      Set content of RSPAN tag on specified mirroring group.
 * Input:
 *      unit    - unit id
 *      mirror_id    - mirror id
 *      pTag - content of RSPAN tag
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT             - invalid input parameter
 *      RT_ERR_MIRROR_ID      - invalid mirror ID
 *      RT_ERR_PRIORITY         - invalid priority
 *      RT_ERR_VLAN_VID         - invalid vid
 * Note:
 *      None
 */
int32
dal_cypress_mirror_rspanTag_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanTag_t *pTag)
{
    uint32  val;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);
    RT_PARAM_CHK((NULL == pTag), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, mirror id=%d, pTag=%x"
            , unit, mirror_id, pTag);

    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);
    RT_PARAM_CHK(pTag->tpidIdx >= HAL_MAX_NUM_OF_CVLAN_TPID(unit), RT_ERR_INPUT);
    RT_PARAM_CHK(RTK_DOT1P_PRIORITY_MAX < pTag->pri, RT_ERR_PRIORITY);
    RT_PARAM_CHK((pTag->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(pTag->cfi > RTK_DOT1P_CFI_MAX, RT_ERR_INPUT);

    MIRROR_SEM_LOCK(unit);

    /* programming value on CHIP*/
    val = pTag->tpidIdx;
    if ((ret = reg_array_field_write(unit, CYPRESS_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, CYPRESS_RSPAN_TAG_TPID_IDXf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pTag->pri;
    if ((ret = reg_array_field_write(unit, CYPRESS_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, CYPRESS_RSPAN_TAG_PRIf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pTag->cfi;
    if ((ret = reg_array_field_write(unit, CYPRESS_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, CYPRESS_RSPAN_TAG_CFIf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    val = pTag->vid;
    if ((ret = reg_array_field_write(unit, CYPRESS_MIR_RSPAN_VLAN_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, CYPRESS_RSPAN_TAG_VIDf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_mirror_rspanIgrTag_set */


/* Module Name    : Mirror             */
/* Sub-module Name: Mirror-based SFLOW */

/* Function Name:
 *      dal_cypress_mirror_sflowMirrorSampleRate_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_MIRROR_ID        - invalid mirror id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_mirror_sflowMirrorSampleRate_get(uint32 unit, uint32 mirror_id, uint32 *pRate)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, mirror_id=%d", unit, mirror_id);

    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);

    MIRROR_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit, CYPRESS_MIR_SAMPLE_RATE_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, CYPRESS_SAMPLE_RATEf, pRate)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_cypress_mirror_sflowMirrorSampleRate_get */

/* Function Name:
 *      dal_cypress_mirror_sflowMirrorSampleRate_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_MIRROR_ID        - invalid mirror id
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 * Note:
 *      None
 */
int32
dal_cypress_mirror_sflowMirrorSampleRate_set(uint32 unit, uint32 mirror_id, uint32 rate)
{
    uint32  val;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, mirror_id=%d, rate=%u"
            , unit, mirror_id, rate);

    RT_PARAM_CHK(mirror_id >= HAL_MAX_NUM_OF_MIRROR(unit), RT_ERR_MIRROR_ID);
    RT_PARAM_CHK(rate > HAL_SFLOW_RATE_MAX(unit), RT_ERR_OUT_OF_RANGE);

    MIRROR_SEM_LOCK(unit);
    val = rate;
    if ((ret = reg_array_field_write(unit, CYPRESS_MIR_SAMPLE_RATE_CTRLr
                        , REG_ARRAY_INDEX_NONE, mirror_id, CYPRESS_SAMPLE_RATEf, &val)) != RT_ERR_OK)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }
    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_mirror_sflowMirrorSampleRate_set */


/* Module Name    : Mirror           */
/* Sub-module Name: Port-based SFLOW */

/* Function Name:
 *      dal_cypress_mirror_sflowSampleCtrl_get
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
dal_cypress_mirror_sflowSampleCtrl_get(uint32 unit, rtk_sflowSampleCtrl_t *pCtrl)
{
    int32   ret;
    int32   val;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d", unit);

    RT_PARAM_CHK((NULL == pCtrl), RT_ERR_NULL_POINTER);

    MIRROR_SEM_LOCK(unit);
    ret = reg_field_read(unit, CYPRESS_SFLOW_CTRLr, CYPRESS_SMPL_SELf, (uint32 *)&val);
    if (RT_ERR_OK != ret)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MPLS), "");
        return ret;
    }
    MIRROR_SEM_UNLOCK(unit);

    if (0 == val)
        *pCtrl = SFLOW_CTRL_INGRESS;
    else if(1 == val)
        *pCtrl = SFLOW_CTRL_EGRESS;

    return RT_ERR_OK;
}    /* end of dal_cypress_mirror_sflowSampleCtrl_get */

/* Function Name:
 *      dal_cypress_mirror_sflowSampleCtrl_set
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
dal_cypress_mirror_sflowSampleCtrl_set(uint32 unit, rtk_sflowSampleCtrl_t ctrl)
{
    int32   ret;
    int32   val;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d ctrl=%d", unit, ctrl);

    RT_PARAM_CHK((SFLOW_CTRL_END <= ctrl), RT_ERR_INPUT);

    if (SFLOW_CTRL_INGRESS == ctrl)
        val = 0;
    else if(SFLOW_CTRL_EGRESS == ctrl)
        val = 1;

    MIRROR_SEM_LOCK(unit);
    ret = reg_field_write(unit, CYPRESS_SFLOW_CTRLr, CYPRESS_SMPL_SELf, (uint32 *)&val);
    if (RT_ERR_OK != ret)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MPLS), "");
        return ret;
    }
    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}    /* end of dal_cypress_mirror_sflowSampleCtrl_set */

/* Function Name:
 *      dal_cypress_mirror_sflowPortIgrSampleRate_get
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
dal_cypress_mirror_sflowPortIgrSampleRate_get(uint32 unit, rtk_port_t port, uint32 *pRate)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, port=%d",unit, port);

    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    MIRROR_SEM_LOCK(unit);
    ret = reg_array_field_read(unit, CYPRESS_SFLOW_PORT_RATE_CTRLr,
                               port, REG_ARRAY_INDEX_NONE,
                               CYPRESS_IGR_RATEf, pRate);
    if (RT_ERR_OK != ret)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MPLS), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_mirror_sflowPortIgrSampleRate_get */

/* Function Name:
 *      dal_cypress_mirror_sflowPortIgrSampleRate_set
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
dal_cypress_mirror_sflowPortIgrSampleRate_set(uint32 unit, rtk_port_t port, uint32 rate)
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
    ret = reg_array_field_write(unit, CYPRESS_SFLOW_PORT_RATE_CTRLr,
                                port, REG_ARRAY_INDEX_NONE,
                                CYPRESS_IGR_RATEf, &rate);
    if (RT_ERR_OK != ret)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_mirror_sflowPortIgrSampleRate_set */

/* Function Name:
 *      dal_cypress_mirror_sflowPortEgrSampleRate_get
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
dal_cypress_mirror_sflowPortEgrSampleRate_get(uint32 unit, rtk_port_t port,
                                              uint32 *pRate)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(mirror_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR), "unit=%d, port=%d",unit, port);

    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    MIRROR_SEM_LOCK(unit);
    ret = reg_array_field_read(unit, CYPRESS_SFLOW_PORT_RATE_CTRLr,
                               port, REG_ARRAY_INDEX_NONE,
                               CYPRESS_EGR_RATEf, pRate);
    if (RT_ERR_OK != ret)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MPLS), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_mirror_sflowPortEgrSampleRate_get */

/* Function Name:
 *      dal_cypress_mirror_sflowPortEgrSampleRate_set
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
dal_cypress_mirror_sflowPortEgrSampleRate_set(uint32 unit, rtk_port_t port,
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
    ret = reg_array_field_write(unit, CYPRESS_SFLOW_PORT_RATE_CTRLr,
                                port, REG_ARRAY_INDEX_NONE,
                                CYPRESS_EGR_RATEf, &rate);
    if (RT_ERR_OK != ret)
    {
        MIRROR_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return ret;
    }

    MIRROR_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_mirror_sflowPortEgrSampleRate_set */


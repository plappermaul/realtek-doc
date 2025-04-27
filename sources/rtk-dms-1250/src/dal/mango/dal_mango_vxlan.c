/*
 * Copyright (C) 2009-2017 Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : Definition those public VXLAN tunneling APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) VXLAN tunneling APIs
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
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/mango/dal_mango_l3.h>
#include <dal/mango/dal_mango_tunnel.h>
#include <dal/mango/dal_mango_vxlan.h>
#include <rtk/vxlan.h>

/*
 * Symbol Definition
 */
#define MANGO_VXLAN_DBG                     (0)         /* debugging level (0 means disable) */
#define DAL_MANGO_VXLAN_VNI_MAGIC           (0x7FFF)
#define DAL_MANGO_VXLAN_VNI_TBL_SRAM_SIZE   (2048)
#define DAL_MANGO_VXLAN_VNI_TBL_BCAM_SIZE   (64)
#define DAL_MANGO_VXLAN_VNI_TBL_SIZE        (DAL_MANGO_VXLAN_VNI_TBL_SRAM_SIZE + DAL_MANGO_VXLAN_VNI_TBL_BCAM_SIZE)

typedef struct dal_mango_vxlan_drvDb_s
{
    //uint32  vni_entry_seeking_idx;   /* to enhance the VNI entry searching performance */

    /* VTEP-VNI table */
    rtk_bitmap_t    vni_entry_bitmap[BITMAP_ARRAY_CNT(DAL_MANGO_VXLAN_VNI_TBL_SIZE)];

    struct
    {
        rtk_intf_id_t   intf_id;
    } vni_entry[DAL_MANGO_VXLAN_VNI_TBL_SIZE];
} dal_mango_vxlan_drvDb_t;

typedef struct dal_mango_vxlan_vniEntry_s
{
    uint32  entry_idx;

    uint32  valid;
    uint32  magic;
    uint32  tt_tunnel_addr;
    uint32  vni;
    uint32  tt_ovid_cmd;
    uint32  tt_ovid;
    uint32  tt_ivid_cmd;
    uint32  tt_ivid;
    uint32  tt_fwd_vlan_sel;
    uint32  tt_pri_sel_idx;
    uint32  tt_int_pri;
    uint32  ts_tunnel_addr;
    uint32  ts_qos_profile;
} dal_mango_vxlan_vniEntry_t;

/*
 * Data Declaration
 */
static uint32                   vxlan_init[RTK_MAX_NUM_OF_UNIT] = { INIT_NOT_COMPLETED };
static osal_mutex_t             vxlan_sem[RTK_MAX_NUM_OF_UNIT];
static dal_mango_vxlan_drvDb_t  *_pVxlanDb[RTK_MAX_NUM_OF_UNIT] = { NULL };

static uint32 _luMisAct[] = {
    ACTION_DROP,
    ACTION_TRAP2CPU,
    };

static uint32 _trapTarget[] = {
    RTK_TRAP_LOCAL,
    RTK_TRAP_MASTER,
    };

static uint32 _invldHdrAct[] = {
    ACTION_DROP,
    ACTION_TRAP2CPU,
    };

static uint32 _ctrlPktAct[] = {
    ACTION_FORWARD,
    ACTION_TRAP2CPU,
    };

static uint32 _excptAct[] = {
    ACTION_DROP,
    ACTION_TRAP2CPU,
    ACTION_FORWARD,
    ACTION_COPY2CPU,
    };


/*
 * Macro Declaration
 */
#define MANGO_VXLAN_DBG_PRINTF(_level, fmt, ...)                                    \
    do {                                                                            \
        if (MANGO_VXLAN_DBG >= (_level))                                            \
            osal_printf("%d:%s(): " fmt, __LINE__, __FUNCTION__, ##__VA_ARGS__);    \
    } while (0)

/* VXLAN semaphore handling */
#define VXLAN_SEM_LOCK(unit)    \
    do {\
        if (osal_sem_mutex_take(vxlan_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
        {\
            RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_VXLAN|MOD_DAL), "semaphore lock failed");\
            return RT_ERR_SEM_LOCK_FAILED;\
        }\
    } while(0)
#define VXLAN_SEM_UNLOCK(unit)   \
    do {\
        if (osal_sem_mutex_give(vxlan_sem[unit]) != RT_ERR_OK)\
        {\
            RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_VXLAN|MOD_DAL), "semaphore unlock failed");\
            return RT_ERR_SEM_UNLOCK_FAILED;\
        }\
    } while(0)

#define VXLAN_REG_FIELD_READ_ERR_HDL(_unit, _reg, _field, _val, _errMsg, _gotoErrLbl, _ret) \
    do {                                                                                    \
        if ((_ret = reg_field_read(_unit, _reg, _field, &_val)) != RT_ERR_OK)               \
        {                                                                                   \
            RT_ERR(_ret, (MOD_DAL|MOD_VXLAN), _errMsg);                                     \
            goto _gotoErrLbl;                                                               \
        }                                                                                   \
    } while(0)
#define VXLAN_REG_FIELD_WRITE_ERR_HDL(_unit, _reg, _field, _val, _errMsg, _gotoErrLbl, _ret)    \
    do {                                                                                        \
        if ((_ret = reg_field_write(_unit, _reg, _field, &_val)) != RT_ERR_OK)                  \
        {                                                                                       \
            RT_ERR(_ret, (MOD_DAL|MOD_VXLAN), _errMsg);                                         \
            goto _gotoErrLbl;                                                                   \
        }                                                                                       \
    } while(0)
#define VXLAN_REG_ARRAY_FIELD_READ_ERR_HDL(_unit, _reg, _idx1, _idx2, _field, _val, _errMsg, _gotoErrLbl, _ret) \
    do {                                                                                                        \
        if ((_ret = reg_array_field_read(_unit, _reg, _idx1, _idx2, _field, &_val)) != RT_ERR_OK)               \
        {                                                                                                       \
            RT_ERR(_ret, (MOD_DAL|MOD_VXLAN), _errMsg);                                                         \
            goto _gotoErrLbl;                                                                                   \
        }                                                                                                       \
    } while(0)
#define VXLAN_REG_ARRAY_FIELD_WRITE_ERR_HDL(_unit, _reg, _idx1, _idx2, _field, _val, _errMsg, _gotoErrLbl, _ret)    \
    do {                                                                                                            \
        if ((_ret = reg_array_field_write(_unit, _reg, _idx1, _idx2, _field, &_val)) != RT_ERR_OK)                  \
        {                                                                                                           \
            RT_ERR(_ret, (MOD_DAL|MOD_VXLAN), _errMsg);                                                             \
            goto _gotoErrLbl;                                                                                       \
        }                                                                                                           \
    } while(0)
#define VXLAN_TABLE_READ_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret) \
    do {                                                                                \
        if ((_ret = table_read(_unit, _tbl, _idx, (uint32 *)&_entry)) != RT_ERR_OK)     \
        {                                                                               \
            RT_ERR(_ret, (MOD_DAL|MOD_VXLAN), _errMsg);                                 \
            goto _gotoErrLbl;                                                           \
        }                                                                               \
    } while(0)
#define VXLAN_TABLE_WRITE_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret)    \
    do {                                                                                    \
        if ((_ret = table_write(_unit, _tbl, _idx, (uint32 *)&_entry)) != RT_ERR_OK)        \
        {                                                                                   \
            RT_ERR(_ret, (MOD_DAL|MOD_VXLAN), _errMsg);                                     \
            goto _gotoErrLbl;                                                               \
        }                                                                                   \
    } while(0)
#define VXLAN_TABLE_FIELD_GET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret)        \
    do {                                                                                                    \
        if ((_ret = table_field_get(_unit, _tbl, _field, (uint32 *)&_val, (uint32 *)&_entry)) != RT_ERR_OK) \
        {                                                                                                   \
            RT_ERR(_ret, (MOD_DAL|MOD_VXLAN), _errMsg);                                                     \
            goto _gotoErrLbl;                                                                               \
        }                                                                                                   \
    } while(0)
#define VXLAN_TABLE_FIELD_SET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret)        \
    do {                                                                                                    \
        if ((_ret = table_field_set(_unit, _tbl, _field, (uint32 *)&_val, (uint32 *)&_entry)) != RT_ERR_OK) \
        {                                                                                                   \
            RT_ERR(_ret, (MOD_DAL|MOD_VXLAN), _errMsg);                                                     \
            goto _gotoErrLbl;                                                                               \
        }                                                                                                   \
    } while(0)
#define VXLAN_TABLE_FIELD_MAC_GET_ERR_HDL(_unit, _tbl, _field, _mac, _entry, _errMsg, _gotoErrLbl, _ret)        \
    do {                                                                                                        \
        if ((_ret = table_field_mac_get(_unit, _tbl, _field, (uint8 *)&_mac, (uint32 *)&_entry)) != RT_ERR_OK)  \
        {                                                                                                       \
            RT_ERR(ret, (MOD_DAL|MOD_VXLAN), _errMsg);                                                          \
            goto _gotoErrLbl;                                                                                   \
        }                                                                                                       \
    } while(0)
#define VXLAN_TABLE_FIELD_MAC_SET_ERR_HDL(_unit, _tbl, _field, _mac, _entry, _errMsg, _gotoErrLbl, _ret)        \
    do {                                                                                                        \
        if ((_ret = table_field_mac_set(_unit, _tbl, _field, (uint8 *)&_mac, (uint32 *)&_entry)) != RT_ERR_OK)  \
        {                                                                                                       \
            RT_ERR(ret, (MOD_DAL|MOD_VXLAN), _errMsg);                                                          \
            goto _gotoErrLbl;                                                                                   \
        }                                                                                                       \
    } while(0)
#define VXLAN_ACTION_TO_VALUE(_actArray, _value, _action, _errMsg, _errHandle, _retval)     \
    do {                                                                                    \
        if ((_retval = RT_UTIL_ACTLIST_INDEX_GET(_actArray, _value, _action)) != RT_ERR_OK) \
        {                                                                                   \
            RT_ERR(_retval, (MOD_DAL|MOD_VXLAN), _errMsg);                                  \
            goto _errHandle;                                                                \
        }                                                                                   \
    } while(0)
#define VXLAN_VALUE_TO_ACTION(_actArray, _action, _value, _errMsg, _errHandle, _retval)         \
    do {                                                                                        \
        if ((_retval = RT_UTIL_ACTLIST_ACTION_GET(_actArray, _action, _value)) != RT_ERR_OK)    \
        {                                                                                       \
            RT_ERR(_retval, (MOD_DAL|MOD_VXLAN), _errMsg);                                      \
            goto _errHandle;                                                                    \
        }                                                                                       \
    } while(0)
#define VXLAN_RT_ERR_HDL_DBG(_op, _args...)                     \
    do {                                                        \
        if (RT_ERR_OK != (_op))                                 \
        {                                                       \
           RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VXLAN), ## _args);    \
        }                                                       \
    } while(0)

#define VXLANDB_ENTRY_VALID_SET(_unit, _idx)    BITMAP_SET(_pVxlanDb[(_unit)]->vni_entry_bitmap, (_idx))
#define VXLANDB_ENTRY_VALID_CLEAR(_unit, _idx)  BITMAP_CLEAR(_pVxlanDb[(_unit)]->vni_entry_bitmap, (_idx))
#define VXLANDB_ENTRY_IS_VALID(_unit, _idx)     BITMAP_IS_SET(_pVxlanDb[(_unit)]->vni_entry_bitmap, (_idx))
#define VXLANDB_ENTRY_IS_INVALID(_unit, _idx)   BITMAP_IS_CLEAR(_pVxlanDb[(_unit)]->vni_entry_bitmap, (_idx))

#define RTK_INTF_ID_IS_L2_TUNNEL(_intfId)       (DAL_MANGO_L3_INTF_ID_IS_TUNNEL(_intfId) && DAL_MANGO_L3_INTF_ID_IS_L2_TUNNEL(_intfId))


/*
 * Function Declaration
 */

/* Function Name:
 *      dal_mango_vxlanMapper_init
 * Description:
 *      Hook VXLAN module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook VXLAN module before calling any VXLAN APIs.
 */
int32
dal_mango_vxlanMapper_init(dal_mapper_t *pMapper)
{
    pMapper->vxlan_init = dal_mango_vxlan_init;
    pMapper->vxlan_vni_add = dal_mango_vxlan_vni_add;
    pMapper->vxlan_vni_del = dal_mango_vxlan_vni_del;
    pMapper->vxlan_vni_delAll = dal_mango_vxlan_vni_delAll;
    pMapper->vxlan_vni_get = dal_mango_vxlan_vni_get;
    pMapper->vxlan_vni_set = dal_mango_vxlan_vni_set;
    pMapper->vxlan_vni_getNext = dal_mango_vxlan_vni_getNext;
    pMapper->vxlan_globalCtrl_get = dal_mango_vxlan_globalCtrl_get;
    pMapper->vxlan_globalCtrl_set = dal_mango_vxlan_globalCtrl_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_vxlan_init
 * Description:
 *      Initialize VXLAN module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize VXLAN module before calling any tunneling APIs.
 */
int32
dal_mango_vxlan_init(uint32 unit)
{
    RT_INIT_REENTRY_CHK(vxlan_init[unit]);
    vxlan_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    vxlan_sem[unit] = osal_sem_mutex_create();
    if (0 == vxlan_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_VXLAN), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* allocate memory for driver database */
    if ((_pVxlanDb[unit] = osal_alloc(sizeof(dal_mango_vxlan_drvDb_t))) == NULL)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_VXLAN), "out of memory");
        return RT_ERR_FAILED;
    }
    osal_memset(_pVxlanDb[unit], 0x00, sizeof(dal_mango_vxlan_drvDb_t));

    /* set init flag to complete init */
    vxlan_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}   /* end of dal_mango_vxlan_init */


static int32 _dal_mango_vxlan_vniEntry_rtk2dal(uint32 unit, dal_mango_vxlan_vniEntry_t *pDalEntry, rtk_vxlan_vniEntry_t *pRtkEntry, int32 dalIdx)
{
    uint32  tunnelIdx = DAL_MANGO_L3_INTF_ID_ENTRY_IDX(pRtkEntry->intf_id);

    osal_memset(pDalEntry, 0x00, sizeof(dal_mango_vxlan_vniEntry_t));
    pDalEntry->valid = 1;
    pDalEntry->magic = 0x7FFF;
    pDalEntry->tt_tunnel_addr = DAL_MANGO_TUNNEL_ENTRY_IDX_TO_ADDR(tunnelIdx);
    pDalEntry->vni = pRtkEntry->vni;
    pDalEntry->tt_ovid_cmd = pRtkEntry->ovid_cmd;
    pDalEntry->tt_ovid = pRtkEntry->ovid;
    pDalEntry->tt_ivid_cmd = pRtkEntry->ivid_cmd;
    pDalEntry->tt_ivid = pRtkEntry->ivid;
    pDalEntry->tt_fwd_vlan_sel = pRtkEntry->fwd_vlan;
    pDalEntry->tt_pri_sel_idx = pRtkEntry->priGrp_idx;
    pDalEntry->tt_int_pri = pRtkEntry->int_pri;
    pDalEntry->ts_tunnel_addr = DAL_MANGO_TUNNEL_ENTRY_IDX_TO_ADDR(tunnelIdx);
    pDalEntry->ts_qos_profile = pRtkEntry->qosPro_idx;

    /* update shadow */
    _pVxlanDb[unit]->vni_entry[dalIdx].intf_id = pRtkEntry->intf_id;

    return RT_ERR_OK;
}

static int32 _dal_mango_vxlan_vniEntry_dal2rtk(uint32 unit, rtk_vxlan_vniEntry_t *pRtkEntry, dal_mango_vxlan_vniEntry_t *pDalEntry, int32 dalIdx)
{
    osal_memset(pRtkEntry, 0x00, sizeof(rtk_vxlan_vniEntry_t));
    pRtkEntry->intf_id = _pVxlanDb[unit]->vni_entry[dalIdx].intf_id;    /* take back from shadow */
    pRtkEntry->vni = pDalEntry->vni;
    pRtkEntry->ovid_cmd = pDalEntry->tt_ovid_cmd;
    pRtkEntry->ovid = pDalEntry->tt_ovid;
    pRtkEntry->ivid_cmd = pDalEntry->tt_ivid_cmd;
    pRtkEntry->ivid = pDalEntry->tt_ivid;
    pRtkEntry->fwd_vlan = pDalEntry->tt_fwd_vlan_sel;
    pRtkEntry->int_pri = pDalEntry->tt_int_pri;
    pRtkEntry->priGrp_idx = pDalEntry->tt_pri_sel_idx;
    pRtkEntry->qosPro_idx = pDalEntry->ts_qos_profile;
    pRtkEntry->entry_idx = dalIdx;

    return RT_ERR_OK;
}

static int32 _dal_mango_vxlan_vniEntry_read(uint32 unit, uint32 index, dal_mango_vxlan_vniEntry_t *pEntry)
{
    int32   ret;
    l2_tnl_entry_t          ttEntry;
    l2_tnl_entry_encap_t    tsEntry;

    if (index < DAL_MANGO_VXLAN_VNI_TBL_SRAM_SIZE)
    {
        VXLAN_TABLE_READ_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, index, ttEntry, "", errHandle, ret);
        VXLAN_TABLE_READ_ERR_HDL(unit, MANGO_L2_TNL_VXLAN_ENCAPt, index, tsEntry, "", errHandle, ret);
    } else {
        VXLAN_TABLE_READ_ERR_HDL(unit, MANGO_L2_TNL_VXLAN_CAMt, (index - DAL_MANGO_VXLAN_VNI_TBL_SRAM_SIZE), ttEntry, "", errHandle, ret);
        VXLAN_TABLE_READ_ERR_HDL(unit, MANGO_L2_TNL_VXLAN_ENCAPt, index, tsEntry, "", errHandle, ret);
    }

    osal_memset(pEntry, 0x00, sizeof(dal_mango_vxlan_vniEntry_t));
    VXLAN_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_VALIDtf,
        pEntry->valid, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_MAGICtf,
        pEntry->magic, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_TT_TUNNEL_IDXtf,
        pEntry->tt_tunnel_addr, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_VNItf,
        pEntry->vni, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_TT_OVID_CMDtf,
        pEntry->tt_ovid_cmd, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_TT_OVIDtf,
        pEntry->tt_ovid, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_TT_IVID_CMDtf,
        pEntry->tt_ivid_cmd, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_TT_IVIDtf,
        pEntry->tt_ivid, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_TT_FWD_VLAN_SELtf,
        pEntry->tt_fwd_vlan_sel, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_TT_PRI_SEL_IDXtf,
        pEntry->tt_pri_sel_idx, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_TT_INT_PRItf,
        pEntry->tt_int_pri, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_TS_TUNNEL_IDXtf,
        pEntry->ts_tunnel_addr, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L2_TNL_VXLAN_ENCAPt, MANGO_L2_TNL_VXLAN_ENCAP_QOS_PROFILEtf,
        pEntry->ts_qos_profile, tsEntry, "", errHandle, ret);

    /* entry index */
    pEntry->entry_idx = index;

errHandle:
    return ret;
}

static int32 _dal_mango_vxlan_vniEntry_write(uint32 unit, uint32 index, dal_mango_vxlan_vniEntry_t *pEntry)
{
    int32   ret;
    l2_tnl_entry_t          ttEntry;
    l2_tnl_entry_encap_t    tsEntry;

    osal_memset(&ttEntry, 0x00, sizeof(l2_tnl_entry_t));
    VXLAN_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_VALIDtf,
        pEntry->valid, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_MAGICtf,
        pEntry->magic, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_TT_TUNNEL_IDXtf,
        pEntry->tt_tunnel_addr, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_VNItf,
        pEntry->vni, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_TT_OVID_CMDtf,
        pEntry->tt_ovid_cmd, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_TT_OVIDtf,
        pEntry->tt_ovid, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_TT_IVID_CMDtf,
        pEntry->tt_ivid_cmd, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_TT_IVIDtf,
        pEntry->tt_ivid, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_TT_FWD_VLAN_SELtf,
        pEntry->tt_fwd_vlan_sel, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_TT_PRI_SEL_IDXtf,
        pEntry->tt_pri_sel_idx, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_TT_INT_PRItf,
        pEntry->tt_int_pri, ttEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, MANGO_L2_TNL_VXLAN_TS_TUNNEL_IDXtf,
        pEntry->ts_tunnel_addr, ttEntry, "", errHandle, ret);

    osal_memset(&tsEntry, 0x00, sizeof(l2_tnl_entry_encap_t));
    VXLAN_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L2_TNL_VXLAN_ENCAPt, MANGO_L2_TNL_VXLAN_ENCAP_MAGICtf,
        pEntry->magic, tsEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L2_TNL_VXLAN_ENCAPt, MANGO_L2_TNL_VXLAN_ENCAP_TS_TUNNEL_IDXtf,
        pEntry->ts_tunnel_addr, tsEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L2_TNL_VXLAN_ENCAPt, MANGO_L2_TNL_VXLAN_ENCAP_VNItf,
        pEntry->vni, tsEntry, "", errHandle, ret);

    VXLAN_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L2_TNL_VXLAN_ENCAPt, MANGO_L2_TNL_VXLAN_ENCAP_QOS_PROFILEtf,
        pEntry->ts_qos_profile, tsEntry, "", errHandle, ret);

    if (index < DAL_MANGO_VXLAN_VNI_TBL_SRAM_SIZE)
    {
        VXLAN_TABLE_WRITE_ERR_HDL(unit, MANGO_L2_TNL_VXLANt, index, ttEntry, "", errHandle, ret);
        VXLAN_TABLE_WRITE_ERR_HDL(unit, MANGO_L2_TNL_VXLAN_ENCAPt, index, tsEntry, "", errHandle, ret);
    } else {
        VXLAN_TABLE_WRITE_ERR_HDL(unit, MANGO_L2_TNL_VXLAN_CAMt, (index - DAL_MANGO_VXLAN_VNI_TBL_SRAM_SIZE), ttEntry, "", errHandle, ret);
        VXLAN_TABLE_WRITE_ERR_HDL(unit, MANGO_L2_TNL_VXLAN_ENCAPt, index, tsEntry, "", errHandle, ret);
    }

    MANGO_VXLAN_DBG_PRINTF(1, " <WRITE> VNI entry index = %d\n", index);
    MANGO_VXLAN_DBG_PRINTF(1, " TT DATA[4]: %08X %08X %08X %08X\n",
        *(((uint32 *)&ttEntry) + 0), *(((uint32 *)&ttEntry) + 1),
        *(((uint32 *)&ttEntry) + 2), *(((uint32 *)&ttEntry) + 3));
    MANGO_VXLAN_DBG_PRINTF(1, " TS DATA[4]: %08X %08X %08X %08X\n",
        *(((uint32 *)&tsEntry) + 0), *(((uint32 *)&tsEntry) + 1),
        *(((uint32 *)&tsEntry) + 2), *(((uint32 *)&tsEntry) + 3));

errHandle:
    return ret;
}



static uint32 _dal_mango_vxlan_vni_hashIdx_ret(uint32 unit, rtk_vxlan_vniEntry_t *pEntry)
{
    /* assume the entry's interface must be an L2 (tunnel) */
    int32   ret;
    uint32  magicNum = 0x7FFFUL;
    uint32  tunnelIdx = DAL_MANGO_L3_INTF_ID_ENTRY_IDX(pEntry->intf_id);
    uint32  tunnelAddr = DAL_MANGO_TUNNEL_ENTRY_IDX_TO_ADDR(tunnelIdx);
    uint32  vni = pEntry->vni;
    uint32  hashAlg;
    uint32  hashIdx;
    uint64  hashValue;

    VXLAN_REG_FIELD_READ_ERR_HDL(unit, MANGO_L2_TUNNEL_CTRLr, MANGO_BSSID_HASH_ALGOf, hashAlg, "", errHandle, ret);

    hashValue = ((magicNum & 0x7FFFULL) << 33) |
                ((tunnelAddr & 0x1FFULL) << 24) |
                ((vni & 0xFFFFFFULL) << 0);

    if (0 == hashAlg)
    {
        /* hash algorithm 0 */
        hashIdx = (hashValue & (0xFFULL << 40)) >> 40;
        hashIdx ^= (hashValue & (0xFFULL << 32)) >> 32;
        hashIdx ^= (hashValue & (0xFFULL << 24)) >> 24;
        hashIdx ^= (hashValue & (0xFFULL << 16)) >> 16;
        hashIdx ^= (hashValue & (0xFFULL << 8)) >> 8;
        hashIdx ^= (hashValue & (0xFFULL << 0)) >> 0;
    } else {
        /* hash algorithm 1 */
        hashIdx = (hashValue & (0x1ULL << 40)) >> (40 - 7);
        hashIdx ^= (hashValue & (0x1ULL << 41)) >> (41 - 6);
        hashIdx ^= (hashValue & (0x1ULL << 42)) >> (42 - 5);
        hashIdx ^= (hashValue & (0x1ULL << 43)) >> (43 - 4);
        hashIdx ^= (hashValue & (0x1ULL << 44)) >> (44 - 3);
        hashIdx ^= (hashValue & (0x1ULL << 45)) >> (45 - 2);
        hashIdx ^= (hashValue & (0x1ULL << 46)) >> (46 - 1);
        hashIdx ^= (hashValue & (0x1ULL << 47)) >> (47 - 0);
        hashIdx ^= (hashValue & (0xFFULL << 32)) >> 32;
        hashIdx ^= (hashValue & (0x1ULL << 24)) >> (24 - 7);
        hashIdx ^= (hashValue & (0x1ULL << 25)) >> (25 - 6);
        hashIdx ^= (hashValue & (0x1ULL << 26)) >> (26 - 5);
        hashIdx ^= (hashValue & (0x1ULL << 27)) >> (27 - 4);
        hashIdx ^= (hashValue & (0x1ULL << 28)) >> (28 - 3);
        hashIdx ^= (hashValue & (0x1ULL << 29)) >> (29 - 2);
        hashIdx ^= (hashValue & (0x1ULL << 30)) >> (30 - 1);
        hashIdx ^= (hashValue & (0x1ULL << 31)) >> (31 - 0);
        hashIdx ^= (hashValue & (0xFFULL << 16)) >> 16;
        hashIdx ^= (hashValue & (0x1ULL <<  8)) >> ( 8 - 7);
        hashIdx ^= (hashValue & (0x1ULL <<  9)) >> ( 9 - 6);
        hashIdx ^= (hashValue & (0x1ULL << 10)) >> (10 - 5);
        hashIdx ^= (hashValue & (0x1ULL << 11)) >> (11 - 4);
        hashIdx ^= (hashValue & (0x1ULL << 12)) >> (12 - 3);
        hashIdx ^= (hashValue & (0x1ULL << 13)) >> (13 - 2);
        hashIdx ^= (hashValue & (0x1ULL << 14)) >> (14 - 1);
        hashIdx ^= (hashValue & (0x1ULL << 15)) >> (15 - 0);
        hashIdx ^= (hashValue & (0xFFULL << 0)) >> 0;
    }

    MANGO_VXLAN_DBG_PRINTF(3, "magicNum=0x%X,tunnelAddr=0x%X,vni=0x%X -> hashIdx=0x%X(%u)\n", magicNum, tunnelAddr, vni, hashIdx, hashIdx);

    return hashIdx;

errHandle:
    return 0;
}


/* find the entry and a valid empty index */
static int32 _dal_mango_vxlan_vni_find(uint32 unit, rtk_vxlan_vniEntry_t *pEntry, int32 *pFoundIdx, int32 *pEmptyIdx)
{
    int32   ret;
    uint32  i;
    uint32  hashIdx = _dal_mango_vxlan_vni_hashIdx_ret(unit, pEntry);
    uint32  entryIdx;
    dal_mango_vxlan_vniEntry_t vniEntry;
    uint32  tunnelIdx, tunnelAddr;

    *pFoundIdx = -1;    /* invalid index */
    *pEmptyIdx = -1;    /* invalid index */

    tunnelIdx = DAL_MANGO_L3_INTF_ID_ENTRY_IDX(pEntry->intf_id);
    tunnelAddr = DAL_MANGO_TUNNEL_ENTRY_IDX_TO_ADDR(tunnelIdx);

    for (i=0; i<8; i++)
    {
        entryIdx = (hashIdx << 3) + i;

        RT_ERR_HDL(_dal_mango_vxlan_vniEntry_read(unit, entryIdx, &vniEntry), errHandle, ret);
        if (vniEntry.valid == 0)
        {
            if (*pEmptyIdx < 0)
                *pEmptyIdx = entryIdx;
        } else {
            if ((vniEntry.magic == DAL_MANGO_VXLAN_VNI_MAGIC) &&
                (vniEntry.tt_tunnel_addr == tunnelAddr) &&
                (vniEntry.vni == pEntry->vni))
            {
                *pFoundIdx = entryIdx;
                return RT_ERR_OK;
            }
        }
    }
    if (i >= 8)
    {
        for (i=0; i<DAL_MANGO_VXLAN_VNI_TBL_BCAM_SIZE; i++)
        {
            entryIdx = DAL_MANGO_VXLAN_VNI_TBL_SRAM_SIZE + i;

            RT_ERR_HDL(_dal_mango_vxlan_vniEntry_read(unit, entryIdx, &vniEntry), errHandle, ret);
            if (vniEntry.valid == 0)
            {
                if (*pEmptyIdx < 0)
                    *pEmptyIdx = entryIdx;
            } else {
                if ((vniEntry.magic == DAL_MANGO_VXLAN_VNI_MAGIC) &&
                    (vniEntry.tt_tunnel_addr == tunnelAddr) &&
                    (vniEntry.vni == pEntry->vni))
                {
                    *pFoundIdx = entryIdx;
                    return RT_ERR_OK;
                }
            }
        }
    }

    ret = RT_ERR_ENTRY_NOTFOUND;

errHandle:
    return ret;
}


/* Function Name:
 *      dal_mango_vxlan_vni_add
 * Description:
 *      Add a new VNI entry
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to VNI entry (intf_id and vni as match key)
 * Output:
 *      pEntry - pointer to VNI entry (all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_TBL_FULL     - table is full
 * Note:
 *      None
 */
int32
dal_mango_vxlan_vni_add(uint32 unit, rtk_vxlan_vniEntry_t *pEntry)
{
    int32   ret = RT_ERR_OK;
    int32   foundIdx, emptyIdx;
    dal_mango_vxlan_vniEntry_t dalEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VXLAN), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vxlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!RTK_INTF_ID_IS_L2_TUNNEL(pEntry->intf_id)), RT_ERR_INPUT);
    RT_PARAM_CHK((pEntry->vni > RTK_VXLAN_VNI_MAX), RT_ERR_INPUT);

    /* function body */
    VXLAN_SEM_LOCK(unit);

    /* check entry existence */
    ret = _dal_mango_vxlan_vni_find(unit, pEntry, &foundIdx, &emptyIdx);
    if (ret == RT_ERR_OK)
    {
        ret = RT_ERR_ENTRY_EXIST;
        goto errHandle;
    }
    else if (emptyIdx < 0)
    {   ret = RT_ERR_TBL_FULL;
        goto errHandle;
    }

    /* not found and have a valid emptry entry */
    RT_ERR_HDL(_dal_mango_vxlan_vniEntry_rtk2dal(unit, &dalEntry, pEntry, emptyIdx), errHandle, ret);
    RT_ERR_HDL(_dal_mango_vxlan_vniEntry_write(unit, emptyIdx, &dalEntry), errHandle, ret);
    VXLANDB_ENTRY_VALID_SET(unit, emptyIdx);

    pEntry->entry_idx = emptyIdx;

errHandle:
    VXLAN_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_vxlan_vni_add */

/* Function Name:
 *      dal_mango_vxlan_vni_del
 * Description:
 *      Delete an extisting VNI entry
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to VNI entry (intf_id and vni as match key)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - entry is not found
 * Note:
 *      None
 */
int32
dal_mango_vxlan_vni_del(uint32 unit, rtk_vxlan_vniEntry_t *pEntry)
{
    int32   ret = RT_ERR_OK;
    int32   foundIdx, emptyIdx;
    dal_mango_vxlan_vniEntry_t dalEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VXLAN), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vxlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!RTK_INTF_ID_IS_L2_TUNNEL(pEntry->intf_id)), RT_ERR_INPUT);
    RT_PARAM_CHK((pEntry->vni > RTK_VXLAN_VNI_MAX), RT_ERR_INPUT);

    /* function body */
    VXLAN_SEM_LOCK(unit);

    /* check entry existence */
    ret = _dal_mango_vxlan_vni_find(unit, pEntry, &foundIdx, &emptyIdx);
    if (ret == RT_ERR_ENTRY_NOTFOUND)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }
    else if (ret != RT_ERR_OK)
    {
        goto errHandle;
    }

    /* found */
    osal_memset(&dalEntry, 0x00, sizeof(dal_mango_vxlan_vniEntry_t));
    RT_ERR_HDL(_dal_mango_vxlan_vniEntry_write(unit, foundIdx, &dalEntry), errHandle, ret);
    VXLANDB_ENTRY_VALID_CLEAR(unit, foundIdx);

errHandle:
    VXLAN_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_vxlan_vni_del */

/* Function Name:
 *      dal_mango_vxlan_vni_delAll
 * Description:
 *      Delete all VNI entries
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 * Note:
 *      None
 */
int32
dal_mango_vxlan_vni_delAll(uint32 unit)
{
    int32   ret = RT_ERR_OK;
    int32   entryIdx;
    dal_mango_vxlan_vniEntry_t dalEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VXLAN), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vxlan_init[unit]);

    /* parameter check */

    /* function body */
    VXLAN_SEM_LOCK(unit);

    osal_memset(&dalEntry, 0x00, sizeof(dal_mango_vxlan_vniEntry_t));
    for (entryIdx=0; entryIdx<DAL_MANGO_VXLAN_VNI_TBL_SIZE; entryIdx++)
    {
        if (VXLANDB_ENTRY_IS_INVALID(unit, entryIdx))
            continue;

        RT_ERR_HDL(_dal_mango_vxlan_vniEntry_write(unit, entryIdx, &dalEntry), errHandle, ret);
        VXLANDB_ENTRY_VALID_CLEAR(unit, entryIdx);
    }

errHandle:
    VXLAN_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_vxlan_vni_delAll */

/* Function Name:
 *      dal_mango_vxlan_vni_get
 * Description:
 *      Get a VNI entry by interface id and VNI as key.
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to VNI entry (interface id and VNI as match key)
 * Output:
 *      pEntry - pointer to VNI entry (all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
int32
dal_mango_vxlan_vni_get(uint32 unit, rtk_vxlan_vniEntry_t *pEntry)
{
    int32   ret = RT_ERR_OK;
    int32   foundIdx, emptyIdx;
    dal_mango_vxlan_vniEntry_t dalEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VXLAN), "unit=%d,pEntry=%p", unit, pEntry);

    /* check Init status */
    RT_INIT_CHK(vxlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!RTK_INTF_ID_IS_L2_TUNNEL(pEntry->intf_id)), RT_ERR_INPUT);
    RT_PARAM_CHK((pEntry->vni > RTK_VXLAN_VNI_MAX), RT_ERR_INPUT);

    /* function body */
    VXLAN_SEM_LOCK(unit);

    /* check entry existence */
    ret = _dal_mango_vxlan_vni_find(unit, pEntry, &foundIdx, &emptyIdx);
    if (ret == RT_ERR_ENTRY_NOTFOUND)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }
    else if (ret != RT_ERR_OK)
    {
        goto errHandle;
    }

    /* found */
    RT_ERR_HDL(_dal_mango_vxlan_vniEntry_read(unit, foundIdx, &dalEntry), errHandle, ret);
    RT_ERR_HDL(_dal_mango_vxlan_vniEntry_dal2rtk(unit, pEntry, &dalEntry, foundIdx), errHandle, ret);

errHandle:
    VXLAN_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_vxlan_vni_get */

/* Function Name:
 *      dal_mango_vxlan_vni_set
 * Description:
 *      Set a VNI entry
 * Input:
 *      unit  - unit id
 *      pEntry - pointer to VNI entry (interface id and VNI as match key)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - the module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOT_FOUND - table is full
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.1.1]
 *          New added function.
 */
int32
dal_mango_vxlan_vni_set(uint32 unit, rtk_vxlan_vniEntry_t *pEntry)
{
    int32   ret = RT_ERR_OK;
    int32   foundIdx, emptyIdx;
    dal_mango_vxlan_vniEntry_t dalEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VXLAN), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vxlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!RTK_INTF_ID_IS_L2_TUNNEL(pEntry->intf_id)), RT_ERR_INPUT);
    RT_PARAM_CHK((pEntry->vni > RTK_VXLAN_VNI_MAX), RT_ERR_INPUT);

    /* function body */
    VXLAN_SEM_LOCK(unit);

    /* check entry existence */
    ret = _dal_mango_vxlan_vni_find(unit, pEntry, &foundIdx, &emptyIdx);
    if (ret == RT_ERR_ENTRY_NOTFOUND)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }
    else if (ret != RT_ERR_OK)
    {
        goto errHandle;
    }

    /* found -> update */
    RT_ERR_HDL(_dal_mango_vxlan_vniEntry_rtk2dal(unit, &dalEntry, pEntry, foundIdx), errHandle, ret);
    RT_ERR_HDL(_dal_mango_vxlan_vniEntry_write(unit, foundIdx, &dalEntry), errHandle, ret);

errHandle:
    VXLAN_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_vxlan_vni_getNext
 * Description:
 *      Get the next VNI entry
 * Input:
 *      unit   - unit id
 *      pBase  - start index
 * Output:
 *      pEntry - pointer to a VNI entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      (1) Use base index -1 to indicate to search from the beginging of VNI entry
 *      (2) If the returned index is -1, means not found the next valid entry
 */
int32
dal_mango_vxlan_vni_getNext(uint32 unit, int32 *pBase, rtk_vxlan_vniEntry_t *pEntry)
{
    int32   ret = RT_ERR_OK;
    int32   entryIdx;
    dal_mango_vxlan_vniEntry_t vniEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VXLAN), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vxlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBase), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    VXLAN_SEM_LOCK(unit);

    for (entryIdx=(*pBase<0)?(0):((*pBase)+1); entryIdx<DAL_MANGO_VXLAN_VNI_TBL_SIZE; entryIdx++)
    {
        if (VXLANDB_ENTRY_IS_INVALID(unit, entryIdx))
            continue;

        RT_ERR_HDL(_dal_mango_vxlan_vniEntry_read(unit, entryIdx, &vniEntry), errHandle, ret);
        if ((vniEntry.valid == 1) &&
            (vniEntry.magic == DAL_MANGO_VXLAN_VNI_MAGIC))
        {
            RT_ERR_HDL(_dal_mango_vxlan_vniEntry_dal2rtk(unit, pEntry, &vniEntry, entryIdx), errHandle, ret);
            pEntry->entry_idx = entryIdx;
            *pBase = entryIdx;

            MANGO_VXLAN_DBG_PRINTF(3, "MAGIC = 0x%X, entry_idx = %u\n", vniEntry.magic, entryIdx);

            goto errOk;
        }
    }

    *pBase = -1;    /* finish (not found) */

errHandle:
errOk:
    VXLAN_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_vxlan_vni_getNext */

/* Function Name:
 *      dal_mango_vxlan_globalCtrl_get
 * Description:
 *      Get the global configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 * Output:
 *      pArg - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
int32
dal_mango_vxlan_globalCtrl_get(uint32 unit, rtk_vxlan_globalCtrlType_t type, int32 *pArg)
{
    int32   ret = RT_ERR_OK;
    uint32  val;
    rtk_action_t act;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VXLAN), "unit=%d,type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(vxlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_VXLAN_GCT_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pArg), RT_ERR_NULL_POINTER);

    /* function body */
    VXLAN_SEM_LOCK(unit);

    switch (type)
    {
    case RTK_VXLAN_GCT_VXLAN_TRAP_TARGET:
        {
            VXLAN_REG_FIELD_READ_ERR_HDL(unit, MANGO_L2_TUNNEL_CTRLr, MANGO_VXLAN_TRAP_TARGETf, val, "", errExit, ret);
            VXLAN_VALUE_TO_ACTION(_trapTarget, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_VXLAN_GCT_VXLAN_INVALID_HDR_ACT:
        {
            VXLAN_REG_FIELD_READ_ERR_HDL(unit, MANGO_L2_TUNNEL_CTRLr, MANGO_VXLAN_INVLD_HDR_ACTf, val, "", errExit, ret);
            VXLAN_VALUE_TO_ACTION(_invldHdrAct, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_VXLAN_GCT_VXLAN_CTRL_FRAME_ACT:
        {
            VXLAN_REG_FIELD_READ_ERR_HDL(unit, MANGO_L2_TUNNEL_CTRLr, MANGO_VXLAN_CTRL_PKT_ACTf, val, "", errExit, ret);
            VXLAN_VALUE_TO_ACTION(_ctrlPktAct, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_VXLAN_GCT_VXLAN_EXCEPT_FLAGS:
        {
            VXLAN_REG_FIELD_READ_ERR_HDL(unit, MANGO_L2_TUNNEL_CTRLr, MANGO_VXLAN_EXCPT_FLAGSf, val, "", errExit, ret);
            *pArg = val;
        }
        break;

    case RTK_VXLAN_GCT_VXLAN_GPE_EXCEPT_FLAGS:
        {
            VXLAN_REG_FIELD_READ_ERR_HDL(unit, MANGO_L2_TUNNEL_CTRLr, MANGO_VXLAN_GPE_EXCPT_FLAGSf, val, "", errExit, ret);
            *pArg = val;
        }
        break;

    case RTK_VXLAN_GCT_VXLAN_EXCEPT_FLAGS_ACT:
        {
            VXLAN_REG_FIELD_READ_ERR_HDL(unit, MANGO_L2_TUNNEL_CTRLr, MANGO_VXLAN_EXCPT_ACTf, val, "", errExit, ret);
            VXLAN_VALUE_TO_ACTION(_excptAct, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_VXLAN_GCT_VXLAN_LU_MIS_ACT:
        {
            VXLAN_REG_FIELD_READ_ERR_HDL(unit, MANGO_L2_TUNNEL_CTRLr, MANGO_VXLAN_VNI_LU_MIS_ACTf, val, "", errExit, ret);
            VXLAN_VALUE_TO_ACTION(_luMisAct, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    default:
        ret = RT_ERR_INPUT;
        goto errInput;
    }

errInput:
errExit:
    VXLAN_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_vxlan_globalCtrl_get */

/* Function Name:
 *      dal_mango_vxlan_globalCtrl_set
 * Description:
 *      Set the global configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 *      arg  - argurment
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
int32
dal_mango_vxlan_globalCtrl_set(uint32 unit, rtk_vxlan_globalCtrlType_t type, int32 arg)
{
    int32   ret = RT_ERR_OK;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VXLAN), "unit=%d,type=%d,arg=0x%08x", unit, type, arg);

    /* check Init status */
    RT_INIT_CHK(vxlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= RTK_VXLAN_GCT_END), RT_ERR_OUT_OF_RANGE);

    /* function body */
    VXLAN_SEM_LOCK(unit);

    switch (type)
    {
    case RTK_VXLAN_GCT_VXLAN_TRAP_TARGET:
        {
            VXLAN_ACTION_TO_VALUE(_trapTarget, val, arg, "", errExit, ret);
            VXLAN_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L2_TUNNEL_CTRLr, MANGO_VXLAN_TRAP_TARGETf, val, "", errExit, ret);
        }
        break;

    case RTK_VXLAN_GCT_VXLAN_INVALID_HDR_ACT:
        {
            VXLAN_ACTION_TO_VALUE(_invldHdrAct, val, arg, "", errExit, ret);
            VXLAN_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L2_TUNNEL_CTRLr, MANGO_VXLAN_INVLD_HDR_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_VXLAN_GCT_VXLAN_CTRL_FRAME_ACT:
        {
            VXLAN_ACTION_TO_VALUE(_ctrlPktAct, val, arg, "", errExit, ret);
            VXLAN_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L2_TUNNEL_CTRLr, MANGO_VXLAN_CTRL_PKT_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_VXLAN_GCT_VXLAN_EXCEPT_FLAGS:
        {
            val = arg;
            VXLAN_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L2_TUNNEL_CTRLr, MANGO_VXLAN_EXCPT_FLAGSf, val, "", errExit, ret);
        }
        break;

    case RTK_VXLAN_GCT_VXLAN_GPE_EXCEPT_FLAGS:
        {
            val = arg;
            VXLAN_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L2_TUNNEL_CTRLr, MANGO_VXLAN_GPE_EXCPT_FLAGSf, val, "", errExit, ret);
        }
        break;

    case RTK_VXLAN_GCT_VXLAN_EXCEPT_FLAGS_ACT:
        {
            VXLAN_ACTION_TO_VALUE(_excptAct, val, arg, "", errExit, ret);
            VXLAN_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L2_TUNNEL_CTRLr, MANGO_VXLAN_EXCPT_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_VXLAN_GCT_VXLAN_LU_MIS_ACT:
        {
            VXLAN_ACTION_TO_VALUE(_luMisAct, val, arg, "", errExit, ret);
            VXLAN_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L2_TUNNEL_CTRLr, MANGO_VXLAN_VNI_LU_MIS_ACTf, val, "", errExit, ret);
        }
        break;

    default:
        ret = RT_ERR_INPUT;
        goto errInput;
    }

errInput:
errExit:
    VXLAN_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_vxlan_globalCtrl_set */



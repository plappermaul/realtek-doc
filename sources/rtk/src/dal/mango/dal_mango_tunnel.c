/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public tunneling APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Tunneling APIs
 *
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
#include <dal/mango/dal_mango_l2.h>
#include <dal/mango/dal_mango_l3.h>
#include <dal/mango/dal_mango_tunnel.h>
#include <rtk/tunnel.h>

/*
 * Symbol Definition
 */
#define MANGO_TUNNEL_DBG                        (0)
#define MANGO_TUNNEL_DBG_PRINTF(_level, _fmt, ...)                                  \
    do {                                                                            \
        if (MANGO_TUNNEL_DBG >= (_level))                                           \
            osal_printf("%s():L%d: " _fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
    } while (0)

#ifdef  CONFIG_SDK_FPGA_PLATFORM
#define DAL_MANGO_TUNNEL_INTF_MAX               (8 * 3)
#else
#define DAL_MANGO_TUNNEL_INTF_MAX               (384)
#endif

#define DAL_MANGO_TUNNEL_L3_INTF_IDX_INVALID    (0)
#define DAL_MANGO_TUNNEL_INTF_PATH_ID_INVALID   (0)


/* tunnel interface valid mask */
#define DAL_MANGO_TUNNEL_INTF_VMSK_WIDTH        (3)
#define DAL_MANGO_TUNNEL_INTF_VMSK_HEIGHT       ((DAL_MANGO_TUNNEL_INTF_MAX) / (DAL_MANGO_TUNNEL_INTF_VMSK_WIDTH))
#define DAL_MANGO_TUNNEL_INTF_VMSK_FULL_MASK    ((1 << (DAL_MANGO_TUNNEL_INTF_VMSK_WIDTH)) - 1)
#define DAL_MANGO_TUNNEL_IP_6RD_DOMAIN_MAX      (2)

#define DAL_MANGO_TUNNEL_VXLAN_HDR_VALUE        (0x0800)    /* Flags: 0x08, Next-Proto: 0x00 */
#define DAL_MANGO_TUNNEL_VXLAN_GPE_HDR_VALUE    (0x0C03)    /* Flags: 0x0C, Next-Proto: 0x03 */

/* structure of tunnel decap entry */
typedef struct dal_mango_tunnel_decapEntry_s
{
    uint32  valid;

    uint32  entry_type;
    uint32  intf_id;
    rtk_ip_addr_t   sip;
    rtk_ip_addr_t   dip;
    rtk_ipv6_addr_t sip6;
    rtk_ipv6_addr_t dip6;
    uint32  proto_type;
    uint32  l4_msb_data;
    uint32  l4_lsb_data;
    uint32  gre_key_exist;

    uint32  bmsk_entry_type;
    uint32  bmsk_intf_id;
    rtk_ip_addr_t   bmsk_sip;
    rtk_ipv6_addr_t bmsk_sip6;
    rtk_ip_addr_t   bmsk_dip;
    rtk_ipv6_addr_t bmsk_dip6;
    uint32  bmsk_proto_type;
    uint32  bmsk_l4_msb_data;
    uint32  bmsk_l4_lsb_data;
    uint32  bmsk_gre_key_exist;

    uint32  tunnel_type;
    uint32  inner_ipv4_en;
    uint32  inner_ipv6_en;
    uint32  use_tunnel_ttl;
    uint32  use_tunnel_dscp;
    uint32  keep_tunnel_dscp;
    uint32  pri_sel_tbl_idx;
    uint32  int_pri;
    uint32  tunnel_intf_id;
} dal_mango_tunnel_decapEntry_t;

/* structure of tunnel encap entry */
typedef struct dal_mango_tunnel_encapEntry_s
{
    uint32  entry_type;
    uint32  tunnel_type;
    rtk_ip_addr_t   sip;
    rtk_ip_addr_t   dip;
    rtk_ipv6_addr_t sip6;
    rtk_ipv6_addr_t dip6;
    uint32  flow_label;
    uint32  ttl_assign;
    uint32  ttl;
    uint32  ttl_dec;
    uint32  dont_frag_assign;
    uint32  dont_frag;
    uint32  l4_msb_data;
    uint32  l4_lsb_data;
    uint32  qos_profile_idx;
    uint32  vxlan_itag_act;
    uint32  vxlan_otag_act;
    uint32  vxlan_vni;
    uint32  gre_key_add;

    uint32  fvid_sel;
    uint32  dbl_tag_vid;
    uint32  nh_info_valid;
    uint32  dbl_tag_sts;
    uint32  nh_dmac_idx;
    uint32  itag_vid;
    uint32  l3_egr_intf_idx;
    rtk_mac_t   nh_dmac_addr;
    rtk_mac_t   nh_smac_addr;
    uint32  itag_tpid_idx;
    uint32  otag_tpid_idx;
    uint32  pe_ecid_ext;
    uint32  pe_ecid_base;

    uint32  ref_l3_hdr_len;
} dal_mango_tunnel_encapEntry_t;

typedef struct dal_mango_tunnel_drvDb_s
{
    /* hardware resource (for internal APIs) */
    struct
    {
        uint32          tunnel_intf_used_cnt;

        /* using mask to indicate the usage of tunnel-intf for speeding up */
        struct
        {
            uint8           ipv6;
            uint8           used_msk;
        } row[DAL_MANGO_TUNNEL_INTF_VMSK_HEIGHT];
    } HW;

    struct
    {
        uint32              valid:1;
        uint32              ip_6rd_en:1;
        uint32              ip_6rd_idx:1;
        uint32              encap_dmac_allocated:1;
        uint32              reserved:28;

        uint32              l3_intf_idx;    /* L3 interface entry index */

        rtk_tunnel_intf_t   intf;
        rtk_l3_pathId_t     path_id;        /* (operational) tunnle interface's nexthop */
        dal_mango_l3_pathInfo_t pathInfo;   /* path information (get from L3, L2 module) */

        /* internal API structure */
        dal_mango_l2_ucastNhAddr_t encap_dmac_l2_entry;
    } tunnel_intf[DAL_MANGO_TUNNEL_INTF_MAX];

    struct
    {
        uint32              valid:1;
        uint32              reserved:31;
    } ip_6rd_domain[DAL_MANGO_TUNNEL_IP_6RD_DOMAIN_MAX];
} dal_mango_tunnel_drvDb_t;

typedef enum dal_mango_tunnel_entry_entryType_e
{
    DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4 = 0,
    DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV6,
} dal_mango_tunnel_entry_entryType_t;

typedef enum dal_mango_tunnel_decap_protoType_e
{
    DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_IPV4 = 0,
    DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_IPV6,
    DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_GRE,
    DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_UDP,
    DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_UDP_LITE,
} dal_mango_tunnel_decap_protoType_t;

typedef enum dal_mango_tunnel_entry_tunnelType_e
{
    DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_CONFIGURED    = 0x0,
    DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_ISATAP        = 0x1,
    DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_6TO4          = 0x2,
    DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_6RD           = 0x3,
    DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_GRE           = 0x4,
    DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_VXLAN         = 0x5,
    DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_VXLAN_GPE     = 0x7,
} dal_mango_tunnel_entry_tunnelType_t;



#if 0
static uint32 _actDecapIp4SipFail[] = {
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };
#endif
static uint32 _actDecapIp6SipFail[] = {
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };
static uint32 _actDecapIsatapSipFail[] = {
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };
static uint32 _actDecap6to4SipFail[] = {
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };
static uint32 _actDecap6to4DipFail[] = {
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };
static uint32 _actDecap6rdDipFail[] = {
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };
static uint32 _actEncapMtuFail[] = {
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };
static uint32 _actEncapTtlFail[] = {
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };
static uint32 _actRouteToTunnel[] = {
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };


/*
 * Data Declaration
 */
static uint32               tunnel_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         tunnel_sem[RTK_MAX_NUM_OF_UNIT];
static osal_mutex_t         tunnel_int_sem[RTK_MAX_NUM_OF_UNIT];
static dal_mango_tunnel_drvDb_t     *_pTunnelDb[RTK_MAX_NUM_OF_UNIT] = { NULL };

/*
 * Macro Declaration
 */
/* semaphore handling */
#define TUNNEL_SEM_LOCK(unit)                                                               \
    do {                                                                                    \
        if (osal_sem_mutex_take(tunnel_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)      \
        {                                                                                   \
            RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_TUNNEL), "semaphore lock failed");  \
            return RT_ERR_SEM_LOCK_FAILED;                                                  \
        }                                                                                   \
    } while(0)
#define TUNNEL_SEM_UNLOCK(unit)                                                                 \
    do {                                                                                        \
        if (osal_sem_mutex_give(tunnel_sem[unit]) != RT_ERR_OK)                                 \
        {                                                                                       \
            RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_TUNNEL), "semaphore unlock failed");  \
            return RT_ERR_SEM_UNLOCK_FAILED;                                                    \
        }                                                                                       \
    } while(0)
#define TUNNEL_INT_SEM_LOCK(unit)                                                                   \
    do {                                                                                            \
        if (osal_sem_mutex_take(tunnel_int_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)          \
        {                                                                                           \
            RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_TUNNEL), "internal semaphore lock failed"); \
            return RT_ERR_SEM_LOCK_FAILED;                                                          \
        }                                                                                           \
    } while(0)
#define TUNNEL_INT_SEM_UNLOCK(unit)                                                                     \
    do {                                                                                                \
        if (osal_sem_mutex_give(tunnel_int_sem[unit]) != RT_ERR_OK)                                     \
        {                                                                                               \
            RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_TUNNEL), "internal semaphore unlock failed"); \
            return RT_ERR_SEM_UNLOCK_FAILED;                                                            \
        }                                                                                               \
    } while(0)
#define TUNNEL_REG_FIELD_READ_ERR_HDL(_unit, _reg, _field, _val, _errMsg, _gotoErrLbl, _ret)    \
    do {                                                                                        \
        if ((_ret = reg_field_read(_unit, _reg, _field, &_val)) != RT_ERR_OK)                   \
        {                                                                                       \
            RT_ERR(_ret, (MOD_DAL|MOD_TUNNEL), _errMsg);                                        \
            goto _gotoErrLbl;                                                                   \
        }                                                                                       \
    } while(0)
#define TUNNEL_REG_FIELD_WRITE_ERR_HDL(_unit, _reg, _field, _val, _errMsg, _gotoErrLbl, _ret)   \
    do {                                                                                        \
        if ((_ret = reg_field_write(_unit, _reg, _field, &_val)) != RT_ERR_OK)                  \
        {                                                                                       \
            RT_ERR(_ret, (MOD_DAL|MOD_TUNNEL), _errMsg);                                        \
            goto _gotoErrLbl;                                                                   \
        }                                                                                       \
    } while(0)
#define TUNNEL_REG_ARRAY_FIELD_READ_ERR_HDL(_unit, _reg, _idx1, _idx2, _field, _val, _errMsg, _gotoErrLbl, _ret)    \
    do {                                                                                                            \
        if ((_ret = reg_array_field_read(_unit, _reg, _idx1, _idx2, _field, &_val)) != RT_ERR_OK)                   \
        {                                                                                                           \
            RT_ERR(_ret, (MOD_DAL|MOD_TUNNEL), _errMsg);                                                            \
            goto _gotoErrLbl;                                                                                       \
        }                                                                                                           \
    } while(0)
#define TUNNEL_REG_ARRAY_FIELD_WRITE_ERR_HDL(_unit, _reg, _idx1, _idx2, _field, _val, _errMsg, _gotoErrLbl, _ret)   \
    do {                                                                                                            \
        if ((_ret = reg_array_field_write(_unit, _reg, _idx1, _idx2, _field, &_val)) != RT_ERR_OK)                  \
        {                                                                                                           \
            RT_ERR(_ret, (MOD_DAL|MOD_TUNNEL), _errMsg);                                                            \
            goto _gotoErrLbl;                                                                                       \
        }                                                                                                           \
    } while(0)
#define TUNNEL_TABLE_READ_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret)    \
    do {                                                                                    \
        if ((_ret = table_read(_unit, _tbl, _idx, (uint32 *)&_entry)) != RT_ERR_OK)         \
        {                                                                                   \
            RT_ERR(_ret, (MOD_DAL|MOD_TUNNEL), _errMsg);                                    \
            goto _gotoErrLbl;                                                               \
        }                                                                                   \
    } while(0)
#define TUNNEL_TABLE_WRITE_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret)   \
    do {                                                                                    \
        if ((_ret = table_write(_unit, _tbl, _idx, (uint32 *)&_entry)) != RT_ERR_OK)        \
        {                                                                                   \
            RT_ERR(_ret, (MOD_DAL|MOD_TUNNEL), _errMsg);                                    \
            goto _gotoErrLbl;                                                               \
        }                                                                                   \
    } while(0)
#define TUNNEL_TABLE_FIELD_GET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret)       \
    do {                                                                                                    \
        if ((_ret = table_field_get(_unit, _tbl, _field, (uint32 *)&_val, (uint32 *)&_entry)) != RT_ERR_OK) \
        {                                                                                                   \
            RT_ERR(_ret, (MOD_DAL|MOD_TUNNEL), _errMsg);                                                    \
            goto _gotoErrLbl;                                                                               \
        }                                                                                                   \
    } while(0)
#define TUNNEL_TABLE_FIELD_SET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret)       \
    do {                                                                                                    \
        if ((_ret = table_field_set(_unit, _tbl, _field, (uint32 *)&_val, (uint32 *)&_entry)) != RT_ERR_OK) \
        {                                                                                                   \
            RT_ERR(_ret, (MOD_DAL|MOD_TUNNEL), _errMsg);                                                    \
            goto _gotoErrLbl;                                                                               \
        }                                                                                                   \
    } while(0)
#define TUNNEL_TABLE_FIELD_MAC_GET_ERR_HDL(_unit, _tbl, _field, _mac, _entry, _errMsg, _gotoErrLbl, _ret)       \
    do {                                                                                                        \
        if ((_ret = table_field_mac_get(_unit, _tbl, _field, (uint8 *)&_mac, (uint32 *)&_entry)) != RT_ERR_OK)  \
        {                                                                                                       \
            RT_ERR(ret, (MOD_DAL|MOD_TUNNEL), _errMsg);                                                         \
            goto _gotoErrLbl;                                                                                   \
        }                                                                                                       \
    } while(0)
#define TUNNEL_TABLE_FIELD_MAC_SET_ERR_HDL(_unit, _tbl, _field, _mac, _entry, _errMsg, _gotoErrLbl, _ret)       \
    do {                                                                                                        \
        if ((_ret = table_field_mac_set(_unit, _tbl, _field, (uint8 *)&_mac, (uint32 *)&_entry)) != RT_ERR_OK)  \
        {                                                                                                       \
            RT_ERR(ret, (MOD_DAL|MOD_TUNNEL), _errMsg);                                                         \
            goto _gotoErrLbl;                                                                                   \
        }                                                                                                       \
    } while(0)
#define TUNNEL_ACTION_TO_VALUE(_actArray, _value, _action, _errMsg, _errHandle, _retval)    \
    do {                                                                                    \
        if ((_retval = RT_UTIL_ACTLIST_INDEX_GET(_actArray, _value, _action)) != RT_ERR_OK) \
        {                                                                                   \
            RT_ERR(_retval, (MOD_DAL|MOD_TUNNEL), _errMsg);                                 \
            goto _errHandle;                                                                \
        }                                                                                   \
    } while(0)
#define TUNNEL_VALUE_TO_ACTION(_actArray, _action, _value, _errMsg, _errHandle, _retval)        \
    do {                                                                                        \
        if ((_retval = RT_UTIL_ACTLIST_ACTION_GET(_actArray, _action, _value)) != RT_ERR_OK)    \
        {                                                                                       \
            RT_ERR(_retval, (MOD_DAL|MOD_TUNNEL), _errMsg);                                     \
            goto _errHandle;                                                                    \
        }                                                                                       \
    } while(0)
#define TUNNEL_RT_ERR_HDL_DBG(_op, _args...)                    \
    do {                                                        \
        if (RT_ERR_OK != (_op))                                 \
        {                                                       \
           RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), ## _args);   \
        }                                                       \
    } while(0)


/*
 * Function Declaration
 */

static int32 _dal_mango_tunnel_tunnel6rdEntry_alloc(uint32 unit, uint32 tunnelIntfIdx)
{
    uint32  index;

    TUNNEL_INT_SEM_LOCK(unit);

    if (FALSE == _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].valid)
    {
        TUNNEL_INT_SEM_UNLOCK(unit);
        return RT_ERR_ENTRY_NOTFOUND;
    }

    for (index=0; index<DAL_MANGO_TUNNEL_IP_6RD_DOMAIN_MAX; index++)
    {
        if (FALSE == _pTunnelDb[unit]->ip_6rd_domain[index].valid)
        {
            _pTunnelDb[unit]->ip_6rd_domain[index].valid = TRUE;
            _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].ip_6rd_en = TRUE;
            _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].ip_6rd_idx = index;

            TUNNEL_INT_SEM_UNLOCK(unit);
            return RT_ERR_OK;
        }
    }

    TUNNEL_INT_SEM_UNLOCK(unit);

    return RT_ERR_TBL_FULL;
}

static int32 _dal_mango_tunnel_tunnel6rdEntry_free(uint32 unit, uint32 tunnelIntfIdx)
{
    uint32 ip_6rd_idx;

    TUNNEL_INT_SEM_LOCK(unit);

    if (FALSE == _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].valid)
    {
        TUNNEL_INT_SEM_UNLOCK(unit);
        return RT_ERR_ENTRY_NOTFOUND;
    }

    if (TRUE != _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].ip_6rd_en)
    {
        TUNNEL_INT_SEM_UNLOCK(unit);
        return RT_ERR_INPUT;
    }

    ip_6rd_idx = _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].ip_6rd_idx;

    if (TRUE == _pTunnelDb[unit]->ip_6rd_domain[ip_6rd_idx].valid)
    {
        _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].ip_6rd_idx = 0;
        _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].ip_6rd_en = FALSE;
        _pTunnelDb[unit]->ip_6rd_domain[ip_6rd_idx].valid = FALSE;

        TUNNEL_INT_SEM_UNLOCK(unit);
        return RT_ERR_OK;
    }

    TUNNEL_INT_SEM_UNLOCK(unit);

    return RT_ERR_ENTRY_NOTFOUND;
}


static int32 _dal_mango_tunnel_tunnelIntfEntry_alloc(uint32 unit, uint32 width, uint32 *pIndex)
{
    int32 i, j;
    int32 index;

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    /* function body */
    TUNNEL_INT_SEM_LOCK(unit);

    if (1 == width)
    {
        /* IPv4 */
        for (i=0; i<DAL_MANGO_TUNNEL_INTF_VMSK_HEIGHT; i++)
        {
            if (_pTunnelDb[unit]->HW.row[i].used_msk < DAL_MANGO_TUNNEL_INTF_VMSK_FULL_MASK)
            {
                for (j=0; j<DAL_MANGO_TUNNEL_INTF_VMSK_WIDTH; j++)
                {
                    if (_pTunnelDb[unit]->HW.row[i].used_msk & (1 << j))
                    {
                        continue;
                    }
                    else
                    {
                        _pTunnelDb[unit]->HW.row[i].used_msk |= (1 << j);

                        index = (i * DAL_MANGO_TUNNEL_INTF_VMSK_WIDTH) + j;
                        _pTunnelDb[unit]->HW.tunnel_intf_used_cnt += width;
                        _pTunnelDb[unit]->tunnel_intf[index].valid = TRUE;

                        *pIndex = index;

                        MANGO_TUNNEL_DBG_PRINTF(1, "IPv4: tunnel_intf_idx = %d\n", index);

                        TUNNEL_INT_SEM_UNLOCK(unit);
                        return RT_ERR_OK;
                    }
                }
            }
        }
    }
    else if (3 == width)
    {
        /* IPv6 */
        for (i=(DAL_MANGO_TUNNEL_INTF_VMSK_HEIGHT-1); i>=0; i--)
        {
            if (_pTunnelDb[unit]->HW.row[i].used_msk & DAL_MANGO_TUNNEL_INTF_VMSK_FULL_MASK)
            {
                continue;
            }
            else
            {
                /* empty */
                _pTunnelDb[unit]->HW.row[i].used_msk |= DAL_MANGO_TUNNEL_INTF_VMSK_FULL_MASK;

                index = (i * DAL_MANGO_TUNNEL_INTF_VMSK_WIDTH);
                _pTunnelDb[unit]->HW.row[i].ipv6 = TRUE;
                _pTunnelDb[unit]->HW.tunnel_intf_used_cnt += width;
                _pTunnelDb[unit]->tunnel_intf[index].valid = TRUE;

                *pIndex = index;

                MANGO_TUNNEL_DBG_PRINTF(1, "IPv6: tunnel_intf_idx = %d\n", index);

                TUNNEL_INT_SEM_UNLOCK(unit);
                return RT_ERR_OK;
            }
        }
    }
    else
    {
        TUNNEL_INT_SEM_UNLOCK(unit);
        return RT_ERR_INPUT;
    }

    TUNNEL_INT_SEM_UNLOCK(unit);
    return RT_ERR_TBL_FULL;
}

static int32 _dal_mango_tunnel_tunnelIntfEntry_free(uint32 unit, uint32 index)
{
    uint32 i = (index / DAL_MANGO_TUNNEL_INTF_VMSK_WIDTH);
    uint32 j = (index % DAL_MANGO_TUNNEL_INTF_VMSK_WIDTH);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* function body */
    TUNNEL_INT_SEM_LOCK(unit);

    if (FALSE == _pTunnelDb[unit]->tunnel_intf[index].valid)
    {
        TUNNEL_INT_SEM_UNLOCK(unit);
        return RT_ERR_ENTRY_NOTFOUND;
    }

    /* clear the relative resources of the entry */
    _pTunnelDb[unit]->tunnel_intf[index].valid = FALSE;
    if (TRUE == _pTunnelDb[unit]->HW.row[i].ipv6)
    {
        _pTunnelDb[unit]->HW.tunnel_intf_used_cnt -= 3;
        _pTunnelDb[unit]->HW.row[i].ipv6 = FALSE;
        _pTunnelDb[unit]->HW.row[i].used_msk = 0;

        MANGO_TUNNEL_DBG_PRINTF(1, "IPv6: tunnel_intf_idx = %u\n", index);
    }
    else
    {
        _pTunnelDb[unit]->HW.tunnel_intf_used_cnt -= 1;
        _pTunnelDb[unit]->HW.row[i].used_msk &= ~(1 << j);

        MANGO_TUNNEL_DBG_PRINTF(1, "IPv4: tunnel_intf_idx = %u\n", index);
    }

    TUNNEL_INT_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}

int32 _dal_mango_tunnel_tunnelIntfEntry_find(uint32 unit, rtk_intf_id_t intfId, uint32 *pIndex)
{
    int32 index;

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    /* function body */
    TUNNEL_INT_SEM_LOCK(unit);

    for (index=0; index<DAL_MANGO_TUNNEL_INTF_MAX; index++)
    {
        if ((_pTunnelDb[unit]->tunnel_intf[index].valid) &&
            (_pTunnelDb[unit]->tunnel_intf[index].intf.intf_id == intfId))
        {
            /* found the entry */
            *pIndex = index;

            TUNNEL_INT_SEM_UNLOCK(unit);
            return RT_ERR_OK;
        }
    }

    TUNNEL_INT_SEM_UNLOCK(unit);
    return RT_ERR_ENTRY_NOTFOUND;
}


#if 0
/* Function Name:
 *      _dal_mango_tunnel_decapEntry_get
 * Description:
 *      Get the specifed tunnel decap entry.
 * Input:
 *      unit   - unit id
 *      index  - entry index
 *      pEntry - pointer to the entry
 * Output:
 *      pEntry - pointer to interface
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      Must initialize Tunnel module before calling any Tunnel APIs.
 */
static int32
_dal_mango_tunnel_decapEntry_get(uint32 unit, uint32 index, dal_mango_tunnel_decapEntry_t *pEntry)
{
    int32 ret = RT_ERR_OK;
    tunnel_decap_entry_t decapEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d,index=%u,pEntry=%p,flags=0x%08x", unit, index, pEntry);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index > HAL_MAX_NUM_OF_TUNNEL(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */

    /* pre-check */
    if (BITMAP_IS_CLEAR(_pTunnelDb[unit]->HW.tunnel_intf_used, index))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* read from chip */
    TUNNEL_TABLE_READ_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, index, decapEntry, "", errHandle, ret);

    /* load data */
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_VALIDtf, pEntry->valid, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_ENTRY_TYPEtf, pEntry->entry_type, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_INTF_IDtf, pEntry->intf_id, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_SIPtf, pEntry->sip, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_DIPtf, pEntry->dip, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_PROTO_TYPEtf, pEntry->bmsk_proto_type, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_L4_SPORTtf, pEntry->l4_msb_data, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_L4_DPORTtf, pEntry->l4_lsb_data, decapEntry, "", errHandle, ret);

    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_BMSK_VIDtf, pEntry->intf_id, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_BMSK_SIPtf, pEntry->sip, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_BMSK_DIPtf, pEntry->dip, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_BMSK_PROTOCOLtf, pEntry->bmsk_proto_type, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_BMSK_L4_SPORTtf, pEntry->bmsk_l4_msb_data, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_BMSK_L4_DPORTtf, pEntry->bmsk_l4_lsb_data, decapEntry, "", errHandle, ret);

    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_TUNNEL_TYPEtf, pEntry->tunnel_type, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_GRE_INNER_IPV4_ENtf, pEntry->inner_ipv4_en, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_GRE_INNER_IPV6_ENtf, pEntry->inner_ipv6_en, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_USE_TUNNEL_TTLtf, pEntry->use_tunnel_ttl, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_USE_TUNNEL_DSCPtf, pEntry->use_tunnel_dscp, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_KEEP_TUNNEL_DSCPtf, pEntry->keep_tunnel_dscp, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_PRI_SEL_TBL_IDXtf, pEntry->pri_sel_tbl_idx, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_INT_PRItf, pEntry->int_pri, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_TUNNEL_INTF_IDtf, pEntry->tunnel_intf_id, decapEntry, "", errHandle, ret);

    MANGO_TUNNEL_DBG_PRINTF(3, "index = %u (0x%08x)\n", index, index);

errHandle:
errEntryNotFound:

    return ret;
}
#endif

/* Function Name:
 *      _dal_mango_tunnel_decapEntry_set
 * Description:
 *      Set the specifed tunnel decap entry.
 * Input:
 *      unit   - unit id
 *      index  - entry index
 *      pEntry - pointer to the entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      Must initialize Tunnel module before calling any Tunnel APIs.
 */
static int32
_dal_mango_tunnel_decapEntry_set(uint32 unit, uint32 index, dal_mango_tunnel_decapEntry_t *pEntry)
{
    int32 ret = RT_ERR_OK;
    uint32 addr;
    uint32 value[3];
    tunnel_decap_entry_t decapEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d,index=%u,pEntry=%p", unit, index, pEntry);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index > HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    TUNNEL_INT_SEM_LOCK(unit);

    /* pre-check */
    if (FALSE == _pTunnelDb[unit]->tunnel_intf[index].valid)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* prepare data */
    /* entry 0 */
    osal_memset(&decapEntry, 0x00, sizeof(tunnel_decap_entry_t));
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_VALIDtf, pEntry->valid, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_ENTRY_TYPEtf, pEntry->entry_type, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_INTF_IDtf, pEntry->intf_id, decapEntry, "", errHandle, ret);
    if (DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4 == pEntry->entry_type)
    {
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
            MANGO_TUNNEL_TERMINATION_SIPtf, pEntry->sip, decapEntry, "", errHandle, ret);
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
            MANGO_TUNNEL_TERMINATION_DIPtf, pEntry->dip, decapEntry, "", errHandle, ret);
    }
    else
    {
        /* assume it must be IPv6 */
        value[0] = (pEntry->sip6.octet[12] << 24) | \
                   (pEntry->sip6.octet[13] << 16) | \
                   (pEntry->sip6.octet[14] << 8)  | \
                   (pEntry->sip6.octet[15] << 0);
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
            MANGO_TUNNEL_TERMINATION_SIPtf, value, decapEntry, "", errHandle, ret);
        value[0] = (pEntry->dip6.octet[12] << 24) | \
                   (pEntry->dip6.octet[13] << 16) | \
                   (pEntry->dip6.octet[14] << 8)  | \
                   (pEntry->dip6.octet[15] << 0);
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
            MANGO_TUNNEL_TERMINATION_DIPtf, value, decapEntry, "", errHandle, ret);
    }
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_PROTO_TYPEtf, pEntry->proto_type, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_L4_MSB_DATAtf, pEntry->l4_msb_data, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_L4_LSB_DATAtf, pEntry->l4_lsb_data, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_GRE_KEY_EXISTtf, pEntry->gre_key_exist, decapEntry, "", errHandle, ret);

    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_BMSK_INTF_IDtf, pEntry->intf_id, decapEntry, "", errHandle, ret);
    if (DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4 == pEntry->entry_type)
    {
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
            MANGO_TUNNEL_TERMINATION_BMSK_SIPtf, pEntry->bmsk_sip, decapEntry, "", errHandle, ret);
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
            MANGO_TUNNEL_TERMINATION_BMSK_DIPtf, pEntry->bmsk_dip, decapEntry, "", errHandle, ret);
    }
    else
    {
        /* assume it must be IPv6 */
        value[0] = (pEntry->bmsk_sip6.octet[12] << 24) | \
                   (pEntry->bmsk_sip6.octet[13] << 16) | \
                   (pEntry->bmsk_sip6.octet[14] << 8)  | \
                   (pEntry->bmsk_sip6.octet[15] << 0);
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
            MANGO_TUNNEL_TERMINATION_BMSK_SIPtf, value, decapEntry, "", errHandle, ret);
        value[0] = (pEntry->bmsk_dip6.octet[12] << 24) | \
                   (pEntry->bmsk_dip6.octet[13] << 16) | \
                   (pEntry->bmsk_dip6.octet[14] << 8)  | \
                   (pEntry->bmsk_dip6.octet[15] << 0);
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
            MANGO_TUNNEL_TERMINATION_BMSK_DIPtf, value, decapEntry, "", errHandle, ret);
    }
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_BMSK_PROTO_TYPEtf, pEntry->bmsk_proto_type, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_BMSK_L4_MSB_DATAtf, pEntry->bmsk_l4_msb_data, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_BMSK_L4_LSB_DATAtf, pEntry->bmsk_l4_lsb_data, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_BMSK_GRE_KEY_EXISTtf, pEntry->bmsk_gre_key_exist, decapEntry, "", errHandle, ret);

    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_TUNNEL_TYPEtf, pEntry->tunnel_type, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_INNER_IPV4_ENtf, pEntry->inner_ipv4_en, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_INNER_IPV6_ENtf, pEntry->inner_ipv6_en, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_USE_TUNNEL_TTLtf, pEntry->use_tunnel_ttl, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_USE_TUNNEL_DSCPtf, pEntry->use_tunnel_dscp, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_KEEP_TUNNEL_DSCPtf, pEntry->keep_tunnel_dscp, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_PRI_SEL_TBL_IDXtf, pEntry->pri_sel_tbl_idx, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_INT_PRItf, pEntry->int_pri, decapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, \
        MANGO_TUNNEL_TERMINATION_TUNNEL_INTF_IDtf, pEntry->tunnel_intf_id, decapEntry, "", errHandle, ret);

    /* entry 0 - write into chip */
    addr = DAL_MANGO_TUNNEL_ENTRY_IDX_TO_ADDR(index);
    TUNNEL_TABLE_WRITE_ERR_HDL(unit, MANGO_TUNNEL_TERMINATIONt, addr, decapEntry, "", errHandle, ret);

    /* entry 1,2 if need */
    if (DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV6 == pEntry->entry_type)
    {
        /* entry 1 */
        osal_memset(&decapEntry, 0x00, sizeof(tunnel_decap_entry_t));
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATION_1t, \
            MANGO_TUNNEL_TERMINATION_1_VALIDtf, pEntry->valid, decapEntry, "", errHandle, ret);
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATION_1t, \
            MANGO_TUNNEL_TERMINATION_1_ENTRY_TYPEtf, pEntry->entry_type, decapEntry, "", errHandle, ret);
        value[2] = (pEntry->sip6.octet[0] << 24) | \
                   (pEntry->sip6.octet[1] << 16) | \
                   (pEntry->sip6.octet[2] << 8)  | \
                   (pEntry->sip6.octet[3] << 0);
        value[1] = (pEntry->sip6.octet[4] << 24) | \
                   (pEntry->sip6.octet[5] << 16) | \
                   (pEntry->sip6.octet[6] << 8)  | \
                   (pEntry->sip6.octet[7] << 0);
        value[0] = (pEntry->sip6.octet[8] << 24) | \
                   (pEntry->sip6.octet[9] << 16) | \
                   (pEntry->sip6.octet[10] << 8)  | \
                   (pEntry->sip6.octet[11] << 0);
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATION_1t, \
            MANGO_TUNNEL_TERMINATION_1_SIP6tf, value, decapEntry, "", errHandle, ret);

        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATION_1t, \
            MANGO_TUNNEL_TERMINATION_1_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, decapEntry, "", errHandle, ret);
        value[2] = (pEntry->bmsk_sip6.octet[0] << 24) | \
                   (pEntry->bmsk_sip6.octet[1] << 16) | \
                   (pEntry->bmsk_sip6.octet[2] << 8)  | \
                   (pEntry->bmsk_sip6.octet[3] << 0);
        value[1] = (pEntry->bmsk_sip6.octet[4] << 24) | \
                   (pEntry->bmsk_sip6.octet[5] << 16) | \
                   (pEntry->bmsk_sip6.octet[6] << 8)  | \
                   (pEntry->bmsk_sip6.octet[7] << 0);
        value[0] = (pEntry->bmsk_sip6.octet[8] << 24) | \
                   (pEntry->bmsk_sip6.octet[9] << 16) | \
                   (pEntry->bmsk_sip6.octet[10] << 8)  | \
                   (pEntry->bmsk_sip6.octet[11] << 0);
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATION_1t, \
            MANGO_TUNNEL_TERMINATION_1_BMSK_SIP6tf, value, decapEntry, "", errHandle, ret);

        /* entry 1 - write into chip */
        addr = DAL_MANGO_TUNNEL_ENTRY_IDX_TO_ADDR(index + 1);
        TUNNEL_TABLE_WRITE_ERR_HDL(unit, MANGO_TUNNEL_TERMINATION_1t, addr, decapEntry, "", errHandle, ret);

        /* entry 2 */
        osal_memset(&decapEntry, 0x00, sizeof(tunnel_decap_entry_t));
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATION_2t, \
            MANGO_TUNNEL_TERMINATION_2_VALIDtf, pEntry->valid, decapEntry, "", errHandle, ret);
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATION_2t, \
            MANGO_TUNNEL_TERMINATION_2_ENTRY_TYPEtf, pEntry->entry_type, decapEntry, "", errHandle, ret);
        value[2] = (pEntry->dip6.octet[0] << 24) | \
                   (pEntry->dip6.octet[1] << 16) | \
                   (pEntry->dip6.octet[2] << 8)  | \
                   (pEntry->dip6.octet[3] << 0);
        value[1] = (pEntry->dip6.octet[4] << 24) | \
                   (pEntry->dip6.octet[5] << 16) | \
                   (pEntry->dip6.octet[6] << 8)  | \
                   (pEntry->dip6.octet[7] << 0);
        value[0] = (pEntry->dip6.octet[8] << 24) | \
                   (pEntry->dip6.octet[9] << 16) | \
                   (pEntry->dip6.octet[10] << 8)  | \
                   (pEntry->dip6.octet[11] << 0);
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATION_2t, \
            MANGO_TUNNEL_TERMINATION_2_DIP6tf, value, decapEntry, "", errHandle, ret);

        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATION_2t, \
            MANGO_TUNNEL_TERMINATION_2_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, decapEntry, "", errHandle, ret);
        value[2] = (pEntry->bmsk_dip6.octet[0] << 24) | \
                   (pEntry->bmsk_dip6.octet[1] << 16) | \
                   (pEntry->bmsk_dip6.octet[2] << 8)  | \
                   (pEntry->bmsk_dip6.octet[3] << 0);
        value[1] = (pEntry->bmsk_dip6.octet[4] << 24) | \
                   (pEntry->bmsk_dip6.octet[5] << 16) | \
                   (pEntry->bmsk_dip6.octet[6] << 8)  | \
                   (pEntry->bmsk_dip6.octet[7] << 0);
        value[0] = (pEntry->bmsk_dip6.octet[8] << 24) | \
                   (pEntry->bmsk_dip6.octet[9] << 16) | \
                   (pEntry->bmsk_dip6.octet[10] << 8)  | \
                   (pEntry->bmsk_dip6.octet[11] << 0);
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_TERMINATION_2t, \
            MANGO_TUNNEL_TERMINATION_2_BMSK_DIP6tf, value, decapEntry, "", errHandle, ret);

        /* entry 2 - write into chip */
        addr = DAL_MANGO_TUNNEL_ENTRY_IDX_TO_ADDR(index + 2);
        TUNNEL_TABLE_WRITE_ERR_HDL(unit, MANGO_TUNNEL_TERMINATION_2t, addr, decapEntry, "", errHandle, ret);
    }

    /* sync shadow info */
    MANGO_TUNNEL_DBG_PRINTF(3, "index = %u (0x%08x)\n", index, index);

errHandle:
errEntryNotFound:
    TUNNEL_INT_SEM_UNLOCK(unit);

    return ret;
}

#if 0
/* Function Name:
 *      _dal_mango_tunnel_encapEntry_get
 * Description:
 *      Get the specifed tunnel encap entry.
 * Input:
 *      unit   - unit id
 *      index  - entry index
 *      pEntry - pointer to the entry
 * Output:
 *      pEntry - pointer to interface
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      Must initialize Tunnel module before calling any Tunnel APIs.
 */
static int32
_dal_mango_tunnel_encapEntry_get(uint32 unit, uint32 index, dal_mango_tunnel_encapEntry_t *pEntry)
{
    int32 ret = RT_ERR_OK;
    tunnel_encap_entry_t encapEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d,index=%u,pEntry=%p,flags=0x%08x", unit, index, pEntry);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index > HAL_MAX_NUM_OF_TUNNEL(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */

    /* pre-check */
    if (BITMAP_IS_CLEAR(_pTunnelDb[unit]->HW.tunnel_intf_used, index))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* read from chip */
    TUNNEL_TABLE_READ_ERR_HDL(unit, MANGO_TUNNEL_STARTt, index, encapEntry, "", errHandle, ret);

    /* load data */
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_STARTt, \
        MANGO_TUNNEL_START_ENTRY_TYPEtf, pEntry->entry_type, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_STARTt, \
        MANGO_TUNNEL_START_TUNNEL_TYPEtf, pEntry->tunnel_type, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_STARTt, \
        MANGO_TUNNEL_START_SIPtf, pEntry->sip, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_TUNNEL_STARTt, \
        MANGO_TUNNEL_START_DIPtf, pEntry->dip, encapEntry, "", errHandle, ret);

    MANGO_TUNNEL_DBG_PRINTF(3, "index = %u (0x%08x)\n", index, index);

errHandle:
errEntryNotFound:

    return ret;
}
#endif

/* Function Name:
 *      _dal_mango_tunnel_encapEntry_set
 * Description:
 *      Set the specifed tunnel encap entry.
 * Input:
 *      unit   - unit id
 *      index  - entry index
 *      pEntry - pointer to the entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      Must initialize Tunnel module before calling any Tunnel APIs.
 */
static int32
_dal_mango_tunnel_encapEntry_set(uint32 unit, uint32 index, dal_mango_tunnel_encapEntry_t *pEntry)
{
    int32 ret = RT_ERR_OK;
    uint32 addr;
    uint32 value[3];
    tunnel_encap_entry_t encapEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d,index=%u,pEntry=%p", unit, index, pEntry);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index > HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    TUNNEL_INT_SEM_LOCK(unit);

    /* pre-check */
    if (FALSE == _pTunnelDb[unit]->tunnel_intf[index].valid)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* prepare data */
    /* entry N */
    osal_memset(&encapEntry, 0x00, sizeof(tunnel_encap_entry_t));
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_ENTRY_TYPEtf, pEntry->entry_type, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_TUNNEL_TYPEtf, pEntry->tunnel_type, encapEntry, "", errHandle, ret);
    if (DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4 == pEntry->entry_type)
    {
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
            MANGO_TUNNEL_START_ENCAP_SIPtf, pEntry->sip, encapEntry, "", errHandle, ret);
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
            MANGO_TUNNEL_START_ENCAP_DIPtf, pEntry->dip, encapEntry, "", errHandle, ret);
    }
    else
    {
        /* assume it must be IPv6 */
        value[0] = (pEntry->sip6.octet[12] << 24) | \
                   (pEntry->sip6.octet[13] << 16) | \
                   (pEntry->sip6.octet[14] << 8)  | \
                   (pEntry->sip6.octet[15] << 0);
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
            MANGO_TUNNEL_START_ENCAP_SIPtf, value, encapEntry, "", errHandle, ret);
        value[0] = (pEntry->dip6.octet[12] << 24) | \
                   (pEntry->dip6.octet[13] << 16) | \
                   (pEntry->dip6.octet[14] << 8)  | \
                   (pEntry->dip6.octet[15] << 0);
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
            MANGO_TUNNEL_START_ENCAP_DIPtf, value, encapEntry, "", errHandle, ret);
    }
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_TTL_ASSIGNtf, pEntry->ttl_assign, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_TTLtf, pEntry->ttl, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_TTL_DECtf, pEntry->ttl_dec, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_DONT_FRAG_ASSIGNtf, pEntry->dont_frag_assign, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_DONT_FRAGtf, pEntry->dont_frag, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_L4_MSB_DATAtf, pEntry->l4_msb_data, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_L4_LSB_DATAtf, pEntry->l4_lsb_data, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_QOS_PROFILE_IDXtf, pEntry->qos_profile_idx, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_VXLAN_ITAG_ACTtf, pEntry->vxlan_itag_act, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_VXLAN_OTAG_ACTtf, pEntry->vxlan_otag_act, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_VXLAN_VNItf, pEntry->vxlan_vni, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_GRE_KEY_ADDtf, pEntry->gre_key_add, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_FVID_SELtf, pEntry->fvid_sel, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_DBL_TAG_VIDtf, pEntry->dbl_tag_vid, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_DBL_TAG_STStf, pEntry->dbl_tag_sts, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_ITAG_VIDtf, pEntry->itag_vid, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_L3_EGR_INTF_IDXtf, pEntry->l3_egr_intf_idx, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_MAC_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_NH_DMAC_ADDRtf, pEntry->nh_dmac_addr, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_MAC_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_NH_SMAC_ADDRtf, pEntry->nh_smac_addr, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_ITAG_TPID_IDXtf, pEntry->itag_tpid_idx, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, \
        MANGO_TUNNEL_START_ENCAP_OTAG_TPID_IDXtf, pEntry->otag_tpid_idx, encapEntry, "", errHandle, ret);

    /* entry N - write into chip */
    addr = DAL_MANGO_TUNNEL_ENTRY_IDX_TO_ADDR(index);
    TUNNEL_TABLE_WRITE_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAPt, addr, encapEntry, "", errHandle, ret);

    /* entry N+1,N+2 if need */
    if (DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV6 == pEntry->entry_type)
    {
        /* entry N+1 */
        osal_memset(&encapEntry, 0x00, sizeof(tunnel_encap_entry_t));
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAP_1t, \
            MANGO_TUNNEL_START_ENCAP_1_ENTRY_TYPEtf, pEntry->entry_type, encapEntry, "", errHandle, ret);
        value[2] = (pEntry->sip6.octet[0] << 24) | \
                   (pEntry->sip6.octet[1] << 16) | \
                   (pEntry->sip6.octet[2] << 8)  | \
                   (pEntry->sip6.octet[3] << 0);
        value[1] = (pEntry->sip6.octet[4] << 24) | \
                   (pEntry->sip6.octet[5] << 16) | \
                   (pEntry->sip6.octet[6] << 8)  | \
                   (pEntry->sip6.octet[7] << 0);
        value[0] = (pEntry->sip6.octet[8] << 24) | \
                   (pEntry->sip6.octet[9] << 16) | \
                   (pEntry->sip6.octet[10] << 8)  | \
                   (pEntry->sip6.octet[11] << 0);
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAP_1t, \
            MANGO_TUNNEL_START_ENCAP_1_SIPtf, value, encapEntry, "", errHandle, ret);
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAP_1t, \
            MANGO_TUNNEL_START_ENCAP_1_FLOW_LABELtf, pEntry->flow_label, encapEntry, "", errHandle, ret);

        /* entry N+1 - write into chip */
        addr = DAL_MANGO_TUNNEL_ENTRY_IDX_TO_ADDR(index + 1);
        TUNNEL_TABLE_WRITE_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAP_1t, addr, encapEntry, "", errHandle, ret);

        /* entry N+2 */
        osal_memset(&encapEntry, 0x00, sizeof(tunnel_encap_entry_t));
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAP_2t, \
            MANGO_TUNNEL_START_ENCAP_2_ENTRY_TYPEtf, pEntry->entry_type, encapEntry, "", errHandle, ret);
        value[2] = (pEntry->dip6.octet[0] << 24) | \
                   (pEntry->dip6.octet[1] << 16) | \
                   (pEntry->dip6.octet[2] << 8)  | \
                   (pEntry->dip6.octet[3] << 0);
        value[1] = (pEntry->dip6.octet[4] << 24) | \
                   (pEntry->dip6.octet[5] << 16) | \
                   (pEntry->dip6.octet[6] << 8)  | \
                   (pEntry->dip6.octet[7] << 0);
        value[0] = (pEntry->dip6.octet[8] << 24) | \
                   (pEntry->dip6.octet[9] << 16) | \
                   (pEntry->dip6.octet[10] << 8)  | \
                   (pEntry->dip6.octet[11] << 0);
        TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAP_2t, \
            MANGO_TUNNEL_START_ENCAP_2_DIPtf, value, encapEntry, "", errHandle, ret);

        /* entry N+2 - write into chip */
        addr = DAL_MANGO_TUNNEL_ENTRY_IDX_TO_ADDR(index + 2);
        TUNNEL_TABLE_WRITE_ERR_HDL(unit, MANGO_TUNNEL_START_ENCAP_2t, addr, encapEntry, "", errHandle, ret);
    }

    /* sync to ALE */
    /* entry N */
    osal_memset(&encapEntry, 0x00, sizeof(tunnel_encap_entry_t));
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_STARTt, \
        MANGO_TUNNEL_START_ENTRY_TYPEtf, pEntry->entry_type, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_STARTt, \
        MANGO_TUNNEL_START_TUNNEL_TYPEtf, pEntry->tunnel_type, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_STARTt, \
        MANGO_TUNNEL_START_TTL_ASSIGNtf, pEntry->ttl_assign, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_STARTt, \
        MANGO_TUNNEL_START_TTL_DECtf, pEntry->ttl_dec, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_STARTt, \
        MANGO_TUNNEL_START_NH_INFO_VALIDtf, pEntry->nh_info_valid, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_STARTt, \
        MANGO_TUNNEL_START_NH_DMAC_IDXtf, pEntry->nh_dmac_idx, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_STARTt, \
        MANGO_TUNNEL_START_L3_EGR_INTF_IDXtf, pEntry->l3_egr_intf_idx, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_STARTt, \
        MANGO_TUNNEL_START_PE_ECID_EXTtf, pEntry->pe_ecid_ext, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_STARTt, \
        MANGO_TUNNEL_START_PE_ECID_BASEtf, pEntry->pe_ecid_base, encapEntry, "", errHandle, ret);
    TUNNEL_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_TUNNEL_STARTt, \
        MANGO_TUNNEL_START_REF_L3_HDR_LENtf, pEntry->ref_l3_hdr_len, encapEntry, "", errHandle, ret);

    /* entry N - write into chip */
    addr = DAL_MANGO_TUNNEL_ENTRY_IDX_TO_ADDR(index);
    TUNNEL_TABLE_WRITE_ERR_HDL(unit, MANGO_TUNNEL_STARTt, addr, encapEntry, "", errHandle, ret);

    /* sync shadow info */
    MANGO_TUNNEL_DBG_PRINTF(2, "index = %u (0x%08x)\n", index, index);

errHandle:
errEntryNotFound:
    TUNNEL_INT_SEM_UNLOCK(unit);

    return ret;
}


static int32 _dal_mango_tunnel_intf2intfEntry(
    dal_mango_l3_intfEntry_t *pEntry,
    rtk_tunnel_intf_t *pIntf,
    uint32 tunnelIdx)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIntf), RT_ERR_NULL_POINTER);

    /* memory clearance */
    osal_memset(pEntry, 0x00, sizeof(dal_mango_l3_intfEntry_t));

    /* common fields */
    pEntry->igrIntf.vrf_id          = pIntf->vrf_id;
    pEntry->egrIntf.ipmc_ttl_scope  = pIntf->ttl;
    pEntry->egrIntf.ip6mc_hl_scope  = pIntf->ttl;
    pEntry->egrIntf.tunnel_if       = 1;
    pEntry->egrIntf.tunnel_idx      = DAL_MANGO_TUNNEL_ENTRY_IDX_TO_ADDR(tunnelIdx);

    switch (pIntf->type)
    {
    /* L3 tunnel */
    case RTK_TUNNEL_TYPE_IP_IN_IP:
    case RTK_TUNNEL_TYPE_IP6_IN_IP:
    case RTK_TUNNEL_TYPE_IPANY_IN_IP:
    case RTK_TUNNEL_TYPE_IP_IN_IP6:
    case RTK_TUNNEL_TYPE_IP6_IN_IP6:
    case RTK_TUNNEL_TYPE_IPANY_IN_IP6:
    case RTK_TUNNEL_TYPE_ISATAP:
    case RTK_TUNNEL_TYPE_6TO4:
    case RTK_TUNNEL_TYPE_6RD:
    case RTK_TUNNEL_TYPE_GRE_IP_IN_IP:
    case RTK_TUNNEL_TYPE_GRE_IP6_IN_IP:
    case RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP:
    case RTK_TUNNEL_TYPE_GRE_IP_IN_IP6:
    case RTK_TUNNEL_TYPE_GRE_IP6_IN_IP6:
    case RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP6:
        pEntry->igrIntf.ipuc_route_en = 0x1;
        pEntry->igrIntf.ip6uc_route_en = 0x1;
        pEntry->igrIntf.ipmc_route_en = 0x1;
        pEntry->igrIntf.ip6mc_route_en = 0x1;
        break;

    /* L2 tunnel */
    case RTK_TUNNEL_TYPE_VXLAN_IP:
    case RTK_TUNNEL_TYPE_VXLAN_IP6:
        break;

    default:
        break;
    }

    return RT_ERR_OK;
}


static int32 _dal_mango_tunnel_intf2decapEntry(
    uint32 unit,
    dal_mango_tunnel_decapEntry_t *pEntry,
    rtk_tunnel_intf_t *pIntf,
    uint32 tnlIntfIdx)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIntf), RT_ERR_NULL_POINTER);

    /* memory clearance */
    osal_memset(pEntry, 0x00, sizeof(dal_mango_tunnel_decapEntry_t));
    switch (pIntf->type)
    {
    case RTK_TUNNEL_TYPE_IP_IN_IP:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->sip = pIntf->ip_in_ip.ip.remote;
            pEntry->dip = pIntf->ip_in_ip.ip.local;
            pEntry->proto_type = DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_IPV4;

            pEntry->bmsk_entry_type = 0x1;
            pEntry->bmsk_sip = 0xFFFFFFFFU;
            pEntry->bmsk_dip = 0xFFFFFFFFU;
            pEntry->bmsk_proto_type = 0x7;

            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_CONFIGURED;
            pEntry->inner_ipv4_en = 0x1;

            pEntry->tunnel_intf_id = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].l3_intf_idx;
        }
        break;

    case RTK_TUNNEL_TYPE_IP6_IN_IP:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->sip = pIntf->ip6_in_ip.ip.remote;
            pEntry->dip = pIntf->ip6_in_ip.ip.local;
            pEntry->proto_type = DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_IPV6;

            pEntry->bmsk_entry_type = 0x1;
            pEntry->bmsk_sip = 0xFFFFFFFFU;
            pEntry->bmsk_dip = 0xFFFFFFFFU;
            pEntry->bmsk_proto_type = 0x7;

            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_CONFIGURED;
            pEntry->inner_ipv6_en = 0x1;

            pEntry->tunnel_intf_id = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].l3_intf_idx;
        }
        break;

    case RTK_TUNNEL_TYPE_IPANY_IN_IP:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->sip = pIntf->ipany_in_ip.ip.remote;
            pEntry->dip = pIntf->ipany_in_ip.ip.local;

            pEntry->bmsk_entry_type = 0x1;
            pEntry->bmsk_sip = 0xFFFFFFFFU;
            pEntry->bmsk_dip = 0xFFFFFFFFU;
            pEntry->bmsk_proto_type = 0;    /* Inner: Any */

            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_CONFIGURED;
            pEntry->inner_ipv4_en = 0x1;
            pEntry->inner_ipv6_en = 0x1;

            pEntry->tunnel_intf_id = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].l3_intf_idx;
        }
        break;

    case RTK_TUNNEL_TYPE_IP_IN_IP6:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV6;
            pEntry->sip6 = pIntf->ip_in_ip6.ip6.remote;
            pEntry->dip6 = pIntf->ip_in_ip6.ip6.local;
            pEntry->proto_type = DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_IPV4;

            pEntry->bmsk_entry_type = 0x1;
            osal_memset(&(pEntry->bmsk_sip6), 0xFF, sizeof(rtk_ipv6_addr_t));
            osal_memset(&(pEntry->bmsk_dip6), 0xFF, sizeof(rtk_ipv6_addr_t));
            pEntry->bmsk_proto_type = 0x7;

            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_CONFIGURED;
            pEntry->inner_ipv4_en = 0x1;

            pEntry->tunnel_intf_id = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].l3_intf_idx;
        }
        break;

    case RTK_TUNNEL_TYPE_IP6_IN_IP6:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV6;
            pEntry->sip6 = pIntf->ip6_in_ip6.ip6.remote;
            pEntry->dip6 = pIntf->ip6_in_ip6.ip6.local;
            pEntry->proto_type = DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_IPV6;

            pEntry->bmsk_entry_type = 0x1;
            osal_memset(&(pEntry->bmsk_sip6), 0xFF, sizeof(rtk_ipv6_addr_t));
            osal_memset(&(pEntry->bmsk_dip6), 0xFF, sizeof(rtk_ipv6_addr_t));
            pEntry->bmsk_proto_type = 0x7;

            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_CONFIGURED;
            pEntry->inner_ipv6_en = 0x1;

            pEntry->tunnel_intf_id = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].l3_intf_idx;
        }
        break;

    case RTK_TUNNEL_TYPE_IPANY_IN_IP6:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV6;
            pEntry->sip6 = pIntf->ipany_in_ip6.ip6.remote;
            pEntry->dip6 = pIntf->ipany_in_ip6.ip6.local;

            pEntry->bmsk_entry_type = 0x1;
            osal_memset(&(pEntry->bmsk_sip6), 0xFF, sizeof(rtk_ipv6_addr_t));
            osal_memset(&(pEntry->bmsk_dip6), 0xFF, sizeof(rtk_ipv6_addr_t));
            pEntry->bmsk_proto_type = 0;    /* Inner: Any */

            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_CONFIGURED;
            pEntry->inner_ipv4_en = 0x1;
            pEntry->inner_ipv6_en = 0x1;

            pEntry->tunnel_intf_id = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].l3_intf_idx;
        }
        break;

    case RTK_TUNNEL_TYPE_ISATAP:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->dip = pIntf->isatap.ip.local;
            pEntry->proto_type = DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_IPV6;

            pEntry->bmsk_entry_type = 0x1;
            pEntry->bmsk_dip = 0xFFFFFFFFU;
            pEntry->bmsk_proto_type = 0x7;

            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_ISATAP;

            pEntry->tunnel_intf_id = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].l3_intf_idx;
        }
        break;

    case RTK_TUNNEL_TYPE_6TO4:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->dip = pIntf->ip_6to4.ip.local;
            pEntry->proto_type = DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_IPV6;

            pEntry->bmsk_entry_type = 0x1;
            pEntry->bmsk_dip = 0xFFFFFFFFU;
            pEntry->bmsk_proto_type = 0x7;

            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_6TO4;

            pEntry->tunnel_intf_id = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].l3_intf_idx;
        }
        break;

    case RTK_TUNNEL_TYPE_6RD:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->dip = pIntf->ip_6rd.ip.local;
            pEntry->proto_type = DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_IPV6;

            pEntry->bmsk_entry_type = 0x1;
            pEntry->bmsk_dip = 0xFFFFFFFFU;
            pEntry->bmsk_proto_type = 0x7;

            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_6RD;

            /* 6rd domain configuration */
            pEntry->inner_ipv4_en = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].ip_6rd_idx;

            pEntry->tunnel_intf_id = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].l3_intf_idx;
        }
        break;

    case RTK_TUNNEL_TYPE_GRE_IP_IN_IP:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->sip = pIntf->gre_ip_in_ip.ip.remote;
            pEntry->dip = pIntf->gre_ip_in_ip.ip.local;
            pEntry->proto_type = DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_GRE;
            pEntry->gre_key_exist = (pIntf->gre_ip_in_ip.key_en)? 1 : 0;

            pEntry->bmsk_entry_type = 0x1;
            pEntry->bmsk_sip = 0xFFFFFFFFU;
            pEntry->bmsk_dip = 0xFFFFFFFFU;
            pEntry->bmsk_proto_type = 0x7;
            pEntry->bmsk_gre_key_exist = 0x1;

            if (pEntry->gre_key_exist)
            {
                pEntry->l4_msb_data = (pIntf->gre_ip_in_ip.key >> 16) & 0xFFFF;
                pEntry->l4_lsb_data = (pIntf->gre_ip_in_ip.key >>  0) & 0xFFFF;

                pEntry->bmsk_l4_msb_data = 0xFFFF;
                pEntry->bmsk_l4_lsb_data = 0xFFFF;
            }

            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_GRE;
            pEntry->inner_ipv4_en = 1;

            pEntry->tunnel_intf_id = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].l3_intf_idx;
        }
        break;

    case RTK_TUNNEL_TYPE_GRE_IP6_IN_IP:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->sip = pIntf->gre_ip6_in_ip.ip.remote;
            pEntry->dip = pIntf->gre_ip6_in_ip.ip.local;
            pEntry->proto_type = DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_GRE;
            pEntry->gre_key_exist = (pIntf->gre_ip6_in_ip.key_en)? 1 : 0;

            pEntry->bmsk_entry_type = 0x1;
            pEntry->bmsk_sip = 0xFFFFFFFFU;
            pEntry->bmsk_dip = 0xFFFFFFFFU;
            pEntry->bmsk_proto_type = 0x7;
            pEntry->bmsk_gre_key_exist = 0x1;

            if (pEntry->gre_key_exist)
            {
                pEntry->l4_msb_data = (pIntf->gre_ip6_in_ip.key >> 16) & 0xFFFF;
                pEntry->l4_lsb_data = (pIntf->gre_ip6_in_ip.key >>  0) & 0xFFFF;

                pEntry->bmsk_l4_msb_data = 0xFFFF;
                pEntry->bmsk_l4_lsb_data = 0xFFFF;
            }

            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_GRE;
            pEntry->inner_ipv6_en = 1;

            pEntry->tunnel_intf_id = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].l3_intf_idx;
        }
        break;

    case RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->sip = pIntf->gre_ipany_in_ip.ip.remote;
            pEntry->dip = pIntf->gre_ipany_in_ip.ip.local;
            pEntry->proto_type = DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_GRE;
            pEntry->gre_key_exist = (pIntf->gre_ipany_in_ip.key_en)? 1 : 0;

            pEntry->bmsk_entry_type = 0x1;
            pEntry->bmsk_sip = 0xFFFFFFFFU;
            pEntry->bmsk_dip = 0xFFFFFFFFU;
            pEntry->bmsk_proto_type = 0x7;
            pEntry->bmsk_gre_key_exist = 0x1;

            if (pEntry->gre_key_exist)
            {
                pEntry->l4_msb_data = (pIntf->gre_ipany_in_ip.key >> 16) & 0xFFFF;
                pEntry->l4_lsb_data = (pIntf->gre_ipany_in_ip.key >>  0) & 0xFFFF;

                pEntry->bmsk_l4_msb_data = 0xFFFF;
                pEntry->bmsk_l4_lsb_data = 0xFFFF;
            }

            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_GRE;
            pEntry->inner_ipv4_en = 1;
            pEntry->inner_ipv6_en = 1;

            pEntry->tunnel_intf_id = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].l3_intf_idx;
        }
        break;

    case RTK_TUNNEL_TYPE_GRE_IP_IN_IP6:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV6;
            pEntry->sip6 = pIntf->gre_ip_in_ip6.ip6.remote;
            pEntry->dip6 = pIntf->gre_ip_in_ip6.ip6.local;
            pEntry->proto_type = DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_GRE;
            pEntry->gre_key_exist = (pIntf->gre_ip_in_ip6.key_en)? 1 : 0;

            pEntry->bmsk_entry_type = 0x1;
            osal_memset(&(pEntry->bmsk_sip6), 0xFF, sizeof(rtk_ipv6_addr_t));
            osal_memset(&(pEntry->bmsk_dip6), 0xFF, sizeof(rtk_ipv6_addr_t));
            pEntry->bmsk_proto_type = 0x7;
            pEntry->bmsk_gre_key_exist = 0x1;

            if (pEntry->gre_key_exist)
            {
                pEntry->l4_msb_data = (pIntf->gre_ip_in_ip6.key >> 16) & 0xFFFF;
                pEntry->l4_lsb_data = (pIntf->gre_ip_in_ip6.key >>  0) & 0xFFFF;

                pEntry->bmsk_l4_msb_data = 0xFFFF;
                pEntry->bmsk_l4_lsb_data = 0xFFFF;
            }

            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_GRE;
            pEntry->inner_ipv4_en = 1;

            pEntry->tunnel_intf_id = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].l3_intf_idx;
        }
        break;

    case RTK_TUNNEL_TYPE_GRE_IP6_IN_IP6:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV6;
            pEntry->sip6 = pIntf->gre_ip6_in_ip6.ip6.remote;
            pEntry->dip6 = pIntf->gre_ip6_in_ip6.ip6.local;
            pEntry->proto_type = DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_GRE;
            pEntry->gre_key_exist = (pIntf->gre_ip6_in_ip6.key_en)? 1 : 0;

            pEntry->bmsk_entry_type = 0x1;
            osal_memset(&(pEntry->bmsk_sip6), 0xFF, sizeof(rtk_ipv6_addr_t));
            osal_memset(&(pEntry->bmsk_dip6), 0xFF, sizeof(rtk_ipv6_addr_t));
            pEntry->bmsk_proto_type = 0x7;
            pEntry->bmsk_gre_key_exist = 0x1;

            if (pEntry->gre_key_exist)
            {
                pEntry->l4_msb_data = (pIntf->gre_ip6_in_ip6.key >> 16) & 0xFFFF;
                pEntry->l4_lsb_data = (pIntf->gre_ip6_in_ip6.key >>  0) & 0xFFFF;

                pEntry->bmsk_l4_msb_data = 0xFFFF;
                pEntry->bmsk_l4_lsb_data = 0xFFFF;
            }

            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_GRE;
            pEntry->inner_ipv6_en = 1;

            pEntry->tunnel_intf_id = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].l3_intf_idx;
        }
        break;

    case RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP6:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV6;
            pEntry->sip6 = pIntf->gre_ipany_in_ip6.ip6.remote;
            pEntry->dip6 = pIntf->gre_ipany_in_ip6.ip6.local;
            pEntry->proto_type = DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_GRE;
            pEntry->gre_key_exist = (pIntf->gre_ipany_in_ip6.key_en)? 1 : 0;

            pEntry->bmsk_entry_type = 0x1;
            osal_memset(&(pEntry->bmsk_sip6), 0xFF, sizeof(rtk_ipv6_addr_t));
            osal_memset(&(pEntry->bmsk_dip6), 0xFF, sizeof(rtk_ipv6_addr_t));
            pEntry->bmsk_proto_type = 0x7;
            pEntry->bmsk_gre_key_exist = 0x1;

            if (pEntry->gre_key_exist)
            {
                pEntry->l4_msb_data = (pIntf->gre_ipany_in_ip6.key >> 16) & 0xFFFF;
                pEntry->l4_lsb_data = (pIntf->gre_ipany_in_ip6.key >>  0) & 0xFFFF;

                pEntry->bmsk_l4_msb_data = 0xFFFF;
                pEntry->bmsk_l4_lsb_data = 0xFFFF;
            }

            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_GRE;
            pEntry->inner_ipv4_en = 1;
            pEntry->inner_ipv6_en = 1;

            pEntry->tunnel_intf_id = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].l3_intf_idx;
        }
        break;

    case RTK_TUNNEL_TYPE_VXLAN_IP:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->sip = pIntf->vxlan_ip.ip.remote;
            pEntry->dip = pIntf->vxlan_ip.ip.local;
            pEntry->proto_type = DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_UDP;
            pEntry->l4_lsb_data = pIntf->vxlan_ip.l4.local_port;

            pEntry->bmsk_entry_type = 0x1;
            pEntry->bmsk_sip = 0xFFFFFFFFU;
            pEntry->bmsk_dip = 0xFFFFFFFFU;
            pEntry->bmsk_proto_type = 0x7;
            pEntry->bmsk_l4_lsb_data = 0xFFFF;

            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_VXLAN;

            pEntry->tunnel_intf_id = 0x1;   /* LSB bit[0] - MAC learning bit */
        }
        break;

    case RTK_TUNNEL_TYPE_VXLAN_IP6:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV6;
            pEntry->sip6 = pIntf->vxlan_ip6.ip6.remote;
            pEntry->dip6 = pIntf->vxlan_ip6.ip6.local;
            pEntry->proto_type = DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_UDP;
            pEntry->l4_lsb_data = pIntf->vxlan_ip6.l4.local_port;

            pEntry->bmsk_entry_type = 0x1;
            osal_memset(&(pEntry->bmsk_sip6), 0xFF, sizeof(rtk_ipv6_addr_t));
            osal_memset(&(pEntry->bmsk_dip6), 0xFF, sizeof(rtk_ipv6_addr_t));
            pEntry->bmsk_proto_type = 0x7;
            pEntry->bmsk_l4_lsb_data = 0xFFFF;

            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_VXLAN;

            pEntry->tunnel_intf_id = 0x1;   /* LSB bit[0] - MAC learning bit */
        }
        break;

    case RTK_TUNNEL_TYPE_VXLAN_GPE_IP:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->sip = pIntf->vxlan_gpe_ip.ip.remote;
            pEntry->dip = pIntf->vxlan_gpe_ip.ip.local;
            pEntry->proto_type = DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_UDP;
            pEntry->l4_lsb_data = pIntf->vxlan_gpe_ip.l4.local_port;

            pEntry->bmsk_entry_type = 0x1;
            pEntry->bmsk_sip = 0xFFFFFFFFU;
            pEntry->bmsk_dip = 0xFFFFFFFFU;
            pEntry->bmsk_proto_type = 0x7;
            pEntry->bmsk_l4_lsb_data = 0xFFFF;

            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_VXLAN_GPE;

            pEntry->tunnel_intf_id = 0x1;   /* LSB bit[0] - MAC learning bit */
        }
        break;

    case RTK_TUNNEL_TYPE_VXLAN_GPE_IP6:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV6;
            pEntry->sip6 = pIntf->vxlan_gpe_ip6.ip6.remote;
            pEntry->dip6 = pIntf->vxlan_gpe_ip6.ip6.local;
            pEntry->proto_type = DAL_MANGO_TUNNEL_DECAP_PROTOTYPE_UDP;
            pEntry->l4_lsb_data = pIntf->vxlan_gpe_ip6.l4.local_port;

            pEntry->bmsk_entry_type = 0x1;
            osal_memset(&(pEntry->bmsk_sip6), 0xFF, sizeof(rtk_ipv6_addr_t));
            osal_memset(&(pEntry->bmsk_dip6), 0xFF, sizeof(rtk_ipv6_addr_t));
            pEntry->bmsk_proto_type = 0x7;
            pEntry->bmsk_l4_lsb_data = 0xFFFF;

            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_VXLAN_GPE;

            pEntry->tunnel_intf_id = 0x1;   /* LSB bit[0] - MAC learning bit */
        }
        break;

    default:
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /* common fields */
    pEntry->valid               = (pIntf->flags & RTK_TUNNEL_FLAG_DECAP_DISABLE)? 0 : 1;
    pEntry->intf_id             = pIntf->decap.l3_intf_id;
    pEntry->bmsk_entry_type     = 0x1;
    pEntry->bmsk_intf_id        = (pIntf->decap.l3_intf_id)? 0x3FF : 0;
    pEntry->use_tunnel_ttl      = (pIntf->flags & RTK_TUNNEL_FLAG_DECAP_USE_CARRIER_TTL)? 1 : 0;
    pEntry->use_tunnel_dscp     = (pIntf->flags & RTK_TUNNEL_FLAG_DECAP_USE_CARRIER_DSCP)? 1 : 0;
    pEntry->keep_tunnel_dscp    = (pIntf->flags & RTK_TUNNEL_FLAG_DECAP_KEEP_PASSENGER_DSCP)? 1 : 0;
    pEntry->pri_sel_tbl_idx     = pIntf->decap.priSelGrp_idx;
    pEntry->int_pri             = pIntf->decap.int_pri;

    MANGO_TUNNEL_DBG_PRINTF(1, "TT: pEntry->tunnel_intf_id = %d\n", pEntry->tunnel_intf_id);

    return RT_ERR_OK;
}

static int32 _dal_mango_tunnel_intf2encapEntry(
    uint32 unit,
    dal_mango_tunnel_encapEntry_t *pEntry,
    rtk_tunnel_intf_t *pIntf,
    uint32 tnlIntfIdx)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIntf), RT_ERR_NULL_POINTER);

    /* memory clearance */
    osal_memset(pEntry, 0x00, sizeof(dal_mango_tunnel_encapEntry_t));

    switch (pIntf->type)
    {
    case RTK_TUNNEL_TYPE_IP_IN_IP:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_CONFIGURED;
            pEntry->sip = pIntf->ip_in_ip.ip.local;
            pEntry->dip = pIntf->ip_in_ip.ip.remote;
            pEntry->ref_l3_hdr_len = 20;
        }
        break;

    case RTK_TUNNEL_TYPE_IP6_IN_IP:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_CONFIGURED;
            pEntry->sip = pIntf->ip6_in_ip.ip.local;
            pEntry->dip = pIntf->ip6_in_ip.ip.remote;
            pEntry->ref_l3_hdr_len = 20;
        }
        break;

    case RTK_TUNNEL_TYPE_IPANY_IN_IP:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_CONFIGURED;
            pEntry->sip = pIntf->ipany_in_ip.ip.local;
            pEntry->dip = pIntf->ipany_in_ip.ip.remote;
            pEntry->ref_l3_hdr_len = 20;
        }
        break;

    case RTK_TUNNEL_TYPE_IP_IN_IP6:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV6;
            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_CONFIGURED;
            pEntry->sip6 = pIntf->ip_in_ip6.ip6.local;
            pEntry->dip6 = pIntf->ip_in_ip6.ip6.remote;
            pEntry->ref_l3_hdr_len = 20;
        }
        break;

    case RTK_TUNNEL_TYPE_IP6_IN_IP6:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV6;
            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_CONFIGURED;
            pEntry->sip6 = pIntf->ip6_in_ip6.ip6.local;
            pEntry->dip6 = pIntf->ip6_in_ip6.ip6.remote;
            pEntry->ref_l3_hdr_len = 40;
        }
        break;

    case RTK_TUNNEL_TYPE_IPANY_IN_IP6:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV6;
            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_CONFIGURED;
            pEntry->sip6 = pIntf->ipany_in_ip6.ip6.local;
            pEntry->dip6 = pIntf->ipany_in_ip6.ip6.remote;
            pEntry->ref_l3_hdr_len = 40;
        }
        break;

    case RTK_TUNNEL_TYPE_ISATAP:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_ISATAP;
            pEntry->sip = pIntf->isatap.ip.local;
            pEntry->ref_l3_hdr_len = 20;
        }
        break;

    case RTK_TUNNEL_TYPE_6TO4:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_6TO4;
            pEntry->sip = pIntf->ip_6to4.ip.local;
            pEntry->ref_l3_hdr_len = 20;
        }
        break;

    case RTK_TUNNEL_TYPE_6RD:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_6RD;
            pEntry->sip = pIntf->ip_6rd.ip.local;
            pEntry->dip = pIntf->ip_6rd.ip.remote;
            pEntry->ref_l3_hdr_len = 20;

            /* 6rd domain configuration */
        }
        break;

    case RTK_TUNNEL_TYPE_GRE_IP_IN_IP:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_GRE;
            pEntry->sip = pIntf->gre_ip_in_ip.ip.local;
            pEntry->dip = pIntf->gre_ip_in_ip.ip.remote;
            pEntry->ref_l3_hdr_len = 20;
            pEntry->gre_key_add = (pIntf->gre_ip_in_ip.key_en)? 1 : 0;

            if (pEntry->gre_key_add)
            {
                pEntry->l4_msb_data = (pIntf->gre_ip_in_ip.key >> 16) & 0xFFFF;
                pEntry->l4_lsb_data = (pIntf->gre_ip_in_ip.key >>  0) & 0xFFFF;
                pEntry->ref_l3_hdr_len += 4;
            }
        }
        break;

    case RTK_TUNNEL_TYPE_GRE_IP6_IN_IP:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_GRE;
            pEntry->sip = pIntf->gre_ip6_in_ip.ip.local;
            pEntry->dip = pIntf->gre_ip6_in_ip.ip.remote;
            pEntry->ref_l3_hdr_len = 20;
            pEntry->gre_key_add = (pIntf->gre_ip6_in_ip.key_en)? 1 : 0;

            if (pEntry->gre_key_add)
            {
                pEntry->l4_msb_data = (pIntf->gre_ip6_in_ip.key >> 16) & 0xFFFF;
                pEntry->l4_lsb_data = (pIntf->gre_ip6_in_ip.key >>  0) & 0xFFFF;
                pEntry->ref_l3_hdr_len += 4;
            }
        }
        break;

    case RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_GRE;
            pEntry->sip = pIntf->gre_ipany_in_ip.ip.local;
            pEntry->dip = pIntf->gre_ipany_in_ip.ip.remote;
            pEntry->ref_l3_hdr_len = 20;
            pEntry->gre_key_add = (pIntf->gre_ipany_in_ip.key_en)? 1 : 0;

            if (pEntry->gre_key_add)
            {
                pEntry->l4_msb_data = (pIntf->gre_ipany_in_ip.key >> 16) & 0xFFFF;
                pEntry->l4_lsb_data = (pIntf->gre_ipany_in_ip.key >>  0) & 0xFFFF;
                pEntry->ref_l3_hdr_len += 4;
            }
        }
        break;

    case RTK_TUNNEL_TYPE_GRE_IP_IN_IP6:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV6;
            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_GRE;
            pEntry->sip6 = pIntf->gre_ip_in_ip6.ip6.local;
            pEntry->dip6 = pIntf->gre_ip_in_ip6.ip6.remote;
            pEntry->ref_l3_hdr_len = 40;
            pEntry->gre_key_add = (pIntf->gre_ip_in_ip6.key_en)? 1 : 0;

            if (pEntry->gre_key_add)
            {
                pEntry->l4_msb_data = (pIntf->gre_ip_in_ip6.key >> 16) & 0xFFFF;
                pEntry->l4_lsb_data = (pIntf->gre_ip_in_ip6.key >>  0) & 0xFFFF;
                pEntry->ref_l3_hdr_len += 4;
            }
        }
        break;

    case RTK_TUNNEL_TYPE_GRE_IP6_IN_IP6:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV6;
            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_GRE;
            pEntry->sip6 = pIntf->gre_ip6_in_ip6.ip6.local;
            pEntry->dip6 = pIntf->gre_ip6_in_ip6.ip6.remote;
            pEntry->ref_l3_hdr_len = 40;
            pEntry->gre_key_add = (pIntf->gre_ip6_in_ip6.key_en)? 1 : 0;

            if (pEntry->gre_key_add)
            {
                pEntry->l4_msb_data = (pIntf->gre_ip6_in_ip6.key >> 16) & 0xFFFF;
                pEntry->l4_lsb_data = (pIntf->gre_ip6_in_ip6.key >>  0) & 0xFFFF;
                pEntry->ref_l3_hdr_len += 4;
            }
        }
        break;

    case RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP6:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV6;
            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_GRE;
            pEntry->sip6 = pIntf->gre_ipany_in_ip6.ip6.local;
            pEntry->dip6 = pIntf->gre_ipany_in_ip6.ip6.remote;
            pEntry->ref_l3_hdr_len = 40;
            pEntry->gre_key_add = (pIntf->gre_ipany_in_ip6.key_en)? 1 : 0;

            if (pEntry->gre_key_add)
            {
                pEntry->l4_msb_data = (pIntf->gre_ipany_in_ip6.key >> 16) & 0xFFFF;
                pEntry->l4_lsb_data = (pIntf->gre_ipany_in_ip6.key >>  0) & 0xFFFF;
                pEntry->ref_l3_hdr_len += 4;
            }
        }
        break;

    case RTK_TUNNEL_TYPE_VXLAN_IP:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_VXLAN;
            pEntry->sip = pIntf->vxlan_ip.ip.local;
            pEntry->dip = pIntf->vxlan_ip.ip.remote;
            pEntry->ref_l3_hdr_len = 20;
            pEntry->l4_msb_data = 0;    /* useless (H/W generates the entropy) */
            pEntry->l4_lsb_data = pIntf->vxlan_ip.l4.remote_port;

            pEntry->vxlan_itag_act = pIntf->vxlan_ip.vlan.inner_tag_status;
            pEntry->vxlan_otag_act = pIntf->vxlan_ip.vlan.outer_tag_status;
            pEntry->vxlan_vni = (pIntf->vxlan_ip.vni != 0)? pIntf->vxlan_ip.vni : DAL_MANGO_TUNNEL_VXLAN_HDR_VALUE;
            pEntry->ref_l3_hdr_len += 8;
        }
        break;

    case RTK_TUNNEL_TYPE_VXLAN_IP6:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV6;
            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_VXLAN;
            pEntry->sip6 = pIntf->vxlan_ip6.ip6.local;
            pEntry->dip6 = pIntf->vxlan_ip6.ip6.remote;
            pEntry->ref_l3_hdr_len = 40;
            pEntry->l4_msb_data = 0;    /* useless (H/W generates the entropy) */
            pEntry->l4_lsb_data = pIntf->vxlan_ip6.l4.remote_port;

            pEntry->vxlan_itag_act = pIntf->vxlan_ip.vlan.inner_tag_status;
            pEntry->vxlan_otag_act = pIntf->vxlan_ip.vlan.outer_tag_status;
            pEntry->vxlan_vni = (pIntf->vxlan_ip6.vni != 0)? pIntf->vxlan_ip6.vni : DAL_MANGO_TUNNEL_VXLAN_HDR_VALUE;
            pEntry->ref_l3_hdr_len += 8;
        }
        break;

    case RTK_TUNNEL_TYPE_VXLAN_GPE_IP:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV4;
            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_VXLAN_GPE;
            pEntry->sip = pIntf->vxlan_gpe_ip.ip.local;
            pEntry->dip = pIntf->vxlan_gpe_ip.ip.remote;
            pEntry->ref_l3_hdr_len = 20;
            pEntry->l4_msb_data = 0;    /* useless (H/W generates the entropy) */
            pEntry->l4_lsb_data = pIntf->vxlan_gpe_ip.l4.remote_port;

            pEntry->vxlan_itag_act = pIntf->vxlan_gpe_ip.vlan.inner_tag_status;
            pEntry->vxlan_otag_act = pIntf->vxlan_gpe_ip.vlan.outer_tag_status;
            pEntry->vxlan_vni = (pIntf->vxlan_gpe_ip.vni != 0)? pIntf->vxlan_gpe_ip.vni : DAL_MANGO_TUNNEL_VXLAN_GPE_HDR_VALUE;
            pEntry->ref_l3_hdr_len += 8;
        }
        break;

    case RTK_TUNNEL_TYPE_VXLAN_GPE_IP6:
        {
            pEntry->entry_type = DAL_MANGO_TUNNEL_ENTRY_ENTRYTYPE_IPV6;
            pEntry->tunnel_type = DAL_MANGO_TUNNEL_ENTRY_TUNNELTYPE_VXLAN_GPE;
            pEntry->sip6 = pIntf->vxlan_gpe_ip6.ip6.local;
            pEntry->dip6 = pIntf->vxlan_gpe_ip6.ip6.remote;
            pEntry->ref_l3_hdr_len = 40;
            pEntry->l4_msb_data = 0;    /* useless (H/W generates the entropy) */
            pEntry->l4_lsb_data = pIntf->vxlan_gpe_ip6.l4.remote_port;

            pEntry->vxlan_itag_act = pIntf->vxlan_gpe_ip6.vlan.inner_tag_status;
            pEntry->vxlan_otag_act = pIntf->vxlan_gpe_ip6.vlan.outer_tag_status;
            pEntry->vxlan_vni = (pIntf->vxlan_gpe_ip6.vni != 0)? pIntf->vxlan_gpe_ip6.vni : DAL_MANGO_TUNNEL_VXLAN_GPE_HDR_VALUE;
            pEntry->ref_l3_hdr_len += 8;
        }
        break;

    default:
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /* common fields */
    pEntry->flow_label          = pIntf->encap.flow_label;
    pEntry->ttl_assign          = (pIntf->flags & RTK_TUNNEL_FLAG_ENCAP_TTL_ASSIGN)? 1 : 0;
    pEntry->ttl                 = pIntf->encap.ttl;
    pEntry->ttl_dec             = (pIntf->flags & RTK_TUNNEL_FLAG_ENCAP_TTL_DEC_IGNORE)? 0 : 1;
    pEntry->dont_frag_assign    = (pIntf->flags & RTK_TUNNEL_FLAG_ENCAP_DONT_FRAG_INHERIT)? 0 : 1;
    pEntry->dont_frag           = (pIntf->encap.dontFrag_en)? 1 : 0;
    pEntry->qos_profile_idx     = pIntf->encap.qosProfile_idx;
    pEntry->fvid_sel            = pIntf->encap.fvid_select;
    pEntry->dbl_tag_vid         = pIntf->encap.dbl_tag_vid;
    pEntry->nh_info_valid       = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].pathInfo.nh_dmac_valid;
    pEntry->nh_dmac_idx         = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].pathInfo.nh_dmac_idx;
    pEntry->l3_egr_intf_idx     = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].pathInfo.l3_intf_idx;
    pEntry->nh_dmac_addr        = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].pathInfo.nh_dmac_addr;
    pEntry->nh_smac_addr        = _pTunnelDb[unit]->tunnel_intf[tnlIntfIdx].pathInfo.l3_intf_mac_addr;
    pEntry->itag_tpid_idx       = pIntf->encap.vlan_inner_tag_tpid_idx;
    pEntry->otag_tpid_idx       = pIntf->encap.vlan_outer_tag_tpid_idx;
    pEntry->pe_ecid_ext         = (pIntf->encap.ecid & (0xFF << 12)) >> 12;
    pEntry->pe_ecid_base        = (pIntf->encap.ecid & (0xFFF << 0)) >> 0;

    return RT_ERR_OK;
}

#if 0
static int32 tunnel_util_vlanMacFid_get(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMacAddr, rtk_fid_t *pFid)
{
    int32           ret = RT_ERR_OK;
    int32           boolMcast;
    int32           boolIVL;
    vlan_entry_t    vlan_entry;
    uint32          temp_var;
    uint32          field;

    /* parameter check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pMacAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pFid), RT_ERR_NULL_POINTER);

    /* is mulcaist address ? */
    boolMcast = (pMacAddr->octet[0] & 0x01)? TRUE : FALSE;

    osal_memset(&vlan_entry, 0x00, sizeof(vlan_entry));

    /*** get VLAN entry from chip ***/
    if ((ret = table_read(unit, MANGO_VLANt, vid, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_TUNNEL|MOD_DAL), "");
        return ret;
    }

    if (FALSE == boolMcast)
    {
        /* get unicast lookup mode */
        if ((ret = table_field_get(unit, MANGO_VLANt, MANGO_VLAN_L2_HKEY_UCASTtf, \
                                   &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_TUNNEL|MOD_DAL), "");
            return ret;
        }

        boolIVL = (0 == temp_var)? TRUE : FALSE;
    }
    else
    {
        /* get multicast lookup mode */
        if ((ret = table_field_get(unit, MANGO_VLANt, MANGO_VLAN_L2_HKEY_MCASTtf, \
                                   &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_TUNNEL|MOD_DAL), "");
            return ret;
        }

        boolIVL = (0 == temp_var)? TRUE : FALSE;
    }

    if (TRUE == boolIVL)
    {
        /* IVL mode, then use the VLAN ID */
        *pFid = vid;
    }
    else
    {
        /* get FID source configuration */
        if ((ret = reg_field_read(unit, MANGO_VLAN_CTRLr, MANGO_SVL_FID_SRCf, \
                                  &temp_var) ) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_TUNNEL|MOD_DAL), "");
            return ret;
        }

        if (0 == temp_var)
        {
            /* MAC mode */
            field = (FALSE == boolMcast)? MANGO_UC_SVL_FIDf : MANGO_MC_SVL_FIDf;

            /* get FID from CHIP */
            if ((ret = reg_field_read(unit, MANGO_VLAN_CTRLr, field, pFid) ) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_TUNNEL|MOD_DAL), "");
                return ret;
            }
        }
        else
        {
            /* VLAN mode */

            /* get FID from entry */
            if ((ret = table_field_get(unit, MANGO_VLANt, MANGO_VLAN_L2_TNL_LST_IDXtf, \
                                       &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_TUNNEL|MOD_DAL), "");
                return ret;
            }

            *pFid = temp_var;
        }
    }

    return ret;
}
#endif

/* Function Name:
 *      dal_mango_tunnelMapper_init
 * Description:
 *      Hook tunnel module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook tunnel module before calling any tunnel APIs.
 */
int32
dal_mango_tunnelMapper_init(dal_mapper_t *pMapper)
{
    pMapper->tunnel_init = dal_mango_tunnel_init;
    pMapper->tunnel_info_get = dal_mango_tunnel_info_get;
    pMapper->tunnel_intf_create = dal_mango_tunnel_intf_create;
    pMapper->tunnel_intf_destroy = dal_mango_tunnel_intf_destroy;
    pMapper->tunnel_intf_destroyAll = dal_mango_tunnel_intf_destroyAll;
    pMapper->tunnel_intf_get = dal_mango_tunnel_intf_get;
    pMapper->tunnel_intf_set = dal_mango_tunnel_intf_set;
    pMapper->tunnel_intfPathId_get = dal_mango_tunnel_intfPathId_get;
    pMapper->tunnel_intfPathId_set = dal_mango_tunnel_intfPathId_set;
    pMapper->tunnel_intfPath_get = dal_mango_tunnel_intfPath_get;
    pMapper->tunnel_intfPath_set = dal_mango_tunnel_intfPath_set;
    pMapper->tunnel_intfStats_get = dal_mango_tunnel_intfStats_get;
    pMapper->tunnel_intfStats_reset = dal_mango_tunnel_intfStats_reset;
    pMapper->tunnel_qosProfile_get = dal_mango_tunnel_qosProfile_get;
    pMapper->tunnel_qosProfile_set = dal_mango_tunnel_qosProfile_set;
    pMapper->tunnel_globalCtrl_get = dal_mango_tunnel_globalCtrl_get;
    pMapper->tunnel_globalCtrl_set = dal_mango_tunnel_globalCtrl_set;

    pMapper->capwap_udpPort_get = dal_mango_capwap_udpPort_get;
    pMapper->capwap_udpPort_set = dal_mango_capwap_udpPort_set;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_tunnel_init
 * Description:
 *      Initialize Tunnel module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Must initialize Tunnel module before calling any tunneling APIs.
 */
int32
dal_mango_tunnel_init(uint32 unit)
{
    uint32 value;

    /* parameter check */
    RT_PARAM_CHK(unit >= RTK_MAX_NUM_OF_UNIT, RT_ERR_OUT_OF_RANGE);

    RT_INIT_REENTRY_CHK(tunnel_init[unit]);
    tunnel_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    tunnel_sem[unit] = osal_sem_mutex_create();
    if (0 == tunnel_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_TUNNEL), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* create internal semaphore */
    tunnel_int_sem[unit] = osal_sem_mutex_create();
    if (0 == tunnel_int_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_TUNNEL), "internal semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* allocate memory that we need */
    if ((_pTunnelDb[unit] = osal_alloc(sizeof(dal_mango_tunnel_drvDb_t))) == NULL)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_TUNNEL), "out of memory");
        return RT_ERR_FAILED;
    }
    osal_memset(_pTunnelDb[unit], 0x00, sizeof(dal_mango_tunnel_drvDb_t));

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        /* enable tunnel-related TCAM blocks */
        value = 0x1;    /* enable */
        reg_field_write(unit, MANGO_ALE_L3_MISC_CTRLr, MANGO_TT_TCAM_ENf, &value);
    }

    /* set init flag to complete init */
    tunnel_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}   /* end of dal_mango_tunnel_init */

/* Function Name:
 *      _dal_mango_tunnel_intf2tunnelIdx
 * Description:
 *      Get the tunnel index of an interface ID.
 * Input:
 *      unit   - unit id
 *      pIndex - pointer to the tunnel idx
 *      intfId - interface id
 * Output:
 *      pIndex - pointer to the tunnel idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NULL_POINTER   - null pointer
 *      RT_ERR_ENTRY_NOTFOUND - entry is not found
 * Note:
 *      Must initialize Tunnel module before calling any Tunnel APIs.
 */
int32
_dal_mango_tunnel_intf2tunnelIdx(uint32 unit, uint32 *pIndex, rtk_intf_id_t intfId)
{
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    /* function body */
    MANGO_TUNNEL_DBG_PRINTF(1, "pIntf->intf_id = 0x%08X\n", intfId);

    if (DAL_MANGO_L3_INTF_ID_IS_L2_TUNNEL(intfId))
    {
        *pIndex = DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId);
    }
    else
    {
        RT_ERR_HDL(_dal_mango_tunnel_tunnelIntfEntry_find(unit, intfId, pIndex), errIntfFind, ret);
    }

    goto errOk;

errIntfFind:
errOk:

    return ret;
}

/* Function Name:
 *      _dal_mango_tunnel_tunnelIdx2intf
 * Description:
 *      Get the interface ID of a tunnel index.
 * Input:
 *      unit      - unit id
 *      pIntfId   - pointer to the interface id
 *      tunnelIdx - tunnel idx
 * Output:
 *      pIntfId   - pointer to the tunnel idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NULL_POINTER   - null pointer
 *      RT_ERR_ENTRY_NOTFOUND - entry is not found
 * Note:
 *      Must initialize Tunnel module before calling any Tunnel APIs.
 */
int32
_dal_mango_tunnel_tunnelIdx2intf(uint32 unit, rtk_intf_id_t *pIntfId, uint32 tunnelIdx)
{
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIntfId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((tunnelIdx >= DAL_MANGO_TUNNEL_INTF_MAX), RT_ERR_INPUT);

    /* function body */
    TUNNEL_INT_SEM_LOCK(unit);

    if (_pTunnelDb[unit]->tunnel_intf[tunnelIdx].valid)
    {
        /* found the entry */
        *pIntfId = _pTunnelDb[unit]->tunnel_intf[tunnelIdx].intf.intf_id;
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
    }

    TUNNEL_INT_SEM_UNLOCK(unit);

    return ret;
}


/* Module Name    : Tunneling                */
/* Sub-module Name: Tunneling error handling */


/* Function Name:
 *      dal_mango_tunnel_info_get
 * Description:
 *      Get tunnel-related information
 * Input:
 *      unit  - unit id
 *      pInfo - pointer to tunnel information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
int32
dal_mango_tunnel_info_get(uint32 unit, rtk_tunnel_info_t *pInfo)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((NULL == pInfo), RT_ERR_NULL_POINTER);

    /* function body */
    pInfo->tunnel_intf_count = _pTunnelDb[unit]->HW.tunnel_intf_used_cnt;
    pInfo->tunnel_decap_used = _pTunnelDb[unit]->HW.tunnel_intf_used_cnt;  /* For Mango, each tunnel intf ocupies both decap and encap entries */
    pInfo->tunnel_decap_max = HAL_MAX_NUM_OF_TUNNEL(unit);
    pInfo->tunnel_encap_used = _pTunnelDb[unit]->HW.tunnel_intf_used_cnt;  /* For Mango, each tunnel intf ocupies both decap and encap entries */
    pInfo->tunnel_encap_max = HAL_MAX_NUM_OF_TUNNEL(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_tunnel_info_get */

/* Function Name:
 *      dal_mango_tunnel_intf_create
 * Description:
 *      Create a new tunnel interface
 * Input:
 *      unit  - unit id
 *      pIntf - pointer to tunnel interface containing the basic inputs
 * Output:
 *      pIntf - pointer to tunnel interface (including all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - the module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be a null pointer
 *      RT_ERR_TBL_FULL                 - table is full
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - variety of MTU values is over the maximum H/W supports
 * Note:
 *      (1) Basic required input parameters of the pRoute as input:
 *          type and corresponding fields about that tunnel.
 *      (2) RTK_TUNNEL_FLAG_WITH_L3_INTF_ID flag is used to specify L3 interface ID,
 *          it is only available while the created tunnel is an L3 tunnel interface.
 */
int32
dal_mango_tunnel_intf_create(uint32 unit, rtk_tunnel_intf_t *pIntf)
{
    int32   ret = RT_ERR_OK;
    uint32  width, l2Tunnel;
    uint32  tunnelIntfIdx;
    uint32  l3IntfIdx;
    uint32  l3Flags = DAL_MANGO_L3_API_FLAG_MOD_TUNNEL;
    dal_mango_l3_intfEntry_t intfEntry;
    dal_mango_tunnel_decapEntry_t decapEntry;
    dal_mango_tunnel_encapEntry_t encapEntry;
    dal_mango_l3_pathInfo_t *pPathInfo;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((NULL == pIntf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((_pTunnelDb[unit]->HW.tunnel_intf_used_cnt >= HAL_MAX_NUM_OF_TUNNEL(unit)), RT_ERR_TBL_FULL);
    RT_PARAM_CHK((pIntf->vrf_id >= HAL_MAX_NUM_OF_VRF(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((pIntf->ttl > RTK_IP_TTL_MAX), RT_ERR_TTL_EXCEED);

    /* preparation */
    switch (pIntf->type)
    {
    case RTK_TUNNEL_TYPE_IP_IN_IP:          { width = 1; l2Tunnel = FALSE; } break;
    case RTK_TUNNEL_TYPE_IP6_IN_IP:         { width = 1; l2Tunnel = FALSE; } break;
    case RTK_TUNNEL_TYPE_IPANY_IN_IP:       { width = 1; l2Tunnel = FALSE; } break;
    case RTK_TUNNEL_TYPE_ISATAP:            { width = 1; l2Tunnel = FALSE; } break;
    case RTK_TUNNEL_TYPE_6TO4:              { width = 1; l2Tunnel = FALSE; } break;
    case RTK_TUNNEL_TYPE_6RD:               { width = 1; l2Tunnel = FALSE; } break;
    case RTK_TUNNEL_TYPE_GRE_IP_IN_IP:      { width = 1; l2Tunnel = FALSE; } break;
    case RTK_TUNNEL_TYPE_GRE_IP6_IN_IP:     { width = 1; l2Tunnel = FALSE; } break;
    case RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP:   { width = 1; l2Tunnel = FALSE; } break;
    case RTK_TUNNEL_TYPE_VXLAN_IP:          { width = 1; l2Tunnel = TRUE; } break;
    case RTK_TUNNEL_TYPE_VXLAN_GPE_IP:      { width = 1; l2Tunnel = TRUE; } break;
    case RTK_TUNNEL_TYPE_IP_IN_IP6:         { width = 3; l2Tunnel = FALSE; } break;
    case RTK_TUNNEL_TYPE_IP6_IN_IP6:        { width = 3; l2Tunnel = FALSE; } break;
    case RTK_TUNNEL_TYPE_IPANY_IN_IP6:      { width = 3; l2Tunnel = FALSE; } break;
    case RTK_TUNNEL_TYPE_GRE_IP_IN_IP6:     { width = 3; l2Tunnel = FALSE; } break;
    case RTK_TUNNEL_TYPE_GRE_IP6_IN_IP6:    { width = 3; l2Tunnel = FALSE; } break;
    case RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP6:  { width = 3; l2Tunnel = FALSE; } break;
    case RTK_TUNNEL_TYPE_VXLAN_IP6:         { width = 3; l2Tunnel = TRUE; } break;
    case RTK_TUNNEL_TYPE_VXLAN_GPE_IP6:     { width = 3; l2Tunnel = TRUE; } break;
    default:
        return RT_ERR_CHIP_NOT_SUPPORTED;   /* return before semaphore lock */

    }

    /* function body */
    TUNNEL_SEM_LOCK(unit);

    /* try to find a valid tunnel-interface entry */
    RT_ERR_HDL(_dal_mango_tunnel_tunnelIntfEntry_alloc(unit, width, &tunnelIntfIdx), errTunnelAlloc, ret);

    /* try to find a valid 6rd domain entry if need */
    if (RTK_TUNNEL_TYPE_6RD == pIntf->type)
    {
        RT_ERR_HDL(_dal_mango_tunnel_tunnel6rdEntry_alloc(unit, tunnelIntfIdx), errTunnel6rdAlloc, ret);
    }

    if (FALSE == l2Tunnel)
    {
        /* L3 tunnel */
        if (pIntf->flags & RTK_TUNNEL_FLAG_WITH_L3_INTF_ID)
        {
            l3Flags |= DAL_MANGO_L3_API_FLAG_WITH_ID;
            l3IntfIdx = pIntf->intf_id;
        }

        RT_ERR_HDL(_dal_mango_l3_intfEntry_alloc(unit, &l3IntfIdx, l3Flags), errIntfAlloc, ret);

        /* write into the chip here */
        MANGO_TUNNEL_DBG_PRINTF(1, "tunnel interface create (idx = %u)\n", tunnelIntfIdx);
        RT_ERR_HDL(_dal_mango_tunnel_intf2intfEntry(&intfEntry, pIntf, tunnelIntfIdx), errHandle, ret);
        RT_ERR_HDL(_dal_mango_l3_intfEntry_set(unit, l3IntfIdx, &intfEntry, l3Flags), errHandle, ret);
    }

    /* update H/W entry */
    RT_ERR_HDL(_dal_mango_tunnel_intf2decapEntry(unit, &decapEntry, pIntf, tunnelIntfIdx), errHandle, ret);
    RT_ERR_HDL(_dal_mango_tunnel_decapEntry_set(unit, tunnelIntfIdx, &decapEntry), errHandle, ret);

    /* get the nexthop info */
    pPathInfo = &_pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].pathInfo;
    if ((pIntf->encap.path_id != 0) &&
        (pIntf->encap.path_id != _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].path_id))
    {
        if (RT_ERR_OK == _dal_mango_l3_pathInfo_get(unit, pIntf->encap.path_id, pPathInfo))
        {
            /* update path info */
            _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].path_id = pIntf->encap.path_id;
        } else {
            _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].path_id = DAL_MANGO_TUNNEL_INTF_PATH_ID_INVALID;
            osal_memset(pPathInfo, 0x00, sizeof(dal_mango_l3_pathInfo_t));
        }
    }

    RT_ERR_HDL(_dal_mango_tunnel_intf2encapEntry(unit, &encapEntry, pIntf, tunnelIntfIdx), errHandle, ret);
    RT_ERR_HDL(_dal_mango_tunnel_encapEntry_set(unit, tunnelIntfIdx, &encapEntry), errHandle, ret);

    /* update interface ID */
    if (TRUE == l2Tunnel)
    {
        pIntf->intf_id = (tunnelIntfIdx | DAL_MANGO_L3_INTF_ID_FLAG_TUNNEL | DAL_MANGO_L3_INTF_ID_FLAG_L2_TUNNEL);
        MANGO_TUNNEL_DBG_PRINTF(1, "tunnelIntfIdx = 0x%08X, pIntf->intf_id = 0x%08X\n", tunnelIntfIdx, pIntf->intf_id);

        _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].l3_intf_idx = DAL_MANGO_TUNNEL_L3_INTF_IDX_INVALID;
    } else {
        //pIntf->intf_id = (l3IntfIdx | DAL_MANGO_L3_INTF_ID_FLAG_TUNNEL);
        pIntf->intf_id = l3IntfIdx; /* uses L3 interface ID as L3 tunnel interface ID */
        MANGO_TUNNEL_DBG_PRINTF(1, "l3IntfIdx = 0x%08X, pIntf->intf_id = 0x%08X\n", l3IntfIdx, pIntf->intf_id);

        _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].l3_intf_idx = l3IntfIdx;
    }

    /* update database */
    _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].intf = *pIntf;

    goto errOk;

errHandle:
    if (FALSE == l2Tunnel)
    {
        TUNNEL_RT_ERR_HDL_DBG(_dal_mango_l3_intfEntry_free(unit, l3IntfIdx, (DAL_MANGO_L3_API_FLAG_MOD_TUNNEL)), "intf free failed");
    }

errIntfAlloc:
    if (RTK_TUNNEL_TYPE_6RD == pIntf->type)
    {
        TUNNEL_RT_ERR_HDL_DBG(_dal_mango_tunnel_tunnel6rdEntry_free(unit, tunnelIntfIdx), "tunnel 6rd free failed");
    }

errTunnel6rdAlloc:
    TUNNEL_RT_ERR_HDL_DBG(_dal_mango_tunnel_tunnelIntfEntry_free(unit, tunnelIntfIdx), "tunnel intf free failed");

errTunnelAlloc:

errOk:
    TUNNEL_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_tunnel_intf_create */

/* Function Name:
 *      dal_mango_tunnel_intf_destroy
 * Description:
 *      Destroy a tunnel interface
 * Input:
 *      unit   - unit id
 *      intfId - tunnel interface ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_ENTRY_NOTFOUND - entry is not found
 * Note:
 */
int32
dal_mango_tunnel_intf_destroy(uint32 unit, rtk_intf_id_t intfId)
{
    int32   ret = RT_ERR_OK;
    uint32  tunnelIntfIdx;
    uint32  l2Tunnel, l3IntfIdx;
    uint32  l3Flags = DAL_MANGO_L3_API_FLAG_MOD_TUNNEL;
    dal_mango_tunnel_decapEntry_t decapEntry;
    dal_mango_tunnel_encapEntry_t encapEntry;
    dal_mango_l3_intfEntry_t intfEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d,intfId=%d", unit, intfId);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_INTF(unit) <= DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId)), RT_ERR_INPUT);
    //RT_PARAM_CHK((!DAL_MANGO_L3_INTF_ID_IS_TUNNEL(intfId)), RT_ERR_INPUT);

    /* function body */
    TUNNEL_SEM_LOCK(unit);

    l2Tunnel = (DAL_MANGO_L3_INTF_ID_IS_L2_TUNNEL(intfId))? TRUE : FALSE;

    if (TRUE == l2Tunnel)
    {
        tunnelIntfIdx = DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId);
        l3IntfIdx = 0;  /* None */
    } else {
        /* L3 tunnel interface */
        RT_ERR_HDL(_dal_mango_tunnel_tunnelIntfEntry_find(unit, intfId, &tunnelIntfIdx), errIntfFind, ret);
        l3IntfIdx = _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].l3_intf_idx;
    }

    /* clear the relative H/W entries */
    MANGO_TUNNEL_DBG_PRINTF(1, "tunnel interface destroy (idx = %u)\n", tunnelIntfIdx);
    osal_memset(&decapEntry, 0x00, sizeof(dal_mango_tunnel_decapEntry_t));
    osal_memset(&encapEntry, 0x00, sizeof(dal_mango_tunnel_encapEntry_t));
    RT_ERR_HDL(_dal_mango_tunnel_decapEntry_set(unit, tunnelIntfIdx, &decapEntry), errHandle, ret);
    RT_ERR_HDL(_dal_mango_tunnel_encapEntry_set(unit, tunnelIntfIdx, &encapEntry), errHandle, ret);

    /* release 6rd domain entry if need */
    if (TRUE == _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].ip_6rd_en)
    {
        RT_ERR_HDL(_dal_mango_tunnel_tunnel6rdEntry_free(unit, tunnelIntfIdx), errHandle, ret);
    }

    if (FALSE == l2Tunnel)
    {
        /* destroy the tunnel interface */
        osal_memset(&intfEntry, 0x00, sizeof(dal_mango_l3_intfEntry_t));
        RT_ERR_HDL(_dal_mango_l3_intfEntry_set(unit, l3IntfIdx, &intfEntry, l3Flags), errHandle, ret);
        RT_ERR_HDL(_dal_mango_l3_intfEntry_free(unit, l3IntfIdx, l3Flags), errHandle, ret);
    }

    /* update database */
    _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].l3_intf_idx = DAL_MANGO_TUNNEL_L3_INTF_IDX_INVALID;
    _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].path_id = DAL_MANGO_TUNNEL_INTF_PATH_ID_INVALID;
    osal_memset(&_pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].intf, 0x00, sizeof(rtk_tunnel_intf_t));
    osal_memset(&_pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].pathInfo, 0x00, sizeof(dal_mango_l3_pathInfo_t));
    RT_ERR_HDL(_dal_mango_tunnel_tunnelIntfEntry_free(unit, tunnelIntfIdx), errHandle, ret);

    goto errOk;

errHandle:

errIntfFind:

errOk:
    TUNNEL_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_tunnel_intf_destroy */

/* Function Name:
 *      dal_mango_tunnel_intf_destroyAll
 * Description:
 *      Destroy all tunnel interfaces
 * Input:
 *      unit  - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
int32
dal_mango_tunnel_intf_destroyAll(uint32 unit)
{
    int32   ret = RT_ERR_OK;
    uint32  tunnelIntfIdx;
    uint32  l3IntfIdx;
    uint32  l3Flags = DAL_MANGO_L3_API_FLAG_MOD_TUNNEL;
    dal_mango_tunnel_decapEntry_t decapEntry;
    dal_mango_tunnel_encapEntry_t encapEntry;
    dal_mango_l3_intfEntry_t intfEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID), RT_ERR_CHIP_NOT_SUPPORTED);

    /* function body */
    TUNNEL_SEM_LOCK(unit);

    /* preparation */
    osal_memset(&decapEntry, 0x00, sizeof(dal_mango_tunnel_decapEntry_t));
    osal_memset(&encapEntry, 0x00, sizeof(dal_mango_tunnel_encapEntry_t));
    osal_memset(&intfEntry, 0x00, sizeof(dal_mango_l3_intfEntry_t));

    /* destroy all tunnel interface */
    for (tunnelIntfIdx=0; tunnelIntfIdx<DAL_MANGO_TUNNEL_INTF_MAX; tunnelIntfIdx++)
    {
        if (_pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].valid)
        {
            l3IntfIdx = _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].l3_intf_idx;

            /* release the relative resources of the entry */
            MANGO_TUNNEL_DBG_PRINTF(1, "tunnel interface destroy (idx = %u)\n", tunnelIntfIdx);
            RT_ERR_HDL(_dal_mango_tunnel_decapEntry_set(unit, tunnelIntfIdx, &decapEntry), errHandle, ret);
            RT_ERR_HDL(_dal_mango_tunnel_encapEntry_set(unit, tunnelIntfIdx, &encapEntry), errHandle, ret);
            RT_ERR_HDL(_dal_mango_l3_intfEntry_set(unit, l3IntfIdx, &intfEntry, l3Flags), errHandle, ret);

            /* destroy the tunnel interface */
            RT_ERR_HDL(_dal_mango_l3_intfEntry_free(unit, l3IntfIdx, l3Flags), errHandle, ret);

            /* release 6rd domain entry if need */
            if (TRUE == _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].ip_6rd_en)
            {
                RT_ERR_HDL(_dal_mango_tunnel_tunnel6rdEntry_free(unit, tunnelIntfIdx), errHandle, ret);
            }

            /* update database */
            _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].l3_intf_idx = DAL_MANGO_TUNNEL_L3_INTF_IDX_INVALID;
            _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].path_id = DAL_MANGO_TUNNEL_INTF_PATH_ID_INVALID;
            osal_memset(&_pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].intf, 0x00, sizeof(rtk_tunnel_intf_t));
            osal_memset(&_pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].pathInfo, 0x00, sizeof(dal_mango_l3_pathInfo_t));
            RT_ERR_HDL(_dal_mango_tunnel_tunnelIntfEntry_free(unit, tunnelIntfIdx), errHandle, ret);
        }
    }

errHandle:
    TUNNEL_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_tunnel_intf_destroyAll */

/* Function Name:
 *      dal_mango_tunnel_intf_get
 * Description:
 *      Get a tunnel interface by interface ID.
 * Input:
 *      unit  - unit id
 *      pIntf - pointer to tunnel interface (interface id)
 * Output:
 *      pIntf - pointer to tunnel interface (all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
int32
dal_mango_tunnel_intf_get(uint32 unit, rtk_tunnel_intf_t *pIntf)
{
    int32   ret = RT_ERR_OK;
    uint32  tunnelIntfIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((NULL == pIntf), RT_ERR_NULL_POINTER);
    //RT_PARAM_CHK((!DAL_MANGO_L3_INTF_ID_IS_TUNNEL(pIntf->intf_id)), RT_ERR_INPUT);

    /* function body */
    TUNNEL_SEM_LOCK(unit);

    MANGO_TUNNEL_DBG_PRINTF(1, "pIntf->intf_id = 0x%08X\n", pIntf->intf_id);

    RT_ERR_HDL(_dal_mango_tunnel_tunnelIntfEntry_find(unit, pIntf->intf_id, &tunnelIntfIdx), errIntfFind, ret);

    /* [NOTICE] currently get data from the shadow */
    *pIntf = _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].intf;

    goto errOk;

errIntfFind:
errOk:
    TUNNEL_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_tunnel_intf_get */

/* Function Name:
 *      dal_mango_tunnel_intf_set
 * Description:
 *      Set a tunnel interface by interface ID.
 * Input:
 *      unit  - unit id
 *      pIntf - pointer to tunnel interface (interface id)
 * Output:
 *      pIntf - pointer to tunnel interface (all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - the module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be a null pointer
 *      RT_ERR_INPUT                    - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE             - input parameter out of range
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - variety of MTU values is over the maximum H/W supports
 * Note:
 *      (1) It cannot change the type of tunnel interface. (pIntf->type won't be referred.)
 *          Need to destroy it then create a new one.
 */
int32
dal_mango_tunnel_intf_set(uint32 unit, rtk_tunnel_intf_t *pIntf)
{
    int32   ret = RT_ERR_OK;
    uint32  l3IntfIdx;
    uint32  l3Flags = DAL_MANGO_L3_API_FLAG_MOD_TUNNEL;
    uint32  tunnelIntfIdx;
    rtk_tunnel_intf_t *pDbIntf;
    dal_mango_l3_intfEntry_t intfEntry;
    dal_mango_l3_pathInfo_t *pPathInfo;
    dal_mango_tunnel_decapEntry_t decapEntry;
    dal_mango_tunnel_encapEntry_t encapEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d,pIntf=%p", unit, pIntf);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((NULL == pIntf), RT_ERR_NULL_POINTER);
    //RT_PARAM_CHK((!DAL_MANGO_L3_INTF_ID_IS_TUNNEL(pIntf->intf_id)), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_VRF(unit) <= pIntf->vrf_id), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((pIntf->ttl > RTK_IP_TTL_MAX), RT_ERR_TTL_EXCEED);

    /* function body */
    TUNNEL_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_tunnel_tunnelIntfEntry_find(unit, pIntf->intf_id, &tunnelIntfIdx), errIntfFind, ret);
    l3IntfIdx = _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].l3_intf_idx;
    pDbIntf = &_pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].intf;

    /* tunnel type cannot be changed (overwrite it) */
    pIntf->type = pDbIntf->type;

    if ( l3IntfIdx != DAL_MANGO_TUNNEL_L3_INTF_IDX_INVALID )
    {
        RT_ERR_HDL(_dal_mango_tunnel_intf2intfEntry(&intfEntry, pIntf, tunnelIntfIdx), errHandle, ret);
        RT_ERR_HDL(_dal_mango_l3_intfEntry_set(unit, l3IntfIdx, &intfEntry, l3Flags), errHandle, ret);
    }

    /* get the 6rd domain entry index if need */
    if (RTK_TUNNEL_TYPE_6RD == pIntf->type)
    {
        uint32 ip6rdIdx = 0;
        uint32 tmp;
        uint32 ignore_ipv4_bits;
        uint32 ipv6_bits;

        ip6rdIdx = _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].ip_6rd_idx;

        MANGO_TUNNEL_DBG_PRINTF(2, "sync to 6rd related registers (ip6rdIdx = %d)\n", ip6rdIdx);

        if (pIntf->ip_6rd.ip.prefix_len < 1)
        {
            ignore_ipv4_bits = 31;  /* at least 1 bit */
        }
        else if (pIntf->ip_6rd.ip.prefix_len > 31)
        {
            ignore_ipv4_bits = 0;   /* at most 32-bit IPv4 address */
        }
        else
        {
            ignore_ipv4_bits = (32 - pIntf->ip_6rd.ip.prefix_len);
        }

        if (pIntf->ip_6rd.ip6.prefix_len > 63)
        {
            ipv6_bits = 63; /* at most 63-bit IPv6 prefix */
        }
        else
        {
            ipv6_bits = pIntf->ip_6rd.ip6.prefix_len;
        }

        /* sync to 6rd related registers */
        tmp =  pIntf->ip_6rd.ip6.prefix.octet[0] << 24;
        tmp |= pIntf->ip_6rd.ip6.prefix.octet[1] << 16;
        tmp |= pIntf->ip_6rd.ip6.prefix.octet[2] << 8;
        tmp |= pIntf->ip_6rd.ip6.prefix.octet[3] << 0;

        TUNNEL_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
            MANGO_TUNNEL_6RD_DOMAINr, REG_ARRAY_INDEX_NONE, ip6rdIdx, \
            MANGO_PREFIX_HIf, tmp, "", errHandle, ret);

        tmp =  pIntf->ip_6rd.ip6.prefix.octet[4] << 24;
        tmp |= pIntf->ip_6rd.ip6.prefix.octet[5] << 16;
        tmp |= pIntf->ip_6rd.ip6.prefix.octet[6] << 8;
        tmp |= pIntf->ip_6rd.ip6.prefix.octet[7] << 0;
        tmp = (tmp >> 1);   /* only take the highest 31 bits */

        TUNNEL_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
            MANGO_TUNNEL_6RD_DOMAINr, REG_ARRAY_INDEX_NONE, ip6rdIdx, \
            MANGO_PREFIX_LOf, tmp, "", errHandle, ret);

        TUNNEL_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
            MANGO_TUNNEL_6RD_DOMAINr, REG_ARRAY_INDEX_NONE, ip6rdIdx, MANGO_IPV4_MASK_LENf, \
            ignore_ipv4_bits, "", errHandle, ret);

        TUNNEL_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
            MANGO_TUNNEL_6RD_DOMAINr, REG_ARRAY_INDEX_NONE, ip6rdIdx, MANGO_PREFIX_LENf, \
            ipv6_bits, "", errHandle, ret);
    }

    RT_ERR_HDL(_dal_mango_tunnel_intf2decapEntry(unit, &decapEntry, pIntf, tunnelIntfIdx), errHandle, ret);
    RT_ERR_HDL(_dal_mango_tunnel_decapEntry_set(unit, tunnelIntfIdx, &decapEntry), errHandle, ret);

    /* get the nexthop info */
    pPathInfo = &_pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].pathInfo;
    if ((pIntf->encap.path_id != 0) &&
        (pIntf->encap.path_id != _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].path_id))
    {
        if (RT_ERR_OK == _dal_mango_l3_pathInfo_get(unit, pIntf->encap.path_id, pPathInfo))
        {
            /* update path info */
            _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].path_id = pIntf->encap.path_id;
        } else {
            _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].path_id = DAL_MANGO_TUNNEL_INTF_PATH_ID_INVALID;
            osal_memset(pPathInfo, 0x00, sizeof(dal_mango_l3_pathInfo_t));
        }
    }
#if 0   /* do NOT change any path info */
    else if ((pIntf->encap.path_id == 0) &&
             (pIntf->encap.path.l3_egr_intf_idx))
    {
        /* update pathInfo */
        pPathInfo->nh_dmac_valid = TRUE;
        pPathInfo->nh_dmac_idx = pIntf->encap.path.nh_dmac_idx;
        pPathInfo->l3_intf_idx = pIntf->encap.path.l3_egr_intf_idx;
    } else {
        _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].path_id = DAL_MANGO_TUNNEL_INTF_PATH_ID_INVALID;
        osal_memset(pPathInfo, 0x00, sizeof(dal_mango_l3_pathInfo_t));
    }
#endif

    /* update tunnel entry */
    RT_ERR_HDL(_dal_mango_tunnel_intf2encapEntry(unit, &encapEntry, pIntf, tunnelIntfIdx), errHandle, ret);
    encapEntry.l3_egr_intf_idx = pPathInfo->l3_intf_idx;
    encapEntry.nh_info_valid = pPathInfo->nh_dmac_valid;
    encapEntry.nh_dmac_idx = pPathInfo->nh_dmac_idx;
    encapEntry.nh_dmac_addr = pPathInfo->nh_dmac_addr;
    encapEntry.nh_smac_addr = pPathInfo->l3_intf_mac_addr;
    RT_ERR_HDL(_dal_mango_tunnel_encapEntry_set(unit, tunnelIntfIdx, &encapEntry), errHandle, ret);

    /* update database */
    _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].intf = *pIntf;

    goto errOk;

errHandle:
errIntfFind:
errOk:
    TUNNEL_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_tunnel_intf_set */

/* Function Name:
 *      dal_mango_tunnel_intfPathId_get
 * Description:
 *      Get the path ID of the specified tunnel interface
 * Input:
 *      unit    - unit id
 *      intfId  - tunnel interface id
 * Output:
 *      pPathId - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
int32
dal_mango_tunnel_intfPathId_get(uint32 unit, rtk_intf_id_t intfId, rtk_l3_pathId_t *pPathId)
{
    int32   ret = RT_ERR_OK;
    uint32  tunnelIntfIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d,intfId=%d", unit, intfId);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID), RT_ERR_CHIP_NOT_SUPPORTED);
    //RT_PARAM_CHK((!DAL_MANGO_L3_INTF_ID_IS_TUNNEL(intfId)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPathId), RT_ERR_NULL_POINTER);

    /* function body */
    TUNNEL_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_tunnel_tunnelIntfEntry_find(unit, intfId, &tunnelIntfIdx), errIntfFind, ret);

    *pPathId = _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].path_id;

errIntfFind:
    TUNNEL_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_tunnel_intfPathId_get */

/* Function Name:
 *      dal_mango_tunnel_intfPathId_set
 * Description:
 *      Set the path ID of the specified tunnel interface
 * Input:
 *      unit   - unit id
 *      intfId - tunnel interface id
 *      pathId - control type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Note:
 *      (1) 9310 only supports Nexthop-type path ID (not ECMP)
 */
int32
dal_mango_tunnel_intfPathId_set(uint32 unit, rtk_intf_id_t intfId, rtk_l3_pathId_t pathId)
{
    int32   ret = RT_ERR_OK;
    uint32  tunnelIntfIdx;
    dal_mango_l3_pathInfo_t *pPathInfo;
    rtk_tunnel_intf_t *pIntf;
    dal_mango_tunnel_encapEntry_t encapEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d,intfId=%d,pathId=%d", unit, intfId, pathId);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID), RT_ERR_CHIP_NOT_SUPPORTED);
    //RT_PARAM_CHK((!DAL_MANGO_L3_INTF_ID_IS_TUNNEL(intfId)), RT_ERR_INPUT);
    RT_PARAM_CHK((!DAL_MANGO_L3_PATH_ID_IS_NH(pathId)), RT_ERR_INPUT);

    /* function body */
    TUNNEL_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_tunnel_tunnelIntfEntry_find(unit, intfId, &tunnelIntfIdx), errIntfFind, ret);

    /* tunnel intf */
    pIntf = &_pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].intf;

    /* get the nexthop info */
    pPathInfo = &_pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].pathInfo;
    RT_ERR_HDL(_dal_mango_l3_pathInfo_get(unit, pathId, pPathInfo), errPathInfoGet, ret);

    /* update tunnel entry */
    RT_ERR_HDL(_dal_mango_tunnel_intf2encapEntry(unit, &encapEntry, pIntf, tunnelIntfIdx), errHandle, ret);
    encapEntry.l3_egr_intf_idx = pPathInfo->l3_intf_idx;
    encapEntry.nh_info_valid = pPathInfo->nh_dmac_valid;
    encapEntry.nh_dmac_idx = pPathInfo->nh_dmac_idx;
    encapEntry.nh_dmac_addr = pPathInfo->nh_dmac_addr;
    encapEntry.nh_smac_addr = pPathInfo->l3_intf_mac_addr;
    RT_ERR_HDL(_dal_mango_tunnel_encapEntry_set(unit, tunnelIntfIdx, &encapEntry), errHandle, ret);

    MANGO_TUNNEL_DBG_PRINTF(1, "[DBG] pathinfo.l3_intf_idx = %d\n", pPathInfo->l3_intf_idx);
    MANGO_TUNNEL_DBG_PRINTF(1, "[DBG] pathinfo.nh_dmac_valid = %d\n", pPathInfo->nh_dmac_valid);
    MANGO_TUNNEL_DBG_PRINTF(1, "[DBG] pathinfo.nh_dmac_idx = %d\n", pPathInfo->nh_dmac_idx);
    MANGO_TUNNEL_DBG_PRINTF(1, "[DBG] pathinfo.nh_dmac_addr = %02X:%02X:%02X:%02X:%02X:%02X\n", \
        pPathInfo->nh_dmac_addr.octet[0], pPathInfo->nh_dmac_addr.octet[1], pPathInfo->nh_dmac_addr.octet[2],
        pPathInfo->nh_dmac_addr.octet[3], pPathInfo->nh_dmac_addr.octet[4], pPathInfo->nh_dmac_addr.octet[5]);
    MANGO_TUNNEL_DBG_PRINTF(1, "[DBG] pathinfo.l3_intf_mac_addr = %02X:%02X:%02X:%02X:%02X:%02X\n", \
        pPathInfo->l3_intf_mac_addr.octet[0], pPathInfo->l3_intf_mac_addr.octet[1], pPathInfo->l3_intf_mac_addr.octet[2],
        pPathInfo->l3_intf_mac_addr.octet[3], pPathInfo->l3_intf_mac_addr.octet[4], pPathInfo->l3_intf_mac_addr.octet[5]);

    /* update database */
    _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].intf.encap.path_id = pathId;
    _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].path_id = pathId;
    _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].intf.encap.path.nh_dmac_idx = pPathInfo->nh_dmac_idx;
    _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].intf.encap.path.l3_egr_intf_idx = pPathInfo->l3_intf_idx;

errIntfFind:
errPathInfoGet:
errHandle:
    TUNNEL_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_tunnel_intfPathId_set */

/* Function Name:
 *      dal_mango_tunnel_intfPath_get
 * Description:
 *      Get the path information of the specified tunnel interface
 * Input:
 *      unit          - unit id
 *      intfId        - tunnel interface id
 * Output:
 *      pNhDmacIdx    - pointer to nexthop DMAC entry index
 *      pL3EgrIntfIdx - pointer to L3 egress interface index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
int32
dal_mango_tunnel_intfPath_get(uint32 unit, rtk_intf_id_t intfId, uint32 *pNhDmacIdx, uint32 *pL3EgrIntfIdx)
{
    int32   ret = RT_ERR_OK;
    uint32  tunnelIntfIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d,intfId=%d", unit, intfId);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((NULL == pNhDmacIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pL3EgrIntfIdx), RT_ERR_NULL_POINTER);

    /* function body */
    TUNNEL_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_tunnel_tunnelIntfEntry_find(unit, intfId, &tunnelIntfIdx), errIntfFind, ret);

    *pNhDmacIdx     = _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].pathInfo.nh_dmac_idx;
    *pL3EgrIntfIdx  = _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].pathInfo.l3_intf_idx;

errIntfFind:
    TUNNEL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_tunnel_intfPath_get */

/* Function Name:
 *      dal_mango_tunnel_intfPath_set
 * Description:
 *      Set the path ID of the specified tunnel interface
 * Input:
 *      unit         - unit id
 *      intfId       - tunnel interface id
 *      nhDmacIdx    - nexthop DMAC entry index
 *      l3EgrIntfIdx - L3 egress interface index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Note:
 *      None
 */
int32
dal_mango_tunnel_intfPath_set(uint32 unit, rtk_intf_id_t intfId, uint32 nhDmacIdx, uint32 l3EgrIntfIdx)
{
    int32   ret = RT_ERR_OK;
    uint32  tunnelIntfIdx;
    dal_mango_l3_pathInfo_t *pPathInfo;
    rtk_tunnel_intf_t *pIntf;
    dal_mango_tunnel_encapEntry_t encapEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d,intfId=%d,nhDmacIdx=%d,l3EgrIntfIdx=%d", unit, intfId, nhDmacIdx, l3EgrIntfIdx);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID), RT_ERR_CHIP_NOT_SUPPORTED);

    /* function body */
    TUNNEL_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_tunnel_tunnelIntfEntry_find(unit, intfId, &tunnelIntfIdx), errIntfFind, ret);

    /* tunnel intf */
    pIntf = &_pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].intf;

    /* get the nexthop info */
    pPathInfo = &_pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].pathInfo;
    RT_ERR_HDL(_dal_mango_l3_pathInfo_get_byIdx(unit, nhDmacIdx, l3EgrIntfIdx, pPathInfo), errPathInfoGet, ret);

    /* update tunnel entry */
    RT_ERR_HDL(_dal_mango_tunnel_intf2encapEntry(unit, &encapEntry, pIntf, tunnelIntfIdx), errHandle, ret);
    encapEntry.l3_egr_intf_idx  = pPathInfo->l3_intf_idx;
    encapEntry.nh_info_valid    = pPathInfo->nh_dmac_valid;
    encapEntry.nh_dmac_idx      = pPathInfo->nh_dmac_idx;
    encapEntry.nh_dmac_addr     = pPathInfo->nh_dmac_addr;
    encapEntry.nh_smac_addr     = pPathInfo->l3_intf_mac_addr;
    RT_ERR_HDL(_dal_mango_tunnel_encapEntry_set(unit, tunnelIntfIdx, &encapEntry), errHandle, ret);

    /* update database */
    _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].intf.encap.path_id = 0;
    _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].path_id = 0;
    _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].intf.encap.path.nh_dmac_idx = pPathInfo->nh_dmac_idx;
    _pTunnelDb[unit]->tunnel_intf[tunnelIntfIdx].intf.encap.path.l3_egr_intf_idx = pPathInfo->l3_intf_idx;

errHandle:
errPathInfoGet:
errIntfFind:
    TUNNEL_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_tunnel_intfPath_set */

/* Function Name:
 *      dal_mango_tunnel_intfStats_get
 * Description:
 *      Get statistic counters of the specified tunnel interface
 * Input:
 *      unit   - unit id
 *      intfId - tunnel interface id
 * Output:
 *      pStats - pointer to the statistic data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - the module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be a null pointer
 *      RT_ERR_NOT_TUNNEL_INTF - input interface type is not tunnel
 * Note:
 *      None
 */
int32
dal_mango_tunnel_intfStats_get(uint32 unit, rtk_intf_id_t intfId, rtk_tunnel_intf_stats_t *pStats)
{
    int32   ret = RT_ERR_OK;
    uint32  tunnelIntfIdx, tunnelIntfAddr;
    uint32  intfIdx = DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId);
    uint32  value[2];
    l3_igr_tunnel_intf_cntr_t igr_cntr;
    l3_egr_tunnel_intf_cntr_t egr_cntr;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d,intfId=%d", unit, intfId);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_INTF(unit) <= intfIdx), RT_ERR_INPUT);
    //RT_PARAM_CHK((!DAL_MANGO_L3_INTF_ID_IS_TUNNEL(intfId)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pStats), RT_ERR_NULL_POINTER);

    /* function body */
    TUNNEL_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_tunnel_tunnelIntfEntry_find(unit, intfId, &tunnelIntfIdx), errIntfFind, ret);

    /* found */
    tunnelIntfAddr = DAL_MANGO_TUNNEL_ENTRY_IDX_TO_ADDR(tunnelIntfIdx);
    TUNNEL_TABLE_READ_ERR_HDL(unit, MANGO_L3_IGR_TUNNEL_INTF_CNTRt, tunnelIntfAddr, igr_cntr, "", errHandle, ret);
    TUNNEL_TABLE_READ_ERR_HDL(unit, MANGO_L3_EGR_TUNNEL_INTF_CNTRt, tunnelIntfAddr, egr_cntr, "", errHandle, ret);

    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_TUNNEL_INTF_CNTRt, \
        MANGO_L3_IGR_TUNNEL_INTF_CNTR_IF_IN_OCTETStf, value, igr_cntr, "", errHandle, ret);
    pStats->rx.octets = ((uint64)value[0] << 10) | value[1];

    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_TUNNEL_INTF_CNTRt, \
        MANGO_L3_IGR_TUNNEL_INTF_CNTR_IF_IN_PKTStf, value, igr_cntr, "", errHandle, ret);
    pStats->rx.pkts = ((uint64)value[0] << 4) | value[1];

    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_TUNNEL_INTF_CNTRt, \
        MANGO_L3_IGR_TUNNEL_INTF_CNTR_IF_IN_DROPStf, value, igr_cntr, "", errHandle, ret);
    pStats->rx.drops = ((uint64)value[0] << 4) | value[1];

    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_TUNNEL_INTF_CNTRt, \
        MANGO_L3_EGR_TUNNEL_INTF_CNTR_IF_OUT_OCTETStf, value, egr_cntr, "", errHandle, ret);
    pStats->tx.octets = ((uint64)value[0] << 10) | value[1];

    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_TUNNEL_INTF_CNTRt, \
        MANGO_L3_EGR_TUNNEL_INTF_CNTR_IF_OUT_PKTStf, value, egr_cntr, "", errHandle, ret);
    pStats->tx.pkts = ((uint64)value[0] << 4) | value[1];

    TUNNEL_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_TUNNEL_INTF_CNTRt, \
        MANGO_L3_EGR_TUNNEL_INTF_CNTR_IF_OUT_DROPStf, value, egr_cntr, "", errHandle, ret);
    pStats->tx.drops = ((uint64)value[0] << 4) | value[1];

    goto errOk;

errHandle:
errIntfFind:
errOk:
    TUNNEL_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_tunnel_intfStats_get */

/* Function Name:
 *      dal_mango_tunnel_intfStats_reset
 * Description:
 *      Reset statistic counters of the specified tunnel interface
 * Input:
 *      unit   - unit id
 *      intfId - tunnel interface id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - the module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be a null pointer
 *      RT_ERR_NOT_TUNNEL_INTF - input interface type is not tunnel
 * Note:
 *      None
 */
int32
dal_mango_tunnel_intfStats_reset(uint32 unit, rtk_intf_id_t intfId)
{
    int32   ret = RT_ERR_OK;
    uint32  tunnelIntfIdx, tunnelIntfAddr;
    uint32  intfIdx = DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId);
    l3_igr_tunnel_intf_cntr_t igr_cntr;
    l3_egr_tunnel_intf_cntr_t egr_cntr;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d,intfId=%d", unit, intfId);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_INTF(unit) <= intfIdx), RT_ERR_INPUT);
    //RT_PARAM_CHK((!DAL_MANGO_L3_INTF_ID_IS_TUNNEL(intfId)), RT_ERR_INPUT);

    /* function body */
    TUNNEL_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_tunnel_tunnelIntfEntry_find(unit, intfId, &tunnelIntfIdx), errIntfFind, ret);

    /* found */
    tunnelIntfAddr = DAL_MANGO_TUNNEL_ENTRY_IDX_TO_ADDR(tunnelIntfIdx);
    osal_memset(&igr_cntr, 0x00, sizeof(l3_igr_tunnel_intf_cntr_t));
    osal_memset(&egr_cntr, 0x00, sizeof(l3_egr_tunnel_intf_cntr_t));
    TUNNEL_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_IGR_TUNNEL_INTF_CNTRt, tunnelIntfAddr, igr_cntr, "", errHandle, ret);
    TUNNEL_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_EGR_TUNNEL_INTF_CNTRt, tunnelIntfAddr, egr_cntr, "", errHandle, ret);

    goto errOk;

errHandle:
errIntfFind:
errOk:
    TUNNEL_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_tunnel_intfStats_reset */

/* Function Name:
 *      dal_mango_tunnel_qosProfile_get
 * Description:
 *      Get the QoS profile with the specified index.
 * Input:
 *      unit     - unit id
 *      idx      - index of QoS profile
 * Output:
 *      pProfile - pointer to the QoS prifle
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
dal_mango_tunnel_qosProfile_get(uint32 unit, uint32 idx, rtk_tunnel_qosProfile_t *pProfile)
{
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d,idx=%d", unit, idx);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_TUNNEL(unit) <= idx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pProfile), RT_ERR_NULL_POINTER);

    /* function body */
    TUNNEL_SEM_LOCK(unit);

    osal_memset(pProfile, 0x00, sizeof(rtk_tunnel_qosProfile_t));
    TUNNEL_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
        MANGO_TUNNEL_QOS_PROFILEr, REG_ARRAY_INDEX_NONE, idx, \
        MANGO_OPRIf, pProfile->outer_pri, "", errHandle, ret);
    TUNNEL_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
        MANGO_TUNNEL_QOS_PROFILEr, REG_ARRAY_INDEX_NONE, idx, \
        MANGO_OPRI_SRCf, pProfile->outer_pri_src, "", errHandle, ret);
    TUNNEL_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
        MANGO_TUNNEL_QOS_PROFILEr, REG_ARRAY_INDEX_NONE, idx, \
        MANGO_IPRIf, pProfile->inner_pri, "", errHandle, ret);
    TUNNEL_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
        MANGO_TUNNEL_QOS_PROFILEr, REG_ARRAY_INDEX_NONE, idx, \
        MANGO_IPRI_SRCf, pProfile->inner_pri_src, "", errHandle, ret);
    TUNNEL_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
        MANGO_TUNNEL_QOS_PROFILEr, REG_ARRAY_INDEX_NONE, idx, \
        MANGO_DSCPf, pProfile->dscp, "", errHandle, ret);
    TUNNEL_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
        MANGO_TUNNEL_QOS_PROFILEr, REG_ARRAY_INDEX_NONE, idx, \
        MANGO_DSCP_SRCf, pProfile->dscp_src, "", errHandle, ret);

errHandle:
    TUNNEL_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_tunnel_qosProfile_get */

/* Function Name:
 *      dal_mango_tunnel_qosProfile_set
 * Description:
 *      Set the QoS profile with the specified index.
 * Input:
 *      unit     - unit id
 *      idx      - index of QoS profile
 *      pProfile - pointer to the QoS prifle
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 */
int32
dal_mango_tunnel_qosProfile_set(uint32 unit, uint32 idx, rtk_tunnel_qosProfile_t profile)
{
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d,idx=%d", unit, idx);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_TUNNEL_QOS_PROFILE(unit) <= idx), RT_ERR_INPUT);

    /* function body */
    TUNNEL_SEM_LOCK(unit);

    TUNNEL_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
        MANGO_TUNNEL_QOS_PROFILEr, REG_ARRAY_INDEX_NONE, idx, \
        MANGO_OPRIf, profile.outer_pri, "", errHandle, ret);
    TUNNEL_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
        MANGO_TUNNEL_QOS_PROFILEr, REG_ARRAY_INDEX_NONE, idx, \
        MANGO_OPRI_SRCf, profile.outer_pri_src, "", errHandle, ret);
    TUNNEL_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
        MANGO_TUNNEL_QOS_PROFILEr, REG_ARRAY_INDEX_NONE, idx, \
        MANGO_IPRIf, profile.inner_pri, "", errHandle, ret);
    TUNNEL_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
        MANGO_TUNNEL_QOS_PROFILEr, REG_ARRAY_INDEX_NONE, idx, \
        MANGO_IPRI_SRCf, profile.inner_pri_src, "", errHandle, ret);
    TUNNEL_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
        MANGO_TUNNEL_QOS_PROFILEr, REG_ARRAY_INDEX_NONE, idx, \
        MANGO_DSCPf, profile.dscp, "", errHandle, ret);
    TUNNEL_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
        MANGO_TUNNEL_QOS_PROFILEr, REG_ARRAY_INDEX_NONE, idx, \
        MANGO_DSCP_SRCf, profile.dscp_src, "", errHandle, ret);

errHandle:
    TUNNEL_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_tunnel_qosProfile_set */


/* Function Name:
 *      dal_mango_tunnel_globalCtrl_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
int32
dal_mango_tunnel_globalCtrl_get(uint32 unit, rtk_tunnel_globalCtrlType_t type, int32 *pArg)
{
    int32   ret = RT_ERR_OK;
    uint32  val;
    rtk_action_t act;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d,type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((RTK_TUNNEL_GCT_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pArg), RT_ERR_NULL_POINTER);

    /* function body */
    TUNNEL_SEM_LOCK(unit);

    switch (type)
    {
    case RTK_TUNNEL_GCT_VXLAN_UDP_DPORT:
        {
            TUNNEL_REG_FIELD_READ_ERR_HDL(unit, MANGO_TUNNEL_VXLAN_CTRLr, MANGO_UDP_PORTf, val, "", errExit, ret);
            *pArg = val;
        }
        break;

#if 0
    case RTK_TUNNEL_GCT_DECAP_IP4_SIP_FAIL_ACT:
        {
            TUNNEL_REG_FIELD_READ_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_IP4_SIP_FAIL_TRAPf, val, "", errExit, ret);
            TUNNEL_VALUE_TO_ACTION(_actDecapIp4SipFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;
#endif

    case RTK_TUNNEL_GCT_DECAP_IP6_SIP_IP4CMPT_CHK:
        {
            TUNNEL_REG_FIELD_READ_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_IP6_SIP_IP4CMPT_CHKf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_IP6_SIP_IP4MAP_CHK:
        {
            TUNNEL_REG_FIELD_READ_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_IP6_SIP_IP4MAP_CHKf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_IP6_SIP_FAIL_ACT:
        {
            TUNNEL_REG_FIELD_READ_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_IP6_SIP_FAIL_TRAPf, val, "", errExit, ret);
            TUNNEL_VALUE_TO_ACTION(_actDecapIp6SipFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_ISATAP_SIP_TYPE_CHK:
        {
            TUNNEL_REG_FIELD_READ_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_ISATAP_SIP_TYPE_CHKf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_ISATAP_SIP_MAP_CHK:
        {
            TUNNEL_REG_FIELD_READ_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_ISATAP_SIP_MAP_CHKf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_ISATAP_SIP_FAIL_ACT:
        {
            TUNNEL_REG_FIELD_READ_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_ISATAP_SIP_FAIL_TRAPf, val, "", errExit, ret);
            TUNNEL_VALUE_TO_ACTION(_actDecapIsatapSipFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_6TO4_SIP_CHK:
        {
            TUNNEL_REG_FIELD_READ_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_6TO4_SIP_CHKf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_6TO4_SIP_FAIL_ACT:
        {
            TUNNEL_REG_FIELD_READ_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_6TO4_SIP_FAIL_TRAPf, val, "", errExit, ret);
            TUNNEL_VALUE_TO_ACTION(_actDecap6to4SipFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_6TO4_DIP_CHK:
        {
            TUNNEL_REG_FIELD_READ_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_6TO4_DIP_CHKf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_6TO4_DIP_FAIL_ACT:
        {
            TUNNEL_REG_FIELD_READ_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_6TO4_DIP_FAIL_TRAPf, val, "", errExit, ret);
            TUNNEL_VALUE_TO_ACTION(_actDecap6to4DipFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_6RD_DIP_CHK:
        {
            TUNNEL_REG_FIELD_READ_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_6RD_DIP_CHKf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_6RD_DIP_FAIL_ACT:
        {
            TUNNEL_REG_FIELD_READ_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_6RD_DIP_FAIL_TRAPf, val, "", errExit, ret);
            TUNNEL_VALUE_TO_ACTION(_actDecap6rdDipFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_TUNNEL_GCT_ENCAP_IP_HDR_IDENTIFICATION:
        {
            TUNNEL_REG_FIELD_READ_ERR_HDL(unit, MANGO_TUNNEL_IP_IDENTIFICATIONr, MANGO_IDf, val, "", errExit, ret);
            *pArg = val;
        }
        break;

    case RTK_TUNNEL_GCT_ENCAP_MTU_FAIL_ACT:
        {
            TUNNEL_REG_FIELD_READ_ERR_HDL(unit, MANGO_TUNNEL_ROUTE_CTRLr, MANGO_MTU_FAIL_ACTf, val, "", errExit, ret);
            TUNNEL_VALUE_TO_ACTION(_actEncapMtuFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_TUNNEL_GCT_ENCAP_TTL_FAIL_ACT:
        {
            TUNNEL_REG_FIELD_READ_ERR_HDL(unit, MANGO_TUNNEL_ROUTE_CTRLr, MANGO_TTL_FAIL_ACTf, val, "", errExit, ret);
            TUNNEL_VALUE_TO_ACTION(_actEncapTtlFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_TUNNEL_GCT_ROUTE_TO_TUNNEL_ACT:
        {
            TUNNEL_REG_FIELD_READ_ERR_HDL(unit, MANGO_TUNNEL_ROUTE_CTRLr, MANGO_ROUTE_TO_TUNNEL_ACTf, val, "", errExit, ret);
            TUNNEL_VALUE_TO_ACTION(_actRouteToTunnel, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    default:
        ret = RT_ERR_INPUT;
        goto errInput;
    }

errInput:
errExit:
    TUNNEL_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_tunnel_globalCtrl_get */

/* Function Name:
 *      dal_mango_tunnel_globalCtrl_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
int32
dal_mango_tunnel_globalCtrl_set(uint32 unit, rtk_tunnel_globalCtrlType_t type, int32 arg)
{
    int32   ret = RT_ERR_OK;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d,type=%d,arg=%d", unit, type, arg);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((RTK_TUNNEL_GCT_END <= type), RT_ERR_INPUT);

    /* function body */
    TUNNEL_SEM_LOCK(unit);

    switch (type)
    {
    case RTK_TUNNEL_GCT_VXLAN_UDP_DPORT:
        {
            val = ((uint32)arg & 0xFFFF);
            TUNNEL_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_TUNNEL_VXLAN_CTRLr, MANGO_UDP_PORTf, val, "", errExit, ret);
            goto errOk;
        }
        break;

#if 0
    case RTK_TUNNEL_GCT_DECAP_IP4_SIP_FAIL_ACT:
        {
            TUNNEL_ACTION_TO_VALUE(_actDecapIp4SipFail, val, arg, "", errExit, ret);
            TUNNEL_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_IP4_SIP_FAIL_TRAPf, val, "", errExit, ret);
        }
        break;
#endif

    case RTK_TUNNEL_GCT_DECAP_IP6_SIP_IP4CMPT_CHK:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            TUNNEL_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_IP6_SIP_IP4CMPT_CHKf, val, "", errExit, ret);
            goto errOk;
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_IP6_SIP_IP4MAP_CHK:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            TUNNEL_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_IP6_SIP_IP4MAP_CHKf, val, "", errExit, ret);
            goto errOk;
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_IP6_SIP_FAIL_ACT:
        {
            TUNNEL_ACTION_TO_VALUE(_actDecapIp6SipFail, val, arg, "", errExit, ret);
            TUNNEL_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_IP6_SIP_FAIL_TRAPf, val, "", errExit, ret);
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_ISATAP_SIP_TYPE_CHK:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            TUNNEL_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_ISATAP_SIP_TYPE_CHKf, val, "", errExit, ret);
            goto errOk;
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_ISATAP_SIP_MAP_CHK:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            TUNNEL_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_ISATAP_SIP_MAP_CHKf, val, "", errExit, ret);
            goto errOk;
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_ISATAP_SIP_FAIL_ACT:
        {
            TUNNEL_ACTION_TO_VALUE(_actDecapIsatapSipFail, val, arg, "", errExit, ret);
            TUNNEL_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_ISATAP_SIP_FAIL_TRAPf, val, "", errExit, ret);
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_6TO4_SIP_CHK:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            TUNNEL_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_6TO4_SIP_CHKf, val, "", errExit, ret);
            goto errOk;
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_6TO4_SIP_FAIL_ACT:
        {
            TUNNEL_ACTION_TO_VALUE(_actDecap6to4SipFail, val, arg, "", errExit, ret);
            TUNNEL_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_6TO4_SIP_FAIL_TRAPf, val, "", errExit, ret);
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_6TO4_DIP_CHK:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            TUNNEL_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_6TO4_DIP_CHKf, val, "", errExit, ret);
            goto errOk;
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_6TO4_DIP_FAIL_ACT:
        {
            TUNNEL_ACTION_TO_VALUE(_actDecap6to4DipFail, val, arg, "", errExit, ret);
            TUNNEL_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_6TO4_DIP_FAIL_TRAPf, val, "", errExit, ret);
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_6RD_DIP_CHK:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            TUNNEL_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_6RD_DIP_CHKf, val, "", errExit, ret);
            goto errOk;
        }
        break;

    case RTK_TUNNEL_GCT_DECAP_6RD_DIP_FAIL_ACT:
        {
            TUNNEL_ACTION_TO_VALUE(_actDecap6rdDipFail, val, arg, "", errExit, ret);
            TUNNEL_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_TUNNEL_IP_ADDR_CHK_CTRLr, MANGO_TT_6RD_DIP_FAIL_TRAPf, val, "", errExit, ret);
        }
        break;

    case RTK_TUNNEL_GCT_ENCAP_IP_HDR_IDENTIFICATION:
        {
            val = ((uint32)arg & 0xFFFF);
            TUNNEL_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_TUNNEL_IP_IDENTIFICATIONr, MANGO_IDf, val, "", errExit, ret);
            goto errOk;
        }
        break;

    case RTK_TUNNEL_GCT_ENCAP_MTU_FAIL_ACT:
        {
            TUNNEL_ACTION_TO_VALUE(_actEncapMtuFail, val, arg, "", errExit, ret);
            TUNNEL_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_TUNNEL_ROUTE_CTRLr, MANGO_MTU_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_TUNNEL_GCT_ENCAP_TTL_FAIL_ACT:
        {
            TUNNEL_ACTION_TO_VALUE(_actEncapTtlFail, val, arg, "", errExit, ret);
            TUNNEL_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_TUNNEL_ROUTE_CTRLr, MANGO_TTL_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_TUNNEL_GCT_ROUTE_TO_TUNNEL_ACT:
        {
            TUNNEL_ACTION_TO_VALUE(_actRouteToTunnel, val, arg, "", errExit, ret);
            TUNNEL_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_TUNNEL_ROUTE_CTRLr, MANGO_ROUTE_TO_TUNNEL_ACTf, val, "", errExit, ret);
        }
        break;

    default:
        ret = RT_ERR_INPUT;
        goto errInput;
    }

errInput:
errExit:
errOk:
    TUNNEL_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_tunnel_globalCtrl_set */

/* Function Name:
 *      dal_mango_capwap_udpPort_get
 * Description:
 *      Get UDP port number of CAPWAP control and data packet.
 * Input:
 *      unit	    - unit id
 * Output:
 *      pCtrl_port	- pointer to udp control port
 *      pData_port 	- pointer to udp data port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT 	- The module is not initial
 *      RT_ERR_NULL_POINTER	- input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_capwap_udpPort_get(uint32 unit, uint32 *pCtrl_port, uint32 *pData_port)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pCtrl_port), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pData_port), RT_ERR_NULL_POINTER);

    /* function body */
    TUNNEL_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MANGO_CAPWAP_UDP_PORTr, MANGO_CTRLf, pCtrl_port)) != RT_ERR_OK)
    {
        TUNNEL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TUNNEL), "");
        return ret;
    }
    if ((ret = reg_field_read(unit, MANGO_CAPWAP_UDP_PORTr, MANGO_DATAf, pData_port)) != RT_ERR_OK)
    {
        TUNNEL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TUNNEL), "");
        return ret;
    }

    TUNNEL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}/* end of dal_mango_capwap_udpPort_get */

/* Function Name:
 *      dal_mango_capwap_udpPort_set
 * Description:
 *      Set UDP port number of CAPWAP control and data packet.
 * Input:
 *      unit	    - unit id
 *      ctrl_port	- udp control port
 *      data_port 	- udp data port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_capwap_udpPort_set(uint32 unit, uint32 ctrl_port, uint32 data_port)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TUNNEL), "unit=%d,ctrl_port=%d,data_port=%d", unit, ctrl_port, data_port);

    /* check Init status */
    RT_INIT_CHK(tunnel_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((65536 <= ctrl_port), RT_ERR_INPUT);
    RT_PARAM_CHK((65536 <= data_port), RT_ERR_INPUT);

    /* function body */
    TUNNEL_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MANGO_CAPWAP_UDP_PORTr, MANGO_CTRLf, &ctrl_port)) != RT_ERR_OK)
    {
        TUNNEL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TUNNEL), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_CAPWAP_UDP_PORTr, MANGO_DATAf, &data_port)) != RT_ERR_OK)
    {
        TUNNEL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TUNNEL), "");
        return ret;
    }

    TUNNEL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}/* end of dal_mango_capwap_udpPort_set */


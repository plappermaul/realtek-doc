/*
 * Copyright (C) 2014 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 46639 $
 * $Date: 2014-02-26 16:48:44 +0800 (週三, 26 二月 2014) $
 *
 * Purpose : Definition of VLAN API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Vlan table configure and modification
 *           (2) Accept frame type
 *           (3) Vlan ingress/egress filter
 *           (4) Port based and protocol based vlan
 *           (5) TPID configuration
 *           (6) Ingress tag handling
 *           (7) Tag format handling
 *
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <dal/ca8277b/dal_ca8277b.h>
#include <dal/ca8277b/dal_ca8277b_vlan.h>
#include <osal/time.h>
#include <scfg.h>

#include <cortina-api/include/vlan.h>
#include <cortina-api/include/port.h>

#include <aal_l2_vlan.h>

/*
 * Symbol Definition
 */

static uint32   vlan_init = INIT_NOT_COMPLETED;

static rtk_enable_t VLAN_FUNC_ENABLE = ENABLED;

static uint32   vlan_valid[(RTK_VLAN_ID_MAX + 1) / 32];

#define DAL_CA8277B_VLAN_SET(vid) \
do {\
    if ((vid) <= RTK_VLAN_ID_MAX) {vlan_valid[vid >> 5] |= (1 << (vid&31));}\
} while (0);\

#define DAL_CA8277B_VLAN_CLEAR(vid) \
do {\
    if ((vid) <= RTK_VLAN_ID_MAX) {vlan_valid[vid >> 5] &= (~(1 << (vid&31)));}\
} while (0);\

#define DAL_CA8277B_VLAN_IS_SET(vid) \
    (((vid) <= RTK_VLAN_ID_MAX)?((vlan_valid[vid >> 5] >> (vid&31)) & 1): 0)


/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_ca8277b_vlan_init
 * Description:
 *      Initialize vlan module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize vlan module before calling any vlan APIs.
 */
int32
dal_ca8277b_vlan_init(void)
{
    int32 ret;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    vlan_init = INIT_COMPLETED;

    if((ret = dal_ca8277b_vlan_vlanFunctionEnable_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    /* Create Default VLAN */
    if((ret = dal_ca8277b_vlan_destroyAll(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_ca8277b_vlan_init */

/* Module Name    : Vlan                                  */
/* Sub-module Name: Vlan table configure and modification */

/* Function Name:
 *      dal_ca8277b_vlan_create
 * Description:
 *      Create the vlan in the specified device.
 * Input:
 *      vid  - vlan id to be created
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_VLAN_VID   - invalid vid
 *      RT_ERR_VLAN_EXIST - vlan is exist
 * Note:
 */
int32
dal_ca8277b_vlan_create(rtk_vlan_t vid)
{
    ca_status_t ret=CA_E_OK;
    rtk_portmask_t Member_portmask;
    rtk_portmask_t Untag_portmask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);

    if(VLAN_FUNC_ENABLE == ENABLED)
    {
        if((ret = ca_l2_vlan_create(0,vid))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
            if(ret == CA_E_EXISTS)
                return RT_ERR_VLAN_EXIST;
            else
                return RT_ERR_FAILED;
        }

        DAL_CA8277B_VLAN_SET(vid)

        RTK_PORTMASK_RESET(Member_portmask);
        RTK_PORTMASK_RESET(Untag_portmask);
        if((ret = dal_ca8277b_vlan_port_set(vid,&Member_portmask,&Untag_portmask))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
} /* end of dal_ca8277b_vlan_create */

/* Function Name:
 *      dal_ca8277b_vlan_destroy
 * Description:
 *      Destroy the vlan.
 * Input:
 *      vid  - vlan id to be destroyed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 * Note:
 *      None
 */
int32
dal_ca8277b_vlan_destroy(rtk_vlan_t vid)
{
    ca_status_t ret=CA_E_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);

    if(VLAN_FUNC_ENABLE == ENABLED)
    {
        if((ret = ca_l2_vlan_delete(0,vid))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
            if(ret == CA_E_NOT_FOUND)
                return RT_ERR_VLAN_ENTRY_NOT_FOUND;
            else
                return RT_ERR_FAILED;
        }

        DAL_CA8277B_VLAN_CLEAR(vid);
    }
    
    return RT_ERR_OK;
} /* end of dal_ca8277b_vlan_destroy */


/* Function Name:
 *      dal_ca8277b_vlan_destroyAll
 * Description:
 *      Destroy all vlans except default vlan.
 * Input:
 *      restore_default_vlan - keep and restore default vlan id or not?
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 * Note:
 *      The restore argument is permit following value:
 *      - 0: remove default vlan
 *      - 1: restore default vlan
 */
int32
dal_ca8277b_vlan_destroyAll(uint32 restoreDefaultVlan)
{
    ca_status_t ret=CA_E_OK;
    rtk_portmask_t allPortMask;
    rtk_portmask_t Member_portmask;
    rtk_portmask_t Untag_portmask;
    rtk_port_t port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= restoreDefaultVlan), RT_ERR_INPUT);

    if(VLAN_FUNC_ENABLE == ENABLED)
    {    
        if((ret = ca_l2_vlan_delete_all(0))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
            return RT_ERR_FAILED;
        }

        osal_memset(vlan_valid, 0x00, sizeof(uint32) * (RTK_VLAN_ID_MAX + 1) / 32);

        if(ENABLED == restoreDefaultVlan)
        {
            /* Create Default VLAN */
            if((ret = dal_ca8277b_vlan_create(DAL_CA8277B_DEFAULT_VLAN_ID)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }

            DAL_CA8277B_VLAN_SET(DAL_CA8277B_DEFAULT_VLAN_ID)

            HAL_GET_ALL_PORTMASK(allPortMask);
            RTK_PORTMASK_ASSIGN(Member_portmask,allPortMask);
            RTK_PORTMASK_ASSIGN(Untag_portmask,allPortMask);
            if((ret = dal_ca8277b_vlan_port_set(DAL_CA8277B_DEFAULT_VLAN_ID,&Member_portmask,&Untag_portmask))!=RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
                return ret;
            }

            /* Set PVID */
            HAL_SCAN_ALL_PORT(port)
            {
                if((ret = dal_ca8277b_vlan_portPvid_set(port, DAL_CA8277B_DEFAULT_VLAN_ID)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }
            }
        }
    }

    return RT_ERR_OK;
} /* end of dal_ca8277b_vlan_destroyAll */

/* Module Name    : Vlan                               */
/* Sub-module Name: Port based and protocol based vlan */

/* Function Name:
 *      dal_ca8277b_vlan_portPvid_get
 * Description:
 *      Get port default vlan id.
 * Input:
 *      port  - port id
 * Output:
 *      pPvid - pointer buffer of port default vlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_ca8277b_vlan_portPvid_get(rtk_port_t port, uint32 *pPvid)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);

    if(VLAN_FUNC_ENABLE == ENABLED)
    {
        if((ret = ca_port_pvid_vlan_get(0,port_id,pPvid))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
            return RT_ERR_FAILED;
        }
    }
    else
    {
        *pPvid = 0;
    }

    return RT_ERR_OK;
} /* end of dal_ca8277b_vlan_portPvid_get */


/* Function Name:
 *      dal_ca8277b_vlan_portPvid_set
 * Description:
 *      Set port default vlan id.
 * Input:
 *      port - port id
 *      pvid - port default vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_VLAN_VID - invalid vid
 * Note:
 *      None
 */
int32
dal_ca8277b_vlan_portPvid_set(rtk_port_t port, uint32 pvid)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);

    if(VLAN_FUNC_ENABLE == ENABLED)
    {
        RT_PARAM_CHK((!DAL_CA8277B_VLAN_IS_SET(pvid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);    

        if((ret = ca_port_pvid_vlan_set(0,port_id,pvid))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
            return RT_ERR_FAILED;
        }
    }
    return RT_ERR_OK;
} /* end of dal_ca8277b_vlan_portPvid_set */

/* Function Name:
 *      dal_ca8277b_vlan_port_get
 * Description:
 *      Get the vlan members.
 * Input:
 *      vid              - vlan id
 * Output:
 *      pMember_portmask - pointer buffer of member ports
 *      pUntag_portmask  - pointer buffer of untagged member ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_ca8277b_vlan_port_get(
    rtk_vlan_t     vid,
    rtk_portmask_t *pMemberPortmask,
    rtk_portmask_t *pUntagPortmask)
{
    ca_status_t ret=CA_E_OK;
    ca_uint8_t member_count=0;
    ca_port_id_t member_ports[RTK_MAX_PORT_ID];
    ca_uint8_t untagged_count=0;
    ca_port_id_t untagged_ports[RTK_MAX_PORT_ID];
    uint32 i;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pMemberPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pUntagPortmask), RT_ERR_NULL_POINTER);

    if(VLAN_FUNC_ENABLE == ENABLED)
    {
        RT_PARAM_CHK((!DAL_CA8277B_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

        if((ret = ca_l2_vlan_port_get(0,vid,&member_count,member_ports,&untagged_count,untagged_ports))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
            return RT_ERR_FAILED;
        }

        RTK_PORTMASK_RESET(*pMemberPortmask);
        RTK_PORTMASK_RESET(*pUntagPortmask);

        for(i=0;i<member_count;i++)
        {
            RTK_PORTMASK_PORT_SET(*pMemberPortmask,PORT_ID(member_ports[i]));
        }

        for(i=0;i<untagged_count;i++)
        {
            RTK_PORTMASK_PORT_SET(*pUntagPortmask,PORT_ID(untagged_ports[i]));
        }
    }
    else
    {
        RTK_PORTMASK_RESET(*pMemberPortmask);
        RTK_PORTMASK_RESET(*pUntagPortmask);
    }

    return RT_ERR_OK;
} /* end of dal_ca8277b_vlan_port_get */


/* Function Name:
 *      dal_ca8277b_vlan_port_set
 * Description:
 *      Replace the vlan members.
 * Input:
 *      vid              - vlan id
 *      pMember_portmask - member ports
 *      pUntag_portmask  - untagged member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Don't care the original vlan members and replace with new configure
 *      directly.
 */
int32
dal_ca8277b_vlan_port_set(
    rtk_vlan_t     vid,
    rtk_portmask_t *pMember_portmask,
    rtk_portmask_t *pUntag_portmask)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_uint8_t member_count=0;
    ca_port_id_t member_ports[RTK_MAX_PORT_ID];
    ca_uint8_t untagged_count=0;
    ca_port_id_t untagged_ports[RTK_MAX_PORT_ID];
    uint32 port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pMember_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pUntag_portmask), RT_ERR_NULL_POINTER);
    
    if(VLAN_FUNC_ENABLE == ENABLED)
    {
        RT_PARAM_CHK((!DAL_CA8277B_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

        for(port=0;port<RTK_MAX_PORT_ID;port++)
        {
            if(RTK_PORTMASK_IS_PORT_SET(*pMember_portmask,port))
            {
                port_id = RTK2CA_PORT_ID(port);
                if(port_id == CAERR_PORT_ID)
                    continue;

                member_ports[member_count++]=port_id;
            }

            if(RTK_PORTMASK_IS_PORT_SET(*pUntag_portmask,port))
            {
                port_id = RTK2CA_PORT_ID(port);
                if(port_id == CAERR_PORT_ID)
                    continue;

                untagged_ports[untagged_count++]=port_id;
            }
        }

        if((ret = ca_l2_vlan_port_set(0,vid,member_count,member_ports,untagged_count,untagged_ports))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
} /* end of dal_ca8277b_vlan_port_set */

/* Function Name:
 *      dal_ca8277b_vlan_vlanFunctionEnable_get
 * Description:
 *      Get the VLAN enable status.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      The status of vlan function is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_ca8277b_vlan_vlanFunctionEnable_get(rtk_enable_t *pEnable)
{
    ca_status_t ret=CA_E_OK;
    rtk_port_t port;
    ca_port_id_t port_id;
    ca_vlan_port_control_t config;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    port = 0;
    port_id = RTK2CA_PORT_ID(port);

    if((ret = ca_l2_vlan_port_control_get(0,port_id,&config))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return RT_ERR_FAILED;
    }
    
    if(config.ingress_membership_check_enable==1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
} /* end of dal_ca8277b_vlan_vlanFunctionEnable_get */


/* Function Name:
 *      dal_ca8277b_vlan_vlanFunctionEnable_set
 * Description:
 *      Set the VLAN enable status.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      The status of vlan function is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_ca8277b_vlan_vlanFunctionEnable_set(rtk_enable_t enable)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_vlan_port_control_t config;
    rtk_port_t port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    uint8 vlan_port_num[]={0,1,2,3,4,6,7,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0xff};

    for (port = 0; port < sizeof(vlan_port_num)/sizeof(uint8); port++)
    {
        if (vlan_port_num[port] == 0xFF)
            break;

        if(vlan_port_num[port] == 6)
        {
            port_id = CA_PORT_ID(CA_PORT_TYPE_ETHERNET,6);
        }
        else if(vlan_port_num[port] == 0x18 || vlan_port_num[port] == 0x19)
        {
            port_id = CA_PORT_ID(CA_PORT_TYPE_L2RP,vlan_port_num[port]);
        }
        else
        {
            if(!HAL_IS_PORT_EXIST(vlan_port_num[port]))
                continue;

            port_id = RTK2CA_PORT_ID(vlan_port_num[port]);
        }

        /* parameter check */
        RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);

        if((ret = ca_l2_vlan_port_control_get(0,port_id,&config))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
            return RT_ERR_FAILED;
        }
        
        if(enable == ENABLED)
        {
            config.drop_unknown_vlan = 1;
            config.ingress_membership_check_enable = 1;
            config.egress_membership_check_enable = 1;
        }
        else
        {
            config.drop_unknown_vlan = 0;
            config.ingress_membership_check_enable = 0;
            config.egress_membership_check_enable = 0;
        }

        if((ret = ca_l2_vlan_port_control_set(0,port_id,&config))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
            return RT_ERR_FAILED;
        }
    }

    aal_l2_vlan_default_cfg_mask_t _mask;
    aal_l2_vlan_default_cfg_t cfg;

    memset(&_mask,0xff,sizeof(aal_l2_vlan_default_cfg_mask_t));
    memset(&cfg,0x00,sizeof(aal_l2_vlan_default_cfg_t));

    if((ret = aal_l2_vlan_default_cfg_get(0,&cfg))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return RT_ERR_FAILED;
    }

    if (config.drop_unknown_vlan)
    {
        cfg.tx_unknown_vlan_memship_check_bmp &= ~(1 << HAL_GET_PON_PORT());
        cfg.rx_unknown_vlan_memship_check_bmp &= ~(1 << HAL_GET_PON_PORT());
    }
    else
    {
        cfg.tx_unknown_vlan_memship_check_bmp |= (1<<HAL_GET_PON_PORT());
        cfg.rx_unknown_vlan_memship_check_bmp |= (1<<HAL_GET_PON_PORT());
    }

    if((ret = aal_l2_vlan_default_cfg_set(0,_mask,&cfg))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return RT_ERR_FAILED;
    }

    if(VLAN_FUNC_ENABLE == ENABLED && enable != ENABLED)
    {
        if((ret = ca_l2_vlan_delete_all(0))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
            return RT_ERR_FAILED;
        }
        /* Set PVID */
        HAL_SCAN_ALL_PORT(port)
        {
            port_id = RTK2CA_PORT_ID(port);

            if((ret = ca_port_pvid_vlan_set(0,port_id,0))!=CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
                return RT_ERR_FAILED;
            }
        }
    }

    VLAN_FUNC_ENABLE = enable;

    return RT_ERR_OK;
} /* end of dal_ca8277b_vlan_vlanFunctionEnable_set */

/* Function Name:
 *      dal_ca8277b_vlan_fidMode_get
 * Description:
 *      Get the filter id mode of the vlan.
 * Input:
 *      vid  - vlan id
 * Output:
 *      pMode - pointer buffer of filter id mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      mode can be: -VLAN__FID_IVL
 *                   -VLAN__FID_SVL
 */
int32
dal_ca8277b_vlan_fidMode_get(rtk_vlan_t vid, rtk_fidMode_t *pMode)
{
    ca_status_t ret=CA_E_OK;
    ca_vlan_learning_mode_t ca_mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    if((ret = ca_l2_vlan_learning_shared_get(0,&ca_mode))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return RT_ERR_FAILED;
    }

    switch(ca_mode)
    {
        case CA_L2_VLAN_LEARNING_IVL:
            *pMode = VLAN_FID_IVL;
            break;
        case CA_L2_VLAN_LEARNING_SVL:
            *pMode = VLAN_FID_SVL;
            break;
        default:
            RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_ca8277b_vlan_fidMode_get */

/* Function Name:
 *      dal_ca8277b_vlan_fidMode_set
 * Description:
 *      Set the filter id mode of the vlan.
 * Input:
 *      vid   - vlan id
 *      mode  - filter id mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 * Note:
 *      mode can be: -VLAN__FID_IVL
 *                   -VLAN__FID_SVL
 */
int32
dal_ca8277b_vlan_fidMode_set(rtk_vlan_t vid, rtk_fidMode_t mode)
{
    ca_status_t ret=CA_E_OK;
    ca_vlan_learning_mode_t ca_mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((VLAN_FID_MODE_END <= mode), RT_ERR_INPUT);

    switch(mode)
    {
        case VLAN_FID_IVL:
            ca_mode = CA_L2_VLAN_LEARNING_IVL;
            break;
        case VLAN_FID_SVL:
            ca_mode = CA_L2_VLAN_LEARNING_SVL;
            break;
        default:
            RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
            return RT_ERR_FAILED;
    }
    
    if((ret = ca_l2_vlan_learning_shared_set(0,ca_mode))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_ca8277b_vlan_fidMode_set */

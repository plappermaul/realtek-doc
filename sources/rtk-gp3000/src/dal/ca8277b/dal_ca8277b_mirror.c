/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (?±ä?, 03 äº”æ? 2013) $
 *
 * Purpose : Definition of Mirror API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Port-based mirror
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <dal/ca8277b/dal_ca8277b.h>
#include <dal/ca8277b/dal_ca8277b_mirror.h>
#include <osal/time.h>

#include <scfg.h>
#include <aal_pon.h>
#include <aal_mir.h>
#include <cortina-api/include/mirror.h>

/*
 * Symbol Definition
 */
static uint32 mirror_init = INIT_NOT_COMPLETED;

uint32 g_mirror_tx=FALSE;
uint32 g_mirror_rx=FALSE;
rtk_port_t g_mirroringPort;
rtk_portmask_t g_mirroredRxPortmask;
rtk_portmask_t g_mirroredTxPortmask;

 /*
 * Macro Declaration
 */
#define CPU_PORT_ID_FOR_MIRROR (0x10)
#define LSPID_FROM_CPU_TO_PON (0x11)
#define LDPID_OF_OMCI_UPSTREAM (0xf)
#define LDPID_OF_OAM_UPSTREAM (0xf)

/*
 * Function Declaration
 */

/* Module Name : Mirror */

/* Function Name:
 *      dal_ca8277b_mirror_init
 * Description:
 *      Initialize the mirroring database.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      Must initialize Mirror module before calling any Mirror APIs.
 */
int32
dal_ca8277b_mirror_init(void)
{
    ca_status_t ret = CA_E_OK;
    aal_mire_mir_rule_mask_t rule_mask;
    aal_mire_mir_rule_t mire_rule;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR),"%s",__FUNCTION__);

    memset (&rule_mask, 0, sizeof(aal_mire_mir_rule_mask_t));
    memset (&mire_rule, 0, sizeof(aal_mire_mir_rule_t));

    rule_mask.bf.mode = 1;
    mire_rule.mode = MIR_DISABLE;

    if((ret = aal_mire_rule_set(0, 0, rule_mask, &mire_rule))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return RT_ERR_FAILED;
    }

    if((ret = aal_mire_rule_set(0, 1, rule_mask, &mire_rule))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return RT_ERR_FAILED;
    }

    if((ret = aal_mire_rule_set(0, 2, rule_mask, &mire_rule))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return RT_ERR_FAILED;
    }

    if((ret = aal_mire_rule_set(0, 3, rule_mask, &mire_rule))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return RT_ERR_FAILED;
    }

    g_mirror_tx = FALSE;
    g_mirror_rx = FALSE;

    mirror_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_ca8277b_mirror_init */

/* Function Name:
 *      dal_ca8277b_mirror_portBased_get
 * Description:
 *      Get port mirror function.
 * Input:
 *      None
 * Output:
 *      pMirroringPort        - Monitor port.
 *      pMirroredRxPortmask   - Rx mirror port mask.
 *      pMirroredTxPortmask   - Tx mirror port mask.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API is to get mirror function of source port and mirror port.
 */
int32
dal_ca8277b_mirror_portBased_get(rtk_port_t *pMirroringPort, rtk_portmask_t *pMirroredRxPortmask, rtk_portmask_t *pMirroredTxPortmask)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(mirror_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMirroringPort), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMirroredRxPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMirroredTxPortmask), RT_ERR_NULL_POINTER);

    if(g_mirror_tx == TRUE || g_mirror_rx == TRUE)
    {
        *pMirroringPort = g_mirroringPort;
        RTK_PORTMASK_ASSIGN((*pMirroredRxPortmask),g_mirroredRxPortmask);
        RTK_PORTMASK_ASSIGN((*pMirroredTxPortmask),g_mirroredTxPortmask);
    }
    else
    {
        *pMirroringPort = 0;
        pMirroredRxPortmask->bits[0] = 0;
        pMirroredTxPortmask->bits[0] = 0;
    }

    return RT_ERR_OK;
} /* end of dal_ca8277b_mirror_portBased_get */

/* Function Name:
 *      dal_ca8277b_mirror_portBased_set
 * Description:
 *      Set port mirror function.
 * Input:
 *      mirroringPort         - Monitor port.
 *      pMirroredRxPortmask   - Rx mirror port mask.
 *      pMirroredTxPortmask   - Tx mirror port mask.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port number
 *      RT_ERR_PORT_MASK    - Invalid portmask.
 * Note:
 *      The API is to set mirror function of source port and mirror port.
 *      The mirror port can only be set to one port and the TX and RX mirror ports
 *      should be identical.
 */
int32
dal_ca8277b_mirror_portBased_set(rtk_port_t mirroringPort, rtk_portmask_t *pMirroredRxPortmask, rtk_portmask_t *pMirroredTxPortmask)
{
    ca_status_t ret = CA_E_OK;
    aal_mire_mir_rule_mask_t rule_mask;
    aal_mire_mir_rule_t mire_rule;
    aal_mire_mir_act_mask_t act_mask;
    aal_mire_mir_act_t act;
    rtk_port_t mirroredPort;
    uint8 isRx=0,isTx=0;
    uint32 ldpid_of_pon=0xFFFFFFFF ;
    ca_uint32_t pon_mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(mirror_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(mirroringPort), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMirroredRxPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMirroredTxPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID((*pMirroredRxPortmask)), RT_ERR_PORT_MASK);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID((*pMirroredTxPortmask)), RT_ERR_PORT_MASK);

    if(RTK_PORTMASK_GET_PORT_COUNT(*pMirroredTxPortmask) > 1)
    {
        RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR),"%s",__FUNCTION__);
        return RT_ERR_PORT_ID;
    }

    if(RTK_PORTMASK_GET_PORT_COUNT(*pMirroredRxPortmask) > 1)
    {
        RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR),"%s",__FUNCTION__);
        return RT_ERR_PORT_ID;
    }
    
    memset (&rule_mask, 0, sizeof(aal_mire_mir_rule_mask_t));
    memset (&mire_rule, 0, sizeof(aal_mire_mir_rule_t));

    rule_mask.bf.mode = 1;
    mire_rule.mode = MIR_DISABLE;

    if((ret = aal_mire_rule_set(0, 0, rule_mask, &mire_rule))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return RT_ERR_FAILED;
    }

    if((ret = aal_mire_rule_set(0, 1, rule_mask, &mire_rule))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return RT_ERR_FAILED;
    }

    if((ret = aal_mire_rule_set(0, 2, rule_mask, &mire_rule))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return RT_ERR_FAILED;
    }

    if((ret = aal_mire_rule_set(0, 3, rule_mask, &mire_rule))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return RT_ERR_FAILED;
    }

    g_mirror_tx = FALSE;
    g_mirror_rx = FALSE;

    if((RTK_PORTMASK_GET_PORT_COUNT(*pMirroredTxPortmask)==0) && (RTK_PORTMASK_GET_PORT_COUNT(*pMirroredRxPortmask)==0))
    {   
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
        return RT_ERR_OK;
    }
    else if((RTK_PORTMASK_GET_PORT_COUNT(*pMirroredTxPortmask)!=0) && (RTK_PORTMASK_GET_PORT_COUNT(*pMirroredRxPortmask)==0))
    {
        isTx = 1;
        mirroredPort = RTK_PORTMASK_GET_FIRST_PORT(*pMirroredTxPortmask);

    }
    else if((RTK_PORTMASK_GET_PORT_COUNT(*pMirroredTxPortmask)==0) && (RTK_PORTMASK_GET_PORT_COUNT(*pMirroredRxPortmask)!=0))
    {
        isRx = 1;
        mirroredPort = RTK_PORTMASK_GET_FIRST_PORT(*pMirroredRxPortmask);
    }
    else
    {
        isTx = 1;
        isRx = 1;
        if(RTK_PORTMASK_GET_FIRST_PORT(*pMirroredTxPortmask) != RTK_PORTMASK_GET_FIRST_PORT(*pMirroredRxPortmask))
        {
            RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR),"%s",__FUNCTION__);
            return RT_ERR_PORT_ID;
        }
        else
        {
            mirroredPort = RTK_PORTMASK_GET_FIRST_PORT(*pMirroredTxPortmask);
        }
    }

    if(isRx == 1)
    {
        memset (&rule_mask, 0, sizeof(aal_mire_mir_rule_mask_t));
        memset (&mire_rule, 0, sizeof(aal_mire_mir_rule_t));

        rule_mask.bf.mode = 1;
        rule_mask.bf.lspid = 1;
        mire_rule.mode = MIR_INGRESS_PORT;
        mire_rule.lspid = mirroredPort;

        if((ret = aal_mire_rule_set(0, 0, rule_mask, &mire_rule))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
            goto err;
        }

        memset (&act_mask, 0, sizeof(aal_mire_mir_act_mask_t));
        memset (&act, 0, sizeof(aal_mire_mir_act_t));
        act_mask.bf.ldpid = 1;
        act.ldpid = mirroringPort;

        if((ret = aal_mire_act_set(0, 0, act_mask, &act))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
            goto err;
        }

        if(mirroredPort == CPU_PORT_ID_FOR_MIRROR)
        {
            //mirror CPU to PON which lspid=LSPID_FROM_CPU_TO_PON
            memset (&rule_mask, 0, sizeof(aal_mire_mir_rule_mask_t));
            memset (&mire_rule, 0, sizeof(aal_mire_mir_rule_t));
    
            rule_mask.bf.mode = 1;
            rule_mask.bf.lspid = 1;
            mire_rule.mode = MIR_INGRESS_PORT;
            mire_rule.lspid = LSPID_FROM_CPU_TO_PON;
    
            if((ret = aal_mire_rule_set(0, 2, rule_mask, &mire_rule))!=CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
                goto err;
            }
    
            memset (&act_mask, 0, sizeof(aal_mire_mir_act_mask_t));
            memset (&act, 0, sizeof(aal_mire_mir_act_t));
            act_mask.bf.ldpid = 1;
            act.ldpid = mirroringPort;
    
            if((ret = aal_mire_act_set(0, 2, act_mask, &act))!=CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
                goto err;
            }
        }

        g_mirror_rx = TRUE;            
    }

    if(isTx == 1)
    {
        memset (&rule_mask, 0, sizeof(aal_mire_mir_rule_mask_t));
        memset (&mire_rule, 0, sizeof(aal_mire_mir_rule_t));

        rule_mask.bf.mode = 1;
        rule_mask.bf.ldpid = 1;
        mire_rule.mode = MIR_EGRESS_PORT;
        mire_rule.ldpid = mirroredPort;

        if((ret = aal_mire_rule_set(0, 1, rule_mask, &mire_rule))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
            goto err;
        }

        memset (&act_mask, 0, sizeof(aal_mire_mir_act_mask_t));
        memset (&act, 0, sizeof(aal_mire_mir_act_t));
        act_mask.bf.ldpid = 1;
        act.ldpid = mirroringPort;

        if((ret = aal_mire_act_set(0, 1, act_mask, &act))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
            goto err;
        }

        if(HAL_IS_PON_PORT(mirroredPort))
        {   //mirror CPU to PON which lspid=LSPID_FROM_CPU_TO_PON
            memset (&rule_mask, 0, sizeof(aal_mire_mir_rule_mask_t));
            memset (&mire_rule, 0, sizeof(aal_mire_mir_rule_t));
    
            rule_mask.bf.mode = 1;
            rule_mask.bf.lspid = 1;
            mire_rule.mode = MIR_INGRESS_PORT;
            mire_rule.lspid = LSPID_FROM_CPU_TO_PON;
    
            if((ret = aal_mire_rule_set(0, 2, rule_mask, &mire_rule))!=CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
                goto err;
            }
    
            memset (&act_mask, 0, sizeof(aal_mire_mir_act_mask_t));
            memset (&act, 0, sizeof(aal_mire_mir_act_t));
            act_mask.bf.ldpid = 1;
            act.ldpid = mirroringPort;
    
            if((ret = aal_mire_act_set(0, 2, act_mask, &act))!=CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
                goto err;
            }

            // mirror OAM/OMCI on PON port egress direction
            pon_mode = aal_pon_mac_mode_get(0);
            if(CA_PON_MODE_IS_GPON_FAMILY(pon_mode))
                ldpid_of_pon = LDPID_OF_OMCI_UPSTREAM;
            else if(CA_PON_MODE_IS_EPON_FAMILY(pon_mode))
                ldpid_of_pon = LDPID_OF_OAM_UPSTREAM;
            
            if(0xFFFFFFFF != ldpid_of_pon)
            {
                memset (&rule_mask, 0, sizeof(aal_mire_mir_rule_mask_t));
                memset (&mire_rule, 0, sizeof(aal_mire_mir_rule_t));
    
                rule_mask.bf.mode = 1;
                rule_mask.bf.ldpid = 1;
                mire_rule.mode = MIR_EGRESS_PORT;
                mire_rule.ldpid = ldpid_of_pon;
    
                if((ret = aal_mire_rule_set(0, 3, rule_mask, &mire_rule))!=CA_E_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
                    goto err;
                }
    
                memset (&act_mask, 0, sizeof(aal_mire_mir_act_mask_t));
                memset (&act, 0, sizeof(aal_mire_mir_act_t));
                act_mask.bf.ldpid = 1;
                act.ldpid = mirroringPort;
    
                if((ret = aal_mire_act_set(0, 3, act_mask, &act))!=CA_E_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
                    goto err;
                }
            }
        }
		
        g_mirror_tx = TRUE;            
    }

    if(g_mirror_tx == TRUE || g_mirror_rx == TRUE)
    {
        g_mirroringPort = mirroringPort;
        RTK_PORTMASK_ASSIGN(g_mirroredRxPortmask,(*pMirroredRxPortmask));
        RTK_PORTMASK_ASSIGN(g_mirroredTxPortmask,(*pMirroredTxPortmask));
    }

    return RT_ERR_OK;

err: //disable all mirror rules
    memset (&rule_mask, 0, sizeof(aal_mire_mir_rule_mask_t));
    memset (&mire_rule, 0, sizeof(aal_mire_mir_rule_t));

    rule_mask.bf.mode = 1;
    mire_rule.mode = MIR_DISABLE;

    if((ret = aal_mire_rule_set(0, 0, rule_mask, &mire_rule))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
    }

    if((ret = aal_mire_rule_set(0, 1, rule_mask, &mire_rule))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
    }

    if((ret = aal_mire_rule_set(0, 2, rule_mask, &mire_rule))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
    }

    if((ret = aal_mire_rule_set(0, 3, rule_mask, &mire_rule))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
    }

    g_mirror_tx = FALSE;
    g_mirror_rx = FALSE;
	
    return RT_ERR_FAILED;
} /* end of dal_ca8277b_mirror_portBased_set */

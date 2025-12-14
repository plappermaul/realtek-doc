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
 * $Date: 2013-05-03 17:35:27 +0800 (週五, 03 五月 2013) $
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
#include <dal/rtl8198f/dal_rtl8198f.h>
#include <dal/rtl8198f/dal_rtl8198f_mirror.h>
#include <osal/time.h>

#include <scfg.h>
#include <cortina-api/include/mirror.h>
#include <cortina-api/include/classifier.h>

#include <aal/include/aal_mir.h>

/*
 * Symbol Definition
 */
static uint32 mirror_init = INIT_NOT_COMPLETED;
static uint32 mirror_vlan = 2018;

uint32 g_mirror_tx=FALSE;
uint32 g_mirror_rx=FALSE;
uint32 g_mirror_type;
rtk_port_t g_mirroringPort;
rtk_portmask_t g_mirroredRxPortmask;
rtk_portmask_t g_mirroredTxPortmask;

 /*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Module Name : Mirror */

/* Function Name:
 *      dal_rtl8198f_mirror_init
 * Description:
 *      Initialize the mirroring database.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must initialize Mirror module before calling any Mirror APIs.
 */
int32
dal_rtl8198f_mirror_init(void)
{
    ca_status_t ret = CA_E_OK;
    ca_device_config_tlv_t scfg_tlv;
    ca_uint32_t vlan = 4018;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR),"%s",__FUNCTION__);

    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    scfg_tlv.type = CA_CFG_ID_MIRROR_VLAN;
    scfg_tlv.len  = sizeof(ca_uint32_t);

    if((ret = ca_device_config_tlv_get(0, &scfg_tlv))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }

    memcpy(&vlan, scfg_tlv.value, sizeof(ca_uint32_t));
    mirror_vlan = vlan;

    mirror_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_rtl8198f_mirror_init */

/* Function Name:
 *      dal_rtl8198f_mirror_portBased_get
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
dal_rtl8198f_mirror_portBased_get(rtk_port_t *pMirroringPort, rtk_portmask_t *pMirroredRxPortmask, rtk_portmask_t *pMirroredTxPortmask)
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
} /* end of dal_rtl8198f_mirror_portBased_get */

/* Function Name:
 *      dal_rtl8198f_mirror_portBased_set
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
dal_rtl8198f_mirror_portBased_set(rtk_port_t mirroringPort, rtk_portmask_t *pMirroredRxPortmask, rtk_portmask_t *pMirroredTxPortmask)
{
    ca_status_t ret = CA_E_OK;
    aal_mire_mir_rule_mask_t rule_mask;
    aal_mire_mir_rule_t mire_rule;
    aal_mire_mir_act_mask_t act_mask;
    aal_mire_mir_act_t act;
    rtk_port_t mirroredPort;
    uint8 isRx=0,isTx=0;

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

    printk("%s %d isTx=%d isRx=%d mirroredPort=%d\n",__FUNCTION__,__LINE__,isTx,isRx,mirroredPort);
    g_mirror_type = dal_rtl8198f_mirror_chk_type(mirroringPort, pMirroredRxPortmask, pMirroredTxPortmask);

    if (g_mirror_type == MIRROR_8198F_EMBEDDED) {
        /* 8198F embedded port mirror */
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
                return RT_ERR_FAILED;
            }

            memset (&act_mask, 0, sizeof(aal_mire_mir_act_mask_t));
            memset (&act, 0, sizeof(aal_mire_mir_act_t));

            if ((aal_mire_act_get(0, 0, &act))!=CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
                return RT_ERR_FAILED;
            }

            act_mask.bf.ldpid = 1;
            act.ldpid = mirroringPort;

            if((ret = aal_mire_act_set(0, 0, act_mask, &act))!=CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
                return RT_ERR_FAILED;
            }

            g_mirror_rx = TRUE;            
        }
        else /*except PON oport,other port only support RX mirror at CA8277*/
        {
            RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR),"%s",__FUNCTION__);
            return RT_ERR_PORT_ID;
        }
    } /* 8198F embedded mirror */
    else if (g_mirror_type == MIRROR_8198F_EXTERNAL) {
        /* RTL8367 LAN to LAN mirror */
        rtk_port_t rtk_83xx_port = dal_rtl8198f_lspid_mapping(mirroringPort);
        rtk_portmask_t rtk_83xx_rx_mask;
        rtk_portmask_t rtk_83xx_tx_mask;

        rtk_83xx_rx_mask.bits[0] = dal_rtl8198f_mirror_lspid_pmask(pMirroredRxPortmask->bits[0]);
        rtk_83xx_tx_mask.bits[0] = dal_rtl8198f_mirror_lspid_pmask(pMirroredTxPortmask->bits[0]);

        ret = rtk_83xx_mirror_portBased_set(rtk_83xx_port, &rtk_83xx_rx_mask, &rtk_83xx_tx_mask);

        if (ret != RT_ERR_OK) {
	    RT_ERR(ret, (MOD_DAL|MOD_MIRROR), "");
	    return RT_ERR_FAILED;
        }

        g_mirror_rx = TRUE;
    }
    else {
        RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MIRROR),"%s",__FUNCTION__);
        return RT_ERR_PORT_ID;
    }

    if(g_mirror_tx == TRUE || g_mirror_rx == TRUE)
    {
        g_mirroringPort = mirroringPort;
        RTK_PORTMASK_ASSIGN(g_mirroredRxPortmask,(*pMirroredRxPortmask));
        RTK_PORTMASK_ASSIGN(g_mirroredTxPortmask,(*pMirroredTxPortmask));
    }

    return RT_ERR_OK;
} /* end of dal_rtl8198f_mirror_portBased_set */

int32
dal_rtl8198f_mirror_chk_type(rtk_port_t mirroringPort, rtk_portmask_t *pMirroredRxPortmask, rtk_portmask_t *pMirroredTxPortmask)
{
	uint32 rtl8198f_board = rtl_get_board_type();

	if ((rtl8198f_board & 0xF00) == 0x600) {
		if (mirroringPort > 1 &&
				(pMirroredRxPortmask->bits[0] > 2 ||
				pMirroredTxPortmask->bits[0] > 2)) {
			return MIRROR_8198F_EXTERNAL;
		}
		else {
			return MIRROR_8198F_EMBEDDED;
		}
	}
	else if ((rtl8198f_board & 0xF00) == 0x500) {
		if (mirroringPort == 1 || pMirroredRxPortmask->bits[0] == 2) {
			return MIRROR_8198F_EMBEDDED;
		}

		return MIRROR_8198F_EXTERNAL;
	}
	else
		return MIRROR_8198F_EXTERNAL;
}

int32 dal_rtl8198f_mirror_lspid_pmask(uint32 port_mask_bit)
{
	uint32 rtl8198f_board = rtl_get_board_type();

	if (((rtl8198f_board & 0xF00) == 0x500) && port_mask_bit == 0)
		return port_mask_bit << 1;
	else if (((rtl8198f_board & 0xF00) == 0x600) && port_mask_bit > 1)
		return port_mask_bit >> 2;
	else
		return port_mask_bit;
}

/*
 * Copyright (C) 2013 Realtek Semiconductor Corp. 
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
 * $Date: 2013-05-03 17:35:27 +0800 (星期五, 03 五月 2013) $
 *
 * Purpose : Definition of OAM Global API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) OAM (802.3ah) configuration
 *
 */




/*
 * Include Files
 */
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <common/rt_type.h>



/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */
 
/* Function Name:
 *      rtk_oam_init
 * Description:
 *      Initialize oam module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize oam module before calling any oam APIs.
 */
int32
rtk_oam_init(void)
{
    rtdrv_oamCfg_t oam_cfg;

    /* function body */
    osal_memset(&oam_cfg,0x0,sizeof(rtdrv_oamCfg_t));
    SETSOCKOPT(RTDRV_OAM_INIT, &oam_cfg, rtdrv_oamCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_init */


/* Module Name : OAM */

/* Function Name:
 *      rtk_oam_parserAction_set
 * Description:
 *      Set OAM parser action
 * Input:
 *      port    - port id
 *      action  - parser action 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
rtk_oam_parserAction_set(rtk_port_t port, rtk_oam_parser_act_t action)
{
    rtdrv_oamCfg_t oam_cfg;

     osal_memset(&oam_cfg, 0x0, sizeof(rtdrv_oamCfg_t));
    /* function body */
    osal_memcpy(&oam_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&oam_cfg.action, &action, sizeof(rtk_oam_parser_act_t));
    SETSOCKOPT(RTDRV_OAM_PARSERACTION_SET, &oam_cfg, rtdrv_oamCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_parserAction_set */

/* Function Name:
 *      rtk_oam_parserAction_set
 * Description:
 *      Get OAM parser action
 * Input:
 *      port    - port id
 * Output:
 *      pAction  - parser action 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
rtk_oam_parserAction_get(rtk_port_t port, rtk_oam_parser_act_t *pAction)
{
    rtdrv_oamCfg_t oam_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&oam_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_OAM_PARSERACTION_GET, &oam_cfg, rtdrv_oamCfg_t, 1);
    osal_memcpy(pAction, &oam_cfg.action, sizeof(rtk_oam_parser_act_t));

    return RT_ERR_OK;
}   /* end of rtk_oam_parserAction_get */


/* Function Name:
 *      rtk_oam_multiplexerAction_set
 * Description:
 *      Set OAM multiplexer action
 * Input:
 *      port    - port id
 *      multAction  - multiplexer action 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
rtk_oam_multiplexerAction_set(rtk_port_t port, rtk_oam_multiplexer_act_t multAction)
{
    rtdrv_oamCfg_t oam_cfg;

    osal_memset(&oam_cfg,0x0,sizeof(rtdrv_oamCfg_t));
    /* function body */
    osal_memcpy(&oam_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&oam_cfg.multAction, &multAction, sizeof(rtk_oam_multiplexer_act_t));
    SETSOCKOPT(RTDRV_OAM_MULTIPLEXERACTION_SET, &oam_cfg, rtdrv_oamCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_oam_multiplexerAction_set */

/* Function Name:
 *      rtk_oam_multiplexerAction_set
 * Description:
 *      Get OAM multiplexer action
 * Input:
 *      port    - port id
 * Output:
 *      pMultAction  - multiplexer action 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
rtk_oam_multiplexerAction_get(rtk_port_t port, rtk_oam_multiplexer_act_t *pMultAction)
{
    rtdrv_oamCfg_t oam_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMultAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&oam_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_OAM_MULTIPLEXERACTION_GET, &oam_cfg, rtdrv_oamCfg_t, 1);
    osal_memcpy(pMultAction, &oam_cfg.multAction, sizeof(rtk_oam_multiplexer_act_t));

    return RT_ERR_OK;
}   /* end of rtk_oam_multiplexerAction_get */



#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <rtk/rt/rt_port.h>
#include <hal/common/halctrl.h>

/*
 * rt_port init
 */
cparser_result_t
cparser_cmd_rt_port_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_port_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_init */

/*
 * rt_port get status port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_rt_port_get_status_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    rt_port_linkStatus_t linkStatus;
    rt_port_speed_t  speed;
    rt_port_duplex_t duplex;
    uint32 txfc;
    uint32 rxfc;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port Status Speed    Duplex TX_FC RX_FC\n");
    diag_util_mprintf("---- ------ -----    ------ ----- -----");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_port_link_get(port, &linkStatus), ret);
        DIAG_UTIL_ERR_CHK(rt_port_speedDuplex_get(port, &speed, &duplex), ret);
        DIAG_UTIL_ERR_CHK(rt_port_flowctrl_get(port, &txfc, &rxfc), ret);

        diag_util_mprintf("\n%-4d %-6s %-8s %-6s %-5s %-5s",
                          port,
                          diagStr_portLinkStatus[linkStatus],
                          diagStr_portSpeed[speed],
                          diagStr_portDuplex[duplex],
                          diagStr_enDisplay[txfc],
                          diagStr_enDisplay[rxfc]);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_get_status_port_ports_all */

/*
 * rt_port get auto-nego port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_rt_port_get_auto_nego_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    rt_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
#if defined(CONFIG_LUNA_G3_SERIES)
        DIAG_UTIL_ERR_CHK(rt_port_phyAutoNegoEnable_get(port, &state), ret);
        diag_util_printf("\n Port %d Auto-Nego state: %s", port, diagStr_enable[state]);
#else
        if(HAL_IS_PHY_EXIST(port))
        {
            DIAG_UTIL_ERR_CHK(rt_port_phyAutoNegoEnable_get(port, &state), ret);
            diag_util_printf("\n Port %d Auto-Nego state: %s", port, diagStr_enable[state]);
        }
        else
            diag_util_printf("\n Port %d << No PHY embedded >>", port);
#endif
    }

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_get_auto_nego_port_ports_all_state */

/*
 * rt_port set auto-nego port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_port_set_auto_nego_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    rt_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('e' == TOKEN_CHAR(6, 0))
        state = ENABLED;
    else
        state = DISABLED;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
#if defined(CONFIG_LUNA_G3_SERIES)
        DIAG_UTIL_ERR_CHK(rt_port_phyAutoNegoEnable_set(port, state), ret);
#else
        if(HAL_IS_PHY_EXIST(port))
        {
            DIAG_UTIL_ERR_CHK(rt_port_phyAutoNegoEnable_set(port, state), ret);
        }
        else
            diag_util_printf("\n There is no embedded PHY at Port %d", port);
#endif
    }

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_set_auto_nego_port_ports_all_state_disable_enable */

/*
 * rt_port get auto-nego port ( <PORT_LIST:ports> | all ) ability
 */
cparser_result_t
cparser_cmd_rt_port_get_auto_nego_port_ports_all_ability(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    rt_port_phy_ability_t ability;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port 10000F 5000F 2500F 1000F 100F 100H 10F 10H FlowControl AsyFC\n");
    diag_util_mprintf("---- ------ ----- ----- ----- ---- ---- --- --- ----------- -----\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
#if defined(CONFIG_LUNA_G3_SERIES)
        DIAG_UTIL_ERR_CHK(rt_port_phyAutoNegoAbility_get(port, &ability), ret);
        diag_util_mprintf("%-4d %-6s %-5s %-5s %-5s %-4s %-4s %-3s %-3s %-11s %-5s\n",
                            port,
                            diagStr_enDisplay[ability.Full_10000],
                            diagStr_enDisplay[ability.Full_5000],
                            diagStr_enDisplay[ability.Full_2500],
                            diagStr_enDisplay[ability.Full_1000],
                            diagStr_enDisplay[ability.Full_100],
                            diagStr_enDisplay[ability.Half_100],
                            diagStr_enDisplay[ability.Full_10],
                            diagStr_enDisplay[ability.Half_10],
                            diagStr_enDisplay[ability.FC],
                            diagStr_enDisplay[ability.AsyFC]);
#else
        if(HAL_IS_PHY_EXIST(port))
        {
            DIAG_UTIL_ERR_CHK(rt_port_phyAutoNegoAbility_get(port, &ability), ret);
            diag_util_mprintf("%-4d %-6s %-5s %-5s %-5s %-4s %-4s %-3s %-3s %-11s %-5s\n",
                                port,
                                diagStr_enDisplay[ability.Full_10000],
                                diagStr_enDisplay[ability.Full_5000],
                                diagStr_enDisplay[ability.Full_2500],
                                diagStr_enDisplay[ability.Full_1000],
                                diagStr_enDisplay[ability.Full_100],
                                diagStr_enDisplay[ability.Half_100],
                                diagStr_enDisplay[ability.Full_10],
                                diagStr_enDisplay[ability.Half_10],
                                diagStr_enDisplay[ability.FC],
                                diagStr_enDisplay[ability.AsyFC]);
        }
        else
        {
            diag_util_mprintf("%-4d  << No PHY embedded >> \n", port);
        }
#endif
    }

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_get_auto_nego_port_ports_all_ability */

/*
 * rt_port set auto-nego port ( <PORT_LIST:ports> | all ) ability { 10h } { 10f } { 100h } { 100f } { 1000f } { 2500f } { 5000f } { 10000f } { flow-control } { asy-flow-control }
 */
cparser_result_t
cparser_cmd_rt_port_set_auto_nego_port_ports_all_ability_10h_10f_100h_100f_1000f_2500f_5000f_10000f_flow_control_asy_flow_control(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    uint32 token;
    rt_port_phy_ability_t ability;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    osal_memset(&ability, 0x00, sizeof(rt_port_phy_ability_t));

    for(token = 6; token < TOKEN_NUM(); token++)
    {
        if(strcmp("10h", TOKEN_STR(token)) == 0)
            ability.Half_10 = ENABLED;
        else if(strcmp("10f", TOKEN_STR(token)) == 0)
            ability.Full_10 = ENABLED;
        else if(strcmp("100h", TOKEN_STR(token)) == 0)
            ability.Half_100 = ENABLED;
        else if(strcmp("100f", TOKEN_STR(token)) == 0)
            ability.Full_100 = ENABLED;
        else if(strcmp("1000f", TOKEN_STR(token)) == 0)
            ability.Full_1000 = ENABLED;
        else if(strcmp("2500f", TOKEN_STR(token)) == 0)
            ability.Full_2500 = ENABLED;
        else if(strcmp("5000f", TOKEN_STR(token)) == 0)
            ability.Full_5000 = ENABLED;
        else if(strcmp("10000f", TOKEN_STR(token)) == 0)
            ability.Full_10000 = ENABLED;
        else if(strcmp("flow-control", TOKEN_STR(token)) == 0)
            ability.FC = ENABLED;
        else if(strcmp("asy-flow-control", TOKEN_STR(token)) == 0)
            ability.AsyFC = ENABLED;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
#if defined(CONFIG_LUNA_G3_SERIES)
        DIAG_UTIL_ERR_CHK(rt_port_phyAutoNegoAbility_set(port, &ability), ret);
#else
        if(HAL_IS_PHY_EXIST(port))
        {
            DIAG_UTIL_ERR_CHK(rt_port_phyAutoNegoAbility_set(port, &ability), ret);
        }
        else
            diag_util_printf("\n There is no embedded PHY at Port %d", port);
#endif
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_set_auto_nego_port_ports_all_ability_10h_10f_100h_100f_1000f_10000f_flow_control_asy_flow_control */

/*
 * rt_port get phy-force port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_rt_port_get_phy_force_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    rt_port_speed_t    speed;
    rt_port_duplex_t   duplex;
    rt_enable_t        flowControl;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_printf("\nPort Speed Duplex FC ");
    diag_util_printf("\n---- ----- ------ -------");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_port_phyForceModeAbility_get(port, &speed,&duplex,&flowControl), ret);

        diag_util_printf("\n%-4d %-5s %-6s %-7s",
                         port,
                         diagStr_portSpeed[speed],
                         diagStr_portDuplex[duplex],
                         diagStr_enDisplay[flowControl]);

    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_get_phy_force_port_ports_all */

/*
 * rt_port set phy-force port ( <PORT_LIST:ports> | all ) ability ( 10h | 10f | 100h | 100f | 1000f | 10000f ) flow-control ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_port_set_phy_force_port_ports_all_ability_10h_10f_100h_100f_1000f_10000f_flow_control_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rt_port_t port = 0;
    rt_port_speed_t    speed = RT_PORT_SPEED_1000M;
    rt_port_duplex_t   duplex = RT_PORT_FULL_DUPLEX;
    rt_enable_t        flowControl = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);


    if(!osal_strcmp(TOKEN_STR(6),"10h"))
    {
        duplex = RT_PORT_HALF_DUPLEX;
        speed = RT_PORT_SPEED_10M;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"10f"))
    {
        duplex = RT_PORT_FULL_DUPLEX;
        speed = RT_PORT_SPEED_10M;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"100h"))
    {
        duplex = RT_PORT_HALF_DUPLEX;
        speed = RT_PORT_SPEED_100M;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"100f"))
    {
        duplex = RT_PORT_FULL_DUPLEX;
        speed = RT_PORT_SPEED_100M;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"1000f"))
    {
        duplex = RT_PORT_FULL_DUPLEX;
        speed = RT_PORT_SPEED_1000M;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"10000f"))
    {
        duplex = RT_PORT_FULL_DUPLEX;
        speed = RT_PORT_SPEED_10G;
    }

    if ('e' == TOKEN_CHAR(8,0))
    {
        flowControl = ENABLED;
   }
    else if ('d' == TOKEN_CHAR(8,0))
    {
        flowControl = DISABLED;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_port_phyForceModeAbility_set(port,speed,duplex,flowControl), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_set_phy_force_port_ports_all_ability_10h_10f_100h_100f_1000f_10000f_flow_control_disable_enable */

/*
 * rt_port get phy-reg port ( <PORT_LIST:ports> | all ) page <UINT:page> register <UINT:register>
 */
cparser_result_t
cparser_cmd_rt_port_get_phy_reg_port_ports_all_page_page_register_register(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *page_ptr,
    uint32_t  *register_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    uint32 data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_port_phyReg_get(port, *page_ptr, *register_ptr, &data), ret);
        diag_util_printf("\n Port %d Page %d Register %d: 0x%04X", port, *page_ptr, *register_ptr, data);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_get_phy_reg_port_ports_all_page_page_register_register */

/*
 * rt_port set phy-reg port ( <PORT_LIST:ports> | all ) page <UINT:page> register <UINT:register> data <UINT:data>
 */
cparser_result_t
cparser_cmd_rt_port_set_phy_reg_port_ports_all_page_page_register_register_data_data(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *page_ptr,
    uint32_t  *register_ptr,
    uint32_t  *data_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_port_phyReg_set(port, *page_ptr, *register_ptr, *data_ptr), ret);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_set_phy_reg_port_ports_all_page_page_register_register_data_data */

/*
 * rt_port get port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_rt_port_get_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rt_port_t      port = 0;
    diag_portlist_t  portlist;
    rt_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_port_adminEnable_get(port, &state), ret);
        diag_util_mprintf("\n Port %d Admin state: %s", port, diagStr_enable[state]);
    }

    diag_util_mprintf("\n");


    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_get_port_ports_all_state */

/*
 * rt_port set port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_port_set_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rt_port_t      port = 0;
    diag_portlist_t  portlist;
    rt_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if('e' == TOKEN_CHAR(5, 0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(5, 0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_port_adminEnable_set(port, state), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_set_port_ports_all_state_disable_enable */

/*
 * rt_port get isolation port ( <PORT_LIST:ports> | all ) ( mode0 | mode1 )
 */
cparser_result_t
cparser_cmd_rt_port_get_isolation_port_ports_all_mode0_mode1(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    rt_port_isoConfig_t mode;
    rt_portmask_t portmask;
    rt_portmask_t extPortmask;
    rtk_portmask_t tmpPortmask;
    uint8 portStr[20];
    uint8 ExtPortStr[20];

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('1' == TOKEN_CHAR(5, 4))
        mode = RT_PORT_ISO_CFG_1;
    else if('0' == TOKEN_CHAR(5, 4))
        mode = RT_PORT_ISO_CFG_0;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_port_isolationEntry_get(mode, port, &portmask, &extPortmask), ret);

        memcpy(tmpPortmask.bits,portmask.bits,RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST*sizeof(uint32));
        DIAG_UTIL_ERR_CHK(diag_util_lPortMask2str(portStr, &tmpPortmask), ret);
        memcpy(tmpPortmask.bits,extPortmask.bits,RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST*sizeof(uint32));
        DIAG_UTIL_ERR_CHK(diag_util_lPortMask2str(ExtPortStr, &tmpPortmask), ret);

        diag_util_printf("\n Port %d:", port);
        diag_util_printf("\n     egress port mask    :%s", portStr);
        diag_util_printf("\n     egress EXT port mask:%s", ExtPortStr);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_get_isolation_port_ports_all_mode0_mode1 */

/*
 * rt_port set isolation port ( <PORT_LIST:ports> | all ) ( mode0 | mode1 ) egress-port ( <PORT_LIST:egressports> | none )
 */
cparser_result_t
cparser_cmd_rt_port_set_isolation_port_ports_all_mode0_mode1_egress_port_egressports_none(
    cparser_context_t *context,
    char * *ports_ptr,
    char * *egressports_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    diag_portlist_t egrportlist;
    rt_port_isoConfig_t mode;
    rt_portmask_t portmask;
    rt_portmask_t extPortmask;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(egrportlist, 7), ret);

    if('1' == TOKEN_CHAR(5, 4))
        mode = RTK_PORT_ISO_CFG_1;
    else if('0' == TOKEN_CHAR(5, 4))
        mode = RTK_PORT_ISO_CFG_0;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_port_isolationEntry_get(mode, port, &portmask, &extPortmask), ret);

        portmask.bits[0] = egrportlist.portmask.bits[0];

        DIAG_UTIL_ERR_CHK(rt_port_isolationEntry_set(mode, port, &portmask, &extPortmask), ret);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_set_isolation_port_ports_all_mode0_mode1_egress_port_egressports_none */

/*
 * rt_port get unitype port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_rt_port_get_unitype_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    rt_port_uniType_t uniType;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_port_uniType_get(port,&uniType), ret);
        diag_util_printf("Port %d:  %s\n", port,(uniType == RT_PORT_UNI_TYPE_VEIP)?"VEIP":"PPTP");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_get_unitype_port_ports_all */

/*
 * rt_port set unitype port ( <PORT_LIST:ports> | all ) type ( veip | pptp )
 */
cparser_result_t
cparser_cmd_rt_port_set_unitype_port_ports_all_type_veip_pptp(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    rt_port_uniType_t uniType;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('v' == TOKEN_CHAR(6, 0))
        uniType = RT_PORT_UNI_TYPE_VEIP;
    else if('p' == TOKEN_CHAR(6, 0))
        uniType = RT_PORT_UNI_TYPE_PPTP;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {

        DIAG_UTIL_ERR_CHK(rt_port_uniType_set(port,uniType), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_set_unitype_port_ports_all_type_veip_pptp */


/*
 * rt_port get mac-force port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_rt_port_get_mac_force_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    uint32 data;
    rt_port_macAbility_t portability;


    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_printf("\n                                                   Link   NWAY    LPI");
    diag_util_printf("\nPort Speed    Duplex Fiber1G Link TX_FC RX_FC NWAY MASTER FAULT   100M Giga Force");
    diag_util_printf("\n---- -------- ------ ------- ---- ----- ----- ---- ------ ------- ---- ---- -----\n");



    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        
        osal_memset(&portability, 0x00, sizeof(rt_port_macAbility_t));
        DIAG_UTIL_ERR_CHK(rt_port_macForceAbility_get(port, &portability), ret);
        diag_util_printf("%-4d %-8s %-6s %-7s %-4s %-5s %-5s %-4s %-6s %-5s %-4s %-4s ",
                         port,
                         diagStr_portSpeed[portability.speed],
                         diagStr_portDuplex[portability.duplex],
                         diagStr_enDisplay[portability.linkFib1g],
                         diagStr_portLinkStatus[portability.linkStatus],
                         diagStr_enDisplay[portability.txFc],
                         diagStr_enDisplay[portability.rxFc],
                         diagStr_enDisplay[portability.nwayAbility],
                         diagStr_enDisplay[portability.masterMod],
                         diagStr_portNwayFault[portability.nwayFault],
                         diagStr_enDisplay[portability.lpi_100m],
                         diagStr_enDisplay[portability.lpi_giga]);

        switch(DIAG_UTIL_CHIP_TYPE)
        {
        default:
            DIAG_UTIL_ERR_CHK(rt_port_macForceAbilityState_get(port,&data), ret);
            diag_util_printf("%-5s\n",diagStr_enDisplay[data]);
            break;
        }
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_get_mac_force_port_ports_all */

/*
 * port set mac-force port ( <PORT_LIST:ports> | all ) ability ( 10h | 10f | 100h | 100f | 1000f | 10000f | 500f | 2g5 | 2g5l | 5g | 5gl | 10g ) flow-control ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_port_set_mac_force_port_ports_all_ability_10h_10f_100h_100f_1000f_10000f_500f_2g5_2g5l_5g_5gl_10g_flow_control_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rt_port_t port = 0;

    rt_port_macAbility_t portAbilityR;
    rt_port_macAbility_t portAbilityW;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    memset(&portAbilityW, 0, sizeof(rt_port_macAbility_t));
    if(!osal_strcmp(TOKEN_STR(6),"10h"))
    {
        portAbilityW.duplex = PORT_HALF_DUPLEX;
        portAbilityW.speed = PORT_SPEED_10M;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"10f"))
    {
        portAbilityW.duplex = PORT_FULL_DUPLEX;
        portAbilityW.speed = PORT_SPEED_10M;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"100h"))
    {
        portAbilityW.duplex = PORT_HALF_DUPLEX;
        portAbilityW.speed = PORT_SPEED_100M;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"100f"))
    {
        portAbilityW.duplex = PORT_FULL_DUPLEX;
        portAbilityW.speed = PORT_SPEED_100M;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"1000f"))
    {
        portAbilityW.duplex = PORT_FULL_DUPLEX;
        portAbilityW.speed = PORT_SPEED_1000M;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"10000f"))
    {
        portAbilityW.duplex = PORT_FULL_DUPLEX;
        portAbilityW.speed = PORT_SPEED_10G;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"500f"))
    {
        portAbilityW.duplex = PORT_FULL_DUPLEX;
        portAbilityW.speed = PORT_SPEED_500M;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"2g5"))
    {
        portAbilityW.duplex = PORT_FULL_DUPLEX;
        portAbilityW.speed = PORT_SPEED_2G5;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"2g5l"))
    {
        portAbilityW.duplex = PORT_FULL_DUPLEX;
        portAbilityW.speed = PORT_SPEED_2G5LITE;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"5g"))
    {
        portAbilityW.duplex = PORT_FULL_DUPLEX;
        portAbilityW.speed = PORT_SPEED_5G;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"5gl"))
    {
        portAbilityW.duplex = PORT_FULL_DUPLEX;
        portAbilityW.speed = PORT_SPEED_5GLITE;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"10g"))
    {
        portAbilityW.duplex = PORT_FULL_DUPLEX;
        portAbilityW.speed = PORT_SPEED_10G;
    }

    if ('e' == TOKEN_CHAR(8,0))
    {
        portAbilityW.txFc = ENABLED;
        portAbilityW.rxFc = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(8,0))
    {
        portAbilityW.txFc = DISABLED;
        portAbilityW.rxFc = DISABLED;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
      
        DIAG_UTIL_ERR_CHK(rt_port_macForceAbility_get(port, &portAbilityR), ret);

        portAbilityR.txFc = portAbilityW.txFc;
        portAbilityR.rxFc = portAbilityW.rxFc;
        portAbilityR.speed = portAbilityW.speed;
        portAbilityR.duplex = portAbilityW.duplex;

        DIAG_UTIL_ERR_CHK(rt_port_macForceAbility_set(port,portAbilityR), ret);
        
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_set_mac_force_port_ports_all_ability_10h_10f_100h_100f_1000f_10000f_500f_2g5_2g5l_5g_5gl_10g_flow_control_disable_enable */

/*
 * rt_port set mac-force port ( <PORT_LIST:ports> | all ) ( lpi-100M | lpi-giga ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_port_set_mac_force_port_ports_all_lpi_100M_lpi_giga_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rt_port_t port = 0;
    rt_port_macAbility_t portAbility;
    rt_enable_t enable=DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('d' == TOKEN_CHAR(7,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(7,0))
    {
        enable = ENABLED;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
      
        DIAG_UTIL_ERR_CHK(rt_port_macForceAbility_get(port, &portAbility), ret);

        if ('1' == TOKEN_CHAR(5,4))
        {
            portAbility.lpi_100m = enable;
        }
        else if ('g' == TOKEN_CHAR(5,4))
        {
            portAbility.lpi_giga = enable;
        }

        DIAG_UTIL_ERR_CHK(rt_port_macForceAbility_set(port, portAbility), ret);
      
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_set_mac_force_port_ports_all_lpi_100m_lpi_giga_state_disable_enable */

/*
 * rt_port set mac-force port ( <PORT_LIST:ports> | all ) link-state ( link-down | link-up )
 */
cparser_result_t
cparser_cmd_rt_port_set_mac_force_port_ports_all_link_state_link_down_link_up(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rt_port_t port = 0;

    rt_port_macAbility_t portAbility;
    rt_port_linkStatus_t   linkStatus;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('d' == TOKEN_CHAR(6,5))
    {
        linkStatus = PORT_LINKDOWN;
    }
    else if ('u' == TOKEN_CHAR(6,5))
    {
        linkStatus = PORT_LINKUP;
    }
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
    
        DIAG_UTIL_ERR_CHK(rt_port_macForceAbility_get(port, &portAbility), ret);

        portAbility.linkStatus = linkStatus;

        DIAG_UTIL_ERR_CHK(rt_port_macForceAbility_set(port, portAbility), ret);
       
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_set_mac_force_port_ports_all_link_state_link_down_link_up */

/*
 * rt_port set mac-force port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_port_set_mac_force_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rt_port_t port = 0;
    rt_enable_t state;
    uint32 data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('d' == TOKEN_CHAR(6,0))
    {
        state = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(6,0))
    {
        state = ENABLED;
    }
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_port_macForceAbilityState_set(port,state), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_set_mac_force_port_ports_all_state_disable_enable */

/*
 * rt_port set eee port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_port_set_eee_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rt_port_t      port = 0;
    diag_portlist_t  portlist;
    rt_enable_t state;
  
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('e' == TOKEN_CHAR(6, 0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_port_eeeEnable_set(port, state), ret);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_set_eee_port_ports_all_state_disable_enable */

/*
 * rt_port get eee port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_rt_port_get_eee_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rt_port_t      port = 0;
    diag_portlist_t  portlist;
    rt_enable_t state;
 
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_port_eeeEnable_get(port, &state), ret);
        diag_util_mprintf("\n Port %d state: %s", port, diagStr_enable[state]);
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_get_eee_port_ports_all_state */

/*
 * rt_port get mac-loopback port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_rt_port_get_mac_loopback_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32            ret = RT_ERR_FAILED;
    rt_port_t        port = 0;
    diag_portlist_t  portlist;
    rt_enable_t enable,swapEnable;
 
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_port_macLoopbackEnable_get(port, &enable, &swapEnable), ret);
        diag_util_mprintf("\n Port %d state: %s, MAC swap state: %s\n", port, diagStr_enable[enable], diagStr_enable[swapEnable]);
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_get_mac_loopback_port_ports_all_state */

/*
 * rt_port set mac-loopback port ( <PORT_LIST:ports> | all ) state ( disable | enable ) mac-swap ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_port_set_mac_loopback_port_ports_all_state_disable_enable_mac_swap_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rt_port_t      port = 0;
    diag_portlist_t  portlist;
    rt_enable_t enable,swapEnable;
  
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        enable = DISABLED;
    else
        return CPARSER_NOT_OK;

    if('e' == TOKEN_CHAR(8, 0))
        swapEnable = ENABLED;
    else if('d' == TOKEN_CHAR(8, 0))
        swapEnable = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_port_macLoopbackEnable_set(port, enable, swapEnable), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_port_set_mac_loopback_port_ports_all_state_disable_enable_mac_swap_disable_enable */



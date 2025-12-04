/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : Define diag shell main function.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) main function.
 */

/*  
 * Include Files 
 */
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <diag_util.h>
#include <diag_om.h>
#include <parser/cparser.h>
#include <parser/cparser_priv.h>
#include <parser/cparser_token.h>
#if defined(CONFIG_COMMON_RT_API)
#include <rtk/rt/rt_port.h>
#endif

#define DIAG_HOSTNAME_DEFAULT        "RTK.0> "    /* Default hostname. */

extern cparser_node_t cparser_root;


cparser_t main_parser;


/* 
 * Function Declaration 
 */
int
diag_main(int argc,	char** argv)
{
    /*support execute command without entering cleshell*/
    char cmd[CPARSER_MAX_LINE_SIZE];
    int i;
    int32 ret;

    main_parser.cfg.root = &cparser_root;
    main_parser.cfg.ch_complete = '\t';
    /* 
     * Instead of making sure the terminal setting of the target and 
     * the host are the same. ch_erase and ch_del both are treated
     * as backspace.
     */
    main_parser.cfg.ch_erase = '\b';
    main_parser.cfg.ch_del = 127;
    main_parser.cfg.ch_help = '?';
    main_parser.cfg.flags = 0;
    strcpy(main_parser.cfg.prompt, DIAG_HOSTNAME_DEFAULT);

    rtk_core_init();
#if defined(CONFIG_COMMON_RT_API) && !defined(RTL_CYGWIN_EMULATE)
    ret = rt_port_portDef_init();
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("diag_main: port def init error.\n");
        return -1;
    }
#endif
    
    diag_om_set_chip_id(DIAG_OM_CHIP_ID_DEFAULT);
    if (diag_om_set_deviceInfo() != RT_ERR_OK)
    {
        diag_util_printf("diag_main: set deviceInfo error.\n");
        return -1;
    }
    

    main_parser.cfg.fd = STDOUT_FILENO;
    
    cparser_io_config(&main_parser);
    
    /* Initialization */
    if (CPARSER_OK != cparser_init(&main_parser.cfg, &main_parser)) {
        diag_util_printf("Fail to initialize parser.\n");
	return -1;
    }

    if(argc>1){
        strcpy(cmd, "");
        i=1;
        while(i<argc){
            
            if((strlen(cmd)+strlen(argv[i]))>(CPARSER_MAX_LINE_SIZE-2))
            {
                break;    
            }
            strcat(cmd, argv[i]);
            strcat(cmd, " ");
            i++;
            //if(i>CPARSER_MAX_LINE_SIZE)
            //    break;    
        }
        if (cmd[0] != '\n' &&cmd[0] != '\r') {
            /*excute command here*/
            cparser_run_not_start_shell(&main_parser,cmd);
            return 0;
        }            
        return 0;
    }
    
    
    /* Main command loop */
    cparser_run(&main_parser);


	diag_util_printf("\n");
    
	return 0;
} /* end of diag_main */

#if (defined(CONFIG_SDK_KERNEL_LINUX) && !defined(CONFIG_SDK_MODEL_MODE_USER) && !defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)) ||defined(RTL_CYGWIN_EMULATE) || defined(CONFIG_LINUX_USER_SHELL) || defined(CYGWIN_MDIO_IO) || defined(RTL_RLX_IO) || defined(CONFIG_OPENWRT_DIAG)

int
main(int argc, char** argv)
{
    /* Ignore Ctrl+C  signal. */
    signal(SIGINT, SIG_IGN);	
   
    
    return diag_main(argc, argv);
} /* end of main */
#endif


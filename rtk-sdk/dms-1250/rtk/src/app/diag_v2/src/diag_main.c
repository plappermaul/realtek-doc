/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
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
#include <common/rt_autoconf.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <diag_debug.h>
#include <parser/cparser.h>
#include <parser/cparser_priv.h>
#include <parser/cparser_token.h>
#include <parser/cparser_tree.h>
#include <osal/memory.h>
#ifdef CONFIG_RISE
#include <hwp/hw_profile.h>
#include <rise/src/rise.h>
#endif

static int32
dlag_initial_promt_unit_id(cparser_t *pParser)
{
    uint32      unit;

    if (diag_om_get_firstAvailableUnit(&unit) != RT_ERR_OK)
    {
        diag_util_printf("diag_main: Obtain avaliable unit ID fail.\n");
        return RT_ERR_FAILED;
    }

    if (diag_om_set_deviceInfo(unit) != RT_ERR_OK)
    {
        diag_util_printf("diag_main: set unit %u deviceInfo error.\n", unit);
        return RT_ERR_FAILED;
    }

    diag_om_get_promptString((uint8 *)pParser->cfg.prompt, sizeof(pParser->cfg.prompt), unit);
    diag_om_set_unitId(unit);
    return RT_ERR_OK;
}


/*
 * Function Declaration
 */
int
diag_main(int argc, char** argv)
{
    cparser_t *pParser;

#ifdef CONFIG_RISE
  #if RISE_IN_TURNKEY
    /* no wait here */
  #else
    //just example: wait RISE kickoff before prompt diag-shell, please remove when you call rise_application_kickoff() in your code.
    {
        uint32 box_list;
        rise_application_kickoff(&box_list);
        //diag_util_printf("box_list=0x%08x\n",box_list);
        //diag_util_printf("\n");
    }
  #endif/* RISE_TURNKEY_PORTING */
#endif

    /* Process diag options */
    if (argc > 1)
    {
        /* -d: debug option to print all chip debug informations */
        if (0 == strcmp(argv[1], "-d"))
        {
            diag_util_mprintf_paging_lines(0);
            diag_debug_dump();

            return 0;
        }
    }

    pParser = osal_alloc(sizeof(cparser_t));
    if (NULL == pParser)
    {
        diag_util_printf("osal_alloc cparser_t structure failed!!\n");
        return -1;
    }
    osal_memset(pParser, 0, sizeof(cparser_t));

    pParser->cfg.root = &cparser_root;
    pParser->cfg.ch_complete = '\t';
    /*
     * Instead of making sure the terminal setting of the target and
     * the host are the same. ch_erase and ch_del both are treated
     * as backspace.
     */
    pParser->cfg.ch_erase = '\b';
    pParser->cfg.ch_del = 127;
    pParser->cfg.ch_help = '?';
    pParser->cfg.flags = 0;
    pParser->cfg.fd = STDOUT_FILENO;
    strcpy(pParser->cfg.str_remark, "//");

    if (dlag_initial_promt_unit_id(pParser) != RT_ERR_OK)
    {
        osal_free(pParser);
        return -1;
    }


    cparser_io_config(pParser);

    /* Initialization */
    if (CPARSER_OK != cparser_init(&(pParser->cfg), pParser)) {
        diag_util_printf("Fail to initialize parser.\n");
        osal_free(pParser);
        return -1;
    }

#ifdef CONFIG_SDK_KERNEL_LINUX
    /*if(system("cat /proc/meminfo >> /tmp/mem_log_rtsdk_diag")==-1)*/
    /*{*/
        /*diag_util_printf("cat /proc/meminfo failed\n");*/
    /*}*/
#endif

    /* Main command loop */
    cparser_run(pParser);

    diag_util_printf("\n");

    osal_free(pParser);

    return 0;
} /* end of diag_main */

#if (defined(CONFIG_SDK_KERNEL_LINUX) && !defined(CONFIG_SDK_MODEL_MODE_USER) && defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE))
int
main(int argc, char** argv)
{
    /* Ignore Ctrl+C  signal. */
    signal(SIGINT, SIG_IGN);
    return diag_main(argc, argv);
} /* end of main */
#endif

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
 * $Revision: 40647 $
 * $Date: 2013-07-01 15:36:16 +0800 (?±ä?, 01 ä¸ƒæ? 2013) $
 *
 * Purpose : Main function of the EPON OAM protocol stack user application
 *           It create two additional threads for packet Rx and state control
 *
 * Feature : Start point of Europa. Use individual threads
 *           for packet Rx and state control
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

//#include "epon_oam_config.h"
//#include "epon_oam_err.h"
//#include "epon_oam_msgq.h"
#include "europa_cli.h"

/* 
 * Symbol Definition 
 */
#define EUROPA_CLI_BRANCH     0x01
#define EUROPA_CLI_LEAF       0x02
#define EUROPA_CLI_END        0x03

typedef struct europa_cli_tree_s {
    unsigned char cmd[16];
    unsigned char type;
    union {
        struct europa_cli_tree_s *pSubTree;
        int (*cli_proc)(int argc, char *argv[]);
    } u;
} europa_cli_tree_t;


/*  
 * Data Declaration  
 */
//static int msgQId;
static europa_cli_tree_t cliRootClear[] = {
    {
        "param",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_param_clear }
    },
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
};

static europa_cli_tree_t cliRootGet[] = {
    {
        "param",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_param_get }
    },
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
};

static europa_cli_tree_t cliRootSet[] = {
    {
        "param",
        EUROPA_CLI_LEAF,
        { .cli_proc = europa_cli_param_set }
    },
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
};

static europa_cli_tree_t cliRoot[] = {
    {
        "clear",
        EUROPA_CLI_BRANCH,
        { cliRootClear }
    },
    {
        "get",
        EUROPA_CLI_BRANCH,
        { cliRootGet }
    },
    {
        "set",
        EUROPA_CLI_BRANCH,
        { cliRootSet }
    },
    {
        "",
        EUROPA_CLI_END,
        { NULL }
    }
};


/* 
 * Macro Definition 
 */

/*  
 * Function Declaration  
 */

/* ------------------------------------------------------------------------- */
/* Internal APIs */

static int europa_cli_param_clear(
    int argc,
    char *argv[])
{
	
printf("europa_cli_param_clear!!!!!!!\n");	
	
#if 0	
    unsigned int llidIdx;

    if(argc == 2)
    {
        pCli->cliType = EPON_OAM_CLI_COUNTER_CLEAR;
        llidIdx = atoi(argv[1]);
        if(llidIdx >= EPON_OAM_SUPPORT_LLID_NUM)
        {
            printf("<llidIdx>: %u~%u\n", 0, EPON_OAM_SUPPORT_LLID_NUM - 1);
            return -1;
        }
        pCli->u.cliLlidIdx.llidIdx = llidIdx;

        return 0;
    }
    else
    {
        printf("%s <llidIdx>\n", argv[0]);
        return -1;
    }
#endif

    return 0;
}

static int europa_cli_param_get(
    int argc,
    char *argv[])
{
    FILE *fp;
    int length, start_addr;
    int output,i;

    if(argc >= 3)
    {
        length = atoi(argv[1]);
        start_addr = atoi(argv[2]); 
        
        printf("europa_cli_param_get: length = %d, start address = %d\n", length, start_addr);       
              
        if ((length < 0) || (start_addr < 0))
        {
            printf("europa_cli_param_get: length  or start address error!!!\n"); 
            return -1;
        }

        fp = fopen("/var/config/europa.data","rb");  

        if (NULL ==fp)
        {
            printf("Open file in /var/config/ error!!!!!!!\n"); 
            return -1;           
        }
        
        /* set the file pointer to start address */
        fseek(fp, start_addr, SEEK_SET);  
        
        for(i=0;i<length;i++)
        {
        	  output = fgetc(fp);
            if (output == EOF)
            {
                break;	
            }
            else
            {
                printf("0x%x \n", output);
            }
        }
#if 0        
        do
        {
            /* read a char from the file */
            output = fgetc(fp);
            /* display the character */
            printf("0x%x \n", output);
        } 
        while (output != EOF);
#endif 
        printf("Output Success!!!\n"); 

    }
    else if (argc<3)
    {
        printf("%s <length> <start address>\n", argv[0]);
        return -1;
    }  

    return 0; 
}

static int europa_cli_param_set(
    int argc,
    char *argv[])
{
    FILE *fp;
    int length, start_addr;
    unsigned int input;
    unsigned int i;
	
    if(argc >= 3)
    {
        length = atoi(argv[1]);
        start_addr = atoi(argv[2]); 

        printf("europa_cli_param_set: length = %d, start address = %d\n", length, start_addr);       
              
        if ((length < 0) || (start_addr < 0))
        {
        	  printf("europa_cli_param_set: length  or start address error!!!\n"); 
            return -1;
    }
        
        fp = fopen("/var/config/europa.data","wb");  

        if (NULL ==fp)
       {
            printf("Open file in /var/config/ error!!!!!!!\n"); 
            return -1;           
        }
        
        /* set the file pointer to start address */
        fseek(fp, start_addr, SEEK_SET);  
        
        printf("Please input europa parameters with length %d:\n", length); 

        for(i = 0; i < length; i++)
        {
        	 scanf("%x",&input);
        	 fputc(input&0xFF, fp);
        }
 
        printf("Input Success!!!\n"); 

    }
    else if (argc<3)
    {
        printf("%s <length> <start address>\n", argv[0]);
        return -1;
    }  

    return 0; 
}

int europa_cli_parse(
	int argc,
	char *argv[],
	europa_cli_tree_t *pCliTree)
{
    int inputLen, cmdLen;

    if(0 == argc)
    {
        /* No any argument for parsing */
        printf("available cmds:\n");
        while(pCliTree->type != EUROPA_CLI_END)
        {
            printf("%s\n", pCliTree->cmd);
            pCliTree += 1;
        }

        return -1;
    }

    if(NULL == pCliTree)
    {
        printf("incorrect command tree\n");
        return -2;
    }

    inputLen = strlen(argv[0]);
    while(pCliTree->type != EUROPA_CLI_END)
    {
        cmdLen = strlen(pCliTree->cmd);
        if(strncmp(argv[0], pCliTree->cmd, inputLen > cmdLen ? cmdLen : inputLen) == 0)
        {
            /* Search subtree or execute the command */
            if(pCliTree->type == EUROPA_CLI_BRANCH)
            {
                return europa_cli_parse(argc - 1, &argv[1], pCliTree->u.pSubTree);
            }
            else if(pCliTree->type == EUROPA_CLI_LEAF)
            {
                if(NULL != pCliTree->u.cli_proc)
                {
                    return pCliTree->u.cli_proc(argc, argv);
                }
                else
                {
                    printf("incorrect command callback\n");
                    return -3;
                }
            }
            else
            {
                printf("incorrect command type\n");
                return -3;
            }
        }
        pCliTree = pCliTree + 1;
    }

    printf("incorrect command\n");
    return -4;
}

int
main(
	int argc,
	char *argv[])
{
    int ret;
    int permits;
    key_t msgQKey = 1568;
    //oam_cli_t cli;

#if 0   
    /* Get message queue to send event to state keeper */
    /* S_IRUSR | S_IWUSR | State keeper can read/write message
     * S_IRGRP | S_IWGRP | All others can read/write message
     * S_IROTH | S_IWOTH   All others can read/write message
     */
    permits = 00666;
    permits |= IPC_CREAT;
    msgQId = msgget(msgQKey, permits);
    if(-1 == msgQId)
    {
        printf("[OAM:%s:%d] msgq create failed %d\n", __FILE__, __LINE__, errno);
        return -1;
    }
#endif


    //memset(&cli, 0x0, sizeof(cli));
    ret = europa_cli_parse(argc - 1, &argv[1], cliRoot);

#if 0
    if(0 == ret)
    {
        epon_oam_eventData_send(0,
            EPON_OAM_EVENT_CLI,
            (unsigned char *)&cli,
            sizeof(cli));
    }
#endif

    return ret;
}


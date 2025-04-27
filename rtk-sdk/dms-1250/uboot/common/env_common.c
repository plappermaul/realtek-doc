/*
 * (C) Copyright 2000-2010
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <environment.h>
#include <linux/stddef.h>
#include <search.h>
#include <errno.h>
#include <soc.h>
#include <pblr.h>
#include <malloc.h>


DECLARE_GLOBAL_DATA_PTR;

/************************************************************************
 * Default settings to be used when no valid environment is found
 */
#define XMK_STR(x)    #x
#define MK_STR(x)    XMK_STR(x)

const uchar default_environment[] = {
#if defined(CONFIG_BOARDVERSION)
    "boardversion="    CONFIG_BOARDVERSION        "\0"
#endif
    "ledModeInitSkip=0\0"
#ifdef    CONFIG_BOOTARGS
    "bootargs="    CONFIG_BOOTARGS            "\0"
#endif
#ifdef    CONFIG_BOOTCOMMAND
    "bootcmd="    CONFIG_BOOTCOMMAND        "\0"
#endif
#ifdef    CONFIG_RAMBOOTCOMMAND
    "ramboot="    CONFIG_RAMBOOTCOMMAND        "\0"
#endif
#ifdef    CONFIG_NFSBOOTCOMMAND
    "nfsboot="    CONFIG_NFSBOOTCOMMAND        "\0"
#endif
#if defined(CONFIG_BOOTDELAY)
    "bootdelay="    MK_STR(CONFIG_BOOTDELAY)    "\0"
#endif
/* Assign the baudrate to the default value at init_baudrate()
   to fix br@default and br@tcl/soc_t mismatch issue
 */
//#if defined(CONFIG_BAUDRATE) && (CONFIG_BAUDRATE >= 0)
//    "baudrate="    MK_STR(CONFIG_BAUDRATE)        "\0"
//#endif
#ifdef    CONFIG_LOADS_ECHO
    "loads_echo="    MK_STR(CONFIG_LOADS_ECHO)    "\0"
#endif
#ifdef    CONFIG_ETHADDR
    "ethaddr="    MK_STR(CONFIG_ETHADDR)        "\0"
#endif
#ifdef    CONFIG_ETH1ADDR
    "eth1addr="    MK_STR(CONFIG_ETH1ADDR)        "\0"
#endif
#ifdef    CONFIG_ETH2ADDR
    "eth2addr="    MK_STR(CONFIG_ETH2ADDR)        "\0"
#endif
#ifdef    CONFIG_ETH3ADDR
    "eth3addr="    MK_STR(CONFIG_ETH3ADDR)        "\0"
#endif
#ifdef    CONFIG_ETH4ADDR
    "eth4addr="    MK_STR(CONFIG_ETH4ADDR)        "\0"
#endif
#ifdef    CONFIG_ETH5ADDR
    "eth5addr="    MK_STR(CONFIG_ETH5ADDR)        "\0"
#endif
#ifdef    CONFIG_IPADDR
    "ipaddr="    MK_STR(CONFIG_IPADDR)        "\0"
#endif
#ifdef    CONFIG_SERVERIP
    "serverip="    MK_STR(CONFIG_SERVERIP)        "\0"
#endif
#ifdef    CONFIG_SYS_AUTOLOAD
    "autoload="    CONFIG_SYS_AUTOLOAD        "\0"
#endif
#ifdef    CONFIG_PREBOOT
    "preboot="    CONFIG_PREBOOT            "\0"
#endif
#ifdef    CONFIG_ROOTPATH
    "rootpath="    CONFIG_ROOTPATH            "\0"
#endif
#ifdef    CONFIG_GATEWAYIP
    "gatewayip="    MK_STR(CONFIG_GATEWAYIP)    "\0"
#endif
#ifdef    CONFIG_NETMASK
    "netmask="    MK_STR(CONFIG_NETMASK)        "\0"
#endif
#ifdef    CONFIG_BAUDRATE
    "baudrate="    MK_STR(CONFIG_BAUDRATE)        "\0"
#endif
#ifdef    CONFIG_HOSTNAME
    "hostname="    MK_STR(CONFIG_HOSTNAME)        "\0"
#endif
#ifdef    CONFIG_BOOTFILE
    "bootfile="    CONFIG_BOOTFILE            "\0"
#endif
#ifdef    CONFIG_LOADADDR
    "loadaddr="    MK_STR(CONFIG_LOADADDR)        "\0"
#endif
#ifdef    CONFIG_CLOCKS_IN_MHZ
    "clocks_in_mhz=1\0"
#endif
#if defined(CONFIG_PCI_BOOTDELAY) && (CONFIG_PCI_BOOTDELAY > 0)
    "pcidelay="    MK_STR(CONFIG_PCI_BOOTDELAY)    "\0"
#endif
#ifdef    CONFIG_POSTWORD
    "postword="    CONFIG_POSTWORD            "\0"
#endif
#ifdef CAMEO_SIMPLE_DISPLAY
    "bootmsg=0" "\0" /*For control sdk debug info*/
#endif

#ifdef    CONFIG_EXTRA_ENV_SETTINGS
    CONFIG_EXTRA_ENV_SETTINGS
#endif
    "\0"
};

const uchar default_environment2[] = {
    "imageId=1\0"
    "configId=1\0"
};

struct hsearch_data env_htab[] = {
    {0},
    {0}
};

unsigned int g_env_id = 0;

static uchar env_get_char_init(int index)
{
    /* if crc was bad, use the default environment */
    if (gd->env_valid)
        return env_get_char_spec(index);
    else
        return default_environment[index];
}

uchar env_get_char_memory(int index)
{
    return *env_get_addr(index);
}

uchar env_get_char(int index)
{
    /* if relocated to RAM */
    if (gd->flags & GD_FLG_RELOC)
        return env_get_char_memory(index);
    else
        return env_get_char_init(index);
}

const uchar *env_get_addr(int index)
{
    if (gd->env_valid)
        return (uchar *)(gd->env_addr + index);
    else
        return &default_environment[index];
}

void set_default_env(const char *s)
{
    char hw_version[CAMEO_STR_SZ] = {0};
    char demo_name[CAMEO_STR_SZ] = {0};
#if defined(CONFIG_ENV_CRC_ERROR_SAVE_DEFAULT_ENV)
    int    save_default_env_flag = 0;
#endif

    if (sizeof(default_environment) > ENV_SIZE) {
        puts("*** Error - default environment is too large\n\n");
        return;
    }

    if (s) {
        if (*s == '!') {
            printf("*** Warning - %s, "
                "using default environment\n\n",
                s + 1);
#if defined(CONFIG_ENV_CRC_ERROR_SAVE_DEFAULT_ENV)
            save_default_env_flag = 1;
#endif
        } else {
            puts(s);
        }
    } else {
        puts("Using default environment\n\n");
    }

    if (himport_r(&env_htab[0], (char *)default_environment,
            sizeof(default_environment), '\0', 0) == 0)
        error("Environment import failed: errno = %d\n", errno);

#if defined(CONFIG_ENV_CRC_ERROR_SAVE_DEFAULT_ENV)
    if(save_default_env_flag == 1)
    {
/*CAMEOTAG:Add by tonghong 20190808.Set the default HW version based on the board hw version */
/*Do the extra env set for the hw version*/
#ifdef CONFIG_EXTRA_ENV_SETTINGS
        strncpy(hw_version,CAMEO_BOARD_HW_VERSION,strlen((const char*)CAMEO_BOARD_HW_VERSION));
        setenv("hw_version",hw_version) ;
#endif
        strncpy(demo_name,CAMEO_BOARD_DEMONAME,strlen((const char*)CAMEO_BOARD_DEMONAME));
        setenv("boardmodel",demo_name) ;

        saveenv();
        printf("*** Store Default environment into ENV\n");
    }
#endif

    gd->flags |= GD_FLG_ENV_READY;
}

void set_default_env2(const char *s)
{
    char hw_version[CAMEO_STR_SZ] = {0};
    char demo_name[CAMEO_STR_SZ] = {0};
#if defined(CONFIG_ENV_CRC_ERROR_SAVE_DEFAULT_ENV)
    int save_default_env_flag = 0;
#endif

    if (sizeof(default_environment2) > ENV_SIZE) {
        puts("*** Error - default environment is too large\n\n");
        return;
    }

    if (s) {
        if (*s == '!') {
            printf("*** Warning - %s, "
                    "using default environment\n\n",
                    s + 1);
#if defined(CONFIG_ENV_CRC_ERROR_SAVE_DEFAULT_ENV)
            save_default_env_flag = 1;
#endif
        } else {
            puts(s);
        }
    } else {
        puts("Using default environment\n\n");
    }

    if (himport_r(&env_htab[1], (char *)default_environment2,
                sizeof(default_environment2), '\0', 0) == 0)
        error("Environment import failed: errno = %d\n", errno);


#if defined(CONFIG_ENV_CRC_ERROR_SAVE_DEFAULT_ENV)
    if(save_default_env_flag == 1)
    {
/*CAMEOTAG:Add by tonghong 20190808.Set the default HW version based on the board hw version */
/*Do the extra env set for the hw version*/
#ifdef CONFIG_EXTRA_ENV_SETTINGS
        strncpy(hw_version,CAMEO_BOARD_HW_VERSION,strlen((const char*)CAMEO_BOARD_HW_VERSION));
        setenv("hw_version",hw_version) ;
#endif
        strncpy(demo_name,CAMEO_BOARD_DEMONAME,strlen((const char*)CAMEO_BOARD_DEMONAME));
        setenv("boardmodel",demo_name) ;

        saveenv2();
        printf("*** Store Default environment into ENV\n");
    }
#endif

    gd->flags2 |= GD_FLG_ENV_READY;
}

/*
 * Check if CRC is valid and (if yes) import the environment.
 * Note that "buf" may or may not be aligned.
 */
int env_import(const char *buf, int check)
{
    env_t *ep = (env_t *)buf;

    if (check) {
        uint32_t crc;

        memcpy(&crc, &ep->crc, sizeof(crc));

        if (crc32(0, ep->data, ENV_SIZE) != crc) {
            if (g_env_id == 0)
                set_default_env("!bad CRC");
            else if (g_env_id == 1)
                set_default_env2("!bad CRC");
            return 0;
        }
    }

    if (himport_r(&env_htab[g_env_id], (char *)ep->data, ENV_SIZE, '\0', 0)) {
        if (g_env_id == 0)
            gd->flags |= GD_FLG_ENV_READY;
        else if (g_env_id == 1)
            gd->flags2 |= GD_FLG_ENV_READY;

        return 1;
    }

    error("Cannot import environment: errno = %d\n", errno);

    if (g_env_id == 0)
        set_default_env("!import failed");
    else if (g_env_id == 1)
        set_default_env2("!import failed");

    return 0;
}

void env_relocate(void)
{  
/*CAMEOTAG:Add by tonghong 20190808.Set the default HW version based on the board info */    
#ifdef CONFIG_EXTRA_ENV_SETTINGS
     if (cameo_get_board_info() == CAMEO_FAILURE)
     {
         return ;
     }
#endif

#if defined(CONFIG_NEEDS_MANUAL_RELOC)
    env_reloc();
#endif
    if (gd->env_valid == 0) {
#if defined(CONFIG_ENV_IS_NOWHERE)    /* Environment not changable */
        set_default_env(NULL);
#else
        show_boot_progress(-60);
        set_default_env("!bad CRC");
#endif
    } else {
        env_relocate_spec();
    }

    if (gd->env2_valid == 0) {
#if defined(CONFIG_ENV_IS_NOWHERE)	/* Environment not changable */
        set_default_env2(NULL);
#else
        show_boot_progress(-60);
        set_default_env2("!bad CRC");
#endif
    } else {
        env_relocate_spec2();
    }
}

#ifdef CONFIG_AUTO_COMPLETE
int env_complete(char *var, int maxv, char *cmdv[], int bufsz, char *buf)
{
    ENTRY *match;
    int found, idx;

    idx = 0;
    found = 0;
    cmdv[0] = NULL;

    while ((idx = hmatch_r(var, idx, &match, &env_htab[0]))) {
        int vallen = strlen(match->key) + 1;

        if (found >= maxv - 2 || bufsz < vallen)
            break;

        cmdv[found++] = buf;
        memcpy(buf, match->key, vallen);
        buf += vallen;
        bufsz -= vallen;
    }

    qsort(cmdv, found, sizeof(cmdv[0]), strcmp_compar);

    if (idx)
        cmdv[found++] = "...";

    cmdv[found] = NULL;
    return found;
}

int env_complete2(char *var, int maxv, char *cmdv[], int bufsz, char *buf)
{
    ENTRY *match;
    int found, idx;

    idx = 0;
    found = 0;
    cmdv[0] = NULL;

    while ((idx = hmatch_r(var, idx, &match, &env_htab[1]))) {
        int vallen = strlen(match->key) + 1;

        if (found >= maxv - 2 || bufsz < vallen)
            break;

        cmdv[found++] = buf;
        memcpy(buf, match->key, vallen);
        buf += vallen;
        bufsz -= vallen;
    }

    qsort(cmdv, found, sizeof(cmdv[0]), strcmp_compar);

    if (idx)
        cmdv[found++] = "...";

    cmdv[found] = NULL;
    return found;
}
#endif





/******************************************************************************
**   cameo_get_board_info 
**   get board info(model name and hw version) from hardware to distinguish
**   different product features.
**
**   In general, first get board id via hardware interface,
**   then according to the model id, assign a model string to model_name.
**   Here, not care about board id, just read board model string from flash.
*******************************************************************************/
int cameo_get_board_info(void)
{
    char cmd[CAMEO_STR_SZ];
    int i;

    cameo_board_info_t cameo_board_info[] =
    {
        {
            .id = ~0,
            .model = "DMS-1250-10SPL",
            .hwversion = "A1",
            .demo = "RTL9302C_2xRTL8224_2XGE"
        },
        {
            .id = ~0,
            .model = "DMS-1250-10SP",
            .hwversion = "A1",
            .demo = "RTL9302C_2xRTL8224_2XGE"
        },
        {
            .id = ~0,
            .model = "DMS-1250-10S",
            .hwversion = "A1",
            .demo = "RTL9302C_2xRTL8224_2XGE"
        },
        {
            .id = ~0,
            .model = "DMS-1250-12TP",
            .hwversion = "A1",
            .demo = "RTL9302C_2xRTL8224_2XGE_2xRTL8261N"
        },
    };


   memset(&gcameoboardinfo, 0, sizeof(gcameoboardinfo));

    sprintf(cmd, "sf probe 0; sf read 0x%lx 0x%lx 0x%lx",
                (unsigned long) CONFIG_LOADADDR, 
                MTD_PART_BDINFO_OFFSET, MTD_PART_BDINFO_SIZE);
    run_command(cmd, 0);

    for (i = 0; i < sizeof(cameo_board_info) / sizeof(cameo_board_info_t); i++)
    {
        if (strncmp((char *) CONFIG_LOADADDR, 
                    cameo_board_info[i].model,
                    strlen(cameo_board_info[i].model)) == 0)
        {
            break;
        }
    }

    if (memchr((char *) CONFIG_LOADADDR, '\0', CAMEO_STR_SZ) == NULL)
    {
        printf("Board model not found!\n");
        return CAMEO_FAILURE;
    }

    if (i == sizeof(cameo_board_info) / sizeof(cameo_board_info_t))
    {
        printf("not in the supported list!\n");
        return CAMEO_FAILURE;
    }
    memcpy(&gcameoboardinfo, &cameo_board_info[i],sizeof(cameo_board_info_t));

    return CAMEO_SUCCESS ;
}

/******************************************************************************
**   cameo_get_board_modulename 
**   get board info model name based on the gcameoboardinfo
*******************************************************************************/
 char* cameo_get_board_modulename(void)
{
    return gcameoboardinfo.model ;
}

/******************************************************************************
**   cameo_get_board_hwversion 
**   get board info hw version based on the gcameoboardinfo
*******************************************************************************/
char*  cameo_get_board_hwversion(void)
{
    return gcameoboardinfo.hwversion ;
}


/******************************************************************************
**   cameo_get_board_demoname
**   get board info demo name based on the gcameoboardinfo
*******************************************************************************/
char*  cameo_get_board_demoname(void)
{
    return gcameoboardinfo.demo ;
}



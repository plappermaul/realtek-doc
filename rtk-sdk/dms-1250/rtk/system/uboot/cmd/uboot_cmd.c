/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * Purpose : RTL83xx SOC and SWCORE commands for U-Boot.
 *
 * Feature :
 *
 */


/*
 * Include Files
 */
#include <config.h>
#include <common.h>
#include <command.h>
#include <common/util.h>
#include <common/type.h>
#include <common/rt_autoconf.h>
#include <uboot/cmd/uboot_cmd.h>
#include <rtk/port.h>
#include "hwp/hw_profile.h"
#include <hal/mac/miim_common_drv.h>
#include <net.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <drv/nic/nic.h>
#include <hal/mac/serdes.h>

#include <hal/chipdef/driver.h>
#include <hal/phy/phy_rtl9310.h>
#include <hal/mac/drv/drv_rtl9310.h>

/*
 * Symbol Definition
 */
#define PORT_LOOPBACK_TEST_PKT_MAX_LEN (1530)
unsigned int uboot_cmd_unit = 0;

#define UBOOT_CMD_SDS_MODE_STR_LEN      20
typedef struct uboot_cmd_sdsMode_s
{
    rt_serdesMode_t     sdsMode;
    char                sdsModeStr[UBOOT_CMD_SDS_MODE_STR_LEN];
} uboot_cmd_sdsMode_t;


/*
 * Data Declaration
 */

uint8 pkt_content[] = {
    0x00, 0x18, 0xe7, 0x17, 0x50, 0x61, 0x00, 0xe0, 0x4c, 0x86, 0x70, 0x01, 0x08, 0x12, 0x45, 0x00,
    0x00, 0x3c, 0xf7, 0x1a, 0x00, 0x00, 0x40, 0x01, 0xff, 0xe5, 0xc0, 0xa8, 0x01, 0x01, 0xc0, 0xa8,
    0x01, 0x6f, 0x00, 0x00, 0x55, 0x42, 0x00, 0x01, 0x00, 0x19, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
    0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
    0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69
    };

uint8 pkt[PORT_LOOPBACK_TEST_PKT_MAX_LEN];
uint8 pkt_recv[PORT_LOOPBACK_TEST_PKT_MAX_LEN];
uint32 isFail, testLen, rcvLen;

uboot_cmd_sdsMode_t     uboot_cmd_sdsMode_str[RTK_MII_END] =
    {
        { RTK_MII_NONE,                     "none"                  },
        { RTK_MII_DISABLE,                  "disable"               },
        { RTK_MII_10GR,                     "10gr"                  },
        { RTK_MII_RXAUI,                    "rxaui"                 },
        { RTK_MII_RXAUI_LITE,               "rxaui_lite"            },
        { RTK_MII_RXAUISGMII_AUTO,          "rxauisgmii_auto"       },
        { RTK_MII_RXAUI1000BX_AUTO,         "rxaui1000bx_auto"      },
        { RTK_MII_RSGMII_PLUS,              "rsgmii_plus"           },
        { RTK_MII_SGMII,                    "sgmii"                 },
        { RTK_MII_QSGMII,                   "qsgmii"                },
        { RTK_MII_1000BX_FIBER,             "1000bx_fiber"          },
        { RTK_MII_100BX_FIBER,              "100bx_fiber"           },
        { RTK_MII_1000BX100BX_AUTO,         "1000bx100bx_auto"      },
        { RTK_MII_10GR1000BX_AUTO,          "10gr1000bx_auto"       },
        { RTK_MII_10GRSGMII_AUTO,           "10grsgmii_auto"        },
        { RTK_MII_XAUI,                     "xaui"                  },
        { RTK_MII_RMII,                     "rmii"                  },
        { RTK_MII_SMII,                     "smii"                  },
        { RTK_MII_SSSMII,                   "sssmii"                },
        { RTK_MII_RSGMII,                   "rsgmii"                },
        { RTK_MII_XSMII,                    "xsmii"                 },
        { RTK_MII_XSGMII,                   "xsgmii"                },
        { RTK_MII_QHSGMII,                  "qhsgmii"               },
        { RTK_MII_HISGMII,                  "hisgmii"               },
        { RTK_MII_HISGMII_5G,               "hisgmii_5g"            },
        { RTK_MII_DUAL_HISGMII,             "dual_hisgmii"          },
        { RTK_MII_2500Base_X,               "2500base_x"            },
        { RTK_MII_RXAUI_PLUS,               "rxaui_plus"            },
        { RTK_MII_USXGMII_10GSXGMII,        "usxgmii_10gsxgmii"     },
        { RTK_MII_USXGMII_10GDXGMII,        "usxgmii_10gdxgmii"     },
        { RTK_MII_USXGMII_10GQXGMII,        "usxgmii_10gqxgmii"     },
        { RTK_MII_USXGMII_5GSXGMII,         "usxgmii_5gsxgmii"      },
        { RTK_MII_USXGMII_5GDXGMII,         "usxgmii_5gdxgmii"      },
        { RTK_MII_USXGMII_2_5GSXGMII,       "usxgmii_2_5gsxgmii"    },
    };

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

drv_nic_rx_t _rx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie)
{
    rcvLen = pPacket->length;

    memcpy((&pkt_recv), pPacket->data, rcvLen);

    rcvLen = pPacket->length;

    if(testLen !=rcvLen)
    {
        isFail = 1;
        return NIC_RX_NOT_HANDLED;
    }

    if( 0 != memcmp(pkt, pkt_recv, rcvLen))
    {
        isFail = 2;
        return NIC_RX_NOT_HANDLED;
    }

    return NIC_RX_NOT_HANDLED;
}

/* Function Name:
 *      do_rtk
 * Description:
 *      Main function of RTK commands.
 * Input:
 *      cmdtp, flag, argc, argv handled by the parser
 * Output:
 *      None
 * Return:
 *      0    - Command is executed.
 *      >=1  - Command syntax error or an error occur during execution of the function.
 * Note:
 *      None
 */
int do_rtk(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
    uint32 argIdx;

    uint32 unit = 0;
    uint32 port;
    uint32 led_index;
    uint32 id;

#ifdef CONFIG_SDK_DRIVER_GPIO
    uint pinNum;
    uint pinStatus;
    uint devAddress;
#endif /* CONFIG_SDK_DRIVER_GPIO */

    if (argc < 2)
    {
        goto usage;
    }


    /* misc init */
    if (0 == strcmp(argv[1], "init"))
    {
        uboot_sdk_init(UBOOT_SDK_INIT_PHASE_RTK);
        rtk_forcelinkDown10Gserdes(unit);
        return 0;
    }

    if (0 == strcmp(argv[1], "rtcore-init"))        /* phase  */
    {
        uboot_sdk_init(UBOOT_SDK_INIT_PHASE_RTCORE);
        return 0;
    }


    if (0 == strcmp(argv[1], "network"))
    {
        if (0 == strcmp(argv[2], "on"))        /* network on */
        {
            printf("Enable network\n");

            rtk_network_on();

            return 0;
        }
        else if (0 == strcmp(argv[2], "off"))  /* network off */
        {
            printf("Disable network\n");

            rtk_network_off();

            return 0;
        }

        goto usage;
    }

    if (0 == strcmp(argv[1], "unit"))
    {
        if (0 == strcmp(argv[2], "get"))
        {
            printf("unit: %u\n", uboot_cmd_unit);
            return 0;
        }
        else if (0 == strcmp(argv[2], "set"))
        {
            if (argc < 4)
                goto usage;
            unit = simple_strtoul(argv[3], NULL, 10);
            if (unit >= RTK_MAX_NUM_OF_UNIT_LOCAL)
            {
                printf("Invalid unit: %u\n", unit);
            }
            printf("Change to unit: %u\n", unit);
            uboot_cmd_unit = unit;
            return 0;
        }
    }

    if (0 == strcmp(argv[1], "port"))
    {
        uint32 port_num;
        unit = uboot_cmd_unit;
        port_num = simple_strtoul(argv[3], NULL, 10);

        if (0 == strcmp(argv[2], "enable"))        /* network on */
        {
            printf("Enable port (%d,%d)\n",unit, port_num);
            rtk_port_adminEnable_set(unit, port_num,ENABLED);

            return 0;
        }
        else if (0 == strcmp(argv[2], "disable"))  /* network off */
        {
            printf("Disable port (%d,%d)\n",unit, port_num);
            rtk_port_adminEnable_set(unit, port_num,DISABLED);

            return 0;
        }

        goto usage;
    }


    if (0 == strcmp(argv[1], "show"))
    {
        if (0 == strcmp(argv[2], "hw_profile_list"))
        {
            rtk_hwProfileList_show();

            return 0;
        }

        goto usage;

    }

    /* comboport */
    if (0 == strcmp(argv[1], "comboport"))
    {
        if (argc < 3)
            goto usage;

        if (0 == strcmp(argv[2], "copper"))        /* comboport copper */
        {
            printf("ComboPort: Copper mode\n");
            rtk_comboport_copper();

            return 0;
        }
        else if (0 == strcmp(argv[2], "fiber"))  /* comboport fiber */
        {
            printf("ComboPort: Fiber mode\n");
            rtk_comboport_fiber();

            return 0;
        }
#if defined(CONFIG_SDK_RTL8390)
        else if (0 == strcmp(argv[2], "port"))  /* per port */
        {
            int portid;

            if (argc < 5)
                goto usage;

            portid = simple_strtoul(argv[3], NULL, 10);
            if (0 == strcmp(argv[4], "copper"))        /* comboport copper */
            {
                printf("ComboPort %d: Copper mode\n", portid);
                rtk_comboport_portcopper(portid);

                return 0;
            }
            else if (0 == strcmp(argv[4], "fiber"))  /* comboport fiber */
            {
                printf("ComboPort %d: Fiber mode\n", portid);
                rtk_comboport_portfiber(portid);

                return 0;
            }
        }
#endif
        goto usage;
    }


#if defined(CONFIG_SDK_RTL8390)
    if ((0 == strcmp(argv[1], "sfp-speed")) && (0 == strcmp(argv[2], "set")))
    {
        int port, speed;

        if (argc < 5)
            goto usage;

        port = simple_strtoul(argv[3], NULL, 10);
        speed = simple_strtoul(argv[4], NULL, 10);
        rtk_sfp_speed_set(port, speed);
        return 0;
    }
    else if ((0 == strcmp(argv[1], "sys-esd")))
    {
        if (argc < 3)
            goto usage;
        if (0 == strcmp(argv[2], "on"))
        {
            rtk_sysEsd_set(1);
        }
        else
        {
            rtk_sysEsd_set(0);
        }

        return 0;
    }
    else if ((0 == strcmp(argv[1], "parameter")) && (0 == strcmp(argv[2], "version")))
    {
        rtk_parameter_version_show();
        return 0;
    }
#endif/* CONFIG_SDK_RTL8390 */

    if(0 == strcmp(argv[1], "port-isolation"))
    {
        if(0 == strcmp(argv[2], "on"))
        {
            rtk_port_isolation_on();
            return 0;
        }
        else if(0 == strcmp(argv[2], "off"))
        {
            rtk_port_isolation_off();
            return 0;
        }
        goto usage;
    }
#ifdef CONFIG_EEE
        /* eee */
    if (0 == strcmp(argv[1], "eee"))
    {
        if (0 == strcmp(argv[2], "on"))        /* eee on */
        {
            printf("Enable EEE function\n");
            rtk_eee_on();

            return 0;
        }
        else if (0 == strcmp(argv[2], "off"))  /* eee off */
        {
            printf("Disable EEE function\n");
            rtk_eee_off();

            return 0;
        }

        goto usage;
    }
#endif

#ifdef CONFIG_SDK_DRIVER_GPIO
    /*get internal GPIO pin status*/
    if (strcmp(argv[1], "pinGet") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: rtk pinGet <pinNum> \n pinNum: 0~7\n\n");
            return 1;
        }
        else
        {
            pinNum = simple_strtoul(argv[2], NULL, 10);

            if(0 != rtk_intGpio_pin_get(pinNum,&pinStatus))
            {
                printf("The pinNum (%d) is invalid.\n", pinNum);
                return 1;
            }
            printf("pin%d:\t%d\n\n",pinNum,pinStatus);
            return 0;
        }
    }

/*set internal GPIO pin status*/
    if (strcmp(argv[1], "pinSet") == 0)
    {
        if (argc < 4)
        {
            printf("Usage: rtk pinSet <pinNum> <status> \n pinNum: 0~7, status: 0/1\n\n");
            return 1;
        }
        else
        {
            pinNum = simple_strtoul(argv[2], NULL, 10);
            pinStatus = simple_strtoul(argv[3], NULL, 10);

            if(pinStatus > 1)
            {
                printf("The status (%d) is invalid.\n", pinStatus);
                return 1;
            }
            else
            {
                if(0 != rtk_intGpio_pin_set(pinNum, pinStatus))
                {
                    printf("The pinNum (%d) is invalid.\n", pinNum);
                    return 1;
                }
                printf("pin%d:\t%d\n\n",pinNum,pinStatus);
                return 0;
            }
        }
    }
#endif /* CONFIG_SDK_DRIVER_GPIO */

#ifdef CONFIG_SDK_DRIVER_GPIO
    if (strcmp(argv[1], "ext-devInit") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: rtk ext-devInit <devAddress> \n unit ID \n devAddress: 0~31\n\n");
            return 1;
        }
        else
        {
            devAddress = simple_strtoul(argv[2], NULL, 10);

            if(0 != rtk_extGpio_device_init(uboot_cmd_unit, devAddress))
            {
                printf("The generalCtrl GPIO device init failed.\n");
                return 1;
            }
            printf("devAddress:\t%d\n\n",devAddress);
            return 0;
        }
    }


    if (strcmp(argv[1], "ext-pinGet") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: rtk ext-pinGet <pinNum> \n pinNum: 0~36\n\n");
            return 1;
        }
        else
        {
            pinNum = simple_strtoul(argv[2], NULL, 10);

            if(0 != rtk_extGpio_pin_get(uboot_cmd_unit, pinNum, &pinStatus))
            {
                printf("The pinNum (%d) is invalid.\n", pinNum);
                return 1;
            }
            printf("pin%d:\t%d\n\n",pinNum,pinStatus);
            return 0;
        }
    }

    /*set external 8231 pin status*/
    if (strcmp(argv[1], "ext-pinSet") == 0)
    {
        if (argc < 4)
        {
            printf("Usage: rtk ext-pinSet <pinNum> <status> \n pinNum: 0~36, status: 0/1\n\n");
            return 1;
        }
        else
        {
            pinNum = simple_strtoul(argv[2], NULL, 10);
            pinStatus = simple_strtoul(argv[3], NULL, 10);

            if(pinStatus > 1)
            {
                printf("The status (%d) is invalid.\n", pinStatus);
                return 1;
            }
            else
            {
                if(0 != rtk_extGpio_pin_set(uboot_cmd_unit,pinNum,pinStatus))
                {
                    printf("The pinNum (%d) is invalid.\n", pinNum);
                    return 1;
                }
                printf("pin%d:\t%d\n\n",pinNum,pinStatus);
                return 0;
            }
        }
    }
#endif /* CONFIG_SDK_DRIVER_GPIO */

#if defined(CONFIG_SDK_DRIVER_I2C)
    if (strcmp(argv[1], "i2c") == 0)
    {
        unsigned int i2c_dev_idx, reg;
        unsigned int clk_dev, clk_pin, data_dev, data_pin, data, freq, intf_id, rtl8231_address, read_type;

        if (argc < 3)
        {
            printf("i2c parameter error\n\n");
            goto usage;
        }

        if ((strcmp(argv[2], "init") == 0)&&(strcmp(argv[3], "sw") == 0))
        {
            int delay, chipid;
            if (argc < 13)
            {
                printf("i2c parameter error\n\n");
                goto usage;
            }
            i2c_dev_idx = simple_strtoul(argv[4], NULL, 10);
            clk_dev = simple_strtoul(argv[5], NULL, 10);
            clk_pin = simple_strtoul(argv[6], NULL, 10);
            data_dev = simple_strtoul(argv[7], NULL, 10);
            data_pin = simple_strtoul(argv[8], NULL, 10);
            chipid= simple_strtoul(argv[10], NULL, 10);
            delay = simple_strtoul(argv[11], NULL, 10);
            rtl8231_address = simple_strtoul(argv[12], NULL, 10);
            read_type = simple_strtoul(argv[13], NULL, 10);
            rtk_i2c_sw_init(i2c_dev_idx, clk_dev, clk_pin, data_dev, data_pin, argv[9], chipid, delay, rtl8231_address, read_type);
            return 0;
        }

        if ((strcmp(argv[2], "init") == 0)&&(strcmp(argv[3], "hw") == 0))
        {
            int chipid;
            if (argc < 10)
            {
                printf("i2c parameter error\n\n");
                goto usage;
            }
            i2c_dev_idx = simple_strtoul(argv[4], NULL, 10);
            intf_id = simple_strtoul(argv[5], NULL, 10);
            data_pin = simple_strtoul(argv[6], NULL, 10);
            chipid= simple_strtoul(argv[8], NULL, 10);
            freq = simple_strtoul(argv[9], NULL, 10);
            read_type = simple_strtoul(argv[10], NULL, 10);
            rtk_i2c_hw_init(i2c_dev_idx, data_pin, argv[7], chipid, freq, intf_id, read_type);
            return 0;
        }

        if (strcmp(argv[2], "read") == 0)
        {
            if (argc < 5)
            {
                printf("i2c parameter error\n\n");
                goto usage;
            }
            i2c_dev_idx = simple_strtoul(argv[3], NULL, 10);
            reg = simple_strtoul(argv[4], NULL, 10);
            rtk_i2c_read(i2c_dev_idx, reg);
            return 0;
        }
        if (strcmp(argv[2], "write") == 0)
        {
            if (argc < 6)
            {
                printf("i2c parameter error\n\n");
                return 1;
            }
            i2c_dev_idx = simple_strtoul(argv[3], NULL, 10);
            reg = simple_strtoul(argv[4], NULL, 10);
            data = simple_strtoul(argv[5], NULL, 10);
            rtk_i2c_write(i2c_dev_idx, reg, data);
            return 0;
        }
        goto usage;
    }
#endif/* defined(CONFIG_SDK_DRIVER_GPIO) */

    /* led test */
    if (strcmp(argv[1], "ledtest") == 0)
    {
        if (argc < 3)
        {
            rtk_ledTest_run();
            return 0;
        }
        if (argc != 4)
        {
            printf("Usage: rtk ledtest [port] [led_index]\n\n");
            return 1;
        }

        port = simple_strtoul(argv[2], NULL, 10);
        led_index = simple_strtoul(argv[3], NULL, 10);
        rtk_swledtest_on(port,led_index);
        return 0;
    }
    /* phyreg get portid page reg */
    if(0 == strcmp(argv[1], "phyreg"))
    {
        if (0 == strcmp(argv[2], "get"))
        {
            rtk_phyreg_get(argv);
            return 1;
        }

        /* phyreg get portid page reg */
        if (0 == strcmp(argv[2], "dump-top"))
        {
            rtk_phyreg_dump_top(argv);
            return 1;
        }

        /* phyreg set */
        if (0 == strcmp(argv[2], "set"))
        {
            rtk_phyreg_set(argv);
            return 1;
        }

        /* phyreg setbymask */
        if (strcmp(argv[2], "setbymask") == 0)
        {
            rtk_phyreg_portmask_set(argv);
            return 1;
        }
        goto usage;
    }

    if(0 == strcmp(argv[1], "phymmd"))
    {
        unsigned int portid, addr, reg, val;

        if (argc < 6)
        {
            printf("phymmd parameter error\n\n");
            goto usage;
        }

        argIdx = 2;
        unit = uboot_cmd_unit;
        portid = simple_strtoul(argv[++argIdx], NULL, 10);
        addr = simple_strtoul(argv[++argIdx], NULL, 10);
        reg = simple_strtoul(argv[++argIdx], NULL, 10);

        if (0 == strcmp(argv[2], "get"))
        {
            rtk_phymmd_get(unit, portid, addr, reg);
            return 1;
        }

        /* phyreg set */
        if (0 == strcmp(argv[2], "set"))
        {
            if (argc < 7)
            {
                printf("phymmd parameter error\n\n");
                goto usage;
            }

            val = simple_strtoul(argv[++argIdx], NULL, 10);
            rtk_phymmd_set(unit, portid, addr, reg, val);
            return 1;
        }
        goto usage;
    }

    /* sdsreg get portid page reg */
    if(0 == strcmp(argv[1], "sdsreg"))
    {
        unsigned int sdsId;
        unsigned int page;
        unsigned int reg;
        unsigned int val;

        if (argc < 6)
        {
            printf("sdsreg parameter error\n\n");
            goto usage;
        }

        argIdx = 3;
        unit = uboot_cmd_unit;
        sdsId = simple_strtoul(argv[++argIdx], NULL, 10);
        page = simple_strtoul(argv[++argIdx], NULL, 10);
        reg = simple_strtoul(argv[++argIdx], NULL, 10);

        if (0 == strcmp(argv[2], "get"))
        {
            rtk_sdsreg_get(unit, sdsId, page, reg);
            return 1;
        }

        /* sdsreg set */
        if (0 == strcmp(argv[2], "set"))
        {
            if (argc < 7)
            {
                printf("sdsreg parameter error\n\n");
                goto usage;
            }
            val = simple_strtoul(argv[++argIdx], NULL, 10);
            rtk_sdsreg_set(unit, sdsId, page, reg, val);
            return 1;
        }
        goto usage;
    }

    if (strcmp(argv[1], "macsds") == 0)
    {
        unsigned int sdsId;
        rt_serdesMode_t sdsMode = RTK_MII_END;
        int i;
        char *mode_str = NULL;

        argIdx = 3;
        sdsId = simple_strtoul(argv[argIdx], NULL, 10);


        /* sdsreg set */
        if (0 == strcmp(argv[2], "set"))
        {
            if (argc < 5)
            {
                printf("macsds parameter error %u\n\n", argc);
                printf("mode options are:\n");
                for (i=0; i<sizeof(uboot_cmd_sdsMode_str)/sizeof(uboot_cmd_sdsMode_t); i++)
                {
                    printf("  %s\n", uboot_cmd_sdsMode_str[i].sdsModeStr);
                }
                goto usage;
            }

            argIdx++;
            mode_str = argv[argIdx];

            for (i=0; i<sizeof(uboot_cmd_sdsMode_str)/sizeof(uboot_cmd_sdsMode_t); i++)
            {
                if (0 == strcmp(mode_str, uboot_cmd_sdsMode_str[i].sdsModeStr))
                {
                    sdsMode = uboot_cmd_sdsMode_str[i].sdsMode;
                    break;
                }
            }
            if (sdsMode == RTK_MII_END)
            {
                printf("sdsmode error\n");
                printf("mode options are:\n");
                for (i=0; i<sizeof(uboot_cmd_sdsMode_str)/sizeof(uboot_cmd_sdsMode_t); i++)
                {
                    printf("  %s\n", uboot_cmd_sdsMode_str[i].sdsModeStr);
                }
                goto usage;
            }
            printf("sds=%u mode=%s\n", sdsId, mode_str);
            rtk_mac_sdsMode_set(sdsId, sdsMode);
            return 1;
        }
    }

#if defined(CONFIG_SDK_RTL8380)
    if (0 == strcmp(argv[1], "l2-testmode"))
    {
        if(0 == strcmp(argv[2], "on"))
        {
            rtk_l2testmode_on();
            return 0;
        }
        if(0 == strcmp(argv[2], "off"))
        {
            rtk_l2testmode_off();
            return 0;
        }
        goto usage;
    }
#endif

    /* testmode */
    if (strcmp(argv[1], "phytestmode") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: testmode <mode> <port> <channel>\n channel: 1=A,2=B,3=C,4=D,0=None\n\n");
        }
        else
        {
            rtk_phy_testmode(argv);
        }

        return 1;
    }
    if (0 == strcmp(argv[1], "boardid"))
    {
        if (argc == 3)
        {
            id = simple_strtoul(argv[2], NULL, 10);
            rtk_boardId_set(id);
            return 0;
        }
        else
        {
            return rtk_boardId_get();
        }
    }

    if (0 == strcmp(argv[1], "boardmodel"))
    {
        if (argc == 3)
        {
            rtk_boardModel_set(argv);
            return 0;
        }
        else
        {
            return rtk_boardModel_get();
        }
    }

    if (0 == strcmp(argv[1], "10g"))
    {
        int order = 2;
        unsigned int port;

        port = simple_strtoul(argv[order++], NULL, 10);
        if (0 == strcmp(argv[order], "none"))
            rtk_10gMedia_none_set(port);
        else if (0 == strcmp(argv[order], "fiber10g"))
            rtk_10gMedia_fiber10g_set(port);
        else if (0 == strcmp(argv[order], "fiber1g"))
            rtk_10gMedia_fiber1g_set(port);
        else if (0 == strcmp(argv[order], "dac50cm"))
            rtk_10gMedia_dac50cm_set(port);
        else if (0 == strcmp(argv[order], "dac100cm"))
            rtk_10gMedia_dac100cm_set(port);
        else if (0 == strcmp(argv[order], "dac300cm"))
            rtk_10gMedia_dac300cm_set(port);
        else
        {
            goto usage;
        }
        return 0;
    }

    if (0 == strcmp(argv[1], "send"))
    {
        uint32 k;
        NetSendPacket(pkt_content, testLen);
        udelay(10000);
        for(k =0; k <80; k ++)
        {
            eth_rx();
        }
        return 0;
    }

#if defined(CONFIG_SDK_RTL9310)
    if (0 == strcmp(argv[1], "mib"))
    {
        mib_portCntr_entry_t portCntr;
        uint32 cntr;

        if (argc == 3)
        {
            int i;
            port = simple_strtoul(argv[2], NULL, 10);
            table_read(unit, MANGO_STAT_PORT_MIB_CNTRt, port, (uint32 *) &portCntr);

            for(i = 0; i < sizeof(portCntr)/4; i ++)
            {
                printf("MIB[%d:%d] %d\n", 1695 - i * 32, 1696 - (i+1)*32, portCntr.entry_data[i]);
            }

        }
        else
        {
            HWP_PORT_TRAVS(unit, port)
            {
                table_read(unit, MANGO_STAT_PORT_MIB_CNTRt, port, (uint32 *) &portCntr);
                table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt, (uint32)MANGO_STAT_PORT_MIB_CNTR_IFOUTUCASTPKTS_Htf,
                            &cntr, (uint32 *) &portCntr);
                table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt, (uint32)MANGO_STAT_PORT_MIB_CNTR_IFOUTUCASTPKTS_Ltf,
                                &cntr, (uint32 *) &portCntr);

                printf("Port %d TX %d ", port, cntr);


                table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt, (uint32)MANGO_STAT_PORT_MIB_CNTR_IFINUCASTPKTS_Htf,
                                &cntr, (uint32 *) &portCntr);
                table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt, (uint32)MANGO_STAT_PORT_MIB_CNTR_IFINUCASTPKTS_Ltf,
                                &cntr, (uint32 *) &portCntr);

                printf("RX %d\n", cntr);

            }

             HWP_PORT_TRAVS(unit, port)
            {
                int i;
                table_read(unit, MANGO_STAT_PORT_MIB_CNTRt, port, (uint32 *) &portCntr);

                printf("Port %d ", port);

                for(i = 0; i < sizeof(portCntr)/4; i ++)
                {
                    printf("%d ", portCntr.entry_data[i]);
                }
                printf("\n");

            }

        }
        return 0;
    }



     if (0 == strcmp(argv[1], "looptest"))
    {
        int times, i, j, k, testMode, currTestMode;
        unsigned int port, flag, loop;
        unsigned int sds, polarRx[RTK_MAX_SDS_PER_UNIT] = {0} , polarTx[RTK_MAX_SDS_PER_UNIT] = {0};
        rtk_port_phyPolarCtrl_t polartCtrl;
        uint32 data, cntr, preCntr, link[2], linkNum, extLinkNum, reg;
        uint32 len[] = {60, 123, 361, 721, 1019, 1514};
        mib_portCntr_entry_t portCntr;
        unsigned int unit = uboot_cmd_unit;
        uint32 speedMode, aleCntr, totalAleCntr;
        uint32 intDelay, extDelay;
        int intTestPorts = 0;
        uint32 doCali, failed;
        uint8 fail_list[125];
        uint8 test_speed_max;
        //uint32 dropCnt;

        testMode = simple_strtoul(argv[2], NULL, 0);
        times = simple_strtoul(argv[3], NULL, 0);
        intDelay = simple_strtoul(argv[4], NULL, 0);
        extDelay = simple_strtoul(argv[5], NULL, 0);
        memset(fail_list, 0, sizeof(fail_list));

        if (0 == strcmp(argv[6], "1g"))
        {
            intTestPorts = 48;
        }
        else if (0 == strcmp(argv[6], "10g"))
        {
            intTestPorts = 6;
        }
        else
            intTestPorts = 54;

        doCali = 1;
        test_speed_max = 2;
         if (argc == 8)
        {
            if (0 == strcmp(argv[7], "fix"))
            {
                test_speed_max = 0;
            }
        }

        printf("test mode %d intTestPorts %d\n", testMode, intTestPorts);

        REG32(0xbb000928) = 0x06400030; // enable CPU port

        HWP_PORT_TRAVS(unit, port)
        {

            REG32(0xbb00c820 + (port / 16)*4) |= 2 << ((port % 16) * 2); //disable new learn

            if(port != 56)
            {
                REG32(0xbb00c830 + (port / 10)*4) |= 2 << ((port % 10) * 3); //trap new learn
            }
        }

        HWP_PORT_TRAVS(unit, port)
        {
            if(port == 56)
                break;
            if (HWP_SERDES_PORT(unit, port))
            {
                    rtk_port_10gMedia_set(unit, port, PORT_10GMEDIA_FIBER_10G);
                    drv_rtl9310_portMacForceLink_set(unit, port, DISABLED,PORT_LINKUP, DRV_RTL9310_FRCLINK_MODULE_LOOPBACK);
            }
        }

        printf("delay %ds\n", extDelay);
        udelay(extDelay * 1000000);

        link[0] = REG32(0xBB000EC0);
        link[0] = REG32(0xBB000EC0);
        link[1] = REG32(0xBB000EC4);
        link[1] = REG32(0xBB000EC4);


        extLinkNum = 0;
        HWP_PORT_TRAVS(unit, port)
        {
            if(port == 56)
                break;
            if(link[port / 32] & (1 << port%32))
            {
                extLinkNum ++;
            }
        }

        printf("link %x %x extLinkNum %d\n", link[1], link[0], extLinkNum);

        drv_nic_rx_register(unit, 2, _rx_callback, 0, 0);
        flag = 1;


        data = 0xFF;
        reg_array_field_write(unit, MANGO_FC_CPU_Q_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, 0,
                        MANGO_ONf, &data);

        reg_array_field_write(unit, MANGO_FC_CPU_Q_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, 0,
                        MANGO_OFFf, &data);

        currTestMode = testMode;
        totalAleCntr = 0;

        failed = 0;

        for(j = 0; j < times; j ++)
        {
TEST_START:
            if(currTestMode & 1)
            {
                linkNum = intTestPorts;
            }
            else
                linkNum = extLinkNum;


            for(speedMode = 0; speedMode <= test_speed_max; speedMode ++)
            {
                switch(speedMode)
                {
                    case 0: reg = 0x2100; break; //100M reg 0 config
                    case 1: reg = 0x0140; break; //1000M reg 0 config
                    case 2: reg = 0x0100; break; //10M reg 0 config
                }

                printf("speed %d reg %x\n", speedMode, reg);

                HWP_PORT_TRAVS(unit, port)
                {
                    if(port == 56)
                        break;
                    if (HWP_SERDES_PORT(unit, port))
                    {
                        sds = HWP_PORT_SDSID(unit, port);

                        switch(speedMode)
                        {
                            case 0: rtk_port_10gMedia_set(unit, port, PORT_10GMEDIA_FIBER_1G); break;
                            case 1: rtk_port_10gMedia_set(unit, port, PORT_10GMEDIA_FIBER_10G); break;
                            case 2:
                                if (currTestMode & 1) //internal
                                {
                                    if(port % 2 == 1 || port == 50)
                                        rtk_port_10gMedia_set(unit, port, PORT_10GMEDIA_FIBER_1G);
                                    else
                                        rtk_port_10gMedia_set(unit, port, PORT_10GMEDIA_FIBER_10G);
                                }
                                else //external
                                {
                                    if(port == 50 || port == 53)
                                        rtk_port_10gMedia_set(unit, port, PORT_10GMEDIA_FIBER_1G);
                                    else
                                        rtk_port_10gMedia_set(unit, port, PORT_10GMEDIA_FIBER_10G);
                                }
                                break;
                        }

                        //if ((dropCnt = REG32(0xbb00D808))!=0) {printf("MacDrop %d Line %d port %d\n", dropCnt, __LINE__, port); goto ROLLBACK;}
                        
                        if (currTestMode & 1)
                        {
                            rtk_port_phyPolar_get(unit, port, &polartCtrl);
                            polarRx[sds] = polartCtrl.phy_polar_rx;
                            polarTx[sds] = polartCtrl.phy_polar_tx;

                            polartCtrl.phy_polar_rx = PHY_POLARITY_NORMAL;
                            polartCtrl.phy_polar_tx = PHY_POLARITY_NORMAL;
                            rtk_port_phyPolar_set(unit, port, &polartCtrl);

                            rtk_port_phyLoopBackEnable_set(unit, port, ENABLED);
                            if(intTestPorts == 48)
                                drv_rtl9310_portMacForceLink_set(unit, port, ENABLED, PORT_LINKDOWN, DRV_RTL9310_FRCLINK_MODULE_LOOPBACK);

                        }

                    }
                    else
                    {
                        if(speedMode == 2)
                        {
                            if (currTestMode & 1)
                            {
                                if(port % 3 == 0)
                                {
                                    reg = 0x0100; //10M
                                }
                                else if(port % 3 == 1)
                                {
                                    reg = 0x2100; // 100M
                                }
                                else if(port % 3 == 2)
                                {
                                    reg = 0x0140; // 1000M
                                }
                            }
                            else
                            {
                                if(port % 6 == 0 || port % 6 == 1)
                                {
                                    reg = 0x0100; //10M
                                }
                                else if(port % 6 == 2 || port % 6 == 3)
                                {
                                    reg = 0x2100; // 100M
                                }
                                else if(port % 6 == 4 || port % 6 == 5)
                                {
                                    reg = 0x0140; // 1000M
                                }
                            }
                        }

                        if (currTestMode & 1)
                        {
                            reg &= ~(1 << 15);
                            reg |= 1 << 14;
                            if(intTestPorts == 6)
                                reg |= 1 << 11;
                        }

                        hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, reg);
                    }
                }

                if(doCali)
                    if ((currTestMode & 1) == 0)
                    {
                        HWP_PORT_TRAVS(unit, port)
                        {
                            if(port == 56)
                                break;

                            link[0] = REG32(0xBB000EC0);
                            link[0] = REG32(0xBB000EC0);
                            link[1] = REG32(0xBB000EC4);
                            link[1] = REG32(0xBB000EC4);

                            if((link[port / 32] & (1 << port%32)) == 0)
                            {
                                continue;
                            }
                            
                            if (HWP_SERDES_PORT(unit, port))
                            {
                                sds = HWP_PORT_SDSID(unit, port);

                                phy_rtl9310_rxCali(unit, sds);
                            }
                        }
                        printf("Calibrated\n");
                    }

                if (currTestMode & 1) // self loop can delay shortly
                {
                    printf("currTestMode: %d\n", currTestMode);
                    printf("delay %ds\n", intDelay);
                    udelay(intDelay * 1000000);
                }
                else
                {
                    printf("currTestMode: %d\n", currTestMode);
                    printf("delay %ds\n", extDelay);
                    udelay(extDelay * 1000000);
                }

                HWP_PORT_TRAVS(unit, port)
                {
                    reg_field_write(unit, MANGO_STAT_PORT_RSTr, MANGO_RST_PORT_MIBf, &port);
                    reg_field_write(unit, MANGO_STAT_PORT_RSTr, MANGO_RST_PORT_FLAGf, &flag);
                    for (loop = 0; loop < 0xFFFFFFFF; loop++)
                    {
                        if ((reg_field_read(unit, MANGO_STAT_PORT_RSTr, MANGO_RST_PORT_FLAGf, &data) == RT_ERR_OK) && (0 == data))
                        {
                            break;
                        }
                    }

                }

                for(k = 1; k <= 80; k ++)
                {
                    REG32(0xbb00D800 + 4 * k); // read to clear debug counter                    
                }
                
                preCntr = 0;

                for(i = 0; i < 6; i ++)
                {
                    testLen = len[i];
                    memset(pkt, j, sizeof(pkt));
                    memcpy(pkt, pkt_content, sizeof(pkt_content));

                    NetSendPacket(pkt, len[i]);
                    udelay(10000);

                    HWP_PORT_TRAVS(unit, port)
                    {
                        if(port == 56)
                            break;

                        link[0] = REG32(0xBB000EC0);
                        link[0] = REG32(0xBB000EC0);
                        link[1] = REG32(0xBB000EC4);
                        link[1] = REG32(0xBB000EC4);

                        if((link[port / 32] & (1 << port%32)) == 0)
                        {
                            continue;
                        }

                        table_read(unit, MANGO_STAT_PORT_MIB_CNTRt, port, (uint32 *) &portCntr);

                        table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt, (uint32)MANGO_STAT_PORT_MIB_CNTR_IFOUTUCASTPKTS_Htf,
                                    &cntr, (uint32 *) &portCntr);
                        table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt, (uint32)MANGO_STAT_PORT_MIB_CNTR_IFOUTUCASTPKTS_Ltf,
                                        &cntr, (uint32 *) &portCntr);
                        printf("port %d outCntr %d ", port, cntr);
                        if(cntr != i + 1)
                        {
                            printf("FAIL! Round %d speed %d port %d len %d TX %d != %d\n", j, speedMode, port, len[i], cntr, i + 1);
                            failed = 1;
                            fail_list[j/8] |= (1 << (j%8));

                            goto ROLLBACK;
                        }

                        table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt, (uint32)MANGO_STAT_PORT_MIB_CNTR_IFINUCASTPKTS_Htf,
                                        &cntr, (uint32 *) &portCntr);
                        table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt, (uint32)MANGO_STAT_PORT_MIB_CNTR_IFINUCASTPKTS_Ltf,
                                        &cntr, (uint32 *) &portCntr);
                        printf("inCntr %d\n", cntr);
                        if(cntr != i + 1)
                        {
                            printf("FAIL! Round %d speed %d port %d len %d RX %d != %d\n", j, speedMode, port, len[i], cntr, i + 1);
                            failed = 1;
                            fail_list[j/8] |= (1 << (j%8));

                            goto ROLLBACK;
                        }
                    }

                    for(k =0; k <80; k ++)
                    {
                        isFail = 0;
                        eth_rx();
                        if(isFail != 0)
                        {
                            printf("FAIL! Round %d speed %d len %d rcvLen %d isFail %d != 0\n", j, speedMode, len[i], rcvLen, isFail);
                            failed = 1;
                            fail_list[j/8] |= (1 << (j%8));

                            goto ROLLBACK;
                        }
                    }

                    table_read(unit, MANGO_STAT_PORT_MIB_CNTRt, 56, (uint32 *) &portCntr);


                    table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt, (uint32)MANGO_STAT_PORT_MIB_CNTR_IFOUTUCASTPKTS_Htf,
                                    &cntr, (uint32 *) &portCntr);
                    table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt, (uint32)MANGO_STAT_PORT_MIB_CNTR_IFOUTUCASTPKTS_Ltf,
                                    &cntr, (uint32 *) &portCntr);
                    printf("outCntr %d\n", cntr);
                    if(cntr != linkNum * (i + 1))
                    {
                        printf("FAIL! Round %d speed %d CPU Port len %d TxCntr %d != %d\n", j, speedMode, len[i], cntr, linkNum * (i + 1));
                        failed = 1;
                        fail_list[j/8] |= (1 << (j%8));

                        goto ROLLBACK;
                    }


                    table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt, (uint32)MANGO_STAT_PORT_MIB_CNTR_IFOUTOCTETS_Htf,
                                    &cntr, (uint32 *) &portCntr);
                    table_field_get(unit, MANGO_STAT_PORT_MIB_CNTRt, (uint32)MANGO_STAT_PORT_MIB_CNTR_IFOUTOCTETS_Ltf,
                                    &cntr, (uint32 *) &portCntr);
                    printf("outCntrByte %d\n", cntr);

                    if(cntr != (len[i]+24) * linkNum + preCntr)
                    {
                        printf("FAIL! Round %d speed %d CPU Port len %d TxByteCntr %d != %d\n", j, speedMode, len[i], cntr, (len[i]+24) * linkNum + preCntr);
                        failed = 1;
                        fail_list[j/8] |= (1 << (j%8));

                        goto ROLLBACK;
                    }

                    preCntr = cntr;

                    aleCntr = REG32(0xbb00D800);
                    printf("REG(0xbb00D800):%d\n", aleCntr);
                    totalAleCntr += aleCntr;


                    for(k = 1; k <= 80; k ++)
                    {
                        if((cntr = REG32(0xbb00D800 + 4 * k)) != 0)
                        {
                            printf("FAIL! Round %d speed %d len %d DropCntr[%d]: %d != 0\n", j, speedMode, len[i], k, cntr);
                            failed = 1;
                            fail_list[j/8] |= (1 << (j%8));

                            goto ROLLBACK;
                        }
                    }
                }


                HWP_PORT_TRAVS(unit, port)
                {
                     if(port == 56)
                        break;

                    if (HWP_SERDES_PORT(unit, port))
                    {
                        if (currTestMode & 1)
                        {
                            sds = HWP_PORT_SDSID(unit, port);

                            polartCtrl.phy_polar_rx = polarRx[sds];
                            polartCtrl.phy_polar_tx = polarTx[sds];
                            rtk_port_phyPolar_set(unit, port, &polartCtrl);

                            rtk_port_phyLoopBackEnable_set(unit, port, DISABLED);
                        }
                        rtk_port_10gMedia_set(unit, port, PORT_10GMEDIA_NONE);
                    }
                    else
                    {
                        hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &data);
                        data = 0x1940;
                        hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, data);
                    }
                }
            }

            if(testMode == 3 && currTestMode == 3) // to do external loopback test
            {
                currTestMode = 2;
                goto TEST_START;
            }
            else if(testMode == 3 && currTestMode == 2) // to do next round test
                currTestMode = 3;

            printf("Round %d PASS! totalAleCntr %d\n", j, totalAleCntr);
        }

ROLLBACK:

        for(k =0; k <80; k ++) // clear CPU RX ring
        {
            eth_rx();
        }

      #if 1
        if(failed == 0)
        {
            if (testMode & 1)
            {
                HWP_PORT_TRAVS(unit, port)
                {
                    if(port == 56)
                        break;

                    if (HWP_SERDES_PORT(unit, port))
                    {

                        sds = HWP_PORT_SDSID(unit, port);

                        polartCtrl.phy_polar_rx = polarRx[sds];
                        polartCtrl.phy_polar_tx = polarTx[sds];
                        rtk_port_phyPolar_set(unit, port, &polartCtrl);

                        rtk_port_phyLoopBackEnable_set(unit, port, DISABLED);
                        drv_rtl9310_portMacForceLink_set(unit, port, DISABLED,PORT_LINKUP, DRV_RTL9310_FRCLINK_MODULE_LOOPBACK);
                    }
                    else
                    {
                        hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &data);
                        data = 0x1140;
                        hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, data);
                    }

                }
            }


            HWP_PORT_TRAVS(unit, port)
            {
                REG32(0xbb00c820 + (port / 16)*4) &= ~(2 << ((port % 16) * 2)); //enable new learn

                if(port != 56)
                    REG32(0xbb00c830 + (port / 10)*4) &= ~(2 << ((port % 10) * 3)); // new learn forward
            }
        }
      #endif

        printf("Test Done\n");
        for(j = 0; j < times; j ++)
        {
            if(fail_list[j/8] & (1 << (j % 8)))
                printf("Failed at round %d\n", j);
        }

        return 0;
    }
#endif //defined(CONFIG_SDK_RTL9310)

usage:
    printf("Usage:\n%s\n", (char *)cmdtp->usage);
    return 1;
} /* end of do_rtk */

U_BOOT_CMD(
    rtk, 14, 0, do_rtk,
    "rtk     - Realtek commands\n",
    "rtk network on\n"
    "        - Enable the networking function\n"
    "rtk netowkr off\n"
    "        - Disable the networking function\n"
    "rtk testmode [mode] [port]\n"
    "        - Set default value for specific testing\n"
#ifdef CONFIG_SDK_DRIVER_GPIO
    "rtk ext-devInit [deviceAddress]\n"
    "        - set RTL8231 MDC address\n"
    "rtk ext-pinGet [pinNum]\n"
    "        - get external 8231 GPIO pin status\n"
    "rtk ext-pinSet [pinNum] [status]\n"
    "        - set external 8231 GPIO pin status\n"
#endif/* CONFIG_SDK_DRIVER_GPIO */
    "rtk i2c init sw [i2c_dev_id] [sck_dev] [sck_pin] [sda_dev] [sda_pin] "
    "[8/16 access type] [chipid] [delay] [rtl8231_address (for Ext-GPIO only)] [read_type 0~1]\n"
    "        - create a i2c group and init\n"
    "rtk i2c init hw [i2c_dev_id] [intf_id 0~1 for HW] [sda_pin] "
    "[8/16 access type] [chipid] [freq 0~3] [read_type 0~1]\n"
    "        - create a i2c group and init\n"
    "rtk i2c read [i2c_dev_id] [reg]\n"
    "rtk i2c write [i2c_dev_id] [reg] [data]\n"
#ifdef CONFIG_SDK_DRIVER_GPIO
    "rtk pinGet [pinNum]\n"
    "        - get internal GPIO pin status\n"
    "rtk pinSet [pinNum] [status]\n"
    "        - set internal GPIO pin status\n"
#endif/* CONFIG_SDK_DRIVER_GPIO */
    "rtk ledtest [port] [led_index]\n"
    "        - led test\n"
#ifdef CONFIG_EEE
    "rtk eee on\n"
    "        - enable eee function\n"
    "rtk eee off\n"
    "        - disable eee function\n"
#endif/* CONFIG_EEE */
    "rtk show hw_profile_list\n"
    "        - show the current all supported hw_profile list\n"
    "rtk phytestmode mode port channel\n"
    "        - Set PHY into test mode; channel: 1=A,2=B,3=C,4=D,0=None\n"
    "rtk boardid\n"
    "        - Get board model id\n"
    "rtk boardid id\n"
    "        - Set board model id\n"
    "rtk boardmodel\n"
    "        - Get board model\n"
    "rtk boardmode <str>model\n"
    "        - Set board model\n"
    "rtk 10g PORT [none | fiber10g | fiber1g | fiber100m | dac50cm | dac100cm | dac300cm]"
    "        - Set 10g port media\n"
    "rtk phyreg get [port] [page] [reg]\n"
    "rtk phyreg set [port] [page] [reg] [data]\n"
    "        - Get/Set PHY register\n"
    "rtk phymmd get [port] [addr] [reg]\n"
    "rtk phymmd set [port] [addr] [reg] [data]\n"
    "        - Get/Set PHY mmd register\n"
    "rtk sdsreg get [sdsId] [page] [reg]\n"
    "rtk sdsreg set [sdsId] [page] [reg] [data]\n"
    "        - Get/Set MAC serdes register\n"
    "rtk macsds set [sdsId] [sds_mode]\n"
    "        - Set MAC serdes mode\n"
    "rtk unit get\n"
    "rtk unit set [unit]\n"
    "        - Get/set unit ID\n"
);


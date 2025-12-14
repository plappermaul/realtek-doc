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
 *
 *
 * $Revision: 40672 $
 * $Date: 2013-07-02 10:10:54 +0800 (Tue, 02 Jul 2013) $
 *
 * Purpose : Fiber mode init
 *
 * Feature : Initial for fiber access
 *
 */

#ifdef CONFIG_FIBER_FEATURE 

/*
 * Include Files
 */
#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/version.h>

/* For RTK APIs */
#include <common/rt_error.h>
#include <rtk/ponmac.h>
#include <rtk/switch.h>
#include <rtk/vlan.h>
#include <rtk/led.h>
#include <rtk/gpio.h>

#if CONFIG_SDK_RTL9601B
#include <linux/kthread.h>
#include <linux/sched.h>
#include <dal/rtl9601b/dal_rtl9601b_flowctrl.h>
#include <dal/rtl9601b/dal_rtl9601b_ponmac.h>
#endif

/*
 * Symbol Definition
 */
enum {
    FIBER_MODE_PREDEF = 0,
    FIBER_MODE_AN_1G,
    FIBER_MODE_AUTO,
    FIBER_MODE_FORCE_1G,
    FIBER_MODE_FORCE_100M,
    /* Use the original fiber bring up architecture for other SDS mode */
    FIBER_MODE_SGMII_1G,  
    FIBER_MODE_END
};

/*
 * Macro Definition
 */
#if CONFIG_SDK_RTL9601B
#define FIBER_MONITOR_WAIT_CNT_DEFAULT      10
#define FIBER_MONITOR_THRESHOLD_DEFAULT     0xff
#endif
#define FIBER_DBG_PRINT(fmt, args...)       \
{                                           \
    if(fiberDbg)                            \
    {                                       \
        printk(fmt, ## args);          \
    }                                       \
}

/*
 * Data Declaration
 */
static unsigned char fiber_mode = FIBER_MODE_PREDEF;
static unsigned char predef_fiber_mode = FIBER_MODE_AUTO;
static unsigned char fiberDbg = 0;
static struct proc_dir_entry *fiber_mode_entry = NULL;
static struct proc_dir_entry *fiber_debug = NULL;
static char *strFiberMode[] = {
    "Pre-defined",
    "AN 1G",
    "Auto",
    "Force 1G",
    "Force 100M",
    "SGMII 1G"
};
static char *strPonMode[] = {
    "",
    "",
    "AN 1G",
    "Force 100M",
    "Auto",
    "Force 1G",
    "Force 100M",
    "SGMII 1G"
};
#if CONFIG_SDK_RTL9601B
struct task_struct *pFiberMonitor;
uint16 waitCntCfg = FIBER_MONITOR_WAIT_CNT_DEFAULT;
uint16 thresholdCfg = FIBER_MONITOR_THRESHOLD_DEFAULT;
#endif

/*
 * Function Declaration
 */
#if CONFIG_SDK_RTL9601B
static int fiber_debugSignal_init(void)
{
    int ret;
    uint16 value;
    rtl9601b_sds_acc_t sds_acc;

    /* REG_RX_EN_TEST=1 */
    sds_acc.index = RTL9601B_SDS_IDX_PON;
    sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
    sds_acc.regaddr = 16;
    if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    value |= (1<<15);
    if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
    {
        return ret;
    }
    /* REG_CDR_SEL_TESTOUT */
    sds_acc.index = RTL9601B_SDS_IDX_PON;
    sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
    sds_acc.regaddr = 19;
    if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    value &= ~(0x3<<12);
    if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
    {
        return ret;
    }
    /* afe debug signal mx */
    sds_acc.index = RTL9601B_SDS_IDX_PON;
    sds_acc.page = RTL9601B_SDS_PAGE_SDS;
    sds_acc.regaddr = 28;
    if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    value &= ~(0x7<<7);
    value &= ~(0x1f);
    if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

static int fiber_cnt_reset(uint8 isFiber1G)
{
    int ret;
    uint16 value;
    rtl9601b_sds_acc_t sds_acc;

    FIBER_DBG_PRINT("counter reset %s\n", isFiber1G ? "1g" : "100m");
    if(isFiber1G)
    {
        /* Select channel 0 counter */
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_SDS_EXT;
        sds_acc.regaddr = 24;
        if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value &= ~(0x7);
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Clear counter */
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_SDS_EXT;
        sds_acc.regaddr = 2;
        value = 0;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
        {
            return ret;
        }
    }
    else
    {
        /* Clear counter */
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_FIB_EXT;
        sds_acc.regaddr = 25;
        value = 0x1005;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
        {
            return ret;
        }
        value = 0x1001;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    return RT_ERR_OK;
}

static int fiber_cnt_get(uint8 isFiber1G, uint32 *pCnt)
{
    int ret;
    uint16 value;
    rtl9601b_sds_acc_t sds_acc;

    FIBER_DBG_PRINT("counter get %s\n", isFiber1G ? "1g" : "100m");
    if(isFiber1G)
    {
        /* Select channel 0 counter */
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_SDS_EXT;
        sds_acc.regaddr = 24;
        if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        value &= ~(0x7);
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
        {
            return ret;
        }
        /* Retrive counter */
        /* bits [23:16] */
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_SDS_EXT;
        sds_acc.regaddr = 3;
        if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        *pCnt = (value & 0xff00) >> 8;
        *pCnt <<= 16;
        /* bits [15:0] */
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_SDS_EXT;
        sds_acc.regaddr = 2;
        if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        *pCnt |= value;
    }
    else
    {
        /* Retrive counter */
        /* bits [16] */
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_FIB_EXT;
        sds_acc.regaddr = 25;
        if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        *pCnt = (value & 0x4000) >> 14;
        *pCnt <<= 16;
        /* bits [15:0] */
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_FIB_EXT;
        sds_acc.regaddr = 28;
        if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        *pCnt |= value;
    }

    return RT_ERR_OK;
}

static int fiber_rxSd_toggle(void)
{
    int ret;
    uint16 value;
    rtl9601b_sds_acc_t sds_acc;

    FIBER_DBG_PRINT("rx_sd toggle\n");

    /* Toggle Rx SD polarity */
    sds_acc.index = RTL9601B_SDS_IDX_PON;
    sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
    sds_acc.regaddr = 12;
    if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    value &= ~(1<<15);
    if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
    {
        return ret;
    }
    value |= (1<<15);
    if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

static int fiber_rxEn_toggle(uint8 isFiber1G)
{
    int ret;
    uint16 value;
    rtl9601b_sds_acc_t sds_acc;

    FIBER_DBG_PRINT("rx_en toggle\n");

    /* Force BEN notify */
    sds_acc.index = RTL9601B_SDS_IDX_PON;
    sds_acc.page = RTL9601B_SDS_PAGE_SDS;
    sds_acc.regaddr = 14;
    if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    value |= (1<<12);
    if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
    {
        return ret;
    }
    sds_acc.index = RTL9601B_SDS_IDX_PON;
    sds_acc.page = RTL9601B_SDS_PAGE_MISC;
    sds_acc.regaddr = 2;
    if(isFiber1G)
    {
        value = 0x1000;
    }
    else
    {
        value = 0xd000;
    }    
    if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Toggle Rx EN */
    sds_acc.index = RTL9601B_SDS_IDX_PON;
    sds_acc.page = RTL9601B_SDS_PAGE_MISC;
    sds_acc.regaddr = 0;
    value = 0x0010;
    if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
    {
        return ret;
    }
    value = 0x0000;
    if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait 10 ms (at least 1 ms) */
    schedule_timeout(1 * HZ / 100);

    /* Release BEN notify */
    sds_acc.index = RTL9601B_SDS_IDX_PON;
    sds_acc.page = RTL9601B_SDS_PAGE_MISC;
    sds_acc.regaddr = 2;
    if(isFiber1G)
    {
        value = 0x0;
    }
    else
    {
        value = 0xc000;
    }    
    if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
    {
        return ret;
    }
    sds_acc.index = RTL9601B_SDS_IDX_PON;
    sds_acc.page = RTL9601B_SDS_PAGE_SDS;
    sds_acc.regaddr = 14;
    if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    value &= ~(1<<12);
    if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

static int fiber_recovery(uint8 isFiber1G)
{
    int ret;
    uint8 isNway = 0;
    uint16 value;
    rtl9601b_sds_acc_t sds_acc;

    FIBER_DBG_PRINT("recovery process\n");

    if(isFiber1G)
    {
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_FIB;
        sds_acc.regaddr = 0;
        if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        isNway = ((value & (1<<12)) ? 1 : 0);

        if(isNway)
        {
            /* Restart N-Way */
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_FIB;
            sds_acc.regaddr = 0;
            if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &value)) != RT_ERR_OK)
            {
                return ret;
            }
            value |= (1<<9);
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
            {
                return ret;
            }

            /* Wait 10 ms (at least 1 ms) */
            schedule_timeout(1 * HZ / 100);

            /* Toggle Rx EN */
            if((ret = fiber_rxEn_toggle(isFiber1G)) != RT_ERR_OK)
            {
                return ret;
            }
        }
        else
        {
            /* Enable N-way */
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_FIB;
            sds_acc.regaddr = 0;
            if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &value)) != RT_ERR_OK)
            {
                return ret;
            }
            value |= (1<<12);
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
            {
                return ret;
            }

            /* Wait 10 ms (at least 1 ms) */
            schedule_timeout(1 * HZ / 100);

            /* Toggle Rx EN */
            if((ret = fiber_rxEn_toggle(isFiber1G)) != RT_ERR_OK)
            {
                return ret;
            }

            /* Disable N-way */
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_FIB;
            sds_acc.regaddr = 0;
            if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &value)) != RT_ERR_OK)
            {
                return ret;
            }
            value &= ~(1<<12);
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
            {
                return ret;
            }
        }             
    }
    else
    {
        /* Force RXIDLE */
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_MISC;
        sds_acc.regaddr = 2;
        value = 0xc000;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
        {
            return ret;
        }

        /* Wait 10 ms (at least 1 ms) */
        schedule_timeout(1 * HZ / 100);

        /* Toggle Rx EN */
        if((ret = fiber_rxEn_toggle(isFiber1G)) != RT_ERR_OK)
        {
            return ret;
        }

        /* Release RXIDLE */
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_MISC;
        sds_acc.regaddr = 2;
        value = 0x0;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, value)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    return RT_ERR_OK;
}

static int fiber_monitor_thread(void *data)
{
    int ret;
    uint8 isFiber1G = 0;
    uint8 isWaiting = 0;
    uint8 isRxErr = 0;
    uint16 value;
    uint16 accFail = 0;
    uint16 waitCnt, threshold;
    uint32 counter;
    rtl9601b_sds_acc_t sds_acc;

    /* PON SDS analog serdes debug signal init */
    if ((ret = fiber_debugSignal_init()) != RT_ERR_OK)
    {
        printk("%s:%d analog serdes debug signal init failed(%d)\n", __FILE__, __LINE__, ret);
    }

    while(!kthread_should_stop())
    {
        /* It is NOT possible to wake up earlier */
        set_current_state(TASK_UNINTERRUPTIBLE);
        schedule_timeout(1 * HZ / 10); /* Wake up every 100 ms*/

        /* Check RXIDLE */
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_SDS_EXT;
        sds_acc.regaddr = 31;
        if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        if(0 == (value & (1<<1)))
        {
            /* Get Fiber SPEED */
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_FIB;
            sds_acc.regaddr = 0;
            if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &value)) != RT_ERR_OK)
            {
                return ret;
            }
            isFiber1G = (value & (1<<6)) ? 1 : 0;

            if(!isWaiting)
            {
                /* Reset counter */
                if ((ret = fiber_cnt_reset(isFiber1G)) != RT_ERR_OK)
                {
                    printk("%s:%d fiber counter reset failed(%d)\n", __FILE__, __LINE__, ret);
                }
                waitCnt = (waitCntCfg != 0) ? waitCntCfg : 1; /* Wait at least 1 time unit */
                threshold = thresholdCfg;
                isWaiting = 1;
            }
            else
            {
                waitCnt --;
                if(0 == waitCnt)
                {
                    /* Retrieve counter */
                    if ((ret = fiber_cnt_get(isFiber1G, &counter)) != RT_ERR_OK)
                    {
                        printk("%s:%d fiber counter get failed(%d)\n", __FILE__, __LINE__, ret);
                    }
                    if(counter >= threshold)
                    {
                        FIBER_DBG_PRINT("exceed threshold (%d >= %d)\n", counter, threshold);
                        accFail ++;
                    }
                    else
                    {
                        accFail = 0;
                    }

                    sds_acc.index = RTL9601B_SDS_IDX_PON;
                    sds_acc.page = RTL9601B_SDS_PAGE_SDS;
                    sds_acc.regaddr = 29;
                    if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &value)) != RT_ERR_OK)
                    {
                        return ret;
                    }
                    isRxErr = (value & 0x1) ? 1 : 0;

                    if((accFail >= 2) || isRxErr)
                    {
#if 0
                        if ((ret = fiber_rxSd_toggle()) != RT_ERR_OK)
                        {
                            printk("%s:%d fiber toggle failed(%d)\n", __FILE__, __LINE__, ret);
                        }
#else
                        if ((ret = fiber_recovery(isFiber1G)) != RT_ERR_OK)
                        {
                            printk("%s:%d fiber recover failed(%d)\n", __FILE__, __LINE__, ret);
                        }

                        /* Wait 50 ms */
                        schedule_timeout(5 * HZ / 100);
#endif
                        accFail = 0;
                    }
                    isWaiting = 0;
                }
            }
        }
        else
        {
            accFail = 0;
            isWaiting = 0;
        }
    }

    return 0;
}
#endif

static int fiber_mode_read(struct seq_file *seq, void *v)
{
    int i;

	seq_printf(seq, "fiber_mode = %s\n", strFiberMode[fiber_mode]);
	seq_printf(seq, "pre-defined fiber_mode = %s\n", strPonMode[predef_fiber_mode]);
    for(i = 0;i < FIBER_MODE_END;i ++) {
    	seq_printf(seq, "   %d:%s\n", i, strFiberMode[i]);
    }

	return 0;
}

static int fiber_mode_write(struct file *file, const char *buffer, size_t count, loff_t *ppos)
{
    int ret;
	unsigned char tmpBuf[16] = {0};
	int len = (count > 15) ? 15 : count;
    int value;
    uint32 chipId, rev, subType;

	if (buffer && !copy_from_user(tmpBuf, buffer, len))
	{
        value = simple_strtoul(tmpBuf, NULL, 10);
        if(value < FIBER_MODE_END)
        {
#ifdef CONFIG_TX_DISABLE_GPIO_PIN
            /* Disable Tx disable by default */
            if ((ret = rtk_gpio_mode_set(CONFIG_TX_DISABLE_GPIO_PIN, GPIO_OUTPUT)) != RT_ERR_OK)
            {
                printk("fiber tx disable config 1 fail %d\n", ret);
                return ret;
            }
            if ((ret = rtk_gpio_databit_set(CONFIG_TX_DISABLE_GPIO_PIN, 0)) != RT_ERR_OK)
            {
                printk("fiber tx disable config 2 fail %d\n", ret);
                return ret;
            }
            if ((ret = rtk_gpio_state_set(CONFIG_TX_DISABLE_GPIO_PIN, ENABLED)) != RT_ERR_OK)
            {
                printk("fiber tx disable config 3 fail %d\n", ret);
                return ret;
            }
#endif

            /* Configure fiber mode */
    		fiber_mode = value;
            switch(fiber_mode)
            {
            case FIBER_MODE_AN_1G:
                rtk_ponmac_mode_set(PONMAC_MODE_FIBER_1G);
                printk("fiber %s mode init complete!\n", strFiberMode[fiber_mode]);
                break;
            case FIBER_MODE_AUTO:
                rtk_ponmac_mode_set(PONMAC_MODE_FIBER_AUTO);
                printk("fiber %s mode init complete!\n", strFiberMode[fiber_mode]);
                break;
            case FIBER_MODE_FORCE_1G:
                rtk_ponmac_mode_set(PONMAC_MODE_FIBER_FORCE_1G);
                printk("fiber %s mode init complete!\n", strFiberMode[fiber_mode]);
                break;
            case FIBER_MODE_FORCE_100M:
                rtk_ponmac_mode_set(PONMAC_MODE_FIBER_FORCE_100M);
                printk("fiber %s mode init complete!\n", strFiberMode[fiber_mode]);
                break;
            case FIBER_MODE_SGMII_1G:
                rtk_ponmac_mode_set(PONMAC_MODE_SDS_SGMII_1G);
                printk("SDS %s mode init complete!\n", strFiberMode[fiber_mode]);
                break;
            case FIBER_MODE_PREDEF:
                rtk_ponmac_mode_set(predef_fiber_mode);
                printk("fiber %s (%s) mode init complete!\n", strFiberMode[fiber_mode], strPonMode[predef_fiber_mode]);
                break;
            default:
            	return -EFAULT;
            }
            
            /* Additional settings */
            if((ret = rtk_switch_version_get(&chipId, &rev, &subType)) != RT_ERR_OK)
            {
                return ret;
            }
#if CONFIG_SDK_RTL9601B
            if(RTL9601B_CHIP_ID == chipId)
            {
                rtk_led_config_t ledConfig;

                /* Disable VLAN for only media converter usage */
                rtk_vlan_vlanFunctionEnable_set(DISABLED);
                /* LED configuration */
                rtk_led_parallelEnable_set(1,ENABLED);

                memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
                ledConfig.ledEnable[LED_CONFIG_SPD1000ACT] = ENABLED;
                ledConfig.ledEnable[LED_CONFIG_SPD100ACT] = ENABLED;
                ledConfig.ledEnable[LED_CONFIG_SPD1000] = ENABLED;
                ledConfig.ledEnable[LED_CONFIG_SPD100] = ENABLED;
                if((ret = rtk_led_config_set(1,LED_TYPE_FIBER1,&ledConfig)) != RT_ERR_OK)
                {
                    return ret;
                }

                /* Enable CPU port queue 0 egress drop */
                rtl9601b_raw_flowctrl_egressDropEnable_set(2, 0, ENABLED);

                if(NULL == pFiberMonitor)
                {
                    pFiberMonitor = kthread_create(fiber_monitor_thread, NULL, "fiberMonitor");
                    if(IS_ERR(pFiberMonitor))
                    {
                        printk("%s:%d fiber monitor init failed %ld!\n", __FILE__, __LINE__, PTR_ERR(pFiberMonitor));
                        pFiberMonitor = NULL;
                    }
                    else
                    {
                        wake_up_process(pFiberMonitor);
                        printk("%s:%d fiber monitor init complete!\n", __FILE__, __LINE__);
                    }
                }
            }
#endif
        }
        else
        {
            printk("Unsupport settings\n");
        }
		return count;
	}
	return -EFAULT;
}

static int fiber_debug_read(struct seq_file *seq, void *v)
{
	seq_printf(seq, "fiberDbg: %s\n", fiberDbg ? "enable" : "disable");

	return 0;
}

static int fiber_debug_write(struct file *file, const char *buffer, size_t count, loff_t *ppos)
{
	unsigned char tmpBuf[16] = {0};
	int len = (count > 15) ? 15 : count;
    int value;

	if (buffer && !copy_from_user(tmpBuf, buffer, len))
	{
        value = simple_strtoul(tmpBuf, NULL, 10);
        if(value != 0)
        {
            fiberDbg = 1;
        }
        else
        {
            fiberDbg = 0;
        }

		return len;
	}
	return -EFAULT;
}

static int fiber_mode_open_proc(struct inode *inode, struct file *file)
{
	return single_open(file, fiber_mode_read, NULL);
}

static int fiber_debug_open_proc(struct inode *inode, struct file *file)
{
	return single_open(file, fiber_debug_read, NULL);
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
struct proc_ops fiber_mode_fop = {
	.proc_open = fiber_mode_open_proc,
	.proc_write = fiber_mode_write,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};
struct proc_ops fiber_debug_fop = {
	.proc_open = fiber_debug_open_proc,
	.proc_write = fiber_debug_write,	
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};
#else
struct file_operations fiber_mode_fop = {
	.open = fiber_mode_open_proc,
	.write = fiber_mode_write,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};
struct file_operations fiber_debug_fop = {
	.open = fiber_debug_open_proc,
	.write = fiber_debug_write,	
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};
#endif

static void fiber_procCmd_init(void)
{
    /* Create proc debug commands */
	fiber_mode_entry = proc_create("fiber_mode", 0, NULL, &fiber_mode_fop);
	if (!fiber_mode_entry) {
		printk("proc fiber_mode_entry create fail\n");
	}
	fiber_debug = proc_create("fiber_debug", 0, NULL, &fiber_debug_fop);
	if (!fiber_debug) {
		printk("proc fiber_debug create fail\n");
	}
	
}

static void fiber_procCmd_exit(void)
{
    /* Remove proc debug commands */
    if(fiber_mode_entry)
    {
    	remove_proc_entry("fiber_mode", NULL);
        fiber_mode_entry = NULL;
    }
    if(fiber_debug)
    {
    	remove_proc_entry("fiber_debug", NULL);
        fiber_debug = NULL;
    }
}

int __init fiber_mode_init(void)
{
    int ret;
#ifdef CONFIG_FIBER_SPEED_1G
    char mode_str[] = "AN 1G";
    predef_fiber_mode = PONMAC_MODE_FIBER_1G;
#elif defined(CONFIG_FIBER_SPEED_100M)
    char mode_str[] = "AN 100M";
    predef_fiber_mode = PONMAC_MODE_FIBER_100M;
#elif defined(CONFIG_FIBER_SPEED_AUTO)
    char mode_str[] = "auto 100M/1G";
    predef_fiber_mode = PONMAC_MODE_FIBER_AUTO;
#elif defined(CONFIG_FIBER_SPEED_FORCE_1G)
    char mode_str[] = "Force 1G";
    predef_fiber_mode = PONMAC_MODE_FIBER_FORCE_1G;
#elif defined(CONFIG_FIBER_SPEED_FORCE_100M)
    char mode_str[] = "Force 100M";
    predef_fiber_mode = PONMAC_MODE_FIBER_FORCE_100M;
#elif defined(CONFIG_SDS_SGMII_1G)
    char mode_str[] = "SGMII 1G";
    predef_fiber_mode = PONMAC_MODE_SDS_SGMII_1G;
#else
    char mode_str[] = "unknown";
    printk("fiber %s mode init fail\n", mode_str);
#endif

    fiber_procCmd_init();

    return 0;
}

void __exit fiber_mode_exit(void)
{
    fiber_procCmd_exit();

    return;
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek fiber module");
MODULE_AUTHOR("Realtek");
module_init(fiber_mode_init);
module_exit(fiber_mode_exit);

#endif /* CONFIG_FIBER_FEATURE */

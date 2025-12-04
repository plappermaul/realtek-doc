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
 * Purpose : external phy auto polling kernel thread
 *
 * Feature : Use kernel thread to perodically polling the external phy state
 *
 */

 /*
  * Include Files
  */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/version.h>
#include <linux/delay.h>

/* For RTK APIs */
#include <common/rt_error.h>
#include <rtk/port.h>
#include <rtk/mdio.h>
#include <rtk/gpio.h>

/*
 * Data Declaration
 */
unsigned int ext_phy_polling_flag = 1;
extern struct proc_dir_entry *realtek_proc;
struct task_struct *pExtPhyPollTask;

#define DEBUG_ALWAYS   0xffffffff

//#define DEBUG_RGMII
#ifdef DEBUG_RGMII
static unsigned int _debug_ = (DEBUG_ALWAYS);//|DEBUG_READ|DEBUG_WRITE|DEBUG_ERASE);
#define DEBUG_RGMII_PRINT(mask, string) \
			if ((_debug_ & mask) || (mask == DEBUG_ALWAYS)) \
			printk string
#else
#define DEBUG_RGMII_PRINT(mask, string)
#endif

#if 1
#define DEBUG_PRINT(mask, string) \
	{ if (ext_phy_polling_flag & mask) \
		printk string; }
#else
#define DEBUG_PRINT(mask, string)
#endif

// for branch code, they do not add these compile flags
#ifndef CONFIG_MDC_MDIO_HW_SET
#define CONFIG_MDC_MDIO_HW_SET 		0
#endif
#ifndef CONFIG_MDC_MDIO_EXT_PORT
#define CONFIG_MDC_MDIO_EXT_PORT 	6
#endif

#define MDC_MDIO_EXT_PORT 	CONFIG_MDC_MDIO_EXT_PORT
#define xGMII_EXT_PHYID 5
#define BIT(nr)			(1UL << (nr))

//external phy register
#define EXT_LINKED 		BIT(2)
#define GIGA_FUL   		BIT(11)
#define GIGA_HAL   		BIT(10)
#define FUL_100			BIT(8)
#define HAL_100			BIT(7)
#define FUL_10			BIT(6)
#define HAL_10			BIT(5)
#define R0_SPEED0		BIT(13)
#define R0_AUTO_NEGO	BIT(12)
#define R0_DUPLEX		BIT(8)
#define R0_SPEED1		BIT(6)
#define R4_ASM_DIR		BIT(11)
#define R4_PAUSE		BIT(10)
#define R5_ASM_DIR		BIT(11)
#define R5_PAUSE		BIT(10)
#define R9_GIGA_FUL_CAP BIT(9)

#define RTL9607C_SET0_MDC_PIN       6
#define RTL9607C_SET0_MDIO_PIN      7
#define RTL9607C_SET1_MDC_PIN       12
#define RTL9607C_SET1_MDIO_PIN      10

typedef enum rtk_phy_state_e
{
    PS_NONE             = 0,
    PS_GIGA_FULL,
    PS_GIGA_HALF,
    PS_100M_FULL,
    PS_100M_HALF,
    PS_10M_FULL,
    PS_10M_HALF,
    PS_LINK_DOWN,
    PS_END
} rtk_phy_state_t;

typedef enum rtk_phy_ability_e
{
    PA_10M             = 0,
    PA_100M,
    PA_1000M,
    PA_AUTO,
    PA_END
} rtk_phy_ability_t;

const static char state_str[8][8] = {
	"None",
	"1000F",
	"1000H",
	"100F",
	"100H",
	"10F",
	"10H",
	"Down"
};

const static unsigned int state_speed[8] = {
	PORT_SPEED_10M, PORT_SPEED_1000M, PORT_SPEED_1000M, PORT_SPEED_100M,
	PORT_SPEED_100M, PORT_SPEED_10M, PORT_SPEED_10M, PORT_SPEED_10M
};

const static unsigned int state_duplex[8] = {
	PORT_HALF_DUPLEX, PORT_FULL_DUPLEX, PORT_HALF_DUPLEX, PORT_FULL_DUPLEX,
	PORT_HALF_DUPLEX, PORT_FULL_DUPLEX, PORT_HALF_DUPLEX, PORT_HALF_DUPLEX
};

/* for ext_phy_polling_flag */
enum EP_BIT_DEF {
	EP_POLLING_ENABLED		= 0x00000001,
	EP_DBG_PHY_STATE		= 0x00000002,
};

static int mdio_init=0;
static int state = PS_NONE;
static unsigned short prev_reg1=0;

#define CHK_PRK(x) do { \
    if ((ret = x) != RT_ERR_OK) { \
        printk("%s-%d error ret=%d",__FILE__,__LINE__,ret); \
        return ret; \
    } \
} while(0)

#define CHK(x) do { \
    if ((ret = x) != RT_ERR_OK) { \
        return ret; \
    } \
} while(0)

/*
 * Function Declaration
 */
int config_mac_speed(unsigned int port, int state, unsigned int tx_fc, unsigned int rx_fc)
{
	rtk_port_macAbility_t port_mac_ability;
	int ret;

	CHK_PRK(rtk_port_macForceAbility_get(port,&port_mac_ability));

	if (state == PS_LINK_DOWN)
		port_mac_ability.linkStatus = PORT_LINKDOWN;
	else {
		port_mac_ability.speed = state_speed[state];
		port_mac_ability.duplex = state_duplex[state];
		port_mac_ability.linkStatus = PORT_LINKUP;
		port_mac_ability.txFc = tx_fc;
		port_mac_ability.rxFc = rx_fc;
	}
	CHK_PRK(rtk_port_macForceAbility_set(port,port_mac_ability));

	if (state != PS_LINK_DOWN)
		CHK_PRK(rtk_port_macForceAbilityState_set(port,ENABLED));

	return ret;
}

int rtk_set_phy_ability(unsigned int ability)
{
	unsigned short reg0,reg4,reg9;
	int ret;

	CHK(rtk_mdio_c22_read(PHY_REG_1000_BASET_CONTROL,&reg9));
	CHK(rtk_mdio_c22_read(PHY_REG_AN_ADVERTISEMENT,&reg4));
	CHK(rtk_mdio_c22_read(PHY_REG_CONTROL,&reg0));

	if ((ability == PA_AUTO) || (ability == PA_1000M)) {
		reg9 |= R9_GIGA_FUL_CAP;
		CHK(rtk_mdio_c22_write(PHY_REG_1000_BASET_CONTROL,reg9));
		reg4 |= (FUL_100|HAL_100|FUL_10|HAL_10);
		CHK(rtk_mdio_c22_write(PHY_REG_AN_ADVERTISEMENT,reg4));
	}
	else if (ability == PA_100M) {
		reg9 &= ~R9_GIGA_FUL_CAP;
		CHK(rtk_mdio_c22_write(PHY_REG_1000_BASET_CONTROL,reg9));
		reg4 |= (FUL_100|HAL_100|FUL_10|HAL_10);
		CHK(rtk_mdio_c22_write(PHY_REG_AN_ADVERTISEMENT,reg4));
	}
	else if (ability == PA_10M) {
		reg9 &= ~R9_GIGA_FUL_CAP;
		CHK(rtk_mdio_c22_write(PHY_REG_1000_BASET_CONTROL,reg9));
		reg4 = (reg4 & ~(FUL_100|HAL_100)) | (FUL_10|HAL_10);
		CHK(rtk_mdio_c22_write(PHY_REG_AN_ADVERTISEMENT,reg4));
	}
	reg0 |= BIT(9); // Restart Auto-Negotiation
	CHK(rtk_mdio_c22_write(PHY_REG_CONTROL,reg0));

	return ret;
}

int probe_ext_phy(void)
{
	unsigned short reg0,reg1,reg4,reg5,reg9,reg10;
	unsigned int tx_fc=DISABLED, rx_fc=DISABLED;
	int ret;

	if(!mdio_init){
		if (CONFIG_MDC_MDIO_HW_SET == 0) {
			CHK(rtk_gpio_state_set(RTL9607C_SET0_MDC_PIN, DISABLED));
			CHK(rtk_gpio_state_set(RTL9607C_SET0_MDIO_PIN, DISABLED));
		}
		else {
			CHK(rtk_gpio_state_set(RTL9607C_SET1_MDC_PIN, DISABLED));
			CHK(rtk_gpio_state_set(RTL9607C_SET1_MDIO_PIN, DISABLED));
		}
		CHK(rtk_mdio_init());
		CHK(rtk_mdio_cfg_set(CONFIG_MDC_MDIO_HW_SET,0,xGMII_EXT_PHYID,MDIO_FMT_C22));

		CHK(rtk_mdio_c22_read(PHY_REG_CONTROL,&reg0));
		CHK(rtk_mdio_c22_write(PHY_REG_CONTROL,(reg0 | BIT(11))));
		mdelay(100);
		CHK(rtk_mdio_c22_write(PHY_REG_CONTROL,reg0));

		mdio_init=1;
	}

	CHK(rtk_mdio_c22_read(PHY_REG_STATUS,&reg0)); // dummy read
	CHK(rtk_mdio_c22_read(PHY_REG_STATUS,&reg1));

	//read success, means you got a external phy.
	if ((reg1 & EXT_LINKED) && !(prev_reg1 & EXT_LINKED)){
		/*linkup*/
		CHK(rtk_mdio_c22_read(PHY_REG_CONTROL,&reg0));
		CHK(rtk_mdio_c22_read(PHY_REG_AN_ADVERTISEMENT,&reg4));

		if (reg0 & R0_AUTO_NEGO) {
			CHK(rtk_mdio_c22_read(PHY_REG_1000_BASET_CONTROL,&reg9));
			CHK(rtk_mdio_c22_read(PHY_REG_1000_BASET_STATUS,&reg10));
			CHK(rtk_mdio_c22_read(PHY_REG_AN_LINKPARTNER,&reg5));

			if ((reg9 & R9_GIGA_FUL_CAP) && (reg10 & GIGA_FUL)) {
				/*link partner is 1000 Full duplex*/
				state = PS_GIGA_FULL;
			}
			else if ((reg9 & R9_GIGA_FUL_CAP) && (reg10 & GIGA_HAL)) {
				/*link partner is 1000 Half duplex*/
				state = PS_GIGA_HALF;
			}
			else{
				if (reg4 & reg5 & FUL_100)
					state = PS_100M_FULL;
				else if (reg4 & reg5 & HAL_100)
					state = PS_100M_HALF;
				else if (reg4 & reg5 & FUL_10)
					state = PS_10M_FULL;
				else if (reg4 & reg5 & HAL_10)
					state = PS_10M_HALF;
				else {
					state = PS_NONE;
					printk("[%s-%d] can't match any speed/duplex for current phy status!\n",__func__,__LINE__);
					return ret;
				}
			}
			/* refer to the table 37-4 of IEEE802.3 */
			reg4 = (reg4 >> 10) & 0x3;
			if (reg4 == 2) {
				if ((reg5 & (R5_PAUSE|R5_ASM_DIR)) == (R5_PAUSE|R5_ASM_DIR))
					tx_fc = ENABLED;
			}
			else if (reg4 == 1) {
				if (reg5 & R5_PAUSE) {
					tx_fc = ENABLED;
					rx_fc = ENABLED;
				}
			}
			else if (reg4 == 3) {
				if ((reg5 & (R5_PAUSE|R5_ASM_DIR)) == (R5_ASM_DIR))
					rx_fc = ENABLED;
				else if (reg5 & R5_PAUSE) {
					tx_fc = ENABLED;
					rx_fc = ENABLED;
				}
			}
		}
		else {
			/* Note: for local phy force mode
			 *	reg0.8:duplex mode, 1:full, 0:half
			 *	reg0.6 and reg0.11:speed, 0b00:10M, 0b01:100M, 0b10:1000M
			 */
			if ((reg0 & R0_SPEED1) && (reg0 & R0_SPEED0)) {
				state = PS_NONE;
				printk("[%s-%d] can't match any speed/duplex for current phy setting!\n",__func__,__LINE__);
				return ret;
			}
			else if (reg0 & R0_SPEED1) {
				state = PS_GIGA_FULL;
			}
			else if (reg0 & R0_SPEED0) {
				if (reg0 & R0_DUPLEX)
					state = PS_100M_FULL;
				else
					state = PS_100M_HALF;
			}
			else {
				if (reg0 & R0_DUPLEX)
					state = PS_10M_FULL;
				else
					state = PS_10M_HALF;
			}

			if (reg4 & R4_PAUSE) {
				tx_fc = ENABLED;
				rx_fc = ENABLED;
			}
		}
		config_mac_speed(MDC_MDIO_EXT_PORT,state,tx_fc,rx_fc);
		DEBUG_PRINT(EP_DBG_PHY_STATE, ("port %d phy link up, state= %s\n",MDC_MDIO_EXT_PORT,state_str[state]));
	}
	else if (!(reg1 & EXT_LINKED) && (prev_reg1 & EXT_LINKED)){
		/*link down*/
		state = PS_LINK_DOWN;
		config_mac_speed(MDC_MDIO_EXT_PORT,state,tx_fc,rx_fc);
		DEBUG_PRINT(EP_DBG_PHY_STATE, ("port %d phy link down, state= %s\n",MDC_MDIO_EXT_PORT,state_str[state]));
	}
	prev_reg1 = reg1;

	return ret;
}

int ext_phy_polling_thread(void *data)
{
	while(!kthread_should_stop())
	{
		/* No need to wake up earlier */
		set_current_state(TASK_UNINTERRUPTIBLE);
		schedule_timeout(5 * HZ / 10);
		if(ext_phy_polling_flag & EP_POLLING_ENABLED){
			probe_ext_phy();
		}
	}
	return 0;
}

static int ext_phy_polling_read(struct seq_file *s, void *v)
{
	seq_printf(s, "[ext_phy_polling_flag = 0x%08x]\n", ext_phy_polling_flag);
	seq_printf(s, "0x1: enable external phy polling\n");
	seq_printf(s, "0x2: enable debug message of external phy polling\n");

	return 0;
}

static int ext_phy_polling_open(struct inode *inode, struct file *file)
{
	return single_open(file, ext_phy_polling_read, (void *) file);
}

static ssize_t ext_phy_polling_write(struct file * file, const char __user * buffer, size_t count, loff_t * off)
{
	unsigned char tmpBuf[64] = {0};
	char	*strptr, *cmd_addr, *tokptr;
	unsigned int ability;

	if(count>=64)
		goto errout;
	
	if (buffer && !copy_from_user(tmpBuf, buffer, count))
	{
		tmpBuf[count] = '\0';
		strptr=tmpBuf;
		cmd_addr = strsep(&strptr," ");
		if (cmd_addr==NULL)
			goto errout;

		if (!memcmp(cmd_addr, "help", 4)) {
			printk("echo phy_ability [10M / 100M / 1000M / auto] > /proc/realtek/ext_phy_polling\n");
			printk("echo [ext_phy_polling_flag] > /proc/realtek/ext_phy_polling\n");
		}
		else if (!memcmp(cmd_addr, "phy_ability", 11))
		{
			tokptr = strsep(&strptr," ");
			if (tokptr==NULL)
				goto errout;

			if(strncmp(tokptr,"10M", 3) == 0)
				ability = PA_10M;
			else if(strncmp(tokptr,"100M", 4) == 0)
				ability = PA_100M;
			else if(strncmp(tokptr,"1000M", 5) == 0)
				ability = PA_1000M;
			else if(strncmp(tokptr,"auto", 4) == 0)
				ability = PA_AUTO;
			else
				ability = PA_AUTO;
			rtk_set_phy_ability(ability);
		}
		else {
			ext_phy_polling_flag = simple_strtoul(cmd_addr, NULL, 16);
			printk("write ext_phy_polling_flag to 0x%08x\n", ext_phy_polling_flag);
		}
		return count;
	}
errout:
	return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops ext_phy_polling_proc_fops = {
	.proc_open		= ext_phy_polling_open,
	.proc_read		= seq_read,
	.proc_write		= ext_phy_polling_write,
	.proc_release	= single_release,
};
#else
static const struct file_operations ext_phy_polling_proc_fops = {
	.owner		= THIS_MODULE,
	.open		= ext_phy_polling_open,
	.read		= seq_read,
	.write		= ext_phy_polling_write,
	.release	= single_release,
};
#endif

static void ext_phy_polling_dbg_init(void)
{
	/* Create proc debug commands */
	if (proc_create("ext_phy_polling", 0, realtek_proc, &ext_phy_polling_proc_fops) == NULL) {
		printk("Fail to create ext_phy_polling proc entry.\n");
	}
}

static void ext_phy_polling_dbg_exit(void)
{
	/* Remove proc debug commands */
	remove_proc_entry("ext_phy_polling", realtek_proc);
}

int __init ext_phy_poling_init(void)
{
	pExtPhyPollTask = kthread_create(ext_phy_polling_thread, NULL, "ext_phy_polling");
	if(IS_ERR(pExtPhyPollTask))
	{
		printk("[%s-%d] init failed %ld!\n", __FILE__, __LINE__, PTR_ERR(pExtPhyPollTask));
	}
	else
	{
		wake_up_process(pExtPhyPollTask);
	}
	ext_phy_polling_dbg_init();

	return 0;
}

void __exit ext_phy_polling_exit(void)
{
	printk("[%s-%d] exit!\n", __FILE__, __LINE__);
	mdio_init = 0;
	kthread_stop(pExtPhyPollTask);
	ext_phy_polling_dbg_exit();
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek external phy auto polling module");
MODULE_AUTHOR("David Chen <david_cw@realtek.com>");
module_init(ext_phy_poling_init);
module_exit(ext_phy_polling_exit);


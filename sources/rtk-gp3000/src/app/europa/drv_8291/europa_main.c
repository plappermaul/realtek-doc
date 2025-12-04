#ifdef CONFIG_RTL8291_FEATURE

#include <osal/memory.h>
#include <osal/sem.h>
#include <osal/time.h>
//#include <common/debug/rt_log.h>

#include <linux/module.h>  /* Needed by all modules */
#include <linux/kernel.h>  /* Needed for KERN_ALERT */
#include <linux/init.h>
#include <linux/version.h>

#include <linux/spinlock.h>
#include <linux/spinlock_types.h>

#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>      // Needed by filp
#include <asm/uaccess.h>   // Needed by segment descriptors

#include <linux/delay.h>

#include "include/rtl8291_reg_acc.h"
#include "include/rtl8291_init.h"
#include "include/rtl8291_reg_definition.h"
#include "include/ddmi.h"

#include <osal/sem.h>

#include <rtk/ldd.h>
#include <rtk/ponmac.h>
#include <rtk/switch.h>
#include <rtk/gpon.h>
#if defined(CONFIG_COMMON_RT_API)
#include <rtk/rt/rt_i2c.h>
#endif

#if defined(CONFIG_LUNA_G3_SERIES)
#include <soc/cortina/registers.h>
#include "cortina-api/include/gpon.h"
#include "cortina-api/include/port.h"
#include "aal_pon.h"
#include "aal_ploam.h"
#endif

//#include "cortina-api/include/gpon.h"
//#include "cortina-api/include/port.h"
//#include "aal_pon.h"
//#include "aal_ploam.h"
//#include "scfg.h"

#include <ioal/mem32.h>
#include <hal/chipdef/allreg.h>

//#if defined(CONFIG_LUNA_G3_SERIES)
//#include <linux/mutex.h>
//DEFINE_MUTEX(ldd_mutex);
//#else
#include <osal/sem.h>
osal_mutex_t ldd_mutex;
//#endif

#ifndef FAILED
#define FAILED -1
#endif

extern uint8 ldd_pon_mode;

rtl8291_param_t ldd_param[LDD_DEVICE_NUM];

uint8 loopmode=1;  // default DCL mode

extern uint8 internal;

//20190211: For txsd mismatch workaround
uint8 txsd_fsu_done;
uint8 txsd_mis_patch;

//20190904: For False TXSD workaround
uint8 fsu_flag;
uint8 loop_mon_time;

uint16 rxlog_flag;

uint8 rtl8290b_pon_mode;

uint8 rxbound_flag;

//0 for enable, 1 for disable, 2 for link up  one-shot, 3 for O5 state only, 5 for debug log
uint8 workaround_ibim;

module_param(loopmode, byte, 0644);
MODULE_PARM_DESC(loopmode, "Europa Loop Mode: 0 for DOL, 1 for DCL, 2 for SCL bias, 3 for SCL mode.");

//uint16 loopcnt=100;
//module_param(loopcnt, short, 0644);
//MODULE_PARM_DESC(loopcnt, "Europa Loop Count: SDADC RSSI calculation loop.");


/* board dependent parameters + */
#ifdef CONFIG_OE_MODULE_I2C_PORT_0
uint8 I2C_PORT=0;
#else
uint8 I2C_PORT=1;
#endif
module_param(I2C_PORT, byte, 0644);
MODULE_PARM_DESC(I2C_PORT, "Europa i2c port.");

uint8 PON_MODE=1;  /* default GPON mode */
module_param(PON_MODE, byte, 0644);
MODULE_PARM_DESC(PON_MODE, "Europa PON Mode.");

/* board dependent parameters - */

uint8 flash_update;

uint16 europa_chip_type;

struct task_struct *pEuropa8291DdmiPollTask = NULL;
struct task_struct *pEuropaIntPollTask = NULL;

uint8 flash_data[LDD_FLASH_DATA_SIZE];

static uint8 calibration_state = FALSE;

//static uint8 europa_debug_level = 1;   /* TODO: debug prints APIs */

extern int ldd_ploam_state;

uint32 count_count = 0; 

#if 0
void europa_debug_level_set(uint8 level)
{
    europa_debug_level = level;
}

uint8 europa_debug_level_get()
{
    return europa_debug_level;
}
#endif


#if 0
int ca77_bwparser(void) 
{
    int ret;
    uint32 chipId, rev, subType;

#define SMALL_LENGTH 125
     
	GPON_MAC_GPON_bwmap_ctl_t	ctl 	= {0};
	GPON_MAC_GPON_bwmap_data0_t data0	= {0};
	GPON_MAC_GPON_bwmap_data1_t data1	= {0};
	uint32 				idx 	= 0;
	uint16				allocId = 0;
	uint8					plsu	= 0;
	uint8				ploamu	= 0;
	uint8				fec 	= 0;
	uint8					dbru	= 0;
	uint8					res 	= 0;
	uint32 				Sstart	= 0;
	uint32 				Sstop	= 0;
	
    if((ret = rtk_switch_version_get(&chipId, &rev, &subType)) != 0)
    {
        return ret;
    }

    //printk(KERN_EMERG "rtl8290b_bwparser: chipId = 0x%x\n", chipId);    

    //PON_MODE: 1 for GPON, 2 for EPON
    if (PON_MODE == 1)
    {
        if (ldd_ploam_state == 5)
        {                          		
			//EUROPA_MUTEX_LOCK();
			
			ctl.wrd = AAL_PON_REG_READ(GPON_MAC_GPON_bwmap_ctl);
			if (0 == ctl.bf.hwdone)
			{
				//EUROPA_MUTEX_UNLOCK();
				//ca_printf("Can't enable capture when hwdone bit is 0\r\n");
				return SUCCESS;
			}
			
			ctl.bf.hwdone = 0;
			ctl.bf.enable = 1;
			AAL_PON_REG_WRITE(ctl.wrd, GPON_MAC_GPON_bwmap_ctl);
			
			//_europa_delay();
			
			mdelay(5);
			
			//get counter now
			memset(&ctl, 0, sizeof(ctl));
			ctl.wrd = AAL_PON_REG_READ(GPON_MAC_GPON_bwmap_ctl);
			if (0 == ctl.bf.count)
			{
				AAL_PON_REG_FIELD_WRITE(GPON_MAC_GPON_bwmap_ctl, enable, 0);
				//EUROPA_MUTEX_UNLOCK();
				//ca_printf("Capture NULL\r\n");
				return SUCCESS;
			}
			
			//scan all the bwmap data
			//ca_printf("\r\n");
			//ca_printf("%-3s|%-7s|%-4s|%-6s|%-3s|%-4s|%-3s|%-6s|%-6s|%-5s\r\n",
			//		  "No","allocId","plsu","ploamu","fec","dbru","res", "Sstart", "Sstop", "len");
			//ca_printf("----------------------------------------------------------\r\n");
			for (idx = 0; idx < ctl.bf.count; idx++)
			{
				allocId = 0;
				plsu	= 0;
				ploamu	= 0;
				fec 	= 0;
				dbru	= 0;
				res 	= 0;
				Sstart	= 0;
				Sstop	= 0;
				memset(&data0, 0, sizeof(data0));
				memset(&data1, 0, sizeof(data1));
			
				data0.wrd = AAL_PON_REG_READ(GPON_MAC_GPON_bwmap_data0);
				data1.wrd = AAL_PON_REG_READ(GPON_MAC_GPON_bwmap_data1);
			
				allocId = (0xfff000 & data1.bf.data) >> 12; //bit12-23
				plsu	= (0x800 & data1.bf.data) >> 11; //bit 11
				ploamu	= (0x400 & data1.bf.data) >> 10; //bit 10
				fec 	= (0x200 & data1.bf.data) >> 9; //bit 9
				dbru	= (0x180 & data1.bf.data) >> 7; //bit 7-8
				res 	= 0x7F & data1.bf.data; //bit 0-6
				Sstart	= (0xffff0000 & data0.bf.data) >> 16; //bit 16-32
				Sstop	= 0xffff & data0.bf.data; //bit 0-15
			
				//ca_printf("%-3d|%-7d|%-4d|%-6d|%-3d|%-4d|%-3d|%-6d|%-6d|%-5d\r\n", 
				//			idx, allocId, plsu, ploamu, fec, dbru, res, Sstart, Sstop, (Sstop-Sstart+1));
			}
			
			//clear the value
			AAL_PON_REG_FIELD_WRITE(GPON_MAC_GPON_bwmap_ctl, enable, 0);
			//EUROPA_MUTEX_UNLOCK();

        }        

    }

    return SUCCESS;
 }
#endif

void calibration_state_set(uint8 state)
{
    // just assign the state
    calibration_state = state;
}

uint8 calibration_state_get(void)
{
    return calibration_state;
}

/* wake up ddmi polling thread */
void kick_8291_ddmi(void)
{
    if (NULL != pEuropa8291DdmiPollTask)
       wake_up_process(pEuropa8291DdmiPollTask);
}


int europa_8291_ddmi_polling_thread(void *data)
{
    uint16 regData;
    uint32 devId, dev_idx;

    regData = 0;  /* make compiler happy */
    devId = 0x5;
    dev_idx = 0;
    while(!kthread_should_stop())
    {
        printk(KERN_EMERG " update 8291_ddmi !!!!!!.\n");    
        //update_8291_ddmi(devId, &ldd_param[dev_idx]);

        /* No need to wake up earlier */
        set_current_state(TASK_UNINTERRUPTIBLE);
        schedule_timeout(3*HZ); 
    }

    return 0;
}


int europa_int_polling_thread(void *data)
{

    while(!kthread_should_stop())
    {
        /* No need to wake up earlier */
        set_current_state(TASK_UNINTERRUPTIBLE);
        schedule_timeout(5 * HZ / 10);
        //schedule_timeout(1 * HZ );

       //laser_drv_intrHandle();

       printk(KERN_EMERG " Interrupt Polling Task !!!!!!.\n");

    }

    return 0;
}

void europa_8291_poling_init(void)
{
    pEuropa8291DdmiPollTask = kthread_create(europa_8291_ddmi_polling_thread, NULL, "europa_8291_ddmi_polling");
    if(IS_ERR(pEuropa8291DdmiPollTask))
    {
        //printk("%s:%d europa_ddmi_polling_thread failed %ld!\n", __FILE__, __LINE__, PTR_ERR(pEuropaDdmiPollTask));
        printk(KERN_EMERG " 8291 DDMI Polling Task Add Fail!!!!.\n");
    }
    else
    {
        wake_up_process(pEuropa8291DdmiPollTask);
        //printk("%s:%d europa_ddmi_polling_thread complete!\n", __FILE__, __LINE__);
        printk(KERN_EMERG " DDMI Polling Task Add Success.\n");
    }

#if 0
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0) //Enable Interrupt Polling in linux4.4
    pEuropaIntPollTask = kthread_create(europa_int_polling_thread, NULL, "europa_int_polling");
    if(IS_ERR(pEuropaIntPollTask))
    {
        printk("%s:%d europa_int_polling_thread failed %ld!\n", __FILE__, __LINE__, PTR_ERR(pEuropaIntPollTask));
        printk(KERN_EMERG " Interrupt Polling Task Add Fail!!!!.\n");
    }
    else
    {
        wake_up_process(pEuropaIntPollTask);
        printk("%s:%d europa_int_polling_thread complete!\n", __FILE__, __LINE__);
        printk(KERN_EMERG " Interrupt Polling Task Add Success!!!!.\n");
    }
#endif
#endif
}

int __init europa_8291_module_init(void)
{
    //int32 retval;
    //uint32 width, clock;
    //uint8 time;
    uint8 dual_mode_flag;
    uint32 devId, pon_mode;
    uint32 value=0, value2=0;
    uint32 dev_idx=0;

    printk(KERN_EMERG "\nRTL8291 driver version [%d.%d.%d]\n\n",
            RTL8291_DRIVER_VERSION_MAJOR,
            RTL8291_DRIVER_VERSION_MINOR,
            RTL8291_DRIVER_VERSION_PATCH);

    printk(KERN_EMERG "Parameter: I2C_PORT=%d PON_MODE=%d\n\n", I2C_PORT, PON_MODE);
	
#if defined(CONFIG_LUNA_G3_SERIES)
	pon_mode = aal_pon_mac_mode_get(0);
#endif

    /* create 8290C API lock mutex */
    /*Note: In ARM-based SoC, ldd_mutex should be announced as uint64 to prevent kernel panic issue.  */
    ldd_mutex = osal_sem_mutex_create();
    printk(KERN_EMERG "osal_sem_mutex_create(): ldd_mutex = %d \n", ldd_mutex);     

    printk(KERN_EMERG "Init I2C for Europa.\n");

    rtl8291_i2c_init(I2C_PORT);

    //LDD_MUTEX_LOCK();
    //printk(KERN_EMERG "LDD_MUTEX_LOCK... \n");
    //LDD_MUTEX_UNLOCK();
    //printk(KERN_EMERG "LDD_MUTEX_UNLOCK... \n");

    devId = RTL8291_DEFAULT_DEVID;

#if 1
    rtl8291_chipInfo_get(devId, &value, &value2);	
    printk(KERN_EMERG "RTL NUM = 0x%04X \n", value);	 	
    if (value != 0x8291)
    {
        printk(KERN_EMERG "Error RTL NUM = 0x%04X !!! \n", value);    
        return FAILED;
    }
#endif

    _rtl8291_load_config_file(devId, flash_data);

    //rtl8290b_pon_mode = PON_MODE;
    //PON_MODE: 1 for GPON, 2 for EPON
    if (PON_MODE == 1)
    {
        ldd_pon_mode = RTL8291_GPON_MODE;
    }
    else if ((PON_MODE == 2)||(PON_MODE == 3))
    {
        dual_mode_flag = flash_data[RTL8291_PARAMETER_BASE_ADDR+RTL8291_DUAL_PON_MODE_OFFSET_ADDR]; 
        if (dual_mode_flag==1)
        {
            ldd_pon_mode = RTL8291_DUAL_EPON_MODE;
        }
        else
        {
            ldd_pon_mode = RTL8291_FORCE_EPON_MODE;
        }
    }
    else
    {
        ldd_pon_mode = RTL8291_GPON_MODE;
    }

#if 1 
    printk(KERN_EMERG "Init RTL8291 module devId = %d...\n", devId);
    dev_idx	= devId - 5;
    rtl8291_init(devId, flash_data, &ldd_param[dev_idx], 0);

    rtl8291_user_parameter_init(devId, &flash_data[RTL8291_PARAMETER_BASE_ADDR], &ldd_param[dev_idx]);
	
#endif
	
#if 0 

    //rxbound_flag = flash_data[RTL8290B_PARAM_ADDR_BASE+RTL8290B_RX_LOWER_BOUND_OFFSET_ADDR];

    printk(KERN_EMERG "ldd_param[%d].mpd0 = 0x%x\n", devId, ldd_param[devId].mpd0);        
        /* set threshold value */
        /* 1. REG_TEMP_TH   -- Get from DDMI A2 byte 0/1  alarm high */
    ddmi_temp_threshold_set();
        /* 2. REG_IMPD_TH   -- Read address (IMPD translate function 1 ) */
        //????????????????
        /* 3. REG_VOL_TH     -- Get from DDMI A2 byte 8/9 (voltage translate function 1) */
    ddmi_volt_threshold_set(internal);        
#endif

    // insert mapper functions
    europa_chip_type = EUROPA_CHIP_RTL8291;
    ldd_mapper_init(europa_chip_type);

#if 0 //RTL8291 Do not need software DDMI....
    europa_8291_poling_init();
#endif

#if 0
    // register ploam state handler
    ldd_ploamState_handler_init();
        
        /* Turn on Interrupt mask */
    europa_intrInit();


#endif 

    printk(KERN_EMERG "Init 8291 Finish.\n");        

    return 0;
}

void __exit europa_8291_exit(void)
{
    // remove mapper functions
    ldd_mapper_exit();
    printk(KERN_EMERG "ldd_mapper_exit...\n");  

//#if !defined(CONFIG_LUNA_G3_SERIES)
    osal_sem_mutex_destroy(ldd_mutex);
    printk(KERN_EMERG "osal_sem_mutex_destroy(ldd_mutex)...\n");    
//#endif	

    //kthread_stop(pEuropa8291DdmiPollTask);


#if 0
    // unregister ploam state handler
    ldd_ploamState_handler_exit();

    europa_intrDeInit();

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0) //Enable Interrupt Polling in linux4.4
    kthread_stop(pEuropaIntPollTask);
#endif

    osal_sem_mutex_destroy(europa_mutex);

    rtl8291_reset(LDD_RESET_CHIP);
#endif

    printk("%s(): exit\n",__FUNCTION__);
}

//MODULE_LICENSE("GPL");
MODULE_LICENSE("Proprietary");
MODULE_DESCRIPTION("RealTek RTL8291 kernel module");
MODULE_AUTHOR("RealTek");

module_init(europa_8291_module_init);
module_exit(europa_8291_exit);

#endif


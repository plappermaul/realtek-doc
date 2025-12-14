/*
 * Copyright (C) 2016 Realtek Semiconductor Corp.
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
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : PPSTOD kernel drivers
 *
 * Feature : This module install PPSTOD kernel callbacks and other 
 * kernel functions
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/math64.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/version.h>

#include <common/rt_error.h>

#include "rtk/time.h"
#include "rtk/ppstod.h"
#include "rtk/intr.h"

#include "soc/type.h"

#define GPS_UTC_DIFF_SEC 315964800
#define WEEK_SEC 604800
#define TOD_MAX_LEN 32

#define TOD_LEN 22

//9602C form bsp_rtl8686/bspchip_8686.h 
//9607C form bsp_rtl9607c/bspchip_9607c.h 
#define BSP_UART0_BASE      0xB8002000
#define BSP_UART0_RBR       (BSP_UART0_BASE + 0x000)
#define BSP_UART0_THR       (BSP_UART0_BASE + 0x000)
#define BSP_UART0_DLL       (BSP_UART0_BASE + 0x000)
#define BSP_UART0_IER       (BSP_UART0_BASE + 0x004)
#define BSP_UART0_DLM       (BSP_UART0_BASE + 0x004)
#define BSP_UART0_IIR       (BSP_UART0_BASE + 0x008)
#define BSP_UART0_FCR       (BSP_UART0_BASE + 0x008)
#define BSP_UART0_LCR       (BSP_UART0_BASE + 0x00C)
#define BSP_UART0_MCR       (BSP_UART0_BASE + 0x010)
#define BSP_UART0_LSR       (BSP_UART0_BASE + 0x014)

#define BSP_UART1_BASE      0xB8002100
#define BSP_UART1_RBR       (BSP_UART1_BASE + 0x000)
#define BSP_UART1_THR       (BSP_UART1_BASE + 0x000)
#define BSP_UART1_DLL       (BSP_UART1_BASE + 0x000)
#define BSP_UART1_IER       (BSP_UART1_BASE + 0x004)
#define BSP_UART1_DLM       (BSP_UART1_BASE + 0x004)
#define BSP_UART1_IIR       (BSP_UART1_BASE + 0x008)
#define BSP_UART1_FCR       (BSP_UART1_BASE + 0x008)
#define BSP_UART1_LCR       (BSP_UART1_BASE + 0x00C)
#define BSP_UART1_MCR       (BSP_UART1_BASE + 0x010)
#define BSP_UART1_LSR       (BSP_UART1_BASE + 0x014)

/*FCR*/
#define BSP_UART1_FCR       (BSP_UART1_BASE + 0x008)
#define BSP_FCR_EN          0x01
#define BSP_FCR_RXRST       0x02
#define     BSP_RXRST             0x02
#define BSP_FCR_TXRST       0x04
#define     BSP_TXRST             0x04
#define BSP_FCR_DMA         0x08
#define BSP_FCR_RTRG        0xC0
#define     BSP_CHAR_TRIGGER_01   0x00
#define     BSP_CHAR_TRIGGER_04   0x40
#define     BSP_CHAR_TRIGGER_08   0x80
#define     BSP_CHAR_TRIGGER_14   0xC0

/*LCR*/
#define BSP_LCR_WLN         0x03
#define     BSP_CHAR_LEN_5        0x00
#define     BSP_CHAR_LEN_6        0x01
#define     BSP_CHAR_LEN_7        0x02
#define     BSP_CHAR_LEN_8        0x03
#define BSP_LCR_STB         0x04
#define     BSP_ONE_STOP          0x00
#define     BSP_TWO_STOP          0x04
#define BSP_LCR_PEN         0x08
#define     BSP_PARITY_ENABLE     0x01
#define     BSP_PARITY_DISABLE    0x00
#define BSP_LCR_EPS         0x30
#define     BSP_PARITY_ODD        0x00
#define     BSP_PARITY_EVEN       0x10
#define     BSP_PARITY_MARK       0x20
#define     BSP_PARITY_SPACE      0x30
#define BSP_LCR_BRK         0x40
#define BSP_LCR_DLAB        0x80
#define     BSP_DLAB              0x80

/*LSR*/
#define BSP_LSR_DR          0x01
#define     BSP_RxCHAR_AVAIL      0x01
#define BSP_LSR_OE          0x02
#define BSP_LSR_PE          0x04
#define BSP_LSR_FE          0x08
#define BSP_LSR_BI          0x10
#define BSP_LSR_THRE        0x20
#define     BSP_TxCHAR_AVAIL      0x00
#define     BSP_TxCHAR_EMPTY      0x20
#define BSP_LSR_TEMT        0x40
#define BSP_LSR_RFE         0x80

unsigned char todFrame[TOD_MAX_LEN]={
    0x43,0x4D,0x01,0x20,0x00,0x10,
    0x00,0x00,0x00,0x00,//GPS seconds in week
    0x00,0x00,0x00, 0x00,//reserved
    0x00,0x00,//GPS week 
    0x0F,//Leap Seconds
    0x00,//Second Pulse status
    0xFF,//TACC
    0x00,0x00,0x00,//reserved
    0xff //crc
};

static uint64 aidGpsWeekUtcSec=0;
static uint16 aidGpsWeek=0;

unsigned char crc8_table[256]={
    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41, 
    0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc, 
    0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62, 
    0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff, 
    0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07, 
    0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a, 
    0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24, 
    0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9, 
    0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd, 
    0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50, 
    0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee, 
    0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73, 
    0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b, 
    0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16, 
    0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8, 
    0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35 
};

#define CRC_INT 0xFF
#define CRC_POLY 0x8C
#define CRC_LEN 20

#define RTK_API_ERR_CHK(op, ret)\
    do {\
        if ((ret = (op)) != RT_ERR_OK)\
        {\
            printk("[%s:%d] Rtk API Error! ret=%d \n", __FUNCTION__, __LINE__,ret); \
        }\
    } while(0)

struct proc_dir_entry* ppstod_proc_dir = NULL;
struct proc_dir_entry* ppstod_reverse_entry = NULL;
struct proc_dir_entry* ppstod_output_entry = NULL;
struct proc_dir_entry* ppstod_baudrate_entry = NULL;
struct proc_dir_entry* ppstod_print_entry = NULL;
struct proc_dir_entry* ppstod_enable_entry = NULL;
struct proc_dir_entry* ppstod_isr_entry = NULL; 
 
static rtk_enable_t ppstod_reverse=DISABLED;
static uint8 ppstod_output=0;
static uint32 ppstod_baudrate=9600;
static uint8 baudrate_value1=0x05;
static uint8 baudrate_value2=0x04;
static rtk_enable_t ppstod_print=DISABLED;
static rtk_enable_t ppstod_enable=DISABLED;
static uint8 ppstod_isr=0;

static uint64 maxDiffSec=0;
static uint32 maxSnsec=0;
static uint32 maxEnsec=0;

static uint32 preGpsInWeekSec=0;

void ppstod_sw_interrupt(void)
{
    uint32 gpsInWeekSec=0;
    uint32 tmpU32=0;
    uint16 tmpU16=0,uartFcnt=0;
    int32 ret = RT_ERR_OK;
    int i;
    unsigned char crc;
    static unsigned char modifyWeek=0,incOneSec=0;
    rtk_time_timeStamp_t timeStamp,endTimeStamp;

    if(ppstod_enable==DISABLED)
        return ;
    
    RTK_API_ERR_CHK(rtk_time_curTime_latch(),ret);
    RTK_API_ERR_CHK(rtk_time_curTime_get(&timeStamp),ret);

    if(timeStamp.sec<GPS_UTC_DIFF_SEC)
    {
        if(ppstod_print>=1)
        {
            printk("system time %llu nsec=%09u is smaller than different seconds %u of GPS and UTC\n",timeStamp.sec,timeStamp.nsec,GPS_UTC_DIFF_SEC);
        }
        return;
    }
    

    if((aidGpsWeekUtcSec==0)|| timeStamp.sec<=aidGpsWeekUtcSec ||
        abs(timeStamp.sec-aidGpsWeekUtcSec)>=WEEK_SEC)
    {
        aidGpsWeek=div_u64((timeStamp.sec-GPS_UTC_DIFF_SEC),WEEK_SEC);
        aidGpsWeekUtcSec=(uint64)(aidGpsWeek*WEEK_SEC+GPS_UTC_DIFF_SEC);
        modifyWeek=1;
    }

    gpsInWeekSec=timeStamp.sec-aidGpsWeekUtcSec;

    /*check this sec is increasing one sec from previous sec*/
    if((gpsInWeekSec>preGpsInWeekSec)&&((gpsInWeekSec-preGpsInWeekSec)==1))
        incOneSec=1;
    else
        incOneSec=0;
    preGpsInWeekSec=gpsInWeekSec;

    /*fill GPS in week sec*/            
    tmpU32=gpsInWeekSec;
    for(i=0;i<4;i++)
    {
        if((incOneSec==1)&&(tmpU32==0))
            break;
        todFrame[9-i]=tmpU32%256;
        tmpU32=tmpU32/256;
    }
    
    /*fill GPS week*/
    if(modifyWeek==1)
    {
        tmpU16=aidGpsWeek;
        for(i=0;i<2;i++)
        {
            todFrame[15-i]=tmpU16%256;
            tmpU16=tmpU16/256;
        }
        modifyWeek=0;
    }

    /*calculat crc*/
    crc=CRC_INT;
    for(i=0;i<CRC_LEN;i++)
    {
        crc=crc^todFrame[2+i];
        crc=crc8_table[crc];
    }

    todFrame[22]=crc;

    if(ppstod_output==1)
    {
        /*Uart 0*/
        for(i=0;i<=TOD_LEN;i++)
        {
            uartFcnt=0;
            while((MEM8_READ(BSP_UART0_LSR)&BSP_LSR_THRE)==0)
            { 
                uartFcnt++; 
                /* Prevent Hanging */
                if(uartFcnt>30000)
                {
                    /* Reset Tx FIFO */
                    MEM8_WRITE(BSP_UART0_FCR,BSP_TXRST | BSP_CHAR_TRIGGER_14);
                    printk("uart 0 THRE wrong 0x%u!\n",MEM8_READ(BSP_UART0_LSR));
                    return ;
                }
            }

            if(ppstod_reverse==DISABLED)
                MEM8_WRITE(BSP_UART0_THR,todFrame[i]);
            else
                MEM8_WRITE(BSP_UART0_THR,(todFrame[i]^0xff));
        }
    }
    else if (ppstod_output==2)
    {
        /*Uart 1*/
        for(i=0;i<=TOD_LEN;i++)
        {
            uartFcnt=0;
            while((MEM8_READ(BSP_UART1_LSR)&BSP_LSR_THRE)==0) //THRE
            { 
                uartFcnt++; 
                /* Prevent Hanging */
                if(uartFcnt>30000)
                {
                    /* Reset Tx FIFO */
                    MEM8_WRITE(BSP_UART1_FCR,BSP_TXRST | BSP_CHAR_TRIGGER_14);
                    printk("uart 1 THRE wrong 0x%u!\n",MEM8_READ(BSP_UART1_LSR));
                    return ;
                }
            }

        
            if(ppstod_reverse==DISABLED)
                MEM8_WRITE(BSP_UART1_THR,todFrame[i]);
            else
                MEM8_WRITE(BSP_UART1_THR,(todFrame[i]^0xff));
        }
    }
    else
    {
        /*Default ppstod pin*/
        for(i=0;i<=TOD_LEN;i++)
        {
            if(ppstod_reverse==DISABLED)
            {
                RTK_API_ERR_CHK(rtk_ppstod_frameData_set(i,todFrame[i]),ret);
            }
            else
            {
                RTK_API_ERR_CHK(rtk_ppstod_frameData_set(i,todFrame[i]^0xff),ret);
            }
        }
    }
    
    if(ppstod_print>=1)
    {
        if(timeStamp.nsec>maxSnsec)
            maxSnsec=timeStamp.nsec;

        RTK_API_ERR_CHK(rtk_time_curTime_latch(),ret);
        RTK_API_ERR_CHK(rtk_time_curTime_get(&endTimeStamp),ret);

        if(endTimeStamp.nsec>maxEnsec)
            maxEnsec=endTimeStamp.nsec;

        if((endTimeStamp.sec-timeStamp.sec)>maxDiffSec)
            maxDiffSec=(endTimeStamp.sec-timeStamp.sec);
        
        printk("\n**maxDiffSec=%llu maxSnsec=%u(%llu %09u) maxEnsec=%u(%llu %09u)**\n",maxDiffSec,maxSnsec,timeStamp.sec,timeStamp.nsec,maxEnsec,endTimeStamp.sec,endTimeStamp.nsec);
    }
    
    if(ppstod_print>=2)
    {
        printk("gpsWeek=%u gpsInWeekSec=%u crc=0x%x\n",aidGpsWeek,gpsInWeekSec,crc);
        printk("ToD Frame=");
        for(i=0;i<=TOD_LEN;i++)
            printk(" 0x%02x",todFrame[i]);
        printk("\n");
    }
}

static void ppstod_set_baudrate(void)
{
    int32 ret = RT_ERR_OK;
    switch(ppstod_output)
    {
        case 0:
            RTK_API_ERR_CHK(rtk_ppstod_baudrate_set(ppstod_baudrate),ret);
            break;
        case 1:
            /* Enable Divisor Latch */
            MEM8_WRITE(BSP_UART0_LCR,BSP_CHAR_LEN_8 | BSP_ONE_STOP | BSP_PARITY_DISABLE | BSP_DLAB);
            /* Set Divisor */
            MEM8_WRITE(BSP_UART0_DLM,baudrate_value1);
            MEM8_WRITE(BSP_UART0_DLL,baudrate_value2);
            /* Disable Divisor Latch */
            MEM8_WRITE(BSP_UART0_LCR,BSP_CHAR_LEN_8 | BSP_ONE_STOP | BSP_PARITY_DISABLE);
            break;
        case 2:
            /* Enable Divisor Latch */
            MEM8_WRITE(BSP_UART1_LCR,BSP_CHAR_LEN_8 | BSP_ONE_STOP | BSP_PARITY_DISABLE | BSP_DLAB);
            /* Set Divisor */
            MEM8_WRITE(BSP_UART1_DLM,baudrate_value1);
            MEM8_WRITE(BSP_UART1_DLL,baudrate_value2);
            /* Disable Divisor Latch */
            MEM8_WRITE(BSP_UART1_LCR,BSP_CHAR_LEN_8 | BSP_ONE_STOP | BSP_PARITY_DISABLE);
            break;
        default:
            printk("ppstod output to %u is not support\n", ppstod_output);
    }
}

static void ppstod_setting(void)
{
    int32 ret = RT_ERR_OK;

    RTK_API_ERR_CHK(rtk_ppstod_init(),ret);
    RTK_API_ERR_CHK(rtk_ppstod_mode_set(PPSTOD_MANUAL_MODE_HW),ret);
    RTK_API_ERR_CHK(rtk_intr_imr_set(INTR_TYPE_PTP,DISABLED),ret);
    RTK_API_ERR_CHK(rtk_intr_isr_set(INTR_TYPE_PTP,DISABLED),ret);
    RTK_API_ERR_CHK(rtk_intr_imr_set(INTR_TYPE_TOD,DISABLED),ret);
    RTK_API_ERR_CHK(rtk_intr_isr_set(INTR_TYPE_TOD,DISABLED),ret);

    if(ppstod_enable==ENABLED)
    {
        maxDiffSec=0;
        maxSnsec=0;
        maxEnsec=0;
        
        RTK_API_ERR_CHK(rtk_ppstod_init(),ret);
        RTK_API_ERR_CHK(rtk_ppstod_mode_set(PPSTOD_MANUAL_MODE_SW),ret);
        if(ppstod_isr==0)
        {
            RTK_API_ERR_CHK(rtk_intr_isr_set(INTR_TYPE_PTP,ENABLED),ret);
            RTK_API_ERR_CHK(rtk_intr_imr_set(INTR_TYPE_PTP,ENABLED),ret);
        }
        else
        {
            RTK_API_ERR_CHK(rtk_intr_isr_set(INTR_TYPE_TOD,ENABLED),ret);
            RTK_API_ERR_CHK(rtk_intr_imr_set(INTR_TYPE_TOD,ENABLED),ret);
        }
    }
}

static int ppstod_reverse_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "ppstod reverse status: %u\n", ppstod_reverse);

    return 0;
}

static int ppstod_reverse_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ppstod_reverse_read, inode->i_private);
}

static int ppstod_reverse_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        ppstod_reverse = simple_strtoul(tmpBuf, NULL, 16);
        printk("\nwrite ppstod reverse to %u\n", ppstod_reverse);
        return count;
    }
    return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops ppstod_reverse_fop = {
    .proc_open      = ppstod_reverse_open_proc,
    .proc_write     = ppstod_reverse_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations ppstod_reverse_fop = {
    .owner          = THIS_MODULE,
    .open           = ppstod_reverse_open_proc,
    .write          = ppstod_reverse_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

static int ppstod_output_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "ppstod output status: %u\n", ppstod_output);
    seq_printf(seq, "0: 1PPSToD pin\n");
    seq_printf(seq, "1: Uart 0\n");
    seq_printf(seq, "2: Uart 1\n");
    return 0;
}

static int ppstod_output_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ppstod_output_read, inode->i_private);
}

static int ppstod_output_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;

    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        switch(simple_strtoul(tmpBuf, NULL, 16))
        {
            case 0:
            case 1:
            case 2:
                break;
            default:
                printk("ppstod output to %lu is not support\n", simple_strtoul(tmpBuf, NULL, 16));
                return -EFAULT;
        }
        ppstod_output = simple_strtoul(tmpBuf, NULL, 16);
        ppstod_set_baudrate();
        return count;
    }
    return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops ppstod_output_fop = {
    .proc_open      = ppstod_output_open_proc,
    .proc_write     = ppstod_output_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations ppstod_output_fop = {
    .owner          = THIS_MODULE,
    .open           = ppstod_output_open_proc,
    .write          = ppstod_output_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

static int ppstod_baudrate_read(struct seq_file *seq, void *v)
{

    seq_printf(seq, "ppstod baudrate status: %u\n", ppstod_baudrate);

    return 0;
}

static int ppstod_baudrate_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ppstod_baudrate_read, inode->i_private);
}

static int ppstod_baudrate_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;

    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        printk("\nwrite ppstod baudrate to %u\n", ppstod_baudrate);

        switch(simple_strtoul(tmpBuf, NULL, 10))
        {
            case 115200:
                baudrate_value1=0x00;
                baudrate_value2=0x6b;
                break;
            case 19200:
                baudrate_value1=0x02;
                baudrate_value2=0x82;
                break;
            case 9600:
                baudrate_value1=0x05;
                baudrate_value2=0x04;
                break;
            case 2400:
                baudrate_value1=0x14;
                baudrate_value2=0x10;
                break;
            default:
                printk("ppstod baudrate to %lu is not support\n", simple_strtoul(tmpBuf, NULL, 10));
                return -EFAULT;
        }
        ppstod_baudrate = simple_strtoul(tmpBuf, NULL, 10);
        ppstod_set_baudrate();
        return count;
    }
    return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops ppstod_baudrate_fop = {
    .proc_open      = ppstod_baudrate_open_proc,
    .proc_write     = ppstod_baudrate_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations ppstod_baudrate_fop = {
    .owner          = THIS_MODULE,
    .open           = ppstod_baudrate_open_proc,
    .write          = ppstod_baudrate_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

static int ppstod_print_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "ppstod print status: %u\n", ppstod_print);
    seq_printf(seq, "1: print maxDiffSec maxSnsec(cur S time) maxEnsec(cur E time)\n");
    seq_printf(seq, "2: print 1 and tod data\n");
    return 0;
}

static int ppstod_print_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ppstod_print_read, inode->i_private);
}

static int ppstod_print_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        ppstod_print = simple_strtoul(tmpBuf, NULL, 16);
        printk("\nwrite ppstod print to %u\n", ppstod_print);
        return count;
    }
    return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops ppstod_print_fop = {
    .proc_open      = ppstod_print_open_proc,
    .proc_write     = ppstod_print_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations ppstod_print_fop = {
    .owner          = THIS_MODULE,
    .open           = ppstod_print_open_proc,
    .write          = ppstod_print_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif


static int ppstod_enable_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "ppstod enable status: %u\n", ppstod_enable);

    return 0;
}

static int ppstod_enable_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ppstod_enable_read, inode->i_private);
}

static int ppstod_enable_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;

    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        switch(simple_strtoul(tmpBuf, NULL, 16))
        {
            case DISABLED:
            case ENABLED:
                break;
            default:
                printk("ppstod enable to %lu is not support\n", simple_strtoul(tmpBuf, NULL, 16));
                return -EFAULT;
        }
        ppstod_enable = simple_strtoul(tmpBuf, NULL, 16);
        printk("\nwrite ppstod enable to %u\n", ppstod_enable);

        ppstod_setting();
        return count;
    }
    return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops ppstod_enable_fop = {
    .proc_open      = ppstod_enable_open_proc,
    .proc_write     = ppstod_enable_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations ppstod_enable_fop = {
    .owner          = THIS_MODULE,
    .open           = ppstod_enable_open_proc,
    .write          = ppstod_enable_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

static int ppstod_isr_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "ppstod isr status: %u (0:ptp 1:tod)\n", ppstod_isr);

    return 0;
}

static int ppstod_isr_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ppstod_isr_read, inode->i_private);
}

static int ppstod_isr_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;

    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        switch(simple_strtoul(tmpBuf, NULL, 16))
        {
            case DISABLED:
            case ENABLED:
                break;
            default:
                printk("ppstod isr to %lu is not support\n", simple_strtoul(tmpBuf, NULL, 16));
                return -EFAULT;
        }
        ppstod_isr = simple_strtoul(tmpBuf, NULL, 16);
        printk("\nwrite ppstod isr to %u\n", ppstod_isr);

        ppstod_setting();
        return count;
    }
    return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops ppstod_isr_fop = {
    .proc_open      = ppstod_isr_open_proc,
    .proc_write     = ppstod_isr_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations ppstod_isr_fop = {
    .owner          = THIS_MODULE,
    .open           = ppstod_isr_open_proc,
    .write          = ppstod_isr_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

int __init ppstod_drv_init(void)
{
    if (NULL == ppstod_proc_dir) {
        ppstod_proc_dir = proc_mkdir ("ppstod_drv", NULL);
    }

    if(ppstod_proc_dir)
    {
        ppstod_reverse_entry = proc_create("reverse", 0644, ppstod_proc_dir, &ppstod_reverse_fop);
        if (!ppstod_reverse_entry) {
            printk("ppstod_reverse_entry, create proc failed!");
        }
    }

    if(ppstod_proc_dir)
    {
        ppstod_output_entry = proc_create("output", 0644, ppstod_proc_dir, &ppstod_output_fop);
        if (!ppstod_output_entry) {
            printk("ppstod_output_entry, create proc failed!");
        }
    }

    if(ppstod_proc_dir)
    {
        ppstod_baudrate_entry = proc_create("baudrate", 0644, ppstod_proc_dir, &ppstod_baudrate_fop);
        if (!ppstod_baudrate_entry) {
            printk("ppstod_baudrate_entry, create proc failed!");
        }
    }

    if(ppstod_proc_dir)
    {
        ppstod_print_entry = proc_create("print", 0644, ppstod_proc_dir, &ppstod_print_fop);
        if (!ppstod_print_entry) {
            printk("ppstod_print_entry, create proc failed!");
        }
    }

    if(ppstod_proc_dir)
    {
        ppstod_enable_entry = proc_create("enable", 0644, ppstod_proc_dir, &ppstod_enable_fop);
        if (!ppstod_enable_entry) {
            printk("ppstod_enable_entry, create proc failed!");
        }
    }

    if(ppstod_proc_dir)
    {
        ppstod_isr_entry = proc_create("isr", 0644, ppstod_proc_dir, &ppstod_isr_fop);
        if (!ppstod_isr_entry) {
            printk("ppstod_isr_entry, create proc failed!");
        }
    }

    return 0;
}

void __exit ppstod_drv_exit(void)
{
    int32 ret = RT_ERR_OK;

    if(ppstod_enable==ENABLED)
    {
        RTK_API_ERR_CHK(rtk_intr_imr_set(INTR_TYPE_PTP,DISABLED),ret);
        RTK_API_ERR_CHK(rtk_intr_isr_set(INTR_TYPE_PTP,DISABLED),ret);
    }

    if(ppstod_reverse_entry)
    {
        remove_proc_entry("reverse", ppstod_proc_dir);
        ppstod_reverse_entry = NULL;
    }
    
    if(ppstod_output_entry)
    {
        remove_proc_entry("output", ppstod_proc_dir);
        ppstod_output_entry = NULL;
    }
    
    if(ppstod_print_entry)
    {
        remove_proc_entry("print", ppstod_proc_dir);
        ppstod_print_entry = NULL;
    }
    
    if(ppstod_enable_entry)
    {
        remove_proc_entry("enable", ppstod_proc_dir);
        ppstod_enable_entry = NULL;
    }

    if(ppstod_isr_entry)
    {
        remove_proc_entry("isr", ppstod_proc_dir);
        ppstod_isr_entry = NULL;
    }
    
    if(ppstod_proc_dir)
    {
        remove_proc_entry("ppstod_drv", NULL);
        ppstod_proc_dir = NULL;
    }
}


EXPORT_SYMBOL(ppstod_sw_interrupt);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek PPSTOD drive module");
MODULE_AUTHOR("Realtek");
module_init(ppstod_drv_init);
module_exit(ppstod_drv_exit);


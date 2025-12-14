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
 *
 *
 * $Revision$
 * $Date$
 *
 * Purpose : IRQ API
 *
 * Feature : Provide the APIs to register/deregister IRQ
 *
 */


/*
 * Include Files
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/version.h>
#if defined(CONFIG_KERNEL_4_14_x)
#include <linux/irqreturn.h>
#include <linux/interrupt.h>
#endif

#include <bspchip.h>
#if defined(OLD_FPGA_DEFINED)
#include <gpio.h>
#else
#if !defined(CONFIG_RTL9607C_SERIES) && !defined(CONFIG_LUNA_G3_SERIES) && !defined(CONFIG_RTL9603CVD_SERIES)
#include <bspchip_8686.h>
#endif
#endif

#include <common/rt_type.h>
#include <common/rt_error.h>
#include <osal/print.h>
#include <osal/spl.h>
//#include <dal/apollo/dal_apollo.h>
#include <dal/apollomp/dal_apollomp.h>
#include <rtk/irq.h>
#include <rtk/gponv2.h>
#include <rtk/gpio.h>

//#include <dal/apollo/gpon/gpon_platform.h>
#include <rtk/intr.h>
#include <rtk/switch.h>
#include <rtk/ponmac.h>

/*
 * Symbol Definition
 */

#if defined(OLD_FPGA_DEFINED)
#define APOLLO_IRQ 16 /* TBD */
#elif defined(FPGA_DEFINED)
#define APOLLO_IRQ BSP_GPIO1_IRQ
#elif defined(CONFIG_RTL9607C_SERIES)||defined(CONFIG_RTL9603CVD_SERIES)
#define APOLLO_IRQ BSP_SWCORE_IRQ
#elif defined(CONFIG_LUNA_G3_SERIES)
#define APOLLO_IRQ 0
#else
#define APOLLO_IRQ BSP_SWITCH_IRQ
#endif

#if defined(CONFIG_LUNA_G3_SERIES)
    //Kem need to check
    #define GPIO_SET1_IRQ 0
    #define GPIO_SET2_IRQ 0
    #define IRQF_DISABLED 0
#else
    #if LINUX_VERSION_CODE >= KERNEL_VERSION(5,1,0)
        #define IRQF_DISABLED 0
    #endif
    
    #define GPIO_SET1_IRQ BSP_GPIO_ABCD_IRQ
    #define GPIO_SET2_IRQ BSP_GPIO_EFGH_IRQ
#endif

/*
 * Data Declaration
 */
static struct net_device switch_dev,gpio_dev,gpio2_dev;
static struct tasklet_struct switch_tasklets,gpio_tasklets,gpio2_tasklets;
static int32 irq_init = {INIT_NOT_COMPLETED};

const int32 gpioIRQId[2]={GPIO_SET1_IRQ,GPIO_SET2_IRQ};

#define GPIO_IMR_NUM 64

static rtk_irq_data_t irq_isr[INTR_TYPE_END-1];

static rtk_irq_data_t gpio_isr[GPIO_IMR_NUM];

static void (*dyinggasp_th_isr)(void);

extern osal_spinlock_t imrLock;

static uint32 gpon_dg_times=3;

/*
 * Macro Definition
 */
#ifndef REG32
#define REG32(reg)      (*((volatile unsigned int *)(reg)))
#endif


/*
 * Function Declaration
 */


static uint32
irq_clear(int imrId){

    uint32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON),"\nInterrupt: %d, no isr register or imr is not enabled!\n",imrId);
    if((ret=rtk_intr_ims_clear(imrId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}




static void
switch_interrupt_bh(uint32 data)
{
    rtk_enable_t status;
    int32 i,ret;
    uint32 curVal;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON),"switch_interrupt_bh 0x%x", data);

    /*loop for check all of interrupt type & isr, explict INTR_TYPE_ALL*/
    for(i=0; i < (INTR_TYPE_END - 1) ; i++)
    {
        /*get status of interrupt type*/
        if((ret=rtk_intr_ims_get(i,&status)) != RT_ERR_OK)
        {
            if(ret!=RT_ERR_INPUT)
            {
                RT_ERR(ret, (MOD_INTR | MOD_DAL), "type=%d",i);
            }
            continue;
        }
        /*if status enabled and isr is registered*/
        if(status && (irq_isr[i].isr != NULL))
        {
            irq_isr[i].isr();
        }else
        if(status){
            irq_clear(i);
        }
    }

    /* restore all of interrupt */
#if !defined(FPGA_DEFINED)
    /* spin lock */
    osal_spl_spin_lock(&imrLock);
#endif
    /* get the current imr for retrive the setting by other app(initial function or CLI) */
    if((ret=rtk_intr_imr_get(INTR_TYPE_ALL,&curVal)) == RT_ERR_OK)
    {
        data |= curVal;
    }
    if((ret=rtk_intr_imr_restore(data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
    }
#if !defined(FPGA_DEFINED)
    /* spin unlock */
    osal_spl_spin_unlock(&imrLock);
#endif
    return;
}



static irqreturn_t
switch_interrupt_th(int irq, void *dev_instance)
{
    uint32 data;
    int32  ret ;
    rtk_enable_t status;
    uint32              cnt=0;
    uint32              state;
#if defined(CONFIG_GPON_FEATURE)
    rtk_gpon_onuState_t onu_state;
#endif

#if defined(OLD_FPGA_DEFINED)
    /* disable interrupt */
    data = 0x3 << (GPIO_B_2*2);
    REG32(GPIO_PAB_IMR) = REG32(GPIO_PAB_IMR)&(~data);

    REG32(0xB8003510) = 0x00000400;      /*  reg.PABCD_ISR : write 1 to clear PORT B pin 2 */

    tasklet_hi_schedule(&switch_tasklets);

    /* enable interrupt */
    data = 0x1 << (GPIO_B_2*2);
    REG32(GPIO_PAB_IMR) = REG32(GPIO_PAB_IMR)|(data);

#else

    /*get current interrupt register*/
    if((ret=rtk_intr_imr_get(INTR_TYPE_ALL,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return IRQ_RETVAL(IRQ_HANDLED);
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON),"switch_interrupt_th 0x%x", data);

    switch_tasklets.data = data;

    /* disable all of interrupt */
    if((ret=rtk_intr_imr_set(INTR_TYPE_ALL,DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return IRQ_RETVAL(IRQ_HANDLED);

    }

    /*get status of dying gasp*/
    if((ret=rtk_intr_ims_get(INTR_TYPE_DYING_GASP,&status)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return IRQ_RETVAL(IRQ_HANDLED);
    }
    /*if status enabled and dyinggasp_th_handler registered*/
    if(status)
    {
        if(dyinggasp_th_isr != NULL)
        {
            dyinggasp_th_isr();
        }
#if defined(CONFIG_GPON_FEATURE)
        if(rtk_gpon_onuState_get(&onu_state) == RT_ERR_OK)
        {
            if(onu_state == GPON_STATE_O5)
            {
                if(rtk_gpon_autoDG_get(&state) == RT_ERR_OK)
                {
                    if(state == 1)
                    {
                        cnt = gpon_dg_times;
                        do
                        {
                            if (rtk_gpon_DgCnt_get(&cnt) == RT_ERR_OK)
                            {
                                if(cnt >= gpon_dg_times)
                                {
                                    if (RT_ERR_OK != (ret = rtk_gpon_autoDG_set(0)))
                                    {
                                        RT_ERR(ret, (MOD_INTR), "");
                                    }
                                    state = 0;
                                    osal_printf("DG_CFG=0x%x\n", REG32(0xbb705014));
                                    osal_printf("DG=0x%x\n", REG32(0xbb705184));
                                }
                            }
                            else
                            {
                                RT_ERR(ret, (MOD_INTR), "");
                            }
                        } while (cnt < gpon_dg_times);
                    }

                    if(state == 0)
                    {
                        if (RT_ERR_OK != (ret = rtk_switch_chip_reset()))
                        {
                            RT_ERR(ret, (MOD_INTR), "");
                        }
                    }
                }
                else
                {
                    RT_ERR(ret, (MOD_INTR), "");
                }
            }
        }
#endif    
    }

    tasklet_hi_schedule(&switch_tasklets);
#endif

    return IRQ_RETVAL(IRQ_HANDLED);
}


static int32
rtk_switch_irq_init(uint32 irq_id)
{
#if defined(OLD_FPGA_DEFINED)
    uint32 data;
#endif
    int32  ret;

    /*find irq_id is user define or from header file*/
    if(irq_id == IRQ_ID_NO_CARE){
        irq_id = APOLLO_IRQ;
    }
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON),"%s: irq_id = %d\n",__FUNCTION__,irq_id);

    memset(&switch_tasklets, 0, sizeof(struct tasklet_struct));
    switch_tasklets.func=(void (*)(unsigned long))switch_interrupt_bh;
    switch_tasklets.data=(unsigned long)NULL;

    /* switch interrupt clear all mask */
    if((ret=rtk_intr_imr_set(INTR_TYPE_ALL,DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return ret;
    }

    /* clear switch interrupt state */
    if((ret=rtk_intr_ims_clear(INTR_TYPE_ALL)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return ret;
    }

#if defined(OLD_FPGA_DEFINED)
    /* switch interrupt polarity set to low */
    if((ret=rtk_intr_polarity_set(INTR_POLAR_LOW)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return ret;
    }
#else
    /* switch interrupt polarity set to high */
    if((ret=rtk_intr_polarity_set(INTR_POLAR_HIGH)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return ret;
    }
#endif

    /* IRQ register */
    osal_memset(&switch_dev, 0x0, sizeof(switch_dev));
    switch_dev.irq = irq_id;
    osal_strcpy(switch_dev.name,"apl_sw");
    
    if (num_online_cpus() > 1)
    {
        irq_set_affinity(switch_dev.irq, cpumask_of(0));
    }

    printk("%s %d irq=%d name=%s\n", __func__, __LINE__, switch_dev.irq, switch_dev.name);
    if((ret = request_irq(switch_dev.irq, (irq_handler_t)switch_interrupt_th, IRQF_DISABLED, switch_dev.name, &switch_dev)))
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

#if defined(OLD_FPGA_DEFINED)
    /* §Q¥?GPIO_B_2 (I/O) °µinterrupt (¥HRTL8672 4P+WIFI ¬°¨ҡA³o­?IN©Ԩ뱥set button) */
    /*  reg.PABCD_DIR     : set direction configuration of PORT B pin 2 to b'0
                            (b'0:input pin , b'1:output pin)   */
    gpioConfig(GPIO_B_2,GPIO_FUNC_INPUT);
    /*  reg.PAB_IMR       : set interrupt mode of PORT B pin 2 to  b'01
                            (b'00=Disable interrupt
                             b'01=Enable falling edge interrupt
                             b'11=Enable both falling or rising edge interrupt) */
    data = 0x1 << (GPIO_B_2*2);
    REG32(GPIO_PAB_IMR) = REG32(GPIO_PAB_IMR)|(data);
#endif

    /*initial isr*/
    if((ret=rtk_irq_isr_unregister(INTR_TYPE_ALL)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


static int32
rtk_switch_irq_exit(void)
{
    int32 ret;
    /* IRQ de-register */
    free_irq(switch_dev.irq, &switch_dev);
    /*initial isr*/
    if((ret=rtk_irq_isr_unregister(INTR_TYPE_ALL)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


static void
gpio_interrupt_bh(uint32 data)
{
    rtk_enable_t status;
    int32 i;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON),"%s",__FUNCTION__);

    /*loop for check all of interrupt type & isr*/
    for(i=0; i < 32 ; i++)
    {

        status = (data >> i) & 0x1;

        /*if status enabled and isr is registered*/
        if(status && (gpio_isr[i].isr != NULL))
        {
            gpio_isr[i].isr();
        }
    }

    return;
}

static void
gpio_interrupt2_bh(uint32 data)
{
    rtk_enable_t status;
    int32 i;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON),"%s",__FUNCTION__);

    /*loop for check all of interrupt type & isr*/
    for(i=0; i < 32 ; i++)
    {
        status = (data >> i) & 0x1;

        /*if status enabled and isr is registered*/
        if(status && (gpio_isr[i+32].isr != NULL))
        {
            gpio_isr[i+32].isr();
        }
    }
    return;
}





static irqreturn_t
gpio_interrupt_th(int irq, void *dev_instance)
{
    int32  ret,gpioId;
    uint32 value;
    struct tasklet_struct *pTasklet;
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON),"%s",__FUNCTION__);


    gpioId   = (irq==GPIO_SET1_IRQ) ? RTK_GPIO_INTR_SET1 : RTK_GPIO_INTR_SET2;
    pTasklet = (irq==GPIO_SET1_IRQ) ? &gpio_tasklets : &gpio2_tasklets;

    if((ret=rtk_gpio_intrStatus_get(gpioId,&value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return ret;
    }
    pTasklet->data = value;

    if((ret=rtk_gpio_intrStatus_clean(gpioId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return ret;
    }

    tasklet_hi_schedule(pTasklet);

    return IRQ_RETVAL(IRQ_HANDLED);
}



static int32
rtk_gpio_irq_init(void)
{
    int32  ret;

    /*find irq_id is user define or from header file*/
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON),"%s: irq_id = %d and %d\n",__FUNCTION__,GPIO_SET1_IRQ,GPIO_SET2_IRQ);


    /* gpio interrupt default disable all */
    if((ret=rtk_gpio_intr_set(RTK_GPIO_INTR_ALL,GPIO_INTR_DISABLE)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return ret;
    }

    /* clean all gpio interrupt status*/
    if((ret=rtk_gpio_intrStatus_clean(RTK_GPIO_INTR_ALL)) != RT_ERR_OK)
    {
         RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
         return ret;
    }
    /* IRQ register */
    osal_memset(&gpio_tasklets, 0, sizeof(struct tasklet_struct));
    gpio_tasklets.func=(void (*)(unsigned long))gpio_interrupt_bh;
    gpio_tasklets.data=(unsigned long)NULL;

    /* IRQ register */
    osal_memset(&gpio2_tasklets, 0, sizeof(struct tasklet_struct));
    gpio2_tasklets.func=(void (*)(unsigned long))gpio_interrupt2_bh;
    gpio2_tasklets.data=(unsigned long)NULL;

    /*register interruot irq */
    osal_memset(&gpio_dev, 0x0, sizeof(gpio_dev));
    gpio_dev.irq = GPIO_SET1_IRQ;

    osal_strcpy(gpio_dev.name,"apl_gpio");
    
#if !defined(CONFIG_GPIO_RTK_SOC)    
    printk("%s %d irq=%d name=%s\n", __func__, __LINE__, gpio_dev.irq, gpio_dev.name);
    if(request_irq(gpio_dev.irq, (irq_handler_t)gpio_interrupt_th, IRQF_DISABLED, gpio_dev.name, &gpio_dev))
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
#endif
    
    osal_memset(&gpio2_dev,0,sizeof(gpio2_dev));
    gpio2_dev.irq = GPIO_SET2_IRQ;
    osal_strcpy(gpio2_dev.name,"apl_gpio2");

#if !defined(CONFIG_GPIO_RTK_SOC)    
    printk("%s %d irq=%d name=%s\n", __func__, __LINE__, gpio2_dev.irq, gpio2_dev.name);
    if(request_irq(gpio2_dev.irq, (irq_handler_t)gpio_interrupt_th, IRQF_DISABLED, gpio2_dev.name, &gpio2_dev))
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
#endif
    /*initial isr*/
    if((ret=rtk_irq_gpioISR_unregister(RTK_GPIO_INTR_ALL)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

static int32
rtk_gpio_irq_exit(void)
{
    int32 ret,i;
    /* IRQ de-register */
#if !defined(CONFIG_GPIO_RTK_SOC)
    for(i=0;i<2;i++)
    {
        free_irq(gpio_dev.irq, &gpio_dev);
    }
#endif    
    /*initial isr*/
    if((ret=rtk_irq_gpioISR_unregister(GPIO_IMR_NUM)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      rtk_irq_isr_register
 * Description:
 *      Register isr handler
 * Input:
 *      intr            - interrupt type
 *      fun            - function pointer of isr hander
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
rtk_irq_isr_register(rtk_intr_type_t intr, void (*fun)(void))
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(irq_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == fun), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((INTR_TYPE_END <= intr), RT_ERR_OUT_OF_RANGE);

    /* hook function point to isr_mapper*/
    irq_isr[intr].isr = fun;

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_irq_isr_unregister
 * Description:
 *      Register isr handler
 * Input:
 *      intr            - interrupt type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
rtk_irq_isr_unregister(rtk_intr_type_t intr){

    int32   i;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(irq_init);

    /* parameter check */
    RT_PARAM_CHK((INTR_TYPE_END <= intr), RT_ERR_OUT_OF_RANGE);

    /* un-hook function point to isr_mapper*/
    if(INTR_TYPE_ALL == intr)
    {
        for(i = 0; i < INTR_TYPE_ALL; i++)
        {
            irq_isr[i].isr = NULL;
        }
    }
    else
    {
            irq_isr[intr].isr = NULL;
    }

    return RT_ERR_OK;

}



/* Function Name:
 *      rtk_irq_gpioISR_register
 * Description:
 *      Register isr handler
 * Input:
 *      gpioId       - gpio id for interrupt ISR
 *      fun            - function pointer of isr hander
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
rtk_irq_gpioISR_register(uint32 gpioId, void (*fun)(void))
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(irq_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == fun), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((GPIO_IMR_NUM < gpioId), RT_ERR_OUT_OF_RANGE);

    /* hook function point to isr_mapper*/
    gpio_isr[gpioId].isr = fun;

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_irq_gpioISR_unregister
 * Description:
 *      Register isr handler
 * Input:
 *      gpioId   - gpio id for interrupt ISR
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
rtk_irq_gpioISR_unregister(uint32 gpioId){

    int32   i;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(irq_init);

    /* parameter check */
    if((gpioId >= GPIO_IMR_NUM) && (gpioId != RTK_GPIO_INTR_ALL))
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    /* un-hook function point to isr_mapper*/
    if(RTK_GPIO_INTR_ALL == gpioId)
    {
        for(i = 0; i < GPIO_IMR_NUM; i++)
        {
            gpio_isr[i].isr = NULL;
        }
    }
    else
    {
        gpio_isr[gpioId].isr = NULL;
    }

    return RT_ERR_OK;

}

/* Function Name:
 *      rtk_irq_isrDgTh_register
 * Description:
 *      Register Dying gasp top half isr handler
 * Input:
 *      intr            - interrupt type
 *      fun            - function pointer of isr hander
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
rtk_irq_isrDgTh_register(void (*fun)(void))
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(irq_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == fun), RT_ERR_NULL_POINTER);

    /* hook function point to isr_mapper*/
    dyinggasp_th_isr = fun;

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_irq_isrDgTh_unregister
 * Description:
 *      Register  Dying gasp top half isr handler
 * Input:
 *      intr            - interrupt type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
rtk_irq_isrDgTh_unregister(void){


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(irq_init);

    /* un-hook function point to isr_mapper*/
    dyinggasp_th_isr = NULL;

    return RT_ERR_OK;

}


/* Function Name:
 *      rtk_irq_dgTimes_set
 * Description:
 *      Register  Dying gasp top half isr handler
 * Input:
 *      dg_times        - dying_gasp times
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
rtk_irq_dgTimes_set(uint32 dg_times){


    gpon_dg_times = dg_times;

    return RT_ERR_OK;

}


/* Function Name:
 *      rtk_irq_init
 * Description:
 *      Register isr handler
 * Input:
 *      intr            - interrupt type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
rtk_irq_init(void){

    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);
    irq_init  = INIT_COMPLETED;
#if defined(CONFIG_LUNA_G3_SERIES)
    return RT_ERR_OK;
#endif
    if((ret = rtk_switch_irq_init(IRQ_ID_NO_CARE))!=RT_ERR_OK)
    {
        irq_init = INIT_NOT_COMPLETED;
    }
#if !(defined (FPGA_DEFINED))
#if !defined(CONFIG_GPIO_RTK_SOC)   
    if((ret = rtk_gpio_irq_init())!=RT_ERR_OK)
    {
        irq_init = INIT_NOT_COMPLETED;
    }
#endif
#endif
    return ret;
}


/* Function Name:
 *      rtk_irq_exit
 * Description:
 *      Register isr handler
 * Input:
 *      intr            - interrupt type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
rtk_irq_exit(void){

    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);
#if defined(CONFIG_LUNA_G3_SERIES)
    return RT_ERR_OK;
#endif
    if( INIT_COMPLETED == irq_init)
    {
        ret = rtk_switch_irq_exit();
        ret = rtk_gpio_irq_exit();
        irq_init = INIT_NOT_COMPLETED;
    }
    return RT_ERR_OK;
}




/*
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 * *
 * $Revision$
 * $Date: 2012-10-16
 *
 * Purpose : EPON MAC register access APIs
 *
 * Feature : Provide the APIs to access EPON MAC
 *
 */

#include <common/rt_error.h>
#include <common/rt_type.h>
#include <rtk/acl.h>
#include <rtk/gpio.h>
#include <dal/apollomp/dal_apollomp.h>
#include <dal/apollomp/dal_apollomp_ponmac.h>
#include <dal/apollomp/dal_apollomp_epon.h>
#include <dal/apollomp/dal_apollomp_gpio.h>
#include <dal/apollomp/dal_apollomp_acl.h>
#include <dal/apollomp/dal_apollomp_switch.h>
#include <dal/apollomp/dal_apollomp_classify.h>
#include <dal/apollomp/raw/apollomp_raw_epon.h>
#include <dal/apollomp/raw/apollomp_raw_port.h>
#include <dal/apollomp/raw/apollomp_raw_ponmac.h>
#include <ioal/mem32.h>
#include <rtk/ponmac.h>
#include <rtk/oam.h>
#include <rtk/irq.h>
#include <rtk/intr.h>
#include <rtk/switch.h>
#include <rtk/classify.h>
#include <dal/apollomp/dal_apollomp_intr.h>
#include <osal/time.h>


#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/random.h>
#endif


#include "rtk/intr.h"
#include "module/intr_bcaster/intr_bcaster.h"


static uint32 epon_init = {INIT_NOT_COMPLETED};

static rtk_epon_polarity_t oe_polarity = EPON_POLARITY_HIGH;
static rtk_epon_laser_status_t forceLaserState = RTK_EPON_LASER_STATUS_NORMAL;

static uint32 _eponRegIntCnt=0;
static uint32 _eponTimeDriftIntCnt=0;
static uint32 _eponMpcpTimeoutIntCnt=0;

static dal_apollomp_epon_mpcp_check_state_t  _eponMpcpCheckState = RTK_EPON_MPCP_CKECK_INIT;

rtk_enable_t usFecState = DISABLED;
rtk_enable_t dsFecState = DISABLED;

static int32
_dal_apollomp_epon_raw_opticalPolarity_set(rtk_epon_polarity_t polarity);

static uint32 dal_apollomp_epon_isr_init(void);
static uint32 dal_apollomp_epon_isr_entry(void);


static unsigned int mpcp_cnt=0;
#if defined(CONFIG_SDK_KERNEL_LINUX)

static void dal_apollomp_intr_dying_gasp_entry(intrBcasterMsg_t	*pMsgData);

static intrBcasterNotifier_t eponDyingGaspNotifier = {
    .notifyType = INTR_TYPE_DYING_GASP,
    .notifierCb = dal_apollomp_intr_dying_gasp_entry,
};


#endif



#if defined(CONFIG_SDK_KERNEL_LINUX)
static void dal_apollomp_intr_dying_gasp_entry(intrBcasterMsg_t	*pMsgData)
{
    int32  ret;

    switch(DAL_UTIL_CHIP_REV)
    {
        case CHIP_REV_ID_A:
        case CHIP_REV_ID_B:
        case CHIP_REV_ID_C:
        case CHIP_REV_ID_D:
        case CHIP_REV_ID_E:
    	{
            if((ret = dal_apollomp_epon_reportMode_set(RTK_EPON_REPORT_FORCE_F)) != RT_ERR_OK)
            {
            	RT_ERR(ret,(MOD_EPON | MOD_DAL), "");
        		return;
            }
            break;
        }
        case CHIP_REV_ID_F:
        default:
            break;
    }
    printk("\n dying_gasp_entry!!!\n");

    return;
}
#endif

uint32 dal_apollomp_epon_isr_entry(void)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)

    int32  ret;
    uint32 tmpVal,data;


    /*LLID register success interrupt check*/
    if ((ret = reg_field_read(APOLLOMP_EPON_INTRr,APOLLOMP_REG_LLID_TX_IMSf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if(0==tmpVal)
    {
        /*epon register syuuess handle*/
        _eponRegIntCnt++;
        /*disable register success interrupt*/
        data = 0;
        if((ret = reg_field_write(APOLLOMP_EPON_INTRr, APOLLOMP_REG_LLID_TX_IMRf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }
        /*clear interrupt status*/
        data = 1;
        if((ret = reg_field_write(APOLLOMP_EPON_INTRr, APOLLOMP_REG_LLID_TX_IMSf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }
      #if 1 /*for type 1/type 2 patch*/


        switch(DAL_UTIL_CHIP_REV)
        {
            case CHIP_REV_ID_A:
            case CHIP_REV_ID_B:
            case CHIP_REV_ID_C:
            case CHIP_REV_ID_D:
            case CHIP_REV_ID_E:
            case CHIP_REV_ID_F:
                /*regiaster success set laser on/off shift time to save value for type1 issue*/
                data = 3;
                if ((ret = reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_ON_SHIFTf,&data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                    return ret;
                }
                data = 10;
                if ((ret = reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_OFF_SHIFTf,&data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                    return ret;
                }  
                break;
        default:
        #if 0 /*new versoon chip just using default setting*/
                /*regiaster success set laser on/off shift time to default value*/
                data = 3;
                if ((ret = reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_ON_SHIFTf,&data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                    return ret;
                }
                data = 3;
                if ((ret = reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_OFF_SHIFTf,&data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                    return ret;
                }  
        #endif
                /*enable stop sync local time*/ 
                if(DAL_UTIL_CHIP_REV>CHIP_REV_ID_F)
                {
                    if ((ret = reg_read(APOLLOMP_RSVD_EPON_CTRL3r, &data)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                        return ret;
                    }
                    /*set bit 27*/
                    data = data | 0x8000000;           
                    if ((ret = reg_write(APOLLOMP_RSVD_EPON_CTRL3r, &data)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                        return ret;
                    }
                } 
                break;
         }       
     #endif   
              
              
    }

#if 0
    /*Time drift interrupt check*/
    if ((ret = reg_field_read(APOLLOMP_EPON_INTRr,APOLLOMP_TIME_DRIFT_IMSf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if(0==tmpVal)
    {
        /*epon time drift handle*/
        _eponTimeDriftIntCnt++;
        /*disable time drift interrupt*/
        data = 0;
        if((ret = reg_field_write(APOLLOMP_EPON_INTRr, APOLLOMP_TIME_DRIFT_IMRf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }

        data = 1;
        if((ret = reg_field_write(APOLLOMP_EPON_INTRr, APOLLOMP_TIME_DRIFT_IMSf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }

    }
#endif

    /*mpcp timeout interrupt check*/
    if ((ret = reg_field_read(APOLLOMP_EPON_INTRr,APOLLOMP_MPCP_TIMEOUT_IMSf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if(0==tmpVal)
    {
        /*epon mpcp timeout handle*/
        _eponMpcpTimeoutIntCnt++;

        /*disable mpcp timeoutinterrupt*/
        data = 0;
        if((ret = reg_field_write(APOLLOMP_EPON_INTRr, APOLLOMP_MPCP_TIMEOUT_IMRf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }

        /*clear interrupt status*/
        data = 1;
        if((ret = reg_field_write(APOLLOMP_EPON_INTRr, APOLLOMP_MPCP_TIMEOUT_IMSf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }

    }


    /* switch interrupt clear EPON state */
    if((ret=dal_apollomp_intr_ims_clear(INTR_TYPE_EPON)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
	}



#endif
    return RT_ERR_OK;
}

uint32 dal_apollomp_epon_isr_init(void)
{
    int32  ret;
    uint32 data;

    /*diable all EPON interrupt mask*/
    /*disable register success interrupt*/
    data = 0;
    if((ret = reg_field_write(APOLLOMP_EPON_INTRr, APOLLOMP_REG_LLID_TX_IMRf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    /*disable time drift interrupt*/
    data = 0;
    if((ret = reg_field_write(APOLLOMP_EPON_INTRr, APOLLOMP_TIME_DRIFT_IMRf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    /*disable mpcp timeout interrupt*/
    data = 0;
    if((ret = reg_field_write(APOLLOMP_EPON_INTRr, APOLLOMP_MPCP_TIMEOUT_IMRf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }


#if defined(CONFIG_SDK_KERNEL_LINUX)
	/*register EPON isr*/
	if((ret = rtk_irq_isr_register(INTR_TYPE_EPON,dal_apollomp_epon_isr_entry)) != RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_EPON | MOD_DAL), "");
		return ret;
	}

    if((ret = dal_apollomp_intr_imr_set(INTR_TYPE_EPON,ENABLED)) != RT_ERR_OK)
    {
    	RT_ERR(ret,(MOD_EPON | MOD_DAL), "");
		return ret;
    }

    switch(DAL_UTIL_CHIP_REV)
    {
        case CHIP_REV_ID_A:
        case CHIP_REV_ID_B:
        case CHIP_REV_ID_C:
        case CHIP_REV_ID_D:
        case CHIP_REV_ID_E:
        case CHIP_REV_ID_F:
    	{
        	if((ret = intr_bcaster_notifier_cb_register(&eponDyingGaspNotifier)) != RT_ERR_OK)
        	{
        		RT_ERR(ret,(MOD_EPON | MOD_DAL), "");
        		return ret;
        	}
            break;
    	}
    	default:
    	    break;
	}


#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_apollomp_epon_init
 * Description:
 *      epon register level initial function
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_init(void)
{
    uint32 data;
    int32  ret;
    apollomp_raw_port_ability_t mac_ability;
    rtk_epon_llid_entry_t   llidEntry;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");


    /*pon mac init*/
    if ((ret = rtk_ponmac_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    /*mapping queue 0~7 to schedule id 0*/
    {
        rtk_ponmac_queueCfg_t   queueCfg;
        rtk_ponmac_queue_t logicalQueue;
        uint32 queueId;

        memset(&queueCfg, 0, sizeof(rtk_ponmac_queueCfg_t));

        logicalQueue.schedulerId = 0 ;

        queueCfg.cir       = 0x0;
        queueCfg.pir       = HAL_PONMAC_PIR_CIR_RATE_MAX();
        queueCfg.type      = STRICT_PRIORITY;
        queueCfg.egrssDrop = DISABLED;
        for(queueId = 0 ; queueId <= RTK_DOT1P_PRIORITY_MAX ; queueId++ )
        {
            logicalQueue.queueId = queueId;

            if((ret= dal_apollomp_ponmac_queue_add(&logicalQueue, &queueCfg)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
        }
    }
    if ((ret = dal_apollomp_ponmac_mode_set(PONMAC_MODE_EPON)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }



    if ((ret = dal_apollomp_epon_fecOverhead_set(0x8)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    if ((ret = dal_apollomp_epon_churningKeyMode_set(RTK_EPON_CHURNING_BL)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    switch(DAL_UTIL_CHIP_REV)
    {
        case CHIP_REV_ID_A:
        case CHIP_REV_ID_B:
        case CHIP_REV_ID_C:
        case CHIP_REV_ID_D:
        case CHIP_REV_ID_E:
            if ((ret = dal_apollomp_epon_reportMode_set(RTK_EPON_REPORT_0_F)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }

            break;
        case CHIP_REV_ID_F:
        default:
            if ((ret = dal_apollomp_epon_reportMode_set(RTK_EPON_REPORT_NORMAL)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            /*for dying gasp report mode*/
            if ((ret = reg_read(APOLLOMP_PON_EGR_RATE_CTRLr, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            /*set bit 5 to 1 to enable dying gasp report option*/
            data = data | 0x20;            
            if ((ret = reg_write(APOLLOMP_PON_EGR_RATE_CTRLr, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            /*note: bit 4 is REV_ID_F report mode option, default set to 1*/
            
            if(DAL_UTIL_CHIP_REV > CHIP_REV_ID_F)
            {
                /*enable CHIP_REV_ID_G epon enhance option*/
                /*RSVD_EPON_CTRL3=0x0420_2850   reg 0x3624C*/
                ioal_mem32_write(0x3624C, 0x04202850);
                    
            }
            
            break;
    }

    /*report include ifg*/
    data = 1;
    if ((ret = reg_field_write(APOLLOMP_PON_PIR_CIR_IFGr,APOLLOMP_DBA_IFGf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    /*init EPON register*/

    /*reset EPON*/
    data = 0;
    if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_00r,APOLLOMP_CFG_SFT_RSTB_EPONf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    /*set laser parameter*/
    data = 34;
    if ((ret = reg_field_write(APOLLOMP_LASER_ON_OFF_TIMEr,APOLLOMP_LASER_ON_TIMEf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    data = 30;
    if ((ret = reg_field_write(APOLLOMP_LASER_ON_OFF_TIMEr,APOLLOMP_LASER_OFF_TIMEf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    /*change laser on/off shifht must set force laser off*/
    /*
        1. force ben off
        2. on shift = 10   off shift = 0
        3. on shift = 10   off shift = 11
        4. on shift = 10   off shift = 0
        5. on shift = 10   off shift = 11
    */
    
    data = 0;
    reg_field_write(APOLLOMP_FORCE_BENr, APOLLOMP_BEN_FORCE_VALUEf, &data);
    data = 1;
    reg_field_write(APOLLOMP_FORCE_BENr, APOLLOMP_BEN_FORCE_MODEf, &data);

    data = 10;
    if ((ret = reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_ON_SHIFTf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    data = 11;
    if ((ret = reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_OFF_SHIFTf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    data = 0;
    if ((ret = reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_OFF_SHIFTf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    data = 11;
    if ((ret = reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_OFF_SHIFTf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    data = 0;
    reg_field_write(APOLLOMP_FORCE_BENr, APOLLOMP_BEN_FORCE_VALUEf, &data);
    data = 1;
    reg_field_write(APOLLOMP_FORCE_BENr, APOLLOMP_BEN_FORCE_MODEf, &data);
    /*end off laser on/off shift*/


    data = 0;
    reg_field_write(APOLLOMP_FORCE_BENr, APOLLOMP_BEN_FORCE_MODEf, &data);


    data = 0x1e; /*ADJ_BC*/
    if ((ret = reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_ADJ_BCf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    data = 0x8; /*ADJ RPT_TMG*/
    if ((ret = reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST1r,APOLLOMP_RPT_TMGf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    data = 0x1; /*bypass DS FEC*/
    if ((ret = reg_field_write(APOLLOMP_EPON_FEC_CONFIGr,APOLLOMP_BYPASS_FECf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }


    /*
    RSVD_PER_PORT_MAC [4][3].RSVD_MEM[7:0] 0x21088 0x55555502
    signal name:  cfg_adj_fec_ovhd
    description  :  adjusting switch upstream FEC overhead
    
    RSVD_PER_PORT_MAC [4][3].RSVD_MEM[31] =1  for stag issue
    
    */
    ioal_mem32_write(0x21088, 0xD5555502);

    /*
    RSVD_EGR_SCH[2:0]
    signal name:      gate_len_cmps
    description        adjusting gate length from gate packet:
                        3'd1 : gate length - 1*8
                        3'd2 : gate length - 2*8
                        3'd3 : gate length - 3*8
                        3'd4 : gate length - 4*8
                        3'd5 : gate length - 5*8
                        3'd6 : gate length - 6*8
                        3'd7 : gate length - 7*8
    */
    /*ioal_mem32_write(0x2de54, 0x0);*/
    data = 0;
    if((ret = reg_field_write(APOLLOMP_RSVD_EGR_SCH0r, APOLLOMP_GATE_LENGTHf, &data))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    data = 0x1;
    if ((ret = reg_field_write(APOLLOMP_EP_MISCr,APOLLOMP_SRT_GNf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    data = 0x1;
    if ((ret = reg_field_write(APOLLOMP_EP_MISCr,APOLLOMP_ALWAYS_SVYf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }


    data = 1; /*release EPON reset*/
    if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_00r,APOLLOMP_CFG_SFT_RSTB_EPONf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }


    /*register mode setting*/
        /*register handle by ASIC*/
    data = 1;
    if ((ret = reg_field_write(APOLLOMP_EPON_RGSTR1r,APOLLOMP_HW_REGISTRATIONf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

        /*register pennding grant set to 4*/
    data = 4;
    if ((ret = reg_field_write(APOLLOMP_EPON_RGSTR3r,APOLLOMP_REG_PENDDING_GRANTf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

        /*register enable random delay*/
    data = 1;
    if ((ret = reg_field_write(APOLLOMP_EPON_DEBUG1r,APOLLOMP_DIS_RANDOM_DELAY_ENf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

        /*register ack flag fields set to 1*/
    data = 0x1;
    if ((ret = reg_field_write(APOLLOMP_EPON_REG_ACKr,APOLLOMP_ACK_FLAGf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }



    /*invalid frame handle set to drop*/
    data = 0;
    if ((ret = reg_field_write(APOLLOMP_EPON_DEBUG1r,APOLLOMP_MODE0_INVALID_HDLf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    data = 0;
    if ((ret = reg_field_write(APOLLOMP_EPON_DEBUG1r,APOLLOMP_MODE1_INVALID_HDLf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }



    data = 0x1; /*for register packet rx*/
    if ((ret = reg_field_write(APOLLOMP_EPON_MPCP_CTRr,APOLLOMP_OTHER_HANDLEf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    /*invalid mpcp packet handle set to drop*/
    data = 0x0;
    if ((ret = reg_field_write(APOLLOMP_EPON_MPCP_CTRr,APOLLOMP_OTHER_HANDLEf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON| MOD_DAL), "");
        return ret;
    }

    data = 0x0;
    if ((ret = reg_field_write(APOLLOMP_EPON_MPCP_CTRr,APOLLOMP_INVALID_LEN_HANDLEf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    /*decryption mode set to churnning key*/
    data = 0x1;
    if ((ret = reg_field_write(APOLLOMP_EPON_DECRYP_CFGr,APOLLOMP_EPON_DECRYPf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    /* function body */
    epon_init = INIT_COMPLETED;

    /*for TK3723 patch, must rx mode=0 LLID=0x7FFF packet*/
    memset(&llidEntry,0x0,sizeof(rtk_epon_llid_entry_t));
    llidEntry.llidIdx = 1;
    llidEntry.llid    = 0x7FFF;
    llidEntry.valid   = ENABLED;
    
    if ((ret = dal_apollomp_epon_llid_entry_set(&llidEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    /*set default register mac address*/
    {
        rtk_mac_t regMac;
        regMac.octet[0]=0x00;
        regMac.octet[1]=0x23;
        regMac.octet[2]=0x89;
        regMac.octet[3]=0x00;
        regMac.octet[4]=0x02;
        regMac.octet[5]=0x55;

        if ((ret = apollomp_raw_epon_regMac_set(&regMac)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }
    }


    /*enable dying gasp*/
    data = 0x1;
    if ((ret = reg_field_write(APOLLOMP_DYNGASP_CTRLr,APOLLOMP_DYNGASP_ENf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    /*set dying gasp queue to 127*/
    data = 127;
    if ((ret = reg_field_write(APOLLOMP_DYNGASP_CTRLr,APOLLOMP_DYNGASP_IDXf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_apollomp_epon_isr_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

	/*force change cf port to pon and enable it*/
	if((ret = dal_apollomp_classify_cf_sel_set(HAL_GET_PON_PORT(),CLASSIFY_CF_SEL_ENABLE))!=RT_ERR_OK &&
       (ret != RT_ERR_DRIVER_NOT_FOUND))
	{
		RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
		return ret;
	}

    /*set CF us unmatch as permit*/
    if((ret = dal_apollomp_classify_unmatchAction_set(CLASSIFY_UNMATCH_PERMIT))!=RT_ERR_OK)
	{
		  RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
	    	  return ret;
	}

    
    /*when EPON not registered to OLT only OAM packet can send out from pon port*/
    if((ret = rtk_oam_multiplexerAction_set(HAL_GET_PON_PORT(),OAM_MULTIPLEXER_ACTION_DISCARD)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    /*CRC check should be enabled*/
    data = 0;
    if ((ret = reg_field_write(APOLLOMP_RSVD_EPON_CTRL0r,APOLLOMP_IGNORE_MPCP_CRCf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }


#ifdef CONFIG_TX_DISABLE_GPIO_PIN
    /* Disable Tx disable by default */
    if ((ret = dal_apollomp_gpio_mode_set(CONFIG_TX_DISABLE_GPIO_PIN, GPIO_OUTPUT)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    if ((ret = dal_apollomp_gpio_databit_set(CONFIG_TX_DISABLE_GPIO_PIN, 0)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    if ((ret = dal_apollomp_gpio_state_set(CONFIG_TX_DISABLE_GPIO_PIN, ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
#endif

    /* PON port set as force mode */
    {
        osal_memset(&mac_ability, 0x00, sizeof(apollomp_raw_port_ability_t));
        mac_ability.speed           = PORT_SPEED_1000M;
        mac_ability.duplex          = PORT_FULL_DUPLEX;
        mac_ability.linkFib1g       = DISABLED;
        mac_ability.linkStatus      = PORT_LINKUP;
        mac_ability.txFc            = DISABLED;
        mac_ability.rxFc            = DISABLED;
        mac_ability.nwayAbility     = DISABLED;
        mac_ability.masterMod       = DISABLED;
        mac_ability.nwayFault       = DISABLED;
        mac_ability.lpi_100m        = DISABLED;
        mac_ability.lpi_giga        = DISABLED;
        if((ret = apollomp_raw_port_ForceAbility_set(HAL_GET_PON_PORT(), &mac_ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }

        if((ret = reg_field_read(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }

        data |= (0x01 << HAL_GET_PON_PORT());

        if((ret = reg_field_write(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_init */




/* Function Name:
 *      dal_apollomp_epon_intrMask_get
 * Description:
 *      Get EPON interrupt mask
 * Input:
 *	  intrType: type of interrupt
 * Output:
 *      pState: point of get interrupt state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_intrMask_get(rtk_epon_intrType_t intrType, rtk_enable_t *pState)
{
	uint32  tmpVal;
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "intrType=%d",intrType);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((EPON_INTR_END <=intrType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    switch(intrType)
    {
        case EPON_INTR_TIMEDRIFT:
            if ((ret = reg_field_read(APOLLOMP_EPON_INTRr,APOLLOMP_TIME_DRIFT_IMRf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            break;
        case EPON_INTR_MPCPTIMEOUT:
            if ((ret = reg_field_read(APOLLOMP_EPON_INTRr,APOLLOMP_MPCP_TIMEOUT_IMRf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            break;
        case EPON_INTR_REG_FIN:
            if ((ret = reg_field_read(APOLLOMP_EPON_INTRr,APOLLOMP_REG_LLID_TX_IMRf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_INPUT;

    }

    if(tmpVal==0)
        *pState = DISABLED;
    else
        *pState = ENABLED;


    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_intrMask_get */


/* Function Name:
 *      dal_apollomp_epon_intrMask_set
 * Description:
 *      Set EPON interrupt mask
 * Input:
 * 	    intrType: type of top interrupt
 *      state: set interrupt mask state
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_intrMask_set(rtk_epon_intrType_t intrType, rtk_enable_t state)
{
	uint32  tmpVal,data=1;
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "intrType=%d,state=%d",intrType, state);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((EPON_INTR_END <=intrType), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if(state==ENABLED)
        tmpVal = 1;
    else
        tmpVal = 0;

    switch(intrType)
    {
        case EPON_INTR_TIMEDRIFT:
            if((ret = reg_field_write(APOLLOMP_EPON_INTRr, APOLLOMP_TIME_DRIFT_IMSf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            if ((ret = reg_field_write(APOLLOMP_EPON_INTRr,APOLLOMP_TIME_DRIFT_IMRf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            break;
        case EPON_INTR_MPCPTIMEOUT:
            if((ret = reg_field_write(APOLLOMP_EPON_INTRr, APOLLOMP_MPCP_TIMEOUT_IMSf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }

            if ((ret = reg_field_write(APOLLOMP_EPON_INTRr,APOLLOMP_MPCP_TIMEOUT_IMRf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            break;
        case EPON_INTR_REG_FIN:
            if((ret = reg_field_write(APOLLOMP_EPON_INTRr, APOLLOMP_REG_LLID_TX_IMSf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            if ((ret = reg_field_write(APOLLOMP_EPON_INTRr,APOLLOMP_REG_LLID_TX_IMRf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_INPUT;

    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_intrMask_set */




/* Function Name:
 *      dal_apollomp_epon_intr_get
 * Description:
 *      Set EPON interrupt state
 * Input:
 * 	  intrType: type of interrupt
 * Output:
 *      pState: point for get  interrupt state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_intr_get(rtk_epon_intrType_t intrType, rtk_enable_t *pState)
{
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "intrType=%d",intrType);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((EPON_INTR_END <=intrType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    switch(intrType)
    {
        case EPON_INTR_TIMEDRIFT:
            if ((ret = apollomp_raw_epon_timeDriftIms_get(pState)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            if(*pState == ENABLED)
            {
                if ((ret = apollomp_raw_epon_timeDriftIms_clear()) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                    return ret;
                }
            }
            break;
        case EPON_INTR_MPCPTIMEOUT:
            if ((ret = apollomp_raw_epon_mpcpTimeoutIms_get(pState)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            if(*pState == ENABLED)
            {
                if ((ret = apollomp_raw_epon_mpcpTimeoutIms_clear()) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                    return ret;
                }
            }
            break;
        case EPON_INTR_REG_FIN:
            if ((ret = apollomp_raw_epon_regLlidImr_get(pState)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            if(*pState == ENABLED)
            {
                if ((ret = apollomp_raw_epon_regLlidTxIms_clear()) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                    return ret;
                }
            }
            break;
        default:
            return RT_ERR_INPUT;

    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_gpon_intr_get */


/* Function Name:
 *      dal_apollomp_epon_intr_disableAll
 * Description:
 *      Disable all of top interrupt for EPON
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_intr_disableAll(void)
{
	uint32  tmpVal;
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    tmpVal = 0;

    /* function body */
    if ((ret = reg_field_write(APOLLOMP_EPON_INTRr,APOLLOMP_TIME_DRIFT_IMRf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    if ((ret = reg_field_write(APOLLOMP_EPON_INTRr,APOLLOMP_MPCP_TIMEOUT_IMRf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    if ((ret = reg_field_write(APOLLOMP_EPON_INTRr,APOLLOMP_REG_LLID_TX_IMRf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_intr_disableAll */



/* Function Name:
 *      dal_apollomp_epon_llid_entry_set
 * Description:
 *      Set llid entry
 * Input:
 *      llidIdx: LLID table index
 * Output:
 *      pLlidEntry: LLID entry
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_llid_entry_set(rtk_epon_llid_entry_t *pLlidEntry)
{
    apollomp_raw_epon_llid_table_t  rawLlidEntry;
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLlidEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pLlidEntry->llidIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pLlidEntry->valid), RT_ERR_INPUT);

    /* function body */
    rawLlidEntry.llid                = pLlidEntry->llid;
    rawLlidEntry.report_timer        = pLlidEntry->reportTimer;
    if(ENABLED == pLlidEntry->valid )
        rawLlidEntry.valid = 1;
    else
        rawLlidEntry.valid = 0;

    if ((ret = apollomp_raw_epon_llidTable_set(pLlidEntry->llidIdx,&rawLlidEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    if ((ret = apollomp_raw_epon_regMac_set(&(pLlidEntry->mac))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    /*not llid valid must link-down pon port*/
    if((pLlidEntry->llidIdx==0) && (DISABLED == pLlidEntry->valid))
    {

        /*when EPON not registered to OLT only OAM packet can send out from pon port*/
        if((ret = rtk_oam_multiplexerAction_set(HAL_GET_PON_PORT(),OAM_MULTIPLEXER_ACTION_DISCARD)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }
        /*drain out all epon packet*/
        {
            int queueId;
            /*drain out queue 126 first, for mac tx not finish issue*/
        	if((ret = apollomp_raw_ponMacQueueDrainOutState_set(126))!=RT_ERR_OK)
        	{
        		 RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        	    	return ret;
        	}

        	for(queueId=0;queueId<8;queueId++)
        	{
            	if((ret = apollomp_raw_ponMacQueueDrainOutState_set(queueId))!=RT_ERR_OK)
            	{
            		 RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
            	    	return ret;
            	}
            }

        	if((ret = apollomp_raw_ponMacQueueDrainOutState_set(127))!=RT_ERR_OK)
        	{
        		 RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        	    	return ret;
        	}
        }
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_llid_entry_set */

/* Function Name:
 *      dal_apollomp_epon_llid_entry_get
 * Description:
 *      Get llid entry
 * Input:
 *      llidIdx: LLID table index
 * Output:
 *      pLlidEntry: LLID entry
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_llid_entry_get(rtk_epon_llid_entry_t *pLlidEntry)
{
    apollomp_raw_epon_llid_table_t  rawLlidEntry;
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLlidEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pLlidEntry->llidIdx), RT_ERR_INPUT);


    /* function body */
    if ((ret = apollomp_raw_epon_llidTable_get(pLlidEntry->llidIdx,&rawLlidEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    pLlidEntry->llid        = rawLlidEntry.llid;
    pLlidEntry->reportTimer = rawLlidEntry.report_timer;

    if(1 == rawLlidEntry.valid)
        pLlidEntry->valid = ENABLED;
    else
        pLlidEntry->valid = DISABLED;

    if(1 == rawLlidEntry.is_report_timeout)
        pLlidEntry->isReportTimeout = ENABLED;
    else
        pLlidEntry->isReportTimeout = DISABLED;

    if ((ret = apollomp_raw_epon_regMac_get(&(pLlidEntry->mac))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_llid_entry_get */


/* Function Name:
 *      dal_apollomp_epon_forceLaserState_set
 * Description:
 *      Set Force Laser status
 * Input:
 *      laserStatus: Force laser status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  dal_apollomp_epon_forceLaserState_set(rtk_epon_laser_status_t laserStatus)
{
    rtk_epon_polarity_t tempPolarity;
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "laserStatus=%d",laserStatus);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EPON_LASER_STATUS_END <=laserStatus), RT_ERR_INPUT);

    /* function body */
    if(RTK_EPON_LASER_STATUS_NORMAL==laserStatus)
    {
        /*set OE polarity to original mode*/
        if ((ret = _dal_apollomp_epon_raw_opticalPolarity_set(oe_polarity)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }
        /*set force laser on disable*/
        if ((ret = apollomp_raw_epon_forceLaserOn_set(DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }
    }
    if(RTK_EPON_LASER_STATUS_FORCE_ON==laserStatus)
    {
        /*set OE polarity to original mode*/
        if ((ret = _dal_apollomp_epon_raw_opticalPolarity_set(oe_polarity)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }
        /*set force laser on enable*/
        if ((ret = apollomp_raw_epon_forceLaserOn_set(ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }
    }
    if(RTK_EPON_LASER_STATUS_FORCE_OFF==laserStatus)
    {
        if(EPON_POLARITY_LOW==oe_polarity)
            tempPolarity=EPON_POLARITY_HIGH;
        else
            tempPolarity=EPON_POLARITY_LOW;

        /*set OE polarity to original mode*/
        if ((ret = _dal_apollomp_epon_raw_opticalPolarity_set(tempPolarity)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }
        /*set force laser on enable*/
        if ((ret = apollomp_raw_epon_forceLaserOn_set(ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }
    }

    forceLaserState = laserStatus;

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_forceLaserState_set */

/* Function Name:
 *      dal_apollomp_epon_forceLaserState_get
 * Description:
 *      Get Force Laser status
 * Input:
 *      None
 * Output:
 *      pLaserStatus: Force laser status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_forceLaserState_get(rtk_epon_laser_status_t *pLaserStatus)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLaserStatus), RT_ERR_NULL_POINTER);

    /* function body */
    *pLaserStatus=forceLaserState;

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_forceLaserState_get */


/* Function Name:
 *      dal_apollomp_epon_laserTime_set
 * Description:
 *      Set laserTime value
 * Input:
 *      laserOnTime:  OE module laser on time
 *      laserOffTime: OE module laser off time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  dal_apollomp_epon_laserTime_set(uint8 laserOnTime, uint8 laserOffTime)
{
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "laserOnTime=%d,laserOffTime=%d",laserOnTime, laserOffTime);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EPON_MAX_LASER_ON_TIME <=laserOnTime), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_EPON_MAX_LASER_OFF_TIME <=laserOffTime), RT_ERR_INPUT);

    /* function body */
    if ((ret = apollomp_raw_epon_laserOnTime_set(laserOnTime)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    if ((ret = apollomp_raw_epon_laserOffTime_set(laserOffTime)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }


    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_laserTime_set */

/* Function Name:
 *      dal_apollomp_epon_laserTime_get
 * Description:
 *      Get laser Time value
 * Input:
 *      None
 * Output:
 *      pLasetOnTime:  OE module laser on time
 *      pLasetOffTime: OE module laser off time
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_laserTime_get(uint8 *pLaserOnTime, uint8 *pLaserOffTime)
{
	uint32  tmpVal;
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON),"");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLaserOnTime), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pLaserOffTime), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = apollomp_raw_epon_laserOnTime_get(&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    *pLaserOnTime = tmpVal;

    if ((ret = apollomp_raw_epon_laserOffTime_get(&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    *pLaserOffTime = tmpVal;

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_laserTime_get */

/* Function Name:
 *      dal_apollomp_epon_syncTime_get
 * Description:
 *      Get sync Time value
 * Input:
 *      None
 * Output:
 *      pSyncTime  : olt assigned sync time
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_syncTime_get(uint8 *pSyncTime)
{
	uint32  tmpVal;
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSyncTime), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = apollomp_raw_epon_normalSyncTime_get(&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    *pSyncTime = tmpVal;

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_syncTime_get */


/* Function Name:
 *      dal_apollomp_epon_registerReq_get
 * Description:
 *      Get register request relative parameter
 * Input:
 *      pRegEntry : register request relative parament
 * Output:
 *       None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_registerReq_get(rtk_epon_regReq_t *pRegEntry)
{
	uint32  tmpVal;
 	int32   ret;
    rtk_enable_t enable;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRegEntry), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = apollomp_raw_epon_regMac_get(&(pRegEntry->mac))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    if ((ret = apollomp_raw_epon_regLlidIdx_get(&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    pRegEntry->llidIdx = tmpVal;

    if ((ret = apollomp_raw_epon_regPendingGrantNum_get(&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    pRegEntry->pendGrantNum = tmpVal;

    if ((ret = apollomp_raw_epon_regReguest_get(&enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    pRegEntry->doRequest = enable;

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_registerReq_get */

/* Function Name:
 *      dal_apollomp_epon_registerReq_set
 * Description:
 *      Set register request relative parameter
 * Input:
 *       None
 * Output:
 *       pRegEntry : register request relative parament
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_registerReq_set(rtk_epon_regReq_t *pRegEntry)
{
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRegEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pRegEntry->llidIdx), RT_ERR_INPUT);

    /* function body */
    if ((ret = apollomp_raw_epon_regMac_set(&(pRegEntry->mac))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    if ((ret = apollomp_raw_epon_regLlidIdx_set(pRegEntry->llidIdx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    if ((ret = apollomp_raw_epon_regPendingGrantNum_set(pRegEntry->pendGrantNum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }


    if(pRegEntry->doRequest == ENABLED)
    {

       uint32 data;

        /*disable US FEC*/
        if ((ret = dal_apollomp_epon_usFecState_set(DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
            return ret;
        }

       /*disable stop sync local time*/ 
       if(DAL_UTIL_CHIP_REV>CHIP_REV_ID_F)
       {
            if ((ret = reg_read(APOLLOMP_RSVD_EPON_CTRL3r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            /*clear bit 27*/
            data = data & 0xF7FFFFFF;            
            if ((ret = reg_write(APOLLOMP_RSVD_EPON_CTRL3r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
       } 


#if 0
        data = 0x0; /*trap all gate to CPU*/
        if ((ret = reg_field_write(APOLLOMP_EPON_MPCP_CTRr,APOLLOMP_GATE_TRAP_TYPEf,&data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
            return ret;
        }
#endif

#if defined(CONFIG_SDK_KERNEL_LINUX)        
        {
            uint8 randDelay;
	        	        	
            /*delay than reset epon mac*/
            get_random_bytes(&randDelay, 1);
            udelay((randDelay%64));
 
            /*reset EPON*/
            data = 0;
            if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_00r,APOLLOMP_CFG_SFT_RSTB_EPONf,&data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            } 
            
            data = 1;
            if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_00r,APOLLOMP_CFG_SFT_RSTB_EPONf,&data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            } 
              
        }
#endif



        /*check laser state*/
        data = 0x0;
        if ((ret = reg_field_write(APOLLOMP_SDS_EXT_REG24r,APOLLOMP_CFG_SYMBOLERR_CNTf,&data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
            return ret;
        }
        /*check symbol error cnt*/
        if ((ret = reg_field_read(APOLLOMP_SDS_EXT_REG25r,APOLLOMP_MUX_SYMBOLERR_CNTf,&data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
            return ret;
        }
        if(data > 0)
        {
        #if 0
            osal_printf("\n symbol error occurs(%d), change laser rx polarity to sync rx again\n",data);
        #endif
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_00r,APOLLOMP_REG_RX_SD_POR_SELf,&data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            }
            data = 0x0;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_00r,APOLLOMP_REG_RX_SD_POR_SELf,&data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            }
        }
        /*enable EPON register interrupt*/
        if ((ret = dal_apollomp_epon_intrMask_set(EPON_INTR_REG_FIN,ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }
        
        /*set mpcp Check state*/
        if ((ret = dal_apollomp_epon_mpcpCheckState_set(RTK_EPON_MPCP_CKECK_INIT)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }
        
        
        
        /*set laser on/off to default value*/
        data = 10;
        if ((ret = reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_ON_SHIFTf,&data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
            return ret;
        }
        data = 11;
        if ((ret = reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_OFF_SHIFTf,&data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
            return ret;
        }     
        
        
        
        
    }

    if ((ret = apollomp_raw_epon_regReguest_set(pRegEntry->doRequest)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_registerReq_set */



/* Function Name:
 *      dal_apollomp_epon_churningKey_set
 * Description:
 *      Set churning key entry
 * Input:
 *       pEntry : churning key relative parameter
 * Output:
 *       None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_churningKey_set(rtk_epon_churningKeyEntry_t *pEntry)
{
 	int32   ret;
    uint8   pKeyTmp[4];

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((2 <= pEntry->keyIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pEntry->llidIdx), RT_ERR_INPUT);

    pKeyTmp[0] = 0;
    pKeyTmp[1] = pEntry->churningKey[0];
    pKeyTmp[2] = pEntry->churningKey[1];
    pKeyTmp[3] = pEntry->churningKey[2];

    if(0==pEntry->keyIdx)
    {
        if ((ret = reg_array_field_write(APOLLOMP_EPON_DECRYP_KEY0r, REG_ARRAY_INDEX_NONE, pEntry->llidIdx, APOLLOMP_EPON_DECRYP_KEY0f, (uint32 *)pKeyTmp)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_array_field_write(APOLLOMP_EPON_DECRYP_KEY1r, REG_ARRAY_INDEX_NONE, pEntry->llidIdx, APOLLOMP_EPON_DECRYP_KEY1f, (uint32 *)pKeyTmp)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_churningKey_set */


/* Function Name:
 *      dal_apollomp_epon_churningKey_get
 * Description:
 *      Get churning key entry
 * Input:
 *       None
 * Output:
 *       pEntry : churning key relative parameter
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_churningKey_get(rtk_epon_churningKeyEntry_t *pEntry)
{
	uint32  tmpVal;
    uint8   *pTmpPtr;
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((2 <= pEntry->keyIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pEntry->llidIdx), RT_ERR_INPUT);

    if(0==pEntry->keyIdx)
    {
        if ((ret = reg_array_field_read(APOLLOMP_EPON_DECRYP_KEY0r, pEntry->llidIdx, REG_ARRAY_INDEX_NONE, APOLLOMP_EPON_DECRYP_KEY0f, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_array_field_read(APOLLOMP_EPON_DECRYP_KEY1r, pEntry->llidIdx, REG_ARRAY_INDEX_NONE, APOLLOMP_EPON_DECRYP_KEY1f, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            return ret;
        }
    }
    pTmpPtr = (uint8 *)&tmpVal;
    pEntry->churningKey[0] = pTmpPtr[0];
    pEntry->churningKey[1] = pTmpPtr[1];
    pEntry->churningKey[2] = pTmpPtr[2];

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_churningKey_set */


/* Function Name:
 *      dal_apollomp_epon_usFecState_get
 * Description:
 *      Get upstream fec state
 * Input:
 *       None
 * Output:
 *       *pState : upstream FEC state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_usFecState_get(rtk_enable_t *pState)
{
 	int32   ret;
	uint32  tmpVal;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_field_read(APOLLOMP_EPON_FEC_CONFIGr,APOLLOMP_FEC_US_ENf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    *pState = usFecState;

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_usFecState_get */


/* Function Name:
 *      dal_apollomp_epon_usFecState_set
 * Description:
 *      Set upstream fec state
 * Input:
 *       state : upstream FEC state
 * Output:
 *       None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_usFecState_set(rtk_enable_t state)
{
 	int32   ret;
	uint32  tmpVal,gateLenAdj;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "state=%d",state);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if(ENABLED==state)
    {
        gateLenAdj = 7;
        tmpVal =0;
	usFecState = ENABLED;
    }
    else
    {
        gateLenAdj = 0;
        tmpVal =0;
	usFecState = DISABLED;
    }
    if ((ret = reg_field_write(APOLLOMP_EPON_FEC_CONFIGr,APOLLOMP_FEC_US_ENf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_EP_MISCr,APOLLOMP_FEC_ENABLEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_write(APOLLOMP_RSVD_EGR_SCH0r,APOLLOMP_GATE_LENGTHf,&gateLenAdj)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_usFecState_set */


/* Function Name:
 *      dal_apollomp_epon_dsFecState_get
 * Description:
 *      Get down-stream fec state
 * Input:
 *       None
 * Output:
 *       *pState : down-stream FEC state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_dsFecState_get(rtk_enable_t *pState)
{
 	int32   ret;
	uint32  tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_field_read(APOLLOMP_EPON_FEC_CONFIGr,APOLLOMP_BYPASS_FECf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    
    *pState = dsFecState;

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_dsFecState_get */


/* Function Name:
 *      dal_apollomp_epon_dsFecState_set
 * Description:
 *      Set down-stream fec state
 * Input:
 *       state : down-stream FEC state
 * Output:
 *       None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_dsFecState_set(rtk_enable_t state)
{
 	int32   ret;
	uint32  tmpVal,recoverFec,data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "state=%d",state);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if(ENABLED==state)
    {
        tmpVal =1;
        recoverFec = 0;
	dsFecState = ENABLED;
    }
    else
    {
        tmpVal =1;
        recoverFec = 0;
	dsFecState = DISABLED;
    }
   
    /*reset EPON*/
    data = 0;
    if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_00r,APOLLOMP_CFG_SFT_RSTB_EPONf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_EPON_FEC_CONFIGr,APOLLOMP_BYPASS_FECf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_EPON_FEC_CONFIGr,APOLLOMP_FEC_RECOVERf,&recoverFec)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    data = 1;
    if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_00r,APOLLOMP_CFG_SFT_RSTB_EPONf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
 
    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_dsFecState_set */


/* Function Name:
 *      dal_apollomp_epon_dsFecState_set
 * Description:
 *      Set down-stream fec state
 * Input:
 *       None
 * Output:
 *       pCounter : EPON mib counter
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_mibCounter_get(rtk_epon_counter_t *pCounter)
{
 	int32   ret;
	uint32  tmpVal,queueId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* parameter check */
    RT_PARAM_CHK((NULL == pCounter), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pCounter->llidIdx), RT_ERR_INPUT);

    /* function body */
    /*get global counter*/
    if ((ret = reg_field_read(APOLLOMP_DOT3_MPCP_RX_DISCr,APOLLOMP_DOT3MPCPRXDISCOVERYGATEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    pCounter->mpcpRxDiscGate=tmpVal;

    if ((ret = reg_field_read(APOLLOMP_DOT3_EPON_FEC_UNCORRECTED_BLOCKSr,APOLLOMP_DOT3EPONFECUNCORRECTABLEBLOCKSf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    pCounter->fecUncorrectedBlocks=tmpVal;

    if ((ret = reg_field_read(APOLLOMP_DOT3_EPON_FEC_CORRECTED_BLOCKSr,APOLLOMP_DOT3EPONFECCORRECTEDBLOCKSf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    pCounter->fecCorrectedBlocks=tmpVal;

    if ((ret = reg_field_read(APOLLOMP_DOT3_EPON_FEC_CODING_VIOr,APOLLOMP_FECPCSCODINGVIOLATIONf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    pCounter->fecCodingVio=tmpVal;

    if ((ret = reg_field_read(APOLLOMP_DOT3_NOT_BROADCAST_BIT_NOT_ONU_LLIDr,APOLLOMP_NOTBROADCASTBITNOTONULLIDf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    pCounter->notBcstBitNotOnuLlid=tmpVal;

    if ((ret = reg_field_read(APOLLOMP_DOT3_BROADCAST_BIT_PLUS_ONU_LLIDr,APOLLOMP_BROADCASTBITPLUSONULLIDf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    pCounter->bcstBitPlusOnuLLid=tmpVal;

    if ((ret = reg_field_read(APOLLOMP_DOT3_BROADCAST_NOT_ONUIDr,APOLLOMP_BROADCASTBITNOTONULLIDf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    pCounter->bcstNotOnuLLid=tmpVal;

    if ((ret = reg_field_read(APOLLOMP_DOT3_CRC8_ERRORSr,APOLLOMP_CRC8ERRORSf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    pCounter->crc8Err=tmpVal;

    if ((ret = reg_field_read(APOLLOMP_DOT3_MPCP_TX_REG_REQr,APOLLOMP_DOT3MPCPTXREGREQUESTf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    pCounter->mpcpTxRegRequest=tmpVal;

    /*get llid counter*/
    if ((ret = reg_array_field_read(APOLLOMP_DOT3_MPCP_TX_REPORTr, REG_ARRAY_INDEX_NONE, pCounter->llidIdx, APOLLOMP_DOT3MPCPTXREPORTf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    pCounter->llidIdxCnt.mpcpTxReport=tmpVal;

    if ((ret = reg_array_field_read(APOLLOMP_DOT3_MPCP_EX_GATEr, REG_ARRAY_INDEX_NONE, pCounter->llidIdx, APOLLOMP_DOT3MPCPRXGATEf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    pCounter->llidIdxCnt.mpcpRxGate=tmpVal;

    if ((ret = reg_array_field_read(APOLLOMP_DOT3_ONUID_NOT_BROADCASTr, REG_ARRAY_INDEX_NONE, pCounter->llidIdx, APOLLOMP_ONULLIDNOTBROADCASTf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    pCounter->llidIdxCnt.onuLlidNotBcst=tmpVal;
    /*LLID Queue basic counter*/

    for(queueId=0;queueId<RTK_EPON_MAX_QUEUE_PER_LLID;queueId++)
    {
        if(queueId>=8)
        {
            pCounter->llidIdxCnt.queueTxFrames[queueId]=0;
            continue;
        }

        if ((ret = reg_array_field_read(APOLLOMP_DOT3_Q_TX_FRAMESr, REG_ARRAY_INDEX_NONE, (pCounter->llidIdx)*8+queueId, APOLLOMP_DOT3EXTPKGSTATTXFRAMESQUEUEf, &tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
            return ret;
        }
        pCounter->llidIdxCnt.queueTxFrames[queueId]=tmpVal;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_mibCounter_get */



/* Function Name:
 *      dal_apollomp_epon_mibGlobal_reset
 * Description:
 *      Reset EPON global counters.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL - Could not reset Global Counter
 * Note:
 *      None
 */
int32
dal_apollomp_epon_mibGlobal_reset(void)
{
 	int32   ret;
	uint32  tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* parameter check */

    /* function body */
	tmpVal = 1;
    if ((ret = reg_field_write(APOLLOMP_EPON_STAT_RSTr,APOLLOMP_RST_EPON_MIBf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    /*trigger mib reset*/
	tmpVal = 1;
    if ((ret = reg_field_write(APOLLOMP_EPON_STAT_RSTr,APOLLOMP_RST_CMDf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    /*check busy flag*/
    do{
        if ((ret = reg_field_write(APOLLOMP_EPON_STAT_RSTr,APOLLOMP_BUSY_STATf,&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
            return ret;
        }
    }while(tmpVal == 0);

    /*set to default*/
	tmpVal = 0;
    if ((ret = reg_field_write(APOLLOMP_EPON_STAT_RSTr,APOLLOMP_RST_EPON_MIBf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_mibGlobal_reset */


/* Function Name:
 *      dal_apollomp_epon_mibLlidIdx_reset
 * Description:
 *      Reset the specified LLID index counters.
 * Input:
 *      llidIdx - LLID table index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_apollomp_epon_mibLlidIdx_reset(uint8 llidIdx)
{
 	int32   ret;
	uint32  tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "llidIdx=%d",llidIdx);

    /* parameter check */
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <=llidIdx), RT_ERR_INPUT);

    /* function body */
	tmpVal = 1;
    if ((ret = reg_field_write(APOLLOMP_EPON_STAT_RSTr,APOLLOMP_RST_LLIDf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

	tmpVal = llidIdx;
    if ((ret = reg_field_write(APOLLOMP_EPON_STAT_RSTr,APOLLOMP_RST_LLID_IDXf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    /*trigger mib reset*/
	tmpVal = 1;
    if ((ret = reg_field_write(APOLLOMP_EPON_STAT_RSTr,APOLLOMP_RST_CMDf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    /*check busy flag*/
    do{
        if ((ret = reg_field_write(APOLLOMP_EPON_STAT_RSTr,APOLLOMP_BUSY_STATf,&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
            return ret;
        }
    }while(tmpVal == 0);

    /*set to default*/
	tmpVal = 0;
    if ((ret = reg_field_write(APOLLOMP_EPON_STAT_RSTr,APOLLOMP_RST_EPON_MIBf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_mibLlidIdx_reset */




/* Function Name:
 *      dal_apollomp_epon_losState_get
 * Description:
 *      Get laser lose of signal state.
 * Input:
 *      pState LOS state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_apollomp_epon_losState_get(rtk_enable_t *pState)
{
	uint32  tmpVal;
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_field_read(APOLLOMP_SDS_EXT_REG29r,APOLLOMP_LINKOK_LATf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if(1==tmpVal)
        *pState = DISABLED;
    else
        *pState = ENABLED;

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_losState_get */



int32
dal_apollomp_epon_churningKeyMode_set(rtk_epon_churning_mode_t mode)
{
 	int32   ret;
	uint32  tmpVal;

    /* parameter check */
    RT_PARAM_CHK((RTK_EPON_CHURNING_END <=mode), RT_ERR_INPUT);

    /* function body */
	if(RTK_EPON_CHURNING_BL == mode)
	    tmpVal = 1;
    else
	    tmpVal = 0;

    if ((ret = reg_field_write(APOLLOMP_RSVD_EPON_CTRL0r,APOLLOMP_CHURN_MODEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;

}



int32
dal_apollomp_epon_fecOverhead_set(uint32 val)
{
 	int32   ret;
	uint32  tmpVal;

    /* parameter check */
    RT_PARAM_CHK((64 <= val), RT_ERR_INPUT);

    /* function body */
    tmpVal = val;
    if ((ret = reg_field_write(APOLLOMP_RSVD_EPON_CTRL0r,APOLLOMP_FEC_OVER_TXf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}




int32
dal_apollomp_epon_reportMode_set(rtk_epon_report_mode_t mode)
{
 	int32   ret;
	uint32  tmpVal;

    /* parameter check */
    RT_PARAM_CHK((RTK_EPON_REPORT_END <= mode), RT_ERR_INPUT);

    /* function body */
    switch(mode)
    {
        case RTK_EPON_REPORT_NORMAL:
            tmpVal = 1;
            if ((ret = reg_field_write(APOLLOMP_PON_MISC_CFGr,APOLLOMP_EPON_RPT_LENGTH_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 0;
            if ((ret = reg_field_write(APOLLOMP_PON_MISC_CFGr,APOLLOMP_EPON_STATIC_RPT_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 0;
            if ((ret = reg_field_write(APOLLOMP_RSVD_EPON_CTRL0r,APOLLOMP_FORCE_REPORTf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            break;
        case RTK_EPON_REPORT_0_F:
            tmpVal = 0;
            if ((ret = reg_field_write(APOLLOMP_PON_MISC_CFGr,APOLLOMP_EPON_RPT_LENGTH_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 1;
            if ((ret = reg_field_write(APOLLOMP_PON_MISC_CFGr,APOLLOMP_EPON_STATIC_RPT_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 0;
            if ((ret = reg_field_write(APOLLOMP_RSVD_EPON_CTRL0r,APOLLOMP_FORCE_REPORTf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            break;
        case RTK_EPON_REPORT_FORCE_0:
            tmpVal = 0;
            if ((ret = reg_field_write(APOLLOMP_PON_MISC_CFGr,APOLLOMP_EPON_RPT_LENGTH_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 0;
            if ((ret = reg_field_write(APOLLOMP_PON_MISC_CFGr,APOLLOMP_EPON_STATIC_RPT_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 1;
            if ((ret = reg_field_write(APOLLOMP_RSVD_EPON_CTRL0r,APOLLOMP_FORCE_REPORTf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 0;
            if ((ret = reg_field_write(APOLLOMP_RSVD_EPON_CTRL0r,APOLLOMP_REPORT_CONTENTf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            break;
        case RTK_EPON_REPORT_FORCE_F:
            tmpVal = 0;
            if ((ret = reg_field_write(APOLLOMP_PON_MISC_CFGr,APOLLOMP_EPON_RPT_LENGTH_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 0;
            if ((ret = reg_field_write(APOLLOMP_PON_MISC_CFGr,APOLLOMP_EPON_STATIC_RPT_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 1;
            if ((ret = reg_field_write(APOLLOMP_RSVD_EPON_CTRL0r,APOLLOMP_FORCE_REPORTf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 1;
            if ((ret = reg_field_write(APOLLOMP_RSVD_EPON_CTRL0r,APOLLOMP_REPORT_CONTENTf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_INPUT;
    }
    return RT_ERR_OK;
}





int32
dal_apollomp_epon_reportMode_get(rtk_epon_report_mode_t *pMode)
{
 	int32   ret;
	uint32  tmpVal;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);
    *pMode = RTK_EPON_REPORT_END;

    /* function body */
    if ((ret = reg_field_read(APOLLOMP_RSVD_EPON_CTRL0r,APOLLOMP_FORCE_REPORTf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if(1==tmpVal)
    {/*force mode*/
        if ((ret = reg_field_read(APOLLOMP_RSVD_EPON_CTRL0r,APOLLOMP_REPORT_CONTENTf,&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
            return ret;
        }
        if(1==tmpVal)/*forcee to 0xFFFF*/
        {
            *pMode = RTK_EPON_REPORT_FORCE_F;
        }
        else
        {
            *pMode = RTK_EPON_REPORT_FORCE_0;
        }
    }
    else
    {
        if ((ret = reg_field_read(APOLLOMP_PON_MISC_CFGr,APOLLOMP_EPON_RPT_LENGTH_MODEf,&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
            return ret;
        }
        if(1==tmpVal)
        {
            *pMode = RTK_EPON_REPORT_NORMAL;
        }
        else
        {
            if ((ret = reg_field_read(APOLLOMP_PON_MISC_CFGr,APOLLOMP_EPON_STATIC_RPT_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            if(1==tmpVal)
            {
                *pMode = RTK_EPON_REPORT_0_F;
            }
        }
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_apollomp_epon_mpcpTimeoutVal_get
 * Description:
 *      Get mpcp time out value.
 * Input:
 *      pTimeVal - pointer of mpcp tomeout value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_apollomp_epon_mpcpTimeoutVal_get(uint32 *pTimeVal)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTimeVal), RT_ERR_NULL_POINTER);

    /* function body */

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_mpcpTimeoutVal_get */



/* Function Name:
 *      dal_apollomp_epon_mpcpTimeoutVal_set
 * Description:
 *      Set mpcp time out value.
 * Input:
 *      timeVal - mpcp tomeout value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_apollomp_epon_mpcpTimeoutVal_set(uint32 timeVal)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "timeVal=%d",timeVal);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((0x80 <=timeVal), RT_ERR_INPUT);

    /* function body */

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_mpcpTimeoutVal_set */


static int32
_dal_apollomp_epon_raw_opticalPolarity_set(rtk_epon_polarity_t polarity)
{
 	int32   ret;
	uint32  tmpVal;

    /* parameter check */
    RT_PARAM_CHK((EPON_POLARITY_END <=polarity), RT_ERR_INPUT);

    /* function body */
    if(EPON_POLARITY_HIGH == polarity)
    {
        tmpVal = 0;
    }
    else
    {
        tmpVal = 1;
    }
    if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_18r,APOLLOMP_CFG_FRC_BEN_INVf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
}
/* Function Name:
 *      dal_apollomp_epon_opticalPolarity_set
 * Description:
 *      Set OE module polarity.
 * Input:
 *      polarity - OE module polarity
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_apollomp_epon_opticalPolarity_set(rtk_epon_polarity_t polarity)
{
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "polarity=%d",polarity);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((EPON_POLARITY_END <=polarity), RT_ERR_INPUT);

    /* function body */
    oe_polarity = polarity;

    if ((ret = _dal_apollomp_epon_raw_opticalPolarity_set(polarity)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_opticalPolarity_set */

/* Function Name:
 *      dal_apollomp_epon_opticalPolarity_get
 * Description:
 *      Set OE module polarity.
 * Input:
 *      pPolarity - pointer of OE module polarity
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_apollomp_epon_opticalPolarity_get(rtk_epon_polarity_t *pPolarity)
{

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPolarity), RT_ERR_NULL_POINTER);

    /* function body */
    *pPolarity = oe_polarity;
#if 0
    if ((ret = reg_field_read(APOLLOMP_WSDS_DIG_18r,APOLLOMP_CFG_FRC_BEN_INVf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if(0 == tmpVal)
        *pPolarity=EPON_POLARITY_HIGH;
    else
        *pPolarity=EPON_POLARITY_LOW;
#endif
    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_opticalPolarity_get */




/* Function Name:
 *      dal_apollomp_epon_fecState_get
 * Description:
 *      Get EPON global fec state
 * Input:
 *       None
 * Output:
 *       *pState : glogal FEC state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_fecState_get(rtk_enable_t *pState)
{
 	int32   ret;
	uint32  tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);



    /* function body */
    tmpVal=0;
    if ((ret = reg_field_read(APOLLOMP_SDS_REG25r,APOLLOMP_CFG_FIB2G_TXCFG_NP_P1f,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    /*bit 1 is PCS FEC mode*/
    if(tmpVal & 0x2)
    {
        *pState=ENABLED;
    }
    else
    {
        *pState=DISABLED;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_fecState_get */


/* Function Name:
 *      dal_apollomp_epon_fecState_set
 * Description:
 *      Set EPON global fec state
 * Input:
 *       state : global FEC state
 * Output:
 *       None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_fecState_set(rtk_enable_t state)
{
 	int32   ret;
	uint32  tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    tmpVal=0;
    if ((ret = reg_field_read(APOLLOMP_SDS_REG25r,APOLLOMP_CFG_FIB2G_TXCFG_NP_P1f,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if(ENABLED == state)
    {
        tmpVal = tmpVal | 0x2;
    }
    else
    {
        tmpVal = tmpVal & (uint32)(~0x00000002);
    }

    if ((ret = reg_field_write(APOLLOMP_SDS_REG25r,APOLLOMP_CFG_FIB2G_TXCFG_NP_P1f,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_fecState_set */



/* Function Name:
 *      dal_apollomp_epon_llidEntryNum_get
 * Description:
 *      Get EPON support LLID entry number
 * Input:
 *       None
 * Output:
 *       *pState : glogal FEC state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_llidEntryNum_get(uint32 *num)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* parameter check */
    RT_PARAM_CHK((NULL == num), RT_ERR_NULL_POINTER);

    /* function body */
    *num = 1;
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_llidEntryNum_get */


/* Function Name:
 *      dal_apollomp_epon_dbgInfo_get
 * Description:
 *      Get EPON debug information
 * Input:
 *	  intrType: type of interrupt
 * Output:
 *      pDbgCnt: point of relative debug counter
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_dbgInfo_get(rtk_epon_dbgCnt_t *pDbgCnt)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDbgCnt), RT_ERR_NULL_POINTER);

    /* function body */
    pDbgCnt->losIntCnt      =mpcp_cnt;
    pDbgCnt->mpcpIntCnt     =_eponMpcpTimeoutIntCnt;
    pDbgCnt->timeDriftIntCnt=_eponTimeDriftIntCnt;
    pDbgCnt->regFinIntCnt   =_eponRegIntCnt;

    /*reset counter*/
    mpcp_cnt = 0;
    _eponRegIntCnt=0;
    _eponTimeDriftIntCnt=0;
    _eponMpcpTimeoutIntCnt=0;

    pDbgCnt->gateBackToBack   =0;
    pDbgCnt->gatenoForceReport=0;
    pDbgCnt->gateHidden       =0;

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_dbgInfo_get */



int32 dal_apollomp_is_epon_txMacHangUp(uint32  *isHangUp)
{
 	int32   ret;
	uint32  tmpVal,checkVal;
    
    *isHangUp = 0;

    /* function body */
    tmpVal=0x700;
    if ((ret = reg_field_write(APOLLOMP_DBG_BLK_SELr,APOLLOMP_DBG_BLK_SELf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    tmpVal=0x1028;
    if ((ret = reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    tmpVal=0;
    if ((ret = reg_field_read(APOLLOMP_CHIP_DEBUG_OUTr,APOLLOMP_DBGOf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    checkVal = 0x800;
    if((tmpVal&0x00000ff0) == checkVal)    
    {
        *isHangUp = 1;    
    }
    
    tmpVal=0x700;
    if ((ret = reg_field_write(APOLLOMP_DBG_BLK_SELr,APOLLOMP_DBG_BLK_SELf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    tmpVal=0x1018;
    if ((ret = reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    tmpVal=0;
    if ((ret = reg_field_read(APOLLOMP_CHIP_DEBUG_OUTr,APOLLOMP_DBGOf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    checkVal = 0x800000;
    if((tmpVal&0x00fe0000) == checkVal)    
    {
        *isHangUp = 1;    
    }    
    
    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_fecState_set */

/* Function Name:
 *      dal_apollomp_epon_forcePRBS_set
 * Description:
 *      Set force PRBS status
 * Input:
 *      prbsCfg             - PRBS config
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_forcePRBS_set(rtk_epon_prbs_t prbsCfg)
{
 	int32   ret;
	uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "prbsCfg=%d",prbsCfg);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EPON_PRBS_END <=prbsCfg), RT_ERR_INPUT);

	/* function body */
	if (RTK_EPON_PRBS_OFF != prbsCfg)
    {
        /*set tpo GPON mode*/
        
        #if 0
        /*reset epon*/
        data = 0;
        if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_00r,APOLLOMP_CFG_SFT_RSTB_EPONf,&data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
            return ret;
        }
        #endif
        
        data = 1;
        if ((ret = reg_field_write(APOLLOMP_PON_MODE_CFGr,APOLLOMP_GPON_ENf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }
        data = 0;
        if ((ret = reg_field_write(APOLLOMP_PON_MODE_CFGr,APOLLOMP_EPON_ENf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }

        /* set the serdes mode to GPON mode */
        data = 0x8;
        if ((ret = reg_field_write(APOLLOMP_SDS_CFGr, APOLLOMP_CFG_SDS_MODEf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }
        
        #if 0
        /*release reset epon*/
        data = 1;
        if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_00r,APOLLOMP_CFG_SFT_RSTB_EPONf,&data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
            return ret;
        }
        #endif
        
        /*delay wait reset ok*/
        osal_time_mdelay(10);

        data = 0xc;
        if ((ret = reg_write(APOLLOMP_WSDS_DIG_01r, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }
        
        /*set prbs mode*/
        switch (prbsCfg)
        {
            case RTK_EPON_PRBS_31:
                data = 0x40AA;
                break;
            case RTK_EPON_PRBS_23:
                data = 0x4099;
                break;
            case RTK_EPON_PRBS_15:
                data = 0x4088;
                break;
            case RTK_EPON_PRBS_7:
                data = 0x4077;
                break;
            case RTK_EPON_PRBS_3:
                data = 0x4066;
                break;

            default:
                return RT_ERR_OUT_OF_RANGE;
                break;
        }

        if (RT_ERR_OK != (ret = reg_field_write(APOLLOMP_WSDS_DIG_19r, APOLLOMP_CFG_PRBS_TYPE_SELf, &data)))
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x", ret);
            return RT_ERR_FAILED;
        }        
        
        /*PRBS enable*/
        data = 1;
        if (RT_ERR_OK != (ret = reg_field_write(APOLLOMP_WSDS_DIG_1Ar, APOLLOMP_CFG_PRBS_ENf, &data)))
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x", ret);
            return RT_ERR_FAILED;
        }
        
    }
    else /*prbs off*/
    {
        /*PRBS disable*/
        data = 0;
        if (RT_ERR_OK != (ret = reg_field_write(APOLLOMP_WSDS_DIG_1Ar, APOLLOMP_CFG_PRBS_ENf, &data)))
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x", ret);
            return RT_ERR_FAILED;
        }
        data = 0x0; 
        if (RT_ERR_OK != (ret = reg_field_write(APOLLOMP_WSDS_DIG_19r, APOLLOMP_CFG_PRBS_TYPE_SELf, &data)))
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x", ret);
            return RT_ERR_FAILED;
        }
        data = 0x0;
        if ((ret = reg_write(APOLLOMP_WSDS_DIG_01r, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }        
                       
        /*epon init*/
        if (RT_ERR_OK != (ret = dal_apollomp_epon_init()))
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x", ret);
            return RT_ERR_FAILED;
        }               
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_forcePRBS_set */

/* Function Name:
 *      dal_apollomp_epon_forcePRBS_get
 * Description:
 *      Get force PRBS status
 * Input:
 *      None
 * Output:
 *      pPrbsCfg            - PRBS config
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_apollomp_epon_forcePRBS_get(rtk_epon_prbs_t *pPrbsCfg)
{
 	int32   ret;
	uint32  data;

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPrbsCfg), RT_ERR_INPUT);

    if (RT_ERR_OK != (ret = reg_field_read(APOLLOMP_WSDS_DIG_1Ar, APOLLOMP_CFG_PRBS_ENf, &data)))
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x", ret);
        return RT_ERR_FAILED;
    }

    if(data)
    {
        if (RT_ERR_OK != (ret = reg_field_read(APOLLOMP_WSDS_DIG_19r, APOLLOMP_CFG_PRBS_TYPE_SELf, &data)))
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x", ret);
            return RT_ERR_FAILED;
        }

        switch(data)
        {
        case 0x40AA:
            *pPrbsCfg = RTK_EPON_PRBS_31;
            break;
        case 0x4099:
            *pPrbsCfg = RTK_EPON_PRBS_23;
            break;
        case 0x4088:
            *pPrbsCfg = RTK_EPON_PRBS_15;
            break;
        case 0x4077:
            *pPrbsCfg = RTK_EPON_PRBS_7;
            break;
        case 0x4066:
            *pPrbsCfg = RTK_EPON_PRBS_3;
            break;
        default:
            return RT_ERR_OUT_OF_RANGE;
        }
    }
    else
    {
        *pPrbsCfg = RTK_EPON_PRBS_OFF;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_forcePRBS_get */



int32 dal_apollomp_epon_mpcpCheckState_set(dal_apollomp_epon_mpcp_check_state_t mpcpState)
{
 
  if(DAL_UTIL_CHIP_REV >= CHIP_REV_ID_F)
  { 
      //osal_printf("\n mpcpCheck change from :%d to %d \n",_eponMpcpCheckState,mpcpState);
      _eponMpcpCheckState= mpcpState; 
  }
  else
  {
    //osal_printf("\n this chip version always set to RTK_EPON_MPCP_CKECK_INVALID_BEN_STATE \n");
    _eponMpcpCheckState= RTK_EPON_MPCP_CKECK_INVALID_BEN_STATE;
    
  }


 return RT_ERR_OK; 
    
}    

int32 dal_apollomp_epon_mpcpCheckState_get(dal_apollomp_epon_mpcp_check_state_t *mpcpState)
{
    
    *mpcpState=_eponMpcpCheckState;
    return RT_ERR_OK;
}   

#if defined(CONFIG_SDK_KERNEL_LINUX)
EXPORT_SYMBOL(dal_apollomp_epon_mpcpCheckState_set);
EXPORT_SYMBOL(dal_apollomp_epon_mpcpCheckState_get);
#endif


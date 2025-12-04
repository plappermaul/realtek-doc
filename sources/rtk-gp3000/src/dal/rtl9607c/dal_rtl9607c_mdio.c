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
 * $Revision: 61228 $
 * $Date: 2016-03-16 14:49:14 +0800 (Thu, 16 Mar 2016) $
 *
 * Purpose : Definition of MAIO API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) MDIO
 *
 */
#include <dal/rtl9607c/dal_rtl9607c.h>
#include <rtk/mdio.h>
#include <osal/time.h>
#if defined(CONFIG_COMMON_RT_API)
#include <rtk/rt/rt_mdio.h>
#endif
#include <dal/rtl9607c/dal_rtl9607c_mdio.h>

static uint32 mdio_init = { INIT_NOT_COMPLETED } ;

static uint8 SET = 0;
static uint8 PORT = 0;
static uint8 PHYID = 0;
static rtk_mdio_format_t FMT = MDIO_FMT_C22;

#define RTL9607C_MDIO_SET_0_PORT        0
#define RTL9607C_MDIO_SET_1_PORT        1
#define RTL9607C_MDIO_SET_0_PHYID       0
#define RTL9607C_MDIO_SET_1_PHYID       0

#if defined(CONFIG_COMMON_RT_API)
static int32
_dal_rtl9607c_mdio_init(void)
{
    int32 ret;
    uint32 tmp,value;

    /* polling_pmsk disable */
    value=0;
    if((ret = reg_array_write(RTL9607C_CFG_POLL_MDX_PMSKr,RTL9607C_MDIO_SET_0_PORT,REG_ARRAY_INDEX_NONE, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }
    if((ret = reg_array_write(RTL9607C_CFG_POLL_MDX_PMSKr,RTL9607C_MDIO_SET_1_PORT,REG_ARRAY_INDEX_NONE, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    /* en mdc0/mdc1 mdc1_5M mdc0_1.25M */
    if((ret = reg_read(RTL9607C_CFG_POLL_MDX_CTRLr, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    tmp=1;
    if((ret = reg_field_set(RTL9607C_CFG_POLL_MDX_CTRLr,RTL9607C_SET0_MDC_ENf,&tmp, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }
    tmp=0;
    if((ret = reg_field_set(RTL9607C_CFG_POLL_MDX_CTRLr,RTL9607C_SMI_SET0_PREAMBLE_SELf,&tmp, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    tmp=1;
    if((ret = reg_field_set(RTL9607C_CFG_POLL_MDX_CTRLr,RTL9607C_SET1_MDC_ENf,&tmp, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }
    tmp=0;
    if((ret = reg_field_set(RTL9607C_CFG_POLL_MDX_CTRLr,RTL9607C_SMI_SET1_PREAMBLE_SELf,&tmp, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    if((ret = reg_write(RTL9607C_CFG_POLL_MDX_CTRLr, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    /* ###setting phy addr and set0/set1## */
    value = (0 << 5) | RTL9607C_MDIO_SET_0_PHYID;
    if((ret = reg_array_write(RTL9607C_CFG_POLL_MDX_ADDRr,RTL9607C_MDIO_SET_0_PORT, REG_ARRAY_INDEX_NONE, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }
    value = (1 << 5) | RTL9607C_MDIO_SET_1_PHYID;
    if((ret = reg_array_write(RTL9607C_CFG_POLL_MDX_ADDRr,RTL9607C_MDIO_SET_1_PORT, REG_ARRAY_INDEX_NONE, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }
        

    /* ##enable MDIO master PAD_LED0/PAD_LED1 ## */
    if((ret = reg_read(RTL9607C_IO_MODE_ENr, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }
    tmp = 0x1;
    if((ret = reg_field_set(RTL9607C_IO_MODE_ENr, RTL9607C_SW_MDX_M_ENf,&tmp, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }
    tmp = 0x1;
    if((ret = reg_field_set(RTL9607C_IO_MODE_ENr, RTL9607C_EXT_MDX_M_ENf,&tmp, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }
    if((ret = reg_write(RTL9607C_IO_MODE_ENr, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}
#endif

/* Function Name:
 *      dal_rtl9607c_mdio_init
 * Description:
 *      Init MDIO configuration
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Init MDIO configuration
 */
int32
dal_rtl9607c_mdio_init(void)
{
    int32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MDIO), "");

    /* function body */
    mdio_init=INIT_COMPLETED;

#if defined(CONFIG_COMMON_RT_API)
    if((ret = _dal_rtl9607c_mdio_init())!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MDIO), "");
        mdio_init = INIT_NOT_COMPLETED;
        return ret;
    }
#else
    if((ret = dal_rtl9607c_mdio_cfg_set(SET,PORT,PHYID,FMT))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MDIO), "");
        mdio_init = INIT_NOT_COMPLETED;
        return ret;
    }
#endif

    RT_INIT_CHK(mdio_init);

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_mdio_init */

/* Function Name:
 *      dal_rtl9607c_mdio_cfg_set
 * Description:
 *      Set MDIO configuration
 * Input:
 *      set   - Select which set of MDIO
 *      port  - Select shich virtual port to access MDIO
 *      phyid - IEEE 802.3
 *      fmt   - MDIO format to access
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must set MDIO configuration before MDIO read/write
 *      The collective of APIs 
 *          dal_rtl9607c_mdio_cfg_set/dal_rtl9607c_mdio_cfg_get
 *          dal_rtl9607c_mdio_c22_write/dal_rtl9607c_mdio_c22_read 
 *          dal_rtl9607c_mdio_c45_write/dal_rtl9607c_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          dal_rtl9607c_mdio_read/dal_rtl9607c_mdio_write
 *      instead
 */
int32
dal_rtl9607c_mdio_cfg_set(uint8 set,uint8 port,uint8 phyid,rtk_mdio_format_t fmt)
{
    int32 ret;
    uint32 tmp,value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MDIO), "set=%d,port=%d,phyid=%d,fmt=%d",set, port, phyid, fmt);

    /* parameter check */
    RT_PARAM_CHK((2 <=set), RT_ERR_INPUT);
    RT_PARAM_CHK((4 <=port), RT_ERR_INPUT);
    RT_PARAM_CHK((32 <=phyid), RT_ERR_INPUT);
    RT_PARAM_CHK((MDIO_FMT_END <=fmt), RT_ERR_INPUT);

    RT_INIT_CHK(mdio_init);

    /* function body */
    if(fmt==MDIO_FMT_C22) //clause 22
    {
        /* polling_pmsk disable */
        value=0;
        if((ret = reg_array_write(RTL9607C_CFG_POLL_MDX_PMSKr,port,REG_ARRAY_INDEX_NONE, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }

        /* en mdc0/mdc1 mdc1_5M mdc0_1.25M */
        if((ret = reg_read(RTL9607C_CFG_POLL_MDX_CTRLr, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
        if(set==0)
        {
            tmp=1;
            if((ret = reg_field_set(RTL9607C_CFG_POLL_MDX_CTRLr,RTL9607C_SET0_MDC_ENf,&tmp, &value))!= RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
                return ret;
            }
            tmp=0;
            if((ret = reg_field_set(RTL9607C_CFG_POLL_MDX_CTRLr,RTL9607C_SMI_SET0_PREAMBLE_SELf,&tmp, &value))!= RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
                return ret;
            }
        }
        else
        {
            tmp=1;
            if((ret = reg_field_set(RTL9607C_CFG_POLL_MDX_CTRLr,RTL9607C_SET1_MDC_ENf,&tmp, &value))!= RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
                return ret;
            }
            tmp=0;
            if((ret = reg_field_set(RTL9607C_CFG_POLL_MDX_CTRLr,RTL9607C_SMI_SET1_PREAMBLE_SELf,&tmp, &value))!= RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
                return ret;
            }
        }
        if((ret = reg_write(RTL9607C_CFG_POLL_MDX_CTRLr, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }

        /* ###setting phy addr and set0/set1## */
        value = set<<5 | phyid;
        if((ret = reg_array_write(RTL9607C_CFG_POLL_MDX_ADDRr,port, REG_ARRAY_INDEX_NONE, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
        
        /* ##setting Polling frame set1 0x0: Clause 22 standard reg.MDIO format## */
        if((ret = reg_read(RTL9607C_CFG_POLL_CTRL_0r, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
        if(set==0)
        {
            tmp = 0x0;
            if((ret = reg_field_set(RTL9607C_CFG_POLL_CTRL_0r, RTL9607C_SMI_SET0_FMT_SELf,&tmp, &value))!= RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
                return ret;
            }
        }
        else
        {
            tmp = 0x0;
            if((ret = reg_field_set(RTL9607C_CFG_POLL_CTRL_0r, RTL9607C_SMI_SET1_FMT_SELf,&tmp, &value))!= RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
                return ret;
            }
        }
        if((ret = reg_write(RTL9607C_CFG_POLL_CTRL_0r, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }

        /* ##enable MDIO master PAD_LED0/PAD_LED1 ## */
        if((ret = reg_read(RTL9607C_IO_MODE_ENr, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }

        if(set==0)
        {
            tmp = 0x1;
            if((ret = reg_field_set(RTL9607C_IO_MODE_ENr, RTL9607C_SW_MDX_M_ENf,&tmp, &value))!= RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
                return ret;
            }
        }
        else
        {
            tmp = 0x1;
            if((ret = reg_field_set(RTL9607C_IO_MODE_ENr, RTL9607C_EXT_MDX_M_ENf,&tmp, &value))!= RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
                return ret;
            }
        }
        if((ret = reg_write(RTL9607C_IO_MODE_ENr, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }

        /* write to what port */
        value = 0x1<<port;
        if((ret = reg_write(RTL9607C_SMI_INDRT_ACCESS_CTRL_2r, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }

        /* read from what port*/
        value = port<<5;
        if((ret = reg_write(RTL9607C_SMI_INDRT_ACCESS_BC_PHYID_CTRLr, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
    }
    else //clause 45
    {
        /* polling_pmsk disable */
        value=0;
        if((ret = reg_array_write(RTL9607C_CFG_POLL_MDX_PMSKr, port,REG_ARRAY_INDEX_NONE, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }

        /* en mdc0/mdc1 mdc1_5M mdc0_1.25M */
        if((ret = reg_read(RTL9607C_CFG_POLL_MDX_CTRLr, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
        if(set==0)
        {
            tmp=1;
            if((ret = reg_field_set(RTL9607C_CFG_POLL_MDX_CTRLr,RTL9607C_SET0_MDC_ENf,&tmp, &value))!= RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
                return ret;
            }
            tmp=0;
            if((ret = reg_field_set(RTL9607C_CFG_POLL_MDX_CTRLr,RTL9607C_SMI_SET0_PREAMBLE_SELf,&tmp, &value))!= RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
                return ret;
            }
        }
        else
        {
            tmp=1;
            if((ret = reg_field_set(RTL9607C_CFG_POLL_MDX_CTRLr,RTL9607C_SET1_MDC_ENf,&tmp, &value))!= RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
                return ret;
            }
            tmp=0;
            if((ret = reg_field_set(RTL9607C_CFG_POLL_MDX_CTRLr,RTL9607C_SMI_SET1_PREAMBLE_SELf,&tmp, &value))!= RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
                return ret;
            }
        }
        if((ret = reg_write(RTL9607C_CFG_POLL_MDX_CTRLr, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }

        /* ###setting phy addr and set0/set1## */
        value = set<<5 | phyid;
        if((ret = reg_array_write(RTL9607C_CFG_POLL_MDX_ADDRr, port, REG_ARRAY_INDEX_NONE, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
        
        /* ##setting Polling frame set1 0x2: Clause 45 standard MDIO format## */
        if((ret = reg_read(RTL9607C_CFG_POLL_CTRL_0r, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
        if(set==0)
        {
            tmp = 0x2;
            if((ret = reg_field_set(RTL9607C_CFG_POLL_CTRL_0r, RTL9607C_SMI_SET0_FMT_SELf,&tmp, &value))!= RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
                return ret;
            }
        }
        else
        {
            tmp = 0x2;
            if((ret = reg_field_set(RTL9607C_CFG_POLL_CTRL_0r, RTL9607C_SMI_SET1_FMT_SELf,&tmp, &value))!= RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
                return ret;
            }
        }
        if((ret = reg_write(RTL9607C_CFG_POLL_CTRL_0r, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }

        /* ##enable MDIO master PAD_LED0/PAD_LED1 ## */
        if((ret = reg_read(RTL9607C_IO_MODE_ENr, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
        if(set==0)
        {
            tmp = 0x1;
            if((ret = reg_field_set(RTL9607C_IO_MODE_ENr, RTL9607C_SW_MDX_M_ENf,&tmp, &value))!= RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
                return ret;
            }
        }
        else
        {
            tmp = 0x1;
            if((ret = reg_field_set(RTL9607C_IO_MODE_ENr, RTL9607C_EXT_MDX_M_ENf,&tmp, &value))!= RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
                return ret;
            }
        }
        if((ret = reg_write(RTL9607C_IO_MODE_ENr, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }

        /* write to what port */
        value = 0x1<<port;
        if((ret = reg_write(RTL9607C_SMI_INDRT_ACCESS_CTRL_2r, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }

        /* read from what port*/
        value = port<<5;
        if((ret = reg_write(RTL9607C_SMI_INDRT_ACCESS_BC_PHYID_CTRLr, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
    }

    SET = set;
    PORT = port;
    PHYID = phyid;
    FMT = fmt;

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_mdio_cfg_set */

/* Function Name:
 *      dal_rtl9607c_mdio_cfg_get
 * Description:
 *      Get MDIO configuration
 * Input:
 *      None
 * Output:
 *      set   - Select which set of MDIO
 *      port  - Select shich virtual port to access MDIO
 *      phyid - clause 22 of IEEE 802.3 define
 *      fmt   - MDIO format to access
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must check MDIO configuration before MDIO read/write
 *      The collective of APIs 
 *          dal_rtl9607c_mdio_cfg_set/dal_rtl9607c_mdio_cfg_get
 *          dal_rtl9607c_mdio_c22_write/dal_rtl9607c_mdio_c22_read 
 *          dal_rtl9607c_mdio_c45_write/dal_rtl9607c_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          dal_rtl9607c_mdio_read/dal_rtl9607c_mdio_write
 *      instead
 */
int32
dal_rtl9607c_mdio_cfg_get(uint8 *set,uint8 *port,uint8 *phyid,rtk_mdio_format_t *fmt)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == set), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == port), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == phyid), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == fmt), RT_ERR_NULL_POINTER);

    /* function body */
    RT_INIT_CHK(mdio_init);

    *set = SET;
    *port = PORT;
    *phyid = PHYID; 
    *fmt = FMT;

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_mdio_cfg_get */

/* Function Name:
 *      dal_rtl9607c_mdio_c22_write
 * Description:
 *      Write MDIO by clause 22
 * Input:
 *      reg   - clause 22 of IEEE 802.3 define
 *      data  - clause 22 of IEEE 802.3 define
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      The collective of APIs 
 *          dal_rtl9607c_mdio_cfg_set/dal_rtl9607c_mdio_cfg_get
 *          dal_rtl9607c_mdio_c22_write/dal_rtl9607c_mdio_c22_read 
 *          dal_rtl9607c_mdio_c45_write/dal_rtl9607c_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          dal_rtl9607c_mdio_read/dal_rtl9607c_mdio_write
 *      instead
 */
int32
dal_rtl9607c_mdio_c22_write(uint8 c22_reg,uint16 data)
{
    int32 ret;
    uint32 value,timeout;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MDIO), "c22_reg=%d,data=%d",c22_reg, data);

    /* parameter check */
    RT_PARAM_CHK((32 <=c22_reg), RT_ERR_INPUT);

    RT_INIT_CHK(mdio_init);

    /* function body */
    value = data;
    if((ret = reg_write(RTL9607C_SMI_INDRT_ACCESS_CTRL_3r, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    value = 0x1FFF<<11 | c22_reg<<6 | 0x1<<4 | 0x1;
    if((ret = reg_write(RTL9607C_SMI_INDRT_ACCESS_CTRL_0r, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    /* Wait for complete */
    timeout = 10000;
    while(timeout > 0)
    {
        if((ret = reg_read(RTL9607C_SMI_INDRT_ACCESS_CTRL_0r, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
        if((value & 0x1) == 0)
        {
            break;
        }
        osal_time_udelay(1);
        timeout --;
    }
    if(0 ==  timeout)
    {
        return RT_ERR_TIMEOUT;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_mdio_c22_write */

/* Function Name:
 *      dal_rtl9607c_mdio_c22_read
 * Description:
 *      Read MDIO by clause 22
 * Input:
 *      reg   - clause 22 of IEEE 802.3 define
 * Output:
 *      data  - clause 22 of IEEE 802.3 define
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      The collective of APIs 
 *          dal_rtl9607c_mdio_cfg_set/dal_rtl9607c_mdio_cfg_get
 *          dal_rtl9607c_mdio_c22_write/dal_rtl9607c_mdio_c22_read 
 *          dal_rtl9607c_mdio_c45_write/dal_rtl9607c_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          dal_rtl9607c_mdio_read/dal_rtl9607c_mdio_write
 *      instead
 */
int32
dal_rtl9607c_mdio_c22_read(uint8 c22_reg,uint16 *data)
{
    int32 ret;
    uint32 value,timeout;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MDIO), "c22_reg=%d",c22_reg);

    /* parameter check */
    RT_PARAM_CHK((32 <=c22_reg), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);

    RT_INIT_CHK(mdio_init);

    /* function body */
    value = 0x1FFF<<11 | c22_reg<<6 | 0x1;
    if((ret = reg_write(RTL9607C_SMI_INDRT_ACCESS_CTRL_0r, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    /* Wait for complete */
    timeout = 10000;
    while(timeout > 0)
    {
        if((ret = reg_read(RTL9607C_SMI_INDRT_ACCESS_CTRL_0r, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
        if((value & 0x1) == 0)
        {
            break;
        }
        osal_time_udelay(1);
        timeout --;
    }
    if(0 ==  timeout)
    {
        return RT_ERR_TIMEOUT;
    }

    if((ret = reg_read(RTL9607C_SMI_INDRT_ACCESS_CTRL_3r, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    *data = (value>>16) & 0xffff;

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_mdio_c22_read */

/* Function Name:
 *      dal_rtl9607c_mdio_c45_write
 * Description:
 *      Write MDIO by clause 45 
 * Input:
 *      dev   - clause 45 of IEEE 802.3 define
 *      reg   - clause 45 of IEEE 802.3 define
 *      data  - clause 45 of IEEE 802.3 define
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      The collective of APIs 
 *          dal_rtl9607c_mdio_cfg_set/dal_rtl9607c_mdio_cfg_get
 *          dal_rtl9607c_mdio_c22_write/dal_rtl9607c_mdio_c22_read 
 *          dal_rtl9607c_mdio_c45_write/dal_rtl9607c_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          dal_rtl9607c_mdio_read/dal_rtl9607c_mdio_write
 *      instead
 */
int32
dal_rtl9607c_mdio_c45_write(uint8 dev,uint16 c45_reg,uint16 data)
{
    int32 ret;
    uint32 value,timeout;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MDIO), "dev=%d,c45_reg=%d,data=%d",dev, c45_reg, data);

    /* parameter check */
    RT_PARAM_CHK((32 <=dev), RT_ERR_INPUT);

    RT_INIT_CHK(mdio_init);

    /* function body */
    value = dev<<16 | c45_reg;
    if((ret = reg_write(RTL9607C_SMI_INDRT_ACCESS_MMD_CTRLr, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    value = data;
    if((ret = reg_write(RTL9607C_SMI_INDRT_ACCESS_CTRL_3r, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    value = 0x1<<4 | 0x1<<3 | 0x1;
    if((ret = reg_write(RTL9607C_SMI_INDRT_ACCESS_CTRL_0r, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }
    
    /* Wait for complete */
    timeout = 10000;
    while(timeout > 0)
    {
        if((ret = reg_read(RTL9607C_SMI_INDRT_ACCESS_CTRL_0r, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
        if((value & 0x1) == 0)
        {
            break;
        }
        osal_time_udelay(1);
        timeout --;
    }
    if(0 ==  timeout)
    {
        return RT_ERR_TIMEOUT;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_mdio_c45_write */

/* Function Name:
 *      dal_rtl9607c_mdio_c45_write
 * Description:
 *      Read MDIO by clause 45 
 * Input:
 *      dev   - clause 45 of IEEE 802.3 define
 *      reg   - clause 45 of IEEE 802.3 define
 * Output:
 *      data  - clause 45 of IEEE 802.3 define
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      The collective of APIs 
 *          dal_rtl9607c_mdio_cfg_set/dal_rtl9607c_mdio_cfg_get
 *          dal_rtl9607c_mdio_c22_write/dal_rtl9607c_mdio_c22_read 
 *          dal_rtl9607c_mdio_c45_write/dal_rtl9607c_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          dal_rtl9607c_mdio_read/dal_rtl9607c_mdio_write
 *      instead
 */
int32
dal_rtl9607c_mdio_c45_read(uint8 dev,uint16 c45_reg,uint16 *data)
{
    int32 ret;
    uint32 value,timeout;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MDIO), "dev=%d,c45_reg=%d",dev, c45_reg);

    /* parameter check */
    RT_PARAM_CHK((32 <=dev), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);

    RT_INIT_CHK(mdio_init);

    /* function body */
    value = dev<<16 | c45_reg;
    if((ret = reg_write(RTL9607C_SMI_INDRT_ACCESS_MMD_CTRLr, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    value = 0x1<<3 | 0x1;
    if((ret = reg_write(RTL9607C_SMI_INDRT_ACCESS_CTRL_0r, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    /* Wait for complete */
    timeout = 10000;
    while(timeout > 0)
    {
        if((ret = reg_read(RTL9607C_SMI_INDRT_ACCESS_CTRL_0r, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
        if((value & 0x1) == 0)
        {
            break;
        }
        osal_time_udelay(1);
        timeout --;
    }
    if(0 ==  timeout)
    {
        return RT_ERR_TIMEOUT;
    }

    if((ret = reg_read(RTL9607C_SMI_INDRT_ACCESS_CTRL_3r, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    *data = (value>>16) & 0xffff;

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_mdio_c45_read */

#if defined(CONFIG_COMMON_RT_API)
/* Function Name:
 *      dal_rtl9607c_mdio_read
 * Description:
 *      Read from MDIO bus
 * Input:
 *      info  - the necessary info for MDIO access
 * Output:
 *      data  - read data from MDIO bus
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      The APIs dal_rtl9607c_mdio_read/dal_rtl9607c_mdio_write replace the collective APIs
 *          dal_rtl9607c_mdio_cfg_set/dal_rtl9607c_mdio_cfg_get
 *          dal_rtl9607c_mdio_c22_write/dal_rtl9607c_mdio_c22_read 
 *          dal_rtl9607c_mdio_c45_write/dal_rtl9607c_mdio_c45_read
 */
int32
dal_rtl9607c_mdio_read(rt_mdio_info_t *info, uint16 *data)
{
    int32 ret;
    uint32 value, tmp,timeout;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MDIO), "fmt=%d, set=%d, phyAddr=%d, dev=%d,c45_reg=%d\n", info->format, info->set, info->phyAddr, info->dev, info->reg);

    RT_INIT_CHK(mdio_init);
    RT_PARAM_CHK((2 <=info->set), RT_ERR_INPUT);
    RT_PARAM_CHK((32 <=info->phyAddr), RT_ERR_INPUT);
    RT_PARAM_CHK((MDIO_FMT_END <=info->format), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);

    /* function body */
    /* ##setting Polling frame MDIO format## */
    if(info->format == RT_MDIO_FMT_C22)
    {
        RT_PARAM_CHK((32 <=info->reg), RT_ERR_INPUT);
        tmp = 0x0;
    }
    else
    {
        RT_PARAM_CHK((32 <=info->dev), RT_ERR_INPUT);
        tmp = 0x2;
    }
    if((ret = reg_read(RTL9607C_CFG_POLL_CTRL_0r, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }
    if(0 == info->set)
    {
        if((ret = reg_field_set(RTL9607C_CFG_POLL_CTRL_0r, RTL9607C_SMI_SET0_FMT_SELf,&tmp, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
    }
    else
    {
        if((ret = reg_field_set(RTL9607C_CFG_POLL_CTRL_0r, RTL9607C_SMI_SET1_FMT_SELf,&tmp, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
    }
    if((ret = reg_write(RTL9607C_CFG_POLL_CTRL_0r, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    /* ###setting phy addr## */
    value = info->phyAddr;
    if(info->set == 0)
    {
        if((ret = reg_array_field_write(RTL9607C_CFG_POLL_MDX_ADDRr,RTL9607C_MDIO_SET_0_PORT, REG_ARRAY_INDEX_NONE, RTL9607C_PORT_ADDRf, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
    }
    else
    {
        if((ret = reg_array_field_write(RTL9607C_CFG_POLL_MDX_ADDRr,RTL9607C_MDIO_SET_1_PORT, REG_ARRAY_INDEX_NONE, RTL9607C_PORT_ADDRf, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
    }

    /* read from what port*/
    value = (info->set == 0 ? RTL9607C_MDIO_SET_0_PORT : RTL9607C_MDIO_SET_1_PORT)<<5;
    if((ret = reg_write(RTL9607C_SMI_INDRT_ACCESS_BC_PHYID_CTRLr, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    value = info->dev<<16 | info->reg;
    if((ret = reg_write(RTL9607C_SMI_INDRT_ACCESS_MMD_CTRLr, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    if(info->format == RT_MDIO_FMT_C22)
    {
        value = (0x1FFF<<11) | (info->reg<<6) | 0x1;
    }
    else
    {
        value = 0x1<<3 | 0x1;
    }
    if((ret = reg_write(RTL9607C_SMI_INDRT_ACCESS_CTRL_0r, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    /* Wait for complete */
    timeout = 10000;
    while(timeout > 0)
    {
        if((ret = reg_read(RTL9607C_SMI_INDRT_ACCESS_CTRL_0r, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
        if((value & 0x1) == 0)
        {
            break;
        }
        osal_time_udelay(1);
        timeout --;
    }
    if(0 ==  timeout)
    {
        return RT_ERR_TIMEOUT;
    }

    if((ret = reg_read(RTL9607C_SMI_INDRT_ACCESS_CTRL_3r, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    *data = (value>>16) & 0xffff;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9607c_mdio_write
 * Description:
 *      Read from MDIO bus
 * Input:
 *      info  - the necessary info for MDIO access
 *      data  - write data to MDIO bus
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      The APIs dal_rtl9607c_mdio_read/dal_rtl9607c_mdio_write replace the collective APIs
 *          dal_rtl9607c_mdio_cfg_set/dal_rtl9607c_mdio_cfg_get
 *          dal_rtl9607c_mdio_c22_write/dal_rtl9607c_mdio_c22_read 
 *          dal_rtl9607c_mdio_c45_write/dal_rtl9607c_mdio_c45_read
 */
int32
dal_rtl9607c_mdio_write(rt_mdio_info_t *info, uint16 data)
{
    int32 ret;
    uint32 value, tmp,timeout;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MDIO), "fmt=%d, set=%d, phyAddr=%d, dev=%d,c45_reg=%d\n", info->format, info->set, info->phyAddr, info->dev, info->reg);

    RT_INIT_CHK(mdio_init);
    RT_PARAM_CHK((2 <=info->set), RT_ERR_INPUT);
    RT_PARAM_CHK((32 <=info->phyAddr), RT_ERR_INPUT);
    RT_PARAM_CHK((MDIO_FMT_END <=info->format), RT_ERR_INPUT);

    /* function body */
    /* ##setting Polling frame MDIO format## */
    if(info->format == RT_MDIO_FMT_C22)
    {
        RT_PARAM_CHK((32 <=info->reg), RT_ERR_INPUT);
        tmp = 0x0;
    }
    else
    {
        RT_PARAM_CHK((32 <=info->dev), RT_ERR_INPUT);
        tmp = 0x2;
    }
    if((ret = reg_read(RTL9607C_CFG_POLL_CTRL_0r, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }
    if(0 == info->set)
    {
        if((ret = reg_field_set(RTL9607C_CFG_POLL_CTRL_0r, RTL9607C_SMI_SET0_FMT_SELf,&tmp, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
    }
    else
    {
        if((ret = reg_field_set(RTL9607C_CFG_POLL_CTRL_0r, RTL9607C_SMI_SET1_FMT_SELf,&tmp, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
    }
    if((ret = reg_write(RTL9607C_CFG_POLL_CTRL_0r, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    /* ###setting phy addr## */
    value = info->phyAddr;
    if(info->set == 0)
    {
        if((ret = reg_array_field_write(RTL9607C_CFG_POLL_MDX_ADDRr,RTL9607C_MDIO_SET_0_PORT, REG_ARRAY_INDEX_NONE, RTL9607C_PORT_ADDRf, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
    }
    else
    {
        if((ret = reg_array_field_write(RTL9607C_CFG_POLL_MDX_ADDRr,RTL9607C_MDIO_SET_1_PORT, REG_ARRAY_INDEX_NONE, RTL9607C_PORT_ADDRf, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
    }

    /* write to what port */
    value = 0x1<< (info->set == 0 ? RTL9607C_MDIO_SET_0_PORT : RTL9607C_MDIO_SET_1_PORT);
    if((ret = reg_write(RTL9607C_SMI_INDRT_ACCESS_CTRL_2r, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    value = (info->dev<<16) | (info->reg);
    if((ret = reg_write(RTL9607C_SMI_INDRT_ACCESS_MMD_CTRLr, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    value = data;
    if((ret = reg_write(RTL9607C_SMI_INDRT_ACCESS_CTRL_3r, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    if(info->format == RT_MDIO_FMT_C22)
    {
        value = 0x1FFF<<11 | info->reg<<6 | 0x1<<4 | 0x1;
    }
    else
    {
        value = 0x1<<4 | 0x1<<3 | 0x1;
    }
    if((ret = reg_write(RTL9607C_SMI_INDRT_ACCESS_CTRL_0r, &value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }
    
    /* Wait for complete */
    timeout = 10000;
    while(timeout > 0)
    {
        if((ret = reg_read(RTL9607C_SMI_INDRT_ACCESS_CTRL_0r, &value))!= RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
            return ret;
        }
        if((value & 0x1) == 0)
        {
            break;
        }
        osal_time_udelay(1);
        timeout --;
    }
    if(0 ==  timeout)
    {
        return RT_ERR_TIMEOUT;
    }

    return RT_ERR_OK;
}
#endif


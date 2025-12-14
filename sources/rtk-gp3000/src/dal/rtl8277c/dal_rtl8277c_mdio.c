/*
 * Copyright (C) 2021 Realtek Semiconductor Corp. 
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
#include <common/rt_type.h>
#include <rtk/mdio.h>
#include <dal/rtl8277c/dal_rtl8277c.h>
#if defined(CONFIG_COMMON_RT_API)
#include <rtk/rt/rt_mdio.h>
#endif
#include <dal/rtl8277c/dal_rtl8277c_mdio.h>
#include <aal_phy.h>

static uint32 mdio_init = { INIT_NOT_COMPLETED } ;

static uint8 SET = 0;
static uint8 PORT = 0;
static uint8 PHYID = 0;
static rtk_mdio_format_t FMT = MDIO_FMT_C22;

/* Function Name:
 *      dal_rtl8277c_mdio_init
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
dal_rtl8277c_mdio_init(void)
{
    int32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MDIO), "");

    /* function body */
    mdio_init=INIT_COMPLETED;

    if((ret = dal_rtl8277c_mdio_cfg_set(SET,PORT,PHYID,FMT))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MDIO), "");
        mdio_init = INIT_NOT_COMPLETED;
        return ret;
    }

    RT_INIT_CHK(mdio_init);

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_mdio_init */

/* Function Name:
 *      dal_rtl8277c_mdio_cfg_set
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
 *          dal_rtl8277c_mdio_cfg_set/dal_rtl8277c_mdio_cfg_get
 *          dal_rtl8277c_mdio_c22_write/dal_rtl8277c_mdio_c22_read 
 *          dal_rtl8277c_mdio_c45_write/dal_rtl8277c_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          dal_rtl8277c_mdio_read/dal_rtl8277c_mdio_write
 *      instead
 */
int32
dal_rtl8277c_mdio_cfg_set(uint8 set,uint8 port,uint8 phyid,rtk_mdio_format_t fmt)
{
    int32 ret;
    uint32 tmp,value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MDIO), "set=%d,port=%d,phyid=%d,fmt=%d",set, port, phyid, fmt);

    /* parameter check */
    RT_PARAM_CHK((1 <=set), RT_ERR_INPUT);
    RT_PARAM_CHK((1 <=port), RT_ERR_INPUT);
    RT_PARAM_CHK((32 <=phyid), RT_ERR_INPUT);
    RT_PARAM_CHK((MDIO_FMT_END <=fmt), RT_ERR_INPUT);

    RT_INIT_CHK(mdio_init);

    /* function body */
    SET = set;
    PORT = port;
    PHYID = phyid;
    FMT = fmt;

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_mdio_cfg_set */

/* Function Name:
 *      dal_rtl8277c_mdio_cfg_get
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
 *          dal_rtl8277c_mdio_cfg_set/dal_rtl8277c_mdio_cfg_get
 *          dal_rtl8277c_mdio_c22_write/dal_rtl8277c_mdio_c22_read 
 *          dal_rtl8277c_mdio_c45_write/dal_rtl8277c_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          dal_rtl8277c_mdio_read/dal_rtl8277c_mdio_write
 *      instead
 */
int32
dal_rtl8277c_mdio_cfg_get(uint8 *set,uint8 *port,uint8 *phyid,rtk_mdio_format_t *fmt)
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
}   /* end of dal_rtl8277c_mdio_cfg_get */

/* Function Name:
 *      dal_rtl8277c_mdio_c22_write
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
 *          dal_rtl8277c_mdio_cfg_set/dal_rtl8277c_mdio_cfg_get
 *          dal_rtl8277c_mdio_c22_write/dal_rtl8277c_mdio_c22_read 
 *          dal_rtl8277c_mdio_c45_write/dal_rtl8277c_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          dal_rtl8277c_mdio_read/dal_rtl8277c_mdio_write
 *      instead
 */
int32
dal_rtl8277c_mdio_c22_write(uint8 c22_reg,uint16 data)
{
    int32 ret;
    uint32 value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MDIO), "c22_reg=%d,data=%d",c22_reg, data);

    /* parameter check */
    RT_PARAM_CHK((32 <=c22_reg), RT_ERR_INPUT);

    RT_INIT_CHK(mdio_init);

    /* function body */
    if(ret = aal_mdio_write(0, 1, PHYID, c22_reg, 0, data) != 0)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_mdio_c22_write */

/* Function Name:
 *      dal_rtl8277c_mdio_c22_read
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
 *          dal_rtl8277c_mdio_cfg_set/dal_rtl8277c_mdio_cfg_get
 *          dal_rtl8277c_mdio_c22_write/dal_rtl8277c_mdio_c22_read 
 *          dal_rtl8277c_mdio_c45_write/dal_rtl8277c_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          dal_rtl8277c_mdio_read/dal_rtl8277c_mdio_write
 *      instead
 */
int32
dal_rtl8277c_mdio_c22_read(uint8 c22_reg,uint16 *data)
{
    int32 ret;
    uint32 value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MDIO), "c22_reg=%d",c22_reg);

    /* parameter check */
    RT_PARAM_CHK((32 <=c22_reg), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);

    RT_INIT_CHK(mdio_init);

    /* function body */
    if(ret = aal_mdio_read(0, 1, PHYID, c22_reg, 0, data) != 0)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_mdio_c22_read */

/* Function Name:
 *      dal_rtl8277c_mdio_c45_write
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
 *          dal_rtl8277c_mdio_cfg_set/dal_rtl8277c_mdio_cfg_get
 *          dal_rtl8277c_mdio_c22_write/dal_rtl8277c_mdio_c22_read 
 *          dal_rtl8277c_mdio_c45_write/dal_rtl8277c_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          dal_rtl8277c_mdio_read/dal_rtl8277c_mdio_write
 *      instead
 */
int32
dal_rtl8277c_mdio_c45_write(uint8 dev,uint16 c45_reg,uint16 data)
{
    int32 ret;
    uint32 value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MDIO), "dev=%d,c45_reg=%d,data=%d",dev, c45_reg, data);

    /* parameter check */
    RT_PARAM_CHK((32 <=dev), RT_ERR_INPUT);

    RT_INIT_CHK(mdio_init);

    /* function body */
    if(ret = aal_mdio_write(0, 0, PHYID, dev, c45_reg, data) != 0)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_mdio_c45_write */

/* Function Name:
 *      dal_rtl8277c_mdio_c45_write
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
 *          dal_rtl8277c_mdio_cfg_set/dal_rtl8277c_mdio_cfg_get
 *          dal_rtl8277c_mdio_c22_write/dal_rtl8277c_mdio_c22_read 
 *          dal_rtl8277c_mdio_c45_write/dal_rtl8277c_mdio_c45_read
 *      are not recommended anymore. User are encurage to use
 *          dal_rtl8277c_mdio_read/dal_rtl8277c_mdio_write
 *      instead
 */
int32
dal_rtl8277c_mdio_c45_read(uint8 dev,uint16 c45_reg,uint16 *data)
{
    int32 ret;
    uint32 value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MDIO), "dev=%d,c45_reg=%d",dev, c45_reg);

    /* parameter check */
    RT_PARAM_CHK((32 <=dev), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);

    RT_INIT_CHK(mdio_init);

    /* function body */
    if(ret = aal_mdio_read(0, 0, PHYID, dev, c45_reg, data) != 0)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_mdio_c45_read */

#if defined(CONFIG_COMMON_RT_API)
/* Function Name:
 *      dal_rtl8277c_mdio_read
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
 *      The APIs dal_rtl8277c_mdio_read/dal_rtl8277c_mdio_write replace the collective APIs
 *          dal_rtl8277c_mdio_cfg_set/dal_rtl8277c_mdio_cfg_get
 *          dal_rtl8277c_mdio_c22_write/dal_rtl8277c_mdio_c22_read 
 *          dal_rtl8277c_mdio_c45_write/dal_rtl8277c_mdio_c45_read
 */
int32
dal_rtl8277c_mdio_read(rt_mdio_info_t *info, uint16 *data)
{
    uint8 st_code = 1;
    int32 ret;
    uint32 value;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MDIO), "fmt=%d, set=%d, phyAddr=%d, dev=%d,c45_reg=%d\n", info->format, info->set, info->phyAddr, info->dev, info->reg);

    RT_INIT_CHK(mdio_init);
    RT_PARAM_CHK((2 <=info->set), RT_ERR_INPUT);
    RT_PARAM_CHK((32 <=info->phyAddr), RT_ERR_INPUT);
    RT_PARAM_CHK((MDIO_FMT_END <=info->format), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);
    if(info->format == RT_MDIO_FMT_C22)
    {
        RT_PARAM_CHK((32 <=info->reg), RT_ERR_INPUT);
        st_code = 1;
    }
    else
    {
        RT_PARAM_CHK((32 <=info->dev), RT_ERR_INPUT);
        st_code = 0;
    }

    /* function body */
    if(ret = aal_mdio_read(0, st_code, info->phyAddr, info->dev, info->reg, data) != 0)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8277c_mdio_write
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
 *      The APIs dal_rtl8277c_mdio_read/dal_rtl8277c_mdio_write replace the collective APIs
 *          dal_rtl8277c_mdio_cfg_set/dal_rtl8277c_mdio_cfg_get
 *          dal_rtl8277c_mdio_c22_write/dal_rtl8277c_mdio_c22_read 
 *          dal_rtl8277c_mdio_c45_write/dal_rtl8277c_mdio_c45_read
 */
int32
dal_rtl8277c_mdio_write(rt_mdio_info_t *info, uint16 data)
{
    uint8 st_code = 1;
    int32 ret;
    uint32 value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_MDIO), "fmt=%d, set=%d, phyAddr=%d, dev=%d,c45_reg=%d\n", info->format, info->set, info->phyAddr, info->dev, info->reg);

    RT_INIT_CHK(mdio_init);
    RT_PARAM_CHK((2 <=info->set), RT_ERR_INPUT);
    RT_PARAM_CHK((32 <=info->phyAddr), RT_ERR_INPUT);
    RT_PARAM_CHK((MDIO_FMT_END <=info->format), RT_ERR_INPUT);
    if(info->format == RT_MDIO_FMT_C22)
    {
        RT_PARAM_CHK((32 <=info->reg), RT_ERR_INPUT);
        st_code = 1;
    }
    else
    {
        RT_PARAM_CHK((32 <=info->dev), RT_ERR_INPUT);
        st_code = 0;
    }

    /* function body */
    if(ret = aal_mdio_write(0, st_code, info->phyAddr, info->dev, info->reg, data) != 0)
    {
        RT_ERR(ret, (MOD_QOS|MOD_MDIO), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}
#endif


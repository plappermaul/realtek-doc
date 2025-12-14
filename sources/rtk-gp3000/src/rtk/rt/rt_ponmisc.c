/*
 * Copyright (C) 2019 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : Definition of PON MISC API
 *
 * Feature : The file includes the following modules and sub-modules
 *           
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <dal/dal_mgmt.h>
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
#include <rtk/rt/rt_ponmisc.h>
#include <rtk/rt/rt_i2c.h>
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
#include <rt/include/rt_ext_mapper.h>
#endif
/*
 * Symbol Definition
 */
rt_ponmisc_ponMode_t PON_MODE = RT_GPON_MODE;
rt_ponmisc_ponSpeed_t PON_SPEED = RT_1G25G_SPEED;

/* By default, TX output power with LSB equal to 0.1 uW*/
static uint32 rt_transceiver_tx_pwr_scale = 1;

static uint32 rt_transceiver_addrs[][2]={
    {20,16},
    {68,16},
    {40,16},
    {96,2},
    {98,2},
    {100,2},
    {102,2},
    {104,2},
};

/*
 * Data Declaration
 */
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
static rt_ext_mapper_t* RT_EXT_MAPPER = NULL;
#endif

#ifdef CONFIG_LUNA_G3_SERIES
#define MAXSIDNUM 512
rt_ponmisc_sidInfo_t g_sidInfo[MAXSIDNUM];
#endif

/*
 * Macro Declaration
 */

 /*
 * Function Declaration
 */
/* Function Name:
 *      rt_ponmisc_init
 * Description:
 *      Initialize pon misc interface.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_FEATURE_NOT_SUPPORTED
 * Note:
 *      Must initialize before calling any other APIs.
 */
int32
rt_ponmisc_init(void)
{
    int32 ret = RT_ERR_OK;

#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_init)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_init();
    RTK_API_UNLOCK();

    memset(g_sidInfo,0,sizeof(rt_ponmisc_sidInfo_t)*MAXSIDNUM);
#endif

    return ret;
}   /* end of rt_ponmisc_init */

/* Function Name:
 *      rt_ponmisc_modeSpeed_get
 * Description:
 *      get pon misc mode speed inforamtion.
 * Input:
 *      None
 * Output:
 *      pPonMode    - current running PON mode
 *      pPonSpeed   - current running PON speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      
 */
int32
rt_ponmisc_modeSpeed_get(rt_ponmisc_ponMode_t *pPonMode,rt_ponmisc_ponSpeed_t *pPonSpeed)
{
    int32 ret = RT_ERR_OK;
    
    RT_PARAM_CHK((NULL == pPonMode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPonSpeed), RT_ERR_NULL_POINTER);

#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_mode_get(pPonMode,pPonSpeed);
    RTK_API_UNLOCK();
#else
    rtk_ponmac_mode_t mode;

    if (NULL == RT_MAPPER->ponmac_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_mode_get(&mode);
    RTK_API_UNLOCK();

    if(mode == PONMAC_MODE_GPON)
        PON_MODE = RT_GPON_MODE;
    else if(mode == PONMAC_MODE_EPON)
        PON_MODE = RT_EPON_MODE;
    else
        return RT_ERR_FEATURE_NOT_SUPPORTED;

    PON_SPEED = RT_1G25G_SPEED;

    *pPonMode = PON_MODE;
    *pPonSpeed = PON_SPEED;
#endif
    return ret;
}   /* end of rt_ponmisc_modeSpeed_get */

/* Function Name:
 *      rt_ponmisc_modeSpeed_set
 * Description:
 *      set pon misc mode and speed inforamtion.
 * Input:
 *      ponMode    - PON mode
 *      ponSpeed   - PON speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      
 */
int32
rt_ponmisc_modeSpeed_set(rt_ponmisc_ponMode_t ponMode,rt_ponmisc_ponSpeed_t ponSpeed)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_mode_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_mode_set(ponMode, ponSpeed);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->ponmac_mode_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_mode_set(ponMode);
    RTK_API_UNLOCK();
#endif

    return ret;
}   /* end of rt_ponmisc_modeSpeed_set */

/* Function Name:
 *      rt_ponmisc_sid_get
 * Description:
 *      get pon misc sid inforamtion.
 * Input:
 *      sid         - stream id
 * Output:
 *      pSidInfo    - corresponding sid infotmation
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT          - Invalid input parameters.
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      
 */
int32
rt_ponmisc_sid_get(uint16 sid,rt_ponmisc_sidInfo_t *pSidInfo)
{
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((NULL == pSidInfo), RT_ERR_NULL_POINTER);

    memset(pSidInfo,0,sizeof(rt_ponmisc_sidInfo_t));

#ifdef CONFIG_LUNA_G3_SERIES
    if(sid < MAXSIDNUM)
        memcpy(pSidInfo,&g_sidInfo[sid],sizeof(rt_ponmisc_sidInfo_t));
#else
    rt_gpon_usFlow_t flow_attr;

    ret = rt_gpon_usFlow_get(sid,&flow_attr);
    if(ret == RT_ERR_OK)
    {
        pSidInfo->enFlag = ENABLED;
        pSidInfo->dqFlag = ENABLED;
        pSidInfo->dstPort = 5;
        pSidInfo->pri = flow_attr.tcQueueId;
        pSidInfo->flowId = sid;
    }
    else
    {
        pSidInfo->enFlag = DISABLED;
        pSidInfo->dqFlag = DISABLED;
        pSidInfo->dstPort = 5;
        pSidInfo->pri = 0;
        pSidInfo->flowId = 0;
        ret = RT_ERR_OK;
    }      
#endif

    return ret;
}   /* end of rt_ponmisc_sid_get */

/* Function Name:
 *      rt_ponmisc_sid_set
 * Description:
 *      set pon misc sid inforamtion.
 * Input:
 *      sid         - stream id
 *      sidInfo     - corresponding sid infotmation
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      
 */
int32
rt_ponmisc_sid_set(uint16 sid,rt_ponmisc_sidInfo_t sidInfo)
{
    int32 ret = RT_ERR_OK;

#ifdef CONFIG_LUNA_G3_SERIES
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
#if defined(CONFIG_CA8279_SERIES)
    /*Not support*/
#else
    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
        if(RT_EXT_MAPPER == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;
    }

    if (NULL == RT_EXT_MAPPER->internal_pon_sid_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    ret = RT_EXT_MAPPER->internal_pon_sid_set(sid,sidInfo);
    RTK_API_UNLOCK()
#endif
#endif

#if defined(CONFIG_CA8279_SERIES) || defined(CONFIG_CA8277B_SERIES)
    /*Not support*/
#else
    if (NULL == RT_MAPPER->rt_cls_pon_sid_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_cls_pon_sid_set(sid,sidInfo);
    RTK_API_UNLOCK();
#endif
    if(sid < MAXSIDNUM)
        memcpy(&g_sidInfo[sid],&sidInfo,sizeof(rt_ponmisc_sidInfo_t));
#else
    //Not support
#endif

    return ret;
}   /* end of rt_ponmisc_sid_set */

/* Function Name:
*      rt_ponmisc_transceiver_get
* Description:
*      Get transceiver value by the specific type
* Input:
*      type            - the transceiver parameter type
* Output:
*      pData           - the pointer of data for the specific transceiver parameter
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
rt_ponmisc_transceiver_get(rt_transceiver_parameter_type_t type, rt_transceiver_data_t *pData)
{
    int32   ret;
    uint32  devID = 0, idx;
    uint32 transceiver_data[TRANSCEIVER_LEN];

    osal_memset(transceiver_data, 0, TRANSCEIVER_LEN);

    /* function body */
    if (NULL == RT_MAPPER->i2c_read)
        return RT_ERR_DRIVER_NOT_FOUND;

    if (RT_TRANSCEIVER_PARA_TYPE_MAX == type)
        return RT_ERR_INPUT;

    if  ((type == RT_TRANSCEIVER_PARA_TYPE_VENDOR_NAME)||(type == RT_TRANSCEIVER_PARA_TYPE_SN)|| (type == RT_TRANSCEIVER_PARA_TYPE_VENDOR_PART_NUM))
    {
        devID = TRANSCEIVER_A0;
    }else if ((type > RT_TRANSCEIVER_PARA_TYPE_VENDOR_PART_NUM)  && (type < RT_TRANSCEIVER_PARA_TYPE_MAX))
    {
        devID = TRANSCEIVER_A2;
    }

    for (idx = rt_transceiver_addrs[type][0]; idx < rt_transceiver_addrs[type][0]+rt_transceiver_addrs[type][1]; idx++)
    {
        ret = rt_i2c_read(TRANSCEIVER_PORT, devID, idx, &(transceiver_data[idx-rt_transceiver_addrs[type][0]]));
        pData->buf[idx-rt_transceiver_addrs[type][0]]=transceiver_data[idx-rt_transceiver_addrs[type][0]];
    }
	pData->tx_pwr_scale=rt_transceiver_tx_pwr_scale;
    return ret;
}   /* end of rt_ponmisc_transceiver_get */

/* Function Name:
*              rt_ponmisc_transceiver_tx_power_scale_set
* Description:
*              Set transceiver tx power scale 
* Input:
*              scale   - scale for TX output power with LSB (in unit 0.1 uW)
* Output:
*              N/A
* Return:
*              RT_ERR_OK       
*              others  
* Note:
*
*/
int32
rt_ponmisc_transceiver_tx_power_scale_set(uint32 scale)
{
	int32 ret = RT_ERR_OK;
    rt_transceiver_tx_pwr_scale = scale;
    return ret;
}



/* Function Name:
*      rt_ponmisc_burstPolarityReverse_get
* Description:
*      Get the status of PON burst polarity reverse
* Input:
*      none
* Output:
*      pPolarity       - pointer of burst polarity reverse status
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
rt_ponmisc_burstPolarityReverse_get(rt_ponmisc_polarity_t *pPolarity)
{
    int32 ret = RT_ERR_OK;
    
    RT_PARAM_CHK((NULL == pPolarity), RT_ERR_NULL_POINTER);

#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_burstPolarityReverse_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_burstPolarityReverse_get(pPolarity);
    RTK_API_UNLOCK();
#else
    rtk_ponmac_polarity_t polarityRtk;

    if (NULL == RT_MAPPER->ponmac_opticalPolarity_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_opticalPolarity_get(&polarityRtk);
    RTK_API_UNLOCK();

    switch(polarityRtk)
    {
        case PONMAC_POLARITY_HIGH:
            *pPolarity = RT_PONMISC_POLARITY_REVERSE_OFF;
        break;
        case PONMAC_POLARITY_LOW:
            *pPolarity = RT_PONMISC_POLARITY_REVERSE_ON;
        break;
        default:
            return RT_ERR_OUT_OF_RANGE;
        break;
    }
#endif

    return ret;
}


/* Function Name:
*      rt_ponmisc_burstPolarityReverse_set
* Description:
*      Set the status of PON burst polarity reverse
* Input:
*      polarity        - the burst polarity reverse status
* Output:
*      none
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
int32
rt_ponmisc_burstPolarityReverse_set(rt_ponmisc_polarity_t polarity)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_burstPolarityReverse_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_burstPolarityReverse_set(polarity);
    RTK_API_UNLOCK();
#else
    rtk_ponmac_polarity_t polarityRtk;

    if (NULL == RT_MAPPER->ponmac_opticalPolarity_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    switch(polarity)
    {
        case RT_PONMISC_POLARITY_REVERSE_OFF:
            polarityRtk = PONMAC_POLARITY_HIGH;
        break;
        case RT_PONMISC_POLARITY_REVERSE_ON:
            polarityRtk = PONMAC_POLARITY_LOW;
        break;
        default:
            return RT_ERR_OUT_OF_RANGE;
        break;
    }

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_opticalPolarity_set(polarityRtk);
    RTK_API_UNLOCK();
#endif

    return ret;
}

/* Function Name:
*      rt_ponmisc_rxlosPolarityReverse_get
* Description:
*      Get the status of PON rx_los polarity reverse
* Input:
*      none
* Output:
*      pPolarity       - pointer of rx_los polarity reverse status
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
rt_ponmisc_rxlosPolarityReverse_get(rt_ponmisc_polarity_t *pPolarity)
{
    int32 ret = RT_ERR_OK;
    
    RT_PARAM_CHK((NULL == pPolarity), RT_ERR_NULL_POINTER);

#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_rxlosPolarityReverse_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_rxlosPolarityReverse_get(pPolarity);
    RTK_API_UNLOCK();
#else
    ret = RT_ERR_FEATURE_NOT_SUPPORTED;
#endif

    return ret;
}


/* Function Name:
*      rt_ponmisc_rxlosPolarityReverse_set
* Description:
*      Set the status of PON rx_los polarity reverse
* Input:
*      polarity        - the rx_los polarity reverse status
* Output:
*      none
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
int32
rt_ponmisc_rxlosPolarityReverse_set(rt_ponmisc_polarity_t polarity)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_rxlosPolarityReverse_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_rxlosPolarityReverse_set(polarity);
    RTK_API_UNLOCK();
#else
    ret = RT_ERR_FEATURE_NOT_SUPPORTED;
#endif

    return ret;
}

/* Function Name:
*      rt_ponmisc_forceLaserState_get
* Description:
*      Get Force Laser status
* Input:
*      none
* Output:
*      pStatus       - pointer of Force Laser status
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
rt_ponmisc_forceLaserState_get(rt_ponmisc_laser_status_t *pStatus)
{
    int32 ret = RT_ERR_OK;
    
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_forceLaserState_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_forceLaserState_get(pStatus);
    RTK_API_UNLOCK();
#else
    rtk_ponmac_mode_t mode;
    rtk_gpon_laser_status_t laserStatusGpon;
    rtk_epon_laser_status_t laserStatusEpon;

    if (NULL == RT_MAPPER->ponmac_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_mode_get(&mode);
    RTK_API_UNLOCK();

    if(ret == RT_ERR_OK)
    {
        if(mode == PONMAC_MODE_GPON)
        {  /* gpon */
            if (NULL == RT_MAPPER->gpon_forceLaser_get)
                return RT_ERR_DRIVER_NOT_FOUND;

            RTK_API_LOCK();
            ret = RT_MAPPER->gpon_forceLaser_get(&laserStatusGpon);
            RTK_API_UNLOCK();

            switch(laserStatusGpon)
            {
                case RTK_GPON_LASER_STATUS_NORMAL:
                    *pStatus =  RT_PONMISC_LASER_STATUS_NORMAL;
                break;
                case RTK_GPON_LASER_STATUS_FORCE_ON:
                    *pStatus =  RT_PONMISC_LASER_STATUS_FORCE_ON;
                break;
                default: 
                    return RT_ERR_OUT_OF_RANGE;
                break;
            }

        }
        else if(mode ==PONMAC_MODE_EPON)
        {  /* epon */
            if (NULL == RT_MAPPER->epon_forceLaserState_get)
                return RT_ERR_DRIVER_NOT_FOUND;
            RTK_API_LOCK();
            ret = RT_MAPPER->epon_forceLaserState_get(&laserStatusEpon);
            RTK_API_UNLOCK();

            switch(laserStatusEpon)
            {
                case RTK_EPON_LASER_STATUS_NORMAL:
                    *pStatus =  RT_PONMISC_LASER_STATUS_NORMAL;
                break;
                case RTK_EPON_LASER_STATUS_FORCE_ON:
                    *pStatus =  RT_PONMISC_LASER_STATUS_FORCE_ON;
                break;
                default: 
                    return RT_ERR_OUT_OF_RANGE;
                break;
            }

        }
        else
             return RT_ERR_FAILED;
    }
#endif

    return ret;
}


/* Function Name:
*      rt_ponmisc_forceLaserState_set
* Description:
*      Set Force Laser status
* Input:
*      status       - Force Laser status
* Output:
*      none
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
int32
rt_ponmisc_forceLaserState_set(rt_ponmisc_laser_status_t status)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_forceLaserState_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_forceLaserState_set(status);
    RTK_API_UNLOCK();
#else
    rtk_ponmac_mode_t mode;
    rtk_gpon_laser_status_t laserStatusGpon;
    rtk_epon_laser_status_t laserStatusEpon;
		
    if (NULL == RT_MAPPER->ponmac_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_mode_get(&mode);
    RTK_API_UNLOCK();

    if(ret == RT_ERR_OK)
    {

        if(mode == PONMAC_MODE_GPON)
        {  /* gpon */
            if (NULL == RT_MAPPER->gpon_forceLaser_set)
                return RT_ERR_DRIVER_NOT_FOUND;

            switch(status)
            {
                case RT_PONMISC_LASER_STATUS_NORMAL:
                    laserStatusGpon = RTK_GPON_LASER_STATUS_NORMAL;
                break;
                case RT_PONMISC_LASER_STATUS_FORCE_ON:
                    laserStatusGpon = RTK_GPON_LASER_STATUS_FORCE_ON;
                break;
                default: 
                    return RT_ERR_OUT_OF_RANGE;
                break;
            }

            RTK_API_LOCK();
            ret = RT_MAPPER->gpon_forceLaser_set(laserStatusGpon);
            RTK_API_UNLOCK();
        }
        else if(mode ==PONMAC_MODE_EPON)
        {  /* epon */
            if (NULL == RT_MAPPER->epon_forceLaserState_set)
                return RT_ERR_DRIVER_NOT_FOUND;

            switch(status)
            {
                case RT_PONMISC_LASER_STATUS_NORMAL:
                    laserStatusEpon = RTK_EPON_LASER_STATUS_NORMAL;
                break;
                case RT_PONMISC_LASER_STATUS_FORCE_ON:
                    laserStatusEpon = RTK_EPON_LASER_STATUS_FORCE_ON;
                break;
                default: 
                    return RT_ERR_OUT_OF_RANGE;
                break;
            }

            RTK_API_LOCK();
            ret = RT_MAPPER->epon_forceLaserState_set(laserStatusEpon);
            RTK_API_UNLOCK();

        }
        else
             return RT_ERR_FAILED;

    }
	
#endif

    return ret;
}


/* Function Name:
*      rt_ponmisc_forcePRBS_get
* Description:
*      Get the PRBS config
* Input:
*      none
* Output:
*      pPrbsCfg       - pointer of PRBS config
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
rt_ponmisc_forcePRBS_get(rt_ponmisc_prbs_t *pPrbsCfg)
{
    int32 ret = RT_ERR_OK;
    
    RT_PARAM_CHK((NULL == pPrbsCfg), RT_ERR_NULL_POINTER);

#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_forcePRBS_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_forcePRBS_get(pPrbsCfg);
    RTK_API_UNLOCK();
#else
    rtk_gpon_prbs_t prbsCfgGpon;
    rtk_epon_prbs_t prbsCfgEpon;
    rtk_ponmac_mode_t mode;

    if (NULL == RT_MAPPER->ponmac_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_mode_get(&mode);
    RTK_API_UNLOCK();

    if(ret == RT_ERR_OK)
    {
        if(mode == PONMAC_MODE_GPON)
        {  /* gpon */
            if (NULL == RT_MAPPER->gpon_forcePRBS_get)
                return RT_ERR_DRIVER_NOT_FOUND;
        
            RTK_API_LOCK();
            ret = RT_MAPPER->gpon_forcePRBS_get(&prbsCfgGpon);
            RTK_API_UNLOCK();

            if(ret == RT_ERR_OK)
            {
                switch(prbsCfgGpon)
                {
                    case RTK_GPON_PRBS_OFF:
                        *pPrbsCfg = RT_PONMISC_PRBS_OFF;
                    break;
                    case RTK_GPON_PRBS_31:
                        *pPrbsCfg = RT_PONMISC_PRBS_31;
                    break;
                    case RTK_GPON_PRBS_23:
                        *pPrbsCfg = RT_PONMISC_PRBS_23;
                    break;
                    case RTK_GPON_PRBS_15:
                        *pPrbsCfg = RT_PONMISC_PRBS_15;
                    break;
                    case RTK_GPON_PRBS_7:
                        *pPrbsCfg = RT_PONMISC_PRBS_7;
                    break;
                    default:
                        ret = RT_ERR_OUT_OF_RANGE;
                        break;
                }
            }

        }
        else if(mode ==PONMAC_MODE_EPON)
        {  /* epon */
            if (NULL == RT_MAPPER->epon_forcePRBS_get)
                return RT_ERR_DRIVER_NOT_FOUND;

            RTK_API_LOCK();
            ret = RT_MAPPER->epon_forcePRBS_get(&prbsCfgEpon);
            RTK_API_UNLOCK();

            if(ret == RT_ERR_OK)
            {
                switch(prbsCfgEpon)
                {
                    case RTK_EPON_PRBS_OFF:
                        *pPrbsCfg = RT_PONMISC_PRBS_OFF;
                    break;
                    case RTK_EPON_PRBS_31:
                        *pPrbsCfg = RT_PONMISC_PRBS_31;
                    break;
                    case RTK_EPON_PRBS_23:
                        *pPrbsCfg = RT_PONMISC_PRBS_23;
                    break;
                    case RTK_EPON_PRBS_15:
                        *pPrbsCfg = RT_PONMISC_PRBS_15;
                    break;
                    case RTK_EPON_PRBS_11:
                        *pPrbsCfg = RT_PONMISC_PRBS_11;
                    break;
                    case RTK_EPON_PRBS_9:
                        *pPrbsCfg = RT_PONMISC_PRBS_9;
                    break;
                    case RTK_EPON_PRBS_7:
                        *pPrbsCfg = RT_PONMISC_PRBS_7;
                    break;
                    case RTK_EPON_PRBS_3:
                        *pPrbsCfg = RT_PONMISC_PRBS_3;
                    break;
                    default:
                        ret = RT_ERR_OUT_OF_RANGE;
                        break;
                }
            }

        }
        else
             return RT_ERR_FAILED;

    }
#endif

    return ret;
}


/* Function Name:
*      rt_ponmisc_forcePRBS_set
* Description:
*      Set the PRBS config
* Input:
*      prbsCfg       - PRBS config
* Output:
*      none
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
int32
rt_ponmisc_forcePRBS_set(rt_ponmisc_prbs_t prbsCfg)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_forcePRBS_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_forcePRBS_set(prbsCfg);
    RTK_API_UNLOCK();
#else
    rtk_gpon_prbs_t prbsCfgGpon;
    rtk_epon_prbs_t prbsCfgEpon;
    rtk_ponmac_mode_t mode;

    if (NULL == RT_MAPPER->ponmac_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_mode_get(&mode);
    RTK_API_UNLOCK();

    if(ret == RT_ERR_OK)
    {
        if(mode == PONMAC_MODE_GPON)
        {  /* gpon */
           if (NULL == RT_MAPPER->gpon_forcePRBS_set)
               return RT_ERR_DRIVER_NOT_FOUND;
       
            if(ret == RT_ERR_OK)
            {
                switch(prbsCfg)
                {
                    case RT_PONMISC_PRBS_OFF:
                        prbsCfgGpon = RTK_GPON_PRBS_OFF;
                    break;
                    case RT_PONMISC_PRBS_31:
                        prbsCfgGpon = RTK_GPON_PRBS_31;
                    break;
                    case RT_PONMISC_PRBS_23:
                        prbsCfgGpon = RTK_GPON_PRBS_23;
                    break;
                    case RT_PONMISC_PRBS_15:
                        prbsCfgGpon = RTK_GPON_PRBS_15;
                    break;
                    case RT_PONMISC_PRBS_7:
                        prbsCfgGpon = RTK_GPON_PRBS_7;
                    break;
                    default:
                        return RT_ERR_OUT_OF_RANGE;
                        break;
                }
            }

           RTK_API_LOCK();
           ret = RT_MAPPER->gpon_forcePRBS_set(prbsCfgGpon);
           RTK_API_UNLOCK();
        }
        else if(mode ==PONMAC_MODE_EPON)
        {  /* epon */
            if (NULL == RT_MAPPER->epon_forcePRBS_set)
                return RT_ERR_DRIVER_NOT_FOUND;

            if(ret == RT_ERR_OK)
            {
                switch(prbsCfg)
                {
                    case RT_PONMISC_PRBS_OFF:
                        prbsCfgEpon = RTK_EPON_PRBS_OFF;
                    break;
                    case RT_PONMISC_PRBS_31:
                        prbsCfgEpon = RTK_EPON_PRBS_31;
                    break;
                    case RT_PONMISC_PRBS_23:
                        prbsCfgEpon = RTK_EPON_PRBS_23;
                    break;
                    case RT_PONMISC_PRBS_15:
                        prbsCfgEpon = RTK_EPON_PRBS_15;
                    break;
                    case RT_PONMISC_PRBS_11:
                        prbsCfgEpon = RTK_EPON_PRBS_11;
                    break;
                    case RT_PONMISC_PRBS_9:
                        prbsCfgEpon = RTK_EPON_PRBS_9;
                    break;
                    case RT_PONMISC_PRBS_7:
                        prbsCfgEpon = RTK_EPON_PRBS_7;
                    break;
                    case RT_PONMISC_PRBS_3:
                        prbsCfgEpon = RTK_EPON_PRBS_3;
                    break;
                    default:
                        return RT_ERR_OUT_OF_RANGE;
                        break;
                }
            }

            RTK_API_LOCK();
            ret = RT_MAPPER->epon_forcePRBS_set(prbsCfgEpon);
            RTK_API_UNLOCK();

        }
        else
             return RT_ERR_FAILED;

    }
#endif

    return ret;
}

/* Function Name:
*      rt_ponmisc_forcePRBS_rx_get
* Description:
*      Get the PRBS RX config
* Input:
*      none
* Output:
*      pPrbsCfg       - pointer of PRBS RX config
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
rt_ponmisc_forcePRBS_rx_get(rt_ponmisc_prbs_t *pPrbsCfg)
{
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((NULL == pPrbsCfg), RT_ERR_NULL_POINTER);

#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_forcePRBS_rx_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_forcePRBS_rx_get(pPrbsCfg);
    RTK_API_UNLOCK();
#else
    rtk_gpon_prbs_t prbsCfgGpon;
    rtk_epon_prbs_t prbsCfgEpon;
    rtk_ponmac_mode_t mode;

    if (NULL == RT_MAPPER->ponmac_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_mode_get(&mode);
    RTK_API_UNLOCK();

	if(ret == RT_ERR_OK)
    {
        if(mode == PONMAC_MODE_GPON)
        {  /* gpon */
           if (NULL == RT_MAPPER->gpon_forcePRBS_rx_get)
               return RT_ERR_DRIVER_NOT_FOUND;

           RTK_API_LOCK();
           ret = RT_MAPPER->gpon_forcePRBS_rx_get(&prbsCfgGpon);
           RTK_API_UNLOCK();
		          
            if(ret == RT_ERR_OK)
            {
                switch(prbsCfgGpon)
                {
                    case RTK_GPON_PRBS_OFF:
                        *pPrbsCfg = RT_PONMISC_PRBS_OFF;
                    break;
                    case RTK_GPON_PRBS_31:
                        *pPrbsCfg = RT_PONMISC_PRBS_31;
                    break;
                    case RTK_GPON_PRBS_23:
                        *pPrbsCfg = RT_PONMISC_PRBS_23;
                    break;
                    case RTK_GPON_PRBS_15:
                        *pPrbsCfg = RT_PONMISC_PRBS_15;
                    break;
                    case RTK_GPON_PRBS_7:
                        *pPrbsCfg = RT_PONMISC_PRBS_7;
                    break;
                    default:
                        return RT_ERR_OUT_OF_RANGE;
                        break;
                }
            }
        }
        else if(mode ==PONMAC_MODE_EPON)
        {  /* epon */
            if (NULL == RT_MAPPER->epon_forcePRBS_rx_get)
                return RT_ERR_DRIVER_NOT_FOUND;

            RTK_API_LOCK();
            ret = RT_MAPPER->epon_forcePRBS_rx_get(&prbsCfgEpon);
            RTK_API_UNLOCK();

            if(ret == RT_ERR_OK)
            {
                switch(prbsCfgEpon)
                {
                    case RTK_EPON_PRBS_OFF:
                        *pPrbsCfg = RT_PONMISC_PRBS_OFF;
                    break;
                    case RTK_EPON_PRBS_31:
                        *pPrbsCfg = RT_PONMISC_PRBS_31;
                    break;
                    case RTK_EPON_PRBS_23:
                        *pPrbsCfg = RT_PONMISC_PRBS_23;
                    break;
                    case RTK_EPON_PRBS_15:
                        *pPrbsCfg = RT_PONMISC_PRBS_15;
                    break;
                    case RTK_EPON_PRBS_11:
                        *pPrbsCfg = RT_PONMISC_PRBS_11;
                    break;
                    case RTK_EPON_PRBS_9:
                        *pPrbsCfg = RT_PONMISC_PRBS_9;
                    break;
                    case RTK_EPON_PRBS_7:
                        *pPrbsCfg = RT_PONMISC_PRBS_7;
                    break;
                    case RTK_EPON_PRBS_3:
                        *pPrbsCfg = RT_PONMISC_PRBS_3;
                    break;
                    default:
                        return RT_ERR_OUT_OF_RANGE;
                        break;
                }
            }
        }
        else
             return RT_ERR_FAILED;
    }
#endif

    return ret;
}


/* Function Name:
*      rt_ponmisc_forcePRBS_rx_set
* Description:
*      Set the PRBS RX config
* Input:
*      prbsCfg       - PRBS RX config
* Output:
*      none
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
int32
rt_ponmisc_forcePRBS_rx_set(rt_ponmisc_prbs_t prbsCfg)
{
    int32   ret = RT_ERR_OK;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_forcePRBS_rx_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_forcePRBS_rx_set(prbsCfg);
    RTK_API_UNLOCK();
#else
    rtk_gpon_prbs_t prbsCfgGpon;
    rtk_epon_prbs_t prbsCfgEpon;
    rtk_ponmac_mode_t mode;

    if (NULL == RT_MAPPER->ponmac_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_mode_get(&mode);
    RTK_API_UNLOCK();

	if(ret == RT_ERR_OK)
    {
        if(mode == PONMAC_MODE_GPON)
        {  /* gpon */
           if (NULL == RT_MAPPER->gpon_forcePRBS_rx_set)
               return RT_ERR_DRIVER_NOT_FOUND;
       
            switch(prbsCfg)
            {
                case RT_PONMISC_PRBS_OFF:
                    prbsCfgGpon = RTK_GPON_PRBS_OFF;
                break;
                case RT_PONMISC_PRBS_31:
                    prbsCfgGpon = RTK_GPON_PRBS_31;
                break;
                case RT_PONMISC_PRBS_23:
                    prbsCfgGpon = RTK_GPON_PRBS_23;
                break;
                case RT_PONMISC_PRBS_15:
                    prbsCfgGpon = RTK_GPON_PRBS_15;
                break;
                case RT_PONMISC_PRBS_7:
                    prbsCfgGpon = RTK_GPON_PRBS_7;
                break;
                default:
                    return RT_ERR_OUT_OF_RANGE;
                break;
            }

           RTK_API_LOCK();
           ret = RT_MAPPER->gpon_forcePRBS_rx_set(prbsCfgGpon);
           RTK_API_UNLOCK();
        }
        else if(mode ==PONMAC_MODE_EPON)
        {  /* epon */
            if (NULL == RT_MAPPER->epon_forcePRBS_rx_set)
                return RT_ERR_DRIVER_NOT_FOUND;

            switch(prbsCfg)
            {
                case RT_PONMISC_PRBS_OFF:
                    prbsCfgEpon = RTK_EPON_PRBS_OFF;
                break;
                case RT_PONMISC_PRBS_31:
                    prbsCfgEpon = RTK_EPON_PRBS_31;
                break;
                case RT_PONMISC_PRBS_23:
                    prbsCfgEpon = RTK_EPON_PRBS_23;
                break;
                case RT_PONMISC_PRBS_15:
                    prbsCfgEpon = RTK_EPON_PRBS_15;
                break;
                case RT_PONMISC_PRBS_11:
                    prbsCfgEpon = RTK_EPON_PRBS_11;
                break;
                case RT_PONMISC_PRBS_9:
                    prbsCfgEpon = RTK_EPON_PRBS_9;
                break;
                case RT_PONMISC_PRBS_7:
                    prbsCfgEpon = RTK_EPON_PRBS_7;
                break;
                case RT_PONMISC_PRBS_3:
                default:
                    return RT_ERR_OUT_OF_RANGE;
                break;
            }

            RTK_API_LOCK();
            ret = RT_MAPPER->epon_forcePRBS_rx_set(prbsCfgEpon);
            RTK_API_UNLOCK();
        }
        else
             return RT_ERR_FAILED;
    }
#endif

    return ret;
}

/* Function Name:
*      rt_ponmisc_prbsRxStatus_get
* Description:
*      Get the PRBS RX Status
* Input:
*      none
* Output:
*      pPrbsRxStatus       - pointer of PRBS RX Status
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
rt_ponmisc_prbsRxStatus_get(rt_ponmisc_prbsRxStatus_t *pPrbsRxStatus)
{
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((NULL == pPrbsRxStatus), RT_ERR_NULL_POINTER);

#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_prbsRxStatus_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_prbsRxStatus_get(pPrbsRxStatus);
    RTK_API_UNLOCK();
#else
    rtk_gpon_prbsRxStatus_t prbsRxStatusGpon;
    rtk_epon_prbsRxStatus_t prbsRxStatusEpon;
    rtk_ponmac_mode_t mode;

    if (NULL == RT_MAPPER->ponmac_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_mode_get(&mode);
    RTK_API_UNLOCK();

	if(ret == RT_ERR_OK)
    {
        if(mode == PONMAC_MODE_GPON)
        {  /* gpon */
           if (NULL == RT_MAPPER->gpon_prbsRxStatus_get)
               return RT_ERR_DRIVER_NOT_FOUND;

           RTK_API_LOCK();
           ret = RT_MAPPER->gpon_prbsRxStatus_get(&prbsRxStatusGpon);
           RTK_API_UNLOCK();
		          
            if(ret != RT_ERR_OK)
                return ret;
            
            pPrbsRxStatus->prbsRxState = prbsRxStatusGpon.prbsRxState;
            pPrbsRxStatus->prbsRxErrCnt = prbsRxStatusGpon.prbsRxErrCnt;
            pPrbsRxStatus->prbsRxErrRate = prbsRxStatusGpon.prbsRxErrRate;
        }
        else if(mode ==PONMAC_MODE_EPON)
        {  
            if (NULL == RT_MAPPER->epon_forcePRBS_rx_get)
                return RT_ERR_DRIVER_NOT_FOUND;

            RTK_API_LOCK();
            ret = RT_MAPPER->epon_prbsRxStatus_get(&prbsRxStatusEpon);
            RTK_API_UNLOCK();

            if(ret != RT_ERR_OK)
                return ret;

            pPrbsRxStatus->prbsRxState = prbsRxStatusEpon.prbsRxState;
            pPrbsRxStatus->prbsRxErrCnt = prbsRxStatusEpon.prbsRxErrCnt;
            pPrbsRxStatus->prbsRxErrRate = prbsRxStatusEpon.prbsRxErrRate;


        }
        else
             return RT_ERR_FAILED;
    }
#endif

    return ret;
}

/* Function Name:
*      rt_ponmisc_selfRogue_cfg_get
* Description:
*      Get the self rogue detect config
* Input:
*      none
* Output:
*      pCfg       - pointer of self rogue detect config
* Return:
*      RT_ERR_OK            - OK
*      RT_ERR_FAILED        - failure
*      RT_ERR_NULL_POINTER  - input parameter may be null pointer
*      RT_ERR_OUT_OF_RANGE  - value out of range
* Note:
*
*/
int32
rt_ponmisc_selfRogue_cfg_get(rtk_enable_t *pCfg)
{
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((NULL == pCfg), RT_ERR_NULL_POINTER);

#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_selfRogue_cfg_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_selfRogue_cfg_get(pCfg);
    RTK_API_UNLOCK();
#else
    ret = RT_ERR_DRIVER_NOT_FOUND;

    if (NULL == RT_MAPPER->ponmac_selfRogue_cfg_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_selfRogue_cfg_get(pCfg);
    RTK_API_UNLOCK();

#endif

    return ret;
}

/* Function Name:
*      rt_ponmisc_selfRogue_cfg_set
* Description:
*      Set the self rogue detect config
* Input:
*      cfg       - self rogue detect config
* Output:
*      none
* Return:
*      RT_ERR_OK           - OK
*      RT_ERR_FAILED       - failure
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
int32
rt_ponmisc_selfRogue_cfg_set(rtk_enable_t cfg)
{
    int32   ret = RT_ERR_OK;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_selfRogue_cfg_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_selfRogue_cfg_set(cfg);
    RTK_API_UNLOCK();
#else
    ret = RT_ERR_DRIVER_NOT_FOUND;

    if (NULL == RT_MAPPER->ponmac_selfRogue_cfg_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_selfRogue_cfg_set(cfg);
    RTK_API_UNLOCK();
#endif

    return ret;
}

/* Function Name:
*      rt_ponmisc_selfRogue_count_get
* Description:
*      Get the self rogue detect counter
* Input:
*      none
* Output:
*      pCnt       - pointer of self rogue detect counter
* Return:
*      RT_ERR_OK            - OK
*      RT_ERR_FAILED        - failure
*      RT_ERR_NULL_POINTER  - input parameter may be null pointer
* Note:
*
*/
int32
rt_ponmisc_selfRogue_count_get(rt_ponmisc_rogue_cnt_t *pCnt)
{
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((NULL == pCnt), RT_ERR_NULL_POINTER);

#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_selfRogue_count_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_selfRogue_count_get(pCnt);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->ponmac_selfRogue_count_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_selfRogue_count_get(pCnt);
    RTK_API_UNLOCK();
#endif

    return ret;
}

/* Function Name:
*      rt_ponmisc_selfRogue_count_clear
* Description:
*      Clear the self rogue detect counter
* Input:
*      none
* Output:
*      none
* Return:
*      RT_ERR_OK           - OK
*      RT_ERR_FAILED       - failure
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
int32
rt_ponmisc_selfRogue_count_clear(void)
{
    int32   ret = RT_ERR_OK;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_selfRogue_count_clear)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_selfRogue_count_clear();
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->ponmac_selfRogue_count_clear)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_selfRogue_count_clear();
    RTK_API_UNLOCK();
#endif

    return ret;
}

/* Function Name:
*      rt_ponmisc_dying_gasp_force_trigger
* Description:
*      force trigger dying gasp signal
* Input:
*      none
* Output:
*      none
* Return:
*      RT_ERR_OK           - OK
*      RT_ERR_FAILED       - failure
* Note:
*
*/
int32
rt_ponmisc_dying_gasp_force_trigger(void)
{
    int32   ret = RT_ERR_OK;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_dying_gasp_force_trigger)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_dying_gasp_force_trigger();
    RTK_API_UNLOCK();
#else
    ret = RT_ERR_DRIVER_NOT_FOUND;
#endif

    return ret;
}


/*
 * Include Files
 */

#include <common/debug/rt_log.h>
#include <scfg.h>
#include <dal/rtl8198f/dal_rtl8198f_rtl83xx.h>
#include <dal/rtl8198f/dal_rtl8226_status.h>

/*
 * luna rtk api Symbol Definition
 */

typedef enum rtk_wrapper_port_speed_e
{
    WRAPPER_PORT_SPEED_10M = 0,
    WRAPPER_PORT_SPEED_100M,
    WRAPPER_PORT_SPEED_1000M,
    WRAPPER_PORT_SPEED_500M,
    WRAPPER_PORT_SPEED_10G,
    WRAPPER_PORT_SPEED_2G5,
    WRAPPER_PORT_SPEED_5G,
    WRAPPER_PORT_SPEED_2G5LITE,
    WRAPPER_PORT_SPEED_5GLITE,
    WRAPPER_PORT_SPEED_END
} rtk_wrapper_port_speed_t;

typedef struct rtk_port_macAbility_s
{
    rtk_wrapper_port_speed_t        speed;
    rtk_port_duplex_t       duplex;
    rtk_enable_t                linkFib1g;
    rtk_port_linkStatus_t   linkStatus;
    rtk_enable_t                txFc;
    rtk_enable_t                rxFc;
    rtk_enable_t                nwayAbility;
    rtk_enable_t                masterMod;
    rtk_enable_t                nwayFault;
    rtk_enable_t                lpi_100m;
    rtk_enable_t                lpi_giga;
} rtk_port_macAbility_t;

/*
 * Symbol Definition
 */

#define CAERR_PORT_ID 0xFFFF

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_rtl8198f_stat_port_getAll
 * Description:
 *      Get all counters of one specified port in the specified device.
 * Input:
 *      port        - port id
 * Output:
 *      pPortCntrs - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Note:
 *      None
 */
int32
dal_rtl8198f_stat_port_getAll(rtk_port_t port, rtk_stat_port_cntr_t *pPortCntrs)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_boolean_t read_keep;
    rtk_83xx_stat_port_cntr_t port_cntrs;

    /* check Init status */
    RT_INIT_CHK(stat_83xx_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortCntrs), RT_ERR_NULL_POINTER);

    memset(&port_cntrs, 0, sizeof(rtk_83xx_stat_port_cntr_t));

	ret = dal_rtl8226_port_stat_get(port, pPortCntrs);
	if (ret == RT_ERR_OK)
		goto assign_stat;
	else
		port = dal_rtl8198f_lspid_mapping(port);

    if((ret = rtk_83xx_stat_port_getAll(port,&port_cntrs)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

assign_stat:
    memset(pPortCntrs, 0, sizeof(rtk_stat_port_cntr_t));

    pPortCntrs->ifInOctets                           = port_cntrs.ifInOctets;
    pPortCntrs->ifInUcastPkts                        = port_cntrs.ifInUcastPkts;        /* received unicast frames                                                   */
    pPortCntrs->ifInMulticastPkts                    = port_cntrs.ifInMulticastPkts;        /* received multicast frames                                                 */
    pPortCntrs->ifInBroadcastPkts                    = port_cntrs.ifInBroadcastPkts;        /* received broadcast frames                                                 */
    pPortCntrs->ifInDiscards                         = 0;
    pPortCntrs->ifOutOctets                          = port_cntrs.ifOutOctets;
    pPortCntrs->ifOutDiscards                        = port_cntrs.ifOutDiscards; 
    pPortCntrs->ifOutUcastPkts                       = port_cntrs.ifOutUcastPkts;        /* transmitted unicast frames                                                */
    pPortCntrs->ifOutMulticastPkts                   = port_cntrs.ifOutMulticastPkts;        /* transmitted multicast frames                                              */
    pPortCntrs->ifOutBrocastPkts                     = port_cntrs.ifOutBrocastPkts;        /* transmitted broadcast frames                                              */
    pPortCntrs->dot1dBasePortDelayExceededDiscards   = port_cntrs.dot1dBasePortDelayExceededDiscards;
    pPortCntrs->dot1dTpPortInDiscards                = port_cntrs.dot1dTpPortInDiscards;
    pPortCntrs->dot1dTpHcPortInDiscards              = 0;
    pPortCntrs->dot3InPauseFrames                    = port_cntrs.dot3InPauseFrames;
    pPortCntrs->dot3OutPauseFrames                   = port_cntrs.dot3OutPauseFrames;
    pPortCntrs->dot3OutPauseOnFrames                 = 0;
    pPortCntrs->dot3StatsAligmentErrors              = 0;
    pPortCntrs->dot3StatsFCSErrors                   = port_cntrs.dot3StatsFCSErrors;
    pPortCntrs->dot3StatsSingleCollisionFrames       = port_cntrs.dot3StatsSingleCollisionFrames;
    pPortCntrs->dot3StatsMultipleCollisionFrames     = port_cntrs.dot3StatsMultipleCollisionFrames;
    pPortCntrs->dot3StatsDeferredTransmissions       = port_cntrs.dot3StatsDeferredTransmissions;
    pPortCntrs->dot3StatsLateCollisions              = port_cntrs.dot3StatsLateCollisions;
    pPortCntrs->dot3StatsExcessiveCollisions         = port_cntrs.dot3StatsExcessiveCollisions;
    pPortCntrs->dot3StatsFrameTooLongs               = 0;
    pPortCntrs->dot3StatsSymbolErrors                = port_cntrs.dot3StatsSymbolErrors;
    pPortCntrs->dot3ControlInUnknownOpcodes          = port_cntrs.dot3ControlInUnknownOpcodes;
    pPortCntrs->etherStatsDropEvents                 = port_cntrs.etherStatsDropEvents;
    pPortCntrs->etherStatsOctets                     = port_cntrs.etherStatsOctets;
    pPortCntrs->etherStatsBcastPkts                  = port_cntrs.etherStatsBcastPkts;
    pPortCntrs->etherStatsMcastPkts                  = port_cntrs.etherStatsMcastPkts;
    pPortCntrs->etherStatsUndersizePkts              = port_cntrs.etherStatsUndersizePkts;
    pPortCntrs->etherStatsOversizePkts               = port_cntrs.etherStatsOversizePkts;
    pPortCntrs->etherStatsFragments                  = port_cntrs.etherStatsFragments;
    pPortCntrs->etherStatsJabbers                    = port_cntrs.etherStatsJabbers;
    pPortCntrs->etherStatsCollisions                 = port_cntrs.etherStatsCollisions;
    pPortCntrs->etherStatsCRCAlignErrors             = 0;
    pPortCntrs->etherStatsPkts64Octets               = port_cntrs.etherStatsPkts64Octets;
    pPortCntrs->etherStatsPkts65to127Octets          = port_cntrs.etherStatsPkts65to127Octets;
    pPortCntrs->etherStatsPkts128to255Octets         = port_cntrs.etherStatsPkts128to255Octets;
    pPortCntrs->etherStatsPkts256to511Octets         = port_cntrs.etherStatsPkts256to511Octets;
    pPortCntrs->etherStatsPkts512to1023Octets        = port_cntrs.etherStatsPkts512to1023Octets;
    pPortCntrs->etherStatsPkts1024to1518Octets       = port_cntrs.etherStatsPkts1024toMaxOctets;
    pPortCntrs->etherStatsTxOctets                   = 0;
    pPortCntrs->etherStatsTxUndersizePkts            = 0;
    pPortCntrs->etherStatsTxOversizePkts             = 0;
    pPortCntrs->etherStatsTxPkts64Octets             = 0;        /* transmitted frames with 64 bytes                                          */
    pPortCntrs->etherStatsTxPkts65to127Octets        = 0;    /* transmitted frames with 65 ~ 127 bytes                                    */
    pPortCntrs->etherStatsTxPkts128to255Octets       = 0;   /* transmitted frames with 128 ~ 255 bytes                                   */
    pPortCntrs->etherStatsTxPkts256to511Octets       = 0;   /* transmitted frames with 256 ~ 511 bytes                                   */
    pPortCntrs->etherStatsTxPkts512to1023Octets      = 0;  /* transmitted frames with 512 ~ 1023 bytes                                  */
    pPortCntrs->etherStatsTxPkts1024to1518Octets     = 0; /* transmitted frames with 1024 ~ 1518 bytes                                 */
    pPortCntrs->etherStatsTxPkts1519toMaxOctets      = 0;  /* transmitted frames of which length is equal to or greater than 1519 bytes */
    pPortCntrs->etherStatsTxBcastPkts                = 0;        /* transmitted broadcast frames                                              */
    pPortCntrs->etherStatsTxMcastPkts                = 0;        /* transmitted multicast frames                                              */
    pPortCntrs->etherStatsTxFragments                = 0;
    pPortCntrs->etherStatsTxJabbers                  = 0;
    pPortCntrs->etherStatsTxCRCAlignErrors           = 0;
    pPortCntrs->etherStatsRxUndersizePkts            = 0; /* received frames undersize frame without FCS err               */
    pPortCntrs->etherStatsRxUndersizeDropPkts        = 0;
    pPortCntrs->etherStatsRxOversizePkts             = 0;
    pPortCntrs->etherStatsRxPkts64Octets             = 0;        /* received frames with 64 bytes                                             */
    pPortCntrs->etherStatsRxPkts65to127Octets        = 0;    /* received frames with 65 ~ 127 bytes                                       */
    pPortCntrs->etherStatsRxPkts128to255Octets       = 0;   /* received frames with 128 ~ 255 bytes                                      */
    pPortCntrs->etherStatsRxPkts256to511Octets       = 0;   /* received frames with 256 ~ 511 bytes                                      */
    pPortCntrs->etherStatsRxPkts512to1023Octets      = 0;  /* received frames with 512 ~ 1023 bytes                                     */
    pPortCntrs->etherStatsRxPkts1024to1518Octets     = 0; /* received frames with 1024 ~ 1518 bytes                                    */
    pPortCntrs->etherStatsRxPkts1519toMaxOctets      = 0;  /* received frames of which length is equal to or greater than 1519 bytes    */
    pPortCntrs->inOampduPkts                         = port_cntrs.inOampduPkts;
    pPortCntrs->outOampduPkts                        = port_cntrs.outOampduPkts;

    return RT_ERR_OK;
}

/* Function Name:
  *      dal_rtl8198f_switch_maxPktLenByPort_get
  * Description:
  *      Get the max packet length setting of specific port
  * Input:
  *      port - speed type
  * Output:
  *      pLen - pointer to the max packet length
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_NULL_POINTER - input parameter may be null pointer
  *      RT_ERR_INPUT        - invalid enum speed type
  * Note:
  */
int32
dal_rtl8198f_switch_maxPktLenByPort_get(rtk_port_t port, uint32 *pLen)
{
    ca_status_t ret=CA_E_OK;
    ca_uint32_t size=0;
    rtk_uint32 cfgid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_83xx_init);

    /* parameter check */
    RT_PARAM_CHK((port == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = rtk_switch_portMaxPktLen_get(port,MAXPKTLEN_LINK_SPEED_GE,&cfgid))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    if((ret = rtk_switch_maxPktLenCfg_get(cfgid,&size))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    *pLen = size;

    return RT_ERR_OK;
} /*end of dal_rtl8198f_switch_maxPktLenByPort_get*/

/* Function Name:
  *      dal_rtl8198f_switch_maxPktLenByPort_set
  * Description:
  *      Set the max packet length of specific port
  * Input:
  *      port  - port
  *      len   - max packet length
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_INPUT   - invalid enum speed type
  * Note:
  */
int32
dal_rtl8198f_switch_maxPktLenByPort_set(rtk_port_t port, uint32 len)
{
    ca_status_t ret=CA_E_OK;
    ca_uint32_t size=0;
    rtk_uint32 cfgid=1;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_83xx_init);

    /* parameter check */
    RT_PARAM_CHK((port == CAERR_PORT_ID), RT_ERR_PORT_ID);
    
    /* function body */
    size = len;

    if((ret = rtk_switch_maxPktLenCfg_set(cfgid,size))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    if((ret = rtk_switch_portMaxPktLen_set(port,MAXPKTLEN_LINK_SPEED_GE,cfgid))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}/*end of dal_rtl8198f_switch_maxPktLenByPort_set*/

/* Function Name:
 *      dal_rtl8198f_port_link_get
 * Description:
 *      Get the link status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pStatus - pointer to the link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The link status of the port is as following:
 *      - LINKDOWN
 *      - LINKUP
 */
int32
dal_rtl8198f_port_link_get(rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    ca_status_t ret=CA_E_OK;
    rtk_port_speed_t speed;
    rtk_port_duplex_t duplex;
    rtk_port_linkStatus_t status;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_83xx_init);

    /* parameter check */
    RT_PARAM_CHK((port == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

	ret = dal_rtl8226_port_link_get(port, &status);
	if (ret == RT_ERR_OK)
		goto assign_status;
	else
		port = dal_rtl8198f_lspid_mapping(port);

    if((ret = rtk_port_phyStatus_get(port, &status, &speed, &duplex)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

assign_status:
    *pStatus = ((status == PORT_LINKUP) ? PORT_LINKUP : PORT_LINKDOWN);

    return RT_ERR_OK;
} /* end of dal_rtl8198f_port_link_get */

/* Function Name:
 *      dal_rtl8198f_port_phyAutoNegoAbility_get
 * Description:
 *      Get PHY auto negotiation ability of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pAbility - pointer to the PHY ability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl8198f_port_phyAutoNegoAbility_get(
    rtk_port_t              port,
    rtk_port_phy_ability_t  *pAbility)
{
    ca_status_t ret=CA_E_OK;
    rtk_83xx_port_phy_ability_t ability;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_83xx_init);

    /* parameter check */
    RT_PARAM_CHK((port == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    memset(pAbility, 0, sizeof(rtk_port_phy_ability_t));

    ret = dal_rtl8226_port_phy_an_ability_get(port, pAbility);
    if (ret == RT_ERR_OK) {
        return RT_ERR_OK;
    }
    else
        port = dal_rtl8198f_lspid_mapping(port);

    memset(&ability,0,sizeof(rtk_port_phy_ability_t));

    if((ret = rtk_83xx_port_phyAutoNegoAbility_get(port,&ability)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    pAbility->Half_10=ability.Half_10;
    pAbility->Full_10=ability.Full_10;
    pAbility->Half_100=ability.Half_100;
    pAbility->Full_100=ability.Full_100;
    pAbility->Full_1000=ability.Full_1000;
//    pAbility->Full_10000=0;
    pAbility->FC=ability.FC;
    pAbility->AsyFC=ability.AsyFC;

    return RT_ERR_OK;
} /* end of dal_rtl8198f_port_phyAutoNegoAbility_get */

/* Function Name:
 *      dal_rtl8198f_port_phyAutoNegoAbility_set
 * Description:
 *      Set PHY auto negotiation ability of the specific port
 * Input:
 *      port     - port id
 *      pAbility - pointer to the PHY ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      You can set these abilities no matter which mode PHY currently stays on
 */
int32
dal_rtl8198f_port_phyAutoNegoAbility_set(
    rtk_port_t              port,
    rtk_port_phy_ability_t  *pAbility)
{
    ca_status_t ret=CA_E_OK;
    rtk_83xx_port_phy_ability_t ability;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_83xx_init);

    /* parameter check */
    RT_PARAM_CHK((port == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    ret = dal_rtl8226_port_phy_an_ability_set(port, pAbility);
    if (ret == RT_ERR_OK) {
        return RT_ERR_OK;
    }
    else
        port = dal_rtl8198f_lspid_mapping(port);

    memset(&ability, 0, sizeof(rtk_83xx_port_phy_ability_t));
    ability.Half_10=pAbility->Half_10;
    ability.Full_10=pAbility->Full_10;
    ability.Half_100=pAbility->Half_100;
    ability.Full_100=pAbility->Full_100;
    ability.Full_1000=pAbility->Full_1000;
    ability.FC=pAbility->FC;
    ability.AsyFC=pAbility->AsyFC;

    if((ret = rtk_83xx_port_phyAutoNegoAbility_set(port,&ability)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl8198f_port_phyAutoNegoAbility_set */

/* Function Name:
 *      dal_rtl8198f_port_phyReg_get
 * Description:
 *      Get PHY register data of the specific port
 * Input:
 *      port  - port id
 *      page  - page id
 *      reg   - reg id
 * Output:
 *      pData - pointer to the PHY reg data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid page id
 *      RT_ERR_PHY_REG_ID   - invalid reg id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */

int32
dal_rtl8198f_port_phyReg_get(
    rtk_port_t          port,
    uint32              page,
    rtk_port_phy_reg_t  reg,
    uint32              *pData)
{
    ca_status_t ret=CA_E_OK;
    rtk_uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_83xx_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    if((ret = rtk_83xx_port_phyReg_get(port, reg, &data)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    *pData = data;

    return ret;
} /* end of dal_rtl8198f_port_phyReg_get */

/* Function Name:
 *      dal_rtl8198f_port_phyReg_set
 * Description:
 *      Set PHY register data of the specific port
 * Input:
 *      port - port id
 *      page - page id
 *      reg  - reg id
 *      data - reg data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID     - invalid port id
 *      RT_ERR_PHY_PAGE_ID - invalid page id
 *      RT_ERR_PHY_REG_ID  - invalid reg id
 * Note:
 *      None
 */
int32
dal_rtl8198f_port_phyReg_set(
    rtk_port_t          port,
    uint32              page,
    rtk_port_phy_reg_t  reg,
    uint32              data)
{
    ca_status_t ret=CA_E_OK;
    rtk_uint32 regdata;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_83xx_init);

    regdata = data;

    if((ret = rtk_83xx_port_phyReg_set(port, reg, regdata)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return ret;
} /* end of dal_rtl8198f_port_phyReg_set */

/* Function Name:
 *      dal_rtl8198f_port_macForceAbility_get
 * Description:
 *      Get MAC forece ability
 * Input:
 *      port - the ports for get ability
 *      pMacAbility - mac ability value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl8198f_port_macForceAbility_get(rtk_port_t port,rtk_port_macAbility_t *pMacAbility)
{
    ca_status_t ret=CA_E_OK;
    rtk_port_mac_ability_t ability;
    rtk_port_linkStatus_t linkstatus;
    rtk_port_speed_t speed;
    rtk_port_duplex_t duplex;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_83xx_init);

    memset(pMacAbility,0,sizeof(rtk_port_macAbility_t));

    if((ret = rtk_port_macForceLink_get(port, &ability)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = rtk_port_phyStatus_get(port, &linkstatus, &speed, &duplex)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    pMacAbility->speed = ability.speed;
    pMacAbility->duplex = ability.duplex;
    pMacAbility->linkStatus = linkstatus;
    pMacAbility->txFc = ability.txpause;
    pMacAbility->rxFc = ability.rxpause;

    return RT_ERR_OK;
} /* end of dal_rtl8198f_port_macForceAbility_set */

/* Function Name:
 *      dal_rtl8198f_port_macForceAbility_set
 * Description:
 *      Set MAC forece ability
 * Input:
 *      port - the ports for set ability
 *      macAbility - mac ability value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl8198f_port_macForceAbility_set(rtk_port_t port,rtk_port_macAbility_t macAbility)
{
    ca_status_t ret=CA_E_OK;
    rtk_port_mac_ability_t ability;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_83xx_init);

	memset(&ability,0,sizeof(rtk_port_mac_ability_t));

    /* parameter check */
    RT_PARAM_CHK((PORT_SPEED_END <= macAbility.speed), RT_ERR_INPUT);
    RT_PARAM_CHK((PORT_DUPLEX_END <= macAbility.duplex), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= macAbility.rxFc), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= macAbility.txFc), RT_ERR_INPUT);

    switch(macAbility.speed)
    {
        case PORT_SPEED_10M:
            ability.speed=PORT_SPEED_10M;
            break;
        case PORT_SPEED_100M:
            ability.speed=PORT_SPEED_100M;
            break;
        case PORT_SPEED_1000M:
            ability.speed=PORT_SPEED_1000M;
            break;
        default:
            return RT_ERR_FAILED;
    }

    ability.forcemode = 1;
    ability.duplex = macAbility.duplex;
    ability.rxpause = (macAbility.rxFc==ENABLED)? 1:0;
    ability.txpause = (macAbility.txFc==ENABLED)? 1:0;

    if (ret = rtk_port_macForceLink_set(port, &ability) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl8198f_port_macForceAbility_set */

/* Function Name:
 *      dal_rtl8198f_port_speedDuplex_get
 * Description:
 *      Get the negotiated port speed and duplex status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pSpeed  - pointer to the port speed
 *      pDuplex - pointer to the port duplex
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN - link down port status
 * Note:
 *      (1) The speed type of the port is as following:
 *          - PORT_SPEED_10M
 *          - PORT_SPEED_100M
 *          - PORT_SPEED_1000M
 *
 *      (2) The duplex mode of the port is as following:
 *          - HALF_DUPLEX
 *          - FULL_DUPLEX
 */
int32
dal_rtl8198f_port_speedDuplex_get(
    rtk_port_t        port,
    rtk_port_speed_t  *pSpeed,
    rtk_port_duplex_t *pDuplex)
{
    ca_status_t ret=CA_E_OK;
    rtk_port_mac_ability_t ability;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_83xx_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);

	memset(&ability,0,sizeof(rtk_port_mac_ability_t));

	ret = dal_rtl8226_port_mac_get(port, &ability);
	if (ret == RT_ERR_OK)
		goto assign_speed;
	else
		port = dal_rtl8198f_lspid_mapping(port);

    if((ret = rtk_port_macStatus_get(port,&ability)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

assign_speed:
    switch(ability.speed)
    {
        case PORT_SPEED_10M:
            *pSpeed=PORT_SPEED_10M;
            break;
        case PORT_SPEED_100M:
            *pSpeed=PORT_SPEED_100M;
            break;
        case PORT_SPEED_1000M:
            *pSpeed=PORT_SPEED_1000M;
            break;
		case PORT_SPEED_2500M:
			*pSpeed=PORT_SPEED_2500M;
			break;
        default:
            return RT_ERR_FAILED;
    }

    *pDuplex = ((1 == ability.duplex) ? PORT_FULL_DUPLEX : PORT_HALF_DUPLEX);
    return RT_ERR_OK;
} /* end of dal_rtl8198f_port_speedDuplex_get */

/* Function Name:
 *      dal_rtl8198f_port_flowctrl_get
 * Description:
 *      Get the negotiated flow control status of the specific port
 * Input:
 *      port      - port id
 * Output:
 *      pTxStatus - pointer to the negotiation result of the Tx flow control
 *      pRxStatus - pointer to the negotiation result of the Rx flow control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN - link down port status
 * Note:
 *      None
 */
int32
dal_rtl8198f_port_flowctrl_get(
    rtk_port_t  port,
    uint32      *pTxStatus,
    uint32      *pRxStatus)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
	rtk_port_mac_ability_t ability;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_83xx_init);

    /* parameter check */
    RT_PARAM_CHK((port == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pTxStatus), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRxStatus), RT_ERR_NULL_POINTER);

	memset(&ability, 0, sizeof(rtk_port_mac_ability_t));

	ret = dal_rtl8226_port_mac_get(port, &ability);
	if (ret == RT_ERR_OK)
		goto assign_trx;
	else
		port = dal_rtl8198f_lspid_mapping(port);

    if((ret = rtk_port_macStatus_get(port,&ability)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

assign_trx:
    *pTxStatus=ability.txpause;
    *pRxStatus=ability.rxpause;

    return RT_ERR_OK;
} /* end of dal_rtl8198f_port_flowctrl_get */

/* Function Name:
 *      dal_rtl8198f_port_adminEnable_get
 * Description:
 *      Get port admin status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to the port admin status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl8198f_port_adminEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
	int32 ret = RT_ERR_OK;
    rtk_uint32 regdata;
	rtk_enable_t enable;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_83xx_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

	ret = dal_rtl8226_port_power_get(port, &enable);
	if (ret == RT_ERR_OK)
		goto assign_speed;
	else
		port = dal_rtl8198f_lspid_mapping(port);

    if((ret = rtk_83xx_port_phyReg_get(port, 0, &regdata)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

	regdata &= (1<<11);

    if(regdata == 0)
        enable = ENABLED;
    else
        enable = DISABLED;

assign_speed:
    if(enable == ENABLED)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;
	

    return RT_ERR_OK;
} /* end of dal_rtl8198f_port_adminEnable_get */

/* Function Name:
 *      dal_rtl8198f_port_adminEnable_set
 * Description:
 *      Set port admin status of the specific port
 * Input:
 *      port    - port id
 *      enable  - port admin status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
dal_rtl8198f_port_adminEnable_set(rtk_port_t port, rtk_enable_t enable)
{
	int32 ret = RT_ERR_OK;
    rtk_uint32 regdata;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_83xx_init);

	ret = dal_rtl8226_port_power_set(port, enable);
	if (ret == RT_ERR_OK)
		return RT_ERR_OK;
	else
		port = dal_rtl8198f_lspid_mapping(port);
	
    if((ret = rtk_83xx_port_phyReg_get(port, 0, &regdata)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

	if(enable == ENABLED)
		regdata &= ~(1<<11); //Power down
	else
		regdata |= (1<<11); //Power on

    if((ret = rtk_83xx_port_phyReg_set(port, 0, regdata)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl8198f_port_adminEnable_set */

#if defined(CONFIG_RTL_83XX_SUPPORT) && !defined(CONFIG_RTL_G3LITE_QOS_SUPPORT)
static uint32 qos_init = INIT_NOT_COMPLETED;
	
int32
dal_rtl8198f_qos_init(void)
{
	int32 retVal;

	qos_init = INIT_COMPLETED;
	if((retVal = rtk_83xx_qos_init(8)) != RT_ERR_OK)
		return retVal;
		
	return RT_ERR_OK;
}

int32
dal_rtl8198f_qos_schedulingQueue_get(rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
    int32 ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQweights), RT_ERR_NULL_POINTER);

    if((ret = rtk_83xx_qos_schedulingQueue_get(port, pQweights)) != CA_E_OK){
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32
dal_rtl8198f_qos_schedulingQueue_set(rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
	int32 ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQweights), RT_ERR_NULL_POINTER);

    if((ret = rtk_83xx_qos_schedulingQueue_set(port, pQweights)) != CA_E_OK){
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32
dal_rtl8198f_qos_priSelGroup_get(uint32 grpIdx, rtk_qos_priSelWeight_t *pWeightOfPriSel)
{
	int32 ret = RT_ERR_OK;
	rtk_priority_select_t priSelWeightFor8367;
		
	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(qos_init);
	
	/* parameter check */
    RT_PARAM_CHK((PRIDECTBL_END <= grpIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pWeightOfPriSel), RT_ERR_NULL_POINTER);


	memset(&priSelWeightFor8367, 0, sizeof(rtk_priority_select_t));
	memset(pWeightOfPriSel, 0, sizeof(rtk_qos_priSelWeight_t));

	if((ret = rtk_83xx_qos_priSelGroup_get(grpIdx, &priSelWeightFor8367)) != CA_E_OK){
		RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
	}

	// here only passing parts of priority decison, for data struction inconsistent with diagshell
	pWeightOfPriSel->weight_of_portBased 		= priSelWeightFor8367.port_pri;
	pWeightOfPriSel->weight_of_dot1q 			= priSelWeightFor8367.dot1q_pri;
	pWeightOfPriSel->weight_of_dscp 			= priSelWeightFor8367.dscp_pri;
	pWeightOfPriSel->weight_of_acl 				= priSelWeightFor8367.acl_pri;
	pWeightOfPriSel->weight_of_svlanBased 		= priSelWeightFor8367.svlan_pri;
	pWeightOfPriSel->weight_of_vlanBased 		= priSelWeightFor8367.cvlan_pri;
	pWeightOfPriSel->weight_of_saBaed 			= priSelWeightFor8367.smac_pri;

	return RT_ERR_OK;
}

int32
dal_rtl8198f_qos_priSelGroup_set(uint32 grpIdx, rtk_qos_priSelWeight_t *pWeightOfPriSel)
{
	int32 ret = RT_ERR_OK;
	rtk_priority_select_t priSelWeightFor8367;
		
	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(qos_init);
	
	/* parameter check */
    RT_PARAM_CHK((PRIDECTBL_END <= grpIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pWeightOfPriSel), RT_ERR_NULL_POINTER);

	memset(&priSelWeightFor8367, 0, sizeof(rtk_priority_select_t));

	if((ret = rtk_83xx_qos_priSelGroup_get(grpIdx, &priSelWeightFor8367)) != CA_E_OK){
		RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
	}

	// here only setting parts of priority decison, for data struction inconsisten with diagshell
	priSelWeightFor8367.port_pri 		= pWeightOfPriSel->weight_of_portBased;
	priSelWeightFor8367.dot1q_pri		= pWeightOfPriSel->weight_of_dot1q;	
	priSelWeightFor8367.dscp_pri 		= pWeightOfPriSel->weight_of_dscp;
	priSelWeightFor8367.acl_pri			= pWeightOfPriSel->weight_of_acl;
	priSelWeightFor8367.svlan_pri		= pWeightOfPriSel->weight_of_svlanBased;
	priSelWeightFor8367.cvlan_pri		= pWeightOfPriSel->weight_of_vlanBased;
	priSelWeightFor8367.smac_pri		= pWeightOfPriSel->weight_of_saBaed;

	if((ret = rtk_83xx_qos_priSelGroup_set(grpIdx, &priSelWeightFor8367)) != CA_E_OK){
		RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
	}

	return RT_ERR_OK;
}

int32
dal_rtl8198f_qos_priMap_get(uint32 group, rtk_qos_pri2queue_t *pPri2qid)
{
	int32 ret = RT_ERR_OK;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(qos_init);
	
	/* parameter check */
    RT_PARAM_CHK((RTK_MAX_NUM_OF_QUEUE < group) || (1 > group), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPri2qid), RT_ERR_NULL_POINTER);

	if((ret = rtk_83xx_qos_priMap_get(group, pPri2qid)) != CA_E_OK){
		RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
	}
	
	return RT_ERR_OK;
}

int32
dal_rtl8198f_qos_priMap_set(uint32 group, rtk_qos_pri2queue_t *pPri2qid)
{
	int32 ret = RT_ERR_OK;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(qos_init);
	
	/* parameter check */
    RT_PARAM_CHK((RTK_MAX_NUM_OF_QUEUE < group) || (1 > group), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPri2qid), RT_ERR_NULL_POINTER);

	if((ret = rtk_83xx_qos_priMap_set(group, pPri2qid)) != CA_E_OK){
		RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
	}
	
	return RT_ERR_OK;
}


int32
dal_rtl8198f_qos_1pPriRemap_get(uint32 grpIdx, uint32 dot1pPri, uint32 *pIntPri, uint32 *pDp)
{
	int32 ret = RT_ERR_OK;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(qos_init);
	
	/* parameter check */
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < dot1pPri), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pIntPri), RT_ERR_NULL_POINTER);

	if((ret = rtk_83xx_qos_1pPriRemap_get(dot1pPri, pIntPri)) != CA_E_OK){
		RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
	}
	
	return RT_ERR_OK;
}

int32
dal_rtl8198f_qos_1pPriRemap_set(uint32 grpIdx, uint32 dot1pPri, uint32 intPri, uint32 dp)
{
	int32 ret = RT_ERR_OK;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(qos_init);
	
	/* parameter check */
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < dot1pPri), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_PRIMAX < intPri), RT_ERR_INPUT);

	if((ret = rtk_83xx_qos_1pPriRemap_set(dot1pPri, intPri)) != CA_E_OK){
		RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
	}
	
	return RT_ERR_OK;
}

int32
dal_rtl8198f_qos_1pRemark_get(uint32 grpIdx, uint32 intPri, uint32 dp, uint32 *pDot1pPri)
{
	int32 ret = RT_ERR_OK;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(qos_init);
	
	/* parameter check */
    RT_PARAM_CHK((RTK_PRIMAX < intPri), RT_ERR_INPUT);
   	RT_PARAM_CHK((NULL == pDot1pPri), RT_ERR_NULL_POINTER);

	if((ret = rtk_83xx_qos_1pRemark_get(intPri, pDot1pPri)) != CA_E_OK){
		RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
	}

	return RT_ERR_OK;
}

int32
dal_rtl8198f_qos_1pRemark_set(uint32 grpIdx, uint32 intPri, uint32 dp, uint32 dot1pPri)
{
	int32 ret = RT_ERR_OK;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(qos_init);
	
	/* parameter check */
    RT_PARAM_CHK((RTK_PRIMAX < intPri), RT_ERR_INPUT);
	RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < dot1pPri), RT_ERR_INPUT);

	if((ret = rtk_83xx_qos_1pRemark_set(intPri, dot1pPri)) != CA_E_OK){
		RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
	}
	
	return RT_ERR_OK;
}

int32
dal_rtl8198f_qos_dscpRemark_get(uint32 grpIdx, uint32 intPri, uint32 dp, uint32 *pDscp)
{
	int32 ret = RT_ERR_OK;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(qos_init);
	
	/* parameter check */
    RT_PARAM_CHK((RTK_PRIMAX < intPri), RT_ERR_INPUT);
   	RT_PARAM_CHK((NULL == pDscp), RT_ERR_NULL_POINTER);

	if((ret = rtk_83xx_qos_dscpRemark_get(intPri, pDscp)) != CA_E_OK){
		RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
	}
	
	return RT_ERR_OK;
}

int32
dal_rtl8198f_qos_dscpRemark_set(uint32 grpIdx, uint32 intPri, uint32 dp, uint32 dscp)
{
	int32 ret = RT_ERR_OK;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(qos_init);
	
	/* parameter check */
    RT_PARAM_CHK((RTK_PRIMAX < intPri), RT_ERR_INPUT);
	RT_PARAM_CHK((RTK_VALUE_OF_DSCP_MAX < dscp), RT_ERR_INPUT);

	if((ret = rtk_83xx_qos_dscpRemark_set(intPri, dscp)) != CA_E_OK){
		RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return RT_ERR_FAILED;
	}

	return RT_ERR_OK;
}

int32
dal_rtl8198f_qos_1pRemarkEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
	int32 ret = RT_ERR_OK;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(qos_init);
	
	/* parameter check */
   	RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
	
	if((ret = rtk_83xx_qos_1pRemarkEnable_get(port, pEnable)) != CA_E_OK){
		RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
		return RT_ERR_FAILED;
	}

	return RT_ERR_OK;
}

int32
dal_rtl8198f_qos_1pRemarkEnable_set(rtk_port_t port, rtk_enable_t enable)
{
	int32 ret = RT_ERR_OK;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(qos_init);
	
	if((ret = rtk_83xx_qos_1pRemarkEnable_set(port, enable)) != CA_E_OK){
		RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
		return RT_ERR_FAILED;
	}
	
	return RT_ERR_OK;
}

int32
dal_rtl8198f_qos_dscpRemarkEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
	int32 ret = RT_ERR_OK;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(qos_init);
	
	/* parameter check */
	RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

	if((ret = rtk_83xx_qos_dscpRemarkEnable_get(port, pEnable)) != CA_E_OK){
		RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
		return RT_ERR_FAILED;
	}
	
	return RT_ERR_OK;
}


int32
dal_rtl8198f_qos_dscpRemarkEnable_set(rtk_port_t port, rtk_enable_t enable)
{
	int32 ret = RT_ERR_OK;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(qos_init);
	
	if((ret = rtk_83xx_qos_dscpRemarkEnable_set(port, enable)) != CA_E_OK){
		RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
		return RT_ERR_FAILED;
	}
	
	return RT_ERR_OK;
}
#endif

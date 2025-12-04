
typedef unsigned long long      rtk_uint64;
typedef long long               rtk_int64;
typedef unsigned int            rtk_uint32;
typedef int                     rtk_int32;
typedef unsigned short          rtk_uint16;
typedef short                   rtk_int16;
typedef unsigned char           rtk_uint8;
typedef char                    rtk_int8;

typedef struct rtk_83xx_stat_port_cntr_s
{
    rtk_uint64 ifInOctets;
    rtk_uint32 dot3StatsFCSErrors;
    rtk_uint32 dot3StatsSymbolErrors;
    rtk_uint32 dot3InPauseFrames;
    rtk_uint32 dot3ControlInUnknownOpcodes;
    rtk_uint32 etherStatsFragments;
    rtk_uint32 etherStatsJabbers;
    rtk_uint32 ifInUcastPkts;
    rtk_uint32 etherStatsDropEvents;
    rtk_uint64 etherStatsOctets;
    rtk_uint32 etherStatsUndersizePkts;
    rtk_uint32 etherStatsOversizePkts;
    rtk_uint32 etherStatsPkts64Octets;
    rtk_uint32 etherStatsPkts65to127Octets;
    rtk_uint32 etherStatsPkts128to255Octets;
    rtk_uint32 etherStatsPkts256to511Octets;
    rtk_uint32 etherStatsPkts512to1023Octets;
    rtk_uint32 etherStatsPkts1024toMaxOctets;
    rtk_uint32 etherStatsMcastPkts;
    rtk_uint32 etherStatsBcastPkts;
    rtk_uint64 ifOutOctets;
    rtk_uint32 dot3StatsSingleCollisionFrames;
    rtk_uint32 dot3StatsMultipleCollisionFrames;
    rtk_uint32 dot3StatsDeferredTransmissions;
    rtk_uint32 dot3StatsLateCollisions;
    rtk_uint32 etherStatsCollisions;
    rtk_uint32 dot3StatsExcessiveCollisions;
    rtk_uint32 dot3OutPauseFrames;
    rtk_uint32 dot1dBasePortDelayExceededDiscards;
    rtk_uint32 dot1dTpPortInDiscards;
    rtk_uint32 ifOutUcastPkts;
    rtk_uint32 ifOutMulticastPkts;
    rtk_uint32 ifOutBrocastPkts;
    rtk_uint32 outOampduPkts;
    rtk_uint32 inOampduPkts;
    rtk_uint32 pktgenPkts;
    rtk_uint32 inMldChecksumError;
    rtk_uint32 inIgmpChecksumError;
    rtk_uint32 inMldSpecificQuery;
    rtk_uint32 inMldGeneralQuery;
    rtk_uint32 inIgmpSpecificQuery;
    rtk_uint32 inIgmpGeneralQuery;
    rtk_uint32 inMldLeaves;
    rtk_uint32 inIgmpLeaves;
    rtk_uint32 inIgmpJoinsSuccess;
    rtk_uint32 inIgmpJoinsFail;
    rtk_uint32 inMldJoinsSuccess;
    rtk_uint32 inMldJoinsFail;
    rtk_uint32 inReportSuppressionDrop;
    rtk_uint32 inLeaveSuppressionDrop;
    rtk_uint32 outIgmpReports;
    rtk_uint32 outIgmpLeaves;
    rtk_uint32 outIgmpGeneralQuery;
    rtk_uint32 outIgmpSpecificQuery;
    rtk_uint32 outMldReports;
    rtk_uint32 outMldLeaves;
    rtk_uint32 outMldGeneralQuery;
    rtk_uint32 outMldSpecificQuery;
    rtk_uint32 inKnownMulticastPkts;
    rtk_uint32 ifInMulticastPkts;
    rtk_uint32 ifInBroadcastPkts;
    rtk_uint32 ifOutDiscards;
}rtk_83xx_stat_port_cntr_t;

typedef enum rtk_switch_maxPktLen_linkSpeed_e {
     MAXPKTLEN_LINK_SPEED_FE = 0,
     MAXPKTLEN_LINK_SPEED_GE,
     MAXPKTLEN_LINK_SPEED_END,
} rtk_switch_maxPktLen_linkSpeed_t;

typedef enum rtk_port_e
{
    UTP_PORT0 = 0,
    UTP_PORT1,
    UTP_PORT2,
    UTP_PORT3,
    UTP_PORT4,
    UTP_PORT5,
    UTP_PORT6,
    UTP_PORT7,

    EXT_PORT0 = 16,
    EXT_PORT1,
    EXT_PORT2,

    UNDEFINE_PORT = 30,
    RTK_PORT_MAX = 31
} rtk_port_t;

typedef enum rtk_port_linkStatus_e
{
    PORT_LINKDOWN = 0,
    PORT_LINKUP,
    PORT_LINKSTATUS_END
} rtk_port_linkStatus_t;

typedef enum rtk_port_speed_e
{
    PORT_SPEED_10M = 0,
    PORT_SPEED_100M,
    PORT_SPEED_1000M,
    PORT_SPEED_500M,
    PORT_SPEED_10G,
    PORT_SPEED_2500M,
    PORT_SPEED_END
} rtk_port_speed_t;

typedef enum rtk_port_duplex_e
{
    PORT_HALF_DUPLEX = 0,
    PORT_FULL_DUPLEX,
    PORT_DUPLEX_END
} rtk_port_duplex_t;

typedef struct rtk_83xx_port_phy_ability_s
{
    rtk_uint32    AutoNegotiation;  /*PHY register 0.12 setting for auto-negotiation process*/
    rtk_uint32    Half_10;          /*PHY register 4.5 setting for 10BASE-TX half duplex capable*/
    rtk_uint32    Full_10;          /*PHY register 4.6 setting for 10BASE-TX full duplex capable*/
    rtk_uint32    Half_100;         /*PHY register 4.7 setting for 100BASE-TX half duplex capable*/
    rtk_uint32    Full_100;         /*PHY register 4.8 setting for 100BASE-TX full duplex capable*/
    rtk_uint32    Full_1000;        /*PHY register 9.9 setting for 1000BASE-T full duplex capable*/
    rtk_uint32    FC;               /*PHY register 4.10 setting for flow control capability*/
    rtk_uint32    AsyFC;            /*PHY register 4.11 setting for  asymmetric flow control capability*/
} rtk_83xx_port_phy_ability_t;

typedef struct  rtk_port_mac_ability_s
{
    rtk_uint32 forcemode;
    rtk_uint32 speed;
    rtk_uint32 duplex;
    rtk_uint32 link;
    rtk_uint32 nway;
    rtk_uint32 txpause;
    rtk_uint32 rxpause;
}rtk_port_mac_ability_t;

typedef enum rtk_port_phy_reg_e
{
    PHY_REG_CONTROL             = 0,
    PHY_REG_STATUS,
    PHY_REG_IDENTIFIER_1,
    PHY_REG_IDENTIFIER_2,
    PHY_REG_AN_ADVERTISEMENT,
    PHY_REG_AN_LINKPARTNER,
    PHY_REG_1000_BASET_CONTROL  = 9,
    PHY_REG_1000_BASET_STATUS,
    PHY_REG_END                 = 32
} rtk_port_phy_reg_t;

typedef rtk_uint32  rtk_port_phy_data_t;     /* phy page  */

/* 
* Group type of storm control 
* 83XX use the same structure with rtk wrapper api 0~3
*/

typedef enum rtk_rate_storm_group_e
{
    STORM_GROUP_UNKNOWN_UNICAST = 0,
    STORM_GROUP_UNKNOWN_MULTICAST,
    STORM_GROUP_MULTICAST,
    STORM_GROUP_BROADCAST,
    STORM_GROUP_DHCP,
    STORM_GROUP_ARP,
    STORM_GROUP_IGMP_MLD,
    STORM_GROUP_END
} rtk_rate_storm_group_t;


#define RTK_MAX_NUM_OF_PRIORITY                     8
#define RTK_MAX_NUM_OF_QUEUE                        8
#define RTK_DOT1P_PRIORITY_MAX                      7
#define RTK_VALUE_OF_DSCP_MAX                       63
#define RTK_PRIMAX                                  7

typedef struct rtk_qos_priSelWeight_s
{
    uint32 weight_of_portBased;
    uint32 weight_of_dot1q;
    uint32 weight_of_dscp;
    uint32 weight_of_acl;
    uint32 weight_of_lutFwd;
    uint32 weight_of_saBaed;
    uint32 weight_of_vlanBased;
    uint32 weight_of_svlanBased;
    uint32 weight_of_l4Based;
} rtk_qos_priSelWeight_t;

typedef struct rtk_priority_select_s
{
    rtk_uint32 port_pri;
    rtk_uint32 dot1q_pri;
    rtk_uint32 acl_pri;
    rtk_uint32 dscp_pri;
    rtk_uint32 cvlan_pri;
    rtk_uint32 svlan_pri;
    rtk_uint32 dmac_pri;
    rtk_uint32 smac_pri;
} rtk_priority_select_t;

typedef enum rtk_qos_priDecTbl_e
{
    PRIDECTBL_IDX0 = 0,
    PRIDECTBL_IDX1,
    PRIDECTBL_END,
}rtk_qos_priDecTbl_t;

typedef struct rtk_qos_pri2queue_s
{
    rtk_uint32 pri2queue[RTK_MAX_NUM_OF_PRIORITY];
} rtk_qos_pri2queue_t;

typedef struct rtk_qos_queue_weights_s
{
    rtk_uint32 weights[RTK_MAX_NUM_OF_QUEUE];
} rtk_qos_queue_weights_t;

typedef struct rtk_port_phy_ability_s
{
    uint32 Half_10:1;
    uint32 Full_10:1;
    uint32 Half_100:1;
    uint32 Full_100:1;
    uint32 Half_1000:1;
    uint32 Full_1000:1;
    uint32 Full_2500:1;
    uint32 Full_10000:1;
    uint32 FC:1;
    uint32 AsyFC:1;
} rtk_port_phy_ability_t;

extern int rtk_83xx_stat_port_getAll(rtk_port_t port, rtk_83xx_stat_port_cntr_t *pPort_cntrs);
extern int rtk_switch_maxPktLenCfg_get(rtk_uint32 cfgId, rtk_uint32 *pPktLen);
extern int rtk_switch_maxPktLenCfg_set(rtk_uint32 cfgId, rtk_uint32 pktLen);
extern int rtk_switch_portMaxPktLen_get(rtk_port_t port, rtk_switch_maxPktLen_linkSpeed_t speed, rtk_uint32 *pCfgId);
extern int rtk_switch_portMaxPktLen_set(rtk_port_t port, rtk_switch_maxPktLen_linkSpeed_t speed, rtk_uint32 cfgId);
extern int rtk_port_phyStatus_get(rtk_port_t port, rtk_port_linkStatus_t *pLinkStatus, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex);
extern int rtk_83xx_port_phyAutoNegoAbility_get(rtk_port_t port, rtk_83xx_port_phy_ability_t *pAbility);
extern int rtk_83xx_port_phyAutoNegoAbility_set(rtk_port_t port, rtk_83xx_port_phy_ability_t *pAbility);
extern int rtk_port_macForceLink_set(rtk_port_t port, rtk_port_mac_ability_t *pPortability);
extern int rtk_port_macForceLink_get(rtk_port_t port, rtk_port_mac_ability_t *pPortability);
extern int rtk_83xx_port_phyReg_set(rtk_port_t port, rtk_port_phy_reg_t reg, rtk_port_phy_data_t regData);
extern int rtk_83xx_port_phyReg_get(rtk_port_t port, rtk_port_phy_reg_t reg, rtk_port_phy_data_t *pData);
extern int rtk_port_macStatus_get(rtk_port_t port, rtk_port_mac_ability_t *pPortstatus);
extern int rtk_83xx_rate_stormControlMeterIdx_set(rtk_port_t port, rtk_rate_storm_group_t stormType, rtk_uint32 index);
extern int rtk_83xx_rate_stormControlMeterIdx_get(rtk_port_t port, rtk_rate_storm_group_t stormType, rtk_uint32 *pIndex);
extern int rtk_83xx_rate_stormControlPortEnable_get(rtk_port_t port, rtk_rate_storm_group_t stormType, rtk_enable_t *pEnable);
extern int rtk_83xx_rate_stormControlPortEnable_set(rtk_port_t port, rtk_rate_storm_group_t stormType, rtk_enable_t enable);

/* 
* init state
*/
extern uint32 stat_83xx_init;
extern uint32 switch_83xx_init;
extern uint32 port_83xx_init;
extern uint32 rate_83xx_init;


extern int rtk_83xx_qos_init(rtk_uint32 queueNum);
extern int rtk_83xx_qos_schedulingQueue_get(rtk_port_t port, rtk_qos_queue_weights_t *pQweights);
extern int rtk_83xx_qos_schedulingQueue_set(rtk_port_t port, rtk_qos_queue_weights_t *pQweights);
extern int rtk_83xx_qos_priSelGroup_get(rtk_qos_priDecTbl_t index, rtk_priority_select_t *pPriDec);
extern int rtk_83xx_qos_priSelGroup_set(rtk_qos_priDecTbl_t index, rtk_priority_select_t *pPriDec);
extern int rtk_83xx_qos_priMap_get(rtk_uint32 queue_num, rtk_qos_pri2queue_t *pPri2qid);
extern int rtk_83xx_qos_priMap_set(rtk_uint32 queue_num, rtk_qos_pri2queue_t *pPri2qid);
extern int rtk_83xx_qos_1pPriRemap_get(rtk_uint32 dot1p_pri, rtk_uint32 *pInt_pri);
extern int rtk_83xx_qos_1pPriRemap_set(rtk_uint32 dot1p_pri, rtk_uint32 int_pri);
extern int rtk_83xx_qos_1pRemark_get(rtk_uint32 int_pri, rtk_uint32 *pDot1p_pri);
extern int rtk_83xx_qos_1pRemark_set(rtk_uint32 int_pri, rtk_uint32 dot1p_pri);
extern int rtk_83xx_qos_dscpRemark_get(rtk_uint32 int_pri, rtk_uint32 *pDscp);
extern int rtk_83xx_qos_dscpRemark_set(rtk_uint32 int_pri, rtk_uint32 dscp);
extern int rtk_83xx_qos_1pRemarkEnable_get(rtk_port_t port, rtk_enable_t *pEnable);
extern int rtk_83xx_qos_1pRemarkEnable_set(rtk_port_t port, rtk_enable_t enable);
extern int rtk_83xx_qos_dscpRemarkEnable_get(rtk_port_t port, rtk_enable_t *pEnable);
extern int rtk_83xx_qos_dscpRemarkEnable_set(rtk_port_t port, rtk_enable_t enable);

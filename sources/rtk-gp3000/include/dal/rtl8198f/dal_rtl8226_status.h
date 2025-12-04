#ifndef __DAL_RTL8226_STATUS_H_
#define __DAL_RTL8226_STATUS_H_

#include <common/debug/rt_log.h>
#include <osal/memory.h>

#define CAERR_PORT_ID 0xFFFF

#define RTL8226_PORT_DUPLEX BIT(3)
#define RTL8226_PORT_TXFC BIT(6)
#define RTL8226_PORT_RXFC BIT(7)

#define RTL8226_PORT1_PHY 0x6
#define RTL8226_PORT0_PHY 0x7

#define RTL8226_PORT_2500F BIT(7)
#define RTL8226_PORT_1000F BIT(9)
#define RTL8226_PORT_100F BIT(8)
#define RTL8226_PORT_100H BIT(7)
#define RTL8226_PORT_10F BIT(6)
#define RTL8226_PORT_10H BIT(5)
#define RTL8226_PORT_ASY_FC BIT(11)
#define RTL8226_PORT_FC BIT(10)
#define RTL8226_PORT_RE_AN BIT(9)


typedef enum {
	RTL8226_LINK_DOWN           = 0,
	RTL8226_LINK_UP_2G5         = 1,
	RTL8226_LINK_UP_2G5LITE     = 2,
	RTL8226_LINK_UP_1G          = 3,
	RTL8226_LINK_UP_500M        = 4,
	RTL8226_LINK_UP_100M        = 5,
	RTL8226_LINK_UP_10M         = 6,
	RTL8226_LINK_UNKNOWN        = 7,
} rtl8226_link_st_t;

typedef struct rtk_stat_port_cntr_s
{
    uint64 ifInOctets;
    uint32 ifInUcastPkts;
    uint32 ifInMulticastPkts;
    uint32 ifInBroadcastPkts;
    uint32 ifInDiscards;
    uint64 ifOutOctets;
    uint32 ifOutDiscards;
    uint32 ifOutUcastPkts;
    uint32 ifOutMulticastPkts;
    uint32 ifOutBrocastPkts;
    uint32 dot1dBasePortDelayExceededDiscards;
    uint32 dot1dTpPortInDiscards;
    uint32 dot1dTpHcPortInDiscards;
    uint32 dot3InPauseFrames;
    uint32 dot3OutPauseFrames;
    uint32 dot3OutPauseOnFrames;
    uint32 dot3StatsAligmentErrors;
    uint32 dot3StatsFCSErrors;
    uint32 dot3StatsSingleCollisionFrames;
    uint32 dot3StatsMultipleCollisionFrames;
    uint32 dot3StatsDeferredTransmissions;
    uint32 dot3StatsLateCollisions;
    uint32 dot3StatsExcessiveCollisions;
    uint32 dot3StatsFrameTooLongs;
    uint32 dot3StatsSymbolErrors;
    uint32 dot3ControlInUnknownOpcodes;
    uint32 etherStatsDropEvents;
    uint64 etherStatsOctets;
    uint32 etherStatsBcastPkts;
    uint32 etherStatsMcastPkts;
    uint32 etherStatsUndersizePkts;
    uint32 etherStatsOversizePkts;
    uint32 etherStatsFragments;
    uint32 etherStatsJabbers;
    uint32 etherStatsCollisions;
    uint32 etherStatsCRCAlignErrors;
    uint32 etherStatsPkts64Octets;
    uint32 etherStatsPkts65to127Octets;
    uint32 etherStatsPkts128to255Octets;
    uint32 etherStatsPkts256to511Octets;
    uint32 etherStatsPkts512to1023Octets;
    uint32 etherStatsPkts1024to1518Octets;
    uint64 etherStatsTxOctets;
    uint32 etherStatsTxUndersizePkts;
    uint32 etherStatsTxOversizePkts;
    uint32 etherStatsTxPkts64Octets;
    uint32 etherStatsTxPkts65to127Octets;
    uint32 etherStatsTxPkts128to255Octets;
    uint32 etherStatsTxPkts256to511Octets;
    uint32 etherStatsTxPkts512to1023Octets;
    uint32 etherStatsTxPkts1024to1518Octets;
    uint32 etherStatsTxPkts1519toMaxOctets;
    uint32 etherStatsTxBcastPkts;
    uint32 etherStatsTxMcastPkts;
    uint32 etherStatsTxFragments;
    uint32 etherStatsTxJabbers;
    uint32 etherStatsTxCRCAlignErrors;
    uint32 etherStatsRxUndersizePkts;
    uint32 etherStatsRxUndersizeDropPkts;
    uint32 etherStatsRxOversizePkts;
    uint32 etherStatsRxPkts64Octets;
    uint32 etherStatsRxPkts65to127Octets;
    uint32 etherStatsRxPkts128to255Octets;
    uint32 etherStatsRxPkts256to511Octets;
    uint32 etherStatsRxPkts512to1023Octets;
    uint32 etherStatsRxPkts1024to1518Octets;
    uint32 etherStatsRxPkts1519toMaxOctets;
    uint32 inOampduPkts;
    uint32 outOampduPkts;
}rtk_stat_port_cntr_t;

extern int rtl8226_get_link_speed(int phyid);
extern int rtl8226_get_link_status(int phyid);
extern unsigned int rtl_get_board_type(void);
extern int32 dal_rtl8198f_stat_embedded_port_get(rtk_port_t port, rtk_stat_port_cntr_t *pPortCntrs);
extern int rtl8226_phy_reg_get(int phyid, unsigned int mmd1, unsigned int mmd2, unsigned short *regdata);
extern int rtl8226_phy_reg_set(int phyid, unsigned int mmd1, unsigned int mmd2, unsigned short regdata);

#if defined(CONFIG_RTL_8226_SUPPORT)
int32 dal_rtl8226_port_link_get(rtk_port_t port,
				rtk_port_linkStatus_t *pLinkStatus);
int32 dal_rtl8226_port_mac_get(rtk_port_t port,
				rtk_port_mac_ability_t *pPortStatus);
int32 dal_rtl8226_port_stat_get(rtk_port_t port,
				rtk_stat_port_cntr_t *pPortCntrs);
rtk_port_t dal_rtl8198f_lspid_mapping(rtk_port_t port);
int32 dal_rtl8226_port_power_get(rtk_port_t port,
				rtk_enable_t *enable);
int32 dal_rtl8226_port_power_set(rtk_port_t port,
				rtk_enable_t enable);
int32 dal_rtl8226_port_phy_an_ability_get(rtk_port_t port,
				rtk_port_phy_ability_t *pAbility);
int32 dal_rtl8226_port_phy_an_ability_set(rtk_port_t port,
				rtk_port_phy_ability_t *pAbility);
#else /* CONFIG_RTL_8226_SUPPORT */
int32 dal_rtl8226_port_link_get(rtk_port_t port, rtk_port_linkStatus_t *pLinkStatus) {return RT_ERR_FAILED;}
int32 dal_rtl8226_port_mac_get(rtk_port_t port, rtk_port_mac_ability_t *pPortStatus) {return RT_ERR_FAILED;}
int32 dal_rtl8226_port_stat_get(rtk_port_t port, rtk_stat_port_cntr_t *pPortCntrs) {return RT_ERR_FAILED;}
rtk_port_t dal_rtl8198f_lspid_mapping(rtk_port_t port) {return port;}
int32 dal_rtl8226_port_power_get(rtk_port_t port,rtk_enable_t *enable) {return RT_ERR_FAILED;}
int32 dal_rtl8226_port_power_set(rtk_port_t port,rtk_enable_t enable) {return RT_ERR_FAILED;}
int32 dal_rtl8226_port_phy_an_ability_get(rtk_port_t port,rtk_port_phy_ability_t *pAbility) {return RT_ERR_FAILED;}
int32 dal_rtl8226_port_phy_an_ability_set(rtk_port_t port,rtk_port_phy_ability_t *pAbility) {return RT_ERR_FAILED;}
#endif /* CONFIG_RTL_8226_SUPPORT */

#endif /*__DAL_RTL8226_STATUS_H_*/


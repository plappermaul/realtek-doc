#ifdef CONFIG_RTL865XC
#include "rtl865xC_tblAsicDrv.h"
#else
#include "rtl8651_tblAsicDrv.h"
#endif

int32 l2_entry_cmp(rtl865x_tblAsicDrv_l2Param_t *entry1, rtl865x_tblAsicDrv_l2Param_t *entry2, int8 *_fun_, uint32 _line_);
int32 l3_entry_cmp(rtl865x_tblAsicDrv_routingParam_t *entry1, rtl865x_tblAsicDrv_routingParam_t *entry2, int8 *_fun_, uint32 _line_);
int32 arp_entry_cmp(rtl865x_tblAsicDrv_arpParam_t *entry1, rtl865x_tblAsicDrv_arpParam_t *entry2, int8 *_fun_, uint32 _line_);
int32 nxthop_entry_cmp(rtl865x_tblAsicDrv_nextHopParam_t *entry1, rtl865x_tblAsicDrv_nextHopParam_t *entry2, int8 *_fun_, uint32 _line_);
int32 pppoe_entry_cmp(rtl865x_tblAsicDrv_pppoeParam_t *entry1, rtl865x_tblAsicDrv_pppoeParam_t *entry2, int8 *_fun_, uint32 _line_);
int32 extl3_entry_cmp(rtl8651_extRouteTable_t *entry1,rtl8651_extRouteTable_t *entry2,int8 *_fun_, uint32 _line_);


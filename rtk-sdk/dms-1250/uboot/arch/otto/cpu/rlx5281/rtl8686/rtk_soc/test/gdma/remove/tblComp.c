#include <rtl_glue.h>
#include "tblComp.h"
#include "drvTest.h"

/*=======================================
  * ASIC Table Comparation
  *=======================================*/
#define ASIC_TABLE_COMPARATION

int32 l2_entry_cmp(rtl865x_tblAsicDrv_l2Param_t *entry1, rtl865x_tblAsicDrv_l2Param_t *entry2, int8 *_fun_, uint32 _line_)
{
	IS_EQUAL_MAC("mac", entry1->macAddr, entry2->macAddr, _fun_, _line_);
	IS_EQUAL_BOOL("cpu", entry1->cpu, entry2->cpu, _fun_, _line_);
	IS_EQUAL_BOOL("static", entry1->isStatic, entry2->isStatic, _fun_, _line_);
	IS_EQUAL_BOOL("srcBlk", entry1->srcBlk, entry2->srcBlk, _fun_, _line_);
	IS_EQUAL_BOOL("nhFlag", entry1->nhFlag, entry2->nhFlag, _fun_, _line_);
	IS_EQUAL_INT("memberPortMask", entry1->memberPortMask, entry2->memberPortMask, _fun_, _line_);
	IS_EQUAL_INT("ageSec", entry1->ageSec, entry2->ageSec, _fun_, _line_);
	IS_EQUAL_INT("fid", entry1->fid, entry2->fid, _fun_, _line_);
	IS_EQUAL_BOOL("auth", entry1->auth, entry2->auth, _fun_, _line_);
	return SUCCESS;
}

int32 l3_entry_cmp(rtl865x_tblAsicDrv_routingParam_t *entry1, rtl865x_tblAsicDrv_routingParam_t *entry2, int8 *_fun_, uint32 _line_)
{
	IS_EQUAL_IP("ipAddr", entry1->ipAddr, entry2->ipAddr, _fun_, _line_);

	if (entry1->ipAddr) /* If default route, msak is meaningless */
	{
		IS_EQUAL_IP("ipMask", entry1->ipMask, entry2->ipMask, _fun_, _line_);
	}
	IS_EQUAL_INT("process", entry1->process, entry2->process, _fun_, _line_);
	IS_EQUAL_INT("internal", entry1->internal, entry2->internal, _fun_, _line_);
	IS_EQUAL_INT("DMZflag", entry1->DMZFlag, entry2->DMZFlag, _fun_, _line_);

	switch(entry1->process) 
	{
		case 0x00: /* PPPoE */
			IS_EQUAL_INT("pppoeIdx", entry1->pppoeIdx, entry2->pppoeIdx, _fun_, _line_);
			IS_EQUAL_INT("vidx", entry1->vidx, entry2->vidx, _fun_, _line_);
			IS_EQUAL_INT("nextHopRow", entry1->nextHopRow, entry2->nextHopRow, _fun_, _line_);
			IS_EQUAL_INT("nextHopColumn", entry1->nextHopColumn, entry2->nextHopColumn, _fun_, _line_);
			break;		
			
		case 0x01: /* L2 */
			IS_EQUAL_INT("nextHopRow", entry1->nextHopRow, entry2->nextHopRow, _fun_, _line_);
			IS_EQUAL_INT("nextHopColumn", entry1->nextHopColumn, entry2->nextHopColumn, _fun_, _line_);
			IS_EQUAL_INT("vidx", entry1->vidx, entry2->vidx, _fun_, _line_);
			break;
			
		case 0x02: /* ARP */
			IS_EQUAL_INT("vidx", entry1->vidx, entry2->vidx, _fun_, _line_);
			IS_EQUAL_INT("arpStart", entry1->arpStart, entry2->arpStart, _fun_, _line_);
			IS_EQUAL_INT("arpEnd", entry1->arpEnd, entry2->arpEnd, _fun_, _line_);
			IS_EQUAL_INT("arpIpIdx", entry1->arpIpIdx, entry2->arpIpIdx, _fun_, _line_);
			break;

		case 0x04: /* CPU */
			break;

		case 0x05: /* NAPT Next Hop */
			IS_EQUAL_INT("nhStart", entry1->nhStart, entry2->nhStart, _fun_, _line_);
			IS_EQUAL_INT("nhNum", entry1->nhNum, entry2->nhNum, _fun_, _line_);
			IS_EQUAL_INT("nhNxt", entry1->nhNxt, entry2->nhNxt, _fun_, _line_);
			IS_EQUAL_INT("nhAlgo", entry1->nhAlgo, entry2->nhAlgo, _fun_, _line_);
			IS_EQUAL_INT("ipDomain", entry1->ipDomain, entry2->ipDomain, _fun_, _line_);
			break;
		
		case 0x06: /* Drop */
			break;
			
		default:
			rtlglue_printf("\t%s(%u): Unknown PROCESS type!!\n", _fun_, _line_);
			return FAILED;	
	}
	return SUCCESS;
}

int32 extl3_entry_cmp(rtl8651_extRouteTable_t *entry1,rtl8651_extRouteTable_t *entry2,int8 *_fun_, uint32 _line_)
{
       IS_EQUAL_IP("ipAddr", entry1->ipAddr, entry2->ipAddr, _fun_, _line_);
	if (entry1->ipAddr) /* If default route, msak is meaningless */
	IS_EQUAL_IP("ipMask", entry1->ipMask, entry2->ipMask, _fun_, _line_);
	IS_EQUAL_IP("nextHop",entry1->nextHop,entry2->nextHop,_fun_, _line_);
	IS_EQUAL_STR("network interface", entry1->ifName,entry2->ifName,_fun_, _line_);
       return SUCCESS;
}

int32 arp_entry_cmp(rtl865x_tblAsicDrv_arpParam_t *entry1, rtl865x_tblAsicDrv_arpParam_t *entry2, int8 *_fun_, uint32 _line_)
{
	IS_EQUAL_INT("nextHopRow", entry1->nextHopRow, entry2->nextHopRow, _fun_, _line_);
	IS_EQUAL_INT("nextHopColumn", entry1->nextHopColumn, entry2->nextHopColumn, _fun_, _line_);
	IS_EQUAL_INT("aging", entry1->aging, entry2->aging, _fun_, _line_);
	return SUCCESS;
}

int32 nxthop_entry_cmp(rtl865x_tblAsicDrv_nextHopParam_t *entry1, rtl865x_tblAsicDrv_nextHopParam_t *entry2, int8 *_fun_, uint32 _line_)
{
	IS_EQUAL_INT("nextHopRow", entry1->nextHopRow, entry2->nextHopRow, _fun_, _line_);
	IS_EQUAL_INT("nextHopColumn", entry1->nextHopColumn, entry2->nextHopColumn, _fun_, _line_);
	IS_EQUAL_INT("pppoeIdx", entry1->pppoeIdx, entry2->pppoeIdx, _fun_, _line_);
	IS_EQUAL_INT("dvid", entry1->dvid, entry2->dvid, _fun_, _line_);
	IS_EQUAL_INT("extIntIpIdx", entry1->extIntIpIdx, entry2->extIntIpIdx, _fun_, _line_);
	IS_EQUAL_BOOL("isPppoe", entry1->isPppoe, entry2->isPppoe, _fun_, _line_);
	return SUCCESS;
}

int32 pppoe_entry_cmp(rtl865x_tblAsicDrv_pppoeParam_t *entry1, rtl865x_tblAsicDrv_pppoeParam_t *entry2, int8 *_fun_, uint32 _line_)
{
	IS_EQUAL_INT("sessionId", entry1->sessionId, entry2->sessionId, _fun_, _line_);
	return SUCCESS;
}





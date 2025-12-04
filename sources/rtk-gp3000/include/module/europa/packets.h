#ifndef H__PACKETS
#define H__PACKETS

#include "common.h"

/*this struct is RTL8213 Nic's CPU tag*/
typedef struct pkt_hdr{
	uint32	status;
	uint8	reserved[2];
	uint16	length;
}pkt_hdr_t;

typedef struct pkt_info
{
	pkt_hdr_t pktHdr;	//RTL8213 special CPU tag
	struct  eth_info
	{
		uint8 da[6];
		uint8 sa[6];		//remote mac address
		uint16 type;
		uint32 vlan;		//this is available only if pktHdr.status bit 12 is 1.
	}ethInfo;
}pkt_info_t;


#endif

/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* $Header: /usr/local/dslrepos/u-boot-2011.12/arch/mips/cpu/rlx5281/rtl8676/vlanTable.c,v 1.1.1.1 2012/02/01 07:50:52 yachang Exp $
*
* Abstract: Switch core vlan table access driver source code.
*
* $Author: yachang $
*
* $Log: vlanTable.c,v $
* Revision 1.1.1.1  2012/02/01 07:50:52  yachang
* First working u-boot for RTL8676
*
*
* Revision 1.1.1.1  2011/06/10 08:06:31  yachang
*
*
* Revision 1.1.1.1  2007/08/06 10:05:02  root
* Initial import source to CVS
*
* Revision 1.6  2006/09/15 03:53:39  ghhuang
* +: Add TFTP download support for RTL8652 FPGA
*
* Revision 1.5  2006/01/16 02:40:41  bo_zhao
* +: add some support for gcc '-O' flag
*
* Revision 1.4  2005/09/22 05:22:31  bo_zhao
* *** empty log message ***
*
* Revision 1.1.1.1  2005/09/05 12:38:25  alva
* initial import for add TFTP server
*
* Revision 1.3  2005/01/28 02:03:48  yjlou
* *: loader version migrates to "00.00.19".
* +: support Hub mode
* +: Ping mode support input IP address
* *: clear WDTIND always.
*
* Revision 1.2  2004/03/31 01:49:20  yjlou
* *: all text files are converted to UNIX format.
*
* Revision 1.1  2004/03/16 06:36:13  yjlou
* *** empty log message ***
*
* Revision 1.2  2004/03/09 00:46:12  danwu
* remove unused code to shrink loader image size under 0xc000 and only flash block 0 & 3 occupied
*
* Revision 1.1.1.1  2003/09/25 08:16:55  tony
*  initial loader tree 
*
* Revision 1.1.1.1  2003/05/07 08:16:06  danwu
* no message
*
* ---------------------------------------------------------------
*/
#include "rtl_types.h"
#include "rtl_errno.h"
#include "asicregs.h"
#include "swCore.h"
#include "vlanTable.h"


#include "swTable.h"
extern void tableAccessForeword(uint32, uint32, void *);
extern int32 swTable_addEntry(uint32 tableType, uint32 eidx, void *entryContent_P);
extern int32 swTable_modifyEntry(uint32 tableType, uint32 eidx, void *entryContent_P);
extern int32 swTable_forceAddEntry(uint32 tableType, uint32 eidx, void *entryContent_P);
extern int32 swTable_readEntry(uint32 tableType, uint32 eidx, void *entryContent_P);

#include "mipsregs.h"
//wei add
int lx4180_ReadStatus()
{
   volatile unsigned int reg;
	reg= read_32bit_cp0_register(CP0_STATUS);
	__asm__ volatile("nop");	// david
   	__asm__ volatile("nop");   
	return reg;

}
void lx4180_WriteStatus(int s)
{
   volatile unsigned int reg=s;
	write_32bit_cp0_register(CP0_STATUS, reg);
	__asm__ volatile("nop");	// david
   	__asm__ volatile("nop");   
	return ;

}



/* STATIC VARIABLE DECLARATIONS
 */

int32 swCore_netifCreate(uint32 idx, rtl_netif_param_t * param)
{
    netif_table_t    entryContent;
    uint32	temp,temp2;
    
    ASSERT_CSP(param);
    
    // disable interrupt
    // I don't know the reason but if you want to use "-O" flag, must disalbe interrupt before swTable_readEntry();
    temp = lx4180_ReadStatus();
    if (0!=temp&0x1) {
	    temp2 = temp&0xfffffffe;
	    lx4180_WriteStatus(temp2);
    }

    swTable_readEntry(TYPE_NETINTERFACE_TABLE, idx, &entryContent);

    // restore status register
    if (0!=temp&0x1) {
	    lx4180_WriteStatus(temp);
    }
    
    if ( entryContent.valid )
    {
       return EEXIST;
    }

    bzero( (void *) &entryContent, sizeof(entryContent) );
    entryContent.valid = param->valid;
    entryContent.vid = param->vid;

    entryContent.mac47_19 = ((param->gMac.mac47_32 << 13) | (param->gMac.mac31_16 >> 3)) & 0xFFFFFFF;
    entryContent.mac18_0 = ((param->gMac.mac31_16 << 16) | param->gMac.mac15_0) & 0x7FFFF;

    entryContent.inACLStartH = (param->inAclStart >> 2) & 0x1f;
    entryContent.inACLStartL = param->inAclStart & 0x3;
    entryContent.inACLEnd = param->inAclEnd;
    entryContent.outACLStart = param->outAclStart;
    entryContent.outACLEnd = param->outAclEnd;
    entryContent.enHWRoute = param->enableRoute;

    entryContent.macMask = 8 - (param->macAddrNumber & 0x7);

    entryContent.mtuH = param->mtu >> 3;
    entryContent.mtuL = param->mtu & 0x7;

    /* Write into hardware */
    if ( swTable_addEntry(TYPE_NETINTERFACE_TABLE, idx, &entryContent) == 0 )
        return 0;
    else
        /* There might be something wrong */
        ASSERT_CSP( 0 );
}

int swCore_netifSetMac(uint32 idx, unsigned char *mac) 
{
	netif_table_t entryContent;
	uint16 *u16Mac;
	
	u16Mac = (uint16 *)mac;
	
	swTable_readEntry(TYPE_NETINTERFACE_TABLE, idx, &entryContent);
	if (! entryContent.valid ) {
		return -1;
	}
	
	entryContent.mac47_19 = ((u16Mac[0] << 13) | (u16Mac[1] >> 3)) & 0xFFFFFF;
	entryContent.mac18_0  = ((u16Mac[1] << 16) | (u16Mac[0])) & 0x7FFFF;
	
	swTable_modifyEntry(TYPE_NETINTERFACE_TABLE,idx,&entryContent);
	return 0;
}


int32 vlanTable_create(uint32 vid, rtl_vlan_param_t * param)
{
    vlan_table_t    entryContent;
    uint32	temp,temp2;
    
    ASSERT_CSP(param);
    
    // disable interrupt
    // I don't know the reason but if you want to use "-O" flag, must disalbe interrupt before swTable_readEntry();
    temp = lx4180_ReadStatus();
    if (0!=temp&0x1) {
	    temp2 = temp&0xfffffffe;
	    lx4180_WriteStatus(temp2);
    }
    
    swTable_readEntry(TYPE_VLAN_TABLE, vid, &entryContent);
    
    // restore status register
    if (0!=temp&0x1) {
	    lx4180_WriteStatus(temp);
    }

    bzero( (void *) &entryContent, sizeof(entryContent) );
    entryContent.memberPort = param->memberPort & ALL_PORT_MASK;
    entryContent.egressUntag = param->egressUntag;
    entryContent.fid = param->fid;

#ifdef CONFIG_RTL8196D
    entryContent.vid = vid;
#endif

    /* Write into hardware */
    if ( swTable_addEntry(TYPE_VLAN_TABLE, vid, &entryContent) == 0 )
        return 0;
    else
        /* There might be something wrong */
        ASSERT_CSP( 0 );
}





#if 0
int32 vlanTable_destroy(uint32 vid)
{
    vlan_table_t    entryContent;
    uint32	temp,temp2;
    
    // disable interrupt
    // I don't know the reason but if you want to use "-O" flag, must disalbe interrupt before swTable_readEntry();
    temp = lx4180_ReadStatus();

    if (0!=temp&0x1) {
	    temp2 = temp&0xfffffffe;
	    lx4180_WriteStatus(temp2);
    }

    swTable_readEntry(TYPE_VLAN_TABLE, vid, &entryContent);
    
    // restore status register
    if (0!=temp&0x1) {
	    lx4180_WriteStatus(temp);
    }
    
    bzero(&entryContent, sizeof(vlan_table_t));
    
    /* Write into hardware */
    if ( swTable_modifyEntry(TYPE_VLAN_TABLE, vid, &entryContent) == 0 )
        return 0;
    else
        /* There might be something wrong */
        ASSERT_CSP( 0 );
}






int32 vlanTable_setStpStatusOfAllPorts(uint32 vid, uint32 STPStatus)
{

}
#endif


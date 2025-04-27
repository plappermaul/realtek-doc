/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* $Header: /usr/local/dslrepos/u-boot-2011.12/arch/mips/cpu/rlx5281/rtl8676/swCore.h,v 1.1.1.1 2012/02/01 07:50:52 yachang Exp $
*
* Abstract: Switch core header file.
*
* $Author: yachang $
*
* $Log: swCore.h,v $
* Revision 1.1.1.1  2012/02/01 07:50:52  yachang
* First working u-boot for RTL8676
*
*
* Revision 1.2  2011/09/08 09:49:36  cathy
* add for external phy detection
*
* Revision 1.1.1.1  2011/06/10 08:06:31  yachang
*
*
* Revision 1.5  2006/09/15 03:53:39  ghhuang
* +: Add TFTP download support for RTL8652 FPGA
*
* Revision 1.4  2005/09/22 05:22:31  bo_zhao
* *** empty log message ***
*
* Revision 1.1.1.1  2005/09/05 12:38:24  alva
* initial import for add TFTP server
*
* Revision 1.3  2004/03/31 01:49:20  yjlou
* *: all text files are converted to UNIX format.
*
* Revision 1.2  2004/03/30 11:34:38  yjlou
* *: commit for 80B IC back
*   +: system clock rate definitions have changed.
*   *: fixed the bug of BIST_READY_PATTERN
*   +: clean all ASIC table when init ASIC.
* -: define _L2_MODE_ to support L2 switch mode.
*
* Revision 1.1  2004/03/16 06:36:13  yjlou
* *** empty log message ***
*
* Revision 1.2  2004/03/16 06:04:04  yjlou
* +: support pure L2 switch mode (for hardware testing)
*
* Revision 1.1.1.1  2003/09/25 08:16:56  tony
*  initial loader tree 
*
* Revision 1.1.1.1  2003/05/07 08:16:07  danwu
* no message
*
* ---------------------------------------------------------------
*/

#ifndef _SWCORE_H
#define _SWCORE_H

/* --------------------------------------------------------------------
 * ROUTINE NAME - swCore_init
 * --------------------------------------------------------------------
 * FUNCTION: This service initializes the switch core.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : Upon successful completion, the function returns 0. 
        Otherwise, 
		ENFILE: Destination slot of vlan table is occupied.
 * NOTE    : None.
 * -------------------------------------------------------------------*/
int swCore_init(void);

#define SWITCH_BASE 0xbb000000

#endif /* _SWCORE_H */


/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* $Header: /usr/local/dslrepos/u-boot-2011.12/arch/mips/cpu/rlx5281/rtl8676/swTable.c,v 1.1.1.1 2012/02/01 07:50:52 yachang Exp $
*
* Abstract: Switch core table access driver source code.
*
* $Author: yachang $
*
* $Log: swTable.c,v $
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
* Revision 1.5  2006/09/15 03:53:39  ghhuang
* +: Add TFTP download support for RTL8652 FPGA
*
* Revision 1.4  2005/09/22 05:22:31  bo_zhao
* *** empty log message ***
*
* Revision 1.1.1.1  2005/09/05 12:38:25  alva
* initial import for add TFTP server
*
* Revision 1.3  2004/03/31 01:49:20  yjlou
* *: all text files are converted to UNIX format.
*
* Revision 1.2  2004/03/30 11:25:11  yjlou
* *: table_entry_length() is fixed to 8 words (in swTable_readEntry() and tableAccessForeword() ).
*
* Revision 1.1  2004/03/16 06:36:13  yjlou
* *** empty log message ***
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



/* LOCAL SUBPROGRAM SPECIFICATIONS
 */
RTL_STATIC_INLINE void tableAccessForeword(uint32, uint32, void *);

int32 swTable_addEntry(uint32 tableType, uint32 eidx, void *entryContent_P)
{

    REG32(SWTCR0) = REG32(SWTCR0) | EN_STOP_TLU;
    while ((REG32(SWTCR0) & STOP_TLU_READY) == 0);

    tableAccessForeword(tableType, eidx, entryContent_P);
        
    /* Activate add command */
    REG32(SWTACR) = ACTION_START | CMD_ADD;
    
    /* Wait for command done */
    while ( (REG32(SWTACR) & ACTION_MASK) != ACTION_DONE );

    REG32(SWTCR0) = REG32(SWTCR0) & ~EN_STOP_TLU;

    /* Check status */
    if ( (REG32(SWTASR) & TABSTS_MASK) != TABSTS_SUCCESS )
        return ECOLLISION;
    else
        return 0;
}



int32 swTable_modifyEntry(uint32 tableType, uint32 eidx, void *entryContent_P)
{
    REG32(SWTCR0) = REG32(SWTCR0) | EN_STOP_TLU;
    while ((REG32(SWTCR0) & STOP_TLU_READY) == 0);

    tableAccessForeword(tableType, eidx, entryContent_P);
        
    /* Activate add command */
    REG32(SWTACR) = ACTION_START | CMD_MODIFY;
    
    /* Wait for command done */
    while ( (REG32(SWTACR) & ACTION_MASK) != ACTION_DONE );

    REG32(SWTCR0) = REG32(SWTCR0) & ~EN_STOP_TLU;

    /* Check status */
    if ( (REG32(SWTASR) & TABSTS_MASK) != TABSTS_SUCCESS )
        return EEMPTY;
    else
        return 0;
}



int32 swTable_forceAddEntry(uint32 tableType, uint32 eidx, void *entryContent_P)
{
    REG32(SWTCR0) = REG32(SWTCR0) | EN_STOP_TLU;
    while ((REG32(SWTCR0) & STOP_TLU_READY) == 0);

    tableAccessForeword(tableType, eidx, entryContent_P);
        
    /* Activate add command */
    REG32(SWTACR) = ACTION_START | CMD_FORCE;
    
    /* Wait for command done */
    while ( (REG32(SWTACR) & ACTION_MASK) != ACTION_DONE );

    REG32(SWTCR0) = REG32(SWTCR0) & ~EN_STOP_TLU;

    /* Check status */
    if ( (REG32(SWTASR) & TABSTS_MASK) == TABSTS_SUCCESS )
        return 0;
        
    /* There might be something wrong */
    ASSERT_CSP( 0 );

}


int32 swTable_readEntry(uint32 tableType, uint32 eidx, void *entryContent_P)
{
    uint32 *    entryAddr;

    REG32(SWTCR0) = REG32(SWTCR0) | EN_STOP_TLU;
    while ((REG32(SWTCR0) & STOP_TLU_READY) == 0);

    ASSERT_CSP(entryContent_P);
    
    entryAddr = (uint32 *) (table_access_addr_base(tableType) + eidx * TABLE_ENTRY_DISTANCE);
    
    /* Wait for command ready */
    while ( (REG32(SWTACR) & ACTION_MASK) != ACTION_DONE );
    
    /* Read registers according to entry width of each table */
    *((uint32 *)entryContent_P + 7) = *(entryAddr + 7);
    *((uint32 *)entryContent_P + 6) = *(entryAddr + 6);
    *((uint32 *)entryContent_P + 5) = *(entryAddr + 5);
    *((uint32 *)entryContent_P + 4) = *(entryAddr + 4);
    *((uint32 *)entryContent_P + 3) = *(entryAddr + 3);
    *((uint32 *)entryContent_P + 2) = *(entryAddr + 2);
    *((uint32 *)entryContent_P + 1) = *(entryAddr + 1);
    *((uint32 *)entryContent_P + 0) = *(entryAddr + 0);

    REG32(SWTCR0) = REG32(SWTCR0) & ~EN_STOP_TLU;

    return 0;
}



//RTL_STATIC_INLINE void tableAccessForeword(uint32 tableType, uint32 eidx,     void *entryContent_P)
void tableAccessForeword(uint32 tableType, uint32 eidx,     void *entryContent_P)
{
    ASSERT_CSP(entryContent_P);

    /* Wait for command done */
    while ( (REG32(SWTACR) & ACTION_MASK) != ACTION_DONE );
    
    /* Write registers according to entry width of each table */
    REG32(TCR7) = *((uint32 *)entryContent_P + 7);
    REG32(TCR6) = *((uint32 *)entryContent_P + 6);
    REG32(TCR5) = *((uint32 *)entryContent_P + 5);
    REG32(TCR4) = *((uint32 *)entryContent_P + 4);
    REG32(TCR3) = *((uint32 *)entryContent_P + 3);
    REG32(TCR2) = *((uint32 *)entryContent_P + 2);
    REG32(TCR1) = *((uint32 *)entryContent_P + 1);
    REG32(TCR0) = *(uint32 *)entryContent_P;
    
    /* Fill address */
    REG32(SWTAA) = table_access_addr_base(tableType) + eidx * TABLE_ENTRY_DISTANCE;
}


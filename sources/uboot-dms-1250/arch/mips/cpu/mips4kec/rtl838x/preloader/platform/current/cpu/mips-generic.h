#ifndef _SOC_MIPS_GENERIC_
#define _SOC_MIPS_GENERIC_

#define CKUSEG 0x00000000
#define CKSEG0 0x80000000
#define CKSEG1 0xa0000000
#define CKSEG2 0xc0000000
#define CKSEG3 0xe0000000

#define CP0_INDEX       $0
#define CP0_RANDOM      $1
#define CP0_ENTRYLO0    $2
#define CP0_ENTRYLO1    $3
#define CP0_CONF        $3
#define CP0_CONTEXT     $4
#define CP0_PAGEMASK    $5
#define CP0_WIRED       $6
#define CP0_INFO        $7
#define CP0_BADVADDR    $8
#define CP0_COUNT       $9
#define CP0_ENTRYHI     $10
#define CP0_COMPARE     $11
#define CP0_STATUS      $12
#define CP0_CAUSE       $13
#define CP0_EPC         $14
#define CP0_PRID        $15
#define CP0_CONFIG      $16
#define CP0_LLADDR      $17
#define CP0_WATCHLO     $18
#define CP0_WATCHHI     $19
#define CP0_XCONTEXT    $20
#define CP0_FRAMEMASK   $21
#define CP0_DIAGNOSTIC  $22
#define CP0_PERFORMANCE $25
#define CP0_ECC         $26
#define CP0_CACHEERR    $27
#define CP0_TAGLO       $28
#define CP0_TAGHI       $29
#define CP0_ERROREPC    $30

#define CP0_STATUS_IE 1

#define CP0_CONF_CACHABLE_NC_WT_nWA   0
#define CP0_CONF_UNCACHED             2
#define CP0_CONF_CACHABLE_NC_WB_WA    3
#define CP0_CONF_UNCACHED_ACCELERATED 7
#define CP0_CONF_CACHE_MASK           0x7

#define Index_Invalidate_I    0x00
#define Index_Writeback_Inv_D 0x01
#define Index_Load_Tag_I      0x04
#define Index_Load_Tag_D      0x05
#define Index_Store_Tag_I     0x08
#define Index_Store_Tag_D     0x09
#define Hit_Invalidate_I      0x10
#define Hit_Invalidate_D      0x11
#define Fill_I                0x14
#define Hit_Writeback_Inv_D   0x15

#define zero $0
#define AT   $1
#define v0   $2  /* return value */
#define v1   $3
#define a0   $4  /* argument registers */
#define a1   $5
#define a2   $6
#define a3   $7
#define t0   $8  /* caller saved */
#define t1   $9
#define t2   $10
#define t3   $11
#define t4   $12
#define t5   $13
#define t6   $14
#define t7   $15
#define s0   $16 /* callee saved */
#define s1   $17
#define s2   $18
#define s3   $19
#define s4   $20
#define s5   $21
#define s6   $22
#define s7   $23
#define t8   $24 /* caller saved */
#define t9   $25
#define k0   $26
#define k1   $27
#define gp   $28
#define sp   $29
#define s8   $30
#define ra   $31

/* LFUNC - declare local function */
#define LFUNC(symbol)	  \
	.text;       \
	.align 4;    \
	.ent symbol; \
symbol:

/* FUNC - declare global function */
#define GFUNC(symbol)	  \
	.text;         \
	.globl symbol; \
	.align 4;      \
	.ent symbol;   \
	symbol:

/* END - mark end of function */
#define END(symbol)	  \
	.end symbol

#endif

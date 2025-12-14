#include <asm/reg.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <asm/ottoregs.h>

/* MIPS, 5281, 5181 */
#define Hit_Invalidate_D 0x11
#define Hit_Writeback_Inv_D 0x15
#define Hit_Writeback_D 0x19

/* MIPS, 5281 */
#define Hit_Invalidate_I 0x10

/* MIPS */
#define Index_Invalidate_I 0x00
#define Index_Writeback_Inv_D 0x01
#define Index_Writeback_Inv_L23 0x03
#define Index_Load_Tag_I 0x04
#define Index_Load_Tag_D 0x05
#define Index_Store_Tag_I 0x08
#define Index_Store_Tag_D 0x09
#define Index_Store_Tag_L23 0x0b
#define Hit_Invalidate_L23 0x13
#define Fill_I 0x14
#define Hit_Writeback_Inv_L23 0x17
#define Hit_Writeback_L23 0x1b
#define Fetch_Lock_I 0x1c
#define Fetch_Lock_D 0x1d

#define cacheop(cmd, addr_reg) cache cmd, 0(addr_reg)

#ifndef SECTION_CACHE_FUNC
#	define SECTION_CACHE_FUNC .text
#endif

#define GFUNC(fname, ...) \
	SECTION_CACHE_FUNC;\
	.global fname;\
	.ent fname;\
	.set noreorder;\
fname: \
	__VA_ARGS__ \
	.end fname;

#define MIPS_CONFIG1_ICACHE_OFF (16)
#define MIPS_CONFIG1_DCACHE_OFF (7)
.macro MIPS_CACHE_V0_SZ_V1_LINE_SZ config1_cache_off
	mfc0  t0, CP0_CONFIG1
	srl   a0, t0, \config1_cache_off //$ assoc.
	andi  a0, a0, 7
	addi  a0, a0, 1

	srl   a1, t0, (\config1_cache_off + 3) //$ line sz, pow(2)
	andi  a1, a1, 7
	addi  a1, a1, 1

	srl   a2, t0, (\config1_cache_off + 6) //$ set num., pow(2)
	andi  a2, a2, 7
	addi  a2, a2, 6

	li    t0, 1
	add   a2, a2, a1 //1 << (line sz + set num)
	sll   a2, t0, a2
	mul   v0, a2, a0 //$ size in byte, v0
	sll   v1, t0, a1 //$ line sz in byte, v1
.endm

//5281 has no such a configuration.
.macro ENABLE_CACHEABLE_K0
enable_cacheable_k0:
	GET_CPU_TYPE;
	beqz  v0, _eck0_mips; nop;
_eck0_5x81:
	jr    ra; nop;
_eck0_mips:
	mfc0  t0, CP0_CONFIG
	li    t1, ~(0x7)
	and   t0, t0, t1
	ori   t0, CP0_CONF_CACHABLE_NC_WB_WA //Non-Coherency, Write-Back, Allocate
	mtc0  t0, CP0_CONFIG
	nop
.endm

.macro CONTINUE_WITH_ICACHE
	lui   ra, %hi(_continue_with_icache)
	addiu ra, %lo(_continue_with_icache)
	jr    ra
	nop
_continue_with_icache:
.endm

.macro CACHE_INIT
	jal   invalidate_icache_all
	nop

	ENABLE_CACHEABLE_K0

	CONTINUE_WITH_ICACHE

	jal   invalidate_dcache_all
	nop
.endm

//v0 == 1: 5x81; v0 == 0: MIPs
.macro GET_CPU_TYPE
	mfc0  v0, CP0_PRID;
	srl   v0, v0, 14;
	andi  v0, v0, 0x1;
.endm

#define CCTL_INV_DCACHE_ALL (1)
#define CCTL_INV_ICACHE_ALL (1<<1)
#define CCTL_WB_DCACHE_ALL (1<<8)
#define CCTL_WB_INV_DCACHE_ALL (1<<9)
.macro RLX_5X81_CCTL, cctl_op_bitmap
	mfc0  t0, CP0_5x81_CCTL;
	li    t1, ~(\cctl_op_bitmap);
	and   t0, t0, t1;
	mtc0  t0, CP0_5x81_CCTL;
	ori   t0, t0, (\cctl_op_bitmap);
	mtc0  t0, CP0_5x81_CCTL;
.endm

//Force line size to 16B. It hurts performance but fits all CPUs.
//note that `sync' is required due to E47 of MD00907-1D-interAptiv-ERS-01.14.pdf
.macro RANGED_CACHE_OPERATION_A0_START_A1_END, cache_op
	move  t0, a0
1:
	cacheop(\cache_op, t0)
	sync
	addi  t0, 16
	blt   t0, a1, 1b
	nop
	jr    ra
	nop
.endm

GFUNC(invalidate_icache_all,
	GET_CPU_TYPE;
	beqz  v0, _iia_mips; nop;
_iia_5x81:
	RLX_5X81_CCTL CCTL_INV_ICACHE_ALL;
	jr    ra; nop;
_iia_mips:
	MIPS_CACHE_V0_SZ_V1_LINE_SZ MIPS_CONFIG1_ICACHE_OFF;
	lui   a0, 0x8000;
	add   a1, a0, v0;
	RANGED_CACHE_OPERATION_A0_START_A1_END Index_Invalidate_I;
)

GFUNC(invalidate_icache_range,
	RANGED_CACHE_OPERATION_A0_START_A1_END Hit_Invalidate_I;
)

GFUNC(fetch_lock_icache_range,
	GET_CPU_TYPE;
	beqz  v0, _flir_mips; nop;
_flir_5x81:
	b     _flir_5x81; nop;
_flir_mips:
	RANGED_CACHE_OPERATION_A0_START_A1_END Fetch_Lock_I;
)

GFUNC(writeback_invalidate_dcache_all,
	GET_CPU_TYPE;
	beqz  v0, _wida_mips; nop;
_wida_5x81:
	RLX_5X81_CCTL CCTL_WB_INV_DCACHE_ALL;
	jr    ra; nop;
_wida_mips:
	MIPS_CACHE_V0_SZ_V1_LINE_SZ MIPS_CONFIG1_DCACHE_OFF;
	lui   a0, 0x8000;
	add   a1, a0, v0;
	RANGED_CACHE_OPERATION_A0_START_A1_END Index_Writeback_Inv_D;
)

GFUNC(writeback_invalidate_dcache_range,
	RANGED_CACHE_OPERATION_A0_START_A1_END Hit_Writeback_Inv_D;
)

//MIPS only supports Hit_Writeback_D; no Index_Writeback_D
GFUNC(writeback_dcache_all,
	GET_CPU_TYPE;
	beqz  v0, _wida_mips; nop;
_wda_5x81:
	RLX_5X81_CCTL CCTL_WB_DCACHE_ALL;
	jr    ra; nop;
_wda_mips:
	b     _wda_mips;
	nop;
)

GFUNC(writeback_dcache_range,
	RANGED_CACHE_OPERATION_A0_START_A1_END Hit_Writeback_D;
)

GFUNC(invalidate_dcache_all,
	GET_CPU_TYPE;
	beqz  v0, _wida_mips; nop;
_ida_5x81:
	RLX_5X81_CCTL CCTL_INV_DCACHE_ALL;
	jr    ra; nop;
_ida_mips:
	MIPS_CACHE_V0_SZ_V1_LINE_SZ MIPS_CONFIG1_DCACHE_OFF;
	lui   a0, 0x8000;
	add   a1, a0, v0;
	mtc0  zero, CP0_TAGLO;
	mtc0  zero, CP0_DTAGLO;
	RANGED_CACHE_OPERATION_A0_START_A1_END Index_Store_Tag_D;
)

GFUNC(invalidate_dcache_range,
	RANGED_CACHE_OPERATION_A0_START_A1_END Hit_Invalidate_D;
)

GFUNC(fetch_lock_dcache_range,
	GET_CPU_TYPE;
	beqz  v0, _fldr_mips; nop;
_fldr_5x81:
	b     _fldr_5x81; nop;
_fldr_mips:
	RANGED_CACHE_OPERATION_A0_START_A1_END Fetch_Lock_D;
)

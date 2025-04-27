#define DISABLE_CPC CPC_SWITCH 0
#define ENABLE_CPC  CPC_SWITCH 1
.macro CPC_SWITCH en
	li  k1, GCR_CPC_BASE
	li  k0, ((CPC_BASE_ADDR & 0x1fffffff) | \en)
	sw  k0, 0(k1)
.endm

.macro LOOP_IF_NOT_CORE0
	mfc0 k0, CP0_EBASE
	ext  k0, k0, 0, 2
_not_core0:
	bne  zero, k0, _not_core0
	nop
.endm

.macro CLOCK_OFF_CORE1
	ENABLE_CPC
	li  k1, CPC_CO_BASE_ADDR
	lui k0, 0x0001
	sw  k0, 0x10(k1)
	li  k0, 0x0001
	sw  k0, 0x00(k1)
	DISABLE_CPC
.endm

.macro CPU_CP0_INIT
cpu_cp0_init:
	mtc0  zero, CP0_WATCHLO
	mtc0  zero, CP0_WATCHHI
	mtc0  zero, CP0_CAUSE
	mtc0  zero, CP0_COUNT
	mtc0  zero, CP0_COMPARE
	mfc0  k0, CP0_STATUS
	li    k1, ~(CP0_STATUS_IE|CP0_STATUS_EXL|CP0_STATUS_ERL)
	and   k0, k1
	mtc0  k0, CP0_STATUS
.endm

.macro INIT_ICACHE cache_sz_b, cacheline_sz_b
init_icache:
	mtc0  zero, CP0_ITAGLO
	lui   t0, 0x8000
	li    t1, (0x80000000 + \cache_sz_b - \cacheline_sz_b)
_reset_next_itag:
	cacheop(Index_Store_Tag_I, t0)
	bne   t0, t1, _reset_next_itag
	addi  t0, \cacheline_sz_b
.endm

.macro ENABLE_CACHEABLE_K0
enable_cacheable_k0:	
	li    a1, CP0_CONF_CACHABLE_NC_WB_WA //Non-Coherency, Write-Back, Allocate
	ins   a0, a1, 0, 3                   // instert K0
	mtc0  a0, CP0_CONFIG                 // write CP0_Config
	nop
.endm

.macro CONTINUE_WITH_ICACHE
	lui   ra, %hi(_continue_with_icache)
	addiu ra, %lo(_continue_with_icache)
	jr.hb ra
        nop
_continue_with_icache:
.endm

.macro INIT_DCACHE cache_sz_b, cacheline_sz_b
init_dcache:
	mtc0  zero, CP0_DTAGLO
	lui   t0, 0x8000
	li    t1, (0x80000000 + \cache_sz_b - \cacheline_sz_b)
_reset_next_dtag:
	cacheop(Index_Store_Tag_D, t0)
	bne   t0, t1, _reset_next_dtag
	addi  t0, \cacheline_sz_b
.endm

.macro CPU_INIT
	CPU_CP0_INIT
.endm

.macro CACHE_INIT
	INIT_ICACHE ICACHE_SIZE, CACHELINE_SIZE

	ENABLE_CACHEABLE_K0
	
	CONTINUE_WITH_ICACHE

	INIT_DCACHE DCACHE_SIZE, CACHELINE_SIZE
.endm

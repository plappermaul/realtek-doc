#ifndef __SIHNAH_H__
#define __SIHNAH_H__ 1

#define SIHNAH_CACHE_LINE_SIZE (64)

#define _UINT8_T_DECLARED 1
#define _UINT16_T_DECLARED 1
#define _UINT32_T_DECLARED 1
#define _UINT64_T_DECLARED 1

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

/* make a cache line aligned array that fits cache line size */
#define SIHNAH_DMA_ALLOC(_ptr_name, _var_size, _cache_line_size) \
	uint8_t _ptr_name[((_var_size + _cache_line_size - 1)&(~(_cache_line_size - 1)))] __attribute__ ((aligned (_cache_line_size)));

enum {
	BOOT_SNOR,
	BOOT_SNAND,
	BOOT_PNAND,
	BOOT_MMC,
	BOOT_ERROR
};

struct sihnah_x_op_s;

typedef struct {
	const struct sihnah_x_op_s *op;
	uint32_t offset;
	uint32_t erase_b;
	uint8_t *prog_buf;
	uint32_t prog_len;
	uint32_t prog_max;
} flash_ctx_t;

typedef struct sihnah_x_op_s {
	char *type;
	int (*init)(flash_ctx_t *_ctx);
	int (*erase)(uint32_t offset, uint32_t sz_b);
	int (*prog)(uint32_t offset, uint32_t sz_b, uint8_t *buf);
} sihnah_x_op_t;

/************************ for Atropine ************************/
#if defined(__ATROPINE__)
#	include <bareline.h>
#	define sihnah_memset bl_memset
#	define sihnah_memcpy bl_memcpy
#	define sihnah_flush_dcache_range(_start, _sz) flush_dcache_range(_start, _sz)

/* sync. to ATF:/include/common/debug.h */
# ifndef LOG_LEVEL
#		define LOG_LEVEL 20
#	endif

#	define LOG_LEVEL_ERROR 10
#	define LOG_LEVEL_NOTICE 20 /* by default should be printed */
#	define LOG_LEVEL_INFO 40
#	define LOG_LEVEL_VERBOSE 50

#	if LOG_LEVEL >= LOG_LEVEL_ERROR
#		define ERROR(...) printf("ERROR: " __VA_ARGS__)
#	else
#		define ERROR(...)
#	endif

#	if LOG_LEVEL >= LOG_LEVEL_NOTICE
#		define NOTICE(...) printf("NOTICE: " __VA_ARGS__)
#	else
#		define NOTICE(...)
#	endif

#	if LOG_LEVEL >= LOG_LEVEL_INFO
#		define INFO(...) printf("INFO: " __VA_ARGS__)
#	else
#		define INFO(...)
#	endif

#	if LOG_LEVEL >= LOG_LEVEL_VERBOSE
#		define VERBOSE(...) printf("VERBOSE: " __VA_ARGS__)
#	else
#		define VERBOSE(...)
#	endif

void flush_dcache_range(long start, long size);
void udelay(uint32_t us);

/************************ for ATF ************************/
#elif defined(TRUSTED_BOARD_BOOT)
#	include <platform_def.h>
#	include <stdio.h>
#	include <string.h>
#	include <arch_helpers.h>
#	include <drivers/delay_timer.h>
#	include <common/debug.h>
#	include <endian.h>

#	define sihnah_memset memset
#	define sihnah_memcpy memcpy
#	define sihnah_flush_dcache_range(_start, _sz) flush_dcache_range(_start, _sz)

#	define __CPU_TO_BE32 be32toh
#	define __CPU_TO_BE16 be16toh
#	define __CPU_TO_LE32 le32toh
#	define __CPU_TO_LE16 le16toh
#	define __BE32_TO_CPU htobe32
#	define __BE16_TO_CPU htobe16
#	define __LE32_TO_CPU htole32
#	define __LE16_TO_CPU htole16

uint8_t *plat_get_sihnah_buf(void);

/************************ for U-Boot ************************/
#elif defined(__UBOOT__)
#	include <common.h>
#	include <compiler.h>
#	include <cpu_func.h>

#	define sihnah_memset memset
#	define sihnah_memcpy memcpy
#	define sihnah_flush_dcache_range(_start, _sz) flush_dcache_range(_start, _sz + _start)

#	define ERROR(...) pr_emerg("EE: " __VA_ARGS__)
#	define NOTICE(...) pr_warning("WW: " __VA_ARGS__) /* by default should be printed */
#	define INFO(...) pr_info("II: " __VA_ARGS__)
#	define VERBOSE(...) pr_debug("DD: " __VA_ARGS__)

#	define __CPU_TO_BE32 cpu_to_be32
#	define __CPU_TO_BE16 cpu_to_be16
#	define __CPU_TO_LE32 cpu_to_le32
#	define __CPU_TO_LE16 cpu_to_le16
#	define __BE32_TO_CPU be32_to_cpu
#	define __BE16_TO_CPU be16_to_cpu
#	define __LE32_TO_CPU le32_to_cpu
#	define __LE16_TO_CPU le16_to_cpu

#elif defined(__unix__)
#else
#endif

#endif

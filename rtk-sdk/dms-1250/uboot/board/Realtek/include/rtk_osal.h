/*
 * Realtek OSAL definitions for U-Boot
 */


#ifndef	__RTK_OSAL_H__
#define	__RTK_OSAL_H__

#ifdef  __UBOOT__

/*
 * Include Files
 */
#include <common.h>
#include <exports.h>

/*
 * Symbol Definition
 */
#ifndef OSAL_PRINTF
#define OSAL_PRINTF(x...)               printf(x)
#endif
#ifndef OSAL_DCACHE_FLUSH
#define OSAL_DCACHE_FLUSH(base, len)    mips_cache_flush_dcache((Tuint32)base, len)
#endif
#ifndef OSAL_MALLOC
#define OSAL_MALLOC(size)               malloc((Tint32)size)
#endif
#ifndef OSAL_MEMSET
#define OSAL_MEMSET(base, val, len)     memset(base, val, len)
#endif
#ifndef OSAL_UDELAY
#define OSAL_UDELAY(utime)              udelay(utime)   /* [FIXME] */
#endif
#ifndef OSAL_MDELAY
#define OSAL_MDELAY(mtime)              mdelay(mtime)   /* [FIXME] */
#endif
#ifndef OSAL_MEMCMP
#define OSAL_MEMCMP(src, dst, len)     memcmp(src, dst, len)
#endif
#endif  /* __UBOOT__ */

#endif	/* __RTK_OSAL_H__ */


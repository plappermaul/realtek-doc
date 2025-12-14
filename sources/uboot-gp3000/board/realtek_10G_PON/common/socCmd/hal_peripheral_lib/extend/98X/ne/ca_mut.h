#ifndef __CA_MUT_H__
#define __CA_MUT_H__

#include <linux/types.h>
//#include <linux/slab.h>
//#include <linux/skbuff.h>
#include <linux/rbtree.h>

#define CA_TRACE_MAX		16
#define CA_FILENAME_MAX		128

typedef enum {
	MUT_STATUS_TRACE,
	MUT_STATUS_UNTRACE,
} ca_mut_status_t;

typedef enum {
	MUT_TYPE_NONE,

	MUT_TYPE_MALLOC,	/* kmalloc, kzalloc */
	MUT_TYPE_SKB,		/* netdev_alloc_skb*, skb_clone, skb_copy */

	MUT_TYPE_ALL,
} ca_mut_type_t;

typedef struct {
	unsigned long		addr;			/* memory address */
	size_t			size;			/* allocated size when called; not accurate */
	ca_mut_status_t		status;
	ca_mut_type_t		type;
	char			filename[CA_FILENAME_MAX];
	int			line;			/* line number in file */
	unsigned long		trace[CA_TRACE_MAX];	/* call stack */
	unsigned int		trace_len;
	unsigned long		jiffies;
	struct rb_node		node;
} ca_mut_record_t;

/***** kmalloc family *****/

void *__ca_malloc(size_t size, gfp_t flags, const char *fname, int line, bool trace);
void __ca_free(void *ptr, const char *fname, int line);

#if 1//from yocto
#define ___GFP_ZERO		0x8000u
#define ___GFP_HIGH		0x20u
#define ___GFP_ATOMIC		0x80000u
#define ___GFP_KSWAPD_RECLAIM	0x2000000u

#define __GFP_ZERO	((__force gfp_t)___GFP_ZERO)
#define __GFP_HIGH	((__force gfp_t)___GFP_HIGH)
#define __GFP_KSWAPD_RECLAIM	((__force gfp_t)___GFP_KSWAPD_RECLAIM) /* kswapd can wake */
#define __GFP_ATOMIC	((__force gfp_t)___GFP_ATOMIC)

#define GFP_ATOMIC	(__GFP_HIGH|__GFP_ATOMIC|__GFP_KSWAPD_RECLAIM)
#endif

#define ca_malloc_trace(size, flags, trace)	__ca_malloc(size, flags, __FILE__, __LINE__, trace)
#define ca_zalloc_trace(size, flags, trace)	__ca_malloc(size, (flags)|__GFP_ZERO, __FILE__, __LINE__, trace)
#define ca_free_trace(ptr)			__ca_free(ptr, __FILE__, __LINE__)

/***** vmalloc family *****/

void *__ca_vmalloc(size_t size, const char *fname, int line, bool trace);
void __ca_vfree(void *ptr, const char *fname, int line);

#define ca_vmalloc_trace(size, trace)		__ca_vmalloc(size, __FILE__, __LINE__, trace)
#define ca_vfree_trace(ptr)			__ca_vfree(ptr, __FILE__, __LINE__)

/***** sk_buff family *****/

struct sk_buff *__ca_netdev_alloc_skb(struct net_device *dev, unsigned int size, const char *fname, int line, bool trace);
struct sk_buff *__ca_netdev_alloc_skb_ip_align(struct net_device *dev, unsigned int size, const char *fname, int line, bool trace);
struct sk_buff *__ca_alloc_skb(unsigned int size, gfp_t mask, const char *fname, int line, bool trace);
struct sk_buff *__ca_skb_copy(struct sk_buff *skb, gfp_t mask, const char *fname, int line, bool trace);
struct sk_buff *__ca_skb_clone(struct sk_buff *skb, gfp_t mask, const char *fname, int line, bool trace);
void __ca_free_skb(struct sk_buff *skb, const char *fname, int line);
void __ca_free_skb_any(struct sk_buff *skb, const char *fname, int line);
void __ca_drop_skb(struct sk_buff *skb, const char *fname, int line);
void __ca_skb_in(struct sk_buff *skb, const char *fname, int line, bool trace);
void __ca_skb_out(struct sk_buff *skb, const char *fname, int line);

#define ca_netdev_alloc_skb(dev, size, trace)			__ca_netdev_alloc_skb(dev, size, __FILE__, __LINE__, trace)
#define ca_netdev_alloc_skb_ip_align(dev, size, trace)		__ca_netdev_alloc_skb_ip_align(dev, size, __FILE__, __LINE__, trace)
#define ca_alloc_skb(size, mask, trace)				__ca_alloc_skb(size, mask, __FILE__, __LINE__, trace)
#define ca_skb_clone(skb, mask, trace)				__ca_skb_clone(skb, mask, __FILE__, __LINE__, trace)
#define ca_skb_copy(skb, mask, trace)				__ca_skb_copy(skb, mask, __FILE__, __LINE__, trace)
#define ca_skb_copy_expand(skb, head, tail, mask, trace)	__ca_skb_copy_expand(skb, head, tail, mask, __FILE__, __LINE__, trace)

#define ca_consume_skb(skb)					__ca_free_skb(skb, __FILE__, __LINE__)
#define ca_dev_kfree_skb(skb)					__ca_free_skb(skb, __FILE__, __LINE__)
#define ca_dev_kfree_skb_any(skb)				__ca_free_skb_any(skb, __FILE__, __LINE__)
#define ca_kfree_skb(skb)					__ca_drop_skb(skb, __FILE__, __LINE__)

#define ca_skb_in(skb, trace)					__ca_skb_in(skb, __FILE__, __LINE__, trace)
#define ca_skb_out(skb)						__ca_skb_out(skb, __FILE__, __LINE__)

/*************************** proc entry ***************************/

extern const struct file_operations fops_ne_memtrace;

int mut_ne_memtrace_proc_open(struct inode *inode, struct file *file);
ssize_t mut_ne_memtrace_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *pos);

/*************************** initialization ***************************/

int ca_mut_init(void);

#endif /* __CA_MUT_H__ */

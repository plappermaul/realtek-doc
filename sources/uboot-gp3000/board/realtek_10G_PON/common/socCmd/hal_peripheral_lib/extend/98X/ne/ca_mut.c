/*
 * ca_mut.c: Memory Usage Tracing for Cortina NE driver
 *
 */
#include <generated/ca_ne_autoconf.h>
#if 0//yocto
#include <linux/version.h>
#else//sd1 uboot
#include "aal_common.h"
#endif

#include <linux/types.h>
//#include <linux/netdevice.h>
//#include <linux/vmalloc.h>
#include <linux/string.h>
//#include <linux/rbtree.h>
//#include <linux/jiffies.h>
//#include <linux/stacktrace.h>
//#include <linux/proc_fs.h>
//#include <linux/seq_file.h>
//#include <linux/rwlock.h>
//#include <linux/sched.h>
#include "ca_mut.h"
#include <malloc.h>

#if 0//yocto
#if defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
#if !defined(CONFIG_STACKTRACE)
#error "ERROR! CONFIG_STACKTRACE is required for MUT advanced mode. Please enable Stack Trace in Linux config."
#endif
#endif

static DEFINE_RWLOCK(mut_lock);
struct rb_root mut_tree = RB_ROOT;

struct kmem_cache *cache_16 = NULL;	/* cache for 16-byte objects */
int verbose = 0;

atomic_t ca_cnt_malloc = ATOMIC_INIT(0);
atomic_t ca_cnt_skb = ATOMIC_INIT(0);
atomic_t ca_cnt_untrace = ATOMIC_INIT(0);

/*************************** RB-tree functions ***************************/

static ca_mut_record_t *mut_tree_search(struct rb_root *root, unsigned long addr)
{
	struct rb_node *node = root->rb_node;
	ca_mut_record_t *record;

	while (node) {
		record = container_of(node, ca_mut_record_t, node);

		if (addr < record->addr)
			node = node->rb_left;
		else if (addr > record->addr)
			node = node->rb_right;
		else
			return record;
	}

	return NULL;
}

static bool mut_tree_insert(struct rb_root *root, ca_mut_record_t *record)
{
	struct rb_node **new = &(root->rb_node), *parent = NULL;
	ca_mut_record_t *this;

	/* Figure out where to put new node */
	while (*new) {
		this = container_of(*new, ca_mut_record_t, node);
		parent = *new;

		if (record->addr < this->addr)
			new = &((*new)->rb_left);
		else if (record->addr > this->addr)
			new = &((*new)->rb_right);
		else
			return false;
	}

	/* Add new node and rebalance tree. */
	rb_link_node(&record->node, parent, new);
	rb_insert_color(&record->node, root);

	return true;
}

/*************************** internal functions ***************************/

#if defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
static int __save_stack_trace(unsigned long *trace)
{
	struct stack_trace stack_trace;

	stack_trace.max_entries = CA_TRACE_MAX;
	stack_trace.nr_entries = 0;
	stack_trace.entries = trace;
	stack_trace.skip = 2;
	save_stack_trace(&stack_trace);

	return stack_trace.nr_entries;
}
#endif

#if defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
static void __insert_record(unsigned long addr, size_t size, ca_mut_type_t type, const char *fname, int line, bool trace)
{
	ca_mut_record_t *record = NULL, *tmp = NULL;

	record = kmalloc(sizeof(ca_mut_record_t), GFP_KERNEL);
	if (record == NULL) {
		printk(KERN_ERR "%s: Fail to allocate memory for MUT record.\n", __func__);
		printk(KERN_ERR "%s:   addr=0x%08lx, size=%zu file=%s:%d\n", __func__, addr, size, fname, line);
	}

	record->addr = addr;
	record->size = size;
	record->status = trace ? MUT_STATUS_TRACE : MUT_STATUS_UNTRACE;
	record->type = type;
	strncpy(record->filename, fname, CA_FILENAME_MAX);
	record->line = line;
	record->trace_len = __save_stack_trace(record->trace);
	record->jiffies = jiffies;

	if (mut_tree_insert(&mut_tree, record) != true) {
		printk(KERN_ERR "%s: ERROR! Same record is inserted already.\n", __func__);
		printk(KERN_ERR "%s:   new data: addr=0x%08lx size=%zu file=%s:%d\n", __func__, addr, size, fname, line);

		tmp = mut_tree_search(&mut_tree, addr);
		if (tmp != NULL)
			printk(KERN_ERR "%s:   orig data: addr=0x%08lx size=%zu file=%s:%d\n", __func__, tmp->addr, tmp->size, tmp->filename, tmp->line);
		else
			printk(KERN_ERR "%s:   orig data: CAN NOT FOUND!\n", __func__);

	} else {
		if (trace == false)
			atomic_inc(&ca_cnt_untrace);
		else if (type == MUT_TYPE_MALLOC)
			atomic_inc(&ca_cnt_malloc);
		else if (type == MUT_TYPE_SKB)
			atomic_inc(&ca_cnt_skb);
	}
}
#endif

static void mut_tree_dump(void)
{
	struct rb_node *node;
	ca_mut_record_t *record;
	unsigned long f;

	printk(KERN_INFO "Dump MUT RB-tree:\n");

	read_lock_irqsave(&mut_lock, f);
	for (node = rb_first(&mut_tree); node; node = rb_next(node)) {
		record = container_of(node, ca_mut_record_t, node);
		printk(KERN_INFO "  addr=%08lx size=%zu filename=%s:%d\n", record->addr, record->size, record->filename, record->line);
		cond_resched();
	}
	read_unlock_irqrestore(&mut_lock, f);
}
#endif//yocto

/*************************** kmalloc family ***************************/

void *__ca_malloc(size_t size, gfp_t flags, const char *fname, int line, bool trace)
{
        void *ptr = NULL;
        unsigned long f;

#if CONFIG_98F_UBOOT_NE_DBG
        printf("\t%s(%d)\n", __FUNCTION__, __LINE__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

#if 0//yocto
#if defined(CONFIG_CA_NE_SMALL_SLAB)
	if (size <= 16)
		ptr = kmem_cache_alloc(cache_16, flags);
	else
		ptr = kmalloc(size, flags);
#else
	ptr = kmalloc(size, flags);
#endif
#else//sd1 uboot for 98f
        ptr = malloc(size);

#if CONFIG_98F_UBOOT_NE_DBG
        printf("\t%s(%d) ptr: 0x%x\n", __FUNCTION__, __LINE__, ptr);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

#endif//sd1 uboot for 98f

	if (ptr == NULL) return NULL;

#if defined(CONFIG_CA_NE_MUT_TRACE_MALLOC)
	if (verbose)
		printk(KERN_INFO "KMALLOC: addr 0x%pK, size %zu, file %s:%d\n", ptr, size, fname, line);

#if defined(CONFIG_CA_NE_MUT_NORMAL_MODE)
	atomic_inc(&ca_cnt_malloc);
#elif defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
	write_lock_irqsave(&mut_lock, f);
	__insert_record((uintptr_t) ptr, size, MUT_TYPE_MALLOC, fname, line, trace);
	write_unlock_irqrestore(&mut_lock, f);
#endif
#endif /* CONFIG_CA_NE_MUT_TRACE_MALLOC */

        return ptr;
}

void __ca_free(void *ptr, const char *fname, int line)
{
        ca_mut_record_t *record = NULL;
        unsigned long f;

#if CONFIG_98F_UBOOT_NE_DBG
        printf("\t%s(%d)\n", __FUNCTION__, __LINE__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

#if defined(CONFIG_CA_NE_MUT_TRACE_MALLOC)
	if (verbose)
		printk(KERN_INFO "KFREE: addr 0x%pK, file %s:%d\n", ptr, fname, line);

#if defined(CONFIG_CA_NE_MUT_NORMAL_MODE)
	atomic_dec(&ca_cnt_malloc);
#elif defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
	write_lock_irqsave(&mut_lock, f);
	record = mut_tree_search(&mut_tree, (uintptr_t) ptr);
	if (record != NULL) {
		if (record->status == MUT_STATUS_UNTRACE)
			atomic_dec(&ca_cnt_untrace);
		else if (record->type == MUT_TYPE_MALLOC)
			atomic_dec(&ca_cnt_malloc);
		else if (record->type == MUT_TYPE_SKB)	// impossible condition
			atomic_dec(&ca_cnt_skb);

		rb_erase(&record->node, &mut_tree);
		kfree(record);
	} else {
		printk(KERN_WARNING "%s: WARNING! MUT free function is called but the data is not recorded by MUT.\n", __func__);
		printk(KERN_WARNING "%s:   addr=0x%pK, filename=%s:%d\n", __func__, ptr, fname, line);
	}
	write_unlock_irqrestore(&mut_lock, f);
#endif
#endif /* CONFIG_CA_NE_MUT_TRACE_MALLOC */

#if 0//yocto
	kfree(ptr);
#else//sd1 uboot for 98f

#if CONFIG_98F_UBOOT_NE_DBG
        printf("\t%s(%d) ptr: 0x%x\n", __FUNCTION__, __LINE__, ptr);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

        free(ptr);
#endif

}

#if 0//yocto
/*************************** vmalloc family ***************************/

void *__ca_vmalloc(size_t size, const char *fname, int line, bool trace)
{
	void *ptr = NULL;
	unsigned long f;

	ptr = vmalloc(size);
	if (ptr == NULL) return NULL;

#if defined(CONFIG_CA_NE_MUT_TRACE_MALLOC)
	if (verbose)
		printk(KERN_INFO "VMALLOC: addr 0x%pK, size %zu, file %s:%d\n", ptr, size, fname, line);

#if defined(CONFIG_CA_NE_MUT_NORMAL_MODE)
	atomic_inc(&ca_cnt_malloc);
#elif defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
	write_lock_irqsave(&mut_lock, f);
	__insert_record((uintptr_t) ptr, size, MUT_TYPE_MALLOC, fname, line, trace);
	write_unlock_irqrestore(&mut_lock, f);
#endif
#endif /* CONFIG_CA_NE_MUT_TRACE_MALLOC */

	return ptr;
}

void __ca_vfree(void *ptr, const char *fname, int line)
{
	ca_mut_record_t *record = NULL;
	unsigned long f;

#if defined(CONFIG_CA_NE_MUT_TRACE_MALLOC)
	if (verbose)
		printk(KERN_INFO "VFREE: addr 0x%pK, file %s:%d\n", ptr, fname, line);

#if defined(CONFIG_CA_NE_MUT_NORMAL_MODE)
	atomic_dec(&ca_cnt_malloc);
#elif defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
	write_lock_irqsave(&mut_lock, f);
	record = mut_tree_search(&mut_tree, (uintptr_t) ptr);
	if (record != NULL) {
		if (record->status == MUT_STATUS_UNTRACE)
			atomic_dec(&ca_cnt_untrace);
		else if (record->type == MUT_TYPE_MALLOC)
			atomic_dec(&ca_cnt_malloc);
		else if (record->type == MUT_TYPE_SKB)	// impossible condition
			atomic_dec(&ca_cnt_skb);

		rb_erase(&record->node, &mut_tree);
		kfree(record);
	} else {
		printk(KERN_WARNING "%s: WARNING! MUT free function is called but the data is not recorded by MUT.\n", __func__);
		printk(KERN_WARNING "%s:   addr=0x%pK, filename=%s:%d\n", __func__, ptr, fname, line);
	}
	write_unlock_irqrestore(&mut_lock, f);
#endif
#endif /* CONFIG_CA_NE_MUT_TRACE_MALLOC */

	vfree(ptr);
}


/*************************** sk_buff family ***************************/

struct sk_buff *__ca_netdev_alloc_skb(struct net_device *dev, unsigned int size, const char *fname, int line, bool trace)
{
	struct sk_buff *skb = NULL;
	unsigned long f;

	skb = netdev_alloc_skb(dev, size);
	if (skb == NULL) return NULL;

#if defined(CONFIG_CA_NE_MUT_TRACE_SKB)
	if (verbose)
		printk(KERN_INFO "SKB_ALLOC: addr 0x%pK, size %u, file %s:%d\n", skb, size, fname, line);

#if defined(CONFIG_CA_NE_MUT_NORMAL_MODE)
	atomic_inc(&ca_cnt_skb);
#elif defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
	write_lock_irqsave(&mut_lock, f);
	__insert_record((uintptr_t) skb, size, MUT_TYPE_SKB, fname, line, trace);
	write_unlock_irqrestore(&mut_lock, f);
#endif
#endif /* CONFIG_CA_NE_MUT_TRACE_SKB */

	return skb;
}

struct sk_buff *__ca_netdev_alloc_skb_ip_align(struct net_device *dev, unsigned int size, const char *fname, int line, bool trace)
{
	struct sk_buff *skb = NULL;
	unsigned long f;

	skb = netdev_alloc_skb_ip_align(dev, size);
	if (skb == NULL) return NULL;

#if defined(CONFIG_CA_NE_MUT_TRACE_SKB)
	if (verbose)
		printk(KERN_INFO "SKB_ALLOC: addr 0x%pK, size %u, file %s:%d\n", skb, size, fname, line);

#if defined(CONFIG_CA_NE_MUT_NORMAL_MODE)
	atomic_inc(&ca_cnt_skb);
#elif defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
	write_lock_irqsave(&mut_lock, f);
	__insert_record((uintptr_t) skb, size, MUT_TYPE_SKB, fname, line, trace);
	write_unlock_irqrestore(&mut_lock, f);
#endif
#endif /* CONFIG_CA_NE_MUT_TRACE_SKB */

	return skb;
}

struct sk_buff *__ca_alloc_skb(unsigned int size, gfp_t mask, const char *fname, int line, bool trace)
{
	struct sk_buff *skb = NULL;
	unsigned long f;

	skb = alloc_skb(size, mask);
	if (skb == NULL) return NULL;

#if defined(CONFIG_CA_NE_MUT_TRACE_SKB)
	if (verbose)
		printk(KERN_INFO "SKB_ALLOC: addr 0x%pK, size %u, file %s:%d\n", skb, size, fname, line);

#if defined(CONFIG_CA_NE_MUT_NORMAL_MODE)
	atomic_inc(&ca_cnt_skb);
#elif defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
	write_lock_irqsave(&mut_lock, f);
	__insert_record((uintptr_t) skb, size, MUT_TYPE_SKB, fname, line, trace);
	write_unlock_irqrestore(&mut_lock, f);
#endif
#endif /* CONFIG_CA_NE_MUT_TRACE_SKB */

	return skb;
}

struct sk_buff *__ca_skb_clone(struct sk_buff *skb, gfp_t mask, const char *fname, int line, bool trace)
{
	struct sk_buff *cskb = NULL;
	ca_mut_record_t *record;
	size_t size = 0;
	unsigned long f;

	cskb = skb_clone(skb, mask);
	if (cskb == NULL) return NULL;

#if defined(CONFIG_CA_NE_MUT_TRACE_SKB)
	if (verbose)
		printk(KERN_INFO "SKB_CLONE: addr 0x%pK, size %zu, file %s:%d\n", cskb, size, fname, line);

#if defined(CONFIG_CA_NE_MUT_NORMAL_MODE)
	atomic_inc(&ca_cnt_skb);
#elif defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
	write_lock_irqsave(&mut_lock, f);
	record = mut_tree_search(&mut_tree, (uintptr_t) skb);
	if (record == NULL) {
		printk(KERN_WARNING "%s: WARNING! Can not find skb in MUT record.\n", __func__);
		size = skb->truesize; /* not match to the parameter size when allocating skb */
	} else {
		size = record->size;
	}
	__insert_record((uintptr_t) cskb, size, MUT_TYPE_SKB, fname, line, trace);
	write_unlock_irqrestore(&mut_lock, f);
#endif
#endif /* CONFIG_CA_NE_MUT_TRACE_SKB */

	return cskb;
}

struct sk_buff *__ca_skb_copy(struct sk_buff *skb, gfp_t mask, const char *fname, int line, bool trace)
{
	struct sk_buff *cskb = NULL;
	ca_mut_record_t *record;
	size_t size = 0;
	unsigned long f;

	cskb = skb_copy(skb, mask);
	if (cskb == NULL) return NULL;

#if defined(CONFIG_CA_NE_MUT_TRACE_SKB)
	if (verbose)
		printk(KERN_INFO "SKB_COPY: addr 0x%pK, size %zu, file %s:%d\n", cskb, size, fname, line);

#if defined(CONFIG_CA_NE_MUT_NORMAL_MODE)
	atomic_inc(&ca_cnt_skb);
#elif defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
	write_lock_irqsave(&mut_lock, f);
	record = mut_tree_search(&mut_tree, (uintptr_t) skb);
	if (record == NULL) {
		printk(KERN_WARNING "%s: WARNING! Can not find skb in MUT record.\n", __func__);
		size = skb->truesize; /* not match to the parameter size when allocating skb */
	} else {
		size = record->size;
	}
	__insert_record((uintptr_t) cskb, size, MUT_TYPE_SKB, fname, line, trace);
	write_unlock_irqrestore(&mut_lock, f);
#endif
#endif /* CONFIG_CA_NE_MUT_TRACE_SKB */

	return cskb;
}

struct sk_buff *__ca_skb_copy_expand(struct sk_buff *skb, int head, int tail, gfp_t mask, const char *fname, int line, bool trace)
{
	struct sk_buff *cskb = NULL;
	ca_mut_record_t *record;
	size_t size = 0;
	unsigned long f;

	if (!skb) return NULL;

	cskb = skb_copy_expand(skb, head, tail, mask);
	if (cskb == NULL) return NULL;

#if defined(CONFIG_CA_NE_MUT_TRACE_SKB)
	if (verbose)
		printk(KERN_INFO "SKB_COPY: addr 0x%pK, size %zu, file %s:%d\n", cskb, size, fname, line);

#if defined(CONFIG_CA_NE_MUT_NORMAL_MODE)
	atomic_inc(&ca_cnt_skb);
#elif defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
	write_lock_irqsave(&mut_lock, f);
	record = mut_tree_search(&mut_tree, (uintptr_t) skb);
	if (record == NULL) {
		printk(KERN_WARNING "%s: WARNING! Can not find skb in MUT record.\n", __func__);
		size = skb->truesize; /* not match to the parameter size when allocating skb */
	} else {
		size = record->size;
	}
	__insert_record((uintptr_t) cskb, size, MUT_TYPE_SKB, fname, line, trace);
	write_unlock_irqrestore(&mut_lock, f);
#endif
#endif /* CONFIG_CA_NE_MUT_TRACE_SKB */

	return cskb;
}

void __ca_free_skb(struct sk_buff *skb, const char *fname, int line)
{
	ca_mut_record_t *record = NULL;
	unsigned long f;

	if (!skb) return;

#if defined(CONFIG_CA_NE_MUT_TRACE_SKB)
	if (verbose)
		printk(KERN_INFO "SKB_FREE: addr 0x%pK, file %s:%d\n", skb, fname, line);

#if defined(CONFIG_CA_NE_MUT_NORMAL_MODE)
	atomic_dec(&ca_cnt_skb);
#elif defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
	write_lock_irqsave(&mut_lock, f);
	record = mut_tree_search(&mut_tree, (uintptr_t) skb);
	if (record != NULL) {
		if (record->status == MUT_STATUS_UNTRACE)
			atomic_dec(&ca_cnt_untrace);
		else if (record->type == MUT_TYPE_SKB)
			atomic_dec(&ca_cnt_skb);
		else if (record->type == MUT_TYPE_MALLOC)	// impossible condition
			atomic_dec(&ca_cnt_malloc);

		rb_erase(&record->node, &mut_tree);
		kfree(record);
	} else {
		printk(KERN_WARNING "%s: WARNING! MUT free function is called but the data is not recorded by MUT.\n", __func__);
		printk(KERN_WARNING "%s:   addr=0x%pK, filename=%s:%d\n", __func__, skb, fname, line);
	}
	write_unlock_irqrestore(&mut_lock, f);
#endif
#endif /* CONFIG_CA_NE_MUT_TRACE_SKB */

	dev_kfree_skb(skb);
}

void __ca_free_skb_any(struct sk_buff *skb, const char *fname, int line)
{
	ca_mut_record_t *record = NULL;
	unsigned long f;

	if (!skb) return;

#if defined(CONFIG_CA_NE_MUT_TRACE_SKB)
	if (verbose)
		printk(KERN_INFO "SKB_FREE: addr 0x%pK, file %s:%d\n", skb, fname, line);

#if defined(CONFIG_CA_NE_MUT_NORMAL_MODE)
	atomic_dec(&ca_cnt_skb);
#elif defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
	write_lock_irqsave(&mut_lock, f);
	record = mut_tree_search(&mut_tree, (uintptr_t) skb);
	if (record != NULL) {
		if (record->status == MUT_STATUS_UNTRACE)
			atomic_dec(&ca_cnt_untrace);
		else if (record->type == MUT_TYPE_SKB)
			atomic_dec(&ca_cnt_skb);
		else if (record->type == MUT_TYPE_MALLOC)	// impossible condition
			atomic_dec(&ca_cnt_malloc);

		rb_erase(&record->node, &mut_tree);
		kfree(record);
	} else {
		printk(KERN_WARNING "%s: WARNING! MUT free function is called but the data is not recorded by MUT.\n", __func__);
		printk(KERN_WARNING "%s:   addr=0x%pK, filename=%s:%d\n", __func__, skb, fname, line);
	}
	write_unlock_irqrestore(&mut_lock, f);
#endif
#endif /* CONFIG_CA_NE_MUT_TRACE_SKB */

	dev_kfree_skb_any(skb);
}

void __ca_drop_skb(struct sk_buff *skb, const char *fname, int line)
{
	ca_mut_record_t *record = NULL;
	unsigned long f;

	if (!skb) return;

#if defined(CONFIG_CA_NE_MUT_TRACE_SKB)
	if (verbose)
		printk(KERN_INFO "SKB_DROP: addr 0x%pK, file %s:%d\n", skb, fname, line);

#if defined(CONFIG_CA_NE_MUT_NORMAL_MODE)
	atomic_dec(&ca_cnt_skb);
#elif defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
	write_lock_irqsave(&mut_lock, f);
	record = mut_tree_search(&mut_tree, (uintptr_t) skb);
	if (record != NULL) {
		if (record->status == MUT_STATUS_UNTRACE)
			atomic_dec(&ca_cnt_untrace);
		else if (record->type == MUT_TYPE_SKB)
			atomic_dec(&ca_cnt_skb);
		else if (record->type == MUT_TYPE_MALLOC)	// impossible condition
			atomic_dec(&ca_cnt_malloc);

		rb_erase(&record->node, &mut_tree);
		kfree(record);
	} else {
		printk(KERN_WARNING "%s: WARNING! MUT free function is called but the data is not recorded by MUT.\n", __func__);
		printk(KERN_WARNING "%s:   addr=0x%pK, filename=%s:%d\n", __func__, skb, fname, line);
	}
	write_unlock_irqrestore(&mut_lock, f);
#endif
#endif /* CONFIG_CA_NE_MUT_TRACE_SKB */

	kfree_skb(skb);
}

void __ca_skb_in(struct sk_buff *skb, const char *fname, int line, bool trace)
{
#if defined(CONFIG_CA_NE_MUT_TRACE_SKB)
	unsigned long f;

	if (!skb) return;

	if (verbose)
		printk(KERN_INFO "SKB_IN: addr 0x%pK, file %s:%d\n", skb, fname, line);

#if defined(CONFIG_CA_NE_MUT_NORMAL_MODE)
	atomic_inc(&ca_cnt_skb);
#elif defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
	write_lock_irqsave(&mut_lock, f);
	__insert_record((uintptr_t) skb, skb->truesize, MUT_TYPE_SKB, fname, line, trace);
	write_unlock_irqrestore(&mut_lock, f);
#endif
#endif /* CONFIG_CA_NE_MUT_TRACE_SKB */
}

void __ca_skb_out(struct sk_buff *skb, const char *fname, int line)
{
#if defined(CONFIG_CA_NE_MUT_TRACE_SKB)
	ca_mut_record_t *record = NULL;
	unsigned long f;

	if (!skb) return;

	if (verbose)
		printk(KERN_INFO "SKB_OUT: addr 0x%pK, file %s:%d\n", skb, fname, line);

#if defined(CONFIG_CA_NE_MUT_NORMAL_MODE)
	atomic_dec(&ca_cnt_skb);
#elif defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
	write_lock_irqsave(&mut_lock, f);
	record = mut_tree_search(&mut_tree, (uintptr_t) skb);
	if (record != NULL) {
		if (record->status == MUT_STATUS_UNTRACE)
			atomic_dec(&ca_cnt_untrace);
		else if (record->type == MUT_TYPE_SKB)
			atomic_dec(&ca_cnt_skb);
		else if (record->type == MUT_TYPE_MALLOC)	// impossible condition
			atomic_dec(&ca_cnt_malloc);

		rb_erase(&record->node, &mut_tree);
		kfree(record);
	} else {
		printk(KERN_WARNING "%s: WARNING! The data is not recorded by MUT.\n", __func__);
		printk(KERN_WARNING "%s:   addr=0x%pK, filename=%s:%d\n", __func__, skb, fname, line);
	}
	write_unlock_irqrestore(&mut_lock, f);
#endif
#endif /* CONFIG_CA_NE_MUT_TRACE_SKB */
}

/*************************** proc entry - ne_memtrace  ***************************/

#define NE_MEMTRACE_HELP_MSG_NORMAL \
	"\n" \
	"# cat ne_memtrace\n" \
	"  Show this help message.\n" \
	"\n" \
	"# echo show > ne_memtrace\n" \
	"  Show counter(s).\n" \
	"\n" \
	"# echo verbose > ne_memtrace\n" \
	"  Enable verbose. Console shows each memory alocation/free action.\n" \
	"\n" \
	"# echo quiet > ne_memtrace\n" \
	"  Disable verbose.\n" \
	"\n"

#define NE_MEMTRACE_HELP_MSG_ADVANCED \
	"\n" \
	"# cat ne_memtrace\n" \
	"  Show this help message.\n" \
	"\n" \
	"# echo show > ne_memtrace\n" \
	"  Show counter(s).\n" \
	"\n" \
	"# echo clear > ne_memtrace\n" \
	"  Do NOT trace allocated objects of MALLOC and SKB types.\n" \
	"  The counter of MALLOC and SKB will be moved to UNTRACE.\n" \
	"\n" \
	"# echo dump all > ne_memtrace\n" \
	"  Dump all tracing memory allocation records.\n" \
	"  NOTE: The size info may not accurate.\n" \
	"\n" \
	"# echo dump malloc > ne_memtrace\n" \
	"  Dump tracing memory allocation records of MALLOC type. (kmalloc, kzalloc, vmalloc).\n" \
	"\n" \
	"# echo dump skb > ne_memtrace\n" \
	"  Dump tracing memory allocation records of SKB type. (netdev_alloc_skb, skb_clone, skb_copy, ...).\n" \
	"\n" \
	"# echo dump size MIN MAX > ne_memtrace\n" \
	"  Dump all tracing memory allocation objects which size are between MIN and MAX.\n" \
	"\n" \
	"# echo verbose > ne_memtrace\n" \
	"  Enable verbose. Console shows each memory alocation/free action.\n" \
	"\n" \
	"# echo quiet > ne_memtrace\n" \
	"  Disable verbose.\n" \
	"\n"

struct proc_dir_entry *proc_ne_memtrace = NULL;

static int mut_proc_show(void)
{
#if defined(CONFIG_CA_NE_MUT_TRACE_MALLOC)
	printk(KERN_INFO "counter (malloc) = %d\n", atomic_read(&ca_cnt_malloc));
#endif

#if defined(CONFIG_CA_NE_MUT_TRACE_SKB)
	printk(KERN_INFO "counter (skb) = %d\n", atomic_read(&ca_cnt_skb));
#endif

#if defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
	printk(KERN_INFO "counter (untrace) = %d\n", atomic_read(&ca_cnt_untrace));
#endif
	return 0;
}

#if defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
static int mut_proc_clear(void)
{
	struct rb_node *node;
	ca_mut_record_t *record;
	unsigned long f;

	write_lock_irqsave(&mut_lock, f);
	for (node = rb_first(&mut_tree); node; node = rb_next(node)) {
		record = container_of(node, ca_mut_record_t, node);
		if (record->status == MUT_STATUS_UNTRACE) continue;
		if (record->type == MUT_TYPE_MALLOC) {
			atomic_dec(&ca_cnt_malloc);
			atomic_inc(&ca_cnt_untrace);
			record->status = MUT_STATUS_UNTRACE;
		} else if (record->type == MUT_TYPE_SKB) {
			atomic_dec(&ca_cnt_skb);
			atomic_inc(&ca_cnt_untrace);
			record->status = MUT_STATUS_UNTRACE;
		}
	}
	write_unlock_irqrestore(&mut_lock, f);

	return 0;
}
#endif

static int mut_proc_dump(ca_mut_type_t type, size_t _min, size_t _max)
{
	struct rb_node *node;
	ca_mut_record_t *record;
	unsigned int mseca_age;
	int i;
	unsigned long f;

	if (_min > _max) return -1;

	read_lock_irqsave(&mut_lock, f);
	for (node = rb_first(&mut_tree); node; node = rb_next(node)) {
		record = container_of(node, ca_mut_record_t, node);

		if (record->status == MUT_STATUS_UNTRACE)
			continue;

		if ( ((_min != 0) || (_max != 0)) && ((record->size < _min) || (record->size > _max)) )
			continue;

		if ((type != MUT_TYPE_ALL) && (record->type != type))
			continue;

		mseca_age = jiffies_to_msecs(jiffies - record->jiffies);

		printk(KERN_INFO "object 0x%08lx, size %zu, jiffies %lu (age %d.%03ds): \n",
		    record->addr, record->size, record->jiffies, mseca_age / 1000, mseca_age % 1000);

		printk(KERN_INFO "  type ");
		switch (record->type) {
		case MUT_TYPE_MALLOC:
			printk(KERN_CONT "MALLOC"); break;
		case MUT_TYPE_SKB:
			printk(KERN_CONT "SKB"); break;
		case MUT_TYPE_NONE:
		case MUT_TYPE_ALL:
		default:
			break;
		}
		printk(KERN_CONT "\n");

		printk(KERN_INFO "  filename %s:%d\n", record->filename, record->line);

		printk(KERN_INFO "  backtrace:\n");
		for (i = 0; i < record->trace_len; i++) {
			void *ptr = (void *) record->trace[i];
			printk(KERN_INFO "    [<%p>] %pS\n", ptr, ptr);
		}

		cond_resched();
	}
	read_unlock_irqrestore(&mut_lock, f);

	return 0;
}

/* cmd examples:
 *   dump all
 *   dump malloc
 *   dump skb
 *   dump size MIN MAX
 */
static int mut_proc_parse_dump(char *cmd)
{
	char *cp;
	ca_mut_type_t type = MUT_TYPE_NONE;
	unsigned long _min = 0, _max = 0;
	int ret;

	/* next token: dump */
	cp = strsep(&cmd, " ");

	/* next possible tokens: all, malloc, skb, size */
	if ((cp = strsep(&cmd, " ")) != NULL) {
		if (strncmp(cp, "all", 3) == 0) {

			type = MUT_TYPE_ALL;

		} else if (strncmp(cp, "malloc", 6) == 0) {

			type = MUT_TYPE_MALLOC;

		} else if (strncmp(cp, "skb", 3) == 0) {

			type = MUT_TYPE_SKB;

		} else if (strncmp(cp, "size", 4) == 0) {

			type = MUT_TYPE_ALL;

			/* next token: MIN of recorded size */
			if ((cp = strsep(&cmd, " ")) == NULL)
				return -EINVAL;

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,14,35)
			if (strict_strtoul(cp, 0, &_min))
#else
			if (kstrtoul(cp, 0, &_min))
#endif
				return -EINVAL;

			/* next token: MAX of recorded size */
			if ((cp = strsep(&cmd, " ")) == NULL)
				return -EINVAL;

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,14,35)
			if (strict_strtoul(cp, 0, &_max))
#else
			if (kstrtoul(cp, 0, &_max))
#endif
				return -EINVAL;

			if (_min > _max) return -EINVAL;

		} else {

			printk(KERN_INFO "Unknown command.\n");
			return -EINVAL;

		}
	}

	ret = mut_proc_dump(type, (size_t) _min, (size_t) _max);

	return ret;
}

static int ni_debug_show_help(struct seq_file *m, void *v)
{
	seq_printf(m, "Memory Usage Tracing for Cortina BSP\n");
#if defined(CONFIG_CA_NE_MUT_NORMAL_MODE)
	seq_printf(m, " * Current is Normal Mode.\n");
#elif defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
	seq_printf(m, " * Current is Advanced Mode.\n");
#endif

	if (verbose)
		seq_printf(m, " * Verbose is enabled.\n");
	else
		seq_printf(m, " * Verbose is disabled.\n");

#if defined(CONFIG_CA_NE_MUT_NORMAL_MODE)
	seq_printf(m, NE_MEMTRACE_HELP_MSG_NORMAL);
#elif defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
	seq_printf(m, NE_MEMTRACE_HELP_MSG_ADVANCED);
#endif

	return 0;
}

int mut_ne_memtrace_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ni_debug_show_help, (void *) file);
}

ssize_t mut_ne_memtrace_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *pos)
{
	char tmp[64];
	ssize_t len;

	len = min((unsigned long) count, (unsigned long)(sizeof(tmp) - 1));
	if (copy_from_user(tmp, buf, len))
		return -EFAULT;
	tmp[len] = '\0';

	if (strncmp(tmp, "show", 4) == 0) {
		mut_proc_show();
	} else if (strncmp(tmp, "verbose", 7) == 0) {
		verbose = 1;
	} else if (strncmp(tmp, "quiet", 5) == 0) {
		verbose = 0;
	}
#if defined(CONFIG_CA_NE_MUT_ADVANCED_MODE)
	else if (strncmp(tmp, "clear", 5) == 0) {
		mut_proc_clear();
	} else if (strncmp(tmp, "dump", 4) == 0) {
		mut_proc_parse_dump(tmp);
	}
#endif
	else {
		printk(KERN_INFO "Unknown command.\n");
		return -EINVAL;
	}

	return count;
}

const struct file_operations fops_ne_memtrace __read_mostly = {
	.owner		= THIS_MODULE,
	.open		= mut_ne_memtrace_proc_open,
	.read		= seq_read,
	.write		= mut_ne_memtrace_proc_write,
};

/*************************** initialization ***************************/

int ca_mut_init(void)
{
#if defined(CONFIG_CA_NE_SMALL_SLAB)
	cache_16 = kmem_cache_create("cortina_cache_16", 16, 0, SLAB_PANIC, NULL);
	if (cache_16 == NULL) {
		printk(KERN_INFO "ERROR! Fail to create cache for 16-byte objects!\n");
		return -1;
	}
#endif

	return 0;
}

#endif//yocto


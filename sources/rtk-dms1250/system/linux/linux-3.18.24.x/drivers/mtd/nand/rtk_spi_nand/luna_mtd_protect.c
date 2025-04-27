/*
 * =====================================================================================
 *
 *       Filename:  luna_mtd_protect.c
 *
 *    Description:
 *
 *        Version:
 *        Created:
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:
 *        Company:
 *
 * =====================================================================================
 */


#include <linux/module.h>



#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include "luna_mtd_protect.h"

uint32_t luna_mtd_protection_top = LUNA_MTD_PROTECTED_BLOCK_DEFAULT_TOP;

uint32_t
luna_mtd_get_blk_protection_top(void)
{
	return luna_mtd_protection_top;
}

void
luna_mtd_set_blk_protection_top(uint32_t new_top_value)
{
	luna_mtd_protection_top = new_top_value;
}

// mtd_protect proc
#define BUFFER_SIZE 100
static int
mtd_protect_proc_w(struct file *f, const char __user * buffer,
		size_t count, loff_t * ofs)
{
	uint8_t tmpbuf[BUFFER_SIZE];
	size_t copy_len = count > (BUFFER_SIZE-1) ? (BUFFER_SIZE-1) : count;
	if (buffer && !copy_from_user(tmpbuf, buffer, copy_len)) {
		tmpbuf[copy_len] = '\0';
		if (sscanf(tmpbuf, "%d", &luna_mtd_protection_top) <= 0) {
			luna_mtd_protection_top = LUNA_MTD_PROTECTED_BLOCK_DEFAULT_TOP;
		} else {
			// do nothing
		}
	}
	return copy_len;
}
#undef BUFFER_SIZE

static int
mtd_protect_proc_r(struct seq_file *m, void *v)
{
	seq_printf(m, "Protected blocks number is %u\n", luna_mtd_protection_top);
	return 0;
}

static int
mtd_protect_proc_open(struct inode *inode, struct file *file){
	return single_open(file, mtd_protect_proc_r, NULL);
}

static const struct file_operations _mtd_protect_ops = {
	.write   = mtd_protect_proc_w,
	.open    = mtd_protect_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};

const struct file_operations *
get_mtd_protect_proc()
{
	return &_mtd_protect_ops;
}



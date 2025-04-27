/*
 * =====================================================================================
 *
 *       Filename:  luna_mtd_protect.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:
 *        Company:
 *
 * =====================================================================================
 */



#include <linux/proc_fs.h>

#ifndef _LUNA_MTD_PROTECT_H__
#define _LUNA_MTD_PROTECT_H__


#define LUNA_MTD_PROTECTED_BLOCK_DEFAULT_TOP 6
#define LUNA_MTD_PROTECTED_NAME "protected"

const struct file_operations *get_mtd_protect_proc(void);
uint32_t luna_mtd_get_blk_protection_top(void);
void luna_mtd_set_blk_protection_top(uint32_t new_top_value);

#endif

/*
 * Copyright (c) Cortina-Access Limited 2015.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 * aal_hashlite_def.h: common or shared HashLite definition.
 *
 */
#ifndef __AAL_HASHLITE_DEF_H__
#define __AAL_HASHLITE_DEF_H__

#define HASHLITE_OVERFLOW_FLAG	0x800	/* indicate the entry index is in overflow table */

#ifdef CONFIG_ARCH_CORTINA_G3LITE
#define HASHLITE_INDEX_MASK	0x0ff	/* mask for hash index in hash table and overflow table */
#else
#define HASHLITE_INDEX_MASK	0x7ff	/* mask for hash index in hash table and overflow table */
#endif

/* NOTE:
 * This is raw data structure. Each entry contains 192 or 384 bits.
 * The actual usage and bit field definition depends on hash action group mask.
 * In simple words, this structure is for type casting.
 */
typedef struct aal_hashlite_hash_action_entry_s {
	/* In G3 or Saturn HGU,
	 *   if CFG.HT_SIZE == 1K, 384 bits are available.
	 *   if CFG.HT_SIZE == 2K, 192 bits are available.
	 * In G3Lite,
	 *   if CFG.HT_SIZE == 256, 192 bits are available.
	 */
	ca_uint64_t	reserved_0;
	ca_uint64_t	reserved_1;
	ca_uint64_t	reserved_2;
#ifndef CONFIG_ARCH_CORTINA_G3LITE
	ca_uint64_t	reserved_3;
	ca_uint64_t	reserved_4;
	ca_uint64_t	reserved_5;
#endif
} aal_hashlite_hash_action_entry_t;

#endif /* __AAL_HASHLITE_DEF_H__ */


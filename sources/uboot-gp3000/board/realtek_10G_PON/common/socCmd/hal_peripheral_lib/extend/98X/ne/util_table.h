/****************************************************************************
            Software License for Customer Use of Cortina Software
                          Grant Terms and Conditions

IMPORTANT NOTICE - READ CAREFULLY: This Software License for Customer Use
of Cortina Software ("LICENSE") is the agreement which governs use of
software of Cortina Access, Inc. and its subsidiaries ("CORTINA"),
including computer software (source code and object code) and associated
printed materials ("SOFTWARE").  The SOFTWARE is protected by copyright laws
and international copyright treaties, as well as other intellectual property
laws and treaties.  The SOFTWARE is not sold, and instead is only licensed
for use, strictly in accordance with this document.  Any hardware sold by
CORTINA is protected by various patents, and is sold but this LICENSE does
not cover that sale, since it may not necessarily be sold as a package with
the SOFTWARE.  This LICENSE sets forth the terms and conditions of the
SOFTWARE LICENSE only.  By downloading, installing, copying, or otherwise
using the SOFTWARE, you agree to be bound by the terms of this LICENSE.
If you do not agree to the terms of this LICENSE, then do not download the
SOFTWARE.

DEFINITIONS:  "DEVICE" means the Cortina Access(TM) G3  SDK product.
"You" or "CUSTOMER" means the entity or individual that uses the SOFTWARE.
"SOFTWARE" means the Cortina Access(TM) SDK software.

GRANT OF LICENSE:  Subject to the restrictions below, CORTINA hereby grants
CUSTOMER a non-exclusive, non-assignable, non-transferable, royalty-free,
perpetual copyright license to (1) install and use the SOFTWARE for
reference only with the DEVICE; and (2) copy the SOFTWARE for your internal
use only for use with the DEVICE.

RESTRICTIONS:  The SOFTWARE must be used solely in conjunction with the
DEVICE and solely with Your own products that incorporate the DEVICE.  You
may not distribute the SOFTWARE to any third party.  You may not modify
the SOFTWARE or make derivatives of the SOFTWARE without assigning any and
all rights in such modifications and derivatives to CORTINA.  You shall not
through incorporation, modification or distribution of the SOFTWARE cause
it to become subject to any open source licenses.  You may not
reverse-assemble, reverse-compile, or otherwise reverse-engineer any
SOFTWARE provided in binary or machine readable form.  You may not
distribute the SOFTWARE to your customers without written permission
from CORTINA.

OWNERSHIP OF SOFTWARE AND COPYRIGHTS. All title and copyrights in and the
SOFTWARE and any accompanying printed materials, and copies of the SOFTWARE,
are owned by CORTINA. The SOFTWARE protected by the copyright laws of the
United States and other countries, and international treaty provisions.
You may not remove any copyright notices from the SOFTWARE.  Except as
otherwise expressly provided, CORTINA grants no express or implied right
under CORTINA patents, copyrights, trademarks, or other intellectual
property rights.

DISCLAIMER OF WARRANTIES. THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY
EXPRESS OR IMPLIED WARRANTY OF ANY KIND, INCLUDING ANY IMPLIED WARRANTIES
OF MERCHANTABILITY, NONINFRINGEMENT, OR FITNESS FOR A PARTICULAR PURPOSE,
TITLE, AND NON-INFRINGEMENT.  CORTINA does not warrant or assume
responsibility for the accuracy or completeness of any information, text,
graphics, links or other items contained within the SOFTWARE.  Without
limiting the foregoing, you are solely responsible for determining and
verifying that the SOFTWARE that you obtain and install is the appropriate
version for your purpose.

LIMITATION OF LIABILITY. IN NO EVENT SHALL CORTINA OR ITS SUPPLIERS BE
LIABLE FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION, LOST
PROFITS, BUSINESS INTERRUPTION, OR LOST INFORMATION) OR ANY LOSS ARISING OUT
OF THE USE OF OR INABILITY TO USE OF OR INABILITY TO USE THE SOFTWARE, EVEN
IF CORTINA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
TERMINATION OF THIS LICENSE. This LICENSE will automatically terminate if
You fail to comply with any of the terms and conditions hereof. Upon
termination, You will immediately cease use of the SOFTWARE and destroy all
copies of the SOFTWARE or return all copies of the SOFTWARE in your control
to CORTINA.  IF you commence or participate in any legal proceeding against
CORTINA, then CORTINA may, in its sole discretion, suspend or terminate all
license grants and any other rights provided under this LICENSE during the
pendency of such legal proceedings.
APPLICABLE LAWS. Claims arising under this LICENSE shall be governed by the
laws of the State of California, excluding its principles of conflict of
laws.  The United Nations Convention on Contracts for the International Sale
of Goods is specifically disclaimed.  You shall not export the SOFTWARE
without first obtaining any required export license or other approval from
the applicable governmental entity, if required.  This is the entire
agreement and understanding between You and CORTINA relating to this subject
matter.
GOVERNMENT RESTRICTED RIGHTS. The SOFTWARE is provided with "RESTRICTED
RIGHTS." Use, duplication, or disclosure by the Government is subject to
restrictions as set forth in FAR52.227-14 and DFAR252.227-7013 et seq. or
its successor. Use of the SOFTWARE by the Government constitutes
acknowledgment of CORTINA's proprietary rights therein. Contractor or
Manufacturer is CORTINA Access.

Copyright (c) 2015 by Cortina Access Incorporated
****************************************************************************/
/*
 * cs_driver_helper.c
 *
 * This file implements driver helper functions for all Cortina Access drivers
 */
#ifndef _UTIL_HASH_H_
#define _UTIL_HASH_H_

#if 0//yocto
#include "os_core.h"
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f
#include "ca_types.h"
#include "util_list.h"

extern ca_uint32 g_table_debug;

#define CA_DEBUG(arg,...) {\
    if (g_table_debug == 1)\
            OS_PRINTF("%s:%d:\n", __FUNCTION__, __LINE__);\
            OS_PRINTF(arg,##__VA_ARGS__);\
}





#define CA_ERROR(arg,...) {\
    OS_PRINTF("%s:%d:\n", __FUNCTION__, __LINE__);\
    OS_PRINTF(arg,##__VA_ARGS__);\
}




#define CA_TABLE_NAME_STR_MAX     20

typedef enum {



CA_LIST_TABLE,


CA_RBTREE_TABLE,
} ca_table_type_e;

typedef struct table {

ca_uint8 table_name[CA_TABLE_NAME_STR_MAX];

ca_table_type_e table_type;	/* use link list or rb tree */

void *position;	/* pointer to the node of currently getfirst or getnext operation */

void *content;		/* the entry content of the table, pointer to a list or a tree */
} ca_table_t;


ca_table_t *ca_table_create(
ca_uint8 *table_name,

ca_table_type_e table_type,

ca_uint32 data_size,

ca_int32(*compare) (void *, void *));

ca_int32 ca_table_destory(ca_table_t * pTable);

ca_int32 ca_table_entry_insert(ca_table_t *pTable, void *key, void *data);

ca_int32 ca_table_entry_remove(ca_table_t *pTable, void *key);

ca_int32 ca_table_entry_remove_all(ca_table_t *pTable);

void *ca_table_entry_search(ca_table_t *pTable, void *key);

void ca_table_entry_traverse(ca_table_t *pTable, void (*traverse) (void *));

void *ca_table_entry_getfirst(ca_table_t *pTable);

void *ca_table_entry_getnext(ca_table_t *pTable);

ca_int32 ca_table_entry_count(ca_table_t *pTable);

#endif				/*
 */


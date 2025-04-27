/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 89583 $
 * $Date: 2018-06-21 16:58:32 +0800 (Thu, 21 Jun 2018) $
 *
 * Purpose : Define those public diag shell PIE APIs.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */


#ifndef _DIAG_PIE_H_
#define _DIAG_PIE_H_

#include <rtk/pie.h>

/*
 * Symbol Definition
 */
typedef struct diag_pie_template_s
{
    rtk_pie_templateFieldType_t type;
    char                        user_info[32];
    char                        abbre[6];    /* abbreviation */
    char                        desc[120];
} diag_pie_template_t;

extern diag_pie_template_t diag_pie_template_list[];

#endif /* end of _DIAG_PIE_H_ */


/*
 * Copyright (C) 2011 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 * Purpose : chip register and its field definition in the SDK.
 *
 * Feature : chip register and its field definition
 *
 */

#ifndef __HAL_CHIPDEF_ALLREG_H__
#define __HAL_CHIPDEF_ALLREG_H__

/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Data Type Declaration
 */
typedef struct rtk_regField_s
{
    unsigned int   name;    /* field name */
    unsigned short lsp;     /* LSP of the field */
    unsigned short len;     /* field length */
} rtk_regField_t;

typedef struct rtk_reg_s
{
    unsigned int offset;             /* offset address */
    unsigned int field_num;          /* total field numbers */
    unsigned int array_offset:16;    /* array offset */
    unsigned int larray:16;          /* array start index */
    unsigned int harray:16;          /* array end index */
    unsigned int lport:16;           /* port start index */
    unsigned int hport:16;           /* port end index */
    rtk_regField_t *fields;          /* register fields */
} rtk_reg_t;


#if defined(CONFIG_SDK_REG_DFLT_VAL)
#define RTK_REG_DFLT_VAL(value)     value
#else
#define RTK_REG_DFLT_VAL(value)
#endif

#endif  /* __HAL_CHIPDEF_ALLREG_H__ */

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
 *
 * Purpose : define supported command flags
 *
 * Feature : None
 *
 */

#ifndef __CMD_FLAG_APOLLO_H__
#define __CMD_FLAG_APOLLO_H__

/*
 * Include Files
 */


/*
 * Symbol Definition
 */
// APOLLO_FPGA_TEST_CMD and APOLLO_NORMAL_CMD must select one
//#define APOLLO_FPGA_TEST_CMD	
#define APOLLO_NORMAL_CMD
#define CMD_DEBUG_SET_PRINT_PORT_MUX_MUX

#if CONFIG_RTK_L34_ENABLE > 0
#define APOLLO_RG_ENABLE 
#endif

/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Module Name : diag */


#endif /* __CMD_FLAG_APOLLO_H__ */


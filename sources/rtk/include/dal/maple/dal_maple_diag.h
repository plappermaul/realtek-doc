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
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public Port APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Diag
 *
 */

#ifndef __DAL_MAPLE_DIAG_H__
#define __DAL_MAPLE_DIAG_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <dal/dal_common.h>
#include <dal/dal_mgmt.h>
#include <rtk/diag.h>



/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

#define L2ENTRY_REVERSE_INVALID			0x0
#define L2ENTRY_REVERSE_VALID			0x1


/*
 * Function Declaration
 */


/* Function Name:
 *      dal_maple_diagMapper_init
 * Description:
 *      Hook diag module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook diag module before calling any diag APIs.
 */
extern int32
dal_maple_diagMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_cypress_diag_init
 * Description:
 *      Initialize diagnostic module of the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Module must be initialized before using all of APIs in this module
 */
extern int32
dal_maple_diag_init(uint32 unit);

/* Function Name:
 *      dal_maple_diag_portRtctResult_get
 * Description:
 *      Get test result of RTCT.
 * Input:
 *      unit        - unit id
 *      port        - the port for retriving RTCT test result
 * Output:
 *      pRtctResult - RTCT result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_RTCT_NOT_FINISH   - RTCT not finish. Need to wait a while.
 *      RT_ERR_TIMEOUT      - RTCT test timeout in this port.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      If linkType is PORT_SPEED_1000M, test result will be stored in ge_result.
 *      If linkType is PORT_SPEED_10M or PORT_SPEED_100M, test result will be stored in fe_result.
 */
extern int32
dal_maple_diag_portRtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult);

/* Function Name:
 *      dal_maple_diag_rtct_start
 * Description:
 *      Start RTCT for ports.
 *      When enable RTCT, the port won't transmit and receive normal traffic.
 * Input:
 *      unit      - unit id
 *      pPortmask - the ports for RTCT test
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_diag_rtct_start(uint32 unit, rtk_portmask_t *pPortmask);

/* Function Name:
 *      dal_maple_diag_table_read
 * Description:
 *      Read one specified table entry by table index.
 * Input:
 *      unit  - unit id
 *      table - table index
 *      addr  - entry address of the table
 * 	  pData - pointer buffer for read back table entry
 *      pRev_vaild - used to sure the revbits is vaild.
 * 	  pRevbits - pointer buffer for read reverse bits which are not contain in entry.
 * Output:
 * 	  pData - pointer buffer for read back table entry
 *      pRev_vaild - used to sure the revbits is vaild.
 * 	  pRevbits - pointer buffer for read reverse bits which are not contain in entry.
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 * Note:
 *      1. Basically, this is a transparent API for table read.
 *      2. For L2 hash table, this API will converse the hiding 12-bits,
 *          and provide to upper layer by pRevbits parameter.
 *      3. addr format :
 *          From RTK and realchip view : hash_key[13:2]location[1:0]
 */

extern int32
dal_maple_diag_table_read(uint32  unit, uint32  table, uint32  addr, uint32	*pData, uint32 *pRev_vaild, uint32 *pRevbits);

#endif /* __DAL_CYPRESS_DIAG_H__ */

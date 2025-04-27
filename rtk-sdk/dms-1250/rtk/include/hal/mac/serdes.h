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
 * Purpose : SerDes indirect access APIs in the SDK.
 *
 * Feature : SerDes indirect access APIs
 *
 */

#ifndef __HAL_MAC_SERDES_H__
#define __HAL_MAC_SERDES_H__

#define SDS_FIELD_R(_u, _s, _p, _r, _end, _start, _d) \
     hal_sds_field_read(_u, _s, _p, _r, _end, _start, _d)
#define SDS_FIELD_W(_u, _s, _p, _r, _end, _start, _d) \
     hal_sds_field_write(_u, _s, _p, _r, _end,  _start, _d)


/* Function Name:
 *      hal_serdes_reg_get
 * Description:
 *      Get SerDes register through indirect access
 * Input:
 *      unit    - unit id
 *      sdsId   - SerDes index
 *      page    - page
 *      reg     - register index/address
 * Output:
 *      pData   - register value
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 *      RT_ERR_CHIP_NOT_SUPPORTED - chip not support
 * Note:
 *      None
 */
extern int32
hal_serdes_reg_get(uint32 unit, uint32 sdsId, uint32 page, uint32 reg, uint32 *pData);

/* Function Name:
 *      hal_serdes_reg_set
 * Description:
 *      Set SerDes register through indirect access
 * Input:
 *      unit    - unit id
 *      sdsId   - SerDes index
 *      page    - page
 *      reg     - register index/address
 *      data    - register value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 *      RT_ERR_CHIP_NOT_SUPPORTED - chip not support
 * Note:
 *      None
 */
extern int32
hal_serdes_reg_set(uint32 unit, uint32 sdsId, uint32 page, uint32 reg, uint32 data);

/* Function Name:
 *      hal_sds_field_read
 * Description:
 *      Read SerDes field configuration
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      page - page number
 *      reg  - register
 *      endBit    - end bit of configure field
 *      startBit  - start bit of configure field
 * Output:
 *      data - SerDes Configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
hal_sds_field_read(uint32 unit, uint32 sds, uint32 page, uint32 reg,
    uint32 endBit, uint32 startBit, uint32 *data);

/* Function Name:
 *      hal_sds_field_write
 * Description:
 *      Write SerDes field configuration
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      page - page number
 *      reg  - register
 *      endBit    - end bit of configure field
 *      startBit  - start bit of configure field
 *      data - SerDes Configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
hal_sds_field_write(uint32 unit, uint32 sds, uint32 page, uint32 reg,
    uint32 endBit, uint32 startBit, uint32 data);

/* Function Name:
 *      hal_mac_serdes_rst
 * Description:
 *      Reset MAC serdes.
 * Input:
 *      unit   - unit id
 *      sds_no   - serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_OUT_OF_RANGE  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
hal_mac_serdes_rst(uint32 unit, uint32 sds_no);

#endif /* __HAL_MAC_SERDES_H__ */

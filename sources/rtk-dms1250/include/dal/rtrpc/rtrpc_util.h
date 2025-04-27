/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_UTIL_H__
#define __RTRPC_UTIL_H__

/*
* Include Files
*/

#ifdef RTUSR
#define rt_util_tblEntry2Field               rtrpc_util_tblEntry2Field
#define rt_util_field2TblEntry               rtrpc_util_field2TblEntry
#define rt_util_serdesTxEyeParam_scan        rtrpc_util_serdesTxEyeParam_scan
#endif //RTUSR

int32
rtrpc_util_tblEntry2Field(uint32 unit,uint32 table,uint32 field,uint32 * pField,uint32 * pEntry);

int32
rtrpc_util_field2TblEntry(uint32 unit,uint32 table,uint32 field,uint32 * pField,uint32 * pEntry);

int32
rtrpc_util_serdesTxEyeParam_scan(uint32 unit, rt_sdsTxScanParam_t * txScanParam, rt_sdsTxScanChart_t * scanResult);


#endif //__RTRPC_UTIL_H__

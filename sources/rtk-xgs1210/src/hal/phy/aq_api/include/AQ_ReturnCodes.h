/* AQ_ReturnCodes.h */

/************************************************************************************
*
* Copyright (c) 2017 Aquantia Corp.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following conditions are met: 
*
* 1. Redistributions of source code must retain the above copyright notice, 
* this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright 
* notice, this list of conditions and the following disclaimer in the 
* documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY 
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
* DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY 
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
* DAMAGE.
*
************************************************************************************/

/************************************************************************************
*                      Copyright Aquantia Corp.
*                              Freeware
*
* Description:
*
*   This file defines the AQ_API functions' integral return codes.
*
*
************************************************************************************/

/*! \file
  This file defines the AQ_API functions' integral return codes.
*/

#ifndef AQ_RETURNCODES_TOKEN
#define AQ_RETURNCODES_TOKEN


/*! \defgroup ReturnCodes
  @{
*/

/*! Most AQ_API functions return AQ_Retcode to report success or failure.
 * The values used are defined as preprocessor symbols in AQ_ReturnCodes.h. 
 * Callers should prefer to test the return values by equivalence to these
 * symbols, rather than using the integer values directly, as these may 
 * not be stable across releases.  The set of possible return codes that may 
 * be returned by a particular API function can be found in the function's 
 * documentation, as well as information on how to interpret each of the 
 * possible return codes. */
typedef unsigned int AQ_Retcode;

/*! \defgroup Success 
  @{ */
#define AQ_RET_OK 0
/*@}*/


/*! \defgroup GeneralErrors 
  @{ */
#define AQ_RET_ERROR 100
#define AQ_RET_UP_BUSY_TIMEOUT 101
/*@}*/

/*! \defgroup FunctionSpecificResults 
  @{ */
#define AQ_RET_FLASH_READY             200
#define AQ_RET_FLASH_READINESS_TIMEOUT 204

#define AQ_RET_FLASHINTF_READY             201
#define AQ_RET_FLASHINTF_READINESS_TIMEOUT 203

#define AQ_RET_FLASH_TYPE_UNKNOWN 205
#define AQ_RET_FLASH_TYPE_BAD     206

#define AQ_RET_FLASH_IMAGE_CORRUPT   207
#define AQ_RET_FLASH_IMAGE_TOO_LARGE 208
#define AQ_RET_FLASH_IMAGE_MISMATCH  209

#define AQ_RET_FLASH_PAGE_SIZE_NEED_CHANGE  202
#define AQ_RET_FLASH_PAGE_SIZE_CHANGED      210
#define AQ_RET_FLASH_ERASE_SIZE_UNSUPPORTED 243

#define AQ_RET_BOOTLOAD_PROVADDR_OOR        211
#define AQ_RET_BOOTLOAD_NONUNIFORM_REGVALS  212
#define AQ_RET_BOOTLOAD_CRC_MISMATCH        213
#define AQ_RET_BOOTLOAD_PROVTABLE_TOO_LARGE 228

#define AQ_RET_LOOPBACK_BAD_ENTRY_STATE 214

#define AQ_RET_DEBUGTRACE_FREEZE_TIMEOUT   215
#define AQ_RET_DEBUGTRACE_UNFREEZE_TIMEOUT 216

#define AQ_RET_CABLEDIAG_ALREADY_RUNNING          217
#define AQ_RET_CABLEDIAG_STILL_RUNNING            218
#define AQ_RET_CABLEDIAG_BAD_PAIRSTATUS           219
#define AQ_RET_CABLEDIAG_RESULTS_ALREDY_COLLECTED 220
#define AQ_RET_CABLEDIAG_BAD_NUM_SAMPLES          221
#define AQ_RET_CABLEDIAG_REPORTEDPAIR_MISMATCH    222
#define AQ_RET_CABLEDIAG_REPORTEDPAIR_OOR         223
#define AQ_RET_CABLEDIAG_STARTED_PAIR_B           224
#define AQ_RET_CABLEDIAG_STARTED_PAIR_C           225
#define AQ_RET_CABLEDIAG_STARTED_PAIR_D           226
#define AQ_RET_CABLEDIAG_TXENABLE_MISMATCH        227

#define AQ_RET_SERDESEYE_BAD_SERDES_MODE 229
#define AQ_RET_SERDESEYE_BAD_MEAS_COUNT  230
#define AQ_RET_SERDESEYE_MEAS_TIMEOUT    231
#define AQ_RET_SERDESEYE_LANE_OOR        232
#define AQ_RET_SERDESEYE_COORD_OOR       233

#define AQ_RET_PIFMAILBOX_ERROR   234
#define AQ_RET_PIFMAILBOX_TIMEOUT 235

#define AQ_RET_SEC_TABLE_INDEX_OOR 236

#define AQ_RET_TXPHASE_PROV_CHECK_FAILED    237
#define AQ_RET_TXPHASE_SANITYCHECK_FAILED   238
#define AQ_RET_TXPHASE_PROV_PREPARE_FAILED  239
#define AQ_RET_TXPHASE_PROV_SET_FAILED      240
#define AQ_RET_TXPHASE_PROV_APPLY_FAILED    241
#define AQ_RET_TXPHASE_LINK_FAILED          242

#define AQ_RET_FLASH_INVALID_PARAMETERS     244
#define AQ_RET_FLASH_ERASE_NOT_SUPPORTED    245

#define AQ_RET_LAST_ERROR                   245 /* AQ_RET_FLASH_ERASE_SIZE_UNSUPPORTED */

/*@}*/

/*@}*/
 
#endif

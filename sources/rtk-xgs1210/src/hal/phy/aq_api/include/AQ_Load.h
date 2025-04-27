/* AQ_Load.h */

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
*   This file defines the image load related constants.
*
*
************************************************************************************/

/*! \file
  This file defines the image load related constants.
*/

#ifndef _AQ_LOAD_H
#define _AQ_LOAD_H

#ifdef __cplusplus
extern "C" {
#endif

/*! FW image version string maximum length. */
#define AQ_VERSION_STRING_SIZE 0x40

/*! The byte offset from top of DRAM to the FW image version string. */
#define AQ_VERSION_STRING_BLOCK_OFFSET 0x0200

/*! The byte offset from the top of the PHY image to the header content (HHD & EUR devices). */
#define AQ_PHY_IMAGE_HEADER_CONTENT_OFFSET_HHD 0x300

/*! The byte offset from the top of the PHY image to the header content (APPIA devices). */
#define AQ_PHY_IMAGE_HEADER_CONTENT_OFFSET_APPIA 0

/*! The table used to compute CRC's within the PHY. */
extern const uint16_t AQ_CRC16Table[256];

#ifdef __cplusplus
}
#endif

#endif
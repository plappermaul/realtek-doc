/*AQ_RegMaps.h*/

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
*                     Copyright Aquantia Corp.
*                             Freeware
*
* Description:
*
*   This file contains includes all appropriate Aquantia PHY device-specific 
*   register map headers.
*
************************************************************************************/

/*! \file
*   This file contains includes all appropriate Aquantia PHY device-specific 
*   register map headers.
 */

#ifndef AQ_REGISTERMAPS_HEADER
#define AQ_REGISTERMAPS_HEADER

#include <AQ_User.h>
#include "AQ_RegGroupMaxSizes.h"


#ifndef AQ_REVERSED_BITFIELD_ORDERING
/* 
 * Include non-reversed header files (bitfields ordered from LSbit to MSbit) 
 */

/* APPIA */
#include "AQ_APPIA_Autonegotiation_registers.h"
#include "AQ_APPIA_GbE_registers.h"
#include "AQ_APPIA_Global_registers.h"
#include "AQ_APPIA_PCS_registers.h"
#include "AQ_APPIA_PHY_XS_registers.h"
#include "AQ_APPIA_PMA_registers.h"

#include "AQ_APPIA_Autonegotiation_registers_Defines.h"
#include "AQ_APPIA_GbE_registers_Defines.h"
#include "AQ_APPIA_Global_registers_Defines.h"
#include "AQ_APPIA_PCS_registers_Defines.h"
#include "AQ_APPIA_PHY_XS_registers_Defines.h"
#include "AQ_APPIA_PMA_registers_Defines.h"

/* HHD */
#include "AQ_HHD_Autonegotiation_registers.h"
#include "AQ_HHD_GbE_registers.h"
#include "AQ_HHD_Global_registers.h"
#include "AQ_HHD_PCS_registers.h"
#include "AQ_HHD_PHY_XS_registers.h"
#include "AQ_HHD_PMA_registers.h"

#include "AQ_HHD_Autonegotiation_registers_Defines.h"
#include "AQ_HHD_GbE_registers_Defines.h"
#include "AQ_HHD_Global_registers_Defines.h"
#include "AQ_HHD_PCS_registers_Defines.h"
#include "AQ_HHD_PHY_XS_registers_Defines.h"
#include "AQ_HHD_PMA_registers_Defines.h"

/* EUR */
#include "AQ_EUR_Autonegotiation_registers.h"
#include "AQ_EUR_GbE_registers.h"
#include "AQ_EUR_Global_registers.h"
#include "AQ_EUR_PCS_registers.h"
#include "AQ_EUR_PHY_XS_registers.h"
#include "AQ_EUR_PMA_registers.h"

#include "AQ_EUR_Autonegotiation_registers_Defines.h"
#include "AQ_EUR_GbE_registers_Defines.h"
#include "AQ_EUR_Global_registers_Defines.h"
#include "AQ_EUR_PCS_registers_Defines.h"
#include "AQ_EUR_PHY_XS_registers_Defines.h"
#include "AQ_EUR_PMA_registers_Defines.h"

#else
/* 
 * Include reversed header files (bitfields ordered from MSbit to LSbit) 
 */

/* APPIA */
#include "AQ_APPIA_Autonegotiation_registers_reversed.h"
#include "AQ_APPIA_GbE_registers_reversed.h"
#include "AQ_APPIA_Global_registers_reversed.h"
#include "AQ_APPIA_PCS_registers_reversed.h"
#include "AQ_APPIA_PHY_XS_registers_reversed.h"
#include "AQ_APPIA_PMA_registers_reversed.h"

#include "AQ_APPIA_Autonegotiation_registers_Defines.h"
#include "AQ_APPIA_GbE_registers_Defines.h"
#include "AQ_APPIA_Global_registers_Defines.h"
#include "AQ_APPIA_PCS_registers_Defines.h"
#include "AQ_APPIA_PHY_XS_registers_Defines.h"
#include "AQ_APPIA_PMA_registers_Defines.h"

/* HHD */
#include "AQ_HHD_Autonegotiation_registers_reversed.h"
#include "AQ_HHD_GbE_registers_reversed.h"
#include "AQ_HHD_Global_registers_reversed.h"
#include "AQ_HHD_PCS_registers_reversed.h"
#include "AQ_HHD_PHY_XS_registers_reversed.h"
#include "AQ_HHD_PMA_registers_reversed.h"

#include "AQ_HHD_Autonegotiation_registers_Defines.h"
#include "AQ_HHD_GbE_registers_Defines.h"
#include "AQ_HHD_Global_registers_Defines.h"
#include "AQ_HHD_PCS_registers_Defines.h"
#include "AQ_HHD_PHY_XS_registers_Defines.h"
#include "AQ_HHD_PMA_registers_Defines.h"

/* EUR */
#include "AQ_EUR_Autonegotiation_registers_reversed.h"
#include "AQ_EUR_GbE_registers_reversed.h"
#include "AQ_EUR_Global_registers_reversed.h"
#include "AQ_EUR_PCS_registers_reversed.h"
#include "AQ_EUR_PHY_XS_registers_reversed.h"
#include "AQ_EUR_PMA_registers_reversed.h"

#include "AQ_EUR_Autonegotiation_registers_Defines.h"
#include "AQ_EUR_GbE_registers_Defines.h"
#include "AQ_EUR_Global_registers_Defines.h"
#include "AQ_EUR_PCS_registers_Defines.h"
#include "AQ_EUR_PHY_XS_registers_Defines.h"
#include "AQ_EUR_PMA_registers_Defines.h"

#endif

#endif

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

/*! \file
  This file contains  macros for accessing the AQ PHYs' registers 
  using the device-specific register map data structures and definitions.
*/

#ifndef AQ_REG_MACRO_TOKEN
#define AQ_REG_MACRO_TOKEN

#include "AQ_PhyInterface.h"


#define AQ_API_ReadRegister(id,reg,wd) AQ_API_ReadRegister_DeviceRestricted(APPIA_HHD_EUR,id,reg,wd)

#define AQ_API_ReadRegister_DeviceRestricted(devices,id,reg,wd) AQ_API_ReadRegister_Devs_ ## devices(id,reg,wd)

#define AQ_API_ReadRegister_Devs_APPIA(id,reg,wd) \
    ((port->device == AQ_DEVICE_APPIA) ? AQ_API_MDIO_Read (id,reg ## _APPIA_mmdAddress,(reg ## _APPIA_baseRegisterAddress + wd)) : \
    (0))

#define AQ_API_ReadRegister_Devs_HHD(id,reg,wd) \
    ((port->device == AQ_DEVICE_HHD) ? AQ_API_MDIO_Read (id,reg ## _HHD_mmdAddress,(reg ## _HHD_baseRegisterAddress + wd)) : \
    (0))

#define AQ_API_ReadRegister_Devs_EUR(id,reg,wd) \
    ((port->device == AQ_DEVICE_EUR) ? AQ_API_MDIO_Read (id,reg ## _EUR_mmdAddress,(reg ## _EUR_baseRegisterAddress + wd)) : \
    (0))

#define AQ_API_ReadRegister_Devs_APPIA_HHD(id,reg,wd) \
    ((port->device == AQ_DEVICE_HHD) ? AQ_API_MDIO_Read (id,reg ## _HHD_mmdAddress,(reg ## _HHD_baseRegisterAddress + wd)) : \
    ((port->device == AQ_DEVICE_APPIA) ? AQ_API_MDIO_Read (id,reg ## _APPIA_mmdAddress,(reg ## _APPIA_baseRegisterAddress + wd)) : \
    (0)))

#define AQ_API_ReadRegister_Devs_APPIA_EUR(id,reg,wd) \
    ((port->device == AQ_DEVICE_EUR) ? AQ_API_MDIO_Read (id,reg ## _EUR_mmdAddress,(reg ## _EUR_baseRegisterAddress + wd)) : \
    ((port->device == AQ_DEVICE_APPIA) ? AQ_API_MDIO_Read (id,reg ## _APPIA_mmdAddress,(reg ## _APPIA_baseRegisterAddress + wd)) : \
    (0)))

#define AQ_API_ReadRegister_Devs_HHD_EUR(id,reg,wd) \
    ((port->device == AQ_DEVICE_EUR) ? AQ_API_MDIO_Read (id,reg ## _EUR_mmdAddress,(reg ## _EUR_baseRegisterAddress + wd)) : \
    ((port->device == AQ_DEVICE_HHD) ? AQ_API_MDIO_Read (id,reg ## _HHD_mmdAddress,(reg ## _HHD_baseRegisterAddress + wd)) : \
    (0)))

#define AQ_API_ReadRegister_Devs_APPIA_HHD_EUR(id,reg,wd) \
    ((port->device == AQ_DEVICE_EUR) ? AQ_API_MDIO_Read (id,reg ## _EUR_mmdAddress,(reg ## _EUR_baseRegisterAddress + wd)) : \
    ((port->device == AQ_DEVICE_HHD) ? AQ_API_MDIO_Read (id,reg ## _HHD_mmdAddress,(reg ## _HHD_baseRegisterAddress + wd)) : \
    ((port->device == AQ_DEVICE_APPIA) ? AQ_API_MDIO_Read (id,reg ## _APPIA_mmdAddress,(reg ## _APPIA_baseRegisterAddress + wd)) : \
    (0))))

#define AQ_API_ReadRegister_Devs_HHD_APPIA(id,reg,wd) AQ_API_ReadRegister_Devs_APPIA_HHD(id,reg,wd)
#define AQ_API_ReadRegister_Devs_EUR_APPIA(id,reg,wd) AQ_API_ReadRegister_Devs_APPIA_EUR(id,reg,wd)
#define AQ_API_ReadRegister_Devs_EUR_HHD(id,reg,wd) AQ_API_ReadRegister_Devs_HHD_EUR(id,reg,wd)
#define AQ_API_ReadRegister_Devs_APPIA_EUR_HHD(id,reg,wd) AQ_API_ReadRegister_Devs_APPIA_HHD_EUR(id,reg,wd)
#define AQ_API_ReadRegister_Devs_HHD_APPIA_EUR(id,reg,wd) AQ_API_ReadRegister_Devs_APPIA_HHD_EUR(id,reg,wd)
#define AQ_API_ReadRegister_Devs_HHD_EUR_APPIA(id,reg,wd) AQ_API_ReadRegister_Devs_APPIA_HHD_EUR(id,reg,wd)
#define AQ_API_ReadRegister_Devs_EUR_APPIA_HHD(id,reg,wd) AQ_API_ReadRegister_Devs_APPIA_HHD_EUR(id,reg,wd)
#define AQ_API_ReadRegister_Devs_EUR_HHD_APPIA(id,reg,wd) AQ_API_ReadRegister_Devs_APPIA_HHD_EUR(id,reg,wd)


#define AQ_API_WriteRegister(id,reg,wd,value) AQ_API_WriteRegister_DeviceRestricted(APPIA_HHD_EUR,id,reg,wd,value)

#define AQ_API_WriteRegister_DeviceRestricted(devices,id,reg,wd,value) AQ_API_WriteRegister_Devs_ ## devices(id,reg,wd,value)

#define AQ_API_WriteRegister_Devs_APPIA(id,reg,wd,value) \
    ((port->device == AQ_DEVICE_APPIA) ? AQ_API_MDIO_Write (id,reg ## _APPIA_mmdAddress,(reg ## _APPIA_baseRegisterAddress + wd),value) : \
    ((void)0))

#define AQ_API_WriteRegister_Devs_HHD(id,reg,wd,value) \
    ((port->device == AQ_DEVICE_HHD) ? AQ_API_MDIO_Write (id,reg ## _HHD_mmdAddress,(reg ## _HHD_baseRegisterAddress + wd),value) : \
    ((void)0))

#define AQ_API_WriteRegister_Devs_EUR(id,reg,wd,value) \
    ((port->device == AQ_DEVICE_EUR) ? AQ_API_MDIO_Write (id,reg ## _EUR_mmdAddress,(reg ## _EUR_baseRegisterAddress + wd),value) : \
    ((void)0))

#define AQ_API_WriteRegister_Devs_APPIA_HHD(id,reg,wd,value) \
    ((port->device == AQ_DEVICE_HHD) ? AQ_API_MDIO_Write (id,reg ## _HHD_mmdAddress,(reg ## _HHD_baseRegisterAddress + wd),value) : \
    ((port->device == AQ_DEVICE_APPIA) ? AQ_API_MDIO_Write (id,reg ## _APPIA_mmdAddress,(reg ## _APPIA_baseRegisterAddress + wd),value) : \
    ((void)0)))

#define AQ_API_WriteRegister_Devs_APPIA_EUR(id,reg,wd,value) \
    ((port->device == AQ_DEVICE_EUR) ? AQ_API_MDIO_Write (id,reg ## _EUR_mmdAddress,(reg ## _EUR_baseRegisterAddress + wd),value) : \
    ((port->device == AQ_DEVICE_APPIA) ? AQ_API_MDIO_Write (id,reg ## _APPIA_mmdAddress,(reg ## _APPIA_baseRegisterAddress + wd),value) : \
    ((void)0)))

#define AQ_API_WriteRegister_Devs_HHD_EUR(id,reg,wd,value) \
    ((port->device == AQ_DEVICE_EUR) ? AQ_API_MDIO_Write (id,reg ## _EUR_mmdAddress,(reg ## _EUR_baseRegisterAddress + wd),value) : \
    ((port->device == AQ_DEVICE_HHD) ? AQ_API_MDIO_Write (id,reg ## _HHD_mmdAddress,(reg ## _HHD_baseRegisterAddress + wd),value) : \
    ((void)0)))

#define AQ_API_WriteRegister_Devs_APPIA_HHD_EUR(id,reg,wd,value) \
    ((port->device == AQ_DEVICE_EUR) ? AQ_API_MDIO_Write (id,reg ## _EUR_mmdAddress,(reg ## _EUR_baseRegisterAddress + wd),value) : \
    ((port->device == AQ_DEVICE_HHD) ? AQ_API_MDIO_Write (id,reg ## _HHD_mmdAddress,(reg ## _HHD_baseRegisterAddress + wd),value) : \
    ((port->device == AQ_DEVICE_APPIA) ? AQ_API_MDIO_Write (id,reg ## _APPIA_mmdAddress,(reg ## _APPIA_baseRegisterAddress + wd),value) : \
    ((void)0))))

#define AQ_API_WriteRegister_Devs_HHD_APPIA(id,reg,wd,value) AQ_API_WriteRegister_Devs_APPIA_HHD(id,reg,wd,value)
#define AQ_API_WriteRegister_Devs_EUR_APPIA(id,reg,wd,value) AQ_API_WriteRegister_Devs_APPIA_EUR(id,reg,wd,value)
#define AQ_API_WriteRegister_Devs_EUR_HHD(id,reg,wd,value) AQ_API_WriteRegister_Devs_HHD_EUR(id,reg,wd,value)
#define AQ_API_WriteRegister_Devs_APPIA_EUR_HHD(id,reg,wd,value) AQ_API_WriteRegister_Devs_APPIA_HHD_EUR(id,reg,wd,value)
#define AQ_API_WriteRegister_Devs_HHD_APPIA_EUR(id,reg,wd,value) AQ_API_WriteRegister_Devs_APPIA_HHD_EUR(id,reg,wd,value)
#define AQ_API_WriteRegister_Devs_HHD_EUR_APPIA(id,reg,wd,value) AQ_API_WriteRegister_Devs_APPIA_HHD_EUR(id,reg,wd,value)
#define AQ_API_WriteRegister_Devs_EUR_APPIA_HHD(id,reg,wd,value) AQ_API_WriteRegister_Devs_APPIA_HHD_EUR(id,reg,wd,value)
#define AQ_API_WriteRegister_Devs_EUR_HHD_APPIA(id,reg,wd,value) AQ_API_WriteRegister_Devs_APPIA_HHD_EUR(id,reg,wd,value)


#ifdef AQ_PHY_SUPPORTS_BLOCK_READ_WRITE

#define AQ_API_BlockReadRegister(id,reg,wd) AQ_API_BlockReadRegister_DeviceRestricted(APPIA_HHD_EUR,id,reg,wd)

#define AQ_API_BlockReadRegister_DeviceRestricted(devices,id,reg,wd) AQ_API_BlockReadRegister_Devs_ ## devices(id,reg,wd)

#define AQ_API_BlockReadRegister_Devs_APPIA(id,reg,wd) \
    ((port->device == AQ_DEVICE_APPIA) ? AQ_API_MDIO_BlockRead (id,reg ## _APPIA_mmdAddress,(reg ## _APPIA_baseRegisterAddress + wd)) : \
    ((void)0))

#define AQ_API_BlockReadRegister_Devs_HHD(id,reg,wd) \
    ((port->device == AQ_DEVICE_HHD) ? AQ_API_MDIO_BlockRead (id,reg ## _HHD_mmdAddress,(reg ## _HHD_baseRegisterAddress + wd)) : \
    ((void)0))

#define AQ_API_BlockReadRegister_Devs_EUR(id,reg,wd) \
    ((port->device == AQ_DEVICE_EUR) ? AQ_API_MDIO_BlockRead (id,reg ## _EUR_mmdAddress,(reg ## _EUR_baseRegisterAddress + wd)) : \
    ((void)0))

#define AQ_API_BlockReadRegister_Devs_APPIA_HHD(id,reg,wd) \
    ((port->device == AQ_DEVICE_HHD) ? AQ_API_MDIO_BlockRead (id,reg ## _HHD_mmdAddress,(reg ## _HHD_baseRegisterAddress + wd)) : \
    ((port->device == AQ_DEVICE_APPIA) ? AQ_API_MDIO_BlockRead (id,reg ## _APPIA_mmdAddress,(reg ## _APPIA_baseRegisterAddress + wd)) : \
    ((void)0)))

#define AQ_API_BlockReadRegister_Devs_APPIA_EUR(id,reg,wd) \
    ((port->device == AQ_DEVICE_EUR) ? AQ_API_MDIO_BlockRead (id,reg ## _EUR_mmdAddress,(reg ## _EUR_baseRegisterAddress + wd)) : \
    ((port->device == AQ_DEVICE_APPIA) ? AQ_API_MDIO_BlockRead (id,reg ## _APPIA_mmdAddress,(reg ## _APPIA_baseRegisterAddress + wd)) : \
    ((void)0)))

#define AQ_API_BlockReadRegister_Devs_HHD_EUR(id,reg,wd) \
    ((port->device == AQ_DEVICE_EUR) ? AQ_API_MDIO_BlockRead (id,reg ## _EUR_mmdAddress,(reg ## _EUR_baseRegisterAddress + wd)) : \
    ((port->device == AQ_DEVICE_HHD) ? AQ_API_MDIO_BlockRead (id,reg ## _HHD_mmdAddress,(reg ## _HHD_baseRegisterAddress + wd)) : \
    ((void)0)))

#define AQ_API_BlockReadRegister_Devs_APPIA_HHD_EUR(id,reg,wd) \
    ((port->device == AQ_DEVICE_EUR) ? AQ_API_MDIO_BlockRead (id,reg ## _EUR_mmdAddress,(reg ## _EUR_baseRegisterAddress + wd)) : \
    ((port->device == AQ_DEVICE_HHD) ? AQ_API_MDIO_BlockRead (id,reg ## _HHD_mmdAddress,(reg ## _HHD_baseRegisterAddress + wd)) : \
    ((port->device == AQ_DEVICE_APPIA) ? AQ_API_MDIO_BlockRead (id,reg ## _APPIA_mmdAddress,(reg ## _APPIA_baseRegisterAddress + wd)) : \
    ((void)0))))

#define AQ_API_BlockReadRegister_Devs_HHD_APPIA(id,reg,wd) AQ_API_BlockReadRegister_Devs_APPIA_HHD(id,reg,wd)
#define AQ_API_BlockReadRegister_Devs_EUR_APPIA(id,reg,wd) AQ_API_BlockReadRegister_Devs_APPIA_EUR(id,reg,wd)
#define AQ_API_BlockReadRegister_Devs_EUR_HHD(id,reg,wd) AQ_API_BlockReadRegister_Devs_HHD_EUR(id,reg,wd)
#define AQ_API_BlockReadRegister_Devs_APPIA_EUR_HHD(id,reg,wd) AQ_API_BlockReadRegister_Devs_APPIA_HHD_EUR(id,reg,wd)
#define AQ_API_BlockReadRegister_Devs_HHD_APPIA_EUR(id,reg,wd) AQ_API_BlockReadRegister_Devs_APPIA_HHD_EUR(id,reg,wd)
#define AQ_API_BlockReadRegister_Devs_HHD_EUR_APPIA(id,reg,wd) AQ_API_BlockReadRegister_Devs_APPIA_HHD_EUR(id,reg,wd)
#define AQ_API_BlockReadRegister_Devs_EUR_APPIA_HHD(id,reg,wd) AQ_API_BlockReadRegister_Devs_APPIA_HHD_EUR(id,reg,wd)
#define AQ_API_BlockReadRegister_Devs_EUR_HHD_APPIA(id,reg,wd) AQ_API_BlockReadRegister_Devs_APPIA_HHD_EUR(id,reg,wd)


#define AQ_API_BlockWriteRegister(id,reg,wd,value) AQ_API_BlockWriteRegister_DeviceRestricted(APPIA_HHD_EUR,id,reg,wd,value)

#define AQ_API_BlockWriteRegister_DeviceRestricted(devices,id,reg,wd,value) AQ_API_BlockWriteRegister_Devs_ ## devices(id,reg,wd,value)

#define AQ_API_BlockWriteRegister_Devs_APPIA(id,reg,wd,value) \
    ((port->device == AQ_DEVICE_APPIA) ? AQ_API_MDIO_BlockWrite (id,reg ## _APPIA_mmdAddress,(reg ## _APPIA_baseRegisterAddress + wd),value) : \
    ((void)0))

#define AQ_API_BlockWriteRegister_Devs_HHD(id,reg,wd,value) \
    ((port->device == AQ_DEVICE_HHD) ? AQ_API_MDIO_BlockWrite (id,reg ## _HHD_mmdAddress,(reg ## _HHD_baseRegisterAddress + wd),value) : \
    ((void)0))

#define AQ_API_BlockWriteRegister_Devs_EUR(id,reg,wd,value) \
    ((port->device == AQ_DEVICE_EUR) ? AQ_API_MDIO_BlockWrite (id,reg ## _EUR_mmdAddress,(reg ## _EUR_baseRegisterAddress + wd),value) : \
    ((void)0))

#define AQ_API_BlockWriteRegister_Devs_APPIA_HHD(id,reg,wd,value) \
    ((port->device == AQ_DEVICE_HHD) ? AQ_API_MDIO_BlockWrite (id,reg ## _HHD_mmdAddress,(reg ## _HHD_baseRegisterAddress + wd),value) : \
    ((port->device == AQ_DEVICE_APPIA) ? AQ_API_MDIO_BlockWrite (id,reg ## _APPIA_mmdAddress,(reg ## _APPIA_baseRegisterAddress + wd),value) : \
    ((void)0)))

#define AQ_API_BlockWriteRegister_Devs_APPIA_EUR(id,reg,wd,value) \
    ((port->device == AQ_DEVICE_EUR) ? AQ_API_MDIO_BlockWrite (id,reg ## _EUR_mmdAddress,(reg ## _EUR_baseRegisterAddress + wd),value) : \
    ((port->device == AQ_DEVICE_APPIA) ? AQ_API_MDIO_BlockWrite (id,reg ## _APPIA_mmdAddress,(reg ## _APPIA_baseRegisterAddress + wd),value) : \
    ((void)0)))

#define AQ_API_BlockWriteRegister_Devs_HHD_EUR(id,reg,wd,value) \
    ((port->device == AQ_DEVICE_EUR) ? AQ_API_MDIO_BlockWrite (id,reg ## _EUR_mmdAddress,(reg ## _EUR_baseRegisterAddress + wd),value) : \
    ((port->device == AQ_DEVICE_HHD) ? AQ_API_MDIO_BlockWrite (id,reg ## _HHD_mmdAddress,(reg ## _HHD_baseRegisterAddress + wd),value) : \
    ((void)0)))

#define AQ_API_BlockWriteRegister_Devs_APPIA_HHD_EUR(id,reg,wd,value) \
    ((port->device == AQ_DEVICE_EUR) ? AQ_API_MDIO_BlockWrite (id,reg ## _EUR_mmdAddress,(reg ## _EUR_baseRegisterAddress + wd),value) : \
    ((port->device == AQ_DEVICE_HHD) ? AQ_API_MDIO_BlockWrite (id,reg ## _HHD_mmdAddress,(reg ## _HHD_baseRegisterAddress + wd),value) : \
    ((port->device == AQ_DEVICE_APPIA) ? AQ_API_MDIO_BlockWrite (id,reg ## _APPIA_mmdAddress,(reg ## _APPIA_baseRegisterAddress + wd),value) : \
    ((void)0))))

#define AQ_API_BlockWriteRegister_Devs_HHD_APPIA(id,reg,wd,value) AQ_API_BlockWriteRegister_Devs_APPIA_HHD(id,reg,wd,value)
#define AQ_API_BlockWriteRegister_Devs_EUR_APPIA(id,reg,wd,value) AQ_API_BlockWriteRegister_Devs_APPIA_EUR(id,reg,wd,value)
#define AQ_API_BlockWriteRegister_Devs_EUR_HHD(id,reg,wd,value) AQ_API_BlockWriteRegister_Devs_HHD_EUR(id,reg,wd,value)
#define AQ_API_BlockWriteRegister_Devs_APPIA_EUR_HHD(id,reg,wd,value) AQ_API_BlockWriteRegister_Devs_APPIA_HHD_EUR(id,reg,wd,value)
#define AQ_API_BlockWriteRegister_Devs_HHD_APPIA_EUR(id,reg,wd,value) AQ_API_BlockWriteRegister_Devs_APPIA_HHD_EUR(id,reg,wd,value)
#define AQ_API_BlockWriteRegister_Devs_HHD_EUR_APPIA(id,reg,wd,value) AQ_API_BlockWriteRegister_Devs_APPIA_HHD_EUR(id,reg,wd,value)
#define AQ_API_BlockWriteRegister_Devs_EUR_APPIA_HHD(id,reg,wd,value) AQ_API_BlockWriteRegister_Devs_APPIA_HHD_EUR(id,reg,wd,value)
#define AQ_API_BlockWriteRegister_Devs_EUR_HHD_APPIA(id,reg,wd,value) AQ_API_BlockWriteRegister_Devs_APPIA_HHD_EUR(id,reg,wd,value)


#endif

#define AQ_API_Variable(reg) AQ_API_Variable_DeviceRestricted(APPIA_HHD_EUR,reg)

#define AQ_API_Variable_DeviceRestricted(devices,reg) AQ_API_Variable_Devs_ ## devices(reg)

#define AQ_API_Variable_Devs_APPIA(reg) uint8_t _local ## reg ## _space[ sizeof(reg ## _BiggestVersion) ];\
  reg ## _APPIA* _local ## reg ## _APPIA = (reg ## _APPIA*) _local ## reg ## _space; \

#define AQ_API_Variable_Devs_HHD(reg) uint8_t _local ## reg ## _space[ sizeof(reg ## _BiggestVersion) ];\
  reg ## _HHD* _local ## reg ## _HHD = (reg ## _HHD*) _local ## reg ## _space; \

#define AQ_API_Variable_Devs_EUR(reg) uint8_t _local ## reg ## _space[ sizeof(reg ## _BiggestVersion) ];\
  reg ## _EUR* _local ## reg ## _EUR = (reg ## _EUR*) _local ## reg ## _space; \

#define AQ_API_Variable_Devs_APPIA_HHD(reg) uint8_t _local ## reg ## _space[ sizeof(reg ## _BiggestVersion) ];\
  reg ## _APPIA* _local ## reg ## _APPIA = (reg ## _APPIA*) _local ## reg ## _space; \
  reg ## _HHD* _local ## reg ## _HHD = (reg ## _HHD*) _local ## reg ## _space; \

#define AQ_API_Variable_Devs_APPIA_EUR(reg) uint8_t _local ## reg ## _space[ sizeof(reg ## _BiggestVersion) ];\
  reg ## _APPIA* _local ## reg ## _APPIA = (reg ## _APPIA*) _local ## reg ## _space; \
  reg ## _EUR* _local ## reg ## _EUR = (reg ## _EUR*) _local ## reg ## _space; \

#define AQ_API_Variable_Devs_HHD_EUR(reg) uint8_t _local ## reg ## _space[ sizeof(reg ## _BiggestVersion) ];\
  reg ## _HHD* _local ## reg ## _HHD = (reg ## _HHD*) _local ## reg ## _space; \
  reg ## _EUR* _local ## reg ## _EUR = (reg ## _EUR*) _local ## reg ## _space; \

#define AQ_API_Variable_Devs_APPIA_HHD_EUR(reg) uint8_t _local ## reg ## _space[ sizeof(reg ## _BiggestVersion) ];\
  reg ## _APPIA* _local ## reg ## _APPIA = (reg ## _APPIA*) _local ## reg ## _space; \
  reg ## _HHD* _local ## reg ## _HHD = (reg ## _HHD*) _local ## reg ## _space; \
  reg ## _EUR* _local ## reg ## _EUR = (reg ## _EUR*) _local ## reg ## _space; \

#define AQ_API_Variable_Devs_HHD_APPIA(reg) AQ_API_Variable_Devs_APPIA_HHD(reg)
#define AQ_API_Variable_Devs_EUR_APPIA(reg) AQ_API_Variable_Devs_APPIA_EUR(reg)
#define AQ_API_Variable_Devs_EUR_HHD(reg) AQ_API_Variable_Devs_HHD_EUR(reg)
#define AQ_API_Variable_Devs_APPIA_EUR_HHD(reg) AQ_API_Variable_Devs_APPIA_HHD_EUR(reg)
#define AQ_API_Variable_Devs_HHD_APPIA_EUR(reg) AQ_API_Variable_Devs_APPIA_HHD_EUR(reg)
#define AQ_API_Variable_Devs_HHD_EUR_APPIA(reg) AQ_API_Variable_Devs_APPIA_HHD_EUR(reg)
#define AQ_API_Variable_Devs_EUR_APPIA_HHD(reg) AQ_API_Variable_Devs_APPIA_HHD_EUR(reg)
#define AQ_API_Variable_Devs_EUR_HHD_APPIA(reg) AQ_API_Variable_Devs_APPIA_HHD_EUR(reg)


#define AQ_API_DeclareLocalStruct(reg,localvar) AQ_API_DeclareLocalStruct_DeviceRestricted(APPIA_HHD_EUR,reg,localvar)

#define AQ_API_DeclareLocalStruct_DeviceRestricted(devices,reg,localvar) AQ_API_DeclareLocalStruct_Devs_ ## devices(reg,localvar)

#define AQ_API_DeclareLocalStruct_Devs_APPIA(reg,localvar) uint8_t localvar ## _space[ sizeof(reg ## _BiggestVersion) ];\
  reg ## _APPIA* localvar ## _APPIA = (reg ## _APPIA*) localvar ## _space; \

#define AQ_API_DeclareLocalStruct_Devs_HHD(reg,localvar) uint8_t localvar ## _space[ sizeof(reg ## _BiggestVersion) ];\
  reg ## _HHD* localvar ## _HHD = (reg ## _HHD*) localvar ## _space; \

#define AQ_API_DeclareLocalStruct_Devs_EUR(reg,localvar) uint8_t localvar ## _space[ sizeof(reg ## _BiggestVersion) ];\
  reg ## _EUR* localvar ## _EUR = (reg ## _EUR*) localvar ## _space; \

#define AQ_API_DeclareLocalStruct_Devs_APPIA_HHD(reg,localvar) uint8_t localvar ## _space[ sizeof(reg ## _BiggestVersion) ];\
  reg ## _APPIA* localvar ## _APPIA = (reg ## _APPIA*) localvar ## _space; \
  reg ## _HHD* localvar ## _HHD = (reg ## _HHD*) localvar ## _space; \

#define AQ_API_DeclareLocalStruct_Devs_APPIA_EUR(reg,localvar) uint8_t localvar ## _space[ sizeof(reg ## _BiggestVersion) ];\
  reg ## _APPIA* localvar ## _APPIA = (reg ## _APPIA*) localvar ## _space; \
  reg ## _EUR* localvar ## _EUR = (reg ## _EUR*) localvar ## _space; \

#define AQ_API_DeclareLocalStruct_Devs_HHD_EUR(reg,localvar) uint8_t localvar ## _space[ sizeof(reg ## _BiggestVersion) ];\
  reg ## _HHD* localvar ## _HHD = (reg ## _HHD*) localvar ## _space; \
  reg ## _EUR* localvar ## _EUR = (reg ## _EUR*) localvar ## _space; \

#define AQ_API_DeclareLocalStruct_Devs_APPIA_HHD_EUR(reg,localvar) uint8_t localvar ## _space[ sizeof(reg ## _BiggestVersion) ];\
  reg ## _APPIA* localvar ## _APPIA = (reg ## _APPIA*) localvar ## _space; \
  reg ## _HHD* localvar ## _HHD = (reg ## _HHD*) localvar ## _space; \
  reg ## _EUR* localvar ## _EUR = (reg ## _EUR*) localvar ## _space; \

#define AQ_API_DeclareLocalStruct_Devs_HHD_APPIA(reg,localvar) AQ_API_DeclareLocalStruct_Devs_APPIA_HHD(reg,localvar)
#define AQ_API_DeclareLocalStruct_Devs_EUR_APPIA(reg,localvar) AQ_API_DeclareLocalStruct_Devs_APPIA_EUR(reg,localvar)
#define AQ_API_DeclareLocalStruct_Devs_EUR_HHD(reg,localvar) AQ_API_DeclareLocalStruct_Devs_HHD_EUR(reg,localvar)
#define AQ_API_DeclareLocalStruct_Devs_APPIA_EUR_HHD(reg,localvar) AQ_API_DeclareLocalStruct_Devs_APPIA_HHD_EUR(reg,localvar)
#define AQ_API_DeclareLocalStruct_Devs_HHD_APPIA_EUR(reg,localvar) AQ_API_DeclareLocalStruct_Devs_APPIA_HHD_EUR(reg,localvar)
#define AQ_API_DeclareLocalStruct_Devs_HHD_EUR_APPIA(reg,localvar) AQ_API_DeclareLocalStruct_Devs_APPIA_HHD_EUR(reg,localvar)
#define AQ_API_DeclareLocalStruct_Devs_EUR_APPIA_HHD(reg,localvar) AQ_API_DeclareLocalStruct_Devs_APPIA_HHD_EUR(reg,localvar)
#define AQ_API_DeclareLocalStruct_Devs_EUR_HHD_APPIA(reg,localvar) AQ_API_DeclareLocalStruct_Devs_APPIA_HHD_EUR(reg,localvar)


#define AQ_API_Set(id,reg,field,value) AQ_API_Set_DeviceRestricted(APPIA_HHD_EUR,id,reg,field,value)

#define AQ_API_Set_DeviceRestricted(devices,id,reg,field,value) AQ_API_Set_Devs_ ## devices(id,reg,field,value)

#define AQ_API_Set_Devs_APPIA(id,reg,field,value) { \
  switch (port->device) { \
    case AQ_DEVICE_APPIA: \
      _local ## reg ## _APPIA->word_ ## reg ## _APPIA_ ## field = AQ_API_ReadRegister_Devs_APPIA(id,reg,reg ## _APPIA_ ## field); \
      if (_local ## reg ## _APPIA->bits_ ## reg ## _APPIA_ ## field.field != value) \
      { \
        _local ## reg ## _APPIA->bits_ ## reg ## _APPIA_ ## field.field = value; \
        AQ_API_WriteRegister_Devs_APPIA(id,reg,reg ## _APPIA_ ## field,_local ## reg ## _APPIA->word_ ## reg ## _APPIA_ ## field); \
      } \
      break; \
    default: break; \
  } \
} 

#define AQ_API_Set_Devs_HHD(id,reg,field,value) { \
  switch (port->device) { \
    case AQ_DEVICE_HHD: \
      _local ## reg ## _HHD->word_ ## reg ## _HHD_ ## field = AQ_API_ReadRegister_Devs_HHD(id,reg,reg ## _HHD_ ## field); \
      if (_local ## reg ## _HHD->bits_ ## reg ## _HHD_ ## field.field != value) \
      { \
        _local ## reg ## _HHD->bits_ ## reg ## _HHD_ ## field.field = value; \
        AQ_API_WriteRegister_Devs_HHD(id,reg,reg ## _HHD_ ## field,_local ## reg ## _HHD->word_ ## reg ## _HHD_ ## field); \
      } \
      break; \
    default: break; \
  } \
} 

#define AQ_API_Set_Devs_EUR(id,reg,field,value) { \
  switch (port->device) { \
    case AQ_DEVICE_EUR: \
      _local ## reg ## _EUR->word_ ## reg ## _EUR_ ## field = AQ_API_ReadRegister_Devs_EUR(id,reg,reg ## _EUR_ ## field); \
      if (_local ## reg ## _EUR->bits_ ## reg ## _EUR_ ## field.field != value) \
      { \
        _local ## reg ## _EUR->bits_ ## reg ## _EUR_ ## field.field = value; \
        AQ_API_WriteRegister_Devs_EUR(id,reg,reg ## _EUR_ ## field,_local ## reg ## _EUR->word_ ## reg ## _EUR_ ## field); \
      } \
      break; \
    default: break; \
  } \
} 

#define AQ_API_Set_Devs_APPIA_HHD(id,reg,field,value) { \
  switch (port->device) { \
    case AQ_DEVICE_APPIA: \
      _local ## reg ## _APPIA->word_ ## reg ## _APPIA_ ## field = AQ_API_ReadRegister_Devs_APPIA_HHD(id,reg,reg ## _APPIA_ ## field); \
      if (_local ## reg ## _APPIA->bits_ ## reg ## _APPIA_ ## field.field != value) \
      { \
        _local ## reg ## _APPIA->bits_ ## reg ## _APPIA_ ## field.field = value; \
        AQ_API_WriteRegister_Devs_APPIA_HHD(id,reg,reg ## _APPIA_ ## field,_local ## reg ## _APPIA->word_ ## reg ## _APPIA_ ## field); \
      } \
      break; \
    case AQ_DEVICE_HHD: \
      _local ## reg ## _HHD->word_ ## reg ## _HHD_ ## field = AQ_API_ReadRegister_Devs_APPIA_HHD(id,reg,reg ## _HHD_ ## field); \
      if (_local ## reg ## _HHD->bits_ ## reg ## _HHD_ ## field.field != value) \
      { \
        _local ## reg ## _HHD->bits_ ## reg ## _HHD_ ## field.field = value; \
        AQ_API_WriteRegister_Devs_APPIA_HHD(id,reg,reg ## _HHD_ ## field,_local ## reg ## _HHD->word_ ## reg ## _HHD_ ## field); \
      } \
      break; \
    default: break; \
  } \
} 

#define AQ_API_Set_Devs_APPIA_EUR(id,reg,field,value) { \
  switch (port->device) { \
    case AQ_DEVICE_APPIA: \
      _local ## reg ## _APPIA->word_ ## reg ## _APPIA_ ## field = AQ_API_ReadRegister_Devs_APPIA_EUR(id,reg,reg ## _APPIA_ ## field); \
      if (_local ## reg ## _APPIA->bits_ ## reg ## _APPIA_ ## field.field != value) \
      { \
        _local ## reg ## _APPIA->bits_ ## reg ## _APPIA_ ## field.field = value; \
        AQ_API_WriteRegister_Devs_APPIA_EUR(id,reg,reg ## _APPIA_ ## field,_local ## reg ## _APPIA->word_ ## reg ## _APPIA_ ## field); \
      } \
      break; \
    case AQ_DEVICE_EUR: \
      _local ## reg ## _EUR->word_ ## reg ## _EUR_ ## field = AQ_API_ReadRegister_Devs_APPIA_EUR(id,reg,reg ## _EUR_ ## field); \
      if (_local ## reg ## _EUR->bits_ ## reg ## _EUR_ ## field.field != value) \
      { \
        _local ## reg ## _EUR->bits_ ## reg ## _EUR_ ## field.field = value; \
        AQ_API_WriteRegister_Devs_APPIA_EUR(id,reg,reg ## _EUR_ ## field,_local ## reg ## _EUR->word_ ## reg ## _EUR_ ## field); \
      } \
      break; \
    default: break; \
  } \
} 

#define AQ_API_Set_Devs_HHD_EUR(id,reg,field,value) { \
  switch (port->device) { \
    case AQ_DEVICE_HHD: \
      _local ## reg ## _HHD->word_ ## reg ## _HHD_ ## field = AQ_API_ReadRegister_Devs_HHD_EUR(id,reg,reg ## _HHD_ ## field); \
      if (_local ## reg ## _HHD->bits_ ## reg ## _HHD_ ## field.field != value) \
      { \
        _local ## reg ## _HHD->bits_ ## reg ## _HHD_ ## field.field = value; \
        AQ_API_WriteRegister_Devs_HHD_EUR(id,reg,reg ## _HHD_ ## field,_local ## reg ## _HHD->word_ ## reg ## _HHD_ ## field); \
      } \
      break; \
    case AQ_DEVICE_EUR: \
      _local ## reg ## _EUR->word_ ## reg ## _EUR_ ## field = AQ_API_ReadRegister_Devs_HHD_EUR(id,reg,reg ## _EUR_ ## field); \
      if (_local ## reg ## _EUR->bits_ ## reg ## _EUR_ ## field.field != value) \
      { \
        _local ## reg ## _EUR->bits_ ## reg ## _EUR_ ## field.field = value; \
        AQ_API_WriteRegister_Devs_HHD_EUR(id,reg,reg ## _EUR_ ## field,_local ## reg ## _EUR->word_ ## reg ## _EUR_ ## field); \
      } \
      break; \
    default: break; \
  } \
} 

#define AQ_API_Set_Devs_APPIA_HHD_EUR(id,reg,field,value) { \
  switch (port->device) { \
    case AQ_DEVICE_APPIA: \
      _local ## reg ## _APPIA->word_ ## reg ## _APPIA_ ## field = AQ_API_ReadRegister_Devs_APPIA_HHD_EUR(id,reg,reg ## _APPIA_ ## field); \
      if (_local ## reg ## _APPIA->bits_ ## reg ## _APPIA_ ## field.field != value) \
      { \
        _local ## reg ## _APPIA->bits_ ## reg ## _APPIA_ ## field.field = value; \
        AQ_API_WriteRegister_Devs_APPIA_HHD_EUR(id,reg,reg ## _APPIA_ ## field,_local ## reg ## _APPIA->word_ ## reg ## _APPIA_ ## field); \
      } \
      break; \
    case AQ_DEVICE_HHD: \
      _local ## reg ## _HHD->word_ ## reg ## _HHD_ ## field = AQ_API_ReadRegister_Devs_APPIA_HHD_EUR(id,reg,reg ## _HHD_ ## field); \
      if (_local ## reg ## _HHD->bits_ ## reg ## _HHD_ ## field.field != value) \
      { \
        _local ## reg ## _HHD->bits_ ## reg ## _HHD_ ## field.field = value; \
        AQ_API_WriteRegister_Devs_APPIA_HHD_EUR(id,reg,reg ## _HHD_ ## field,_local ## reg ## _HHD->word_ ## reg ## _HHD_ ## field); \
      } \
      break; \
    case AQ_DEVICE_EUR: \
      _local ## reg ## _EUR->word_ ## reg ## _EUR_ ## field = AQ_API_ReadRegister_Devs_APPIA_HHD_EUR(id,reg,reg ## _EUR_ ## field); \
      if (_local ## reg ## _EUR->bits_ ## reg ## _EUR_ ## field.field != value) \
      { \
        _local ## reg ## _EUR->bits_ ## reg ## _EUR_ ## field.field = value; \
        AQ_API_WriteRegister_Devs_APPIA_HHD_EUR(id,reg,reg ## _EUR_ ## field,_local ## reg ## _EUR->word_ ## reg ## _EUR_ ## field); \
      } \
      break; \
    default: break; \
  } \
} 

#define AQ_API_Set_Devs_HHD_APPIA(id,reg,field,value) AQ_API_Set_Devs_APPIA_HHD(id,reg,field,value)
#define AQ_API_Set_Devs_EUR_APPIA(id,reg,field,value) AQ_API_Set_Devs_APPIA_EUR(id,reg,field,value)
#define AQ_API_Set_Devs_EUR_HHD(id,reg,field,value) AQ_API_Set_Devs_HHD_EUR(id,reg,field,value)
#define AQ_API_Set_Devs_APPIA_EUR_HHD(id,reg,field,value) AQ_API_Set_Devs_APPIA_HHD_EUR(id,reg,field,value)
#define AQ_API_Set_Devs_HHD_APPIA_EUR(id,reg,field,value) AQ_API_Set_Devs_APPIA_HHD_EUR(id,reg,field,value)
#define AQ_API_Set_Devs_HHD_EUR_APPIA(id,reg,field,value) AQ_API_Set_Devs_APPIA_HHD_EUR(id,reg,field,value)
#define AQ_API_Set_Devs_EUR_APPIA_HHD(id,reg,field,value) AQ_API_Set_Devs_APPIA_HHD_EUR(id,reg,field,value)
#define AQ_API_Set_Devs_EUR_HHD_APPIA(id,reg,field,value) AQ_API_Set_Devs_APPIA_HHD_EUR(id,reg,field,value)


#define AQ_API_Get(id,reg,field,value) AQ_API_Get_DeviceRestricted(APPIA_HHD_EUR,id,reg,field,value)

#define AQ_API_Get_DeviceRestricted(devices,id,reg,field,value) AQ_API_Get_Devs_ ## devices(id,reg,field,value)

#define AQ_API_Get_Devs_APPIA(id,reg,field,value) { \
  switch (port->device) { \
    case AQ_DEVICE_APPIA: \
      _local ## reg ## _APPIA->word_ ## reg ## _APPIA_ ## field = AQ_API_ReadRegister_Devs_APPIA(id,reg,reg ## _APPIA_ ## field); \
      value = _local ## reg ## _APPIA->bits_ ## reg ## _APPIA_ ## field.field; \
      break; \
    default: value = 0; break; \
  } \
} 

#define AQ_API_Get_Devs_HHD(id,reg,field,value) { \
  switch (port->device) { \
    case AQ_DEVICE_HHD: \
      _local ## reg ## _HHD->word_ ## reg ## _HHD_ ## field = AQ_API_ReadRegister_Devs_HHD(id,reg,reg ## _HHD_ ## field); \
      value = _local ## reg ## _HHD->bits_ ## reg ## _HHD_ ## field.field; \
      break; \
    default: value = 0; break; \
  } \
} 

#define AQ_API_Get_Devs_EUR(id,reg,field,value) { \
  switch (port->device) { \
    case AQ_DEVICE_EUR: \
      _local ## reg ## _EUR->word_ ## reg ## _EUR_ ## field = AQ_API_ReadRegister_Devs_EUR(id,reg,reg ## _EUR_ ## field); \
      value = _local ## reg ## _EUR->bits_ ## reg ## _EUR_ ## field.field; \
      break; \
    default: value = 0; break; \
  } \
} 

#define AQ_API_Get_Devs_APPIA_HHD(id,reg,field,value) { \
  switch (port->device) { \
    case AQ_DEVICE_APPIA: \
      _local ## reg ## _APPIA->word_ ## reg ## _APPIA_ ## field = AQ_API_ReadRegister_Devs_APPIA_HHD(id,reg,reg ## _APPIA_ ## field); \
      value = _local ## reg ## _APPIA->bits_ ## reg ## _APPIA_ ## field.field; \
      break; \
    case AQ_DEVICE_HHD: \
      _local ## reg ## _HHD->word_ ## reg ## _HHD_ ## field = AQ_API_ReadRegister_Devs_APPIA_HHD(id,reg,reg ## _HHD_ ## field); \
      value = _local ## reg ## _HHD->bits_ ## reg ## _HHD_ ## field.field; \
      break; \
    default: value = 0; break; \
  } \
} 

#define AQ_API_Get_Devs_APPIA_EUR(id,reg,field,value) { \
  switch (port->device) { \
    case AQ_DEVICE_APPIA: \
      _local ## reg ## _APPIA->word_ ## reg ## _APPIA_ ## field = AQ_API_ReadRegister_Devs_APPIA_EUR(id,reg,reg ## _APPIA_ ## field); \
      value = _local ## reg ## _APPIA->bits_ ## reg ## _APPIA_ ## field.field; \
      break; \
    case AQ_DEVICE_EUR: \
      _local ## reg ## _EUR->word_ ## reg ## _EUR_ ## field = AQ_API_ReadRegister_Devs_APPIA_EUR(id,reg,reg ## _EUR_ ## field); \
      value = _local ## reg ## _EUR->bits_ ## reg ## _EUR_ ## field.field; \
      break; \
    default: value = 0; break; \
  } \
} 

#define AQ_API_Get_Devs_HHD_EUR(id,reg,field,value) { \
  switch (port->device) { \
    case AQ_DEVICE_HHD: \
      _local ## reg ## _HHD->word_ ## reg ## _HHD_ ## field = AQ_API_ReadRegister_Devs_HHD_EUR(id,reg,reg ## _HHD_ ## field); \
      value = _local ## reg ## _HHD->bits_ ## reg ## _HHD_ ## field.field; \
      break; \
    case AQ_DEVICE_EUR: \
      _local ## reg ## _EUR->word_ ## reg ## _EUR_ ## field = AQ_API_ReadRegister_Devs_HHD_EUR(id,reg,reg ## _EUR_ ## field); \
      value = _local ## reg ## _EUR->bits_ ## reg ## _EUR_ ## field.field; \
      break; \
    default: value = 0; break; \
  } \
} 

#define AQ_API_Get_Devs_APPIA_HHD_EUR(id,reg,field,value) { \
  switch (port->device) { \
    case AQ_DEVICE_APPIA: \
      _local ## reg ## _APPIA->word_ ## reg ## _APPIA_ ## field = AQ_API_ReadRegister_Devs_APPIA_HHD_EUR(id,reg,reg ## _APPIA_ ## field); \
      value = _local ## reg ## _APPIA->bits_ ## reg ## _APPIA_ ## field.field; \
      break; \
    case AQ_DEVICE_HHD: \
      _local ## reg ## _HHD->word_ ## reg ## _HHD_ ## field = AQ_API_ReadRegister_Devs_APPIA_HHD_EUR(id,reg,reg ## _HHD_ ## field); \
      value = _local ## reg ## _HHD->bits_ ## reg ## _HHD_ ## field.field; \
      break; \
    case AQ_DEVICE_EUR: \
      _local ## reg ## _EUR->word_ ## reg ## _EUR_ ## field = AQ_API_ReadRegister_Devs_APPIA_HHD_EUR(id,reg,reg ## _EUR_ ## field); \
      value = _local ## reg ## _EUR->bits_ ## reg ## _EUR_ ## field.field; \
      break; \
    default: value = 0; break; \
  } \
} 

#define AQ_API_Get_Devs_HHD_APPIA(id,reg,field,value) AQ_API_Get_Devs_APPIA_HHD(id,reg,field,value)
#define AQ_API_Get_Devs_EUR_APPIA(id,reg,field,value) AQ_API_Get_Devs_APPIA_EUR(id,reg,field,value)
#define AQ_API_Get_Devs_EUR_HHD(id,reg,field,value) AQ_API_Get_Devs_HHD_EUR(id,reg,field,value)
#define AQ_API_Get_Devs_APPIA_EUR_HHD(id,reg,field,value) AQ_API_Get_Devs_APPIA_HHD_EUR(id,reg,field,value)
#define AQ_API_Get_Devs_HHD_APPIA_EUR(id,reg,field,value) AQ_API_Get_Devs_APPIA_HHD_EUR(id,reg,field,value)
#define AQ_API_Get_Devs_HHD_EUR_APPIA(id,reg,field,value) AQ_API_Get_Devs_APPIA_HHD_EUR(id,reg,field,value)
#define AQ_API_Get_Devs_EUR_APPIA_HHD(id,reg,field,value) AQ_API_Get_Devs_APPIA_HHD_EUR(id,reg,field,value)
#define AQ_API_Get_Devs_EUR_HHD_APPIA(id,reg,field,value) AQ_API_Get_Devs_APPIA_HHD_EUR(id,reg,field,value)


#define AQ_API_BitfieldOfLocalStruct(reg,localvar,field) AQ_API_BitfieldOfLocalStruct_DeviceRestricted(APPIA_HHD_EUR,reg,localvar,field)

#define AQ_API_BitfieldOfLocalStruct_DeviceRestricted(devices,reg,localvar,field) AQ_API_BitfieldOfLocalStruct_Devs_ ## devices(reg,localvar,field)

#define AQ_API_BitfieldOfLocalStruct_Devs_APPIA(reg,localvar,field) \
    ((port->device == AQ_DEVICE_APPIA) ? ((localvar ## _APPIA)->bits_ ## reg ## _APPIA ## _ ## field.field) : \
    (0))

#define AQ_API_BitfieldOfLocalStruct_Devs_HHD(reg,localvar,field) \
    ((port->device == AQ_DEVICE_HHD) ? ((localvar ## _HHD)->bits_ ## reg ## _HHD ## _ ## field.field) : \
    (0))

#define AQ_API_BitfieldOfLocalStruct_Devs_EUR(reg,localvar,field) \
    ((port->device == AQ_DEVICE_EUR) ? ((localvar ## _EUR)->bits_ ## reg ## _EUR ## _ ## field.field) : \
    (0))

#define AQ_API_BitfieldOfLocalStruct_Devs_APPIA_HHD(reg,localvar,field) \
    ((port->device == AQ_DEVICE_HHD) ? ((localvar ## _HHD)->bits_ ## reg ## _HHD ## _ ## field.field) : \
    ((port->device == AQ_DEVICE_APPIA) ? ((localvar ## _APPIA)->bits_ ## reg ## _APPIA ## _ ## field.field) : \
    (0)))

#define AQ_API_BitfieldOfLocalStruct_Devs_APPIA_EUR(reg,localvar,field) \
    ((port->device == AQ_DEVICE_EUR) ? ((localvar ## _EUR)->bits_ ## reg ## _EUR ## _ ## field.field) : \
    ((port->device == AQ_DEVICE_APPIA) ? ((localvar ## _APPIA)->bits_ ## reg ## _APPIA ## _ ## field.field) : \
    (0)))

#define AQ_API_BitfieldOfLocalStruct_Devs_HHD_EUR(reg,localvar,field) \
    ((port->device == AQ_DEVICE_EUR) ? ((localvar ## _EUR)->bits_ ## reg ## _EUR ## _ ## field.field) : \
    ((port->device == AQ_DEVICE_HHD) ? ((localvar ## _HHD)->bits_ ## reg ## _HHD ## _ ## field.field) : \
    (0)))

#define AQ_API_BitfieldOfLocalStruct_Devs_APPIA_HHD_EUR(reg,localvar,field) \
    ((port->device == AQ_DEVICE_EUR) ? ((localvar ## _EUR)->bits_ ## reg ## _EUR ## _ ## field.field) : \
    ((port->device == AQ_DEVICE_HHD) ? ((localvar ## _HHD)->bits_ ## reg ## _HHD ## _ ## field.field) : \
    ((port->device == AQ_DEVICE_APPIA) ? ((localvar ## _APPIA)->bits_ ## reg ## _APPIA ## _ ## field.field) : \
    (0))))

#define AQ_API_BitfieldOfLocalStruct_Devs_HHD_APPIA(reg,localvar,field) AQ_API_BitfieldOfLocalStruct_Devs_APPIA_HHD(reg,localvar,field)
#define AQ_API_BitfieldOfLocalStruct_Devs_EUR_APPIA(reg,localvar,field) AQ_API_BitfieldOfLocalStruct_Devs_APPIA_EUR(reg,localvar,field)
#define AQ_API_BitfieldOfLocalStruct_Devs_EUR_HHD(reg,localvar,field) AQ_API_BitfieldOfLocalStruct_Devs_HHD_EUR(reg,localvar,field)
#define AQ_API_BitfieldOfLocalStruct_Devs_APPIA_EUR_HHD(reg,localvar,field) AQ_API_BitfieldOfLocalStruct_Devs_APPIA_HHD_EUR(reg,localvar,field)
#define AQ_API_BitfieldOfLocalStruct_Devs_HHD_APPIA_EUR(reg,localvar,field) AQ_API_BitfieldOfLocalStruct_Devs_APPIA_HHD_EUR(reg,localvar,field)
#define AQ_API_BitfieldOfLocalStruct_Devs_HHD_EUR_APPIA(reg,localvar,field) AQ_API_BitfieldOfLocalStruct_Devs_APPIA_HHD_EUR(reg,localvar,field)
#define AQ_API_BitfieldOfLocalStruct_Devs_EUR_APPIA_HHD(reg,localvar,field) AQ_API_BitfieldOfLocalStruct_Devs_APPIA_HHD_EUR(reg,localvar,field)
#define AQ_API_BitfieldOfLocalStruct_Devs_EUR_HHD_APPIA(reg,localvar,field) AQ_API_BitfieldOfLocalStruct_Devs_APPIA_HHD_EUR(reg,localvar,field)


#define AQ_API_AssignBitfieldOfLocalStruct(reg,localvar,field,value) AQ_API_AssignBitfieldOfLocalStruct_DeviceRestricted(APPIA_HHD_EUR,reg,localvar,field,value)

#define AQ_API_AssignBitfieldOfLocalStruct_DeviceRestricted(devices,reg,localvar,field,value) AQ_API_AssignBitfieldOfLocalStruct_Devs_ ## devices(reg,localvar,field,value)

#define AQ_API_AssignBitfieldOfLocalStruct_Devs_APPIA(reg,localvar,field,value) \
    ((port->device == AQ_DEVICE_APPIA) ? ((localvar ## _APPIA)->bits_ ## reg ## _APPIA ## _ ## field.field = value) : \
    (0))

#define AQ_API_AssignBitfieldOfLocalStruct_Devs_HHD(reg,localvar,field,value) \
    ((port->device == AQ_DEVICE_HHD) ? ((localvar ## _HHD)->bits_ ## reg ## _HHD ## _ ## field.field = value) : \
    (0))

#define AQ_API_AssignBitfieldOfLocalStruct_Devs_EUR(reg,localvar,field,value) \
    ((port->device == AQ_DEVICE_EUR) ? ((localvar ## _EUR)->bits_ ## reg ## _EUR ## _ ## field.field = value) : \
    (0))

#define AQ_API_AssignBitfieldOfLocalStruct_Devs_APPIA_HHD(reg,localvar,field,value) \
    ((port->device == AQ_DEVICE_HHD) ? ((localvar ## _HHD)->bits_ ## reg ## _HHD ## _ ## field.field = value) : \
    ((port->device == AQ_DEVICE_APPIA) ? ((localvar ## _APPIA)->bits_ ## reg ## _APPIA ## _ ## field.field = value) : \
    (0)))

#define AQ_API_AssignBitfieldOfLocalStruct_Devs_APPIA_EUR(reg,localvar,field,value) \
    ((port->device == AQ_DEVICE_EUR) ? ((localvar ## _EUR)->bits_ ## reg ## _EUR ## _ ## field.field = value) : \
    ((port->device == AQ_DEVICE_APPIA) ? ((localvar ## _APPIA)->bits_ ## reg ## _APPIA ## _ ## field.field = value) : \
    (0)))

#define AQ_API_AssignBitfieldOfLocalStruct_Devs_HHD_EUR(reg,localvar,field,value) \
    ((port->device == AQ_DEVICE_EUR) ? ((localvar ## _EUR)->bits_ ## reg ## _EUR ## _ ## field.field = value) : \
    ((port->device == AQ_DEVICE_HHD) ? ((localvar ## _HHD)->bits_ ## reg ## _HHD ## _ ## field.field = value) : \
    (0)))

#define AQ_API_AssignBitfieldOfLocalStruct_Devs_APPIA_HHD_EUR(reg,localvar,field,value) \
    ((port->device == AQ_DEVICE_EUR) ? ((localvar ## _EUR)->bits_ ## reg ## _EUR ## _ ## field.field = value) : \
    ((port->device == AQ_DEVICE_HHD) ? ((localvar ## _HHD)->bits_ ## reg ## _HHD ## _ ## field.field = value) : \
    ((port->device == AQ_DEVICE_APPIA) ? ((localvar ## _APPIA)->bits_ ## reg ## _APPIA ## _ ## field.field = value) : \
    (0))))

#define AQ_API_AssignBitfieldOfLocalStruct_Devs_HHD_APPIA(reg,localvar,field,value) AQ_API_AssignBitfieldOfLocalStruct_Devs_APPIA_HHD(reg,localvar,field,value)
#define AQ_API_AssignBitfieldOfLocalStruct_Devs_EUR_APPIA(reg,localvar,field,value) AQ_API_AssignBitfieldOfLocalStruct_Devs_APPIA_EUR(reg,localvar,field,value)
#define AQ_API_AssignBitfieldOfLocalStruct_Devs_EUR_HHD(reg,localvar,field,value) AQ_API_AssignBitfieldOfLocalStruct_Devs_HHD_EUR(reg,localvar,field,value)
#define AQ_API_AssignBitfieldOfLocalStruct_Devs_APPIA_EUR_HHD(reg,localvar,field,value) AQ_API_AssignBitfieldOfLocalStruct_Devs_APPIA_HHD_EUR(reg,localvar,field,value)
#define AQ_API_AssignBitfieldOfLocalStruct_Devs_HHD_APPIA_EUR(reg,localvar,field,value) AQ_API_AssignBitfieldOfLocalStruct_Devs_APPIA_HHD_EUR(reg,localvar,field,value)
#define AQ_API_AssignBitfieldOfLocalStruct_Devs_HHD_EUR_APPIA(reg,localvar,field,value) AQ_API_AssignBitfieldOfLocalStruct_Devs_APPIA_HHD_EUR(reg,localvar,field,value)
#define AQ_API_AssignBitfieldOfLocalStruct_Devs_EUR_APPIA_HHD(reg,localvar,field,value) AQ_API_AssignBitfieldOfLocalStruct_Devs_APPIA_HHD_EUR(reg,localvar,field,value)
#define AQ_API_AssignBitfieldOfLocalStruct_Devs_EUR_HHD_APPIA(reg,localvar,field,value) AQ_API_AssignBitfieldOfLocalStruct_Devs_APPIA_HHD_EUR(reg,localvar,field,value)


#define AQ_API_WordOfLocalStruct(localvar,wd) AQ_API_WordOfLocalStruct_DeviceRestricted(APPIA_HHD_EUR,localvar,wd)

#define AQ_API_WordOfLocalStruct_DeviceRestricted(devices,localvar,wd) AQ_API_WordOfLocalStruct_Devs_ ## devices(localvar,wd)

#define AQ_API_WordOfLocalStruct_Devs_APPIA(localvar,wd) \
    ((port->device == AQ_DEVICE_APPIA) ? ((localvar ## _APPIA)->u ## wd.word_ ## wd) : \
    (0))

#define AQ_API_WordOfLocalStruct_Devs_HHD(localvar,wd) \
    ((port->device == AQ_DEVICE_HHD) ? ((localvar ## _HHD)->u ## wd.word_ ## wd) : \
    (0))

#define AQ_API_WordOfLocalStruct_Devs_EUR(localvar,wd) \
    ((port->device == AQ_DEVICE_EUR) ? ((localvar ## _EUR)->u ## wd.word_ ## wd) : \
    (0))

#define AQ_API_WordOfLocalStruct_Devs_APPIA_HHD(localvar,wd) \
    ((port->device == AQ_DEVICE_HHD) ? ((localvar ## _HHD)->u ## wd.word_ ## wd) : \
    ((port->device == AQ_DEVICE_APPIA) ? ((localvar ## _APPIA)->u ## wd.word_ ## wd) : \
    (0)))

#define AQ_API_WordOfLocalStruct_Devs_APPIA_EUR(localvar,wd) \
    ((port->device == AQ_DEVICE_EUR) ? ((localvar ## _EUR)->u ## wd.word_ ## wd) : \
    ((port->device == AQ_DEVICE_APPIA) ? ((localvar ## _APPIA)->u ## wd.word_ ## wd) : \
    (0)))

#define AQ_API_WordOfLocalStruct_Devs_HHD_EUR(localvar,wd) \
    ((port->device == AQ_DEVICE_EUR) ? ((localvar ## _EUR)->u ## wd.word_ ## wd) : \
    ((port->device == AQ_DEVICE_HHD) ? ((localvar ## _HHD)->u ## wd.word_ ## wd) : \
    (0)))

#define AQ_API_WordOfLocalStruct_Devs_APPIA_HHD_EUR(localvar,wd) \
    ((port->device == AQ_DEVICE_EUR) ? ((localvar ## _EUR)->u ## wd.word_ ## wd) : \
    ((port->device == AQ_DEVICE_HHD) ? ((localvar ## _HHD)->u ## wd.word_ ## wd) : \
    ((port->device == AQ_DEVICE_APPIA) ? ((localvar ## _APPIA)->u ## wd.word_ ## wd) : \
    (0))))

#define AQ_API_WordOfLocalStruct_Devs_HHD_APPIA(localvar,wd) AQ_API_WordOfLocalStruct_Devs_APPIA_HHD(localvar,wd)
#define AQ_API_WordOfLocalStruct_Devs_EUR_APPIA(localvar,wd) AQ_API_WordOfLocalStruct_Devs_APPIA_EUR(localvar,wd)
#define AQ_API_WordOfLocalStruct_Devs_EUR_HHD(localvar,wd) AQ_API_WordOfLocalStruct_Devs_HHD_EUR(localvar,wd)
#define AQ_API_WordOfLocalStruct_Devs_APPIA_EUR_HHD(localvar,wd) AQ_API_WordOfLocalStruct_Devs_APPIA_HHD_EUR(localvar,wd)
#define AQ_API_WordOfLocalStruct_Devs_HHD_APPIA_EUR(localvar,wd) AQ_API_WordOfLocalStruct_Devs_APPIA_HHD_EUR(localvar,wd)
#define AQ_API_WordOfLocalStruct_Devs_HHD_EUR_APPIA(localvar,wd) AQ_API_WordOfLocalStruct_Devs_APPIA_HHD_EUR(localvar,wd)
#define AQ_API_WordOfLocalStruct_Devs_EUR_APPIA_HHD(localvar,wd) AQ_API_WordOfLocalStruct_Devs_APPIA_HHD_EUR(localvar,wd)
#define AQ_API_WordOfLocalStruct_Devs_EUR_HHD_APPIA(localvar,wd) AQ_API_WordOfLocalStruct_Devs_APPIA_HHD_EUR(localvar,wd)


#define AQ_API_AssignWordOfLocalStruct(localvar,wd,value) AQ_API_AssignWordOfLocalStruct_DeviceRestricted(APPIA_HHD_EUR,localvar,wd,value)

#define AQ_API_AssignWordOfLocalStruct_DeviceRestricted(devices,localvar,wd,value) AQ_API_AssignWordOfLocalStruct_Devs_ ## devices(localvar,wd,value)

#define AQ_API_AssignWordOfLocalStruct_Devs_APPIA(localvar,wd,value) \
    ((port->device == AQ_DEVICE_APPIA) ? ((localvar ## _APPIA)->u ## wd.word_ ## wd = value) : \
    (0))

#define AQ_API_AssignWordOfLocalStruct_Devs_HHD(localvar,wd,value) \
    ((port->device == AQ_DEVICE_HHD) ? ((localvar ## _HHD)->u ## wd.word_ ## wd = value) : \
    (0))

#define AQ_API_AssignWordOfLocalStruct_Devs_EUR(localvar,wd,value) \
    ((port->device == AQ_DEVICE_EUR) ? ((localvar ## _EUR)->u ## wd.word_ ## wd = value) : \
    (0))

#define AQ_API_AssignWordOfLocalStruct_Devs_APPIA_HHD(localvar,wd,value) \
    ((port->device == AQ_DEVICE_HHD) ? ((localvar ## _HHD)->u ## wd.word_ ## wd = value) : \
    ((port->device == AQ_DEVICE_APPIA) ? ((localvar ## _APPIA)->u ## wd.word_ ## wd = value) : \
    (0)))

#define AQ_API_AssignWordOfLocalStruct_Devs_APPIA_EUR(localvar,wd,value) \
    ((port->device == AQ_DEVICE_EUR) ? ((localvar ## _EUR)->u ## wd.word_ ## wd = value) : \
    ((port->device == AQ_DEVICE_APPIA) ? ((localvar ## _APPIA)->u ## wd.word_ ## wd = value) : \
    (0)))

#define AQ_API_AssignWordOfLocalStruct_Devs_HHD_EUR(localvar,wd,value) \
    ((port->device == AQ_DEVICE_EUR) ? ((localvar ## _EUR)->u ## wd.word_ ## wd = value) : \
    ((port->device == AQ_DEVICE_HHD) ? ((localvar ## _HHD)->u ## wd.word_ ## wd = value) : \
    (0)))

#define AQ_API_AssignWordOfLocalStruct_Devs_APPIA_HHD_EUR(localvar,wd,value) \
    ((port->device == AQ_DEVICE_EUR) ? ((localvar ## _EUR)->u ## wd.word_ ## wd = value) : \
    ((port->device == AQ_DEVICE_HHD) ? ((localvar ## _HHD)->u ## wd.word_ ## wd = value) : \
    ((port->device == AQ_DEVICE_APPIA) ? ((localvar ## _APPIA)->u ## wd.word_ ## wd = value) : \
    (0))))

#define AQ_API_AssignWordOfLocalStruct_Devs_HHD_APPIA(localvar,wd,value) AQ_API_AssignWordOfLocalStruct_Devs_APPIA_HHD(localvar,wd,value)
#define AQ_API_AssignWordOfLocalStruct_Devs_EUR_APPIA(localvar,wd,value) AQ_API_AssignWordOfLocalStruct_Devs_APPIA_EUR(localvar,wd,value)
#define AQ_API_AssignWordOfLocalStruct_Devs_EUR_HHD(localvar,wd,value) AQ_API_AssignWordOfLocalStruct_Devs_HHD_EUR(localvar,wd,value)
#define AQ_API_AssignWordOfLocalStruct_Devs_APPIA_EUR_HHD(localvar,wd,value) AQ_API_AssignWordOfLocalStruct_Devs_APPIA_HHD_EUR(localvar,wd,value)
#define AQ_API_AssignWordOfLocalStruct_Devs_HHD_APPIA_EUR(localvar,wd,value) AQ_API_AssignWordOfLocalStruct_Devs_APPIA_HHD_EUR(localvar,wd,value)
#define AQ_API_AssignWordOfLocalStruct_Devs_HHD_EUR_APPIA(localvar,wd,value) AQ_API_AssignWordOfLocalStruct_Devs_APPIA_HHD_EUR(localvar,wd,value)
#define AQ_API_AssignWordOfLocalStruct_Devs_EUR_APPIA_HHD(localvar,wd,value) AQ_API_AssignWordOfLocalStruct_Devs_APPIA_HHD_EUR(localvar,wd,value)
#define AQ_API_AssignWordOfLocalStruct_Devs_EUR_HHD_APPIA(localvar,wd,value) AQ_API_AssignWordOfLocalStruct_Devs_APPIA_HHD_EUR(localvar,wd,value)


#define AQ_API_Field(id,reg,field) AQ_API_Field_DeviceRestricted(APPIA_HHD_EUR,id,reg,field)

#define AQ_API_Field_DeviceRestricted(devices,id,reg,field) AQ_API_Field_Devs_ ## devices(id,reg,field)

#define AQ_API_Field_Devs_APPIA(id,reg,field) \
    ((port->device == AQ_DEVICE_APPIA) ? (reg ## _APPIA_ ## field) : \
    (0))

#define AQ_API_Field_Devs_HHD(id,reg,field) \
    ((port->device == AQ_DEVICE_HHD) ? (reg ## _HHD_ ## field) : \
    (0))

#define AQ_API_Field_Devs_EUR(id,reg,field) \
    ((port->device == AQ_DEVICE_EUR) ? (reg ## _EUR_ ## field) : \
    (0))

#define AQ_API_Field_Devs_APPIA_HHD(id,reg,field) \
    ((port->device == AQ_DEVICE_HHD) ? (reg ## _HHD_ ## field) : \
    ((port->device == AQ_DEVICE_APPIA) ? (reg ## _APPIA_ ## field) : \
    (0)))

#define AQ_API_Field_Devs_APPIA_EUR(id,reg,field) \
    ((port->device == AQ_DEVICE_EUR) ? (reg ## _EUR_ ## field) : \
    ((port->device == AQ_DEVICE_APPIA) ? (reg ## _APPIA_ ## field) : \
    (0)))

#define AQ_API_Field_Devs_HHD_EUR(id,reg,field) \
    ((port->device == AQ_DEVICE_EUR) ? (reg ## _EUR_ ## field) : \
    ((port->device == AQ_DEVICE_HHD) ? (reg ## _HHD_ ## field) : \
    (0)))

#define AQ_API_Field_Devs_APPIA_HHD_EUR(id,reg,field) \
    ((port->device == AQ_DEVICE_EUR) ? (reg ## _EUR_ ## field) : \
    ((port->device == AQ_DEVICE_HHD) ? (reg ## _HHD_ ## field) : \
    ((port->device == AQ_DEVICE_APPIA) ? (reg ## _APPIA_ ## field) : \
    (0))))

#define AQ_API_Field_Devs_HHD_APPIA(id,reg,field) AQ_API_Field_Devs_APPIA_HHD(id,reg,field)
#define AQ_API_Field_Devs_EUR_APPIA(id,reg,field) AQ_API_Field_Devs_APPIA_EUR(id,reg,field)
#define AQ_API_Field_Devs_EUR_HHD(id,reg,field) AQ_API_Field_Devs_HHD_EUR(id,reg,field)
#define AQ_API_Field_Devs_APPIA_EUR_HHD(id,reg,field) AQ_API_Field_Devs_APPIA_HHD_EUR(id,reg,field)
#define AQ_API_Field_Devs_HHD_APPIA_EUR(id,reg,field) AQ_API_Field_Devs_APPIA_HHD_EUR(id,reg,field)
#define AQ_API_Field_Devs_HHD_EUR_APPIA(id,reg,field) AQ_API_Field_Devs_APPIA_HHD_EUR(id,reg,field)
#define AQ_API_Field_Devs_EUR_APPIA_HHD(id,reg,field) AQ_API_Field_Devs_APPIA_HHD_EUR(id,reg,field)
#define AQ_API_Field_Devs_EUR_HHD_APPIA(id,reg,field) AQ_API_Field_Devs_APPIA_HHD_EUR(id,reg,field)


#endif


/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Header File for IC Export Function
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: icExport.h,v 1.12 2006-08-01 09:48:00 yjlou Exp $
*/
#include "rtl_types.h"
#include "virtualMac.h"

enum EXPORT_REGISTER_FORMAT
{
	EXPORT_RTK_SIM,			/* Export registers for designer to simulate IC operation, said 'memw' commands. */
	EXPORT_RTK_EXPECT,		/* Export registers for designer to compare results, said 'memr' commands. */
	EXPORT_CLESHELL,		/* Export registers to import from CLE Shell command. */
	EXPORT_ICEMON,				/* Export registers to for ICE MON command. */
};

/*===========================================================================*
 *  Common Export Functions                                                  *
 *===========================================================================*/
char* modelExportRaw( char* str );
#define modelExportValue( addr, value ) modelExportValueMask( addr, value, 0xffffffff )
char* modelExportValueMask( uint32 addr, uint32 value, uint32 mask );
char* modelExportPointerRegister( uint32 addr );
#define modelExportRegister( addr ) modelExportRegisterMask( addr, 0xffffffff )
char* modelExportRegisterMask( uint32 addr, uint32 mask );
char* modelExportMemory( uint32 addr, uint32 len );
char* modelExportComment( char* pszComment );
char* modelExportNop( int32 delayCycles );

/*===========================================================================*
 *  IC-Specified Export Functions                                            *
 *===========================================================================*/
int32 modelExportGdmaRegisters( void );
int32 modelExportSwcoreRegistersAndTables( void );
int32 modelExportAllRegisters( void );


/*===========================================================================*
 *   VIRTUAL ADDRESS TO PHYSICAL ADDRESS                                     *
 *===========================================================================*/
uint32 queryModelToPhysical( uint32 virtual );
uint32 queryPhysicalToModel( uint32 physical );
int32 addModelToPhysical( uint32 virtualStart, uint32 virtualEnd, uint32 physicalStart );
int32 delModelToPhysical( uint32 virtualStart );

int32 modelExportSetOutputForm( enum EXPORT_REGISTER_FORMAT newFormat );
int32 modelExportGetOutputForm( enum EXPORT_REGISTER_FORMAT *oldFormat );
int32 modelExportSetIcType( enum IC_TYPE newType );
int32 modelExportGetIcType( enum IC_TYPE *oldType );
int32 modelExportOpenFile( char* pszFilename );
int32 modelExportCloseFile( void );
int32 runModelExport(uint32 userId,  int32 argc,int8 **saved);


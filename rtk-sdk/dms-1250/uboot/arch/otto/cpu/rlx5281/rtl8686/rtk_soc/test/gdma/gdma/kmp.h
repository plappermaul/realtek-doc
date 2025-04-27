/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Header file for FSM of KMP
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: kmp.h,v 1.4 2006-07-13 16:03:07 chenyl Exp $
*/
#include "rtl_types.h"
#include "gdma.h"
#include "vsm.h"


#ifndef _FSM_KMP_
#define _FSM_KMP_

int32 vsmGenFromKMP( pmVsm_t* pVsm, uint8* pattern, uint32 lenPattern );

#endif

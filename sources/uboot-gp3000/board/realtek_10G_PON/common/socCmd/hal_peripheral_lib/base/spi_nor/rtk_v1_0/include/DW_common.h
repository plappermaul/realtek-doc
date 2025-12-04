/* --------------------------------------------------------------------
**
**  This software and the associated documentation are confidential
**  and proprietary to Synopsys Inc.  Your use or disclosure of this
**  software is subject to the terms and conditions of a written
**  license agreement between you, or your company, and Synopsys Inc.
**  In the event of publication, the following notice is applicable:
**
**                 COPYRIGHT (C) 2004 SYNOPSYS INC.
**                       ALL RIGHTS RESERVED
**
** The entire notice above must be reproduced on all authorized copies
**
** --------------------------------------------------------------------
**
** File     : DW_common.h
** Created  : Fri Nov 15 17:22:26 IST 2002
** Modified : $Date: 2005/04/06 $
** Abstract : Common header files
**
** --------------------------------------------------------------------
*/


#ifndef DW_COMMON_H
#define DW_COMMON_H

#include <stdarg.h>             // variable-length argument support
#include <stddef.h>             // standard definitions
//#include <inttypes.h>           // defined-width data types
#include "DW_common_list.h"     // linked list macros
#include "DW_common_types.h"    // custom data type definitions
#include "DW_common_io.h"       // low-level I/O
#include "DW_common_bitops.h"   // bit-manipulation macros
#include "DW_common_errno.h"    // error codes
#include "DW_common_dbc.h"      // assertion macros

#endif  // DW_COMMON_H


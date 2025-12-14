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
** File     : DW_common_errno.h
** Created  : Fri Aug 15 17:16:59 IST 2003
** Modified : $Date: 2005/04/06 $
** Abstract : List of error codes.
**
** --------------------------------------------------------------------
*/

#ifndef DW_COMMON_ERRNO_H
#define DW_COMMON_ERRNO_H

#define DW_EPERM            1   // operation not permitted
#define DW_EIO              5   // I/O error
#define DW_ENXIO            6   // no such device or address
#define DW_ENOMEM           12  // out of memory
#define DW_EACCES           13  // permission denied
#define DW_EBUSY            16  // device or resource busy
#define DW_ENODEV           19  // no such device
#define DW_EINVAL           22  // invalid argument
#define DW_ENOSPC           28  // no space left on device
#define DW_ENOSYS           38  // function not implemented/supported
#define DW_ECHRNG           44  // channel number out of range
#define DW_ENODATA          61  // no data available
#define DW_ETIME            62  // timer expired
#define DW_EPROTO           71  // protocol error


#endif  // DW_COMMON_ERRNO_H


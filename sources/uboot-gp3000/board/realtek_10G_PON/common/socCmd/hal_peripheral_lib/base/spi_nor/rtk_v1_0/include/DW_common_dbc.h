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
** File     : DW_common_dbc.h
** Created  : Fri Aug 15 17:19:19 IST 2003
** Modified : $Date: 2005/04/06 $
** Abstract : Assertion macros used to aid debugging.
**
** --------------------------------------------------------------------
*/

#ifndef DW_COMMON_DBC_H
#define DW_COMMON_DBC_H

#ifdef __cplusplus
extern "C" {    // allow C++ to use these headers
#endif

// DW_NASSERT macro disables all contract validations
// (assertions, preconditions, postconditions, and invariants).
#ifndef DW_NASSERT 

// callback invoked in case of assertion failure
extern void onAssert__(char const *file, unsigned line);

#define DW_DEFINE_THIS_FILE static const char THIS_FILE__[] = __FILE__

#define DW_ASSERT(test_)   \
        if (test_) {    \
        }               \
        else onAssert__(THIS_FILE__, __LINE__)

#define DW_REQUIRE(test_)           DW_ASSERT(test_)
#define DW_ENSURE(test_)            DW_ASSERT(test_)
#define DW_INVARIANT(test_)         DW_ASSERT(test_)
#define DW_ALLEGE(test_)            DW_ASSERT(test_)

#else   // DW_NASSERT

#define DW_DEFINE_THIS_FILE extern const char THIS_FILE__[]
#define DW_ASSERT(test_)
#define DW_REQUIRE(test_)
#define DW_ENSURE(test_)
#define DW_INVARIANT(test_)
#define DW_ALLEGE(test_)   \
   if(test_) {          \
   }                    \
   else

#endif  // DW_NASSERT


#ifdef __cplusplus
}
#endif

#endif  // DW_COMMON_DBC_H


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
** File     : DW_common_bitops.h
** Created  : Fri Nov 15 17:21:08 IST 2002
** Modified : $Date: 2005/04/06 $
** Abstract : Bit manipulation macros.
**
** --------------------------------------------------------------------
*/

#ifndef DW_COMMON_BITOPS_H
#define DW_COMMON_BITOPS_H

#ifdef __cplusplus
extern "C" {    // allow C++ to use these headers
#endif

/****h* include.bitops
 * NAME
 *  DW_common_bitops.h -- bit manipulation macros
 * DESCRIPTION
 *  This set of macros accesses data using a width/shift parameter.
 *  This assumes that constants bfoXXX and bfwXXX are defined, 
 *  where XXX is the value specified in the parameter.
 *  + bfoXXX is the offset of the lowest bit.
 *  + bfwXXX is the number of bits to be accessed.
 ***/

/****d* include.bitops/DW_BITS
 * DESCRIPTION
 *  Constant definitions for various bits of a 32-bit word.
 * SOURCE
 */
#define DW_BIT0     0x00000001
#define DW_BIT1     0x00000002
#define DW_BIT2     0x00000004
#define DW_BIT3     0x00000008
#define DW_BIT4     0x00000010
#define DW_BIT5     0x00000020
#define DW_BIT6     0x00000040
#define DW_BIT7     0x00000080
#define DW_BIT8     0x00000100
#define DW_BIT9     0x00000200
#define DW_BIT10    0x00000400
#define DW_BIT11    0x00000800
#define DW_BIT12    0x00001000
#define DW_BIT13    0x00002000
#define DW_BIT14    0x00004000
#define DW_BIT15    0x00008000
#define DW_BIT16    0x00010000
#define DW_BIT17    0x00020000
#define DW_BIT18    0x00040000
#define DW_BIT19    0x00080000
#define DW_BIT20    0x00100000
#define DW_BIT21    0x00200000
#define DW_BIT22    0x00400000
#define DW_BIT23    0x00800000
#define DW_BIT24    0x01000000
#define DW_BIT25    0x02000000
#define DW_BIT26    0x04000000
#define DW_BIT27    0x08000000
#define DW_BIT28    0x10000000
#define DW_BIT29    0x20000000
#define DW_BIT30    0x40000000
#define DW_BIT31    0x80000000
#define DW_BITS_ALL 0xFFFFFFFF
/*****/

/****d* include.bitops/DW_BIT_WIDTH
 * DESCRIPTION
 *  Returns the width of the specified bit-field.
 * ARGUMENTS
 *  __bfws      a width/shift pair       
 * SOURCE
 */
#define DW_BIT_WIDTH(__bfws)    ((uint32_t) (bfw ## __bfws))
/*****/

/****d* include.bitops/DW_BIT_OFFSET
 * DESCRIPTION
 *  Returns the offset of the specified bit-field.
 * ARGUMENTS
 *  __bfws      a width/shift pair       
 * SOURCE
 */
#define DW_BIT_OFFSET(__bfws)   ((uint32_t) (bfo ## __bfws))
/*****/

/****d* include.bitops/DW_BIT_MASK
 * DESCRIPTION
 *  Returns a mask with the bits to be addressed set and all others
 *  cleared.
 * ARGUMENTS
 *  __bfws      shift pair       
 *  __bits      bit width
 * SOURCE
 */
#define DW_BIT_MASK(__bfws) ((uint32_t) (((bfw ## __bfws) == 32) ?  \
        0xFFFFFFFF : ((1U << (bfw ## __bfws)) - 1)) << (bfo ## __bfws))

#define DW_BITS_MASK(__bfws, __bits) ((uint32_t) (( __bfws) == 32) ?  \
        0x0 : ( ((0xffffffff)>>(32- __bits)) <<(__bfws)))

#define DW_BIT_MASK(__bfws) ((uint32_t) (( __bfws) == 32) ?  \
        0x0 : ( 0x1 <<(__bfws)))

#define DW_BIT_MASK_WIDTH(__bfws, __bits) ((uint32_t) (( __bfws) == 32) ?  \
        0xFFFFFFFF : (((1<< (__bits)) - 1) << (__bfws)))
/*****/

/****d* include.bitops/DW_BIT_CLEAR
 * DESCRIPTION
 *  Clear the specified bits.
 * ARGUMENTS
 *  __datum     the word of data to be modified
 *  __bfws      shift pair      
 *  __bits      bit width
 * SOURCE
 */
#define DW_BITS_CLEAR(__datum, __bfws, __bits)                      \
    ((__datum) = ((uint32_t) (__datum) & ~DW_BITS_MASK(__bfws, __bits)))

#define DW_BIT_CLEAR(__datum, __bfws)                               \
    ((__datum) = ((uint32_t) (__datum) & ~DW_BIT_MASK(__bfws)))
   
/*****/

/****d* include.bitops/DW_BIT_GET_UNSHIFTED
 * DESCRIPTION
 *  Returns the relevant bits masked from the data word, still at their
 *  original offset.
 * ARGUMENTS
 *  __datum     the word of data to be accessed
 *  __bfws      a width/shift pair       
 * SOURCE
 */
#define DW_BIT_GET_UNSHIFTED(__datum, __bfws)                       \
    ((uint32_t) ((__datum) & DW_BIT_MASK(__bfws)))
/*****/

/****d* include.bitops/DW_BIT_GET
 * DESCRIPTION
 *  Returns the relevant bits masked from the data word shifted to bit
 *  zero (i.e. access the specifed bits from a word of data as an
 *  integer value).
 * ARGUMENTS
 *  __datum     the word of data to be accessed
 *  __bfws      a width/shift pair      
 *
 * SOURCE
 */
#define DW_BIT_GET(__datum, __bfws)                                 \
    ((uint32_t) (((__datum) & DW_BIT_MASK(__bfws)) >>               \
                 (__bfws)))
/*****/

/****d* include.bitops/DW_BIT_SET
 * DESCRIPTION
 *  Place the specified value into the specified bits of a word of data
 *  (first the data is read, and the non-specified bits are re-written).
 * ARGUMENTS
 *  __datum     the word of data to be accessed
 *  __bfws      a width/shift pair      
 *  __bits      bit width
 *  __val       the data value to be shifted into the specified bits
 * SOURCE
 */
#define DW_BIT_SET(__datum, __bfws, __val)                          \
    ((__datum) = ((uint32_t) (__datum) & ~DW_BIT_MASK(__bfws)) |    \
            ((__val << (bfo ## __bfws)) & DW_BIT_MASK(__bfws)))

#define DW_BITS_SET(__datum, __bfws, __bits)                          \
    ((__datum) = ((uint32_t) (__datum) & ~DW_BITS_MASK(__bfws, __bits)) |    \
            (DW_BITS_MASK(__bfws, __bits)))

#define DW_BIT_SET(__datum, __bfws)                          \
    ((__datum) = ((uint32_t) (__datum) & ~DW_BIT_MASK(__bfws)) |    \
            (DW_BIT_MASK(__bfws)))

#define DW_BITS_SET_VAL(__datum, __bfws, __val, bit_num)                          \
    ((__datum) = ((uint32_t) (__datum) & ~DW_BIT_MASK_WIDTH(__bfws, bit_num)) |    \
            ((__val << (__bfws)) & DW_BIT_MASK_WIDTH(__bfws, bit_num)))
/*****/

/****d* include.bitops/DW_BIT_SET_NOREAD
 * DESCRIPTION
 *  Place the specified value into the specified bits of a word of data
 *  without reading first - for sensitive interrupt type registers
 * ARGUMENTS
 *  __datum     the word of data to be accessed
 *  __bfws      a width/shift pair       
 *  __val       the data value to be shifted into the specified bits
 * SOURCE
 */
#define DW_BIT_SET_NOREAD(__datum, __bfws, __val)                   \
    ((uint32_t) ((__datum) = (((__val) << (bfo ## __bfws)) &        \
                              DW_BIT_MASK(__bfws))))
/*****/

/****d* include.bitops/DW_BIT_BUILD
 * DESCRIPTION
 *  Shift the specified value into the desired bits.
 * ARGUMENTS
 *  __bfws      a width/shift pair       
 *  __val       the data value to be shifted into the specified bits
 * SOURCE
 */
#define DW_BIT_BUILD(__bfws, __val)                                 \
    ((uint32_t) (((__val) << (bfo ## __bfws)) & DW_BIT_MASK(__bfws)))
/*****/

#ifdef __cplusplus
}
#endif

#endif      // DW_COMMON_BITOPS_H


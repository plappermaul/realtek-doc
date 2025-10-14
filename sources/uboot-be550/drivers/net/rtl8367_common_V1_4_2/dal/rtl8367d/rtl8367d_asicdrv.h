#ifndef _RTL8367D_ASICDRV_H_
#define _RTL8367D_ASICDRV_H_

#include <rtk_types.h>
#include <rtk_error.h>
#include <rtl8367d_reg.h>

#define RTL8367D_REGBITLENGTH               16
#define RTL8367D_REGDATAMAX                 0xFFFF

#define RTL8367D_QOS_RATE_INPUT_MAX         (0x1FFFF * 8)
#define RTL8367D_QOS_RATE_INPUT_MAX_HSG     (0x7FFFF * 8)
#define RTL8367D_QOS_PPS_INPUT_MAX          (0x7FFFF)

#define RTL8367D_PORTNO                     11
#define RTL8367D_PORTIDMAX                  (RTL8367D_PORTNO-1)
#define RTL8367D_PMSKMAX                    ((1<<(RTL8367D_PORTNO))-1)
#define RTL8367D_PORTMASK                   0xFF

#define RTL8367D_PRIMAX                     7
#define RTL8367D_DSCPMAX                    63

#define RTL8367D_VIDMAX                     0xFFF
#define RTL8367D_FIDMAX                     3
#define RTL8367D_MSTIMAX                    3

#define RTL8367D_VLAN_4KTABLE_LEN           (2)
#define RTL8367D_VLAN_BUSY_CHECK_NO         (10)

#define RTL8367D_QUEUENO                    8
#define RTL8367D_QIDMAX                     (RTL8367D_QUEUENO-1)

#define RTL8367D_TB_OP_READ                 0
#define RTL8367D_TB_OP_WRITE                1

#define RTL8367D_TB_TARGET_ACLRULE          1
#define RTL8367D_TB_TARGET_ACLACT           2
#define RTL8367D_TB_TARGET_CVLAN            3
#define RTL8367D_TB_TARGET_L2               4
#define RTL8367D_TB_TARGET_IGMP_GROUP       5

#define RTL8367D_C2SIDXMAX                  31
#define RTL8367D_SP2CMAX                    63

/*=======================================================================
 *  Enum
 *========================================================================*/


#define RTL8367D_TABLE_ACCESS_REG_DATA(op, target)    ((op << 3) | target)

/*=======================================================================
 *  Structures
 *========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif
extern ret_t rtl8367d_setAsicRegBit(rtk_uint32 reg, rtk_uint32 bit, rtk_uint32 value);
extern ret_t rtl8367d_getAsicRegBit(rtk_uint32 reg, rtk_uint32 bit, rtk_uint32 *pValue);

extern ret_t rtl8367d_setAsicRegBits(rtk_uint32 reg, rtk_uint32 bits, rtk_uint32 value);
extern ret_t rtl8367d_getAsicRegBits(rtk_uint32 reg, rtk_uint32 bits, rtk_uint32 *pValue);

extern ret_t rtl8367d_setAsicReg(rtk_uint32 reg, rtk_uint32 value);
extern ret_t rtl8367d_getAsicReg(rtk_uint32 reg, rtk_uint32 *pValue);

#ifdef __cplusplus
}
#endif



#endif /*#ifndef _RTL8367D_ASICDRV_H_*/


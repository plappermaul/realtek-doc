/****************************************************************************
            Software License for Customer Use of Cortina Software
                          Grant Terms and Conditions

IMPORTANT NOTICE - READ CAREFULLY: This Software License for Customer Use
of Cortina Software ("LICENSE") is the agreement which governs use of
software of Cortina Access, Inc. and its subsidiaries ("CORTINA"),
including computer software (source code and object code) and associated
printed materials ("SOFTWARE").  The SOFTWARE is protected by copyright laws
and international copyright treaties, as well as other intellectual property
laws and treaties.  The SOFTWARE is not sold, and instead is only licensed
for use, strictly in accordance with this document.  Any hardware sold by
CORTINA is protected by various patents, and is sold but this LICENSE does
not cover that sale, since it may not necessarily be sold as a package with
the SOFTWARE.  This LICENSE sets forth the terms and conditions of the
SOFTWARE LICENSE only.  By downloading, installing, copying, or otherwise
using the SOFTWARE, you agree to be bound by the terms of this LICENSE.
If you do not agree to the terms of this LICENSE, then do not download the
SOFTWARE.

DEFINITIONS:  "DEVICE" means the Cortina Access?IROS SDK product.
"You" or "CUSTOMER" means the entity or individual that uses the SOFTWARE.
"SOFTWARE" means the Cortina Access?SDK software.

GRANT OF LICENSE:  Subject to the restrictions below, CORTINA hereby grants
CUSTOMER a non-exclusive, non-assignable, non-transferable, royalty-free,
perpetual copyright license to (1) install and use the SOFTWARE for
reference only with the DEVICE; and (2) copy the SOFTWARE for your internal
use only for use with the DEVICE.

RESTRICTIONS:  The SOFTWARE must be used solely in conjunction with the
DEVICE and solely with Your own products that incorporate the DEVICE.  You
may not distribute the SOFTWARE to any third party.  You may not modify
the SOFTWARE or make derivatives of the SOFTWARE without assigning any and
all rights in such modifications and derivatives to CORTINA.  You shall not
through incorporation, modification or distribution of the SOFTWARE cause
it to become subject to any open source licenses.  You may not
reverse-assemble, reverse-compile, or otherwise reverse-engineer any
SOFTWARE provided in binary or machine readable form.  You may not
distribute the SOFTWARE to your customers without written permission
from CORTINA.

OWNERSHIP OF SOFTWARE AND COPYRIGHTS. All title and copyrights in and the
SOFTWARE and any accompanying printed materials, and copies of the SOFTWARE,
are owned by CORTINA. The SOFTWARE protected by the copyright laws of the
United States and other countries, and international treaty provisions.
You may not remove any copyright notices from the SOFTWARE.  Except as
otherwise expressly provided, CORTINA grants no express or implied right
under CORTINA patents, copyrights, trademarks, or other intellectual
property rights.

DISCLAIMER OF WARRANTIES. THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY
EXPRESS OR IMPLIED WARRANTY OF ANY KIND, INCLUDING ANY IMPLIED WARRANTIES
OF MERCHANTABILITY, NONINFRINGEMENT, OR FITNESS FOR A PARTICULAR PURPOSE,
TITLE, AND NON-INFRINGEMENT.  CORTINA does not warrant or assume
responsibility for the accuracy or completeness of any information, text,
graphics, links or other items contained within the SOFTWARE.  Without
limiting the foregoing, you are solely responsible for determining and
verifying that the SOFTWARE that you obtain and install is the appropriate
version for your purpose.

LIMITATION OF LIABILITY. IN NO EVENT SHALL CORTINA OR ITS SUPPLIERS BE
LIABLE FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION, LOST
PROFITS, BUSINESS INTERRUPTION, OR LOST INFORMATION) OR ANY LOSS ARISING OUT
OF THE USE OF OR INABILITY TO USE OF OR INABILITY TO USE THE SOFTWARE, EVEN
IF CORTINA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
TERMINATION OF THIS LICENSE. This LICENSE will automatically terminate if
You fail to comply with any of the terms and conditions hereof. Upon
termination, You will immediately cease use of the SOFTWARE and destroy all
copies of the SOFTWARE or return all copies of the SOFTWARE in your control
to CORTINA.  IF you commence or participate in any legal proceeding against
CORTINA, then CORTINA may, in its sole discretion, suspend or terminate all
license grants and any other rights provided under this LICENSE during the
pendency of such legal proceedings.
APPLICABLE LAWS. Claims arising under this LICENSE shall be governed by the
laws of the State of California, excluding its principles of conflict of
laws.  The United Nations Convention on Contracts for the International Sale
of Goods is specifically disclaimed.  You shall not export the SOFTWARE
without first obtaining any required export license or other approval from
the applicable governmental entity, if required.  This is the entire
agreement and understanding between You and CORTINA relating to this subject
matter.
GOVERNMENT RESTRICTED RIGHTS. The SOFTWARE is provided with "RESTRICTED
RIGHTS." Use, duplication, or disclosure by the Government is subject to
restrictions as set forth in FAR52.227-14 and DFAR252.227-7013 et seq. or
its successor. Use of the SOFTWARE by the Government constitutes
acknowledgment of CORTINA's proprietary rights therein. Contractor or
Manufacturer is CORTINA.

Copyright (c) 2024 by Cortina Access Incorporated
****************************************************************************/
#ifndef _UTIL_MISC_H_
#define _UTIL_MISC_H_
#include "ca_types.h"

#define CA_INVALID_RING_BUF     0

#if 0//yocto
#define  CA_ASSERT_RET(x, ret)    \
    do{ \
        if (!(x))  { \
            OS_PRINTF("%s() _%d_: assert fail\n", __FUNCTION__, __LINE__);\
            return ret; \
        } \
    }while(0)     
#else//sd1 uboot for 98f
#define  CA_ASSERT_RET(x, ret)    \
    do{ \
        if (!(x))  { \
            printf("%s() _%d_: assert fail\n", __FUNCTION__, __LINE__);\
            return ret; \
        } \
    }while(0)
#endif
        
#define CA_DEV_ID_CHECK(device_id)     if((device_id) > 0) { \
													ca_printf("%s:[%d] device_id is %d!!\n", __FUNCTION__, __LINE__,device_id); \
	                                                return CA_E_PARAM; \
                                                }
        
#define CA_BOOL_VAL_CHECK(val)         if(((val) != FALSE) && ((val) != TRUE)) { \
	  												ca_printf("%s:[%d] val is %d!!\n", __FUNCTION__, __LINE__,val); \
                                                    return CA_E_PARAM; \
                                                }

#define CA_RET_VALUE_CHECK(ret)         do{ \
                                            if(CA_E_OK != ret) { \
                                                ca_printf("%s:[%d] return %d!!\n", __FUNCTION__, __LINE__, ret); \
                                                return ret; \
                                            } \
                                        } while(0)
#define CA_NULL_PTR_CHECK(ptr)             do { \
                                                if((ptr) == NULL) { \
													ca_printf("%s:[%d] ptr is %d!!\n", __FUNCTION__, __LINE__,ptr); \
                                                    return CA_E_PARAM; \
                                                } \
                                            } while(0)     
#define CA_CFI_DEI_CHECK(cfi)          if((cfi) > 1) { \
											ca_printf("%s:[%d] cfi is %d!!\n", __FUNCTION__, __LINE__,cfi); \
                                            return CA_E_PARAM; \
                                        }

#define CA_COS_CHECK(cos)              if((cos) > 7) { \
											ca_printf("%s:[%d] cos is %d!!\n", __FUNCTION__, __LINE__,cos); \
                                            return CA_E_PARAM; \
                                        }

#define CA_DOT_1P_CHECK(dot1p)         if((dot1p) > 7) { \
											ca_printf("%s:[%d] dot1p is %d!!\n", __FUNCTION__, __LINE__,dot1p); \
                                            return CA_E_PARAM; \
                                        }

#define CA_DSCP_CHECK(dscp)             if((dscp) > 63) { \
											ca_printf("%s:[%d] dscp is %d!!\n", __FUNCTION__, __LINE__,dscp); \
                                            return CA_E_PARAM; \
                                        }

#define CA_ETHER_TYPE_CHECK(eth_type)  if((eth_type) > 0xFFFF) { \
											ca_printf("%s:[%d] eth_type is 0x%x!!\n", __FUNCTION__, __LINE__,eth_type); \
                                            return CA_E_PARAM; \
                                        }


#define CA_TPID_CHECK(tpid)            if((tpid) > 0xFFFF) { \
											ca_printf("%s:[%d] tpid is 0x%x!!\n", __FUNCTION__, __LINE__,tpid); \
                                            return CA_E_PARAM; \
                                        }

#define CA_TRAFFIC_CLASS_CHECK(tc)     if((tc) > 0xFF) { \
											ca_printf("%s:[%d] tc is 0x%x!!\n", __FUNCTION__, __LINE__,tc); \
                                            return CA_E_PARAM; \
                                        }

#define CA_VLAN_ID_CHECK(vid)          if((vid) > 0xFFF) { \
											ca_printf("%s:[%d] vid is 0x%x!!\n", __FUNCTION__, __LINE__,vid); \
                                            return CA_E_PARAM; \
                                        }

extern ca_int32_t   ca_atoi(ca_int8_t *str);
extern ca_int32_t   ca_str2dec(ca_int8_t *str , ca_uint32_t *num);
extern ca_uint32_t  ca_strtol(ca_int8_t *str);
extern ca_int32_t   ca_str2decl(ca_int8_t *str, ca_uint64_t *num);
extern ca_uint64_t  ca_strtoll(ca_int8_t *str);
extern ca_int32_t   ca_str2mac(ca_int8_t *mac_str , ca_int8_t *mac_addr);
extern ca_int32_t   ca_str2ip(ca_int8_t *ip_str , ca_uint32_t *ipaddr);
extern ca_int32_t   ca_ip2str(ca_int8_t *ip_str , ca_uint32_t ipaddr);
extern ca_int32_t   ca_str2hex(char *oui_str , char *oui_value , int maxbytes);
extern ca_uint32_t  ca_strtoul(const char *nptr, char **endptr, int base);
extern ca_int32_t   ca_str2ipv6(ca_int8 *ip_str, ca_uint8 * ipv6_addr);
extern ca_int32_t   ca_ipv6tostr(ca_int8 *ip_str,ca_uint8 * ipv6_addr);

extern ca_uint_t    ca_rbuf_create(ca_int32_t rbuf_num);
extern ca_int32_t   ca_rbuf_read(ca_uint_t rbuf_id, void **buf);
extern ca_int32_t   ca_rbuf_write(ca_uint_t rbuf_id, void *buf);
extern void  ca_buffer_dump( char *tp, ca_uint8_t *dp, ca_uint16_t len);

#endif


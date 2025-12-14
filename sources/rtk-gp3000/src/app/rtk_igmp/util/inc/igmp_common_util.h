/*
 * Copyright (C) 2010 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 16086 $
 * $Date: 2011-03-04 09:41:35 +0800 (Fri, 04 Mar 2011) $
 *
 * Purpose :
 *
 * Feature :
 *
 */

#ifndef __IGMP_COMMON_UTIL_H__
#define __IGMP_COMMON_UTIL_H__

/*
 * Include Files
 */
#include <sys_def.h>

extern uint32 *p_vlan_ref_cnt;

typedef unsigned char mac_address_t[MAC_ADDR_LEN];

#define INVALID_PORT                    0xFFFFFFFF

#define MIN_VLAN_ID		                1

#define MAX_VLAN_ID		                4094

#define MIN_VLAN_OFFSET(mode)           ((MODE_GPON == mode) ? -1 : 0)

#define MAX_VLAN_OFFSET(mode)           ((MODE_GPON == mode) ? 1 : 0)

#define VALID_VLAN_ID(x, l, h)          ((((x) >= (MIN_VLAN_ID + (l))) && ((x) <= (MAX_VLAN_ID + (h))))? 1 : 0)

#define IS_MULTICAST(x)                 (x[0]&0x1)

#define IS_VID_INVALID(x, l, h)         (0 == VALID_VLAN_ID(x,l,h))

#define IS_LOGIC_PORT_INVALID(port)     ((port > (HAL_GET_PON_PORT() + 1))||(port < LOGIC_PORT_START))

#define IS_ENABLE_INVALID(enable)       ((enable != DISABLED)&&(enable != ENABLED))

#define IS_ZERO_RANGE(arr, start, end, pRet) \
        do { \
            uint32 _i;  \
            *pRet = TRUE; \
            for (_i = start; _i < end; _i ++) \
            { \
                if (0 != arr[_i]) *pRet = FALSE; \
            } \
        } while(0)

#define LgcPortFor(lport) for((lport) = 1; (lport) <= HAL_GET_PON_PORT(); (lport)++)

#define BITMAP_REMOVE(dstArray, srcArray, length) \
        do { \
            uint32  array_index;\
            for (array_index = 0; array_index < length; array_index++) \
            { \
                dstArray[array_index] &= ~srcArray[array_index]; \
            } \
        } while(0)

#define RTK_PORTMASK_REMOVE(dstPortmask, srcPortmask) \
        BITMAP_REMOVE((dstPortmask).bits, (srcPortmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)

#define IS_VALID_PORT_ID(pid)           (((pid) >= LOGIC_PORT_START && (pid) < igmpCtrl.sys_max_port_num - 1) ? TRUE : FALSE)

#define PortLogic2PhyID(port)           ((((!HAL_IS_PORT_EXIST(port - 1))) ? INVALID_PORT : (port - 1)))

#define BYTE_SET_BIT(_bit, _byte) do{                               \
            (_byte) |= (uint8)((uint8)1 << (uint8)(_bit));          \
        }while(0)

#define VID_LIST_SETBIT(_vid, _pucBuf) do{ \
            BYTE_SET_BIT((uint32)(_vid) % 8, ((uint8 *)(_pucBuf))[(_vid) / 8]); \
        }while(0)

#define TEST_BIT_8(_bit, _byte)         (((uint8)(_byte) & ((uint8)1 << (uint8)(_bit))) != 0)

#define TEST_VID_LIST(_vid, _pucBuf)    (TEST_BIT_8((_vid) % 8, ((uint8 *)(_pucBuf))[(_vid) / 8]))

#define ForEachListVlan(_vid, _vidListBuf)                     \
        for((_vid=1); (_vid)<=4094; (_vid)++)                  \
            if(TEST_VID_LIST((_vid), (_vidListBuf)))

#define VLANMASK_IS_VIDSET(vlanlist,vid) TEST_VID_LIST(vid,vlanlist)

#define VLANMASK_SET_VID(vlanlist,vid)   VID_LIST_SETBIT(vid,vlanlist)

#define VLANMASK_CLEAR_ALL(vlanlist)     memset(vlanlist,0,sizeof(vlanlist))

#define VLANMASK_SET_ALL(vlanMsk)        memset(vlanMsk,0xff,sizeof(vlanMsk))

#define SDK_MIB_BUFFER_LEN (256)


/*
 * Function Declaration
 */

/* Function Name:
 *      igmp_util_mac_addr_get
 * Description:
 *      Get mac address by layer 2 device name
 * Input:
 *      name - layer 2 device name
 * Output:
 *      pMac - mac address
 * Return:
 *      SYS_ERR_OK
 *      SYS_ERR_FAILED
 *      SYS_ERR_NULL_POINTER
 *
 * Note:
 *      None
 */
extern int32 igmp_util_mac_addr_get(char * name, sys_mac_t *pMac);

/* Function Name:
 *      igmp_util_ipAddr_get
 * Description:
 *      Get ip address and netmask by layer 2 device name
 * Input:
 *      devName - layer 2 device name
 * Output:
 *      pMac - mac address
 * Return:
 *      SYS_ERR_OK
 *      SYS_ERR_NULL_POINTER
 *
 * Note:
 *      None
 */
extern int32 igmp_util_ipAddr_get(char *devName, sys_ipv4_addr_t *pIp, sys_ipv4_addr_t *pNetmask);

/* Function Name:
 *      igmp_util_checksum_get
 * Description:
 *      Compute checksum for a piece of data.
 * Input:
 *      addr - start poiter of data
 *      len  - length of data
 * Output:
  *     pChksum - pointer of return check sum
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
extern uint16 igmp_util_checksum_get(uint16 *addr, int32 len, uint16* pChksum);


extern uint32 igmp_util_get_sdk_mib_value_by_name(char *pName, char *pVal);

#endif /* __IGMP_COMMON_UTIL_H__ */


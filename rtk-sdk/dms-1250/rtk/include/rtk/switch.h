/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of Switch Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Switch parameter settings
 *           (2) Management address and vlan configuration.
 *
 */

#ifndef __RTK_SWITCH_H__
#define __RTK_SWITCH_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <hal/chipdef/chipdef.h>
#include <hal/phy/phydef.h>

/*
 * Symbol Definition
 */
typedef enum rtk_switch_flexTblFmt_e
{
    RTK_SWITCH_FLEX_TBL_FMT_L2_TUNNEL = 0,   /* 0x0: TBL0 L2 tunnel,         TBL1 L2 tunnel list */
    RTK_SWITCH_FLEX_TBL_FMT_MPLS,            /* 0x1: TBL0 MPLS decap,        TBL1 MPLS Next Hop */
    RTK_SWITCH_FLEX_TBL_FMT_IP_MAC_BINDING,  /* 0x2: TBL0 IP-MAC binding,    TBL1 unused */
    RTK_SWITCH_FLEX_TBL_FMT_PE_ECID,         /* 0x3: TBL0 PE ECID-to-VLAN,   TBL1 PE ECID List */
    RTK_SWITCH_FLEX_TBL_FMT_END
} rtk_switch_flexTblFmt_t;

typedef enum rtk_switch_unifiedTblType_e
{
    RTK_SWITCH_UNIFIED_TBL_TYPE_IPUCAST = 0,    /* Unicast Routing (LPM) table */
    RTK_SWITCH_UNIFIED_TBL_TYPE_IPMCAST,        /* Multicast table (bridging & routing) table */
    RTK_SWITCH_UNIFIED_TBL_TYPE_OPENFLOW,       /* Openflow (L3 flowtable) table */
    RTK_SWITCH_UNIFIED_TBL_TYPE_END
} rtk_switch_unifiedTblType_t;


/*
 * Data Declaration
 */
/* information of device */
typedef struct rtk_switch_devInfo_s
{
    uint32  chipId;
    uint32  revision;
    uint32  familyId;
    uint32  port_number;
    rt_portType_info_t fe;
    rt_portType_info_t ge;
    rt_portType_info_t xge;
    rt_portType_info_t copper;
    rt_portType_info_t serdes;
    rt_portType_info_t combo;
    rt_portType_info_t ether;
    rt_portType_info_t fiber;
    rt_portType_info_t uplink;
    rt_portType_info_t stack;
    rt_portType_info_t all;
    phy_type_t phyType[RTK_MAX_PORT_PER_UNIT];
    rt_register_capacity_t  capacityInfo;
    uint32 macPpInfo_blockNum;
    rt_macPpInfo_t macPpInfo;
    int32   cpuPort; /* use (-1) for HWP_VALUE_NO_INIT */
} rtk_switch_devInfo_t;

typedef struct switch_reg_value_s
{
    uint32  reg;
    uint32  val;
} switch_reg_value_t;


/* type of the acceptable direction */
typedef enum rtk_switch_pktDir_e
{
    PKTDIR_RX = 0,
    PKTDIR_TX,
    PKTDIR_END
} rtk_switch_pktDir_t;

/* type of the acceptable packet length in different link speed */
typedef enum rtk_switch_maxPktLen_linkSpeed_e
{
    MAXPKTLEN_LINK_SPEED_FE = 0,
    MAXPKTLEN_LINK_SPEED_GE,
    MAXPKTLEN_LINK_SPEED_END,
} rtk_switch_maxPktLen_linkSpeed_t;

/* type of checksum fail */
typedef enum rtk_switch_chksum_fail_e
{
    LAYER2_CHKSUM_FAIL = 0,
    CHKSUM_FAIL_END
} rtk_switch_chksum_fail_t;

typedef enum rtk_switch_pkt2CpuType_e
{
    PKT2CPU_TYPE_FORWARD,
    PKT2CPU_TYPE_TRAP,
    PKT2CPU_TYPE_COPY,      /*reserved for future use*/
    PKT2CPU_TYPE_VLAN,
    PKT2CPU_ETAG_TYPE_FORWARD,
    PKT2CPU_ETAG_TYPE_TRAP,
    PKT2CPU_TYPE_END,
} rtk_switch_pkt2CpuType_t;

typedef struct rtk_switch_unifiedTblBlkInfo_s
{
    rtk_switch_unifiedTblType_t type;
    uint32                      size;
} rtk_switch_unifiedTblBlkInfo_t;


/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      rtk_switch_init
 * Description:
 *      Initialize switch module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize switch module before calling any switch APIs.
 * Changes:
 *      None
 */
extern int32
rtk_switch_init(uint32 unit);


/* Function Name:
 *      rtk_switch_deviceCapability_print
 * Description:
 *      Print device information of the specific unit for parsing the variable to testing script.
 * Input:
 *      unit     - unit id
 * Output:
 *      pDevInfo - pointer to the device information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The routine have provided following information based on SDK hw_profile
 *      - chip id, chip revision id, chip capacity
 *      - total port number and cpu port id
 *      - detail information in each port type
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_switch_deviceCapability_print(uint32 unit);

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      rtk_switch_deviceInfo_get
 * Description:
 *      Get device information of the specific unit
 * Input:
 *      unit     - unit id
 * Output:
 *      pDevInfo - pointer to the device information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The routine have provided following information based on SDK probe result
 *      - chip id, chip revision id, chip capacity
 *      - total port number and cpu port id
 *      - detail information in each port type
 * Changes:
 *      None
 */

extern int32
rtk_switch_deviceInfo_get(uint32 unit, rtk_switch_devInfo_t *pDevInfo);

/* Function Name:
 *      rtk_switch_cpuMaxPktLen_get
 * Description:
 *      Get the max packet length setting on CPU port of the specific unit
 * Input:
 *      unit - unit id
 *      dir  - direction of packet
 * Output:
 *      pLen - pointer to the max packet length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid packet direction
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Supported Maximum packet lenght is 16383B.
 *      (2) Supported management frame is as following:
 *          - rtk_switch_pktDir_t \ Chip:   8380   8390   9300  9310
 *          - PKTDIR_RX                     O       O      O     O
 *          - PKTDIR_TX                     O       O      O     O
 * Changes:
 *      None
 */
extern int32
rtk_switch_cpuMaxPktLen_get(uint32 unit, rtk_switch_pktDir_t dir, uint32 *pLen);

/* Function Name:
 *      rtk_switch_cpuMaxPktLen_set
 * Description:
 *      Set the max packet length setting on CPU port of the specific unit
 * Input:
 *      unit - unit id
 *      dir  - direction of packet
 *      len  - max packet length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid packet direction
 *      RT_ERR_OUT_OF_RANGE - invalid packet length
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Supported Maximum packet lenght is 16383B.
 *      (2) Supported management frame is as following:
 *          - rtk_switch_pktDir_t \ Chip:   8380   8390   9300  9310
 *          - PKTDIR_RX                     O       O      O     O
 *          - PKTDIR_TX                     O       O      O     O
 * Changes:
 *      None
 */
extern int32
rtk_switch_cpuMaxPktLen_set(uint32 unit, rtk_switch_pktDir_t dir, uint32 len);

/* Function Name:
 *      rtk_switch_maxPktLenLinkSpeed_get
 * Description:
 *      Get the max packet length setting of the specific speed type
 * Input:
 *      unit  - unit id
 *      speed - speed type
 * Output:
 *      pLen  - pointer to the max packet length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid enum speed type
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Maximum packet length filtering is examined on both receiving and transmitting ports
 *          according to port's link speed.
 *      (2) Supported Maximum packet lenght is 16383B.
 *      (3) Max packet length setting speed type
 *          - MAXPKTLEN_LINK_SPEED_FE
 *          - MAXPKTLEN_LINK_SPEED_GE
 *      (4) Change to use rtk_switch_portMaxPktLenLinkSpeed_get(unit, port, speed, *pLen) for 9300,9310
 * Changes:
 *      None
 */
extern int32
rtk_switch_maxPktLenLinkSpeed_get(uint32 unit, rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen);

/* Function Name:
 *      rtk_switch_maxPktLenLinkSpeed_set
 * Description:
 *      Set the max packet length of the specific speed type
 * Input:
 *      unit  - unit id
 *      speed - speed type
 *      len   - max packet length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid enum speed type
 *      RT_ERR_OUT_OF_RANGE - invalid packet length
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Maximum packet length filtering is examined on both receiving and transmitting ports
 *          according to port's link speed.
 *      (2) Supported Maximum packet lenght is 16383B.
 *      (3) Max packet length setting speed type
 *          - MAXPKTLEN_LINK_SPEED_FE
 *          - MAXPKTLEN_LINK_SPEED_GE
 *      (4) Change to use rtk_switch_portMaxPktLenLinkSpeed_set(unit, port, speed, len) for 9300,9310
 * Changes:
 *      None
 */
extern int32
rtk_switch_maxPktLenLinkSpeed_set(uint32 unit, rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len);

/* Function Name:
 *      rtk_switch_maxPktLenTagLenCntIncEnable_get
 * Description:
 *      Get include or exclude tag length state of max packet length in the specified device.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer buffer of including/excluding tag length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Inner(4B) and outer(4B) tag length can be included or excluded to the maximum packet length filtering.
 *      (2) Change to use rtk_switch_portMaxPktLenTagLenCntIncEnable_get(unit, port, pEnable) for 9300,9310
 * Changes:
 *      None
 */
extern int32
rtk_switch_maxPktLenTagLenCntIncEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_switch_maxPktLenTagLenCntIncEnable_set
 * Description:
 *      Set the max packet length to include or exclude tag length in the specified device.
 * Input:
 *      unit   - unit id
 *      enable - include/exclude Tag length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Inner(4B) and outer(4B) tag length can be included or excluded to the maximum packet length filtering.
 *      (2) Change to use rtk_switch_portMaxPktLenTagLenCntIncEnable_set(unit, port, enable) for 9300,9310
 * Changes:
 *      None
 */
extern int32
rtk_switch_maxPktLenTagLenCntIncEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      rtk_switch_snapMode_get
 * Description:
 *      Get the status of SNAP qualification mode.
 * Input:
 *      unit      - unit id
 * Output:
 *      pSnapMode - pointer to SNAP mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      OUI(00-00-00) comparsion can be ommited for SNAP qualification.
 *      SNAP mode is as following
 *      - SNAP_MODE_AAAA03000000
 *      - SNAP_MODE_AAAA03
 * Changes:
 *      None
 */
extern int32
rtk_switch_snapMode_get(uint32 unit, rtk_snapMode_t *pSnapMode);

/* Function Name:
 *      rtk_switch_snapMode_set
 * Description:
 *      Set the status of SNAP qualification mode.
 * Input:
 *      unit     - unit id
 *      snapMode - SNAP mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      OUI(00-00-00) comparsion can be ommited for SNAP qualification.
 *      SNAP mode is as following
 *      - SNAP_MODE_AAAA03000000
 *      - SNAP_MODE_AAAA03
 * Changes:
 *      None
 */
extern int32
rtk_switch_snapMode_set(uint32 unit, rtk_snapMode_t snapMode);

/* Function Name:
 *      rtk_switch_chksumFailAction_get
 * Description:
 *      Get forwarding action of checksum error on specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      failType - checksum fail type
 * Output:
 *      pAction  - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300
 * Note:
 *      (1) Checksum fail type is as following \ Chip :     8390 9300
 *          - LAYER2_CHKSUM_FAIL                              O    O
 *      (2) Forwarding action is as following \ Chip :      8390 9300
 *          - ACTION_FORWARD                                  O    O
 *          - ACTION_DROP                                     O    O
 * Changes:
 *      None
 */
extern int32
rtk_switch_chksumFailAction_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_switch_chksum_fail_t    failType,
    rtk_action_t                *pAction);

/* Function Name:
 *      rtk_switch_chksumFailAction_set
 * Description:
 *      Set forwarding action of checksum error on specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      failType - checksum fail type
 *      action   - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid error forwarding action
 *      RT_ERR_INPUT      - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300
 * Note:
 *      (1) Checksum fail type is as following \ Chip :     8390 9300
 *          - LAYER2_CHKSUM_FAIL                              O    O
 *      (2) Forwarding action is as following \ Chip :      8390 9300
 *          - ACTION_FORWARD                                  O    O
 *          - ACTION_DROP                                     O    O
 * Changes:
 *      None
 */
extern int32
rtk_switch_chksumFailAction_set(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_switch_chksum_fail_t    failType,
    rtk_action_t                action);

/* Function Name:
 *      rtk_switch_recalcCRCEnable_get
 * Description:
 *      Get enable status of recaculate CRC on specified egress port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of recaculate CRC
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      When enable, mirrored packet with bad CRC will be recaculate at mirroring port.
 *      When enable, only when packet is modified CRC will be recalculated.
 * Changes:
 *      None
 */
extern int32
rtk_switch_recalcCRCEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_switch_recalcCRCEnable_set
 * Description:
 *      Set enable status of recaculate CRC on specified egress port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of recaculate CRC
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      When enable, mirrored packet with bad CRC will be recaculate at mirroring port.
 *      When enable, only when packet is modified CRC will be recalculated.
 * Changes:
 *      None
 */
extern int32
rtk_switch_recalcCRCEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Module Name    : Switch     */
/* Sub-module Name: Management address and vlan configuration */

/* Function Name:
 *      rtk_switch_mgmtMacAddr_get
 * Description:
 *      Get MAC address of switch.
 * Input:
 *      unit - unit id
 * Output:
 *      pMac - pointer to MAC address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The MAC address is used when sending Pause frame.
 * Changes:
 *      None
 */
extern int32
rtk_switch_mgmtMacAddr_get(uint32 unit, rtk_mac_t *pMac);

/* Function Name:
 *      rtk_switch_mgmtMacAddr_set
 * Description:
 *      Set MAC address of switch.
 * Input:
 *      unit - unit id
 *      pMac - MAC address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_MAC          - invalid mac address
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The MAC address is used when sending Pause frame.
 * Changes:
 *      None
 */
extern int32
rtk_switch_mgmtMacAddr_set(uint32 unit, rtk_mac_t *pMac);

/* Function Name:
 *      rtk_switch_IPv4Addr_get
 * Description:
 *      Get IPv4 address of switch.
 * Input:
 *      unit    - unit id
 * Output:
 *      pIpAddr - pointer to IPv4 address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_switch_IPv4Addr_get(uint32 unit, uint32 *pIpAddr);

/* Function Name:
 *      rtk_switch_IPv4Addr_set
 * Description:
 *      Set IPv4 address of switch.
 * Input:
 *      unit   - unit id
 *      ipAddr - IPv4 address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_IPV4_ADDRESS - invalid IPv4 address
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_switch_IPv4Addr_set(uint32 unit, uint32 ipAddr);

/* Function Name:
 *      rtk_switch_pkt2CpuTypeFormat_get
 * Description:
 *      Get the configuration about content state for packet to CPU
 * Input:
 *      unit    - unit id
 *      type    - method of packet to CPU
 * Output:
 *      pFormat - type of format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The type of packet to CPU:
 *      - PKT2CPU_TYPE_FORWARD
 *      - PKT2CPU_TYPE_TRAP
 *
 *      The type of format:
 *      - TRAP_PKT_ORIGINAL
 *      - TRAP_PKT_MODIFIED
 * Changes:
 *      None
 */
extern int32
rtk_switch_pkt2CpuTypeFormat_get(
    uint32                      unit,
    rtk_switch_pkt2CpuType_t    type,
    rtk_pktFormat_t             *pFormat);

/* Function Name:
 *      rtk_switch_pkt2CpuTypeFormat_set
 * Description:
 *      Set the configuration about content state for packet to CPU
 * Input:
 *      unit   - unit id
 *      type   - method of packet to CPU
 *      format - packet format to CPU
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The type of format:
 *      - TRAP_PKT_ORIGINAL
 *      - TRAP_PKT_MODIFIED
 * Changes:
 *      None
 */
extern int32
rtk_switch_pkt2CpuTypeFormat_set(
    uint32                      unit,
    rtk_switch_pkt2CpuType_t    type,
    rtk_pktFormat_t             format);

/* Function Name:
 *      rtk_switch_pppoeIpParseEnable_get
 * Description:
 *      Get enable status of PPPoE IP-parse functionality.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status of PPPoE parse functionality
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Enable the device to recognize PPPoE frame.
 *      The PPPoE frame is treated as unknown L2 frame if PPPoE IP-parse is disabled.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_switch_pppoeIpParseEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_switch_pppoeIpParseEnable_set
 * Description:
 *      Set enable status of PPPoE IP-parse functionality.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Enable the device to recognize PPPoE frame.
 *      The PPPoE frame is treated as unknown L2 frame if PPPoE IP-parse is disabled.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_switch_pppoeIpParseEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      rtk_switch_cpuPktTruncateEnable_get
 * Description:
 *      Get CPU port truncation function state.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - enable state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Packet is truncated to specified length if it is trap/forward to CPU and packet length is
 *          over than the truncation length.
 *      (2) The truncation length is specified by rtk_switch_cpuPktTruncateLen_set.
 * Changes:
 *      None
 */
extern int32
rtk_switch_cpuPktTruncateEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_switch_cpuPktTruncateEnable_set
 * Description:
 *      Set CPU port truncation function state.
 * Input:
 *      unit    - unit id
 *      enable  - enable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Packet is truncated to specified length if it is trap/forward to CPU and packet length is
 *          over than the truncation length.
 *      (2) The truncation length is specified by rtk_switch_cpuPktTruncateLen_set.
 * Changes:
 *      None
 */
extern int32
rtk_switch_cpuPktTruncateEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      rtk_switch_cpuPktTruncateLen_get
 * Description:
 *      Get the packet length for CPU port truncation function.
 * Input:
 *      unit - unit id
 * Output:
 *      pLen  - packet length to truncate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Packet is truncated to specified length if it is trap/forward to CPU and packet length is
 *          over than the truncation length.
 *      (2) The truncation function takes effect if rtk_switch_cpuPktTruncateEnable_set is enabled.
 * Changes:
 *      None
 */
extern int32
rtk_switch_cpuPktTruncateLen_get(uint32 unit, uint32 *pLen);

/* Function Name:
 *      rtk_switch_cpuPktTruncateLen_set
 * Description:
 *      Set the packet length for CPU port truncation function.
 * Input:
 *      unit - unit id
 *      len  - packet length to truncate
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - invalid truncation length
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Packet is truncated to specified length if it is trap/forward to CPU and packet length is
 *          over than the truncation length.
 *      (2) The truncation function takes effect if rtk_switch_cpuPktTruncateEnable_set is enabled.
 * Changes:
 *      None
 */
extern int32
rtk_switch_cpuPktTruncateLen_set(uint32 unit, uint32 len);

/* Function Name:
 *      rtk_switch_portMaxPktLenLinkSpeed_get
 * Description:
 *      Get the max packet length setting of the specific speed type and port
 * Input:
 *      unit  - unit id
 *      port  - the specific port
 *      speed - speed type
 * Output:
 *      pLen  - pointer to the max packet length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid enum speed type
 * Applicable:
 *      9300, 9310
 * Note:
 *      Max packet length setting speed type
 *      - MAXPKTLEN_LINK_SPEED_FE
 *      - MAXPKTLEN_LINK_SPEED_GE
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_switch_portMaxPktLenLinkSpeed_get(uint32 unit, rtk_port_t port,
    rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen);

/* Function Name:
 *      rtk_switch_portMaxPktLenLinkSpeed_set
 * Description:
 *      Set the max packet length of the specific speed type and port
 * Input:
 *      unit  - unit id
 *      port  - the specific port
 *      speed - speed type
 *      len   - max packet length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - invalid enum speed type
 *      RT_ERR_OUT_OF_RANGE - invalid packet length
 * Applicable:
 *      9300, 9310
 * Note:
 *      Max packet length setting speed type
 *      - MAXPKTLEN_LINK_SPEED_FE
 *      - MAXPKTLEN_LINK_SPEED_GE
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_switch_portMaxPktLenLinkSpeed_set(uint32 unit, rtk_port_t port,
    rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len);

/* Function Name:
 *      rtk_switch_portMaxPktLenTagLenCntIncEnable_get
 * Description:
 *      Get include or exclude tag length state of max packet length
 *      in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - the specific port
 * Output:
 *      pEnable     - pointer buffer of including/excluding tag length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_switch_portMaxPktLenTagLenCntIncEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_switch_portMaxPktLenTagLenCntIncEnable_set
 * Description:
 *      Set the max packet length to include or exclude tag length
 *      in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - the specific port
 *      enable  - include/exclude Tag length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_switch_portMaxPktLenTagLenCntIncEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

/* Function Name:
 *      rtk_switch_flexTblFmt_get
 * Description:
 *      Get the flexible table format
 * Input:
 *      unit - unit id
 * Output:
 *      pFmt - pointer to the table format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_switch_flexTblFmt_get(uint32 unit, rtk_switch_flexTblFmt_t *pFmt);

/* Function Name:
 *      rtk_switch_flexTblFmt_set
 * Description:
 *      Set the flexible table format
 * Input:
 *      unit - unit id
 * Output:
 *      fmt  - table format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_switch_flexTblFmt_set(uint32 unit, rtk_switch_flexTblFmt_t fmt);

#endif /* __RTK_SWITCH_H__ */


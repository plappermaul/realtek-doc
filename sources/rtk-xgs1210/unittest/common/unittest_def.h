/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 73269 $
 * $Date: 2016-11-11 17:26:21 +0800 (Fri, 11 Nov 2016) $
 *
 * Purpose : Definition of DAL Macro
 *
 * Feature : DAL test APIs
 *
 */

#ifndef __UNIT_TEST_DEF_H__
#define __UNIT_TEST_DEF_H__

/* Unit ID */
#define TEST_MIN_UNIT_ID                0
#define TEST_MAX_UNIT_ID                RTK_MAX_UNIT_ID
/* Test Port*/
#define TEST_PORT_ID_MIN(unit)          (HWP_UNITTEST_PORT_MACID_MIN(unit))
#define TEST_PORT_ID_MAX(unit)          (HWP_UNITTEST_PORT_MACID_MAX(unit))
#define TEST_ETHER_PORT_ID_MIN(unit)    (HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit))
#define TEST_ETHER_PORT_ID_MAX(unit)    (HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit))
#define TEST_PORT_ID_MAX_WO_CPU(unit)   (TEST_ETHER_PORT_ID_MAX(unit))


/* VLAN */
#define TEST_VLAN_ID_MIN                RTK_VLAN_ID_MIN
#define TEST_VLAN_ID_MAX                RTK_VLAN_ID_MAX
/* Enable Status */
#define TEST_ENABLE_STATUS_MIN          DISABLED
#define TEST_ENABLE_STATUS_MAX          ENABLED
/*Priority*/
#define TEST_PRI_ID_MIN                 0
#define TEST_PRI_ID_MAX                 RTK_DOT1P_PRIORITY_MAX

#endif  /*__UNIT_TEST_DEF_H__*/


/*
 * Copyright (C) 2018 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : Definition of Switch Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Switch parameter settings
 *           (2) Switch interface/version info retrieve
 *
 */
#ifndef __RT_SWITCH_H__
#define __RT_SWITCH_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <hal/chipdef/chip.h>
#include <rtk/switch.h>

/*
 * Symbol Definition
 */
/* information of device */
typedef struct rt_switch_devInfo_s
{
    uint32  chipId;                 /* Chip ID  */
    uint32  revision;               /* Chip revisio */
    uint32  port_number;            /* Number of physical ports */
    rt_portType_info_t cpu;         /* CPU port info */
    rt_portType_info_t fe;          /* FE port info */
    rt_portType_info_t ge;          /* GE port info */
    rt_portType_info_t ge_combo;    /* GE combo port info */
    rt_portType_info_t serdes;      /* Serdes port info */
    rt_portType_info_t ether;       /* Ethernet port info */
    rt_portType_info_t dsl;         /* DSL port info */
    rt_portType_info_t ext;         /* Extension port info */
    rt_portType_info_t all;         /* All port info */
    rt_portType_info_t swpbo_lb;    /* Deprecated */
    rt_register_capacity_t  capacityInfo; /* Various of functions' HW limit */
    int32   cpuPort;                /* CPU port number, use (-1) for VALUE_NO_INIT */
    int32   rgmiiPort;              /* RGMII port number, use (-1) for VALUE_NO_INIT */
    int32   ponPort;                /* PON port number, use (-1) for VALUE_NO_INIT */
    int32   swPboLbPort;            /* Deprecated */
} rt_switch_devInfo_t;

typedef enum rt_switch_port_name_e
{
    /*normal UTP port*/
    LOG_PORT_UTP0 = 0,
    LOG_PORT_UTP1,
    LOG_PORT_UTP2,
    LOG_PORT_UTP3,
    LOG_PORT_UTP4,
    LOG_PORT_UTP5,
    LOG_PORT_UTP6,
    LOG_PORT_UTP7,
    LOG_PORT_UTP8,
    LOG_PORT_UTP9,
    LOG_PORT_UTP10,
    LOG_PORT_UTP11 = 63,

    /*PON port*/
    LOG_PORT_PON = 128,

    /* HiSGMII port */
    LOG_PORT_HSG0 = 1024,
    LOG_PORT_HSG1 = 1025,

    /*CPU port*/
    LOG_PORT_CPU7 = 32544,
    LOG_PORT_CPU6 = 32576,
    LOG_PORT_CPU5 = 32608,
    LOG_PORT_CPU4 = 32640,
    LOG_PORT_CPU3 = 32672,
    LOG_PORT_CPU2 = 32704,
    LOG_PORT_CPU1 = 32736,
    LOG_PORT_CPU0 = 32768,
    LOG_PORT_CPU  = 32768,

    LOG_PORT_NAME_END = 0xFFFFFFFF,/*uint32 enum*/
} rt_switch_port_name_t;

typedef enum rt_switch_mode_e
{
    RT_SWITCH_MODE_SFU = 0,
    RT_SWITCH_MODE_HGU,
    RT_SWITCH_MODE_HYBRID,
    RT_SWITCH_MODE_END
} rt_switch_mode_t;

/*
 * Function Declaration
 */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      rt_switch_init
 * Description:
 *      Initialize switch module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 * Note:
 *      Module must be initialized before using all of APIs in this module
 */
extern int32
rt_switch_init(void);

/* Function Name:
  *      rt_switch_maxPktLenByPort_get
 * Description:
  *      Get the max packet length setting of specific port
 * Input:
  *      port - speed type
 * Output:
  *      pLen - pointer to the max packet length
 * Return:
  *      RT_ERR_OK           - OK
  *      RT_ERR_FAILED       - failed
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
  *      RT_ERR_INPUT        - invalid enum speed type
 * Note:
  *      
 */
extern int32
rt_switch_maxPktLenByPort_get(rt_port_t port, uint32 *pLen);

/* Function Name:
  *      rt_switch_maxPktLenByPort_set
 * Description:
  *      Set the max packet length of specific port
 * Input:
  *      port  - port
  *      len   - max packet length
 * Output:
  *      None
 * Return:
  *      RT_ERR_OK      - OK
  *      RT_ERR_FAILED  - failed
  *      RT_ERR_INPUT   - invalid enum speed type
 * Note:
  *      
 */
extern int32
rt_switch_maxPktLenByPort_set(rt_port_t port, uint32 len);

/* Function Name:
  *      rt_switch_mode_set
 * Description:
  *      Set switch mode
 * Input:
  *      mode  - switch mode
 * Output:
 *      None
 * Return:
  *      RT_ERR_OK      - OK
  *      RT_ERR_FAILED  - failed
  *      RT_ERR_INPUT   - invalid enum mode type
  * Note:
  *      
 */
extern int32
rt_switch_mode_set(rt_switch_mode_t mode);

/* Function Name:
  *      rt_switch_mode_get
  * Description:
  *      Get switch mode
  * Input:
  *      None
  * Output:
  *      mode  - switch mode
  * Return:
  *      RT_ERR_OK      - OK
  *      RT_ERR_FAILED  - failed
  *      RT_ERR_INPUT   - invalid enum mode type
  * Note:
  *      
  */
extern int32
rt_switch_mode_get(rt_switch_mode_t *pMode);

/* Sub-module Name: Switch interface/version info retrieve */

/* Function Name:
 *      rt_switch_deviceInfo_get
  * Description:
 *      Get device information of the specific unit
  * Input:
 *      none
  * Output:
 *      pDevInfo - pointer to the device information
  * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
  * Note:
 *      
  */
extern int32
rt_switch_deviceInfo_get(rt_switch_devInfo_t *pDevInfo);

/* Function Name:
 *      rt_switch_phyPortId_get
  * Description:
 *      Get physical port id from logical port name
  * Input:
 *      portName - logical port name
  * Output:
 *      pPortId  - pointer to the physical port id
  * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
  * Note:
 *      Call RTK API the port ID must get from this API
  */
extern int32
rt_switch_phyPortId_get(rt_switch_port_name_t portName, int32 *pPortId);

/* Function Name:
 *      rt_switch_version_get
  * Description:
 *      Get chip version
  * Input:
 *      pChipId    - chip id
 *      pRev       - revision id
 *      pSubtype   - sub type
  * Output:
 *      None
  * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      
  */
extern int32
rt_switch_version_get(uint32 *pChipId, uint32 *pRev, uint32 *pSubtype);

#endif

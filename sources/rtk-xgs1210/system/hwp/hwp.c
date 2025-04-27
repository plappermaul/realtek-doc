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
 * $Revision: 96304 $
 * $Date: 2019-04-13 15:15:42 +0800 (Sat, 13 Apr 2019) $
 *
 * Purpose : Provide hardware profile related information.
 *
 * Feature :
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <common/util/rt_util.h>
#include <hwp/hw_profile.h>
#include <osal/print.h>
#include <hal/chipdef/chipdef.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>


/*
 * Symbol Definition
 */


/*
 * Data Type Definition
 */


/*
 * Function Declaration
 */


/* Function Name:
 *      hwp_chipInfo_update
 * Description:
 *      Get chip revision and update to hardware profile database.
 * Input:
 *      unit                - unit id
 * Output:
 *      hwp_hwProfile_t     - Pointer of profile description struct.
 * Return:
 *      RT_ERR_OK         - if success
 *      RT_ERR_FAILED   -if failed
 * Note:
 *      None
 */
int32
hwp_chipInfo_update(uint32 unit)
{
    uint32 chip_id = 0;
    uint32 chip_rev_id = 0;
    int32  ret;

    
    if (hwp_myHwProfile != NULL && HWP_SWCORE_ACC_METHOD(unit) == HWP_SW_ACC_VIR)
    {
        chip_id = HWP_SWITCH_DESCP(unit)->chip_id;
        chip_rev_id = 0;
    }
    else if ((ret = drv_swcore_cid_get(unit, &chip_id, &chip_rev_id)) != RT_ERR_OK)
    {
        RT_INIT_ERR(ret, MOD_RTCORE, "hwp get chip revision failed (unit %d)", unit);
        return RT_ERR_FAILED;
    }

    unitMapStruct[unit].pUnitInfo->hwp_chip_id= chip_id;
    unitMapStruct[unit].pUnitInfo->hwp_chip_revision= chip_rev_id;

    return RT_ERR_OK;
}


/* Function Name:
 *      hwp_portmaskAttriValid_Chk
 * Description:
 *      Check whether the input attribute of portmask is valid by chip.
 * Input:
 *      unit        - unit id
 *      portmask    - the portmask to be checked
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - valid portMask
 *      RT_ERR_FAILED - invalid portMask
 * Note:
 */
int32
hwp_portmaskAttriValid_Chk(uint32 unit, rtk_portmask_t *portmask, rt_port_attri_t PortMasktype, operationType_t opType)
{
    rtk_portmask_t  srcMask;
    rtk_portmask_t  dstMask;
    uint32 result = 0;
    uint32 bit_pos;
    int type;
    int i;

    if (portmask == NULL || (PortMasktype == 0) || opType >= HWP_OPERATION_END)
        return RT_ERR_FAILED;

    srcMask = *portmask;
    for (i = 0; i < RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST; i++)
    {
        result |= srcMask.bits[i];
    }
    if (result == 0)
        return RT_ERR_OK;

    RTK_PORTMASK_RESET(dstMask);
    if (opType == HWP_OPERATION_AND)
        RTK_PORTMASK_REVERT(dstMask);
    do
    {
        result = 0;
        bit_pos = 0;
        type = (PortMasktype) & (~(PortMasktype - 1));
        PortMasktype &= (~type);

        while((type >> bit_pos) > 1)
            bit_pos++;

        HWP_GET_ATTRI_PORTMASK(unit, bit_pos, srcMask);
        if (opType == HWP_OPERATION_OR)
        {
            RTK_PORTMASK_OR(dstMask, srcMask);
        }
        else
        {
            RTK_PORTMASK_AND(dstMask, srcMask);
        }
    }while(PortMasktype);

    srcMask = *portmask;
    RTK_PORTMASK_AND(srcMask, dstMask);

    if (RTK_PORTMASK_COMPARE(srcMask, *portmask) == 0)
        return RT_ERR_OK;

    return RT_ERR_FAILED;

}

/* Function Name:
 *      hwp_portmaskEthTypeValid_Chk
 * Description:
 *      Check whether the input ether type of portmask is valid by chip.
 * Input:
 *      unit        - unit id
 *      portmask    - the portmask to be checked
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - valid portMask
 *      RT_ERR_FAILED - invalid portMask
 * Note:
 */
int32
hwp_portmaskEthTypeValid_Chk(uint32 unit, rtk_portmask_t *portmask, rt_port_ethType_t PortMasktype, operationType_t opType)
{
    rtk_portmask_t  srcMask;
    rtk_portmask_t  dstMask;
    uint32 result = 0;
    uint32 bit_pos;
    int type;
    int i;

    if (portmask == NULL || (PortMasktype == 0) || opType >= HWP_OPERATION_END)
        return RT_ERR_FAILED;

    srcMask = *portmask;
    for (i = 0; i < RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST; i++)
    {
        result |= srcMask.bits[i];
    }
    if (result == 0)
        return RT_ERR_OK;

    RTK_PORTMASK_RESET(dstMask);
    if (opType == HWP_OPERATION_AND)
        RTK_PORTMASK_REVERT(dstMask);
    do
    {
        result = 0;
        bit_pos = 0;
        type = (PortMasktype) & (~(PortMasktype - 1));
        PortMasktype &= (~type);

        while((type >> bit_pos) > 1)
            bit_pos++;

        HWP_GET_ETHTYPE_PORTMASK(unit, bit_pos, srcMask);

        if (opType == HWP_OPERATION_OR)
        {
            RTK_PORTMASK_OR(dstMask, srcMask);
        }
        else
        {
            RTK_PORTMASK_AND(dstMask, srcMask);
        }
    }while(PortMasktype);

    srcMask = *portmask;
    RTK_PORTMASK_AND(srcMask, dstMask);

    if (RTK_PORTMASK_COMPARE(srcMask, *portmask) == 0)
        return RT_ERR_OK;

    return RT_ERR_FAILED;
}

/* Function Name:
 *      hwp_portmaskMediumValid_Chk
 * Description:
 *      Check whether the medium of portmask is valid by chip.
 * Input:
 *      unit        - unit id
 *      portmask    - the portmask to be checked
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - valid portMask
 *      RT_ERR_FAILED - invalid portMask
 * Note:
 */
int32
hwp_portmaskMediumValid_Chk(uint32 unit, rtk_portmask_t *portmask, rt_port_medium_t PortMasktype, operationType_t opType)
{
    rtk_portmask_t  srcMask;
    rtk_portmask_t  dstMask;
    uint32 result = 0;
    uint32 bit_pos;
    int type;
    int i;

    if (portmask == NULL || (PortMasktype == 0) || opType >= HWP_OPERATION_END)
        return RT_ERR_FAILED;

    srcMask = *portmask;
    for (i = 0; i < RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST; i++)
    {
        result |= srcMask.bits[i];
    }
    if (result == 0)
        return RT_ERR_OK;

    RTK_PORTMASK_RESET(dstMask);
    if (opType == HWP_OPERATION_AND)
        RTK_PORTMASK_REVERT(dstMask);
    do
    {
        result = 0;
        bit_pos = 0;
        dstMask = *portmask;
        type = (PortMasktype) & (~(PortMasktype - 1));
        PortMasktype &= (~type);

        while((type >> bit_pos) > 1)
            bit_pos++;

        HWP_GET_MEDIUM_PORTMASK(unit, bit_pos, srcMask);

        if (opType == HWP_OPERATION_OR)
        {
            RTK_PORTMASK_OR(dstMask, srcMask);
        }
        else
        {
            RTK_PORTMASK_AND(dstMask, srcMask);
        }
    }while(PortMasktype);

    srcMask = *portmask;
    RTK_PORTMASK_AND(srcMask, dstMask);

    if (RTK_PORTMASK_COMPARE(srcMask, *portmask) == 0)
        return RT_ERR_OK;

    return RT_ERR_FAILED;
}

/* Function Name:
 *      hwp_portmaskValid_Chk
 * Description:
 *      Check whether portmask is valid by chip.
 * Input:
 *      unit        - unit id
 *      portmask    - the portmask to be checked
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - valid portMask
 *      RT_ERR_FAILED - invalid portMask
 * Note:
 */
int32
hwp_portmaskValid_Chk(uint32 unit, rtk_portmask_t *portmask)
{
    rtk_portmask_t  srcMask;
    rtk_portmask_t  dstMask;
    int i;

    if (portmask == NULL)
        return RT_ERR_FAILED;

    dstMask = *portmask;

    HWP_GET_ALL_PORTMASK(unit, srcMask);
    RTK_PORTMASK_REMOVE(dstMask, srcMask);
    for (i = 0; i < RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST; i++)
    {
        if(dstMask.bits[i] != 0)
        {
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      hwp_unit_get_next
 * Description:
 *      Get first or next valid unit id information
 * Input:
 *      unit     - unit id.
 *                 unit is declared as integer here, so, if unit id is less than zero,
 *                 this API will return first available unit id.
 * Output:
 *      pUnitInfo - pointer to the unit information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_ENTRY_NOTFOUND   - There is no next entry according to input unit id.
 * Note:
 *      None
 */
int32
hwp_unit_get_next(int32 unit, int32 *pNextUnit)
{
    uint32      unit_id;

    RT_PARAM_CHK((NULL == pNextUnit), RT_ERR_NULL_POINTER);

    HWP_UNIT_TRAVS_LOCAL(unit_id)
    {
        if (unit < 0)
        {
            *pNextUnit = (int32)unit_id;
            return RT_ERR_OK;
        }

        if( unit_id > unit)
        {
            *pNextUnit = (int32)unit_id;
            return RT_ERR_OK;
        }
    }

    return RT_ERR_ENTRY_NOTFOUND;

}


/* Function Name:
 *      _hwp_portInUplink_check
 * Description:
 *      Check a port number if in the range of UPLINK port
 *      (uplink port has a special definition here: the last 4 ports of 9300/9310)
 * Input:
 *      unit        - unit ID
 *      port        - port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - in range
 *      RT_ERR_FAILED   - out of range
 * Note:
 *      None
 */
int32
_hwp_portInUplink_check(uint32 family_id, uint32 port)
{
    switch(family_id)
    {
        case RTL9300_FAMILY_ID:
            if ((port >= RTL9300_UPLINK_PORT_START) && (port <= RTL9300_UPLINK_PORT_END))
                return RT_ERR_OK;
            break;

        case RTL9310_FAMILY_ID:
            if ((port >= RTL9310_UPLINK_PORT_START) && (port <= RTL9310_UPLINK_PORT_END))
                return RT_ERR_OK;
            break;

        default:
            break;
    }
    return RT_ERR_FAILED;
}


/* Function Name:
 *      hwp_multiSdsPortLaneNum_get
 * Description:
 *      Get lane number of the serdes for multi-serdes-Port.
 *      The lowest ID serdes is lane 0.
 * Input:
 *      unit        - unit ID
 *      sdsId       - serdes ID
 * Output:
 *      pLane       - lane number
 * Return:
 *      RT_ERR_OK       - success
 *      RT_ERR_FAILED   - failed. The sdsId is not multi-serdes-Port's serdes.
 * Note:
 *      None
 */
int32
hwp_multiSdsPortLaneNum_get(uint32 unit, uint8 sdsId, uint32 *pLane)
{
    uint32          lane;
    uint32          id, baseSds;

    *pLane = 0;
    if (sdsId >= RTK_MAX_SDS_PER_UNIT)
        return RT_ERR_FAILED;

    if ((baseSds = HWP_MULTI_SDS_BASESDS(unit, sdsId)) == HWP_NONE)
        return RT_ERR_FAILED;

    lane = 0;
    for (id = baseSds; id < RTK_MAX_SDS_PER_UNIT; id++)
    {
        if (HWP_MULTI_SDS_BASESDS(unit, id) != baseSds)
            continue;

        if (id == sdsId)
        {
            *pLane = lane;
            return RT_ERR_OK;
        }
        else
        {
            lane++;
        }
    }/* end for */

    return RT_ERR_FAILED;
}


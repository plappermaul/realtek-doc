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
 * Purpose : Define hardware profile structure, API, and macros.
 *           Hardware profile describes the hardware board design, includes what switch ASIC,
 *           PHY chip, BSP, ...etc have been designed.
 *
 * Feature :
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <common/util/rt_util.h>
#include <hwp/hw_profile.h>
#include <common/rtcore/rtcore_init.h>
#include <osal/sem.h>
#include <osal/lib.h>
#include <osal/print.h>
#include <osal/memory.h>
#include <hwp/hwp_util.h>
#include <hal/chipdef/chipdef.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>


/*
 * Data Type Definition
 */
uint32                      hwp_init_status = INIT_NOT_COMPLETED;
static uint32               hwp_init_sys_status = INIT_NOT_COMPLETED;
uint32                      hwp_unit_pool[RTK_MAX_NUM_OF_UNIT]={FALSE};
hwp_hwProfile_t             *hwp_myHwProfile = NULL;
static uint32               parsedInfo_cnt=0;
hwp_portDescp_t             portDescpEmpty;
hwp_serdesDescp_t           serdesDescpEmpty;
hwp_scDescp_t               scDescpEmpty;
hwp_swDescp_t               swDescpEmpty;
hwp_hwProfile_t             hwpEmpty;
hwp_unitInfo_t              unitInfoEmpty;
hwp_parsedInfo_t            parsedInfoEmpty;
hwp_unitMap_t               unitMapStruct[RTK_MAX_NUM_OF_UNIT];
#if defined(CONFIG_SDK_EXTERNAL_CPU)
int                         hwp_ExternalCpu=TRUE;
#else
int                         hwp_ExternalCpu=FALSE;
#endif

/* semaphore */
osal_mutex_t hwp_unit_sem;

#define HWP_SEM_LOCK()    \
    do {\
        if (osal_sem_mutex_take(hwp_unit_sem, OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
        {\
            RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_INIT), "semaphore lock failed");\
            return RT_ERR_SEM_LOCK_FAILED;\
        }\
    } while(0)

#define HWP_SEM_UNLOCK()   \
    do {\
        if (osal_sem_mutex_give(hwp_unit_sem) != RT_ERR_OK)\
        {\
            RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_INIT), "semaphore unlock failed");\
            return RT_ERR_SEM_UNLOCK_FAILED;\
        }\
    } while(0)

/*
 * Function Declaration
 */


/* Function Name:
 *      _hwp_chipFamilyID_get
 * Description:
 *      Map chip model to chip family
 * Input:
 *      None
 * Output:
 *      hwp_swDescp_t     - Pointer of chip description struct.
 * Return:
 *      RT_ERR_OK       - if success
 *      RT_ERR_FAILED   - if failed
 * Note:
 *      None
 */
static int32
_hwp_chipFamilyID_get(hwp_swDescp_t *pSw, hwp_unitInfo_t *unit_info)
{
    switch(pSw->chip_id)
    {
        case RTL8351M_CHIP_ID:
        case RTL8352M_CHIP_ID:
        case RTL8353M_CHIP_ID:
        case RTL8352MES_CHIP_ID:
        case RTL8353MES_CHIP_ID:
            unit_info->hwp_chip_family_id = RTL8350_FAMILY_ID;
            break;
        case RTL8390M_CHIP_ID:
        case RTL8391M_CHIP_ID:
        case RTL8392M_CHIP_ID:
        case RTL8393M_CHIP_ID:
        case RTL8396M_CHIP_ID:
        case RTL8392MES_CHIP_ID:
        case RTL8393MES_CHIP_ID:
        case RTL8396MES_CHIP_ID:
            unit_info->hwp_chip_family_id = RTL8390_FAMILY_ID;
            break;
        case RTL8330M_CHIP_ID:
        case RTL8332M_CHIP_ID:
            unit_info->hwp_chip_family_id = RTL8330_FAMILY_ID;
            break;
        case RTL8380M_CHIP_ID:
        case RTL8381M_CHIP_ID:
        case RTL8382M_CHIP_ID:
            unit_info->hwp_chip_family_id = RTL8380_FAMILY_ID;
            break;
        case RTL9310_CHIP_ID:
        case RTL9311_CHIP_ID:
        case RTL9311R_CHIP_ID:
        case RTL9311E_CHIP_ID:
        case RTL9312_CHIP_ID:
        case RTL9313_CHIP_ID:
            unit_info->hwp_chip_family_id = RTL9310_FAMILY_ID;
            break;
        case RTL9301_CHIP_ID_24G:
        case RTL9303_CHIP_ID_8XG:
        case RTL9301_CHIP_ID:
        case RTL9303_CHIP_ID:
        case RTL9301H_CHIP_ID_4X2_5G:
        case RTL9302A_CHIP_ID_12X2_5G:
        case RTL9302B_CHIP_ID_8X2_5G:
        case RTL9302C_CHIP_ID_16X2_5G:
        case RTL9302D_CHIP_ID_24X2_5G:
        case RTL9302DE_CHIP_ID:
        case RTL9301H_CHIP_ID:
        case RTL9302A_CHIP_ID:
        case RTL9302B_CHIP_ID:
        case RTL9302C_CHIP_ID:
        case RTL9302D_CHIP_ID:
            unit_info->hwp_chip_family_id = RTL9300_FAMILY_ID;
            break;
        case THIRD_PARTY_MAC_CHIP_ID:
            unit_info->hwp_chip_family_id = RTLCHIP_FAMILY_ID_NONE;
            break;
        default:
            RT_ERR(RT_ERR_FAILED, MOD_INIT, "Wrong chip ID!(%d)",pSw->chip_id);
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      _hwp_unitInfo2unitIdx_get
 * Description:
 *      Give a hwp_unitInfo_t, and find out which local unit index it is
 * Input:
 *      *unit_info   - the pointer of hwp_unitInfo_t
 * Output:
 *      *idx         - the local unit index of this unit_info
 * Return:
 *      RT_ERR_OK       - if success
 *      RT_ERR_FAILED   - if failed
 * Note:
 *      None
 */
int32
_hwp_unitInfo2unitIdx_get(uint32 *idx, hwp_unitInfo_t *unit_info)
{
    int i;
    hwp_parsedInfo_t *parsedInfo = (hwp_parsedInfo_t *)unit_info->parsed_info;

    for(i=0;i<RTK_MAX_NUM_OF_UNIT_LOCAL;i++)
    {
        if(&parsedInfo->unitInfo[i] == unit_info)
        {
            *idx = i;
            return RT_ERR_OK;
        }
    }

    return RT_ERR_FAILED;
}


/* Function Name:
 *      _hwp_emptyDB_init
 * Description:
 *      There are some empty database. They are initialized here.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - if success
 *      RT_ERR_FAILED   - if failed
 * Note:
 *      None
 */
static int32
_hwp_emptyDB_init(void)
{
    int i,j;

    osal_memset(&portDescpEmpty, 0, sizeof(hwp_portDescp_t));
    osal_memset(&serdesDescpEmpty, 0, sizeof(hwp_serdesDescp_t));
    osal_memset(&scDescpEmpty, 0, sizeof(hwp_scDescp_t));
    osal_memset(&swDescpEmpty, 0, sizeof(hwp_swDescp_t));
    osal_memset(&hwpEmpty, 0, sizeof(hwp_hwProfile_t));
    osal_memset(&unitInfoEmpty, 0, sizeof(hwp_unitInfo_t));
    osal_memset(&parsedInfoEmpty, 0, sizeof(hwp_parsedInfo_t));

    parsedInfoEmpty.hwp = &hwpEmpty;
    hwpEmpty.parsed_info = &parsedInfoEmpty;
    serdesDescpEmpty.mode = RTK_MII_NONE;

    for(j=0;j<RTK_MAX_PORT_PER_UNIT;j++)
    {
        unitInfoEmpty.hwp_macID2PortDescp[j] = &portDescpEmpty;
        unitInfoEmpty.hwp_macID2SCDescp[j] = &scDescpEmpty;
    }

    for(j=0;j<RTK_MAX_SDS_PER_UNIT;j++)
    {
        unitInfoEmpty.hwp_sdsID2SerdesDescp[j] = &serdesDescpEmpty;
    }


    for(i=0;i<RTK_MAX_NUM_OF_UNIT_LOCAL;i++)
    {
        hwpEmpty.swDescp[i] = &swDescpEmpty;
        parsedInfoEmpty.unitInfo[i] = unitInfoEmpty;
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      _hwp_portCount_default
 * Description:
 *      Init port count structure to default
 * Input:
 *      None
 * Output:
 *      hwp_portCount_t     - Pointer of port count struct.
 * Return:
 *      None
 * Note:
 *      None
 */
static void
_hwp_portCount_default(hwp_portCount_t  *pc)
{
    pc->count = 0;
    pc->min = RTK_MAX_PORT_PER_UNIT;
    pc->max = 0;
}

/* Function Name:
 *      _hwp_portCountAll_default
 * Description:
 *      Init port count structure to default for all kinds of port
 * Input:
 *      None
 * Output:
 *      unit_info     - Pointer of hwp_unitInfo_t struct.
 * Return:
 *      None
 * Note:
 *      None
 */
static void
_hwp_portCountAll_default(hwp_unitInfo_t *unit_info)
{
    _hwp_portCount_default(&unit_info->hwp_portCntEther);
    _hwp_portCount_default(&unit_info->hwp_portCntUplink);
    _hwp_portCount_default(&unit_info->hwp_portCntCascade);
    _hwp_portCount_default(&unit_info->hwp_portCntFe);
    _hwp_portCount_default(&unit_info->hwp_portCntGe);
    _hwp_portCount_default(&unit_info->hwp_portCntXge);
    _hwp_portCount_default(&unit_info->hwp_portCntSXge);
    _hwp_portCount_default(&unit_info->hwp_portCntCopper);
    _hwp_portCount_default(&unit_info->hwp_portCntFiber);
    _hwp_portCount_default(&unit_info->hwp_portCntCombo);
    _hwp_portCount_default(&unit_info->hwp_portCntSerdes);
}


/* Function Name:
 *      _hwp_portMaskAll_default
 * Description:
 *      dfault all port mask
 * Input:
 *      None
 * Output:
 *      unit_info     - Pointer of hwp_unitInfo_t struct.
 * Return:
 *      None
 * Note:
 *      None
 */
static void
_hwp_portMaskAll_default(hwp_unitInfo_t *unit_info)
{
    int     i;

    RTK_PORTMASK_RESET(unit_info->hwp_allPortMask);

    for (i = 0; i < HWP_ATTRI_ID_END; i++)
    {
        RTK_PORTMASK_RESET(unit_info->hwp_attriPortMask[i]);
    }

    for (i = 0; i < HWP_ETHTYPE_ID_END; i++)
    {
        RTK_PORTMASK_RESET(unit_info->hwp_ethTypePortMask[i]);
    }

    for (i = 0; i < HWP_MEDIUM_ID_END; i++)
    {
        RTK_PORTMASK_RESET(unit_info->hwp_mediumPortMask[i]);
    }
}


/* Function Name:
 *      _hwp_parsedInfo_default
 * Description:
 *      Initial default value for hwp_parsedInfo_t
 * Input:
 *      None
 * Output:
 *      parsedInfo - parsedInfo with default value
 * Return:
 *      None
 * Note:
 *      None
 */
void
_hwp_parsedInfo_default(hwp_parsedInfo_t *parsedInfo)
{
    int i, j;

    osal_memset(parsedInfo, 0, sizeof(hwp_parsedInfo_t));

    parsedInfo->hwp = NULL;
    parsedInfo->hwp_embeddedCpuUnitId = HWP_NONE;
    parsedInfo->hwp_localUnitCnt = 0;

    for(i = 0; i < RTK_MAX_NUM_OF_UNIT_LOCAL; i++)
    {
        for(j = 0 ; j < RTK_MAX_PORT_PER_UNIT; j++)
        {
            parsedInfo->unitInfo[i].hwp_macID2PortDescp[j] = &portDescpEmpty;
            parsedInfo->unitInfo[i].hwp_macID2SCDescp[j] = &scDescpEmpty;
            parsedInfo->unitInfo[i].hwp_macID2phyAddr[j] = HWP_NONE;
            parsedInfo->unitInfo[i].hwp_macID2phyType[j] = RTK_PHYTYPE_INVALID;
            parsedInfo->unitInfo[i].hwp_macID2SerdesID[j] = HWP_NONE;
            parsedInfo->unitInfo[i].hwp_phyBaseMacID[j] = HWP_NONE;
            parsedInfo->unitInfo[i].hwp_scBaseMacID[j] = HWP_NONE;
        }
        parsedInfo->unitInfo[i].hwp_chip_family_id = RTLCHIP_FAMILY_ID_NONE;
        parsedInfo->unitInfo[i].hwp_portCpuMacId = HWP_VALUE_NO_INIT;
        _hwp_portCountAll_default(&parsedInfo->unitInfo[i]);
        _hwp_portMaskAll_default(&parsedInfo->unitInfo[i]);

        for(j = 0; j < RTK_MAX_SDS_PER_UNIT; j++)
        {
            parsedInfo->unitInfo[i].hwp_sdsID2phyType[j] = RTK_PHYTYPE_NONE;
            parsedInfo->unitInfo[i].hwp_sdsID2MacID[j] = HWP_NONE;
            parsedInfo->unitInfo[i].hwp_multiSdsBaseSds[j] = HWP_NONE;
            parsedInfo->unitInfo[i].hwp_sdsID2SerdesDescp[j] = &serdesDescpEmpty;
        }
    }/* end for i */
}


/* Function Name:
 *      _hwp_parsedInfo_allocate
 * Description:
 *      Allocate a hwp_parsedInfo_t and init to default.
 * Input:
 *      None
 * Output:
 *      hwp_parsedInfo_t *  - return the result
 * Return:
 *      RT_ERR_OK       - if success
 *      RT_ERR_FAILED   - if failed
 * Note:
 *      None
 */
static int32
_hwp_parsedInfo_allocate(hwp_parsedInfo_t **p)
{
    hwp_parsedInfo_t *parsedInfo;

    if (parsedInfo_cnt >= HWP_MAX_PROFILE)
    {
        return RT_ERR_FAILED;
    }

    parsedInfo = osal_alloc(sizeof(hwp_parsedInfo_t));
    if (NULL == parsedInfo)
    {
        return RT_ERR_FAILED;
    }

    parsedInfo_cnt++;

    _hwp_parsedInfo_default(parsedInfo);

    *p = parsedInfo;

    return RT_ERR_OK;
}


/* Function Name:
 *      _hwp_chip_parse
 * Description:
 *      Get chip related var.
 * Input:
 *      None
 * Output:
 *      hwp_hwProfile_t     - Pointer of profile description struct.
 * Return:
 *      RT_ERR_OK       - if success
 *      RT_ERR_FAILED   - failed
 * Note:
 *      None
 */
int32
_hwp_chip_parse(hwp_parsedInfo_t *parsedInfo)
{
    hwp_hwProfile_t *p = parsedInfo->hwp;
    uint32  i;

    if (NULL == p)
    {
        RT_ERR(RT_ERR_FAILED, MOD_INIT, "parsedInfo doesn't contain hwp!");
        return RT_ERR_FAILED;
    }

    if (p->sw_count > RTK_MAX_NUM_OF_UNIT_LOCAL)
    {
        RT_ERR(RT_ERR_FAILED, MOD_INIT, "Wrong hardware profile [%s]: sw_count too large!",p->identifier.name);
        return RT_ERR_FAILED;
    }

    if (p->soc.swDescp_index >= p->sw_count)
    {
        RT_ERR(RT_ERR_FAILED, MOD_INIT, "Wrong hardware profile [%s]: swDescp_index too large!",p->identifier.name);
        return RT_ERR_FAILED;
    }

    parsedInfo->hwp_localUnitCnt = p->sw_count;

    for(i = 0;i < p->sw_count; i++)
    {
        if (p->swDescp[i] == NULL)
        {
            RT_ERR(RT_ERR_FAILED, MOD_INIT, "Wrong hardware profile [%s]: swDescp NULL!",p->identifier.name);
            return RT_ERR_FAILED;
        }

        parsedInfo->unitInfo[i].swDescp = p->swDescp[i];
        parsedInfo->unitInfo[i].parsed_info = (void *)parsedInfo;
        parsedInfo->unitInfo[i].hwp_chip_id = p->swDescp[i]->chip_id;

        /* init chip family ID */
        _hwp_chipFamilyID_get(p->swDescp[i], &parsedInfo->unitInfo[i]);

    }

    /* init embedded Cpu */
    parsedInfo->hwp_embeddedCpuUnitId = p->soc.swDescp_index;

    return RT_ERR_OK;
}


/* Function Name:
 *      _hwp_portCount_add
 * Description:
 *      Increase port coun
 * Input:
 *      None
 * Output:
 *      hwp_portCount_t     - Pointer of port count struct.
 *      macId               - Port's mac ID
 * Return:
 *      None
 * Note:
 *      None
 */
static void
_hwp_portCount_add(hwp_portCount_t  *pc, uint32 macId)
{
    pc->count++;
    if (macId < pc->min)
        pc->min = macId;
    if (macId > pc->max)
        pc->max = macId;
}


/* Function Name:
 *      _hwp_portCount_init
 * Description:
 *      Initial port count database from port description structure
 * Input:
 *      None
 * Output:
 *      unit                - Unit ID
 *      portDescp           - port description structure
 * Return:
 *      None
 * Note:
 *      None
 */
static void
_hwp_portCount_init(hwp_unitInfo_t *unitInfo, hwp_portDescp_t *portDescp)
{
    uint32 macId =portDescp->mac_id;

    if (portDescp->attr & HWP_ETH)
        _hwp_portCount_add(&unitInfo->hwp_portCntEther, macId);
    if (portDescp->attr & HWP_UPLINK)
        _hwp_portCount_add(&unitInfo->hwp_portCntUplink, macId);
    if (portDescp->attr & HWP_CASCADE)
        _hwp_portCount_add(&unitInfo->hwp_portCntCascade, macId);
    if (portDescp->attr & HWP_CPU)
        unitInfo->hwp_portCpuMacId = macId;

    if (portDescp->eth != HWP_NONE)
    {
        if(portDescp->eth & HWP_FE)
            _hwp_portCount_add(&unitInfo->hwp_portCntFe, macId);
        if(portDescp->eth & HWP_GE)
            _hwp_portCount_add(&unitInfo->hwp_portCntGe, macId);
        if(portDescp->eth & HWP_2_5GE)
            _hwp_portCount_add(&unitInfo->hwp_portCnt2_5ge, macId);
        if(portDescp->eth & HWP_5GE)
            _hwp_portCount_add(&unitInfo->hwp_portCnt5ge, macId);
        if(portDescp->eth & HWP_XGE)
            _hwp_portCount_add(&unitInfo->hwp_portCntXge, macId);
        if(portDescp->eth & HWP_SXGE)
            _hwp_portCount_add(&unitInfo->hwp_portCntSXge, macId);
    }

    if (portDescp->medi != HWP_NONE)
    {
        if( portDescp->medi & HWP_COPPER)
            _hwp_portCount_add(&unitInfo->hwp_portCntCopper, macId);
        if( portDescp->medi & HWP_FIBER)
            _hwp_portCount_add(&unitInfo->hwp_portCntFiber, macId);
        if( portDescp->medi & HWP_COMBO)
            _hwp_portCount_add(&unitInfo->hwp_portCntCombo, macId);
        if( portDescp->medi & HWP_SERDES)
            _hwp_portCount_add(&unitInfo->hwp_portCntSerdes, macId);
    }

}


/* Function Name:
 *      _hwp_portMask_init
 * Description:
 *      Initial port mask database from port description structure
 * Input:
 *      None
 * Output:
 *      unit                - Unit ID
 *      portDescp           - port description structure
 * Return:
 *      None
 * Note:
 *      None
 */
static void
_hwp_portMask_init(hwp_unitInfo_t *unitInfo, hwp_portDescp_t *portDescp)
{
    uint32 macId =portDescp->mac_id;

    RTK_PORTMASK_PORT_SET(unitInfo->hwp_allPortMask, macId);

    if(portDescp->attr & HWP_ETH)
        RTK_PORTMASK_PORT_SET(unitInfo->hwp_attriPortMask[HWP_ETH_ID], macId);
    if(portDescp->attr & HWP_UPLINK)
        RTK_PORTMASK_PORT_SET(unitInfo->hwp_attriPortMask[HWP_UPLINK_ID], macId);
    if(portDescp->attr & HWP_CASCADE)
        RTK_PORTMASK_PORT_SET(unitInfo->hwp_attriPortMask[HWP_CASCADE_ID], macId);
    if(portDescp->attr & HWP_CPU)
        RTK_PORTMASK_PORT_SET(unitInfo->hwp_attriPortMask[HWP_CPU_ID], macId);
    if(portDescp->attr & HWP_SC)
        RTK_PORTMASK_PORT_SET(unitInfo->hwp_attriPortMask[HWP_SC_ID], macId);

    if (portDescp->eth != HWP_NONE)
    {
        if(portDescp->eth & HWP_FE)
            RTK_PORTMASK_PORT_SET(unitInfo->hwp_ethTypePortMask[HWP_FE_ID], macId);
        if(portDescp->eth & HWP_GE)
            RTK_PORTMASK_PORT_SET(unitInfo->hwp_ethTypePortMask[HWP_GE_ID], macId);
        if(portDescp->eth & HWP_2_5GE)
            RTK_PORTMASK_PORT_SET(unitInfo->hwp_ethTypePortMask[HWP_2_5GE_ID], macId);
        if(portDescp->eth & HWP_5GE)
            RTK_PORTMASK_PORT_SET(unitInfo->hwp_ethTypePortMask[HWP_5GE_ID], macId);
        if(portDescp->eth & HWP_XGE)
            RTK_PORTMASK_PORT_SET(unitInfo->hwp_ethTypePortMask[HWP_XGE_ID], macId);
        if(portDescp->eth & HWP_SXGE)
            RTK_PORTMASK_PORT_SET(unitInfo->hwp_ethTypePortMask[HWP_SXGE_ID], macId);

    }

    if (portDescp->medi != HWP_NONE)
    {
        if(portDescp->medi & HWP_COPPER)
            RTK_PORTMASK_PORT_SET(unitInfo->hwp_mediumPortMask[HWP_COPPER_ID], macId);
        if(portDescp->medi & HWP_FIBER)
            RTK_PORTMASK_PORT_SET(unitInfo->hwp_mediumPortMask[HWP_FIBER_ID], macId);
        if(portDescp->medi & HWP_COMBO)
            RTK_PORTMASK_PORT_SET(unitInfo->hwp_mediumPortMask[HWP_COMBO_ID], macId);
        if(portDescp->medi & HWP_SERDES)
            RTK_PORTMASK_PORT_SET(unitInfo->hwp_mediumPortMask[HWP_SERDES_ID], macId);
    }
}


/* Function Name:
 *      _hwp_port_parse
 * Description:
 *      Init port related var.
 * Input:
 *      None
 * Output:
 *      hwp_hwProfile_t     - Pointer of profile description struct.
 * Return:
 *      RT_ERR_OK - if success
 *      RT_ERR_FAILED - if something wrong, mostly the hardware profile is wrong
 *
 * Note:
 *      None
 */
int32
_hwp_port_parse(hwp_parsedInfo_t *parsedInfo)
{
    hwp_hwProfile_t *p = parsedInfo->hwp;
    uint8  macId, phy_idx, sc_idx;
    uint32 i;
    uint32 j;
    uint32 count;
    hwp_swDescp_t   *sw;
    uint8   scBaseMacId_perSc[RTK_MAX_SC_PER_UNIT];

    /* init var. */
    for(i = 0; i < p->sw_count; i++)
    {
        sw = p->swDescp[i];
        parsedInfo->unitInfo[i].hwp_minMacId = RTK_MAX_PORT_PER_UNIT;
        parsedInfo->unitInfo[i].hwp_maxMacId = 0;

        /* calculate the port count */
        count = 0;
        for(j = 0 ; j < RTK_MAX_PORT_PER_UNIT; j++)
        {
            if(sw->port.descp[count].mac_id == HWP_END)
                break;
            count++;
        }
        sw->port.count = count;

        /* calculate the phy count */
        count = 0;
        for(j = 0 ; j < RTK_MAX_PHY_PER_UNIT; j++)
        {
            if(sw->phy.descp[count].chip == HWP_END)
                break;
            count++;
        }
        sw->phy.count = count;

        /* calculate the sc count */
        for(j = 0 ; j < RTK_MAX_SC_PER_UNIT; j++)
            scBaseMacId_perSc[j] = HWP_NONE;
        count = 0;
        for(j = 0 ; j < sw->port.count; j++)
        {
            if ((sw->port.descp[j].attr & HWP_SC) && ((sc_idx = sw->port.descp[j].sc_idx) < RTK_MAX_SC_PER_UNIT))
            {
                if (scBaseMacId_perSc[sc_idx] == HWP_NONE)
                {
                    scBaseMacId_perSc[sc_idx] = sw->port.descp[j].mac_id;
                    count++;
                }
            }
        }
        sw->sc.count = count;


        _hwp_portCountAll_default(&parsedInfo->unitInfo[i]);
        _hwp_portMaskAll_default(&parsedInfo->unitInfo[i]);

        for(j = 0 ; j < sw->port.count ; j++)
        {
            macId = sw->port.descp[j].mac_id;
            phy_idx = sw->port.descp[j].phy_idx;

            if(macId >= RTK_MAX_PORT_ID_PER_UNIT)
            {
                RT_ERR(RT_ERR_FAILED, MOD_INIT, "Wrong hardware profile [%s]: mac ID too large",p->identifier.name);
                return RT_ERR_FAILED;
            }

            if(phy_idx == HWP_NONE)
            {
                if (sw->port.descp[j].medi & HWP_SERDES)
                {
                    parsedInfo->unitInfo[i].hwp_macID2phyType[macId] = RTK_PHYTYPE_SERDES;
                    parsedInfo->unitInfo[i].hwp_phyBaseMacID[macId]  = macId;
                }
            }
            else if (phy_idx == HWP_TBD)
            {

            }
            else
            {
                if(phy_idx >= sw->phy.count)
                {
                    RT_ERR(RT_ERR_FAILED, MOD_INIT, "Wrong hardware profile [%s]: port%d phy_idx too large",p->identifier.name,macId);
                    return RT_ERR_FAILED;
                }
                parsedInfo->unitInfo[i].hwp_macID2phyType[macId] = sw->phy.descp[phy_idx].chip;
                parsedInfo->unitInfo[i].hwp_phyBaseMacID[macId]  = sw->phy.descp[phy_idx].mac_id;
            }

            parsedInfo->unitInfo[i].hwp_macID2PortDescp[macId] = &(sw->port.descp[j]);
            parsedInfo->unitInfo[i].hwp_macID2phyAddr[macId] = sw->port.descp[j].phy_addr;
            if( RT_ERR_OK == _hwp_portInUplink_check(FAMILY_ID(sw->chip_id), macId))
            {
                parsedInfo->unitInfo[i].hwp_macID2PortDescp[macId]->attr |= HWP_UPLINK;
            }


            if (sw->port.descp[j].attr & HWP_SC)
            {
                if ((sc_idx = sw->port.descp[j].sc_idx) < RTK_MAX_SC_PER_UNIT)
                {
                    parsedInfo->unitInfo[i].hwp_macID2SCDescp[macId] = &(sw->sc.descp[sc_idx]);
                    parsedInfo->unitInfo[i].hwp_scBaseMacID[macId] = scBaseMacId_perSc[sc_idx];
                }
            }

            /* init hwp_minMacId and hwp_maxMacId */
            if(macId < parsedInfo->unitInfo[i].hwp_minMacId)
                parsedInfo->unitInfo[i].hwp_minMacId = macId;
            if(macId > parsedInfo->unitInfo[i].hwp_maxMacId)
                parsedInfo->unitInfo[i].hwp_maxMacId = macId;

            _hwp_portMask_init(&parsedInfo->unitInfo[i], &(sw->port.descp[j]));

            _hwp_portCount_init(&parsedInfo->unitInfo[i], &(sw->port.descp[j]));

        }
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      _hwp_serdes_parse
 * Description:
 *      Initial serdes related information
 * Input:
 *      None
 * Output:
 *      hwp_chipDescp_t     - Pointer of chip description struct.
 * Return:
 *      None
 *
 * Note:
 *      None
 */
int32
_hwp_serdes_parse(hwp_parsedInfo_t *parsedInfo)
{
    hwp_hwProfile_t *p = parsedInfo->hwp;
    uint32          i, j;
    uint32          count;
    uint32          sdsId, macId, phy_idx, sds_idx;
    hwp_swDescp_t  *swDescp;

    for(i = 0; i < p->sw_count; i++)
    {
        if(p->swDescp[i] == NULL)
            continue;

        swDescp = p->swDescp[i];
        /* calculate the serdes count */
        count = 0;
        for(j = 0 ; j < RTK_MAX_SDS_PER_UNIT; j++)
        {
            if(swDescp->serdes.descp[j].sds_id == HWP_NONE)
                break;
            count++;
        }
        swDescp->serdes.count = count;

        for (j = 0; j < swDescp->serdes.count; j++)
        {
            sdsId = swDescp->serdes.descp[j].sds_id;
            if (sdsId >= RTK_MAX_SDS_PER_UNIT)
            {
                RT_ERR(RT_ERR_FAILED, MOD_INIT, "Wrong hardware profile [%s]: wrong serdes ID %d!", p->identifier.name, sdsId);
                return RT_ERR_FAILED;
            }
            parsedInfo->unitInfo[i].hwp_sdsID2SerdesDescp[sdsId] = &swDescp->serdes.descp[j];

            parsedInfo->unitInfo[i].hwp_sdsID2phyType[sdsId] = RTK_PHYTYPE_NONE;
            parsedInfo->unitInfo[i].hwp_multiSdsBaseSds[sdsId] = HWP_NONE;
        }/* end for j */

        for (j = 0; j < swDescp->port.count; j++)
        {
            macId = swDescp->port.descp[j].mac_id;

            if (swDescp->port.descp[j].sds_idx == HWP_END)
                continue;

            if (!SDS_IS_MASK(swDescp->port.descp[j].sds_idx))
            {
                if (swDescp->port.descp[j].sds_idx >= swDescp->serdes.count)
                {
                    RT_ERR(RT_ERR_FAILED, MOD_INIT, "Wrong hardware profile [%s]: port %u sds_idx %d too large", p->identifier.name, macId, swDescp->port.descp[j].sds_idx);
                    return RT_ERR_FAILED;
                }
                sdsId = swDescp->serdes.descp[ swDescp->port.descp[j].sds_idx ].sds_id;

                /* init hwp_macID2SerdesID[][] */
                parsedInfo->unitInfo[i].hwp_macID2SerdesID[macId] = sdsId;
                if (parsedInfo->unitInfo[i].hwp_sdsID2MacID[sdsId] == HWP_NONE)
                {
                    parsedInfo->unitInfo[i].hwp_sdsID2MacID[sdsId] = macId;
                }

                phy_idx = swDescp->port.descp[j].phy_idx;

                if ((phy_idx != HWP_NONE)&&(phy_idx != HWP_TBD))
                {
                    if (parsedInfo->unitInfo[i].hwp_sdsID2phyType[sdsId] == RTK_PHYTYPE_NONE)
                    {
                        parsedInfo->unitInfo[i].hwp_sdsID2phyType[sdsId] = swDescp->phy.descp[phy_idx].chip;
                    }
                }
                else if ((swDescp->port.descp[j].medi != HWP_NONE) && (swDescp->port.descp[j].medi & HWP_SERDES))
                {
                    parsedInfo->unitInfo[i].hwp_sdsID2phyType[sdsId] = RTK_PHYTYPE_SERDES;
                }
            }
            else
            {
                /* port with multi serdes process */
                uint32      sdsIdxMsk;
                uint8       baseSdsId = HWP_NONE;

                sdsIdxMsk = swDescp->port.descp[j].sds_idx;
                for (sds_idx = 0; sds_idx < SDS_MASK_BIT; sds_idx++)
                {
                    if (sds_idx >= RTK_MAX_SDS_PER_UNIT)
                    {
                        break;
                    }

                    if (!SDS_IS_BIT_SET(sdsIdxMsk, sds_idx))
                        continue;

                    if (sds_idx >= swDescp->serdes.count)
                    {
                        RT_ERR(RT_ERR_FAILED, MOD_INIT, "Wrong hardware profile [%s]: port %u sds_idx %d too large", p->identifier.name, macId, sds_idx);
                        continue;
                    }
                    /* get serdes-id */
                    sdsId = swDescp->serdes.descp[ sds_idx ].sds_id;
                    if (baseSdsId == HWP_NONE)
                        baseSdsId = sdsId;

                    parsedInfo->unitInfo[i].hwp_multiSdsPortSdsmask[macId] |= SDS_BIT(sdsId);
                    parsedInfo->unitInfo[i].hwp_multiSdsBaseSds[sdsId] = baseSdsId;
                    if (parsedInfo->unitInfo[i].hwp_sdsID2MacID[sdsId] == HWP_NONE)
                    {
                        parsedInfo->unitInfo[i].hwp_sdsID2MacID[sdsId] = macId;
                    }

                    phy_idx = swDescp->port.descp[j].phy_idx;

                    if ((phy_idx != HWP_NONE)&&(phy_idx != HWP_TBD))
                    {
                        if (parsedInfo->unitInfo[i].hwp_sdsID2phyType[sdsId] == RTK_PHYTYPE_NONE)
                        {
                            parsedInfo->unitInfo[i].hwp_sdsID2phyType[sdsId] = swDescp->phy.descp[phy_idx].chip;
                        }
                    }
                    else if ((swDescp->port.descp[j].medi != HWP_NONE) && (swDescp->port.descp[j].medi & HWP_SERDES))
                    {
                        parsedInfo->unitInfo[i].hwp_sdsID2phyType[sdsId] = RTK_PHYTYPE_SERDES;
                    }

                }/* end for sds_idx */
                parsedInfo->unitInfo[i].hwp_macID2SerdesID[macId] = baseSdsId;
            }/* end if-else */

        }/* end for j */

    }/* end for i */

    return RT_ERR_OK;
}


/* Function Name:
 *      _hwp_led_parse
 * Description:
 *      Initial led related information
 * Input:
 *      None
 * Output:
 *      hwp_hwProfile_t     - Pointer of profile description struct.
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32
_hwp_led_parse(hwp_parsedInfo_t *parsedInfo)
{
    hwp_hwProfile_t *p = parsedInfo->hwp;
    uint8           led_def = 0;
    uint32          i;
    uint32          j;
    uint32          led_idx;
    uint32          ledNum_chk = 0;
    uint32          flag = 0;
#if defined(CONFIG_SDK_RTL8380)
    uint32          minExistPort24_27 = 0;
#endif
    hwp_swDescp_t  *swDescp;


    for(i = 0; i < (p->sw_count); i++)
    {
        if(p->swDescp[i] == NULL)
            continue;

        swDescp = p->swDescp[i];

        /*init led number*/
        j=0;
        while(swDescp->port.descp[j].mac_id != HWP_END)
        {
            uint32          ledNum = 0;
            hwp_portDescp_t *pd = &swDescp->port.descp[j];

            if((pd->led_c == HWP_NONE) && (pd->led_f == HWP_NONE))
            {
                parsedInfo->unitInfo[i].hwp_ledNum[swDescp->port.descp[j].mac_id] = HWP_NONE;
                j++;
                continue;
            }

            led_def = ((pd->led_c < RTK_MAX_LED_MOD) && (pd->led_c != HWP_NONE)) ? (pd->led_c) : (pd->led_f);
            for(led_idx = 0; led_idx < RTK_MAX_LED_PER_PORT; led_idx++)
            {
                if(((pd->led_c > RTK_MAX_LED_MOD) && (pd->led_c != HWP_NONE))
                    || ((pd->led_f > RTK_MAX_LED_MOD) && (pd->led_f != HWP_NONE)))
                {
                    RT_ERR(RT_ERR_FAILED, MOD_INIT, "Invalid led mode !\n");
                }

                if(swDescp->led.descp.led_definition_set[led_def].led[led_idx] != HWP_LED_END)
                    ledNum++;
                else
                    break;
            }
            if (ledNum == 0)
            {
                RT_ERR(RT_ERR_FAILED, MOD_INIT, "Error led number !\n");
            }
            parsedInfo->unitInfo[i].hwp_ledNum[swDescp->port.descp[j].mac_id] = ledNum;
            j++;
        }

#if defined(CONFIG_SDK_RTL8380)
        j=0;
        while(swDescp->port.descp[j].mac_id != HWP_END)
        {
            if(swDescp->port.descp[j].mac_id > 23 && swDescp->port.descp[j].mac_id <= 27)
            {
                minExistPort24_27 = j;
                break;
            }
            j++;
        }
#endif
        /*check hardware profile data*/
        if ((swDescp->led.descp.led_if_sel != LED_IF_SEL_SERIAL) && (swDescp->led.descp.led_active != LED_ACTIVE_HIGH))
            RT_ERR(RT_ERR_FAILED, MOD_INIT, "Error led active configuration for scan mode!\n");
        j=0;
        while(swDescp->port.descp[j].mac_id != HWP_END)
        {
            hwp_portDescp_t *pd = &swDescp->port.descp[j];

            if((parsedInfo->unitInfo[i].hwp_ledNum[swDescp->port.descp[j].mac_id] != HWP_NONE) && (flag == 0))
            {
                ledNum_chk = parsedInfo->unitInfo[i].hwp_ledNum[swDescp->port.descp[j].mac_id];
                flag = 1;
            }
            /*only rtl8380, rtl8390 not support per port led number.*/
            if ((FAMILY_ID(swDescp->chip_id) == RTL8380_FAMILY_ID) || (FAMILY_ID(swDescp->chip_id) == RTL8390_FAMILY_ID))
            {
                if (parsedInfo->unitInfo[i].hwp_ledNum[swDescp->port.descp[j].mac_id] != ledNum_chk && (parsedInfo->unitInfo[i].hwp_ledNum[swDescp->port.descp[j].mac_id] != HWP_NONE))
                {
                    RT_ERR(RT_ERR_FAILED, MOD_INIT, "Led number must be the same\n!");
                }
            }
            if ((pd->medi == HWP_COMBO) && (pd->led_layout == HWP_NONE))
            {
                RT_ERR(RT_ERR_FAILED, MOD_INIT, "Combo port should choose led layout.\n");
            }

#if defined(CONFIG_SDK_RTL8380)
            if (FAMILY_ID(swDescp->chip_id) == RTL8380_FAMILY_ID)
            {
                hwp_portDescp_t *pd_min = &swDescp->port.descp[0];
                hwp_portDescp_t *pd_min24_27 = &swDescp->port.descp[minExistPort24_27];

                /*if next exist port idx is equal to the minimum exist port idx of Mac id 24-27, reset led check number.*/
                if (minExistPort24_27 == (j + 1))
                {
                    flag = 0;
                    ledNum_chk = 0;
                }

                if((swDescp->port.descp[j].mac_id <= 23) && (pd_min->led_c != pd->led_c) && (pd->led_c != HWP_NONE))
                {
                    osal_printf("Port0~Port23 led mode must be the same!\n");
                }

                if((swDescp->port.descp[j].mac_id > 23) && (pd_min24_27->led_c != pd->led_c) && (pd->led_c != HWP_NONE))
                {
                    osal_printf("Port24~Port27 led mode must be the same!\n");
                }
            }
#endif
            j++;
        }
        flag = 0;/*reset led_num checking flag for each unit*/
    }/* end for i */

    return RT_ERR_OK;
}


/* Function Name:
 *      hwp_9300cascade_probe
 * Description:
 *      Probe if this board is 9300 cascade mode, this could be ported by customer
 * Input:
 *      hwp_hwProfile_t * - my hardware profile
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32
_hwp_cascade_parse(hwp_parsedInfo_t *parsedInfo)
{
    hwp_hwProfile_t *hwp = parsedInfo->hwp;
    uint32 i;

    for (i = 0; i < (hwp->sw_count); i++)
    {
        if (FAMILY_ID(hwp->swDescp[i]->chip_id) != RTL9300_FAMILY_ID)
        {
            parsedInfo->hwp_cascade_mode = FALSE;
            return RT_ERR_OK;
        }
    }

    if ( (hwp->sw_count ==2) && (hwp->soc.slaveInterruptPin != HWP_NONE) )
    {
        parsedInfo->hwp_cascade_mode = TRUE;

        for (i = 0; i < (hwp->sw_count); i++)
        {
            if (i != hwp->soc.swDescp_index)
            {
                parsedInfo->hwp_cascadeSlaveUnitID = i;
                break;
            }
        }
    }
    else
    {
        parsedInfo->hwp_cascade_mode = FALSE;
    }

    return RT_ERR_OK;

}


void _hwp_debug_portCnt_show(hwp_portCount_t *p, char *s)
{
    osal_printf("%s: count=%d, max=%d, min=%d\n",s,p->count,p->max,p->min);
}

/* Function Name:
 *      hwp_unit_show
 * Description:
 *      Show unit information
 * Input:
 *      unit        - not used
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32
hwp_unit_show(uint32 unit)
{
    int i;

    for(i=0;i<RTK_MAX_NUM_OF_UNIT;i++)
    {
        osal_printf("unitMapStruct[%d].pUnitInfo:%x,parsed_info=%x,swDescp=%x \n", i,(uint32)(uintptr)unitMapStruct[i].pUnitInfo,
            (uint32)(uintptr)unitMapStruct[i].pUnitInfo->parsed_info,(uint32)(uintptr)unitMapStruct[i].pUnitInfo->swDescp);
    }

    osal_printf("(&parsedInfoEmpty.unitInfo[EMPTY]=%x)\n",(uint32)(uintptr)&parsedInfoEmpty.unitInfo[HWP_LOCALUNIT_EMPTY]);

    return RT_ERR_OK;
}



/* Function Name:
 *      hwp_parsedInfo_show
 * Description:
 *      Show hardware profile parsed information debug message
 * Input:
 *      unit        - unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32
hwp_parsedInfo_show(uint32 unit)
{
    int i,j,k;
    hwp_parsedInfo_t *info = (hwp_parsedInfo_t *)unitMapStruct[unit].pUnitInfo->parsed_info;

    osal_printf("info->hwp=%s(%x)                   \n",info->hwp->identifier.name,(uint32)(uintptr)info->hwp);
    osal_printf("info->hwp_localUnitCnt=%d          \n",info->hwp_localUnitCnt);
    osal_printf("info->hwp_embeddedCpuUnitId=%d     \n",info->hwp_embeddedCpuUnitId);
    osal_printf("info->hwp_cascade_mode=%d          \n",info->hwp_cascade_mode);
    osal_printf("info->hwp_cascadeSlaveUnitID=%d    \n",info->hwp_cascadeSlaveUnitID);
    for(i=0;i<RTK_MAX_NUM_OF_UNIT_LOCAL;i++)
    {
        osal_printf("info->hwp->swDescp[%d]=%x\n",i,(uint32)(uintptr)info->hwp->swDescp[i]);
    }

    for (i=0; i<info->hwp_localUnitCnt; i++)
    {
        osal_printf("info->unitInfo[%d].hwp_chip_id=%x\n",       i,                           (uint32)info->unitInfo[i].hwp_chip_id);
        osal_printf("info->unitInfo[%d].hwp_chip_revision=%d\n", i,                           info->unitInfo[i].hwp_chip_revision);
        osal_printf("info->unitInfo[%d].hwp_chip_family_id=%x\n",i,                           (uint32)info->unitInfo[i].hwp_chip_family_id);
        osal_printf("info->unitInfo[%d].hwp_maxMacId=%d\n",      i,                           info->unitInfo[i].hwp_maxMacId);
        osal_printf("info->unitInfo[%d].hwp_minMacId=%d\n",      i,                           info->unitInfo[i].hwp_minMacId);

        osal_printf("info->unitInfo[%d].hwp_macID2PortDescp:\n",i);
        for(j=0;j<RTK_MAX_PORT_PER_UNIT;j++)
        {
            osal_printf("p[%d]=%x,", j, (uint32)(uintptr)info->unitInfo[i].hwp_macID2PortDescp[j]);
            if(info->unitInfo[i].hwp_macID2PortDescp[j])
            {
                osal_printf("mac_id=%d,",info->unitInfo[i].hwp_macID2PortDescp[j]->mac_id);
                osal_printf("phyIdx=%d,",info->unitInfo[i].hwp_macID2PortDescp[j]->phy_idx);
                if (SDS_IS_MASK(info->unitInfo[i].hwp_macID2PortDescp[j]->sds_idx))
                    osal_printf("sdsIdxMsk=0x%X,",info->unitInfo[i].hwp_macID2PortDescp[j]->sds_idx);
                else
                    osal_printf("sdsIdx=%d,",info->unitInfo[i].hwp_macID2PortDescp[j]->sds_idx);
                osal_printf("smi=%d,",info->unitInfo[i].hwp_macID2PortDescp[j]->smi);
                osal_printf("phy_addr=%d,",info->unitInfo[i].hwp_macID2PortDescp[j]->phy_addr);
                osal_printf("attr=%d,",info->unitInfo[i].hwp_macID2PortDescp[j]->attr);
                osal_printf("eth=%d,",info->unitInfo[i].hwp_macID2PortDescp[j]->eth);
                osal_printf("medi=%d,",info->unitInfo[i].hwp_macID2PortDescp[j]->medi);
                if (info->unitInfo[i].hwp_macID2PortDescp[j]->attr & HWP_SC)
                    osal_printf("scIdx=%d", info->unitInfo[i].hwp_macID2PortDescp[j]->sc_idx);

                osal_printf("\n");
            }
        }

        osal_printf("\ninfo->unitInfo[%d].hwp_macID2SerdesID:\n  ",i);
        for(j=0;j<RTK_MAX_PORT_PER_UNIT;j++)
        {
            osal_printf("p[%d]=%d,",j,info->unitInfo[i].hwp_macID2SerdesID[j]);
        }

        osal_printf("\ninfo->unitInfo[%d].hwp_portCpuMacId=%d\n",i, info->unitInfo[i].hwp_portCpuMacId);

        _hwp_debug_portCnt_show(&info->unitInfo[i].hwp_portCntEther     ,"Ether  ");
        _hwp_debug_portCnt_show(&info->unitInfo[i].hwp_portCntUplink    ,"Uplink ");
        _hwp_debug_portCnt_show(&info->unitInfo[i].hwp_portCntCascade   ,"Cascade");
        _hwp_debug_portCnt_show(&info->unitInfo[i].hwp_portCntFe        ,"Fe     ");
        _hwp_debug_portCnt_show(&info->unitInfo[i].hwp_portCntGe        ,"Ge     ");
        _hwp_debug_portCnt_show(&info->unitInfo[i].hwp_portCntXge       ,"Xge    ");
        _hwp_debug_portCnt_show(&info->unitInfo[i].hwp_portCntSXge      ,"SXge   ");
        _hwp_debug_portCnt_show(&info->unitInfo[i].hwp_portCntCopper    ,"Copper ");
        _hwp_debug_portCnt_show(&info->unitInfo[i].hwp_portCntFiber     ,"Fiber  ");
        _hwp_debug_portCnt_show(&info->unitInfo[i].hwp_portCntCombo     ,"Combo  ");
        _hwp_debug_portCnt_show(&info->unitInfo[i].hwp_portCntSerdes    ,"Serdes ");

        osal_printf("info->unitInfo[%d].hwp_sdsID2phyType:\n  ",i);
        for(j = 0; j < RTK_MAX_SDS_PER_UNIT; j++)
        {
            if (info->unitInfo[i].hwp_sdsID2phyType[j] != RTK_PHYTYPE_NONE)
                osal_printf("SDS %2d PHY:%u\n", j, info->unitInfo[i].hwp_sdsID2phyType[j]);
        }

        osal_printf("info->unitInfo[%d].hwp_sdsID2MacID:\n  ",i);
        for(j = 0; j < RTK_MAX_SDS_PER_UNIT; j++)
        {
            if (info->unitInfo[i].hwp_sdsID2MacID[j] != HWP_NONE)
                osal_printf("s[%d]=%u,", j, info->unitInfo[i].hwp_sdsID2MacID[j]);
        }
        osal_printf("\n");

        osal_printf("info->unitInfo[%d].hwp_attriPortMask:\n  ",i);
        for(j=0;j<HWP_ATTRI_ID_END;j++)
            for (k = 0; k < RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST; k++)
                osal_printf("attr[%d]=%x\n",j,info->unitInfo[i].hwp_attriPortMask[j].bits[k]);

        osal_printf("\ninfo->unitInfo[%d].hwp_ethTypePortMask:\n  ",i);
        for(j=0;j<HWP_ETHTYPE_ID_END;j++)
            for (k = 0; k < RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST; k++)
                osal_printf("eth[%d]=%x\n",j,info->unitInfo[i].hwp_ethTypePortMask[j].bits[k]);

        osal_printf("\ninfo->unitInfo[%d].hwp_mediumPortMask:\n  ",i);
        for(j=0;j<HWP_MEDIUM_ID_END;j++)
            for (k = 0; k < RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST; k++)
                osal_printf("med[%d]=%x\n",j,info->unitInfo[i].hwp_mediumPortMask[j].bits[k]);
        for (k = 0; k < RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST; k++)
            osal_printf("\ninfo->unitInfo[%d].hwp_allPortMask=%x\n",i,(uint32)info->unitInfo[i].hwp_allPortMask.bits[k]);

        osal_printf("info->unitInfo[%d].hwp_macID2phyAddr:\n  ",i);
        for(j=0;j<RTK_MAX_PORT_PER_UNIT;j++)
            osal_printf("p[%d]=%d,",j,info->unitInfo[i].hwp_macID2phyAddr[j]);

        osal_printf("\ninfo->unitInfo[%d].hwp_macID2phyType:\n  ",i);
        for(j=0;j<RTK_MAX_PORT_PER_UNIT;j++)
            osal_printf("p[%d]=%d,",j,info->unitInfo[i].hwp_macID2phyType[j]);

        osal_printf("\ninfo->unitInfo[%d].hwp_sdsID2SerdesDescp:\n  ",i);
        for(j=0;j<RTK_MAX_SDS_PER_UNIT;j++)
            osal_printf("sds[%d]=%x,",j,(uint32)(uintptr)info->unitInfo[i].hwp_sdsID2SerdesDescp[j]);

        osal_printf("\ninfo->unitInfo[%d].hwp_phyBaseMacID:\n  ",i);
        for(j=0;j<RTK_MAX_PORT_PER_UNIT;j++)
            osal_printf("p[%d]=%d,",j,info->unitInfo[i].hwp_phyBaseMacID[j]);

        osal_printf("\ninfo->unitInfo[%d].hwp_ledNum:\n  ",i);
        for(j=0;j<RTK_MAX_PORT_PER_UNIT;j++)
            osal_printf("p[%d]=%d,",j,info->unitInfo[i].hwp_ledNum[j]);

        osal_printf("\ninfo->unitInfo[%d].hwp_multiSdsPortSdsmask:\n",i);
        for(j=0;j<RTK_MAX_PORT_PER_UNIT;j++)
        {
            if (info->unitInfo[i].hwp_multiSdsPortSdsmask[j] == 0)
                continue;
            osal_printf(" p[%d]=0x%08X,", j, info->unitInfo[i].hwp_multiSdsPortSdsmask[j]);
        }

        osal_printf("\ninfo->unitInfo[%d].hwp_multiSdsBaseSds:\n",i);
        for(j=0;j<RTK_MAX_SDS_PER_UNIT;j++)
        {
            if (info->unitInfo[i].hwp_multiSdsBaseSds[j] == HWP_NONE)
                continue;
            osal_printf(" sds[%d]=%u,", j, info->unitInfo[i].hwp_multiSdsBaseSds[j]);
        }

        osal_printf("\ninfo->unitInfo[%d].hwp_macID2SCDescp:\n",i);
        for(j=0;j<RTK_MAX_PORT_PER_UNIT;j++)
        {
            if (info->unitInfo[i].hwp_macID2SCDescp[j] == &scDescpEmpty)
                continue;
            osal_printf(" mac[%d]=%x,", j, (uint32)(uintptr)info->unitInfo[i].hwp_macID2SCDescp[j]);
        }

        osal_printf("\ninfo->unitInfo[%d].hwp_scBaseMacID:\n",i);
        for(j=0;j<RTK_MAX_PORT_PER_UNIT;j++)
        {
            if (info->unitInfo[i].hwp_scBaseMacID[j] == HWP_NONE)
                continue;
            osal_printf(" mac[%d]=%u,", j, info->unitInfo[i].hwp_scBaseMacID[j]);
        }

        osal_printf("\ninfo->unitInfo[%d].swDescp=%x\n",i, (uint32)(uintptr)info->unitInfo[i].swDescp);
        osal_printf("info->unitInfo[%d].parsed_info=%x\n",i, (uint32)(uintptr)info->unitInfo[i].parsed_info);
    }

    osal_printf("\n\n");

    return RT_ERR_OK;

}

/*
 * Phy type to name
 */
uint8 *
_phyType_name(phy_type_t type)
{
    uint8 *pPhyName[] =
    {
        RTK_PHYTYPE_STRING
    };

    if (type > RTK_PHYTYPE_END)
        return (uint8*)"UNKNOWN";

    return pPhyName[type];
}


/* Function Name:
 *      _hwp_infoPortCntPortMask_show
 * Description:
 *      Show the hardware profile port information
 * Input:
 *      name        - port type name
 *      attr_str    - attribute string
 *      pCnt        - port count
 *      pMask       - port mask
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
_hwp_infoPortCntPortMask_show(char *name, char *attr_str, hwp_portCount_t *pCnt, rtk_portmask_t *pMask)
{
    int     i, hasPort = 0;

    RTK_PORTMASK_FOR_EACH_WORDS(i)
    {
        if ((*pMask).bits[i] != 0)
        {
            hasPort = 1;
            break;
        }
    }

    if ((pCnt->count != 0) || (hasPort == 1))
    {
        RT_INIT_MSG("          %-7s cnt:%2d, macId(m/M):%2d/%2d msk:"PMSK_FMT" (%s)\n", name,  pCnt->count, pCnt->min, pCnt->max, PMSK_ARG(*pMask), attr_str);
    }
}


/* Function Name:
 *      hwp_info_show
 * Description:
 *      Show the hardware profile information
 * Input:
 *      target_unit - show the profile infomration of this unit
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
hwp_info_show(uint32 target_unit)
{
    uint32 unit,port,i,j;
    hwp_parsedInfo_t *parsedInfo = unitMapStruct[target_unit].pUnitInfo->parsed_info;
    hwp_hwProfile_t *hardwareProfile = parsedInfo->hwp;

    if(NULL == hardwareProfile)
    {
        RT_ERR(RT_ERR_FAILED, MOD_INIT, "No Hardware profile exist.\n");
        return RT_ERR_FAILED;
    }

    RT_INIT_MSG("    Profile Information (%X):\n", (uint32)(uintptr)hardwareProfile);
    RT_INIT_MSG("      Profile-Name: %s\n", hardwareProfile->identifier.name);
    RT_INIT_MSG("      Profile-ID: %d\n",hardwareProfile->identifier.id);
    RT_INIT_MSG("      Profile-ID-Type: %d\n",hardwareProfile->identifier.type);
    RT_INIT_MSG("      CPU Type: %s\n", (HWP_CPU_EMBEDDED())?"Embedded":"External");
    RT_INIT_MSG("      SW Desc IDX: %u\n", hardwareProfile->soc.swDescp_index);
    RT_INIT_MSG("      Slave Interrupt Pin: %u\n", hardwareProfile->soc.slaveInterruptPin);
    RT_INIT_MSG("      Chip count: %d\n", hardwareProfile->sw_count);
    RT_INIT_MSG("      Cascade Mode: %s\n", parsedInfo->hwp_cascade_mode?"Yes":"No" );
    HWP_UNIT_TRAVS_LOCAL(unit)
    {
        RT_INIT_MSG("        unit %2d, %X(%d) %s\n",unit,RT_CHIP_ID_DISPLAY(HWP_CHIP_ID(unit)),HWP_CHIP_REV(unit),((HWP_IS_CPU_UNIT(unit))?"(cpu)":""));
        RT_INIT_MSG("          swcore support: %d\n",HWP_SWCORE_SUPPORT(unit));
        if(!HWP_SWCORE_SUPPORT(unit))
            continue;
        RT_INIT_MSG("          swcore-CPU Interface(1:normal, 2:SPI, 3: PCIe): %d\n",HWP_SWCORE_ACC_METHOD(unit));
        RT_INIT_MSG("          nic support: %d\n", unitMapStruct[unit].pUnitInfo->swDescp->nic_supported);
        RT_INIT_MSG("          port count:%2d\n",HWP_PORT_COUNT(unit));
        /* show all macID */
        RT_INIT_MSG("          macId:");
        i=0;
        HWP_PORT_TRAVS(unit,port){
            RT_INIT_MSG("%2d,",port);
            if(++i%16==0)
                RT_INIT_MSG("\n                ");
        }
        RT_INIT_MSG("\n");

        RT_INIT_MSG("          CpuMacId: %d  (attr)\n", unitMapStruct[unit].pUnitInfo->hwp_portCpuMacId);
        _hwp_infoPortCntPortMask_show("Ether",      "A", &(unitMapStruct[unit].pUnitInfo->hwp_portCntEther),    &(unitMapStruct[unit].pUnitInfo->hwp_attriPortMask[HWP_ETH_ID]));
        _hwp_infoPortCntPortMask_show("Uplink",     "A", &(unitMapStruct[unit].pUnitInfo->hwp_portCntUplink),   &(unitMapStruct[unit].pUnitInfo->hwp_attriPortMask[HWP_UPLINK_ID]));
        _hwp_infoPortCntPortMask_show("Cascade",    "A", &(unitMapStruct[unit].pUnitInfo->hwp_portCntCascade),  &(unitMapStruct[unit].pUnitInfo->hwp_attriPortMask[HWP_CASCADE_ID]));
        _hwp_infoPortCntPortMask_show("FE",         "E", &(unitMapStruct[unit].pUnitInfo->hwp_portCntFe),       &(unitMapStruct[unit].pUnitInfo->hwp_ethTypePortMask[HWP_FE_ID]));
        _hwp_infoPortCntPortMask_show("GE",         "E", &(unitMapStruct[unit].pUnitInfo->hwp_portCntGe),       &(unitMapStruct[unit].pUnitInfo->hwp_ethTypePortMask[HWP_GE_ID]));
        _hwp_infoPortCntPortMask_show("2_5GE",      "E", &(unitMapStruct[unit].pUnitInfo->hwp_portCnt2_5ge),    &(unitMapStruct[unit].pUnitInfo->hwp_ethTypePortMask[HWP_2_5GE_ID]));
        _hwp_infoPortCntPortMask_show("XGE",        "E", &(unitMapStruct[unit].pUnitInfo->hwp_portCntXge),      &(unitMapStruct[unit].pUnitInfo->hwp_ethTypePortMask[HWP_XGE_ID]));
        _hwp_infoPortCntPortMask_show("SXGE",       "E", &(unitMapStruct[unit].pUnitInfo->hwp_portCntSXge),     &(unitMapStruct[unit].pUnitInfo->hwp_ethTypePortMask[HWP_SXGE_ID]));
        _hwp_infoPortCntPortMask_show("Copper",     "M", &(unitMapStruct[unit].pUnitInfo->hwp_portCntCopper),   &(unitMapStruct[unit].pUnitInfo->hwp_mediumPortMask[HWP_COPPER_ID]));
        _hwp_infoPortCntPortMask_show("Fiber",      "M", &(unitMapStruct[unit].pUnitInfo->hwp_portCntFiber),    &(unitMapStruct[unit].pUnitInfo->hwp_mediumPortMask[HWP_FIBER_ID]));
        _hwp_infoPortCntPortMask_show("Combo",      "M", &(unitMapStruct[unit].pUnitInfo->hwp_portCntCombo),    &(unitMapStruct[unit].pUnitInfo->hwp_mediumPortMask[HWP_COMBO_ID]));
        _hwp_infoPortCntPortMask_show("Serdes",     "M", &(unitMapStruct[unit].pUnitInfo->hwp_portCntSerdes),   &(unitMapStruct[unit].pUnitInfo->hwp_mediumPortMask[HWP_SERDES_ID]));

        for (i = 0; i < unitMapStruct[unit].pUnitInfo->swDescp->port.count; i++)
        {
            RT_INIT_MSG("          port:%2d, phyIdx%u\n", unitMapStruct[unit].pUnitInfo->swDescp->port.descp[i].mac_id, unitMapStruct[unit].pUnitInfo->swDescp->port.descp[i].phy_idx);
        }
        RT_INIT_MSG("\n");


        RT_INIT_MSG("          PHY Count:%u\n", (uint32)unitMapStruct[unit].pUnitInfo->swDescp->phy.count);
        for (i = 0; i < unitMapStruct[unit].pUnitInfo->swDescp->phy.count; i++)
        {
            RT_INIT_MSG("            type:%02u(%-12s) port:%u baseMac:%u\n", unitMapStruct[unit].pUnitInfo->swDescp->phy.descp[i].chip,
                                        _phyType_name((phy_type_t)unitMapStruct[unit].pUnitInfo->swDescp->phy.descp[i].chip),
                                        unitMapStruct[unit].pUnitInfo->swDescp->phy.descp[i].phy_max,
                                        unitMapStruct[unit].pUnitInfo->swDescp->phy.descp[i].mac_id);
        }

        j=0;
        while(unitMapStruct[unit].pUnitInfo->swDescp->phy.descp[j].chip != HWP_END)
        {
            RT_INIT_MSG("          PHY[%d]: chipID %2d, baseMacId %2d, phy_max %2d \n",j,
            unitMapStruct[unit].pUnitInfo->swDescp->phy.descp[j].chip,
            unitMapStruct[unit].pUnitInfo->swDescp->phy.descp[j].mac_id,
            unitMapStruct[unit].pUnitInfo->swDescp->phy.descp[j].phy_max);
            j++;
        }

        /* dump SC info */
        {
            uint8   sc_idx = HWP_NONE, prev_sc_idx = HWP_NONE;
            for (i = 0; i < unitMapStruct[unit].pUnitInfo->swDescp->port.count; i++)
            {
                if ((unitMapStruct[unit].pUnitInfo->swDescp->port.descp[i].attr & HWP_SC) && (sc_idx = unitMapStruct[unit].pUnitInfo->swDescp->port.descp[i].sc_idx) < RTK_MAX_SC_PER_UNIT)
                {
                    if (sc_idx != prev_sc_idx)
                    {
                        RT_INIT_MSG("          SC[%d]: chipID %2d, smi %2d, addr %2d \n", sc_idx,
                            unitMapStruct[unit].pUnitInfo->swDescp->sc.descp[sc_idx].chip, unitMapStruct[unit].pUnitInfo->swDescp->sc.descp[sc_idx].smi, unitMapStruct[unit].pUnitInfo->swDescp->sc.descp[sc_idx].phy_addr);
                        prev_sc_idx = sc_idx;
                    }
                }
            }/* end for */
        }

    }

    return RT_ERR_OK;

}


/* Function Name:
 *      _hwp_info_parse
 * Description:
 *      Parse hardware profile and store information in hwp_parsedInfo_t
 * Input:
 *      hwp_parsedInfo_t *parsedInfo - db to store parsed information, and p->hwp must be given
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Init ok
 *      RT_ERR_FAILED   - Init failed
 * Note:
 *      None
 */
int32
_hwp_info_parse(hwp_parsedInfo_t *parsedInfo)
{

    if (_hwp_chip_parse(parsedInfo) != RT_ERR_OK)
        return RT_ERR_FAILED;

    if (_hwp_port_parse(parsedInfo) != RT_ERR_OK)
        return RT_ERR_FAILED;

    if (_hwp_serdes_parse(parsedInfo) != RT_ERR_OK)
        return RT_ERR_FAILED;

    if (_hwp_led_parse(parsedInfo) != RT_ERR_OK)
        return RT_ERR_FAILED;

    if (_hwp_cascade_parse(parsedInfo) != RT_ERR_OK)
        return RT_ERR_FAILED;

    return RT_ERR_OK;

}


/* Function Name:
 *      hwp_parsedInfo_buildup
 * Description:
 *      Build up a new hwp_parsedInfo_t from a hardware profile
 * Input:
 *      hwp_hwProfile_t     - which hardware profile will be build
 * Output:
 *      hwp_parsedInfo_t *  - return the result
 * Return:
 *      RT_ERR_OK       - if success
 *      RT_ERR_FAILED   - if failed
 * Note:
 *      None
 */
int32
hwp_parsedInfo_buildup(hwp_parsedInfo_t **p, hwp_hwProfile_t *hwp)
{
    hwp_parsedInfo_t *info=NULL;

    if (&parsedInfoEmpty == *p)
    {
        if (_hwp_parsedInfo_allocate(&info) != RT_ERR_OK)
            return RT_ERR_FAILED;

        if (NULL == info)
        {
            RT_ERR(RT_ERR_FAILED, MOD_INIT, "get hwp free parsedInfo failed!");
            return RT_ERR_FAILED;
        }
        *p = info;
    }
    else
    {
        info = *p;
    }

    info->hwp = hwp;
    hwp->parsed_info = (void *)info;
    if (_hwp_info_parse(info) != RT_ERR_OK)
    {
        RT_ERR(RT_ERR_FAILED, MOD_INIT, "hwp info parse fail");
    }

    return RT_ERR_OK;
}

int32
hwp_unitID_allocate(int if_local, int local_unit_index, uint32 *unit)
{
    int i;

    if(if_local==TRUE)
    {
        *unit = local_unit_index;
        hwp_unit_pool[*unit] = TRUE;
        return RT_ERR_OK;
    }

    for(i=0;i<RTK_MAX_NUM_OF_UNIT;i++)
    {
        if(hwp_unit_pool[i]==TRUE)
            continue;

        *unit = i;
        hwp_unit_pool[i] = TRUE;
        return RT_ERR_OK;
    }

    return RT_ERR_FAILED;

}


/* Function Name:
 *      hwp_unit_attach
 * Description:
 *      Add unit into system
 * Input:
 *      unit                - the new unit ID
 *      hwp                 - the point of hardware profile of this board
 *      local_unit_index    - the unit ID(index) of this chip on this hwp
 *      if_local            - TRUE: local hardware board, FALSE: remote stacking board
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
hwp_unit_attach(uint32 unit, hwp_hwProfile_t *hwp, int local_unit_index, uint32 if_local)
{

    /* parsing hardware profile */
    if(hwp->parsed_info == (void *)&parsedInfoEmpty)
    {
        hwp_parsedInfo_buildup((hwp_parsedInfo_t **)&hwp->parsed_info, hwp);
    }

    /* map unit */
    HWP_SEM_LOCK();

    /* allocate by upper layer application, so remark */
    //if(hwp_unitID_allocate(if_local, local_unit_index, unit) != RT_ERR_OK)
    //{
    //    HWP_SEM_UNLOCK();
    //    return RT_ERR_FAILED;
    //}

    unitMapStruct[unit].pUnitInfo = &((hwp_parsedInfo_t *)hwp->parsed_info)->unitInfo[local_unit_index];
    if(if_local==TRUE)
        unitMapStruct[unit].flag |= HWP_UNITMAPFLAG_LOCALCHIP;

    HWP_SEM_UNLOCK();

    return RT_ERR_OK;
}


/* Function Name:
 *      hwp_unit_detach
 * Description:
 *      Remove a unit from hardware profile database. Local unit should not be deleted.
 * Input:
 *      unit        - unit to delete
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
hwp_unit_detach(uint32 unit)
{

    HWP_SEM_LOCK();

    unitMapStruct[unit].pUnitInfo = &parsedInfoEmpty.unitInfo[HWP_LOCALUNIT_EMPTY];
    //hwp_unit_pool[unit] = FALSE;

    HWP_SEM_UNLOCK();

    return RT_ERR_OK;
}

/* Function Name:
 *      hwp_init_sys
 * Description:
 *      Initialize hwp common parts
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Init ok
 *      RT_ERR_FAILED   - Init failed
 * Note:
 *      None
 */
int32
hwp_init_sys(void)
{
    int i;

    if(hwp_init_sys_status == INIT_COMPLETED) //init only once
        return RT_ERR_OK;

    RT_INIT_MSG("  Hardware-profile init\n");

    if (hwp_myHwProfile == NULL)
    {
        RT_ERR(RT_ERR_FAILED, MOD_INIT, "Hardware profile not exist!");
        return RT_ERR_FAILED;
    }

    /* create semaphore */
    hwp_unit_sem = osal_sem_mutex_create();
    if (0 == hwp_unit_sem)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_INIT), "semaphore create failed!");
        return RT_ERR_FAILED;
    }

    /* init database */
    for(i=0; i<RTK_MAX_NUM_OF_UNIT; i++)
    {
        unitMapStruct[i].pUnitInfo = &parsedInfoEmpty.unitInfo[HWP_LOCALUNIT_EMPTY];
        unitMapStruct[i].flag = 0;
    }

    _hwp_emptyDB_init();

    hwp_init_sys_status = INIT_COMPLETED;

    return RT_ERR_OK;
}


/* Function Name:
 *      hwp_init
 * Description:
 *      Initialize hardware profile related variables
 * Input:
 *      hwp             - hardware profile, NULL if this is local board init
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Init ok
 *      RT_ERR_FAILED   - Init failed
 * Note:
 *      None
 */
int32
hwp_init(hwp_hwProfile_t *hwp)
{
    uint32 i, unit, if_local_board=FALSE;

    hwp_init_sys();

    if(hwp==NULL)
    {
        if_local_board = TRUE;
        hwp = hwp_myHwProfile;
    }

    for(i=0;i<hwp->sw_count;i++)
    {
        unit = i;

        if(hwp_unit_attach(unit, hwp, i, if_local_board) != RT_ERR_OK)
        {
            return RT_ERR_FAILED;
        }
    }


    hwp_init_status = INIT_COMPLETED;

    return RT_ERR_OK;
}



/* Function Name:
 *      hwp_profilePhy_del
 * Description:
 *      Delete PHY from hardware profile
 * Input:
 *      unit        - unit ID
 *      delMsk      - port mask for PHY delete
 * Output:
 *      None
 * Return:
 *      int32       - actual updated(deleted) number of ports to the hardware profile
 * Note:
 *      None
 */
int32
hwp_profilePhy_del(uint32 unit, rtk_portmask_t *delMsk)
{
    hwp_unitInfo_t      *unitInfo;
    hwp_swDescp_t       *sw;
    uint32              macId;
    int                 j, phy_idx, delCnt = 0;

    if ((unitInfo = unitMapStruct[unit].pUnitInfo) == NULL)
    {
        return 0;
    }

    if ((sw = unitInfo->swDescp) == NULL)
    {
        return 0;
    }

    /* set PHY descp .chip as RTK_PHYTYPE_UNKNOWN */
    for(j = 0 ; j < RTK_MAX_PHY_PER_UNIT; j++)
    {
        if (sw->phy.descp[j].chip == HWP_END)
        {
            break;
        }

        macId = sw->phy.descp[j].mac_id;
        if (RTK_PORTMASK_IS_PORT_SET((*delMsk), macId))
        {
            sw->phy.descp[j].chip = RTK_PHYTYPE_UNKNOWN;

            /* when base port of the PHY is in delete list, then set all ports of the PHY to delete list for remove */
            macId++;
            for ( ; macId < (sw->phy.descp[j].mac_id + sw->phy.descp[j].phy_max); macId++)
            {
                RTK_PORTMASK_PORT_SET((*delMsk), macId);
            }
        }
    }/* end for */

    /* remove phy_idx from the port */
    for(j = 0 ; j < sw->port.count ; j++)
    {
        macId = sw->port.descp[j].mac_id;

        if(macId >= RTK_MAX_PORT_ID_PER_UNIT)
        {
            continue;
        }

        phy_idx = sw->port.descp[j].phy_idx;

        if ((phy_idx == HWP_NONE)||(phy_idx == HWP_TBD))
            continue;

        if (RTK_PORTMASK_IS_PORT_SET((*delMsk), macId))
        {
            sw->port.descp[j].phy_idx = HWP_NONE;
            sw->port.descp[j].medi    = HWP_NONE;
            delCnt++;
        }
    } /* end for */

    return delCnt;

}




/* Function Name:
 *      hwp_profilePhy_update
 * Description:
 *      Update PHY type from hardware profile.
 * Input:
 *      unit        - unit ID
 *      baseport    - base mac ID of the PHY
 *      phyType     - new PHY type
 * Output:
 *      None
 * Return:
 *      int32       - update count
 * Note:
 *      None
 */
int32
hwp_profilePhy_update(uint32 unit, uint32 baseport, uint32 phyType)
{
    hwp_unitInfo_t      *unitInfo;
    hwp_swDescp_t       *sw;
    uint32              macId;
    int                 j, phy_idx, updateCnt = 0;

    if ((unitInfo = unitMapStruct[unit].pUnitInfo) == NULL)
    {
        return 0;
    }

    if ((sw = unitInfo->swDescp) == NULL)
    {
        return 0;
    }

    /* remove phy_idx from the port */
    for(j = 0 ; j < sw->port.count ; j++)
    {
        macId = sw->port.descp[j].mac_id;

        if(macId != baseport)
        {
            continue;
        }

        if ((phy_idx = sw->port.descp[j].phy_idx) == HWP_NONE)
            break;

        sw->phy.descp[phy_idx].chip = phyType;
        updateCnt++;
        break;
    } /* end for */

    return updateCnt;
}


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
#include <osal/lib.h>
#include <osal/print.h>
#include <hwp/hw_profile.h>
#include <common/util/rt_util_system.h>
#include <ioal/ioal_init.h>
#include <ioal/ioal_param.h>
#include <hal/chipdef/chipdef.h>
#include <hal/chipdef/chip.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>

#ifdef RTK_HWP_DEFAULT_PROFILE
#include <hwp/hw_profiles/rtl833x_default.c>
#include <hwp/hw_profiles/rtl838x_default.c>
#include <hwp/hw_profiles/rtl835x_default.c>
#include <hwp/hw_profiles/rtl839x_default.c>
#include <hwp/hw_profiles/rtl9301_default.c>
#include <hwp/hw_profiles/rtl9303_default.c>
#endif /* end RTK_HWP_DEFAULT_PROFILE */

/*
 * Symbol Definition
 */
#define HWP_FORCE_PROFILE_STR_DEFAULT       "Default None"
#define HWP_FORCE_PROFILE_ID_DEFAULT        0
//#define HWP_FORCE_PROFILE_SAMPLE_CODE

#define HWP_DEFAULT_PROFILE_STR             "default"
#define HWP_DEFAULT_PROFILE_ID              999



/*
 * Data Type Definition
 */
extern hwp_hwProfile_t      *hwp_hwProfile_list[];
extern hwp_parsedInfo_t     parsedInfoEmpty;

static hwp_identifier_t     hwp_forcedHwp =
{
    .type   = HWP_ID_TYPE_END,
    .name   = HWP_FORCE_PROFILE_STR_DEFAULT,
    .id     = HWP_FORCE_PROFILE_ID_DEFAULT,
};

static hwp_identifier_t     hwp_defaultHwp =
{
    .type   = HWP_ID_TYPE_END,
    .name   = HWP_DEFAULT_PROFILE_STR,
    .id     = HWP_DEFAULT_PROFILE_ID,
};


uint32                      hwp_useDefHwp = FALSE;

/*
 * Function Declaration
 */



/* Function Name:
 *      hwp_forcedProfile_get
 * Description:
 *      Check if force hardware profile
 * Input:
 *      None
 * Output:
 *      hwp_identifier_t *identifier
 * Return:
 *      RT_ERR_OK       - Got
 *      RT_ERR_FAILED   - Auto probe
 * Note:
 *      None
 */
int32
hwp_forcedProfile_get(hwp_identifier_t *identifier)
{

    if(HWP_ID_TYPE_STRING == hwp_forcedHwp.type)
    {
        RT_INIT_MSG("    (Forced profile: %s)\n",hwp_forcedHwp.name);
    }
    else if(HWP_ID_TYPE_INTEGER == hwp_forcedHwp.type)
    {
        RT_INIT_MSG("    (Forced profile ID: %d)\n",hwp_forcedHwp.id);
    }
    else
    {
        return RT_ERR_FAILED;
    }

    identifier->type = hwp_forcedHwp.type;
    osal_strncpy(identifier->name, hwp_forcedHwp.name, HWP_PROFILE_NAME_LEN_MAX);
    identifier->id = hwp_forcedHwp.id;
    return RT_ERR_OK;
}



/* Function Name:
 *      hwp_identifier_get
 * Description:
 *      Get profile identifier from system. either a string or a integer.
 *      This is used to decide what hardware board the code is running on.
 * Input:
 *      None
 * Output:
 *      hwp_identifier_t *identifier
 * Return:
 *      RT_ERR_OK       - Got
 *      RT_ERR_FAILED   - Not found
 * Note:
 *      None
 */
int32
hwp_identifier_get(hwp_identifier_t *identifier)
{
    int32 ret = RT_ERR_FAILED;
    hwp_hwProfile_t     *hwp;
    hwp_identifier_t    hwp_id_key;

    if( (RT_ERR_OK == hwp_forcedProfile_get(&hwp_id_key)) || (ioal_param_hwpIdKey_get(&hwp_id_key) != RT_ERR_OK))
    {
        return RT_ERR_FAILED;
    }

    if (hwp_profile_find(&hwp_id_key, &hwp) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }


    osal_memcpy(identifier, &hwp->identifier, sizeof(hwp_identifier_t));

    return ret;

}


/* Function Name:
 *      hwp_identifier_cmp
 * Description:
 *      Compare two identifiers. User/Customer could change the comparing method.
 *      Using string or integer type, depends on the "type" defined in id1.
 * Input:
 *      id1     -   the first hwp_identifier_t
 *      id2     -   the second hwp_identifier_t
 * Output:
 *      None
 * Return:
 *      TRUE    - same
 *      FALSE   - different
 * Note:
 *      None
 */
int32
hwp_identifier_cmp(hwp_identifier_t *id1, hwp_identifier_t *id2)
{

    if(HWP_ID_TYPE_STRING == id1->type)
    {
        if(osal_strncmp(id1->name, id2->name, HWP_PROFILE_NAME_LEN_MAX)==0)
            return TRUE;
    }

    if(HWP_ID_TYPE_INTEGER == id1->type)
    {
        if(id1->id == id2->id)
            return TRUE;
    }

    return FALSE;

}


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


int32
hwp_defaultProfilePort_update(hwp_hwProfile_t *defaultHwp, uint32 unit, uint8 mac_id, uint8 attr, uint8 medi, uint8 eth, uint8 phy_idx)
{
    hwp_swDescp_t *sw;
    int seq_num;

    if (unit >= defaultHwp->sw_count)
    {
        return RT_ERR_FAILED;
    }

    if ((sw = defaultHwp->swDescp[unit]) == NULL)
    {
        return RT_ERR_FAILED;
    }

    for(seq_num=0;seq_num<RTK_MAX_PORT_PER_UNIT;seq_num++)
    {
        if( mac_id == sw->port.descp[seq_num].mac_id)
        {
            sw->port.descp[seq_num].attr  = attr;
            sw->port.descp[seq_num].medi  = medi;
            sw->port.descp[seq_num].eth   = eth;
            sw->port.descp[seq_num].phy_idx = phy_idx;
            return RT_ERR_OK;
        }
    }
    return RT_ERR_FAILED;
}


#ifdef RTK_HWP_DEFAULT_PROFILE
/* Function Name:
 *      hwp_defaultProfilePort_build
 * Description:
 *      Build up default hardware profile - port part
 * Input:
 *      defaultHwp  - the default hardware profile that try to build port information
 *      devInfo     - the rt_device_t information of unit 0 of defaultHwp
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - build up
 *      RT_ERR_FAILED   - failed
 * Note:
 *      None
 */
int32
hwp_defaultProfilePort_build(hwp_hwProfile_t *defaultHwp, rt_device_t *devInfo)
{
    hwp_swDescp_t *sw = defaultHwp->swDescp[0];
    int port,seq_num;

    seq_num=0;
    for(port=0;port<RTK_MAX_PORT_PER_UNIT;port++)
    {
        switch( devInfo->pPortinfo->portType[port])
        {
            case RT_FE_PORT:
                sw->port.descp[seq_num].attr  = HWP_ETH;
                sw->port.descp[seq_num].medi  = HWP_COPPER;
                sw->port.descp[seq_num].eth   = HWP_FE;
                break;
            case RT_GE_PORT:
                sw->port.descp[seq_num].attr  = HWP_ETH;
                sw->port.descp[seq_num].medi  = HWP_COPPER;
                sw->port.descp[seq_num].eth   = HWP_GE;
                break;
            case RT_GE_COMBO_PORT:
                sw->port.descp[seq_num].attr  = HWP_ETH;
                sw->port.descp[seq_num].medi  = HWP_COMBO;
                sw->port.descp[seq_num].eth   = HWP_GE;
                break;
            case RT_GE_SERDES_PORT:
                sw->port.descp[seq_num].attr  = HWP_ETH;
                sw->port.descp[seq_num].medi  = HWP_SERDES;
                sw->port.descp[seq_num].eth   = HWP_GE;
                break;
            case RT_2_5GE_PORT:
                sw->port.descp[seq_num].attr  = HWP_ETH;
                sw->port.descp[seq_num].medi  = HWP_COPPER;
                sw->port.descp[seq_num].eth   = HWP_2_5GE;
                break;
            case RT_2_5GE_FIBER_PORT:
                sw->port.descp[seq_num].attr  = HWP_ETH;
                sw->port.descp[seq_num].medi  = HWP_FIBER;
                sw->port.descp[seq_num].eth   = HWP_2_5GE;
                break;
            case RT_2_5GE_SERDES_PORT:
                sw->port.descp[seq_num].attr  = HWP_ETH;
                sw->port.descp[seq_num].medi  = HWP_SERDES;
                sw->port.descp[seq_num].eth   = HWP_2_5GE;
                break;

            case RT_5GE_PORT:
                sw->port.descp[seq_num].attr  = HWP_ETH;
                sw->port.descp[seq_num].medi  = HWP_COPPER;
                sw->port.descp[seq_num].eth   = HWP_5GE;
                break;
            case RT_5GE_FIBER_PORT:
                sw->port.descp[seq_num].attr  = HWP_ETH;
                sw->port.descp[seq_num].medi  = HWP_FIBER;
                sw->port.descp[seq_num].eth   = HWP_5GE;
                break;
            case RT_5GE_SERDES_PORT:
                sw->port.descp[seq_num].attr  = HWP_ETH;
                sw->port.descp[seq_num].medi  = HWP_SERDES;
                sw->port.descp[seq_num].eth   = HWP_5GE;
                break;

            case RT_10GE_PORT:
                sw->port.descp[seq_num].attr  = HWP_ETH;
                sw->port.descp[seq_num].medi  = HWP_COPPER;
                sw->port.descp[seq_num].eth   = HWP_XGE;
                break;
            case RT_10GE_FIBER_PORT:
                sw->port.descp[seq_num].attr  = HWP_ETH;
                sw->port.descp[seq_num].medi  = HWP_FIBER;
                sw->port.descp[seq_num].eth   = HWP_XGE;
                break;
            case RT_10GE_SERDES_PORT:
                sw->port.descp[seq_num].attr  = HWP_ETH;
                sw->port.descp[seq_num].medi  = HWP_SERDES;
                sw->port.descp[seq_num].eth   = HWP_XGE;
                break;
            case RT_CPU_PORT:
                sw->port.descp[seq_num].attr  = HWP_CPU;
                sw->port.descp[seq_num].medi  = HWP_NONE;
                sw->port.descp[seq_num].eth   = HWP_NONE;
                break;
            case RT_INT_FE_PORT:
                sw->port.descp[seq_num].attr  = HWP_ETH;
                sw->port.descp[seq_num].medi  = HWP_COPPER;
                sw->port.descp[seq_num].eth   = HWP_FE;
                break;
            case RT_PORT_NONE:
            case RT_PORT_TYPE_END:
            default:
                continue;
        }

        sw->port.descp[seq_num].mac_id = port;

        sw->port.descp[seq_num].phy_addr = sw->port.descp[seq_num].mac_id;
        seq_num++;
        //sw->port.descp[seq_num].smi
    }

    sw->port.descp[seq_num].mac_id = HWP_END;

    return RT_ERR_OK;

}
#endif /* RTK_HWP_DEFAULT_PROFILE */


#ifdef RTK_HWP_DEFAULT_PROFILE
/* Function Name:
 *      hwp_defaultProfilePhy_build
 * Description:
 *      Build up default hardware profile - PHY part
 * Input:
 *      defaultHwp  - the default hardware profile that try to build port information
 *      port        - port number
 *      phyCtrl     - the control block of the phy connectted to the port
 *      phyBasePort - base port of this phy
 *      phyIndex    - phy index in hardware profile
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - build up
 *      RT_ERR_FAILED   - failed
 * Note:
 *      None
 */
int32
hwp_defaultProfilePhy_build(hwp_hwProfile_t *defaultHwp, uint8 port, rt_phyctrl_t *phyCtrl, uint8 phyBasePort, int phyIndex)
{
    defaultHwp->swDescp[0]->phy.descp[phyIndex].chip = phyCtrl->phyType;
    defaultHwp->swDescp[0]->phy.descp[phyIndex].phy_max = phyCtrl->pPhyInfo->phy_num;
    defaultHwp->swDescp[0]->phy.descp[phyIndex].mac_id = phyBasePort;
    defaultHwp->swDescp[0]->phy.descp[phyIndex+1].chip = HWP_END;

    return RT_ERR_OK;
}
#endif /* RTK_HWP_DEFAULT_PROFILE */

#ifdef RTK_HWP_DEFAULT_PROFILE
/* Function Name:
 *      hwp_defaultProfile_build
 * Description:
 *      Build up default hardware profile
 * Input:
 *      None
 * Output:
 *      hwp_hwProfile_t ** p - the result
 * Return:
 *      RT_ERR_OK       - build up
 *      RT_ERR_FAILED   - failed
 * Note:
 *      None
 */
int32
hwp_defaultProfile_build(hwp_hwProfile_t ** p)
{
    uint32 chip_id, chip_rev_id;
    hwp_hwProfile_t *defaultHwp=NULL;

    ioal_init(HWP_DEFHWP_UNIT_ID);

    if( RT_ERR_OK != drv_swcore_cid_get(HWP_DEFHWP_UNIT_ID,&chip_id, &chip_rev_id) )
        return RT_ERR_FAILED;

    switch(chip_id)
    {
        case RTL8330M_CHIP_ID:
        case RTL8332M_CHIP_ID:
            defaultHwp = &rtl833x_defaultHwp;
            break;
        case RTL8380M_CHIP_ID:
        case RTL8382M_CHIP_ID:
        case RTL8381M_CHIP_ID:
            defaultHwp = &rtl838x_defaultHwp;
            break;
        case RTL8351M_CHIP_ID:
        case RTL8352M_CHIP_ID:
        case RTL8353M_CHIP_ID:
        case RTL8352MES_CHIP_ID:
        case RTL8353MES_CHIP_ID:
            defaultHwp = &rtl835x_defaultHwp;
            break;
        case RTL8390M_CHIP_ID:
        case RTL8391M_CHIP_ID:
        case RTL8392M_CHIP_ID:
        case RTL8393M_CHIP_ID:
        case RTL8396M_CHIP_ID:
        case RTL8392MES_CHIP_ID:
        case RTL8393MES_CHIP_ID:
        case RTL8396MES_CHIP_ID:
            defaultHwp = &rtl839x_defaultHwp;
            break;
        case RTL9301_CHIP_ID_24G:
        case RTL9301_CHIP_ID:
            defaultHwp = &rtl9301_defaultHwp;
            break;
        case RTL9303_CHIP_ID_8XG:
        case RTL9303_CHIP_ID:
            defaultHwp = &rtl9303_defaultHwp;
            break;
        case RTL9301H_CHIP_ID:
        case RTL9302A_CHIP_ID:
        case RTL9302B_CHIP_ID:
        case RTL9302C_CHIP_ID:
        case RTL9302D_CHIP_ID:
        case RTL9301H_CHIP_ID_4X2_5G:
        case RTL9302A_CHIP_ID_12X2_5G:
        case RTL9302B_CHIP_ID_8X2_5G:
        case RTL9302C_CHIP_ID_16X2_5G:
        case RTL9302D_CHIP_ID_24X2_5G:
        case RTL9302DE_CHIP_ID:
            break;
        case RTL9310_CHIP_ID:
            break;
        default:
            return RT_ERR_FAILED;
    }

    if (NULL == defaultHwp)
        return RT_ERR_FAILED;

    defaultHwp->soc.swDescp_index = 0;
    defaultHwp->soc.slaveInterruptPin = HWP_NONE;
    defaultHwp->sw_count = 1;
    defaultHwp->swDescp[0]->chip_id = chip_id;
    defaultHwp->swDescp[0]->swcore_supported = TRUE;
    defaultHwp->swDescp[0]->swcore_access_method = HWP_SW_ACC_MEM;
    defaultHwp->swDescp[0]->swcore_spi_chip_select = HWP_NOT_USED;
    defaultHwp->swDescp[0]->nic_supported = TRUE;

    *p = defaultHwp;
    return RT_ERR_OK;

}
#endif/* RTK_HWP_DEFAULT_PROFILE */

/* Function Name:
 *      hwp_profileList_show
 * Description:
 *      Show available hardware profile list
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 *
 * Note:
 *      None
 */
void hwp_profileList_show(void)
{
    hwp_hwProfile_t **pp_bdPro;
    int no_profile=1;

    for (pp_bdPro = hwp_hwProfile_list; *pp_bdPro != NULL; pp_bdPro++)
    {
        if(no_profile==1)
        {
            RT_INIT_MSG("      Available profiles are:\n");
            no_profile=0;
        }
        RT_INIT_MSG("        %s (%d)\n", (*pp_bdPro)->identifier.name,(*pp_bdPro)->identifier.id);
    }

    if(no_profile==1)
        RT_INIT_MSG("      (No any profile built in the image)\n");
    else
        RT_INIT_MSG("      (uboot: setenv boardmodel 'YOUR_HARDWARE_PROFILE_NAME')\n");

    return;

}


/* Function Name:
 *      hwp_probeFailure_handler
 * Description:
 *      Error handling once profile not found
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - handled
 * Note:
 *      None
 */
int32
hwp_probeFailure_handler(void)
{
    RT_INIT_MSG("    No hardware profile matched!\n");

    hwp_profileList_show();

    return RT_ERR_OK;
}



/* Function Name:
 *      hwp_profile_find
 * Description:
 *      Give hwp_identifier_t, and it will scan all hardware profile and return the matched one
 * Input:
 *      hwp_identifier_t *id        - the ID that try to match
 * Output:
 *      hwp_hwProfile_t **profile   - the matched the profile
 * Return:
 *      RT_ERR_OK       - Got
 *      RT_ERR_FAILED   - Not found
 * Note:
 *      None
 */
int32
hwp_profile_find(hwp_identifier_t *id, hwp_hwProfile_t **profile)
{
    hwp_hwProfile_t     **pp_bdPro;

    for (pp_bdPro = hwp_hwProfile_list; *pp_bdPro != NULL; pp_bdPro++)
    {
        if( hwp_identifier_cmp(id, &(*pp_bdPro)->identifier) == FALSE)
            continue;

        *profile = *pp_bdPro;
        return RT_ERR_OK;

    }/* end for */

    return RT_ERR_FAILED;
}

void
_hwp_hwpDatabaseDefault_init(void)
{
    hwp_hwProfile_t **pp_bdPro, *hwp;
    int i;

    for (pp_bdPro = hwp_hwProfile_list; *pp_bdPro != NULL; pp_bdPro++)
    {
        hwp = *pp_bdPro;
        hwp->parsed_info = &parsedInfoEmpty;
    }

    /* init database */
    for(i=0; i<RTK_MAX_NUM_OF_UNIT; i++)
    {
        unitMapStruct[i].pUnitInfo = &parsedInfoEmpty.unitInfo[HWP_LOCALUNIT_EMPTY];
        unitMapStruct[i].flag = 0;
    }

}


/* Function Name:
 *      hwp_probe
 * Description:
 *      Probe what hardware profile this board belongs to
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Got
 *      RT_ERR_FAILED   - Not found
 * Note:
 *      None
 */
int32
hwp_probe(void)
{
    hwp_identifier_t    myIdentifier;

    /*
     * This is sample codes to force a hardware profile, instead of auto-probe.
     * The code could be pasted anywhere before calling hwp_probe().
     */
#ifdef HWP_FORCE_PROFILE_SAMPLE_CODE
    {
        hwp_identifier_t forcedhwp;
        forcedhwp.type = HWP_ID_TYPE_STRING; //or HWP_ID_TYPE_INTEGER
        osal_strncpy(forcedhwp.name, "RTL8393M_DEMO", HWP_PROFILE_NAME_LEN_MAX);
        forcedhwp.id = HWP_RTL8393M_DEMO;

        hwp_forceProfile(&forcedhwp);
    }
#endif

    RT_INIT_MSG("  Hardware-profile probe");

    /* init database to default*/
    _hwp_hwpDatabaseDefault_init();
    osal_memset(&myIdentifier, 0, sizeof(myIdentifier));

    /* get profile identifier */
    if ( (RT_ERR_OK == hwp_forcedProfile_get(&myIdentifier)) || (ioal_param_hwpIdKey_get(&myIdentifier) == RT_ERR_OK) )
    {
        if (TRUE == hwp_identifier_cmp(&myIdentifier, &hwp_defaultHwp))
        {
            hwp_useDefHwp = TRUE;
        }
        else
        {
            /* scan all hardware profiles */
            if( hwp_profile_find(&myIdentifier, &hwp_myHwProfile) == RT_ERR_OK)
            {
                if (HWP_ID_TYPE_STRING == hwp_myHwProfile->identifier.type)
                    RT_INIT_MSG(" (%s)\n",hwp_myHwProfile->identifier.name);
                else if(HWP_ID_TYPE_INTEGER == hwp_myHwProfile->identifier.type)
                    RT_INIT_MSG(" (%d)\n",hwp_myHwProfile->identifier.id);
                else
                    RT_INIT_MSG(" ()\n");

                return RT_ERR_OK;
            }
            else
            {
                RT_INIT_MSG("\n");
                hwp_probeFailure_handler();
                return RT_ERR_FAILED;
            }
        }
    }
    else
    {
        hwp_useDefHwp = TRUE;
    }

    RT_INIT_MSG("\n");
#ifdef RTK_HWP_DEFAULT_PROFILE
    if (TRUE == hwp_useDefHwp)
    {
        RT_INIT_MSG("    Using Default Hardware Profile\n");
        if (RT_ERR_OK != hwp_defaultProfile_build(&hwp_myHwProfile))
            return RT_ERR_FAILED;
    }
#endif/* RTK_HWP_DEFAULT_PROFILE */

    return RT_ERR_OK;

}


/* Function Name:
 *      hwp_forceProfile
 * Description:
 *      Force to set the hardware profile of this board. Using the mechanism will
 *      take priority over auto-probe.
 * Input:
 *      id1         - Hardware profile identifier
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Set finished
 * Note:
 *      None
 */
int32
hwp_forceProfile(hwp_identifier_t *identifier)
{
    osal_memset(&hwp_forcedHwp, 0, sizeof(hwp_forcedHwp));

    hwp_forcedHwp.id =  identifier->id;
    hwp_forcedHwp.type = identifier->type;
    osal_strncpy(hwp_forcedHwp.name, identifier->name, HWP_PROFILE_NAME_LEN_MAX-1);

    return RT_ERR_OK;
}


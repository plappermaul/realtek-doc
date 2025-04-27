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
 * $Revision: 83077 $
 * $Date: 2017-11-02 18:51:47 +0800 (Thu, 02 Nov 2017) $
 *
 * Purpose : chip symbol and data type definition in the SDK.
 *
 * Feature : chip symbol and data type definition
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/debug/rt_log.h>
#include <osal/print.h>
#include <ioal/ioal_init.h>
#include <ioal/mem32.h>

#if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_RTL8390_SERIES))
#include <private/drv/swcore/swcore_rtl8390.h>
#endif
#if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_RTL8380_SERIES))
#include <private/drv/swcore/swcore_rtl8380.h>
#endif
#if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_RTL9300_SERIES))
#include <private/drv/swcore/swcore_rtl9300.h>
#endif
#if (defined(CONFIG_SDK_RTL9310) || defined(CONFIG_RTL9310_SERIES))
#include <private/drv/swcore/swcore_rtl9310.h>
#endif

#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>

#include <hwp/hw_profile.h>



/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
#if !defined(CONFIG_SDK_EXTERNAL_CPU)
#if !defined(__BOOTLOADER__) && defined(__KERNEL__)
extern int32 bsp_mem32_read(uint32 unit, uint32 addr, uint32 *pVal);
extern int32 bsp_mem32_write(uint32 unit, uint32 addr, uint32 val);
#endif
#endif/* #if !defined(CONFIG_SDK_EXTERNAL_CPU) */


int drv_swcore_early_init=FALSE; /* TRUE: for early init procedure usage, FLASE: else */
static ioal_mem32_read_t   ioal_mem32_read_cb;
static ioal_mem32_write_t  ioal_mem32_write_cb;


typedef int32 (*hal_get_chip_id_f)(uint32 unit, uint32 *pChip_id, uint32 *pChip_rev_id);

int rtk_chip_type[RTK_MAX_NUM_OF_UNIT]={0};
#define BSP_CHIP_TYPE_ASSIGN(unit, type)\
    do {\
        rtk_chip_type[unit] = type;\
    } while(0)

#if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_RTL8380_SERIES))
static int32 _drv_swcore_cid8380_get(uint32 unit, uint32 *pChip_id, uint32 *pChip_rev_id);
#endif
#if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_RTL8390_SERIES))
static int32 _drv_swcore_cid8390_get(uint32 unit, uint32 *pChip_id, uint32 *pChip_rev_id);
#endif
#if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_RTL9300_SERIES))
static int32 _drv_swcore_cid9300_get(uint32 unit, uint32 *pChip_id, uint32 *pChip_rev_id);
#endif
#if (defined(CONFIG_SDK_RTL9310) || defined(CONFIG_RTL9310_SERIES))
static int32 _drv_swcore_cid9310_get(uint32 unit, uint32 *pChip_id, uint32 *pChip_rev_id);
#endif


#define CHIP_ID_INVALID (0xFFFFFFFF)

static hal_get_chip_id_f func[] =
{
#if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_RTL8380_SERIES))
        _drv_swcore_cid8380_get,
#endif
#if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_RTL8390_SERIES))
        _drv_swcore_cid8390_get,
#endif
#if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_RTL9300_SERIES))
        _drv_swcore_cid9300_get,
#endif
#if (defined(CONFIG_SDK_RTL9310) || defined(CONFIG_RTL9310_SERIES))
        _drv_swcore_cid9310_get,
#endif
};

/*
 * Macro Definition
 */



/*
 * Function Declaration
 */

int32
drv_swcore_reg32_read(uint32 unit, uint32 addr, uint32 *pVal)
{
#if !defined(CONFIG_SDK_EXTERNAL_CPU)
#if !defined(__BOOTLOADER__) && defined(__KERNEL__)
    if(drv_swcore_early_init)
        return bsp_mem32_read(unit,addr,pVal);
    else
#endif
#endif /* #if !defined(CONFIG_SDK_EXTERNAL_CPU) */

        return ioal_mem32_read_cb(unit,addr,pVal);

    return RT_ERR_FAILED;
}

int32
drv_swcore_reg32_write(uint32 unit, uint32 addr, uint32 val)
{
#if !defined(CONFIG_SDK_EXTERNAL_CPU)
#if !defined(__BOOTLOADER__) && defined(__KERNEL__)
    if(drv_swcore_early_init)
        return bsp_mem32_write(unit,addr,val);
    else
#endif
#endif /* #if !defined(CONFIG_SDK_EXTERNAL_CPU) */
        return ioal_mem32_write_cb(unit,addr,val);

    return RT_ERR_FAILED;
}


void
drv_swcore_earlyInit_start(void)
{
    drv_swcore_early_init = TRUE;
}

void
drv_swcore_earlyInit_end(void)
{
    drv_swcore_early_init = FALSE;
}

void
drv_swcore_ioalCB_register(ioal_mem32_read_t read_f,ioal_mem32_write_t write_f)
{
    ioal_mem32_read_cb = read_f;
    ioal_mem32_write_cb = write_f;
    return;
}

#if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_RTL8380_SERIES))
static int32
_drv_swcore_cid8380_get(uint32 unit, uint32 *pChip_id, uint32 *pChip_rev_id)
{
    uint32 original_data_intRd;
    uint32 chipId;
    uint32 chipVersion;
    uint32 sub_version = 1;

    uint32 val = 0;

    /* parameter check */
    RT_INTERNAL_PARAM_CHK((NULL == pChip_id), RT_ERR_NULL_POINTER);
    RT_INTERNAL_PARAM_CHK((NULL == pChip_rev_id), RT_ERR_NULL_POINTER);

    if (drv_swcore_reg32_read(unit, RTL8380_MODEL_NAME_INFO_ADDR, &chipId) != RT_ERR_OK)
        return RT_ERR_FAILED;

    if ((((chipId>>16)&0xFFFF) != 0x8330) && (((chipId>>16)&0xFFFF) != 0x8332) &&
        (((chipId>>16)&0xFFFF) != 0x8380) && (((chipId>>16)&0xFFFF) != 0x8382))
        return RT_ERR_FAILED;

    drv_swcore_reg32_read(unit, RTL8380_INT_RW_CTRL_ADDR, &val);
    original_data_intRd = val;
    val |= (0x3);
    drv_swcore_reg32_write(unit, RTL8380_INT_RW_CTRL_ADDR, val);

    drv_swcore_reg32_read(unit, RTL8380_MODEL_NAME_INFO_ADDR, &chipId);
    drv_swcore_reg32_read(unit, RTL8380_EXT_VERSION_ADDR, &chipVersion);
    chipVersion &= 0x1f;

    /* Check sub-version */
    drv_swcore_reg32_read(unit, RTL8380_MODE_DEFINE_CTL_ADDR, &sub_version);
    sub_version = sub_version & (1UL<<23);

    drv_swcore_reg32_write(unit, RTL8380_INT_RW_CTRL_ADDR, original_data_intRd);

    switch (chipId & 0xfffff800)
    {
        case RTL8330M_CHIP_ID & 0xfffff800:
            *pChip_id = RTL8330M_CHIP_ID;
            *pChip_rev_id = chipVersion == 2 ? CHIP_REV_ID_B : CHIP_REV_ID_C;
            break;
        case RTL8332M_CHIP_ID & 0xfffff800:
            *pChip_id = RTL8332M_CHIP_ID;
            *pChip_rev_id = chipVersion == 2 ? CHIP_REV_ID_B : CHIP_REV_ID_C;
            break;
        case RTL8380M_CHIP_ID & 0xfffff800:
            if(sub_version == 0)
            {
                *pChip_id = RTL8381M_CHIP_ID;
                *pChip_rev_id = chipVersion == 2 ? CHIP_REV_ID_B  : CHIP_REV_ID_C;
            }
            else
            {
                *pChip_id = RTL8380M_CHIP_ID;
                *pChip_rev_id = chipVersion == 2 ? CHIP_REV_ID_B : CHIP_REV_ID_C;
            }
            break;
        case RTL8382M_CHIP_ID & 0xfffff800:
            *pChip_id = RTL8382M_CHIP_ID;
            *pChip_rev_id = chipVersion == 2 ? CHIP_REV_ID_B : CHIP_REV_ID_C;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of _drv_swcore_cid8380_get */
#endif /*8380 series*/

#if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_RTL8390_SERIES))
static int32
_drv_swcore_cid8390_get(uint32 unit, uint32 *pChip_id, uint32 *pChip_rev_id)
{
    uint32 model_info = 0;
#if !defined(__MODEL_USER__)
    uint32 tmp = 0;
#endif
    /* parameter check */
    RT_INTERNAL_PARAM_CHK((NULL == pChip_id), RT_ERR_NULL_POINTER);
    RT_INTERNAL_PARAM_CHK((NULL == pChip_rev_id), RT_ERR_NULL_POINTER);

#if defined(__MODEL_USER__)
    *pChip_id = model_info = RTL8393M_CHIP_ID;
    *pChip_rev_id = CHIP_REV_ID_A;
#else
    drv_swcore_reg32_read(unit, RTL8390_MODEL_NAME_INFO_ADDR, &model_info);
    tmp = (model_info & 0x3E) >> 1;
    model_info &= 0xFFFFFFC0;

    switch (model_info)
    {
        case RTL8391M_CHIP_ID:
            *pChip_id = RTL8391M_CHIP_ID;
            break;

        case RTL8392M_CHIP_ID:
            if (tmp == 0)
            {
                *pChip_id = RTL8392MES_CHIP_ID;
                BSP_CHIP_TYPE_ASSIGN(unit,CHIP_TYPE_1);
            }
            else
            {
                *pChip_id = RTL8392M_CHIP_ID;
            }
            break;
        case RTL8393M_CHIP_ID:
            if (tmp == 0)
            {
                *pChip_id = RTL8393MES_CHIP_ID;
                BSP_CHIP_TYPE_ASSIGN(unit, CHIP_TYPE_1);
            }
            else
            {
                *pChip_id = RTL8393M_CHIP_ID;
            }
            break;
        case RTL8396M_CHIP_ID:
            if (tmp == 0)
            {
                *pChip_id = RTL8396MES_CHIP_ID;
                BSP_CHIP_TYPE_ASSIGN(unit,CHIP_TYPE_1);
            }
            else
            {
                *pChip_id = RTL8396M_CHIP_ID;
            }
            break;
        case RTL8351M_CHIP_ID:
            *pChip_id = RTL8351M_CHIP_ID;
            break;
        case RTL8352M_CHIP_ID:
            if (tmp == 0)
            {
                *pChip_id = RTL8352MES_CHIP_ID;
                BSP_CHIP_TYPE_ASSIGN(unit, CHIP_TYPE_1);
            }
            else
            {
                *pChip_id = RTL8352M_CHIP_ID;
            }
            break;
        case RTL8353M_CHIP_ID:
            if (tmp == 0)
            {
                *pChip_id = RTL8353MES_CHIP_ID;
                BSP_CHIP_TYPE_ASSIGN(unit, CHIP_TYPE_1);
            }
            else
            {
                *pChip_id = RTL8353M_CHIP_ID;
            }
            break;
        default:
            return RT_ERR_FAILED;
    }

    *pChip_rev_id = tmp;
#endif

    return RT_ERR_OK;
} /* end of _drv_swcore_cid8390_get */
#endif /*8390 series*/


#if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_RTL9300_SERIES))
static int32
_drv_swcore_cid9300_get(uint32 unit, uint32 *pChip_id, uint32 *pChip_rev_id)
{
    uint32 model_info = 0;
    uint32 temp = 0;
    uint32 model_char_3rd = 0; /* 1: testchip; 0: formal chip */

    drv_swcore_reg32_read(unit, RTL9300_MODEL_NAME_INFO_ADDR, &temp);
    model_info = temp & 0xFFFFFFC0;
    /* bit 6:4 */
    model_char_3rd = (temp & RTL9300_MODEL_NAME_INFO_MODEL_CHAR_3RD_MASK) >> RTL9300_MODEL_NAME_INFO_MODEL_CHAR_3RD_OFFSET;
    /* bit 3:0 */
    *pChip_rev_id = (temp & RTL9300_MODEL_NAME_INFO_RTL_VID_MASK) >> RTL9300_MODEL_NAME_INFO_RTL_VID_OFFSET;

    switch (model_info)
    {
        case RTL9301_CHIP_ID & 0xFFFFFFC0:
            if(0 == model_char_3rd)
            {
                *pChip_id = RTL9301_CHIP_ID;
                if( *pChip_rev_id > RTL9300_CHIP_ID_LASTEST_REV)
                    *pChip_rev_id = RTL9300_CHIP_ID_LASTEST_REV;
            }
            else
            {
                *pChip_id = RTL9301_CHIP_ID_24G;
                if( *pChip_rev_id > RTL9300_CHIP_ID_ES_LASTEST_REV)
                    *pChip_rev_id = RTL9300_CHIP_ID_ES_LASTEST_REV;
                BSP_CHIP_TYPE_ASSIGN(unit, CHIP_TYPE_1);
            }
            break;
        case RTL9301_CHIP_ID_24G & 0xFFFFFFC0:
            if(0 == model_char_3rd)
            {
                *pChip_id = RTL9301_CHIP_ID;
                if( *pChip_rev_id > RTL9300_CHIP_ID_LASTEST_REV)
                    *pChip_rev_id = RTL9300_CHIP_ID_LASTEST_REV;
            }
            else
            {
                *pChip_id = RTL9301_CHIP_ID_24G;
                if( *pChip_rev_id > RTL9300_CHIP_ID_ES_LASTEST_REV)
                    *pChip_rev_id = RTL9300_CHIP_ID_ES_LASTEST_REV;
                BSP_CHIP_TYPE_ASSIGN(unit, CHIP_TYPE_1);
            }
            break;
        case RTL9303_CHIP_ID & 0xFFFFFFC0:
            if(0 == model_char_3rd)
            {
                *pChip_id = RTL9303_CHIP_ID;
                if( *pChip_rev_id > RTL9300_CHIP_ID_LASTEST_REV)
                    *pChip_rev_id = RTL9300_CHIP_ID_LASTEST_REV;
            }
            else
            {
                *pChip_id = RTL9303_CHIP_ID_8XG;
                if( *pChip_rev_id > RTL9300_CHIP_ID_ES_LASTEST_REV)
                    *pChip_rev_id = RTL9300_CHIP_ID_ES_LASTEST_REV;
                BSP_CHIP_TYPE_ASSIGN(unit, CHIP_TYPE_1);
            }
            break;
        case RTL9303_CHIP_ID_8XG & 0xFFFFFFC0:
            if(0 == model_char_3rd)
            {
                *pChip_id = RTL9303_CHIP_ID;
                if( *pChip_rev_id > RTL9300_CHIP_ID_LASTEST_REV)
                    *pChip_rev_id = RTL9300_CHIP_ID_LASTEST_REV;
            }
            else
            {
                *pChip_id = RTL9303_CHIP_ID_8XG;
                if( *pChip_rev_id > RTL9300_CHIP_ID_ES_LASTEST_REV)
                    *pChip_rev_id = RTL9300_CHIP_ID_ES_LASTEST_REV;
                BSP_CHIP_TYPE_ASSIGN(unit, CHIP_TYPE_1);
            }
            break;
        case RTL9301H_CHIP_ID & 0xFFFFFFC0:
            if(0 == model_char_3rd)
            {
                *pChip_id = RTL9301H_CHIP_ID;
                if( *pChip_rev_id > RTL9300_CHIP_ID_LASTEST_REV)
                    *pChip_rev_id = RTL9300_CHIP_ID_LASTEST_REV;
            }
            else
            {
                *pChip_id = RTL9301H_CHIP_ID_4X2_5G;
                if( *pChip_rev_id > RTL9300_CHIP_ID_ES_LASTEST_REV)
                    *pChip_rev_id = RTL9300_CHIP_ID_ES_LASTEST_REV;
                BSP_CHIP_TYPE_ASSIGN(unit, CHIP_TYPE_1);
            }
            break;
        case RTL9302A_CHIP_ID & 0xFFFFFFC0:
            if(0 == model_char_3rd)
            {
                *pChip_id = RTL9302A_CHIP_ID;
                if( *pChip_rev_id > RTL9300_CHIP_ID_LASTEST_REV)
                    *pChip_rev_id = RTL9300_CHIP_ID_LASTEST_REV;
            }
            else
            {
                *pChip_id = RTL9302A_CHIP_ID_12X2_5G;
                if( *pChip_rev_id > RTL9300_CHIP_ID_ES_LASTEST_REV)
                    *pChip_rev_id = RTL9300_CHIP_ID_ES_LASTEST_REV;
                BSP_CHIP_TYPE_ASSIGN(unit, CHIP_TYPE_1);
            }
            break;
        case RTL9302B_CHIP_ID & 0xFFFFFFC0:
            if(0 == model_char_3rd)
            {
                *pChip_id = RTL9302B_CHIP_ID;
                if( *pChip_rev_id > RTL9300_CHIP_ID_LASTEST_REV)
                    *pChip_rev_id = RTL9300_CHIP_ID_LASTEST_REV;
            }
            else
            {
                *pChip_id = RTL9302B_CHIP_ID_8X2_5G;
                if( *pChip_rev_id > RTL9300_CHIP_ID_ES_LASTEST_REV)
                    *pChip_rev_id = RTL9300_CHIP_ID_ES_LASTEST_REV;
                BSP_CHIP_TYPE_ASSIGN(unit, CHIP_TYPE_1);
            }
            break;
        case RTL9302C_CHIP_ID & 0xFFFFFFC0:
            if(0 == model_char_3rd)
            {
                *pChip_id = RTL9302C_CHIP_ID;
                if( *pChip_rev_id > RTL9300_CHIP_ID_LASTEST_REV)
                    *pChip_rev_id = RTL9300_CHIP_ID_LASTEST_REV;
            }
            else
            {
                *pChip_id = RTL9302C_CHIP_ID_16X2_5G;
                if( *pChip_rev_id > RTL9300_CHIP_ID_ES_LASTEST_REV)
                    *pChip_rev_id = RTL9300_CHIP_ID_ES_LASTEST_REV;
                BSP_CHIP_TYPE_ASSIGN(unit, CHIP_TYPE_1);
            }
            break;
        case RTL9302D_CHIP_ID & 0xFFFFFFC0:
            if(0 == model_char_3rd)
            {
                *pChip_id = RTL9302D_CHIP_ID;
                if( *pChip_rev_id > RTL9300_CHIP_ID_LASTEST_REV)
                    *pChip_rev_id = RTL9300_CHIP_ID_LASTEST_REV;
            }
            else
            {
                *pChip_id = RTL9302D_CHIP_ID_24X2_5G;
                if( *pChip_rev_id > RTL9300_CHIP_ID_ES_LASTEST_REV)
                    *pChip_rev_id = RTL9300_CHIP_ID_ES_LASTEST_REV;
                BSP_CHIP_TYPE_ASSIGN(unit, CHIP_TYPE_1);
            }
            break;
        case RTL9302DE_CHIP_ID & 0xFFFFFFC0:
            if(0 == model_char_3rd)
            {
                *pChip_id = RTL9302DE_CHIP_ID;
                if( *pChip_rev_id > RTL9300_CHIP_ID_LASTEST_REV)
                    *pChip_rev_id = RTL9300_CHIP_ID_LASTEST_REV;
            }
            break;

        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of _drv_swcore_cid9300_get */
#endif /*9300 series*/

#if (defined(CONFIG_SDK_RTL9310) || defined(CONFIG_RTL9310_SERIES))
static int32
_drv_swcore_cid9310_get(uint32 unit, uint32 *pChip_id, uint32 *pChip_rev_id)
{
    uint32 model_info = 0;

#if defined(__MODEL_USER__)
    *pChip_id = model_info = RTL9310_CHIP_ID;
    *pChip_rev_id = CHIP_REV_ID_A;
#else
    drv_swcore_reg32_read(unit, RTL9310_MODEL_NAME_INFO_ADDR, &model_info);

    *pChip_rev_id = model_info & 0xF;

    if ((model_info >> 4) & 0x1)
    {
        BSP_CHIP_TYPE_ASSIGN(unit, CHIP_TYPE_1);

        if( *pChip_rev_id > RTL9310_ES_LASTEST_REV)
            *pChip_rev_id = RTL9310_ES_LASTEST_REV;
    }
    else
    {
        if( *pChip_rev_id > RTL9310_LASTEST_REV)
            *pChip_rev_id = RTL9310_LASTEST_REV;
    }

    model_info &= 0xFFFFFF00;
    switch (model_info)
    {
        case RTL9310_CHIP_ID & 0xffff0000:
            *pChip_id = RTL9310_CHIP_ID;
            break;
        case RTL9311E_CHIP_ID & 0xffffff00:
            *pChip_id = RTL9311E_CHIP_ID;
            break;
        case RTL9311R_CHIP_ID & 0xffffff00:
            *pChip_id = RTL9311R_CHIP_ID;
            break;
        case RTL9311_CHIP_ID & 0xffffff00:
            *pChip_id = RTL9311_CHIP_ID;
            break;
        case RTL9312_CHIP_ID & 0xffffff00:
            *pChip_id = RTL9312_CHIP_ID;
            break;
        case RTL9313_CHIP_ID & 0xffff0000:
            *pChip_id = RTL9313_CHIP_ID;
            break;
        default:
            osal_printf("%s %x\n", __func__, model_info);
            return RT_ERR_FAILED;
    }
#endif

    return RT_ERR_OK;
} /* end of _bsp_drv_swcore_cid9310_get */
#endif /*9310 series*/

/* Function Name:
 *      drv_swcore_chipFamilyId_get
 * Description:
 *      Get chip family id
 * Input:
 *      chip_id         - chip id (e.g. RTL9301_CHIP_ID,...)
 * Output:
 *      None
 * Return:
 *      chip family ID
 */
uint32
drv_swcore_chipFamilyId_get(uint32 chip_id)
{
    if(RTL8382B_CHIP_ID == chip_id)
    {
        return RTL8382B_FAMILY_ID;
    }

    return (chip_id&FAMILY_ID_MASK);

} /* end of drv_swcore_cid_get */


/* Function Name:
 *      drv_swcore_cid_get
 * Description:
 *      Get chip id and chip revision id.
 * Input:
 *      unit           - unit id
 * Output:
 *      pChip_id       - pointer buffer of chip id
 *      pChip_rev_id   - pointer buffer of chip revision id
 * Return:
 *      RT_ERR_OK      - OK
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_NULL_POINTER
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_swcore_cid_get(uint32 unit, uint32 *pChip_id, uint32 *pChip_rev_id)
{
    uint32  i;
    hal_get_chip_id_f f;

    /* parameter check */
    RT_PARAM_CHK((NULL == pChip_id), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pChip_rev_id), RT_ERR_NULL_POINTER);

    for (i = 0; i < (sizeof(func)/sizeof(hal_get_chip_id_f)); i++)
    {
        f = (hal_get_chip_id_f) func[i];
        if (RT_ERR_OK == ((f)(unit, pChip_id, pChip_rev_id)))
            return RT_ERR_OK;
    }

    return RT_ERR_FAILED;

} /* end of drv_swcore_cid_get */


/* Function Name:
 *      drv_swcore_cid_cmp
 * Description:
 *      Compare cmp_id with the chip id of unit
 * Input:
 *      unit           - unit id
 *      cmp_id         - chip id for compare
 * Output:
 *      None
 * Return:
 *      0              - identical
 *      RT_ERR_FAILED  - not identical
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_swcore_cid_cmp(uint32 unit, uint32 cmp_id)
{
    uint32 cid = 0, crid = 0;

    if (RT_ERR_OK != drv_swcore_cid_get(unit, &cid, &crid))
        return RT_ERR_FAILED;

    if (cmp_id == cid)
        return 0;
    else
        return RT_ERR_FAILED;

} /* end of drv_swcore_cid_cmp */




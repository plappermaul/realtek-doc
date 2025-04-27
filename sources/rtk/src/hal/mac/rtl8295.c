/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
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
 * Purpose : 8295 Driver APIs.
 *
 * Feature : 8295 Driver APIs for both PHY mode and SerDes Converter mode
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <soc/type.h>
#include <osal/time.h>
#include <osal/print.h>
#include <hal/mac/miim_common_drv.h>
#include <hal/common/halctrl.h>
#include <hal/mac/rtl8295.h>


/*
 * Symbol Definition
 */
#define HAL_RTL8295_SDS_BCST                8
#define HAL_RTL8295_MAX_SDS_IDX             HAL_RTL8295_SDS_BCST


/*
 * Data Declaration
 */
/* <<< SerDes page offset >>> */
static uint32  sds_reg_page_offset[HAL_RTL8295_MAX_SDS_IDX+1] =
                    {256,   //SDS_S0 for Serdes S0
                     768,   //SDS_S2 for Serdes S1
                     512,   //SDS_S1
                     2304,  //SDS_S3
                     1024,  //SDS_S4 for Serdes S4
                     1280,  //SDS_S5 for Serdes S5
                     1536,  //SDS_S6 for Serdes S6
                     1792,  //SDS_S7 for Serdes S7
                     2048   //BCAST
                    };

static uint32  sds_reg_mmdAddr_offset[HAL_RTL8295_MAX_SDS_IDX+1] =
                    {0x0800,  //SDS_S0 for Serdes S0
                     0x1800,  //SDS_S2 for Serdes S1
                     0x1000,  //SDS_S1
                     0x4800,  //SDS_S3
                     0x2000,  //SDS_S4 for Serdes S4
                     0x2800,  //SDS_S5 for Serdes S5
                     0x3000,  //SDS_S6 for Serdes S6
                     0x3800,  //SDS_S7 for Serdes S7
                     0x4000   //BCAST
                    };

/*
 * Macro Declaration
 */
#define RTL8295_REG_ADDR_TO_PAGE(_addr)         ((_addr / 4) / 8)
#define RTL8295_REG_ADDR_TO_IDX(_addr)          (((_addr / 4) % 8) + 16)


#define RTL8295_MMD_DEV_VENDOR_1                30
#define RTL8295_REG_MMD_DEV                     RTL8295_MMD_DEV_VENDOR_1
#define RTL8295_REG_ADDR_TO_MMDREG(_addr)       (_addr / 4)
#define RTL8295_SDSREG_MMD_DEV                  RTL8295_MMD_DEV_VENDOR_1


/*
 * Function Declaration
 */

/*
 * Get MDX protocol type
 */
int32
_rtl8295_macSmiProto_get(uint32 unit, rtk_port_t port)
{
    drv_smi_mdxProtoSel_t   proto;

    if (hal_miim_portSmiMdxProto_get(unit, port, &proto) == RT_ERR_OK)
    {
        if (proto == DRV_SMI_MDX_PROTO_C45)
        {
            return 1;//C.45
        }
    }
    return 0;
}

/*
 * Transfer 8295 SerDes page/reg to global page/reg
 */
static int32
_sdsPageReg_to_chipPageReg(uint32 sdsId, uint32 page, uint32 reg, uint32 *pScPage, uint32 *pScReg)
{
    uint32      tmp;

    page = page & 0x3F;
    reg = reg & 0x1F;

    tmp = (page << 5) + reg ;
    *pScPage = (tmp / 8) + sds_reg_page_offset[sdsId];
    *pScReg  = (tmp % 8) + 16;

    return RT_ERR_OK;
}

/*
 * Transfer 8295 SerDes page/reg to MMD address
 */
int32
_sdsPageReg_to_mmdReg(uint32 sdsId, uint32 page, uint32 reg, uint32 *pMmdAddr)
{
    uint32      tmp;

    page = page & 0x3F;
    reg = reg & 0x1F;

    tmp = (page << 5) + reg;
    *pMmdAddr = tmp + sds_reg_mmdAddr_offset[sdsId];

    return RT_ERR_OK;
}

static int32
_rtl8295_miim_read(
    uint32      unit,
    rtk_port_t  port,
    uint32      page,
    uint32      phy_reg,
    uint32      *pData)
{
    hal_control_t   *pHalCtrl;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_CHIP_NOT_FOUND;

    if (MACDRV(pHalCtrl)->fMdrv_miim_read == NULL)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    return MACDRV(pHalCtrl)->fMdrv_miim_read(unit, port, page, phy_reg, pData);
}


static int32
_rtl8295_miim_write(
        uint32      unit,
        rtk_port_t  port,
        uint32      page,
        uint32      phy_reg,
        uint32      data)
{
    hal_control_t   *pHalCtrl;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_CHIP_NOT_FOUND;

    if (MACDRV(pHalCtrl)->fMdrv_miim_write == NULL)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    return MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, page, phy_reg, data);
}




static int32
_rtl8295_mmd_read(
        uint32      unit,
        rtk_port_t  port,
        uint32      mmdAddr,
        uint32      mmdReg,
        uint32      *pData)
{
    hal_control_t   *pHalCtrl;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_CHIP_NOT_FOUND;

    if (MACDRV(pHalCtrl)->fMdrv_miim_mmd_read == NULL)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    return MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, mmdAddr, mmdReg, pData);
}


static int32
_rtl8295_mmd_write(
        uint32      unit,
        rtk_port_t  port,
        uint32      mmdAddr,
        uint32      mmdReg,
        uint32      data)
{
    hal_control_t   *pHalCtrl;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_CHIP_NOT_FOUND;

    if (MACDRV(pHalCtrl)->fMdrv_miim_mmd_write == NULL)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    return MACDRV(pHalCtrl)->fMdrv_miim_mmd_write(unit, port, mmdAddr, mmdReg, data);
}

/* Function Name:
 *      hal_rtl8295_reg_read
 * Description:
 *      Read from register
 * Input:
 *      unit        - unit id
 *      mdxMacId    - MAC ID for MDC/MDIO
 *      reg_addr    - register address
 * Output:
 *      pData       - register data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
hal_rtl8295_reg_read(uint32 unit, rtk_port_t mdxMacId, uint32 reg_addr, uint32 *pData)
{
    uint32      page, reg, mmdReg;
    int32       ret;

    if (HWP_10GE_PORT(unit, mdxMacId))
    {
        if (_rtl8295_macSmiProto_get(unit, mdxMacId))
        {
            mmdReg = RTL8295_REG_ADDR_TO_MMDREG(reg_addr);
            if ((ret = _rtl8295_mmd_read(unit, mdxMacId, RTL8295_REG_MMD_DEV, mmdReg, pData)) != RT_ERR_OK)
            {
                return ret;
            }
            return RT_ERR_OK;
        }
    }

    page = RTL8295_REG_ADDR_TO_PAGE(reg_addr);
    reg = RTL8295_REG_ADDR_TO_IDX(reg_addr);
    if ((ret = _rtl8295_miim_read(unit, mdxMacId, page, reg, pData)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      hal_rtl8295_reg_write
 * Description:
 *      Write to register
 * Input:
 *      unit        - unit id
 *      mdxMacId    - MAC ID for MDC/MDIO
 *      reg_addr    - register address
 *      data        - data to write to register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
hal_rtl8295_reg_write(uint32 unit, rtk_port_t mdxMacId, uint32 reg_addr, uint32 data)
{
    uint32      page, reg, mmdReg;
    int32       ret;

    if (HWP_10GE_PORT(unit, mdxMacId))
    {
        if (_rtl8295_macSmiProto_get(unit, mdxMacId))
        {
            mmdReg = RTL8295_REG_ADDR_TO_MMDREG(reg_addr);
            if ((ret = _rtl8295_mmd_write(unit, mdxMacId, RTL8295_REG_MMD_DEV, mmdReg, data)) != RT_ERR_OK)
            {
                return ret;
            }
            return RT_ERR_OK;
        }
    }

    page = RTL8295_REG_ADDR_TO_PAGE(reg_addr);
    reg = RTL8295_REG_ADDR_TO_IDX(reg_addr);
    if ((ret = _rtl8295_miim_write(unit, mdxMacId, page, reg, data)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      hal_rtl8295_sds_read
 * Description:
 *      Read from SerDes register
 * Input:
 *      unit        - unit id
 *      mdxMacId    - MAC ID for MDC/MDIO
 *      sdsId       - SerDes Id
 *      page        - page
 *      reg         - register index
 * Output:
 *      pData       - register data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
hal_rtl8295_sds_read(uint32 unit, rtk_port_t mdxMacId, uint32 sdsId, uint32 page, uint32 reg, uint32 *pData)
{
    uint32      sc_page, sc_reg, mmdReg;
    int32       ret;

    if (sdsId >= HAL_RTL8295_MAX_SDS_IDX)
    {
        return RT_ERR_FAILED;
    }

    //RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d,port=%d,sds=%u,page:%u,reg:%u", unit, port, sdsId, page, reg);

    if (HWP_10GE_PORT(unit, mdxMacId))
    {
        if (_rtl8295_macSmiProto_get(unit, mdxMacId))
        {
            _sdsPageReg_to_mmdReg(sdsId, page, reg, &mmdReg);
            if ((ret = _rtl8295_mmd_read(unit, mdxMacId, RTL8295_SDSREG_MMD_DEV, mmdReg, pData)) != RT_ERR_OK)
            {
                return ret;
            }
            return RT_ERR_OK;
        }
    }

    _sdsPageReg_to_chipPageReg(sdsId, page, reg, &sc_page, &sc_reg);
    if ((ret = _rtl8295_miim_read(unit, mdxMacId, sc_page, sc_reg, pData)) != RT_ERR_OK)
    {
        return ret;
    }

    //RT_LOG(LOG_DEBUG, (MOD_HAL), "pData=0x%x", *pData);

    return RT_ERR_OK;

}


/* Function Name:
 *      hal_rtl8295_sds_write
 * Description:
 *      Write to SerDes register
 * Input:
 *      unit        - unit id
 *      mdxMacId    - MAC ID for MDC/MDIO
 *      sdsId       - SerDes Id
 *      page        - page
 *      reg         - register index
 *      data        - data to write to register
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
hal_rtl8295_sds_write(uint32 unit, rtk_port_t mdxMacId, uint32 sdsId, uint32 page, uint32 reg, uint32 data)
{
    uint32      sc_page, sc_reg, mmdReg;
    int32       ret;


    if (sdsId >= HAL_RTL8295_MAX_SDS_IDX)
    {
        return RT_ERR_FAILED;
    }

    //RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d,port=%d,sds=%u,page:%u,reg:%u", unit, port, sdsId, page, reg);

    if (HWP_10GE_PORT(unit, mdxMacId))
    {
        if (_rtl8295_macSmiProto_get(unit, mdxMacId))
        {
            _sdsPageReg_to_mmdReg(sdsId, page, reg, &mmdReg);
            if ((ret = _rtl8295_mmd_write(unit, mdxMacId, RTL8295_SDSREG_MMD_DEV, mmdReg, data)) != RT_ERR_OK)
            {
                return ret;
            }
            return RT_ERR_OK;
        }
    }

    _sdsPageReg_to_chipPageReg(sdsId, page, reg, &sc_page, &sc_reg);
    if ((ret = _rtl8295_miim_write(unit, mdxMacId, sc_page, sc_reg, data)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      hal_rtl8295_mdxMacId_get
 * Description:
 *      Transfer port to the MAC ID that are used for access 8295
 * Input:
 *      unit        - unit id
 *      port        - port number
 * Output:
 *      pMdxMacId   - MAC ID for MDC/MDIO
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
hal_rtl8295_mdxMacId_get(uint32 unit, rtk_port_t port, uint32 *pMdxMacId)
{
    rtk_port_t      base_port;
    if (HWP_CHIP_ID(unit) == RTL8396M_CHIP_ID)
    {
        /* 96M 10G port use Cl.22 to access to 95R */
        if (HWP_10GE_PORT(unit, port))
        {
            drv_smi_mdxProtoSel_t   mdxProto = DRV_SMI_MDX_PROTO_C22;
            if (   (hal_miim_portSmiMdxProto_get(unit, port, &mdxProto) == RT_ERR_OK)
                && (mdxProto == DRV_SMI_MDX_PROTO_C22))
            {
                if (port == 24)
                {
                    *pMdxMacId = 25;
                    return RT_ERR_OK;
                }
                else if (port == 36)
                {
                    *pMdxMacId = 26;
                    return RT_ERR_OK;
                }
            }
        }
    }

    if ((base_port = HWP_PHY_BASE_MACID(unit, port)) != HWP_NONE)
    {
        *pMdxMacId = base_port;
        return RT_ERR_OK;
    }

    return RT_ERR_FAILED;
}




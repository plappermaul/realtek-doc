/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Realtek Switch SDK Rtusr API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) register
 *
 */

#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>

int32 rtusr_reg_register_get(uint32 reg, uint32 *pValue)
{
    rtdrv_regCfg_t reg_cfg;

    reg_cfg.reg = reg;
    
    GETSOCKOPT(RTDRV_REG_REGISTER_GET, &reg_cfg, rtdrv_regCfg_t, 1);
    *pValue = reg_cfg.value;
    
    return RT_ERR_OK;    
}

int32 rtusr_reg_register_set(uint32 reg, uint32 value)
{
    rtdrv_regCfg_t reg_cfg;


    memset(&reg_cfg,0x0,sizeof(rtdrv_regCfg_t));
    reg_cfg.reg = reg;
    reg_cfg.value = value;
    SETSOCKOPT(RTDRV_REG_REGISTER_SET, &reg_cfg, rtdrv_regCfg_t, 1); 

    return RT_ERR_OK;    
}

int32 rtusr_reg_idx2Addr_get(uint32 regIdx, uint32 *pAddr)
{
    rtdrv_regCfg_t reg_cfg;
    

    reg_cfg.reg = regIdx;
    
    GETSOCKOPT(RTDRV_REG_IDX2ADDR_GET, &reg_cfg, rtdrv_regCfg_t, 1);
    *pAddr = reg_cfg.value;
    
    return RT_ERR_OK;    
}


int32 rtusr_reg_address_get(uint32 addr, uint32 *pValue)
{
    rtdrv_addrCfg_t addr_cfg;

    addr_cfg.address = addr;
    
    GETSOCKOPT(RTDRV_REG_ADDRESS_GET, &addr_cfg, rtdrv_addrCfg_t, 1);
    *pValue = addr_cfg.value;
    
    return RT_ERR_OK;    
}

int32 rtusr_reg_address_set(uint32 addr, uint32 value)
{
    rtdrv_addrCfg_t addr_cfg;

    addr_cfg.address = addr;
    addr_cfg.value = value;
    SETSOCKOPT(RTDRV_REG_ADDRESS_SET, &addr_cfg, rtdrv_addrCfg_t, 1); 

    return RT_ERR_OK;    
}




int32 rtusr_soc_address_get(uint32 addr, uint32 *pValue)
{
    rtdrv_addrCfg_t addr_cfg;

    addr_cfg.address = addr;
    
    GETSOCKOPT(RTDRV_SOC_ADDRESS_GET, &addr_cfg, rtdrv_addrCfg_t, 1);
    *pValue = addr_cfg.value;
    
    return RT_ERR_OK;    
}

int32 rtusr_soc_address_set(uint32 addr, uint32 value)
{
    rtdrv_addrCfg_t addr_cfg;

    addr_cfg.address = addr;
    addr_cfg.value = value;
    SETSOCKOPT(RTDRV_SOC_ADDRESS_SET, &addr_cfg, rtdrv_addrCfg_t, 1); 

    return RT_ERR_OK;    
}


int32 rtusr_phy_reg_get(uint8 phy_id,uint8 reg, uint16 *pValue)
{
    rtdrv_phyCfg_t phy_cfg;

    phy_cfg.phy_id = phy_id;
    phy_cfg.reg = reg;

    GETSOCKOPT(RTDRV_PHY_REG_GET, &phy_cfg, rtdrv_phyCfg_t, 1);
    *pValue = phy_cfg.value;
    
    return RT_ERR_OK;    
}

int32 rtusr_phy_reg_set(uint8 phy_id,uint8 reg, uint16 value)
{
    rtdrv_phyCfg_t phy_cfg;

    phy_cfg.phy_id = phy_id;
    phy_cfg.reg = reg;
    phy_cfg.value = value;
    
    SETSOCKOPT(RTDRV_PHY_REG_SET, &phy_cfg, rtdrv_phyCfg_t, 1); 

    return RT_ERR_OK;    
}


int32
debug_mem32_debugMode_set(rtk_enable_t mode)
{
    rtdrv_unitCfg_t unit_cfg;

    memset(&unit_cfg,0x0,sizeof(rtdrv_unitCfg_t));
    unit_cfg.data = mode;
    SETSOCKOPT(RTDRV_DEBUG_ADDRESS_DUMP_SET, &unit_cfg, rtdrv_unitCfg_t, 1); 
    
    return RT_ERR_OK;
    
}    

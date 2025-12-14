/*
 * Copyright(c) Realtek Semiconductor Corporation, 2011
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of HAL API test APIs in the SDK
 *
 * Feature : HAL API test APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <osal/lib.h>
#include <osal/print.h>
#include <ioal/mem32.h>
#include <hal/hal_reg_test_case.h>
#include <hal/chipdef/swdef.h>

#include <hal/chipdef/apollo/apollo_reg_struct.h>
#include <hal/mac/reg.h>
#include <osal/time.h>

/* Define symbol used for test input */
#define GPON_REG_GTC_US_WRPROTECT_ENA       0xCC19
#define GPON_REG_GTC_US_WRPROTECT_DIS       0x0000

typedef struct reg_mask_s
{
    uint32 addr;
    uint32 size;
}reg_mask_t;

static reg_mask_t ignore_addr_mask[] = {{0x23020, 1}, /* IO_MODE_EN */
                                        {0x4c, 1}, /* CFG_PHY_POLL_ADR_0 */
                                        {0x8c, 1}, /* MISCELLANEOUS_CONFIGRE0 */
                                        {0x8c, 1}, /* MISCELLANEOUS_CONFIGRE0 */
                                        {0x160, 1}, /* SDS_CFG */
                                        {0x190, 1}, /* PON_MODE_CFG */
                                        {0x2209c, 1}, /* WSDS_DIG_01 */
                                        {0x2301c, 1}, /* IO_LED_EN */
                                        {0x23020, 1}, /* IO_MODE_EN */
                                        {0x701404, 128}, /* GPON_GTC_DS_TRAFFIC_CFG */
                                        {0x703024, 1}, /* GPON_AES_WORD_DATA */
                                        {0x7050E0, 8}, /* GPON_GTC_US_PLOAM_DATA */
                                        {0x706400, 128}, /* GPON_GEM_US_PORT_MAP */
                                        {0x000400, 16128}, /* RESERVED */
                                        {0x010000, 1024}, /* CHIP_INFO */
                                        {0x012000, 1024}, /* ALE_TABLE */
                                        {0x016000, 1024}, /* RESERVED */
                                        {0x01E000, 1024}, /* LED */
                                        {0x01F000, 1024}, /* RESERVED */
                                        #if 0
                                        {0x020400, 256}, /* MAC 1 */
                                        {0x021000, 256}, /* MAC 4 */
                                        {0x021400, 256}, /* MAC 5 */
                                        #endif
                                        {0x022000, 512}, /* SDSREG */
                                        {0x022800, 512}, /* SDS_IP */
                                        {0x024000, 1024}, /* RESERVED */
                                        {0x028000, 1024}, /* ALE_HSB */
                                        {0x029000, 1024}, /* ALE_HSM */
                                        {0x02B000, 1024}, /* ALE_DEBUG */
                                        {0x02C000, 1024}, /* ALE_PKTGEN */
                                        {0x02E000, 1024}, /* PKT_ENCAP */
                                        {0x02F000, 1024}, /* PKT_PARSER */
                                        {0x030000, 1024}, /* CFM_GEM */
                                        {0x032000, 2048}, /* MIB_DATA */
                                        {0x035000, 1024}, /* MAC_PON */
                                        {0x036000, 1024}, /* EPON_CTRL */
                                        {0x037000, 1024}, /* TBD */
                                        {0x038000, 1024}, /* RESERVED */
                                        #if 0
                                        {0x710000, 256}, /* NIC */
                                        {0x720000, 512}, /* NIC_DMA */
                                        {0x800000, 1792} /* NAT */
                                        #endif
                                        };

static reg_mask_t ignore_write_addr_mask[] = {{0x23020, 1}, /* IO_MODE_EN */
                                        {0x4c, 1}, /* CFG_PHY_POLL_ADR_0 */
                                        {0x68, 1}, /* CHIP_RST */
                                        {0x8c, 1}, /* MISCELLANEOUS_CONFIGRE0 */
                                        {0x8c, 1}, /* MISCELLANEOUS_CONFIGRE0 */
                                        {0x160, 1}, /* SDS_CFG */
                                        {0x190, 1}, /* PON_MODE_CFG */
                                        {0x19c, 1}, /* GPHY_AFE_DBG_CFG */
                                        {0x1c4, 1}, /* CHIP_INF_SEL */
                                        {0x1c8, 1}, /* AUTODET_CTRL */
                                        {0x23c, 1}, /* RSVD_GLB_CTRL */
                                        {0x2301c, 1}, /* IO_LED_EN */
                                        {0x23020, 1}, /* IO_MODE_EN */
                                        {0x70000C, 1}, /* GPON_RESET */
                                        {0x701080, 1}, /* GPON_GTC_DS_PLOAM_IND */
                                        {0x703024, 1}, /* GPON_AES_WORD_DATA */
                                        {0x7050c0, 1}, /* GPON_GTC_US_PLOAM_IND */
                                        {0x7050E0, 8}, /* GPON_GTC_US_PLOAM_DATA */
                                        {0x800010, 1}, /* SWTCR0 */
                                        {0x800104, 1}, /* NAT_TBL_ACCESS_CLR */
                                        {0x000400, 16128}, /* RESERVED */
                                        {0x010000, 1024}, /* CHIP_INFO */
                                        {0x012000, 1024}, /* ALE_TABLE */
                                        {0x016000, 1024}, /* RESERVED */
                                        {0x01E000, 1024}, /* LED */
                                        {0x01F000, 1024}, /* RESERVED */
                                        #if 0
                                        {0x020400, 256}, /* MAC 1 */
                                        {0x021000, 256}, /* MAC 4 */
                                        {0x021400, 256}, /* MAC 5 */

                                        {0x022000, 512}, /* SDSREG */
                                        {0x022800, 512}, /* SDS_IP */
                                        #endif
                                        {0x024000, 1024}, /* RESERVED */
                                        {0x028000, 1024}, /* ALE_HSB */
                                        {0x029000, 1024}, /* ALE_HSM */
                                        {0x02B000, 1024}, /* ALE_DEBUG */
                                        {0x02C000, 1024}, /* ALE_PKTGEN */
                                        {0x02E000, 1024}, /* PKT_ENCAP */
                                        {0x02F000, 1024}, /* PKT_PARSER */
                                        {0x030000, 1024}, /* CFM_GEM */
                                        {0x032000, 2048}, /* MIB_DATA */
                                        {0x035000, 1024}, /* MAC_PON */
                                        {0x036000, 1024}, /* EPON_CTRL */
                                        {0x037000, 1024}, /* TBD */
                                        {0x038000, 1024}, /* RESERVED */
                                        #if 0
                                        {0x710000, 256}, /* NIC */
                                        {0x720000, 512}, /* NIC_DMA */
                                        {0x800000, 1792} /* NAT */
                                        #endif
                                        };
/*
 * Function Declaration
 */
static int32 check_ignore(uint32 addr)
{
    uint32 i;
    uint32 total_entry;

    total_entry = sizeof(ignore_addr_mask)/sizeof(reg_mask_t);
    for(i = 0; i < total_entry; i++)
    {
        if((addr >= ignore_addr_mask[i].addr) &&
           (addr < (ignore_addr_mask[i].addr + ((ignore_addr_mask[i].size)*4))))
            return 1;
    }
    return 0;
}

static int32 check_ignore_write(uint32 addr)
{
    uint32 i;
    uint32 total_entry;

    total_entry = sizeof(ignore_write_addr_mask)/sizeof(reg_mask_t);
    for(i = 0; i < total_entry; i++)
    {
        if((addr >= ignore_write_addr_mask[i].addr) &&
           (addr < (ignore_write_addr_mask[i].addr + ((ignore_write_addr_mask[i].size)*4))))
            return 1;
    }
    return 0;
}

int32 hal_reg_def_test(uint32 testcase)
{
	int32 test_entry=0, i, ret=RT_ERR_OK;
	uint32 read_data;

#if defined(CONFIG_SDK_APOLLO)
    osal_printf("default test: total entry %d\n", REG_MAP_ENTRY_NUM);

	/* default value test */
	for(i = 0; i < REG_MAP_ENTRY_NUM; i++)
	{
        if(0 == reg_map_def[i].write_bit)
            continue;

        if(1 == check_ignore(reg_map_def[i].reg_addr))
            continue;

		/*osal_printf("read addr=0x%x\n",reg_map_def[i].reg_addr);*/
		if(ioal_mem32_read(reg_map_def[i].reg_addr, &read_data) != RT_ERR_OK)
		{
			osal_printf("read addr 0x%x fail, err_code=0x%x\n",reg_map_def[i].reg_addr,ret);
			return RT_ERR_FAILED;
		}
		if((read_data & reg_map_def[i].write_bit) != (reg_map_def[i].reset_val & reg_map_def[i].write_bit))
		{
			osal_printf("default value mismatch, addr=0x%x, read_data=0x%x, default=0x%x, write_bit=0x%x\n",reg_map_def[i].reg_addr,read_data,reg_map_def[i].reset_val,reg_map_def[i].write_bit);
            ret = RT_ERR_FAILED;
		}
        test_entry++;
        osal_time_mdelay(10);
	}
    osal_printf("test entry %d\n",test_entry);
#endif

	return ret;
}

static int32 chk_protect(uint32 addr)
{
    uint32 protect_addr[] = {0x705014, 0x705020, 0x705024, 0x705028, 0x705040, 0x705200};
    int32 i, total_entry;

    total_entry = sizeof(protect_addr)/sizeof(uint32);

    for(i=0; i<total_entry; i++)
    {
        if( addr == protect_addr[i])
            return 1;
    }
    return 0;
}

int32 hal_reg_rw_test(uint32 testcase)
{

    int32 i, j, ret=RT_ERR_OK;
    uint32 read_data, write_data;
    int32 protect;
    uint32 tmp;
    int32 entry_number=0;
#if defined(CONFIG_SDK_APOLLO)
    osal_printf("read write test: total entry %d\n", REG_MAP_ENTRY_NUM);

    /* write value test */
    for(i = 0; i < REG_MAP_ENTRY_NUM; i++)
    {
        /*osal_printf("test addr=0x%x\n",reg_map_def[i].reg_addr);*/
        if(0 == reg_map_def[i].write_bit)
            continue;

        if(1 == check_ignore_write(reg_map_def[i].reg_addr))
            continue;

        protect = chk_protect(reg_map_def[i].reg_addr);

    	for(j=0;j<2;j++)
    	{
            if(j == 0)
            {
                write_data = reg_map_def[i].write_bit;
            }
    	    else
            {
                write_data = 0;
                entry_number++;
            }

            if(1 == protect)
            {
                tmp = GPON_REG_GTC_US_WRPROTECT_ENA;
                reg_write(GPON_GTC_US_WRITE_PROTECTr,&tmp);
            }


            if(ioal_mem32_write(reg_map_def[i].reg_addr, write_data) != RT_ERR_OK)
            {
                osal_printf("write addr 0x%x fail, data=0x%x, err_code=0x%x\n",reg_map_def[i].reg_addr,write_data,ret);
                return RT_ERR_FAILED;
            }

            if(1 == protect)
            {
                tmp = GPON_REG_GTC_US_WRPROTECT_DIS;
                reg_write(GPON_GTC_US_WRITE_PROTECTr,&tmp);
            }

            osal_time_mdelay(10);

            if(ioal_mem32_read(reg_map_def[i].reg_addr, &read_data) != RT_ERR_OK)
            {
                osal_printf("read addr 0x%x fail, err_code=0x%x\n",reg_map_def[i].reg_addr,ret);
                return RT_ERR_FAILED;
            }
            if((read_data & reg_map_def[i].write_bit) != write_data)
            {
                osal_printf("write value mismatch, addr=0x%x, write_data=0x%x, read_data=0x%x, write_bit=0x%x\n",
                            reg_map_def[i].reg_addr,write_data,read_data,reg_map_def[i].write_bit);
                ret = RT_ERR_FAILED;
            }
            osal_time_mdelay(10);
#if 0
            if(ioal_mem32_read(0x2209c, &read_data) != RT_ERR_OK)
            {
                osal_printf("read addr 0x2209c fail, err_code=0x%x\n",ret);
                return RT_ERR_FAILED;
            }
            else
            {
                if(read_data != 0xc)
                {
                    osal_printf("0x2209c != 0xc (0x%x), after 0x%x\n",read_data, reg_map_def[i].reg_addr);
                    return RT_ERR_FAILED;
                }
            }
            if(ioal_mem32_read(0x701000, &read_data) != RT_ERR_OK)
            {
                osal_printf("read addr 0x701000 fail, err_code=0x%x\n",ret);
                return RT_ERR_FAILED;
            }
            else
            {
                if(read_data == 0xca0eca0f)
                {
                    osal_printf("0x701000 = 0x%x, after 0x%x\n",read_data, reg_map_def[i].reg_addr);
                    return RT_ERR_FAILED;
                }
            }
#endif
    	}
    }

    osal_printf("test entry %d\n",entry_number);
#endif
	return ret;
}


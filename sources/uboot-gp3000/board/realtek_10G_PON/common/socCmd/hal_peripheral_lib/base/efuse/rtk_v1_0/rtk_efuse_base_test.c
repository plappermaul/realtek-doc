/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "basic_types.h"
#include "diag.h"
//#include "rtk_hal_efuse.h"
#include "section_config.h"
#include "rtk_efuse_base_test.h"
#include "rtl_utility.h"
#include "rand.h"

#include "peripheral.h"



VOID    
EFUSETestApp(
    IN  VOID    *EFUSETestData
){

        PEFUSE_VERI_PARA pEFUSEVerParaData = (PEFUSE_VERI_PARA)EFUSETestData;

        u32 data = pEFUSEVerParaData->Data;
        u32 mask = pEFUSEVerParaData->Mask;
        u32 port = pEFUSEVerParaData->DataPort;
        u32 index = pEFUSEVerParaData->DataIdx;

        switch(pEFUSEVerParaData->VeriItem)
        {
        case EFUSE_INIT_TEST:
                // useless in 8197G because we already design autload feature
                //load_efuse_data_to_reg();
        break;

        case EFUSE_READ_TEST:
                //read_efuse_byte(index);
        break;

	case EFUSE_WRITE_TEST:
                write_efuse_byte(data, mask, port, index);
        break;

        case EFUSE_DUMP_TEST:
                dump_efuse_reg_data();
        break;

        case EFUSE_ISR_TEST:
                efuse_isr_verification();
        break;

        default:
        break;
        }
	
}

//#include <stdio.h>

//#include "common/1.01a/DW_common.h"
#include "include/DW_common.h"
#include "include/spi_flash_private.h"

struct command_info cmd_info ={
				   DEF_RD_DUAL_TYPE,
                               DEF_RD_QUAD_TYPE,
                               DEF_WR_DUAL_TYPE,
                               DEF_WR_QUAD_TYPE,
                               DEF_RD_DUAL_DUMMY_CYCLE,
                               DEF_RD_QUAD_DUMMY_CYCLE,
                               DEF_RD_FAST_DUMMY_CYCLE,
                               DEF_RD_TUNING_DUMMY_CYCLE,
                               DEF_WR_BLOCK_BOUND
                               };


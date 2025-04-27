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
 * Purpose : The data for setting MAC Serdes parameters
 *
 * Feature : The data struct of  MAC Serdes parameters
 *
 */

#ifndef __DAL_MANGO_CONSTRUCT_H__
#define __DAL_MANGO_CONSTRUCT_H__

#include <dal/dal_construct.h>

/*
 * Symbol Definition
 */
#define MANGO_SWRED_QUEUE_DP0_HIGH_THRESH       78
#define MANGO_SWRED_QUEUE_DP0_LOW_THRESH        68
#define MANGO_SWRED_QUEUE_DP0_DROP_RATE         255
#define MANGO_SWRED_QUEUE_DP1_HIGH_THRESH       74
#define MANGO_SWRED_QUEUE_DP1_LOW_THRESH        64
#define MANGO_SWRED_QUEUE_DP1_DROP_RATE         255
#define MANGO_SWRED_QUEUE_DP2_HIGH_THRESH       70
#define MANGO_SWRED_QUEUE_DP2_LOW_THRESH        60
#define MANGO_SWRED_QUEUE_DP2_DROP_RATE         255
#define MANGO_FC_REPCT_Q_DROP_THR_H             78
#define MANGO_FC_REPCT_Q_DROP_THR_L             68
#define MANGO_FC_JUMBO_P_THR_GUAR               0
#define MANGO_FC_JUMBO_P_THR_IDX                3
#define MANGO_FC_JUMBO_P_THR_H_ON               200
#define MANGO_FC_JUMBO_P_THR_H_OFF              130
#define MANGO_FC_JUMBO_P_THR_L_ON               110
#define MANGO_FC_JUMBO_P_THR_L_OFF              35
#define MANGO_FC_JUMBO_S_THR_H_ON               3572
#define MANGO_FC_JUMBO_S_THR_H_OFF              3397
#define MANGO_FC_JUMBO_S_THR_L_ON               2510
#define MANGO_FC_JUMBO_S_THR_L_OFF              2335
#define MANGO_FC_OFF_JUMBO_P_THR_H_ON           200
#define MANGO_FC_OFF_JUMBO_P_THR_H_OFF          130
#define MANGO_FC_OFF_JUMBO_P_THR_L_ON           105
#define MANGO_FC_OFF_JUMBO_P_THR_L_OFF          35
#define MANGO_FC_OFF_JUMBO_S_THR_H_ON           2000
#define MANGO_FC_OFF_JUMBO_S_THR_H_OFF          1825
#define MANGO_FC_OFF_JUMBO_S_THR_L_ON           1000
#define MANGO_FC_OFF_JUMBO_S_THR_L_OFF          825
/* #define MANGO_FC_S_THR_H_OFF                    3472 */
#define MANGO_FC_P_THR_L_ON                     25
#define MANGO_IGBW_Q_DROP_THR_H                 220
#define MANGO_IGBW_Q_DROP_THR_L                 210
#define MANGO_JUMBO_EGBW_Q_THR_IDX              2
#define MANGO_JUMBO_EGBW_Q_DROP_THR_H           300
#define MANGO_JUMBO_EGBW_Q_DROP_THR_L           220
#define MANGO_IGRBW_LB_TICK_650M                53
#define MANGO_IGRBW_LB_TKN_650M                 171
#define MANGO_IGRBW_LB_TICK_325M                53
#define MANGO_IGRBW_LB_TKN_325M                 342
#define MANGO_IGRBW_LB_TICK_175M                123
#define MANGO_IGRBW_LB_TKN_175M                 1474
#define MANGO_EGRBW_LB_TICK_650M                53
#define MANGO_EGRBW_LB_TKN_650M                 171
#define MANGO_EGRBW_LB_TICK_325M                53
#define MANGO_EGRBW_LB_TKN_325M                 342
#define MANGO_EGRBW_LB_TICK_175M                62
#define MANGO_EGRBW_LB_TKN_175M                 743
#define MANGO_EGRBW_LB_CPU_PPS_TICK_650M        620
#define MANGO_EGRBW_LB_CPU_PPS_TKN_650M         1
#define MANGO_EGRBW_LB_CPU_PPS_TICK_325M        310
#define MANGO_EGRBW_LB_CPU_PPS_TKN_325M         1
#define MANGO_EGRBW_LB_CPU_PPS_TICK_175M        167
#define MANGO_EGRBW_LB_CPU_PPS_TKN_175M         1
#define MANGO_STORM_LB_TICK_650M                53
#define MANGO_STORM_LB_TKN_650M                 171
#define MANGO_STORM_LB_TICK_325M                53
#define MANGO_STORM_LB_TKN_325M                 342
#define MANGO_STORM_LB_TICK_175M                62
#define MANGO_STORM_LB_TKN_175M                 743
#define MANGO_STORM_LB_PPS_TICK_650M            39
#define MANGO_STORM_LB_PPS_TKN_650M             1
#define MANGO_STORM_LB_PPS_TICK_325M            77
#define MANGO_STORM_LB_PPS_TKN_325M             1
#define MANGO_STORM_LB_PPS_TICK_175M            42
#define MANGO_STORM_LB_PPS_TKN_175M             1
#define MANGO_STORM_LB_PROTO_TICK_650M          1269531
#define MANGO_STORM_LB_PROTO_TICK_325M          634766
#define MANGO_STORM_LB_PROTO_TICK_175M          341797

#define MANGO_METER_BPS_TICK_650M   53
#define MANGO_METER_BPS_TKN_650M    171
#define MANGO_METER_BPS_TICK_325M   53
#define MANGO_METER_BPS_TKN_325M    342
#define MANGO_METER_BPS_TICK_175M   123
#define MANGO_METER_BPS_TKN_175M    1474

#define MANGO_METER_PPS_TICK_650M   620
#define MANGO_METER_PPS_TKN_650M    1
#define MANGO_METER_PPS_TICK_325M   310
#define MANGO_METER_PPS_TKN_325M    1
#define MANGO_METER_PPS_TICK_175M   167
#define MANGO_METER_PPS_TKN_175M    1


/*
 * Data Declaration
 */
typedef enum dal_mango_rate_sysClk_e
{
    RATE_SYSCLK_650 = 0,
    RATE_SYSCLK_325,
    RATE_SYSCLK_175,
    RATE_SYSCLK_END
} dal_mango_rate_sysClk_t;

/*
 * Function Declaration
 */
void dal_mango_construct_default_init(uint32 unit);
int32 dal_mango_construct_macConfig_init(uint32 unit);
void dal_mango_construct_phyConfig_init(uint32 unit);
void dal_mango_construct_serdesConfig_init(uint32 unit);
void dal_mango_construct_pollingConfig_disable(uint32 unit);

/* Function Name:
 *      dal_mango_construct_pollingConfig_init
 * Description:
 *      Polling Configuration
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
dal_mango_construct_pollingConfig_init(uint32 unit);

/* Function Name:
 *      dal_mango_construct_phy_reset
 * Description:
 *      Reset PHY.
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
void dal_mango_construct_phy_reset(uint32 unit);

/* Function Name:
 *      dal_mango_construct_serdes_off
 * Description:
 *      Turn off SerDes mode
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
dal_mango_construct_serdes_off(uint32 unit);


/* Function Name:
 *      dal_mango_construct_macPollingPhy_init
 * Description:
 *      MAC Polling PHY config initialization
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
dal_mango_construct_macPollingPhy_init(uint32 unit);


#endif //__DAL_MANGO_CONSTRUCT_H__



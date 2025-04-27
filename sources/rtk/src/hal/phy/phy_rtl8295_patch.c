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
 * Purpose : PHY 8295 Driver APIs.
 *
 * Feature : PHY 8295 Driver APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <soc/type.h>
#include <hwp/hw_profile.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>
#include <hal/phy/construct/conftypes.h>
#include <hal/phy/rtl8295_reg_def.h>
#include <hal/phy/phy_rtl8295.h>
#include <hal/phy/phy_rtl8295_patch.h>
#include <hal/common/miim.h>
#include <osal/memory.h>
#include <osal/time.h>
#include <osal/lib.h>



#define RT_8295_LOG(level, module, fmt, args...)      RT_LOG(level, module, "%s:%d:"fmt, __FUNCTION__, __LINE__, ##args)


/*
 * Symbol Definition
 */
typedef enum {
    TAB0 = 0,
    TAB1,
    TAB2,
    TAB3,
    TAB4,
    TAB_END,
}phy_8295_tab_t;

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */
#define BITS_MASK(mBit, lBit)           (uint32)((((uint64)0x1 << (mBit - lBit + 1)) - 1) << lBit)


typedef struct phy_8295r_patch_info_s
{
    phy_8295r_txParam_t     tx_param[RTK_MAX_NUM_OF_PORTS][PORT_10GMEDIA_END];
    phy_8295_rxCaliConf_t   *prxCaliConf[RTK_MAX_NUM_OF_PORTS];
} phy_8295r_patch_info_t;


typedef struct phy_8214qf_patch_info_s
{
    phy_8295_rxCaliConf_t   *prxCaliConf[RTK_MAX_NUM_OF_PORTS];
} phy_8214qf_patch_info_t;

typedef struct phy_8295_rxCaliSds_s
{
    uint32          unit;
    uint32          port;
    uint32          mdxMacId;
    uint32          sdsId;
} phy_8295_rxCaliSds_t;


typedef struct phy_8295_rxCaliInfo_s
{
    phy_8295_rxCaliParam_t  param;

    struct
    {
        uint32          flag_recal;
    } status;
} phy_8295_rxCaliInfo_t;


phy_8295r_patch_info_t   gPhy8295rPI[RTK_MAX_NUM_OF_UNIT];
uint32                   phy8295rPatch_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};

phy_8214qf_patch_info_t  gPhy82x4qfPI[RTK_MAX_NUM_OF_UNIT];
uint32                   phy82x4qfPatch_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};

static uint32       patchFlowEnable = FALSE;
#define PATCH_FLOW_PRINT(fmt, args...)  do {                                                        \
                                            if (patchFlowEnable == TRUE)                            \
                                                osal_printf(fmt, ## args);                          \
                                        } while(0)


/*
 * Function Declaration
 */


static uint32       patchDbgEnable = 0;//PHY_8295_PATCH_DBG_OUT;


/* debug print */
#define PATCH_PRINT(fmt, args...)                                                                   \
            do {                                                                                    \
                if (patchDbgEnable & PHY_8295_PATCH_DBG_OUT)                                        \
                    osal_printf("%s,%d : "fmt"\n", "RX_CAL", __LINE__, ##args);                     \
            }while(0)

#define PHY_REG_WR(_unit, _macId, _phyPage, _phyReg, _data)                                         \
            do {                                                                                    \
                PATCH_FLOW_PRINT("WR-reg unit %u macId %u %u.%u 0x%X\n", _unit, _macId, _phyPage, _phyReg, _data);\
                phy_8295_phyReg_write(_unit, _macId, _phyPage, _phyReg, _data);                     \
            } while(0)

#define PHY_REG_FIELD_WR(_unit, _macId, _phyPage, _phyReg, _mBit, _lBit, _val)                      \
            do {                                                                                    \
                int32       _rv;                                                                    \
                uint32      _data;                                                                  \
                if ((_rv = phy_8295_phyReg_read(_unit, _macId, _phyPage, _phyReg, &_data)) != RT_ERR_OK)\
                {                                                                                   \
                    RT_ERR(_rv, (MOD_HAL), "unit %u macId %u %u.%u", _unit, _macId, _phyPage, _phyReg);\
                } else {                                                                            \
                    _data = REG32_FIELD_SET(_data, _val, _lBit, BITS_MASK(_mBit, _lBit));           \
                    PATCH_FLOW_PRINT("WR-reg unit %u macId %u %u.%u 0x%X\n", _unit, _macId, _phyPage, _phyReg, _data);\
                    phy_8295_phyReg_write(_unit, _macId, _phyPage, _phyReg, _data);                 \
                }                                                                                   \
            } while(0)

#define PHY_REG_RD(_unit, _macId, _phyPage, _phyReg, _pData)                                        \
            do {                                                                                    \
                int32      _rv;                                                                     \
                if ((_rv = phy_8295_phyReg_read(_unit, _macId, _phyPage, _phyReg, _pData)) != RT_ERR_OK) \
                {                                                                                   \
                    RT_ERR(_rv, (MOD_HAL), "unit %u macId %u RD %u.%u", _unit, _macId, _phyPage, _phyReg);\
                    *_pData = 0;                                                                    \
                }                                                                                   \
            } while(0)

#define PHY_REG_FIELD_RD(_unit, _macId, _phyPage, _phyReg, _mBit, _lBit, _pVal)                     \
            do {                                                                                    \
                int32       _rv;                                                                    \
                uint32      _data;                                                                  \
                if ((_rv = phy_8295_phyReg_read(_unit, _macId, _phyPage, _phyReg, &_data)) != RT_ERR_OK)\
                {                                                                                   \
                    RT_ERR(_rv, (MOD_HAL), "unit %u macId %u %u.%u", _unit, _macId, _phyPage, _phyReg);\
                    *_pVal = 0;                                                                     \
                } else {                                                                            \
                    *_pVal = REG32_FIELD_GET(_data, _lBit, BITS_MASK(_mBit, _lBit));                \
                }                                                                                   \
            } while(0)

#define PHY_SDS_WR(_unit, _macId, _sdsId, _pageIdx, _phyReg, _data)                                 \
            do {                                                                                    \
                PATCH_FLOW_PRINT("WR-sds unit %u macId %u S%u %u.%u 0x%X\n", _unit, _macId, _sdsId, (_pageIdx+PHY_8295_PAGE_BASE_OFFSET_S0), _phyReg, _data);\
                phy_8295_sdsReg_write(_unit, _macId, _sdsId, (_pageIdx+PHY_8295_PAGE_BASE_OFFSET_S0), _phyReg, _data);    \
            } while(0)

#define PHY_SDS_FIELD_WR(_unit, _macId, _sdsId, _pageIdx, _phyReg, _mBit, _lBit, _val)              \
            do {                                                                                    \
                int32       _rv;                                                                    \
                uint32      _data, _phyPage = _pageIdx + PHY_8295_PAGE_BASE_OFFSET_S0;              \
                if ((_rv = phy_8295_sdsReg_read(_unit, _macId, _sdsId, _phyPage, _phyReg, &_data)) != RT_ERR_OK)\
                {                                                                                   \
                    RT_ERR(_rv, (MOD_HAL), "unit %u macId %u S%u %u.%u", _unit, _macId, _sdsId, _phyPage, _phyReg);\
                } else {                                                                            \
                    _data = REG32_FIELD_SET(_data, _val, _lBit, BITS_MASK(_mBit, _lBit));           \
                    PATCH_FLOW_PRINT("WR-sds unit %u macId %u S%u %u.%u 0x%X\n", _unit, _macId, _sdsId, _phyPage, _phyReg, _data);\
                    phy_8295_sdsReg_write(_unit, _macId, _sdsId, _phyPage, _phyReg, _data);         \
                }                                                                                   \
            } while(0)

#define PHY_SDS_RD(_unit, _macId, _sdsId, _pageIdx, _phyReg, _pData)                                \
            do {                                                                                    \
                int32   _rv;                                                                        \
                uint32  _phyPage = _pageIdx + PHY_8295_PAGE_BASE_OFFSET_S0;                         \
                if ((_rv = phy_8295_sdsReg_read(_unit, _macId, _sdsId, _phyPage, _phyReg, _pData)) != RT_ERR_OK) \
                {                                                                                   \
                    RT_ERR(_rv, (MOD_HAL), "unit %u macId %u S%u RD %u.%u", _unit, _macId, _sdsId, _phyPage, _phyReg);\
                    *_pData = 0;                                                                    \
                }                                                                                   \
            } while(0)

#define PHY_SDS_FIELD_RD(_unit, _macId, _sdsId, _pageIdx, _phyReg, _mBit, _lBit, _pVal)             \
            do {                                                                                    \
                int32   _rv;                                                                        \
                uint32  _data, _phyPage = _pageIdx + PHY_8295_PAGE_BASE_OFFSET_S0;                  \
                if ((_rv = phy_8295_sdsReg_read(_unit, _macId, _sdsId, _phyPage, _phyReg, &_data)) != RT_ERR_OK) \
                {                                                                                   \
                    RT_ERR(_rv, (MOD_HAL), "unit %u macId %u S%u RD %u.%u", _unit, _macId, _sdsId, _phyPage, _phyReg);\
                    *_pVal = 0;                                                                     \
                } else {                                                                            \
                    *_pVal = REG32_FIELD_GET(_data, _lBit, BITS_MASK(_mBit, _lBit));                \
                }                                                                                   \
            } while(0)


#define DATA_BITS_GET(data, mBit, lBit)         REG32_FIELD_GET(data, lBit, BITS_MASK(mBit, lBit))
#define DATA_BITS_SET(data, val, mBit, lBit)    REG32_FIELD_SET(data, val, lBit, BITS_MASK(mBit, lBit))


/*
 * Function Declaration
 */
void _phy_8295_patch_a_init_rtl8214qf(uint32 unit, uint32 port, uint32 mdxMacId);
void _phy_8295_patch_a_init_rtl8295r(uint32 unit, uint32 port, uint32 mdxMacId);
void _phy_8295_rxCali_a_startall(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo);
void _phy_8295_rxCali_a_rx_cal_nodfe (phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo);
void _phy_8295_rxCali_a_DBG(phy_8295_rxCaliSds_t *ps);
void _phy_8295_rxCali_a_input_DCVS0(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 dcvs0);
void _phy_8295_rxCali_a_input_DCVS1(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 dcvs1);
void _phy_8295_rxCali_a_input_DCVS2(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 dcvs2);
void _phy_8295_rxCali_a_input_DCVS3(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 dcvs3);
void _phy_8295_rxCali_a_input_DCVS4(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 dcvs4);
void _phy_8295_rxCali_a_input_DCVS5(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 dcvs5);
void _phy_8295_rxCali_a_input_tap0(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 tap);
void _phy_8295_rxCali_a_input_vth(phy_8295_rxCaliSds_t *ps, uint32 vthp, uint32 vthn);
void _phy_8295_rxCali_a_input_tap1_even(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 tap);
void _phy_8295_rxCali_a_input_tap1_odd(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 tap);
void _phy_8295_rxCali_a_input_tap2_even(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 tap);
void _phy_8295_rxCali_a_input_tap2_odd(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 tap);
void _phy_8295_rxCali_a_input_tap3_even(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 tap);
void _phy_8295_rxCali_a_input_tap3_odd(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 tap);
void _phy_8295_rxCali_a_input_tap4_even(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 tap);
void _phy_8295_rxCali_a_input_tap4_odd(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 tap);
void _phy_8295_rxCali_a_Readtap0(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo, uint32 *psign_out, uint32 *pcoef_bin, uint32 *pVthp, uint32 *pVthn);
void _phy_8295_rxCali_a_Othertap(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo, uint32 *psign_out_Even, uint32 *pcoef_bin_Even, uint32 *psign_out_Odd, uint32 *pcoef_bin_Odd);
void _phy_8295_rxCali_a_dump_leq(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo, uint32 *pLeq);
void _phy_8295_rxCali_a_servo(phy_8295_rxCaliSds_t *ps);
void _phy_8295_rxCali_a_start_1d1d1(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo);
void _phy_8295_rxCali_a_start_1d2(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo);
void _phy_8295_rxCali_a_start_1d2d3(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo, uint32 *fgcal_offset_binary);
void _phy_8295_rxCali_a_FGCAL_chk(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo, uint32 fgcal_offset_binary);
void _phy_8295_rxCali_a_start_1d2d4(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo);
void _phy_8295_rxCali_a_start_1d3d1(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo);
void _phy_8295_rxCali_a_start_1d4(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo);
void _phy_8295_rxCali_a_start_1d5d2(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo);
void _phy_8295_rxCali_a_eye_scan(phy_8295_rxCaliSds_t *ps, int32 eye_int);
void _phy_8295_patch_bT_init_chip(uint32 unit, uint32 port, uint32 mdxMacId);
void _phy_8295_patch_bT_sds_rst(uint32 unit, uint32 port, uint32 mdxMacId);
void _phy_8295_patch_bT_DBG(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId);
void _phy_8295_patch_bT_dump_leq(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 *pLeq);
void _phy_8295_patch_bT_Readtap0(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 *psign_out, uint32 *pcoef_bin, uint32 *pVthp, uint32 *pVthn);
void _phy_8295_patch_bT_Othertap(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 *psign_out_Even, uint32 *pcoef_bin_Even, uint32 *psign_out_Odd, uint32 *pcoef_bin_Odd);
void _phy_8295_patch_bT_input_tap0(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 tap);
void _phy_8295_patch_bT_input_tap1_even(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 tap);
void _phy_8295_patch_bT_input_tap1_odd(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 tap);
void _phy_8295_patch_bT_input_tap2_even(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 tap);
void _phy_8295_patch_bT_input_tap2_odd(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 tap);
void _phy_8295_patch_bT_input_tap3_even(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 tap);
void _phy_8295_patch_bT_input_tap3_odd(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 tap);
void _phy_8295_patch_bT_input_tap4_even(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 tap);
void _phy_8295_patch_bT_input_tap4_odd(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 tap);
void _phy_8295_patch_bT_input_DCVS0(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 dcvs0);
void _phy_8295_patch_bT_input_DCVS1(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 dcvs1);
void _phy_8295_patch_bT_input_DCVS2(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 dcvs2);
void _phy_8295_patch_bT_input_DCVS3(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 dcvs3);
void _phy_8295_patch_bT_input_DCVS4(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 dcvs4);
void _phy_8295_patch_bT_input_DCVS5(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 dcvs5);
void _phy_8295_patch_bT_FGCAL_chk(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 fgcal_offset_binary);
void _phy_8295_patch_bT_start_1d1d1(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 *adj_tx_short);
void _phy_8295_patch_bT_start_1d2(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId);
void _phy_8295_patch_bT_start_1d2d3(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 *fgcal_offset_binary);
void _phy_8295_patch_bT_start_1d2d4(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId);
void _phy_8295_patch_bT_start_1d3d1(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 *pShort_flag);
void _phy_8295_patch_bT_start_1d4(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId);
void _phy_8295_patch_bT_start_1d5d2(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId);
void _phy_8295_patch_bT_input_vth(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 vthp, uint32 vthn);
void _phy_8295_patch_bT_servo(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId);
void _phy_8295_patch_bT_rx_cal_nodfe(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 *pAdj_tx_short, uint32 *pShort_flag);
void _phy_8295_patch_bT_init_sfp(uint32 unit, uint32 port, uint32 mdxMacId);
void _phy_8295_patch_bT_startall(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId);



void
_phy_8295_patch_bT_init_chip(uint32 unit, uint32 port, uint32 mdxMacId)
{
    /* <<< init_chip >>> */


    PHY_REG_WR(unit, mdxMacId, 1, 20, 0x3898);
    PHY_SDS_WR(unit, mdxMacId, 0, 24, 16, 0x0);
    PHY_SDS_WR(unit, mdxMacId, 1, 24, 16, 0x0);
    PHY_SDS_WR(unit, mdxMacId, 0, 125, 18, 0x0130);
    PHY_SDS_WR(unit, mdxMacId, 1, 125, 18, 0x0130);


    PHY_REG_WR(unit, mdxMacId, 28, 19, 0x8000);
    PHY_REG_WR(unit, mdxMacId, 33, 23, 0x8000);
    PHY_REG_WR(unit, mdxMacId, 44, 23, 0x8000);


    PHY_SDS_WR(unit, mdxMacId, 0, 0, 18, 0x30E0);
    PHY_SDS_WR(unit, mdxMacId, 1, 0, 18, 0x30E0);
    PHY_SDS_WR(unit, mdxMacId, 4, 0, 18, 0x30E0);
    PHY_SDS_WR(unit, mdxMacId, 5, 0, 18, 0x30E0);
    PHY_SDS_WR(unit, mdxMacId, 6, 0, 18, 0x30E0);
    PHY_SDS_WR(unit, mdxMacId, 7, 0, 18, 0x30E0);


    PHY_SDS_WR(unit, mdxMacId, 0, 132, 16, 0xE150);
    PHY_SDS_WR(unit, mdxMacId, 1, 132, 16, 0xE150);
    PHY_SDS_WR(unit, mdxMacId, 0, 190, 20, 0x41BC);
    PHY_SDS_WR(unit, mdxMacId, 0, 190, 21, 0x3DBC);
    PHY_SDS_WR(unit, mdxMacId, 1, 190, 20, 0x41BC);
    PHY_SDS_WR(unit, mdxMacId, 1, 190, 21, 0x3DBC);

    PHY_SDS_WR(unit, mdxMacId, 0, 132, 17, 0x9090);
    PHY_SDS_WR(unit, mdxMacId, 0, 132, 21, 0x4A8D);
    PHY_SDS_WR(unit, mdxMacId, 0, 184, 19, 0xF000);
    PHY_SDS_WR(unit, mdxMacId, 0, 185, 23, 0xBFDF);
    PHY_SDS_WR(unit, mdxMacId, 0, 186, 20, 0x1178);
    PHY_SDS_WR(unit, mdxMacId, 0, 187, 22, 0x07FF);
    PHY_SDS_WR(unit, mdxMacId, 0, 188, 18, 0x114F);
    PHY_SDS_WR(unit, mdxMacId, 0, 188, 21, 0x787C);
    PHY_SDS_WR(unit, mdxMacId, 0, 191, 16, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 0, 191, 17, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 0, 191, 19, 0x0000);
    PHY_SDS_WR(unit, mdxMacId, 0, 191, 21, 0x0019);
    PHY_SDS_WR(unit, mdxMacId, 0, 191, 23, 0xF0E0);

    PHY_SDS_WR(unit, mdxMacId, 0, 189, 19, 0x7C0F);
    PHY_SDS_WR(unit, mdxMacId, 1, 132, 17, 0x9090);
    PHY_SDS_WR(unit, mdxMacId, 1, 132, 21, 0x4A8D);
    PHY_SDS_WR(unit, mdxMacId, 1, 184, 19, 0xF000);
    PHY_SDS_WR(unit, mdxMacId, 1, 185, 23, 0xBFDF);
    PHY_SDS_WR(unit, mdxMacId, 1, 186, 20, 0x1178);
    PHY_SDS_WR(unit, mdxMacId, 1, 187, 22, 0x07FF);
    PHY_SDS_WR(unit, mdxMacId, 1, 188, 18, 0x114F);
    PHY_SDS_WR(unit, mdxMacId, 1, 188, 21, 0x787C);
    PHY_SDS_WR(unit, mdxMacId, 1, 191, 16, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 1, 191, 17, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 1, 191, 19, 0x0000);
    PHY_SDS_WR(unit, mdxMacId, 1, 191, 21, 0x0019);
    PHY_SDS_WR(unit, mdxMacId, 1, 191, 23, 0xF0E0);

    PHY_SDS_WR(unit, mdxMacId, 1, 189, 19, 0x7C0F);



    PHY_SDS_WR(unit, mdxMacId, 0, 175, 19, 0x0000);
    PHY_SDS_WR(unit, mdxMacId, 0, 175, 21, 0x0011);
    PHY_SDS_WR(unit, mdxMacId, 0, 175, 23, 0x0F0F);

    PHY_SDS_WR(unit, mdxMacId, 4, 132, 17, 0xF807);
    PHY_SDS_WR(unit, mdxMacId, 4, 144, 21, 0x981B);
    PHY_SDS_WR(unit, mdxMacId, 4, 146, 16, 0xF807);
    PHY_SDS_WR(unit, mdxMacId, 4, 160, 21, 0x981B);
    PHY_SDS_WR(unit, mdxMacId, 4, 162, 16, 0xFA07);

    PHY_SDS_WR(unit, mdxMacId, 5, 144, 21, 0x981B);
    PHY_SDS_WR(unit, mdxMacId, 5, 146, 16, 0xF807);
    PHY_SDS_WR(unit, mdxMacId, 5, 160, 21, 0x981B);
    PHY_SDS_WR(unit, mdxMacId, 5, 162, 16, 0xFA07);

    PHY_SDS_WR(unit, mdxMacId, 6, 132, 17, 0xFA07);
    PHY_SDS_WR(unit, mdxMacId, 6, 144, 21, 0x981B);
    PHY_SDS_WR(unit, mdxMacId, 6, 146, 16, 0xF807);
    PHY_SDS_WR(unit, mdxMacId, 6, 160, 21, 0x981B);
    PHY_SDS_WR(unit, mdxMacId, 6, 162, 16, 0xFA07);

    PHY_SDS_WR(unit, mdxMacId, 7, 144, 21, 0x981B);
    PHY_SDS_WR(unit, mdxMacId, 7, 146, 16, 0xF807);
    PHY_SDS_WR(unit, mdxMacId, 7, 160, 21, 0x981B);
    PHY_SDS_WR(unit, mdxMacId, 7, 162, 16, 0xFA07);

    PHY_SDS_FIELD_WR(unit, mdxMacId, 4, 144, 19, 6, 2, 0x18);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 5, 144, 19, 6, 2, 0x18);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 6, 144, 19, 6, 2, 0x18);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 7, 144, 19, 6, 2, 0x18);

    PHY_SDS_FIELD_WR(unit, mdxMacId, 4, 145, 20, 15, 12, 0xE);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 5, 145, 20, 15, 12, 0xE);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 6, 145, 20, 15, 12, 0xE);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 7, 145, 20, 15, 12, 0xE);

    /* set SP_CFG_EN_LINK_FIB1G for enable fiberNwayForceLink */
    PHY_SDS_FIELD_WR(unit, mdxMacId, 4, 0, 20, 2, 2, 1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 5, 0, 20, 2, 2, 1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 6, 0, 20, 2, 2, 1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 7, 0, 20, 2, 2, 1);

    _phy_8295_patch_bT_sds_rst(unit, port, mdxMacId);

    PATCH_PRINT("Initialize Chip patch....Done!");
}


#if defined(CONFIG_SDK_RTL8214QF)
void
_phy_8295_patch_a_init_rtl8214qf(uint32 unit, uint32 port, uint32 mdxMacId)
{
    /* init_rtl8214qf */
    uint32      val;

    PHY_REG_FIELD_RD(unit, mdxMacId, 0, 19, 8, 4, &val);
    if (val == 0)
    {
        PHY_SDS_FIELD_WR(unit, mdxMacId, 0, 25, 21, 11, 8, 0xF);
        PHY_SDS_FIELD_WR(unit, mdxMacId, 1, 25, 21, 11, 8, 0xF);
    }

    PHY_SDS_FIELD_WR(unit, mdxMacId, 0, 27, 17, 6, 6, 0);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 1, 27, 17, 6, 6, 0);

    PHY_REG_WR(unit, mdxMacId, 1, 22, 0x5800);

    PHY_SDS_WR(unit, mdxMacId, 0, 132, 16, 0xE350);

    PHY_SDS_WR(unit, mdxMacId, 0, 132, 21, 0x420D);

    PHY_SDS_WR(unit, mdxMacId, 0, 186, 18, 0x2080);
    PHY_SDS_WR(unit, mdxMacId, 0, 186, 20, 0x1378);
    PHY_SDS_WR(unit, mdxMacId, 0, 189, 19, 0x7C07);
    PHY_SDS_WR(unit, mdxMacId, 0, 190, 20, 0x41B8);
    PHY_SDS_WR(unit, mdxMacId, 0, 190, 21, 0x3DB8);

    if (val == 0)
    {
        PHY_SDS_WR(unit, mdxMacId, 0, 188, 17, 0x91CC);
        PHY_SDS_WR(unit, mdxMacId, 0, 188, 18, 0x116F);
    }

    PHY_SDS_WR(unit, mdxMacId, 0, 187, 22, 0x07FA);


    PHY_SDS_WR(unit, mdxMacId, 0, 168, 19, 0xF000);
    PHY_SDS_WR(unit, mdxMacId, 0, 169, 23, 0xBFDF);
    PHY_SDS_WR(unit, mdxMacId, 0, 170, 18, 0x2100);
    PHY_SDS_WR(unit, mdxMacId, 0, 170, 20, 0x1378);
    PHY_SDS_WR(unit, mdxMacId, 0, 171, 22, 0x07FA);
    PHY_SDS_WR(unit, mdxMacId, 0, 172, 18, 0x114F);
    PHY_SDS_WR(unit, mdxMacId, 0, 172, 21, 0x787C);
    PHY_SDS_WR(unit, mdxMacId, 0, 175, 16, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 0, 175, 17, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 0, 175, 21, 0x0019);
    PHY_SDS_WR(unit, mdxMacId, 0, 175, 22, 0xF0E0);

    PHY_SDS_WR(unit, mdxMacId, 1, 132, 16, 0xE350);
    PHY_SDS_WR(unit, mdxMacId, 1, 132, 21, 0x420D);

    PHY_SDS_WR(unit, mdxMacId, 1, 186, 18, 0x2080);
    PHY_SDS_WR(unit, mdxMacId, 1, 186, 20, 0x1378);
    PHY_SDS_WR(unit, mdxMacId, 1, 189, 19, 0x7C07);
    PHY_SDS_WR(unit, mdxMacId, 1, 190, 20, 0x41B8);
    PHY_SDS_WR(unit, mdxMacId, 1, 190, 21, 0x3DB8);

    if (val == 0)
    {
        PHY_SDS_WR(unit, mdxMacId, 1, 188, 17, 0x91CC);
        PHY_SDS_WR(unit, mdxMacId, 1, 188, 18, 0x116F);
    }

    PHY_SDS_WR(unit, mdxMacId, 1, 187, 22, 0x07FA);

    PHY_SDS_WR(unit, mdxMacId, 1, 168, 19, 0xF000);
    PHY_SDS_WR(unit, mdxMacId, 1, 169, 23, 0xBFDF);
    PHY_SDS_WR(unit, mdxMacId, 1, 170, 18, 0x2100);
    PHY_SDS_WR(unit, mdxMacId, 1, 170, 20, 0x1378);
    PHY_SDS_WR(unit, mdxMacId, 1, 171, 22, 0x07FA);
    PHY_SDS_WR(unit, mdxMacId, 1, 172, 18, 0x114F);
    PHY_SDS_WR(unit, mdxMacId, 1, 172, 21, 0x787C);
    PHY_SDS_WR(unit, mdxMacId, 1, 175, 16, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 1, 175, 17, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 1, 175, 21, 0x0019);
    PHY_SDS_WR(unit, mdxMacId, 1, 175, 22, 0xF0E0);

    PHY_SDS_FIELD_WR(unit, mdxMacId, 4, 145, 20, 15, 12, 0xe);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 5, 145, 20, 15, 12, 0xe);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 6, 145, 20, 15, 12, 0xe);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 7, 145, 20, 15, 12, 0xe);

    PHY_SDS_FIELD_WR(unit, mdxMacId, 4, 144, 19, 6, 2, 0x16);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 5, 144, 19, 6, 2, 0x16);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 6, 144, 19, 6, 2, 0x16);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 7, 144, 19, 6, 2, 0x16);

    /* set SP_CFG_EN_LINK_FIB1G for enable fiberNwayForceLink */
    PHY_SDS_FIELD_WR(unit, mdxMacId, 4, 0, 20, 2, 2, 1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 5, 0, 20, 2, 2, 1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 6, 0, 20, 2, 2, 1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 7, 0, 20, 2, 2, 1);

    PHY_SDS_WR(unit, mdxMacId, 0, 174, 20, 0x4144);
    PHY_SDS_WR(unit, mdxMacId, 0, 174, 21, 0x3d44);
    PHY_SDS_WR(unit, mdxMacId, 0, 128, 16, 0x400);
    PHY_SDS_WR(unit, mdxMacId, 0, 128, 16, 0xc00);
    PHY_SDS_WR(unit, mdxMacId, 0, 128, 16, 0x0);


    PHY_SDS_WR(unit, mdxMacId, 0, 132, 16, 0xE150);
    PHY_SDS_WR(unit, mdxMacId, 0, 132, 18, 0x1CA4);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 0, 132, 19, 4, 2, 0x4);

    PHY_SDS_WR(unit, mdxMacId, 0, 190, 20, 0x8389);
    PHY_SDS_WR(unit, mdxMacId, 0, 190, 21, 0x3F89);
    PHY_SDS_WR(unit, mdxMacId, 1, 190, 20, 0x8389);
    PHY_SDS_WR(unit, mdxMacId, 1, 190, 21, 0x3F89);

    PHY_SDS_WR(unit, mdxMacId, 0, 174, 20, 0x8294);
    PHY_SDS_WR(unit, mdxMacId, 0, 174, 21, 0x3E94);
    PHY_SDS_WR(unit, mdxMacId, 1, 174, 20, 0x8294);
    PHY_SDS_WR(unit, mdxMacId, 1, 174, 21, 0x3E94);

    PHY_SDS_WR(unit, mdxMacId, 0, 150, 20, 0x8294);
    PHY_SDS_WR(unit, mdxMacId, 0, 150, 21, 0x3E94);
    PHY_SDS_WR(unit, mdxMacId, 1, 150, 20, 0x8294);
    PHY_SDS_WR(unit, mdxMacId, 1, 150, 21, 0x3E94);

    PHY_REG_WR(unit, mdxMacId, 1, 21, 0x6);

}
#endif

#if defined(CONFIG_SDK_RTL8214QF)
void
_phy_8295_patch_8214qf_config(uint32 unit, uint32 basePort, uint32 mdxMacId)
{
     uint8      sdsId;
     uint32     val;
     uint32     phy_port;

    if (RTK_PHYTYPE_RTL8214QF_NC5 == HWP_PHY_MODEL_BY_PORT(unit, basePort))
    {
        /* disable serdes */
        PHY_REG_WR(unit, mdxMacId, 5, 20, 0x1F);
        PHY_REG_WR(unit, mdxMacId, 5, 21, 0x1F);
        PHY_REG_WR(unit, mdxMacId, 5, 22, 0x1F);
        PHY_REG_WR(unit, mdxMacId, 5, 23, 0x1F);
    }

    /* disable port */
    for (phy_port = 0; phy_port < HWP_PHY_BASE_PHY_MAX(unit, basePort); phy_port++)
    {
        phy_8214qf_enable_set(unit, (basePort+phy_port), DISABLED);
    }

    /* enable 1G auto nego */
    PHY_SDS_FIELD_WR(unit, mdxMacId, 4, 8, 16, 12, 12, 0x1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 5, 8, 16, 12, 12, 0x1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 6, 8, 16, 12, 12, 0x1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 7, 8, 16, 12, 12, 0x1);

    /* Inverse S0 TX/RX polarity when the 14QF is Cascade Slave */
    PHY_REG_FIELD_RD(unit, mdxMacId, 0, 17, 3, 0, &val);
    if (val == 0xB) /* BOND_MODE_SEL is in C3 - Cascade mode */
    {
        /* Inverse S0 TX/RX polarity */
        PHY_SDS_FIELD_WR(unit, mdxMacId, 0, 0, 16, 9, 9, 0x1);
        PHY_SDS_FIELD_WR(unit, mdxMacId, 0, 0, 16, 8, 8, 0x1);
    }
    else
    {
        /* 14QF directly connects to MAC, change polarity according to hardware profile */
        sdsId = HWP_PORT_SDSID(unit, basePort);
        if (SERDES_POLARITY_CHANGE == HWP_SDS_RX_POLARITY(unit, sdsId))
        {
            PHY_SDS_FIELD_WR(unit, mdxMacId, 0, 0, 16, 9, 9, 0x1);
        }
        if (SERDES_POLARITY_CHANGE == HWP_SDS_TX_POLARITY(unit, sdsId))
        {
            PHY_SDS_FIELD_WR(unit, mdxMacId, 0, 0, 16, 8, 8, 0x1);
        }
    }

    if (HWP_8380_30_FAMILY(unit))
    {
        PHY_SDS_FIELD_WR(unit, mdxMacId, 0, 0, 16, 15, 15, 0x1);
    }

}
#endif

void
_phy_8295_patch_bT_sds_rst(uint32 unit, uint32 port, uint32 mdxMacId)
{

    PHY_REG_WR(unit, mdxMacId, 1, 21, 0x6);
}


void
_phy_8295_patch_bT_DBG(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId)
{


    PHY_SDS_FIELD_WR(unit, mdxMacId, 0, 124, 18, 4, 4, 1);


    PHY_SDS_FIELD_WR(unit, mdxMacId, 0, 3, 20, 9, 0, 0x0);


    switch(sdsId)
    {
      case 0:
        PHY_REG_WR(unit, mdxMacId, 0, 23, 0x2000);
        PHY_SDS_WR(unit, mdxMacId, 0, 124, 18, 0x0016);
        break;
      case 1:
        PHY_REG_WR(unit, mdxMacId, 0, 23, 0x2000);
        PHY_SDS_WR(unit, mdxMacId, 0, 124, 18, 0x0018);
        break;
    }
}

void
_phy_8295_patch_bT_dump_leq(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 *pLeq)
{
    uint32      leq_cali;

    _phy_8295_patch_bT_DBG(unit, port, mdxMacId, sdsId);
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 17, 5, 0, 0x10);
    //#==============================================================
    //#LEQ Read Out
    PHY_REG_FIELD_RD(unit, mdxMacId, 1, 16, 7, 3, &leq_cali);
    //#==============================================================
    PATCH_PRINT(" leq_cali_S%u = %u", sdsId, leq_cali);

    //# dis RX_EN_TEST
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 0);
    *pLeq = leq_cali;
}

void
_phy_8295_patch_bT_Readtap0(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId,
                         uint32 *psign_out, uint32 *pcoef_bin,
                         uint32 *pVthp, uint32 *pVthn)
{
    uint32      data;


    _phy_8295_patch_bT_DBG(unit, port, mdxMacId, sdsId);

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 17, 5, 0, 0x20);
    //#==============================================================
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 20, 5, 0, 0);
    //#==============================================================

    PHY_REG_FIELD_RD(unit, mdxMacId, 1, 16, 5, 0, &data);
    *psign_out = DATA_BITS_GET(data, 5, 5);
    *pcoef_bin = DATA_BITS_GET(data, 4, 0);
    //#===================================================
    PATCH_PRINT(" tap0_sign_out = 0x%X", *psign_out);
    PATCH_PRINT(" tap0_coef_bin = 0x%X", *pcoef_bin);
    //#===================================================

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 0);

    _phy_8295_patch_bT_DBG(unit, port, mdxMacId, sdsId);

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 17, 5, 0, 0x20);
    //#==============================================================
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 20, 5, 0, 0xC);
    //#==============================================================

    PHY_REG_RD(unit, mdxMacId, 1, 16, &data);
    *pVthn = DATA_BITS_GET(data, 5, 3);
    *pVthp = DATA_BITS_GET(data, 2, 0);
    //#==============================================================
    PATCH_PRINT(" vthn_set_bin = 0x%04X", *pVthn);
    PATCH_PRINT(" vthp_set_bin = 0x%04X", *pVthp);

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 0);

}

void
_phy_8295_patch_bT_Othertap(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId,
                         uint32 *psign_out_Even, uint32 *pcoef_bin_Even,
                         uint32 *psign_out_Odd,   uint32 *pcoef_bin_Odd)
{
    uint32          data;


    _phy_8295_patch_bT_DBG(unit, port, mdxMacId, sdsId);

    //#==============================================================

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 17, 5, 0, 0x20);
    //#==============================================================
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 20, 5, 0, 0x1);
    //#==============================================================

    PHY_REG_RD(unit, mdxMacId, 1, 16, &data);
    psign_out_Even[TAB1] = DATA_BITS_GET(data, 6, 6);
    pcoef_bin_Even[TAB1] = DATA_BITS_GET(data, 5, 0);
    PATCH_PRINT(" tap1_sign_out_Even = 0x%X", psign_out_Even[TAB1]);
    PATCH_PRINT(" tap1_coef_bin_Even = 0x%X", pcoef_bin_Even[TAB1]);
    //#==============================================================
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 20, 5, 0, 0x6);
    //#==============================================================

    PHY_REG_RD(unit, mdxMacId, 1, 16, &data);
    psign_out_Odd[TAB1] = DATA_BITS_GET(data, 6, 6);
    pcoef_bin_Odd[TAB1] = DATA_BITS_GET(data, 5, 0);
    //#==============================================================
    PATCH_PRINT(" tap1_sign_out_Odd = 0x%X", psign_out_Odd[TAB1]);
    PATCH_PRINT(" tap1_coef_bin_Odd = 0x%X", pcoef_bin_Odd[TAB1]);


    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 0);
    //#==============================================================

    _phy_8295_patch_bT_DBG(unit, port, mdxMacId, sdsId);

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 17, 5, 0, 0x20);
    //#==============================================================
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 20, 5, 0, 0x2);
    //#==============================================================

    PHY_REG_RD(unit, mdxMacId, 1, 16, &data);
    psign_out_Even[TAB2] = DATA_BITS_GET(data, 5, 5);
    pcoef_bin_Even[TAB2] = DATA_BITS_GET(data, 4, 0);
    //#==============================================================
    PATCH_PRINT(" tap2_sign_out_Even = 0x%X", psign_out_Even[TAB2]);
    PATCH_PRINT(" tap2_coef_bin_Even = 0x%X", pcoef_bin_Even[TAB2]);
    //#==============================================================
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 20, 5, 0, 0x7);
    //#==============================================================

    PHY_REG_RD(unit, mdxMacId, 1, 16, &data);
    psign_out_Odd[TAB2] = DATA_BITS_GET(data, 5, 5);
    pcoef_bin_Odd[TAB2] = DATA_BITS_GET(data, 4, 0);
    //#==============================================================
    PATCH_PRINT(" tap2_sign_out_Odd = 0x%X", psign_out_Odd[TAB2]);
    PATCH_PRINT(" tap2_coef_bin_Odd = 0x%X", pcoef_bin_Odd[TAB2]);
    //#==============================================================

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 0);
    //#==============================================================

    _phy_8295_patch_bT_DBG(unit, port, mdxMacId, sdsId);


    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 17, 5, 0, 0x20);
    //#==============================================================
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 20, 5, 0, 0x3);
    //#==============================================================

    PHY_REG_RD(unit, mdxMacId, 1, 16, &data);
    psign_out_Even[TAB3] = DATA_BITS_GET(data, 5, 5);
    pcoef_bin_Even[TAB3] = DATA_BITS_GET(data, 4, 0);
    //#==============================================================
    PATCH_PRINT(" tap3_sign_out_Even = 0x%X", psign_out_Even[TAB3]);
    PATCH_PRINT(" tap3_coef_bin_Even = 0x%X", pcoef_bin_Even[TAB3]);
    //#==============================================================
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 20, 5, 0, 0x8);
    //#==============================================================

    PHY_REG_RD(unit, mdxMacId, 1, 16, &data);
    psign_out_Odd[TAB3] = DATA_BITS_GET(data, 5, 5);
    pcoef_bin_Odd[TAB3] = DATA_BITS_GET(data, 4, 0);
    //#==============================================================
    PATCH_PRINT(" tap3_sign_out_Odd = 0x%X", psign_out_Odd[TAB3]);
    PATCH_PRINT(" tap3_coef_bin_Odd = 0x%X", pcoef_bin_Odd[TAB3]);
    //#==============================================================

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 0);
    //#==============================================================

    _phy_8295_patch_bT_DBG(unit, port, mdxMacId, sdsId);


    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 17, 5, 0, 0x20);
    //#==============================================================
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 20, 5, 0, 0x4);
    //#==============================================================

    PHY_REG_RD(unit, mdxMacId, 1, 16, &data);
    psign_out_Even[TAB4] = DATA_BITS_GET(data, 5, 5);
    pcoef_bin_Even[TAB4] = DATA_BITS_GET(data, 4, 0);
    //#==============================================================
    PATCH_PRINT(" tap4_sign_out_Even = 0x%X", psign_out_Even[TAB4]);
    PATCH_PRINT(" tap4_coef_bin_Even = 0x%X", pcoef_bin_Even[TAB4]);
    //#==============================================================
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 20, 5, 0, 0x9);
    //#==============================================================


    PHY_REG_RD(unit, mdxMacId, 1, 16, &data);
    psign_out_Odd[TAB4] = DATA_BITS_GET(data, 5, 5);
    pcoef_bin_Odd[TAB4] = DATA_BITS_GET(data, 4, 0);
    //#==============================================================
    PATCH_PRINT(" tap4_sign_out_Odd = 0x%X", psign_out_Odd[TAB4]);
    PATCH_PRINT(" tap4_coef_bin_Odd = 0x%X", pcoef_bin_Odd[TAB4]);
    //#==============================================================

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 0);
    //#==============================================================
}

void
_phy_8295_patch_bT_input_tap0(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 tap)
{

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 188, 19, 5, 0, ((sign << 5) + tap));
}

void
_phy_8295_patch_bT_input_tap1_even(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 tap)
{
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 184, 19, 11, 6, ((sign << 5) + tap));
}

void
_phy_8295_patch_bT_input_tap1_odd(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 tap)
{
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 184, 21, 5, 0, ((sign << 5) + tap));
}

void
_phy_8295_patch_bT_input_tap2_even(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 tap)
{
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 184, 19, 5, 0, ((sign << 5) + tap));
}

void
_phy_8295_patch_bT_input_tap2_odd(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 tap)
{
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 184, 21, 11, 6, ((sign << 5) + tap));
}

void
_phy_8295_patch_bT_input_tap3_even(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 tap)
{
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 184, 20, 11, 6, ((sign << 5) + tap));
}

void
_phy_8295_patch_bT_input_tap3_odd(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 tap)
{
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 184, 22, 5, 0, ((sign << 5) + tap));
}

void
_phy_8295_patch_bT_input_tap4_even(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 tap)
{
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 184, 20, 5, 0, ((sign << 5) + tap));
}

void
_phy_8295_patch_bT_input_tap4_odd(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 tap)
{
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 184, 22, 11, 6, ((sign << 5) + tap));
}



void
_phy_8295_patch_bT_input_DCVS0(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 dcvs0)
{
    uint32      data;
    data = (sign << 4) + dcvs0;
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 187, 20, 4, 0, data);
}

void
_phy_8295_patch_bT_input_DCVS1(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 dcvs1)
{
    uint32      data;
    data = (sign << 4) + dcvs1;
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 187, 21, 15, 11, data);
}

void
_phy_8295_patch_bT_input_DCVS2(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 dcvs2)
{
    uint32      data;
    data = (sign << 4) + dcvs2;
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 187, 21, 10, 6, data);
}

void
_phy_8295_patch_bT_input_DCVS3(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 dcvs3)
{
    uint32      data;
    data = (sign << 4) + dcvs3;
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 187, 21, 5, 1, data);
}

void
_phy_8295_patch_bT_input_DCVS4(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 dcvs4)
{
    uint32      data;
    data = (sign << 4) + dcvs4;
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 191, 16, 14, 10, data);
}

void
_phy_8295_patch_bT_input_DCVS5(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 sign, uint32 dcvs5)
{
    uint32      data;
    data = (sign << 4) + dcvs5;
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 191, 17, 14, 10, data);
}

void
_phy_8295_patch_bT_FGCAL_chk(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 fgcal_offset_binary)
{
    /* <<< FGCAL_chk >>> */
    uint32      data;
    uint32      FGCAL_flag;
    uint32      offset_range;


    //#===============================================================================================

    FGCAL_flag = ((fgcal_offset_binary > 60) || (fgcal_offset_binary < 4)) ? 1 : 0;
    if (FGCAL_flag == 1)
    {
        PHY_SDS_RD(unit, mdxMacId, sdsId, 186, 21, &data);
        offset_range = data >> 14;
        PATCH_PRINT("Range = %u, Redo FGCAL w/Range +1.", offset_range);
        switch (offset_range)
        {
          case 3:
            PATCH_PRINT("FgCal marginal!");
            break;
          case 2:
            PHY_SDS_WR(unit, mdxMacId, sdsId, 186, 21, 0xE0C1);

            _phy_8295_patch_bT_start_1d2(unit, port, mdxMacId, sdsId);
            _phy_8295_patch_bT_start_1d2d3(unit, port, mdxMacId, sdsId, &fgcal_offset_binary);
            break;
          case 1:
            PHY_SDS_WR(unit, mdxMacId, sdsId, 186, 21, 0xA0C1);

            _phy_8295_patch_bT_start_1d2(unit, port, mdxMacId, sdsId);
            _phy_8295_patch_bT_start_1d2d3(unit, port, mdxMacId, sdsId, &fgcal_offset_binary);
            break;
        }/* switch */
    }


    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 187, 20, 10, 5, fgcal_offset_binary);

    PATCH_PRINT("Write FGCAL = 0x%04X", fgcal_offset_binary);

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 187, 21, 0, 0, 1);
    //#======================================================================================
}

void
_phy_8295_patch_bT_start_1d1d1(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 *adj_tx_short)
{
    /* <<< start_1.1.1 >>> */

    PATCH_PRINT("start_1.1.1 ++");

    PHY_SDS_WR(unit, mdxMacId, sdsId, 187, 20, 0x0400);
    PHY_SDS_WR(unit, mdxMacId, sdsId, 187, 21, 0x0001);
    PHY_SDS_WR(unit, mdxMacId, sdsId, 187, 22, 0x7FFF);
    PHY_SDS_WR(unit, mdxMacId, sdsId, 188, 16, 0x820F);
    PHY_SDS_WR(unit, mdxMacId, sdsId, 188, 18, 0x114F);
    PHY_SDS_WR(unit, mdxMacId, sdsId, 191, 16, 0x8307);
    PHY_SDS_WR(unit, mdxMacId, sdsId, 191, 17, 0x8307);


    PHY_SDS_WR(unit, mdxMacId, sdsId, 184, 19, 0xF000);
    PHY_SDS_WR(unit, mdxMacId, sdsId, 184, 20, 0xA000);
    PHY_SDS_WR(unit, mdxMacId, sdsId, 184, 21, 0x2000);
    PHY_SDS_WR(unit, mdxMacId, sdsId, 184, 22, 0xC000);
    PHY_SDS_WR(unit, mdxMacId, sdsId, 186, 19, 0x027F);


    PHY_SDS_WR(unit, mdxMacId, sdsId, 189, 19, 0x7C0F);

    PATCH_PRINT("start_1.1.1 --");

}

void
_phy_8295_patch_bT_start_1d2(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId)
{
    uint32          data1, data2, data2t, data3;

    /* <<< start_1.2 >>> */

    PATCH_PRINT("start_1.2 ++");

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 185, 23, 15, 15, 1);


    PHY_SDS_WR(unit, mdxMacId, sdsId, 188, 18, 0x114B);
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 187, 21, 0, 0, 0);


    PHY_SDS_RD(unit, mdxMacId, sdsId, 186, 21, &data1);
    data2 = data1 & 0xFED1;
    data2t = data2 | 0x0010;
    PHY_SDS_WR(unit, mdxMacId, sdsId, 186, 21, data2t);
    osal_time_udelay(1 * 1000);
    data3 = data2 & 0xFEC1;
    PHY_SDS_WR(unit, mdxMacId, sdsId, 186, 21, data3);
    osal_time_udelay(1 * 1000);

    PATCH_PRINT("start_1.2 --");
}

void
_phy_8295_patch_bT_start_1d2d3(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 *fgcal_offset_binary)
{
    /* <<< start_1.2.3 >>> */
    uint32      datat71;

    PATCH_PRINT("start_1.2.3 ++");

    //#==============================================================
    _phy_8295_patch_bT_DBG(unit, port, mdxMacId, sdsId);

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 1);


    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 17, 5, 0, 0x20);


    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 20, 5, 0, 0xF);
    //#==============================================================

    PHY_REG_RD(unit, mdxMacId, 1, 16, &datat71);
    *fgcal_offset_binary = datat71 & 0x003F;
    PATCH_PRINT(" fgcal_offset_binary_S%u = %u", sdsId, *fgcal_offset_binary);

    //#==============================================================

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 0);
    //#==============================================================

    PATCH_PRINT("start_1.2.3 --");

}

void
_phy_8295_patch_bT_start_1d2d4(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId)
{
    /* <<< start_1.2.4 >>> */
    uint32      data1, data2, data2t, data3, data81;
    uint32      dcvs0_coef_bin, dcvs0_sign_out,
                dcvs1_coef_bin, dcvs1_sign_out,
                dcvs2_coef_bin, dcvs2_sign_out,
                dcvs3_coef_bin, dcvs3_sign_out,
                dcvs4_coef_bin, dcvs4_sign_out,
                dcvs5_coef_bin, dcvs5_sign_out;

    PATCH_PRINT("start_1.2.4 ++");

    PHY_SDS_WR(unit, mdxMacId, sdsId, 188, 18, 0x114F);

    PHY_SDS_WR(unit, mdxMacId, sdsId, 187, 22, 0x07FF);
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 191, 16, 9, 9, 0);
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 191, 17, 9, 9, 0);
    //#================================================================


    PHY_SDS_RD(unit, mdxMacId, sdsId, 186, 21, &data1);
    data2  = data1 & 0xFED1;
    data2t = data2 | 0x0010;
    PHY_SDS_WR(unit, mdxMacId, sdsId, 186, 21, data2t);
    osal_time_udelay(1 * 1000);
    data3 = data2 & 0xFEC1;
    PHY_SDS_WR(unit, mdxMacId, sdsId, 186, 21, data3);
    osal_time_udelay(1 * 1000);


    //#==============================================================

    _phy_8295_patch_bT_DBG(unit, port, mdxMacId, sdsId);

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 1);
    //#==============================================================

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 17, 5, 0, 0x20);


    //#==============================================================

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 20, 5, 0, 0x22);
    //#==============================================================

    PHY_REG_RD(unit, mdxMacId, 1, 16, &data81);
    dcvs0_coef_bin = DATA_BITS_GET(data81, 3, 0);
    dcvs0_sign_out = DATA_BITS_GET(data81, 4, 4);

    PATCH_PRINT(" dcvs0_coef_bin = %u",  dcvs0_coef_bin);
    PATCH_PRINT(" dcvs0_sign_out = %u", dcvs0_sign_out);


    //#==============================================================

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 20, 5, 0, 0x23);
    //#==============================================================

    PHY_REG_RD(unit, mdxMacId, 1, 16, &data81);
    dcvs1_coef_bin = DATA_BITS_GET(data81, 3, 0);
    dcvs1_sign_out = DATA_BITS_GET(data81, 4, 4);

    PATCH_PRINT(" dcvs1_coef_bin = %u ", dcvs1_coef_bin);
    PATCH_PRINT(" dcvs1_sign_out = %u ", dcvs1_sign_out);


    //#==============================================================

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 20, 5, 0, 0x24);
    //#==============================================================

    PHY_REG_RD(unit, mdxMacId, 1, 16, &data81);
    dcvs2_coef_bin = DATA_BITS_GET(data81, 3, 0);
    dcvs2_sign_out = DATA_BITS_GET(data81, 4, 4);

    PATCH_PRINT(" dcvs2_coef_bin = %u ", dcvs2_coef_bin);
    PATCH_PRINT(" dcvs2_sign_out = %u ", dcvs2_sign_out);


    //#==============================================================

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 20, 5, 0, 0x25);
    //#==============================================================

    PHY_REG_RD(unit, mdxMacId, 1, 16, &data81);
    dcvs3_coef_bin = DATA_BITS_GET(data81, 3, 0);
    dcvs3_sign_out = DATA_BITS_GET(data81, 4, 4);

    PATCH_PRINT(" dcvs3_coef_bin = %u", dcvs3_coef_bin);
    PATCH_PRINT(" dcvs3_sign_out = %u", dcvs3_sign_out);


    //#==============================================================

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 20, 5, 0, 0x2C);
    //#==============================================================

    PHY_REG_RD(unit, mdxMacId, 1, 16, &data81);
    dcvs4_coef_bin = DATA_BITS_GET(data81, 3, 0);
    dcvs4_sign_out = DATA_BITS_GET(data81, 4, 4);

    PATCH_PRINT(" dcvs4_coef_bin = %u", dcvs4_coef_bin);
    PATCH_PRINT(" dcvs4_sign_out = %u", dcvs4_sign_out);


    //#==============================================================

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 20, 5, 0, 0x2D);
    //#==============================================================

    PHY_REG_RD(unit, mdxMacId, 1, 16, &data81);
    dcvs5_coef_bin = DATA_BITS_GET(data81, 3, 0);
    dcvs5_sign_out = DATA_BITS_GET(data81, 4, 4);

    PATCH_PRINT(" dcvs5_coef_bin = %u", dcvs5_coef_bin);
    PATCH_PRINT(" dcvs5_sign_out = %u", dcvs5_sign_out);


    //#==============================================================

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 0);
    //#===================================================================
    _phy_8295_patch_bT_input_DCVS0(unit, port, mdxMacId, sdsId, dcvs0_sign_out, dcvs0_coef_bin);
    _phy_8295_patch_bT_input_DCVS1(unit, port, mdxMacId, sdsId, dcvs1_sign_out, dcvs1_coef_bin);
    _phy_8295_patch_bT_input_DCVS2(unit, port, mdxMacId, sdsId, dcvs2_sign_out, dcvs2_coef_bin);
    _phy_8295_patch_bT_input_DCVS3(unit, port, mdxMacId, sdsId, dcvs3_sign_out, dcvs3_coef_bin);
    _phy_8295_patch_bT_input_DCVS4(unit, port, mdxMacId, sdsId, dcvs4_sign_out, dcvs4_coef_bin);
    _phy_8295_patch_bT_input_DCVS5(unit, port, mdxMacId, sdsId, dcvs5_sign_out, dcvs5_coef_bin);
    //#==============================================================

    PHY_SDS_WR(unit, mdxMacId, sdsId, 187, 22, 0x7FFF);
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 191, 16, 9, 9, 1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 191, 17, 9, 9, 1);

    PATCH_PRINT("start_1.2.4 --");

}





void
_phy_8295_patch_bT_start_1d3d1(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 *pShort_flag)
{
    /* <<< start_1.3.1 >>> */
    int32       i;
    uint32      leq_temp, sum10, avg10, int10;
    uint32      LEQ_OFFSET, LEQ_last;
    uint32      data1, data1t, data2t;

    PATCH_PRINT("start_1.3.1 ++");

    //#1.3.1(a) release Eq_SEL =0, (b) Start LEQ training eq_hold=0
    PHY_SDS_WR(unit, mdxMacId, sdsId, 187, 16, 0x3E48);
    //#sdsrw w $::sds_num 0x2E 0x17 0xA100

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 23, 7, 7, 0);
    //#1.3.2(a) dump LEQ x 10times. get sum10, avg10(round), avg10(int)
    sum10 = 0;
    for (i = 0; i < 10; i++)
    {
        osal_time_usleep(10 * 1000);
        _phy_8295_patch_bT_dump_leq(unit, port, mdxMacId, sdsId, &leq_temp);
        sum10 += leq_temp;
    }

    avg10 = (sum10/10) + (((sum10 % 10) >= 5) ? 1 : 0);     //avg10 = round($::sum10/10.0);
    int10 = sum10/10;
    //#puts " Leq sum = $::sum10"

    if (sum10 >= 5)
    {
        LEQ_OFFSET = avg10 + 3;
        PHY_SDS_RD(unit, mdxMacId, sdsId, 186, 22, &data1);
        data1t = data1 & 0x00FF;
        LEQ_last = LEQ_OFFSET << 8;
        data2t = data1t | LEQ_last;
        PHY_SDS_WR(unit, mdxMacId, sdsId, 186, 22, data2t);

        PATCH_PRINT(" Set %u.", (avg10 + 3));
        //#1.3.2 Stop LEQ training eq_hold=1, 1.3.3(d)set EQ_SELREG =1 to load LEQ manual value
        //#sdsrw w $::sds_num 0x2E 0x17 0xA180

        PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 23, 7, 7, 1);
        //#11/25 EQOUT_OFFSET +3
        PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 23, 6, 2, 0x3);
        PHY_SDS_WR(unit, mdxMacId, sdsId, 187, 16, 0xBE48);
    }
    else if (sum10 >= 3)
    {
        LEQ_OFFSET = avg10 + 2;
        PHY_SDS_RD(unit, mdxMacId, sdsId, 186, 22, &data1);
        data1t = data1 & 0x00FF;
        LEQ_last = LEQ_OFFSET << 8;
        data2t = data1t | LEQ_last;
        PHY_SDS_WR(unit, mdxMacId, sdsId, 186, 22, data2t);

        PATCH_PRINT(" Set %u.", (avg10 + 2));
        //#1.3.2 Stop LEQ training eq_hold=1, 1.3.3(d)set EQ_SELREG =1 to load LEQ manual value
        //#sdsrw w $::sds_num 0x2E 0x17 0xA180
        PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 23, 7, 7, 1);
        //#11/25 EQOUT_OFFSET +2
        PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 23, 6, 2, 0x2);
        PHY_SDS_WR(unit, mdxMacId, sdsId, 187, 16, 0xBE48);
    }
    else if (sum10 < 3)
    {
        PHY_SDS_RD(unit, mdxMacId, sdsId, 186, 22, &data1);
        data1t = data1 & 0x00FF;
        LEQ_last = (avg10 + 1) << 8;
        data2t = data1t | LEQ_last;
        PHY_SDS_WR(unit, mdxMacId, sdsId, 186, 22, data2t);

        PATCH_PRINT(" Set %u.", (avg10 + 1));
        //#1.3.2 Stop LEQ training eq_hold=1, 1.3.3(d)set EQ_SELREG =1 to load LEQ manual value
        //#sdsrw w $::sds_num 0x2E 0x17 0xA180
        PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 23, 7, 7, 1);
        //#11/25 EQOUT_OFFSET +1
        PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 23, 6, 2, 0x1);
        PHY_SDS_WR(unit, mdxMacId, sdsId, 187, 16, 0xBE48);
    }
    else
    {
        PHY_SDS_RD(unit, mdxMacId, sdsId, 186, 22, &data1);
        data1t = data1 & 0x00FF;
        LEQ_last = (int10 + 2) << 8;
        data2t = data1t | LEQ_last;
        PHY_SDS_WR(unit, mdxMacId, sdsId, 186, 22, data2t);
        PATCH_PRINT(" Set %u.", (int10 + 2));
        //#1.3.2 Stop LEQ training eq_hold=1, 1.3.3(d)set EQ_SELREG =1 to load LEQ manual value
        //#sdsrw w $::sds_num 0x2E 0x17 0xA180
        PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 23, 7, 7, 1);
        //#11/25 EQOUT_OFFSET +2
        PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 23, 6, 2, 0x2);
        PHY_SDS_WR(unit, mdxMacId, sdsId, 187, 16, 0xBE48);
    }

    PATCH_PRINT("start_1.3.1 --");

}


void
_phy_8295_patch_bT_start_1d4(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId)
{
    /* <<< start_1.4 >>> */
    uint32      data1, data1t;
    uint32      data2, data2t;
    uint32      tap0_sign_out, tap0_coef_bin, vthp_set_bin, vthn_set_bin;
    uint32      fgcal_offset_binary;
    uint32      leq_temp;

    PATCH_PRINT("start_1.4 ++");

    //#1.4.1 start DFE tap0 & Vth training
    PHY_SDS_RD(unit, mdxMacId, sdsId, 185, 23, &data1);
    data1t = data1 & 0xDF7F;
    PHY_SDS_WR(unit, mdxMacId, sdsId, 185, 23, data1t);
    //#1.4.2 stop DFE tap0 & Vth training

    //#Readout DFE tap 0, Vth and write back
    _phy_8295_patch_bT_Readtap0(unit, port, mdxMacId, sdsId, &tap0_sign_out, &tap0_coef_bin, &vthp_set_bin, &vthn_set_bin);
    _phy_8295_patch_bT_input_tap0(unit, port, mdxMacId, sdsId, tap0_sign_out, tap0_coef_bin);
    _phy_8295_patch_bT_input_vth(unit, port, mdxMacId, sdsId, vthp_set_bin, vthn_set_bin);

    //#Set DFE manual to load setting
    PHY_SDS_RD(unit, mdxMacId, sdsId, 185, 23, &data2);
    data2t = data2 | 0x2080;
    PHY_SDS_WR(unit, mdxMacId, sdsId, 185, 23, data2t);

    //#1.5.1 Redo FGCAL and DCVS cali

    _phy_8295_patch_bT_start_1d2(unit, port, mdxMacId, sdsId);
    _phy_8295_patch_bT_start_1d2d3(unit, port, mdxMacId, sdsId, &fgcal_offset_binary);
    PATCH_PRINT(" fg_bin_S%u=%u", sdsId, fgcal_offset_binary);
    _phy_8295_patch_bT_FGCAL_chk(unit, port, mdxMacId, sdsId, fgcal_offset_binary);

    _phy_8295_patch_bT_start_1d2d4(unit, port, mdxMacId, sdsId);

    osal_time_usleep(50 * 1000);

    PHY_SDS_WR(unit, mdxMacId, sdsId, 187, 16, 0x3E48);
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 23, 7, 7, 0);

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 185, 20, 8, 8, 1);
    _phy_8295_patch_bT_dump_leq(unit, port, mdxMacId, sdsId, &leq_temp);
    PATCH_PRINT(" max %u", leq_temp);

    PATCH_PRINT("start_1.4 --");

}

void
_phy_8295_patch_bT_start_1d5d2(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId)
{
    /* <<< start_1.5.2 >>> */
    uint32      data1, data2;
    uint32      sign_out_Even[TAB_END];
    uint32      coef_bin_Even[TAB_END];
    uint32      sign_out_Odd[TAB_END];
    uint32      coef_bin_Odd[TAB_END];

    PATCH_PRINT("start_1.5.2 ++");

    //#=======================================

    PHY_SDS_RD(unit, mdxMacId, sdsId, 185, 23, &data1);
    PHY_SDS_WR(unit, mdxMacId, sdsId, 185, 23, (data1 & 0xF0FF));

    //#==============================================================
    _phy_8295_patch_bT_Othertap(unit, port, mdxMacId, sdsId, sign_out_Even, coef_bin_Even, sign_out_Odd, coef_bin_Odd);
    _phy_8295_patch_bT_input_tap1_even(unit, port, mdxMacId, sdsId, sign_out_Even[TAB1], coef_bin_Even[TAB1]);
    _phy_8295_patch_bT_input_tap1_odd(unit, port, mdxMacId, sdsId,  sign_out_Odd[TAB1],  coef_bin_Odd[TAB1]);
    _phy_8295_patch_bT_input_tap2_even(unit, port, mdxMacId, sdsId, sign_out_Even[TAB2], coef_bin_Even[TAB2]);
    _phy_8295_patch_bT_input_tap2_odd(unit, port, mdxMacId, sdsId,  sign_out_Odd[TAB2],  coef_bin_Odd[TAB2]);
    _phy_8295_patch_bT_input_tap3_even(unit, port, mdxMacId, sdsId, sign_out_Even[TAB3], coef_bin_Even[TAB3]);
    _phy_8295_patch_bT_input_tap3_odd(unit, port, mdxMacId, sdsId,  sign_out_Odd[TAB3],  coef_bin_Odd[TAB3]);
    _phy_8295_patch_bT_input_tap4_even(unit, port, mdxMacId, sdsId, sign_out_Even[TAB4], coef_bin_Even[TAB4]);
    _phy_8295_patch_bT_input_tap4_odd(unit, port, mdxMacId, sdsId,  sign_out_Odd[TAB4],  coef_bin_Odd[TAB4]);


    PHY_SDS_RD(unit, mdxMacId, sdsId, 185, 23, &data2);
    PHY_SDS_WR(unit, mdxMacId, sdsId, 185, 23, (data2 | 0x2F80));

    PATCH_PRINT("start_1.5.2 --");

}


void
_phy_8295_patch_bT_input_vth(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 vthp, uint32 vthn)
{
    uint32      data;


    data = (vthp << 3) + vthn;
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 19, 5, 0, data);
    PATCH_PRINT("Write Vth = 0x%04X", data);
}


void
_phy_8295_patch_bT_servo(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId)
{
    uint32      servo_data_even_bin;
    uint32      servo_data_Odd_bin;

    _phy_8295_patch_bT_DBG(unit, port, mdxMacId, sdsId);

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 17, 5, 0, 0x20);
    //#==============================================================

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 20, 5, 0, 0xB);
    //#==============================================================
    PHY_REG_FIELD_RD(unit, mdxMacId, 1, 16, 4, 0, &servo_data_even_bin);
    PATCH_PRINT(" servo_data_even_bin = %u", servo_data_even_bin);
    //#==============================================================

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 189, 20, 5, 0, 0x29);
    //#==============================================================
    PHY_REG_FIELD_RD(unit, mdxMacId, 1, 16, 4, 0, &servo_data_Odd_bin);
    //#==============================================================
    PATCH_PRINT(" servo_data_Odd_bin = %u", servo_data_Odd_bin);

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 21, 9, 9, 0);
}

void
_phy_8295_patch_bT_rx_cal_nodfe(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId, uint32 *pAdj_tx_short, uint32 *pShort_flag)
{
    uint32      fgcal_offset_binary;

    PATCH_PRINT("Start  PHYID %u,  S%u RX calibration:", mdxMacId, sdsId);

    _phy_8295_patch_bT_start_1d1d1(unit, port, mdxMacId, sdsId, pAdj_tx_short);


    PHY_SDS_WR(unit, mdxMacId, sdsId, 185, 23, 0xBFDF);

    PHY_SDS_FIELD_WR(unit, mdxMacId, sdsId, 186, 23, 7, 2, 0x20);
    PHY_SDS_WR(unit, mdxMacId, sdsId, 185, 20, 0x2427);
    PHY_SDS_WR(unit, mdxMacId, sdsId, 185, 19, 0x1892);
    PHY_SDS_WR(unit, mdxMacId, sdsId, 186, 18, 0x2040);
    PHY_SDS_WR(unit, mdxMacId, sdsId, 188, 18, 0x114F);

    PHY_SDS_WR(unit, mdxMacId, sdsId, 185, 23, 0xBFDF);
    PHY_SDS_WR(unit, mdxMacId, sdsId, 186, 22, 0x00CB);
    PHY_SDS_WR(unit, mdxMacId, sdsId, 188, 21, 0x787C);

    _phy_8295_patch_bT_start_1d2(unit, port, mdxMacId, sdsId);

    _phy_8295_patch_bT_start_1d2d3(unit, port, mdxMacId, sdsId, &fgcal_offset_binary);

    _phy_8295_patch_bT_FGCAL_chk(unit, port, mdxMacId, sdsId, fgcal_offset_binary);


    _phy_8295_patch_bT_start_1d3d1(unit, port, mdxMacId, sdsId, pShort_flag);

}


void
_phy_8295_patch_bT_init_sfp(uint32 unit, uint32 port, uint32 mdxMacId)
{
    /* <<< init_sfp >>> */



    PHY_REG_WR(unit, mdxMacId, 4, 20, 0x1000);

    PHY_REG_WR(unit, mdxMacId, 1, 20, 0x3898);

    //#inverse S1 RX P/N for SFP connection
    PHY_SDS_WR(unit, mdxMacId, 1, 0, 16, 0x1603);
    PHY_SDS_WR(unit, mdxMacId, 1, 24, 18, 0x2000);

    PHY_SDS_WR(unit, mdxMacId, 0, 24, 16, 0x0);
    PHY_SDS_WR(unit, mdxMacId, 1, 24, 16, 0x0);
    PHY_SDS_WR(unit, mdxMacId, 0, 125, 18, 0x0130);
    PHY_SDS_WR(unit, mdxMacId, 1, 125, 18, 0x0130);

    PHY_SDS_WR(unit, mdxMacId, 0, 0, 18, 0x30E0);
    PHY_SDS_WR(unit, mdxMacId, 1, 0, 18, 0x30E0);
    PHY_SDS_WR(unit, mdxMacId, 4, 0, 18, 0x30E0);
    PHY_SDS_WR(unit, mdxMacId, 5, 0, 18, 0x30E0);
    PHY_SDS_WR(unit, mdxMacId, 6, 0, 18, 0x30E0);
    PHY_SDS_WR(unit, mdxMacId, 7, 0, 18, 0x30E0);



    PHY_SDS_WR(unit, mdxMacId, 0, 132, 16, 0xE150);
    PHY_SDS_WR(unit, mdxMacId, 1, 132, 16, 0xE150);
    PHY_SDS_WR(unit, mdxMacId, 0, 190, 20, 0x41BC);
    PHY_SDS_WR(unit, mdxMacId, 0, 190, 21, 0x3DBC);
    PHY_SDS_WR(unit, mdxMacId, 1, 190, 20, 0x41BC);
    PHY_SDS_WR(unit, mdxMacId, 1, 190, 21, 0x3DBC);

    PHY_SDS_WR(unit, mdxMacId, 0, 132, 17, 0x9090);


    PHY_SDS_WR(unit, mdxMacId, 0, 132, 21, 0x4A8D);



    PHY_SDS_WR(unit, mdxMacId, 0, 184, 19, 0xF000);
    PHY_SDS_WR(unit, mdxMacId, 0, 185, 23, 0xBFDF);
    PHY_SDS_WR(unit, mdxMacId, 0, 186, 20, 0x1178);
    PHY_SDS_WR(unit, mdxMacId, 0, 187, 22, 0x07FF);
    PHY_SDS_WR(unit, mdxMacId, 0, 188, 18, 0x114F);
    PHY_SDS_WR(unit, mdxMacId, 0, 188, 21, 0x787C);
    PHY_SDS_WR(unit, mdxMacId, 0, 191, 16, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 0, 191, 17, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 0, 191, 19, 0x0000);

    PHY_SDS_WR(unit, mdxMacId, 0, 191, 21, 0x0019);
    PHY_SDS_WR(unit, mdxMacId, 0, 191, 23, 0xF0E0);


    PHY_SDS_WR(unit, mdxMacId, 0, 189, 19, 0x7C0F);

    PHY_SDS_WR(unit, mdxMacId, 1, 132, 17, 0x9090);

    PHY_SDS_WR(unit, mdxMacId, 1, 132, 21, 0x4A8D);


    PHY_SDS_WR(unit, mdxMacId, 1, 184, 19, 0xF000);
    PHY_SDS_WR(unit, mdxMacId, 1, 185, 23, 0xBFDF);
    PHY_SDS_WR(unit, mdxMacId, 1, 186, 20, 0x1178);
    PHY_SDS_WR(unit, mdxMacId, 1, 187, 22, 0x07FF);
    PHY_SDS_WR(unit, mdxMacId, 1, 188, 18, 0x114F);
    PHY_SDS_WR(unit, mdxMacId, 1, 188, 21, 0x787C);
    PHY_SDS_WR(unit, mdxMacId, 1, 191, 16, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 1, 191, 17, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 1, 191, 19, 0x0000);
    PHY_SDS_WR(unit, mdxMacId, 1, 191, 21, 0x0019);
    PHY_SDS_WR(unit, mdxMacId, 1, 191, 23, 0xF0E0);


    PHY_SDS_WR(unit, mdxMacId, 1, 189, 19, 0x7C0F);


    PHY_REG_WR(unit, mdxMacId, 5, 18, 0x1F);

    PHY_REG_WR(unit, mdxMacId, 5, 17, 0x1F);

    PHY_SDS_WR(unit, mdxMacId, 1, 189, 23, 0x0742);

    PHY_SDS_WR(unit, mdxMacId, 1, 190, 16, 0x1700);
    _phy_8295_patch_bT_sds_rst(unit, port, mdxMacId);

    PATCH_PRINT("Initialize %u Chip patch SFP+ and Enable DFE....Done!", mdxMacId);
}


#if defined(CONFIG_SDK_RTL8295R)
void
_phy_8295_patch_a_init_rtl8295r(uint32 unit, uint32 port, uint32 mdxMacId)
{
    uint32  val, data;

    PHY_REG_FIELD_RD(unit, mdxMacId, 0, 19, 8, 4, &val);
    if (val == 0)
    {
        PHY_SDS_FIELD_WR(unit, mdxMacId, 0, 25, 21, 11, 8, 0xF);
        PHY_SDS_FIELD_WR(unit, mdxMacId, 1, 25, 21, 11, 8, 0xF);
    }

    PHY_SDS_FIELD_WR(unit, mdxMacId, 1, 26, 17, 13, 12, 3);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 1, 125, 20, 2, 2, 1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 1, 125, 21, 5, 5, 1);

    PHY_SDS_FIELD_WR(unit, mdxMacId, 0, 27, 17, 6, 6, 0);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 1, 27, 17, 6, 6, 0);

    PHY_REG_WR(unit, mdxMacId, 1, 22, 0x5800);

    PHY_SDS_WR(unit, mdxMacId, 0, 132, 16, 0xE350);

    PHY_SDS_WR(unit, mdxMacId, 0, 132, 21, 0x420D);

    PHY_SDS_WR(unit, mdxMacId, 0, 186, 18, 0x2080);
    PHY_SDS_WR(unit, mdxMacId, 0, 186, 20, 0x1378);
    PHY_SDS_WR(unit, mdxMacId, 0, 189, 19, 0x7C07);
    PHY_SDS_WR(unit, mdxMacId, 0, 190, 20, 0x41B8);
    PHY_SDS_WR(unit, mdxMacId, 0, 190, 21, 0x3DB8);

    if (val == 0)
    {
        PHY_SDS_WR(unit, mdxMacId, 0, 188, 17, 0x91CC);
        PHY_SDS_WR(unit, mdxMacId, 0, 188, 18, 0x116F);
    }

    PHY_SDS_WR(unit, mdxMacId, 0, 187, 22, 0x07FA);

    PHY_SDS_WR(unit, mdxMacId, 0, 168, 19, 0xF000);
    PHY_SDS_WR(unit, mdxMacId, 0, 169, 23, 0xBFDF);
    PHY_SDS_WR(unit, mdxMacId, 0, 170, 18, 0x2100);
    PHY_SDS_WR(unit, mdxMacId, 0, 170, 20, 0x1378);
    PHY_SDS_WR(unit, mdxMacId, 0, 171, 22, 0x07FA);
    PHY_SDS_WR(unit, mdxMacId, 0, 172, 18, 0x114F);
    PHY_SDS_WR(unit, mdxMacId, 0, 172, 21, 0x787C);
    PHY_SDS_WR(unit, mdxMacId, 0, 175, 16, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 0, 175, 17, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 0, 175, 21, 0x0019);
    PHY_SDS_WR(unit, mdxMacId, 0, 175, 22, 0xF0E0);

    PHY_SDS_WR(unit, mdxMacId, 1, 132, 16, 0xE350);

    PHY_SDS_WR(unit, mdxMacId, 1, 132, 21, 0x420D);

    PHY_SDS_WR(unit, mdxMacId, 1, 186, 18, 0x2080);
    PHY_SDS_WR(unit, mdxMacId, 1, 186, 20, 0x1378);
    PHY_SDS_WR(unit, mdxMacId, 1, 189, 19, 0x7C07);
    PHY_SDS_WR(unit, mdxMacId, 1, 190, 20, 0x41B8);
    PHY_SDS_WR(unit, mdxMacId, 1, 190, 21, 0x3DB8);

    if (val == 0)
    {
        PHY_SDS_WR(unit, mdxMacId, 1, 188, 17, 0x91CC);
        PHY_SDS_WR(unit, mdxMacId, 1, 188, 18, 0x116F);
    }

    PHY_SDS_WR(unit, mdxMacId, 1, 187, 22, 0x07FA);

    PHY_SDS_WR(unit, mdxMacId, 1, 168, 19, 0xF000);
    PHY_SDS_WR(unit, mdxMacId, 1, 169, 23, 0xBFDF);
    PHY_SDS_WR(unit, mdxMacId, 1, 170, 18, 0x2100);
    PHY_SDS_WR(unit, mdxMacId, 1, 170, 20, 0x1378);
    PHY_SDS_WR(unit, mdxMacId, 1, 171, 22, 0x07FA);
    PHY_SDS_WR(unit, mdxMacId, 1, 172, 18, 0x114F);
    PHY_SDS_WR(unit, mdxMacId, 1, 172, 21, 0x787C);
    PHY_SDS_WR(unit, mdxMacId, 1, 175, 16, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 1, 175, 17, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 1, 175, 21, 0x0019);
    PHY_SDS_WR(unit, mdxMacId, 1, 175, 22, 0xF0E0);



    /* CMU patch */
    //Page 0x21 (ANA_COM) Reg 0x0 => 0xE150  (FLD_DSEL =0)
    //Page 0x21 (ANA_COM) Reg 0x2 => 0x1CA4 (TIME0_CK =7)
    //REG_LCCMU_init_time<2:0>=3'b100 : 0x21, 0x3, [4:2]
    PHY_SDS_WR(unit, mdxMacId, 0, 132, 16, 0xE150);
    PHY_SDS_WR(unit, mdxMacId, 0, 132, 18, 0x1CA4);
    PHY_SDS_RD(unit, mdxMacId, 0, 132, 19, &data);
    data = REG32_FIELD_SET(data, 0x4, 2, (0x7 << 2));
    PHY_SDS_WR(unit, mdxMacId, 0, 132, 19, data);

    //10GR:
    //S0 & S1 Page 0x2F (10p3125g_EXT) Reg 0x14 => 0x8389 ( divide_num =32, up limit 905)
    //S0 & S1 Page 0x2F (10p3125g_EXT) Reg 0x15 => 0x3F89 (down limit 905)
    PHY_SDS_WR(unit, mdxMacId, 0, 190, 20, 0x8389);
    PHY_SDS_WR(unit, mdxMacId, 0, 190, 21, 0x3F89);
    PHY_SDS_WR(unit, mdxMacId, 1, 190, 20, 0x8389);
    PHY_SDS_WR(unit, mdxMacId, 1, 190, 21, 0x3F89);

    //5G:
    //S0 & S1 Page 0x2B (1p25g_EXT) Reg 0x14 => 0x8294 (divide_num =32, up limit 660)
    //S0 & S1 Page 0x2B (1p25g_EXT) Reg 0x15 => 0x3E94 (down limit 660)
    PHY_SDS_WR (unit, mdxMacId, 0, 174, 20, 0x8294);
    PHY_SDS_WR (unit, mdxMacId, 0, 174, 21, 0x3E94);
    PHY_SDS_WR (unit, mdxMacId, 1, 174, 20, 0x8294);
    PHY_SDS_WR (unit, mdxMacId, 1, 174, 21, 0x3E94);

    //1.25G:
    //S0 & S1 Page 0x25 (1p25g_EXT) Reg 0x14 => 0x8294 (divide_num =32, up limit 660)
    //S0 & S1 Page 0x25 (1p25g_EXT) Reg 0x15 => 0x3E94 (down limit 660)
    PHY_SDS_WR(unit, mdxMacId, 0, 150, 20, 0x8294);
    PHY_SDS_WR(unit, mdxMacId, 0, 150, 21, 0x3E94);
    PHY_SDS_WR(unit, mdxMacId, 1, 150, 20, 0x8294);
    PHY_SDS_WR(unit, mdxMacId, 1, 150, 21, 0x3E94);
    /* CMU patch end */


    /* SDK: add inverse S1 RX polarity for specific model */
    if (HWP_8390_50_FAMILY(unit))
    {
        /* inverse S1 RX polarity */
        PHY_SDS_WR(unit, mdxMacId, 1, 0, 16, 0x1603);
        PHY_SDS_WR(unit, mdxMacId, 1, 24, 18, 0x2000);
    }

}
#endif

#if defined(CONFIG_SDK_RTL8295R)
void
_phy_8295_patch_8295r_config(uint32 unit, uint32 port, uint32 mdxMacId)
{
    uint32      sdsId;

    if (((sdsId = HWP_PORT_SDSID(unit, port)) != HWP_NONE))
    {
        if (HWP_SDS_MODE(unit, sdsId) == RTK_MII_USXGMII_10GSXGMII)
        {
            /* configure S0 as USXGMII */
            PHY_REG_FIELD_WR(unit, mdxMacId, RTL8295_SDS_CTRL_SDS_CTRL_S0_PAGE, RTL8295_SDS_CTRL_SDS_CTRL_S0_REG, 4, 0, 0xD);
        }
    }

    /* disable FCTRL/EEE */
    PHY_SDS_FIELD_WR(unit, mdxMacId, 1, 125, 19, 6, 5, 0x0);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 1, 125, 19, 9, 9, 0x0);

    /* enable 1G auto nego */
    PHY_SDS_FIELD_WR(unit, mdxMacId, 1, 8, 16, 12, 12, 0x1);

    if (!HWP_8390_50_FAMILY(unit))
    {
        PHY_SDS_FIELD_WR(unit, mdxMacId, 0, 8, 16, 12, 12, 0x1);
    }

    /* Configure S1 SerDes OFF */
    PHY_REG_FIELD_WR(unit, mdxMacId, 5, 18, 4, 0, 0x1F);

}
#endif

void
_phy_8295_patch_bT_startall(uint32 unit, uint32 port, uint32 mdxMacId, uint32 sdsId)
{
    uint32      short_flag = 0;
    uint32      adj_tx_short = 0;

    /* <<< startall >>> */

    _phy_8295_patch_bT_rx_cal_nodfe(unit, port, mdxMacId, sdsId, &adj_tx_short, &short_flag);
    _phy_8295_patch_bT_start_1d4(unit, port, mdxMacId, sdsId);
    _phy_8295_patch_bT_start_1d5d2(unit, port, mdxMacId, sdsId);

}


void
_phy_8295_rxCali_a_startall(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo)
{
    uint32      RXsym_err = 0;
    uint32      start_1d5d2_en = (pinfo->param.flags & PHY_8295_RXCALI_PARAM_FLAG_DFETAP1_4_DISABLE) ? 0 : 1;

    /* <<< startall >>> */

    _phy_8295_rxCali_a_rx_cal_nodfe(ps, pinfo);
    _phy_8295_rxCali_a_start_1d4(ps, pinfo);
    if (start_1d5d2_en == 1)
    {
        _phy_8295_rxCali_a_start_1d5d2(ps, pinfo);
    }


    osal_time_usleep(10 * 1000);
    //#read symbol ERR twice to check if the value was Zero
    PHY_SDS_FIELD_RD(ps->unit, ps->mdxMacId, ps->sdsId, 20, 17, 7, 0, &RXsym_err);
    osal_time_usleep(100 * 1000);
    PHY_SDS_FIELD_RD(ps->unit, ps->mdxMacId, ps->sdsId, 20, 17, 7, 0, &RXsym_err);
    if (RXsym_err > 0)
    {
        PATCH_PRINT("RXsym_err = %u! recal", RXsym_err);
        pinfo->status.flag_recal = 1;
    }


    if (patchDbgEnable & PHY_8295_PATCH_DBG_OUT)
    {
        _phy_8295_rxCali_a_eye_scan(ps, 10);
    }
}

void
_phy_8295_rxCali_a_rx_cal_nodfe (phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo)
{
    uint32      fgcal_offset_binary;

    PATCH_PRINT("Start  PHYID %u,  S%u RX calibration:", ps->mdxMacId, ps->sdsId);
    _phy_8295_rxCali_a_start_1d1d1(ps, pinfo);

    //#1.1.2 Load DFE init value
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 185, 23, 13, 7, 0x7F);
    //#1.1.3 Disable LEQ training, Enable DFE clock
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 23, 7, 2, 0x20);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 185, 20, 8, 8, 0);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 185, 19, 4, 4, 1);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 18, 14, 14, 0);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 188, 18, 15, 15, 0);
    //#1.1.4 Offset Calibration setting & 1.1.5 set LEQ and DFE
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 185, 23, 15, 14, 0x2);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 22, 3, 2, 0x2);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 185, 23, 6, 0, 0x5F);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 188, 21, 7, 2, 0x1F);

    _phy_8295_rxCali_a_start_1d2(ps, pinfo);

    osal_time_usleep(10 * 1000);
    _phy_8295_rxCali_a_start_1d2d3(ps, pinfo, &fgcal_offset_binary);

    _phy_8295_rxCali_a_FGCAL_chk(ps, pinfo, fgcal_offset_binary);

    osal_time_usleep(10 * 1000);
    _phy_8295_rxCali_a_start_1d3d1(ps, pinfo);
    osal_time_usleep(10 * 1000);

}


void
_phy_8295_rxCali_a_DBG(phy_8295_rxCaliSds_t *ps)
{
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, 0, 124, 18, 4, 4, 1);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, 0, 3, 20, 9, 0, 0x0);

    switch(ps->sdsId)
    {
      case 0:
        PHY_REG_WR(ps->unit, ps->mdxMacId, 0, 23, 0x2000);
        PHY_SDS_WR(ps->unit, ps->mdxMacId, 0, 124, 18, 0x0016);
        break;
      case 1:
        PHY_REG_WR(ps->unit, ps->mdxMacId, 0, 23, 0x2000);
        PHY_SDS_WR(ps->unit, ps->mdxMacId, 0, 124, 18, 0x0018);
        break;
    }
}

void
_phy_8295_rxCali_a_input_DCVS0(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 dcvs0)
{
    uint32      data;
    data = (sign << 4) + dcvs0;
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 20, 4, 0, data);
}

void
_phy_8295_rxCali_a_input_DCVS1(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 dcvs1)
{
    uint32      data;
    data = (sign << 4) + dcvs1;
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 21, 15, 11, data);
}

void
_phy_8295_rxCali_a_input_DCVS2(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 dcvs2)
{
    uint32      data;
    data = (sign << 4) + dcvs2;
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 21, 10, 6, data);
}

void
_phy_8295_rxCali_a_input_DCVS3(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 dcvs3)
{
    uint32      data;
    data = (sign << 4) + dcvs3;
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 21, 5, 1, data);
}

void
_phy_8295_rxCali_a_input_DCVS4(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 dcvs4)
{
    uint32      data;
    data = (sign << 4) + dcvs4;
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 191, 16, 14, 10, data);
}

void
_phy_8295_rxCali_a_input_DCVS5(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 dcvs5)
{
    uint32      data;
    data = (sign << 4) + dcvs5;
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 191, 17, 14, 10, data);
}


void
_phy_8295_rxCali_a_input_tap0(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 tap)
{

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 188, 19, 5, 0, ((sign << 5) + tap));
}


void
_phy_8295_rxCali_a_input_vth(phy_8295_rxCaliSds_t *ps, uint32 vthp, uint32 vthn)
{
    uint32      data;

    data = (vthp << 3) + vthn;
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 19, 5, 0, data);
    PATCH_PRINT("Write Vth = 0x%04X", data);
}

void
_phy_8295_rxCali_a_input_tap1_even(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 tap)
{
    //#Page 0x2E Reg 0x3([10:6]:reg0_tap1_init_even)
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 184, 19, 10, 6, tap);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 191, 23, 1, 1, sign);
}

void
_phy_8295_rxCali_a_input_tap1_odd(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 tap)
{
    //#Page 0x2E Reg 0x5([4:0]:reg0_tap1_init_odd)
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 184, 21, 4, 0, tap);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 191, 23, 2, 2, sign);
}

void
_phy_8295_rxCali_a_input_tap2_even(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 tap)
{
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 184, 19, 5, 0, ((sign << 5) + tap));
}

void
_phy_8295_rxCali_a_input_tap2_odd(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 tap)
{
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 184, 21, 11, 6, ((sign << 5) + tap));
}

void
_phy_8295_rxCali_a_input_tap3_even(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 tap)
{
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 184, 20, 11, 6, ((sign << 5) + tap));
}

void
_phy_8295_rxCali_a_input_tap3_odd(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 tap)
{
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 184, 22, 5, 0, ((sign << 5) + tap));
}

void
_phy_8295_rxCali_a_input_tap4_even(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 tap)
{
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 184, 20, 5, 0, ((sign << 5) + tap));
}

void
_phy_8295_rxCali_a_input_tap4_odd(phy_8295_rxCaliSds_t *ps, uint32 sign, uint32 tap)
{
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 184, 22, 11, 6, ((sign << 5) + tap));
}

void
_phy_8295_rxCali_a_Readtap0(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo,
                         uint32 *psign_out, uint32 *pcoef_bin,
                         uint32 *pVthp, uint32 *pVthn)
{
    uint32      data;


    _phy_8295_rxCali_a_DBG(ps);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 1);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 17, 5, 0, 0x20);
    //#==============================================================
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 20, 5, 0, 0);
    //#==============================================================

    PHY_REG_FIELD_RD(ps->unit, ps->mdxMacId, 1, 16, 5, 0, &data);
    *psign_out = DATA_BITS_GET(data, 5, 5);
    *pcoef_bin = DATA_BITS_GET(data, 4, 0);
    //#===================================================
    PATCH_PRINT(" tap0_sign = 0x%X", *psign_out);
    PATCH_PRINT(" tap0_value = 0x%X", *pcoef_bin);
    //#===================================================

    if (*pcoef_bin <= 6)
    {
        PATCH_PRINT("coef_bin = %d! recal", *pcoef_bin);
        pinfo->status.flag_recal = 1;
    }

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 0);

    _phy_8295_rxCali_a_DBG(ps);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 1);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 17, 5, 0, 0x20);
    //#==============================================================
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 20, 5, 0, 0xC);
    //#==============================================================

    PHY_REG_RD(ps->unit, ps->mdxMacId, 1, 16, &data);
    *pVthn = DATA_BITS_GET(data, 5, 3);
    *pVthp = DATA_BITS_GET(data, 2, 0);
    //#==============================================================
    PATCH_PRINT(" vthn = 0x%04X", *pVthn);
    PATCH_PRINT(" vthp = 0x%04X", *pVthp);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 0);

}

void
_phy_8295_rxCali_a_Othertap(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo,
                         uint32 *psign_out_Even, uint32 *pcoef_bin_Even,
                         uint32 *psign_out_Odd,   uint32 *pcoef_bin_Odd)
{
    uint32          data;

    _phy_8295_rxCali_a_DBG(ps);

    //#==============================================================
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 1);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 17, 5, 0, 0x20);
    //#==============================================================
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 20, 5, 0, 0x1);
    //#==============================================================

    PHY_REG_RD(ps->unit, ps->mdxMacId, 1, 16, &data);
    psign_out_Even[TAB1] = DATA_BITS_GET(data, 5, 5);
    pcoef_bin_Even[TAB1] = DATA_BITS_GET(data, 4, 0);
    PATCH_PRINT(" tap1E_sign = 0x%X", psign_out_Even[TAB1]);
    PATCH_PRINT(" tap1E_value = 0x%X", pcoef_bin_Even[TAB1]);

    if (pcoef_bin_Even[TAB1] >= 0x10)
    {
        PATCH_PRINT("recal!");
        pinfo->status.flag_recal = 1;
    }

    //#==============================================================
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 20, 5, 0, 0x6);
    //#==============================================================

    PHY_REG_RD(ps->unit, ps->mdxMacId, 1, 16, &data);
    psign_out_Odd[TAB1] = DATA_BITS_GET(data, 5, 5);
    pcoef_bin_Odd[TAB1] = DATA_BITS_GET(data, 4, 0);
    //#==============================================================
    PATCH_PRINT(" tap1O_sign = 0x%X", psign_out_Odd[TAB1]);
    PATCH_PRINT(" tap1O_value = 0x%X", pcoef_bin_Odd[TAB1]);

    if (pcoef_bin_Odd[TAB1] >= 0x10)
    {
        PATCH_PRINT("recal!");
        pinfo->status.flag_recal = 1;
    }

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 0);
    //#==============================================================

    _phy_8295_rxCali_a_DBG(ps);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 1);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 17, 5, 0, 0x20);
    //#==============================================================
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 20, 5, 0, 0x2);
    //#==============================================================

    PHY_REG_RD(ps->unit, ps->mdxMacId, 1, 16, &data);
    psign_out_Even[TAB2] = DATA_BITS_GET(data, 5, 5);
    pcoef_bin_Even[TAB2] = DATA_BITS_GET(data, 4, 0);
    //#==============================================================
    PATCH_PRINT(" tap2E_sign = 0x%X", psign_out_Even[TAB2]);
    PATCH_PRINT(" tap2E_value = 0x%X", pcoef_bin_Even[TAB2]);

    if (pcoef_bin_Even[TAB2] >= 0x10)
    {
        PATCH_PRINT("recal!");
        pinfo->status.flag_recal = 1;
    }

    //#==============================================================
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 20, 5, 0, 0x7);
    //#==============================================================

    PHY_REG_RD(ps->unit, ps->mdxMacId, 1, 16, &data);
    psign_out_Odd[TAB2] = DATA_BITS_GET(data, 5, 5);
    pcoef_bin_Odd[TAB2] = DATA_BITS_GET(data, 4, 0);
    //#==============================================================
    PATCH_PRINT(" tap2O_sign = 0x%X", psign_out_Odd[TAB2]);
    PATCH_PRINT(" tap2O_value = 0x%X", pcoef_bin_Odd[TAB2]);

    if (pcoef_bin_Odd[TAB2] >= 0x10)
    {
        PATCH_PRINT("recal!");
        pinfo->status.flag_recal = 1;
    }

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 0);
    //#==============================================================

    _phy_8295_rxCali_a_DBG(ps);


    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 1);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 17, 5, 0, 0x20);
    //#==============================================================
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 20, 5, 0, 0x3);
    //#==============================================================

    PHY_REG_RD(ps->unit, ps->mdxMacId, 1, 16, &data);
    psign_out_Even[TAB3] = DATA_BITS_GET(data, 5, 5);
    pcoef_bin_Even[TAB3] = DATA_BITS_GET(data, 4, 0);
    //#==============================================================
    PATCH_PRINT(" tap3E_sign = 0x%X", psign_out_Even[TAB3]);
    PATCH_PRINT(" tap3E_value = 0x%X", pcoef_bin_Even[TAB3]);

    if (pcoef_bin_Even[TAB3] >= 0x10)
    {
        PATCH_PRINT("recal!");
        pinfo->status.flag_recal = 1;
    }

    //#==============================================================
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 20, 5, 0, 0x8);
    //#==============================================================

    PHY_REG_RD(ps->unit, ps->mdxMacId, 1, 16, &data);
    psign_out_Odd[TAB3] = DATA_BITS_GET(data, 5, 5);
    pcoef_bin_Odd[TAB3] = DATA_BITS_GET(data, 4, 0);
    //#==============================================================
    PATCH_PRINT(" tap3O_sign = 0x%X", psign_out_Odd[TAB3]);
    PATCH_PRINT(" tap3O_value = 0x%X", pcoef_bin_Odd[TAB3]);

    if (pcoef_bin_Odd[TAB3] >= 0x10)
    {
        PATCH_PRINT("recal!");
        pinfo->status.flag_recal = 1;
    }

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 0);
    //#==============================================================

    _phy_8295_rxCali_a_DBG(ps);


    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 1);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 17, 5, 0, 0x20);
    //#==============================================================
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 20, 5, 0, 0x4);
    //#==============================================================

    PHY_REG_RD(ps->unit, ps->mdxMacId, 1, 16, &data);
    psign_out_Even[TAB4] = DATA_BITS_GET(data, 5, 5);
    pcoef_bin_Even[TAB4] = DATA_BITS_GET(data, 4, 0);
    //#==============================================================
    PATCH_PRINT(" tap4E_sign = 0x%X", psign_out_Even[TAB4]);
    PATCH_PRINT(" tap4E_value = 0x%X", pcoef_bin_Even[TAB4]);

    if (pcoef_bin_Even[TAB4] >= 0x10)
    {
        PATCH_PRINT("recal!");
        pinfo->status.flag_recal = 1;
    }

    //#==============================================================
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 20, 5, 0, 0x9);
    //#==============================================================


    PHY_REG_RD(ps->unit, ps->mdxMacId, 1, 16, &data);
    psign_out_Odd[TAB4] = DATA_BITS_GET(data, 5, 5);
    pcoef_bin_Odd[TAB4] = DATA_BITS_GET(data, 4, 0);
    //#==============================================================
    PATCH_PRINT(" tap4O_sign = 0x%X", psign_out_Odd[TAB4]);
    PATCH_PRINT(" tap4O_value = 0x%X", pcoef_bin_Odd[TAB4]);
    //#==============================================================

    if (pcoef_bin_Odd[TAB4] >= 0x10)
    {
        PATCH_PRINT("recal!");
        pinfo->status.flag_recal = 1;
    }

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 0);
    //#==============================================================
}


void
_phy_8295_rxCali_a_dump_leq(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo, uint32 *pLeq)
{
    uint32      leq_cali;

    _phy_8295_rxCali_a_DBG(ps);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 1);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 17, 5, 0, 0x10);
    //#==============================================================
    //#LEQ Read Out
    PHY_REG_FIELD_RD(ps->unit, ps->mdxMacId, 1, 16, 7, 3, &leq_cali);
    //#==============================================================

    PATCH_PRINT(" leq_cali_S%u = %u", ps->sdsId, leq_cali);

    //# dis RX_EN_TEST
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 0);
    *pLeq = leq_cali;

    if (leq_cali >= 0x10)
    {
        PATCH_PRINT("leq_cali = 0x%x! recal", leq_cali);
        pinfo->status.flag_recal = 1;
    }

}

void
_phy_8295_rxCali_a_servo(phy_8295_rxCaliSds_t *ps)
{
    uint32      servo_data_even_bin;
    uint32      servo_data_Odd_bin;

    _phy_8295_rxCali_a_DBG(ps);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 1);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 17, 5, 0, 0x20);
    //#==============================================================

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 20, 5, 0, 0xB);
    //#==============================================================
    PHY_REG_FIELD_RD(ps->unit, ps->mdxMacId, 1, 16, 4, 0, &servo_data_even_bin);
    PATCH_PRINT(" servo_data_even_bin = %u", servo_data_even_bin);
    //#==============================================================

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 20, 5, 0, 0x29);
    //#==============================================================
    PHY_REG_FIELD_RD(ps->unit, ps->mdxMacId, 1, 16, 4, 0, &servo_data_Odd_bin);
    //#==============================================================
    PATCH_PRINT(" servo_data_Odd_bin = %u", servo_data_Odd_bin);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 0);
}


void
_phy_8295_rxCali_a_start_1d1d1(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo)
{
    /* <<< start_1.1.1 >>> */

    PATCH_PRINT("start_1.1.1 ++");

    PHY_SDS_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 21, 0x0001);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 20, 10, 0, 0x400);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 188, 18, 0, 0, 1);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 22, 14, 11, 0xF);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 191, 16, 15, 9, 0x41);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 191, 17, 15, 9, 0x41);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 188, 16, 3, 0, 0xF);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 22, 14, 8, 0x0);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 188, 19, 5, 0, pinfo->param.tap0_init_val);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 184, 19, 11, 0, 0x0);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 184, 20, 11, 0, 0x0);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 184, 21, 11, 0, 0x0);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 184, 22, 11, 0, 0x0);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 19, 5, 0, 0x3F);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 19, 5, 3, pinfo->param.vth_min);


    PATCH_PRINT("start_1.1.1 --");

}

void
_phy_8295_rxCali_a_start_1d2(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo)
{
    /* <<< start_1.2 >>> */
    uint32          data1, data2, data2t, data3;


    PATCH_PRINT("start_1.2 ++");

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 185, 23, 15, 15, 1);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 188, 18, 2, 1, 0x1);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 21, 0, 0, 0);

    osal_time_udelay(1 * 1000);
    PHY_SDS_RD(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, &data1);
    data2 = data1 & 0xFED1 ;
    data2t = data2 | 0x0010 ;
    PHY_SDS_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, data2t);
    osal_time_udelay(1 * 1000);
    data3 = data2 & 0xFEC1 ;
    PHY_SDS_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, data3);
    osal_time_udelay(1 * 1000);


    PATCH_PRINT("start_1.2 --");
}


void
_phy_8295_rxCali_a_start_1d2d3(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo, uint32 *fgcal_offset_binary)
{
    /* <<< start_1.2.3 >>> */
    uint32      datat71;

    PATCH_PRINT("start_1.2.3 ++");

    //#==============================================================
    _phy_8295_rxCali_a_DBG(ps);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 1);


    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 17, 5, 0, 0x20);


    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 20, 5, 0, 0xF);
    //#==============================================================

    PHY_REG_RD(ps->unit, ps->mdxMacId, 1, 16, &datat71);
    *fgcal_offset_binary = datat71 & 0x003F;

    //#==============================================================

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 0);
    //#==============================================================

    PATCH_PRINT("start_1.2.3 --");
}

void
_phy_8295_rxCali_a_FGCAL_chk(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo, uint32 fgcal_offset_binary)
{
    /* <<< FGCAL_chk >>> */
    uint32      data;
    uint32      FGCAL_flag;
    uint32      offset_range;

    //#===============================================================================================
    FGCAL_flag = ((fgcal_offset_binary > 60) || (fgcal_offset_binary < 4)) ? 1 : 0;
    if (FGCAL_flag == 1)
    {
        PHY_SDS_RD(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, &data);
        offset_range = data >> 14;
        PATCH_PRINT("Range = %u, Redo FGCAL w/Range +1.", offset_range);
        switch (offset_range)
        {
          case 3:
            PATCH_PRINT("FgCal marginal!");
            break;
          case 2:
            PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 15, 14, 0x3);

            _phy_8295_rxCali_a_start_1d2(ps, pinfo);
            _phy_8295_rxCali_a_start_1d2d3(ps, pinfo, &fgcal_offset_binary);
            break;
          case 1:
            PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 15, 14, 0x2);

            _phy_8295_rxCali_a_start_1d2(ps, pinfo);
            _phy_8295_rxCali_a_start_1d2d3(ps, pinfo, &fgcal_offset_binary);
            break;
        }/* switch */
    }

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 20, 10, 5, fgcal_offset_binary);


    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 21, 0, 0, 1);
    //#======================================================================================
}



void
_phy_8295_rxCali_a_start_1d2d4(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo)
{
    /* <<< start_1.2.4 >>> */
    uint32      data1, data2, data2t, data3, data81;
    uint32      dcvs0_coef_bin, dcvs0_sign_out,
                dcvs1_coef_bin, dcvs1_sign_out,
                dcvs2_coef_bin, dcvs2_sign_out,
                dcvs3_coef_bin, dcvs3_sign_out,
                dcvs4_coef_bin, dcvs4_sign_out,
                dcvs5_coef_bin, dcvs5_sign_out;

    PATCH_PRINT("start_1.2.4 ++");

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 188, 18, 2, 1, 0x3);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 22, 14, 11, 0x0);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 191, 16, 9, 9, 0);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 191, 17, 9, 9, 0);
    //#================================================================


    PHY_SDS_RD(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, &data1);
    data2  = data1 & 0xFED1;
    data2t = data2 | 0x0010;
    PHY_SDS_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, data2t);
    osal_time_udelay(1 * 1000);
    data3 = data2 & 0xFEC1;
    PHY_SDS_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, data3);
    osal_time_udelay(1 * 1000);


    //#==============================================================

    _phy_8295_rxCali_a_DBG(ps);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 1);
    //#==============================================================

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 17, 5, 0, 0x20);


    //#==============================================================

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 20, 5, 0, 0x22);
    //#==============================================================

    PHY_REG_RD(ps->unit, ps->mdxMacId, 1, 16, &data81);
    dcvs0_coef_bin = DATA_BITS_GET(data81, 3, 0);
    dcvs0_sign_out = DATA_BITS_GET(data81, 4, 4);

    PATCH_PRINT(" dcvs0_coef_bin = %u",  dcvs0_coef_bin);
    PATCH_PRINT(" dcvs0_sign_out = %u", dcvs0_sign_out);


    //#==============================================================

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 20, 5, 0, 0x23);
    //#==============================================================

    PHY_REG_RD(ps->unit, ps->mdxMacId, 1, 16, &data81);
    dcvs1_coef_bin = DATA_BITS_GET(data81, 3, 0);
    dcvs1_sign_out = DATA_BITS_GET(data81, 4, 4);

    PATCH_PRINT(" dcvs1_coef_bin = %u ", dcvs1_coef_bin);
    PATCH_PRINT(" dcvs1_sign_out = %u ", dcvs1_sign_out);


    //#==============================================================

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 20, 5, 0, 0x24);
    //#==============================================================

    PHY_REG_RD(ps->unit, ps->mdxMacId, 1, 16, &data81);
    dcvs2_coef_bin = DATA_BITS_GET(data81, 3, 0);
    dcvs2_sign_out = DATA_BITS_GET(data81, 4, 4);

    PATCH_PRINT(" dcvs2_coef_bin = %u ", dcvs2_coef_bin);
    PATCH_PRINT(" dcvs2_sign_out = %u ", dcvs2_sign_out);


    //#==============================================================

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 20, 5, 0, 0x25);
    //#==============================================================

    PHY_REG_RD(ps->unit, ps->mdxMacId, 1, 16, &data81);
    dcvs3_coef_bin = DATA_BITS_GET(data81, 3, 0);
    dcvs3_sign_out = DATA_BITS_GET(data81, 4, 4);

    PATCH_PRINT(" dcvs3_coef_bin = %u", dcvs3_coef_bin);
    PATCH_PRINT(" dcvs3_sign_out = %u", dcvs3_sign_out);


    //#==============================================================

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 20, 5, 0, 0x2C);
    //#==============================================================

    PHY_REG_RD(ps->unit, ps->mdxMacId, 1, 16, &data81);
    dcvs4_coef_bin = DATA_BITS_GET(data81, 3, 0);
    dcvs4_sign_out = DATA_BITS_GET(data81, 4, 4);

    PATCH_PRINT(" dcvs4_coef_bin = %u", dcvs4_coef_bin);
    PATCH_PRINT(" dcvs4_sign_out = %u", dcvs4_sign_out);


    //#==============================================================

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 189, 20, 5, 0, 0x2D);
    //#==============================================================

    PHY_REG_RD(ps->unit, ps->mdxMacId, 1, 16, &data81);
    dcvs5_coef_bin = DATA_BITS_GET(data81, 3, 0);
    dcvs5_sign_out = DATA_BITS_GET(data81, 4, 4);

    PATCH_PRINT(" dcvs5_coef_bin = %u", dcvs5_coef_bin);
    PATCH_PRINT(" dcvs5_sign_out = %u", dcvs5_sign_out);


    //#==============================================================

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 21, 9, 9, 0);
    //#===================================================================
    _phy_8295_rxCali_a_input_DCVS0(ps, dcvs0_sign_out, dcvs0_coef_bin);
    _phy_8295_rxCali_a_input_DCVS1(ps, dcvs1_sign_out, dcvs1_coef_bin);
    _phy_8295_rxCali_a_input_DCVS2(ps, dcvs2_sign_out, dcvs2_coef_bin);
    _phy_8295_rxCali_a_input_DCVS3(ps, dcvs3_sign_out, dcvs3_coef_bin);
    _phy_8295_rxCali_a_input_DCVS4(ps, dcvs4_sign_out, dcvs4_coef_bin);
    _phy_8295_rxCali_a_input_DCVS5(ps, dcvs5_sign_out, dcvs5_coef_bin);
    //#==============================================================

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 22, 14, 11, 0xF);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 191, 16, 9, 9, 1);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 191, 17, 9, 9, 1);

    PATCH_PRINT("start_1.2.4 --");
}

void
_phy_8295_rxCali_a_start_1d3d1(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo)
{
    /* <<< start_1.3.1 >>> */
    int32       i;
    uint32      leq_temp, sum10, avg10, int10;
    uint32      LEQ_OFFSET;
    uint32      flag_dac_long = pinfo->param.cable_offset;

    PATCH_PRINT("start_1.3.1 ++");

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 16, 15, 15, 0);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 23, 7, 7, 0);

    //#1.3.2(a) dump LEQ x 10times. get sum10, avg10(round), avg10(int)
    sum10 = 0;
    for (i = 0; i < 10; i++)
    {
        osal_time_usleep(10 * 1000);
        _phy_8295_rxCali_a_dump_leq(ps, pinfo, &leq_temp);
        sum10 += leq_temp;
    }

    avg10 = (sum10/10) + (((sum10 % 10) >= 5) ? 1 : 0);     //avg10 = round($::sum10/10.0);
    int10 = sum10/10;

    if (flag_dac_long != 0)
    {
        LEQ_OFFSET = avg10 + flag_dac_long;
        PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 22, 15, 8, LEQ_OFFSET);

        PATCH_PRINT(" Set %d +%d.", avg10, flag_dac_long);
        //#1.3.2 Stop LEQ training eq_hold=1, 1.3.3(d)set EQ_SELREG =1 to load LEQ manual value
        PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 23, 7, 7, 1);
        //#11/25 EQOUT_OFFSET +3
        PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 23, 6, 2, flag_dac_long);
        PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 16, 15, 15, 1);
    }
    else if (sum10 >= 5)
    {
        LEQ_OFFSET = avg10 +3;
        PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 22, 15, 8, LEQ_OFFSET);

        PATCH_PRINT(" Set %u +3.", avg10);
        //#1.3.2 Stop LEQ training eq_hold=1, 1.3.3(d)set EQ_SELREG =1 to load LEQ manual value
        PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 23, 7, 7, 1);
        //#11/25 EQOUT_OFFSET +2
        PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 23, 6, 2, 0x3);
        PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 16, 15, 15, 1);
    }
    else
    {
        LEQ_OFFSET = int10;
        PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 22, 15, 8, LEQ_OFFSET);

        PATCH_PRINT(" Set %u+0.", int10);
        //#1.3.2 Stop LEQ training eq_hold=1, 1.3.3(d)set EQ_SELREG =1 to load LEQ manual value
        PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 23, 7, 7, 1);
        //#11/25 EQOUT_OFFSET +0
        PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 23, 6, 2, 0x0);
        PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 16, 15, 15, 1);
    }

    PATCH_PRINT("start_1.3.1 --");
}

void
_phy_8295_rxCali_a_start_1d4(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo)
{
    /* <<< start_1.4 >>> */
    uint32      data1, data1t;
    uint32      data2, data2t;
    uint32      tap0_sign_out, tap0_coef_bin, vthp_set_bin, vthn_set_bin;
    uint32      fgcal_offset_binary;
    uint32      leq_temp;
    uint32      flag_dac_long = pinfo->param.cable_offset;
    uint32      eq_hold_en = (pinfo->param.flags & PHY_8295_RXCALI_PARAM_FLAG_EQ_HOLD_DISABLE) ? 0 : 1;

    PATCH_PRINT("start_1.4 ++");

    PHY_SDS_RD(ps->unit, ps->mdxMacId, ps->sdsId, 185, 23, &data1);
    data1t = data1 & 0xDF7F;
    PHY_SDS_WR(ps->unit, ps->mdxMacId, ps->sdsId, 185, 23, data1t);

    osal_time_usleep(10 * 1000);

    _phy_8295_rxCali_a_Readtap0(ps, pinfo, &tap0_sign_out, &tap0_coef_bin, &vthp_set_bin, &vthn_set_bin);
    _phy_8295_rxCali_a_input_tap0(ps, tap0_sign_out, tap0_coef_bin);
    _phy_8295_rxCali_a_input_vth(ps, vthp_set_bin, vthn_set_bin);

    PHY_SDS_RD(ps->unit, ps->mdxMacId, ps->sdsId, 185, 23, &data2);
    data2t = data2 | 0x2080;
    PHY_SDS_WR(ps->unit, ps->mdxMacId, ps->sdsId, 185, 23, data2t);

    osal_time_usleep(10 * 1000);

    _phy_8295_rxCali_a_start_1d2(ps, pinfo);
    _phy_8295_rxCali_a_start_1d2d3(ps, pinfo, &fgcal_offset_binary);
    PATCH_PRINT(" fg_bin_S%u=%u", ps->sdsId, fgcal_offset_binary);
    _phy_8295_rxCali_a_FGCAL_chk(ps, pinfo, fgcal_offset_binary);

    _phy_8295_rxCali_a_start_1d2d4(ps, pinfo);

    osal_time_usleep(50 * 1000);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 16, 15, 15, 0);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 23, 7, 7, 0);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 185, 20, 8, 8, 1);
    _phy_8295_rxCali_a_dump_leq(ps, pinfo, &leq_temp);
    PATCH_PRINT(" max %u", leq_temp);

    //#add a option to set fixed LEQ value and make flag_dac_long offset value active.
    if (eq_hold_en == 1)
    {
        PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 186, 23, 7, 7, 1);
        _phy_8295_rxCali_a_dump_leq(ps, pinfo, &leq_temp);
        PATCH_PRINT(" max_eq_hold %u + %u", leq_temp, flag_dac_long);
    }

    PATCH_PRINT("start_1.4 --");

}


void
_phy_8295_rxCali_a_start_1d5d2(phy_8295_rxCaliSds_t *ps, phy_8295_rxCaliInfo_t *pinfo)
{
    /* <<< start_1.5.2 >>> */
    uint32      data1, data2;
    uint32      sign_out_Even[TAB_END];
    uint32      coef_bin_Even[TAB_END];
    uint32      sign_out_Odd[TAB_END];
    uint32      coef_bin_Odd[TAB_END];
    uint32      dfe_auto_en = (pinfo->param.flags & PHY_8295_RXCALI_PARAM_FLAG_DFEAUTO_DISABLE) ? 0 : 1;

    PATCH_PRINT("start_1.5.2 ++");

    //#=======================================

    PHY_SDS_RD(ps->unit, ps->mdxMacId, ps->sdsId, 185, 23, &data1);
    PHY_SDS_WR(ps->unit, ps->mdxMacId, ps->sdsId, 185, 23, (data1 & 0xF0FF));

    if (dfe_auto_en == 1)
    {
        return;
    }

    //#==============================================================
    _phy_8295_rxCali_a_Othertap(ps, pinfo, sign_out_Even, coef_bin_Even, sign_out_Odd, coef_bin_Odd);
    _phy_8295_rxCali_a_input_tap1_even(ps, sign_out_Even[TAB1], coef_bin_Even[TAB1]);
    _phy_8295_rxCali_a_input_tap1_odd(ps,  sign_out_Odd[TAB1],  coef_bin_Odd[TAB1]);
    _phy_8295_rxCali_a_input_tap2_even(ps, sign_out_Even[TAB2], coef_bin_Even[TAB2]);
    _phy_8295_rxCali_a_input_tap2_odd(ps,  sign_out_Odd[TAB2],  coef_bin_Odd[TAB2]);
    _phy_8295_rxCali_a_input_tap3_even(ps, sign_out_Even[TAB3], coef_bin_Even[TAB3]);
    _phy_8295_rxCali_a_input_tap3_odd(ps,  sign_out_Odd[TAB3],  coef_bin_Odd[TAB3]);
    _phy_8295_rxCali_a_input_tap4_even(ps, sign_out_Even[TAB4], coef_bin_Even[TAB4]);
    _phy_8295_rxCali_a_input_tap4_odd(ps,  sign_out_Odd[TAB4],  coef_bin_Odd[TAB4]);

    PATCH_PRINT(".");

    PHY_SDS_RD(ps->unit, ps->mdxMacId, ps->sdsId, 185, 23, &data2);
    PHY_SDS_WR(ps->unit, ps->mdxMacId, ps->sdsId, 185, 23, (data2 | 0x2F80));

    PATCH_PRINT("start_1.5.2 --");
}

void
_phy_8295_rxCali_a_eye_scan(phy_8295_rxCaliSds_t *ps, int32 eye_int)
{
    /* <<< eye_scan >>> */
    uint32  eye_valid, score;

    PATCH_PRINT("unit %u port %u Enable S%u eye_scan", ps->unit, ps->mdxMacId, ps->sdsId);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 133, 16, 8, 5, eye_int);
    PATCH_PRINT("EYE_Vth_Interval = %d", eye_int);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 18, 9, 6, 0x9);
    PHY_SDS_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 19, 0xFFFF);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 187, 18, 5, 0, 0x1);
    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 184, 18, 12, 11, 0x1);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 185, 16, 14, 14, 0x1);
    PATCH_PRINT(" Eye_scan_en =1");
    PHY_SDS_FIELD_RD(ps->unit, ps->mdxMacId, ps->sdsId, 133, 18, 8, 8, &eye_valid);
    PATCH_PRINT(" EYE_valid = %u", eye_valid);
    PHY_SDS_FIELD_RD(ps->unit, ps->mdxMacId, ps->sdsId, 133, 18, 7, 0, &score);
    score = 256 - score;
    PATCH_PRINT(" EYE_score = %u", score);

    PHY_SDS_FIELD_WR(ps->unit, ps->mdxMacId, ps->sdsId, 185, 16, 14, 14, 0x0);
    PATCH_PRINT(" Disable Eye_Scan");
}


#if defined(CONFIG_SDK_RTL8214QF)
void
phy_8214qf_patch_set(uint32 unit, uint32 basePort)
{
    int32           ret;
    rtk_port_t      mdxMacId;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,basePort=%u", unit, basePort);

    if ((ret = phy_8295_mdxMacId_get(unit, basePort, &mdxMacId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit %u port %u get mdxMacId failed", unit, basePort);
        return;
    }

    if (RTK_PHYTYPE_RTL8214QF_NC5 == HWP_PHY_MODEL_BY_PORT(unit, basePort))
    {
        _phy_8295_patch_bT_init_chip(unit, basePort, mdxMacId);
    }
    else
    {
        _phy_8295_patch_a_init_rtl8214qf(unit, basePort, mdxMacId);
    }

    _phy_8295_patch_8214qf_config(unit, basePort, mdxMacId);
}
#endif

#if defined(CONFIG_SDK_RTL8214QF)
void
phy_8214qf_patch_cust_set(uint32 unit, uint32 basePort, phy_8214qf_conf_entry_t *pConf_ent)
{
    int32           ret;
    rtk_port_t      mdxMacId;
    phy_8295_pageRegFieldVal_t  *conf_param;

    if (pConf_ent == NULL)
    {
        return;
    }

    if ((conf_param = pConf_ent->conf_param) == NULL)
    {
        return;
    }

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,basePort=%u", unit, basePort);

    if ((ret = phy_8295_mdxMacId_get(unit, basePort, &mdxMacId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit %u port %u get mdxMacId failed", unit, basePort);
        return;
    }

    while(conf_param->page != PHY_8295_PAGEREGVAL_END)
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "CUST_CONF %u.%u.[%u:%u]=0x%X", (uint32)conf_param->page, (uint32)conf_param->reg, conf_param->endBit, conf_param->startBit, (uint32)conf_param->val);
        PHY_REG_FIELD_WR(unit, mdxMacId, (uint32)conf_param->page, (uint32)conf_param->reg, conf_param->endBit, conf_param->startBit, (uint32)conf_param->val);
        conf_param++;
    }
}
#endif



#if defined(CONFIG_SDK_RTL8224QF)
void
_phy_8295_patch_init_rtl8224qf(uint32 unit, uint32 port, uint32 mdxMacId)
{
    uint32  val;

    PHY_REG_FIELD_RD(unit, mdxMacId, 0, 19, 8, 4, &val);
    if (val == 0)
    {
        PHY_SDS_FIELD_WR(unit, mdxMacId, 0, 25, 21, 11, 8, 0xF);
        PHY_SDS_FIELD_WR(unit, mdxMacId, 1, 25, 21, 11, 8, 0xF);
    }

    PHY_SDS_FIELD_WR(unit, mdxMacId, 0, 27, 17, 6, 6, 0);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 1, 27, 17, 6, 6, 0);
    PHY_REG_WR(unit, mdxMacId, 1, 22, 0x5820);

    PHY_REG_WR(unit, mdxMacId, 5, 17, 0x1f);
    PHY_REG_WR(unit, mdxMacId, 5, 20, 0x1f);
    PHY_REG_WR(unit, mdxMacId, 5, 21, 0x1f);
    PHY_REG_WR(unit, mdxMacId, 5, 22, 0x1f);
    PHY_REG_WR(unit, mdxMacId, 5, 23, 0x1f);

    //config s0 to USXGMII, 4 port sub-mode
    PHY_REG_WR(unit, mdxMacId, 5, 17, 0xD);
    PHY_REG_WR(unit, mdxMacId, 7, 22, 0x20);

    //set S4~S7 to 2500BASE-X
    PHY_REG_WR(unit, mdxMacId, 5, 20, 0x16);
    PHY_REG_WR(unit, mdxMacId, 5, 21, 0x16);
    PHY_REG_WR(unit, mdxMacId, 5, 22, 0x16);
    PHY_REG_WR(unit, mdxMacId, 5, 23, 0x16);

    PHY_SDS_WR(unit, mdxMacId, 0, 132, 16, 0xE350);
    PHY_SDS_WR(unit, mdxMacId, 0, 133, 17, 0x1);

    PHY_SDS_WR(unit, mdxMacId, 0, 132, 21, 0x420D);
    PHY_SDS_WR(unit, mdxMacId, 0, 132, 18, 0x1CA4);

    PHY_SDS_WR(unit, mdxMacId, 0, 184, 19, 0x8000);
    PHY_SDS_WR(unit, mdxMacId, 0, 186, 18, 0x2080);
    PHY_SDS_WR(unit, mdxMacId, 0, 186, 20, 0x1378);
    PHY_SDS_WR(unit, mdxMacId, 0, 187, 22, 0x07F8);

    PHY_SDS_WR(unit, mdxMacId, 0, 188, 18, 0x1147);
    PHY_SDS_WR(unit, mdxMacId, 0, 188, 19, 0xFFDF);
    PHY_SDS_WR(unit, mdxMacId, 0, 189, 18, 0xBC2F);
    PHY_SDS_WR(unit, mdxMacId, 0, 189, 19, 0x7C07);
    PHY_SDS_WR(unit, mdxMacId, 0, 191, 19, 0x8000);


    if (val == 0)
    {
        PHY_SDS_WR(unit, mdxMacId, 0, 188, 17, 0x91CC);
        PHY_SDS_WR(unit, mdxMacId, 0, 188, 18, 0x116F);
    }

    PHY_SDS_WR(unit, mdxMacId, 0, 187, 22, 0x07FA);

    PHY_SDS_WR(unit, mdxMacId, 0, 168, 19, 0xF000);
    PHY_SDS_WR(unit, mdxMacId, 0, 169, 23, 0xBFDF);
    PHY_SDS_WR(unit, mdxMacId, 0, 170, 18, 0x2100);
    PHY_SDS_WR(unit, mdxMacId, 0, 170, 20, 0x1378);
    PHY_SDS_WR(unit, mdxMacId, 0, 171, 22, 0x07FA);
    PHY_SDS_WR(unit, mdxMacId, 0, 172, 18, 0x114F);
    PHY_SDS_WR(unit, mdxMacId, 0, 172, 21, 0x787C);
    PHY_SDS_WR(unit, mdxMacId, 0, 175, 16, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 0, 175, 17, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 0, 175, 21, 0x0019);
    PHY_SDS_WR(unit, mdxMacId, 0, 175, 22, 0xF0E0);

    PHY_SDS_WR(unit, mdxMacId, 1, 132, 16, 0xE350);
    PHY_SDS_WR(unit, mdxMacId, 1, 133, 17, 0x1);

    PHY_SDS_WR(unit, mdxMacId, 1, 132, 21, 0x420D);

    PHY_SDS_WR(unit, mdxMacId, 1, 184, 19, 0x8000);
    PHY_SDS_WR(unit, mdxMacId, 1, 186, 18, 0x2080);
    PHY_SDS_WR(unit, mdxMacId, 1, 186, 20, 0x1378);
    PHY_SDS_WR(unit, mdxMacId, 1, 187, 22, 0x07F8);

    PHY_SDS_WR(unit, mdxMacId, 1, 188, 18, 0x1147);
    PHY_SDS_WR(unit, mdxMacId, 1, 188, 19, 0xFFDF);
    PHY_SDS_WR(unit, mdxMacId, 1, 189, 18, 0xBC2F);
    PHY_SDS_WR(unit, mdxMacId, 1, 189, 19, 0x7C07);
    PHY_SDS_WR(unit, mdxMacId, 1, 191, 19, 0x8000);

    if (val == 0)
    {
        PHY_SDS_WR(unit, mdxMacId, 1, 188, 17, 0x91CC);
        PHY_SDS_WR(unit, mdxMacId, 1, 188, 18, 0x116F);
    }

    PHY_SDS_WR(unit, mdxMacId, 1, 187, 22, 0x07FA);

    PHY_SDS_WR(unit, mdxMacId, 1, 168, 19, 0xF000);
    PHY_SDS_WR(unit, mdxMacId, 1, 169, 23, 0xBFDF);
    PHY_SDS_WR(unit, mdxMacId, 1, 170, 18, 0x2100);
    PHY_SDS_WR(unit, mdxMacId, 1, 170, 20, 0x1378);
    PHY_SDS_WR(unit, mdxMacId, 1, 171, 22, 0x07FA);
    PHY_SDS_WR(unit, mdxMacId, 1, 172, 18, 0x114F);
    PHY_SDS_WR(unit, mdxMacId, 1, 172, 21, 0x787C);
    PHY_SDS_WR(unit, mdxMacId, 1, 175, 16, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 1, 175, 17, 0x8107);
    PHY_SDS_WR(unit, mdxMacId, 1, 175, 21, 0x0019);
    PHY_SDS_WR(unit, mdxMacId, 1, 175, 22, 0xF0E0);

    PHY_SDS_FIELD_WR(unit, mdxMacId, 4, 145, 20, 15, 12, 0xe);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 5, 145, 20, 15, 12, 0xe);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 6, 145, 20, 15, 12, 0xe);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 7, 145, 20, 15, 12, 0xe);

    PHY_SDS_FIELD_WR(unit, mdxMacId, 4, 144, 19, 6, 2, 0x16);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 5, 144, 19, 6, 2, 0x16);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 6, 144, 19, 6, 2, 0x16);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 7, 144, 19, 6, 2, 0x16);


    PHY_SDS_FIELD_WR(unit, mdxMacId, 4, 0, 20, 2, 2, 1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 5, 0, 20, 2, 2, 1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 6, 0, 20, 2, 2, 1);
    PHY_SDS_FIELD_WR(unit, mdxMacId, 7, 0, 20, 2, 2, 1);

    PHY_SDS_WR(unit, mdxMacId, 0, 128, 16, 0x400);
    PHY_SDS_WR(unit, mdxMacId, 0, 128, 16, 0xc00);
    PHY_SDS_WR(unit, mdxMacId, 0, 128, 16, 0x0);

    PHY_SDS_WR(unit, mdxMacId, 0, 190, 20, 0x8389);
    PHY_SDS_WR(unit, mdxMacId, 0, 190, 21, 0x3F89);
    PHY_SDS_WR(unit, mdxMacId, 1, 190, 20, 0x8389);
    PHY_SDS_WR(unit, mdxMacId, 1, 190, 21, 0x3F89);

    PHY_SDS_WR(unit, mdxMacId, 0, 174, 20, 0x8294);
    PHY_SDS_WR(unit, mdxMacId, 0, 174, 21, 0x3E94);
    PHY_SDS_WR(unit, mdxMacId, 1, 174, 20, 0x8294);
    PHY_SDS_WR(unit, mdxMacId, 1, 174, 21, 0x3E94);

    PHY_SDS_WR(unit, mdxMacId, 0, 150, 20, 0x8294);
    PHY_SDS_WR(unit, mdxMacId, 0, 150, 21, 0x3E94);
    PHY_SDS_WR(unit, mdxMacId, 1, 150, 20, 0x8294);
    PHY_SDS_WR(unit, mdxMacId, 1, 150, 21, 0x3E94);


    PHY_SDS_WR(unit, mdxMacId, 4, 124, 21, 0x4);
    PHY_SDS_WR(unit, mdxMacId, 5, 124, 21, 0x4);
    PHY_SDS_WR(unit, mdxMacId, 6, 124, 21, 0x4);
    PHY_SDS_WR(unit, mdxMacId, 7, 124, 21, 0x4);


    PHY_SDS_WR(unit, mdxMacId, 0, 26, 18, 0xA4);

    PHY_SDS_WR(unit, mdxMacId, 0, 25, 22, 0x3F5A);

    PHY_SDS_WR(unit, mdxMacId, 0, 26, 19, 0x68C1);
    PHY_SDS_WR(unit, mdxMacId, 0, 26, 20, 0xF021);

    PHY_SDS_WR(unit, mdxMacId, 0, 28, 22, 0x1401);

    PHY_SDS_WR(unit, mdxMacId, 0, 26, 19, 0x0);
    PHY_SDS_WR(unit, mdxMacId, 0, 26, 20, 0x0);
    PHY_SDS_WR(unit, mdxMacId, 0, 26, 21, 0x0);
    PHY_SDS_WR(unit, mdxMacId, 0, 26, 22, 0x0);
    PHY_SDS_WR(unit, mdxMacId, 0, 26, 23, 0x0);
    PHY_SDS_WR(unit, mdxMacId, 0, 27, 16, 0x0);

    PHY_SDS_WR(unit, mdxMacId, 0, 26, 18, 0x606);

    PHY_REG_WR(unit, mdxMacId, 1, 21, 0x6);

}
#endif /* defined(CONFIG_SDK_RTL8224QF) */


#if defined(CONFIG_SDK_RTL8224QF)
void
phy_8224qf_patch_set(uint32 unit, uint32 basePort)
{
    int32           ret;
    rtk_port_t      mdxMacId;
    uint8      sdsId;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,basePort=%u", unit, basePort);

    if ((ret = phy_8295_mdxMacId_get(unit, basePort, &mdxMacId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit %u port %u get mdxMacId failed", unit, basePort);
        return;
    }

    _phy_8295_patch_init_rtl8224qf(unit, basePort, mdxMacId);

#ifdef PHY_ONLY
    if(!(HWP_9300_FAMILY_ID(unit) || HWP_9310_FAMILY_ID(unit)))
    {
        /* AM insertion period */
        PHY_SDS_WR(unit, mdxMacId, 0, 26, 18, 0xA4);
    }
#endif

    /* disable serdes to prevent port link during init */
    PHY_REG_WR(unit, mdxMacId, 5, 20, 0x1f);
    PHY_REG_WR(unit, mdxMacId, 5, 21, 0x1f);
    PHY_REG_WR(unit, mdxMacId, 5, 22, 0x1f);
    PHY_REG_WR(unit, mdxMacId, 5, 23, 0x1f);

    /* change polarity according to hardware profile */
    sdsId = HWP_PORT_SDSID(unit, basePort);
    if (SERDES_POLARITY_CHANGE == HWP_SDS_RX_POLARITY(unit, sdsId))
    {
        PHY_SDS_FIELD_WR(unit, mdxMacId, 0, 0, 16, 9, 9, 0x1);
    }
    if (SERDES_POLARITY_CHANGE == HWP_SDS_TX_POLARITY(unit, sdsId))
    {
        PHY_SDS_FIELD_WR(unit, mdxMacId, 0, 0, 16, 8, 8, 0x1);
    }
}
#endif

#if defined(CONFIG_SDK_RTL8224QF)
void
phy_8224qf_patch_cust_set(uint32 unit, uint32 basePort, phy_8224qf_conf_entry_t *pConf_ent)
{
    int32           ret;
    rtk_port_t      mdxMacId;
    phy_8295_pageRegFieldVal_t  *conf_param;

    if (pConf_ent == NULL)
    {
        return;
    }

    if ((conf_param = pConf_ent->conf_param) == NULL)
    {
        return;
    }

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,basePort=%u", unit, basePort);

    if ((ret = phy_8295_mdxMacId_get(unit, basePort, &mdxMacId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit %u port %u get mdxMacId failed", unit, basePort);
        return;
    }

    while(conf_param->page != PHY_8295_PAGEREGVAL_END)
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "CUST_CONF %u.%u.[%u:%u]=0x%X", (uint32)conf_param->page, (uint32)conf_param->reg, conf_param->endBit, conf_param->startBit, (uint32)conf_param->val);
        PHY_REG_FIELD_WR(unit, mdxMacId, (uint32)conf_param->page, (uint32)conf_param->reg, conf_param->endBit, conf_param->startBit, (uint32)conf_param->val);
        conf_param++;
    }
}
#endif

int32
phy_8295_rxCali_start(uint32 unit, uint32 basePort, uint32 sdsId, phy_8295_rxCaliParam_t *param)
{
    int32   ret;
    uint32  mdxMacId;
    phy_8295_rxCaliSds_t        rxCaliSds;
    phy_8295_rxCaliInfo_t       rxCaliInfo;


    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sds=%u cabOffset=%u tap0Init=0x%X vthMin=0x%X eqHoldEn=%d DFETap1_4En=%d DFEAuto=%d",
            unit, basePort, sdsId, param->cable_offset, param->tap0_init_val, param->vth_min,
            ((param->flags&PHY_8295_RXCALI_PARAM_FLAG_EQ_HOLD_DISABLE)?0:1), ((param->flags&PHY_8295_RXCALI_PARAM_FLAG_DFETAP1_4_DISABLE)?0:1),
            ((param->flags&PHY_8295_RXCALI_PARAM_FLAG_DFEAUTO_DISABLE)?0:1));

    if ((ret = phy_8295_mdxMacId_get(unit, basePort, &mdxMacId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit %u port %u get mdxMacId failed", unit, basePort);
        return RT_ERR_PORT_ID;
    }

    if ((RTK_PHYTYPE_RTL8295R_C22 == HWP_PHY_MODEL_BY_PORT(unit, basePort)) || (RTK_PHYTYPE_RTL8214QF_NC5 == HWP_PHY_MODEL_BY_PORT(unit, basePort)))
    {
        _phy_8295_patch_bT_startall(unit, basePort, mdxMacId, sdsId);
        return RT_ERR_OK;
    }
    else
    {
        osal_memset(&rxCaliSds, 0, sizeof(phy_8295_rxCaliSds_t));
        rxCaliSds.unit = unit;
        rxCaliSds.port = basePort;
        rxCaliSds.mdxMacId = mdxMacId;
        rxCaliSds.sdsId = sdsId;

        osal_memset(&rxCaliInfo, 0, sizeof(phy_8295_rxCaliInfo_t));
        osal_memcpy(&rxCaliInfo.param, param, sizeof(phy_8295_rxCaliParam_t));

        _phy_8295_rxCali_a_startall(&rxCaliSds, &rxCaliInfo);
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sds=%u flag_recal=%d", unit, basePort, sdsId, rxCaliInfo.status.flag_recal);
        if (rxCaliInfo.status.flag_recal == 0)
        {
            ret = RT_ERR_OK;
        }
        else
        {
            ret = RT_ERR_FAILED;
        }
        return ret;
    }
}


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295_rxCaliRegDefault_init
 * Description:
 *      Initialize rx calibration registers to default value
 * Input:
 *      unit - unit ID
 *      basePort - PHY base port ID
 *      sdsId - PHY SerDes ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_PORT_ID       - input port id is wrong
 * Note:
 *      None
 */
int32
phy_8295_rxCaliRegDefault_init(uint32 unit, uint32 basePort, uint32 sdsId)
{
    int32   ret;
    uint32  mdxMacId;
    phy_8295_rxCaliSds_t        rxCaliSds;
    phy_8295_rxCaliInfo_t       rxCaliInfo;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,sds=%u reg default", unit, basePort, sdsId);

    if ((ret = phy_8295_mdxMacId_get(unit, basePort, &mdxMacId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit %u port %u get mdxMacId failed", unit, basePort);
        return RT_ERR_PORT_ID;
    }

    if ((RTK_PHYTYPE_RTL8295R_C22 == HWP_PHY_MODEL_BY_PORT(unit, basePort)) || (RTK_PHYTYPE_RTL8214QF_NC5 == HWP_PHY_MODEL_BY_PORT(unit, basePort)))
    {
        uint32      short_flag = 0;
        _phy_8295_patch_bT_start_1d1d1(unit, basePort, mdxMacId, sdsId, &short_flag);
        return RT_ERR_OK;
    }
    else
    {
        osal_memset(&rxCaliSds, 0, sizeof(phy_8295_rxCaliSds_t));
        rxCaliSds.unit = unit;
        rxCaliSds.port = basePort;
        rxCaliSds.mdxMacId = mdxMacId;
        rxCaliSds.sdsId = sdsId;

        osal_memset(&rxCaliInfo, 0, sizeof(phy_8295_rxCaliInfo_t));
        phy_8295_rxCaliParamDefault_get(sdsId, &rxCaliInfo.param);

        _phy_8295_rxCali_a_start_1d1d1(&rxCaliSds, &rxCaliInfo);
        return RT_ERR_OK;
    }
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
void
phy_8295r_patch_set(uint32 unit, uint32 basePort)
{
    int32       ret;
    uint32      mdxMacId;

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, basePort);

    if ((ret = phy_8295_mdxMacId_get(unit, basePort, &mdxMacId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit %u port %u get mdxMacId failed", unit, basePort);
        return;
    }

    if (RTK_PHYTYPE_RTL8295R_C22 == HWP_PHY_MODEL_BY_PORT(unit, basePort))
    {
        _phy_8295_patch_bT_init_sfp(unit, basePort, mdxMacId);
    }
    else
    {
        _phy_8295_patch_a_init_rtl8295r(unit, basePort, mdxMacId);
    }

    _phy_8295_patch_8295r_config(unit, basePort, mdxMacId);
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
void
phy_8295r_patch_cust_set(uint32 unit, uint32 basePort, phy_8295r_conf_entry_t *pConf_ent)
{
    int32       ret;
    uint32      mdxMacId;
    phy_8295_pageRegFieldVal_t  *conf_param;

    if (pConf_ent == NULL)
    {
        return;
    }

    if ((conf_param = pConf_ent->conf_param) == NULL)
    {
        return;
    }

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, basePort);
    if ((ret = phy_8295_mdxMacId_get(unit, basePort, &mdxMacId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit %u port %u get mdxMacId failed", unit, basePort);
        return;
    }

    while(conf_param->page != PHY_8295_PAGEREGVAL_END)
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "CUST_CONF %u.%u.[%u:%u]=0x%X", (uint32)conf_param->page, (uint32)conf_param->reg, conf_param->endBit, conf_param->startBit, (uint32)conf_param->val);
        PHY_REG_FIELD_WR(unit, mdxMacId, (uint32)conf_param->page, (uint32)conf_param->reg, conf_param->endBit, conf_param->startBit, (uint32)conf_param->val);
        conf_param++;
    }
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/*
 * Fiber TX attribute
 */
int32
phy_8295r_patch_txParam_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media)
{
    uint32      mdxMacId;
    int32       ret;
    phy_8295r_txParam_t     *p;
    phy_8295_pageRegVal_t   *param;

    if (phy8295rPatch_init[unit] != INIT_COMPLETED)
    {
        RT_ERR(RT_ERR_NOT_INIT, (MOD_HAL), "unit %u port %u not initialed", unit, port);
        return RT_ERR_NOT_INIT;
    }

    p = &gPhy8295rPI[unit].tx_param[port][media];
    if ((param = p->prv) == NULL)
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,media=%d no param!", unit, port, media);
        return RT_ERR_OK;
    }

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,media=%d", unit, port, media);
    if ((ret = phy_8295_mdxMacId_get(unit, port, &mdxMacId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit %u port %u get mdxMacId failed", unit, port);
        return ret;
    }

    while(param->page != PHY_8295_PAGEREGVAL_END)
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,S1TXATTR:page=%x,reg=%x,val=%x", unit, port, (uint32)param->page, (uint32)param->reg, (uint32)param->val);
        /* the page/reg is already indicates S1's phy_page/phy_reg, so use PHY_REG_WR() */
        PHY_REG_WR(unit, mdxMacId, (uint32)param->page, (uint32)param->reg, (uint32)param->val);
        param++;
    }
    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_txParam_init
 * Description:
 *      initial tx parameter of the port.
 *      the parameter are keep in software and will apply to chip when configure 10g media
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      prv    - page/reg/value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_INPUT        - invalid parameter
 * Note:
 *      None
 */
int32
phy_8295r_txParam_init(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media, phy_8295r_txParam_t *tx_param)
{
    phy_8295r_patch_info_t   *ppi;

    if ((unit >= RTK_MAX_NUM_OF_UNIT) || (port >= RTK_MAX_NUM_OF_PORTS))
        return RT_ERR_INPUT;

    if (media >= PORT_10GMEDIA_END)
        return RT_ERR_INPUT;

    if (phy8295rPatch_init[unit] == INIT_NOT_COMPLETED)
    {
        osal_memset(&gPhy8295rPI[unit], 0, sizeof(phy_8295r_patch_info_t));
        phy8295rPatch_init[unit] = INIT_COMPLETED;
    }

    ppi = &gPhy8295rPI[unit];
    ppi->tx_param[port][media].prv = tx_param->prv;
    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_txParamCfg_init
 * Description:
 *      initial tx parameter of the port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      prv    - page/reg/value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_INPUT        - invalid parameter
 * Note:
 *      None
 */
int32
phy_8295r_txParamCfg_init(uint32 unit, rtk_port_t port, phy_8295_txParamCfg_t *tx_param_cfg)
{
    if ((unit >= RTK_MAX_NUM_OF_UNIT) || (port >= RTK_MAX_NUM_OF_PORTS))
        return RT_ERR_INPUT;

    if (phy8295rPatch_init[unit] == INIT_NOT_COMPLETED)
    {
        osal_memset(&gPhy8295rPI[unit], 0, sizeof(phy_8295r_patch_info_t));
        phy8295rPatch_init[unit] = INIT_COMPLETED;
    }

    if (tx_param_cfg->fiber10g_8431_param.prv != NULL)
        phy_8295r_txParam_init(unit, port, PORT_10GMEDIA_FIBER_10G, &tx_param_cfg->fiber10g_8431_param);
    if (tx_param_cfg->dac50cm_tx_param.prv != NULL)
        phy_8295r_txParam_init(unit, port, PORT_10GMEDIA_DAC_50CM,  &tx_param_cfg->dac50cm_tx_param);
    if (tx_param_cfg->dac100cm_tx_param.prv != NULL)
        phy_8295r_txParam_init(unit, port, PORT_10GMEDIA_DAC_100CM, &tx_param_cfg->dac100cm_tx_param);
    if (tx_param_cfg->dac300cm_tx_param.prv != NULL)
        phy_8295r_txParam_init(unit, port, PORT_10GMEDIA_DAC_300CM, &tx_param_cfg->dac300cm_tx_param);
    if (tx_param_cfg->dac500cm_tx_param.prv != NULL)
        phy_8295r_txParam_init(unit, port, PORT_10GMEDIA_DAC_500CM, &tx_param_cfg->dac500cm_tx_param);

    return RT_ERR_OK;
}
#endif


/* Function Name:
 *      phy_8295_rxCaliParamDefault_get
 * Description:
 *      get rx calibration parameter's default
 * Input:
 *      sdsId   - serdes ID
 * Output:
 *      param   - default parameter
 * Return:
 *      RT_ERR_OK           - OK
 * Note:
 *      None
 */
void
phy_8295_rxCaliParamDefault_get(uint32 sdsId, phy_8295_rxCaliParam_t *param)
{
    //set ::offset_dac_long 0
    //set ::eq_hold_en(0) 1
    //set ::eq_hold_en(1) 1
    //set ::tap0_init_val(0) 0x1F
    //set ::tap0_init_val(1) 0x1F
    //set ::vth_min(0) 2
    //set ::vth_min(1) 2
    //set ::1d5d2_en(0) 0
    //set ::1d5d2_en(1) 0
    //set ::dfe_auto_en(0) 0
    //set ::dfe_auto_en(1) 0


    osal_memset(param, 0, sizeof(phy_8295_rxCaliParam_t));
    param->cable_offset  = 0;       /* offset_dac_long */
    param->tap0_init_val = 0x1F;
    param->vth_min       = 2;
    param->flags |= PHY_8295_RXCALI_PARAM_FLAG_DFETAP1_4_DISABLE;   /* 1d5d2_en */
    param->flags |= PHY_8295_RXCALI_PARAM_FLAG_DFEAUTO_DISABLE;     /* dfe_auto_en */

    return;
}


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_rxCaliConfPort_set
 * Description:
 *      Set rx calibration parameters
 * Input:
 *      unit   - unit id
 *      conf   - configuration of RX calibration parameters
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_INPUT        - invalid parameter
 * Note:
 *      None
 */
int32
phy_8295r_rxCaliConfPort_set(uint32 unit, uint32 port, phy_8295_rxCaliConf_t *conf)
{
    rtk_enable_t    enable;

    if (unit >= RTK_MAX_NUM_OF_UNIT)
    {
        return RT_ERR_UNIT_ID;
    }

    if (port >= RTK_MAX_NUM_OF_PORTS)
    {
        return RT_ERR_PORT_ID;
    }

    if (phy8295rPatch_init[unit] == INIT_NOT_COMPLETED)
    {
        osal_memset(&gPhy8295rPI[unit], 0, sizeof(phy_8295r_patch_info_t));
        phy8295rPatch_init[unit] = INIT_COMPLETED;
    }

    if (gPhy8295rPI[unit].prxCaliConf[port] == NULL)
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,malloc", unit, port);
        gPhy8295rPI[unit].prxCaliConf[port] = osal_alloc(sizeof(phy_8295_rxCaliConf_t));
        if (gPhy8295rPI[unit].prxCaliConf[port]  == NULL)
        {
            RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,malloc fail", unit, port);
            return RT_ERR_MEM_ALLOC;
        }
    }

    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u", unit, port);
    osal_memcpy(gPhy8295rPI[unit].prxCaliConf[port], conf, sizeof(phy_8295_rxCaliConf_t));

    enable = (gPhy8295rPI[unit].prxCaliConf[port]->s0.rxCaliEnable) ? ENABLED : DISABLED;
    phy_8295r_portSdsRxCaliEnable_set(unit, port, 0, enable);

    enable = (gPhy8295rPI[unit].prxCaliConf[port]->s1.rxCaliEnable) ? ENABLED : DISABLED;
    phy_8295r_portSdsRxCaliEnable_set(unit, port, 1, enable);

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      phy_8295r_rxCaliConfPort_get
 * Description:
 *      get rx calibration config of the port
 * Input:
 *      unit   - unit id
 * Output:
 *      conf   - configuration of RX calibration parameters
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_INPUT        - invalid parameter
 * Note:
 *      None
 */
int32
phy_8295r_rxCaliConfPort_get(uint32 unit, uint32 port, phy_8295_rxCaliConf_t *conf)
{

    if (unit >= RTK_MAX_NUM_OF_UNIT)
    {
        return RT_ERR_UNIT_ID;
    }

    if (port >= RTK_MAX_NUM_OF_PORTS)
    {
        return RT_ERR_PORT_ID;
    }

    if (phy8295rPatch_init[unit] != INIT_COMPLETED)
    {
        return RT_ERR_NOT_INIT;
    }

    if (gPhy8295rPI[unit].prxCaliConf[port] == NULL)
    {
        return RT_ERR_NULL_POINTER;
    }

    osal_memcpy(conf, gPhy8295rPI[unit].prxCaliConf[port], sizeof(phy_8295_rxCaliConf_t));
    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_rxCaliConf_set
 * Description:
 *      Set rx calibration parameters
 * Input:
 *      unit   - unit id
 *      conf   - configuration of RX calibration parameters
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_INPUT        - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214qf_rxCaliConf_set(uint32 unit, rtk_port_t basePort, phy_8295_rxCaliConf_t *conf)
{
    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u, cableOffset=%u s0(rxcali=%u tap0=0x%x vth=0x%x qe=%u dfeTap1=%u dfeAuto=%u) s1(rxcali=%u tap0=0x%x vth=0x%x qe=%u dfeTap1=%u dfeAuto=%u)", unit, basePort,
                    conf->dacLongCableOffset,
                    conf->s0.rxCaliEnable, conf->s0.tap0InitVal, conf->s0.vthMinThr, conf->s0.eqHoldEnable, conf->s0.dfeTap1_4Enable, conf->s0.dfeAuto,
                    conf->s1.rxCaliEnable, conf->s1.tap0InitVal, conf->s1.vthMinThr, conf->s1.eqHoldEnable, conf->s1.dfeTap1_4Enable, conf->s1.dfeAuto);

    if (unit >= RTK_MAX_NUM_OF_UNIT)
    {
        return RT_ERR_UNIT_ID;
    }

    if (basePort >= RTK_MAX_NUM_OF_PORTS)
    {
        return RT_ERR_PORT_ID;
    }

    if (basePort != HWP_PHY_BASE_MACID(unit, basePort))
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,baseMacId=%u", unit, basePort, HWP_PHY_BASE_MACID(unit, basePort));
        return RT_ERR_PORT_ID;
    }

    if (phy82x4qfPatch_init[unit] == INIT_NOT_COMPLETED)
    {
        osal_memset(&gPhy82x4qfPI[unit], 0, sizeof(phy_8214qf_patch_info_t));
        phy82x4qfPatch_init[unit] = INIT_COMPLETED;
    }

    if (gPhy82x4qfPI[unit].prxCaliConf[basePort] == NULL)
    {
        gPhy82x4qfPI[unit].prxCaliConf[basePort] = osal_alloc(sizeof(phy_8295_rxCaliConf_t));
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,malloc=%X", unit, basePort, (uint32)(uintptr)gPhy82x4qfPI[unit].prxCaliConf[basePort]);
        if (gPhy82x4qfPI[unit].prxCaliConf[basePort] == NULL)
        {
            RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u,malloc fail", unit, basePort);
            return RT_ERR_MEM_ALLOC;
        }
    }
    RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u copy config", unit, basePort);
    osal_memcpy(gPhy82x4qfPI[unit].prxCaliConf[basePort], conf, sizeof(phy_8295_rxCaliConf_t));

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      phy_8214qf_rxCaliConf_get
 * Description:
 *      initial rx calibration parameters
 * Input:
 *      unit   - unit id
 * Output:
 *      conf   - configuration of RX calibration parameters
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_INPUT        - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214qf_rxCaliConf_get(uint32 unit, rtk_port_t basePort, phy_8295_rxCaliConf_t *conf)
{
    if (unit >= RTK_MAX_NUM_OF_UNIT)
    {
        return RT_ERR_UNIT_ID;
    }

    if (basePort >= RTK_MAX_NUM_OF_PORTS)
    {
        return RT_ERR_PORT_ID;
    }

    if (basePort != HWP_PHY_BASE_MACID(unit, basePort))
    {
        return RT_ERR_PORT_ID;
    }

    if (phy82x4qfPatch_init[unit] != INIT_COMPLETED)
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u, not init", unit, basePort);
        return RT_ERR_NOT_INIT;
    }

    if (gPhy82x4qfPI[unit].prxCaliConf[basePort] == NULL)
    {
        RT_8295_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u,port=%u, NULL", unit, basePort);
        return RT_ERR_NULL_POINTER;
    }

    osal_memcpy(conf, gPhy82x4qfPI[unit].prxCaliConf[basePort], sizeof(phy_8295_rxCaliConf_t));
    return RT_ERR_OK;
}
#endif

/*
 * for diag
 */
int32
phy_8295_patch_debugEnable_set(uint32 enable)
{
    if (enable)
        patchDbgEnable |= PHY_8295_PATCH_DBG_OUT;
    else
        patchDbgEnable &= ~PHY_8295_PATCH_DBG_OUT;
    return RT_ERR_OK;
}




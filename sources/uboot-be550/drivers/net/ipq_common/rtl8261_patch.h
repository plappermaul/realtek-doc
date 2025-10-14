#ifndef _RTL8261_PATCH_H_
#define _RTL8261_PATCH_H_

#include "rtl8261_phy.h"

#define PHYPATCH_PHYCTRL_IN_HALCTRL   0  /* 3.6.x: 1 ,4.0.x: 1, 4.1.x+: 0 */
#define PHYPATCH_FMAILY_IN_HWP        0  /* 3.6.x: 1 ,4.0.x: 0, 4.1.x+: 0 */
#define PHY_PATCH_MODE_BCAST_DEFAULT  PHY_PATCH_MODE_BCAST  /* 3.6.x: PHY_PATCH_MODE_BCAST_BUS ,4.0.x+: PHY_PATCH_MODE_BCAST */

#define PHY_PATCH_MODE_NORMAL       0
#define PHY_PATCH_MODE_CMP          1
#define PHY_PATCH_MODE_BCAST        2
#define PHY_PATCH_MODE_BCAST_BUS    3

#define RTK_PATCH_CMP_W        0  /* write */
#define RTK_PATCH_CMP_WC       1  /* compare */
#define RTK_PATCH_CMP_SWC      2  /* sram compare */
#define RTK_PATCH_CMP_WS       3  /* skip */

#define RTK_PATCH_OP_SECTION_SIZE           50
#define RTK_PATCH_OP_TO_CMP(_op, _cmp)       (_op + (RTK_PATCH_OP_SECTION_SIZE * _cmp))

#define RTK_PATCH_OP_PHY                     0
#define RTK_PATCH_OP_PHYOCP                  1
#define RTK_PATCH_OP_TOP                     2
#define RTK_PATCH_OP_TOPOCP                  3
#define RTK_PATCH_OP_PSDS0                   4
#define RTK_PATCH_OP_PSDS1                   5
#define RTK_PATCH_OP_MSDS                    6
#define RTK_PATCH_OP_MAC                     7

/* 50~99 normal op for compare */
#define RTK_PATCH_OP_CMP_PHY                 RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_PHY     , RTK_PATCH_CMP_WC)
#define RTK_PATCH_OP_CMP_PHYOCP              RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_PHYOCP  , RTK_PATCH_CMP_WC)
#define RTK_PATCH_OP_CMP_TOP                 RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_TOP     , RTK_PATCH_CMP_WC)
#define RTK_PATCH_OP_CMP_TOPOCP              RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_TOPOCP  , RTK_PATCH_CMP_WC)
#define RTK_PATCH_OP_CMP_PSDS0               RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_PSDS0   , RTK_PATCH_CMP_WC)
#define RTK_PATCH_OP_CMP_PSDS1               RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_PSDS1   , RTK_PATCH_CMP_WC)
#define RTK_PATCH_OP_CMP_MSDS                RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_MSDS    , RTK_PATCH_CMP_WC)
#define RTK_PATCH_OP_CMP_MAC                 RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_MAC     , RTK_PATCH_CMP_WC)

/* 100~149 normal op for sram compare */
#define RTK_PATCH_OP_CMP_SRAM_PHY            RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_PHY     , RTK_PATCH_CMP_SWC)
#define RTK_PATCH_OP_CMP_SRAM_PHYOCP         RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_PHYOCP  , RTK_PATCH_CMP_SWC)
#define RTK_PATCH_OP_CMP_SRAM_TOP            RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_TOP     , RTK_PATCH_CMP_SWC)
#define RTK_PATCH_OP_CMP_SRAM_TOPOCP         RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_TOPOCP  , RTK_PATCH_CMP_SWC)
#define RTK_PATCH_OP_CMP_SRAM_PSDS0          RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_PSDS0   , RTK_PATCH_CMP_SWC)
#define RTK_PATCH_OP_CMP_SRAM_PSDS1          RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_PSDS1   , RTK_PATCH_CMP_SWC)
#define RTK_PATCH_OP_CMP_SRAM_MSDS           RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_MSDS    , RTK_PATCH_CMP_SWC)
#define RTK_PATCH_OP_CMP_SRAM_MAC            RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_MAC     , RTK_PATCH_CMP_SWC)

#define RTK_PATCH_OP_DELAY_MS                200
#define RTK_PATCH_OP_SKIP                    255

#define RTK_PATCH_TYPE_IS_DATA(_patch_type)    (_patch_type > PHY_PATCH_TYPE_NONE && _patch_type < PHY_PATCH_TYPE_END)
#define RTK_PATCH_TYPE_IS_FLOW(_patch_type)    (_patch_type >= PHY_PATCH_TYPE_END && _patch_type <= (PHY_PATCH_TYPE_END + RTK_PATCH_TYPE_FLOWID_MAX))

#define PHYPATCH_TABLE_ASSIGN(_pPatchDb, _table, _idx, _patch_type, _para) \
    do {\
        if (RTK_PATCH_TYPE_IS_DATA(_patch_type)) {\
            _pPatchDb->_table[_idx].patch_type = _patch_type;\
            _pPatchDb->_table[_idx].patch.data.conf = _para;\
            _pPatchDb->_table[_idx].patch.data.size = sizeof(_para);\
        }\
        else if (RTK_PATCH_TYPE_IS_FLOW(_patch_type)) {\
            _pPatchDb->_table[_idx].patch_type = _patch_type;\
            _pPatchDb->_table[_idx].patch.flow_id = _patch_type;\
        }\
        else {\
            _pPatchDb->_table[_idx].patch_type = PHY_PATCH_TYPE_NONE;\
        }\
    } while(0)
#define PHYPATCH_SEQ_TABLE_ASSIGN(_pPatchDb, _idx, _patch_type, _para) PHYPATCH_TABLE_ASSIGN(_pPatchDb, seq_table, _idx, _patch_type, _para)
#define PHYPATCH_CMP_TABLE_ASSIGN(_pPatchDb, _idx, _patch_type, _para) PHYPATCH_TABLE_ASSIGN(_pPatchDb, cmp_table, _idx, _patch_type, _para)

#define PHYPATCH_COMPARE(_mmdpage, _reg, _msb, _lsb, _exp, _real, _mask) \
    do {\
        uint32 _rData = REG32_FIELD_GET(_real, _lsb, _mask);\
        if (_exp != _rData) {\
            printf("PATCH CHECK: %u(0x%X).%u(0x%X)[%u:%u] = 0x%X (!= 0x%X)\n", _mmdpage, _mmdpage, _reg, _reg, _msb, _lsb, _rData, _exp);\
            return RT_ERR_CHECK_FAILED;\
        }\
    } while (0)

#endif /* _RTL8261_PATCH_H_ */


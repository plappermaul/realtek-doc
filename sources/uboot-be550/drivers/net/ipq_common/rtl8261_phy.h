#ifndef _RTL8261_PHY_H_
#define _RTL8261_PHY_H_

#define BIT_0        0x00000001U
#define BIT_1        0x00000002U
#define BIT_2        0x00000004U
#define BIT_3        0x00000008U
#define BIT_4        0x00000010U
#define BIT_5        0x00000020U
#define BIT_6        0x00000040U
#define BIT_7        0x00000080U
#define BIT_8        0x00000100U
#define BIT_9        0x00000200U
#define BIT_10       0x00000400U
#define BIT_11       0x00000800U
#define BIT_12       0x00001000U
#define BIT_13       0x00002000U
#define BIT_14       0x00004000U
#define BIT_15       0x00008000U
#define BIT_16       0x00010000U
#define BIT_17       0x00020000U
#define BIT_18       0x00040000U
#define BIT_19       0x00080000U
#define BIT_20       0x00100000U
#define BIT_21       0x00200000U
#define BIT_22       0x00400000U
#define BIT_23       0x00800000U
#define BIT_24       0x01000000U
#define BIT_25       0x02000000U
#define BIT_26       0x04000000U
#define BIT_27       0x08000000U
#define BIT_28       0x10000000U
#define BIT_29       0x20000000U
#define BIT_30       0x40000000U
#define BIT_31       0x80000000U

#define SPEED_MASK   0x00000630U
#define SPEED_10M    0x00000000U  
#define SPEED_100M   0x00000010U
#define SPEED_1000M  0x00000020U
#define SPEED_2500M  0x00000210U
#define SPEED_5000M  0x00000220U
#define SPEED_10000M 0x00000200U

#define RTK_PHY_INTR_NEXT_PAGE_RECV       (BIT_0)
#define RTK_PHY_INTR_AN_COMPLETE          (BIT_1)
#define RTK_PHY_INTR_LINK_CHANGE          (BIT_2)
#define RTK_PHY_INTR_ALDPS_STATE_CHANGE   (BIT_3)
#define RTK_PHY_INTR_RLFD                 (BIT_4)
#define RTK_PHY_INTR_TM_LOW               (BIT_5)
#define RTK_PHY_INTR_TM_HIGH              (BIT_6)
#define RTK_PHY_INTR_FATAL_ERROR          (BIT_7)
#define RTK_PHY_INTR_MACSEC               (BIT_8)
#define RTK_PHY_INTR_PTP1588              (BIT_9)
#define RTK_PHY_INTR_WOL                  (BIT_10)

#define RTL8261_MII_ADDR_C45  (1<<30)
#define RTL8261_REG_ADDRESS(dev_ad, reg_num) (RTL8261_MII_ADDR_C45 |\
				((dev_ad & 0x1f) << 16) | (reg_num & 0xFFFF))


#define RTL8261_MMD_PMAPMD 0x1
#define RTL8261_MMD_PMAPMD_PHY_ID1 0x2
#define RTL8261_MMD_PMAPMD_PHY_ID2 0x3

#define RTL8261_MMD_AUTONEG 0x7

#define RTL8261_MMD_AUTONEG 0x7
#define RTL8261_MMD_AN_CTRL 0x0
#define RTL8261_MMD_AN_STAT 0x1

#define PHY_MMD_PCS                 3
#define PHY_MMD_AN                  7
#define PHY_MMD_VEND1               30   /* Vendor specific 1 */
#define PHY_MMD_VEND2               31   /* Vendor specific 2 */

#define REG32_FIELD_SET(_data, _val, _fOffset, _fMask)      ((_data & ~(_fMask)) | ((_val << (_fOffset)) & (_fMask)))
#define REG32_FIELD_GET(_data, _fOffset, _fMask)            ((_data & (_fMask)) >> (_fOffset))
#define UINT32_BITS_MASK(_mBit, _lBit)                      ((0xFFFFFFFF >> (31 - _mBit)) ^ ((1 << _lBit) - 1))
#define RTK_PHYLIB_ERR_CHK(op)\
do {\
    if ((ret = (op)) != 0)\
    {			\
	printf("error ret(%d) %s, %d\n", ret, __FUNCTION__, __LINE__); \
        return ret;\
    } \
} while(0)

typedef enum rtk_phypatch_type_e
{
    PHY_PATCH_TYPE_NONE = 0,
    PHY_PATCH_TYPE_TOP = 1,
    PHY_PATCH_TYPE_SDS,
    PHY_PATCH_TYPE_AFE,
    PHY_PATCH_TYPE_UC,
    PHY_PATCH_TYPE_UC2,
    PHY_PATCH_TYPE_NCTL0,
    PHY_PATCH_TYPE_NCTL1,
    PHY_PATCH_TYPE_NCTL2,
    PHY_PATCH_TYPE_ALGXG,
    PHY_PATCH_TYPE_ALG1G,
    PHY_PATCH_TYPE_NORMAL,
    PHY_PATCH_TYPE_DATARAM,
    PHY_PATCH_TYPE_RTCT,
    PHY_PATCH_TYPE_END
} rtk_phypatch_type_t;

#define RTK_PATCH_TYPE_FLOW(_id)    (PHY_PATCH_TYPE_END + _id)
#define RTK_PATCH_TYPE_FLOWID_MAX   PHY_PATCH_TYPE_END
#define RTK_PATCH_SEQ_MAX     ( PHY_PATCH_TYPE_END + RTK_PATCH_TYPE_FLOWID_MAX -1)

#define uint8 u8
#define uint16 u16
#define uint32 u32
#define int32 int32_t

typedef struct rtk_hwpatch_s
{
    uint8    patch_op;
    uint8    portmask;
    uint16   pagemmd;
    uint16   addr;
    uint8    msb;
    uint8    lsb;
    uint16   data;
    uint8    compare_op;
    uint16   sram_p;
    uint16   sram_rr;
    uint16   sram_rw;
    uint16   sram_a;
} rtk_hwpatch_t;

typedef struct rtk_hwpatch_data_s
{
    rtk_hwpatch_t *conf;
    uint32        size;
} rtk_hwpatch_data_t;

typedef struct rtk_hwpatch_seq_s
{
    uint8 patch_type;
    union
    {
        rtk_hwpatch_data_t data;
        uint8 flow_id;
    } patch;
} rtk_hwpatch_seq_t;

typedef struct rt_phy_patch_db_s
{
    /* patch operation */
    int32_t   (*fPatch_op)(u32 phy_id, u8 portOffset, rtk_hwpatch_t *pPatch_data, u8 patch_mode);
    int32_t   (*fPatch_flow)(u32 phy_id, u8 portOffset, u8 patch_flow, u8 patch_mode);

    /* patch data */
    rtk_hwpatch_seq_t seq_table[RTK_PATCH_SEQ_MAX];
    rtk_hwpatch_seq_t cmp_table[RTK_PATCH_SEQ_MAX];

} rt_phy_patch_db_t;

int32_t phy_common_general_reg_mmd_get(u32 phy_id, u32 mmdAddr, u32 mmdReg, u32 *pData);
int32_t phy_common_general_reg_mmd_set(u32 phy_id, u32 mmdAddr, u32 mmdReg, u32 data);

int32_t rtk_phylib_time_usecs_get(ulong *pUsec);

rt_phy_patch_db_t * get_patch_db(int32_t phy_id);

#ifndef WAIT_COMPLETE_VAR
#define WAIT_COMPLETE_VAR() \
    ulong    _t, _now, _t_wait=0, _timeout;  \
    int32           _chkCnt=0;
    
#define WAIT_COMPLETE(_timeout_us)     \
    _timeout = _timeout_us;  \
    for(rtk_phylib_time_usecs_get(&_t),rtk_phylib_time_usecs_get(&_now),_t_wait=0,_chkCnt=0 ; \
        (_t_wait <= _timeout); \
        rtk_phylib_time_usecs_get(&_now), _chkCnt++, _t_wait += ((_now >= _t) ? (_now - _t) : (0xFFFFFFFF - _t + _now)),_t = _now \
       )
        
#define WAIT_COMPLETE_IS_TIMEOUT()   (_t_wait > _timeout)
#endif

#define HWP_PORT_SMI(phy_id)               0
#define HWP_PHY_MODEL_BY_PORT(phy_id)      0
#define HWP_PHY_ADDR(phy_id)               0
#define HWP_PHY_BASE_MACID(phy_id)            0
#define HWP_PORT_TRAVS_EXCEPT_CPU(phy_id, p)     if (bcast_phyad < 0x1F && p != NULL)

#define RT_LOG(level, module, fmt, args...)               do {} while(0)
#define RT_ERR(error_code, module, fmt, args...)          do {} while(0)
#define RT_INIT_ERR(error_code, module, fmt, args...)     do {} while(0)
#define RT_INIT_MSG(fmt, args...)                         do {} while(0)

#define phy_826xb_ctrl_set(phy_id, RTK_PHY_CTRL_MIIM_BCAST_PHYAD, bcast_phyad)  0

#define PHYPATCH_DB_GET(phy_id, _pPatchDb) \
    do { \
        _pPatchDb = get_patch_db(phy_id); \
        /*printk("[PHYPATCH_DB_GET] ? [%s]\n", (_pDb != NULL) ? "E":"N");*/ \
    } while(0)

#endif				/* _RTL8261_PHY_H_ */


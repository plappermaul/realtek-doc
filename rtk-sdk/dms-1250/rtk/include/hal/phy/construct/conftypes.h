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
 * Purpose :
 *
 * Feature :
 *
 */

#ifndef __CONFTYPES_H__
#define __CONFTYPES_H__


/*
 * Gerneral format
 */

/* per-chip: {phy, page, reg, mask, val} */
typedef struct {
    unsigned char  phy:3;
    unsigned char  page:5;
    unsigned char  reg;
    unsigned short mask;
    unsigned short val;
} __attribute__ ((aligned(1), packed)) confcode_pprmv_t;

/* per-port: {page, reg, mask, val} */
typedef struct {
    unsigned char  page;
    unsigned char  reg;
    unsigned short mask;
    unsigned short val;
} confcode_prmv_t;


/*
 * Reduced format (for code size issue)
 */

/* per-chip: {phy, reg, val} */
typedef struct {
    unsigned char  phy:3;
    unsigned char  reg:5;
    unsigned short val;
} __attribute__ ((aligned(1), packed)) confcode_prv_t;

/* per-port: {reg, val} */
typedef struct {
    unsigned char  reg;
    unsigned short val;
} confcode_rv_t;

typedef struct {
    unsigned char   reg;
    unsigned char   endBit;
    unsigned char   startBit;
    unsigned short  val;
} confcode_phy_patch_t;

typedef struct phy_confcode_prd_s {
    uint16      page;
    uint16      reg;
    uint16      data;
} phy_confcode_prd_t;

typedef struct phy_confcode_pprfv_s {
    uint8       phy_port;    /* PHY chip's port ID. */
    uint16      page;
    uint16      reg;
    uint8       endBit;     /* config field end bit */
    uint8       startBit;   /* config field start bit */
    uint16      value;      /* config field value */
} phy_confcode_pprfv_t;
#define PHY_CONFCODE_PHYPORT_END    0xFF


typedef struct phy_confcode_mad_s {
    uint16      mmd;        /* mmd address */
    uint16      addr;       /* register addr */
    uint16      data;       /* config data */
} phy_confcode_mad_t;


typedef struct phy_confcode_pmad_s {
    uint8       phy_port;   /* PHY chip's port ID. */
    uint16      mmd;        /* mmd address */
    uint16      addr;       /* register addr */
    uint16      data;       /* config data */
} phy_confcode_pmad_t;

#define RTK_HWPATCH_OP_UNKNOWN 0
#define RTK_HWPATCH_OP_SDS     1
#define RTK_HWPATCH_OP_PHY     2
#define RTK_HWPATCH_OP_PHYW    3
#define RTK_HWPATCH_OP_ALGO    4
#define RTK_HWPATCH_OP_TOP     5
#define RTK_HWPATCH_OP_MMD     6
#define RTK_HWPATCH_OP_DATARAM 7

typedef struct rtk_phy_hwpatch_s
{   
    uint8    patch_op;
    uint8    portmask;
    uint16   pagemmd;
    uint16   addr;
    uint8    msb;
    uint8    lsb;
    uint16   data;
} rtk_phy_hwpatch_t;

typedef struct phy_eyeParamConf_s {
    uint32    sdsId;
    rtk_sds_eyeParam_t  eyeParam;
} phy_eyeParamConf_t;
#define PHY_CONFCODE_SDSID_END      0xFF

typedef struct phy_confcode_ent_s {
    uint8                   base_mac_id_of_this_phy;
    phy_confcode_pprfv_t    *phy_param;
    uint32                  sds_eye_param_valid; /* TRUE or FALSE */
    phy_eyeParamConf_t      sds_eye_param[RTK_MAX_SDS_PER_PHY];     /* set last entry with .sdsId = PHY_CONFCODE_SDSID_END */
} phy_confcode_ent_t;
#define PHY_CONFCODE_MACID_END      0xFF

typedef struct phy_conf_per_unit_s {
    phy_confcode_ent_t  phy_ent[RTK_MAX_PHY_PER_UNIT+1];
} phy_conf_per_unit_t;

typedef struct phy_conf_per_hwp_s {
    int                     hwp_id; /* mapping to hwp_hwProfile_t->identifier.id */
    phy_conf_per_unit_t     *phy_conf_per_unit[RTK_MAX_NUM_OF_UNIT_LOCAL+1];
} phy_conf_per_hwp_t;

#define PHY_PATCH_BIT_SET(_unit, _port, _page, _reg, _endBit, _startBit, _val)  \
    do {                                                                        \
        unsigned char   _len;                                                   \
        unsigned int    _data, _mask;                                           \
                                                                                \
        _len = _endBit - _startBit + 1;                                         \
                                                                                \
        if (32 == _len)                                                         \
            _data = _val;                                                       \
        else                                                                    \
        {                                                                       \
            _mask = (1 << _len) - 1;                                            \
            RTK_MII_READ(unit, _port, _page, _reg, &_data);                     \
            _data &= ~(_mask << _startBit);                                     \
            _data |= (_val << _startBit);                                       \
        }                                                                       \
        RTK_MII_WRITE(unit, _port, _page, _reg, (int)_data);                    \
    } while(0);

#define PHY_PATCH_SET(unit, _p, _page, _in) PHY_PATCH_BIT_SET(unit, _p, _page, _in.reg, _in.endBit, _in.startBit, _in.val)

#endif /* __CONFTYPES_H__ */


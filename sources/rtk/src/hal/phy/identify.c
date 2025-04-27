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
 * Purpose : PHY identify service APIs in the SDK.
 *
 * Feature : PHY identify service APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_autoconf.h>
#include <hal/common/halctrl.h>
#include <hal/common/miim.h>
#include <soc/type.h>
#include <hal/mac/reg.h>
#include <hal/phy/identify.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>
#include <hal/mac/miim_common_drv.h>
#include <osal/memory.h>
#if (defined(CONFIG_SDK_RTL8208D) || defined(CONFIG_SDK_RTL8208G) || defined(CONFIG_SDK_RTL8208L))
#include <hal/phy/phy_rtl8208.h>
#endif
#if (defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8214FB) || defined(CONFIG_SDK_RTL8212B))
#include <hal/phy/phy_rtl8214fb.h>
#endif
#if (defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8218FB) || defined(CONFIG_SDK_RTL8214FC))
#include <hal/phy/phy_rtl8218b.h>
#endif
#if defined(CONFIG_SDK_RTL8218D)
#include <hal/phy/phy_rtl8218d.h>
#endif
#if defined(CONFIG_SDK_RTL8218E)
#include <hal/phy/phy_rtl8218e.h>
#endif
#if (defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF))
#include <hal/phy/rtl8295_reg_def.h>
#include <hal/phy/phy_rtl8295.h>
#endif

#if defined(CONFIG_SDK_RTL8380)
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/phy/phy_rtl8380.h>
#endif
#if defined(CONFIG_SDK_RTL8390)
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/phy/phy_rtl8390.h>
#endif
#if defined(CONFIG_SDK_RTL8214C)
#include <hal/phy/phy_rtl8214c.h>
#endif

#if defined(CONFIG_SDK_RTL9300)
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <hal/phy/phy_rtl9300.h>
#endif

#if defined(CONFIG_SDK_RTL9310)
#include <hal/phy/phy_rtl9310.h>
#endif
#if defined(CONFIG_SDK_RTL8226)
#include <hal/phy/phy_rtl8226.h>
#endif
#if defined(CONFIG_SDK_RTL826X) || defined(CONFIG_SDK_RTL826XI) || defined(CONFIG_SDK_RTL825X) || defined(CONFIG_SDK_RTL825XI) || defined(CONFIG_SDK_RTL825XL) || defined(CONFIG_SDK_RTL8224)
#include <hal/phy/phy_rtl826x.h>
#endif
#if defined(CONFIG_SDK_RTL826XB)
#include <hal/phy/phy_rtl826xb.h>
#endif
#if defined(CONFIG_SDK_RTL8224)
#include <hal/phy/phy_rtl8224.h>
#endif
#if defined(CONFIG_SDK_PHY_CUST1)
#include <hal/phy/phy_cust1.h>
#endif
#if defined(CONFIG_SDK_PHY_CUST2)
#include <hal/phy/phy_cust2.h>
#endif
#if defined(CONFIG_SDK_PHY_CUST3)
#include <hal/phy/phy_cust3.h>
#endif
#if defined(CONFIG_SDK_PHY_CUST4)
#include <hal/phy/phy_cust4.h>
#endif
#if defined(CONFIG_SDK_PHY_CUST5)
#include <hal/phy/phy_cust5.h>
#endif
#if defined(CONFIG_SDK_EXPERIMENTAL_RTL8211FS)
#include <hal/phy/phy_experimental_rtl8211fs.h>
#endif

/*
 * Symbol Definition
 */
/* linking list node of PHY control */
typedef struct rt_phyctrl_linkNode_s
{
    struct rt_phyctrl_linkNode_s    *next;
    rt_phyctrl_t                    *pPhyctrl;
}rt_phyctrl_linkNode_t;

extern uint32 rtl8380_sds_modify_flag;

/*
 * Data Declaration
 */
int32 _phy_identify_default(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
int32 _phy_identify_default_c45(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#if (defined(CONFIG_SDK_RTL8214FB) || defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8212B))
static int32 _phy_identify_8214FB(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
static int32 _phy_identify_8214B(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
static int32 _phy_identify_8212B(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif
#if defined(CONFIG_SDK_RTL8218B)
static int32 _phy_identify_8218B(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif
#if defined(CONFIG_SDK_RTL8218FB)
static int32 _phy_identify_8218FB(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
static int32 _phy_identify_8218FB_MP(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif  /* CONFIG_SDK_RTL8218FB */
#ifdef CONFIG_SDK_RTL8214FC
static int32 _phy_identify_8214FC(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
static int32 _phy_identify_8214FC_MP(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif  /* CONFIG_SDK_RTL8214FC */
#if defined(CONFIG_SDK_RTL8295R)
static int32 _phy_identify_8295R(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif /* CONFIG_SDK_RTL8295R */
#if defined(CONFIG_SDK_RTL8214QF)
static int32 _phy_identify_8214QF(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif /* CONFIG_SDK_RTL8214QF */
#if defined(CONFIG_SDK_RTL8224QF)
static int32 _phy_identify_8224QF(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif /* CONFIG_SDK_RTL8224QF */
#if defined(CONFIG_SDK_RTL8380) && defined(CONFIG_SDK_RTL8218B)
static int32
_phy_identify_8330_8208Lint(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif /* CONFIG_SDK_RTL8380 && CONFIG_SDK_RTL8218B */
#if defined(CONFIG_SDK_RTL8380)
static int32 _phy_identify_8380_serdes_ge(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif
#if defined(CONFIG_SDK_RTL8390)
static int32 _phy_identify_8390_serdes_ge(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
static int32 _phy_identify_8390_serdes_10ge(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif
#if defined(CONFIG_SDK_RTL9300)
static int32 _phy_identify_9300_serdes(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif
#if defined(CONFIG_SDK_RTL8218D)
static int32 _phy_identify_8218d(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
static int32 _phy_identify_8218dnmp(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif
#if defined(CONFIG_SDK_RTL9310)
static int32 _phy_identify_9310_serdes(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif
#if defined(CONFIG_SDK_RTL8226)
static int32 _phy_identify_8226(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif
#if defined(CONFIG_SDK_RTL8218E)
static int32 _phy_identify_8218e(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif

#if defined(CONFIG_SDK_RTL826XB)
static int32 _phy_identify_8261B(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
static int32 _phy_identify_8264B(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif

#if (defined(CONFIG_SDK_RTL826X) || defined(CONFIG_SDK_RTL826XI))
static int32 _phy_identify_8261(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
static int32 _phy_identify_8264(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
static int32 _phy_identify_8261I(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
static int32 _phy_identify_8264I(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif

#if defined(CONFIG_SDK_RTL825X)
static int32 _phy_identify_8251(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
static int32 _phy_identify_8254(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif

#if defined(CONFIG_SDK_RTL825XI)
static int32 _phy_identify_8251I(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
static int32 _phy_identify_8254I(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif

#if defined(CONFIG_SDK_RTL825XL)
static int32 _phy_identify_8251L(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
static int32 _phy_identify_8254L(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif

#if defined(CONFIG_SDK_RTL8224)
static int32 _phy_identify_8224(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);
#endif

/* supported external PHY chip lists */
static rt_phyctrl_t supported_phys[] =
{
#if defined(CONFIG_SDK_RTL8208G)
    {_phy_identify_default, PHY_MODEL_ID_RTL8208G, PHY_REV_NO_A, RTK_PHYTYPE_RTL8208G, NULL, phy_8208Gdrv_fe_mapperInit, 0, &phy_8208_info},
    {_phy_identify_default, PHY_MODEL_ID_RTL8208G, PHY_REV_NO_B, RTK_PHYTYPE_RTL8208G, NULL, phy_8208Gdrv_fe_mapperInit, 0, &phy_8208_info},
#endif
#if (defined(CONFIG_SDK_RTL8214FB) || defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8212B))
    {_phy_identify_8214FB, PHY_MODEL_ID_RTL8214FB, PHY_REV_NO_B, RTK_PHYTYPE_RTL8214FB, NULL, phy_8214FBdrv_ge_mapperInit, 0, &phy_8214b_info},
    {_phy_identify_8214B, PHY_MODEL_ID_RTL8214B,  PHY_REV_NO_B, RTK_PHYTYPE_RTL8214B, NULL, phy_8214Bdrv_ge_mapperInit, 0, &phy_8214b_info},
    {_phy_identify_8212B, PHY_MODEL_ID_RTL8212B,  PHY_REV_NO_B, RTK_PHYTYPE_RTL8212B, NULL, phy_8212Bdrv_ge_mapperInit, 0, &phy_8212b_info},
#endif
#if defined(CONFIG_SDK_RTL8208D)
    {_phy_identify_default, PHY_MODEL_ID_RTL8208D_EXT, PHY_REV_NO_C, RTK_PHYTYPE_RTL8208D, NULL, phy_8208Ddrv_fe_mapperInit, 0, &phy_8208_info},
#endif
#if defined(CONFIG_SDK_RTL8208L)
    {_phy_identify_default, PHY_MODEL_ID_RTL8208L, PHY_REV_NO_C, RTK_PHYTYPE_RTL8208L, NULL, phy_8208Ddrv_fe_mapperInit, 0, &phy_8208_info},
#endif
#if defined(CONFIG_SDK_RTL8218B)
    {_phy_identify_8218B, PHY_MODEL_ID_RTL8218B_EXT,  PHY_REV_NO_B, RTK_PHYTYPE_RTL8218B, NULL, phy_8218Bdrv_ge_mapperInit, 0, &phy_8218B_info},
#endif
#if defined(CONFIG_SDK_RTL8218FB)
    {_phy_identify_8218FB, PHY_MODEL_ID_RTL8218FB,  PHY_REV_NO_B, RTK_PHYTYPE_RTL8218FB, NULL, phy_8218FBdrv_ge_mapperInit, 0, &phy_8218FB_info},
    {_phy_identify_8218FB_MP, PHY_MODEL_ID_RTL8218FB,  PHY_REV_NO_A, RTK_PHYTYPE_RTL8218FB, NULL, phy_8218FBdrv_MP_ge_mapperInit, 0, &phy_8218FB_info},
#endif  /* CONFIG_SDK_RTL8218FB */
#if defined(CONFIG_SDK_RTL8214FC)
    {_phy_identify_8214FC, PHY_MODEL_ID_RTL8214FC,  PHY_REV_NO_B, RTK_PHYTYPE_RTL8214FC, NULL, phy_8214FCdrv_ge_mapperInit, 0, &phy_8214FC_info},
    {_phy_identify_8214FC_MP, PHY_MODEL_ID_RTL8214FC,  PHY_REV_NO_A, RTK_PHYTYPE_RTL8214FC, NULL, phy_8214FCdrv_MP_ge_mapperInit, 0, &phy_8214FC_info},
#endif  /* CONFIG_SDK_RTL8214FC */
#if defined(CONFIG_SDK_RTL8214C)
    {_phy_identify_default, PHY_MODEL_ID_RTL8214C, PHY_REV_NO_C, RTK_PHYTYPE_RTL8214C, NULL, phy_8214Cdrv_ge_mapperInit,    0, &phy_8214C_info},
#endif
#if defined(CONFIG_SDK_RTL8218D)
    {_phy_identify_8218d, PHY_MODEL_ID_RTL8218D,  PHY_REV_NO_D, RTK_PHYTYPE_RTL8218D, NULL, phy_8218Ddrv_ge_mapperInit,   1, &phy_8218D_info},
    {_phy_identify_8218dnmp, PHY_MODEL_ID_RTL8218D,  PHY_REV_NO_D, RTK_PHYTYPE_RTL8218D_NMP, NULL, phy_8218Ddrv_ge_mapperInit,   0, &phy_8218D_info},
#endif
#if defined(CONFIG_SDK_RTL8295R)
    {_phy_identify_8295R,     PHY_MODEL_ID_RTL8295R,  PHY_REV_NO_A, RTK_PHYTYPE_RTL8295R,   NULL, phy_8295Rdrv_mapperInit,  1, &phy_8295R_info},
#endif
#if defined(CONFIG_SDK_RTL8214QF)
    {_phy_identify_8214QF,    PHY_MODEL_ID_RTL8214QF, PHY_REV_NO_A, RTK_PHYTYPE_RTL8214QF,      NULL, phy_8214QFdrv_mapperInit, 1, &phy_8214QF_info},
#endif
#if defined(CONFIG_SDK_RTL8224QF)
    {_phy_identify_8224QF,    PHY_MODEL_ID_RTL8224QF, PHY_REV_NO_A, RTK_PHYTYPE_RTL8224QF,      NULL, phy_8224QFdrv_mapperInit, 1, &phy_8224QF_info},
#endif
#if defined(CONFIG_SDK_RTL8226)
    {_phy_identify_8226,      PHY_MODEL_ID_RTL8226,   8, RTK_PHYTYPE_RTL8226,        NULL, phy_8226drv_mapperInit,   1, &phy_8226_info},
    {_phy_identify_8226,      PHY_MODEL_ID_RTL8226B,  0, RTK_PHYTYPE_RTL8226B,       NULL, phy_8226bdrv_mapperInit,  1, &phy_8226b_info},
#endif
#if defined(CONFIG_SDK_RTL8218E)
    {_phy_identify_8218e, PHY_MODEL_ID_RTL8218E,  PHY_REV_NO_E, RTK_PHYTYPE_RTL8218E, NULL, phy_8218Edrv_ge_mapperInit,   1, &phy_8218E_info},
#endif
#if defined(CONFIG_SDK_RTL826XB)
    {_phy_identify_8261B, PHY_MODEL_ID_RTL826X, PHY_REV_NO_D, RTK_PHYTYPE_RTL8261B, NULL, phy_826XBdrv_mapperInit, 0, &phy_8261B_info},
    {_phy_identify_8264B, PHY_MODEL_ID_RTL8264B, PHY_REV_NO_D, RTK_PHYTYPE_RTL8264B, NULL, phy_826XBdrv_mapperInit, 0, &phy_8264B_info},
#endif
#if (defined(CONFIG_SDK_RTL826X) || defined(CONFIG_SDK_RTL826XI))
    {_phy_identify_8261, PHY_MODEL_ID_RTL826X, PHY_REV_NO_A, RTK_PHYTYPE_RTL8261, NULL, phy_826Xdrv_mapperInit, 0, &phy_8261_info},
    {_phy_identify_8264, PHY_MODEL_ID_RTL826X, PHY_REV_NO_A, RTK_PHYTYPE_RTL8264, NULL, phy_826Xdrv_mapperInit, 0, &phy_8264_info},
    {_phy_identify_8261I, PHY_MODEL_ID_RTL826X, PHY_REV_NO_A, RTK_PHYTYPE_RTL8261I, NULL, phy_826Xdrv_mapperInit, 0, &phy_8261_info},
    {_phy_identify_8264I, PHY_MODEL_ID_RTL826X, PHY_REV_NO_A, RTK_PHYTYPE_RTL8264I, NULL, phy_826Xdrv_mapperInit, 0, &phy_8264_info},
#endif
#if defined(CONFIG_SDK_RTL825X)
    {_phy_identify_8251, PHY_MODEL_ID_RTL826X, PHY_REV_NO_A, RTK_PHYTYPE_RTL8251, NULL, phy_826Xdrv_mapperInit, 0, &phy_8251_info},
    {_phy_identify_8254, PHY_MODEL_ID_RTL826X, PHY_REV_NO_A, RTK_PHYTYPE_RTL8254, NULL, phy_826Xdrv_mapperInit, 0, &phy_8254_info},
#endif
#if defined(CONFIG_SDK_RTL825XI)
    {_phy_identify_8251I, PHY_MODEL_ID_RTL826X, PHY_REV_NO_A, RTK_PHYTYPE_RTL8251I, NULL, phy_826Xdrv_mapperInit, 0, &phy_8251_info},
    {_phy_identify_8254I, PHY_MODEL_ID_RTL826X, PHY_REV_NO_A, RTK_PHYTYPE_RTL8254I, NULL, phy_826Xdrv_mapperInit, 0, &phy_8254_info},
#endif
#if defined(CONFIG_SDK_RTL825XL)
    {_phy_identify_8251L, PHY_MODEL_ID_RTL826X, PHY_REV_NO_A, RTK_PHYTYPE_RTL8251L, NULL, phy_825XLdrv_mapperInit, 0, &phy_8251_info},
    {_phy_identify_8254L, PHY_MODEL_ID_RTL826X, PHY_REV_NO_A, RTK_PHYTYPE_RTL8254L, NULL, phy_825XLdrv_mapperInit, 0, &phy_8254_info},
#endif
#if defined(CONFIG_SDK_RTL8224)
    {_phy_identify_8224, PHY_MODEL_ID_RTL8224, PHY_REV_NO_A, RTK_PHYTYPE_RTL8224, NULL, phy_8224drv_mapperInit, 0, &phy_8224_info},
#endif
#if defined(CONFIG_SDK_PHY_CUST5)   /* for customer defined PHY5 */
    {phy_identify_cust5,     PHY_MODEL_ID_CUST5,     PHY_REV_NO_A, RTK_PHYTYPE_CUST5,      NULL, phy_cust5drv_mapperInit, 0, &phy_cust5_info},
#endif
#if defined(CONFIG_SDK_PHY_CUST4)   /* for customer defined PHY4 */
    {phy_identify_cust4,     PHY_MODEL_ID_CUST4,     PHY_REV_NO_A, RTK_PHYTYPE_CUST4,      NULL, phy_cust4drv_mapperInit, 0, &phy_cust4_info},
#endif
#if defined(CONFIG_SDK_PHY_CUST3)   /* for customer defined PHY3 */
    {phy_identify_cust3,     PHY_MODEL_ID_CUST3,     PHY_REV_NO_A, RTK_PHYTYPE_CUST3,      NULL, phy_cust3drv_mapperInit, 0, &phy_cust3_info},
#endif
#if defined(CONFIG_SDK_PHY_CUST2)   /* for customer defined PHY2 */
    {phy_identify_cust2,     PHY_MODEL_ID_CUST2,     PHY_REV_NO_A, RTK_PHYTYPE_CUST2,      NULL, phy_cust2drv_mapperInit, 0, &phy_cust2_info},
#endif
#if defined(CONFIG_SDK_PHY_CUST1)   /* for customer defined PHY1 */
    {phy_identify_cust1,     PHY_MODEL_ID_CUST1,     PHY_REV_NO_A, RTK_PHYTYPE_CUST1,      NULL, phy_cust1drv_mapperInit, 0, &phy_cust1_info},
#endif
#if defined(CONFIG_SDK_EXPERIMENTAL_RTL8211FS)
    {_phy_identify_default, PHY_MODEL_ID_RTL8211FS,  PHY_REV_NO_G, RTK_PHYTYPE_EXP_RTL8211FS, NULL, phy_experimental_8211FS_mapperInit,   0, &phy_8211FS_info},
#endif
}; /* end of supported_phys */


/* supported internal PHY chip lists */
static rt_phyctrl_t supported_int_phys[] =
{
#if defined(CONFIG_SDK_RTL8380)
#if defined(CONFIG_SDK_RTL8218B)
    {_phy_identify_default, PHY_MODEL_ID_RTL8218B_INT,  PHY_REV_NO_A, RTK_PHYTYPE_RTL8218B, NULL, phy_8380drv_int_ge_mapperInit, 0, &phy_8218B_info},
    {_phy_identify_8330_8208Lint, PHY_MODEL_ID_RTL8208L_INT, PHY_REV_NO_A, RTK_PHYTYPE_RTL8208L_INT, NULL, phy_8380drv_int_ge_mapperInit, 0, &phy_8218B_info},  /* SST-104 */
#endif
    {_phy_identify_8380_serdes_ge, RTL8380_FAMILY_ID, PHY_MODEL_ID_NULL, RTK_PHYTYPE_SERDES, NULL, phy_8380_serdes_ge_mapperInit, 0, &phy_8380sds_info},
#endif
#if defined(CONFIG_SDK_RTL8390)
    {_phy_identify_8390_serdes_10ge, RTL8390_FAMILY_ID, PHY_MODEL_ID_NULL, RTK_PHYTYPE_SERDES, NULL, phy_8390_serdes_ge_mapperInit, 0, &phy_8390sds10G_info},
    {_phy_identify_8390_serdes_ge, RTL8390_FAMILY_ID, PHY_MODEL_ID_NULL, RTK_PHYTYPE_SERDES, NULL, phy_8390_serdes_ge_mapperInit, 0, &phy_8390sds_info},
#endif
#if defined(CONFIG_SDK_RTL9300)
    {_phy_identify_9300_serdes, RTL9300_FAMILY_ID, PHY_MODEL_ID_NULL, RTK_PHYTYPE_SERDES, NULL, phy_9300_serdes_mapperInit, 0, &phy_9300sds_info},
#endif /* CONFIG_SDK_RTL9300 */
#if defined(CONFIG_SDK_RTL9310)
    {_phy_identify_9310_serdes, RTL9310_FAMILY_ID, PHY_MODEL_ID_NULL, RTK_PHYTYPE_SERDES, NULL, phy_rtl9310_serdes_mapperInit, 0, &phy_rtl9310sds_info},
#endif /* CONFIG_SDK_RTL9310 */
};

/* PHY control linking list indexes by RTK_PHYTYPE_xxx */
uint32                          phyctrlLnkList_init = FALSE;
rt_phyctrl_linkNode_t           *phyctrlLnkList[RTK_PHYTYPE_END] = {NULL};

/*
 * Function Declaration
 */

/* Static Function Body */

/* Function Name:
 *      _phy_identify_default
 * Description:
 *      Identify the port is match input PHY information or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is match the PHY information
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not match the PHY information
 * Note:
 *      None
 */
int32
_phy_identify_default(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    uint32 real_model_id, real_rev_id;

    if (RT_ERR_OK != phy_identify_phyid_get(unit, port, &real_model_id, &real_rev_id))
    {
        return RT_ERR_FAILED;
    }

    if (RT_ERR_OK != phy_identify_OUI_check(unit, port))
    {
        return RT_ERR_FAILED;
    }

    if ((real_model_id == model_id) && (real_rev_id >= rev_id))
        return RT_ERR_OK;

    return RT_ERR_PHY_NOT_MATCH;
} /* end of _phy_identify_default */


/* Function Name:
 *      _phy_rtk_oui_chk
 * Description:
 *      Identify the OUI is the realtek OUI or not?
 * Input:
 *      reg2_data - device identifier reg 2 data
 *      reg3_data - device identifier reg 3 data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - Realtek OUI
 *      RT_ERR_FAILED - not Realtek OUI
 * Note:
 *      None
 */
int32
_phy_rtk_oui_chk(uint32 reg2_data, uint32 reg3_data)
{
    if ((reg2_data != PHY_IDENT_OUI_03_18) ||
        ((reg3_data >> OUI_19_24_OFFSET) != PHY_IDENT_OUI_19_24))
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      _phy_identify_default_c45
 * Description:
 *      Identify the port is match input PHY information or not for clause 45 PHY
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is match the PHY information
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not match the PHY information
 * Note:
 *      None
 */
int32
_phy_identify_default_c45(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    int32       ret;
    uint32      reg2_data = 0, reg3_data = 0;
    uint32      chip_model_id, chip_rev_id;
    hal_control_t   *pHalCtrl = NULL;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);
    RT_PARAM_CHK((NULL == MACDRV(pHalCtrl)->fMdrv_miim_read), RT_ERR_FAILED);

    MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 1, 2, &reg2_data);
    ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 1, 3, &reg3_data);

    if ((ret = _phy_rtk_oui_chk(reg2_data, reg3_data)) != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "Compare OUI reg2_data failed(unit %d, port %d): reg2_data= 0x%x; PHY_IDENT_OUI_03_18 = 0x%x!!", unit, port, reg2_data, PHY_IDENT_OUI_03_18);
        RT_LOG(LOG_TRACE, MOD_HAL, "Compare OUI reg3_data failed(unit %d, port %d): reg3_data.b[24:19]= 0x%x; PHY_IDENT_OUI_19_24 = 0x%x!!", unit, port, (reg3_data >> OUI_19_24_OFFSET), PHY_IDENT_OUI_19_24);
        return ret;
    }

    chip_model_id = PHY_IDENT_MODEL_NUM(reg3_data);
    chip_rev_id   = PHY_IDENT_REV_NUM(reg3_data);

    if ((chip_model_id == model_id) && (chip_rev_id >= rev_id))
    {
        return RT_ERR_OK;
    }

    return RT_ERR_PHY_NOT_MATCH;
}

#if defined(CONFIG_SDK_RTL8380)
/* Function Name:
 *      _phy_identify_8380_serdes_ge
 * Description:
 *      Identify the port is 8380 intra serdes PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is 8380 intra serdes PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not 8380 intra serdes PHY
 * Note:
 *      None
 */
static int32
_phy_identify_8380_serdes_ge(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    uint32  serdes_mode = 0;
    int32   ret = RT_ERR_FAILED;
    uint32 val = 0;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);
    RT_LOG(LOG_EVENT, MOD_HAL, "_phy_identify_8380_serdes_ge(unit %d, port %d) function!!", unit, port);

    if (!HWP_8380_30_FAMILY(unit))
    {
        return RT_ERR_PHY_NOT_MATCH;
    }
    if(1 == rtl8380_sds_modify_flag)
    {
        val = 0x9;
        reg_write(unit, MAPLE_INT_MODE_CTRLr, &val);
    }
    {
        /*For RTL8380 only port24 & port26 can be configured as fiber port*/
        switch (port)
        {
            case 24:
                RT_LOG(LOG_EVENT, MOD_HAL, "_phy_identify_8380_serdes_ge(unit %d, port %d) switch case 0 function!!", unit, port);
                if ((ret = reg_field_read(unit, MAPLE_INT_MODE_CTRLr, MAPLE_SDS4_INTF_MODE_2_0f, &serdes_mode)) != RT_ERR_OK)
                    return ret;
                if( (1 == serdes_mode) || (2 == serdes_mode))  /*0x1: fiber;  0x2: SGMII*/
                    return RT_ERR_OK;
                else
                    return RT_ERR_FAILED;
                break;
            case 26:
                RT_LOG(LOG_EVENT, MOD_HAL, "_phy_identify_8380_serdes_ge(unit %d, port %d) switch case 1 function!!", unit, port);
                if ((ret = reg_field_read(unit, MAPLE_INT_MODE_CTRLr, MAPLE_SDS5_INTF_MODE_2_0f, &serdes_mode)) != RT_ERR_OK)
                    return ret;
                if( (1 == serdes_mode) || (2 == serdes_mode))  /*0x1: fiber;  0x2: SGMII*/
                    return RT_ERR_OK;
                else
                    return RT_ERR_FAILED;
                break;
            case 1:
            case 3:
            default:
                RT_LOG(LOG_EVENT, MOD_HAL, "_phy_identify_8380_serdes_ge(unit %d, port %d) switch case 2/3 function!!", unit, port);
                return RT_ERR_FAILED;
        }
    }

    return RT_ERR_PHY_NOT_MATCH;
} /* end  _phy_identify_8380_serdes_ge */
#endif


#if defined(CONFIG_SDK_RTL8380) && defined(CONFIG_SDK_RTL8218B)
static int32
_phy_identify_8330_8208Lint(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    uint32 real_model_id, real_rev_id;

    if (HWP_CHIP_FAMILY_ID(unit) == RTL8330_FAMILY_ID)
    {
        if (RT_ERR_OK != phy_identify_phyid_get(unit, port, &real_model_id, &real_rev_id))
        {
            return RT_ERR_FAILED;
        }

        if (RT_ERR_OK != phy_identify_OUI_check(unit, port))
        {
            return RT_ERR_FAILED;
        }

        if ((real_model_id == model_id) && (real_rev_id >= rev_id))
            return RT_ERR_OK;
    }

    return RT_ERR_PHY_NOT_MATCH;
}
#endif /* end CONFIG_SDK_RTL8380 && CONFIG_SDK_RTL8218B */

#if defined(CONFIG_SDK_RTL8390)
/* Function Name:
 *      _phy_identify_8390_serdes_ge
 * Description:
 *      Identify the port is 8390 intra serdes PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is 8390 intra serdes PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not 8390 intra serdes PHY
 * Note:
 *      None
 */
static int32
_phy_identify_8390_serdes_ge(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    uint32          sdsId;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);
    RT_LOG(LOG_EVENT, MOD_HAL, "_phy_identify_8390_serdes_ge(unit %d, port %d) function!!", unit, port);

    if (!HWP_8390_50_FAMILY(unit))
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if (port != 48 && port != 49)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if (!HWP_SERDES_PORT(unit, port))
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    sdsId = HWP_PORT_SDSID(unit, port);
    if (HWP_NONE == sdsId)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if (HWP_PORT_MEDI(unit, port) == HWP_SERDES)
    {
        return RT_ERR_OK;
    }

    return RT_ERR_PHY_NOT_MATCH;
} /* end  _phy_identify_8390_serdes_ge */

/* Function Name:
 *      _phy_identify_8390_serdes_10ge
 * Description:
 *      Identify the port is 8390 intra serdes PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is 8390 intra serdes PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not 8390 intra serdes PHY
 * Note:
 *      None
 */
static int32
_phy_identify_8390_serdes_10ge(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    RT_LOG(LOG_EVENT, MOD_HAL, "_phy_identify_8390_serdes_10ge(unit %d, port %d) function!!", unit, port);

    if (!HWP_8390_50_FAMILY(unit))
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    /*For RTL8390 only port24 & port36 can be configured as 10G fiber port*/
    if (HWP_CHIP_ID(unit) != RTL8396M_CHIP_ID)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if (port == 24)
    {
        return RT_ERR_OK;
    }
    else if (port == 36)
    {
        return RT_ERR_OK;
    }

    return RT_ERR_PHY_NOT_MATCH;
} /* end  _phy_identify_8390_serdes_ge */

#endif

#if (defined(CONFIG_SDK_RTL8214FB) || defined(CONFIG_SDK_RTL8214B) || defined(CONFIG_SDK_RTL8212B))
/* Function Name:
 *      _phy_identify_8214B
 * Description:
 *      Identify the port is 8214B PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is 8214B PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not 8214B PHY
 * Note:
 *      None
 */
static int32
_phy_identify_8214B(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    uint32 real_model_id, real_rev_id;
    uint32 base_port = 0, data;
    hal_control_t   *pHalCtrl = NULL;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);
    if (RT_ERR_OK != phy_identify_phyid_get(unit, port, &real_model_id, &real_rev_id))
    {
        return RT_ERR_FAILED;
    }

    if (RT_ERR_OK != phy_identify_OUI_check(unit, port))
    {
        return RT_ERR_FAILED;
    }

    if ((real_model_id == model_id) && (real_rev_id >= rev_id))
    {
        base_port = port - (port % PORT_NUM_IN_8214FB);
        if (MACDRV(pHalCtrl)->fMdrv_miim_read(unit, base_port, 10, 18, &data) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (0x0 == ((data) & 0xF))
        {
            if (MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port+3, 8, 18, 0x93f0) != RT_ERR_OK)
                return RT_ERR_FAILED;
            if (MACDRV(pHalCtrl)->fMdrv_miim_read(unit, base_port+3, 8, 19, &data) != RT_ERR_OK)
                return RT_ERR_FAILED;

            if (((data & 0xF) == 0xF) || ((data & 0xF) == 0xC) || ((data & 0xF) == 0x0))
                return RT_ERR_OK;
        }
    }

    return RT_ERR_PHY_NOT_MATCH;
} /* end of _phy_identify_8214B */

/* Function Name:
 *      _phy_identify_8214FB
 * Description:
 *      Identify the port is 8214FB PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is 8214FB PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not 8214FB PHY
 * Note:
 *      None
 */
static int32
_phy_identify_8214FB(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    uint32 real_model_id, real_rev_id;
    uint32 base_port = 0, data;
    hal_control_t   *pHalCtrl = NULL;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (RT_ERR_OK != phy_identify_phyid_get(unit, port, &real_model_id, &real_rev_id))
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "phy_identify_phyid_get(unit %d, port %d) failed - real_model_id = 0x%x, real_rev_id = 0x%x!!", unit, port, real_model_id, real_rev_id);
        return RT_ERR_FAILED;
    }

    RT_LOG(LOG_TRACE, MOD_HAL, "phy_identify_phyid_get(unit %d, port %d) real_model_id = 0x%x, real_rev_id= 0x%x!!", unit, port, real_model_id, real_rev_id);
    if (RT_ERR_OK != phy_identify_OUI_check(unit, port))
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "phy_identify_OUI_check(unit %d, port %d) failed!!", unit, port);
        return RT_ERR_FAILED;
    }

    if ((real_model_id == model_id) && (real_rev_id >= rev_id))
    {
        base_port = port - (port % PORT_NUM_IN_8214FB);
        if (MACDRV(pHalCtrl)->fMdrv_miim_read(unit, base_port, 10, 18, &data) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (0xC == ((data) & 0xF))
            return RT_ERR_OK;
    }

    return RT_ERR_PHY_NOT_MATCH;
} /* end of _phy_identify_8214FB */

/* Function Name:
 *      _phy_identify_8212B
 * Description:
 *      Identify the port is 8212B PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is 8212B PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not 8212B PHY
 * Note:
 *      None
 */
static int32
_phy_identify_8212B(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    uint32 real_model_id, real_rev_id;
    uint32 base_port = 0, data;
    hal_control_t   *pHalCtrl = NULL;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);
    if (RT_ERR_OK != phy_identify_phyid_get(unit, port, &real_model_id, &real_rev_id))
    {
        return RT_ERR_FAILED;
    }

    if (RT_ERR_OK != phy_identify_OUI_check(unit, port))
    {
        return RT_ERR_FAILED;
    }

    if ((real_model_id == model_id) && (real_rev_id >= rev_id))
    {
        base_port = port - (port % PORT_NUM_IN_8214FB);
        if (MACDRV(pHalCtrl)->fMdrv_miim_read(unit, base_port, 10, 18, &data) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (0x0 == ((data) & 0xF))
        {
            if (MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port+3, 8, 18, 0x93f0) != RT_ERR_OK)
                return RT_ERR_FAILED;
            if (MACDRV(pHalCtrl)->fMdrv_miim_read(unit, base_port+3, 8, 19, &data) != RT_ERR_OK)
                return RT_ERR_FAILED;

            if (((data & 0xF) == 0xE) || ((data & 0xF) == 0x8))
            {
                if ((port-base_port) == 0 || (port-base_port) == 1)
                    return RT_ERR_OK;
            }
        }
    }

    return RT_ERR_PHY_NOT_MATCH;
} /* end of _phy_identify_8212B */
#endif

#if defined(CONFIG_SDK_RTL8218B)
/* Function Name:
 *      _phy_identify_8218B
 * Description:
 *      Identify the port is 8218B PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is 8218B PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not 8218B PHY
 * Note:
 *      None
 */
static int32
_phy_identify_8218B(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    uint32 real_model_id, real_rev_id;
    uint32 base_port = 0, data;
    hal_control_t   *pHalCtrl = NULL;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (RT_ERR_OK != phy_identify_phyid_get(unit, port, &real_model_id, &real_rev_id))
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "phy_identify_phyid_get(unit %d, port %d) failed - real_model_id = 0x%x, real_rev_id = 0x%x!!", unit, port, real_model_id, real_rev_id);
        return RT_ERR_FAILED;
    }

    RT_LOG(LOG_TRACE, MOD_HAL, "phy_identify_phyid_get(unit %d, port %d) real_model_id = 0x%x, real_rev_id= 0x%x!!", unit, port, real_model_id, real_rev_id);
    if (RT_ERR_OK != phy_identify_OUI_check(unit, port))
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "phy_identify_OUI_check(unit %d, port %d) failed!!", unit, port);
        return RT_ERR_FAILED;
    }

    if ((real_model_id == model_id) && (real_rev_id >= rev_id))
    {
        if ((PHY_REV_NO_A == real_rev_id) && (HWP_8390_FAMILY_ID(unit) || HWP_8380_FAMILY_ID(unit)))
        {
            return RT_ERR_OK;
        }

        base_port = port - (port % PORT_NUM_IN_8218B);
        if (MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, 0x0a42, 29, 0x0008) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, 0x0278, 18, 0x0455) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (MACDRV(pHalCtrl)->fMdrv_miim_read(unit, base_port, 0x0260, 18, &data) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, 0x0a42, 29, 0x0000) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (0xD == ((data >> CFG_MODE_CHIP_OFFSET) & 0xF) || 0xF == ((data >> CFG_MODE_CHIP_OFFSET) & 0xF))
            return RT_ERR_OK;
    }

    return RT_ERR_PHY_NOT_MATCH;
} /* end of _phy_identify_8218B */
#endif


#if defined(CONFIG_SDK_RTL8218FB)
/* Function Name:
 *      _phy_identify_8218FB
 * Description:
 *      Identify the port is 8218FB PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is 8218FB PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not 8218FB PHY
 * Note:
 *      None
 */
static int32
_phy_identify_8218FB(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    uint32 real_model_id, real_rev_id;
    uint32 base_port = 0, data;
    hal_control_t   *pHalCtrl = NULL;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (RT_ERR_OK != phy_identify_phyid_get(unit, port, &real_model_id, &real_rev_id))
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "phy_identify_phyid_get(unit %d, port %d) failed - real_model_id = 0x%x, real_rev_id = 0x%x!!", unit, port, real_model_id, real_rev_id);
        return RT_ERR_FAILED;
    }

    RT_LOG(LOG_TRACE, MOD_HAL, "phy_identify_phyid_get(unit %d, port %d) real_model_id = 0x%x, real_rev_id= 0x%x!!", unit, port, real_model_id, real_rev_id);
    if (RT_ERR_OK != phy_identify_OUI_check(unit, port))
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "phy_identify_OUI_check(unit %d, port %d) failed!!", unit, port);
        return RT_ERR_FAILED;
    }

    if ((real_model_id == model_id) && (real_rev_id >= rev_id))
    {
        base_port = port - (port % PORT_NUM_IN_8218FB);

        if (MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, 0x0a42, 29, 0x0008) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, 0x0278, 18, 0x0455) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (MACDRV(pHalCtrl)->fMdrv_miim_read(unit, base_port, 0x0260, 18, &data) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, 0x0a42, 29, 0x0000) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (0xC == ((data) & 0xF) || 0xE == ((data) & 0xF))
        {
            if (0xC == ((data>>4) & 0xF) || 0xE == ((data>>4) & 0xF))
                return RT_ERR_OK;
        }
    }

    return RT_ERR_PHY_NOT_MATCH;
} /* end of _phy_identify_8218FB */

/* Function Name:
 *      _phy_identify_8218FB_MP
 * Description:
 *      Identify the port is 8218FB_MP PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is 8218FB_MP PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not 8218FB_MP PHY
 * Note:
 *      None
 */
static int32
_phy_identify_8218FB_MP(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    uint32 real_model_id, real_rev_id;
    uint32 base_port = 0, data;
    hal_control_t   *pHalCtrl = NULL;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (RT_ERR_OK != phy_identify_phyid_get(unit, port, &real_model_id, &real_rev_id))
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "phy_identify_phyid_get(unit %d, port %d) failed - real_model_id = 0x%x, real_rev_id = 0x%x!!", unit, port, real_model_id, real_rev_id);
        return RT_ERR_FAILED;
    }

    RT_LOG(LOG_TRACE, MOD_HAL, "phy_identify_phyid_get(unit %d, port %d) real_model_id = 0x%x, real_rev_id= 0x%x!!", unit, port, real_model_id, real_rev_id);
    if (RT_ERR_OK != phy_identify_OUI_check(unit, port))
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "phy_identify_OUI_check(unit %d, port %d) failed!!", unit, port);
        return RT_ERR_FAILED;
    }

    if ((real_model_id == model_id) && (real_rev_id >= rev_id))
    {
        base_port = port - (port % PORT_NUM_IN_8218FB);

        if (MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, PHY_PAGE_0, 29, 0x0008) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (MACDRV(pHalCtrl)->fMdrv_miim_read(unit, base_port, 0x260, 18, &data) != RT_ERR_OK)
            goto ERR;
        if (0xC != ((data>>4) & 0xF) && 0xE != ((data>>4) & 0xF))
            goto ERR;

        if (MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, PHY_PAGE_0, 29, 0x0001) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, 0x0a43, 19, 0x0002) != RT_ERR_OK)
            goto ERR;
        if (MACDRV(pHalCtrl)->fMdrv_miim_read(unit, base_port, 0x0a43, 20, &data) != RT_ERR_OK)
            goto ERR;
        if (0x6276 != data)
            goto ERR;

        MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, PHY_PAGE_0, 29, 0x0000);
        return RT_ERR_OK;
    }

    return RT_ERR_PHY_NOT_MATCH;

ERR:
    MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, PHY_PAGE_0, 29, 0x0000);
    return RT_ERR_FAILED;

} /* end of _phy_identify_8218FB_MP */
#endif  /* CONFIG_SDK_RTL8218FB */
#ifdef CONFIG_SDK_RTL8214FC
/* Function Name:
 *      _phy_identify_8214FC
 * Description:
 *      Identify the port is 8214FC PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is 8214FC PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not 8214FC PHY
 * Note:
 *      None
 */
static int32
_phy_identify_8214FC(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    uint32 real_model_id, real_rev_id;
    uint32 base_port = 0, data;
    hal_control_t   *pHalCtrl = NULL;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (RT_ERR_OK != phy_identify_phyid_get(unit, port, &real_model_id, &real_rev_id))
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "phy_identify_phyid_get(unit %d, port %d) failed - real_model_id = 0x%x, real_rev_id = 0x%x!!", unit, port, real_model_id, real_rev_id);
        return RT_ERR_FAILED;
    }

    RT_LOG(LOG_TRACE, MOD_HAL, "phy_identify_phyid_get(unit %d, port %d) real_model_id = 0x%x, real_rev_id= 0x%x!!", unit, port, real_model_id, real_rev_id);
    if (RT_ERR_OK != phy_identify_OUI_check(unit, port))
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "phy_identify_OUI_check(unit %d, port %d) failed!!", unit, port);
        return RT_ERR_FAILED;
    }

    if ((real_model_id == model_id) && (real_rev_id >= rev_id))
    {
        base_port = port - (port % PORT_NUM_IN_8214FC);
        if (MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, 0x0a42, 29, 0x0008) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, 0x0278, 18, 0x0455) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (MACDRV(pHalCtrl)->fMdrv_miim_read(unit, base_port, 0x0260, 18, &data) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, 0x0278, 18, 0x0) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, 0x0a42, 29, 0x0000) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (0x4 == ((data>>4) & 0xF) || 0x6 == ((data>>4) & 0xF))
            return RT_ERR_OK;
    }

    return RT_ERR_PHY_NOT_MATCH;
} /* end of _phy_identify_8214FC */

/* Function Name:
 *      _phy_identify_8214FC_MP
 * Description:
 *      Identify the port is 8214FC_MP PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is 8214FC_MP PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not 8214FC_MP PHY
 * Note:
 *      None
 */
static int32
_phy_identify_8214FC_MP(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    uint32 real_model_id, real_rev_id;
    uint32 base_port = 0, data;
    hal_control_t   *pHalCtrl = NULL;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (RT_ERR_OK != phy_identify_phyid_get(unit, port, &real_model_id, &real_rev_id))
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "phy_identify_phyid_get(unit %d, port %d) failed - real_model_id = 0x%x, real_rev_id = 0x%x!!", unit, port, real_model_id, real_rev_id);
        return RT_ERR_FAILED;
    }

    RT_LOG(LOG_TRACE, MOD_HAL, "phy_identify_phyid_get(unit %d, port %d) real_model_id = 0x%x, real_rev_id= 0x%x!!", unit, port, real_model_id, real_rev_id);
    if (RT_ERR_OK != phy_identify_OUI_check(unit, port))
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "phy_identify_OUI_check(unit %d, port %d) failed!!", unit, port);
        return RT_ERR_FAILED;
    }

    if ((real_model_id == model_id) && (real_rev_id >= rev_id))
    {
        base_port = port - (port % PORT_NUM_IN_8214FC);

        if (MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, PHY_PAGE_0, 29, 0x0008) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (MACDRV(pHalCtrl)->fMdrv_miim_read(unit, base_port, 0x260, 18, &data) != RT_ERR_OK)
            goto ERR;
        if (0x4 != ((data>>4) & 0xF) && 0x6 != ((data>>4) & 0xF))
            goto ERR;

        if (MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, PHY_PAGE_0, 29, 0x0001) != RT_ERR_OK)
            return RT_ERR_FAILED;
        if (MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, 0x0a43, 19, 0x0002) != RT_ERR_OK)
            goto ERR;
        if (MACDRV(pHalCtrl)->fMdrv_miim_read(unit, base_port, 0x0a43, 20, &data) != RT_ERR_OK)
            goto ERR;
        if (0x6276 != data)
            goto ERR;

        MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, PHY_PAGE_0, 29, 0x0000);
        return RT_ERR_OK;
    }

    return RT_ERR_PHY_NOT_MATCH;

ERR:
    MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, PHY_PAGE_0, 29, 0x0000);
    return RT_ERR_FAILED;

} /* end of _phy_identify_8214FC_MP */
#endif


#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      _phy_identify_8295R
 * Description:
 *      Identify the port is 8295R PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is 8214FC_MP PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not 8214FC_MP PHY
 * Note:
 *      None
 */
static int32
_phy_identify_8295R(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    uint32          id = 0, c1 = 0, c2 = 0, subId = 0, mdxMacId, blk;
    int32           ret, retSmi;
    drv_smi_mdxProtoSel_t   mdxProto, org_mdxProto;


    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (port >= RTK_MAX_PORT_PER_UNIT)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if (!HWP_10GE_PORT(unit, port))
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if ((ret = phy_8295_mdxMacId_get(unit, port, &mdxMacId)) != RT_ERR_OK)
    {
        mdxMacId = port;
    }

    retSmi = hal_miim_portSmiMdxProto_get(unit, port, &org_mdxProto);
    if (retSmi != RT_ERR_OK)
    {
        ret = RT_ERR_PHY_NOT_MATCH;
        return ret;
    }

    mdxProto = org_mdxProto;
    /* try to change to C45 */
    if (   (org_mdxProto != DRV_SMI_MDX_PROTO_C45)
        && (hal_miim_portSmiMdxProto_set(unit, port, DRV_SMI_MDX_PROTO_C45) == RT_ERR_OK))
    {
        mdxProto = DRV_SMI_MDX_PROTO_C45;
    }

    if(mdxProto == DRV_SMI_MDX_PROTO_C22)
    {
        if (RT_ERR_OK != (ret = _phy_identify_default(unit, mdxMacId, model_id, rev_id)))
        {
            goto EXIT;
        }

        if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, mdxMacId, 0, 30, &blk)) != RT_ERR_OK)
        {
            RT_LOG(LOG_TRACE, MOD_HAL, "unit %u port %u failed %x id=%x", unit, port, ret, id);
            goto EXIT;
        }

        MACDRV(pHalCtrl)->fMdrv_miim_write(unit, mdxMacId, 0, 30, 8);
        MACDRV(pHalCtrl)->fMdrv_miim_read(unit, mdxMacId, 0x279, 16, &id);
        MACDRV(pHalCtrl)->fMdrv_miim_read(unit, mdxMacId, 0x278, 22, &subId);
        MACDRV(pHalCtrl)->fMdrv_miim_write(unit, mdxMacId, 0, 30, blk);
    }
    else if (mdxProto == DRV_SMI_MDX_PROTO_C45)
    {
        MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, mdxMacId, 30, 32771, &id);
        MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, mdxMacId, 30, 32772, &subId);
    }
    else
    {
        ret = RT_ERR_PHY_NOT_MATCH;
        RT_LOG(LOG_TRACE, MOD_HAL, "unit %u port %u MDX Clause %u unreconized", unit, port, mdxProto);
        goto EXIT;
    }

    ret = RT_ERR_PHY_NOT_MATCH;
    if (id == 0x8295)
    {
        c1 = REG32_FIELD_GET(subId, 11, 0xF800); /* [15:11] */
        c2 = REG32_FIELD_GET(subId, 6, 0x07C0); /* [10:6] */
        if ((c1 == 0x12) && (c2 == 0x0))
        {
            ret = RT_ERR_OK;
        }
    }

  EXIT:
    if ((ret != RT_ERR_OK) && (org_mdxProto != mdxProto))
    {
        hal_miim_portSmiMdxProto_set(unit, port, org_mdxProto);
    }

    return ret;
}
#endif

#if defined(CONFIG_SDK_RTL8214QF)
/* Function Name:
 *      _phy_identify_8214QF
 * Description:
 *      Identify the port is 8214QF PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is 8214FC_MP PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not 8214FC_MP PHY
 * Note:
 *      None
 */
static int32
_phy_identify_8214QF(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    uint32      id = 0, c1 = 0, c2 = 0, data = 0, blk;
    int32           ret;
    rtk_port_t      base_port;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (RT_ERR_OK != (ret = _phy_identify_default(unit, port, model_id, rev_id)))
    {
        return ret;
    }

    if ((base_port = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        base_port = port - (port % 4);
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, base_port, 0, 30, &blk)) != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "unit %u port %u failed %x id=%x", unit, port, ret, id);
        return ret;
    }

    MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, 0, 30, 8);
    MACDRV(pHalCtrl)->fMdrv_miim_read(unit, base_port, 0x279, 16, &id);

    ret = RT_ERR_PHY_NOT_MATCH;
    if (id == 0x8214)
    {
        MACDRV(pHalCtrl)->fMdrv_miim_read(unit, base_port, 0x278, 22, &data);
        c1 = REG32_FIELD_GET(data, 11, 0xF800); /* [15:11] */
        c2 = REG32_FIELD_GET(data, 6, 0x07C0);  /* [10:6] */
        if ((c1 == 0x11) && (c2 == 0x6))
        {
            ret = RT_ERR_OK;
        }
    }

    MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, 0, 30, blk);

    return ret;
}
#endif /* CONFIG_SDK_RTL8214QF */

#if defined(CONFIG_SDK_RTL8224QF)
/* Function Name:
 *      _phy_identify_8224QF
 * Description:
 *      Identify the port is 8224QF PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is 8214FC_MP PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not 8214FC_MP PHY
 * Note:
 *      None
 */
static int32
_phy_identify_8224QF(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    uint32      id = 0, c1 = 0, c2 = 0, data = 0, blk;
    int32           ret;
    rtk_port_t      base_port;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (RT_ERR_OK != (ret = _phy_identify_default(unit, port, model_id, rev_id)))
    {
        return ret;
    }

    if ((base_port = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        base_port = port - (port % 4);
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, base_port, 0, 30, &blk)) != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "unit %u port %u failed %x id=%x", unit, port, ret, id);
        return ret;
    }

    MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, 0, 30, 8);
    MACDRV(pHalCtrl)->fMdrv_miim_read(unit, base_port, 0x279, 16, &id);

    ret = RT_ERR_PHY_NOT_MATCH;
    if (id == 0x8224)
    {
        MACDRV(pHalCtrl)->fMdrv_miim_read(unit, base_port, 0x278, 22, &data);
        c1 = REG32_FIELD_GET(data, 11, 0xF800); /* [15:11] */
        c2 = REG32_FIELD_GET(data, 6, 0x07C0);  /* [10:6] */
        if ((c1 == 0x11) && (c2 == 0x6))
        {
            ret = RT_ERR_OK;
        }
    }

    MACDRV(pHalCtrl)->fMdrv_miim_write(unit, base_port, 0, 30, blk);

    return ret;
}
#endif /* CONFIG_SDK_RTL8224QF */

#if defined(CONFIG_SDK_RTL8226)
static int32
_phy_identify_8226(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    int32           ret, retSmi;
    drv_smi_mdxProtoSel_t org_mdxProto;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (port >= RTK_MAX_PORT_PER_UNIT)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }


    retSmi = hal_miim_portSmiMdxProto_get(unit, port, &org_mdxProto);
    if (retSmi != RT_ERR_OK)
    {
        ret = RT_ERR_PHY_NOT_MATCH;
        return ret;
    }

    /* try to change to C45 */
    if (org_mdxProto != DRV_SMI_MDX_PROTO_C45)
    {
        hal_miim_portSmiMdxProto_set(unit, port, DRV_SMI_MDX_PROTO_C45);
    }

    ret = _phy_identify_default_c45(unit, port, model_id, rev_id);

    /* if identify result is not 8226, then change SMI bus back to original state */
    if ((ret != RT_ERR_OK) && (org_mdxProto != DRV_SMI_MDX_PROTO_C45))
    {
        hal_miim_portSmiMdxProto_set(unit, port, org_mdxProto);
    }

    return ret;

}
#endif /* CONFIG_SDK_RTL8226 */


#if (defined(CONFIG_SDK_RTL826X) || defined(CONFIG_SDK_RTL826XI))
static int32
_phy_identify_8261(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    int32           ret;
    uint32          reg_data1 = 0;
    uint32          reg_data2 = 0;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (port >= RTK_MAX_PORT_PER_UNIT)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if ((ret = _phy_identify_default_c45(unit, port, model_id, rev_id)) != RT_ERR_OK)
    {
        return ret;
    }

#if 1
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x103, &reg_data1)) != RT_ERR_OK)
        return ret;
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x104, &reg_data2)) != RT_ERR_OK)
        return ret;
    reg_data2 = REG32_FIELD_GET(reg_data2, 11, 0xF800);

    if (reg_data1 == 0x8291 && reg_data2 == 0x1F)
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_PHY_NOT_MATCH;
    }
#else
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x101, &reg_data)) != RT_ERR_OK)
        return ret;
    if ((reg_data & 0xF000) != 0xB000)
    {
        reg_data &= (~(0xF000));
        reg_data |= (0xB000);
        if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_write(unit, port, 30, 0x101, reg_data)) != RT_ERR_OK)
            return ret;
    }
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x102, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((reg_data & 0x60) != 0)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }
#endif

    return RT_ERR_OK;
}

static int32
_phy_identify_8264(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    int32           ret;
    uint32          reg_data1 = 0;
    uint32          reg_data2 = 0;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (port >= RTK_MAX_PORT_PER_UNIT)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if ((ret = _phy_identify_default_c45(unit, port, model_id, rev_id)) != RT_ERR_OK)
    {
        return ret;
    }

#if 1
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x103, &reg_data1)) != RT_ERR_OK)
        return ret;
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x104, &reg_data2)) != RT_ERR_OK)
        return ret;
    reg_data2 = REG32_FIELD_GET(reg_data2, 11, 0xF800);

    if (reg_data1 == 0x8294 && reg_data2 == 0x1F)
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_PHY_NOT_MATCH;
    }
#else
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x101, &reg_data)) != RT_ERR_OK)
        return ret;
    if ((reg_data & 0xF000) != 0xB000)
    {
        reg_data &= (~(0xF000));
        reg_data |= (0xB000);
        if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_write(unit, port, 30, 0x101, reg_data)) != RT_ERR_OK)
            return ret;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x102, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((reg_data & 0x60) == 0)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }
#endif

    return RT_ERR_OK;
}

static int32
_phy_identify_8261I(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    int32           ret;
    uint32          reg_data1 = 0;
    uint32          reg_data2 = 0;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (port >= RTK_MAX_PORT_PER_UNIT)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if ((ret = _phy_identify_default_c45(unit, port, model_id, rev_id)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x103, &reg_data1)) != RT_ERR_OK)
        return ret;
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x104, &reg_data2)) != RT_ERR_OK)
        return ret;
    reg_data2 = REG32_FIELD_GET(reg_data2, 11, 0xF800);

    if (reg_data1 == 0x8291 && reg_data2 == 0x0)
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    return RT_ERR_OK;
}

static int32
_phy_identify_8264I(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    int32           ret;
    uint32          reg_data1 = 0;
    uint32          reg_data2 = 0;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (port >= RTK_MAX_PORT_PER_UNIT)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if ((ret = _phy_identify_default_c45(unit, port, model_id, rev_id)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x103, &reg_data1)) != RT_ERR_OK)
        return ret;
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x104, &reg_data2)) != RT_ERR_OK)
        return ret;
    reg_data2 = REG32_FIELD_GET(reg_data2, 11, 0xF800);

    if (reg_data1 == 0x8294 && reg_data2 == 0x0)
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    return RT_ERR_OK;
}
#endif /* CONFIG_SDK_RTL826X || CONFIG_SDK_RTL826XI*/

#if defined(CONFIG_SDK_RTL825X)
static int32
_phy_identify_8251(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    int32           ret;
    uint32          reg_data1 = 0;
    uint32          reg_data2 = 0;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (port >= RTK_MAX_PORT_PER_UNIT)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if ((ret = _phy_identify_default_c45(unit, port, model_id, rev_id)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x103, &reg_data1)) != RT_ERR_OK)
        return ret;
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x104, &reg_data2)) != RT_ERR_OK)
        return ret;
    reg_data2 = REG32_FIELD_GET(reg_data2, 11, 0xF800);

    if (reg_data1 == 0x8251 && reg_data2 == 0x0)
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    return RT_ERR_OK;
}

static int32
_phy_identify_8254(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    int32           ret;
    uint32          reg_data1 = 0;
    uint32          reg_data2 = 0;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (port >= RTK_MAX_PORT_PER_UNIT)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if ((ret = _phy_identify_default_c45(unit, port, model_id, rev_id)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x103, &reg_data1)) != RT_ERR_OK)
        return ret;
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x104, &reg_data2)) != RT_ERR_OK)
        return ret;
    reg_data2 = REG32_FIELD_GET(reg_data2, 11, 0xF800);

    if (reg_data1 == 0x8254 && reg_data2 == 0x0)
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    return RT_ERR_OK;
}
#endif /* CONFIG_SDK_RTL825X */

#if defined(CONFIG_SDK_RTL825XI)
static int32
_phy_identify_8251I(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    int32           ret;
    uint32          reg_data1 = 0;
    uint32          reg_data2 = 0;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (port >= RTK_MAX_PORT_PER_UNIT)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if ((ret = _phy_identify_default_c45(unit, port, model_id, rev_id)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x103, &reg_data1)) != RT_ERR_OK)
        return ret;
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x104, &reg_data2)) != RT_ERR_OK)
        return ret;
    reg_data2 = REG32_FIELD_GET(reg_data2, 11, 0xF800);

    if (reg_data1 == 0x8251 && reg_data2 == 0x9)
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    return RT_ERR_OK;
}

static int32
_phy_identify_8254I(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    int32           ret;
    uint32          reg_data1 = 0;
    uint32          reg_data2 = 0;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (port >= RTK_MAX_PORT_PER_UNIT)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if ((ret = _phy_identify_default_c45(unit, port, model_id, rev_id)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x103, &reg_data1)) != RT_ERR_OK)
        return ret;
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x104, &reg_data2)) != RT_ERR_OK)
        return ret;
    reg_data2 = REG32_FIELD_GET(reg_data2, 11, 0xF800);

    if (reg_data1 == 0x8254 && reg_data2 == 0x9)
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    return RT_ERR_OK;
}
#endif /* CONFIG_SDK_RTL825XI */

#if defined(CONFIG_SDK_RTL825XL)
static int32
_phy_identify_8251L(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    int32           ret;
    uint32          reg_data1 = 0;
    uint32          reg_data2 = 0;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (port >= RTK_MAX_PORT_PER_UNIT)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if ((ret = _phy_identify_default_c45(unit, port, model_id, rev_id)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x103, &reg_data1)) != RT_ERR_OK)
        return ret;
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x104, &reg_data2)) != RT_ERR_OK)
        return ret;
    reg_data2 = REG32_FIELD_GET(reg_data2, 11, 0xF800);

    if (reg_data1 == 0x8251 && reg_data2 == 0xC)
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    return RT_ERR_OK;
}

static int32
_phy_identify_8254L(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    int32           ret;
    uint32          reg_data1 = 0;
    uint32          reg_data2 = 0;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (port >= RTK_MAX_PORT_PER_UNIT)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if ((ret = _phy_identify_default_c45(unit, port, model_id, rev_id)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x103, &reg_data1)) != RT_ERR_OK)
        return ret;
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x104, &reg_data2)) != RT_ERR_OK)
        return ret;
    reg_data2 = REG32_FIELD_GET(reg_data2, 11, 0xF800);

    if (reg_data1 == 0x8254 && reg_data2 == 0xC)
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    return RT_ERR_OK;
}
#endif /* CONFIG_SDK_RTL825XL */

#if defined(CONFIG_SDK_RTL8224)
static int32
_phy_identify_8224(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    int32           ret;
    uint32          reg_data1 = 0;
    uint32          reg_data2 = 0;
    rtk_port_t      base_port = 0;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (port >= RTK_MAX_PORT_PER_UNIT)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if ((ret = _phy_identify_default_c45(unit, port, model_id, rev_id)) != RT_ERR_OK)
    {
        return ret;
    }

    base_port = HWP_PHY_BASE_MACID(unit, port);

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, base_port, 30, 0x0004, &reg_data1)) != RT_ERR_OK)
        return ret;
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, base_port, 30, 0x0005, &reg_data2)) != RT_ERR_OK)
        return ret;

    if (reg_data1 == 0x0 && reg_data2 == 0x8224)
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    return RT_ERR_OK;
}
#endif /* CONFIG_SDK_RTL8224 */


#if defined(CONFIG_SDK_RTL826XB)
static int32
_phy_identify_8261B(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    int32           ret;
    uint32          reg_data1 = 0;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (port >= RTK_MAX_PORT_PER_UNIT)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if ((ret = _phy_identify_default_c45(unit, port, model_id, rev_id)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x103, &reg_data1)) != RT_ERR_OK)
        return ret;

    if (reg_data1 == 0x8261)
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_PHY_NOT_MATCH;
    }
}

static int32
_phy_identify_8264B(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    int32           ret;
    uint32          reg_data1 = 0;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (port >= RTK_MAX_PORT_PER_UNIT)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if ((ret = _phy_identify_default_c45(unit, port, model_id, rev_id)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 30, 0x103, &reg_data1)) != RT_ERR_OK)
        return ret;

    if (reg_data1 == 0x8264)
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_PHY_NOT_MATCH;
    }
}
#endif

#if defined(CONFIG_SDK_RTL9300)
/* Function Name:
 *      _phy_identify_9300_serdes
 * Description:
 *      Identify the port is 9300 intra serdes PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is intra serdes PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not intra serdes PHY
 * Note:
 *      None
 */
static int32
_phy_identify_9300_serdes(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    int32           sdsId;

    RT_LOG(LOG_EVENT, MOD_HAL, "%s(unit %d, port %d) function!!", __FUNCTION__, unit, port);

    if (!HWP_9300_FAMILY_ID(unit))
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if (!HWP_SERDES_PORT(unit, port))
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    sdsId = HWP_PORT_SDSID(unit, port);
    if (sdsId == HWP_NONE)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    switch(HWP_PORT_ETH(unit, port))
    {
        case HWP_GE:
            RT_LOG(LOG_EVENT, MOD_HAL, "_phy_identify_9300_serdes(unit %d, port %d) switch case HWP_GE function!!", unit, port);
            if((RTK_MII_1000BX_FIBER == HWP_SDS_MODE(unit, sdsId)) || (RTK_MII_SGMII == HWP_SDS_MODE(unit, sdsId)))
                return RT_ERR_OK;
            else
                return RT_ERR_FAILED;
            break;
        case HWP_XGE:
            RT_LOG(LOG_EVENT, MOD_HAL, "_phy_identify_9300_serdes(unit %d, port %d) switch case HWP_XGE function!!", unit, port);
            if((RTK_MII_10GR == HWP_SDS_MODE(unit, sdsId))||(RTK_MII_2500Base_X == HWP_SDS_MODE(unit, sdsId)))
                return RT_ERR_OK;
            else
                return RT_ERR_FAILED;
            break;
        case HWP_SXGE:
            RT_LOG(LOG_EVENT, MOD_HAL, "_phy_identify_9300_serdes(unit %d, port %d) switch case HWP_SXGE function!!", unit, port);
            if(RTK_MII_RXAUI_PLUS == HWP_SDS_MODE(unit, sdsId))
                return RT_ERR_OK;
            else
                return RT_ERR_FAILED;
            break;
         default:
            RT_LOG(LOG_EVENT, MOD_HAL, "_phy_identify_9300_serdes(unit %d, port %d) switch case default function!!", unit, port);
                return RT_ERR_FAILED;
    }

    return RT_ERR_PHY_NOT_MATCH;
}
#endif/* CONFIG_SDK_RTL9300 */

#if defined(CONFIG_SDK_RTL8218D)
/* Function Name:
 *      _phy_identify_8218dnmp
 * Description:
 *      Identify the port is 8218d nmp  PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is intra serdes PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not intra serdes PHY
 * Note:
 *      None
 */
static int32
_phy_identify_8218dnmp(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    uint32          basePort, blk;
    int32           ret;
    uint32          data;

    if ((ret = _phy_identify_default(unit, port, model_id, rev_id)) != RT_ERR_OK)
    {
        return ret;
    }

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    basePort = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, basePort, 0, 30, &blk)) != RT_ERR_OK)
    {
        return ret;
    }

    MACDRV(pHalCtrl)->fMdrv_miim_write(unit, basePort, 0, 30, 8);
    MACDRV(pHalCtrl)->fMdrv_miim_read(unit, basePort, 0x327, 0x15, &data);
    MACDRV(pHalCtrl)->fMdrv_miim_write(unit, basePort, 0, 30, blk);

    if (REG32_FIELD_GET(data, PHY_8218D_CHIP_ID_MAP_OFFSET, PHY_8218D_CHIP_ID_MAP_MASK) == 0)
    {
        return RT_ERR_OK;
    }
    return RT_ERR_PHY_NOT_MATCH;

}

/* Function Name:
 *      _phy_identify_8218d
 * Description:
 *      Identify the port is 8218d nmp  PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is intra serdes PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not intra serdes PHY
 * Note:
 *      None
 */
static int32
_phy_identify_8218d(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    hal_control_t   *pHalCtrl = NULL;
    uint32          basePort, blk;
    int32           ret;
    uint32          data;

    if ((ret = _phy_identify_default(unit, port, model_id, rev_id)) != RT_ERR_OK)
    {
        return ret;
    }

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    basePort = HWP_PHY_BASE_MACID(unit, port);
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, basePort, 0, 30, &blk)) != RT_ERR_OK)
    {
        return ret;
    }

    MACDRV(pHalCtrl)->fMdrv_miim_write(unit, basePort, 0, 30, 8);
    MACDRV(pHalCtrl)->fMdrv_miim_read(unit, basePort, 0x327, 0x15, &data);
    MACDRV(pHalCtrl)->fMdrv_miim_write(unit, basePort, 0, 30, blk);

    if (REG32_FIELD_GET(data, PHY_8218D_CHIP_ID_MAP_OFFSET, PHY_8218D_CHIP_ID_MAP_MASK) == 1)
    {
        return RT_ERR_OK;
    }
    return RT_ERR_PHY_NOT_MATCH;
}
#endif

#if defined(CONFIG_SDK_RTL8218E)
static int32
_phy_identify_8218e(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    int32           ret = RT_ERR_OK;

    if ((ret = _phy_identify_default(unit, port, model_id, rev_id)) != RT_ERR_OK)
    {
        return ret;
    }

    return ret;
}
#endif


#if defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      _phy_identify_9310_serdes
 * Description:
 *      Identify the port is 9310 intra serdes or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is intra serdes PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not intra serdes PHY
 * Note:
 *      None
 */
static int32
_phy_identify_9310_serdes(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    int32           sdsId;

    RT_LOG(LOG_EVENT, MOD_HAL, "%s(unit %d, port %d) function!!", __FUNCTION__, unit, port);

    if (!HWP_9310_FAMILY_ID(unit))
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if (!HWP_SERDES_PORT(unit, port))
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    sdsId = HWP_PORT_SDSID(unit, port);
    if (HWP_NONE == sdsId)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if (HWP_PORT_MEDI(unit, port) == HWP_SERDES)
        return RT_ERR_OK;

    return RT_ERR_PHY_NOT_MATCH;
}
#endif/* CONFIG_SDK_RTL9310 */

/* Public Function Body */

/* Function Name:
 *      phy_identify_OUI_check
 * Description:
 *      Identify the OUI is the realtek OUI or not?
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - Realtek OUI
 *      RT_ERR_FAILED - not Realtek OUI
 * Note:
 *      None
 */
int32
phy_identify_OUI_check(uint32 unit, rtk_port_t port)
{
    uint32  data0, data1;
    uint32  page;
    int32   ret = RT_ERR_FAILED;
    hal_control_t   *pHalCtrl = NULL;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);
    RT_PARAM_CHK((NULL == MACDRV(pHalCtrl)->fMdrv_miim_read), RT_ERR_FAILED);

    page = PHY_PAGE_0;  /* Normal case: access the page 0 */

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, port, page, PHY_IDENTIFIER_1_REG, &data0)) != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "fMdrv_miim_read(unit %d, port %d) PHY_IDENTIFIER_1_REG failed!!", unit, port);
        return ret;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, port, page, PHY_IDENTIFIER_2_REG, &data1)) != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "fMdrv_miim_read(unit %d, port %d) PHY_IDENTIFIER_2_REG failed!!", unit, port);
        return ret;
    }

    if ((data0 != PHY_IDENT_OUI_03_18) ||
        ((data1 >> OUI_19_24_OFFSET) != PHY_IDENT_OUI_19_24))
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "Compare OUI data0 failed(unit %d, port %d): data0= 0x%x; PHY_IDENT_OUI_03_18 = 0x%x!!", unit, port, data0, PHY_IDENT_OUI_03_18);
        RT_LOG(LOG_TRACE, MOD_HAL, "Compare OUI data1 failed(unit %d, port %d): data1.b[24:19]= 0x%x; PHY_IDENT_OUI_19_24 = 0x%x!!", unit, port, (data1 >> OUI_19_24_OFFSET), PHY_IDENT_OUI_19_24);
        return RT_ERR_FAILED;
    }

    return ret;
} /* end of phy_identify_OUI_check */

/* Function Name:
 *      phy_mapper_hook
 * Description:
 *      Hook PHY mapper with the specified chip.
 * Input:
 *      rt_phyctrl_t - pointer of rt_phyctrl_t
 * Output:
 *      rt_phyctrl_t - pointer of rt_phyctrl_t
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
_phy_mapper_hook(rt_phyctrl_t *pPhyctrl)
{
    uintptr *fP = NULL;
    uint32 i = 0;
    rt_phydrv_t *pPhydrv;

    pPhydrv = (rt_phydrv_t*)osal_alloc(sizeof(rt_phydrv_t));

    if(NULL == pPhydrv)
    {
        osal_printf("Failed to create phy driver");
        return RT_ERR_FAILED;
    }
    osal_memset(pPhydrv, 0, sizeof(rt_phydrv_t));

    fP = (uintptr *)pPhydrv;
    for (i = 0; i < sizeof(rt_phydrv_t)/sizeof(void *); i++,  fP++)
    {
        *fP = (uintptr)(phy_common_unavail);
    }
    pPhyctrl->mapperInit_func(pPhydrv);
    pPhyctrl->pPhydrv = pPhydrv;

    return RT_ERR_OK;
}/* end of phy_mapper_hook */


/* Function Name:
 *      phy_identify_find
 * Description:
 *      Find this kind of PHY control structure from the phy supported list.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of PHY control structure that found
 * Note:
 *      None
 */
rt_phyctrl_t *
phy_identify_find(uint32 unit, rtk_port_t port)
{
    phy_type_t              phyType;
    rt_phyctrl_linkNode_t   *node;
    int32                   ret;

    if ((phyType = HWP_PHY_MODEL_BY_PORT(unit, port)) >= RTK_PHYTYPE_END)
    {
        return NULL;
    }

    node = phyctrlLnkList[phyType];
    for ( ; node != NULL; node = node->next)
    {
        ret = (node->pPhyctrl->chk_func)(unit, port, node->pPhyctrl->phy_model_id, node->pPhyctrl->phy_rev_id);
        if (ret == RT_ERR_PHY_NOT_MATCH)
        {
            continue;
        }
        else if(ret == RT_ERR_OK)
        {
            return node->pPhyctrl;
        }
        else
        {
            RT_LOG(LOG_TRACE, MOD_HAL, "unit %u port %u identify not found by PHY type %d.\n", unit, port, phyType);
            return NULL;
        }

    }

    return NULL;
} /* end of phy_identify_find */


/* Function Name:
 *      phy_identify_phyid_get
 * Description:
 *      Get this phy model id and its revision id from chip.
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pModel_id - pointer buffer of phy model id
 *      pRev_id   - pointer buffer of phy revision id
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_identify_phyid_get(
    uint32      unit,
    rtk_port_t  port,
    uint32      *pModel_id,
    uint32      *pRev_id)
{
    uint32  data;
    uint32  page;
    int32   ret = RT_ERR_FAILED;
    hal_control_t   *pHalCtrl = NULL;

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);
    RT_PARAM_CHK((NULL == MACDRV(pHalCtrl)->fMdrv_miim_read), RT_ERR_FAILED);

    page = PHY_PAGE_0;   /* Normal case: access the page 0 */

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, port, page, PHY_IDENTIFIER_2_REG, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* model id is bit[9:4]; rev id is bit[3:0] */
    *pModel_id = (data & ModelNumber_MASK) >> ModelNumber_OFFSET;
    *pRev_id = (data & RevisionNumber_MASK) >> RevisionNumber_OFFSET;

    return ret;
} /* end of phy_identify_phyid_get */


/* Function Name:
 *      phy_identify_int_find
 * Description:
 *      Find this kind of PHY control structure from the internal phy supported list.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of PHY control structure that found
 * Note:
 *      None
 */
rt_phyctrl_t *
phy_identify_int_find(uint32 unit, rtk_port_t port)
{
    int32  size = 0, i;

    size = sizeof(supported_int_phys) / sizeof(rt_phyctrl_t);

    for (i = size - 1; i >= 0; i--)
    {
        if ((supported_int_phys[i].chk_func)(unit, port, supported_int_phys[i].phy_model_id, supported_int_phys[i].phy_rev_id) == RT_ERR_OK)
        {
            return (&supported_int_phys[i]);
        }
    }

    return NULL;
} /* end of phy_identify_int_find */


/* Function Name:
 *      phy_identify_10GeSerdesDrv_find
 * Description:
 *      Find 10GE serdes driver
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of PHY control structure that found
 * Note:
 *      None
 */
rt_phyctrl_t *
phy_identify_10GeSerdesDrv_find(uint32 unit, rtk_port_t port)
{
    int32  size = 0, i;

    size = sizeof(supported_int_phys) / sizeof(rt_phyctrl_t);

    for (i = size - 1; i >= 0; i--)
    {
        if (HWP_9310_FAMILY_ID(unit) &&
            (supported_int_phys[i].pPhydrv->phydrv_index == RT_PHYDRV_RTL9310_SERDES) && (supported_int_phys[i].pPhyInfo->eth_type == HWP_XGE))
        {
            return (&supported_int_phys[i]);
        }
        else
        if (HWP_9300_FAMILY_ID(unit) &&
            (supported_int_phys[i].pPhydrv->phydrv_index == RT_PHYDRV_RTL9300_SERDES) && (supported_int_phys[i].pPhyInfo->eth_type == HWP_XGE))
        {
            return (&supported_int_phys[i]);
        }
        else if (HWP_8390_FAMILY_ID(unit) &&
            (supported_int_phys[i].pPhydrv->phydrv_index == RT_PHYDRV_RTL8390_SERDES_GE) && (supported_int_phys[i].pPhyInfo->eth_type == HWP_XGE))
        {
            return (&supported_int_phys[i]);
        }
    }
    return NULL;
}

rt_phyctrl_t *
phy_identify_serdesDrvCtrl_get(uint32 unit, rtk_port_t port)
{
    int32  size = 0, i;

    size = sizeof(supported_int_phys) / sizeof(rt_phyctrl_t);

    for (i = size - 1; i >= 0; i--)
    {
#if defined(CONFIG_SDK_RTL8390)
        if (HWP_8390_50_FAMILY(unit))
        {
            if ((supported_int_phys[i].phy_model_id == RTL8390_FAMILY_ID) && (supported_int_phys[i].pPhyInfo->eth_type == HWP_PORT_ETH(unit, port)))
                return &supported_int_phys[i];
        }
        else
#endif
#if defined(CONFIG_SDK_RTL8380)
        if (HWP_8380_30_FAMILY(unit))
        {
            if ((supported_int_phys[i].phy_model_id == RTL8380_FAMILY_ID) && (supported_int_phys[i].pPhyInfo->eth_type == HWP_PORT_ETH(unit, port)))
                return &supported_int_phys[i];
        }
        else
#endif
        if ((supported_int_phys[i].phy_model_id == HWP_CHIP_FAMILY_ID(unit)) && (supported_int_phys[i].pPhyInfo->eth_type == HWP_PORT_ETH(unit, port)))
        {
            return &supported_int_phys[i];
        }
    }
    return NULL;
}


/* Function Name:
 *      _phy_identify_phyctrl_register
 * Description:
 *      Register a phyctrl node.
 * Input:
 *      pPhyctrl    - pointer of PHY control data entry.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
_phy_identify_phyctrl_register(rt_phyctrl_t *pPhyctrl)
{
    rt_phyctrl_linkNode_t    *node;

    if (pPhyctrl->phyType >= RTK_PHYTYPE_END)
    {
        RT_INIT_ERR(RT_ERR_FAILED, MOD_HAL, "Unreconized PHY type: %d.\n", pPhyctrl->phyType);
        return;
    }
    if (RT_ERR_FAILED == _phy_mapper_hook(pPhyctrl))
        return;

    node = (rt_phyctrl_linkNode_t *) osal_alloc(sizeof(rt_phyctrl_linkNode_t));
    if (node == NULL)
    {
        RT_INIT_ERR(RT_ERR_FAILED, MOD_HAL, "Insufficient memory for PHY driver.\n");
        return;
    }

    osal_memset(node, 0, sizeof(rt_phyctrl_linkNode_t));
    node->pPhyctrl = pPhyctrl;
    if (phyctrlLnkList[pPhyctrl->phyType] != NULL)
    {
        node->next = phyctrlLnkList[pPhyctrl->phyType];
    }
    phyctrlLnkList[pPhyctrl->phyType] = node;
}


/* Function Name:
 *      phy_identify_driver_find_by_phyType
 * Description:
 *      Find PHY driver from the phy type.
 * Input:
 *      phyType - PHY type
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of PHY driver that found
 * Note:
 *      None
 */
rt_phyctrl_t *
phy_identify_driver_find_by_phyType(phy_type_t phyType)
{
    rt_phyctrl_linkNode_t   *node;

    if (phyType >= RTK_PHYTYPE_END)
    {
        RT_ERR(RT_ERR_FAILED, MOD_HAL, "Unreconized PHY type: %d.\n", phyType);
        return NULL;
    }

    if ((node = phyctrlLnkList[phyType]) == NULL)
        return NULL;

    return node->pPhyctrl;
}

/* Function Name:
 *      phy_identify_driver_find_blind
 * Description:
 *      Find PHY driver from all drivers
 * Input:
 *      unit -  unit id
 *      port -  mac id
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of PHY control that found
 * Note:
 *      None
 */
rt_phyctrl_t *
phy_identify_driver_find_blindly(uint32 unit, rtk_port_t port)
{
    int32  size = 0, i;

    size = sizeof(supported_phys) / sizeof(rt_phyctrl_t);
    for (i = size - 1; i >= 0; i--)
    {
        if ((supported_phys[i].chk_func)(unit, port, supported_phys[i].phy_model_id, supported_phys[i].phy_rev_id) == RT_ERR_OK)
        {
            return (&supported_phys[i]);
        }
    }

    size = sizeof(supported_int_phys) / sizeof(rt_phyctrl_t);
    for (i = size - 1; i >= 0; i--)
    {
        if ((supported_int_phys[i].chk_func)(unit, port, supported_int_phys[i].phy_model_id, supported_int_phys[i].phy_rev_id) == RT_ERR_OK)
        {
            return (&supported_int_phys[i]);
        }
    }

    return NULL;
}


/* Function Name:
 *      phy_identify_init
 * Description:
 *      Initial identify databases
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
phy_identify_init(void)
{
    int32           size, i;

    if (phyctrlLnkList_init == TRUE)
    {
        return;
    }

    /* internal phy */
    size = sizeof(supported_int_phys) / sizeof(rt_phyctrl_t);
    for (i = 0; i < size; i++)
    {
        _phy_identify_phyctrl_register(&supported_int_phys[i]);
    }/* end for */

    /* external phy */
    size = sizeof(supported_phys) / sizeof(rt_phyctrl_t);
    for (i = 0; i < size; i++)
    {
        _phy_identify_phyctrl_register(&supported_phys[i]);
    }

    phyctrlLnkList_init = TRUE;

}


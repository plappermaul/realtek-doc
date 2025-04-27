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
 * Purpose : Model list.
 *           This file shall be custom defined.
 * Feature :
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <osal/lib.h>
#include <osal/print.h>
#include <hwp/hw_profile.h>
#include <hal/chipdef/chipdef.h>
#include <drv/gpio/generalCtrl_gpio.h>
#include <common/debug/rt_log.h>
#include <common/util/rt_util.h>

/* Include hardware profile files of supported boards */

#if defined(CONFIG_SDK_RTL9300)
    #include <hwp/hw_profiles/rtl9301_2x8214QF_4xge.c>
    #include <hwp/hw_profiles/rtl9301_3x8218b_4xge.c>
    #include <hwp/hw_profiles/rtl9301_8218b_4xge.c>
    #include <hwp/hw_profiles/rtl9301_8218b_4xge_cascade.c>
    #include <hwp/hw_profiles/rtl9301_2x8214QF_4xge_cascade.c>
    #include <hwp/hw_profiles/rtl9301_14QF_4xge_18b_4xge_cascade.c>
    #include <hwp/hw_profiles/rtl9303_8xge.c>
    #include <hwp/hw_profiles/rtl9303_8x2_5g.c>
    #include <hwp/hw_profiles/rtl9303_8x8226.c>
    #include <hwp/hw_profiles/rtl9302de_2xRTL8284.c>
    #include <hwp/hw_profiles/rtl9301_3x8218d_4xge.c>
    #include <hwp/hw_profiles/rtl9301_6x8214QF_4xge.c>
    #include <hwp/hw_profiles/rtl9301_2x8214fc_4x8214QF_4xge.c>
    #include <hwp/hw_profiles/rtl9301_6x8218d_2x8295r_cascade.c>
    #include <hwp/hw_profiles/rtl9301_2x8218b_4x8218d_2x8295r_cascade.c>
    #include <hwp/hw_profiles/rtl9303_2xcust1.c>
    #include <hwp/hw_profiles/rtl9302c_4xcust1.c>
    #include <hwp/hw_profiles/rtl9302b_2x8218d_2xcust1_4xge.c>
    #include <hwp/hw_profiles/rtl9302b_2x8218d_2x8284_4xge.c>
    #include <hwp/hw_profiles/rtl9302b_2x8218e_2x8224qf_4xge.c>
    #include <hwp/hw_profiles/rtl9302c_2x8284_2xge.c>
    #include <hwp/hw_profiles/rtl9302c_2x8224_2xge.c>
    #include <hwp/hw_profiles/rtl9302c_2x8224_2xge_2x8261n.c>
    #include <hwp/hw_profiles/rtl9302c_4xrtl8284_4xge.c>
    #include <hwp/hw_profiles/rtl9302d_6x8224qf_2xge.c>
    #include <hwp/hw_profiles/rtl9301_3x8218d_2x8226card_2xge.c>
    #include <hwp/hw_profiles/rtl9301_3x8218e_4xge.c>
    #include <hwp/hw_profiles/rtl9303_2x8254L_demo.c>
    #include <hwp/hw_profiles/rtl9303_6x8254L_6xspi.c>
#endif

#if defined(CONFIG_SDK_RTL8390)
  #include <hwp/hw_profiles/rtl8396m_8218b_8214qf_8295r_C45_demo.c>
  #include <hwp/hw_profiles/rtl8396m_8218b_8214qf_8295r_demo.c>
  #include <hwp/hw_profiles/rtl8396m_8214qf_8295r_C45_demo.c>
  #include <hwp/hw_profiles/rtl8396m_8214qf_8295r_es_demo.c>
  #include <hwp/hw_profiles/rtl8391m_demo.c>
  #include <hwp/hw_profiles/rtl8391m_14c_demo.c>
  #include <hwp/hw_profiles/rtl8392m_demo.c>
  #include <hwp/hw_profiles/rtl8393m_demo.c>
  #include <hwp/hw_profiles/rtl8396m_demo.c>
  #include <hwp/hw_profiles/rtl8353m_demo.c>
  #include <hwp/hw_profiles/rtl8353m_qa.c>
  #include <hwp/hw_profiles/rtl8353m_14b_demo.c>
  #include <hwp/hw_profiles/rtl8351m_demo.c>
  #include <hwp/hw_profiles/rtl8393m_8218d.c>
  #include <hwp/hw_profiles/rtl8393m_8218e.c>
#endif /* CONFIG_SDK_RTL8390 */

#if defined(CONFIG_SDK_RTL8380)
  #include <hwp/hw_profiles/rtl8382m_8218b_intphy_8218b_8214fc_demo.c>
  #include <hwp/hw_profiles/rtl8382m_8218d_intphy_8218d_8214fc_demo.c>
  #include <hwp/hw_profiles/rtl8382m_8218b_intphy_8218b_2fib_1g_demo.c>
  #include <hwp/hw_profiles/rtl8382m_8218b_intphy_8218b_8214b_demo.c>
  #include <hwp/hw_profiles/rtl8382m_8218b_intphy_8218fb_demo.c>
  #include <hwp/hw_profiles/rtl8382m_8218d_intphy_8218d_2fib_1g_demo.c>
  #include <hwp/hw_profiles/rtl8382m_8218e_intphy_8218e_2fib_1g_demo.c>
  #include <hwp/hw_profiles/rtl8380m_intphy_2fib_1g_demo.c>
  #include <hwp/hw_profiles/rtl8380h_intphy_demo.c>
  #include <hwp/hw_profiles/rtl8332m_8208l_intphy_8208l_8214b_demo.c>
  #include <hwp/hw_profiles/rtl8332m_8208l_intphy_8208l_8214c_demo.c>
  #include <hwp/hw_profiles/rtl8330m_intphy_2fib_1g_demo.c>
  #include <hwp/hw_profiles/rtl8330m_intphy_8212b_2ge_demo.c>
  #include <hwp/hw_profiles/rtl8330m_intphy_8212b_demo.c>
  #include <hwp/hw_profiles/rtl8381m_intphy_8214c_2fib_1g_demo.c>
  #include <hwp/hw_profiles/rtl8381m_intphy_8214fc_2fib_1g_demo.c>
#endif /* CONFIG_SDK_RTL8380 */

#if defined(CONFIG_SDK_RTL9310)
    #include <hwp/hw_profiles/rtl9311_6x8218d_6xge.c>
    #include <hwp/hw_profiles/rtl9311_6x8218d_6xge_pci.c>
    #include <hwp/hw_profiles/rtl9311_6x8218d_6xge_demo.c>
    #include <hwp/hw_profiles/rtl9311_6x8218e_6xge_demo.c>
    #include <hwp/hw_profiles/rtl9311_6x8218d_6xge1g.c>
    #include <hwp/hw_profiles/rtl9311_6x8218d_6xFib1g100.c>
    #include <hwp/hw_profiles/rtl9311E.c>
    #include <hwp/hw_profiles/rtl9311R.c>
    #include <hwp/hw_profiles/rtl9311R_pci.c>
    #include <hwp/hw_profiles/rtl9311_usermodelkm_pci_default.c>
    #include <hwp/hw_profiles/rtl9313_12xge.c>
    #include <hwp/hw_profiles/rtl9313_12xge_pci.c>
    #include <hwp/hw_profiles/rtl9313_12xge1g.c>
    #include <hwp/hw_profiles/rtl9313_12xFib1g100.c>
    #include <hwp/hw_profiles/rtl9312_24x2p5g_6xge.c>
    #include <hwp/hw_profiles/rtl9313_2x8264_2x8261_2xge_demo.c>
    #include <hwp/hw_profiles/rtl9313_2x8264_2x8261_2xge_demo_b.c>
    #include <hwp/hw_profiles/rtl9313_2x8254L_2x8261I_2xge_demo.c>
    #include <hwp/hw_profiles/rtl9313_2x8264b_2x8261n_2xge_ab.c>
    #include <hwp/hw_profiles/rtl9313_2x8264b_2x8261n_2xge_d.c>
    #include <hwp/hw_profiles/rtl9313_2x8264b_2x8261n_2xge_gh.c>
    #include <hwp/hw_profiles/rtl9313_10x8261n.c>
#endif  /* CONFIG_SDK_RTL9310 */

#include <hwp/hw_profile_internal.c>


/*
 * Data Declaration
 */

/* list of supported hardware profiles */
const hwp_hwProfile_t *hwp_hwProfile_list[] =
{
#if defined(CONFIG_SDK_RTL9300)
    &rtl9301_2x8214qf_4xge,
    &rtl9301_3x8218b_4xge,
    &rtl9301_8218b_4xge,
    &rtl9301_8218b_4xge_cascade,
    &rtl9301_2x8214qf_4xge_cascade,
    &rtl9301_14qf_4xge_18b_4xge_cascade,
    &rtl9301_3x8218d_4xge,
    &rtl9301_6x8214qf_4xge,
    &rtl9301_2x8214fc_4x8214qf_4xge,
    &rtl9301_6x8218d_2x8295r_cascade,
    &rtl9301_2x8218b_4x8218d_2x8295r_cascade,
    &rtl9302b_2x8218d_2xcust1_4xge,
    &rtl9302c_4xcust1,
    &rtl9303_2xcust1,
    &rtl9303_8xge,
    &rtl9303_8x2_5g,
    &rtl9303_8x8226,
    &rtl9302de_2xRTL8284,
    &rtl9302d_6x8224qf_2xge,
    &rtl9302c_2x8284_2xge,
    &rtl9302c_2x8224_2xge,
    &rtl9302c_2x8224_2xge_2x8261n,
    &rtl9302c_4x8284_4xge,
    &rtl9302b_2x8218d_2x8284_4xge,
    &rtl9302b_2x8218e_2x8224qf_4xge,
    &rtl9301_3x8218d_2x8226card_2xge,
    &rtl9301_3x8218e_4xge,
    &rtl9303_2x8254L_demo,
    &rtl9303_6x8254L_6xspi,
#endif

#if defined(CONFIG_SDK_RTL8390)
    &rtl8396m_8218b_8214qf_8295r_c45_demo,
    &rtl8396m_8218b_8214qf_8295r_demo,
    &rtl8396m_8214qf_8295r_c45_demo,
    &rtl8396m_8214qf_8295r_es_demo,
    &rtl8391m_demo,
    &rtl8391m_14c_demo,
    &rtl8393m_demo,
    &rtl8396m_demo,
    &rtl8353m_demo,
    &rtl8353m_qa,
    &rtl8353m_8214b_demo,
    &rtl8351m_demo,
    &rtl8392m_demo,
    &rtl8393m_8218d,
    &rtl8393m_8218e,
#endif

#if defined(CONFIG_SDK_RTL8380)
    &rtl8382m_8218b_intphy_8218b_8214fc_demo,
    &rtl8382m_8218d_intphy_8218d_8214fc_demo,
    &rtl8382m_8218b_intphy_8218fb_demo,
    &rtl8382m_8218b_intphy_8218b_8214b_demo,
    &rtl8382m_8218b_intphy_8218b_2fib_1g_demo,
    &rtl8382m_8218d_intphy_8218d_2fib_1g_demo,
    &rtl8382m_8218e_intphy_8218e_2fib_1g_demo,
    &rtl8381m_intphy_8214fc_2fib_1g_demo,
    &rtl8381m_intphy_8214c_2fib_1g_demo,
    &rtl8380m_intphy_2fib_1g_demo,
    &rtl8380h_intphy_demo,
    &rtl8332m_8208l_intphy_8208l_8214b_demo,
    &rtl8332m_8208l_intphy_8208l_8214c_demo,
    &rtl8330m_intphy_8212b_demo,
    &rtl8330m_intphy_2fib_1g_demo,
    &rtl8330m_intphy_8212b_2ge_demo,
#endif
#if defined(CONFIG_SDK_RTL9310)
    &rtl9311_6x8218d_6xge,
    &rtl9311_6x8218d_6xge_pci,
    &rtl9311_6x8218d_6xge_demo,
    &rtl9311_6x8218e_6xge_demo,
    &rtl9311_6x8218d_6xge1g,
    &rtl9311_6x8218d_6xFib1g100,
    &rtl9311E,
    &rtl9311R,
    &rtl9311R_pci,
    &rtl9311_usermodelkm_pci_default,
    &rtl9313_12xge,
    &rtl9313_12xge_pci,
    &rtl9313_12xge1g,
    &rtl9313_12xFib1g100,
    &rtl9312_24x2p5g_6xge,
    &rtl9313_2x8264_2x8261_2xge_demo,
    &rtl9313_2x8264_2x8261_2xge_demo_b,
    &rtl9313_2x8254L_2x8261I_2xge_demo,
    &rtl9313_2x8264b_2x8261n_2xge_ab,
    &rtl9313_2x8264b_2x8261n_2xge_d,
    &rtl9313_2x8264b_2x8261n_2xge_gh,
    &rtl9313_10x8261n,
#endif  /* CONFIG_SDK_RTL9310 */
    RTK_INTERNAL_PROFILE
    NULL,
}; /* end hwp_hwProfile_list */



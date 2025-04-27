

/* Default parameter */
#if (defined(CONFIG_SDK_RTL8295R) && defined(CONFIG_SDK_RTL8390))
phy_8295_pageRegVal_t rtl8295r_8390_s18431_default[] = {
    {957, 23, 0x0742},  /* 0x2F, 0xF  */
    {958, 16, 0x0541},  /* 0x2F, 0x10 */
    {PHY_8295_PAGEREGVAL_END, },
};

phy_8295_pageRegVal_t rtl8295r_8390_s1dacLong_default[] = {
    {957, 23, 0x0742},  /* 0x2F, 0xF  */
    {958, 16, 0x1700},  /* 0x2F, 0x10 */
    {PHY_8295_PAGEREGVAL_END, },
};


phy_8295_txParamCfg_t rtl8295r_8390_tx_param_cfg = {
    .fiber10g_8431_param.prv = rtl8295r_8390_s18431_default,
    .dac50cm_tx_param.prv    = rtl8295r_8390_s18431_default,
    .dac100cm_tx_param.prv   = rtl8295r_8390_s18431_default,
    .dac300cm_tx_param.prv   = rtl8295r_8390_s1dacLong_default,
    .dac500cm_tx_param.prv   = rtl8295r_8390_s1dacLong_default,
};


phy_8295_rxCaliConf_t rtl8295r_8390_rxCaliConf_default =
        {
            .dacLongCableOffset             = 3,

            .s1.rxCaliEnable                = ENABLED,
            .s1.tap0InitVal                 = 0x1F,
            .s1.vthMinThr                   = 0x2,
            .s1.eqHoldEnable                = ENABLED,
            .s1.dfeTap1_4Enable             = DISABLED,
            .s1.dfeAuto                     = DISABLED,

            .s0.rxCaliEnable                = ENABLED,
            .s0.tap0InitVal                 = 0x1F,
            .s0.vthMinThr                   = 0x2,
            .s0.eqHoldEnable                = ENABLED,
            .s0.dfeTap1_4Enable             = DISABLED,
            .s0.dfeAuto                     = DISABLED,
        };
#endif /* (defined(CONFIG_SDK_RTL8295R) && defined(CONFIG_SDK_RTL8390)) */

#if (defined(CONFIG_SDK_RTL8295R) && defined(CONFIG_SDK_RTL9300))
phy_8295_pageRegVal_t rtl8295r_9300_s18431_default[] = {
    {957, 23, 0x0742},  /* 0x2F, 0xF  */
    {958, 16, 0x0541},  /* 0x2F, 0x10 */
    {PHY_8295_PAGEREGVAL_END, },
};

phy_8295_pageRegVal_t rtl8295r_9300_s1dacLong_default[] = {
    {957, 23, 0x0742},  /* 0x2F, 0xF  */
    {958, 16, 0x1700},  /* 0x2F, 0x10 */
    {PHY_8295_PAGEREGVAL_END, },
};

phy_8295_txParamCfg_t rtl8295r_9300_tx_param_cfg = {
    .fiber10g_8431_param.prv = rtl8295r_9300_s18431_default,
    .dac50cm_tx_param.prv    = rtl8295r_9300_s18431_default,
    .dac100cm_tx_param.prv   = rtl8295r_9300_s18431_default,
    .dac300cm_tx_param.prv   = rtl8295r_9300_s1dacLong_default,
    .dac500cm_tx_param.prv   = rtl8295r_9300_s1dacLong_default,
};


phy_8295_pageRegVal_t rtl8295r_9300_s18431_default_CASCADE_unit1_port26[] = {
    {957, 23, 0x0342},  /* 1, 0x2F, 0xF  */
    {958, 16, 0x01C0},  /* 1, 0x2F, 0x10 */
    {388, 21, 0x438D},  /* 0, 0x21, 0x5 */
    {958, 22, 0x540C},  /* 1, 0x2F, 0x16 */
    {PHY_8295_PAGEREGVAL_END, },
};

phy_8295_pageRegVal_t rtl8295r_9300_s18431_default_CASCADE_unit1_port27[] = {
    {957, 23, 0x0342},  /* 1, 0x2F, 0xF  */
    {958, 16, 0x0181},  /* 1, 0x2F, 0x10 */
    {PHY_8295_PAGEREGVAL_END, },
};


phy_8295_pageRegVal_t rtl8295r_9300_s1dacLong_default_CASCADE_unit1_port26[] = {
    {957, 23, 0x0742},  /* 1, 0x2F, 0xF  */
    {958, 16, 0x1700},  /* 1, 0x2F, 0x10 */
    {388, 21, 0x420D},  /* 0, 0x21, 0x5 */
    {958, 22, 0x7C0C},  /* 1, 0x2F, 0x16 */
    {PHY_8295_PAGEREGVAL_END, },
};

phy_8295_pageRegVal_t rtl8295r_9300_s1dacLong_default_CASCADE_unit1_port27[] = {
    {957, 23, 0x0742},  /* 1, 0x2F, 0xF  */
    {958, 16, 0x1700},  /* 1, 0x2F, 0x10 */
    {PHY_8295_PAGEREGVAL_END, },
};


phy_8295_txParamCfg_t rtl8295r_93000_tx_param_cfg_CASCADE_unit1_port26 = {
    .fiber10g_8431_param.prv = rtl8295r_9300_s18431_default_CASCADE_unit1_port26,
    .dac50cm_tx_param.prv    = rtl8295r_9300_s18431_default_CASCADE_unit1_port26,
    .dac100cm_tx_param.prv   = rtl8295r_9300_s18431_default_CASCADE_unit1_port26,
    .dac300cm_tx_param.prv   = rtl8295r_9300_s1dacLong_default_CASCADE_unit1_port26,
    .dac500cm_tx_param.prv   = rtl8295r_9300_s1dacLong_default_CASCADE_unit1_port26,
};

phy_8295_txParamCfg_t rtl8295r_93000_tx_param_cfg_CASCADE_unit1_port27 = {
    .fiber10g_8431_param.prv = rtl8295r_9300_s18431_default_CASCADE_unit1_port27,
    .dac50cm_tx_param.prv    = rtl8295r_9300_s18431_default_CASCADE_unit1_port27,
    .dac100cm_tx_param.prv   = rtl8295r_9300_s18431_default_CASCADE_unit1_port27,
    .dac300cm_tx_param.prv   = rtl8295r_9300_s1dacLong_default_CASCADE_unit1_port27,
    .dac500cm_tx_param.prv   = rtl8295r_9300_s1dacLong_default_CASCADE_unit1_port27,
};

phy_8295_rxCaliConf_t rtl8295r_9300_rxCaliConf_default =
        {
            .dacLongCableOffset             = 3,

            .s1.rxCaliEnable                = ENABLED,
            .s1.tap0InitVal                 = 0x1F,
            .s1.vthMinThr                   = 0x2,
            .s1.eqHoldEnable                = ENABLED,
            .s1.dfeTap1_4Enable             = DISABLED,
            .s1.dfeAuto                     = DISABLED,

            .s0.rxCaliEnable                = ENABLED,
            .s0.tap0InitVal                 = 0x1F,
            .s0.vthMinThr                   = 0x2,
            .s0.eqHoldEnable                = ENABLED,
            .s0.dfeTap1_4Enable             = DISABLED,
            .s0.dfeAuto                     = DISABLED,
        };
#endif /* (defined(CONFIG_SDK_RTL8295R) && defined(CONFIG_SDK_RTL9300)) */


#if defined(CONFIG_SDK_RTL8295R)
phy_8295_pageRegVal_t rtl8295r_default_s18431[] = {
    {957, 23, 0x0742},  /* 0x2F, 0xF  */
    {958, 16, 0x0541},  /* 0x2F, 0x10 */
    {PHY_8295_PAGEREGVAL_END, },
};

phy_8295_pageRegVal_t rtl8295r_default_s1dacLong[] = {
    {957, 23, 0x0742},  /* 0x2F, 0xF  */
    {958, 16, 0x1700},  /* 0x2F, 0x10 */
    {PHY_8295_PAGEREGVAL_END, },
};

phy_8295_txParamCfg_t rtl8295r_default_tx_param_cfg = {
    .fiber10g_8431_param.prv = rtl8295r_default_s18431,
    .dac50cm_tx_param.prv    = rtl8295r_default_s18431,
    .dac100cm_tx_param.prv   = rtl8295r_default_s18431,
    .dac300cm_tx_param.prv   = rtl8295r_default_s1dacLong,
    .dac500cm_tx_param.prv   = rtl8295r_default_s1dacLong,
};

phy_8295_rxCaliConf_t rtl8295r_default_rxCaliConf =
        {
            .dacLongCableOffset             = 3,

            .s1.rxCaliEnable                = ENABLED,
            .s1.tap0InitVal                 = 0x1F,
            .s1.vthMinThr                   = 0x2,
            .s1.eqHoldEnable                = ENABLED,
            .s1.dfeTap1_4Enable             = DISABLED,
            .s1.dfeAuto                     = DISABLED,

            .s0.rxCaliEnable                = ENABLED,
            .s0.tap0InitVal                 = 0x1F,
            .s0.vthMinThr                   = 0x2,
            .s0.eqHoldEnable                = ENABLED,
            .s0.dfeTap1_4Enable             = DISABLED,
            .s0.dfeAuto                     = DISABLED,
        };
#endif /* defined(CONFIG_SDK_RTL8295R)*/





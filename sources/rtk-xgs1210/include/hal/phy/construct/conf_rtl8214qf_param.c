
#if (defined(CONFIG_SDK_RTL8214QF) && defined(CONFIG_SDK_RTL9300))
phy_8295_rxCaliConf_t rtl8214qf_9300_rxCaliConf_default =
        {
            .dacLongCableOffset             = 0,    /* not used for 8214QF */

            .s1.rxCaliEnable                = DISABLED,
            .s1.tap0InitVal                 = 0x1F,
            .s1.vthMinThr                   = 0x2,
            .s1.eqHoldEnable                = ENABLED,
            .s1.dfeTap1_4Enable             = DISABLED,
            .s1.dfeAuto                     = DISABLED,

            .s0.rxCaliEnable                = DISABLED,
            .s0.tap0InitVal                 = 0x1F,
            .s0.vthMinThr                   = 0x2,
            .s0.eqHoldEnable                = ENABLED,
            .s0.dfeTap1_4Enable             = DISABLED,
            .s0.dfeAuto                     = DISABLED,
        };
#endif


/*Customize parameter*/
#if (defined(CONFIG_SDK_RTL8214QF) && defined(CONFIG_SDK_RTL9300))
phy_8295_rxCaliConf_t rtl8214qf_9300_rxCaliConf_myParam =
        {
            .dacLongCableOffset             = 0,    /* not used for 8214QF */

            .s1.rxCaliEnable                = DISABLED,
            .s1.tap0InitVal                 = 0x1F,
            .s1.vthMinThr                   = 0x2,
            .s1.eqHoldEnable                = ENABLED,
            .s1.dfeTap1_4Enable             = DISABLED,
            .s1.dfeAuto                     = DISABLED,

            .s0.rxCaliEnable                = DISABLED,
            .s0.tap0InitVal                 = 0x1F,
            .s0.vthMinThr                   = 0x2,
            .s0.eqHoldEnable                = ENABLED,
            .s0.dfeTap1_4Enable             = DISABLED,
            .s0.dfeAuto                     = DISABLED,
        };
#endif


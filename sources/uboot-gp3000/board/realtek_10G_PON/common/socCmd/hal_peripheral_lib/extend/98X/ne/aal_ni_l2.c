#if 1//add for using assert
#include <common.h>
#endif

#include "ne_hw_init.h"
#include "osal_common.h"
#include "aal_common.h"
#include "aal_port.h"
#include "aal_ni_l2.h"
#include "scfg.h"

/******************************************************************************/
/*                             Import Variables and Functions                 */
/******************************************************************************/

ca_uint8_t wan_port_id = AAL_LPORT_ETH_NI0; // wan port id will be inited by ca_load_init() in ni_drv.

/******************************************************************************/
/*                             Local MACROs                                   */
/******************************************************************************/
/*
 * NI registers read/write MACROs
 */
#define __NI_PORT_REG_STRIDE                  (APB0_NI_HV_PT_STRIDE)
#define __NI_TBL_READ_FLAG                    (0)
#define __NI_TBL_WRITE_FLAG                   (1)

#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3LITE)
#define __GE_PORT_START  0
#define __GE_PORT_END    4
ca_uint8_t  dpid_mapping[] = {// index is src_port, value is dst_port
    1, // 0 -> 1
    2, // 1 -> 2
    3, // 2-> 3
    0, // 3-> 0
    0  // unused now
};
#elif defined(CONFIG_ARCH_CORTINA_SATURN_SFU) || defined(CONFIG_ARCH_CORTINA_G3HGU)
#define __GE_PORT_START  3
#define __GE_PORT_END    4
    ca_uint8_t  dpid_mapping[] = {// index is src_port, value is dst_port
        4, // 3 -> 4
        3  // 4 -> 3
    };
#else
//#error "not support this arch board config!"
#endif
#define __FOR_ALL_GE_DO(port)   \
                       for((port)=__GE_PORT_START; (port)<=__GE_PORT_END; ++(port))

#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
#define __XE_PORT_START 6
#define __XE_PORT_END   7
#elif defined(CONFIG_ARCH_CORTINA_G3LITE)
#define __XE_PORT_START AAL_PORT_INVALID
#define __XE_PORT_END   AAL_PORT_INVALID
#elif  defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
#define __XE_PORT_START 5
#define __XE_PORT_END   6
#else
//#error "not support this arch board config!"
#endif
#define __FOR_ALL_XE_DO(port)   \
                       for((port)=__XE_PORT_START; (port)<=__XE_PORT_END; ++(port))

#define __NI_RXMUX_FC_THRD_HI  130
#define __NI_RXMUX_FC_THRD_LO  10

#define __NI_PAUSE_QUANTA_DFT  0xFFFF

#define __NI_REG_READ(dev,reg_addr)           CA_NI_REG_READ(reg_addr)
#define __NI_REG_WRITE(dev,reg_addr,reg_val)  CA_NI_REG_WRITE(reg_val, reg_addr)

#define FOR_ALL_AAL_NI_PORT_DO(port) \
        for ((port) = AAL_PORT_NUM_FIRST_ETH; (port) < AAL_NI_PORT_NUM; ++(port))

#if 0
#define __NI_ASSERT(x)      AAL_ASSERT_RET(x, CA_E_PARAM)
#else
#define __NI_ASSERT(x)      assert(x)
#endif

/*
 * NI ASIC table access MACROs
 */
#if 0//Yocto
#define  __NI_TABLE_ACCESS_TIMEOUT 5000
#else//98F uboot @ FPGA
#define  __NI_TABLE_ACCESS_TIMEOUT 5000
#endif
#define  __NI_TABLE_ACTION_WAIT(device_id, tbl_name, acc_reg)   do {     \
                ca_uint32_t  timeout = 0;                           \
                do {                                                \
                    (acc_reg).wrd = __NI_REG_READ(device_id, tbl_name);       \
                    if (timeout++ > __NI_TABLE_ACCESS_TIMEOUT)      \
                        return CA_E_TIMEOUT;                        \
                }while(0 != (acc_reg).bf.access);                   \
            }while(0)

/*
 * NI resource check MACROs
 */
#define __NI_PORT_CHK(port)  do {                             \
                               if (port >= AAL_NI_PORT_NUM || port < AAL_PORT_NUM_FIRST_ETH) \
                                   return CA_E_PARAM;         \
                           } while(0)

#if 1//Yocto
#define  __NI_MC_TABLE_ACCESS(device_id,rw,tbl_name,address,acc_reg)         do {\
                (acc_reg).wrd        = 0;                                     \
                (acc_reg).bf.access  = 1;                                     \
                (acc_reg).bf.rbw     = (rw);                                  \
                (acc_reg).bf.addr = (address);                                \
                __NI_REG_WRITE(device_id, tbl_name, (acc_reg).wrd);                \
                __NI_TABLE_ACTION_WAIT(device_id, tbl_name, acc_reg);              \
            }while(0)

#define  __NI_MIB_TABLE_ACCESS(device_id,rw,tbl_name,port_id,mib_id,act,acc_reg)  do { \
                (acc_reg).wrd        = 0;                                           \
                (acc_reg).bf.access  = 1;                                           \
                (acc_reg).bf.rbw     = (rw);                                        \
                (acc_reg).bf.op_code = (act);                                       \
                (acc_reg).bf.port_id    = (port_id);                                \
                (acc_reg).bf.counter_id = (mib_id);                                 \
                __NI_REG_WRITE(device_id, tbl_name, (acc_reg).wrd);                      \
                __NI_TABLE_ACTION_WAIT(device_id, tbl_name, acc_reg);                    \
            }while(0)
#else
#define  __NI_MC_TABLE_ACCESS(device_id,rw,tbl_name,address,acc_reg)         do {\
                (acc_reg).wrd        = 0;                                     \
                (acc_reg).bf.access  = 1;                                     \
                (acc_reg).bf.rbw     = (rw);                                  \
                (acc_reg).bf.addr = (address);                                \
                __NI_REG_WRITE(device_id, tbl_name, (acc_reg).wrd);                \
            }while(0)

#define  __NI_MIB_TABLE_ACCESS(device_id,rw,tbl_name,port_id,mib_id,act,acc_reg)  do { \
                (acc_reg).wrd        = 0;                                           \
                (acc_reg).bf.access  = 1;                                           \
                (acc_reg).bf.rbw     = (rw);                                        \
                (acc_reg).bf.op_code = (act);                                       \
                (acc_reg).bf.port_id    = (port_id);                                \
                (acc_reg).bf.counter_id = (mib_id);                                 \
                __NI_REG_WRITE(device_id, tbl_name, (acc_reg).wrd);                      \
            }while(0)
#endif


static ca_status_t __ni_mem_init(ca_device_id_t device_id)
{
        NI_HV_GLB_INIT_DONE_t  init_done;
        ca_int32_t wait_loop = 5000;
#if 1//for 98F uboot @ FPGA
        NI_HV_GLB_INTF_RST_CONFIG_t  intf_rst_config;
#endif

        do {
                init_done.wrd = __NI_REG_READ(device_id, NI_HV_GLB_INIT_DONE);
                if (wait_loop-- <= 0) {
#if CONFIG_98F_UBOOT_NE_DBG
                        ca_printf("\t%s(%d)\tNI init time out!!\n", __FUNCTION__, __LINE__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
ca_printf("%s(%d)\t0x%08x\n", __FUNCTION__, __LINE__, __NI_REG_READ(device_id, NI_HV_GLB_INIT_DONE));
                        return CA_E_TIMEOUT;
                }
        } while (!init_done.bf.ni_init_done);

#if CONFIG_98F_UBOOT_NE_DBG
        ca_printf("\t%s(%d)\tNI init done!!\n", __FUNCTION__, __LINE__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

#if 1//for 98F uboot @ FPGA
        intf_rst_config.wrd = __NI_REG_READ(device_id, NI_HV_GLB_INTF_RST_CONFIG);

        /* toggle the reset pin */
        intf_rst_config.bf.intf_rst_p0 = 0;
        intf_rst_config.bf.mac_rx_rst_p0 = 0;
        intf_rst_config.bf.mac_tx_rst_p0 = 0;
        intf_rst_config.bf.intf_rst_p1 = 0;
        intf_rst_config.bf.mac_rx_rst_p1 = 0;
        intf_rst_config.bf.mac_tx_rst_p1 = 0;
        intf_rst_config.bf.intf_rst_p2 = 0;
        intf_rst_config.bf.mac_rx_rst_p2 = 0;
        intf_rst_config.bf.mac_tx_rst_p2 = 0;
        intf_rst_config.bf.intf_rst_p3 = 0;
        intf_rst_config.bf.mac_rx_rst_p3 = 0;
        intf_rst_config.bf.mac_tx_rst_p3 = 0;
        intf_rst_config.bf.intf_rst_p4 = 0;
        intf_rst_config.bf.mac_rx_rst_p4 = 0;
        intf_rst_config.bf.mac_tx_rst_p4 = 0;

        intf_rst_config.wrd = __NI_REG_READ(device_id, NI_HV_GLB_INTF_RST_CONFIG);

        __NI_REG_WRITE(device_id, NI_HV_GLB_INTF_RST_CONFIG, intf_rst_config.wrd);

#if CONFIG_98F_UBOOT_NE_DBG
        ca_printf("\t%s(%d)\tNI_HV_GLB_INTF_RST_CONFIG(0x%08x): 0x%08x\n", \
                __FUNCTION__, __LINE__, NI_HV_GLB_INTF_RST_CONFIG, \
                __NI_REG_READ(device_id, NI_HV_GLB_INTF_RST_CONFIG));
#endif /* CONFIG_98F_UBOOT_NE_DBG */
#endif

        return CA_E_OK;
}


ca_status_t aal_ni_port_intr_status_clear(/*AUTO-CLI gen ignore*/
    CA_IN ca_device_id_t              device_id,
    CA_IN ca_uint32_t             port_id,
    CA_IN aal_ni_port_intr_t      *status
)
{
#if defined(CONFIG_ARCH_CORTINA_G3LITE)

    NI_HV_GLB_PORT_0_INTERRUPT_t reg_data;

    __NI_PORT_CHK(port_id);
    __NI_ASSERT(status);

    reg_data.wrd = 0;
    reg_data.bf.link_stat_chgI             = status->link_stat_chg            ;
    reg_data.bf.txfifo_underrunI           = status->txfifo_underrun          ;
    reg_data.bf.txfifo_overrunI            = status->txfifo_overrun           ;
    reg_data.bf.rxcntrl_overrunI           = status->rxcntrl_overrun          ;
    reg_data.bf.rxmib_cntmsb_setI          = status->rxmib_cntmsb_set         ;
    reg_data.bf.txmib_cntmsb_setI          = status->txmib_cntmsb_set         ;
    reg_data.bf.lpi_assert_stat_chgI       = status->lpi_assert_stat_chg      ;
    reg_data.bf.rx_lpi_stat_chgI           = status->rx_lpi_stat_chg          ;
    reg_data.bf.ni_wol_int                 = status->ni_wol_int               ;
    if (device_id == 0)
        CA_NI_REG_WRITE(reg_data.wrd, NI_HV_GLB_PORT_0_INTERRUPT + (port_id * NI_HV_GLB_PORT_0_INTERRUPT_STRIDE));

#endif

    return CA_E_OK;

}


ca_status_t aal_ni_glb_intr_enable_set(
    CA_IN ca_device_id_t              device_id,
    CA_IN aal_ni_glb_intr_mask_t   mask,
    CA_IN aal_ni_glb_intr_t       *config
)
{
#if 0//Yocto original
    NI_HV_GLB_INTERRUPTE_t reg_data;

    __NI_ASSERT(config);

    reg_data.wrd = __NI_REG_READ(device_id, NI_HV_GLB_INTERRUPTE);

#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)|| defined(CONFIG_ARCH_CORTINA_G3LITE)
    if (mask.s.int_stat_pspid_0     )  reg_data.bf.int_stat_pspid_0E         = config->int_stat_pspid_0     ;
    if (mask.s.int_stat_pspid_1     )  reg_data.bf.int_stat_pspid_1E         = config->int_stat_pspid_1     ;
    if (mask.s.int_stat_pspid_2     )  reg_data.bf.int_stat_pspid_2E         = config->int_stat_pspid_2     ;
#endif
    if (mask.s.int_stat_pspid_3     )  reg_data.bf.int_stat_pspid_3E         = config->int_stat_pspid_3     ;
    if (mask.s.int_stat_pspid_4     )  reg_data.bf.int_stat_pspid_4E         = config->int_stat_pspid_4     ;
    if (mask.s.int_stat_pspid_8     )  reg_data.bf.int_stat_pspid_8E         = config->int_stat_pspid_8     ;
    if (mask.s.int_stat_pspid_9     )  reg_data.bf.int_stat_pspid_9E         = config->int_stat_pspid_9     ;
    if (mask.s.int_stat_pspid_10    )  reg_data.bf.int_stat_pspid_10E        = config->int_stat_pspid_10    ;
    if (mask.s.int_stat_txem        )  reg_data.bf.int_stat_txemE            = config->int_stat_txem        ;
#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)|| defined(CONFIG_ARCH_CORTINA_G3LITE)
    if (mask.s.int_stat_pc0         )  reg_data.bf.int_stat_pc0E             = config->int_stat_pc0         ;
    if (mask.s.int_stat_pc1         )  reg_data.bf.int_stat_pc1E             = config->int_stat_pc1         ;
    if (mask.s.int_stat_pc2         )  reg_data.bf.int_stat_pc2E             = config->int_stat_pc2         ;
#endif
    if (mask.s.int_stat_pc3         )  reg_data.bf.int_stat_pc3E             = config->int_stat_pc3         ;
    if (mask.s.int_stat_pc4         )  reg_data.bf.int_stat_pc4E             = config->int_stat_pc4         ;
    if (mask.s.cpuxram_int_stat_cntr)  reg_data.bf.cpuxram_int_stat_cntrE    = config->cpuxram_int_stat_cntr;
    if (mask.s.cpuxram_int_stat_err )  reg_data.bf.cpuxram_int_stat_errE     = config->cpuxram_int_stat_err ;
    if (mask.s.int_stat_ptp         )  reg_data.bf.int_stat_ptpE             = config->int_stat_ptp         ;
    if (mask.s.int_stat_mceng       )  reg_data.bf.int_stat_mcengE           = config->int_stat_mceng       ;
    if (mask.s.cpuxram_rxpkt        )  reg_data.bf.cpuxram_rxpktE            = config->cpuxram_rxpkt        ;
    if (mask.s.cpuxram_txpkt        )  reg_data.bf.cpuxram_txpktE            = config->cpuxram_txpkt        ;

    __NI_REG_WRITE(device_id, NI_HV_GLB_INTERRUPTE, reg_data.wrd);

#else
#endif
    return CA_E_OK;

}



ca_status_t aal_ni_port_intr_enable_set(
    CA_IN ca_device_id_t              device_id,
    CA_IN ca_uint32_t              port_id,
    CA_IN aal_ni_port_intr_mask_t  mask,
    CA_IN aal_ni_port_intr_t      *config
)
{
#if defined(CONFIG_ARCH_CORTINA_G3LITE)

    NI_HV_GLB_PORT_0_INTERRUPTE_t reg_data;

    __NI_PORT_CHK(port_id);

    if (device_id == 0)
        reg_data.wrd = CA_NI_REG_READ(NI_HV_GLB_PORT_0_INTERRUPTE + (port_id * NI_HV_GLB_PORT_0_INTERRUPTE_STRIDE));

    if (mask.s.link_stat_chg             )
        reg_data.bf.link_stat_chgIE             = config->link_stat_chg            ;
    if (mask.s.txfifo_underrun           )
        reg_data.bf.txfifo_underrunIE           = config->txfifo_underrun          ;
    if (mask.s.txfifo_overrun            )
        reg_data.bf.txfifo_overrunIE            = config->txfifo_overrun           ;
    if (mask.s.rxcntrl_overrun           )
        reg_data.bf.rxcntrl_overrunIE           = config->rxcntrl_overrun          ;
    if (mask.s.rxmib_cntmsb_set          )
        reg_data.bf.rxmib_cntmsb_setIE          = config->rxmib_cntmsb_set         ;
    if (mask.s.txmib_cntmsb_set          )
        reg_data.bf.txmib_cntmsb_setIE          = config->txmib_cntmsb_set         ;
    if (mask.s.lpi_assert_stat_chg       )
        reg_data.bf.lpi_assert_stat_chgIE       = config->lpi_assert_stat_chg      ;
    if (mask.s.rx_lpi_stat_chg           )
        reg_data.bf.rx_lpi_stat_chgIE           = config->rx_lpi_stat_chg          ;
    if (mask.s.ni_wol_int                )
        reg_data.bf.ni_wol_intE                 = config->ni_wol_int               ;
    if (device_id == 0)
        CA_NI_REG_WRITE(reg_data.wrd, NI_HV_GLB_PORT_0_INTERRUPTE + (port_id * NI_HV_GLB_PORT_0_INTERRUPTE_STRIDE));
#endif

    return CA_E_OK;

}



ca_status_t aal_ni_pkt_len_set(
    CA_IN ca_device_id_t                   device_id,
    CA_IN aal_ni_pkt_len_config_mask_t  mask,
    CA_IN aal_ni_pkt_len_config_t      *config
)
{
    NI_HV_GLB_PKT_LEN_CFG_t       reg_data1;
    NI_HV_GLB_PKT_STOR_LEN_CFG_t  reg_data2;

    __NI_ASSERT(config);

    if (0 == mask.u32) {
        return CA_E_OK;
    }

    if ((mask.s.min_pkt_size &&
        (config->min_pkt_size < AAL_NI_PKT_LEN_MIN ||
         config->min_pkt_size > AAL_NI_PKT_LEN_MAX)) ||
        (mask.s.max_pkt_size &&
        (config->max_pkt_size < AAL_NI_PKT_LEN_MIN ||
         config->max_pkt_size > AAL_NI_PKT_LEN_MAX))) {
        return CA_E_PARAM;
    }

    /* MIN pkt size config should be less-equal than MAX pkt size */
    if (mask.s.min_pkt_size &&
        mask.s.max_pkt_size &&
        config->min_pkt_size > config->max_pkt_size) {

        return CA_E_PARAM;
    }

    if (mask.s.max_store_pkt_len &&
        config->max_store_pkt_len > AAL_NI_PKT_LEN_MAX) {
        return CA_E_PARAM;
    }

    reg_data1.wrd = __NI_REG_READ(device_id, NI_HV_GLB_PKT_LEN_CFG);
    reg_data2.wrd = __NI_REG_READ(device_id, NI_HV_GLB_PKT_STOR_LEN_CFG);

    if (mask.s.min_pkt_size){
        reg_data1.bf.min_pkt_size = config->min_pkt_size;
    }

    if (mask.s.max_pkt_size)  {
        reg_data1.bf.max_pkt_size = config->max_pkt_size;
    }

    if (mask.s.min_pkt_size || mask.s.max_pkt_size) {
        __NI_REG_WRITE(device_id, NI_HV_GLB_PKT_LEN_CFG, reg_data1.wrd);
    }

    if (mask.s.max_store_pkt_len){
        reg_data2.bf.max_stor_pkt_len = config->max_store_pkt_len;
        __NI_REG_WRITE(device_id, NI_HV_GLB_PKT_LEN_CFG, reg_data2.wrd);
    }

#if CONFIG_98F_UBOOT_NE_DBG
        ca_printf("\t%s(%d)\tNI_HV_GLB_PKT_LEN_CFG(0x%08x): 0x%08x\n", \
                __FUNCTION__, __LINE__, NI_HV_GLB_PKT_LEN_CFG, \
                __NI_REG_READ(device_id, NI_HV_GLB_PKT_LEN_CFG));
        ca_printf("\t%s(%d)\tNI_HV_GLB_PKT_STOR_LEN_CFG(0x%08x): 0x%08x\n", \
                __FUNCTION__, __LINE__, NI_HV_GLB_PKT_STOR_LEN_CFG, \
                __NI_REG_READ(device_id, NI_HV_GLB_PKT_STOR_LEN_CFG));
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    return CA_E_OK;

}


ca_status_t aal_ni_mc_lkup_set(
    CA_IN  ca_device_id_t                 device_id,
    CA_IN  ca_uint32_t                 mc_group,
    CA_IN  ca_uint64_t                 port_bmp
)
{
        NI_HV_MCE_INDX_LKUP_ACCESS_t  table_access;
        NI_HV_MCE_INDX_LKUP_DATA0_t   table_data;
        NI_HV_MCE_INDX_LKUP_DATA1_t   table_data1;

        if (mc_group >= AAL_NI_MC_GROUP_NUM) {
                return CA_E_PARAM;
        }
#if 0//yocto
#if 0
        __NI_MC_TABLE_ACCESS(device_id,__NI_TBL_READ_FLAG,
                NI_HV_MCE_INDX_LKUP_ACCESS,
                mc_group, table_access);

        table_data.wrd = __NI_REG_READ(device_id, NI_HV_MCE_INDX_LKUP_DATA);
#endif
#endif
        table_data.wrd = 0;
        table_data.bf.mc_vec  = port_bmp;
        table_data1.wrd = 0;
        table_data1.bf.mc_vec = port_bmp >> 32;

        __NI_REG_WRITE(device_id, NI_HV_MCE_INDX_LKUP_DATA0, table_data.wrd);
        __NI_REG_WRITE(device_id, NI_HV_MCE_INDX_LKUP_DATA1, table_data1.wrd);

        __NI_MC_TABLE_ACCESS(device_id, __NI_TBL_WRITE_FLAG,
                NI_HV_MCE_INDX_LKUP_ACCESS,
                mc_group, table_access);

#if 0//CONFIG_98F_UBOOT_NE_DBG//too many message
__NI_MC_TABLE_ACCESS(device_id, __NI_TBL_READ_FLAG,
        NI_HV_MCE_INDX_LKUP_ACCESS,
        mc_group, table_access);

ca_printf("\t%s(%d)\tmc_group: 0x%x\n", \
        __func__, __LINE__, mc_group);

ca_printf("\t%s(%d)\tNI_HV_MCE_INDX_LKUP_ACCESS(0x%08x): 0x%08x\n", \
        __func__, __LINE__, NI_HV_MCE_INDX_LKUP_ACCESS, \
        __NI_REG_READ(device_id, NI_HV_MCE_INDX_LKUP_ACCESS));

ca_printf("\t%s(%d)\tNI_HV_MCE_INDX_LKUP_DATA0(0x%08x): 0x%08x\n", \
        __func__, __LINE__, NI_HV_MCE_INDX_LKUP_DATA0, \
        __NI_REG_READ(device_id, NI_HV_MCE_INDX_LKUP_DATA0));

ca_printf("\t%s(%d)\tNI_HV_MCE_INDX_LKUP_DATA1(0x%08x): 0x%08x\n", \
        __func__, __LINE__, NI_HV_MCE_INDX_LKUP_DATA1, \
        __NI_REG_READ(device_id, NI_HV_MCE_INDX_LKUP_DATA1));
#endif /* CONFIG_98F_UBOOT_NE_DBG */

        return CA_E_OK;

}


ca_status_t aal_ni_port_rx_ctrl_set(
    CA_IN ca_device_id_t                  device_id,
    CA_IN ca_uint32_t                 port_id,
    CA_IN aal_ni_port_rx_ctrl_mask_t   mask,
    CA_IN aal_ni_port_rx_ctrl_t       *config
)
{
    NI_HV_PT_RX_CNTRL_CFG_t     reg_cfg;

    __NI_ASSERT(config);
    __NI_PORT_CHK(port_id);

    reg_cfg.wrd = __NI_REG_READ(device_id, NI_HV_PT_RX_CNTRL_CFG + port_id * __NI_PORT_REG_STRIDE);

#if CONFIG_98F_UBOOT_NE_DBG
ca_printf("\t%s(%d)\t[before] NI_HV_PT_RX_CNTRL_CFG(0x%08x): 0x%08x\n", \
        __FUNCTION__, __LINE__, \
        NI_HV_PT_RX_CNTRL_CFG + port_id * __NI_PORT_REG_STRIDE, \
        reg_cfg.wrd);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    if (mask.s.ff_overrun_drop_dis)
        reg_cfg.bf.ff_overrun_drop_dis = config->ff_overrun_drop_dis;

    if (mask.s.crc_drop_dis)
        reg_cfg.bf.crc_drop_dis = config->crc_drop_dis;

    if (mask.s.ovr_drop_dis)
        reg_cfg.bf.ovr_drop_dis = config->ovr_drop_dis;

    if (mask.s.runt_drop_dis)
        reg_cfg.bf.runt_drop_dis = config->runt_drop_dis;

    if (mask.s.runt_drop_dis)
        reg_cfg.bf.runt_drop_dis = config->runt_drop_dis;

    if (mask.s.ukopcode_drop_dis)
        reg_cfg.bf.ukopcode_drop_dis = config->ukopcode_drop_dis;

    if (mask.s.oam_drop_dis)
        reg_cfg.bf.oam_drop_dis = config->oam_drop_dis;

    if (mask.s.linkstat_drop_dis)
        reg_cfg.bf.linkstat_drop_dis = config->linkstat_drop_dis;

    if (mask.s.pfc_drop_dis)
        reg_cfg.bf.pfc_drop_dis = config->pfc_drop_dis;

    if (mask.s.rxctrl_byp_dpid)
        reg_cfg.bf.rxctrl_byp_dpid = config->rxctrl_byp_dpid;

    if (mask.s.rxctrl_byp_cos)
        reg_cfg.bf.rxctrl_byp_cos = config->rxctrl_byp_cos;

    if (mask.s.rxctrl_byp_en)
        reg_cfg.bf.rxctrl_byp_en = config->rxctrl_byp_en;

    __NI_REG_WRITE(device_id, NI_HV_PT_RX_CNTRL_CFG + port_id * __NI_PORT_REG_STRIDE, reg_cfg.wrd);

#if CONFIG_98F_UBOOT_NE_DBG
        ca_printf("\t%s(%d)\t[after] NI_HV_PT_RX_CNTRL_CFG(0x%08x): 0x%08x\n", \
                __FUNCTION__, __LINE__, NI_HV_PT_RX_CNTRL_CFG + port_id * __NI_PORT_REG_STRIDE, \
                __NI_REG_READ(device_id, NI_HV_PT_RX_CNTRL_CFG + port_id * __NI_PORT_REG_STRIDE));
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    return CA_E_OK;

}



ca_status_t aal_ni_xge_flowctrl_set(
    CA_IN  ca_device_id_t                device_id,
    CA_IN  ca_uint32_t                port_id,
    CA_IN  aal_ni_xge_flowctrl_mask_t mask,
    CA_IN  aal_ni_xge_flowctrl_t     *config)
{
#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
    NI_HV_GLB_XGE6_FLOW_CONTROL_CFG_t   fc_cfg;//no such reg. for 98F

    AAL_ASSERT_RET(config, CA_E_PARAM);
    AAL_ASSERT_RET(port_id < NI_HV_GLB_XGE6_FLOW_CONTROL_CFG_COUNT, CA_E_PARAM);

    fc_cfg.wrd = CA_NI_REG_READ(NI_HV_GLB_XGE6_FLOW_CONTROL_CFG + port_id * NI_HV_GLB_XGE6_FLOW_CONTROL_CFG_STRIDE );

    if (mask.s.qbb_tx_en          )
        fc_cfg.bf.qbb_tx_en           = config->qbb_tx_en          ;
    if (mask.s.qbb_rx_en          )
        fc_cfg.bf.qbb_rx_en           = config->qbb_rx_en          ;
    if (mask.s.pau_tx_en          )
        fc_cfg.bf.pau_tx_en           = config->pau_tx_en          ;
    if (mask.s.pau_rx_en          )
        fc_cfg.bf.pau_rx_en           = config->pau_rx_en          ;
    if (mask.s.adjust_pause_cnt   )
        fc_cfg.bf.adjust_pause_cnt    = config->adjust_pause_cnt   ;
    if (mask.s.qbb_qen            )
        fc_cfg.bf.qbb_qen             = config->qbb_qen            ;
    if (mask.s.rx_drp_pau_en      )
        fc_cfg.bf.rx_drp_pau_en       = config->rx_drp_pau_en      ;
    if (mask.s.internal_txstop_sel)
        fc_cfg.bf.internal_txstop_sel = config->internal_txstop_sel;
    if (mask.s.tx_remheader_dis   )
        fc_cfg.bf.tx_remheader_dis    = config->tx_remheader_dis   ;
    if (mask.s.rx_pausereq_sel    )
        fc_cfg.bf.rx_pausereq_sel     = config->rx_pausereq_sel    ;
    if (mask.s.te_pausereq_sel    )
        fc_cfg.bf.te_pausereq_sel     = config->te_pausereq_sel    ;

    if (mask.u32)
        CA_NI_REG_WRITE(fc_cfg.wrd, NI_HV_GLB_XGE6_FLOW_CONTROL_CFG + port_id * NI_HV_GLB_XGE6_FLOW_CONTROL_CFG_STRIDE);
#endif

    return CA_E_OK;
}


ca_status_t aal_ni_xge_rx_config_set(
    CA_IN ca_device_id_t                device_id,
    CA_IN ca_uint32_t                port_id,
    CA_IN aal_ni_xge_cfg_rx_mask_t   mask,
    CA_IN aal_ni_xge_cfg_rx_t       *config)
{
#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU) || defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    NI_HV_XGE_MAC_CFG_RX_t  cfg_data;//no such reg. for 98F

    AAL_ASSERT_RET(config, CA_E_PARAM);
    AAL_ASSERT_RET(mask.u32, CA_E_OK);
    __XGE_PORT_CHK(port_id);

    cfg_data.wrd = __NI_REG_READ(device_id,
        NI_HV_XGE_MAC_CFG_RX + port_id * __NI_XGE_REG_STRIDE);

    if (mask.s.promis_mode       )
        cfg_data.bf.promis_mode        = config->promis_mode       ;
    if (mask.s.drop_short        )
        cfg_data.bf.drop_short         = config->drop_short        ;
    if (mask.s.strip             )
        cfg_data.bf.strip              = config->strip             ;
    if (mask.s.pause_term        )
        cfg_data.bf.pause_term         = config->pause_term        ;
    if (mask.s.pause_extract     )
        cfg_data.bf.pause_extract      = config->pause_extract     ;
    if (mask.s.len_chk_en        )
        cfg_data.bf.len_chk_en         = config->len_chk_en        ;
    if (mask.s.len_chk_en_pause  )
        cfg_data.bf.len_chk_en_pause   = config->len_chk_en_pause  ;
    if (mask.s.len_chk_en_pad    )
        cfg_data.bf.len_chk_en_pad     = config->len_chk_en_pad    ;
    if (mask.s.fcs_chk_en        )
        cfg_data.bf.fcs_chk_en         = config->fcs_chk_en        ;
    if (mask.s.ifg_mode_rx       )
        cfg_data.bf.IFG_mode_rx        = config->ifg_mode_rx       ;
    if (mask.s.crc_err_fatal     )
        cfg_data.bf.crc_err_fatal      = config->crc_err_fatal     ;
    if (mask.s.maxlen_mode       )
        cfg_data.bf.maxlen_mode        = config->maxlen_mode       ;
    if (mask.s.prmbl_sfd_gmii_err)
        cfg_data.bf.prmbl_sfd_gmii_err = config->prmbl_sfd_gmii_err;

    __NI_REG_WRITE(device_id,
        NI_HV_XGE_MAC_CFG_RX + port_id * __NI_XGE_REG_STRIDE, cfg_data.wrd);
#endif

    return CA_E_OK;

}


ca_status_t aal_ni_xge_tx_config_set(
    CA_IN ca_device_id_t                device_id,
    CA_IN ca_uint32_t                port_id,
    CA_IN aal_ni_xge_cfg_tx_mask_t   mask,
    CA_IN aal_ni_xge_cfg_tx_t       *config)
{
#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU) || defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    NI_HV_XGE_MAC_CFG_TX_t      cfg1;//no such reg. for 98F
    NI_HV_XGE_MAC_CFG_TX_2_t    cfg2;//no such reg. for 98F
    NI_HV_XGE_MAC_CFG_TX_IFG_t  cfg3;//no such reg. for 98F

    AAL_ASSERT_RET(config, CA_E_PARAM);
    AAL_ASSERT_RET(mask.u32, CA_E_OK);
    __XGE_PORT_CHK(port_id);

    cfg1.wrd = __NI_REG_READ(device_id, NI_HV_XGE_MAC_CFG_TX + port_id * __NI_XGE_REG_STRIDE);
    cfg2.wrd = __NI_REG_READ(device_id, NI_HV_XGE_MAC_CFG_TX_2 + port_id * __NI_XGE_REG_STRIDE);
    cfg3.wrd = __NI_REG_READ(device_id, NI_HV_XGE_MAC_CFG_TX_IFG + port_id * __NI_XGE_REG_STRIDE);

    if (mask.s.vlan_pad_mode     )
        cfg1.bf.vlan_pad_mode     = config->vlan_pad_mode     ;
    if (mask.s.lf_auto_flush_en  )
        cfg1.bf.lf_auto_flush_en  = config->lf_auto_flush_en  ;
    if (mask.s.pause_req_auto_xon)
        cfg1.bf.pause_req_auto_xon= config->pause_req_auto_xon;
    if (mask.s.pause_req_en      )
        cfg1.bf.pause_req_en      = config->pause_req_en      ;
    if (mask.s.insert_sa             )
        cfg1.bf.insSA             = config->insert_sa         ;
    if (mask.s.halt              )
        cfg1.bf.halt              = config->halt              ;
    if (mask.s.ifg_mode_tx       )
        cfg1.bf.IFG_mode_tx       = config->ifg_mode_tx       ;
    if (mask.s.fifo1_read_thr    )
        cfg2.bf.fifo1_read_thr    = config->fifo1_read_thr    ;
    if (mask.s.fifo1_thr_afull   )
        cfg2.bf.fifo1_thr_afull   = config->fifo1_thr_afull   ;
    if (mask.s.ifg               )
        cfg3.bf.ifg               = config->ifg               ;

    __NI_REG_WRITE(device_id, NI_HV_XGE_MAC_CFG_TX + port_id * __NI_XGE_REG_STRIDE, cfg1.wrd);
    __NI_REG_WRITE(device_id, NI_HV_XGE_MAC_CFG_TX_2 + port_id * __NI_XGE_REG_STRIDE, cfg2.wrd);
    __NI_REG_WRITE(device_id, NI_HV_XGE_MAC_CFG_TX_IFG + port_id * __NI_XGE_REG_STRIDE, cfg3.wrd);
#endif

    return CA_E_OK;
}


ca_status_t aal_ni_xaui_mac_tx_ctrl_set(
    CA_IN  ca_device_id_t                     device_id,
    CA_IN  aal_ni_xaui_mac_tx_ctrl_mask_t  mask,
    CA_IN  aal_ni_xaui_mac_tx_ctrl_t      *config)
{
#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
    NI_HV_XAUI_XAUI_MAC_CTRL_XAUI_TX_CTL_t   cfg_data;//no such reg. for 98F

    AAL_ASSERT_RET(config, CA_E_PARAM);
    AAL_ASSERT_RET(mask.u32, CA_E_OK);

    cfg_data.wrd = CA_NI_REG_READ( NI_HV_XAUI_XAUI_MAC_CTRL_XAUI_TX_CTL);

    if (mask.s.tx_remheader_dis     )
        cfg_data.bf.tx_remheader_dis      = config->tx_remheader_dis      ;
    if (mask.s.tx_pkt_clr           )
        cfg_data.bf.tx_pkt_clr            = config->tx_pkt_clr            ;
    if (mask.s.tx_pad_en            )
        cfg_data.bf.tx_pad_en             = config->tx_pad_en             ;
    if (mask.s.loopback             )
        cfg_data.bf.loopback              = config->loopback              ;
    if (mask.s.ptp_eg_ts_comp       )
        cfg_data.bf.ptp_eg_ts_comp        = config->ptp_eg_ts_comp        ;
    if (mask.s.external_pausereq_pol)
        cfg_data.bf.external_pausereq_pol = config->external_pausereq_pol ;
    if (mask.s.qbb_flow_en          )
        cfg_data.bf.qbb_flow_en           = config->qbb_flow_en           ;
    if (mask.s.qbb_qen              )
        cfg_data.bf.qbb_qen               = config->qbb_qen               ;
    if (mask.s.external_txstop_sel  )
        cfg_data.bf.external_txstop_sel   = config->external_txstop_sel   ;
    if (mask.s.internal_txstop_sel  )
        cfg_data.bf.internal_txstop_sel   = config->internal_txstop_sel   ;
    if (mask.s.external_pausereq_sel)
        cfg_data.bf.external_pausereq_sel = config->external_pausereq_sel ;
    if (mask.s.rx_pausereq_sel      )
        cfg_data.bf.rx_pausereq_sel       = config->rx_pausereq_sel       ;

    CA_NI_REG_WRITE( cfg_data.wrd, NI_HV_XAUI_XAUI_MAC_CTRL_XAUI_TX_CTL);
#endif

    return CA_E_OK;
}


ca_status_t aal_ni_rxmux_fc_thrshld_set(
    CA_IN  ca_device_id_t                      device_id,
    CA_IN  ca_uint32_t                      port_id,
    CA_IN  aal_ni_rxmux_fc_thrshld_mask_t   mask,
    CA_IN  aal_ni_rxmux_fc_thrshld_t        *cfg
)
{
    NI_HV_GLB_RXMUX_PORT0_FC_THRESHOLD_CFG_t fc_thrshld;

    __NI_ASSERT(port_id < NI_HV_GLB_RXMUX_PORT0_FC_THRESHOLD_CFG_COUNT);

    __NI_ASSERT(cfg);

    fc_thrshld.wrd = __NI_REG_READ(device_id,
        NI_HV_GLB_RXMUX_PORT0_FC_THRESHOLD_CFG + (port_id *NI_HV_GLB_RXMUX_PORT0_FC_THRESHOLD_CFG_STRIDE));

    if(mask.s.hi_threshold){
        fc_thrshld.bf.thre_hi = cfg->hi_threshold;
    }

    if(mask.s.low_threshold){
        fc_thrshld.bf.thre_lo = cfg->low_threshold;
    }

    __NI_REG_WRITE(device_id,
        NI_HV_GLB_RXMUX_PORT0_FC_THRESHOLD_CFG + (port_id * NI_HV_GLB_RXMUX_PORT0_FC_THRESHOLD_CFG_STRIDE),
        fc_thrshld.wrd);

    return CA_E_OK;
}


ca_status_t aal_ni_eth_if_set(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint32_t                   port_id,
    CA_IN aal_ni_eth_if_config_mask_t   mask,
    CA_IN aal_ni_eth_if_config_t       *config
)
{
    NI_HV_PT_PORT_STATIC_CFG_t  reg_data;

    __NI_ASSERT(config);
    __NI_PORT_CHK(port_id);

    reg_data.wrd = __NI_REG_READ(device_id,  NI_HV_PT_PORT_STATIC_CFG + port_id * __NI_PORT_REG_STRIDE);

    if (mask.s.int_cfg               ) reg_data.bf.int_cfg                = config->int_cfg               ;
    if (mask.s.phy_mode              ) reg_data.bf.phy_mode               = config->phy_mode              ;
    if (mask.s.rmii_clksrc           ) reg_data.bf.rmii_clksrc            = config->rmii_clksrc           ;
    if (mask.s.inv_clk_in            ) reg_data.bf.inv_clk_in             = config->inv_clk_in            ;
    if (mask.s.inv_clk_out           ) reg_data.bf.inv_clk_out            = config->inv_clk_out           ;
    if (mask.s.inv_rxclk_out         ) reg_data.bf.inv_rxclk_out          = config->inv_rxclk_out         ;
    if (mask.s.tx_use_gefifo         ) reg_data.bf.tx_use_gefifo          = config->tx_use_gefifo         ;
    if (mask.s.smii_tx_stat          ) reg_data.bf.smii_tx_stat           = config->smii_tx_stat          ;
    if (mask.s.crs_polarity          ) reg_data.bf.crs_polarity           = config->crs_polarity          ;
    if (mask.s.lpbk_mode             ) reg_data.bf.lpbk_mode              = config->lpbk_mode             ;
    if (mask.s.sup_tx_to_rx_lpbk_data) reg_data.bf.sup_tx_to_rx_lpbk_data = config->sup_tx_to_rx_lpbk_data;
    if (mask.s.mac_addr6             ) reg_data.bf.mac_addr6              = config->mac_addr6             ;

    __NI_REG_WRITE(device_id,  NI_HV_PT_PORT_STATIC_CFG + port_id * __NI_PORT_REG_STRIDE, reg_data.wrd);

    return CA_E_OK;

}


ca_status_t aal_ni_eth_pfc_quanta_set(
    CA_IN ca_device_id_t    device_id,
    CA_IN ca_uint32_t    port_id,
    CA_IN ca_uint8_t     cos,
    CA_IN ca_uint16_t    quanta)
{
    NI_HV_PT_ETH_TXMAC_PAUSE_TIMER_CFG0_t reg_cfg0;
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
     __NI_PORT_CHK(port_id);
#else
    __NI_ASSERT(port_id < AAL_NI_PORT_NUM);
#endif
    __NI_ASSERT(cos <= 7);

    reg_cfg0.wrd = __NI_REG_READ(device_id,
        NI_HV_PT_ETH_TXMAC_PAUSE_TIMER_CFG0 +
        (port_id * __NI_PORT_REG_STRIDE) +
        ((cos >> 1) << 2));

    if (cos & 0x1) {
        reg_cfg0.bf.txpfc_time_1 = quanta;
    } else {
        reg_cfg0.bf.txpfc_time_0 = quanta;
    }

    __NI_REG_WRITE(device_id,
        NI_HV_PT_ETH_TXMAC_PAUSE_TIMER_CFG0 +
        (port_id * __NI_PORT_REG_STRIDE) +
        ((cos >> 1) << 2), reg_cfg0.wrd);

    return CA_E_OK;
}


ca_status_t aal_ni_xge_pause_quanta_set(
    CA_IN ca_device_id_t    device_id,
    CA_IN ca_uint32_t    port_id,
    CA_IN ca_uint16_t    quanta)
{
#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU) || defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    NI_HV_XGE_MAC_PQUANTA_t  cfg_data;//no such reg. for 98F

    __XGE_PORT_CHK(port_id);

    cfg_data.wrd = 0;
    cfg_data.bf.pq = quanta;

    __NI_REG_WRITE(device_id,
        NI_HV_XGE_MAC_PQUANTA + port_id *__NI_XGE_REG_STRIDE, cfg_data.wrd);
#endif

    return CA_E_OK;
}

static void aal_ni_eth_port_config_set(ca_device_id_t device_id, ca_uint32_t port_id, ca_uint32_t speed)
{
    ca_uint8_t phy_intfs[CFG_ID_PHY_INTERFACE_LEN];
    NI_HV_PT_PORT_STATIC_CFG_t port_static_cfg;

#if 0//98f luna
    aal_scfg_read(CFG_ID_PHY_INTERFACE, CFG_ID_PHY_INTERFACE_LEN, &(phy_intfs[0]));
#else//sd1 uboot for 98f
    phy_intfs[0] = 0x01;
    phy_intfs[1] = 0x01;
    phy_intfs[2] = 0x01;
    phy_intfs[3] = 0x01;
    phy_intfs[4] = 0x01;
    phy_intfs[5] = 0x03;
    phy_intfs[6] = 0x03;
#endif//sd1 uboot for 98f
        port_static_cfg.wrd = __NI_REG_READ(device_id, NI_HV_PT_PORT_STATIC_CFG + port_id * __NI_PORT_REG_STRIDE);

#if 0//CONFIG_98F_UBOOT_NE_DBG
ca_uint32 readReg = NI_HV_PT_PORT_STATIC_CFG + port_id * __NI_PORT_REG_STRIDE;
ca_printf("\t%s(%d)\t read %#x: 0x%08x\n", __FUNCTION__, __LINE__, readReg, __NI_REG_READ(0, readReg));
#endif /* CONFIG_98F_UBOOT_NE_DBG */

#if defined(CONFIG_RTL_83XX_SUPPORT)
    if (__ETH_IS_CONNECT_SWITCH(port_id)) { /* Connect to RTL switch chipset */
        phy_intfs[port_id] = AAL_NI_IF_MODE_GE_RGMII;
    }		
#endif

    if (phy_intfs[port_id] == AAL_NI_IF_MODE_GE_RGMII || phy_intfs[port_id] == AAL_NI_IF_MODE_FE_RGMII) {
        if (speed == AAL_NI_MAC_SPEED_1000M) {
            port_static_cfg.bf.int_cfg = AAL_NI_IF_MODE_GE_RGMII;
        }
        else {
            port_static_cfg.bf.int_cfg = AAL_NI_IF_MODE_FE_RGMII;
        }
    }
    else {
        if (speed == AAL_NI_MAC_SPEED_1000M) {
            port_static_cfg.bf.int_cfg = AAL_NI_IF_MODE_GE_GMII;
        }
        else {
            port_static_cfg.bf.int_cfg = AAL_NI_IF_MODE_FE_MII;
        }
    }

        __NI_REG_WRITE(device_id, NI_HV_PT_PORT_STATIC_CFG + port_id * __NI_PORT_REG_STRIDE, port_static_cfg.wrd);
#if CONFIG_98F_UBOOT_NE_DBG
    printf("\t%s(%d)\t\t read %#x: 0x%08x\n", __FUNCTION__, __LINE__, readReg, __NI_REG_READ(0, readReg));
#endif /* CONFIG_98F_UBOOT_NE_DBG */

#ifndef CONFIG_98FH_MP_ENABLE
    printf("port%d (intfs: %d) port_cfg: %#x\n", port_id, phy_intfs[port_id], port_static_cfg.wrd);
#endif

}

#if 0//yocto
#else//sd1 uboot for 98f mp test
static void aal_ni_eth_port4_config_rgmii_set(ca_boolean_t is_ge_rgmii)
{
    ca_device_id_t device_id = 0;
    NI_HV_PT_PORT_STATIC_CFG_t port_static_cfg;
    NI_HV_PT_PORT_GLB_CFG_t ni_hv_pt_port_glb_cfg;
    ca_port_id_t port_id = 4;
    ca_boolean_t link_up, duplex;
    ca_uint8_t speed;
    ca_uint32_t port_static_reg, port_glb_reg;

    ca_fe_phy_link_status_get(port_id, link_up, speed, duplex);

    /* config if */
    port_static_reg = NI_HV_PT_PORT_STATIC_CFG + port_id * __NI_PORT_REG_STRIDE;
    port_glb_reg = NI_HV_PT_PORT_GLB_CFG + port_id * __NI_PORT_REG_STRIDE;
    port_static_cfg.wrd = __NI_REG_READ(device_id, port_static_reg);
    if(ENABLE == is_ge_rgmii){
        port_static_cfg.bf.int_cfg = AAL_NI_IF_MODE_GE_RGMII;
    } else {
        port_static_cfg.bf.int_cfg = AAL_NI_IF_MODE_FE_MII;

        ni_hv_pt_port_glb_cfg.wrd = __NI_REG_READ(device_id, port_glb_reg);
        ni_hv_pt_port_glb_cfg.bf.speed = speed;
        ni_hv_pt_port_glb_cfg.bf.duplex = duplex;
        __NI_REG_WRITE(device_id, port_glb_reg, ni_hv_pt_port_glb_cfg.wrd);
    }
    __NI_REG_WRITE(device_id, port_static_reg, port_static_cfg.wrd);

#ifndef CONFIG_98FH_MP_ENABLE
    printf("\tport%d:\tconfig %#x = %#x\n", port_id, port_static_reg, port_static_cfg.wrd);
#endif /* if not def CONFIG_98FH_MP_ENABLE */
}
#endif//sd1 uboot for 98f mp test

ca_status_t aal_ni_eth_port_mac_set(
    CA_IN ca_device_id_t                      device_id,
    CA_IN ca_uint32_t                     port_id,
    CA_IN aal_ni_eth_mac_config_mask_t  mask,
    CA_IN aal_ni_eth_mac_config_t      *config
)
{
    NI_HV_PT_RXMAC_CFG_t     reg_cfg0;
    NI_HV_PT_TXMAC_CFG_t     reg_cfg1;
    NI_HV_PT_PORT_GLB_CFG_t  reg_cfg2;
    L2TM_L2TE_FC_CTRL_t      reg_cfg3;
    L2TM_L2TE_FC_GLB_TIMER_t reg_cfg4;

    __NI_ASSERT(config);

    if (!mask.u32) {
#if 0//yocto
#else//sd1 uboot for 98f
        ca_printf("%s(%d) %s\n", __func__, __LINE__, __FILE__);
#endif//sd1 uboot for 98f
        return CA_E_OK;
    }
    reg_cfg4.wrd = __NI_REG_READ(device_id, L2TM_L2TE_FC_GLB_TIMER);

    if (port_id < AAL_NI_PORT_NUM) {
        reg_cfg0.wrd = __NI_REG_READ(device_id, NI_HV_PT_RXMAC_CFG + port_id * __NI_PORT_REG_STRIDE);
        reg_cfg1.wrd = __NI_REG_READ(device_id, NI_HV_PT_TXMAC_CFG + port_id * __NI_PORT_REG_STRIDE);
        reg_cfg2.wrd = __NI_REG_READ(device_id, NI_HV_PT_PORT_GLB_CFG + port_id * __NI_PORT_REG_STRIDE);

        if (mask.s.rx_en           ) reg_cfg0.bf.rx_en            = config->rx_en           ;
        if (mask.s.rx_flow_disable ) reg_cfg0.bf.rx_flow_disable  = config->rx_flow_disable ;
        if (mask.s.rx_flow_to_tx_en) reg_cfg0.bf.rx_flow_to_tx_en = config->rx_flow_to_tx_en;
        if (mask.s.rx_pfc_disable  ) reg_cfg0.bf.rx_pfc_disable   = config->rx_pfc_disable  ;
        if (mask.s.send_pg_data    ) reg_cfg0.bf.send_pg_data     = config->send_pg_data    ;
        if (mask.s.tx_en           ) reg_cfg1.bf.tx_en            = config->tx_en           ;
        if (mask.s.mac_crc_calc_en ) reg_cfg1.bf.mac_crc_calc_en  = config->mac_crc_calc_en ;
        if (mask.s.tx_ipg_sel      ) reg_cfg1.bf.tx_ipg_sel       = config->tx_ipg_sel      ;
        if (mask.s.tx_flow_disable ) reg_cfg1.bf.tx_flow_disable  = config->tx_flow_disable ;
        if (mask.s.tx_drain        ) reg_cfg1.bf.tx_drain         = config->tx_drain        ;
        if (mask.s.tx_pfc_disable  ) reg_cfg1.bf.tx_pfc_disable   = config->tx_pfc_disable  ;
        if (mask.s.tx_auto_xon     ) reg_cfg1.bf.tx_auto_xon      = config->tx_auto_xon     ;
        if (mask.s.tx_pause_sel     ) reg_cfg1.bf.tx_pau_sel      = config->tx_pause_sel    ;
        if (mask.s.power_dwn_tx    ) reg_cfg2.bf.power_dwn_tx     = config->power_dwn_tx;
        if (mask.s.power_dwn_rx    ) reg_cfg2.bf.power_dwn_rx     = config->power_dwn_rx;
        if (mask.s.duplex          ) reg_cfg2.bf.duplex           = (config->duplex == AAL_NI_MAC_DUPLEX_HALF) ? 1 : 0     ;
        if (mask.s.speed           ) reg_cfg2.bf.speed            = (config->speed == AAL_NI_MAC_SPEED_10M) ? 1 : 0;

        __NI_REG_WRITE(device_id, NI_HV_PT_RXMAC_CFG + port_id * __NI_PORT_REG_STRIDE, reg_cfg0.wrd);
        __NI_REG_WRITE(device_id, NI_HV_PT_TXMAC_CFG + port_id * __NI_PORT_REG_STRIDE, reg_cfg1.wrd);
        __NI_REG_WRITE(device_id, NI_HV_PT_PORT_GLB_CFG + port_id * __NI_PORT_REG_STRIDE, reg_cfg2.wrd);

    if (mask.s.speed)
        aal_ni_eth_port_config_set(device_id, port_id, config->speed);
    }

    if (port_id < APB0_L2TM_L2TE_FC_COUNT) {
        reg_cfg3.wrd = __NI_REG_READ(device_id, L2TM_L2TE_FC_CTRL + port_id * APB0_L2TM_L2TE_FC_STRIDE);
        if (mask.s.tx_pause_en     ) reg_cfg3.bf.tx_en            = config->tx_pause_en;
        if (mask.s.rx_pause_en     ) reg_cfg3.bf.rx_en            = config->rx_pause_en;
        __NI_REG_WRITE(device_id, L2TM_L2TE_FC_CTRL + port_id * APB0_L2TM_L2TE_FC_STRIDE, reg_cfg3.wrd);

    }

    return CA_E_OK;
}


static ca_status_t __ni_rx_init(ca_device_id_t device_id)
{
    NI_HV_GLB_RXMUX_CTRL_CFG_t rxmux;
    NI_HV_PT_RXMAC_CFG_t rx_cfg;
    ca_uint32_t port_id = 0;

    rxmux.wrd = __NI_REG_READ(device_id, NI_HV_GLB_RXMUX_CTRL_CFG);

#if  defined(CONFIG_ARCH_CORTINA_SATURN_SFU) && defined(CONFIG_ARCH_CORTINA_G3HGU)
    rxmux.bf.dbg_sel = AAL_LPORT_ETH_WAN;
#endif

#if CONFIG_98F_UBOOT_NE_DBG
        ca_printf("\t%s(%d)\tAAL_LPORT_ETH_WAN: 0x%x\n", __FUNCTION__, __LINE__, AAL_LPORT_ETH_WAN);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    __NI_REG_WRITE(device_id, NI_HV_GLB_RXMUX_CTRL_CFG, rxmux.wrd);

    FOR_ALL_AAL_NI_PORT_DO(port_id) {
        rx_cfg.wrd = __NI_REG_READ(device_id, NI_HV_PT_RXMAC_CFG + port_id * __NI_PORT_REG_STRIDE);
        rx_cfg.bf.rx_en = 0;
        __NI_REG_WRITE(device_id, NI_HV_PT_RXMAC_CFG + port_id * __NI_PORT_REG_STRIDE, rx_cfg.wrd);

#if CONFIG_98F_UBOOT_NE_DBG
        ca_printf("\t%s(%d)\tNI_HV_PT_RXMAC_CFG(0x%x): 0x%08x\n", __FUNCTION__, __LINE__, NI_HV_PT_RXMAC_CFG + port_id * __NI_PORT_REG_STRIDE, __NI_REG_READ(device_id, NI_HV_PT_RXMAC_CFG + port_id * __NI_PORT_REG_STRIDE));
#endif /* CONFIG_98F_UBOOT_NE_DBG */
    }

    return CA_E_OK;

}

static ca_status_t __ni_tx_init(ca_device_id_t device_id)
{
    NI_HV_GLB_NITX_MISC_CFG_t nitx_misc;

    nitx_misc.wrd = __NI_REG_READ(device_id, NI_HV_GLB_NITX_MISC_CFG);

/*
 * This time base signal is used in port 0~4 Energy-Efficient Ethernet function.
 * When core_clk is 333Mhz, set it to 33; 3ns x 33 = 99 ns
 * When core_clk is 300Mhz, set it to 30; 3.3ns x 30 = 100 ns
 * When core_clk is 250Mhz, set it to 25, 4ns x 25 = 100 ns
 * When core_clk is 200Mhz, set it to 20; 5ns x 20 = 100 ns //98F NE
 */
#ifdef CONFIG_CORTINA_BOARD_FPGA
    nitx_misc.bf.time_base_cnt = 30;
#else /* for 98F ASIC */
    nitx_misc.bf.time_base_cnt = 20;
#endif /* CONFIG_CORTINA_BOARD_FPGA */

    __NI_REG_WRITE(device_id, NI_HV_GLB_NITX_MISC_CFG, nitx_misc.wrd);

#if CONFIG_98F_UBOOT_NE_DBG
        ca_printf("\t%s(%d)\tNI_HV_GLB_NITX_MISC_CFG(0x%08x): 0x%08x\n", \
                __FUNCTION__, __LINE__, NI_HV_GLB_NITX_MISC_CFG, \
                __NI_REG_READ(device_id, NI_HV_GLB_NITX_MISC_CFG));
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    return CA_E_OK;

}

static ca_status_t __ni_rxmux_init(ca_device_id_t device_id)
{
        aal_ni_port_rx_ctrl_mask_t   ni_port_rx_ctrl_mask;
        aal_ni_port_rx_ctrl_t        ni_port_rx_ctrl;
        ca_uint32_t                  port = 0;
        ca_status_t                  ret = CA_E_OK;

        ni_port_rx_ctrl_mask.u32 = 0;
        memset(&ni_port_rx_ctrl, 0, sizeof(aal_ni_port_rx_ctrl_t));

        ni_port_rx_ctrl_mask.s.oam_drop_dis      = 1;
        ni_port_rx_ctrl_mask.s.ukopcode_drop_dis = 1;

        ni_port_rx_ctrl.oam_drop_dis      = 1; /* Not drop OAM by default in NI, should do in the FE */
        ni_port_rx_ctrl.ukopcode_drop_dis = 1;

        FOR_ALL_AAL_NI_PORT_DO(port)  {
#if CONFIG_98F_UBOOT_NE_DBG
        ca_printf("\t%s(%d)\tport: 0x%x\n", __FUNCTION__, __LINE__, port);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

                ret = aal_ni_port_rx_ctrl_set(device_id, port,
                        ni_port_rx_ctrl_mask, &ni_port_rx_ctrl);
                if (CA_E_OK != ret) {
                        ca_printf("%s: aal_ni_port_rx_ctrl_set return 0x%x\n", __FUNCTION__, ret);
                }
        }

#if CONFIG_98F_UBOOT_NE_DBG
        ca_printf("\t%s(%d)\n", __FUNCTION__, __LINE__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
        return CA_E_OK;

}

static ca_status_t __ni_mc_init(ca_device_id_t device_id)
{
        ca_uint32_t  tmp32 = 0;

        tmp32 = __NI_REG_READ(device_id, NI_HV_MCE_ERROR_STS); /* clear stats */

        for (tmp32 = 0; tmp32 < AAL_NI_MC_GROUP_NUM; ++tmp32) {
                (void)aal_ni_mc_lkup_set(device_id, tmp32, 0);
        }

        return CA_E_OK;
}

static ca_status_t __ni_pkt_len_init(ca_device_id_t device_id)
{
    ca_status_t                   ret = CA_E_OK;
    aal_ni_pkt_len_config_mask_t  len_mask;
    aal_ni_pkt_len_config_t       len_cfg;

    len_mask.u32 = 0;
    len_mask.s.max_pkt_size = 1;
    len_mask.s.min_pkt_size = 1;

    memset(&len_cfg, 0, sizeof(len_cfg));

    len_cfg.max_pkt_size  = AAL_NI_PKT_LEN_MAX;
    len_cfg.min_pkt_size  = AAL_NI_PKT_LEN_MIN;

    ret = aal_ni_pkt_len_set(device_id, len_mask, &len_cfg);
    if (CA_E_OK != ret) {
        return ret;
    }

    return ret;

}

ca_status_t __ni_flow_ctrl_init(CA_IN ca_device_id_t device_id)
{
    ca_status_t                     ret = CA_E_OK;
    ca_uint32_t                     ge  = 0;
    ca_uint8_t                      cos = 0;
    ca_uint32_t                     port = 0;
#if 0//not for 98F
    ca_uint32_t                     xge = 0;

    aal_ni_xaui_mac_tx_ctrl_mask_t  xaui_tx_ctrl_mask;
    aal_ni_xaui_mac_tx_ctrl_t       xaui_tx_ctrl;
    aal_ni_xge_flowctrl_mask_t      xge_fc_mask;
    aal_ni_xge_flowctrl_t           xge_fc_config;
    aal_ni_xge_cfg_rx_mask_t        xge_rx_mask;
    aal_ni_xge_cfg_rx_t             xge_rx_cfg;
    aal_ni_xge_cfg_tx_mask_t        xge_tx_mask;
    aal_ni_xge_cfg_tx_t             xge_tx_cfg;
#endif
    aal_ni_eth_mac_config_mask_t    ge_mac_mask;
    aal_ni_eth_mac_config_t         ge_mac_cfg;
    L2TM_L2TE_FC_GLB_TIMER_t        fc_glb_timer;
    aal_ni_rxmux_fc_thrshld_mask_t  rx_mux_fc_thrd_mask;
    aal_ni_rxmux_fc_thrshld_t       rx_mux_fc_thrd;

    ge_mac_mask.u32 = 0;
    ge_mac_mask.s.rx_pfc_disable = 1;
    ge_mac_mask.s.tx_pfc_disable = 1;
    ge_mac_mask.s.rx_pause_en    = 1;
    ge_mac_mask.s.tx_pause_en    = 1;
    ge_mac_mask.s.tx_auto_xon    = 1;
    ge_mac_mask.s.tx_pause_sel   = 1;

    memset(&ge_mac_cfg, 0, sizeof(ge_mac_cfg));

    ge_mac_cfg.rx_pfc_disable = 1;
    ge_mac_cfg.tx_pfc_disable = 1;
    ge_mac_cfg.rx_pause_en    = 0;
    ge_mac_cfg.tx_pause_en    = 0;
    ge_mac_cfg.tx_auto_xon    = 1;
    ge_mac_cfg.tx_pause_sel   = AAL_NI_TX_PAUSE_SEL_TE_FC_THRED;

    fc_glb_timer.wrd = __NI_REG_READ(device_id, L2TM_L2TE_FC_GLB_TIMER);
    fc_glb_timer.bf.enable = 1; /* Enable the TE FC timer by default */
    __NI_REG_WRITE(device_id, L2TM_L2TE_FC_GLB_TIMER, fc_glb_timer.wrd);

        /* GE FlowControl init */
        __FOR_ALL_GE_DO(ge) {
        ret = aal_ni_eth_port_mac_set(device_id, ge, ge_mac_mask, &ge_mac_cfg);
        if (CA_E_OK != ret){
                __NI_ASSERT(!ret);
                return ret;
        }

        for (cos = 0; cos < 8; cos++) {//config for PriFC
                (void)aal_ni_eth_pfc_quanta_set(device_id, ge, cos,__NI_PAUSE_QUANTA_DFT);
        }
    }

#if 0//no XGE for 98F
    /* XGE FlowControl init */
    xge_fc_mask.u32 = 0;
    xge_fc_mask.s.pau_tx_en = 1;
    xge_fc_mask.s.pau_rx_en = 1;

    memset(&xge_fc_config, 0, sizeof(xge_fc_config));
    xge_fc_config.pau_tx_en = 0;
    xge_fc_config.pau_rx_en = 0;

    xge_rx_mask.u32 = 0;
    xge_rx_mask.s.pause_extract = 1;
    xge_rx_mask.s.pause_term    = 1;

    memset(&xge_rx_cfg, 0, sizeof(xge_rx_cfg));
    xge_rx_cfg.pause_extract    = 1;
    xge_rx_cfg.pause_term       = 1;

    xge_tx_mask.u32 = 0;
    xge_tx_mask.s.pause_req_auto_xon = 1;
    xge_tx_mask.s.pause_req_en = 1;
    xge_tx_mask.s.ifg_mode_tx = 1;

    memset(&xge_tx_cfg, 0, sizeof(xge_tx_cfg));
    xge_tx_cfg.pause_req_auto_xon = 1;
    xge_tx_cfg.pause_req_en = 1;
    xge_tx_cfg.ifg_mode_tx = 1;

    __FOR_ALL_XE_DO(ge) {
        xge = ge - __XE_PORT_START;
        ret = aal_ni_xge_pause_quanta_set(device_id, xge, __NI_PAUSE_QUANTA_DFT);//not for 98F

        if (CA_E_OK == ret)
            ret = aal_ni_xge_flowctrl_set(device_id, xge, xge_fc_mask, &xge_fc_config);//not for 98F

        if (CA_E_OK == ret)
            ret = aal_ni_xge_rx_config_set(device_id, xge, xge_rx_mask, &xge_rx_cfg);//not for 98F

        if (CA_E_OK == ret)
            ret = aal_ni_xge_tx_config_set(device_id, xge, xge_tx_mask, &xge_tx_cfg);//not for 98F

        if (CA_E_OK != ret)
            return ret;
    }
#endif

#if 0//no XAUI for 98F
    xaui_tx_ctrl_mask.u32 = 0;
    memset(&xaui_tx_ctrl, 0, sizeof(aal_ni_xaui_mac_tx_ctrl_t));

    xaui_tx_ctrl_mask.s.rx_pausereq_sel = 1;
    xaui_tx_ctrl.rx_pausereq_sel = 1;

    ret = aal_ni_xaui_mac_tx_ctrl_set(device_id, xaui_tx_ctrl_mask, &xaui_tx_ctrl);//not for 98F
    if (CA_E_OK != ret)
            return ret;
#endif

    rx_mux_fc_thrd_mask.u32 = 0;
    rx_mux_fc_thrd_mask.s.hi_threshold = 1;
    rx_mux_fc_thrd_mask.s.low_threshold = 1;
    memset(&rx_mux_fc_thrd, 0, sizeof(aal_ni_rxmux_fc_thrshld_t));
    rx_mux_fc_thrd.hi_threshold     = __NI_RXMUX_FC_THRD_HI;
    rx_mux_fc_thrd.low_threshold    = __NI_RXMUX_FC_THRD_LO;
    for(port = 0; port <4; port++){
        ret = aal_ni_rxmux_fc_thrshld_set(device_id,port,rx_mux_fc_thrd_mask,&rx_mux_fc_thrd);
        if(CA_E_OK != ret){
            return ret;
        }
    }
    return ret;

}

static ca_status_t __ni_mib_init(ca_device_id_t device_id)
{
    NI_HV_GLB_STATIC_CFG_t  glb_cfg;

    glb_cfg.wrd = __NI_REG_READ(device_id, NI_HV_GLB_STATIC_CFG);
    glb_cfg.bf.cnt_op_mode = 2; /* clear on read */
    __NI_REG_WRITE(device_id, NI_HV_GLB_STATIC_CFG, glb_cfg.wrd);

    return CA_E_OK;
}

static ca_status_t __ni_intr_init(ca_device_id_t device_id)
{
    aal_ni_port_intr_mask_t  intr_mask;
    aal_ni_port_intr_t       intr_cfg;
    ca_uint32_t                port_id = 0;
    ca_status_t                 ret = CA_E_OK;

    intr_mask.u32 = ~0U;

    for (port_id = 0; port_id < AAL_NI_PORT_NUM; ++port_id) {
        memset(&intr_cfg, 0, sizeof(intr_cfg));
        ret = aal_ni_port_intr_enable_set(device_id, port_id, intr_mask, &intr_cfg);
        if (CA_E_OK != ret)
            return CA_E_ERROR;

        memset(&intr_cfg, 0xFF, sizeof(intr_cfg));
        (void)aal_ni_port_intr_status_clear(device_id, port_id, &intr_cfg);
    }

   return CA_E_OK;
}

#define CA_MAX_HW_RST_WAIT_COUNT    10000
ca_status_t aal_glb_ni_ne_rst(CA_IN ca_device_id_t  device_id)
{
    ca_status_t ret = CA_E_OK;

#if 0//yocto
#else//sd1 uboot for 98f
    bool ni_init_done = 0, 
        l2fe_init_done = 0, 
        l2tm_init_done = 0, 
//        l3fe_init_done = 0, 
        ne_init_done = 0;
    ca_uint32 init_wait_count = 0;
    GLOBAL_BLOCK_RESET_SECURE_t global_block_reset_secure;
    NI_HV_GLB_INIT_DONE_t ni_hv_glb_init_done;
    L2FE_GLB_STTS_t l2fe_glb_stts;
    L2TM_L2TM_GLB_STS_t l2tm_l2tm_glb_sts;

#if 0
    ni_init_done = 0;
    l2fe_init_done = 0;
    l2tm_init_done = 0;
    l3fe_init_done = 0;
    ne_init_done = 0;
#endif

    /* assert ne reset */
    global_block_reset_secure.wrd = __NI_REG_READ(0, GLOBAL_BLOCK_RESET_SECURE);
    global_block_reset_secure.bf.reset_ni = 1;
    global_block_reset_secure.bf.reset_l2fe = 1;
    global_block_reset_secure.bf.reset_l2tm = 1;
    global_block_reset_secure.bf.reset_l3fe = 1;

    global_block_reset_secure.bf.reset_fbm = 1;
    global_block_reset_secure.bf.reset_tqm = 1;
    global_block_reset_secure.bf.reset_dma = 1;
    __NI_REG_WRITE(0, GLOBAL_BLOCK_RESET_SECURE, global_block_reset_secure.wrd);

    /* de-assert ne reset */
    global_block_reset_secure.wrd = __NI_REG_READ(0, GLOBAL_BLOCK_RESET_SECURE);
    global_block_reset_secure.bf.reset_ni = 0;
    global_block_reset_secure.bf.reset_l2fe = 0;
    global_block_reset_secure.bf.reset_l2tm = 0;
    global_block_reset_secure.bf.reset_l3fe = 0;

    global_block_reset_secure.bf.reset_fbm = 0;
    global_block_reset_secure.bf.reset_tqm = 0;
    global_block_reset_secure.bf.reset_dma = 0;
    __NI_REG_WRITE(0, GLOBAL_BLOCK_RESET_SECURE, global_block_reset_secure.wrd);

    /* wait until ne reset done */
    do{
        ni_hv_glb_init_done.wrd = __NI_REG_READ(0, GLOBAL_BLOCK_RESET_SECURE);
        l2fe_glb_stts.wrd = __NI_REG_READ(0, L2FE_GLB_STTS);
        ni_init_done = ni_hv_glb_init_done.bf.ni_init_done;
        l2fe_init_done = l2fe_glb_stts.bf.l2fe_init_done;
        l2tm_init_done = l2tm_l2tm_glb_sts.bf.init_done;
//        l3fe_init_done = ;//DNE? 

        if( (!ni_init_done) && (!l2fe_init_done) && (!l2tm_init_done)){
            if(init_wait_count < CA_MAX_HW_RST_WAIT_COUNT){
                init_wait_count++;
            } else {
                ret = CA_E_TIMEOUT;
                return ret;
            }
        } else {
            ne_init_done = 1;
#if CONFIG_98F_UBOOT_NE_DBG
            ca_printf("%s:\tni & ne - hw reset done!]\n", __func__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
        }
    }
    while(!ne_init_done);
#endif//sd1 uboot for 98f

    return ret;

}

/**  aal_ni_init
 * Initialize NI block to drive GE, XAUI, NI-MUX, PTP
 *
 */
ca_status_t aal_ni_init(CA_IN ca_device_id_t  device_id)
{
    ca_status_t ret = CA_E_OK;
#if 1//sd1 uboot for 98f MP test
    ca_uint32 mcgid;
    ca_uint64 mcPBM;
#endif//sd1 uboot for 98f MP test

    ret = __ni_mem_init(device_id);/* polling until ni init done */
    if (CA_E_OK != ret) {
        ca_printf("__ni_mem_init fail, return %#x\n", ret);
        goto __end;
    }

    ret = __ni_rx_init(device_id); /* disable MAC Rx */
    if (CA_E_OK != ret) {
        ca_printf("__ni_tx_init fail, return %#x\n", ret);
        goto __end;
    }

    ret = __ni_tx_init(device_id);
    if (CA_E_OK != ret) {
        ca_printf("__ni_tx_init fail, return %#x\n", ret);
        goto __end;
    }

    ret = __ni_rxmux_init(device_id);
    if (CA_E_OK != ret) {
        ca_printf("__ni_rxmux_init fail, return %#x\n", ret);
        goto __end;
    }

    ret = __ni_mc_init(device_id);
    if (CA_E_OK != ret) {
        ca_printf("__ni_mc_init fail, return %#x\n", ret);
        goto __end;
    }

    ret = __ni_pkt_len_init(device_id);
    if (CA_E_OK != ret) {
        ca_printf("__ni_pkt_len_init fail, return %#x\n", ret);
        goto __end;
    }

    ret = __ni_flow_ctrl_init(device_id);
    if (CA_E_OK != ret) {
        ca_printf("__ni_flow_ctrl_init fail, return %#x\n", ret);
        goto __end;
    }

    ret = __ni_mib_init(device_id);
    if (CA_E_OK != ret) {
        ca_printf("__ni_mib_init fail, return %#x\n", ret);
        goto __end;
    }

    ret = __ni_intr_init(device_id);
    if (CA_E_OK != ret) {
        ca_printf("__ni_intr_init fail, return %#x\n", ret);
        goto __end;
    }

__end:

#if 0//CONFIG_98F_UBOOT_NE_DBG//yocto test
ca_printf("%s(%d) ==== %s\textra debug config (+) ==== \n", __func__, __LINE__, __FILE__);
    NI_HV_GLB_RXMUX_CTRL_CFG_t ni_hv_glb_rxmux_ctrl_cfg;
    ni_hv_glb_rxmux_ctrl_cfg.wrd = __NI_REG_READ(0, NI_HV_GLB_RXMUX_CTRL_CFG);
    ni_hv_glb_rxmux_ctrl_cfg.bf.dbg_sel = 2;//sel P2 to monitor
    __NI_REG_WRITE(0, NI_HV_GLB_RXMUX_CTRL_CFG, ni_hv_glb_rxmux_ctrl_cfg.wrd);

#if 0
    /* active_pcs[7] = 1 */
    GLOBAL_ENABLE_IP_1_SECURE_t global_enable_ip_1_secure;
    global_enable_ip_1_secure.wrd = __NI_REG_READ(0, GLOBAL_ENABLE_IP_1_SECURE);
    global_enable_ip_1_secure.bf.active_pcs = 1;
    __NI_REG_WRITE(0, global_enable_ip_1_secure.wrd, GLOBAL_ENABLE_IP_1_SECURE);
#endif
ca_printf("%s(%d) ==== %s\textra debug config (-) ==== \n", __func__, __LINE__, __FILE__);
#endif

#if 0//yocto
#else//sd1 uboot for 98f MP
    mcgid = 0;
    mcPBM = 0x1f;
#if CONFIG_98F_UBOOT_NE_DBG
    ca_printf("\t%s(%d) %s => create default flooding domain\n", __func__, __LINE__, __FILE__);
    ca_printf("\tmcgid: %#x\tmcPBM: %#x\n", mcgid, mcPBM);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
    aal_ni_mc_lkup_set(device_id, mcgid, mcPBM);
#endif//sd1 uboot for 98f MP

    return ret;
}

#if 0//yocto
#else//sd1 uboot for 98f mp test
void ni_p4_rgmii_set(
    ca_boolean_t is_ge_rgmii
){
    aal_ni_eth_port4_config_rgmii_set(is_ge_rgmii);

    return;
}

#define DBG_NI_EXTDEVCHK    0
ca_uint32_t  ni_extDevChk(
    ca_uint32_t ExtDev_PAGE,
    ca_uint32_t ExtDev_PHY_ADDR,
    ca_uint32_t ExtDev_PHYID1_REG,
    ca_uint32_t ExtDev_PHYID2_REG,
    ca_uint32_t ExtDev_PHYID1,
    ca_uint32_t ExtDev_PHYID1_bm,
    ca_uint32_t ExtDev_PHYID2,
    ca_uint32_t ExtDev_PHYID2_bm
){
    ca_uint32_t ret;

#if DBG_NI_EXTDEVCHK
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PAGE", ExtDev_PAGE, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHY_ADDR", ExtDev_PHY_ADDR, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHYID1_REG", ExtDev_PHYID1_REG, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHYID2_REG", ExtDev_PHYID2_REG, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHYID1", ExtDev_PHYID1, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHYID1_bm", ExtDev_PHYID1_bm, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHYID2", ExtDev_PHYID2, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHYID2_bm", ExtDev_PHYID2_bm, __func__, __LINE__);
#endif /* DBG_NI_EXTDEVCHK */

    ret = aal_mdioAccessExtDevPhyID(ExtDev_PAGE, ExtDev_PHY_ADDR, ExtDev_PHYID1_REG, ExtDev_PHYID2_REG, ExtDev_PHYID1, ExtDev_PHYID1_bm, ExtDev_PHYID2, ExtDev_PHYID2_bm);

    return ret;
}
#endif//sd1 uboot for 98f mp test


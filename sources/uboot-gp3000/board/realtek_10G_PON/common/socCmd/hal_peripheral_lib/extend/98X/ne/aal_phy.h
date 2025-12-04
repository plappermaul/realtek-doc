#ifndef __AAL_PHY_H__
#define __AAL_PHY_H__

#include "ca_types.h"
#if 0//Yocto
#include <soc/cortina/registers.h>
#else//98F uboot
#include "g3lite_registers.h"
#endif

#define CA_AAL_MDIO_WR_FLAG        (0)
#define CA_AAL_MDIO_RD_FLAG        (1)
#define CA_AAL_MDIO_ACCESS_TIMEOUT (10000)
#define CA_AAL_MDIO_PER_CLK        (62500)
#define CA_AAL_MDIO_DATA_MASK      0x0000FFFF

typedef enum {
	AAL_PHY_MDI_MDI = 0,	/* Fixed MDI       */
	AAL_PHY_MDI_MDIX = 1,	/* Fixed MDIX      */
	AAL_PHY_MDI_AUTO = 2	/* Auto MDI/MDIX   */
} aal_phy_mdi_mode_t;

typedef enum {
	AAL_PHY_SPEED_10 = 0,	/* 10M bps */
	AAL_PHY_SPEED_100 = 1,	/* 100M bps */
	AAL_PHY_SPEED_1000 = 2,	/* 1G bps */
	AAL_PHY_SPEED_10000 = 3,	/* 10G bps */
	AAL_PHY_SPEED_2500 = 4 /* 2.5G bps */
} aal_phy_speed_mode_t;

typedef enum {
        AAL_SXGMII_PCS_CONTROL_INDEX_0 = 0,   /* PCS control 1 */
        AAL_SXGMII_PCS_CONTROL_INDEX_1 = 1    /* PCS control 2 */
} aal_phy_sxgmii_pcs_control_index_t;

typedef struct {
	ca_boolean_t tx2rx_enable;
	ca_boolean_t rx2tx_enable;
} aal_phy_lpbk_mode_t;

typedef struct {
	ca_uint32_t adv_10base_t_half:1;
	ca_uint32_t adv_10base_t_full:1;
	ca_uint32_t adv_100base_tx_half:1;
	ca_uint32_t adv_100base_tx_full:1;
	ca_uint32_t adv_100base_t4:1;
	ca_uint32_t adv_pause:1;
	ca_uint32_t adv_asym_pause:1;
	ca_uint32_t adv_1000base_t_half:1;
	ca_uint32_t adv_1000base_t_full:1;
	ca_uint32_t rsrvd:23;
} aal_phy_autoneg_adv_t;

typedef struct {
	ca_boolean_t link_up;
	ca_boolean_t duplex;
	aal_phy_speed_mode_t speed;
} aal_phy_link_status_t;

typedef volatile union {
  struct {
    ca_uint16_t selector               : 5 ; /* bits 4:0 */
    ca_uint16_t adv_10base_t_half      : 1 ; /* bits 5:5 */
    ca_uint16_t adv_10base_t_full      : 1 ; /* bits 6:6 */
    ca_uint16_t adv_100base_tx_half    : 1 ; /* bits 7:7 */
    ca_uint16_t adv_100base_tx_full    : 1 ; /* bits 8:8 */
    ca_uint16_t adv_100base_t4         : 1 ; /* bits 9:9 */
    ca_uint16_t adv_pause              : 1 ; /* bits 10:10 */
    ca_uint16_t adv_asym_pause         : 1 ; /* bits 11:11 */
    ca_uint16_t reserved1              : 1 ; /* bits 12:12 */
    ca_uint16_t remote_fault           : 1 ; /* bits 13:13 */
    ca_uint16_t reserved2              : 1 ; /* bits 14:14 */
    ca_uint16_t next_page              : 1 ; /* bits 15:15 */
  } bf ;
  ca_uint16_t     wrd ;
} aal_phy_reg_ANAR_t;

typedef volatile union {
  struct {
    ca_uint16_t selector               : 5 ; /* bits 4:0 */
    ca_uint16_t adv_10base_t_half      : 1 ; /* bits 5:5 */
    ca_uint16_t adv_10base_t_full      : 1 ; /* bits 6:6 */
    ca_uint16_t adv_100base_tx_half    : 1 ; /* bits 7:7 */
    ca_uint16_t adv_100base_tx_full    : 1 ; /* bits 8:8 */
    ca_uint16_t adv_100base_t4         : 1 ; /* bits 9:9 */
    ca_uint16_t adv_pause              : 1 ; /* bits 10:10 */
    ca_uint16_t adv_asym_pause         : 1 ; /* bits 11:11 */
    ca_uint16_t reserved1              : 1 ; /* bits 12:12 */
    ca_uint16_t remote_fault           : 1 ; /* bits 13:13 */
    ca_uint16_t reserved2              : 1 ; /* bits 14:14 */
    ca_uint16_t next_page              : 1 ; /* bits 15:15 */
  } bf ;
  ca_uint16_t     wrd ;
} aal_phy_reg_ANLPAR_t;

typedef volatile union {
  struct {
    ca_uint16_t reserved               : 9 ; /* bits 8:0 */
    ca_uint16_t adv_1000base_t_full    : 1 ; /* bits 9:9 */
    ca_uint16_t adv_port_type          : 1 ; /* bits 10:10 */
    ca_uint16_t adv_master_slave       : 1 ; /* bits 11:11 */
    ca_uint16_t enable_master_slave    : 1 ; /* bits 12:12 */
    ca_uint16_t test_mode              : 3 ; /* bits 15:13 */
  } bf ;
  ca_uint16_t     wrd ;
} aal_phy_reg_GBCR_t;

typedef volatile union {
  struct {
    ca_uint16_t idle_error_cnt         : 8 ; /* bits 7:0 */
    ca_uint16_t reserved               : 2 ; /* bits 9:8 */
    ca_uint16_t lp_1000base_t_half     : 1 ; /* bits 10:10 */
    ca_uint16_t lp_1000base_t_full     : 1 ; /* bits 11:11 */
    ca_uint16_t remote_recv_status     : 1 ; /* bits 12:12 */
    ca_uint16_t local_recv_status      : 1 ; /* bits 13:13 */
    ca_uint16_t master_slave_result    : 1 ; /* bits 14:14 */
    ca_uint16_t master_slave_fault     : 1 ; /* bits 15:15 */
  } bf ;
  ca_uint16_t     wrd ;
} aal_phy_reg_GBSR_t;


ca_status_t aal_mdio_speed_set(CA_IN ca_device_id_t device_id,
			       CA_IN ca_uint32_t addr, CA_IN ca_uint32_t clock);

ca_status_t aal_mdio_speed_get(CA_IN ca_device_id_t device_id,
			       CA_IN ca_uint32_t addr,
			       CA_OUT ca_uint32_t * clock);

ca_status_t aal_mdio_write (
    CA_IN       ca_device_id_t             device_id,
    CA_IN      ca_uint8_t              st_code,
    CA_IN      ca_uint8_t              phy_port_addr,
    CA_IN      ca_uint8_t              reg_dev_addr,
    CA_IN      ca_uint16_t              addr,
    CA_IN      ca_uint16_t              data);

ca_status_t aal_mdio_read (
    CA_IN       ca_device_id_t             device_id,
    CA_IN      ca_uint8_t              st_code,
    CA_IN      ca_uint8_t              phy_port_addr,
    CA_IN      ca_uint8_t              reg_dev_addr,
    CA_IN      ca_uint16_t              addr,
    CA_OUT   ca_uint16_t             *data);

ca_status_t aal_eth_phy_reset(ca_device_id_t device_id, ca_uint8_t phy_addr);

ca_status_t aal_eth_phy_auto_neg_restart(ca_device_id_t device_id,
					 ca_uint8_t phy_addr);

ca_status_t aal_eth_phy_power_set(ca_device_id_t device_id,
				  ca_uint8_t phy_addr, ca_boolean_t power_up);

ca_status_t aal_eth_phy_power_get(ca_device_id_t device_id,
				  ca_uint8_t phy_addr, ca_boolean_t * power_up);

ca_status_t aal_eth_phy_auto_neg_set(ca_device_id_t device_id,
				     ca_uint8_t phy_addr, ca_boolean_t enable);

ca_status_t aal_eth_phy_auto_neg_get(ca_device_id_t device_id,
				     ca_uint8_t phy_addr,
				     ca_boolean_t * enable);

ca_status_t aal_eth_phy_ability_adv_set(ca_device_id_t device_id,
					ca_uint8_t phy_addr,
					aal_phy_autoneg_adv_t adv);

ca_status_t aal_eth_phy_ability_adv_get(ca_device_id_t device_id,
					ca_uint8_t phy_addr,
					aal_phy_autoneg_adv_t * adv);

ca_status_t aal_eth_phy_partner_ability_adv_get(ca_device_id_t device_id,
						ca_uint8_t phy_addr,
						aal_phy_autoneg_adv_t * adv);

ca_status_t aal_eth_phy_link_status_get(ca_device_id_t device_id,
					ca_uint8_t phy_addr,
					aal_phy_link_status_t * link_status);

ca_status_t aal_eth_phy_speed_set(ca_device_id_t device_id,
				  ca_uint8_t phy_addr,
				  aal_phy_speed_mode_t speed);

ca_status_t aal_eth_phy_speed_get(ca_device_id_t device_id,
				  ca_uint8_t phy_addr,
				  aal_phy_speed_mode_t * speed);

ca_status_t aal_eth_phy_duplex_set(ca_device_id_t device_id,
				   ca_uint8_t phy_addr, ca_boolean_t is_full);

ca_status_t aal_eth_phy_duplex_get(ca_device_id_t device_id,
				   ca_uint8_t phy_addr, ca_boolean_t * is_full);

ca_status_t aal_eth_phy_loopback_set(ca_device_id_t device_id,
				     ca_uint8_t phy_addr,
				     aal_phy_lpbk_mode_t mode);

ca_status_t aal_eth_phy_loopback_get(ca_device_id_t device_id,
				     ca_uint8_t phy_addr,
				     aal_phy_lpbk_mode_t * mode);

ca_status_t  aal_eth_phy_mdix_get(
                    ca_device_id_t    device_id,
                    ca_uint8_t     phy_addr,
                    aal_phy_mdi_mode_t *mdix_mode);

ca_status_t  aal_eth_phy_mdix_set(
                    ca_device_id_t    device_id,
                    ca_uint8_t     phy_addr,
                    aal_phy_mdi_mode_t mdix_mode);

ca_status_t aal_eth_phy_init(ca_device_id_t device_id, ca_uint8_t phy_addr);

ca_status_t aal_mdio_global_init(ca_device_id_t device_id);

ca_status_t aal_mdio_global_exit(ca_device_id_t device_id);

#ifndef CONFIG_CORTINA_BOARD_FPGA
ca_status_t aal_internal_phy_init(ca_device_id_t  device_id);
#endif

#if 0//yocto
#else//sd1 uboot for 98f

typedef enum {
	AAL_GE_NIC_SPEED_10 = 0,	/* 10M bps */
	AAL_GE_NIC_SPEED_100 = 1,	/* 100M bps */
	AAL_GE_NIC_SPEED_1000 = 2,	/* 1G bps */
	AAL_GE_NIC_SPEED_10000 = 3,	/* 10G bps */
	AAL_GE_NIC_SPEED_2500 = 4 /* 2.5G bps */
} aal_ge_nic_speed_mode_t;

typedef enum {
	AAL_GE_NIC_HALF_DUPLEX = 0,
	AAL_GE_NIC_FULL_DUPLEX = 1
} aal_ge_nic_duplex_mode_t;

typedef enum {
	AAL_GE_NIC_OP_AN = 0,
	AAL_GE_NIC_OP_FM = 1,
	AAL_GE_NIC_OP_LPBK = 2
} aal_ge_nic_op_mode_t;

#endif//sd1 uboot for 98f

#endif

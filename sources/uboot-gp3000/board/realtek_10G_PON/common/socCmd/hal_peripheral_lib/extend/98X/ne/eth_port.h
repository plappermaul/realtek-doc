#ifndef __ETH_PORT_H__
#define __ETH_PORT_H__

#include "ca_types.h"

#define CA_ETH_PORT_NUM       (8)    /* Total Ethernet port number of the device */
#define CA_MDIO_ADDR_MIN      (0)    /* Minimal MDIO bus address */
#define CA_MDIO_ADDR_MAX      (31)   /* Maximal MDIO bus address */
#define CA_MDIO_CLOCK_MIN     (1)    /* Minimal MDIO bus clock speed */
#define CA_MDIO_CLOCK_MAX     (20000)/* Maximal MDIO bus clock speed */


typedef enum {
    CA_PORT_SPEED_INVALID       = 0, /* Port speed is invalid. If link is in auto-negotiation process or link is down, this valud is returned to caller. */
    CA_PORT_SPEED_10M           = 1, /* Speed is 10Mbps */
    CA_PORT_SPEED_100M          = 2, /* Speed is 100Mbps */
    CA_PORT_SPEED_1G            = 3, /* Speed is 1Gbps */
    CA_PORT_SPEED_2_5G          = 4, /* Speed is 2.5Gbps */
    CA_PORT_SPEED_10G           = 5, /* Speed is 10Gbps */
    CA_PORT_SPEED_AUTO          = 6  /* speed obtained via autonegotiation */
} ca_eth_port_speed_t;

typedef enum {
    CA_PORT_LINK_UP             = 0, /* Port is up */
    CA_PORT_LINK_DOWN           = 1  /* Port is down */
} ca_eth_port_link_status_t;

typedef enum {
    CA_PORT_DUPLEX_HALF         = 0, /* Half duplex mode */
    CA_PORT_DUPLEX_FULL         = 1, /* Full duplex mode */
    CA_PORT_DUPLEX_AUTO         = 2
} ca_eth_port_duplex_t;

typedef enum {
    CA_PORT_MDI                 = 0, /* Force MDI */
    CA_PORT_MDIX                = 1, /* Force MDIX */
    CA_PORT_MDIX_AUTO           = 2  /* Auto selection for MDI/MDIX */
} ca_eth_port_mdix_t;


typedef struct {/* PHY counters: gotten from PHY via MDIO */
    ca_uint32_t single_collision_frames;
    ca_uint32_t multiple_collision_frames;
    ca_uint32_t alignmentErrors;
    ca_uint32_t frames_with_deferredXmissions;
    ca_uint32_t late_collisions;
    ca_uint32_t frames_aborted_dueto_XS_colls;
    ca_uint32_t frames_lost_duetoint_mac_xmit_error;
    ca_uint32_t frames_lost_duetoint_mac_rcv_error;
    ca_uint32_t frames_with_excessive_deferral;
    ca_uint32_t in_rangeLength_errors;
    ca_uint32_t outof_range_length_field;
    ca_uint32_t collision_frames;
    ca_uint32_t symbol_error_during_carrier;
    ca_uint32_t media_available;
    ca_uint32_t autoneg_remote_signaling;
    ca_uint32_t autoneg_auto_config;
    ca_uint32_t unsupported_opcodes_received;
    ca_uint32_t pause_link_delay_allowance;
} ca_phy_stat_t;


typedef struct {
    ca_uint64_t rx_good_octets;      /* received octets without error                                             */
    ca_uint64_t rx_uc_frames;        /* received unicast frames                                                   */
    ca_uint64_t rx_mc_frames;        /* received multicast frames                                                 */
    ca_uint64_t rx_bc_frames;        /* received broadcast frames                                                 */
    ca_uint64_t rx_fcs_error_frames; /* received frames with CRC32 error                                          */
    ca_uint64_t rx_pause_frames;     /* received MAC PAUSE frames                                                 */
    ca_uint64_t rx_undersize_frames; /* received frames undersize frame without FCS err               */
    ca_uint64_t rx_runt_frames;      /* received frames of which length is lesser than 64                         */
    ca_uint64_t rx_gaint_frames;     /* received frames of which length is greater than max frames size           */
    ca_uint64_t rx_64_frames;        /* received frames with 64 bytes                                             */
    ca_uint64_t rx_65_127_frames;    /* received frames with 65 ~ 127 bytes                                       */
    ca_uint64_t rx_128_255_frames;   /* received frames with 128 ~ 255 bytes                                      */
    ca_uint64_t rx_256_511_frames;   /* received frames with 256 ~ 511 bytes                                      */
    ca_uint64_t rx_512_1023_frames;  /* received frames with 512 ~ 1023 bytes                                     */
    ca_uint64_t rx_1024_1518_frames; /* received frames with 1024 ~ 1518 bytes                                    */
    ca_uint64_t rx_1519_max_frames;  /* received frames of which length is equal to or greater than 1519 bytes    */
    ca_uint64_t tx_good_octets;      /* transmitted octets without error                                          */
    ca_uint64_t tx_uc_frames;        /* transmitted unicast frames                                                */
    ca_uint64_t tx_mc_frames;        /* transmitted multicast frames                                              */
    ca_uint64_t tx_bc_frames;        /* transmitted broadcast frames                                              */
    ca_uint64_t tx_pause_frames;     /* transmitted MAC PAUSE frames                                              */
    ca_uint64_t tx_64_frames;        /* transmitted frames with 64 bytes                                          */
    ca_uint64_t tx_65_127_frames;    /* transmitted frames with 65 ~ 127 bytes                                    */
    ca_uint64_t tx_128_255_frames;   /* transmitted frames with 128 ~ 255 bytes                                   */
    ca_uint64_t tx_256_511_frames;   /* transmitted frames with 256 ~ 511 bytes                                   */
    ca_uint64_t tx_512_1023_frames;  /* transmitted frames with 512 ~ 1023 bytes                                  */
    ca_uint64_t tx_1024_1518_frames; /* transmitted frames with 1024 ~ 1518 bytes                                 */
    ca_uint64_t tx_1519_max_frames;  /* transmitted frames of which length is equal to or greater than 1519 bytes */

    ca_phy_stat_t phy_stats; /* ethernet PHY statistics */
} ca_eth_port_stat_t;

typedef struct {
    ca_boolean_t    half_10;        /* PHY REG 4.5 setting for 10BASE-TX half duplex capable    */
    ca_boolean_t    full_10;        /* PHY REG 4.6 setting for 10BASE-TX full duplex capable    */
    ca_boolean_t    half_100;       /* PHY REG 4.7 setting for 10BASE-TX half full capable      */
    ca_boolean_t    full_100;       /* PHY REG 4.8 setting for 100BASE-TX half duplex capable   */
    ca_boolean_t    full_1g;        /* PHY REG 4.9 setting for 1000BASE-TX half full capable    */
    ca_boolean_t    full_10g;       /* XGE */
    ca_boolean_t    flow_ctrl;      /* PHY REG 4.10 setting for flow control capable            */
    ca_boolean_t    asym_flow_ctrl; /* PHY REG 4.11 setting for asymmetric flow control capable */
} ca_eth_port_ability_t;



/*****************************************************************************
Syntax
                ca_eth_port_pause_set
                ca_eth_port_pause_get
Parameters
                device_id      CA device number
                port_id     Ethernet port ID
                pfc_enable  Enable/Disable priority based flow control(802.1Qbb)
                pause_rx    Reception control for PAUSE frame
                pause_tx    Transmission control for PAUSE frame
Description
                These API routines can be used to set or get PAUSE TX and RX
                independently, which are also known as asymmetric flow control.
                If pause_rx is enabled, the device will process PAUSE frame and
                decide to transmit data or not according to time parameter in
                PAUSE frame. If pause_tx is enabled, the device will transmit
                PAUSE frame when the traffic load exceed the a configured
                threshold value.
Returns
                CA_E_OK     Operation completed successfully
                CA_E_XXX    Other errors

****************************************************************************/
ca_status_t ca_eth_port_pause_set(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_boolean_t pfc_enable,
    CA_IN ca_boolean_t pause_rx,
    CA_IN ca_boolean_t pause_tx
);

ca_status_t ca_eth_port_pause_get(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_OUT ca_boolean_t *pfc_enable,
    CA_OUT ca_boolean_t *pause_rx,
    CA_OUT ca_boolean_t *pause_tx
);

/*****************************************************************************
Syntax
                ca_eth_port_pause_quanta_set
                ca_eth_port_pause_quanta_get
Parameters
                device_id          CA device number
                port_id         Ethernet port ID
                pause_quanta    Pause quanta for the port
                                (each quanta is 512bit times)

Description
                These API routines can be used to set or get PAUSE quanta of
                given port.
Returns
                CA_E_OK     Operation completed successfully
                CA_E_XXX    Other errors

****************************************************************************/

ca_status_t ca_eth_port_pause_quanta_set (
    CA_IN ca_device_id_t       device_id,
    CA_IN ca_port_id_t      port_id,
    CA_IN ca_uint16_t       pause_quanta
);

ca_status_t ca_eth_port_pause_quanta_get (
    CA_IN  ca_device_id_t      device_id,
    CA_IN  ca_port_id_t     port_id,
    CA_OUT ca_uint16_t     *pause_quanta
);

/*****************************************************************************
Syntax
                ca_eth_port_link_scan_set
                ca_eth_port_link_scan_get
Parameters
                device_id      CA device number
                port_id     Ethernet Port ID
                enable      Link scan enable flag


Description
                These API routines can be used to set or get the link scanning
                state for the specified port. If link scanning function is
                enabled, event CA_EVENT_LINK_CHNAGE will be posted the caller
                upon the change of link status..
Returns
                CA_E_OK     Operation completed successfully
                CA_E_XXX    Other errors

****************************************************************************/

ca_status_t ca_eth_port_link_scan_set(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_boolean_t enable
);

ca_status_t ca_eth_port_link_scan_get(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_OUT ca_boolean_t *enable
);

/*****************************************************************************
Syntax
                ca_eth_port_link_status_get
Parameters
                device_id      CA device number
                port_id     Ethernet Port ID
                status      Pointer to link status

Description
                System provides CA_EVENT_PORT_LINK event to notify application
                software of link status in real time. In addition, application
                software can use this API to retrieve link status..
Returns
                CA_E_OK     Operation completed successfully
                CA_E_XXX    Other errors

****************************************************************************/

ca_status_t ca_eth_port_link_status_get(
    CA_IN  ca_device_id_t               device_id,
    CA_IN  ca_port_id_t              port_id,
    CA_OUT ca_eth_port_link_status_t *status
);

/*****************************************************************************
Syntax
                ca_eth_port_stat_get
Parameters
                device_id      CA device number
                port_id     Ethernet Port ID
                read_keep   Counter READ-KEEP flag
                            {FALSE, TRUE}
                data        Statistics data

Description
                This API routine is used to retrieve the statistics data for
                Ethernet port.
Returns
                CA_E_OK     Operation completed successfully
                CA_E_XXX    Other errors

****************************************************************************/

ca_status_t ca_eth_port_stat_get(
    CA_IN  ca_device_id_t        device_id,
    CA_IN  ca_port_id_t       port_id,
    CA_IN  ca_boolean_t       read_keep,
    CA_OUT ca_eth_port_stat_t *data
);

/*****************************************************************************
Syntax
                ca_eth_port_autoneg_set
                ca_eth_port_autoneg_get
Parameters
                device_id      CA device number
                port_id     Ethernet Port ID
                enable      Enable or disable auto-negotiation function


Description
                These APIs can be used to enable, disable or retrieve the
                auto-negotiation function of a port. If auto-negotiation
                function is enabled before, the re-enabling operation will cause
                auto-negotiation to be restarted. Note that prior to calling
                this API, the desired advertised mode must be configured.
Returns
                CA_E_OK     Operation completed successfully
                CA_E_XXX    Other errors

****************************************************************************/

ca_status_t ca_eth_port_autoneg_set (
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_boolean_t enable
);

ca_status_t ca_eth_port_autoneg_get (
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_OUT ca_boolean_t *enable
);


/*****************************************************************************
Syntax
                ca_eth_port_advert_set
                ca_eth_port_advert_get
Parameters
                device_id      CA device number
                port_id     Ethernet Port ID
                ability     Mask for port advertisement ability

Description
                These APIs can be used to set or get the advertisement settings
                for the specified port. If the port is operating with
                auto-negotiation disabled, these settings will only be
                configured into hardware but not take effect till
                auto-negotiation is enabled. If the port is in auto-negotiation
                enabled state when these settings are changed, auto-negotiation
                must be restarted using ca_eth_port_autoneg_set(). Setting or
                getting current ability can be done at any time, including in
                auto-negotiation disabled state. However the setting will only
                take effect when auto-negotiation is enabled or restarted.
Returns
                CA_E_OK     Operation completed successfully
                CA_E_XXX    Other errors

****************************************************************************/
ca_status_t ca_eth_port_advert_set (
    CA_IN ca_device_id_t           device_id,
    CA_IN ca_port_id_t          port_id,
    CA_IN ca_eth_port_ability_t ability
);

ca_status_t ca_eth_port_advert_get (
    CA_IN  ca_device_id_t           device_id,
    CA_IN  ca_port_id_t          port_id,
    CA_OUT ca_eth_port_ability_t *ability
);

/*****************************************************************************
Syntax
                ca_eth_port_advert_remote_get

Parameters
                device_id      CA device number
                port_id     Ethernet Port ID
                ability     Port advertisement ability

Description
                The API can be used to get the advertisement for the link
                partner of port, which is captured by local PHY during
                auto-negotiation. If auto-negotiation is in progress or
                disabled, the remote advertisement is returned as 0.
Returns
                CA_E_OK     Operation completed successfully
                CA_E_XXX    Other errors

****************************************************************************/

ca_status_t ca_eth_port_advert_remote_get (
    CA_IN  ca_device_id_t           device_id,
    CA_IN  ca_port_id_t          port_id,
    CA_OUT ca_eth_port_ability_t *ability
);

/*****************************************************************************
Syntax
                ca_eth_port_speed_set
                ca_eth_port_speed_get

Parameters
                device_id      CA device number
                port_id     Ethernet Port ID
                speed       Selector for operating speed

Description
                These API routines can be used to set or get the link speed for
                a port. Setting operation will cause auto-negotiation to be
                disabled if it is enabled. When retrieving the speed for a port,
                current negotiated speed is returned if auto-negotiation is
                enabled. If auto-negotiation is in progress or link is not
                established properly, speed is returned as value 0.
                If auto-negotiation is disabled, the configured speed of the
                port is returned regardless of the link state.
Returns
                CA_E_OK     Operation completed successfully
                CA_E_XXX    Other errors

****************************************************************************/

ca_status_t ca_eth_port_speed_set (
    CA_IN ca_device_id_t         device_id,
    CA_IN ca_port_id_t        port_id,
    CA_IN ca_eth_port_speed_t speed
);

ca_status_t ca_eth_port_speed_get (
    CA_IN  ca_device_id_t         device_id,
    CA_IN  ca_port_id_t        port_id,
    CA_OUT ca_eth_port_speed_t *speed
);

/*****************************************************************************
Syntax
                ca_eth_port_duplex_set
                ca_eth_port_duplex_get

Parameters
                device_id      CA device number
                port_id     Ethernet Port ID
                duplex      Duplex mode


Description
                These API routines can be used to set or get the link duplex
                mode for a port. Setting operation will cause auto-negotiation
                to be disabled if it is enabled. When retrieving the duplex
                mode for a port, current negotiated duplex status is returned
                if auto-negotiation is enabled. If auto-negotiation is in
                progress or link is not established properly, duplex is returned
                as value 0(half-duplex). If auto-negotiation is disabled, the
                configured duplex of the port is returned regardless of the
                link state.
Returns
                CA_E_OK     Operation completed successfully
                CA_E_XXX    Other errors

****************************************************************************/

ca_status_t ca_eth_port_duplex_set (
    CA_IN ca_device_id_t          device_id,
    CA_IN ca_port_id_t         port_id,
    CA_IN ca_eth_port_duplex_t duplex
);

ca_status_t ca_eth_port_duplex_get (
    CA_IN  ca_device_id_t          device_id,
    CA_IN  ca_port_id_t         port_id,
    CA_OUT ca_eth_port_duplex_t *duplex
);

/*****************************************************************************
Syntax
                ca_eth_port_mdix_set
                ca_eth_port_mdix_get

Parameters
                device_id      CA device number
                port_id     Ethernet Port ID
                mode        MDI/MDIX mode

Description
                These API routines can be used to set or get the link
                MDI/MDIX/MDIX_auto mode for a port.
Returns
                CA_E_OK     Operation completed successfully
                CA_E_XXX    Other errors

****************************************************************************/

ca_status_t ca_eth_port_mdix_set (
    CA_IN  ca_device_id_t          device_id,
    CA_IN  ca_port_id_t         port_id,
    CA_IN  ca_eth_port_mdix_t   mode);

ca_status_t ca_eth_port_mdix_get (
    CA_IN  ca_device_id_t          device_id,
    CA_IN  ca_port_id_t         port_id,
    CA_OUT ca_eth_port_mdix_t   *mode);


/*****************************************************************************
Syntax
                cap_mdio_speed_set
                cap_mdio_speed_get

Parameters
                device_id      CA device number
                addr        MDIO bus address
                clock       MDIO bus clock

Description
                These API routines can be used to set or get the MDIO device
                clock speed.
Returns
                CA_E_OK     Operation completed successfully
                CA_E_XXX    Other errors

****************************************************************************/

ca_status_t cap_mdio_speed_set (
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_uint32_t  addr,
    CA_IN  ca_uint32_t  clock);

ca_status_t cap_mdio_speed_get (
    CA_IN       ca_device_id_t         device_id,
    CA_IN       ca_uint32_t         addr,
    CA_OUT      ca_uint32_t        *clock);

ca_status_t cap_mdio_write(CA_IN ca_device_id_t device_id,
CA_IN      ca_uint8_t              st_code,
CA_IN      ca_uint8_t              phy_port_addr,
CA_IN      ca_uint8_t              reg_dev_addr,
CA_IN      ca_uint16_t             addr,
CA_IN      ca_uint16_t            data);

ca_status_t cap_mdio_read(CA_IN ca_device_id_t device_id,
CA_IN      ca_uint8_t              st_code,
CA_IN      ca_uint8_t              phy_port_addr,
CA_IN      ca_uint8_t              reg_dev_addr,
CA_IN      ca_uint16_t             addr,
CA_OUT   ca_uint16_t         *data);

#if 1//sd1 uboot for 98f ne mp test
#define RTL8198F_FE_PHY_NUM     5
#endif//sd1 uboot for 98f ne mp test

#endif


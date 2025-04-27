/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 74911 $
 * $Date: 2016-12-30 17:48:29 +0800 (Fri, 30 Dec 2016) $
 *
 * Purpose :
 *
 * Feature :
 *
 */

#ifndef __RTK_SWITCH_H__
#define __RTK_SWITCH_H__

/*
 * Include Files
 */
#include <rtk_type.h>


/*
 * Symbol Definition
 */
#define RTL8352M_CHIP_ID    (0x83526800)
#define RTL8353M_CHIP_ID    (0x83536800)
#define RTL8390M_CHIP_ID    (0x83906800)
#define RTL8391M_CHIP_ID    (0x83916800)
#define RTL8392M_CHIP_ID    (0x83926800)
#define RTL8393M_CHIP_ID    (0x83936800)
#define RTL8396M_CHIP_ID    (0x83966800)
#define RTL8330M_CHIP_ID    (0x83306800)
#define RTL8332M_CHIP_ID    (0x83326800)
#define RTL8380M_CHIP_ID    (0x83806800)
#define RTL8382M_CHIP_ID    (0x83826800)
#define RTL8381M_CHIP_ID    (0x83816800)

typedef enum rtk_chip_e {       /* sorted by family */
    RTK_CHIP_NONE = 0,
    RTK_CHIP_RTL8208,
    RTK_CHIP_RTL8208D,
    RTK_CHIP_RTL8208L,
    RTK_CHIP_RTL8212F,
    RTK_CHIP_RTL8212B,
    RTK_CHIP_RTL8214,
    RTK_CHIP_RTL8214F,
    RTK_CHIP_RTL8214FB,
    RTK_CHIP_RTL8214FC,
    RTK_CHIP_RTL8214B,
    RTK_CHIP_RTL8218,
    RTK_CHIP_RTL8218B,
    RTK_CHIP_RTL8218C,
    RTK_CHIP_RTL8218FB,
    RTK_CHIP_RTL8352M,
    RTK_CHIP_RTL8353M,
    RTK_CHIP_RTL8391M,
    RTK_CHIP_RTL8392M,
    RTK_CHIP_RTL8393M,
    RTK_CHIP_RTL8396M,
    RTK_CHIP_RTL8382M,
    RTK_CHIP_RTL8380M,
    RTK_CHIP_RTL8330M,
    RTK_CHIP_RTL8332M,
    RTK_CHIP_UNKNOWN,
} rtk_chip_t;

typedef enum rtk_mii_e {
    RTK_MII_NONE,
    RTK_MII_RMII,
    RTK_MII_SMII,               /* 125M */
    RTK_MII_SSSMII,             /* 125M */
    RTK_MII_100BX_FIBER,        /* 125M*/
    RTK_MII_1000BX_FIBER,       /* 1.25G */
    RTK_MII_RSGMII,             /* 2.5G */
    RTK_MII_RSGMII_PLUS,        /* 5G */
    RTK_MII_XSMII,                     /* RS8MII   2.5G */
    RTK_MII_QSGMII,                   /* 5G*/
    RTK_MII_SGMII,                     /* 1.25G*/
    RTK_MII_HISGMII,                 /* 3.125G*/
} rtk_mii_t;

typedef enum serdes_polarity_e {
    SERDES_POLARITY_NORMAL = 0,
    SERDES_POLARITY_CHANGE = 1,
} serdes_polarity_t;

typedef struct rtk_phy_s {
    Tuint32 chip;
    Tuint8  phyid;
} rtk_phy_t;

typedef enum led_if_sel_e {
    LED_IF_SEL_NONE,
    LED_IF_SEL_SERIAL,
    LED_IF_SEL_SINGLE_COLOR_SCAN,
    LED_IF_SEL_BI_COLOR_SCAN,
} led_if_sel_t;

typedef enum phy_rstGpioType_e {
    PHY_RSTGPIOTYPE_NONE,
    PHY_RSTGPIOTYPE_INTERNAL,
    PHY_RSTGPIOTYPE_EXTERNAL,
    PHY_RSTGPIOTYPE_END,
} phy_rstGpioType_t;

typedef struct phy_rstIntGpio_e {
    Tuint8  pin;
} phy_rstIntGpio_t;

typedef struct phy_rstExtGpio_e {
    Tuint8  addr;
    Tuint8  pin;
} phy_rstExtGpio_t;

#define RTK_SWITCH_MODEL_NAME_MAX   (64)
#define RTK_SWITCH_MODEL_PORT_MAX   (64)
#define RTK_SWITCH_MODEL_SERDES_MAX (16)
#define RTK_SWITCH_MODEL_PHY_MAX    (8)

typedef struct rtk_switch_model_s {
    Tint8   name[RTK_SWITCH_MODEL_NAME_MAX];
    Tuint32 chip;
    struct {
        Tuint8  offset;
        Tuint8  count;
        Tuint8  num;
        Tuint8  p0_p23_led_num:4;
        Tuint8  p24_p27_led_num:4;
        Tuint8  sel_pwr_on_led;
        Tuint16 sel_p0_p23_led_mod;
        Tuint16 sel_p24_p27_led_mod;

        /* RTL835x/RTL839x */
        led_if_sel_t    led_if_sel;     /* LED interface */
        Tuint32 copr_pmask[2];
        Tuint32 fib_pmask[2];
        Tuint32 led_combo[2];
        struct {
            Tuint8 led[3];
        } led_definition_set[4];
        Tuint32 led_copr_set_psel_bit0_pmask[2];
        Tuint32 led_copr_set_psel_bit1_pmask[2];
        Tuint32 led_fib_set_psel_bit0_pmask[2];
        Tuint32 led_fib_set_psel_bit1_pmask[2];
    } led;
    struct {
        Tuint8  offset;
        Tuint8  count;
        struct {
            Tuint8  mac_id;     /* Physical MAC ID */
            Tuint8  phy_idx;
            Tint8  phy;
        } list[RTK_SWITCH_MODEL_PORT_MAX];
    } port;
    struct {
        Tuint8  offset;
        Tuint8  count;
        struct {
            Tuint8  sds_id;     /* Physical Serdes ID */
            Tuint8  phy_idx;
            Tuint8  mii:4;
            Tuint8  reserved:2;
            Tuint8  rx_polarity:1;
            Tuint8  tx_polarity:1;
        } list[RTK_SWITCH_MODEL_SERDES_MAX];
    } serdes;
    struct {
        Tuint8  baseid;
        Tuint8  count;
        Tuint8  rstGpioType;
        union {
            phy_rstIntGpio_t    rstIntGpio;
            phy_rstExtGpio_t    rstExtGpio;
        } rstGpio;
        struct {
            Tuint32 chip;
            Tuint8  phy_max;
            Tuint8  mac_id;
        } list[RTK_SWITCH_MODEL_PHY_MAX];
    } phy;
} rtk_switch_model_t;

typedef struct rtk_mac_drv_s
{
    unsigned int miim_max_page;
    int (*drv_probe)(void);
    int (*drv_miim_read)(int, int, int, unsigned int *);
    int (*drv_miim_write)(int, int, int, unsigned int);
    int (*drv_miim_portmask_write)(unsigned long long, int, int, unsigned int);
} rtk_mac_drv_t;

typedef struct rtk_extGpio_drv_s
{
    void (*extGpio_init)(void);
    int (*extGpio_reg_read)(uint32, uint32, uint32 *);
    int (*extGpio_reg_write)(uint32, uint32, uint32);
} rtk_extGpio_drv_t;

typedef enum {
    SERIAL_IO_BAUD=0,
    SERIAL_IO_LOOPBACK,
    TOTAL_SERIAL_IOCTL
} serial_io_t;

#endif  /*__RTK_SWITCH_H__*/


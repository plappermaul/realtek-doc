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
 * Purpose : Chip model ID definition in the SDK.
 *
 * Feature : Chip model ID definition
 *
 */
#ifndef __HAL_CHIPDEF_CHIP_H__
#define __HAL_CHIPDEF_CHIP_H__




/* Definition vendor ID */
#define REALTEK_VENDOR_ID           (0x10EC)
#define REALTEK_MES_CHIP_ID         (0x6966)

/* Definition family ID */
#define RTL8390_FAMILY_ID           (0x83900000)
#define RTL8350_FAMILY_ID           (0x83500000)
#define RTL8380_FAMILY_ID           (0x83800000)
#define RTL8330_FAMILY_ID           (0x83300000)
#define RTL9310_FAMILY_ID           (0x93100000)
#define RTL9300_FAMILY_ID           (0x93000000)
#define RTL8382B_FAMILY_ID          (0x8382B000)
#define RTLCHIP_FAMILY_ID_NONE      (0xFFFFFFFF)


/* Definition chip ID */
#define RTL8351M_CHIP_ID            (0x83516800)
#define RTL8352M_CHIP_ID            (0x83526800)
#define RTL8353M_CHIP_ID            (0x83536800)
#define RTL8390M_CHIP_ID            (0x83906800)
#define RTL8391M_CHIP_ID            (0x83916800)
#define RTL8392M_CHIP_ID            (0x83926800)
#define RTL8393M_CHIP_ID            (0x83936800)
#define RTL8396M_CHIP_ID            (0x83966800)
#define RTL8352MES_CHIP_ID          (0x83526966)
#define RTL8353MES_CHIP_ID          (0x83536966)
#define RTL8392MES_CHIP_ID          (0x83926966)
#define RTL8393MES_CHIP_ID          (0x83936966)
#define RTL8396MES_CHIP_ID          (0x83966966)
#define RTL8330M_CHIP_ID            (0x83306800)
#define RTL8332M_CHIP_ID            (0x83326800)
#define RTL8380M_CHIP_ID            (0x83806800)
#define RTL8382M_CHIP_ID            (0x83826800)
#define RTL8381M_CHIP_ID            (0x83816800)

#define RTL9301_CHIP_ID_24G         (0x93016810)
#define RTL9303_CHIP_ID_8XG         (0x93036810)
#define RTL9301H_CHIP_ID_4X2_5G     (0x93014010)
#define RTL9302A_CHIP_ID_12X2_5G    (0x93020810)
#define RTL9302B_CHIP_ID_8X2_5G     (0x93021010)
#define RTL9302C_CHIP_ID_16X2_5G    (0x93021810)
#define RTL9302D_CHIP_ID_24X2_5G    (0x93022010)

#define RTL9301_CHIP_ID             (0x93010000)
#define RTL9301H_CHIP_ID            (0x93014000)
#define RTL9302A_CHIP_ID            (0x93020800)
#define RTL9302B_CHIP_ID            (0x93021000)
#define RTL9302C_CHIP_ID            (0x93021800)
#define RTL9302D_CHIP_ID            (0x93022000)
#define RTL9303_CHIP_ID             (0x93030000)
#define RTL9302DE_CHIP_ID           (0x93022140)
#define RTL9302F_CHIP_ID            (0x93023001)


#define RTL9300_CHIP_ID_LASTEST_REV  1
#define RTL9300_CHIP_ID_ES_LASTEST_REV     3

#define RTL9310_CHIP_ID             (0x93100000)
#define RTL9311_CHIP_ID             (0x93110000)
#define RTL9311E_CHIP_ID            (0x93112800)
#define RTL9311R_CHIP_ID            (0x93119000)
#define RTL9312_CHIP_ID             (0x93120000)
#define RTL9313_CHIP_ID             (0x93130000)

#define RTL9310_LASTEST_REV         0
#define RTL9310_ES_LASTEST_REV      0

#define RTL8382B_CHIP_ID            (0x8382B000)

#define THIRD_PARTY_MAC_CHIP_ID     (0xFFFFFFFE)

/* Definition chip revision ID */
#define CHIP_REV_ID_A               (0x0)
#define CHIP_REV_ID_B               (0x1)
#define CHIP_REV_ID_C               (0x2)
#define CHIP_REV_ID_D               (0x3)
#define CHIP_REV_ID_MAX             (255)   /* 0xFF */



/* Definition chip type */
#define CHIP_TYPE_1                 (0x1)
extern int rtk_chip_type[];
#define IF_CHIP_TYPE_1(unit)        (rtk_chip_type[unit]==CHIP_TYPE_1)



#endif/* __HAL_CHIPDEF_CHIP_H__ */

/*
 * Realtek Semiconductor Corp.
 *
 * bsp/bsp_ioal.h:
 *     bsp IO Abstraction Layer
 *
 * Copyright (C) 2006-2019
 */


/*
 * Register access macro
 */

#define BSP_REG32(reg)		(*(volatile unsigned int   *)(reg))


#define BSP_REG16(reg)		(*(volatile unsigned short *)(reg))
#define BSP_REG08(reg)		(*(volatile unsigned char  *)(reg))
#define BSP_REG8(reg)       (*(volatile unsigned char *)((unsigned int)reg))



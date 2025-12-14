#include "rtl8195a.h"

#define JTAG_PIN_FCTRL(ctrl)         HAL_PERL_ON_FUNC_CTRL(REG_CPU_PERIPHERAL_CTRL, BIT(16), ctrl)

PINMUX_RAM_BSS_SECTION
u16 GPIOState[11];

PINMUX_ROM_TEXT_SECTION
static u8
GPIOChk(
    IN u8   GPIOPortIdx,
    IN u16  GPIOIdx,
    IN u8   Operation)
{
    if (Operation) {

        if (GPIOState[GPIOPortIdx] & GPIOIdx) {

            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
static VOID
GPIOCtrl(
    IN u8   GPIOPortIdx,
    IN u16  GPIOIdx,
    IN u8   Operation)
{
    if (Operation) {

        GPIOState[GPIOPortIdx] |= GPIOIdx;
        HAL_PERI_ON_WRITE32(REG_GPIO_SHTDN_CTRL, (HAL_PERI_ON_READ32(REG_GPIO_SHTDN_CTRL)| BIT(GPIOPortIdx)));
    } else {

        GPIOState[GPIOPortIdx] &= ~GPIOIdx;

        if (!GPIOState[GPIOPortIdx]) {
            HAL_PERI_ON_WRITE32(REG_GPIO_SHTDN_CTRL, (HAL_PERI_ON_READ32(REG_GPIO_SHTDN_CTRL)& (~BIT(GPIOPortIdx))));
        }
    }
}

PINMUX_ROM_TEXT_SECTION
u8
HalUart0PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOC (2, 0:3)
                Rtemp = GPIOChk(2, 0x0F, Operation);

                if (Rtemp) {
                    GPIOCtrl(2, 0x0F, Operation);
                }
            } else {

                GPIOCtrl(2, 0x0F, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOE (4, 0:3)
                Rtemp = GPIOChk(4, 0x0F, Operation);

                if (Rtemp) {
                    GPIOCtrl(4, 0x0F, Operation);
                }
            } else {

                GPIOCtrl(4, 0x0F, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOA(0, 3), GPIOA(0, 5:7)
                Rtemp  = GPIOChk(0, 0xE8, Operation);

                if (Rtemp) {
                    GPIOCtrl(0, 0xE8, Operation);
                }
            } else {

                GPIOCtrl(0, 0xE8, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        UART0_PIN_SEL(PinLocation);
        //Enable function pin
        UART0_PIN_CTRL(ON);
    } else {
        if (!Operation) {
            //pin sel
            UART0_PIN_SEL(S0);
            //Enable function pin
            UART0_PIN_CTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalUart1PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOD (3, 0:3)
                Rtemp = GPIOChk(3, 0x0F, Operation);

                if (Rtemp) {
                    GPIOCtrl(3, 0x0F, Operation);
                }
            } else {
                GPIOCtrl(3, 0x0F, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOE (4, 4:7)
                Rtemp = GPIOChk(4, 0xF0, Operation);

                if (Rtemp) {
                    GPIOCtrl(4, 0xF0, Operation);
                }
            } else {
                GPIOCtrl(4, 0xF0, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOB (1, 4:7)
                Rtemp = GPIOChk(1, 0xF0, Operation);

                if (Rtemp) {
                    GPIOCtrl(1, 0xF0, Operation);
                }
            } else {
                GPIOCtrl(1, 0xF0, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        UART1_PIN_SEL(PinLocation);
        //Enable function pin
        UART1_PIN_CTRL(ON);
    } else {
        if (!Operation) {
            //pin sel
            UART1_PIN_SEL(S0);
            //Enable function pin
            UART1_PIN_CTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalUart2PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOA (0, 0:2, 4),
                Rtemp  = GPIOChk(0, 0x17, Operation);

                if (Rtemp) {
                    GPIOCtrl(0, 0x17, Operation);
                }
            } else {
                GPIOCtrl(0, 0x17, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOC (2, 6:9)
                Rtemp = GPIOChk(2, 0x3C0, Operation);

                if (Rtemp) {
                    GPIOCtrl(2, 0x3C0, Operation);
                }
            } else {
                GPIOCtrl(2, 0x3C0, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOD (3, 4:7)
                Rtemp = GPIOChk(3, 0xF0, Operation);

                if (Rtemp) {
                    GPIOCtrl(3, 0xF0, Operation);
                }
            } else {
                GPIOCtrl(3, 0xF0, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        UART2_PIN_SEL(PinLocation);
        //Enable function pin
        UART2_PIN_CTRL(ON);
    } else {
        if (!Operation) {
            //pin sel
            UART2_PIN_SEL(S0);
            //Enable function pin
            UART2_PIN_CTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalSPI0PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOE (4, 0:3)
                Rtemp  = GPIOChk(4, 0x0F, Operation);

                if (Rtemp) {
                    GPIOCtrl(4, 0x0F, Operation);
                }
            } else {
                GPIOCtrl(4, 0x0F, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOC (2, 0:3)
                Rtemp = GPIOChk(2, 0x0F, Operation);

                if (Rtemp) {
                    GPIOCtrl(2, 0x0F, Operation);
                }
            } else {
                GPIOCtrl(2, 0x0F, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        SPI0_PIN_SEL(PinLocation);
        //Enable function pin
        SPI0_PIN_CTRL(ON);
    } else {
        if (!Operation) {
            SPI0_PIN_SEL(S0);
            //Enable function pin
            SPI0_PIN_CTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalSPI1PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOA (0, 0:2, 4)
                Rtemp  = GPIOChk(0, 0x17, Operation);

                if (Rtemp) {
                    GPIOCtrl(0, 0x17, Operation);
                }
            } else {
                GPIOCtrl(0, 0x17, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOB (1, 4:7)
                Rtemp = GPIOChk(1, 0xF0, Operation);

                if (Rtemp) {
                    GPIOCtrl(1, 0xF0, Operation);
                }
            } else {
                GPIOCtrl(1, 0xF0, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOD (3, 4:7)
                Rtemp  = GPIOChk(3, 0xF0, Operation);

                if (Rtemp) {
                    GPIOCtrl(3, 0xF0, Operation);
                }
            } else {
                GPIOCtrl(3, 0xF0, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        SPI1_PIN_SEL(PinLocation);
        //Enable function pin
        SPI1_PIN_CTRL(ON);
    } else {
        if (!Operation) {
            SPI1_PIN_SEL(S0);
            //Enable function pin
            SPI1_PIN_CTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalSPI2PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOG (6, 0:3)
                Rtemp = GPIOChk(6, 0x0F, Operation);

                if (Rtemp) {
                    GPIOCtrl(6, 0x0F, Operation);
                }
            } else {
                GPIOCtrl(6, 0x0F, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOE (4, 4:7)
                Rtemp = GPIOChk(4, 0xF0, Operation);

                if (Rtemp) {
                    GPIOCtrl(4, 0xF0, Operation);
                }
            } else {
                GPIOCtrl(4, 0xF0, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOD (3, 0:3)
                Rtemp = GPIOChk(3, 0x0F, Operation);

                if (Rtemp) {
                    GPIOCtrl(3, 0x0F, Operation);
                }
            } else {
                GPIOCtrl(3, 0x0F, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        SPI2_PIN_SEL(PinLocation);
        //Enable function pin
        SPI2_PIN_CTRL(ON);
    } else {
        if (!Operation) {
            SPI2_PIN_SEL(S0);
            //Enable function pin
            SPI2_PIN_CTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalSPI0MCSPinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOE (4, 0:3),  GPIOE (4, 4:A)
                Rtemp  = GPIOChk(4, 0x7FF, Operation);

                if (Rtemp) {
                    GPIOCtrl(4, 0x7FF, Operation);
                }
            } else {
                GPIOCtrl(4, 0x7FF, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOC (2, 0:3),  GPIOC (2, 4:9)
                Rtemp  = GPIOChk(2, 0x3FF, Operation);

                if (Rtemp) {
                    GPIOCtrl(2, 0x3FF, Operation);
                }
            } else {
                GPIOCtrl(2, 0x3FF, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        SPI0_PIN_SEL(PinLocation);
        //Enable function pin
        SPI0_PIN_CTRL(ON);
        SPI0_MULTI_CS_CTRL(ON);
    } else {
        if (!Operation) {
            SPI0_PIN_SEL(S0);
            //Enable function pin
            SPI0_PIN_CTRL(OFF);
            SPI0_MULTI_CS_CTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalI2C0PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOD (3, 4:5)
                Rtemp = GPIOChk(3, 0x30, Operation);

                if (Rtemp) {
                    GPIOCtrl(3, 0x30, Operation);
                }
            } else {
                GPIOCtrl(3, 0x30, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOH (7, 0:1)
                Rtemp = GPIOChk(7, 0x03, Operation);

                if (Rtemp) {
                    GPIOCtrl(7, 0x03, Operation);
                }
            } else {
                GPIOCtrl(7, 0x03, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOC (2, 8:9)
                Rtemp = GPIOChk(2, 0x300, Operation);

                if (Rtemp) {
                    GPIOCtrl(2, 0x300, Operation);
                }
            } else {
                GPIOCtrl(2, 0x300, Operation);
            }
            break;

        case S3:

            if (Operation) {
                //GPIOE (4, 6:7)
                Rtemp = GPIOChk(4, 0xC0, Operation);

                if (Rtemp) {
                    GPIOCtrl(4, 0xC0, Operation);
                }
            } else {
                GPIOCtrl(4, 0xC0, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        I2C0_PIN_SEL(PinLocation);
        //Enable function pin
        I2C0_PIN_CTRL(ON);
    } else {
        if (!Operation) {
            I2C0_PIN_SEL(S0);
            //Enable function pin
            I2C0_PIN_CTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalI2C1PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOC (2, 4:5)
                Rtemp = GPIOChk(2, 0x30, Operation);

                if (Rtemp) {
                    GPIOCtrl(2, 0x30, Operation);
                }
            } else {
                GPIOCtrl(2, 0x30, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOH (7, 2:3)
                Rtemp = GPIOChk(7, 0x0C, Operation);

                if (Rtemp) {
                    GPIOCtrl(7, 0x0C, Operation);
                }
            } else {
                GPIOCtrl(7, 0x0C, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOD (3, 6:7)
                Rtemp = GPIOChk(3, 0xC0, Operation);

                if (Rtemp) {
                    GPIOCtrl(3, 0xC0, Operation);
                }
            } else {
                GPIOCtrl(3, 0xC0, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        I2C1_PIN_SEL(PinLocation);
        //Enable function pin
        I2C1_PIN_CTRL(ON);
    } else {
        if (!Operation) {
            I2C1_PIN_SEL(S0);
            //Enable function pin
            I2C1_PIN_CTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalI2C2PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOB (1, 6:7)
                Rtemp = GPIOChk(1, 0xC0, Operation);

                if (Rtemp) {
                    GPIOCtrl(1, 0xC0, Operation);
                }
            } else {
                GPIOCtrl(1, 0xC0, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOE (4, 0:1)
                Rtemp = GPIOChk(4, 0x03, Operation);

                if (Rtemp) {
                    GPIOCtrl(4, 0x03, Operation);
                }
            } else {
                GPIOCtrl(4, 0x03, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOC (2, 6:7)
                Rtemp = GPIOChk(2, 0xC0, Operation);

                if (Rtemp) {
                    GPIOCtrl(2, 0xC0, Operation);
                }
            } else {
                GPIOCtrl(2, 0xC0, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        I2C2_PIN_SEL(PinLocation);
        //Enable function pin
        I2C2_PIN_CTRL(ON);
    } else {
        if (!Operation) {
            I2C2_PIN_SEL(S0);
            //Enable function pin
            I2C2_PIN_CTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalI2C3PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOB (1, 2:3)
                Rtemp = GPIOChk(1, 0x0C, Operation);

                if (Rtemp) {
                    GPIOCtrl(1, 0x0C, Operation);
                }
            } else {
                GPIOCtrl(1, 0x0C, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOE (4, 2:3)
                Rtemp = GPIOChk(4, 0x0C, Operation);

                if (Rtemp) {
                    GPIOCtrl(4, 0x0C, Operation);
                }
            } else {
                GPIOCtrl(4, 0x0C, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOE (4, 4:5)
                Rtemp = GPIOChk(4, 0x30, Operation);

                if (Rtemp) {
                    GPIOCtrl(4, 0x30, Operation);
                }
            } else {
                GPIOCtrl(4, 0x30, Operation);
            }
            break;

        case S3:

            if (Operation) {
                //GPIOD (3, 8:9)
                Rtemp  = GPIOChk(3, 0x300, Operation);

                if (Rtemp) {
                    GPIOCtrl(3, 0x300, Operation);
                }
            } else {
                GPIOCtrl(3, 0x300, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        I2C3_PIN_SEL(PinLocation);
        //Enable function pin
        I2C3_PIN_CTRL(ON);
    } else {
        if (!Operation) {
            I2C3_PIN_SEL(S0);
            //Enable function pin
            I2C3_PIN_CTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}


PINMUX_ROM_TEXT_SECTION
u8
HalI2S0PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOE (4, 0:3),  GPIOD (3, 6)
                Rtemp  = GPIOChk(4, 0x0F, Operation);
                Rtemp &= GPIOChk(3, 0x40, Operation);

                if (Rtemp) {
                    GPIOCtrl(4, 0x0F, Operation);
                    GPIOCtrl(3, 0x40, Operation);
                }
            } else {
                GPIOCtrl(4, 0x0F, Operation);
                GPIOCtrl(3, 0x40, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOH (7, 0:3),  GPIOH (7, 5)
                Rtemp  = GPIOChk(7, 0x2F, Operation);

                if (Rtemp) {
                    GPIOCtrl(7, 0x2F, Operation);
                }
            } else {
                GPIOCtrl(7, 0x2F, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOD (3, 0:3,6),
                Rtemp  = GPIOChk(3, 0x4F, Operation);

                if (Rtemp) {
                    GPIOCtrl(3, 0x4F, Operation);
                }
            } else {
                GPIOCtrl(3, 0x4F, Operation);
            }
            break;

        case S3:

            if (Operation) {
                //GPIOC (2, 5:9),
                Rtemp  = GPIOChk(2, 0x3E0, Operation);

                if (Rtemp) {
                    GPIOCtrl(2, 0x3E0, Operation);
                }
            } else {
                GPIOCtrl(2, 0x3E0, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        I2S0_PIN_SEL(PinLocation);
        //Enable function pin
        I2S0_PIN_CTRL(ON);
    } else {
        if (!Operation) {
            I2S0_PIN_SEL(S0);
            //Enable function pin
            I2S0_PIN_CTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalI2S1PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOC (2, 0:4)
                Rtemp  = GPIOChk(2, 0x1F, Operation);

                if (Rtemp) {
                    GPIOCtrl(2, 0x1F, Operation);
                }
            } else {
                GPIOCtrl(2, 0x1F, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOD (3, 3:7),
                Rtemp  = GPIOChk(3, 0xF8, Operation);

                if (Rtemp) {
                    GPIOCtrl(3, 0xF8, Operation);
                }
            } else {
                GPIOCtrl(3, 0xF8, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOE (4, 4:8),
                Rtemp  = GPIOChk(4, 0x1F0, Operation);

                if (Rtemp) {
                    GPIOCtrl(4, 0x1F0, Operation);
                }
            } else {
                GPIOCtrl(4, 0x1F0, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        I2S1_PIN_SEL(PinLocation);
        //Enable function pin
        I2S1_PIN_CTRL(ON);
    } else {
        if (!Operation) {
            I2S1_PIN_SEL(S0);
            //Enable function pin
            I2S1_PIN_CTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalPCM0PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOH (7, 0:3)
                Rtemp  = GPIOChk(7, 0x0F, Operation);

                if (Rtemp) {
                    GPIOCtrl(7, 0x0F, Operation);
                }
            } else {
                GPIOCtrl(7, 0x0F, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOC (2, 6:9)
                Rtemp  = GPIOChk(2, 0x3C0, Operation);

                if (Rtemp) {
                    GPIOCtrl(2, 0x3C0, Operation);
                }
            } else {
                GPIOCtrl(2, 0x3C0, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOD (3, 0:3)
                Rtemp  = GPIOChk(3, 0x0F, Operation);

                if (Rtemp) {
                    GPIOCtrl(3, 0x0F, Operation);
                }
            } else {
                GPIOCtrl(3, 0x0F, Operation);
            }
            break;

        case S3:

            if (Operation) {
                //GPIOE (4, 0:3)
                Rtemp  = GPIOChk(4, 0x0F, Operation);

                if (Rtemp) {
                    GPIOCtrl(4, 0x0F, Operation);
                }
            } else {
                GPIOCtrl(4, 0x0F, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        PCM0_PIN_SEL(PinLocation);
        //Enable function pin
        PCM0_PIN_CTRL(ON);
    } else {
        if (!Operation) {
            PCM0_PIN_SEL(S0);
            //Enable function pin
            PCM0_PIN_CTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalPCM1PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOC (2, 0:3)
                Rtemp  = GPIOChk(2, 0x0F, Operation);

                if (Rtemp) {
                    GPIOCtrl(2, 0x0F, Operation);
                }
            } else {
                GPIOCtrl(2, 0x0F, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOD (3, 4:7)
                Rtemp  = GPIOChk(3, 0xF0, Operation);

                if (Rtemp) {
                    GPIOCtrl(3, 0xF0, Operation);
                }
            } else {
                GPIOCtrl(3, 0xF0, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOE (4, 4:7)
                Rtemp  = GPIOChk(4, 0xF0, Operation);

                if (Rtemp) {
                    GPIOCtrl(4, 0xF0, Operation);
                }
            } else {
                GPIOCtrl(4, 0xF0, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        PCM1_PIN_SEL(PinLocation);
        //Enable function pin
        PCM1_PIN_CTRL(ON);
    } else {
        if (!Operation) {
            PCM1_PIN_SEL(S0);
            //Enable function pin
            PCM1_PIN_CTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalSDIODPinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    if (Operation) {
        Rtemp  = GPIOChk(0, 0x7F, Operation);
    }

    if (Rtemp) {
        //Enable GPIO
        GPIOCtrl(0, 0x7F, ON);
        //Enable function pin
        SDIOH_PIN_FCTRL(ON);
    } else {
        if (!Operation) {
            GPIOCtrl(0, 0x7F, OFF);
            //Enable function pin
            SDIOH_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalSDIOHPinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    if (Operation) {
        Rtemp  = GPIOChk(0, 0xFF, Operation);
    }

    if (Rtemp) {
        //Enable GPIO
        GPIOCtrl(0, 0xFF, ON);
        //Enable function pin
        SDIOH_PIN_FCTRL(ON);
    } else {
        if (!Operation) {
            GPIOCtrl(0, 0xFF, OFF);
            //Enable function pin
            SDIOH_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalMIIPinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    if (Operation) {
        //GPIOA(0,7:0), GPIOC(2, 6:0), GPIOD(3,5:4)
        Rtemp  = GPIOChk(0, 0xFF, Operation);
        Rtemp &= GPIOChk(2, 0x7F, Operation);
        Rtemp &= GPIOChk(3, 0x30, Operation);
    }

    if (Rtemp) {
        //Enable GPIO
        GPIOCtrl(0, 0xFF, ON);
        GPIOCtrl(2, 0x7F, ON);
        GPIOCtrl(3, 0x30, ON);
        //Enable function pin
        MII_PIN_FCTRL(Operation);
    } else {
        if (!Operation) {
            GPIOCtrl(0, 0xFF, OFF);
            GPIOCtrl(2, 0x7F, OFF);
            GPIOCtrl(3, 0x30, OFF);
            //Enable function pin
            MII_PIN_FCTRL(Operation);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalWLLEDPinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOB (1, 5)
                Rtemp  = GPIOChk(1, 0x20, Operation);

                if (Rtemp) {
                    GPIOCtrl(1, 0x20, Operation);
                }
            } else {
                GPIOCtrl(1, 0x20, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOB (1, 4)
                Rtemp  = GPIOChk(1, 0x10, Operation);

                if (Rtemp) {
                    GPIOCtrl(1, 0x10, Operation);
                }
            } else {
                GPIOCtrl(1, 0x10, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOB (1, 1)
                Rtemp  = GPIOChk(1, 0x02, Operation);

                if (Rtemp) {
                    GPIOCtrl(1, 0x02, Operation);
                }
            } else {
                GPIOCtrl(1, 0x02, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        LED_PIN_SEL(PinLocation);
        //Enable function pin
        LED_PIN_FCTRL(ON);
    } else {
        if (!Operation) {
            LED_PIN_SEL(S0);
            //Enable function pin
            LED_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalWLANT0PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    if (Operation) {
        //GPIOB (1, 2:3)
        Rtemp  = GPIOChk(1, 0x0C, Operation);
    }

    if (Rtemp) {
        //Enable GPIO
        GPIOCtrl(1, 0x0C, ON);
        //Enable function pin
        ANT0_PIN_FCTRL(ON);
    } else {
        if (!Operation) {
            GPIOCtrl(1, 0x0C, OFF);
            //Enable function pin
            ANT0_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalWLANT1PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    if (Operation) {
        //GPIOB (1, 6:7)
        Rtemp  = GPIOChk(1, 0xC0, Operation);
    }

    if (Rtemp) {
        //Enable GPIO
        GPIOCtrl(1, 0xC0, ON);
        //Enable function pin
        ANT1_PIN_FCTRL(ON);
    } else {
        if (!Operation) {
            GPIOCtrl(1, 0xC0, OFF);
            //Enable function pin
            ANT1_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalWLBTCOEXPinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    if (Operation) {
        //GPIOC (2, 0:3)
        Rtemp  = GPIOChk(2, 0x0F, Operation);
    }

    if (Rtemp) {
        //Enable GPIO
        GPIOCtrl(2, 0x0F, ON);
        //Enable function pin
        BTCOEX_PIN_FCTRL(ON);
    } else {
        if (!Operation) {
            GPIOCtrl(2, 0x0F, OFF);
            //Enable function pin
            BTCOEX_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalWLBTCMDPinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    if (Operation) {
        //GPIOC (2, 4:6)
        Rtemp  = GPIOChk(2, 0x70, Operation);
    }

    if (Rtemp) {
        //Enable GPIO
        GPIOCtrl(2, 0x70, ON);
        //Enable function pin
        BTCMD_PIN_FCTRL(ON);
    } else {
        if (!Operation) {
            GPIOCtrl(2, 0x70, OFF);
            //Enable function pin
            BTCMD_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalNFCPinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    if (Operation) {
        //GPIOB (1, 0:5)
        Rtemp  = GPIOChk(1, 0x3F, Operation);
    }

    if (Rtemp) {
        //Enable GPIO
        GPIOCtrl(1, 0x3F, ON);
        //Enable function pin
        NFC_PIN_FCTRL(ON);
    } else {
        if (!Operation) {
            GPIOCtrl(1, 0x3F, OFF);
            //Enable function pin
            NFC_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalPWM0PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOB (1, 4)
                Rtemp  = GPIOChk(1, 0x10, Operation);

                if (Rtemp) {
                    GPIOCtrl(1, 0x10, Operation);
                }
            } else {
                GPIOCtrl(1, 0x10, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOC (2, 0)
                Rtemp  = GPIOChk(2, 0x1, Operation);

                if (Rtemp) {
                    GPIOCtrl(2, 0x1, Operation);
                }
            } else {
                GPIOCtrl(2, 0x1, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOD (3, 4)
                Rtemp  = GPIOChk(3, 0x10, Operation);

                if (Rtemp) {
                    GPIOCtrl(3, 0x10, Operation);
                }
            } else {
                GPIOCtrl(3, 0x10, Operation);
            }
            break;

        case S3:

            if (Operation) {
                //GPIOE (4, 0)
                Rtemp  = GPIOChk(4, 0x1, Operation);

                if (Rtemp) {
                    GPIOCtrl(4, 0x1, Operation);
                }
            } else {
                GPIOCtrl(4, 0x1, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        PWM0_PIN_SEL(PinLocation);
        //Enable function pin
        PWM0_PIN_FCTRL(Operation);
    } else {
        if (!Operation) {
            PWM0_PIN_SEL(S0);
            //Enable function pin
            PWM0_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalPWM1PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOB (1, 5)
                Rtemp  = GPIOChk(1, 0x20, Operation);

                if (Rtemp) {
                    GPIOCtrl(1, 0x20, Operation);
                }
            } else {
                GPIOCtrl(1, 0x20, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOC (2, 1)
                Rtemp  = GPIOChk(2, 0x2, Operation);

                if (Rtemp) {
                    GPIOCtrl(2, 0x2, Operation);
                }
            } else {
                GPIOCtrl(2, 0x2, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOD (3, 5)
                Rtemp  = GPIOChk(3, 0x20, Operation);

                if (Rtemp) {
                    GPIOCtrl(3, 0x20, Operation);
                }
            } else {
                GPIOCtrl(3, 0x20, Operation);
            }
            break;

        case S3:

            if (Operation) {
                //GPIOE (4, 1)
                Rtemp  = GPIOChk(4, 0x2, Operation);

                if (Rtemp) {
                    GPIOCtrl(4, 0x2, Operation);
                }
            } else {
                GPIOCtrl(4, 0x2, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        PWM1_PIN_SEL(PinLocation);
        //Enable function pin
        PWM1_PIN_FCTRL(Operation);
    } else {
        if (!Operation) {
            PWM1_PIN_SEL(S0);
            //Enable function pin
            PWM1_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalPWM2PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOB (1, 6)
                Rtemp  = GPIOChk(1, 0x40, Operation);

                if (Rtemp) {
                    GPIOCtrl(1, 0x40, Operation);
                }
            } else {
                GPIOCtrl(1, 0x40, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOC (2, 2)
                Rtemp  = GPIOChk(2, 0x4, Operation);

                if (Rtemp) {
                    GPIOCtrl(2, 0x4, Operation);
                }
            } else {
                GPIOCtrl(2, 0x4, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOD (3, 6)
                Rtemp  = GPIOChk(3, 0x40, Operation);

                if (Rtemp) {
                    GPIOCtrl(3, 0x40, Operation);
                }
            } else {
                GPIOCtrl(3, 0x40, Operation);
            }
            break;

        case S3:

            if (Operation) {
                //GPIOE (4, 2)
                Rtemp  = GPIOChk(4, 0x4, Operation);

                if (Rtemp) {
                    GPIOCtrl(4, 0x4, Operation);
                }
            } else {
                GPIOCtrl(4, 0x4, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        PWM2_PIN_SEL(PinLocation);
        //Enable function pin
        PWM2_PIN_FCTRL(ON);
    } else {
        if (!Operation) {
            PWM2_PIN_SEL(S0);
            //Enable function pin
            PWM2_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalPWM3PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOB (1, 7)
                Rtemp  = GPIOChk(1, 0x80, Operation);

                if (Rtemp) {
                    GPIOCtrl(1, 0x80, Operation);
                }
            } else {
                GPIOCtrl(1, 0x80, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOC (2, 3)
                Rtemp  = GPIOChk(2, 0x8, Operation);

                if (Rtemp) {
                    GPIOCtrl(2, 0x8, Operation);
                }
            } else {
                GPIOCtrl(2, 0x8, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOD (3, 7)
                Rtemp  = GPIOChk(3, 0x80, Operation);

                if (Rtemp) {
                    GPIOCtrl(3, 0x80, Operation);
                }
            } else {
                GPIOCtrl(3, 0x80, Operation);
            }
            break;

        case S3:

            if (Operation) {
                //GPIOE (4, 3)
                Rtemp  = GPIOChk(4, 0x8, Operation);

                if (Rtemp) {
                    GPIOCtrl(4, 0x8, Operation);
                }
            } else {
                GPIOCtrl(4, 0x8, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        PWM3_PIN_SEL(PinLocation);
        //Enable function pin
        PWM3_PIN_FCTRL(Operation);
    } else {
        if (!Operation) {
            PWM3_PIN_SEL(S0);
            //Enable function pin
            PWM3_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalETE0PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOB (1, 0)
                Rtemp  = GPIOChk(1, 0x1, Operation);

                if (Rtemp) {
                    GPIOCtrl(1, 0x1, Operation);
                }
            } else {
                GPIOCtrl(1, 0x1, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOC (2, 0)
                Rtemp  = GPIOChk(2, 0x1, Operation);

                if (Rtemp) {
                    GPIOCtrl(2, 0x1, Operation);
                }
            } else {
                GPIOCtrl(2, 0x1, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOD (3, 4)
                Rtemp  = GPIOChk(3, 0x10, Operation);

                if (Rtemp) {
                    GPIOCtrl(3, 0x10, Operation);
                }
            } else {
                GPIOCtrl(3, 0x10, Operation);
            }
            break;

        case S3:

            if (Operation) {
                //GPIOH (7, 0)
                Rtemp  = GPIOChk(7, 0x1, Operation);

                if (Rtemp) {
                    GPIOCtrl(7, 0x1, Operation);
                }
            } else {
                GPIOCtrl(7, 0x1, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        ETE0_PIN_SEL(PinLocation);
        //Enable function pin
        ETE0_PIN_FCTRL(Operation);
    } else {
        if (!Operation) {
            ETE0_PIN_SEL(S0);
            //Enable function pin
            ETE0_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalETE1PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOB (1, 1)
                Rtemp  = GPIOChk(1, 0x2, Operation);

                if (Rtemp) {
                    GPIOCtrl(1, 0x2, Operation);
                }
            } else {
                GPIOCtrl(1, 0x2, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOC (2, 1)
                Rtemp  = GPIOChk(2, 0x2, Operation);

                if (Rtemp) {
                    GPIOCtrl(2, 0x2, Operation);
                }
            } else {
                GPIOCtrl(2, 0x2, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOD (3, 5)
                Rtemp  = GPIOChk(3, 0x20, Operation);

                if (Rtemp) {
                    GPIOCtrl(3, 0x20, Operation);
                }
            } else {
                GPIOCtrl(3, 0x20, Operation);
            }
            break;

        case S3:

            if (Operation) {
                //GPIOH (7, 1)
                Rtemp  = GPIOChk(7, 0x2, Operation);

                if (Rtemp) {
                    GPIOCtrl(7, 0x2, Operation);
                }
            } else {
                GPIOCtrl(7, 0x2, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        ETE1_PIN_SEL(PinLocation);
        //Enable function pin
        ETE1_PIN_FCTRL(Operation);
    } else {
        if (!Operation) {
            ETE1_PIN_SEL(S0);
            //Enable function pin
            ETE1_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalETE2PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOB (1, 2)
                Rtemp  = GPIOChk(1, 0x4, Operation);

                if (Rtemp) {
                    GPIOCtrl(1, 0x4, Operation);
                }
            } else {
                GPIOCtrl(1, 0x4, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOC (2, 2)
                Rtemp  = GPIOChk(2, 0x4, Operation);

                if (Rtemp) {
                    GPIOCtrl(2, 0x4, Operation);
                }
            } else {
                GPIOCtrl(2, 0x4, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOD (3, 6)
                Rtemp  = GPIOChk(3, 0x40, Operation);

                if (Rtemp) {
                    GPIOCtrl(3, 0x40, Operation);
                }
            } else {
                GPIOCtrl(3, 0x40, Operation);
            }
            break;

        case S3:

            if (Operation) {
                //GPIOH (7, 2)
                Rtemp  = GPIOChk(7, 0x4, Operation);

                if (Rtemp) {
                    GPIOCtrl(7, 0x4, Operation);
                }
            } else {
                GPIOCtrl(7, 0x4, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        ETE2_PIN_SEL(PinLocation);
        //Enable function pin
        ETE2_PIN_FCTRL(Operation);
    } else {
        if (!Operation) {
            ETE2_PIN_SEL(S0);
            //Enable function pin
            ETE2_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalETE3PinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOB (1, 3)
                Rtemp  = GPIOChk(1, 0x8, Operation);

                if (Rtemp) {
                    GPIOCtrl(1, 0x8, Operation);
                }
            } else {
                GPIOCtrl(1, 0x8, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOC (2, 3)
                Rtemp  = GPIOChk(2, 0x8, Operation);

                if (Rtemp) {
                    GPIOCtrl(2, 0x8, Operation);
                }
            } else {
                GPIOCtrl(2, 0x8, Operation);
            }
            break;

        case S2:

            if (Operation) {
                //GPIOD (3, 7)
                Rtemp  = GPIOChk(3, 0x80, Operation);

                if (Rtemp) {
                    GPIOCtrl(3, 0x80, Operation);
                }
            } else {
                GPIOCtrl(3, 0x80, Operation);
            }
            break;

        case S3:

            if (Operation) {
                //GPIOH (7, 3)
                Rtemp  = GPIOChk(7, 0x8, Operation);

                if (Rtemp) {
                    GPIOCtrl(7, 0x8, Operation);
                }
            } else {
                GPIOCtrl(7, 0x8, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        ETE3_PIN_SEL(PinLocation);
        //Enable function pin
        ETE3_PIN_FCTRL(Operation);
    } else {
        if (!Operation) {
            ETE3_PIN_SEL(S0);
            //Enable function pin
            ETE3_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalEGTIMPinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    if (Operation) {
        //GPIOB(1,5), GPIOC (2, 0:7), GPIOE (4, 0:9),
        Rtemp  = GPIOChk(1, 0x20, Operation);
        Rtemp &= GPIOChk(2, 0xFF, Operation);
        Rtemp &= GPIOChk(4, 0x3FF, Operation);
    }

    if (Rtemp) {
        //Enable GPIO
        GPIOCtrl(1, 0x20, Operation);
        GPIOCtrl(2, 0xFF, Operation);
        GPIOCtrl(4, 0x3FF, Operation);
        //Enable function pin
        EGTIM_FCTRL(Operation);
    } else {
        if (!Operation) {
            GPIOCtrl(1, 0x20, OFF);
            GPIOCtrl(2, 0xFF, OFF);
            GPIOCtrl(4, 0x3FF, OFF);
            //Enable function pin
            EGTIM_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalSPIFlashPinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    switch ( PinLocation ) {

        case S0:

            if (Operation) {
                //GPIOF (5, 0:5),
                Rtemp  = GPIOChk(5, 0x3F, Operation);

                if (Rtemp) {
                    //Enable GPIO
                    GPIOCtrl(5, 0x3F, Operation);
                }
            } else {
                GPIOCtrl(5, 0x3F, Operation);
            }
            break;

        case S1:

            if (Operation) {
                //GPIOC (2, 0:3),
                Rtemp  = GPIOChk(2, 0x0F, Operation);

                if (Rtemp) {
                    //Enable GPIO
                    GPIOCtrl(2, 0x0F, Operation);
                }
            } else {
                GPIOCtrl(2, 0x0F, Operation);
            }
            break;
    }

    if (Rtemp) {
        //pin sel
        SPI_FLASH_PIN_SEL(PinLocation);
        //Enable function pin
        SPI_FLASH_PIN_FCTRL(Operation);
    } else {
        if (!Operation) {
            SPI_FLASH_PIN_SEL(S0);
            //Enable function pin
            SPI_FLASH_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalSDRPinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    if (Operation) {
        //GPIOG (6, 0:7),  GPIOH (7, 0:7), GPIOI (8, 0:7), GPIOJ (9, 0:6), GPIOK (10, 0:6),
        Rtemp  = GPIOChk(6, 0xFF, Operation);
        Rtemp &= GPIOChk(7, 0xFF, Operation);
        Rtemp &= GPIOChk(8, 0xFF, Operation);
        Rtemp &= GPIOChk(9, 0x7F, Operation);
        Rtemp &= GPIOChk(10, 0x7F, Operation);
    }

    if (Rtemp) {
        //Enable GPIO
        GPIOCtrl(6, 0xFF, Operation);
        GPIOCtrl(7, 0xFF, Operation);
        GPIOCtrl(8, 0xFF, Operation);
        GPIOCtrl(9, 0x7F, Operation);
        GPIOCtrl(10, 0x7F, Operation);
        //Enable function pin
        SDR_PIN_FCTRL(Operation);
    } else {
        if (!Operation) {
            GPIOCtrl(6, 0xFF, OFF);
            GPIOCtrl(7, 0xFF, OFF);
            GPIOCtrl(8, 0xFF, OFF);
            GPIOCtrl(9, 0x7F, OFF);
            GPIOCtrl(10, 0x7F, OFF);
            //Enable function pin
            SDR_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalJTAGPinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    if (Operation) {
        //GPIOE (4, 0:4),
        Rtemp  = GPIOChk(4, 0x1F, Operation);
    }

    if (Rtemp) {
        //Enable GPIO
        GPIOCtrl(4, 0x1F, Operation);
        //Enable function pin
        JTAG_PIN_FCTRL(Operation);
    } else {
        if (!Operation) {
            GPIOCtrl(4, 0x1F, OFF);
            //Enable function pin
            JTAG_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalTRACEPinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    if (Operation) {
        //GPIOE (4, 5:9),
        Rtemp  = GPIOChk(4, 0x3E0, Operation);
    }

    if (Rtemp) {
        //Enable GPIO
        GPIOCtrl(4, 0x3E0, Operation);
        //Enable function pin
        TRACE_PIN_FCTRL(Operation);
    } else {
        if (!Operation) {
            GPIOCtrl(4, 0x3E0, OFF);
            //Enable function pin
            TRACE_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalLOGUartPinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    if (Operation) {
        //GPIOB (1, 0:1),
        Rtemp  = GPIOChk(1, 0x03, Operation);
    }

    if (Rtemp) {
        //Enable GPIO
        GPIOCtrl(1, 0x03, Operation);
        //Enable function pin
        LOG_UART_IR_FCTRL(OFF);
        LOG_UART_PIN_FCTRL(Operation);
    } else {
        if (!Operation) {
            GPIOCtrl(1, 0x03, OFF);
            //Enable function pin
            LOG_UART_IR_FCTRL(OFF);
            LOG_UART_PIN_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalLOGUartIRPinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    if (Operation) {
        //GPIOB (1, 0:1),
        Rtemp  = GPIOChk(1, 0x03, Operation);
    }

    if (Rtemp) {
        //Enable GPIO
        GPIOCtrl(1, 0x03, Operation);
        //Enable function pin
        LOG_UART_PIN_FCTRL(Operation);
        LOG_UART_IR_FCTRL(Operation);
    } else {
        if (!Operation) {
            GPIOCtrl(1, 0x03, OFF);
            //Enable function pin
            LOG_UART_PIN_FCTRL(OFF);
            LOG_UART_IR_FCTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalSICPinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    if (Operation) {
        //GPIOC (2, 4:5),
        Rtemp  = GPIOChk(2, 0x30, Operation);
    }

    if (Rtemp) {
        //Enable GPIO
        GPIOCtrl(2, 0x30, Operation);
        //Enable function pin
        SIC_PIN_CTRL(Operation);
    } else {
        if (!Operation) {
            GPIOCtrl(2, 0x30, OFF);
            //Enable function pin
            SIC_PIN_CTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalEEPROMPinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    if (Operation) {
        //GPIOC (2, 0:3),
        Rtemp  = GPIOChk(2, 0x0F, Operation);
    }

    if (Rtemp) {
        //Enable GPIO
        GPIOCtrl(2, 0x0F, Operation);
        //Enable function pin
        EEPROM_PIN_CTRL(Operation);
    } else {
        if (!Operation) {
            GPIOCtrl(2, 0x0F, OFF);
            //Enable function pin
            EEPROM_PIN_CTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalDEBUGPinCtrlRtl8195A(
    IN u32  PinLocation,
    IN BOOL   Operation)
{
    u8 Rtemp = 0;

    if (Operation) {
        //GPIOA(0, 0:7), GPIOB(1, 0:5), GPIOC(2, 0:7), GPIOD(3,4:5), GPIOE(4, 0:5), GPIOF(5,4:5),
        Rtemp  = GPIOChk(0, 0xFF, Operation);
        Rtemp &= GPIOChk(1, 0x3F, Operation);
        Rtemp &= GPIOChk(2, 0xFF, Operation);
        Rtemp &= GPIOChk(3, 0x30, Operation);
        Rtemp &= GPIOChk(4, 0x3F, Operation);
        Rtemp &= GPIOChk(5, 0x30, Operation);
    }

    if (Rtemp) {
        //Enable GPIO
        GPIOCtrl(0, 0xFF, Operation);
        GPIOCtrl(1, 0x3F, Operation);
        GPIOCtrl(2, 0xFF, Operation);
        GPIOCtrl(3, 0x30, Operation);
        GPIOCtrl(4, 0x3F, Operation);
        GPIOCtrl(5, 0x30, Operation);
        //Enable function pin
        DEBUG_PIN_CTRL(Operation);
    } else {
        if (!Operation) {
            GPIOCtrl(0, 0xFF, OFF);
            GPIOCtrl(1, 0x3F, OFF);
            GPIOCtrl(2, 0xFF, OFF);
            GPIOCtrl(3, 0x30, OFF);
            GPIOCtrl(4, 0x3F, OFF);
            GPIOCtrl(5, 0x30, OFF);
            //Enable function pin
            DEBUG_PIN_CTRL(OFF);
        } else {
            return _FALSE;
        }
    }
    return _TRUE;
}

PINMUX_ROM_TEXT_SECTION
u8
HalPinCtrlRtl8195A(
    IN u32  Function,
    IN u32  PinLocation,
    IN BOOL   Operation)
{

    u8 Rtemp = 0;

    switch ( Function ) {

        case UART0:
            Rtemp = HalUart0PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case UART1:
            Rtemp = HalUart1PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case UART2:
            Rtemp = HalUart2PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case SPI0:
            Rtemp = HalSPI0PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case SPI1:
            Rtemp = HalSPI1PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case SPI2:
            Rtemp = HalSPI2PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case SPI0_MCS:
            Rtemp = HalSPI0MCSPinCtrlRtl8195A(PinLocation,Operation);
            break;

        case I2C0:
            Rtemp = HalI2C0PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case I2C1:
            Rtemp = HalI2C1PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case I2C2:
            Rtemp = HalI2C2PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case I2C3:
            Rtemp = HalI2C3PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case I2S0:
            Rtemp = HalI2S0PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case I2S1:
            Rtemp = HalI2S1PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case PCM0:
            Rtemp = HalPCM0PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case PCM1:
            Rtemp = HalPCM1PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case SDIOD:
            Rtemp = HalSDIODPinCtrlRtl8195A(PinLocation,Operation);
            break;

        case SDIOH:
            Rtemp = HalSDIOHPinCtrlRtl8195A(PinLocation,Operation);
            break;

        case MII:
            Rtemp = HalMIIPinCtrlRtl8195A(PinLocation,Operation);
            break;

        case WL_LED:
            Rtemp = HalWLLEDPinCtrlRtl8195A(PinLocation,Operation);
            break;

        case WL_ANT0:
            Rtemp = HalWLANT0PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case WL_ANT1:
            Rtemp = HalWLANT1PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case WL_BTCOEX:
            Rtemp = HalWLBTCOEXPinCtrlRtl8195A(PinLocation,Operation);
            break;

        case WL_BTCMD:
            Rtemp = HalWLBTCMDPinCtrlRtl8195A(PinLocation,Operation);
            break;

        case NFC:
            Rtemp = HalNFCPinCtrlRtl8195A(PinLocation,Operation);
            break;

        case PWM0:
            Rtemp = HalPWM0PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case PWM1:
            Rtemp = HalPWM1PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case PWM2:
            Rtemp = HalPWM2PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case PWM3:
            Rtemp = HalPWM3PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case ETE0:
            Rtemp = HalETE0PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case ETE1:
            Rtemp = HalETE1PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case ETE2:
            Rtemp = HalETE2PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case ETE3:
            Rtemp = HalETE3PinCtrlRtl8195A(PinLocation,Operation);
            break;

        case EGTIM:
            Rtemp = HalEGTIMPinCtrlRtl8195A(PinLocation,Operation);
            break;

        case SPI_FLASH:
            Rtemp = HalSPIFlashPinCtrlRtl8195A(PinLocation,Operation);
            break;

        case SDR:
            Rtemp = HalSDRPinCtrlRtl8195A(PinLocation,Operation);
            break;

        case JTAG:
            Rtemp = HalJTAGPinCtrlRtl8195A(PinLocation,Operation);
            break;

        case TRACE:
            Rtemp = HalTRACEPinCtrlRtl8195A(PinLocation,Operation);
            break;

        case LOG_UART:
            Rtemp = HalLOGUartPinCtrlRtl8195A(PinLocation,Operation);
            break;

        case LOG_UART_IR:
            Rtemp = HalLOGUartIRPinCtrlRtl8195A(PinLocation,Operation);
            break;

        case SIC:
            Rtemp = HalSICPinCtrlRtl8195A(PinLocation,Operation);
            break;

        case EEPROM:
            Rtemp = HalEEPROMPinCtrlRtl8195A(PinLocation,Operation);
            break;

        case DEBUG:
            Rtemp = HalDEBUGPinCtrlRtl8195A(PinLocation,Operation);
            break;
    }
    return Rtemp;
}

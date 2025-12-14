#include "section_config.h"
#include "hal_api.h"


PINMUX_RAM_BSS_SECTION
u16 GPIOState[11] = {0,0,0,0,0,0,0,0,0,0,0};

PINMUX_ROM_TEXT_SECTION
static u8 
GPIOChk(
    IN u8   GPIOPortIdx,
    IN u16  GPIOIdx,
    IN u8   Operation)
{
    if (Operation) {
        
        if (GPIOState[GPIOPortIdx] & GPIOIdx){
            
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
    }
    else {

        GPIOState[GPIOPortIdx] &= ~GPIOIdx;

        if (!GPIOState[GPIOPortIdx]) {
            HAL_PERI_ON_WRITE32(REG_GPIO_SHTDN_CTRL, (HAL_PERI_ON_READ32(REG_GPIO_SHTDN_CTRL)& (~BIT(GPIOPortIdx))));
        }
    }
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
            //Enable GPIO
            switch ( PinLocation ) {
                
                case DW_MUX_0:
                    //GPIOC (2, 0:3)
                    Rtemp = GPIOChk(2, 0xf, Operation);
                    
                    if (Rtemp) {
                        GPIOCtrl(2, 0xf, Operation);
                    }
                    break;
                    
                case DW_MUX_1:
                    //GPIOE (4, 0:3)
                    Rtemp = GPIOChk(4, 0xf, Operation);
                    
                    if (Rtemp) {
                        GPIOCtrl(4, 0xf, Operation);
                    }
                    break;
                    
                case DW_MUX_2:
                    //GPIOK(10, 4:6),  GPIOG(6, 7)
                    Rtemp  = GPIOChk(10, 0x70, Operation);
                    Rtemp &= GPIOChk(6, 0x80, Operation);
                    
                    if (Rtemp) {
                        GPIOCtrl(10, 0x70, Operation);
                        GPIOCtrl(6, 0x80, Operation);
                    }
                    break;        
            }
            
            if (Rtemp) {
                //pin sel
                UART0_PIN_SEL(PinLocation);
                //Enable function pin
                UART0_PIN_CTRL(Operation);
            }
            break;
            
         case UART1:
            //Enable GPIO
            switch ( PinLocation ) {
                
                case DW_MUX_0:
                    //GPIOD (3, 0:3)
                    Rtemp = GPIOChk(3, 0xf, Operation);
                    
                    if (Rtemp) {
                        GPIOCtrl(3, 0xf, Operation);
                    }
                    break;
                    
                case DW_MUX_1:
                    //GPIOE (4, 4:7)
                    Rtemp = GPIOChk(4, 0xf0, Operation);
                    
                    if (Rtemp) {
                        GPIOCtrl(4, 0xf0, Operation);
                    }
                    break;
                    
                case DW_MUX_2:
                    //GPIOH (7, 4:7)
                    Rtemp = GPIOChk(7, 0xf0, Operation);
                    
                    if (Rtemp) {
                        GPIOCtrl(7, 0xf0, Operation);
                    }
                    break;        
            }
            
            if (Rtemp) {
                //pin sel
                UART1_PIN_SEL(PinLocation);
                //Enable function pin
                UART1_PIN_CTRL(Operation);
            }
            break;

        case UART2:
            //Enable GPIO
            switch ( PinLocation ) {
                
                case DW_MUX_0:
                    //GPIOA (0, 0:2, 4)
                    Rtemp  = GPIOChk(0, 0x7, Operation);
                    Rtemp &= GPIOChk(0, 0x10, Operation);
                    
                    if (Rtemp) {
                        GPIOCtrl(0, 0x7, Operation);
                        GPIOCtrl(0, 0x10, Operation);
                    }
                    break;
                    
                case DW_MUX_1:
                    //GPIOI (8, 2:5)
                    Rtemp = GPIOChk(8, 0x3c, Operation);
                    
                    if (Rtemp) {
                        GPIOCtrl(8, 0x3c, Operation);
                    }
                    break;   
            }
            
            if (Rtemp) {
                //pin sel
                UART2_PIN_SEL(PinLocation);
                //Enable function pin
                UART2_PIN_CTRL(Operation);
            }
            break;

        case SPI0:
            //Enable GPIO
            switch ( PinLocation ) {
                
                case DW_MUX_0:
                    //GPIOE (4, 0:3)
                    Rtemp  = GPIOChk(4, 0x0f, Operation);
                    
                    if (Rtemp) {
                        GPIOCtrl(4, 0x0f, Operation);
                    }
                    break;
                    
                case DW_MUX_1:
                    //GPIOC (2, 0:3)
                    Rtemp = GPIOChk(2, 0x0f, Operation);
                    
                    if (Rtemp) {
                        GPIOCtrl(2, 0x0f, Operation);
                    }
                    break;  
                    
                case DW_MUX_2:
                    //GPIOI (8, 2:5)
                    Rtemp = GPIOChk(8, 0x3c, Operation);
                    
                    if (Rtemp) {
                        GPIOCtrl(8, 0x3c, Operation);
                    }
                    break; 
            }
            
            if (Rtemp) {
                //pin sel
                SPI0_PIN_SEL(PinLocation);
                //Enable function pin
                SPI0_PIN_CTRL(Operation);
            }
            break;        

        case SPI1:
            //Enable GPIO
            switch ( PinLocation ) {
                
                case DW_MUX_0:
                    //GPIOA (0, 0:2, 4)
                    Rtemp  = GPIOChk(0, 0x7, Operation);
                    Rtemp &= GPIOChk(0, 0x10, Operation);

                    if (Rtemp) {
                        GPIOCtrl(0, 0x7, Operation);
                        GPIOCtrl(0, 0x10, Operation);
                    }
                    break;
                    
                case DW_MUX_1:
                    //GPIOB (1, 4:7)
                    Rtemp = GPIOChk(1, 0xf0, Operation);

                    if (Rtemp) {
                        GPIOCtrl(1, 0xf0, Operation);
                    }
                    break;  
                    
                case DW_MUX_2:
                    //GPIOD (3, 4:5)   GPIOE(4, 9:10)
                    Rtemp  = GPIOChk(3, 0x30, Operation);
                    Rtemp &= GPIOChk(4, 0x600, Operation);

                    if (Rtemp) {
                        GPIOCtrl(3, 0x30, Operation);
                        GPIOCtrl(4, 0x600, Operation);
                    }
                    break; 
            }
            
            if (Rtemp) {
                //pin sel
                SPI1_PIN_SEL(PinLocation);
                //Enable function pin
                SPI1_PIN_CTRL(Operation);
            }
            break; 

        case SPI2:
            //Enable GPIO
            switch ( PinLocation ) {
                
                case DW_MUX_0:
                    //GPIOG (6, 0:3)
                    Rtemp = GPIOChk(6, 0xf, Operation);

                    if (Rtemp) {
                        GPIOCtrl(6, 0xf, Operation);
                    }
                    break;
                    
                case DW_MUX_1:
                    //GPIOE (4, 4:7)
                    Rtemp = GPIOChk(4, 0xf0, Operation);

                    if (Rtemp) {
                        GPIOCtrl(4, 0xf0, Operation);
                    }
                    break;  
                    
                case DW_MUX_2:
                    //GPIOD (3, 0:3) 
                    Rtemp = GPIOChk(3, 0x0f, Operation);

                    if (Rtemp) {
                        GPIOCtrl(3, 0x0f, Operation);
                    }
                    break; 
            }
            
            if (Rtemp) {
                //pin sel
                SPI2_PIN_SEL(PinLocation);
                //Enable function pin
                SPI2_PIN_CTRL(Operation);
            }
            break;

        case SPI0_MCS:             
            //Enable GPIO
            switch ( PinLocation ) {
                case DW_MUX_0:
                    //GPIOE (4, 0:3),  GPIOE (4, 4:A) 
                    Rtemp  = GPIOChk(4, 0x0f, Operation);
                    Rtemp &= GPIOChk(4, 0x7f0, Operation);

                    if (Rtemp) {
                        GPIOCtrl(4, 0x0f, Operation);
                        GPIOCtrl(4, 0x7f0, Operation);
                    }
                    break;
                    
                case DW_MUX_2:
                    //GPIOI (8, 2:7),  GPIOJ (9, 0), GPIOK (10, 0:3)  
                    Rtemp  = GPIOChk(8, 0xfc, Operation);
                    Rtemp &= GPIOChk(9, 0x01, Operation);
                    Rtemp &= GPIOChk(10, 0x0f, Operation);

                    if (Rtemp) {
                        GPIOCtrl(8, 0xfc, Operation);
                        GPIOCtrl(9, 0x01, Operation);
                        GPIOCtrl(10, 0x0f, Operation);
                    }
                    break; 
            }
            
            if (Rtemp) {
                //pin sel
                SPI0_PIN_SEL(PinLocation);
                //Enable function pin
                SPI0_PIN_CTRL(Operation);
                SPI0_MULTI_CS_CTRL(Operation);
            }
            break;

        case I2C0:
            //Enable GPIO
            switch ( PinLocation ) {
                case DW_MUX_0:
                    //GPIOD (3, 4:5)
                    Rtemp = GPIOChk(3, 0x30, Operation);

                    if (Rtemp) {
                        GPIOCtrl(3, 0x30, Operation);
                    }
                    break;
                    
                case DW_MUX_1:
                    //GPIOH (7, 0:1)
                    Rtemp = GPIOChk(7, 0x03, Operation);

                    if (Rtemp) {
                        GPIOCtrl(7, 0x03, Operation);
                    }
                    break;  
                    
                case DW_MUX_2:
                    //GPIOI (8, 0:1) 
                    Rtemp = GPIOChk(8, 0x03, Operation);

                    if (Rtemp) {
                        GPIOCtrl(8, 0x03, Operation);
                    }
                    break; 
                    
                case DW_MUX_3:
                    //GPIOE (4, 6:7) 
                    Rtemp = GPIOChk(4, 0xc0, Operation);

                    if (Rtemp) {
                        GPIOCtrl(4, 0xc0, Operation);
                    }
                    break; 
            }

            if (Rtemp) {
                //pin sel
                I2C0_PIN_SEL(PinLocation);
                //Enable function pin
                I2C0_PIN_CTRL(Operation);
            }
            break;

        case I2C1:
            //Enable GPIO
            switch ( PinLocation ) {
                case DW_MUX_0:
                    //GPIOC (2, 4:5)
                    Rtemp = GPIOChk(2, 0x30, Operation);

                    if (Rtemp) {
                        GPIOCtrl(2, 0x30, Operation);
                    }
                    break;
                    
                case DW_MUX_1:
                    //GPIOH (7, 2:3)
                    Rtemp = GPIOChk(7, 0x0c, Operation);

                    if (Rtemp) {
                        GPIOCtrl(7, 0x0c, Operation);
                    }
                    break;  
                    
                case DW_MUX_2:
                    //GPIOJ (9, 1:2) 
                    Rtemp = GPIOChk(9, 0x06, Operation);

                    if (Rtemp) {
                        GPIOCtrl(9, 0x06, Operation);
                    }
                    break; 
            }

            if (Rtemp) {
                //pin sel
                I2C1_PIN_SEL(PinLocation);
                //Enable function pin
                I2C1_PIN_CTRL(Operation);
            }
            break;

        case I2C2:
            //Enable GPIO
            switch ( PinLocation ) {
                case DW_MUX_0:
                    //GPIOB (1, 6:7)
                    Rtemp = GPIOChk(1, 0xc0, Operation);

                    if (Rtemp) {
                        GPIOCtrl(1, 0xc0, Operation);
                    }
                    break;
                    
                case DW_MUX_1:
                    //GPIOE (4, 0:1)
                    Rtemp = GPIOChk(4, 0x03, Operation);

                    if (Rtemp) {
                        GPIOCtrl(4, 0x03, Operation);
                    }
                    break;  
                    
                case DW_MUX_2:
                    //GPIOJ (9, 5:6) 
                    Rtemp = GPIOChk(9, 0x60, Operation);

                    if (Rtemp) {
                        GPIOCtrl(9, 0x60, Operation);
                    }
                    break; 
            }

            if (Rtemp) {
                //pin sel
                I2C2_PIN_SEL(PinLocation);
                //Enable function pin
                I2C2_PIN_CTRL(Operation);
            }
            break;

        case I2C3:
            //Enable GPIO
            switch ( PinLocation ) {
                case DW_MUX_0:
                    //GPIOB (1, 2:3)
                    Rtemp = GPIOChk(1, 0x0c, Operation);

                    if (Rtemp) {
                        GPIOCtrl(1, 0x0c, Operation);
                    }
                    break;
                    
                case DW_MUX_1:
                    //GPIOE (4, 2:3)
                    Rtemp = GPIOChk(4, 0x0c, Operation);

                    if (Rtemp) {
                        GPIOCtrl(4, 0x0c, Operation);
                    }
                    break;  
                    
                case DW_MUX_2:
                    //GPIOE (4, 4:5) 
                    Rtemp = GPIOChk(4, 0x30, Operation);

                    if (Rtemp) {
                        GPIOCtrl(4, 0x30, Operation);
                    }
                    break; 
                    
                case DW_MUX_3:
                    //GPIOI (8, 7),  GPIOJ (9, 0) 
                    Rtemp  = GPIOChk(8, 0x80, Operation);
                    Rtemp &= GPIOChk(9, 0x01, Operation);

                    if (Rtemp) {
                        GPIOCtrl(8, 0x80, Operation);
                        GPIOCtrl(9, 0x01, Operation);
                    }
                    break; 
            }

            if (Rtemp) {
                //pin sel
                I2C3_PIN_SEL(PinLocation);
                //Enable function pin
                I2C3_PIN_CTRL(Operation);
            }
            break;

        case I2S0:
            //Enable GPIO
            switch ( PinLocation ) {
                case DW_MUX_0:
                    //GPIOE (4, 0:3),  GPIOD (3, 3) 
                    Rtemp  = GPIOChk(4, 0x0f, Operation);
                    Rtemp &= GPIOChk(3, 0x08, Operation);

                    if (Rtemp) {
                        GPIOCtrl(4, 0x0f, Operation);
                        GPIOCtrl(3, 0x08, Operation);
                    }
                    break;
                    
                case DW_MUX_1:
                    //GPIOH (7, 0:3),  GPIOH (7, 5) 
                    Rtemp  = GPIOChk(7, 0x0f, Operation);
                    Rtemp &= GPIOChk(7, 0x20, Operation);

                    if (Rtemp) {
                        GPIOCtrl(7, 0x0f, Operation);
                        GPIOCtrl(7, 0x20, Operation);
                    }
                    break;  
                    
                case DW_MUX_2:
                    //GPIOK (10, 0:3),  GPIOJ (9, 6) 
                    Rtemp  = GPIOChk(10, 0x0f, Operation);
                    Rtemp &= GPIOChk(9, 0x40, Operation);

                    if (Rtemp) {
                        GPIOCtrl(10, 0x0f, Operation);
                        GPIOCtrl(9, 0x40, Operation);
                    }
                    break; 
            }

            if (Rtemp) {
                //pin sel
                I2S0_PIN_SEL(PinLocation);
                //Enable function pin
                I2S0_PIN_CTRL(Operation);
            }
            break;

        case I2S1:
            //Enable GPIO
            switch ( PinLocation ) {
                case DW_MUX_0:
                    //GPIOC (2, 0:4)
                    Rtemp  = GPIOChk(2, 0x1f, Operation);

                    if (Rtemp) {
                        GPIOCtrl(2, 0x1f, Operation);
                    }
                    break;
                    
                case DW_MUX_1:
                    //GPIOD (3, 4:5),  GPIOE (4, 8:10) 
                    Rtemp  = GPIOChk(3, 0x30, Operation);
                    Rtemp &= GPIOChk(4, 0x700, Operation);

                    if (Rtemp) {
                        GPIOCtrl(3, 0x30, Operation);
                        GPIOCtrl(4, 0x700, Operation);
                    }
                    break;  
            }

            if (Rtemp) {
                //pin sel
                I2S1_PIN_SEL(PinLocation);
                //Enable function pin
                I2S1_PIN_CTRL(Operation);
            }
            break;

        case PCM0:
            //Enable GPIO
            switch ( PinLocation ) {
                case DW_MUX_0:
                    //GPIOH (7, 0:3)
                    Rtemp  = GPIOChk(7, 0x0f, Operation);

                    if (Rtemp) {
                        GPIOCtrl(7, 0x0f, Operation);
                    }
                    break;
                    
                case DW_MUX_1:
                    //GPIOJ (9, 1:2),  GPIOJ (9, 4:5) 
                    Rtemp  = GPIOChk(9, 0x06, Operation);
                    Rtemp &= GPIOChk(9, 0x30, Operation);

                    if (Rtemp) {
                        GPIOCtrl(9, 0x06, Operation);
                        GPIOCtrl(9, 0x30, Operation);
                    }
                    break;
                    
                case DW_MUX_2:
                    //GPIOK (10, 0:3)
                    Rtemp  = GPIOChk(10, 0x0f, Operation);

                    if (Rtemp) {
                        GPIOCtrl(10, 0x0f, Operation);
                    }
                    break; 
            }

            if (Rtemp) {
                //pin sel
                PCM0_PIN_SEL(PinLocation);
                //Enable function pin
                PCM0_PIN_CTRL(Operation);
            }
            break;

        case PCM1:
            //Enable GPIO
            switch ( PinLocation ) {
                case DW_MUX_0:
                    //GPIOC (2, 0:3)
                    Rtemp  = GPIOChk(2, 0x0f, Operation);

                    if (Rtemp) {
                        GPIOCtrl(2, 0x0f, Operation);
                    }
                    break;
                    
                case DW_MUX_1:
                    //GPIOD (3, 4:5),  GPIOE (4, 9:10) 
                    Rtemp  = GPIOChk(3, 0x30, Operation);
                    Rtemp &= GPIOChk(4, 0x600, Operation);

                    if (Rtemp) {
                        GPIOCtrl(3, 0x30, Operation);
                        GPIOCtrl(4, 0x600, Operation);
                    }
                    break;
            }

            if (Rtemp) {
                //pin sel
                PCM1_PIN_SEL(PinLocation);
                //Enable function pin
                PCM1_PIN_CTRL(Operation);
            }
            break;

        case SDIOD:
            //GPIOA (0, 0:5)
            Rtemp  = GPIOChk(0, 0x3f, Operation);
            
            if (Rtemp) {
                //Enable GPIO
                GPIOCtrl(0, 0x3f, Operation);
                //Enable function pin
                SDIOD_PIN_FCTRL(Operation);
            }
            break;

        case SDIOH:
            //GPIOA (0, 0:5)
            Rtemp  = GPIOChk(0, 0xff, Operation);
            
            if (Rtemp) {
                //Enable GPIO
                GPIOCtrl(0, 0xff, Operation);
                //Enable function pin
                SDIOH_PIN_FCTRL(Operation);
            }          
            break;
            
        case WL_LED:
            //Enable GPIO
            switch ( PinLocation ) {
                case DW_MUX_0:
                    //GPIOB (1, 5)
                    Rtemp  = GPIOChk(1, 0x20, Operation);

                    if (Rtemp) {
                        GPIOCtrl(1, 0x20, Operation);
                    }
                    break;
                    
                case DW_MUX_1:
                    //GPIOB (1, 4) 
                    Rtemp  = GPIOChk(1, 0x10, Operation);

                    if (Rtemp) {
                        GPIOCtrl(1, 0x10, Operation);
                    }
                    break;
                    
                case DW_MUX_2:
                    //GPIOB (1, 1)
                    Rtemp  = GPIOChk(1, 0x02, Operation);

                    if (Rtemp) {
                        GPIOCtrl(1, 0x02, Operation);
                    }
                    break; 
            }

            if (Rtemp) {
                //pin sel
                LED_PIN_SEL(PinLocation);
                //Enable function pin
                LED_PIN_FCTRL(Operation);
            }
            break;

        case WL_ANT0:          
            //GPIOB (1, 2:3)
            Rtemp  = GPIOChk(1, 0x0c, Operation);
            
            if (Rtemp) {
                //Enable GPIO
                GPIOCtrl(1, 0x0c, Operation);
                //Enable function pin
                ANT0_PIN_FCTRL(Operation);
            }
            break;

        case WL_ANT1:
            //GPIOB (1, 6:7)
            Rtemp  = GPIOChk(1, 0xc0, Operation);

            if (Rtemp) {
                //Enable GPIO
                GPIOCtrl(1, 0xc0, Operation);
                //Enable function pin
                ANT1_PIN_FCTRL(Operation);
            }          
            break;

        case WL_BTCOEX:
            //GPIOC (2, 0:3)
            Rtemp  = GPIOChk(2, 0x0f, Operation);

            if (Rtemp) {
                //Enable GPIO
                GPIOCtrl(2, 0x0f, Operation);
                //Enable function pin
                BTCOEX_PIN_FCTRL(Operation);
            }
            break;

        case WL_BTCMD:
            //GPIOC (2, 4:5),  GPIOH (7, 4),
            Rtemp  = GPIOChk(2, 0x30, Operation);
            Rtemp &= GPIOChk(7, 0x10, Operation);

            if (Rtemp) {
                //Enable GPIO 
                GPIOCtrl(2, 0x30, Operation);
                GPIOCtrl(7, 0x10, Operation);
                //Enable function pin
                BTCMD_PIN_FCTRL(Operation);
            }
            break;

        case SPI_FLASH:
            //GPIOF (5, 0:5), 
            Rtemp  = GPIOChk(5, 0x3f, Operation);

            if (Rtemp) {
                //Enable GPIO 
                GPIOCtrl(5, 0x3f, Operation);
                //Enable function pin
                SPI_FLASH_PIN_FCTRL(Operation);
            }
            break;
            
        case SDR:
            //GPIOG (6, 0:7),  GPIOH (7, 0:7), GPIOI (8, 0:7), GPIOJ (9, 0:7), GPIOK (10, 0:7),
            Rtemp  = GPIOChk(6, 0xff, Operation);
            Rtemp &= GPIOChk(7, 0xff, Operation);
            Rtemp &= GPIOChk(8, 0xff, Operation);
            Rtemp &= GPIOChk(9, 0x7f, Operation);
            Rtemp &= GPIOChk(10, 0x7f, Operation);

            if (Rtemp) {
                //Enable GPIO
                GPIOCtrl(6, 0xff, Operation);
                GPIOCtrl(7, 0xff, Operation);
                GPIOCtrl(8, 0xff, Operation);
                GPIOCtrl(9, 0x7f, Operation);
                GPIOCtrl(10, 0x7f, Operation);
                //Enable function pin
                SDR_PIN_FCTRL(Operation);
            }
            break;

        case JTAG:
            //GPIOE (4, 0:4),
            Rtemp  = GPIOChk(4, 0x1f, Operation);

            if (Rtemp) {
                //Enable GPIO
                GPIOCtrl(4, 0x1f, Operation);
                //Enable function pin
                JTAG_PIN_FCTRL(Operation);
            }
            break;

        case TRACE:
            //GPIOE (4, 5:9), 
            Rtemp  = GPIOChk(4, 0x3e0, Operation);

            if (Rtemp) {
                //Enable GPIO
                GPIOCtrl(4, 0x3e0, Operation);
                //Enable function pin
                TRACE_PIN_FCTRL(Operation);
            }  
            break;

        case LOG_UART:
            //GPIOB (1, 0:1), 
            Rtemp  = GPIOChk(1, 0x03, Operation);

            if (Rtemp) {
                //Enable GPIO
                GPIOCtrl(1, 0x03, Operation);
                //Enable function pin
                LOG_UART_IR_FCTRL(OFF);
                LOG_UART_PIN_FCTRL(Operation);
            }
            break;

        case LOG_UART_IR:
            //GPIOB (1, 0:1), 
            Rtemp  = GPIOChk(1, 0x03, Operation);

            if (Rtemp) {
                //Enable GPIO
                GPIOCtrl(1, 0x03, Operation);
                //Enable function pin
                LOG_UART_PIN_FCTRL(Operation);
                LOG_UART_IR_FCTRL(Operation);
            }
            break;

        case SIC:
            //GPIOC (2, 4:5), 
            Rtemp  = GPIOChk(2, 0x30, Operation);

            if (Rtemp) {
                //Enable GPIO
                GPIOCtrl(2, 0x30, Operation);
                //Enable function pin
                SIC_PIN_CTRL(Operation);
            }
            break;

        case EEPROM:
            //GPIOB (1, 0:3), 
            Rtemp  = GPIOChk(1, 0x0f, Operation);

            if (Rtemp) {
                //Enable GPIO
                GPIOCtrl(1, 0x0f, Operation);
                //Enable function pin
                EEPROM_PIN_CTRL(Operation);
            }
            break;

        case DEBUG:
            //GPIOA(0, 0:2,4), GPIOB(1, 0:7), GPIOC(2, 0:5), GPIOD(3,4:5), GPIOE(4, 0:5), GPIOH(7,0:5),
            Rtemp  = GPIOChk(0, 0x17, Operation);
            Rtemp &= GPIOChk(1, 0xff, Operation);
            Rtemp &= GPIOChk(2, 0x3f, Operation);
            Rtemp &= GPIOChk(3, 0x30, Operation);
            Rtemp &= GPIOChk(4, 0x3f, Operation);
            Rtemp &= GPIOChk(7, 0x3f, Operation);

            if (Rtemp) {
                //Enable GPIO
                GPIOCtrl(0, 0x17, Operation);
                GPIOCtrl(1, 0xff, Operation);
                GPIOCtrl(2, 0x3f, Operation);
                GPIOCtrl(3, 0x30, Operation);
                GPIOCtrl(4, 0x3f, Operation);
                GPIOCtrl(7, 0x3f, Operation);
                //Enable function pin
                DEBUG_PIN_CTRL(Operation);
            }
            break;
    }
    return Rtemp;
}

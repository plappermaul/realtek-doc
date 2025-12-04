/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _SECTION_CONFIG_H_
#define _SECTION_CONFIG_H_

#include "basic_types.h"
#include "platform_autoconf.h"

#ifdef CONFIG_NO_SECTION

#define RAM_DEDECATED_VECTOR_TABLE_SECTION  
#define RAM_USER_IRQ_FUN_TABLE_SECTION      
#define RAM_USER_IRQ_DATA_TABLE_SECTION     
//3 Timer Section
#define SECTION_RAM_TIMER2TO7_VECTOR_TABLE  
#define SECTION_RAM_BSS_TIMER_RECORDER_TABLE 
#define TIMER_ROM_TEXT_SECTION               
#define TIMER_ROM_DATA_SECTION               
#define TIMER_RAM_TEXT_SECTION               
#define TIMER_RAM_DATA_SECTION               

//3 Hal Section
#define HAL_ROM_TEXT_SECTION                 
#define HAL_ROM_DATA_SECTION                 
#define HAL_RAM_TEXT_SECTION                 
#define HAL_RAM_DATA_SECTION                 
#define HAL_RAM_BSS_SECTION                  
//3 Infra Section
#define INFRA_ROM_TEXT_SECTION               
#define INFRA_ROM_DATA_SECTION               
#define INFRA_RAM_TEXT_SECTION               
#define INFRA_RAM_DATA_SECTION               
#define INFRA_RAM_BSS_SECTION                      

//3 Pin Mutex  Section
#define PINMUX_ROM_TEXT_SECTION                    
#define PINMUX_ROM_DATA_SECTION                    
#define PINMUX_RAM_TEXT_SECTION                    
#define PINMUX_RAM_DATA_SECTION                    
#define PINMUX_RAM_BSS_SECTION                     

//3 Monitor App Section
#define MON_ROM_TEXT_SECTION                      
#define MON_ROM_DATA_SECTION                  
#define MON_RAM_TEXT_SECTION                  
#define MON_RAM_DATA_SECTION                  
#define MON_RAM_BSS_SECTION                   

//3 SDIO Section
#define SECTION_SDIO_RAM
#define SECTION_SDIO_ROM

//3 SRAM Config Section
#define SRAM_BD_DATA_SECTION                  
#define SRAM_BF_DATA_SECTION                  
#define SRAM_BF_TEXT_SECTION                      
#define START_RAM_FUN_SECTION                 
#define PATCH_START_RAM_FUN_SECTION           
#define SDRAM_DATA_SECTION                    

//3 Wlan  Section
#define WLAN_ROM_TEXT_SECTION                      
#define WLAN_ROM_DATA_SECTION                      
#define WLAN_RAM_TEXT_SECTION                     
#define WLAN_RAM_DATA_SECTION                     
#define WLAN_RAM_BSS_SECTION                             

#else

#define RAM_DEDECATED_VECTOR_TABLE_SECTION          \
        SECTION(".ram_dedecated_vector_table")

#define RAM_USER_IRQ_FUN_TABLE_SECTION              \
        SECTION(".ram_user_define_irq_table")

#define RAM_USER_IRQ_DATA_TABLE_SECTION             \
        SECTION(".ram_user_define_data_table")

//3 Timer Section
#define SECTION_RAM_TIMER2TO7_VECTOR_TABLE          \
        SECTION(".timer2_7_vector_table.data")

#define SECTION_RAM_BSS_TIMER_RECORDER_TABLE       \
        SECTION(".timer.rom.data")

#define TIMER_ROM_TEXT_SECTION                      \
        SECTION(".timer.rom.text")

#define TIMER_ROM_DATA_SECTION                      \
        SECTION(".timer.rom.rodata")

#define TIMER_RAM_TEXT_SECTION                      \
        SECTION(".timer.ram.text")

#define TIMER_RAM_DATA_SECTION                      \
        SECTION(".timer.ram.data")


//3 Hal Section
#define HAL_ROM_TEXT_SECTION                         \
        SECTION(".hal.rom.text")

#define HAL_ROM_DATA_SECTION                         \
        SECTION(".hal.rom.rodata")

#define HAL_RAM_TEXT_SECTION                         \
        SECTION(".hal.ram.text")

#define HAL_RAM_DATA_SECTION                         \
        SECTION(".hal.ram.data")

#define HAL_RAM_BSS_SECTION                         \
        SECTION(".hal.ram.bss")

//3 Infra Section
#define INFRA_ROM_TEXT_SECTION                         \
        SECTION(".infra.rom.text")

#define INFRA_ROM_DATA_SECTION                         \
        SECTION(".infra.rom.rodata")

#define INFRA_RAM_TEXT_SECTION                         \
        SECTION(".infra.ram.text")

#define INFRA_RAM_DATA_SECTION                         \
        SECTION(".infra.ram.data")

#define INFRA_RAM_BSS_SECTION                         \
        SECTION(".infra.ram.bss")



//3 Pin Mutex  Section
#define PINMUX_ROM_TEXT_SECTION                      \
        SECTION(".hal.rom.text")

#define PINMUX_ROM_DATA_SECTION                      \
        SECTION(".hal.rom.rodata")

#define PINMUX_RAM_TEXT_SECTION                      \
        SECTION(".hal.ram.text")

#define PINMUX_RAM_DATA_SECTION                      \
        SECTION(".hal.ram.data")

#define PINMUX_RAM_BSS_SECTION                      \
        SECTION(".hal.ram.bss")


//3 Monitor App Section
#define MON_ROM_TEXT_SECTION                      \
        SECTION(".mon.rom.text")

#define MON_ROM_DATA_SECTION                      \
        SECTION(".mon.rom.rodata")

#define MON_RAM_TEXT_SECTION                      \
        SECTION(".mon.ram.text")

#define MON_RAM_DATA_SECTION                      \
        SECTION(".mon.ram.data")

#define MON_RAM_BSS_SECTION                      \
        SECTION(".mon.ram.bss")


//3 SDIO Section
#define SECTION_SDIO_RAM
#define SECTION_SDIO_ROM

//3 SRAM Config Section
#define SRAM_BD_DATA_SECTION                        \
        SECTION(".bdsram.data")

#define SRAM_BF_DATA_SECTION                        \
        SECTION(".bfsram.data")

#define SRAM_BF_TEXT_SECTION                        \
        SECTION(".bfsram.text")

    
#define START_RAM_FUN_SECTION                      \
        SECTION(".start.ram.data")

#define PATCH_START_RAM_FUN_SECTION                \
        SECTION(".patch.start.ram.data")

#define SDRAM_DATA_SECTION                         \
        SECTION(".sdram.data")


//3 Wlan  Section
#define WLAN_ROM_TEXT_SECTION                      \
        SECTION(".wlan.rom.text")

#define WLAN_ROM_DATA_SECTION                      \
        SECTION(".wlan.rom.rodata")

#define WLAN_RAM_TEXT_SECTION                      \
        SECTION(".wlan.ram.text")

#define WLAN_RAM_DATA_SECTION                      \
        SECTION(".wlan.ram.data")

#define WLAN_RAM_BSS_SECTION                       \
        SECTION(".wlan.ram.bss")
#endif //CONFIG_NO_SECTION
    
#endif //_SECTION_CONFIG_H_

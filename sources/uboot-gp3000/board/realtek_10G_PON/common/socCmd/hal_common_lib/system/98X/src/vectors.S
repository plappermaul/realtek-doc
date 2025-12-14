/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "platform_autoconf.h"
#ifdef CONFIG_RTL8195A_MEMORY_TCM_TEST
    .equ STACK_TOP, 0x20009000
#else
    .equ STACK_TOP, 0x1FFFFFFF
#endif

    .global Reset_Handler
    .global NMI_Handler
    .global HardFault_Handler
    .global MemManage_Handler
    .global BusFault_Handler
    .global UsageFault_Handler
    .code 16
    .syntax unified


    .section .vectors_table
    .align 8
    .globl __vectors_table
__vectors_table:
	.word    STACK_TOP
	.word    Reset_Handler
	.word    NMI_Handler
	.word    HardFault_Handler
	.word    MemManage_Handler
	.word    BusFault_Handler
	.word    UsageFault_Handler

    .text
    .thumb
    .thumb_func
    .section .vectors_table_text
    .align 8 
    .align 2
    .globl    Reset_Handler
    .type    Reset_Handler, function
Reset_Handler:
	ldr     sp,=STACK_TOP
	B   HalResetVsr


    .text
    .thumb
    .thumb_func
    .section .vectors_table_text
    .align 2
    .globl    NMI_Handler
    .type    NMI_Handler, function
NMI_Handler:
    B   HalNMIHandler

    .text
    .thumb
    .thumb_func
    .section .vectors_table_text
    .align 2
    .globl    HardFault_Handler
    .type    HardFault_Handler, function
HardFault_Handler:
    TST		LR, #4
    ITE		EQ
    MRSEQ	R0, MSP
    MRSNE	R0, PSP
    B		HalHardFaultHandler

    .text
    .thumb
    .thumb_func
    .section .vectors_table_text
    .align 2
    .globl    MemManage_Handler
    .type    MemManage_Handler, function
MemManage_Handler:
    B   HalMemManageHandler

    .text
    .thumb
    .thumb_func
    .section .vectors_table_text
    .align 2
    .globl    BusFault_Handler
    .type    BusFault_Handler, function
BusFault_Handler:
    B   HalBusFaultHandler

    .text
    .thumb
    .thumb_func
    .section .vectors_table_text
    .align 2
    .globl    UsageFault_Handler
    .type    UsageFault_Handler, function
UsageFault_Handler:
    B   HalUsageFaultHandler

.end

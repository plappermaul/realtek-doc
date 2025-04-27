
#ifndef _RTL8196_H
#define _RTL8196_H
#define WRITE_MEM32(addr, val)   (*(volatile unsigned int *) (addr)) = (val)
#define READ_MEM32(addr)         (*(volatile unsigned int *) (addr))
#define WRITE_MEM16(addr, val)   (*(volatile unsigned short *) (addr)) = (val)
#define READ_MEM16(addr)         (*(volatile unsigned short *) (addr))
#define WRITE_MEM8(addr, val)    (*(volatile unsigned char *) (addr)) = (val)
#define READ_MEM8(addr)          (*(volatile unsigned char *) (addr))



/*   System Clock Configuration   */
#if 0
	#define ASIC_8196 1
#else
	#define FPGA_8196 1
#endif


/*   System Clock Configuration   */
/*8196 ASIC*/
#if ASIC_8196
	#define CPU_CLOCK			(500 *1000*1000)  //330MHz OCP 8196 ASIC 
	#define SYS_CLK_RATE	  	(200*1000*1000)      //200MHz  LX 8196B ASIC
#endif

/*FPGA*/
#if FPGA_8196
	//#define CPU_CLOCK			(50 *1000*1000)  //50MHz OCP 8196 FPGA 
//	#define SYS_CLK_RATE	  	(40*1000*1000)      //40MHz  8196B FPGA
	#define SYS_CLK_RATE	  	(20*1000*1000)      //20MHz  8196B FPGA	// JoeyLin
	#define CPU_CLOCK			(66 *1000*1000)  //40MHz OCP 8196 FPGA 
	//#define SYS_CLK_RATE	  	(3386*10*1000)      //33.86MHz  8196B FPGA
	//#define SYS_CLK_RATE	  	(50*1000*1000)      
#endif


/*   SWITCH TFTP Configuration   */
#define CONFIG_SWITCH_MODULE	1	// JoeyLin
#define CONFIG_RTL8196C	1	// JoeyLin
#define CONFIG_RTL8196D	1	// JoeyLin
#ifdef CONFIG_RTL8196D
#define ALL_PORT_MASK                 0x1f		//0x7
#else
#define ALL_PORT_MASK                 0x3F
#endif

//#define PHY_Patch_8366 0 //JSW: For 5 Port Giga PHY	
//#define P0_8196_PHY_Patch_8211C 0 //JSW: For Port0 RGMII



/* Turn on RTL8198(5281) cache Write allocation */
#define RTL8198_TURN_ON_CACHE_WRITE_ALLOCATION 0

///////////////////////////////////////////////////////////////////
#define SPI_DBG_MESSAGE 0


#if 1 //NOR
	#define Boot_from_NOR 1 	//NOR Flash
	#define FAST_READ 0 	//Just prevent compiler error
#else //SPI
	#define Boot_from_SPI 1 	//SPI Flash
	#if 0 //Old 8196 SPI 
		#define SUPPORT_SPI_8196  //Old 8196 SPI Controller
		#define FAST_READ 	//for Single IO only ,when SPI_FREQ > 40MHZ ,must enable it 
	#else //New 8198 SPI Controller
		 #define SUPPORT_SPI_MIO_8198_8196C  1  //New 8198 SPI Controller
		 
			#if 1 /*For Single IO*/				
				 #define BOOT_SIO_8198 1  	//for Single IO
				 #define FAST_READ 0 		//for Single IO speed up
			
			#else /*for Dual/Quad IO*/

				 #undef FAST_READ   //DIO and QIO don't have FAST_READ

				
				#if 0	 /*JSW: For Dual selection*/
					 #define BOOT_DIO_8198   	//Dual IO		

				#else 
					/*JSW: For Quad selection*/
					
					 #define BOOT_QIO_8198   	//Quad IO
						#if 1
						  #define MXIC_SPI_QPP_Instruction_code  //for MXIC
						#else
						 //#define Spansion_SPI_QPP_Instruction_code  //only for Spansion SPI Quad IO
						#endif //end of QPP instruction between MXIC and Spansion
				#endif //end of  Quad
				
			#endif	//end of SIO and Dual/Quad
			
	 #endif  //end of 8196/8198
	 
#endif //end of NOR/SPI
///////////////////////////////////////////////////////////////////

#define CONFIG_NFBI_MODULE 0

//===============================================
//Start of RTL8196B_ASIC_FT2 setting

/*    JSW: RTL8196B_ASIC_FT2 setting  */
//#define RTL8196B_ASIC_FT2 1//Top root
#define DBG 0

#define ENABLE_DMEM_TEST 0   //Test for DMEM , JSW note: It will cause reboot

#define SIMPLIED_DMEM_TEST 1

#define SKIP_L2_ESC 0       // "1",skip ESC semaphore


/*   DTR Configuration    */
#if 0
	#define RTL8196_208PIN_SUPPORT_DDR_200MHZ 0
	#define RTL8196_208PIN_SUPPORT_DDR_166MHZ 0
	#define RTL8196_208PIN_SUPPORT_DDR_133MHZ 1
#else 
	#define RTL8196_208PIN_SUPPORT_SDR_166MHZ 0
	#define RTL8196_208PIN_SUPPORT_SDR_148MHZ 0
#endif

/*   Dram Power Management Test     */

#define DramPowerManagementTest 0  //JSW:Test with "dramtest 0" in console ,"monitor.c'

	

/*    PCIE Testing Configuration   */
	#define PCIE_Test_With_8102E 0  //"0 "=8111C /8111D  ,"1" =8102E

	#define PCIE_RESET_PROCEDURE 0

	#define PCIE_External_Loopback_test 0

	/*JSW: "2"=test P1 and P0 (208 pin_only)  , "1"=test P1 , "0"= test P0 (128pin_only)  */
	#define PCIE_Port0and1_8196B_208pin 0  //JSW:20090205:P1 will be tested by recognize BOND_OPTION_REG(0XB8000030),but still set it to 2

	#define Use_External_PCIE_CLK 0
	
	#define Test_with_GPIO_C1_trigger 0       //For CP Tester used

	#define READ_MINI_PCIE 0




/*    Set Shared pin to LED Scan mode   */
	
	#define SHARED_PIN_LED_MODE 0


/*    More HW Testing Configuration   */

#define CONFIG_TEST_HW 0
#define ENABLE_DRAM_SELF_TEST 0

//End of RTL8196B_ASIC_FT2 setting
//===============================================


	//Chip ID Code
	#define ChipIDCode_RTL8196B_128pin 0x2A   //
	#define ChipIDCode_RTL8196_208pin 0x78     //
	#define ChipIDCode_RTL8197B_208pin 0x39   // (For Panasonic)

	//Chip ID Code (VoIP)	
	#define ChipIDCode_RTL8972B_208pin     0x30   
	#define ChipIDCode_RTL8982B_208pin     0x31
	#define ChipIDCode_RTL8981B_208pin     0x32 	 //JSW:20090508
	#define ChipIDCode_RTL8984B_208pin     0x33
	

	//For system register
	#define PLL_REG 0xb8000020
	#define HW_STRAP_REG 0xb8000008
	#define PIN_MUX_SEL 0xb8000030
	#define Bond_Option_REG 0xb800000c


	//For 8196  DRAM
	#define DDCR_REG 0xb8001050
	#define MPMR_REG 0xB8001040
	#define MCR_REG 0xb8001000
	#define DCR_REG 0xb8001004
	#define DTR_REG 0xb8001008


	//For 8196 Switch Bist
	#define SSIR_REG 0xbb804204
	#define SBCR_REG 0xbb804208
	#define MSCR_REG 0xb8004410
	#define BIST_CONTROL_REG 0xb8000014
	#define BIST_DONE_REG 0xb8000018
	#define BIST_FAIL_REG 0xb800001C
	 

	/*
 	FT2 L2 DONE Semaphore
	*/

	#define KEYCODE_BS			0x08
	#define KEYCODE_TAB			0x09
	#define KEYCODE_ESC			0x1B
	#define KEYCODE_SP			0x20
	#define KEYCODE_CR			0x0D
	#define KEYCODE_LF			0x0A


	

	#define WRITE_MEM32(addr, val)   (*(volatile unsigned int *) (addr)) = (val)
	#define READ_MEM32(addr)         (*(volatile unsigned int *) (addr))
	#define WRITE_MEM16(addr, val)   (*(volatile unsigned short *) (addr)) = (val)
	#define READ_MEM16(addr)         (*(volatile unsigned short *) (addr))
	#define WRITE_MEM8(addr, val)    (*(volatile unsigned char *) (addr)) = (val)
	#define READ_MEM8(addr)          (*(volatile unsigned char *) (addr))

	
	
 //end of 20081210 :for RTL8196B_ASIC_FT2
//=========================================



/*   USB  Testing Configuration */

#define SUPPORT_USB_OTG 0     //JSW: (1)Enable USB_OTG Verifying  , (0)Disable USB_OTG Verifying

/*   Peripheral Configuration  */

#define GPIO_test  0
#define WatchDog_test 1
#define Timer_test 1
#define LDO_test 1



/*Cyrus Tsai*/
//#ifndef __ASSEMBLY__
typedef unsigned long Int32;  
typedef unsigned short Int16;  
typedef unsigned char Int8;  
//
typedef short INT16;
typedef unsigned short UINT16;
typedef int INT32;
typedef unsigned int UINT32;
typedef char CHAR8;
typedef unsigned char UCHAR8;




#define Virtual2Physical(x)		(((Int32)x) & 0x1fffffff)
#define Physical2Virtual(x)		(((Int32)x) | 0x80000000)
#define Virtual2NonCache(x)		(((Int32)x) | 0x20000000)
#define Physical2NonCache(x)		(((Int32)x) | 0xa0000000)
#define UNCACHE_MASK			0x20000000  //wei add

//void check_dram_freq_reg(void);

/*Cyrus Tsai*/

#define TRUE		0x01
#define FALSE	0x0


#define BIT(x)	(1 << (x))

#define rtl_inb(offset)		(*(volatile unsigned char *)(mips_io_port_base + offset))
#define rtl_inw(offset)	(*(volatile unsigned short *)(mips_io_port_base + offset))
#define rtl_inl(offset)		(*(volatile unsigned long *)(mips_io_port_base + offset))

#define rtl_outb(offset,val)	(*(volatile unsigned char *)(mips_io_port_base + offset) = val)
#define rtl_outw(offset,val)	(*(volatile unsigned short *)(mips_io_port_base + offset) = val)
#define rtl_outl(offset,val)	(*(volatile unsigned long *)(mips_io_port_base + offset) = val)
#define rtl_outl_otg(offset,val)	(*(volatile unsigned long *)(otg_io_port_base + offset) = val)



#define mips_io_port_base (0xB8000000) //JSW: for 8196
#define otg_io_port_base (0xB8030000)



/* 20070719: JSW  
    TODO: For 8672/8196 OCP architecture.

    1.Remark: In 8672/8196 OCP architecture , 0xbfc00000=0xbd000000 
    2.TODO:
     ex:     mips_io_port_base + FLASH_BASE   =  0xbd000000
                  (0xb8000000)     + (0x05000000) =  0xbd000000 
  */
#define FLASH_BASE 		0x05000000 //JSW : 8672/8196 OCP
//#define FLASH_BASE 		0x06000000 //8650c

//JSW:System Register for 11n WMAC
#define clk_manage_REG	0xb8000010



//JSW:Add UART 0/1 ,"0"=UART0,"1"=UART1

#if  0// For Uart1 Controller 8196 
#define UART_RBR	0x2100
#define UART_THR	0x2100
#define UART_DLL	0x2100
#define	UART_IER	0x2104
#define	UART_DLM	0x2104
#define	UART_IIR	0x2108

#define	UART_FCR	0x2108
#define UART_LCR	0x210c
#define	UART_MCR	0x2110
#define	UART_LSR	0x2114
#define	UART_MSR	0x2118
#define	UART_SCR	0x211c

#else
// For Uart0 Controller 8196 
#define UART_RBR	0x2000
#define UART_THR	0x2000
#define UART_DLL	0x2000
#define	UART_IER	0x2004
#define	UART_DLM	0x2004
#define	UART_IIR	0x2008

#define	UART_FCR	0x2008
#define  UART_LCR	0x200c
#define	UART_MCR	0x2010
#define	UART_LSR	0x2014
#define	UART_MSR	0x2018
#define	UART_SCR	0x201c
#endif


// For Uart1 Flags
#define UART_RXFULL	BIT(0)
#define UART_TXEMPTY	(BIT(6) | BIT(5))
#define UART_RXFULL_MASK	BIT(0)
#define UART_TXEMPTY_MASK	BIT(1)



// For Interrupt Controller
#define GIMR0	0x3000
#define GISR		0x3004
#define IRR0		0x3008
#define IRR1		0x300c
#define IRR2		0x3010
#define IRR3		0x3014


/* 20070719:JSW
    For USB OTG verifying    
*/
#define GUSBCFG 0x000c
#define GAHBCFG 0x0008
#define HPRT 0x0440
#define GINTMSK 0x0018
#define DIEPCTLn 0x0900
#define DOEPCTLn 0x0B00
#define DAINTMSK 0x081c
#define DOEPMSK 0x0814
#define DIEPMSK 0x0810
#define DOEPTSIZ0 0x0B10
#define DSTS 0x0808
#define DCFG 0x0800



//-----------------------------------------------------------------------
/* Register access macro
*/

#define REG32(reg) (*(volatile unsigned int *)(reg))
#define REG16(reg) (*(volatile unsigned short *)(reg))
#define REG8(reg) (*(volatile unsigned char *)(reg))


#define MEM_CONTROLLER_REG	0xB8001000	// memory contoller register

#define PIN_MUX_SEL 0xb8000030


/*GPIO register */
#define GPIO_BASE                           0xB8003500
#define PABCDCNR_REG                            (0x000 + GPIO_BASE)     /* Port ABCD control */
#define PABCDPTYPE_REG                          (0x004 + GPIO_BASE)     /* Port ABCD type */
#define PABCDDIR_REG                             (0x008 + GPIO_BASE)     /* Port ABCD direction */
#define PABCDDAT_REG                             (0x00C + GPIO_BASE)     /* Port ABCD data */
#define PABCDISR_REG                             (0x010 + GPIO_BASE)     /* Port ABCD interrupt status */
#define PABIMR_REG                               (0x014 + GPIO_BASE)     /* Port AB interrupt mask */
#define PCDIMR_REG                               (0x018 + GPIO_BASE)     /* Port CD interrupt mask */
#define PEFGHCNR_REG                             (0x01C + GPIO_BASE)     /* Port ABCD control */
#define PEFGHPTYPE_REG                           (0x020 + GPIO_BASE)     /* Port ABCD type */
#define PEFGHDIR_REG                             (0x024 + GPIO_BASE)     /* Port ABCD direction */
#define PEFGHDAT_REG                             (0x028 + GPIO_BASE)     /* Port ABCD data */
#define PEFGHISR_REG                             (0x02C + GPIO_BASE)     /* Port ABCD interrupt status */
#define PEFIMR_REG                               (0x030 + GPIO_BASE)     /* Port AB interrupt mask */
#define PGHIMR_REG                               (0x034 + GPIO_BASE)     /* Port CD interrupt mask */

/* Timer control registers 
*/
// For General Purpose Timer/Counter
#define TC0DATA		0x3100
#define TC1DATA		0x3104
#define TC2DATA		0x68
#define TC3DATA		0x6C
#define TC0CNT		0x3108
#define TC1CNT		0x310c
#define TC2CNT		0x78
#define TC3CNT		0x7C
#define TCCNR		0x3110
#define TCIR		       0x3114
#define BTDATA		0x3118
#define WDTCNR		0x311c
#define GICR                          0xB8003000
#define CDBR         0xb8003118 

//JSW :add for 8196 timer/counter
#define GICR_BASE                           0xB8003000
#define TC0DATA_REG                             (0x100 + GICR_BASE)       /* Timer/Counter 0 data,Normal */ 
#define TC1DATA_REG                             (0x104 + GICR_BASE)       /* Timer/Counter 1 data */
#define TC2DATA_REG                             (0x120 + GICR_BASE)       /* Timer/Counter 1 data */
#define TC3DATA_REG                             (0x124 + GICR_BASE)       /* Timer/Counter 1 data */

#define TC0CNT_REG                              (0x108 + GICR_BASE)       /* Timer/Counter 0 count,Normal */
#define TC1CNT_REG                              (0x10C + GICR_BASE)       /* Timer/Counter 1 count */
#define TC2CNT_REG                              (0x128 + GICR_BASE)       /* Timer/Counter 1 count */
#define TC3CNT_REG                              (0x12C + GICR_BASE)       /* Timer/Counter 1 count */

#define TCCNR_REG                               (0x110 + GICR_BASE)       /* Timer/Counter control */
#define TCIR_REG                                (0x114 + GICR_BASE)       /* Timer/Counter intertupt */
  
#define CDBR_REG                                (0x118 + GICR_BASE)       /* Clock division base */
#define WDTCNR_REG                              (0x11C + GICR_BASE)       /* Watchdog timer control */

// JSW:For WatchDog

#define WDTE_OFFSET                         24              /* Watchdog enable */
#define WDSTOP_PATTERN                      0xA5            /* Watchdog stop pattern */
#define WDTCLR                               (1 << 23)       /* Watchdog timer clear */
#define OVSEL_15                            0               /* Overflow select count 2^15 */
#define OVSEL_16                            (1 << 21)       /* Overflow select count 2^16 */
#define OVSEL_17                            (2 << 21)       /* Overflow select count 2^17 */
#define OVSEL_18                            (3 << 21)       /* Overflow select count 2^18 */
#define WDTIND                              (1 << 20)       /* Indicate whether watchdog ever occurs */

/* Global interrupt control registers 
*/
#define GICR_BASE                           0xB8003000
#define GIMR_REG                                (0x000 + GICR_BASE)       /* Global interrupt mask */
#define GISR_REG                                (0x004 + GICR_BASE)       /* Global interrupt status */
#define IRR_REG                                 (0x008 + GICR_BASE)       /* Interrupt routing */
#define IRR1_REG                                (0x00C + GICR_BASE)       /* Interrupt routing */
#define IRR2_REG                                (0x010 + GICR_BASE)       /* Interrupt routing */
#define IRR3_REG                                (0x014 + GICR_BASE)       /* Interrupt routing */

/*Bus Clock manage
*/
#define clk_manage_REG 0xb8000010

//-----------------------------------------------------------------------
//extern int dprintf(char *fmt, ...);
//#define printf dprintf
#define CONFIG_RTL865XC 1

#define PRINT_INTERRUPT_MSG 0
//---------------------------------------------------------------------

/* Switch core misc control register field definitions 
*/
#define DIS_P5_LOOPBACK                     (1 << 30)   /* Disable port 5 loopback */

#define LINK_RGMII							0			/* RGMII mode */
#define LINK_MII_MAC						1			/* GMII/MII MAC auto mode */
#define LINK_MII_PHY						2			/* GMII/MII PHY auto mode */
#define LINKMODE_OFFSET						23			/* Link type offset */
#define P5_LINK_RGMII						LINK_RGMII				/* Port 5 RGMII mode */
#define P5_LINK_MII_MAC                     LINK_MII_MAC			/* Port 5 GMII/MII MAC auto mode */
#define P5_LINK_MII_PHY                     LINK_MII_PHY			/* Port 5 GMII/MII PHY auto mode */
#define P5_LINK_OFFSET                      LINKMODE_OFFSET			/* Port 5 link type offset */
/*#define P4_USB_SEL                          (1 << 25)    Select port USB interface */
#define EN_P5_LINK_PHY                      (1 << 26)   /* Enable port 5 PHY provides link status to MAC */
#define EN_P4_LINK_PHY                      (1 << 25)   /* Enable port 4 PHY provides link status to MAC */
#define EN_P3_LINK_PHY                      (1 << 24)   /* Enable port 3 PHY provides link status to MAC */
#define EN_P2_LINK_PHY                      (1 << 23)   /* Enable port 2 PHY provides link status to MAC */
#define EN_P1_LINK_PHY                      (1 << 22)   /* Enable port 1 PHY provides link status to MAC */
#define EN_P0_LINK_PHY                      (1 << 21)   /* Enable port 0 PHY provides link status to MAC */

	
	

	//For system register
	#define PLL_REG 0xb8000020
	#define HW_STRAP_REG 0xb8000008
	#define PIN_MUX_SEL_REG 0xb8000030
	#define Bond_Option_REG 0xb800000c

	
	//For 8196 Switch Bist
	#define SSIR_REG 0xbb804204
	#define SBCR_REG 0xbb804208
	#define MSCR_REG 0xbb804410
	#define BIST_CONTROL_REG 0xb8000014
	#define BIST_DONE_REG 0xb8000018
	#define BIST_FAIL_REG 0xb800001C

	//PCIE Register
	#define PCIE_PLL 0xb8000044
	#define CLK_MANAGE 0xb8000010
	#define PCIE_MDIO_P0 0xb800003C
	#define PCIE_MDIO_P1 0xb8000040
	#define PCIE_PHY0 0xb8b01008
	#define PCIE_PHY1 0xb8b21008
	#define PCIE_PHY0_REG 0xb8b01000
	#define PCIE_PHY1_REG 0xb8b21000
#endif



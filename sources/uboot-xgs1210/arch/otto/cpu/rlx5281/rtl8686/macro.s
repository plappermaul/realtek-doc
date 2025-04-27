

	################################################################################
	#  Noop_Delay  <iteration> <register,default T1>, For All production
	################################################################################
	.macro Noop_Delay iter reg=t1
		li	\reg,\iter
		1:
		subu		\reg,1
		nop	
		bnez		\reg,1b
		nop			
	.endm

	################################################################################
	#  Detect_Clock_Source - Detect clock source and store result to v0, For RTL8676
	#    v0=0 : 35.328Mhz
	#    v0=1 : 40Mhz
	################################################################################
	.macro Detect_Clock_Source
		li		t1,0xb8000308
		lw		t0,0(t1)
		nop
		srl		t0,t0,23
		and		v0,t0,1		
	.endm
	
	################################################################################
	#  Mem_Set_Word <StartAddr> <Size> <Pattern>  - Fill memory with pattern, For All production
	################################################################################
	.macro Mem_Set_Word addr size patt
		#define	CLR_TEMP1	t0
		#define	CLR_TEMP2	t2
		#define	CLR_TEMP3	t3
		li		CLR_TEMP1,\addr				//start address
		addiu	CLR_TEMP2,CLR_TEMP1,\size		//2k bytes, end address
		li		CLR_TEMP3,\patt
		1:	
		sw		CLR_TEMP3,0(CLR_TEMP1)
		addi	CLR_TEMP1,CLR_TEMP1,4
		bne		CLR_TEMP1,CLR_TEMP2,1b
		nop		
		#undef	CLR_TEMP1	
		#undef	CLR_TEMP2	
		#undef	CLR_TEMP3	
	.endm
	
	
	################################################################################
	#  Detect_RAM_Size
	################################################################################
	#	 here we auto detect the sdram size. according Designer's info's on memory controller behavior
	#	 use address range = 0x0000_0000 ~ 0x03FF_FFFF
	#	input address 0xA3F0_1234 => physical address = 0x03F0_1234
	#	predefine 16bits (bytes) DRAM => DRAM address = 0x03F0_1234 / 2 = 0x1F8_091A
	#	column address range a0~a9 (0x400) => 0x1F8_091A mod 0x400 = 11A (COL address)
	#	                                                0x1F8_091A / 0x400 = 0x7E02 ( for ROW / BANK)
	#	row address range a0~a12 (0x2000)  => 0x7E02 mod 0x2000 = 1E02 (ROW address)
	#                                                                    0x7E02 / 0x2000 = 3 (BANK address)
	# we have conclusion on MCR with 64MB setting:
	#	2MB *(0xa3f01234) == *(0xa1f01234)
	#	8MB *(0xa3f01234) == *(0xA3F01034)
	#	16MB *(0xa3f01234) == *(0xA3701634)
	#	32MB *(0xa3f01234) == *(0xA3F01634)
	#	64MB *(0xa3f01234) unique
	
	.macro Detect_RAM_Size
		li	t5,0xb8001000		//detect DDR or SDR on board.
		lw	t5,0(t5)
		nop
		and	t6,t5,0x80000000
		nop
		beqz	t6,is_SDRAM_MCR
		nop
		//li 	t6, 0x00030000
		li	t0,0xb8001004
		lw	t1,0(t0)
		nop
		and 	t6,t1,0x30000
		b	is_DDR_MCR
		nop
is_SDRAM_MCR:
		li	t6, 0x00000000		
is_DDR_MCR:		
		li  	t0, 0xb8001004
		or	t1, t6, 0x54880000 
		
		#set MCR to 64MB setting temprory
		sw	t1, 0(t0)
		nop
		li	t2, 0xAAAA5555
		li	t3, 0xA3f01234
		sw	$0, 0(t3)
		li	t3, 0xa1f01234
		sw	$0, 0(t3)
		li	t3, 0xA3F01034
		sw	$0, 0(t3)
		li	t3, 0xA3701634
		sw	$0, 0(t3)
		li	t3, 0xA3F01634
		sw	$0, 0(t3)		
		nop
		li	t3, 0xA3F01234
		sw	t2, 0(t3)
		nop
		li	t3, 0xA1f01234
		lw	t4, 0(t3)
		nop
		beq  t4, t2,  SDRAM_2MB
		nop
		li	t3, 0xA3F01034
		lw 	t4, 0(t3)
		nop
		beq  t4, t2, SDRAM_8MB
		nop
		li	t3, 0xA3701634
		lw 	t4, 0(t3)
		nop
		beq  t4, t2, SDRAM_16MB
		nop
		li	t3, 0xA3F01634
		lw 	t4, 0(t3)
		nop
		beq  t4, t2, SDRAM_32MB
		nop
                # Otherwise, 64MB is the only possible setting
		b    SDRAM_64MB
		nop		
SDRAM_2MB:
		li	t2, 0x50000000
		b	setting_mcr
		nop
SDRAM_8MB:
		li	t2, 0x52080000
		b	setting_mcr
		nop		
SDRAM_16MB:
		or	t2, t6, 0x52480000 
		b	setting_mcr
		nop		
SDRAM_32MB:
		or	t2, t6, 0x54480000 
		b	setting_mcr
		nop		
SDRAM_64MB:
		or	t2, t6, 0x54880000 
		b	setting_mcr
		nop		
setting_mcr:
		li	t3, 0xb8001004
		nop
		sw	t2, 0(t3)
		nop

	.endm // Detect_SDRAM_Size

	################################################################################
	#  SRAM_Map	 <Bus Address> <Size> <Segment> <Base>
	#     Size      (0-128/1-256/2-512/3-1k/4-2k/5-4k/6-8k/7-16k/8-32k) 
	#     Segment   (0x00/0x10/0x20/0x30) which set of register to use
	################################################################################	
	.macro SRAM_Map addr size segment base
		/////////  SRAM unmap & map  /////////
		#define	SRAM_START_ADDR	t0
		#define	SRAM_SIZE			t1
		#define	SRAM_BASE			t2
		#define	REG_TEMP			t3
		////// Segment 0 ,32k bytes	//////
		li	SRAM_START_ADDR,(\addr | 0x00000001)		//last bit it define enable
		li	SRAM_SIZE,\size					//32k bytes
		li	REG_TEMP,0xb8001300				//unmap addr reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_START_ADDR,0(REG_TEMP)
		nop
		li	REG_TEMP,0xb8001304				//unmap size reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_SIZE,0(REG_TEMP)
		nop	
		li	REG_TEMP,0xb8004000				//map addr reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_START_ADDR,0(REG_TEMP)
		nop
		li	REG_TEMP,0xb8004004				//map size reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_SIZE,0(REG_TEMP)	
		nop
		li	SRAM_BASE,\base					//
		li	REG_TEMP,0xb8004008				//Segment 0 Base reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_BASE,0(REG_TEMP)	
		nop
		#undef	REG_TEMP
		#undef	SRAM_BASE
		#undef	SRAM_SIZE
		#undef	SRAM_START_ADDR
	.endm

	################################################################################
	#  UN_SRAM_Map	<Segment> <Base=0>
	#     
	#     Segment   (0/1/2/3) -> (0x00/0x10/0x20/0x30)	
	################################################################################	
	.macro UN_SRAM_Map segment
		/////////  SRAM unmap & map  /////////
		#define	SRAM_START_ADDR	t0
		#define	SRAM_SIZE			t1
		#define	SRAM_BASE			t2
		#define	REG_TEMP			t3
		////// Segment 0 ,32k bytes	//////
		li	SRAM_START_ADDR,(0x00000000)		//last bit it define enable
		li	SRAM_SIZE,(0x00000000)					//set size to zero
		li	REG_TEMP,0xb8001300				      //unmap addr reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_START_ADDR,0(REG_TEMP)
		nop
		li	REG_TEMP,0xb8001304				//unmap size reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_SIZE,0(REG_TEMP)
		nop	
		li	REG_TEMP,0xb8004000				//map addr reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_START_ADDR,0(REG_TEMP)
		nop
		li	REG_TEMP,0xb8004004				//map size reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_SIZE,0(REG_TEMP)	
		nop
		li	SRAM_BASE,(0x00000000)					//
		li	REG_TEMP,0xb8004008				//Segment 0 Base reg.
		addi	REG_TEMP,REG_TEMP,\segment
		sw	SRAM_BASE,0(REG_TEMP)
		nop
		#undef	REG_TEMP
		#undef	SRAM_BASE
		#undef	SRAM_SIZE
		#undef	SRAM_START_ADDR
	.endm
	
	################################################################################
	#  Mem_Init_RTL8676
	################################################################################
	.macro Mem_Init_RTL8676

		// SWR parameter change
		li		t1,0xb8000218
		li		t0,0x845555e7
		sw		t0,0(t1)
		nop
		li		t1,0xb800021c
		li		t0,0x0003a11e
		sw		t0,0(t1)
		nop		
		Noop_Delay 1500
		//
		li		t1,0xb8000200			//load System Clock Frequency Control Register	
		li		t0,0x77001300			//clear OCP and DRAM CLK fields
		li		t2,0xb8000308			//PCM is bonded if bit24=1 or bit25=1 in 0xb8000308
		lw 		t3,0(t2)
		nop
		srl 		t3,t3,24
		andi		t3,t3,0x3
		bne		t3,0x0,PCM_OCP_CLK
		nop
NORMAL_OCP_CLK:
		ori		t0,t0,0x1A			//OCP=560MHz (0x1A)
		b		DRAM_CLK
		nop
PCM_OCP_CLK:
		ori		t0,t0,0x1D			//OCP=620MHz (0x1D)
DRAM_CLK:
#ifdef LX_210_DDR166
		li		t2,0x080000			//Set DRAM=166MHz
		or		t0,t0,t2
#else
		li		t2,0x060000			//Set DRAM=133MHz
		or		t0,t0,t2
#endif
		sw		t0,0(t1)
		nop
		Noop_Delay 500
		nop

		##############################################
		# reg b800_1200: bit31-29,0:RAM clk/2, 1: RAM clk/4, 2: RAM clk/6, 3: RAM clk/8
		# Set default spi clock as RAM clk/6, e.g.200Mhz/6 = 33.333Mhz
		# bit26-22: deselect time
		# CS# deselect time min 100n for MX25L3205D, so set 19 to guarantee all device access normally.
		and		t2,t0,0x0F0000
		blt		t2,0x70000,set_div4		//DRAM freq < 150MHz, div 4, deselect value set to 0x10
		nop
		li		t0,0x5cc00000				//DRAM freq >= 150MHz, div 6, deselect value set to 0x13
		b		set_div
		nop
set_div4:
		li		t0,0x3c000000			//
set_div:		
		li		t1,0xb8001200
		sw		t0,0(t1)
		nop	
		Noop_Delay 100
		###############################################
		
		nop
		
		li		t1,0xb8000300
		li		t0,0x4
		sw		t0,0(t1)
		nop
		
		###############################################
		li		t0,0xb8000308		//Pin Status Register
		lw		t1,0(t0)
		nop
		and		t1,t1,~0x20000000	//bit29:Switch setting (0:master, 1:slave)
		sw		t1,0(t0)
		nop	
		
		//0xb800030c, Enable IP / IP selection Register,
		
	.endm
	
	
	#define NAND_CHECK_READY()      \
123:;							\
	la		s0, NAND_CTRL_BASE+0x4; \
	lw    	t0, 0(s0);       \
	and   	t0, t0, 0x80000000;      \
	bne   	t0, 0x80000000, 123b;    \
	nop
	
	
	################################################################################
	#  NAND Controller Registers	
	################################################################################
	#define NandRegCtrl			(NAND_CTRL_BASE+0x04)
	#define NandRegRW			(NAND_CTRL_BASE+0x10)
	#define NandRegFlashAddr	(NAND_CTRL_BASE+0x18)
	#define NandRegRAMAddr		(NAND_CTRL_BASE+0x1C)
	#define NandRegTagAddr		(NAND_CTRL_BASE+0x54)
		
	################################################################################
	#  NAND_Ready  
	#     Loop until NAND controller is ready
	################################################################################
	.macro NAND_Reay
	11:
		la		t7, NandRegCtrl
		lw		t7, 0(t7)
		nop
		srl		t7, 31
		beq		zero, t7, 11b
		nop
	.endm
	
	################################################################################
	#  NAND_Copy_2048 <page_from> <page_to> <Ram> <Tag> 
	#     Copy n x 2k page from NAND Flash address to RAM/Tag
	################################################################################			
	.macro NAND_Copy_2048 PageFrom PageTo RamAddr TagAddr
		la		t5, 0xc00fffff
		la		t4, NandRegCtrl
		sw		t5, 0(t4)
		
		li		t1, \PageFrom * 0x1000
		li		t2, \RamAddr
		li		t3, \TagAddr
		li		t4, \PageTo * 0x1000
	
21:		
		li		t0, 4
20:		
		la		t5, NandRegFlashAddr	# initialize variables
		sw		t1, 0(t5)
		la		t6, NandRegRAMAddr
		sw		t2, 0(t6)
		la		t5, NandRegTagAddr
		sw		t3, 0(t5)
		la		t6, NandRegRW
		li		t5, 0x5b				# start write
		sw		t5, 0(t6)
		NAND_Reay
		
		addi	t0, t0, -1
		addi	t1, t1, 0x210
		addi	t2, t2, 0x200
		addi	t3, t3, 0x10
		bne		zero, t0, 20b
		nop
		
		addi	t1, t1, (0x1000 - 0x840)				
		bne		t1, t4, 21b
		nop		
	.endm
	
	################################################################################
	#  NAND_Copy_512 <Page> <Ram> <Tag> <Size>
	#     Size      (0-128/1-256/2-512/3-1k/4-2k/5-4k/6-8k/7-16k/8-32k) 
	#     Segment   (0x00/0x10/0x20/0x30) which set of register to use
	################################################################################
	.macro NAND_Copy_512 PageFrom PageTo RamAddr TagAddr
		la		t5, 0xc00fffff
		la		t4, NandRegCtrl
		sw		t5, 0(t4)
		
		
		li		t2, \RamAddr
		li		t3, \TagAddr
		li		t1, \PageFrom * 0x200
		li		t0, \PageTo * 0x200
		
30:
		la		t4, NandRegFlashAddr	# initialize variables
		sw		t1, 0(t4)
		la		t5, NandRegRAMAddr
		sw		t2, 0(t5)
		la		t4, NandRegTagAddr
		sw		t3, 0(t4)
		la		t5, NandRegRW
		li		t4, 0x5b				# start write
		sw		t4, 0(t5)
		NAND_Reay
		
		addi	t1, t1, 0x200
		addi	t2, t2, 0x200
		addi	t3, t3, 0x10
		
		bne		t0, t1, 30b
		nop		
	.endm
	
	

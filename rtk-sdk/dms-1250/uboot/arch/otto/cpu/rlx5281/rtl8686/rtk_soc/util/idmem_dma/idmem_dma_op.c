#include <asm/arch/idmem_dma.h>

#undef DEBUG_IDMEM_REG

#define IMEM_DMEM_DMA_POLLING_LIMIT	(0x800000)

void imemDmemDMA_polling(void)
{
	unsigned polling_time;

retry:
	polling_time = 0;

	while( polling_time < IMEM_DMEM_DMA_POLLING_LIMIT) {	
		if((REG(DMDMA_CTL_REG)&IMEMDMEM_START)){
			//printf("\rPolling...");
			polling_time++;
		}
		else
			break;
	}

	if(polling_time>=IMEM_DMEM_DMA_POLLING_LIMIT){
		printf("REG(0x%08x) = 0x%08x\n ", DMDMA_CTL_REG, REG(DMDMA_CTL_REG));
		printf("function %s: %d IMEM_DMEM_DMA Polling timeout\n", __FUNCTION__, __LINE__);
		while(1);
	}

	return;
}

/*
sdram_addr: 
	External memory address. (virtual address)
mem_addr:   
	Mirrored IMEM/DMEM address. (virtual address)
mem_base:   
	Mirrored MEM/DMEM base address. (virtual address)
size:	    
	The number of bytes to be transfered.
direction:
	DIR_IMEMDMEM_TO_SDRAM, transfer data from IMEM/DMEM to SDRAM.
	DIR_SDRAM_TO_IMEMDMEM, transfer data from SDRAM to IMEM/DMEM.
target:
	SEL_IMEM0, Choose IMEM0 to be the target memory.
	SEL_IMEM1, Choose IMEM1 to be the target memory.
	SEL_DMEM0, Choose DMEM0 to be the target memory.
	SEL_DMEM1, Choose DMEM1 to be the target memory.
	SEL_L2MEM, Choose L2MEM to be the target memory.
mode:
	STORE_FORWARD_MODE
	ACCELERATION_MODE: overlap the transactions of getting data and sending data.
*/


void imemDmemDMA(unsigned int sdram_addr, unsigned int mem_addr, unsigned int mem_base,\
		 unsigned int byte_size, unsigned int direction, unsigned int target, unsigned int mode)
{

	//reset DMA
	REG(DMDMA_CTL_REG) = 0;
	REG(DMDMA_CTL_REG) = IMEMDMEM_SREST;
	
	//Set IMEM_DMEM start addr
	REG(IMEM_DMEM_SA_REG) = IMEM_DMEM_ADDR_OF(mem_addr - mem_base);

	//Set external memory address
	REG(EXTM_SA_REG) = EXTM_SA_OF(sdram_addr);
	//Set control regisetr
	REG(DMDMA_CTL_REG) = (IMEMDMEM_L2MEM_SLE(target) | ACCEL_SLE(mode)| \
				DIRECTION_SEL(direction) | TRAN_SIZE_INWORDS(byte_size));
#ifdef DEBUG_IDMEM_REG
	printf("REG(IMEM_DMEM_SA_REG) = %x\n", REG(IMEM_DMEM_SA_REG));
	printf("REG(EXTM_SA_REG) = %x\n", REG(EXTM_SA_REG) );
	printf("REG(DMDMA_CTL_REG) %x\n", REG(DMDMA_CTL_REG) );
#endif
	//if( byte_size != 0)
	REG(DMDMA_CTL_REG) = (REG(DMDMA_CTL_REG) | IMEMDMEM_START);

	//Polling status
	POLLING_IMEM_DMEM_DMA;

	//OK
	return;	
}

#ifdef CONFIG_RLX5181
void imemDmemDMA(unsigned int sdram_addr, unsigned int mem_addr, unsigned int mem_base,\
		 unsigned int byte_size, unsigned int direction, unsigned int target, unsigned int mode)
{
	if( (byte_size == 0) || (byte_size > 0x4000)){
		printf("\nillegal size (%d), blocking~", byte_size);
		while(1);
	}
	if(target == SEL_IMEM0 || target == SEL_IMEM1){
	        //reset DMA
       		REG(IMEM_DMA_CTL_REG) = 0;
        	REG(IMEM_DMA_CTL_REG) = IMEMDMEM_SREST;

        	//Set IMEM_DMEM start addr
        	REG(IMEM_SA_REG) = IMEM_DMEM_ADDR_OF(mem_addr - mem_base);

        	//Set external memory address
        	REG(IMEM_EXTM_SA_REG) = EXTM_SA_OF(sdram_addr);
        	//Set control register
        	REG(IMEM_DMA_CTL_REG) = (DIRECTION_SEL(direction)|TRAN_SIZE_INWORDS(byte_size)|IMEMDMEM_START);
		if( byte_size != 0){
       			REG(IMEM_DMA_CTL_REG) = (REG(IMEM_DMA_CTL_REG) | IMEMDMEM_START);
        		//Polling status
        		POLLING_IMEM_DMA;
		}
	} //DMEM0 or DMEM1
	else{
        	//reset DMA
        	REG(DMEM_DMA_CTL_REG) = 0;
        	REG(DMEM_DMA_CTL_REG) = IMEMDMEM_SREST;

        	//Set IMEM_DMEM start addr
        	REG(DMEM_SA_REG) = IMEM_DMEM_ADDR_OF(mem_addr - mem_base);

        	//Set external memory address
       		REG(DMEM_EXTM_SA_REG) = EXTM_SA_OF(sdram_addr);
        	//Set control register
        	REG(DMEM_DMA_CTL_REG) = (DIRECTION_SEL(direction)|TRAN_SIZE_INWORDS(byte_size)|IMEMDMEM_START);
		if( byte_size != 0){
       			REG(DMEM_DMA_CTL_REG) = (REG(DMEM_DMA_CTL_REG) | IMEMDMEM_START);
        		//Polling status
        		POLLING_DMEM_DMA;
		}
	}

        return;
}
#endif

#ifdef CONFIG_RLX5281
unsigned int setImemDmemDMA(unsigned int sdram_addr, unsigned int mem_addr, unsigned int mem_base,\
		 unsigned int byte_size, unsigned int direction, unsigned int target, unsigned int mode)
{
	//reset DMA
	REG(DMDMA_CTL_REG) = 0;
	REG(DMDMA_CTL_REG) = IMEMDMEM_SREST;
	
	//Set IMEM_DMEM start addr
	REG(IMEM_DMEM_SA_REG) = IMEM_DMEM_ADDR_OF(mem_addr - mem_base);

	//Set external memory address
	REG(EXTM_SA_REG) = EXTM_SA_OF(sdram_addr);
	//Set control regisetr
	REG(DMDMA_CTL_REG) = (IMEMDMEM_L2MEM_SLE(target) | ACCEL_SLE(mode)| \
				DIRECTION_SEL(direction) | TRAN_SIZE_INWORDS(byte_size));
#ifdef DEBUG_IDMEM_REG
	printf("REG(IMEM_DMEM_SA_REG) = %x\n", REG(IMEM_DMEM_SA_REG));
	printf("REG(EXTM_SA_REG) = %x\n", REG(EXTM_SA_REG) );
	printf("REG(DMDMA_CTL_REG) %x\n", REG(DMDMA_CTL_REG) );
#endif

	//Kick off DMA
	//if( byte_size != 0)
	//REG(DMDMA_CTL_REG) = (REG(DMDMA_CTL_REG) | IMEMDMEM_START);
	return (REG(DMDMA_CTL_REG) | IMEMDMEM_START);

	//Polling status
	//POLLING_IMEM_DMEM_DMA;

}
#endif

#ifdef CONFIG_RLX5181
unsigned int setImemDmemDMA(unsigned int sdram_addr, unsigned int mem_addr, unsigned int mem_base,\
		 unsigned int byte_size, unsigned int direction, unsigned int target, unsigned int mode)
{
        //reset DMA
        REG(DMDMA_CTL_REG) = 0;
        REG(DMDMA_CTL_REG) = IMEMDMEM_SREST;

        //Set IMEM_DMEM start addr
        REG(IMEM_DMEM_SA_REG) = IMEM_DMEM_ADDR_OF(mem_addr - mem_base);

        //Set external memory address
        REG(EXTM_SA_REG) = EXTM_SA_OF(sdram_addr);
        //Set control register
        REG(DMDMA_CTL_REG) = (DIRECTION_SEL(direction) | TRAN_SIZE_INWORDS(byte_size) | IMEMDMEM_START);
        //Kick off DMA
	//if( byte_size != 0)
       		//REG(DMDMA_CTL_REG) = (REG(DMDMA_CTL_REG) | IMEMDMEM_START);

	return (REG(DMDMA_CTL_REG) | IMEMDMEM_START);

        //Polling status
        //POLLING_IMEM_DMEM_DMA;
}
#endif


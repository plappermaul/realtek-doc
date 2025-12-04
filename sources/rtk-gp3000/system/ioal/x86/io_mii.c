/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : For linux kernel mode
 *           I/O read/write APIs using MDIO interface in the SDK.
 *
 * Feature : I/O read/write APIs, by access swich register by MDIO interface
 *
 */

/*
 * Include Files
 */
#include <common/error.h>
#include <common/util.h>
#include <ioal/ioal_init.h>
#include <ioal/io_mii.h>
#include <osal/print.h>


#if defined(CYGWIN_MDIO_IO)
#include <ioal/x86/io.h>
#include <osal/time.h>


/*
 * Symbol Definition
 */
#define MII_ADDR  10

#define MII_REG0 0
#define MII_REG1 1
#define MII_REG2 2
#define MII_REG3 3
#define MII_REG4 4
#define MII_REG5 5
#define MII_REG6 6

int DATA_PORT=0x378;
int STATUS_PORT=0x379;
int CONTROL_PORT=0x37A;
char SCL_0_SDA_0=0;
char SCL_1_SDA_0=0;
char SCL_0_SDA_1=0;
char SCL_1_SDA_1=0;
char SCL_0_read_SDA=0;
char SCL_1_read_SDA=0;

char SDA_0_SCL_0=0;
char SDA_0_SCL_1=0;
char SDA_1_SCL_0=0;
char SDA_1_SCL_1=0;
char SDA_Z_SCL_0=0;
char SDA_Z_SCL_1=0;
char SDA_Z_SCL_Z=0;

// MDC&MDIO parameter and functions
char SET_HIGH0=0 ;
char SET_HIGH1=0;
char SET_LOW0=0 ;
char SET_LOW1=0 ;
char READ_TA0=0 ;
char READ_TA1=0 ;
char SET_EN_DISABLE=0;
char PC_DIN=0x08; // STATUS_PORT.bit 3, print port pin 15

/*
 * Function Declaration
 */
void miiar_read(unsigned char phyaddr,unsigned char regaddr,unsigned short *value);
void miiar_write(unsigned char phyaddr,unsigned char regaddr,unsigned short value);


void read_ta(void);
void read_ta2(void);
int read32(void);
void Dis_EN_pin(void);
void Disable_Power(void);
void wait_half_CLK(void);
void sethigh(void);
void setlow(void);
void MDC_PIN_DEF(int Power_bit, int CLK_bit, int CLK_EN_bit, int DA_bit, int DA_EN_bit);

/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */


/* Public Function Body */


/*MDC&MDIO command*/

void MDC_PIN_DEF(int Power_bit, int CLK_bit, int CLK_EN_bit, int DA_bit, int DA_EN_bit)
{
	int i;

	i = 1 << Power_bit;
	SET_EN_DISABLE=SET_EN_DISABLE | i;
	i=i|(1<<CLK_EN_bit);
	SET_HIGH0=SET_HIGH0 | i;	SET_HIGH1=SET_HIGH1 | i;
	SET_LOW0=SET_LOW0 | i;		SET_LOW1=SET_LOW1 | i;
	READ_TA0=READ_TA0 | i;		READ_TA1=READ_TA1 | i;

	i=1<<CLK_bit;
	SET_HIGH1=SET_HIGH1 | i;	SET_LOW1=SET_LOW1 | i;
	READ_TA1=READ_TA1 | i;

	i=1<<DA_bit;
	SET_HIGH0=SET_HIGH0 | i;	SET_HIGH1=SET_HIGH1 | i;

	i=1<<DA_EN_bit;
	SET_HIGH0=SET_HIGH0 | i;	SET_HIGH1=SET_HIGH1 | i;
	SET_LOW0=SET_LOW0 | i;		SET_LOW1=SET_LOW1 | i;
}


void io_mii_init(void)
{
    LoadIODLL();
	MDC_PIN_DEF(3,0,4,1,2);
}

void io_mii_deinit(void)
{
}

void wait_half_CLK(void)
{
    return;
}

void read_ta(void)  /*bit2=r/w (read=0) */
{
	PortOut(DATA_PORT,READ_TA0);
	wait_half_CLK();

	PortOut(DATA_PORT,READ_TA1); /* DIR = read and clock=high */
	wait_half_CLK();

}
void read_ta2(void)  /*bit2=r/w (read=0) */
{

    PortOut(DATA_PORT,READ_TA0); /* DIR = read and clock=low */
 	wait_half_CLK();

    PortOut(DATA_PORT,READ_TA1); /* DIR = read and clock=high */
	wait_half_CLK();
}


void Dis_EN_pin(void) /*disable pc to write mdio, DIR=read */
{
	PortOut(DATA_PORT,SET_EN_DISABLE);
}

void Disable_Power(void) /*disable pc to write mdio, DIR=read */
{
	PortOut(DATA_PORT,0x0);
}

int read32(void) /*bit2=r/w (0=r,1=w)*/
{
	int j,tmp,data;

 
	data =0;
	for(j=0;j<=15;j++) {
		data = data<<1;
		PortOut(DATA_PORT,READ_TA0); /* DIR=read and clock=low*/
		wait_half_CLK();

		tmp = PortIn(STATUS_PORT);
		/* printf("read data=0x%X ",tmp);*/
		tmp = tmp & PC_DIN; /* extract the bit of PC data in */
		if(tmp>0) data = data | 0x1;
		PortOut(DATA_PORT,READ_TA1); /* DIR=read and clock=high */
		wait_half_CLK();
	}

	read_ta();
 	Dis_EN_pin();



	return(data);
}

void sethigh(void)
{
    PortOut(DATA_PORT,SET_HIGH0); /* DIR=write, data=1, clock=0 */

	wait_half_CLK();
    PortOut(DATA_PORT,SET_HIGH1); /* DIR=write, data=1, clock=1 */
 	wait_half_CLK();

}

void setlow(void)
{
    PortOut(DATA_PORT,SET_LOW0); /* DIR=write, data=0, clock=0 */
	wait_half_CLK();
    PortOut(DATA_PORT,SET_LOW1); /* DIR=write, data=0, clock=1 */
	wait_half_CLK();
}



void miiar_read(unsigned char PHY_ADDR,unsigned char REG_ADDR, unsigned short *value)
{
	int i;
    int data;


	for (i=0;i<32;i++) /* output 32 bit preamble */
		sethigh();

	setlow();  sethigh(); /*start of frame = 01 */
	sethigh(); setlow();  /*operation code = 10 : read */

	/* output PHY address */
	/* printf("PHY_ADDR=%x",PHY_ADDR); */
	if((PHY_ADDR & 0x10)>=1) sethigh(); else setlow();
	if((PHY_ADDR & 0x08)>=1) sethigh(); else setlow();
	if((PHY_ADDR & 0x04)>=1) sethigh(); else setlow();
	if((PHY_ADDR & 0x02)>=1) sethigh(); else setlow();
	if((PHY_ADDR & 0x01)>=1) sethigh(); else setlow();

	/* output Register address */
	/* printf("REG_ADDR=%x",REG_ADDR); */
	if((REG_ADDR & 0x10)>=1) sethigh(); else setlow();
	if((REG_ADDR & 0x08)>=1) sethigh(); else setlow();
	if((REG_ADDR & 0x04)>=1) sethigh(); else setlow();
	if((REG_ADDR & 0x02)>=1) sethigh(); else setlow();
	if((REG_ADDR & 0x01)>=1) sethigh(); else setlow();


	read_ta(); /*turn around 1  */
	read_ta2(); /*turn around 2  */

	data=read32();
#if 0
    osal_printf("\n%s  phy:%d reg:%d data:0x%x\n",__FUNCTION__,PHY_ADDR,REG_ADDR,data);
#endif
    *value = (unsigned short)data;
    
	return;
}  


void miiar_write(unsigned char PHY_ADDR,unsigned char REG_ADDR,unsigned short data)
{
	int i;
	BOOL ONE;
    
	for (i=0;i<32;i++)
		sethigh();

	setlow();  sethigh(); /*start of frame */
	setlow();  sethigh();  /*operation code */

	/* output PHY address */
	if((PHY_ADDR & 0x10)>=1) sethigh(); else setlow();
	if((PHY_ADDR & 0x08)>=1) sethigh(); else setlow();
	if((PHY_ADDR & 0x04)>=1) sethigh(); else setlow();
	if((PHY_ADDR & 0x02)>=1) sethigh(); else setlow();
	if((PHY_ADDR & 0x01)>=1) sethigh(); else setlow();

	/* output register address */
	if((REG_ADDR & 0x10)>=1) sethigh(); else setlow();
	if((REG_ADDR & 0x08)>=1) sethigh(); else setlow();
	if((REG_ADDR & 0x04)>=1) sethigh(); else setlow();
	if((REG_ADDR & 0x02)>=1) sethigh(); else setlow();
	if((REG_ADDR & 0x01)>=1) sethigh(); else setlow();

	sethigh(); /*turn around1 */
	setlow();  /*turn around2 */

	/* output data-16bit */
	for(i=15;i>=0;i--) {
		ONE=((data>>i) & 0x1)>0?1:0;
		if(ONE) sethigh(); else setlow();
	}

	/*read_ta(); */
	Dis_EN_pin();
#if 0
    osal_printf("\n%s  phy:%d reg:%d data:0x%x\n",__FUNCTION__,PHY_ADDR,REG_ADDR,data);
#endif

} /* end of w_phy() */




int io_mii_memory_write(uint32 memaddr,uint32 data)
{
    unsigned short value;
    uint32 tmp_data;


    /*address msb to REG1  lsb to REG0*/
    tmp_data = memaddr >>16;
    tmp_data = tmp_data&0xFFFF;
    value = (unsigned short)tmp_data;
    miiar_write(MII_ADDR,MII_REG1,value);


    tmp_data = memaddr & 0x0000FFFF;
    value = (unsigned short)tmp_data;
    miiar_write(MII_ADDR,MII_REG0,value);

    /*data msb to REG3  lsb to REG2*/
    tmp_data = data & 0xFFFF0000;
    tmp_data = tmp_data>>16;
    value = (unsigned short)tmp_data;
    miiar_write(MII_ADDR,MII_REG3,value);


    tmp_data = data & 0x0000FFFF;
    value = (unsigned short)tmp_data;
    miiar_write(MII_ADDR,MII_REG2,value);


    /*set REG6 to 0x804b trigger write procedure*/
    miiar_write(MII_ADDR,MII_REG6,0x804B);

    miiar_read(MII_ADDR,MII_REG6,&value);
    if(value != 0x4B)
        osal_printf("\nWrite Error!! mdio access not ready!! \n");    

    return RT_ERR_OK;

}

uint32 io_mii_memory_read(uint32 memaddr)
{
    unsigned short value;
    uint32 tmp_data;

    /*address msb to REG1  lsb to REG0*/
    tmp_data = memaddr >>16;
    tmp_data = tmp_data&0xFFFF;
    value = (unsigned short)tmp_data;
    miiar_write(MII_ADDR,MII_REG1,value);

    tmp_data = memaddr & 0x0000FFFF;
    value = (unsigned short)tmp_data;
    miiar_write(MII_ADDR,MII_REG0,value);

    /*set REG6 to 0x800b trigger read procedure*/
    miiar_write(MII_ADDR,MII_REG6,0x800B);
    
    miiar_read(MII_ADDR,MII_REG6,&value);
    if(value != 0xB)
        osal_printf("\nRead Error!! mdio access not ready!! \n");    


    /*read data from data msb from REG5  lsb from REG4*/
    tmp_data = 0;
    miiar_read(MII_ADDR,MII_REG5,&value);
    tmp_data = value;
    tmp_data = tmp_data <<16;
    miiar_read(MII_ADDR,MII_REG4,&value);
    tmp_data = tmp_data | value;

    return tmp_data;
}
#endif /*defined(CYGWIN_MDIO_IO)*/


int io_mii_phy_reg_write(uint8 phy_id,uint8 reg, uint16 value)
{
#if defined(CYGWIN_MDIO_IO)
    miiar_write(phy_id,reg,value);
    osal_time_mdelay(100);
#else
    if(phy_id);
    if(reg);
    if(value);
#endif

    return RT_ERR_OK;
}

int io_mii_phy_reg_read(uint8 phy_id,uint8 reg, uint16 *pValue)
{
#if defined(CYGWIN_MDIO_IO)
    miiar_read(phy_id,reg,pValue);
#else
    if(phy_id);
    if(reg);
    if(pValue);
#endif
    return RT_ERR_OK;
}

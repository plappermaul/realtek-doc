#include "spi_flash_private.h"




#ifndef DEVICE_DEFINITION_H
#define DEVICE_DEFINITION_H



/*        SPI DUMMY CLIBRATION      */
/*Note: User must make sure SPI device is abailable to calibrate  */
#define CONFIG_SPI_DUMMY_CLB 0
#if CONFIG_SPI_DUMMY_CLB
        #define CONFIG_SPI_DW_BITS_SET_VAL 0 //"0":disable DW_BITS_SET_VAL
#else
        #define CONFIG_SPI_DW_BITS_SET_VAL 1 //"1": enable DW_BITS_SET_VAL
#endif

/*Select SPIC and  flash I/O setting  type*/
/*Note: User must make sure SPI device is matching with SPIC's I/O setting  type  */
/*        SIO      */ 
#if 1
#define SUPPORT_SPI_SIO
       /*Note: fast read(0x0b) can't co-exist with normal read(0x3) */
	#define SUPPORT_SPI_SIO_FASTREAD 0 //"1": enable fastread , "0":disable fastread
#endif

/*        DIO      */ 
/*
Note:In DIO mode , if  wanna use auto-mode , please to call 
spic_init_auto_mode(struct dw_device *dev)  first
*/
#if 0
	#define SUPPORT_SPI_DIO
#endif


/*        QIO      */ 
/*
Note:In QIO mode , if  wanna use auto-mode , please to call 
spic_init_auto_mode(struct dw_device *dev)  first
*/
#if 0
	#define SUPPORT_SPI_QIO
#endif



/*For 3to4BYTES addressing , enable below config*/
#define CONFIG_SPI_3to4BYTES_ADDRESS_SUPPORT 1


#ifdef VENDER_MXIC
  //****************************************
  //********** MXIC: Multi-channel *********

    #ifdef SUPPORT_SPI_SIO
      #define DEF_RD_DUAL_TYPE           RD_MULTI_NONE // means SIO only
      #define DEF_WR_DUAL_TYPE           WR_MULTI_NONE // means SIO only

      #define DEF_RD_QUAD_TYPE           RD_MULTI_NONE // means SIO only  
      #define DEF_WR_QUAD_TYPE           WR_MULTI_NONE // means SIO only
    #endif

    #ifdef SUPPORT_SPI_DIO //DIO read , SIO write
      //#define DEF_RD_DUAL_TYPE           RD_DUAL_O //addrx1  
      #define DEF_RD_DUAL_TYPE           RD_DUAL_IO //addrx2
      #define DEF_WR_DUAL_TYPE            WR_MULTI_NONE //Not every SPI flash support this
    
      #define DEF_WR_QUAD_TYPE          WR_MULTI_NONE
      #define DEF_RD_QUAD_TYPE           RD_MULTI_NONE 
    #endif

    #ifdef SUPPORT_SPI_QIO //QIO read , QIO write
      #define DEF_RD_DUAL_TYPE           RD_MULTI_NONE 
      #define DEF_WR_DUAL_TYPE           WR_MULTI_NONE 
	
      //#define  DEF_RD_QUAD_TYPE           RD_QUAD_O 	
      #define  DEF_RD_QUAD_TYPE           RD_QUAD_IO 	
      //#define DEF_WR_QUAD_TYPE            WR_QUAD_I  //addrx1
      #define DEF_WR_QUAD_TYPE            WR_QUAD_II //addrx4
    #endif

   #define DEF_RD_DUAL_DUMMY_CYCLE    0x4
   //#define DEF_RD_QUAD_DUMMY_CYCLE    0x4 //some flash needed "0x4" dummy cycle
   #define DEF_RD_QUAD_DUMMY_CYCLE    0x6 //some flash needed "0x6" dummy cycle , mostly
   // #define DEF_RD_QUAD_DUMMY_CYCLE    0x8 //some flash needed "0x8" dummy cycle
   #define DEF_RD_FAST_DUMMY_CYCLE    0x8
   #define DEF_RD_TUNING_DUMMY_CYCLE  0x2
   #define DEF_WR_BLOCK_BOUND         256
   //#define DEF_WR_BLOCK_BOUND         1

  //****************************************
#endif


//////////////////////////////////////////////////////////////////   









#ifdef VENDER_SST
  //****************************************
  //****** SST single-channel *****
  #define DEF_RD_DUAL_TYPE           RD_MULTI_NONE 
  #define DEF_RD_QUAD_TYPE           RD_MULTI_NONE 
  #define DEF_WR_DUAL_TYPE           WR_MULTI_NONE 
  #define DEF_WR_QUAD_TYPE           WR_MULTI_NONE  
  #define DEF_RD_DUAL_DUMMY_CYCLE    0x0
  #define DEF_RD_QUAD_DUMMY_CYCLE    0x0
  #define DEF_RD_FAST_DUMMY_CYCLE    0x8
  #define DEF_RD_TUNING_DUMMY_CYCLE  0x0
  #define DEF_WR_BLOCK_BOUND         1
  //****************************************
#endif

#ifdef VENDER_SPANSION
  //****************************************
  //****** SPANDSION single-channel *****
  #define DEF_RD_DUAL_TYPE           RD_MULTI_NONE 
  #define DEF_RD_QUAD_TYPE           RD_MULTI_NONE 
  #define DEF_WR_DUAL_TYPE           WR_MULTI_NONE 
  #define DEF_WR_QUAD_TYPE           WR_MULTI_NONE  
  #define DEF_RD_DUAL_DUMMY_CYCLE    0x0
  #define DEF_RD_QUAD_DUMMY_CYCLE    0x0
  #define DEF_RD_FAST_DUMMY_CYCLE    0x8
  #define DEF_RD_TUNING_DUMMY_CYCLE  0x0
  #define DEF_WR_BLOCK_BOUND         256
  //****************************************
#endif

#ifdef VENDER_ATMEL
  //****************************************
  //********** ATMEL:Multi-channel *********
   #define DEF_RD_DUAL_TYPE           RD_DUAL_O 
   #define DEF_RD_QUAD_TYPE           RD_MULTI_NONE
   #define DEF_WR_DUAL_TYPE           WR_DUAL_I 
   #define DEF_WR_QUAD_TYPE           WR_MULTI_NONE
   #define DEF_RD_DUAL_DUMMY_CYCLE    0x1
   #define DEF_RD_QUAD_DUMMY_CYCLE    0x0
   #define DEF_RD_FAST_DUMMY_CYCLE    0x8
   #define DEF_RD_TUNING_DUMMY_CYCLE  0x0
   #define DEF_WR_BLOCK_BOUND         256
  //****************************************
#endif

#ifdef NUMONYX
  //****************************************
  //******* NUMONYX: Multi-channel *********
  #define DEF_RD_DUAL_TYPE           RD_DUAL_O 
  #define DEF_RD_QUAD_TYPE           RD_MULTI_NONE 
  #define DEF_WR_DUAL_TYPE           WR_DUAL_I 
  #define DEF_WR_QUAD_TYPE           WR_MULTI_NONE  
  #define DEF_RD_DUAL_DUMMY_CYCLE    0x8
  #define DEF_RD_QUAD_DUMMY_CYCLE    0x0
  #define DEF_RD_FAST_DUMMY_CYCLE    0x8
  #define DEF_RD_TUNING_DUMMY_CYCLE  0x0
  #define DEF_WR_BLOCK_BOUND         256
  //****************************************
#endif




#ifdef VENDER_WINBOND
  //****************************************
  //******** WINBOND: Multi-channel ********
  //#define DEF_RD_DUAL_TYPE            RD_DUAL_O 
  //#define DEF_RD_QUAD_TYPE            RD_QUAD_O 
  //#define DEF_WR_DUAL_TYPE            WR_MULTI_NONE 
  //#define DEF_WR_QUAD_TYPE            WR_QUAD_I  
  //#define DEF_RD_DUAL_DUMMY_CYCLE     0x8
  //#define DEF_RD_QUAD_DUMMY_CYCLE     0x8
  //#define DEF_RD_FAST_DUMMY_CYCLE     0x8
  //#define DEF_RD_TUNING_DUMMY_CYCLE   0x0
  //#define DEF_WR_BLOCK_BOUND          256

  #define DEF_RD_DUAL_TYPE           RD_DUAL_IO 
  #define DEF_RD_QUAD_TYPE           RD_QUAD_IO 
  #define DEF_WR_DUAL_TYPE           WR_MULTI_NONE 
  #define DEF_WR_QUAD_TYPE           WR_QUAD_I  
  #define DEF_RD_DUAL_DUMMY_CYCLE    0x4
  #define DEF_RD_QUAD_DUMMY_CYCLE    0x6
  #define DEF_RD_FAST_DUMMY_CYCLE    0x8
  #define DEF_RD_TUNING_DUMMY_CYCLE  0x0
  #define DEF_WR_BLOCK_BOUND         256
  //****************************************
#endif


#endif /*DEVICE_DEFINITION.H*/ 

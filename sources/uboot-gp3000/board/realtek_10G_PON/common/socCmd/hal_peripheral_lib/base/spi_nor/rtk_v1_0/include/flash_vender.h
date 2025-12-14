#include "spi_flash_private.h"

#ifndef FLASH_VENDER_H
#define FLASH_VENDER_H
/* --------------------------------------------------------------------
**
** File     : flash_vender.h
** Created  : $Date: 2008/10/30 $
** Abstract : This is the public header file for the SPI
**            low-level-driver
**
** --------------------------------------------------------------------
*/

////***** flash.data/flash_ID     
//// * DESCRIPTION
//// *  This data type is used to describe the manufacturer identifiation of Flash 
//// */
//struct flash_info {
//  uint8_t flash_id;
//  struct  flash_cmd *cmd;
//};
//
//
extern struct command_info cmd_info ;


struct FLASH_DEVICE {
  struct flash_info *VENDER[7]; // MXIC    ;
                                // SPANSION;
                                // SST     ;
                                // WINBOND ;
                                // NUMONYX ;
                                // ATMEL   ;
                               // ESMT   ;
};

//***** flash.data/flash_command
// * DESCRIPTION
// *  This data type is used to describe the FLASH controller's command
// *  number.  This data type is used by many of the API functions in
// *  the driver.
// */
//
// wrsr      
// pp       
// ppx2     
// ppx4     
// readx2   
// readx4   
// wrdi     
// rdsr     
// wren     
// fast_read
// rdid     
// ce       
// se       

struct flash_cmd MXIC_CMD = {
       0x01, // wrsr      
       0x02, // pp       
       0x02, // pp_4b
       0x02, // pp_ix2    
       0x02, // pp_iix2     
       0x02, // pp_ix4     
       0x38, // pp_iix4     
       //0x02, // read_ix2  
       0x03, // read_4b
       0x3b, // read_ix2  
       0xbb, // read_iox2   
      // 0x02, // read_ix4   
       0x6b, // read_ix4  
       0xeb, // read_iox4   
       0x04, // wrdi     
       0x05, // rdsr     
       0x06, // wren     
       0x0b, // fast_read
       0x9f, // rdid     
       0xc7, // ce       
       0x20  // se       
}; 
/*****/
struct flash_cmd SPANSION_CMD = {
       0x01, // wrsr      
       0x02, // pp       
       0x02, // pp_4b
       0x02, // pp_ix2    
       0x02, // pp_iix2     
       0x02, // pp_ix4   
       0x02, // pp_iix4
       0x03, // read_4b
       0x03, // read_ix2  
       0x03, // read_iox2   
       0x03, // read_ix4   
       0x03, // read_iox4   
       0x04, // wrdi     
       0x05, // rdsr     
       0x06, // wren     
       0x0b, // fast_read
       0x9f, // rdid     
       0xc7, // ce       
       0xd8  // se       
} ;
/*****/
struct flash_cmd SST_CMD = {
       0x01, // wrsr      
       0x02, // pp       
       0x02, // pp_4b
       0x02, // pp_ix2    
       0x02, // pp_iix2     
       0x02, // pp_ix4     
       0x02, // pp_iix4     
       0x03, // read_4b
       0x03, // read_ix2  
       0x03, // read_iox2   
       0x03, // read_ix4   
       0x03, // read_iox4   
       0x04, // wrdi     
       0x05, // rdsr     
       0x06, // wren     
       0x0b, // fast_read
       0x9f, // rdid     
       0xc7, // ce       
       0x20  // se             
} ;
/*****/
struct flash_cmd WINBOND_CMD = {
       0x01, //wrsr     
       0x02, //pp       
       0x12, //pp_4b
       0x02, //pp_ix2    
       0x02, //pp_iix2     
       0x32, //pp_ix4    
       0x02, //pp_iix4
       0x13, //read_4b
       0xbb, //read_ix2  
       0x3b, //read_iox2   
       0x6b, //read_ix4   
       0xeb, //read_iox4   
       0x04, //wrdi     
       0x05, //rdsr     
       0x06, //wren    
       0x0b, //fast_read
       0x9f, //rdid     
       0xc7, //ce       
       0x20  //se       
} ;
/*****/

/*****/
struct flash_cmd NUMONYX_CMD = {
       0x01, //wrsr     
       0x02, //pp
       0x02, //pp_4b
       0xa2, //pp_ix2    
       0x02, //pp_iix2     
       0x02, //pp_ix4    
       0x02, //pp_iix4  
       0x03, //read_4b
       0x3b, //read_ix2  
       0x02, //read_iox2   
       0x02, //read_ix4   
       0x02, //read_iox4   
       0x04, //wrdi     
       0x05, //rdsr     
       0x06, //wren    
       0x0b, //fast_read
       0x9e, //rdid     
       0xc7, //ce       
       0x20  //se       
} ;
/*****/

struct flash_cmd ATMEL_CMD = {
       0x01, //wrsr     
       0x02, //pp    
       0x02, //pp_4b
       0xa2, //pp_ix2    
       0x02, //pp_iix2     
       0x02, //pp_ix4    
       0x02, //pp_iix4  
       0x03, //read_4b
       0x3b, //read_ix2  
       0x02, //read_iox2   
       0x02, //read_ix4   
       0x02, //read_iox4   
       0x04, //wrdi     
       0x05, //rdsr     
       0x06, //wren    
       0x0b, //fast_read
       0x9f, //rdid     
       0xc7, //ce       
       0x20  //se       
} ;
/*****/
struct flash_cmd ESMT_CMD = {
       0x01, // wrsr      
       0x02, // pp       
       0x02, //pp_4b
       0x02, // pp_ix2    
       0x02, // pp_iix2     
       0x02, // pp_ix4     
       0x38, // pp_iix4   
       0x03, // read_4b
       0x02, // read_ix2  
       0xbb, // read_iox2   
       0x02, // read_ix4   
       0xeb, // read_iox4   
       0x04, // wrdi     
       0x05, // rdsr     
       0x06, // wren     
       0x0b, // fast_read
       0x9f, // rdid     
       0xc7, // ce       
       0x20  // se       
}; 



struct flash_info INFO_MXIC = {
       0xC2,    // flash_id 
       "MXIC",   // vender
       &MXIC_CMD // flash_cmd
} ;

struct flash_info INFO_SPANSION = {
       0x01,        // flash_id 
       "SPANSION", 
       &SPANSION_CMD // flash_cmd
} ;

struct flash_info INFO_SST = {
       0xBF,    //flash_id 
       "SST",  
       &SST_CMD  //flash_cmd
} ;

struct flash_info INFO_WINBOND = {
       0xEF,       // flash_id
       "WINBOND", 
       &WINBOND_CMD // struct flash_cmd
} ;

struct flash_info INFO_NUMONYX = {
       0x20,       // flash_id
       "NUMONYX", 
       &NUMONYX_CMD // struct flash_cmd
} ;

struct flash_info INFO_ATMEL = {
       0x1F,       // flash_id
       "ATMEL", 
       &ATMEL_CMD  // struct flash_cmd
} ;

struct flash_info INFO_ESMT = {
       0x8c,       // flash_id
       "ESMT", 
       &ESMT_CMD  // struct flash_cmd
} ;


struct FLASH_DEVICE DEVICE = {
  &INFO_MXIC    ,
  &INFO_SPANSION,
  &INFO_SST     ,
  &INFO_WINBOND ,
  &INFO_NUMONYX ,
  &INFO_ATMEL,
  &INFO_ESMT
};
/*****/

#endif /*FLASH_VENDER_H*/ 

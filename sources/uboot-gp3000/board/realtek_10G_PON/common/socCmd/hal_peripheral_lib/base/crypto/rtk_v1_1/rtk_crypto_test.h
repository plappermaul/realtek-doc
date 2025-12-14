/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef __RTK_CRYPTO_TEST_H__
#define __RTK_CRYPTO_TEST_H__

//config test mode.

// select the DDLEN = IPS_DDLEN_8, IPS_DDLEN_12, IPS_DDLEN_16, IPS_DDLEN_20
//  (0<<24), (1<<24), (2<<24), (3<<24)
// Destination Descriptor Length : Length is x*DW : for SHA-2/SHA-1/MD5
#define IPS_DDLEN_NOW (3<<24) //IPS_DDLEN_20
//#define IPS_DDLEN_NOW (2<<24) //IPS_DDLEN_16
//#define IPS_DDLEN_NOW (1<<24) //IPS_DDLEN_12
//#define IPS_DDLEN_NOW (0<<24) //IPS_DDLEN_8

//select crypto test DMA memory type, sram or SDRAM
#define CRYPTO_TEST_SRAM 1
#define CRYPTO_TEST_NOT_ALIGN 3 //select 0~3
//select DMA burst length
// test dma burst length from 16, 32, 64bytes
#define CRYPTO_TEST_DMA_MODE 2  //select 0~2
#define MAX_A2EO_LIMIT 1

#define CRYPTO_VECTOR_NEO 1 //select neo crypto test vector

//#define CRYPTO_HASH_PADDING_WITH_DATA 1 // select hash padding with data

//#define CRYPTO_LOOPBACK_DEBUG 1

#endif  //__RTK_CRYPTO_TEST_H__


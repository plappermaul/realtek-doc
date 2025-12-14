#ifndef __RTK_CRYPTO_BASE_TYPE_H__
#define __RTK_CRYPTO_BASE_TYPE_H__

#include "rtk_crypto_base_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/// @cond DOXYGEN_GENERAL_REG_TYPE || DOXYGEN_CRYPTO_REG_TYPE

/**
 * @addtogroup hs_hal_crypto_reg CRYPTO Register Type.
 * @ingroup hs_hal_crypto
 * @{
 */

/**
 * \brief Union type to access crypto_srcdesc_status_reg (@ 0x00000000).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000000) Source Descriptor FIFO status Register             */

  struct {
    __IM  uint32_t fifo_empty_cnt : 16;       /*!< [15..0] Source Descriptor FIFO empty counter                      */
    __IM  uint32_t            : 8;            /*!< [23..16] reserved                                                 */
    __IOM uint32_t src_fail   : 1;            /*!< [24..24] Source Descriptor fail interrupt                         */
    __IM  uint32_t src_fail_status : 2;       /*!< [26..25] Source Descriptor fail status                            */
    __IM  uint32_t            : 3;            /*!< [29..27] reserved                                                 */
    __IOM uint32_t pk_up      : 1;            /*!< [30..30] packet base update wptr to engine                        */
    __IOM uint32_t src_rst    : 1;            /*!< [31..31] Source Descriptor reset(only for pk_up=1'b1              */
  } b;                                        /*!< bit fields for crypto_srcdesc_status_reg                          */
} crypto_srcdesc_status_reg_t, *pcrypto_srcdesc_status_reg_t;

/**
 * \brief Union type to access crypto_sdfw_reg (@ 0x00000004).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000004) IPSec Source Descriptor first word                 */

  struct {
    __IOM uint32_t sdfw       : 32;           /*!< [31..0] IPSec Source Descriptor first word                        */
  } b;                                        /*!< bit fields for crypto_sdfw_reg                                    */
} crypto_sdfw_reg_t, *pcrypto_sdfw_reg_t;

/**
 *  \brief Union type to access crypto_sdsw_reg (@ 0x00000008).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000008) IPSec Source Descriptor second word                */

  struct {
    __IOM uint32_t sdsw       : 32;           /*!< [31..0] IPSec Source Descriptor second word                       */
  } b;                                        /*!< bit fields for crypto_sdsw_reg                                    */
} crypto_sdsw_reg_t, *pcrypto_sdsw_reg_t;

/**
 * \brief Union type to access crypto_srcdesc_sdptr_reg (@ 0x0000000C).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000000) Source Descriptor FIFO status Register             */

  struct {
    __IM  uint32_t swptr      : 16;           /*!< [15..8] Source Descriptor FIFO write pointer                      */
    __IM  uint32_t srptr      : 16;           /*!< [23..16] Source Descriptor FIFO read pointer                      */
  } b;
} crypto_srcdesc_sdptr_reg_t, *pcrypto_srcdesc_sdptr_reg_t;

/**
 * \brief Union type to access crypto_ipscsr_reset_isr_conf_reg (@ 0x00000010).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000010) IPSec Command/Status Register,
                                               *                  for reset / isr poll status
                                               */

  struct {
    __OM  uint32_t soft_rst   : 1;            /*!< [0..0] Software Reset write 1 to reset                            */
    __IM  uint32_t            : 2;
    __IM  uint32_t dma_busy   : 1;            /*!< [3..3] Ipsec dma busy                                             */
    __IOM uint32_t cmd_ok     : 1;            /*!< [4..4] Command OK interrupt                                       */
    __IM  uint32_t            : 2;
    __IOM uint32_t intr_mode  : 1;            /*!< [7..7] Select ok interrupt mode                                   */
    __IM  uint32_t ok_intr_cnt : 8;           /*!< [15..8] OK interrupt counter                                      */
    __IOM uint32_t clear_ok_intr_num : 8;     /*!< [23..16] Clear OK interrupt number                                */
    __IM  uint32_t            : 7;
    __OM  uint32_t ipsec_rst  : 1;            /*!< [31..31] Ipsec engine Reset Write 1 to reset the crypto engine
                                               *    and DMA engine
                                               */
  } b;                                        /*!< bit fields for crypto_ipscsr_reset_isr_conf_reg                   */
} crypto_ipscsr_reset_isr_conf_reg_t, *pcrypto_ipscsr_reset_isr_conf_reg_t;

/**
 * \brief Union type to access crypto_ipscsr_int_mask_reg (@ 0x00000014).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000014) IPSec Command/Status Register, for interrupt mask  */

  struct {
    __IOM uint32_t cmd_ok_m   : 1;            /*!< [0..0] Source Descriptor fail interrupt Mask 1: Mask Enable 
                                               * 0: Mask Disable   
                                               */
    __IOM uint32_t src_fail_m : 1;            /*!< [1..1] Source Descriptor Error 0 Interrupt Mask 1: Mask Enable
                                               *   0: Mask Disable
                                               */
    __IOM uint32_t dst_fail_m : 1;            /*!< [1..1] Destination Descriptor Error 0 Interrupt Mask 1: Mask Enable
                                               *   0: Mask Disable
                                               */
    __IOM uint32_t src_err0_m : 1;            /*!< [3..3] Source Descriptor Error 0 Interrupt Mask 1: Mask Enable
                                               *   0: Mask Disable
                                               */
    __IOM uint32_t src_err1_m : 1;            /*!< [4..4] Source Descriptor Error 1 Interrupt Mask 1: Mask Enable
                                               *   0: Mask Disable
                                               */
    __IOM uint32_t src_err2_m : 1;            /*!< [5..5] Source Descriptor Error 2 Interrupt Mask 1: Mask Enable
                                               *   0: Mask Disable
                                               */
    __IOM uint32_t src_err3_m : 1;            /*!< [6..6] Source Descriptor Error 3 Interrupt Mask 1: Mask Enable
                                               *   0: Mask Disable
                                               */
    __IOM uint32_t src_err4_m : 1;            /*!< [7..7] Source Descriptor Error 4 Interrupt Mask 1: Mask Enable
                                               *   0: Mask Disable
                                               */
    __IOM uint32_t src_err5_m : 1;            /*!< [8..8] Source Descriptor Error 5 Interrupt Mask 1: Mask Enable
                                               *   0: Mask Disable
                                               */
    __IOM uint32_t src_err6_m : 1;            /*!< [9..9] Source Descriptor Error 6 Interrupt Mask 1: Mask Enable
                                               *   0: Mask Disable
                                               */
    __IOM uint32_t src_err7_m : 1;            /*!< [10..10] Source Descriptor Error 7 Interrupt Mask 1: Mask Enable
                                               *    0: Mask Disable
                                               */
    __IOM uint32_t src_err8_m : 1;            /*!< [11..11] Source Descriptor Error 8 Interrupt Mask 1: Mask Enable
                                               *   0: Mask Disable
                                               */
    __IOM uint32_t src_err9_m : 1;            /*!< [12..12] Source Descriptor Error 9 Interrupt Mask 1: Mask Enable
                                               *   0: Mask Disable
                                               */
    __IOM uint32_t dst_err1_m : 1;            /*!< [13..13] Destination Descriptor Error 1 Interrupt Mask 1: Mask
                                               *   Enable 0: Mask Disable
                                               */
    __IOM uint32_t dst_err2_m : 1;            /*!< [14..14] Destination Descriptor Error 2 Interrupt Mask 1: Mask
                                               *   Enable 0: Mask Disable
                                               */
    __IOM uint32_t dst_err3_m : 1;            /*!< [15..15] Destination Descriptor Error 3 Interrupt Mask 1: Mask
                                               *   Enable 0: Mask Disable
                                               */
    __IOM uint32_t dst_err4_m : 1;            /*!< [16..16] Destination Descriptor Error 4 Interrupt Mask 1: Mask
                                               *   Enable 0: Mask Disable
                                               */
    __IOM uint32_t dst_err5_m : 1;            /*!< [17..17] Destination Descriptor Error 5 Interrupt Mask 1: Mask
                                               *   Enable 0: Mask Disable
                                               */
    __IOM uint32_t dst_err6_m : 1;            /*!< [18..18] Destination Descriptor Error 6 Interrupt Mask 1: Mask
                                               *   Enable 0: Mask Disable
                                               */
  } b;                          /*!< bit fields for crypto_ipscsr_int_mask_reg                         */
} crypto_ipscsr_int_mask_reg_t, *pcrypto_ipscsr_int_mask_reg_t;

/**
 * \brief Union type to access crypto_ipscsr_debug_reg (@ 0x00000018).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000018) IPSec Command/Status Register, for debug           */

  struct {
    __IOM uint32_t dma_wait_cycle : 16;       /*!< [15..0] Software Reset                                            */
    __IOM uint32_t arbiter_mode : 1;          /*!< [16..16] dma arbiter mode: 1'b0 : round-robin 1'b1 : detect
                                               *   fifo wasted level
                                               */
    __IM  uint32_t            : 3;            /*!< [17..17] reserved                                                 */
    __IOM uint32_t debug_port_sel : 4;        /*!< [23..20] Debug port selection: 4'd0 : engine_dbg,
                                               *   4'd1 : dma_lexra_dbg, 4'd2 : dma_rx_dbg, 4'd3 : dma_tx_dbg
                                               */
    __IOM uint32_t engine_clk_en : 1;         /*!< [24..24] Ipsec Engine clock enable                                */
    __IM  uint32_t            : 3;            /*!< [27..25] reserved                                                 */
    __IOM uint32_t mst_bad_sel   : 2;         /*!< [29..28] <for debug port> select Lexra master data bus  (option)
                                               * 2'd0 :  mst_ipsec_bad[07:00]       2'd1:  mst_ipsec_bad[15:08]
                                               * 2'd2 :  mst_ipsec_bad[23:16]       2'd3:  mst_ipsec_bad[31:24]   
                                               */
    __IM  uint32_t            : 1;            /*!< [30..30] reserved                                                 */
    __IOM uint32_t debug_wb   : 1;            /*!< [31..31] Debug : write back mode                                  */
  } b;                                        /*!< bit fields for crypto_ipscsr_debug_reg                            */
} crypto_ipscsr_debug_reg_t, *pcrypto_ipscsr_debug_reg_t;

/**
 * \brief Union type to access crypto_ipscsr_err_stats_reg (@ 0x0000001C).
 */
typedef union {
  __IOM uint16_t w;                           /*!< (@ 0x0000001C) IPSec Command/Status Register, for error status    */

  struct {
    __IOM uint16_t src_err0   : 1;            /*!< [0..0] Source Descriptor Error 0 interrupt First segment descriptor
                                               *   is not set (FS=0), when descriptor is pointing to the first
                                               *   segment of the packet. (or) First segment descriptor is
                                               *   set (FS=1), when descriptor is polling to the first segment
                                               *   of the packet. Write 1 to clear
                                               */
    __IOM uint16_t src_err1   : 1;            /*!< [1..1] Source Descriptor Error 1 interrupt Last segment descriptor
                                               *   is not set (LS=1), when descriptor is pointing to the first
                                               *   segment of the packet. Write 1 to clear
                                               */
    __IOM uint16_t src_err2   : 1;            /*!< [2..2] Source Descriptor Error 2 interrupt 1. Source Descriptor
                                               *   Starting Address error(SDSA[1:0]!=2'd0) 2. memory protection
                                               *   event Write 1 to clear
                                               */
    __IOM uint16_t src_err3   : 1;            /*!< [3..3] Source Descriptor Error 3 interrupt Length error :
                                               *   1. first descriptor & (CL!=3)
                                               *   2. ((CL=0)&(kl==0)&(il==0)&(pl==0)&(shil==0)
                                               *   Write 1 to clear
                                               */
    __IOM uint16_t src_err4   : 1;            /*!< [4..4] Source Descriptor Error 4 interrupt Source Data Buffer
                                               *   Pointer error(SDDB[1:0]!=2'd0) Write 1 to clear
                                               */
    __IOM uint16_t src_err5   : 1;            /*!< [5..5] Source Descriptor Error 5 interrupt Data length = a2eo(epl)
                                               *   + enl + apl = 0 Write 1 to clear
                                               */
    __IOM uint16_t src_err6   : 1;            /*!< [6..6] Source Descriptor Error 6 interrupt Sum(a2eo)!=header
                                               *   total length Write 1 to clear
                                               */
    __IOM uint16_t src_err7   : 1;            /*!< [7..7] Source Descriptor Error 7 interrupt Sum(enl)!=encryption
                                               *   total length Write 1 to clear
                                               */
    __IOM uint16_t src_err8   : 1;            /*!< [8..8] Source Descriptor Error 8 interrupt Sum(apl)!=hash padding
                                               *   total length Write 1 to clear
                                               */
    __IOM uint16_t src_err9   : 1;            /*!< [9..9] Source Descriptor Error 9 interrupt Sum(enl)!=encryption
                                               *   padding total length Write 1 to clear
                                               */
    __IOM uint16_t des_err1   : 1;            /*!< [10..10] Destination Descriptor Error 1 interrupt 1. Destination
                                               *   Descriptor Starting Address error(DDSA[1:0]!=2'd0) 2. memory
                                               *   protection error Write 1 to clear
                                               */
    __IOM uint16_t des_err2   : 1;            /*!< [11..11] Destination Descriptor Error 2 interrupt (ENC=1'b1),
                                               *   when only active hash engine Write 1 to clear
                                               */
    __IOM uint16_t des_err3   : 1;            /*!< [12..12] Destination Descriptor Error 2 interrupt (ENC=1'b0),
                                               *   when only active encryption engine Write 1 to clear
                                               */
    __IOM uint16_t des_err4   : 1;            /*!< [13..13] Destination Descriptor Error 4 interrupt 1. (enl_sum
                                               *   > enc_len) 2. (enl==0) & (apl==0) Write 1 to clear
                                               */
    __IOM uint16_t des_err5   : 1;            /*!< [14..14] Destination Descriptor Error 5 interrupt 1. md5: adl>16
                                               *   2. sha1: adl>20 3. sha224: adl>28 4. sha256: adl>32 5.
                                               *   sha384: adl>48 6. sha512: adl>64 Write 1 to clear
                                               */
    __IOM uint16_t des_err6   : 1;            /*!< [15..15] Destination Descriptor Error 6 interrupt when set sequential
                                               *   hash 1. md5: adl!=16 2. sha1: adl!=20 3. sha224/256: adl!=32
                                               *   4. sha384/512: adl!=64 Write 1 to clear
                                               */
  } b;                                        /*!< bit fields for crypto_ipscsr_err_stats_reg */
} crypto_ipscsr_err_stats_reg_t, *pcrypto_ipscsr_err_stats_reg_t;

/**
 * \brief Union type to access crypto_ipscsr_a2eo_sum_reg (@ 0x00000020).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000020) IPSec Command/Status Register, for sum of a2eo             */
  
  struct {
    __IM  uint32_t a2eo_sum   : 11;           /*!< [10..0] sum of a2eo: header total length                                  */
  } b;                                        /*!< bit fields for crypto_ipscsr_a2eo_sum_reg */
} crypto_ipscsr_a2eo_sum_reg_t, *pcrypto_ipscsr_a2eo_sum_reg_t;

/**
 * \brief Union type to access crypto_ipscsr_enl_sum_reg (@ 0x00000024).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000024) IPSec Command/Status Register, for sum of enl              */
  
  struct {
    __IM  uint32_t enl_sum    : 24;           /*!< [23..0] sum of enl: encryption total length                               */
  } b;                                        /*!< bit fields for crypto_ipscsr_enl_sum_reg */
} crypto_ipscsr_enl_sum_reg_t, *pcrypto_ipscsr_enl_sum_reg_t;

/**
 * \brief Union type to access crypto_ipscsr_apl_sum_reg (@ 0x00000028).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000028) IPSec Command/Status Register, for sum of apl              */
  
  struct {
    __IM  uint32_t apl_sum    : 12;           /*!< [11..0] sum of apl: hash padding total length                             */
  } b;                                        /*!< bit fields for crypto_ipscsr_apl_sum_reg */
} crypto_ipscsr_apl_sum_reg_t, *pcrypto_ipscsr_apl_sum_reg_t;

/**
 * \brief Union type to access crypto_ipscsr_epl_sum_reg (@ 0x0000002C).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000002C) IPSec Command/Status Register, for sum of epl              */
  
  struct {
    __IM  uint32_t epl_sum    : 6;            /*!< [5..0] sum of epl: encryption padding total length                        */
  } b;                                        /*!< bit fields for crypto_ipscsr_epl_sum_reg */
} crypto_ipscsr_epl_sum_reg_t, *pcrypto_ipscsr_epl_sum_reg_t;

/**
 * \brief Union type to access crypto_ipscsr_swap_burst_reg (@ 0x00000030).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000030) IPSec Command/Status Register, for swap / burst
                                               *                  numbers
                                               */

  struct {
    __IOM uint32_t set_swap   : 1;            /*!< [0..0] Byte swap for command setting data 1: Enable 0: Disable    */
    __IOM uint32_t key_iv_swap : 1;           /*!< [1..1] Byte swap for key and iv 1: Enable 0: Disable              */
    __IOM uint32_t key_pad_swap : 1;          /*!< [2..2] Byte swap for hmac key 1: Enable 0: Disable                */
    __IOM uint32_t hash_inital_value_swap : 1;/*!< [3..3] Byte swap for sequential hash initial value 1: Enable
                                               *   0: Disable
                                               */
    __IOM uint32_t dma_in_little_endian : 1;  /*!< [4..4] Input data is little endian 1: little endian 0: big endian */

    __IM  uint32_t            : 3;            /*!< [7..5] reserved                                                   */
    __IOM uint32_t tx_byte_swap : 1;          /*!< [8..8] Byte swap for dma_tx engine input data 1: Enable 0: Disable */
    __IOM uint32_t data_out_little_endian : 1;/*!< [9..9] Output data is little endian 1: little endian 0: big
                                               *   endian
                                               */
    __IOM uint32_t mac_out_little_endian : 1; /*!< [10..10] Output mac is little endian 1: little endian 0: big
                                               *   endian
                                               */
    __IOM uint32_t rx_wd_swap : 1;            /*!< [11..11] Word swap for dma_rx engine input data 1: Enable 0:
                                               *   Disable
                                               */
    __IOM uint32_t tx_wd_swap : 1;            /*!< [12..12] Word swap for dma_tx engine input data 1: Enable 0:
                                               *   Disable
                                               */
    __IOM uint32_t auto_padding_swap : 1;     /*!< [13..13] byte swap for padding_len input data 1: Enable 0:
                                               *   Disable
                                               */
    __IM  uint32_t            : 2;            /*!< [15..14] reserved                                                 */
    __IOM uint32_t dma_burst_length : 6;      /*!< [21..16] dma burst length Max : 16 Min: 1                         */
    __IM  uint32_t            : 2;            /*!< [23..22] reserved                                                 */
    __IOM uint32_t md5_output_data_byte_swap : 1; /*!< [24..24] byte swap for md5 output data 1: Enable 0:
                                               *   Disable
                                               */
    __IOM uint32_t md5_input_data_byte_swap  : 1; /*!< [25..25] byte swap for md5 output data 1: Enable 0:
                                               *   Disable
                                               */
    __IM  uint32_t            : 6;            /*!< [31..26] reserved                                                 */

  } b;                                        /*!< bit fields for crypto_ipscsr_swap_burst_reg */
} crypto_ipscsr_swap_burst_reg_t, *pcrypto_ipscsr_swap_burst_reg_t;

/**
  \brief Union type to access crypto_ipscsr_engine_dbg_reg (@ 0x00000040).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000040) IPSec Command/Status Register, for engine debug            */
  
  struct {
    __IM  uint32_t engine_dbg : 32;           /*!< [31..0] Crypto engine debug values                                        */
  } b;                                        /*!< bit fields for crypto_ipscsr_engine_dbg_reg */
} crypto_ipscsr_engine_dbg_reg_t, *pcrypto_ipscsr_engine_dbg_reg_t;

/**
  \brief Union type to access crypto_ipscsr_dma_lx_dbg_reg (@ 0x00000044).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000044) IPSec Command/Status Register, for DMA lx debug            */
  
  struct {
    __IM  uint32_t dma_lx_dbg : 32;           /*!< [31..0] DMA lx debug values                                               */
  } b;                                        /*!< bit fields for crypto_ipscsr_dma_lx_dbg_reg */
} crypto_ipscsr_dma_lx_dbg_reg_t, *pcrypto_ipscsr_dma_lx_dbg_reg_t;

/**
  \brief Union type to access crypto_ipscsr_dma_rx_dbg_reg (@ 0x00000048).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000048) IPSec Command/Status Register, for DMA rx debug            */
  
  struct {
    __IM  uint32_t dma_rx_dbg : 32;           /*!< [31..0] DMA rx debug values                                               */
  } b;                                        /*!< bit fields for crypto_ipscsr_dma_rx_dbg_reg */
} crypto_ipscsr_dma_rx_dbg_reg_t, *pcrypto_ipscsr_dma_rx_dbg_reg_t;

/**
  \brief Union type to access crypto_ipscsr_dma_tx_dbg_reg (@ 0x0000004C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000004C) IPSec Command/Status Register, for DMA tx debug            */
  
  struct {
    __IM  uint32_t dma_tx_dbg : 32;           /*!< [31..0] DMA tx debug values                                               */
  } b;                                        /*!< bit fields for crypto_ipscsr_dma_tx_dbg_reg */
} crypto_ipscsr_dma_tx_dbg_reg_t, *pcrypto_ipscsr_dma_tx_dbg_reg_t;

/**
  \brief Union type to access crypto_ipscsr_sta_sdes_cfg_reg (@ 0x00000050).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000050) IPSec Command/Status Register, for source descriptor
                                                                  configure status                                           */
  
  struct {
    __IM  uint32_t sta_sdes_cfg : 32;         /*!< [31..0] Source descriptor configure status values                         */
  } b;                                        /*!< bit fields for crypto_ipscsr_sta_sdes_cfg_reg */
} crypto_ipscsr_sta_sdes_cfg_reg_t, *pcrypto_ipscsr_sta_sdes_cfg_reg_t;

/**
  \brief Union type to access crypto_ipscsr_sta_sdes_ptr_reg (@ 0x00000054).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000054) IPSec Command/Status Register, for source descriptor
                                                                  pointer status                                             */
  
  struct {
    __IM  uint32_t sta_sdes_ptr : 32;         /*!< [31..0] Source descriptor pointer status values                           */
  } b;                                        /*!< bit fields for crypto_ipscsr_sta_sdes_ptr_reg */
} crypto_ipscsr_sta_sdes_ptr_reg_t, *pcrypto_ipscsr_sta_sdes_ptr_reg_t;

/**
  \brief Union type to access crypto_ipscsr_sta_sdes_cmd1 (@ 0x00000058).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000058) IPSec Command/Status Register, for source descriptor
                                                                  command 1 status                                           */
  
  struct {
    __IM  uint32_t sta_sdes_cmd1 : 32;        /*!< [31..0] Source descriptor command 1 status values                         */
  } b;                                        /*!< bit fields for crypto_ipscsr_sta_sdes_cmd1 */
} crypto_ipscsr_sta_sdes_cmd1_t, *pcrypto_ipscsr_sta_sdes_cmd1_t;

/**
  \brief Union type to access crypto_ipscsr_sta_sdes_cmd2 (@ 0x0000005C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000005C) IPSec Command/Status Register, for source descriptor
                                                                  command 2 status                                           */
  
  struct {
    __IM  uint32_t sta_sdes_cmd2 : 32;        /*!< [31..0] Source descriptor command 2 status values                         */
  } b;                                        /*!< bit fields for crypto_ipscsr_sta_sdes_cmd2 */
} crypto_ipscsr_sta_sdes_cmd2_t, *pcrypto_ipscsr_sta_sdes_cmd2_t;

/**
  \brief Union type to access crypto_ipscsr_sta_sdes_cmd3 (@ 0x00000060).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000060) IPSec Command/Status Register, for source descriptor
                                                                  command 3 status                                           */
  
  struct {
    __IM  uint32_t sta_sdes_cmd3 : 32;        /*!< [31..0] Source descriptor command 3 status values                         */
  } b;                                        /*!< bit fields for crypto_ipscsr_sta_sdes_cmd3 */
} crypto_ipscsr_sta_sdes_cmd3_t, *pcrypto_ipscsr_sta_sdes_cmd3_t;

/**
  \brief Union type to access crypto_ipscsr_sta_ddes_cfg_reg (@ 0x00000064).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000064) IPSec Command/Status Register, for destination
                                                                  descriptor configure status                                */
  
  struct {
    __IM  uint32_t sta_ddes_cfg : 32;         /*!< [31..0] Destination descriptor configure status values                    */
  } b;                                        /*!< bit fields for crypto_ipscsr_sta_ddes_cfg_reg */
} crypto_ipscsr_sta_ddes_cfg_reg_t, *pcrypto_ipscsr_sta_ddes_cfg_reg_t;

/**
  \brief Union type to access crypto_ipscsr_sta_ddes_ptr_reg (@ 0x00000068).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000068) IPSec Command/Status Register, for destination
                                                                  descriptor pointer status                                  */
  
  struct {
    __IM  uint32_t sta_ddes_ptr : 32;         /*!< [31..0] Destination descriptor pointer status values                      */
  } b;                                        /*!< bit fields for crypto_ipscsr_sta_ddes_ptr_reg */
} crypto_ipscsr_sta_ddes_ptr_reg_t, *pcrypto_ipscsr_sta_ddes_ptr_reg_t;

/**
 * \brief Union type to access crypto_crc_rst_reg (@ 0x00000100).
 */
typedef union {
  __OM  uint32_t w;                           /*!< (@ 0x00000100) crc reset                                          */

  struct {
    __OM  uint32_t rst        : 1;            /*!< [0..0] CRC reset                                                  */
  } b;                                        /*!< bit fields for crypto_crc_rst_reg */
} crypto_crc_rst_reg_t, *pcrypto_crc_rst_reg_t;

/**
 * \brief Union type to access crypto_crc_op_reg (@ 0x00000104).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000104) crc operation                                      */

  struct {
    __IOM uint32_t crc_iswap  : 3;            /*!< [2..0] swap input data                                            */
    __IOM uint32_t crc_oswap  : 1;            /*!< [3..3] swap output data                                           */
    __IOM uint32_t crc_sel    : 3;            /*!< [6..4] crc sel                                                    */
    __IOM uint32_t crc_dma    : 1;            /*!< [7..7] DMA mode                                                   */
    __IOM uint32_t crc_be     : 2;            /*!< [9..8] big endian                                                 */

    __IM  uint32_t            : 2;
    __IOM uint32_t crc_last   : 1;            /*!< [12..12] crc last                                                 */

    __IM  uint32_t            : 3;
    __IOM uint32_t crc_length : 16;           /*!< [31..16] length                                                   */
  } b;                                        /*!< bit fields for crypto_crc_op_reg */
} crypto_crc_op_reg_t, *pcrypto_crc_op_reg_t;

/**
 * \brief Union type to access crypto_crc_poly_reg (@ 0x00000108).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000108) CRC polynomia register                             */

  struct {
    __IOM uint32_t crc_poly   : 32;           /*!< [31..0] crc polynomial coefficients                               */
  } b;                                        /*!< bit fields for crypto_crc_poly_reg */
} crypto_crc_poly_reg_t, *pcrypto_crc_poly_reg_t;

/**
 * \brief Union type to access crypto_crc_iv_reg (@ 0x0000010C).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000010C) CRC initial value register                         */

  struct {
    __IOM uint32_t crc_init   : 32;           /*!< [31..0] crc init value                                            */
  } b;                                        /*!< bit fields for crypto_crc_iv_reg */
} crypto_crc_iv_reg_t, *pcrypto_crc_iv_reg_t;

/**
 * \brief Union type to access crypto_crc_oxor_reg (@ 0x00000110).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000110) CRC initial value register                         */

  struct {
    __IOM uint32_t crc_oxor   : 32;           /*!< [31..0] if output xor mode                                        */
  } b;                                        /*!< bit fields for crypto_crc_oxor_reg */
} crypto_crc_oxor_reg_t, *pcrypto_crc_oxor_reg_t;

/**
 * \brief Union type to access crypto_crc_data_reg (@ 0x00000114).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000114) CRC data register                                  */

  struct {
    __IOM uint32_t crc_data   : 32;           /*!< [31..0] din - Command mode : DMA==1'b0 SDBP - Source Data Buffer
                                               *   Pointer : DMA==1'b1
                                               */
  } b;                                        /*!< bit fields for crypto_crc_data_reg */
} crypto_crc_data_reg_t, *pcrypto_crc_data_reg_t;

/**
 * \brief Union type to access crypto_crc_stat_reg (@ 0x00000118).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000118) CRC status register                                */

  struct {
    __IM  uint32_t busy       : 1;            /*!< [0..0] still busy in previous execution                           */
    __IM  uint32_t crc_ok     : 1;            /*!< [1..1] finish execution                                           */
    __IOM uint32_t crc_intr_mask : 1;         /*!< [2..2] interrupt mask                                             */
    __OM  uint32_t crc_little_endian : 1;     /*!< [3..3] little endian                                              */
  } b;                                        /*!< bit fields for crypto_crc_stat_reg */
} crypto_crc_stat_reg_t, *pcrypto_crc_stat_reg_t;

/**
 * \brief Union type to access crypto_crc_result_reg (@ 0x0000011C).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000011C) CRC result register                                */

  struct {
    __IM  uint32_t crc_out    : 32;           /*!< [31..0] output result                                             */
  } b;                                        /*!< bit fields for crypto_crc_result_reg */
} crypto_crc_result_reg_t, *pcrypto_crc_result_reg_t;

/**
 * \brief Union type to access crypto_crc_count_reg (@ 0x00000120).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000120) CRC count register                                 */

  struct {
    __IM  uint32_t crc_cnt    : 16;           /*!< [15..0] count number in bytes                                     */
  } b;                                        /*!< bit fields for crypto_crc_count_reg */
} crypto_crc_count_reg_t, *pcrypto_crc_count_reg_t;

/**
  \brief Union type to access crypto_crc_dma_rx_dbg_reg (@ 0x00000124).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000124) CRC DMA rx dbg register                            */
  
  struct {
    __IM  uint32_t crc_dma_rx_dbg : 32;       /*!< [31..0] CRC DMA rx debug values                                   */
  } b;                                        /*!< bit fields for crypto_crc_dma_rx_dbg_reg */
} crypto_crc_dma_rx_dbg_reg_t, *pcrypto_crc_dma_rx_dbg_reg_t;

/**
  \brief Union type to access crypto_axi_cache_reg (@ 0x00000200).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000200) AXI cache register                                 */
  
  struct {
    __IOM uint32_t awcache : 4;               /*!< [3..0] AXI write cache setting                                    */
    __IM  uint32_t         : 4;               /*!< [7..4] reserved                                                   */
    __IOM uint32_t arcache : 4;               /*!< [11..8] AXI read cache setting                                    */
    __IM  uint32_t         : 19;              /*!< [30..12] reserved                                                 */
    __IOM uint32_t awcache_last_tran_en : 1;  /*!< [31..31] Use for the last write transition                        */
  } b;                                        /*!< bit fields for crypto_axi_cache_reg */
} crypto_axi_cache_reg_t, *pcrypto_axi_cache_reg_t;

/**
  \brief Union type to access crypto_axi_resp_reg (@ 0x00000204).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000204) AXI response register                              */
  
  struct {
    __IM  uint32_t bresp : 2;                 /*!< [1..0] AXI write response                                         */
    __IM  uint32_t rresp : 2;                 /*!< [3..2] AXI read response                                          */
    __IM  uint32_t       : 28;                /*!< [31..4] reserved                                                  */
  } b;                                        /*!< bit fields for crypto_axi_resp_reg */
} crypto_axi_resp_reg_t, *pcrypto_axi_resp_reg_t;

/**
  \brief Union type to access crypto_4k_boundary_reg (@ 0x00000208).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000208) 4K boundary register                               */
  
  struct {
    __IOM uint32_t cfg_4k_boundary_en : 1;    /*!< [0..0] 4KB Address Boundary Limitation for Bursts                 
                                               * 0 : disable        1 : enable
                                               */
    __IM  uint32_t       : 31;                /*!< [31..1] reserved                                                  */
  } b;                                        /*!< bit fields for crypto_4k_boundary_reg */
} crypto_4k_boundary_reg_t, *pcrypto_4k_boundary_reg_t;

/**
 * \brief Union type to access crypto_key_lock_reg (@ 0x000004F0).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000004F0) secure key lock register                           */

  struct {
    __IOM uint32_t key0_lock  : 1;            /*!< [0..0] Control the write lock of key0                 
                                               * 1: key write is locked      0: key write is unlocked
                                               */
    __IOM uint32_t key1_lock  : 1;            /*!< [1..1] Control the write lock of key1                 
                                               * 1: key write is locked      0: key write is unlocked
                                               */
    __IOM uint32_t key2_lock  : 1;            /*!< [2..2] Control the write lock of key2                 
                                               * 1: key write is locked      0: key write is unlocked
                                               */
    __IOM uint32_t key3_lock  : 1;            /*!< [3..3] Control the write lock of key3                 
                                               * 1: key write is locked      0: key write is unlocked
                                               */
    __IOM uint32_t key4_lock  : 1;            /*!< [4..4] Control the write lock of key4                 
                                               * 1: key write is locked      0: key write is unlocked
                                               */
    __IOM uint32_t key5_lock  : 1;            /*!< [5..5] Control the write lock of key5                 
                                               * 1: key write is locked      0: key write is unlocked
                                               */
    __IOM uint32_t key6_lock  : 1;            /*!< [6..6] Control the write lock of key6                 
                                               * 1: key write is locked      0: key write is unlocked
                                               */
    __IOM uint32_t key7_lock  : 1;            /*!< [7..7] Control the write lock of key7                 
                                               * 1: key write is locked      0: key write is unlocked
                                               */
    __IOM uint32_t key8_lock  : 1;            /*!< [8..8] Control the write lock of key8                 
                                               * 1: key write is locked      0: key write is unlocked
                                               */
    __IOM uint32_t key9_lock  : 1;            /*!< [9..9] Control the write lock of key9                 
                                               * 1: key write is locked      0: key write is unlocked
                                               */
    __IOM uint32_t key10_lock : 1;            /*!< [10..10] Control the write lock of key10                 
                                               * 1: key write is locked      0: key write is unlocked
                                               */
    __IOM uint32_t key11_lock : 1;            /*!< [11..11] Control the write lock of key11                 
                                               * 1: key write is locked      0: key write is unlocked
                                               */
    __IM  uint32_t            : 18;           /*!< [29..12] reserved                                                */
    __IOM uint32_t sk_wb_byte_swap : 1;       /*!< [30..30] Byte swap for secure key write back                 
                                               * 1: enable      0: disable
                                               */
    __IOM uint32_t sk_wb_word_swap : 1;       /*!< [31..31] Word swap for secure key write back                 
                                               * 1: enable      0: disable
                                               */
  } b;                                        /*!< bit fields for crypto_dstdesc_status_reg */
} crypto_key_lock_reg_t, *pcrypto_key_lock_reg_t;

/**
  \brief Union type to access crypto_otp_key_swap_reg (@ 0x000004FC).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000004FC) secure otp key swap register                     */
  
  struct {
    __IOM uint32_t otp_byte_swap : 1;         /*!< [0..0] Byte swap for OTP key                  
                                               * 1: enable      0: disable
                                               */
    __IOM uint32_t otp_word_swap : 1;         /*!< [1..1] Word swap for OTP key                 
                                               * 1: enable      0: disable
                                               */
  } b;                                        /*!< bit fields for crypto_sk0_7_reg */
} crypto_otp_key_swap_reg_t, *pcrypto_otp_key_swap_reg_t;

#if 0
/**
  \brief Union type to access crypto_sk0_7_reg (@ 0x00000500).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000500) secure key storage SK0[255:224] register           */
  
  struct {
    __IM  uint32_t sk0_7 : 32;                /*!< [31..0] SK0[255:224]                                              */
  } b;                                        /*!< bit fields for crypto_sk0_7_reg */
} crypto_sk0_7_reg_t, *pcrypto_sk0_7_reg_t;

/**
  \brief Union type to access crypto_sk0_6_reg (@ 0x00000504).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000504) secure key storage SK0[223:192] register           */
  
  struct {
    __IM  uint32_t sk0_6 : 32;                /*!< [31..0] SK0[223:192]                                              */
  } b;                                        /*!< bit fields for crypto_sk0_6_reg */
} crypto_sk0_6_reg_t, *pcrypto_sk0_6_reg_t;

/**
  \brief Union type to access crypto_sk0_5_reg (@ 0x00000508).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000508) secure key storage SK0[191:160] register           */
  
  struct {
    __IM  uint32_t sk0_5 : 32;                /*!< [31..0] SK0[191:160]                                              */
  } b;                                        /*!< bit fields for crypto_sk0_5_reg */
} crypto_sk0_5_reg_t, *pcrypto_sk0_5_reg_t;

/**
  \brief Union type to access crypto_sk0_4_reg (@ 0x0000050C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000050C) secure key storage SK0[159:128] register           */
  
  struct {
    __IM  uint32_t sk0_4 : 32;                /*!< [31..0] SK0[159:128]                                              */
  } b;                                        /*!< bit fields for crypto_sk0_4_reg */
} crypto_sk0_4_reg_t, *pcrypto_sk0_4_reg_t;

/**
  \brief Union type to access crypto_sk0_3_reg (@ 0x00000510).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000510) secure key storage SK0[127:96] register            */
  
  struct {
    __IM  uint32_t sk0_3 : 32;                /*!< [31..0] SK0[127:96]                                               */
  } b;                                        /*!< bit fields for crypto_sk0_3_reg */
} crypto_sk0_3_reg_t, *pcrypto_sk0_3_reg_t;

/**
  \brief Union type to access crypto_sk0_2_reg (@ 0x00000514).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000514) secure key storage SK0[95:64] register             */
  
  struct {
    __IM  uint32_t sk0_2 : 32;                /*!< [31..0] SK0[95:64]                                                */
  } b;                                        /*!< bit fields for crypto_sk0_2_reg */
} crypto_sk0_2_reg_t, *pcrypto_sk0_2_reg_t;

/**
  \brief Union type to access crypto_sk0_1_reg (@ 0x00000518).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000518) secure key storage SK0[63:32] register             */
  
  struct {
    __IM  uint32_t sk0_1 : 32;                /*!< [31..0] SK0[63:32]                                                */
  } b;                                        /*!< bit fields for crypto_sk0_1_reg */
} crypto_sk0_1_reg_t, *pcrypto_sk0_1_reg_t;

/**
  \brief Union type to access crypto_sk0_0_reg (@ 0x0000051C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000051C) secure key storage SK0[31:0] register              */
  
  struct {
    __IM  uint32_t sk0_0: 32;                 /*!< [31..0] SK0[31:0]                                                 */
  } b;                                        /*!< bit fields for crypto_sk0_0_reg */
} crypto_sk0_0_reg_t, *pcrypto_sk0_0_reg_t;

/**
  \brief Union type to access crypto_sk1_7_reg (@ 0x00000520).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000520) secure key storage SK1[255:224] register           */
  
  struct {
    __IM  uint32_t sk1_7 : 32;                /*!< [31..0] SK1[255:224]                                              */
  } b;                                        /*!< bit fields for crypto_sk1_7_reg */
} crypto_sk1_7_reg_t, *pcrypto_sk1_7_reg_t;

/**
  \brief Union type to access crypto_sk1_6_reg (@ 0x00000524).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000524) secure key storage SK1[223:192] register           */
  
  struct {
    __IM  uint32_t sk1_6 : 32;                /*!< [31..0] SK1[223:192]                                              */
  } b;                                        /*!< bit fields for crypto_sk1_6_reg */
} crypto_sk1_6_reg_t, *pcrypto_sk1_6_reg_t;

/**
  \brief Union type to access crypto_sk1_5_reg (@ 0x00000528).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000528) secure key storage SK1[191:160] register           */
  
  struct {
    __IM  uint32_t sk1_5 : 32;                /*!< [31..0] SK1[191:160]                                              */
  } b;                                        /*!< bit fields for crypto_sk1_5_reg */
} crypto_sk1_5_reg_t, *pcrypto_sk1_5_reg_t;

/**
  \brief Union type to access crypto_sk1_4_reg (@ 0x0000052C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000052C) secure key storage SK1[159:128] register           */
  
  struct {
    __IM  uint32_t sk1_4 : 32;                /*!< [31..0] SK1[159:128]                                              */
  } b;                                        /*!< bit fields for crypto_sk1_4_reg */
} crypto_sk1_4_reg_t, *pcrypto_sk1_4_reg_t;

/**
  \brief Union type to access crypto_sk1_3_reg (@ 0x00000530).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000530) secure key storage SK1[127:96] register            */
  
  struct {
    __IM  uint32_t sk1_3 : 32;                /*!< [31..0] SK1[127:96]                                               */
  } b;                                        /*!< bit fields for crypto_sk1_3_reg */
} crypto_sk1_3_reg_t, *pcrypto_sk1_3_reg_t;

/**
  \brief Union type to access crypto_sk1_2_reg (@ 0x00000534).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000534) secure key storage SK1[95:64] register             */
  
  struct {
    __IM  uint32_t sk1_2 : 32;                /*!< [31..0] SK1[95:64]                                                */
  } b;                                        /*!< bit fields for crypto_sk1_2_reg */
} crypto_sk1_2_reg_t, *pcrypto_sk1_2_reg_t;

/**
  \brief Union type to access crypto_sk1_1_reg (@ 0x00000538).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000538) secure key storage SK1[63:32] register             */
  
  struct {
    __IM  uint32_t sk1_1 : 32;                /*!< [31..0] SK1[63:32]                                                */
  } b;                                        /*!< bit fields for crypto_sk1_1_reg */
} crypto_sk1_1_reg_t, *pcrypto_sk1_1_reg_t;

/**
  \brief Union type to access crypto_sk1_0_reg (@ 0x0000053C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000053C) secure key storage SK1[31:0] register              */
  
  struct {
    __IM  uint32_t sk1_0: 32;                 /*!< [31..0] SK1[31:0]                                                 */
  } b;                                        /*!< bit fields for crypto_sk1_0_reg */
} crypto_sk1_0_reg_t, *pcrypto_sk1_0_reg_t;

/**
  \brief Union type to access crypto_sk2_7_reg (@ 0x00000540).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000540) secure key storage SK2[255:224] register           */
  
  struct {
    __IM  uint32_t sk2_7 : 32;                /*!< [31..0] SK2[255:224]                                              */
  } b;                                        /*!< bit fields for crypto_sk2_7_reg */
} crypto_sk2_7_reg_t, *pcrypto_sk2_7_reg_t;

/**
  \brief Union type to access crypto_sk2_6_reg (@ 0x00000544).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000544) secure key storage SK2[223:192] register           */
  
  struct {
    __IM  uint32_t sk2_6 : 32;                /*!< [31..0] SK2[223:192]                                              */
  } b;                                        /*!< bit fields for crypto_sk2_6_reg */
} crypto_sk2_6_reg_t, *pcrypto_sk2_6_reg_t;

/**
  \brief Union type to access crypto_sk2_5_reg (@ 0x00000548).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000548) secure key storage SK2[191:160] register           */
  
  struct {
    __IM  uint32_t sk2_5 : 32;                /*!< [31..0] SK2[191:160]                                              */
  } b;                                        /*!< bit fields for crypto_sk2_5_reg */
} crypto_sk2_5_reg_t, *pcrypto_sk2_5_reg_t;

/**
  \brief Union type to access crypto_sk2_4_reg (@ 0x0000054C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000054C) secure key storage SK2[159:128] register           */
  
  struct {
    __IM  uint32_t sk2_4 : 32;                /*!< [31..0] SK2[159:128]                                              */
  } b;                                        /*!< bit fields for crypto_sk2_4_reg */
} crypto_sk2_4_reg_t, *pcrypto_sk2_4_reg_t;

/**
  \brief Union type to access crypto_sk2_3_reg (@ 0x00000550).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000550) secure key storage SK2[127:96] register            */
  
  struct {
    __IM  uint32_t sk2_3 : 32;                /*!< [31..0] SK2[127:96]                                               */
  } b;                                        /*!< bit fields for crypto_sk2_3_reg */
} crypto_sk2_3_reg_t, *pcrypto_sk2_3_reg_t;

/**
  \brief Union type to access crypto_sk2_2_reg (@ 0x00000554).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000554) secure key storage SK2[95:64] register             */
  
  struct {
    __IM  uint32_t sk2_2 : 32;                /*!< [31..0] SK2[95:64]                                                */
  } b;                                        /*!< bit fields for crypto_sk2_2_reg */
} crypto_sk2_2_reg_t, *pcrypto_sk2_2_reg_t;

/**
  \brief Union type to access crypto_sk2_1_reg (@ 0x00000558).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000558) secure key storage SK2[63:32] register             */
  
  struct {
    __IM  uint32_t sk2_1 : 32;                /*!< [31..0] SK2[63:32]                                                */
  } b;                                        /*!< bit fields for crypto_sk2_1_reg */
} crypto_sk2_1_reg_t, *pcrypto_sk2_1_reg_t;

/**
  \brief Union type to access crypto_sk2_0_reg (@ 0x0000055C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000055C) secure key storage SK2[31:0] register              */
  
  struct {
    __IM  uint32_t sk2_0: 32;                 /*!< [31..0] SK2[31:0]                                                 */
  } b;                                        /*!< bit fields for crypto_sk2_0_reg */
} crypto_sk2_0_reg_t, *pcrypto_sk2_0_reg_t;

/**
  \brief Union type to access crypto_sk3_7_reg (@ 0x00000560).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000560) secure key storage SK3[255:224] register           */
  
  struct {
    __IM  uint32_t sk3_7 : 32;                /*!< [31..0] SK3[255:224]                                              */
  } b;                                        /*!< bit fields for crypto_sk3_7_reg */
} crypto_sk3_7_reg_t, *pcrypto_sk3_7_reg_t;

/**
  \brief Union type to access crypto_sk3_6_reg (@ 0x00000564).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000564) secure key storage SK3[223:192] register           */
  
  struct {
    __IM  uint32_t sk3_6 : 32;                /*!< [31..0] SK3[223:192]                                              */
  } b;                                        /*!< bit fields for crypto_sk3_6_reg */
} crypto_sk3_6_reg_t, *pcrypto_sk3_6_reg_t;

/**
  \brief Union type to access crypto_sk3_5_reg (@ 0x00000568).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000568) secure key storage SK3[191:160] register           */
  
  struct {
    __IM  uint32_t sk3_5 : 32;                /*!< [31..0] SK3[191:160]                                              */
  } b;                                        /*!< bit fields for crypto_sk3_5_reg */
} crypto_sk3_5_reg_t, *pcrypto_sk3_5_reg_t;

/**
  \brief Union type to access crypto_sk3_4_reg (@ 0x0000056C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000056C) secure key storage SK3[159:128] register           */
  
  struct {
    __IM  uint32_t sk3_4 : 32;                /*!< [31..0] SK3[159:128]                                              */
  } b;                                        /*!< bit fields for crypto_sk3_4_reg */
} crypto_sk3_4_reg_t, *pcrypto_sk3_4_reg_t;

/**
  \brief Union type to access crypto_sk3_3_reg (@ 0x00000570).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000570) secure key storage SK3[127:96] register            */
  
  struct {
    __IM  uint32_t sk3_3 : 32;                /*!< [31..0] SK3[127:96]                                               */
  } b;                                        /*!< bit fields for crypto_sk3_3_reg */
} crypto_sk3_3_reg_t, *pcrypto_sk3_3_reg_t;

/**
  \brief Union type to access crypto_sk3_2_reg (@ 0x00000574).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000574) secure key storage SK3[95:64] register             */
  
  struct {
    __IM  uint32_t sk3_2 : 32;                /*!< [31..0] SK3[95:64]                                                */
  } b;                                        /*!< bit fields for crypto_sk3_2_reg */
} crypto_sk3_2_reg_t, *pcrypto_sk3_2_reg_t;

/**
  \brief Union type to access crypto_sk3_1_reg (@ 0x00000578).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000578) secure key storage SK3[63:32] register             */
  
  struct {
    __IM  uint32_t sk3_1 : 32;                /*!< [31..0] SK3[63:32]                                                */
  } b;                                        /*!< bit fields for crypto_sk3_1_reg */
} crypto_sk3_1_reg_t, *pcrypto_sk3_1_reg_t;

/**
  \brief Union type to access crypto_sk3_0_reg (@ 0x0000057C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000057C) secure key storage SK3[31:0] register              */
  
  struct {
    __IM  uint32_t sk3_0: 32;                 /*!< [31..0] SK3[31:0]                                                 */
  } b;                                        /*!< bit fields for crypto_sk3_0_reg */
} crypto_sk3_0_reg_t, *pcrypto_sk3_0_reg_t;

/**
  \brief Union type to access crypto_sk4_7_reg (@ 0x00000580).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000580) secure key storage SK4[255:224] register           */
  
  struct {
    __IM  uint32_t sk4_7 : 32;                /*!< [31..0] SK4[255:224]                                              */
  } b;                                        /*!< bit fields for crypto_sk4_7_reg */
} crypto_sk4_7_reg_t, *pcrypto_sk4_7_reg_t;

/**
  \brief Union type to access crypto_sk4_6_reg (@ 0x00000584).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000584) secure key storage SK4[223:192] register           */
  
  struct {
    __IM  uint32_t sk4_6 : 32;                /*!< [31..0] SK4[223:192]                                              */
  } b;                                        /*!< bit fields for crypto_sk4_6_reg */
} crypto_sk4_6_reg_t, *pcrypto_sk4_6_reg_t;

/**
  \brief Union type to access crypto_sk4_5_reg (@ 0x00000588).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000588) secure key storage SK4[191:160] register           */
  
  struct {
    __IM  uint32_t sk4_5 : 32;                /*!< [31..0] SK4[191:160]                                              */
  } b;                                        /*!< bit fields for crypto_sk4_5_reg */
} crypto_sk4_5_reg_t, *pcrypto_sk4_5_reg_t;

/**
  \brief Union type to access crypto_sk4_4_reg (@ 0x0000058C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000058C) secure key storage SK4[159:128] register           */
  
  struct {
    __IM  uint32_t sk4_4 : 32;                /*!< [31..0] SK4[159:128]                                              */
  } b;                                        /*!< bit fields for crypto_sk4_4_reg */
} crypto_sk4_4_reg_t, *pcrypto_sk4_4_reg_t;

/**
  \brief Union type to access crypto_sk4_3_reg (@ 0x00000590).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000590) secure key storage SK4[127:96] register            */
  
  struct {
    __IM  uint32_t sk4_3 : 32;                /*!< [31..0] SK4[127:96]                                               */
  } b;                                        /*!< bit fields for crypto_sk4_3_reg */
} crypto_sk4_3_reg_t, *pcrypto_sk4_3_reg_t;

/**
  \brief Union type to access crypto_sk4_2_reg (@ 0x00000594).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000594) secure key storage SK4[95:64] register             */
  
  struct {
    __IM  uint32_t sk4_2 : 32;                /*!< [31..0] SK4[95:64]                                                */
  } b;                                        /*!< bit fields for crypto_sk4_2_reg */
} crypto_sk4_2_reg_t, *pcrypto_sk4_2_reg_t;

/**
  \brief Union type to access crypto_sk4_1_reg (@ 0x00000598).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000598) secure key storage SK4[63:32] register             */
  
  struct {
    __IM  uint32_t sk4_1 : 32;                /*!< [31..0] SK4[63:32]                                                */
  } b;                                        /*!< bit fields for crypto_sk4_1_reg */
} crypto_sk4_1_reg_t, *pcrypto_sk4_1_reg_t;

/**
  \brief Union type to access crypto_sk4_0_reg (@ 0x0000059C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000059C) secure key storage SK4[31:0] register              */
  
  struct {
    __IM  uint32_t sk4_0: 32;                 /*!< [31..0] SK4[31:0]                                                 */
  } b;                                        /*!< bit fields for crypto_sk4_0_reg */
} crypto_sk4_0_reg_t, *pcrypto_sk4_0_reg_t;

/**
  \brief Union type to access crypto_sk5_7_reg (@ 0x000005A0).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005A0) secure key storage SK5[255:224] register           */
  
  struct {
    __IM  uint32_t sk5_7 : 32;                /*!< [31..0] SK5[255:224]                                              */
  } b;                                        /*!< bit fields for crypto_sk5_7_reg */
} crypto_sk5_7_reg_t, *pcrypto_sk5_7_reg_t;

/**
  \brief Union type to access crypto_sk5_6_reg (@ 0x000005A4).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005A4) secure key storage SK5[223:192] register           */
  
  struct {
    __IM  uint32_t sk5_6 : 32;                /*!< [31..0] SK5[223:192]                                              */
  } b;                                        /*!< bit fields for crypto_sk5_6_reg */
} crypto_sk5_6_reg_t, *pcrypto_sk5_6_reg_t;

/**
  \brief Union type to access crypto_sk5_5_reg (@ 0x000005A8).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005A8) secure key storage SK5[191:160] register           */
  
  struct {
    __IM  uint32_t sk5_5 : 32;                /*!< [31..0] SK5[191:160]                                              */
  } b;                                        /*!< bit fields for crypto_sk5_5_reg */
} crypto_sk5_5_reg_t, *pcrypto_sk5_5_reg_t;

/**
  \brief Union type to access crypto_sk5_4_reg (@ 0x000005AC).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005AC) secure key storage SK5[159:128] register           */
  
  struct {
    __IM  uint32_t sk5_4 : 32;                /*!< [31..0] SK5[159:128]                                              */
  } b;                                        /*!< bit fields for crypto_sk5_4_reg */
} crypto_sk5_4_reg_t, *pcrypto_sk5_4_reg_t;

/**
  \brief Union type to access crypto_sk5_3_reg (@ 0x000005B0).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005B0) secure key storage SK5[127:96] register            */
  
  struct {
    __IM  uint32_t sk5_3 : 32;                /*!< [31..0] SK5[127:96]                                               */
  } b;                                        /*!< bit fields for crypto_sk5_3_reg */
} crypto_sk5_3_reg_t, *pcrypto_sk5_3_reg_t;

/**
  \brief Union type to access crypto_sk5_2_reg (@ 0x000005B4).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005B4) secure key storage SK5[95:64] register             */
  
  struct {
    __IM  uint32_t sk5_2 : 32;                /*!< [31..0] SK5[95:64]                                                */
  } b;                                        /*!< bit fields for crypto_sk5_2_reg */
} crypto_sk5_2_reg_t, *pcrypto_sk5_2_reg_t;

/**
  \brief Union type to access crypto_sk5_1_reg (@ 0x000005B8).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005B8) secure key storage SK5[63:32] register             */
  
  struct {
    __IM  uint32_t sk5_1 : 32;                /*!< [31..0] SK5[63:32]                                                */
  } b;                                        /*!< bit fields for crypto_sk5_1_reg */
} crypto_sk5_1_reg_t, *pcrypto_sk5_1_reg_t;

/**
  \brief Union type to access crypto_sk5_0_reg (@ 0x000005BC).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005BC) secure key storage SK5[31:0] register              */
  
  struct {
    __IM  uint32_t sk5_0: 32;                 /*!< [31..0] SK5[31:0]                                                 */
  } b;                                        /*!< bit fields for crypto_sk5_0_reg */
} crypto_sk5_0_reg_t, *pcrypto_sk5_0_reg_t;

/**
  \brief Union type to access crypto_sk6_7_reg (@ 0x000005C0).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005C0) secure key storage SK6[255:224] register           */
  
  struct {
    __IM  uint32_t sk6_7 : 32;                /*!< [31..0] SK6[255:224]                                              */
  } b;                                        /*!< bit fields for crypto_sk6_7_reg */
} crypto_sk6_7_reg_t, *pcrypto_sk6_7_reg_t;

/**
  \brief Union type to access crypto_sk6_6_reg (@ 0x000005C4).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005C4) secure key storage SK6[223:192] register           */
  
  struct {
    __IM  uint32_t sk6_6 : 32;                /*!< [31..0] SK6[223:192]                                              */
  } b;                                        /*!< bit fields for crypto_sk6_6_reg */
} crypto_sk6_6_reg_t, *pcrypto_sk6_6_reg_t;

/**
  \brief Union type to access crypto_sk6_5_reg (@ 0x000005C8).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005C8) secure key storage SK6[191:160] register           */
  
  struct {
    __IM  uint32_t sk6_5 : 32;                /*!< [31..0] SK6[191:160]                                              */
  } b;                                        /*!< bit fields for crypto_sk6_5_reg */
} crypto_sk6_5_reg_t, *pcrypto_sk6_5_reg_t;

/**
  \brief Union type to access crypto_sk6_4_reg (@ 0x000005CC).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005CC) secure key storage SK6[159:128] register           */
  
  struct {
    __IM  uint32_t sk6_4 : 32;                /*!< [31..0] SK6[159:128]                                              */
  } b;                                        /*!< bit fields for crypto_sk6_4_reg */
} crypto_sk6_4_reg_t, *pcrypto_sk6_4_reg_t;

/**
  \brief Union type to access crypto_sk6_3_reg (@ 0x000005D0).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005D0) secure key storage SK6[127:96] register            */
  
  struct {
    __IM  uint32_t sk6_3 : 32;                /*!< [31..0] SK6[127:96]                                               */
  } b;                                        /*!< bit fields for crypto_sk6_3_reg */
} crypto_sk6_3_reg_t, *pcrypto_sk6_3_reg_t;

/**
  \brief Union type to access crypto_sk6_2_reg (@ 0x000005D4).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005D4) secure key storage SK6[95:64] register             */
  
  struct {
    __IM  uint32_t sk6_2 : 32;                /*!< [31..0] SK6[95:64]                                                */
  } b;                                        /*!< bit fields for crypto_sk6_2_reg */
} crypto_sk6_2_reg_t, *pcrypto_sk6_2_reg_t;

/**
  \brief Union type to access crypto_sk6_1_reg (@ 0x000005D8).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005D8) secure key storage SK6[63:32] register             */
  
  struct {
    __IM  uint32_t sk6_1 : 32;                /*!< [31..0] SK6[63:32]                                                */
  } b;                                        /*!< bit fields for crypto_sk6_1_reg */
} crypto_sk6_1_reg_t, *pcrypto_sk6_1_reg_t;

/**
  \brief Union type to access crypto_sk6_0_reg (@ 0x000005DC).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005DC) secure key storage SK6[31:0] register              */
  
  struct {
    __IM  uint32_t sk6_0: 32;                 /*!< [31..0] SK6[31:0]                                                 */
  } b;                                        /*!< bit fields for crypto_sk6_0_reg */
} crypto_sk6_0_reg_t, *pcrypto_sk6_0_reg_t;

/**
  \brief Union type to access crypto_sk7_7_reg (@ 0x000005E0).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005E0) secure key storage SK7[255:224] register           */
  
  struct {
    __IM  uint32_t sk7_7 : 32;                /*!< [31..0] SK7[255:224]                                              */
  } b;                                        /*!< bit fields for crypto_sk7_7_reg */
} crypto_sk7_7_reg_t, *pcrypto_sk7_7_reg_t;

/**
  \brief Union type to access crypto_sk7_6_reg (@ 0x000005E4).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005E4) secure key storage SK7[223:192] register           */
  
  struct {
    __IM  uint32_t sk7_6 : 32;                /*!< [31..0] SK7[223:192]                                              */
  } b;                                        /*!< bit fields for crypto_sk7_6_reg */
} crypto_sk7_6_reg_t, *pcrypto_sk7_6_reg_t;

/**
  \brief Union type to access crypto_sk7_5_reg (@ 0x000005E8).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005E8) secure key storage SK7[191:160] register           */
  
  struct {
    __IM  uint32_t sk7_5 : 32;                /*!< [31..0] SK7[191:160]                                              */
  } b;                                        /*!< bit fields for crypto_sk7_5_reg */
} crypto_sk7_5_reg_t, *pcrypto_sk7_5_reg_t;

/**
  \brief Union type to access crypto_sk7_4_reg (@ 0x000005EC).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005EC) secure key storage SK7[159:128] register           */
  
  struct {
    __IM  uint32_t sk7_4 : 32;                /*!< [31..0] SK7[159:128]                                              */
  } b;                                        /*!< bit fields for crypto_sk7_4_reg */
} crypto_sk7_4_reg_t, *pcrypto_sk7_4_reg_t;

/**
  \brief Union type to access crypto_sk7_3_reg (@ 0x000005F0).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005F0) secure key storage SK7[127:96] register            */
  
  struct {
    __IM  uint32_t sk7_3 : 32;                /*!< [31..0] SK7[127:96]                                               */
  } b;                                        /*!< bit fields for crypto_sk7_3_reg */
} crypto_sk7_3_reg_t, *pcrypto_sk7_3_reg_t;

/**
  \brief Union type to access crypto_sk7_2_reg (@ 0x000005F4).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005F4) secure key storage SK7[95:64] register             */
  
  struct {
    __IM  uint32_t sk7_2 : 32;                /*!< [31..0] SK7[95:64]                                                */
  } b;                                        /*!< bit fields for crypto_sk7_2_reg */
} crypto_sk7_2_reg_t, *pcrypto_sk7_2_reg_t;

/**
  \brief Union type to access crypto_sk7_1_reg (@ 0x000005F8).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005F8) secure key storage SK7[63:32] register             */
  
  struct {
    __IM  uint32_t sk7_1 : 32;                /*!< [31..0] SK7[63:32]                                                */
  } b;                                        /*!< bit fields for crypto_sk7_1_reg */
} crypto_sk7_1_reg_t, *pcrypto_sk7_1_reg_t;

/**
  \brief Union type to access crypto_sk7_0_reg (@ 0x000005FC).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000005FC) secure key storage SK7[31:0] register              */
  
  struct {
    __IM  uint32_t sk7_0: 32;                 /*!< [31..0] SK7[31:0]                                                 */
  } b;                                        /*!< bit fields for crypto_sk7_0_reg */
} crypto_sk7_0_reg_t, *pcrypto_sk7_0_reg_t;

/**
  \brief Union type to access crypto_sk8_7_reg (@ 0x00000600).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000600) secure key storage SK8[255:224] register           */
  
  struct {
    __IM  uint32_t sk8_7 : 32;                /*!< [31..0] SK8[255:224]                                              */
  } b;                                        /*!< bit fields for crypto_sk8_7_reg */
} crypto_sk8_7_reg_t, *pcrypto_sk8_7_reg_t;

/**
  \brief Union type to access crypto_sk8_6_reg (@ 0x00000604).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000604) secure key storage SK8[223:192] register           */
  
  struct {
    __IM  uint32_t sk8_6 : 32;                /*!< [31..0] SK8[223:192]                                              */
  } b;                                        /*!< bit fields for crypto_sk8_6_reg */
} crypto_sk8_6_reg_t, *pcrypto_sk8_6_reg_t;

/**
  \brief Union type to access crypto_sk8_5_reg (@ 0x00000608).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000608) secure key storage SK8[191:160] register           */
  
  struct {
    __IM  uint32_t sk8_5 : 32;                /*!< [31..0] SK8[191:160]                                              */
  } b;                                        /*!< bit fields for crypto_sk8_5_reg */
} crypto_sk8_5_reg_t, *pcrypto_sk8_5_reg_t;

/**
  \brief Union type to access crypto_sk8_4_reg (@ 0x0000060C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000060C) secure key storage SK8[159:128] register           */
  
  struct {
    __IM  uint32_t sk8_4 : 32;                /*!< [31..0] SK8[159:128]                                              */
  } b;                                        /*!< bit fields for crypto_sk8_4_reg */
} crypto_sk8_4_reg_t, *pcrypto_sk8_4_reg_t;

/**
  \brief Union type to access crypto_sk8_3_reg (@ 0x00000610).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000610) secure key storage SK8[127:96] register            */
  
  struct {
    __IM  uint32_t sk8_3 : 32;                /*!< [31..0] SK8[127:96]                                               */
  } b;                                        /*!< bit fields for crypto_sk8_3_reg */
} crypto_sk8_3_reg_t, *pcrypto_sk8_3_reg_t;

/**
  \brief Union type to access crypto_sk8_2_reg (@ 0x00000614).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000614) secure key storage SK8[95:64] register             */
  
  struct {
    __IM  uint32_t sk8_2 : 32;                /*!< [31..0] SK8[95:64]                                                */
  } b;                                        /*!< bit fields for crypto_sk8_2_reg */
} crypto_sk8_2_reg_t, *pcrypto_sk8_2_reg_t;

/**
  \brief Union type to access crypto_sk8_1_reg (@ 0x00000618).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000618) secure key storage SK8[63:32] register             */
  
  struct {
    __IM  uint32_t sk8_1 : 32;                /*!< [31..0] SK8[63:32]                                                */
  } b;                                        /*!< bit fields for crypto_sk8_1_reg */
} crypto_sk8_1_reg_t, *pcrypto_sk8_1_reg_t;

/**
  \brief Union type to access crypto_sk8_0_reg (@ 0x0000061C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000061C) secure key storage SK8[31:0] register              */
  
  struct {
    __IM  uint32_t sk8_0: 32;                 /*!< [31..0] SK8[31:0]                                                 */
  } b;                                        /*!< bit fields for crypto_sk8_0_reg */
} crypto_sk8_0_reg_t, *pcrypto_sk8_0_reg_t;

/**
  \brief Union type to access crypto_sk9_7_reg (@ 0x00000620).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000620) secure key storage SK9[255:224] register           */
  
  struct {
    __IM  uint32_t sk9_7 : 32;                /*!< [31..0] SK9[255:224]                                              */
  } b;                                        /*!< bit fields for crypto_sk9_7_reg */
} crypto_sk9_7_reg_t, *pcrypto_sk9_7_reg_t;

/**
  \brief Union type to access crypto_sk9_6_reg (@ 0x00000624).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000624) secure key storage SK9[223:192] register           */
  
  struct {
    __IM  uint32_t sk9_6 : 32;                /*!< [31..0] SK9[223:192]                                              */
  } b;                                        /*!< bit fields for crypto_sk9_6_reg */
} crypto_sk9_6_reg_t, *pcrypto_sk9_6_reg_t;

/**
  \brief Union type to access crypto_sk9_5_reg (@ 0x00000628).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000628) secure key storage SK9[191:160] register           */
  
  struct {
    __IM  uint32_t sk9_5 : 32;                /*!< [31..0] SK9[191:160]                                              */
  } b;                                        /*!< bit fields for crypto_sk9_5_reg */
} crypto_sk9_5_reg_t, *pcrypto_sk9_5_reg_t;

/**
  \brief Union type to access crypto_sk9_4_reg (@ 0x0000062C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000062C) secure key storage SK9[159:128] register           */
  
  struct {
    __IM  uint32_t sk9_4 : 32;                /*!< [31..0] SK9[159:128]                                              */
  } b;                                        /*!< bit fields for crypto_sk9_4_reg */
} crypto_sk9_4_reg_t, *pcrypto_sk9_4_reg_t;

/**
  \brief Union type to access crypto_sk9_3_reg (@ 0x00000630).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000630) secure key storage SK9[127:96] register            */
  
  struct {
    __IM  uint32_t sk9_3 : 32;                /*!< [31..0] SK9[127:96]                                               */
  } b;                                        /*!< bit fields for crypto_sk9_3_reg */
} crypto_sk9_3_reg_t, *pcrypto_sk9_3_reg_t;

/**
  \brief Union type to access crypto_sk9_2_reg (@ 0x00000634).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000634) secure key storage SK9[95:64] register             */
  
  struct {
    __IM  uint32_t sk9_2 : 32;                /*!< [31..0] SK9[95:64]                                                */
  } b;                                        /*!< bit fields for crypto_sk9_2_reg */
} crypto_sk9_2_reg_t, *pcrypto_sk9_2_reg_t;

/**
  \brief Union type to access crypto_sk9_1_reg (@ 0x00000638).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000638) secure key storage SK9[63:32] register             */
  
  struct {
    __IM  uint32_t sk9_1 : 32;                /*!< [31..0] SK9[63:32]                                                */
  } b;                                        /*!< bit fields for crypto_sk9_1_reg */
} crypto_sk9_1_reg_t, *pcrypto_sk9_1_reg_t;

/**
  \brief Union type to access crypto_sk9_0_reg (@ 0x0000063C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000063C) secure key storage SK9[31:0] register              */
  
  struct {
    __IM  uint32_t sk9_0: 32;                 /*!< [31..0] SK9[31:0]                                                 */
  } b;                                        /*!< bit fields for crypto_sk9_0_reg */
} crypto_sk9_0_reg_t, *pcrypto_sk9_0_reg_t;

/**
  \brief Union type to access crypto_sk10_7_reg (@ 0x00000640).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000640) secure key storage SK10[255:224] register          */
  
  struct {
    __IM  uint32_t sk10_7 : 32;               /*!< [31..0] SK10[255:224]                                            */
  } b;                                        /*!< bit fields for crypto_sk10_7_reg */
} crypto_sk10_7_reg_t, *pcrypto_sk10_7_reg_t;

/**
  \brief Union type to access crypto_sk10_6_reg (@ 0x00000644).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000644) secure key storage SK10[223:192] register          */
  
  struct {
    __IM  uint32_t sk10_6 : 32;               /*!< [31..0] SK10[223:192]                                            */
  } b;                                        /*!< bit fields for crypto_sk10_6_reg */
} crypto_sk10_6_reg_t, *pcrypto_sk10_6_reg_t;

/**
  \brief Union type to access crypto_sk10_5_reg (@ 0x00000648).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000648) secure key storage SK10[191:160] register          */
  
  struct {
    __IM  uint32_t sk10_5 : 32;               /*!< [31..0] SK10[191:160]                                            */
  } b;                                        /*!< bit fields for crypto_sk10_5_reg */
} crypto_sk10_5_reg_t, *pcrypto_sk10_5_reg_t;

/**
  \brief Union type to access crypto_sk10_4_reg (@ 0x0000064C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000064C) secure key storage SK10[159:128] register          */
  
  struct {
    __IM  uint32_t sk10_4 : 32;               /*!< [31..0] SK10[159:128]                                            */
  } b;                                        /*!< bit fields for crypto_sk10_4_reg */
} crypto_sk10_4_reg_t, *pcrypto_sk10_4_reg_t;

/**
  \brief Union type to access crypto_sk10_3_reg (@ 0x00000650).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000650) secure key storage SK10[127:96] register           */
  
  struct {
    __IM  uint32_t sk10_3 : 32;               /*!< [31..0] SK10[127:96]                                             */
  } b;                                        /*!< bit fields for crypto_sk10_3_reg */
} crypto_sk10_3_reg_t, *pcrypto_sk10_3_reg_t;

/**
  \brief Union type to access crypto_sk10_2_reg (@ 0x00000654).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000654) secure key storage SK10[95:64] register            */
  
  struct {
    __IM  uint32_t sk10_2 : 32;               /*!< [31..0] SK10[95:64]                                              */
  } b;                                        /*!< bit fields for crypto_sk10_2_reg */
} crypto_sk10_2_reg_t, *pcrypto_sk10_2_reg_t;

/**
  \brief Union type to access crypto_sk10_1_reg (@ 0x00000658).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000658) secure key storage SK10[63:32] register            */
  
  struct {
    __IM  uint32_t sk10_1 : 32;               /*!< [31..0] SK10[63:32]                                              */
  } b;                                        /*!< bit fields for crypto_sk10_1_reg */
} crypto_sk10_1_reg_t, *pcrypto_sk10_1_reg_t;

/**
  \brief Union type to access crypto_sk10_0_reg (@ 0x0000065C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000065C) secure key storage SK10[31:0] register             */
  
  struct {
    __IM  uint32_t sk10_0: 32;                /*!< [31..0] SK10[31:0]                                               */
  } b;                                        /*!< bit fields for crypto_sk10_0_reg */
} crypto_sk10_0_reg_t, *pcrypto_sk10_0_reg_t;

/**
  \brief Union type to access crypto_sk11_7_reg (@ 0x00000660).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000660) secure key storage SK11[255:224] register          */
  
  struct {
    __IM  uint32_t sk11_7 : 32;               /*!< [31..0] SK11[255:224]                                            */
  } b;                                        /*!< bit fields for crypto_sk11_7_reg */
} crypto_sk11_7_reg_t, *pcrypto_sk11_7_reg_t;

/**
  \brief Union type to access crypto_sk11_6_reg (@ 0x00000664).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000664) secure key storage SK11[223:192] register          */
  
  struct {
    __IM  uint32_t sk11_6 : 32;               /*!< [31..0] SK11[223:192]                                            */
  } b;                                        /*!< bit fields for crypto_sk11_6_reg */
} crypto_sk11_6_reg_t, *pcrypto_sk11_6_reg_t;

/**
  \brief Union type to access crypto_sk11_5_reg (@ 0x00000668).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000668) secure key storage SK11[191:160] register          */
  
  struct {
    __IM  uint32_t sk11_5 : 32;               /*!< [31..0] SK11[191:160]                                            */
  } b;                                        /*!< bit fields for crypto_sk11_5_reg */
} crypto_sk11_5_reg_t, *pcrypto_sk11_5_reg_t;

/**
  \brief Union type to access crypto_sk11_4_reg (@ 0x0000066C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000066C) secure key storage SK11[159:128] register          */
  
  struct {
    __IM  uint32_t sk11_4 : 32;               /*!< [31..0] SK11[159:128]                                            */
  } b;                                        /*!< bit fields for crypto_sk11_4_reg */
} crypto_sk11_4_reg_t, *pcrypto_sk11_4_reg_t;

/**
  \brief Union type to access crypto_sk11_3_reg (@ 0x00000670).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000670) secure key storage SK11[127:96] register           */
  
  struct {
    __IM  uint32_t sk11_3 : 32;               /*!< [31..0] SK11[127:96]                                             */
  } b;                                        /*!< bit fields for crypto_sk11_3_reg */
} crypto_sk11_3_reg_t, *pcrypto_sk11_3_reg_t;

/**
  \brief Union type to access crypto_sk11_2_reg (@ 0x00000674).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000674) secure key storage SK11[95:64] register            */
  
  struct {
    __IM  uint32_t sk11_2 : 32;               /*!< [31..0] SK11[95:64]                                              */
  } b;                                        /*!< bit fields for crypto_sk11_2_reg */
} crypto_sk11_2_reg_t, *pcrypto_sk11_2_reg_t;

/**
  \brief Union type to access crypto_sk11_1_reg (@ 0x00000678).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00000678) secure key storage SK11[63:32] register            */
  
  struct {
    __IM  uint32_t sk11_1 : 32;               /*!< [31..0] SK11[63:32]                                              */
  } b;                                        /*!< bit fields for crypto_sk11_1_reg */
} crypto_sk11_1_reg_t, *pcrypto_sk11_1_reg_t;

/**
  \brief Union type to access crypto_sk11_0_reg (@ 0x0000067C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000067C) secure key storage SK11[31:0] register             */
  
  struct {
    __IM  uint32_t sk11_0: 32;                /*!< [31..0] SK11[31:0]                                               */
  } b;                                        /*!< bit fields for crypto_sk11_0_reg */
} crypto_sk11_0_reg_t, *pcrypto_sk11_0_reg_t;
#endif

/**
 * \brief Union type to access crypto_dstdesc_status_reg (@ 0x00001000).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001000) destination descriptor status                      */

  struct {
    __IM  uint32_t fifo_empty_cnt : 16;       /*!< [15..0] Destination Descriptor FIFO empty counter                 */
    __IM  uint32_t            : 8;            /*!< [23..16] reserved                                                 */
    __IOM uint32_t dst_fail   : 1;            /*!< [24..24] Destination Descriptor fail interrupt                    */
    __IM  uint32_t dst_fail_status : 2;       /*!< [26..25] Destination Descriptor fail status                       */
    __IM  uint32_t            : 4;            /*!< [30..27] reserved                                                 */
    __IOM uint32_t dst_rst    : 1;            /*!< [31..31] Destination Descriptor reset                             */
  } b;                                        /*!< bit fields for crypto_dstdesc_status_reg */
} crypto_dstdesc_status_reg_t, *pcrypto_dstdesc_status_reg_t;

/**
 * \brief Union type to access crypto_ddfw_reg (@ 0x00001004).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001004) destination descriptor first word                  */

  struct {
    __IOM uint32_t ddfw       : 32;           /*!< [31..0] IPSec Destination Descriptor first word                   */
  } b;                                        /*!< bit fields for crypto_ddfw_reg */
} crypto_ddfw_reg_t, *pcrypto_ddfw_reg_t;

/**
 * \brief Union type to access crypto_ddsw_reg (@ 0x00001008).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001008) destination descriptor second word                 */

  struct {
    __IOM uint32_t ddsw       : 32;           /*!< [31..0] IPSec Destination Descriptor second word                  */
  } b;                                        /*!< bit fields for crypto_ddsw_reg */
} crypto_ddsw_reg_t, *pcrypto_ddsw_reg_t;

/**
 * \brief Union type to access crypto_desc_pkt_conf_reg (@ 0x0000100C).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000100C) descriptor packet setting                          */

  struct {
    __IM  uint32_t            : 16;           /*!< [15..0] reserved                                                  */
    __IM  uint32_t pk_arbiter : 2;            /*!< [17..16] Packet arbiter 2'd0 : bus1, 2'd1 : bus2, 2'd2 : bus3     */
    __IM  uint32_t            : 6;            /*!< [23..18] reserved                                                 */
    __IOM uint32_t bus1_priority_th : 2;      /*!< [25..24] Bus1 priority threshold                                  */
    __IOM uint32_t bus2_priority_th : 2;      /*!< [27..26] Bus2 priority threshold                                  */
    __IOM uint32_t                  : 2;      /*!< [29..28] reserved                                                 */
    __IOM uint32_t pk_arbiter_mode : 2;       /*!< [31..30] Packet arbiter mode                                      */
  } b;                                        /*!< bit fields for crypto_desc_pkt_conf_reg */
} crypto_desc_pkt_conf_reg_t, *pcrypto_desc_pkt_conf_reg_t;

/**
 * \brief Union type to access crypto_dbgsdr_reg (@ 0x00001010).
 */
typedef union {
  __IM uint32_t w;                            /*!< (@ 0x00001010) Debug Source Descriptor Data                       */

  struct {
    __IM uint32_t dbg_sd     : 32;            /*!< [31..0] This register is used with DBG_SPTR.                      */
  } b;                                        /*!< bit fields for crypto_dbgsdr_reg */
} crypto_dbgsdr_reg_t, *pcrypto_dbgsdr_reg_t;

/**
 * \brief Union type to access crypto_dbgddr_reg (@ 0x00001014).
 */
typedef union {
  __IM uint32_t w;                            /*!< (@ 0x00001014) Debug Destination Descriptor Data                  */

  struct {
    __IM uint32_t dbg_dd     : 32;            /*!< [31..0] This register is used with DBG_DPTR.                      */
  } b;                                        /*!< bit fields for crypto_dbgddr_reg */
} crypto_dbgddr_reg_t, *pcrypto_dbgddr_reg_t;

/**
 * \brief Union type to access crypto_dbgptr_reg (@ 0x00001018).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001018) Debug Destination Descriptor Data                  */

  struct {
    __IOM uint32_t dbg_sptr  : 16;            /*!< [15..0] Source Descriptor FIFO empty counter                      */
    __IOM uint32_t dbg_dptr  : 16;            /*!< [31..15] Destination Descriptor FIFO write pointer                */
  } b;                                        /*!< bit fields for crypto_dbgptr_reg */
} crypto_dbgptr_reg_t, *pcrypto_dbgptr_reg_t;

/**
 * \brief Union type to access crypto_srcdesc_ddptr_reg (@ 0x0000101C).
 */
typedef union {
  __IM uint32_t w;                            /*!< (@ 0x0000101C) Destination Descriptor FIFO status Register        */

  struct {
    __IM  uint32_t dwptr     : 16;            /*!< [15..0] Destination Descriptor FIFO write pointer                 */
    __IM  uint32_t drptr     : 16;            /*!< [31..16] Destination Descriptor FIFO read pointer                 */
  } b;
} crypto_srcdesc_ddptr_reg_t, *pcrypto_srcdesc_ddptr_reg_t;

/**
 * \brief Union type to access crypto_keypad_locklen0_reg (@ 0x000010C0).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000010C0) secure keypad lock & len 0 register                */

  struct {
    __IOM uint32_t keypad_len    : 5;         /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit                  */
    __IM  uint32_t               : 26;        /*!< [30..5] reserved                                                  */
    __IOM uint32_t keypad_lock   : 1;         /*!< [31..31] Control the write lock of keypad
                                               * 1: keypad write is locked      0: keypad write is unlocked
                                               */
  } b;
} crypto_keypad_locklen0_reg_t, *pcrypto_keypad_locklen0_reg_t;

/**
 * \brief Union type to access crypto_keypad_locklen1_reg (@ 0x000010C4).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000010C4) secure keypad lock & len 1 register                */

  struct {
    __IOM uint32_t keypad_len    : 5;         /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit                  */
    __IM  uint32_t               : 26;        /*!< [30..5] reserved                                                  */
    __IOM uint32_t keypad_lock   : 1;         /*!< [31..31] Control the write lock of keypad
                                               * 1: keypad write is locked      0: keypad write is unlocked
                                               */
  } b;
} crypto_keypad_locklen1_reg_t, *pcrypto_keypad_locklen1_reg_t;

/**
 * \brief Union type to access crypto_keypad_locklen2_reg (@ 0x000010C8).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000010C8) secure keypad lock & len 2 register                */

  struct {
    __IOM uint32_t keypad_len    : 5;         /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit                  */
    __IM  uint32_t               : 26;        /*!< [30..5] reserved                                                  */
    __IOM uint32_t keypad_lock   : 1;         /*!< [31..31] Control the write lock of keypad
                                               * 1: keypad write is locked      0: keypad write is unlocked
                                               */
  } b;
} crypto_keypad_locklen2_reg_t, *pcrypto_keypad_locklen2_reg_t;

/**
 * \brief Union type to access crypto_keypad_locklen3_reg (@ 0x000010CC).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000010CC) secure keypad lock & len 3 register                */

  struct {
    __IOM uint32_t keypad_len    : 5;         /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit                  */
    __IM  uint32_t               : 26;        /*!< [30..5] reserved                                                  */
    __IOM uint32_t keypad_lock   : 1;         /*!< [31..31] Control the write lock of keypad
                                               * 1: keypad write is locked      0: keypad write is unlocked
                                               */
  } b;
} crypto_keypad_locklen3_reg_t, *pcrypto_keypad_locklen3_reg_t;

/**
 * \brief Union type to access crypto_keypad_locklen4_reg (@ 0x000010D0).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000010D0) secure keypad lock & len 4 register                */

  struct {
    __IOM uint32_t keypad_len    : 5;         /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit                  */
    __IM  uint32_t               : 26;        /*!< [30..5] reserved                                                  */
    __IOM uint32_t keypad_lock   : 1;         /*!< [31..31] Control the write lock of keypad
                                               * 1: keypad write is locked      0: keypad write is unlocked
                                               */
  } b;
} crypto_keypad_locklen4_reg_t, *pcrypto_keypad_locklen4_reg_t;

/**
 * \brief Union type to access crypto_keypad_locklen5_reg (@ 0x000010D4).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000010D4) secure keypad lock & len 5 register                */

  struct {
    __IOM uint32_t keypad_len    : 5;         /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit                  */
    __IM  uint32_t               : 26;        /*!< [30..5] reserved                                                  */
    __IOM uint32_t keypad_lock   : 1;         /*!< [31..31] Control the write lock of keypad
                                               * 1: keypad write is locked      0: keypad write is unlocked
                                               */
  } b;
} crypto_keypad_locklen5_reg_t, *pcrypto_keypad_locklen5_reg_t;

/**
 * \brief Union type to access crypto_keypad_locklen6_reg (@ 0x000010D8).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000010D8) secure keypad lock & len 6 register                */

  struct {
    __IOM uint32_t keypad_len    : 5;         /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit                  */
    __IM  uint32_t               : 26;        /*!< [30..5] reserved                                                  */
    __IOM uint32_t keypad_lock   : 1;         /*!< [31..31] Control the write lock of keypad
                                               * 1: keypad write is locked      0: keypad write is unlocked
                                               */
  } b;
} crypto_keypad_locklen6_reg_t, *pcrypto_keypad_locklen6_reg_t;

/**
 * \brief Union type to access crypto_keypad_locklen7_reg (@ 0x000010DC).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000010DC) secure keypad lock & len 7 register                */

  struct {
    __IOM uint32_t keypad_len    : 5;         /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit                  */
    __IM  uint32_t               : 26;        /*!< [30..5] reserved                                                  */
    __IOM uint32_t keypad_lock   : 1;         /*!< [31..31] Control the write lock of keypad
                                               * 1: keypad write is locked      0: keypad write is unlocked
                                               */
  } b;
} crypto_keypad_locklen7_reg_t, *pcrypto_keypad_locklen7_reg_t;

/**
 * \brief Union type to access crypto_keypad_locklen8_reg (@ 0x000010E0).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000010E0) secure keypad lock & len 8 register                */

  struct {
    __IOM uint32_t keypad_len    : 5;         /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit                  */
    __IM  uint32_t               : 26;        /*!< [30..5] reserved                                                  */
    __IOM uint32_t keypad_lock   : 1;         /*!< [31..31] Control the write lock of keypad
                                               * 1: keypad write is locked      0: keypad write is unlocked
                                               */
  } b;
} crypto_keypad_locklen8_reg_t, *pcrypto_keypad_locklen8_reg_t;

/**
 * \brief Union type to access crypto_keypad_locklen9_reg (@ 0x000010E4).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000010E4) secure keypad lock & len 9 register                */

  struct {
    __IOM uint32_t keypad_len    : 5;         /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit                  */
    __IM  uint32_t               : 26;        /*!< [30..5] reserved                                                  */
    __IOM uint32_t keypad_lock   : 1;         /*!< [31..31] Control the write lock of keypad
                                               * 1: keypad write is locked      0: keypad write is unlocked
                                               */
  } b;
} crypto_keypad_locklen9_reg_t, *pcrypto_keypad_locklen9_reg_t;

/**
 * \brief Union type to access crypto_keypad_locklen10_reg (@ 0x000010E8).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000010E8) secure keypad lock & len 10 register               */

  struct {
    __IOM uint32_t keypad_len    : 5;         /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit                  */
    __IM  uint32_t               : 26;        /*!< [30..5] reserved                                                  */
    __IOM uint32_t keypad_lock   : 1;         /*!< [31..31] Control the write lock of keypad
                                               * 1: keypad write is locked      0: keypad write is unlocked
                                               */
  } b;
} crypto_keypad_locklen10_reg_t, *pcrypto_keypad_locklen10_reg_t;

/**
 * \brief Union type to access crypto_keypad_locklen11_reg (@ 0x000010EC).
 */
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x000010EC) secure keypad lock & len 11 register               */

  struct {
    __IOM uint32_t keypad_len    : 5;         /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit                  */
    __IM  uint32_t               : 26;        /*!< [30..5] reserved                                                  */
    __IOM uint32_t keypad_lock   : 1;         /*!< [31..31] Control the write lock of keypad
                                               * 1: keypad write is locked      0: keypad write is unlocked
                                               */
    
  } b;
} crypto_keypad_locklen11_reg_t, *pcrypto_keypad_locklen11_reg_t;

#if 0
/**
  \brief Union type to access crypto_skp0_15_reg (@ 0x00001100).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001100) secure key storage skp0[511:480] register          */
  
  struct {
    __OM  uint32_t skp0_15 : 32;              /*!< [31..0] skp0[511:480]                                             */
  } b;                                        /*!< bit fields for crypto_skp0_15_reg */
} crypto_skp0_15_reg_t, *pcrypto_skp0_15_reg_t;

/**
  \brief Union type to access crypto_skp0_14_reg (@ 0x00001104).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001104) secure key storage skp0[479:448] register          */
  
  struct {
    __OM  uint32_t skp0_14 : 32;              /*!< [31..0] skp0[479:448]                                             */
  } b;                                        /*!< bit fields for crypto_skp0_14_reg */
} crypto_skp0_14_reg_t, *pcrypto_skp0_14_reg_t;

/**
  \brief Union type to access crypto_skp0_13_reg (@ 0x00001108).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001108) secure key storage skp0[447:416] register          */
  
  struct {
    __OM  uint32_t skp0_13 : 32;              /*!< [31..0] skp0[447:416]                                             */
  } b;                                        /*!< bit fields for crypto_skp0_13_reg */
} crypto_skp0_13_reg_t, *pcrypto_skp0_13_reg_t;

/**
  \brief Union type to access crypto_skp0_12_reg (@ 0x0000110C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000110C) secure key storage skp0[415:384] register          */
  
  struct {
    __OM  uint32_t skp0_12 : 32;              /*!< [31..0] skp0[415:384]                                             */
  } b;                                        /*!< bit fields for crypto_skp0_12_reg */
} crypto_skp0_12_reg_t, *pcrypto_skp0_12_reg_t;

/**
  \brief Union type to access crypto_skp0_11_reg (@ 0x00001110).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001110) secure key storage skp0[383:352] register          */
  
  struct {
    __OM  uint32_t skp0_11 : 32;              /*!< [31..0] skp0[383:352]                                             */
  } b;                                        /*!< bit fields for crypto_skp0_11_reg */
} crypto_skp0_11_reg_t, *pcrypto_skp0_11_reg_t;

/**
  \brief Union type to access crypto_skp0_10_reg (@ 0x00001114).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001114) secure key storage skp0[351:320] register          */
  
  struct {
    __OM  uint32_t skp0_10 : 32;              /*!< [31..0] skp0[351:320]                                             */
  } b;                                        /*!< bit fields for crypto_skp0_10_reg */
} crypto_skp0_10_reg_t, *pcrypto_skp0_10_reg_t;

/**
  \brief Union type to access crypto_skp0_9_reg (@ 0x00001118).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001118) secure key storage skp0[319:288] register          */
  
  struct {
    __OM  uint32_t skp0_9 : 32;               /*!< [31..0] skp0[319:288]                                             */
  } b;                                        /*!< bit fields for crypto_skp0_9_reg */
} crypto_skp0_9_reg_t, *pcrypto_skp0_9_reg_t;

/**
  \brief Union type to access crypto_skp0_8_reg (@ 0x0000111C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000111C) secure key storage skp0[287:256] register          */
  
  struct {
    __OM  uint32_t skp0_8 : 32;               /*!< [31..0] skp0[287:256]                                             */
  } b;                                        /*!< bit fields for crypto_skp0_8_reg */
} crypto_skp0_8_reg_t, *pcrypto_skp0_8_reg_t;

/**
  \brief Union type to access crypto_skp0_7_reg (@ 0x00001120).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001120) secure key storage skp0[255:224] register          */
  
  struct {
    __OM  uint32_t skp0_7 : 32;               /*!< [31..0] skp0[255:224]                                             */
  } b;                                        /*!< bit fields for crypto_skp0_7_reg */
} crypto_skp0_7_reg_t, *pcrypto_skp0_7_reg_t;

/**
  \brief Union type to access crypto_skp0_6_reg (@ 0x00001124).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001124) secure key storage skp0[223:192] register          */
  
  struct {
    __OM  uint32_t skp0_6 : 32;               /*!< [31..0] skp0[223:192]                                             */
  } b;                                        /*!< bit fields for crypto_skp0_6_reg */
} crypto_skp0_6_reg_t, *pcrypto_skp0_6_reg_t;

/**
  \brief Union type to access crypto_skp0_5_reg (@ 0x00001128).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001128) secure key storage skp0[191:160] register          */
  
  struct {
    __OM  uint32_t skp0_5 : 32;               /*!< [31..0] skp0[191:160]                                             */
  } b;                                        /*!< bit fields for crypto_skp0_5_reg */
} crypto_skp0_5_reg_t, *pcrypto_skp0_5_reg_t;

/**
  \brief Union type to access crypto_skp0_4_reg (@ 0x0000112C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000112C) secure key storage skp0[159:128] register          */
  
  struct {
    __OM  uint32_t skp0_4 : 32;               /*!< [31..0] skp0[159:128]                                             */
  } b;                                        /*!< bit fields for crypto_skp0_4_reg */
} crypto_skp0_4_reg_t, *pcrypto_skp0_4_reg_t;

/**
  \brief Union type to access crypto_skp0_3_reg (@ 0x00001130).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001130) secure key storage skp0[127:96] register           */
  
  struct {
    __OM  uint32_t skp0_3 : 32;               /*!< [31..0] skp0[127:96]                                              */
  } b;                                        /*!< bit fields for crypto_skp0_3_reg */
} crypto_skp0_3_reg_t, *pcrypto_skp0_3_reg_t;

/**
  \brief Union type to access crypto_skp0_2_reg (@ 0x00001134).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001134) secure key storage skp0[95:64] register            */
  
  struct {
    __OM  uint32_t skp0_2 : 32;               /*!< [31..0] skp0[95:64]                                               */
  } b;                                        /*!< bit fields for crypto_skp0_2_reg */
} crypto_skp0_2_reg_t, *pcrypto_skp0_2_reg_t;

/**
  \brief Union type to access crypto_skp0_1_reg (@ 0x00001138).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001138) secure key storage skp0[63:32] register            */
  
  struct {
    __OM  uint32_t skp0_1 : 32;               /*!< [31..0] skp0[63:32]                                               */
  } b;                                        /*!< bit fields for crypto_skp0_1_reg */
} crypto_skp0_1_reg_t, *pcrypto_skp0_1_reg_t;

/**
  \brief Union type to access crypto_skp0_0_reg (@ 0x0000113C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000113C) secure key storage skp0[31:0] register             */
  
  struct {
    __OM  uint32_t skp0_0: 32;                /*!< [31..0] skp0[31:0]                                                */
  } b;                                        /*!< bit fields for crypto_skp0_0_reg */
} crypto_skp0_0_reg_t, *pcrypto_skp0_0_reg_t;

/**
  \brief Union type to access crypto_skp1_15_reg (@ 0x00001200).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001200) secure key storage skp1[511:480] register          */
  
  struct {
    __OM  uint32_t skp1_15 : 32;              /*!< [31..0] skp1[511:480]                                             */
  } b;                                        /*!< bit fields for crypto_skp1_15_reg */
} crypto_skp1_15_reg_t, *pcrypto_skp1_15_reg_t;

/**
  \brief Union type to access crypto_skp1_14_reg (@ 0x00001204).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001204) secure key storage skp1[479:448] register          */
  
  struct {
    __OM  uint32_t skp1_14 : 32;              /*!< [31..0] skp1[479:448]                                             */
  } b;                                        /*!< bit fields for crypto_skp1_14_reg */
} crypto_skp1_14_reg_t, *pcrypto_skp1_14_reg_t;

/**
  \brief Union type to access crypto_skp1_13_reg (@ 0x00001208).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001208) secure key storage skp1[447:416] register          */
  
  struct {
    __OM  uint32_t skp1_13 : 32;              /*!< [31..0] skp1[447:416]                                             */
  } b;                                        /*!< bit fields for crypto_skp1_13_reg */
} crypto_skp1_13_reg_t, *pcrypto_skp1_13_reg_t;

/**
  \brief Union type to access crypto_skp1_12_reg (@ 0x0000120C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000120C) secure key storage skp1[415:384] register          */
  
  struct {
    __OM  uint32_t skp1_12 : 32;              /*!< [31..0] skp1[415:384]                                             */
  } b;                                        /*!< bit fields for crypto_skp1_12_reg */
} crypto_skp1_12_reg_t, *pcrypto_skp1_12_reg_t;

/**
  \brief Union type to access crypto_skp1_11_reg (@ 0x00001210).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001210) secure key storage skp1[383:352] register          */
  
  struct {
    __OM  uint32_t skp1_11 : 32;              /*!< [31..0] skp1[383:352]                                             */
  } b;                                        /*!< bit fields for crypto_skp1_11_reg */
} crypto_skp1_11_reg_t, *pcrypto_skp1_11_reg_t;

/**
  \brief Union type to access crypto_skp1_10_reg (@ 0x00001214).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001214) secure key storage skp1[351:320] register          */
  
  struct {
    __OM  uint32_t skp1_10 : 32;              /*!< [31..0] skp1[351:320]                                             */
  } b;                                        /*!< bit fields for crypto_skp1_10_reg */
} crypto_skp1_10_reg_t, *pcrypto_skp1_10_reg_t;

/**
  \brief Union type to access crypto_skp1_9_reg (@ 0x00001218).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001218) secure key storage skp1[319:288] register          */
  
  struct {
    __OM  uint32_t skp1_9 : 32;               /*!< [31..0] skp1[319:288]                                             */
  } b;                                        /*!< bit fields for crypto_skp1_9_reg */
} crypto_skp1_9_reg_t, *pcrypto_skp1_9_reg_t;

/**
  \brief Union type to access crypto_skp1_8_reg (@ 0x0000121C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000121C) secure key storage skp1[287:256] register          */
  
  struct {
    __OM  uint32_t skp1_8 : 32;               /*!< [31..0] skp1[287:256]                                             */
  } b;                                        /*!< bit fields for crypto_skp1_8_reg */
} crypto_skp1_8_reg_t, *pcrypto_skp1_8_reg_t;

/**
  \brief Union type to access crypto_skp1_7_reg (@ 0x00001220).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001220) secure key storage skp1[255:224] register          */
  
  struct {
    __OM  uint32_t skp1_7 : 32;               /*!< [31..0] skp1[255:224]                                             */
  } b;                                        /*!< bit fields for crypto_skp1_7_reg */
} crypto_skp1_7_reg_t, *pcrypto_skp1_7_reg_t;

/**
  \brief Union type to access crypto_skp1_6_reg (@ 0x00001224).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001224) secure key storage skp1[223:192] register          */
  
  struct {
    __OM  uint32_t skp1_6 : 32;               /*!< [31..0] skp1[223:192]                                             */
  } b;                                        /*!< bit fields for crypto_skp1_6_reg */
} crypto_skp1_6_reg_t, *pcrypto_skp1_6_reg_t;

/**
  \brief Union type to access crypto_skp1_5_reg (@ 0x00001228).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001228) secure key storage skp1[191:160] register          */
  
  struct {
    __OM  uint32_t skp1_5 : 32;               /*!< [31..0] skp1[191:160]                                             */
  } b;                                        /*!< bit fields for crypto_skp1_5_reg */
} crypto_skp1_5_reg_t, *pcrypto_skp1_5_reg_t;

/**
  \brief Union type to access crypto_skp1_4_reg (@ 0x0000122C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000122C) secure key storage skp1[159:128] register          */
  
  struct {
    __OM  uint32_t skp1_4 : 32;               /*!< [31..0] skp1[159:128]                                             */
  } b;                                        /*!< bit fields for crypto_skp1_4_reg */
} crypto_skp1_4_reg_t, *pcrypto_skp1_4_reg_t;

/**
  \brief Union type to access crypto_skp1_3_reg (@ 0x00001230).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001230) secure key storage skp1[127:96] register           */
  
  struct {
    __OM  uint32_t skp1_3 : 32;               /*!< [31..0] skp1[127:96]                                              */
  } b;                                        /*!< bit fields for crypto_skp1_3_reg */
} crypto_skp1_3_reg_t, *pcrypto_skp1_3_reg_t;

/**
  \brief Union type to access crypto_skp1_2_reg (@ 0x00001234).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001234) secure key storage skp1[95:64] register            */
  
  struct {
    __OM  uint32_t skp1_2 : 32;               /*!< [31..0] skp1[95:64]                                               */
  } b;                                        /*!< bit fields for crypto_skp1_2_reg */
} crypto_skp1_2_reg_t, *pcrypto_skp1_2_reg_t;

/**
  \brief Union type to access crypto_skp1_1_reg (@ 0x00001238).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001238) secure key storage skp1[63:32] register            */
  
  struct {
    __OM  uint32_t skp1_1 : 32;               /*!< [31..0] skp1[63:32]                                               */
  } b;                                        /*!< bit fields for crypto_skp1_1_reg */
} crypto_skp1_1_reg_t, *pcrypto_skp1_1_reg_t;

/**
  \brief Union type to access crypto_skp1_0_reg (@ 0x0000123C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000123C) secure key storage skp1[31:0] register             */
  
  struct {
    __OM  uint32_t skp1_0: 32;                /*!< [31..0] skp1[31:0]                                                */
  } b;                                        /*!< bit fields for crypto_skp1_0_reg */
} crypto_skp1_0_reg_t, *pcrypto_skp1_0_reg_t;

/**
  \brief Union type to access crypto_skp2_15_reg (@ 0x00001300).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001300) secure key storage skp2[511:480] register          */
  
  struct {
    __OM  uint32_t skp2_15 : 32;              /*!< [31..0] skp2[511:480]                                             */
  } b;                                        /*!< bit fields for crypto_skp2_15_reg */
} crypto_skp2_15_reg_t, *pcrypto_skp2_15_reg_t;

/**
  \brief Union type to access crypto_skp2_14_reg (@ 0x00001304).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001304) secure key storage skp2[479:448] register          */
  
  struct {
    __OM  uint32_t skp2_14 : 32;              /*!< [31..0] skp2[479:448]                                             */
  } b;                                        /*!< bit fields for crypto_skp2_14_reg */
} crypto_skp2_14_reg_t, *pcrypto_skp2_14_reg_t;

/**
  \brief Union type to access crypto_skp2_13_reg (@ 0x00001308).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001308) secure key storage skp2[447:416] register          */
  
  struct {
    __OM  uint32_t skp2_13 : 32;              /*!< [31..0] skp2[447:416]                                             */
  } b;                                        /*!< bit fields for crypto_skp2_13_reg */
} crypto_skp2_13_reg_t, *pcrypto_skp2_13_reg_t;

/**
  \brief Union type to access crypto_skp2_12_reg (@ 0x0000130C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000130C) secure key storage skp2[415:384] register          */
  
  struct {
    __OM  uint32_t skp2_12 : 32;              /*!< [31..0] skp2[415:384]                                             */
  } b;                                        /*!< bit fields for crypto_skp2_12_reg */
} crypto_skp2_12_reg_t, *pcrypto_skp2_12_reg_t;

/**
  \brief Union type to access crypto_skp2_11_reg (@ 0x00001310).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001310) secure key storage skp2[383:352] register          */
  
  struct {
    __OM  uint32_t skp2_11 : 32;              /*!< [31..0] skp2[383:352]                                             */
  } b;                                        /*!< bit fields for crypto_skp2_11_reg */
} crypto_skp2_11_reg_t, *pcrypto_skp2_11_reg_t;

/**
  \brief Union type to access crypto_skp2_10_reg (@ 0x00001314).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001314) secure key storage skp2[351:320] register          */
  
  struct {
    __OM  uint32_t skp2_10 : 32;              /*!< [31..0] skp2[351:320]                                             */
  } b;                                        /*!< bit fields for crypto_skp2_10_reg */
} crypto_skp2_10_reg_t, *pcrypto_skp2_10_reg_t;

/**
  \brief Union type to access crypto_skp2_9_reg (@ 0x00001318).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001318) secure key storage skp2[319:288] register          */
  
  struct {
    __OM  uint32_t skp2_9 : 32;               /*!< [31..0] skp2[319:288]                                             */
  } b;                                        /*!< bit fields for crypto_skp2_9_reg */
} crypto_skp2_9_reg_t, *pcrypto_skp2_9_reg_t;

/**
  \brief Union type to access crypto_skp2_8_reg (@ 0x0000131C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000131C) secure key storage skp2[287:256] register          */
  
  struct {
    __OM  uint32_t skp2_8 : 32;               /*!< [31..0] skp2[287:256]                                             */
  } b;                                        /*!< bit fields for crypto_skp2_8_reg */
} crypto_skp2_8_reg_t, *pcrypto_skp2_8_reg_t;

/**
  \brief Union type to access crypto_skp2_7_reg (@ 0x00001320).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001320) secure key storage skp2[255:224] register          */
  
  struct {
    __OM  uint32_t skp2_7 : 32;               /*!< [31..0] skp2[255:224]                                             */
  } b;                                        /*!< bit fields for crypto_skp2_7_reg */
} crypto_skp2_7_reg_t, *pcrypto_skp2_7_reg_t;

/**
  \brief Union type to access crypto_skp2_6_reg (@ 0x00001324).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001324) secure key storage skp2[223:192] register          */
  
  struct {
    __OM  uint32_t skp2_6 : 32;               /*!< [31..0] skp2[223:192]                                             */
  } b;                                        /*!< bit fields for crypto_skp2_6_reg */
} crypto_skp2_6_reg_t, *pcrypto_skp2_6_reg_t;

/**
  \brief Union type to access crypto_skp2_5_reg (@ 0x00001328).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001328) secure key storage skp2[191:160] register          */
  
  struct {
    __OM  uint32_t skp2_5 : 32;               /*!< [31..0] skp2[191:160]                                             */
  } b;                                        /*!< bit fields for crypto_skp2_5_reg */
} crypto_skp2_5_reg_t, *pcrypto_skp2_5_reg_t;

/**
  \brief Union type to access crypto_skp2_4_reg (@ 0x0000132C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000132C) secure key storage skp2[159:128] register          */
  
  struct {
    __OM  uint32_t skp2_4 : 32;               /*!< [31..0] skp2[159:128]                                             */
  } b;                                        /*!< bit fields for crypto_skp2_4_reg */
} crypto_skp2_4_reg_t, *pcrypto_skp2_4_reg_t;

/**
  \brief Union type to access crypto_skp2_3_reg (@ 0x00001330).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001330) secure key storage skp2[127:96] register           */
  
  struct {
    __OM  uint32_t skp2_3 : 32;               /*!< [31..0] skp2[127:96]                                              */
  } b;                                        /*!< bit fields for crypto_skp2_3_reg */
} crypto_skp2_3_reg_t, *pcrypto_skp2_3_reg_t;

/**
  \brief Union type to access crypto_skp2_2_reg (@ 0x00001334).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001334) secure key storage skp2[95:64] register            */
  
  struct {
    __OM  uint32_t skp2_2 : 32;               /*!< [31..0] skp2[95:64]                                               */
  } b;                                        /*!< bit fields for crypto_skp2_2_reg */
} crypto_skp2_2_reg_t, *pcrypto_skp2_2_reg_t;

/**
  \brief Union type to access crypto_skp2_1_reg (@ 0x00001338).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001338) secure key storage skp2[63:32] register            */
  
  struct {
    __OM  uint32_t skp2_1 : 32;               /*!< [31..0] skp2[63:32]                                               */
  } b;                                        /*!< bit fields for crypto_skp2_1_reg */
} crypto_skp2_1_reg_t, *pcrypto_skp2_1_reg_t;

/**
  \brief Union type to access crypto_skp2_0_reg (@ 0x0000133C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000133C) secure key storage skp2[31:0] register             */
  
  struct {
    __OM  uint32_t skp2_0: 32;                /*!< [31..0] skp2[31:0]                                                */
  } b;                                        /*!< bit fields for crypto_skp2_0_reg */
} crypto_skp2_0_reg_t, *pcrypto_skp2_0_reg_t;

/**
  \brief Union type to access crypto_skp3_15_reg (@ 0x00001400).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001400) secure key storage skp3[511:480] register          */
  
  struct {
    __OM  uint32_t skp3_15 : 32;              /*!< [31..0] skp3[511:480]                                             */
  } b;                                        /*!< bit fields for crypto_skp3_15_reg */
} crypto_skp3_15_reg_t, *pcrypto_skp3_15_reg_t;

/**
  \brief Union type to access crypto_skp3_14_reg (@ 0x00001404).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001404) secure key storage skp3[479:448] register          */
  
  struct {
    __OM  uint32_t skp3_14 : 32;              /*!< [31..0] skp3[479:448]                                             */
  } b;                                        /*!< bit fields for crypto_skp3_14_reg */
} crypto_skp3_14_reg_t, *pcrypto_skp3_14_reg_t;

/**
  \brief Union type to access crypto_skp3_13_reg (@ 0x00001408).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001408) secure key storage skp3[447:416] register          */
  
  struct {
    __OM  uint32_t skp3_13 : 32;              /*!< [31..0] skp3[447:416]                                             */
  } b;                                        /*!< bit fields for crypto_skp3_13_reg */
} crypto_skp3_13_reg_t, *pcrypto_skp3_13_reg_t;

/**
  \brief Union type to access crypto_skp3_12_reg (@ 0x0000140C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000140C) secure key storage skp3[415:384] register          */
  
  struct {
    __OM  uint32_t skp3_12 : 32;              /*!< [31..0] skp3[415:384]                                             */
  } b;                                        /*!< bit fields for crypto_skp3_12_reg */
} crypto_skp3_12_reg_t, *pcrypto_skp3_12_reg_t;

/**
  \brief Union type to access crypto_skp3_11_reg (@ 0x00001410).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001410) secure key storage skp3[383:352] register          */
  
  struct {
    __OM  uint32_t skp3_11 : 32;              /*!< [31..0] skp3[383:352]                                             */
  } b;                                        /*!< bit fields for crypto_skp3_11_reg */
} crypto_skp3_11_reg_t, *pcrypto_skp3_11_reg_t;

/**
  \brief Union type to access crypto_skp3_10_reg (@ 0x00001414).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001414) secure key storage skp3[351:320] register          */
  
  struct {
    __OM  uint32_t skp3_10 : 32;              /*!< [31..0] skp3[351:320]                                             */
  } b;                                        /*!< bit fields for crypto_skp3_10_reg */
} crypto_skp3_10_reg_t, *pcrypto_skp3_10_reg_t;

/**
  \brief Union type to access crypto_skp3_9_reg (@ 0x00001418).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001418) secure key storage skp3[319:288] register          */
  
  struct {
    __OM  uint32_t skp3_9 : 32;               /*!< [31..0] skp3[319:288]                                             */
  } b;                                        /*!< bit fields for crypto_skp3_9_reg */
} crypto_skp3_9_reg_t, *pcrypto_skp3_9_reg_t;

/**
  \brief Union type to access crypto_skp3_8_reg (@ 0x0000141C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000141C) secure key storage skp3[287:256] register          */
  
  struct {
    __OM  uint32_t skp3_8 : 32;               /*!< [31..0] skp3[287:256]                                             */
  } b;                                        /*!< bit fields for crypto_skp3_8_reg */
} crypto_skp3_8_reg_t, *pcrypto_skp3_8_reg_t;

/**
  \brief Union type to access crypto_skp3_7_reg (@ 0x00001420).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001420) secure key storage skp3[255:224] register          */
  
  struct {
    __OM  uint32_t skp3_7 : 32;               /*!< [31..0] skp3[255:224]                                             */
  } b;                                        /*!< bit fields for crypto_skp3_7_reg */
} crypto_skp3_7_reg_t, *pcrypto_skp3_7_reg_t;

/**
  \brief Union type to access crypto_skp3_6_reg (@ 0x00001424).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001424) secure key storage skp3[223:192] register          */
  
  struct {
    __OM  uint32_t skp3_6 : 32;               /*!< [31..0] skp3[223:192]                                             */
  } b;                                        /*!< bit fields for crypto_skp3_6_reg */
} crypto_skp3_6_reg_t, *pcrypto_skp3_6_reg_t;

/**
  \brief Union type to access crypto_skp3_5_reg (@ 0x00001428).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001428) secure key storage skp3[191:160] register          */
  
  struct {
    __OM  uint32_t skp3_5 : 32;               /*!< [31..0] skp3[191:160]                                             */
  } b;                                        /*!< bit fields for crypto_skp3_5_reg */
} crypto_skp3_5_reg_t, *pcrypto_skp3_5_reg_t;

/**
  \brief Union type to access crypto_skp3_4_reg (@ 0x0000142C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000142C) secure key storage skp3[159:128] register          */
  
  struct {
    __OM  uint32_t skp3_4 : 32;               /*!< [31..0] skp3[159:128]                                             */
  } b;                                        /*!< bit fields for crypto_skp3_4_reg */
} crypto_skp3_4_reg_t, *pcrypto_skp3_4_reg_t;

/**
  \brief Union type to access crypto_skp3_3_reg (@ 0x00001430).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001430) secure key storage skp3[127:96] register           */
  
  struct {
    __OM  uint32_t skp3_3 : 32;               /*!< [31..0] skp3[127:96]                                              */
  } b;                                        /*!< bit fields for crypto_skp3_3_reg */
} crypto_skp3_3_reg_t, *pcrypto_skp3_3_reg_t;

/**
  \brief Union type to access crypto_skp3_2_reg (@ 0x00001434).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001434) secure key storage skp3[95:64] register            */
  
  struct {
    __OM  uint32_t skp3_2 : 32;               /*!< [31..0] skp3[95:64]                                               */
  } b;                                        /*!< bit fields for crypto_skp3_2_reg */
} crypto_skp3_2_reg_t, *pcrypto_skp3_2_reg_t;

/**
  \brief Union type to access crypto_skp3_1_reg (@ 0x00001438).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001438) secure key storage skp3[63:32] register            */
  
  struct {
    __OM  uint32_t skp3_1 : 32;               /*!< [31..0] skp3[63:32]                                               */
  } b;                                        /*!< bit fields for crypto_skp3_1_reg */
} crypto_skp3_1_reg_t, *pcrypto_skp3_1_reg_t;

/**
  \brief Union type to access crypto_skp3_0_reg (@ 0x0000143C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000143C) secure key storage skp3[31:0] register             */
  
  struct {
    __OM  uint32_t skp3_0: 32;                /*!< [31..0] skp3[31:0]                                                */
  } b;                                        /*!< bit fields for crypto_skp3_0_reg */
} crypto_skp3_0_reg_t, *pcrypto_skp3_0_reg_t;

/**
  \brief Union type to access crypto_skp4_15_reg (@ 0x00001500).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001500) secure key storage skp4[511:480] register          */
  
  struct {
    __OM  uint32_t skp4_15 : 32;              /*!< [31..0] skp4[511:480]                                             */
  } b;                                        /*!< bit fields for crypto_skp4_15_reg */
} crypto_skp4_15_reg_t, *pcrypto_skp4_15_reg_t;

/**
  \brief Union type to access crypto_skp4_14_reg (@ 0x00001504).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001504) secure key storage skp4[479:448] register          */
  
  struct {
    __OM  uint32_t skp4_14 : 32;              /*!< [31..0] skp4[479:448]                                             */
  } b;                                        /*!< bit fields for crypto_skp4_14_reg */
} crypto_skp4_14_reg_t, *pcrypto_skp4_14_reg_t;

/**
  \brief Union type to access crypto_skp4_13_reg (@ 0x00001508).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001508) secure key storage skp4[447:416] register          */
  
  struct {
    __OM  uint32_t skp4_13 : 32;              /*!< [31..0] skp4[447:416]                                             */
  } b;                                        /*!< bit fields for crypto_skp4_13_reg */
} crypto_skp4_13_reg_t, *pcrypto_skp4_13_reg_t;

/**
  \brief Union type to access crypto_skp4_12_reg (@ 0x0000150C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000150C) secure key storage skp4[415:384] register          */
  
  struct {
    __OM  uint32_t skp4_12 : 32;              /*!< [31..0] skp4[415:384]                                             */
  } b;                                        /*!< bit fields for crypto_skp4_12_reg */
} crypto_skp4_12_reg_t, *pcrypto_skp4_12_reg_t;

/**
  \brief Union type to access crypto_skp4_11_reg (@ 0x00001510).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001510) secure key storage skp4[383:352] register          */
  
  struct {
    __OM  uint32_t skp4_11 : 32;              /*!< [31..0] skp4[383:352]                                             */
  } b;                                        /*!< bit fields for crypto_skp4_11_reg */
} crypto_skp4_11_reg_t, *pcrypto_skp4_11_reg_t;

/**
  \brief Union type to access crypto_skp4_10_reg (@ 0x00001514).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001514) secure key storage skp4[351:320] register          */
  
  struct {
    __OM  uint32_t skp4_10 : 32;              /*!< [31..0] skp4[351:320]                                             */
  } b;                                        /*!< bit fields for crypto_skp4_10_reg */
} crypto_skp4_10_reg_t, *pcrypto_skp4_10_reg_t;

/**
  \brief Union type to access crypto_skp4_9_reg (@ 0x00001518).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001518) secure key storage skp4[319:288] register          */
  
  struct {
    __OM  uint32_t skp4_9 : 32;               /*!< [31..0] skp4[319:288]                                             */
  } b;                                        /*!< bit fields for crypto_skp4_9_reg */
} crypto_skp4_9_reg_t, *pcrypto_skp4_9_reg_t;

/**
  \brief Union type to access crypto_skp4_8_reg (@ 0x0000151C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000151C) secure key storage skp4[287:256] register          */
  
  struct {
    __OM  uint32_t skp4_8 : 32;               /*!< [31..0] skp4[287:256]                                             */
  } b;                                        /*!< bit fields for crypto_skp4_8_reg */
} crypto_skp4_8_reg_t, *pcrypto_skp4_8_reg_t;

/**
  \brief Union type to access crypto_skp4_7_reg (@ 0x00001520).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001520) secure key storage skp4[255:224] register          */
  
  struct {
    __OM  uint32_t skp4_7 : 32;               /*!< [31..0] skp4[255:224]                                             */
  } b;                                        /*!< bit fields for crypto_skp4_7_reg */
} crypto_skp4_7_reg_t, *pcrypto_skp4_7_reg_t;

/**
  \brief Union type to access crypto_skp4_6_reg (@ 0x00001524).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001524) secure key storage skp4[223:192] register          */
  
  struct {
    __OM  uint32_t skp4_6 : 32;               /*!< [31..0] skp4[223:192]                                             */
  } b;                                        /*!< bit fields for crypto_skp4_6_reg */
} crypto_skp4_6_reg_t, *pcrypto_skp4_6_reg_t;

/**
  \brief Union type to access crypto_skp4_5_reg (@ 0x00001528).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001528) secure key storage skp4[191:160] register          */
  
  struct {
    __OM  uint32_t skp4_5 : 32;               /*!< [31..0] skp4[191:160]                                             */
  } b;                                        /*!< bit fields for crypto_skp4_5_reg */
} crypto_skp4_5_reg_t, *pcrypto_skp4_5_reg_t;

/**
  \brief Union type to access crypto_skp4_4_reg (@ 0x0000152C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000152C) secure key storage skp4[159:128] register          */
  
  struct {
    __OM  uint32_t skp4_4 : 32;               /*!< [31..0] skp4[159:128]                                             */
  } b;                                        /*!< bit fields for crypto_skp4_4_reg */
} crypto_skp4_4_reg_t, *pcrypto_skp4_4_reg_t;

/**
  \brief Union type to access crypto_skp4_3_reg (@ 0x00001530).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001530) secure key storage skp4[127:96] register           */
  
  struct {
    __OM  uint32_t skp4_3 : 32;               /*!< [31..0] skp4[127:96]                                              */
  } b;                                        /*!< bit fields for crypto_skp4_3_reg */
} crypto_skp4_3_reg_t, *pcrypto_skp4_3_reg_t;

/**
  \brief Union type to access crypto_skp4_2_reg (@ 0x00001534).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001534) secure key storage skp4[95:64] register            */
  
  struct {
    __OM  uint32_t skp4_2 : 32;               /*!< [31..0] skp4[95:64]                                               */
  } b;                                        /*!< bit fields for crypto_skp4_2_reg */
} crypto_skp4_2_reg_t, *pcrypto_skp4_2_reg_t;

/**
  \brief Union type to access crypto_skp4_1_reg (@ 0x00001538).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001538) secure key storage skp4[63:32] register            */
  
  struct {
    __OM  uint32_t skp4_1 : 32;               /*!< [31..0] skp4[63:32]                                               */
  } b;                                        /*!< bit fields for crypto_skp4_1_reg */
} crypto_skp4_1_reg_t, *pcrypto_skp4_1_reg_t;

/**
  \brief Union type to access crypto_skp4_0_reg (@ 0x0000153C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000153C) secure key storage skp4[31:0] register             */
  
  struct {
    __OM  uint32_t skp4_0: 32;                /*!< [31..0] skp4[31:0]                                                */
  } b;                                        /*!< bit fields for crypto_skp4_0_reg */
} crypto_skp4_0_reg_t, *pcrypto_skp4_0_reg_t;

/**
  \brief Union type to access crypto_skp5_15_reg (@ 0x00001600).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001600) secure key storage skp5[511:480] register          */
  
  struct {
    __OM  uint32_t skp5_15 : 32;              /*!< [31..0] skp5[511:480]                                             */
  } b;                                        /*!< bit fields for crypto_skp5_15_reg */
} crypto_skp5_15_reg_t, *pcrypto_skp5_15_reg_t;

/**
  \brief Union type to access crypto_skp5_14_reg (@ 0x00001604).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001604) secure key storage skp5[479:448] register          */
  
  struct {
    __OM  uint32_t skp5_14 : 32;              /*!< [31..0] skp5[479:448]                                             */
  } b;                                        /*!< bit fields for crypto_skp5_14_reg */
} crypto_skp5_14_reg_t, *pcrypto_skp5_14_reg_t;

/**
  \brief Union type to access crypto_skp5_13_reg (@ 0x00001608).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001608) secure key storage skp5[447:416] register          */
  
  struct {
    __OM  uint32_t skp5_13 : 32;              /*!< [31..0] skp5[447:416]                                             */
  } b;                                        /*!< bit fields for crypto_skp5_13_reg */
} crypto_skp5_13_reg_t, *pcrypto_skp5_13_reg_t;

/**
  \brief Union type to access crypto_skp5_12_reg (@ 0x0000160C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000160C) secure key storage skp5[415:384] register          */
  
  struct {
    __OM  uint32_t skp5_12 : 32;              /*!< [31..0] skp5[415:384]                                             */
  } b;                                        /*!< bit fields for crypto_skp5_12_reg */
} crypto_skp5_12_reg_t, *pcrypto_skp5_12_reg_t;

/**
  \brief Union type to access crypto_skp5_11_reg (@ 0x00001610).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001610) secure key storage skp5[383:352] register          */
  
  struct {
    __OM  uint32_t skp5_11 : 32;              /*!< [31..0] skp5[383:352]                                             */
  } b;                                        /*!< bit fields for crypto_skp5_11_reg */
} crypto_skp5_11_reg_t, *pcrypto_skp5_11_reg_t;

/**
  \brief Union type to access crypto_skp5_10_reg (@ 0x00001614).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001614) secure key storage skp5[351:320] register          */
  
  struct {
    __OM  uint32_t skp5_10 : 32;              /*!< [31..0] skp5[351:320]                                             */
  } b;                                        /*!< bit fields for crypto_skp5_10_reg */
} crypto_skp5_10_reg_t, *pcrypto_skp5_10_reg_t;

/**
  \brief Union type to access crypto_skp5_9_reg (@ 0x00001618).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001618) secure key storage skp5[319:288] register          */
  
  struct {
    __OM  uint32_t skp5_9 : 32;               /*!< [31..0] skp5[319:288]                                             */
  } b;                                        /*!< bit fields for crypto_skp5_9_reg */
} crypto_skp5_9_reg_t, *pcrypto_skp5_9_reg_t;

/**
  \brief Union type to access crypto_skp5_8_reg (@ 0x0000161C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000161C) secure key storage skp5[287:256] register          */
  
  struct {
    __OM  uint32_t skp5_8 : 32;               /*!< [31..0] skp5[287:256]                                             */
  } b;                                        /*!< bit fields for crypto_skp5_8_reg */
} crypto_skp5_8_reg_t, *pcrypto_skp5_8_reg_t;

/**
  \brief Union type to access crypto_skp5_7_reg (@ 0x00001620).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001620) secure key storage skp5[255:224] register          */
  
  struct {
    __OM  uint32_t skp5_7 : 32;               /*!< [31..0] skp5[255:224]                                             */
  } b;                                        /*!< bit fields for crypto_skp5_7_reg */
} crypto_skp5_7_reg_t, *pcrypto_skp5_7_reg_t;

/**
  \brief Union type to access crypto_skp5_6_reg (@ 0x00001624).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001624) secure key storage skp5[223:192] register          */
  
  struct {
    __OM  uint32_t skp5_6 : 32;               /*!< [31..0] skp5[223:192]                                             */
  } b;                                        /*!< bit fields for crypto_skp5_6_reg */
} crypto_skp5_6_reg_t, *pcrypto_skp5_6_reg_t;

/**
  \brief Union type to access crypto_skp5_5_reg (@ 0x00001628).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001628) secure key storage skp5[191:160] register          */
  
  struct {
    __OM  uint32_t skp5_5 : 32;               /*!< [31..0] skp5[191:160]                                             */
  } b;                                        /*!< bit fields for crypto_skp5_5_reg */
} crypto_skp5_5_reg_t, *pcrypto_skp5_5_reg_t;

/**
  \brief Union type to access crypto_skp5_4_reg (@ 0x0000162C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000162C) secure key storage skp5[159:128] register          */
  
  struct {
    __OM  uint32_t skp5_4 : 32;               /*!< [31..0] skp5[159:128]                                             */
  } b;                                        /*!< bit fields for crypto_skp5_4_reg */
} crypto_skp5_4_reg_t, *pcrypto_skp5_4_reg_t;

/**
  \brief Union type to access crypto_skp5_3_reg (@ 0x00001630).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001630) secure key storage skp5[127:96] register           */
  
  struct {
    __OM  uint32_t skp5_3 : 32;               /*!< [31..0] skp5[127:96]                                              */
  } b;                                        /*!< bit fields for crypto_skp5_3_reg */
} crypto_skp5_3_reg_t, *pcrypto_skp5_3_reg_t;

/**
  \brief Union type to access crypto_skp5_2_reg (@ 0x00001634).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001634) secure key storage skp5[95:64] register            */
  
  struct {
    __OM  uint32_t skp5_2 : 32;               /*!< [31..0] skp5[95:64]                                               */
  } b;                                        /*!< bit fields for crypto_skp5_2_reg */
} crypto_skp5_2_reg_t, *pcrypto_skp5_2_reg_t;

/**
  \brief Union type to access crypto_skp5_1_reg (@ 0x00001638).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001638) secure key storage skp5[63:32] register            */
  
  struct {
    __OM  uint32_t skp5_1 : 32;               /*!< [31..0] skp5[63:32]                                               */
  } b;                                        /*!< bit fields for crypto_skp5_1_reg */
} crypto_skp5_1_reg_t, *pcrypto_skp5_1_reg_t;

/**
  \brief Union type to access crypto_skp5_0_reg (@ 0x0000163C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000163C) secure key storage skp5[31:0] register             */
  
  struct {
    __OM  uint32_t skp5_0: 32;                /*!< [31..0] skp5[31:0]                                                */
  } b;                                        /*!< bit fields for crypto_skp5_0_reg */
} crypto_skp5_0_reg_t, *pcrypto_skp5_0_reg_t;

/**
  \brief Union type to access crypto_skp6_15_reg (@ 0x00001700).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001700) secure key storage skp6[511:480] register          */
  
  struct {
    __OM  uint32_t skp6_15 : 32;              /*!< [31..0] skp6[511:480]                                             */
  } b;                                        /*!< bit fields for crypto_skp6_15_reg */
} crypto_skp6_15_reg_t, *pcrypto_skp6_15_reg_t;

/**
  \brief Union type to access crypto_skp6_14_reg (@ 0x00001704).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001704) secure key storage skp6[479:448] register          */
  
  struct {
    __OM  uint32_t skp6_14 : 32;              /*!< [31..0] skp6[479:448]                                             */
  } b;                                        /*!< bit fields for crypto_skp6_14_reg */
} crypto_skp6_14_reg_t, *pcrypto_skp6_14_reg_t;

/**
  \brief Union type to access crypto_skp6_13_reg (@ 0x00001708).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001708) secure key storage skp6[447:416] register          */
  
  struct {
    __OM  uint32_t skp6_13 : 32;              /*!< [31..0] skp6[447:416]                                             */
  } b;                                        /*!< bit fields for crypto_skp6_13_reg */
} crypto_skp6_13_reg_t, *pcrypto_skp6_13_reg_t;

/**
  \brief Union type to access crypto_skp6_12_reg (@ 0x0000170C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000170C) secure key storage skp6[415:384] register          */
  
  struct {
    __OM  uint32_t skp6_12 : 32;              /*!< [31..0] skp6[415:384]                                             */
  } b;                                        /*!< bit fields for crypto_skp6_12_reg */
} crypto_skp6_12_reg_t, *pcrypto_skp6_12_reg_t;

/**
  \brief Union type to access crypto_skp6_11_reg (@ 0x00001710).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001710) secure key storage skp6[383:352] register          */
  
  struct {
    __OM  uint32_t skp6_11 : 32;              /*!< [31..0] skp6[383:352]                                             */
  } b;                                        /*!< bit fields for crypto_skp6_11_reg */
} crypto_skp6_11_reg_t, *pcrypto_skp6_11_reg_t;

/**
  \brief Union type to access crypto_skp6_10_reg (@ 0x00001714).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001714) secure key storage skp6[351:320] register          */
  
  struct {
    __OM  uint32_t skp6_10 : 32;              /*!< [31..0] skp6[351:320]                                             */
  } b;                                        /*!< bit fields for crypto_skp6_10_reg */
} crypto_skp6_10_reg_t, *pcrypto_skp6_10_reg_t;

/**
  \brief Union type to access crypto_skp6_9_reg (@ 0x00001718).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001718) secure key storage skp6[319:288] register          */
  
  struct {
    __OM  uint32_t skp6_9 : 32;               /*!< [31..0] skp6[319:288]                                             */
  } b;                                        /*!< bit fields for crypto_skp6_9_reg */
} crypto_skp6_9_reg_t, *pcrypto_skp6_9_reg_t;

/**
  \brief Union type to access crypto_skp6_8_reg (@ 0x0000171C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000171C) secure key storage skp6[287:256] register          */
  
  struct {
    __OM  uint32_t skp6_8 : 32;               /*!< [31..0] skp6[287:256]                                             */
  } b;                                        /*!< bit fields for crypto_skp6_8_reg */
} crypto_skp6_8_reg_t, *pcrypto_skp6_8_reg_t;

/**
  \brief Union type to access crypto_skp6_7_reg (@ 0x00001720).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001720) secure key storage skp6[255:224] register          */
  
  struct {
    __OM  uint32_t skp6_7 : 32;               /*!< [31..0] skp6[255:224]                                             */
  } b;                                        /*!< bit fields for crypto_skp6_7_reg */
} crypto_skp6_7_reg_t, *pcrypto_skp6_7_reg_t;

/**
  \brief Union type to access crypto_skp6_6_reg (@ 0x00001724).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001724) secure key storage skp6[223:192] register          */
  
  struct {
    __OM  uint32_t skp6_6 : 32;               /*!< [31..0] skp6[223:192]                                             */
  } b;                                        /*!< bit fields for crypto_skp6_6_reg */
} crypto_skp6_6_reg_t, *pcrypto_skp6_6_reg_t;

/**
  \brief Union type to access crypto_skp6_5_reg (@ 0x00001728).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001728) secure key storage skp6[191:160] register          */
  
  struct {
    __OM  uint32_t skp6_5 : 32;               /*!< [31..0] skp6[191:160]                                             */
  } b;                                        /*!< bit fields for crypto_skp6_5_reg */
} crypto_skp6_5_reg_t, *pcrypto_skp6_5_reg_t;

/**
  \brief Union type to access crypto_skp6_4_reg (@ 0x0000172C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000172C) secure key storage skp6[159:128] register          */
  
  struct {
    __OM  uint32_t skp6_4 : 32;               /*!< [31..0] skp6[159:128]                                             */
  } b;                                        /*!< bit fields for crypto_skp6_4_reg */
} crypto_skp6_4_reg_t, *pcrypto_skp6_4_reg_t;

/**
  \brief Union type to access crypto_skp6_3_reg (@ 0x00001730).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001730) secure key storage skp6[127:96] register           */
  
  struct {
    __OM  uint32_t skp6_3 : 32;               /*!< [31..0] skp6[127:96]                                              */
  } b;                                        /*!< bit fields for crypto_skp6_3_reg */
} crypto_skp6_3_reg_t, *pcrypto_skp6_3_reg_t;

/**
  \brief Union type to access crypto_skp6_2_reg (@ 0x00001734).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001734) secure key storage skp6[95:64] register            */
  
  struct {
    __OM  uint32_t skp6_2 : 32;               /*!< [31..0] skp6[95:64]                                               */
  } b;                                        /*!< bit fields for crypto_skp6_2_reg */
} crypto_skp6_2_reg_t, *pcrypto_skp6_2_reg_t;

/**
  \brief Union type to access crypto_skp6_1_reg (@ 0x00001738).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001738) secure key storage skp6[63:32] register            */
  
  struct {
    __OM  uint32_t skp6_1 : 32;               /*!< [31..0] skp6[63:32]                                               */
  } b;                                        /*!< bit fields for crypto_skp6_1_reg */
} crypto_skp6_1_reg_t, *pcrypto_skp6_1_reg_t;

/**
  \brief Union type to access crypto_skp6_0_reg (@ 0x0000173C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000173C) secure key storage skp6[31:0] register             */
  
  struct {
    __OM  uint32_t skp6_0: 32;                /*!< [31..0] skp6[31:0]                                                */
  } b;                                        /*!< bit fields for crypto_skp6_0_reg */
} crypto_skp6_0_reg_t, *pcrypto_skp6_0_reg_t;

/**
  \brief Union type to access crypto_skp7_15_reg (@ 0x00001800).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001800) secure key storage skp7[511:480] register          */
  
  struct {
    __OM  uint32_t skp7_15 : 32;              /*!< [31..0] skp7[511:480]                                             */
  } b;                                        /*!< bit fields for crypto_skp7_15_reg */
} crypto_skp7_15_reg_t, *pcrypto_skp7_15_reg_t;

/**
  \brief Union type to access crypto_skp7_14_reg (@ 0x00001804).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001804) secure key storage skp7[479:448] register          */
  
  struct {
    __OM  uint32_t skp7_14 : 32;              /*!< [31..0] skp7[479:448]                                             */
  } b;                                        /*!< bit fields for crypto_skp7_14_reg */
} crypto_skp7_14_reg_t, *pcrypto_skp7_14_reg_t;

/**
  \brief Union type to access crypto_skp7_13_reg (@ 0x00001808).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001808) secure key storage skp7[447:416] register          */
  
  struct {
    __OM  uint32_t skp7_13 : 32;              /*!< [31..0] skp7[447:416]                                             */
  } b;                                        /*!< bit fields for crypto_skp7_13_reg */
} crypto_skp7_13_reg_t, *pcrypto_skp7_13_reg_t;

/**
  \brief Union type to access crypto_skp7_12_reg (@ 0x0000180C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000180C) secure key storage skp7[415:384] register          */
  
  struct {
    __OM  uint32_t skp7_12 : 32;              /*!< [31..0] skp7[415:384]                                             */
  } b;                                        /*!< bit fields for crypto_skp7_12_reg */
} crypto_skp7_12_reg_t, *pcrypto_skp7_12_reg_t;

/**
  \brief Union type to access crypto_skp7_11_reg (@ 0x00001810).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001810) secure key storage skp7[383:352] register          */
  
  struct {
    __OM  uint32_t skp7_11 : 32;              /*!< [31..0] skp7[383:352]                                             */
  } b;                                        /*!< bit fields for crypto_skp7_11_reg */
} crypto_skp7_11_reg_t, *pcrypto_skp7_11_reg_t;

/**
  \brief Union type to access crypto_skp7_10_reg (@ 0x00001814).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001814) secure key storage skp7[351:320] register          */
  
  struct {
    __OM  uint32_t skp7_10 : 32;              /*!< [31..0] skp7[351:320]                                             */
  } b;                                        /*!< bit fields for crypto_skp7_10_reg */
} crypto_skp7_10_reg_t, *pcrypto_skp7_10_reg_t;

/**
  \brief Union type to access crypto_skp7_9_reg (@ 0x00001818).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001818) secure key storage skp7[319:288] register          */
  
  struct {
    __OM  uint32_t skp7_9 : 32;               /*!< [31..0] skp7[319:288]                                             */
  } b;                                        /*!< bit fields for crypto_skp7_9_reg */
} crypto_skp7_9_reg_t, *pcrypto_skp7_9_reg_t;

/**
  \brief Union type to access crypto_skp7_8_reg (@ 0x0000181C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000181C) secure key storage skp7[287:256] register          */
  
  struct {
    __OM  uint32_t skp7_8 : 32;               /*!< [31..0] skp7[287:256]                                             */
  } b;                                        /*!< bit fields for crypto_skp7_8_reg */
} crypto_skp7_8_reg_t, *pcrypto_skp7_8_reg_t;

/**
  \brief Union type to access crypto_skp7_7_reg (@ 0x00001820).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001820) secure key storage skp7[255:224] register          */
  
  struct {
    __OM  uint32_t skp7_7 : 32;               /*!< [31..0] skp7[255:224]                                             */
  } b;                                        /*!< bit fields for crypto_skp7_7_reg */
} crypto_skp7_7_reg_t, *pcrypto_skp7_7_reg_t;

/**
  \brief Union type to access crypto_skp7_6_reg (@ 0x00001824).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001824) secure key storage skp7[223:192] register          */
  
  struct {
    __OM  uint32_t skp7_6 : 32;               /*!< [31..0] skp7[223:192]                                             */
  } b;                                        /*!< bit fields for crypto_skp7_6_reg */
} crypto_skp7_6_reg_t, *pcrypto_skp7_6_reg_t;

/**
  \brief Union type to access crypto_skp7_5_reg (@ 0x00001828).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001828) secure key storage skp7[191:160] register          */
  
  struct {
    __OM  uint32_t skp7_5 : 32;               /*!< [31..0] skp7[191:160]                                             */
  } b;                                        /*!< bit fields for crypto_skp7_5_reg */
} crypto_skp7_5_reg_t, *pcrypto_skp7_5_reg_t;

/**
  \brief Union type to access crypto_skp7_4_reg (@ 0x0000182C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000182C) secure key storage skp7[159:128] register          */
  
  struct {
    __OM  uint32_t skp7_4 : 32;               /*!< [31..0] skp7[159:128]                                             */
  } b;                                        /*!< bit fields for crypto_skp7_4_reg */
} crypto_skp7_4_reg_t, *pcrypto_skp7_4_reg_t;

/**
  \brief Union type to access crypto_skp7_3_reg (@ 0x00001830).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001830) secure key storage skp7[127:96] register           */
  
  struct {
    __OM  uint32_t skp7_3 : 32;               /*!< [31..0] skp7[127:96]                                              */
  } b;                                        /*!< bit fields for crypto_skp7_3_reg */
} crypto_skp7_3_reg_t, *pcrypto_skp7_3_reg_t;

/**
  \brief Union type to access crypto_skp7_2_reg (@ 0x00001834).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001834) secure key storage skp7[95:64] register            */
  
  struct {
    __OM  uint32_t skp7_2 : 32;               /*!< [31..0] skp7[95:64]                                               */
  } b;                                        /*!< bit fields for crypto_skp7_2_reg */
} crypto_skp7_2_reg_t, *pcrypto_skp7_2_reg_t;

/**
  \brief Union type to access crypto_skp7_1_reg (@ 0x00001838).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001838) secure key storage skp7[63:32] register            */
  
  struct {
    __OM  uint32_t skp7_1 : 32;               /*!< [31..0] skp7[63:32]                                               */
  } b;                                        /*!< bit fields for crypto_skp7_1_reg */
} crypto_skp7_1_reg_t, *pcrypto_skp7_1_reg_t;

/**
  \brief Union type to access crypto_skp7_0_reg (@ 0x0000183C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000183C) secure key storage skp7[31:0] register             */
  
  struct {
    __OM  uint32_t skp7_0: 32;                /*!< [31..0] skp7[31:0]                                                */
  } b;                                        /*!< bit fields for crypto_skp7_0_reg */
} crypto_skp7_0_reg_t, *pcrypto_skp7_0_reg_t;

/**
  \brief Union type to access crypto_skp8_15_reg (@ 0x00001900).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001900) secure key storage skp8[511:480] register          */
  
  struct {
    __OM  uint32_t skp8_15 : 32;              /*!< [31..0] skp8[511:480]                                             */
  } b;                                        /*!< bit fields for crypto_skp8_15_reg */
} crypto_skp8_15_reg_t, *pcrypto_skp8_15_reg_t;

/**
  \brief Union type to access crypto_skp8_14_reg (@ 0x00001904).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001904) secure key storage skp8[479:448] register          */
  
  struct {
    __OM  uint32_t skp8_14 : 32;              /*!< [31..0] skp8[479:448]                                             */
  } b;                                        /*!< bit fields for crypto_skp8_14_reg */
} crypto_skp8_14_reg_t, *pcrypto_skp8_14_reg_t;

/**
  \brief Union type to access crypto_skp8_13_reg (@ 0x00001908).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001908) secure key storage skp8[447:416] register          */
  
  struct {
    __OM  uint32_t skp8_13 : 32;              /*!< [31..0] skp8[447:416]                                             */
  } b;                                        /*!< bit fields for crypto_skp8_13_reg */
} crypto_skp8_13_reg_t, *pcrypto_skp8_13_reg_t;

/**
  \brief Union type to access crypto_skp8_12_reg (@ 0x0000190C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000190C) secure key storage skp8[415:384] register          */
  
  struct {
    __OM  uint32_t skp8_12 : 32;              /*!< [31..0] skp8[415:384]                                             */
  } b;                                        /*!< bit fields for crypto_skp8_12_reg */
} crypto_skp8_12_reg_t, *pcrypto_skp8_12_reg_t;

/**
  \brief Union type to access crypto_skp8_11_reg (@ 0x00001910).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001910) secure key storage skp8[383:352] register          */
  
  struct {
    __OM  uint32_t skp8_11 : 32;              /*!< [31..0] skp8[383:352]                                             */
  } b;                                        /*!< bit fields for crypto_skp8_11_reg */
} crypto_skp8_11_reg_t, *pcrypto_skp8_11_reg_t;

/**
  \brief Union type to access crypto_skp8_10_reg (@ 0x00001914).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001914) secure key storage skp8[351:320] register          */
  
  struct {
    __OM  uint32_t skp8_10 : 32;              /*!< [31..0] skp8[351:320]                                             */
  } b;                                        /*!< bit fields for crypto_skp8_10_reg */
} crypto_skp8_10_reg_t, *pcrypto_skp8_10_reg_t;

/**
  \brief Union type to access crypto_skp8_9_reg (@ 0x00001918).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001918) secure key storage skp8[319:288] register          */
  
  struct {
    __OM  uint32_t skp8_9 : 32;               /*!< [31..0] skp8[319:288]                                             */
  } b;                                        /*!< bit fields for crypto_skp8_9_reg */
} crypto_skp8_9_reg_t, *pcrypto_skp8_9_reg_t;

/**
  \brief Union type to access crypto_skp8_8_reg (@ 0x0000191C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000191C) secure key storage skp8[287:256] register          */
  
  struct {
    __OM  uint32_t skp8_8 : 32;               /*!< [31..0] skp8[287:256]                                             */
  } b;                                        /*!< bit fields for crypto_skp8_8_reg */
} crypto_skp8_8_reg_t, *pcrypto_skp8_8_reg_t;

/**
  \brief Union type to access crypto_skp8_7_reg (@ 0x00001920).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001920) secure key storage skp8[255:224] register          */
  
  struct {
    __OM  uint32_t skp8_7 : 32;               /*!< [31..0] skp8[255:224]                                             */
  } b;                                        /*!< bit fields for crypto_skp8_7_reg */
} crypto_skp8_7_reg_t, *pcrypto_skp8_7_reg_t;

/**
  \brief Union type to access crypto_skp8_6_reg (@ 0x00001924).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001924) secure key storage skp8[223:192] register          */
  
  struct {
    __OM  uint32_t skp8_6 : 32;               /*!< [31..0] skp8[223:192]                                             */
  } b;                                        /*!< bit fields for crypto_skp8_6_reg */
} crypto_skp8_6_reg_t, *pcrypto_skp8_6_reg_t;

/**
  \brief Union type to access crypto_skp8_5_reg (@ 0x00001928).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001928) secure key storage skp8[191:160] register          */
  
  struct {
    __OM  uint32_t skp8_5 : 32;               /*!< [31..0] skp8[191:160]                                             */
  } b;                                        /*!< bit fields for crypto_skp8_5_reg */
} crypto_skp8_5_reg_t, *pcrypto_skp8_5_reg_t;

/**
  \brief Union type to access crypto_skp8_4_reg (@ 0x0000192C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000192C) secure key storage skp8[159:128] register          */
  
  struct {
    __OM  uint32_t skp8_4 : 32;               /*!< [31..0] skp8[159:128]                                             */
  } b;                                        /*!< bit fields for crypto_skp8_4_reg */
} crypto_skp8_4_reg_t, *pcrypto_skp8_4_reg_t;

/**
  \brief Union type to access crypto_skp8_3_reg (@ 0x00001930).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001930) secure key storage skp8[127:96] register           */
  
  struct {
    __OM  uint32_t skp8_3 : 32;               /*!< [31..0] skp8[127:96]                                              */
  } b;                                        /*!< bit fields for crypto_skp8_3_reg */
} crypto_skp8_3_reg_t, *pcrypto_skp8_3_reg_t;

/**
  \brief Union type to access crypto_skp8_2_reg (@ 0x00001934).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001934) secure key storage skp8[95:64] register            */
  
  struct {
    __OM  uint32_t skp8_2 : 32;               /*!< [31..0] skp8[95:64]                                               */
  } b;                                        /*!< bit fields for crypto_skp8_2_reg */
} crypto_skp8_2_reg_t, *pcrypto_skp8_2_reg_t;

/**
  \brief Union type to access crypto_skp8_1_reg (@ 0x00001938).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001938) secure key storage skp8[63:32] register            */
  
  struct {
    __OM  uint32_t skp8_1 : 32;               /*!< [31..0] skp8[63:32]                                               */
  } b;                                        /*!< bit fields for crypto_skp8_1_reg */
} crypto_skp8_1_reg_t, *pcrypto_skp8_1_reg_t;

/**
  \brief Union type to access crypto_skp8_0_reg (@ 0x0000193C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x0000193C) secure key storage skp8[31:0] register             */
  
  struct {
    __OM  uint32_t skp8_0: 32;                /*!< [31..0] skp8[31:0]                                                */
  } b;                                        /*!< bit fields for crypto_skp8_0_reg */
} crypto_skp8_0_reg_t, *pcrypto_skp8_0_reg_t;

/**
  \brief Union type to access crypto_skp9_15_reg (@ 0x00001A00).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001A00) secure key storage skp9[511:480] register          */
  
  struct {
    __OM  uint32_t skp9_15 : 32;              /*!< [31..0] skp9[511:480]                                             */
  } b;                                        /*!< bit fields for crypto_skp9_15_reg */
} crypto_skp9_15_reg_t, *pcrypto_skp9_15_reg_t;

/**
  \brief Union type to access crypto_skp9_14_reg (@ 0x00001A04).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001A04) secure key storage skp9[479:448] register          */
  
  struct {
    __OM  uint32_t skp9_14 : 32;              /*!< [31..0] skp9[479:448]                                             */
  } b;                                        /*!< bit fields for crypto_skp9_14_reg */
} crypto_skp9_14_reg_t, *pcrypto_skp9_14_reg_t;

/**
  \brief Union type to access crypto_skp9_13_reg (@ 0x00001A08).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001A08) secure key storage skp9[447:416] register          */
  
  struct {
    __OM  uint32_t skp9_13 : 32;              /*!< [31..0] skp9[447:416]                                             */
  } b;                                        /*!< bit fields for crypto_skp9_13_reg */
} crypto_skp9_13_reg_t, *pcrypto_skp9_13_reg_t;

/**
  \brief Union type to access crypto_skp9_12_reg (@ 0x00001A0C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001A0C) secure key storage skp9[415:384] register          */
  
  struct {
    __OM  uint32_t skp9_12 : 32;              /*!< [31..0] skp9[415:384]                                             */
  } b;                                        /*!< bit fields for crypto_skp9_12_reg */
} crypto_skp9_12_reg_t, *pcrypto_skp9_12_reg_t;

/**
  \brief Union type to access crypto_skp9_11_reg (@ 0x00001A10).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001A10) secure key storage skp9[383:352] register          */
  
  struct {
    __OM  uint32_t skp9_11 : 32;              /*!< [31..0] skp9[383:352]                                             */
  } b;                                        /*!< bit fields for crypto_skp9_11_reg */
} crypto_skp9_11_reg_t, *pcrypto_skp9_11_reg_t;

/**
  \brief Union type to access crypto_skp9_10_reg (@ 0x00001A14).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001A14) secure key storage skp9[351:320] register          */
  
  struct {
    __OM  uint32_t skp9_10 : 32;              /*!< [31..0] skp9[351:320]                                             */
  } b;                                        /*!< bit fields for crypto_skp9_10_reg */
} crypto_skp9_10_reg_t, *pcrypto_skp9_10_reg_t;

/**
  \brief Union type to access crypto_skp9_9_reg (@ 0x00001A18).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001A18) secure key storage skp9[319:288] register          */
  
  struct {
    __OM  uint32_t skp9_9 : 32;               /*!< [31..0] skp9[319:288]                                             */
  } b;                                        /*!< bit fields for crypto_skp9_9_reg */
} crypto_skp9_9_reg_t, *pcrypto_skp9_9_reg_t;

/**
  \brief Union type to access crypto_skp9_8_reg (@ 0x00001A1C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001A1C) secure key storage skp9[287:256] register          */
  
  struct {
    __OM  uint32_t skp9_8 : 32;               /*!< [31..0] skp9[287:256]                                             */
  } b;                                        /*!< bit fields for crypto_skp9_8_reg */
} crypto_skp9_8_reg_t, *pcrypto_skp9_8_reg_t;

/**
  \brief Union type to access crypto_skp9_7_reg (@ 0x00001A20).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001A20) secure key storage skp9[255:224] register          */
  
  struct {
    __OM  uint32_t skp9_7 : 32;               /*!< [31..0] skp9[255:224]                                             */
  } b;                                        /*!< bit fields for crypto_skp9_7_reg */
} crypto_skp9_7_reg_t, *pcrypto_skp9_7_reg_t;

/**
  \brief Union type to access crypto_skp9_6_reg (@ 0x00001A24).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001A24) secure key storage skp9[223:192] register          */
  
  struct {
    __OM  uint32_t skp9_6 : 32;               /*!< [31..0] skp9[223:192]                                             */
  } b;                                        /*!< bit fields for crypto_skp9_6_reg */
} crypto_skp9_6_reg_t, *pcrypto_skp9_6_reg_t;

/**
  \brief Union type to access crypto_skp9_5_reg (@ 0x00001A28).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001A28) secure key storage skp9[191:160] register          */
  
  struct {
    __OM  uint32_t skp9_5 : 32;               /*!< [31..0] skp9[191:160]                                             */
  } b;                                        /*!< bit fields for crypto_skp9_5_reg */
} crypto_skp9_5_reg_t, *pcrypto_skp9_5_reg_t;

/**
  \brief Union type to access crypto_skp9_4_reg (@ 0x00001A2C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001A2C) secure key storage skp9[159:128] register          */
  
  struct {
    __OM  uint32_t skp9_4 : 32;               /*!< [31..0] skp9[159:128]                                             */
  } b;                                        /*!< bit fields for crypto_skp9_4_reg */
} crypto_skp9_4_reg_t, *pcrypto_skp9_4_reg_t;

/**
  \brief Union type to access crypto_skp9_3_reg (@ 0x00001A30).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001A30) secure key storage skp9[127:96] register           */
  
  struct {
    __OM  uint32_t skp9_3 : 32;               /*!< [31..0] skp9[127:96]                                              */
  } b;                                        /*!< bit fields for crypto_skp9_3_reg */
} crypto_skp9_3_reg_t, *pcrypto_skp9_3_reg_t;

/**
  \brief Union type to access crypto_skp9_2_reg (@ 0x00001A34).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001A34) secure key storage skp9[95:64] register            */
  
  struct {
    __OM  uint32_t skp9_2 : 32;               /*!< [31..0] skp9[95:64]                                               */
  } b;                                        /*!< bit fields for crypto_skp9_2_reg */
} crypto_skp9_2_reg_t, *pcrypto_skp9_2_reg_t;

/**
  \brief Union type to access crypto_skp9_1_reg (@ 0x00001A38).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001A38) secure key storage skp9[63:32] register            */
  
  struct {
    __OM  uint32_t skp9_1 : 32;               /*!< [31..0] skp9[63:32]                                               */
  } b;                                        /*!< bit fields for crypto_skp9_1_reg */
} crypto_skp9_1_reg_t, *pcrypto_skp9_1_reg_t;

/**
  \brief Union type to access crypto_skp9_0_reg (@ 0x00001A3C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001A3C) secure key storage skp9[31:0] register             */
  
  struct {
    __OM  uint32_t skp9_0: 32;                /*!< [31..0] skp9[31:0]                                                */
  } b;                                        /*!< bit fields for crypto_skp9_0_reg */
} crypto_skp9_0_reg_t, *pcrypto_skp9_0_reg_t;

/**
  \brief Union type to access crypto_skp10_15_reg (@ 0x00001B00).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001B00) secure key storage skp10[511:480] register          */
  
  struct {
    __OM  uint32_t skp10_15 : 32;             /*!< [31..0] skp10[511:480]                                             */
  } b;                                        /*!< bit fields for crypto_skp10_15_reg */
} crypto_skp10_15_reg_t, *pcrypto_skp10_15_reg_t;

/**
  \brief Union type to access crypto_skp10_14_reg (@ 0x00001B04).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001B04) secure key storage skp10[479:448] register          */
  
  struct {
    __OM  uint32_t skp10_14 : 32;             /*!< [31..0] skp10[479:448]                                             */
  } b;                                        /*!< bit fields for crypto_skp10_14_reg */
} crypto_skp10_14_reg_t, *pcrypto_skp10_14_reg_t;

/**
  \brief Union type to access crypto_skp10_13_reg (@ 0x00001B08).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001B08) secure key storage skp10[447:416] register          */
  
  struct {
    __OM  uint32_t skp10_13 : 32;             /*!< [31..0] skp10[447:416]                                             */
  } b;                                        /*!< bit fields for crypto_skp10_13_reg */
} crypto_skp10_13_reg_t, *pcrypto_skp10_13_reg_t;

/**
  \brief Union type to access crypto_skp10_12_reg (@ 0x00001B0C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001B0C) secure key storage skp10[415:384] register          */
  
  struct {
    __OM  uint32_t skp10_12 : 32;             /*!< [31..0] skp10[415:384]                                             */
  } b;                                        /*!< bit fields for crypto_skp10_12_reg */
} crypto_skp10_12_reg_t, *pcrypto_skp10_12_reg_t;

/**
  \brief Union type to access crypto_skp10_11_reg (@ 0x00001B10).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001B10) secure key storage skp10[383:352] register          */
  
  struct {
    __OM  uint32_t skp10_11 : 32;             /*!< [31..0] skp10[383:352]                                             */
  } b;                                        /*!< bit fields for crypto_skp10_11_reg */
} crypto_skp10_11_reg_t, *pcrypto_skp10_11_reg_t;

/**
  \brief Union type to access crypto_skp10_10_reg (@ 0x00001B14).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001B14) secure key storage skp10[351:320] register          */
  
  struct {
    __OM  uint32_t skp10_10 : 32;             /*!< [31..0] skp10[351:320]                                             */
  } b;                                        /*!< bit fields for crypto_skp10_10_reg */
} crypto_skp10_10_reg_t, *pcrypto_skp10_10_reg_t;

/**
  \brief Union type to access crypto_skp10_9_reg (@ 0x00001B18).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001B18) secure key storage skp10[319:288] register          */
  
  struct {
    __OM  uint32_t skp10_9 : 32;              /*!< [31..0] skp10[319:288]                                             */
  } b;                                        /*!< bit fields for crypto_skp10_9_reg */
} crypto_skp10_9_reg_t, *pcrypto_skp10_9_reg_t;

/**
  \brief Union type to access crypto_skp10_8_reg (@ 0x00001B1C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001B1C) secure key storage skp10[287:256] register          */
  
  struct {
    __OM  uint32_t skp10_8 : 32;              /*!< [31..0] skp10[287:256]                                             */
  } b;                                        /*!< bit fields for crypto_skp10_8_reg */
} crypto_skp10_8_reg_t, *pcrypto_skp10_8_reg_t;

/**
  \brief Union type to access crypto_skp10_7_reg (@ 0x00001B20).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001B20) secure key storage skp10[255:224] register          */
  
  struct {
    __OM  uint32_t skp10_7 : 32;              /*!< [31..0] skp10[255:224]                                             */
  } b;                                        /*!< bit fields for crypto_skp10_7_reg */
} crypto_skp10_7_reg_t, *pcrypto_skp10_7_reg_t;

/**
  \brief Union type to access crypto_skp10_6_reg (@ 0x00001B24).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001B24) secure key storage skp10[223:192] register          */
  
  struct {
    __OM  uint32_t skp10_6 : 32;              /*!< [31..0] skp10[223:192]                                             */
  } b;                                        /*!< bit fields for crypto_skp10_6_reg */
} crypto_skp10_6_reg_t, *pcrypto_skp10_6_reg_t;

/**
  \brief Union type to access crypto_skp10_5_reg (@ 0x00001B28).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001B28) secure key storage skp10[191:160] register          */
  
  struct {
    __OM  uint32_t skp10_5 : 32;              /*!< [31..0] skp10[191:160]                                             */
  } b;                                        /*!< bit fields for crypto_skp10_5_reg */
} crypto_skp10_5_reg_t, *pcrypto_skp10_5_reg_t;

/**
  \brief Union type to access crypto_skp10_4_reg (@ 0x00001B2C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001B2C) secure key storage skp10[159:128] register          */
  
  struct {
    __OM  uint32_t skp10_4 : 32;              /*!< [31..0] skp10[159:128]                                             */
  } b;                                        /*!< bit fields for crypto_skp10_4_reg */
} crypto_skp10_4_reg_t, *pcrypto_skp10_4_reg_t;

/**
  \brief Union type to access crypto_skp10_3_reg (@ 0x00001B30).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001B30) secure key storage skp10[127:96] register           */
  
  struct {
    __OM  uint32_t skp10_3 : 32;              /*!< [31..0] skp10[127:96]                                              */
  } b;                                        /*!< bit fields for crypto_skp10_3_reg */
} crypto_skp10_3_reg_t, *pcrypto_skp10_3_reg_t;

/**
  \brief Union type to access crypto_skp10_2_reg (@ 0x00001B34).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001B34) secure key storage skp10[95:64] register            */
  
  struct {
    __OM  uint32_t skp10_2 : 32;              /*!< [31..0] skp10[95:64]                                               */
  } b;                                        /*!< bit fields for crypto_skp10_2_reg */
} crypto_skp10_2_reg_t, *pcrypto_skp10_2_reg_t;

/**
  \brief Union type to access crypto_skp10_1_reg (@ 0x00001B38).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001B38) secure key storage skp10[63:32] register            */
  
  struct {
    __OM  uint32_t skp10_1 : 32;              /*!< [31..0] skp10[63:32]                                               */
  } b;                                        /*!< bit fields for crypto_skp10_1_reg */
} crypto_skp10_1_reg_t, *pcrypto_skp10_1_reg_t;

/**
  \brief Union type to access crypto_skp10_0_reg (@ 0x00001B3C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001B3C) secure key storage skp10[31:0] register             */
  
  struct {
    __OM  uint32_t skp10_0: 32;               /*!< [31..0] skp10[31:0]                                                */
  } b;                                        /*!< bit fields for crypto_skp10_0_reg */
} crypto_skp10_0_reg_t, *pcrypto_skp10_0_reg_t;

/**
  \brief Union type to access crypto_skp11_15_reg (@ 0x00001C00).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001C00) secure key storage skp11[511:480] register          */
  
  struct {
    __OM  uint32_t skp11_15 : 32;             /*!< [31..0] skp11[511:480]                                             */
  } b;                                        /*!< bit fields for crypto_skp11_15_reg */
} crypto_skp11_15_reg_t, *pcrypto_skp11_15_reg_t;

/**
  \brief Union type to access crypto_skp11_14_reg (@ 0x00001C04).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001C04) secure key storage skp11[479:448] register          */
  
  struct {
    __OM  uint32_t skp11_14 : 32;             /*!< [31..0] skp11[479:448]                                             */
  } b;                                        /*!< bit fields for crypto_skp11_14_reg */
} crypto_skp11_14_reg_t, *pcrypto_skp11_14_reg_t;

/**
  \brief Union type to access crypto_skp11_13_reg (@ 0x00001C08).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001C08) secure key storage skp11[447:416] register          */
  
  struct {
    __OM  uint32_t skp11_13 : 32;             /*!< [31..0] skp11[447:416]                                             */
  } b;                                        /*!< bit fields for crypto_skp11_13_reg */
} crypto_skp11_13_reg_t, *pcrypto_skp11_13_reg_t;

/**
  \brief Union type to access crypto_skp11_12_reg (@ 0x00001C0C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001C0C) secure key storage skp11[415:384] register          */
  
  struct {
    __OM  uint32_t skp11_12 : 32;             /*!< [31..0] skp11[415:384]                                             */
  } b;                                        /*!< bit fields for crypto_skp11_12_reg */
} crypto_skp11_12_reg_t, *pcrypto_skp11_12_reg_t;

/**
  \brief Union type to access crypto_skp11_11_reg (@ 0x00001C10).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001C10) secure key storage skp11[383:352] register          */
  
  struct {
    __OM  uint32_t skp11_11 : 32;             /*!< [31..0] skp11[383:352]                                             */
  } b;                                        /*!< bit fields for crypto_skp11_11_reg */
} crypto_skp11_11_reg_t, *pcrypto_skp11_11_reg_t;

/**
  \brief Union type to access crypto_skp11_10_reg (@ 0x00001C14).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001C14) secure key storage skp11[351:320] register          */
  
  struct {
    __OM  uint32_t skp11_10 : 32;             /*!< [31..0] skp11[351:320]                                             */
  } b;                                        /*!< bit fields for crypto_skp11_10_reg */
} crypto_skp11_10_reg_t, *pcrypto_skp11_10_reg_t;

/**
  \brief Union type to access crypto_skp11_9_reg (@ 0x00001C18).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001C18) secure key storage skp11[319:288] register          */
  
  struct {
    __OM  uint32_t skp11_9 : 32;              /*!< [31..0] skp11[319:288]                                             */
  } b;                                        /*!< bit fields for crypto_skp11_9_reg */
} crypto_skp11_9_reg_t, *pcrypto_skp11_9_reg_t;

/**
  \brief Union type to access crypto_skp11_8_reg (@ 0x00001C1C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001C1C) secure key storage skp11[287:256] register          */
  
  struct {
    __OM  uint32_t skp11_8 : 32;              /*!< [31..0] skp11[287:256]                                             */
  } b;                                        /*!< bit fields for crypto_skp11_8_reg */
} crypto_skp11_8_reg_t, *pcrypto_skp11_8_reg_t;

/**
  \brief Union type to access crypto_skp11_7_reg (@ 0x00001C20).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001C20) secure key storage skp11[255:224] register          */
  
  struct {
    __OM  uint32_t skp11_7 : 32;              /*!< [31..0] skp11[255:224]                                             */
  } b;                                        /*!< bit fields for crypto_skp11_7_reg */
} crypto_skp11_7_reg_t, *pcrypto_skp11_7_reg_t;

/**
  \brief Union type to access crypto_skp11_6_reg (@ 0x00001C24).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001C24) secure key storage skp11[223:192] register          */
  
  struct {
    __OM  uint32_t skp11_6 : 32;              /*!< [31..0] skp11[223:192]                                             */
  } b;                                        /*!< bit fields for crypto_skp11_6_reg */
} crypto_skp11_6_reg_t, *pcrypto_skp11_6_reg_t;

/**
  \brief Union type to access crypto_skp11_5_reg (@ 0x00001C28).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001C28) secure key storage skp11[191:160] register          */
  
  struct {
    __OM  uint32_t skp11_5 : 32;              /*!< [31..0] skp11[191:160]                                             */
  } b;                                        /*!< bit fields for crypto_skp11_5_reg */
} crypto_skp11_5_reg_t, *pcrypto_skp11_5_reg_t;

/**
  \brief Union type to access crypto_skp11_4_reg (@ 0x00001C2C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001C2C) secure key storage skp11[159:128] register          */
  
  struct {
    __OM  uint32_t skp11_4 : 32;              /*!< [31..0] skp11[159:128]                                             */
  } b;                                        /*!< bit fields for crypto_skp11_4_reg */
} crypto_skp11_4_reg_t, *pcrypto_skp11_4_reg_t;

/**
  \brief Union type to access crypto_skp11_3_reg (@ 0x00001C30).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001C30) secure key storage skp11[127:96] register           */
  
  struct {
    __OM  uint32_t skp11_3 : 32;              /*!< [31..0] skp11[127:96]                                              */
  } b;                                        /*!< bit fields for crypto_skp11_3_reg */
} crypto_skp11_3_reg_t, *pcrypto_skp11_3_reg_t;

/**
  \brief Union type to access crypto_skp11_2_reg (@ 0x00001C34).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001C34) secure key storage skp11[95:64] register            */
  
  struct {
    __OM  uint32_t skp11_2 : 32;              /*!< [31..0] skp11[95:64]                                               */
  } b;                                        /*!< bit fields for crypto_skp11_2_reg */
} crypto_skp11_2_reg_t, *pcrypto_skp11_2_reg_t;

/**
  \brief Union type to access crypto_skp11_1_reg (@ 0x00001C38).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001C38) secure key storage skp11[63:32] register            */
  
  struct {
    __OM  uint32_t skp11_1 : 32;              /*!< [31..0] skp11[63:32]                                               */
  } b;                                        /*!< bit fields for crypto_skp11_1_reg */
} crypto_skp11_1_reg_t, *pcrypto_skp11_1_reg_t;

/**
  \brief Union type to access crypto_skp11_0_reg (@ 0x00001C3C).
*/
typedef union {
  __IOM uint32_t w;                           /*!< (@ 0x00001C3C) secure key storage skp11[31:0] register             */
  
  struct {
    __OM  uint32_t skp11_0: 32;               /*!< [31..0] skp11[31:0]                                                */
  } b;                                        /*!< bit fields for crypto_skp11_0_reg */
} crypto_skp11_0_reg_t, *pcrypto_skp11_0_reg_t;
#endif

/** @} */ /* End of group hs_hal_crypto_reg */
/// @endcond /* End of condition DOXYGEN_GENERAL_REG_TYPE || DOXYGEN_CRYPTO_REG_TYPE */


#ifdef  __cplusplus
}
#endif

#endif    // end of #ifndef __RTK_CRYPTO_BASE_TYPE_H__
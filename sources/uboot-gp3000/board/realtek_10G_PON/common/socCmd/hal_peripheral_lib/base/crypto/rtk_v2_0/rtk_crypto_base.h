#ifndef __RTK_CRYPTO_BASE_H__
#define __RTK_CRYPTO_BASE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ================================================================================================================= */
/* ================                            Device Specific Peripheral Section                             ====== */
/* ================================================================================================================= */

/// @cond DOXYGEN_CRYPTO_REG_TYPE

/** @addtogroup crypto_device_reg Crypto Engine Registers.
 *  @ingroup hs_hal_crypto
 *  @{
 */




/* ================================================================================================================= */
/* ================                                          CRYPTO                                           ====== */
/* ================================================================================================================= */


/**
 * @brief Crypto in Secure (CRYPTO)
 */
typedef struct {                                /*!< (@ 0x50070000) CRYPTO Structure */

  union {
    __IOM uint32_t srcdesc_status_reg;          /*!< (@ 0x00000000) Source Descriptor FIFO status Register */

    struct {
      __IM  uint32_t fifo_empty_cnt : 16;       /*!< [15..0] Source Descriptor FIFO empty counter                   */
      __IM  uint32_t            : 3;            /*!< [23..16] reserved                                              */
      __IOM uint32_t src_fail   : 1;            /*!< [24..24] Source Descriptor fail interrupt                      */
      __IM  uint32_t src_fail_status : 2;       /*!< [26..25] Source Descriptor fail status                         */
      __IM  uint32_t            : 3;            /*!< [29..27] reserved                                              */
      __IOM uint32_t pk_up      : 1;            /*!< [30..30] packet base update wptr to engine                     */
      __IOM uint32_t src_rst    : 1;            /*!< [31..31] Source Descriptor reset(only for pk_up=1'b1           */
    } srcdesc_status_reg_b;
  } ;

  union {
    __IOM uint32_t sdfw_reg;                    /*!< (@ 0x00000004) IPSec Source Descriptor first word              */

    struct {
      __IOM uint32_t sdfw       : 32;           /*!< [31..0] IPSec Source Descriptor first word                     */
    } sdfw_reg_b;
  } ;

  union {
    __IOM uint32_t sdsw_reg;                    /*!< (@ 0x00000008) IPSec Source Descriptor second word             */

    struct {
      __IOM uint32_t sdsw       : 32;           /*!< [31..0] IPSec Source Descriptor second word                    */
    } sdsw_reg_b;
  } ;
  //__IM  uint32_t  RESERVED;                     /*!< (@ 0x0000000C) reserved                                        */
  union {
    __IOM uint32_t srcdesc_sdptr_reg;           /*!< (@ 0x0000000C) Source Descriptor FIFO status Register */

    struct {
      __IM  uint32_t swptr      : 16;           /*!< [15..0] Source Descriptor FIFO write pointer                   */
      __IM  uint32_t srptr      : 16;           /*!< [31..16] Source Descriptor FIFO read pointer                   */
    } srcdesc_sdptr_reg_b;
  } ;

  union {
    __IOM uint32_t ipscsr_reset_isr_conf_reg;   /*!< (@ 0x00000010) IPSec Command/Status Register,
                                                 *                  for reset / isr poll status
                                                 */

    struct {
      __OM  uint32_t soft_rst   : 1;            /*!< [0..0] Software Reset write 1 to reset                         */
      __IM  uint32_t            : 2;
      __IM  uint32_t dma_busy   : 1;            /*!< [3..3] Ipsec dma busy                                          */
      __IOM uint32_t cmd_ok     : 1;            /*!< [4..4] Command OK interrupt                                    */

      __IM  uint32_t            : 2;
      __IOM uint32_t intr_mode  : 1;            /*!< [7..7] Select ok interrupt mode                                */
      __IM  uint32_t ok_intr_cnt : 8;           /*!< [15..8] OK interrupt counter                                   */
      __IOM uint32_t clear_ok_intr_num : 8;     /*!< [23..16] Clear OK interrupt number                             */

      __IM  uint32_t            : 7;
      __OM  uint32_t ipsec_rst  : 1;            /*!< [31..31] Ipsec engine Reset Write 1 to reset the crypto engine
                                                 *    and DMA engine
                                                 */
    } ipscsr_reset_isr_conf_reg_b;
  } ;

  union {
    __IOM uint32_t ipscsr_int_mask_reg;         /*!< (@ 0x00000014) IPSec Command/Status Register, for interrupt mask */

    struct {
      __IOM uint32_t cmd_ok_m   : 1;            /*!< [0..0] Command OK interrupt Mask 1: Mask Enable
                                                 * 0: Mask Disable
                                                 */
      __IOM uint32_t src_fail_m : 1;            /*!< [1..1] Source Descriptor Error 0 Interrupt Mask 1: Mask Enable
                                                 * 0: Mask Disable
                                                 */
      __IOM uint32_t dst_fail_m : 1;            /*!< [1..1] Destination Descriptor Error 0 Interrupt Mask 1: Mask Enable
                                                 * 0: Mask Disable
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
                                                 *   0: Mask Disable
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
    } ipscsr_int_mask_reg_b;
  } ;

  union {
    __IOM uint32_t ipscsr_debug_reg;            /*!< (@ 0x00000018) IPSec Command/Status Register, for debug        */

    struct {
      __IOM uint32_t dma_wait_cycle : 16;       /*!< [15..0] Software Reset                                         */
      __IOM uint32_t arbiter_mode : 1;          /*!< [16..16] dma arbiter mode: 1'b0 : round-robin 1'b1 : detect
                                                 *   fifo wasted level
                                                 */
      __IM  uint32_t            : 3;
      __IOM uint32_t debug_port_sel : 4;        /*!< [23..20] Debug port selection: 4'd0 : engine_dbg,
                                                 *   4'd1 : dma_lexra_dbg, 4'd2 : dma_rx_dbg, 4'd3 : dma_tx_dbg
                                                 */
      __IOM uint32_t engine_clk_en : 1;         /*!< [24..24] Ipsec Engine clock enable                             */
      __IM  uint32_t            : 3;            /*!< [27..25] reserved                                              */
      __IOM uint32_t mst_bad_sel   : 2;         /*!< [29..28] <for debug port> select Lexra master data bus  (option)
                                                 * 2'd0 :  mst_ipsec_bad[07:00]       2'd1:  mst_ipsec_bad[15:08]
                                                 * 2'd2 :  mst_ipsec_bad[23:16]       2'd3:  mst_ipsec_bad[31:24]
                                                 */
      __IM  uint32_t            : 1;            /*!< [30..30] reserved                                              */
      __IOM uint32_t debug_wb   : 1;            /*!< [31..31] Debug : write back mode                               */
    } ipscsr_debug_reg_b;
  } ;

  union {
    __IOM uint16_t ipscsr_err_stats_reg;        /*!< (@ 0x0000001C) IPSec Command/Status Register, for error status */

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
      __IOM uint16_t des_err6   : 1;            /*!< [15..15] Destination Descriptor Error 6 interrupt
                                                 *   when set sequential hash 1. md5: adl!=16 2. sha1: adl!=20
                                                 *   3. sha224/256: adl!=32 4. sha384/512: adl!=64 Write 1 to clear
                                                 */
    } ipscsr_err_stats_reg_b;
  } ;
  //__IM  uint16_t  RESERVED1;

  union {
    __IM  uint32_t ipscsr_a2eo_sum_reg;         /*!< (@ 0x00000020) IPSec Command/Status Register, for sum of a2eo             */

    struct {
      __IM  uint32_t a2eo_sum   : 11;           /*!< [10..0] sum of a2eo: header total length                                  */
    } ipscsr_a2eo_sum_reg_b;
  } ;

  union {
    __IM  uint32_t ipscsr_enl_sum_reg;          /*!< (@ 0x00000024) IPSec Command/Status Register, for sum of enl              */

    struct {
      __IM  uint32_t enl_sum    : 24;           /*!< [23..0] sum of enl: encryption total length                               */
    } ipscsr_enl_sum_reg_b;
  } ;

  union {
    __IM  uint32_t ipscsr_apl_sum_reg;          /*!< (@ 0x00000028) IPSec Command/Status Register, for sum of apl              */

    struct {
      __IM  uint32_t apl_sum    : 12;           /*!< [11..0] sum of apl: hash padding total length                             */
    } ipscsr_apl_sum_reg_b;
  } ;

  union {
    __IM  uint32_t ipscsr_epl_sum_reg;          /*!< (@ 0x0000002C) IPSec Command/Status Register, for sum of epl              */

    struct {
      __IM  uint32_t epl_sum    : 6;            /*!< [5..0] sum of epl: encryption padding total length                        */
    } ipscsr_epl_sum_reg_b;
  } ;

  union {
    __IOM uint32_t ipscsr_swap_burst_reg;       /*!< (@ 0x00000030) IPSec Command/Status Register, for swap / burst
                                                 *                  numbers
                                                 */

    struct {
      __IOM uint32_t set_swap   : 1;            /*!< [0..0] Byte swap for command setting data 1: Enable 0: Disable */
      __IOM uint32_t key_iv_swap : 1;           /*!< [1..1] Byte swap for key and iv 1: Enable 0: Disable           */
      __IOM uint32_t key_pad_swap : 1;          /*!< [2..2] Byte swap for hmac key 1: Enable 0: Disable             */
      __IOM uint32_t hash_inital_value_swap : 1;/*!< [3..3] Byte swap for sequential hash initial value 1: Enable
                                                 *   0: Disable
                                                 */
      __IOM uint32_t dma_in_little_endian : 1;  /*!< [4..4] Input data is little endian 1: little endian 0: big endian
                                                 */
      __IM  uint32_t            : 3;
      __IOM uint32_t tx_byte_swap : 1;          /*!< [8..8] Byte swap for dma_tx engine input data 1: Enable 0: Disable
                                                 */
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
      __IM  uint32_t            : 2;            /*!< [15..14] reserved                                              */
      __IOM uint32_t dma_burst_length : 6;      /*!< [21..16] dma burst length Max : 16 Min: 1                      */
      __IM  uint32_t            : 2;            /*!< [23..22] reserved                                              */
      __IOM uint32_t md5_output_data_byte_swap : 1; /*!< [24..24] byte swap for md5 output data 1: Enable 0:
                                                 *   Disable
                                                 */
      __IOM uint32_t md5_input_data_byte_swap  : 1; /*!< [25..25] byte swap for md5 output data 1: Enable 0:
                                                 *   Disable
                                                 */
      __IM  uint32_t            : 6;            /*!< [31..26] reserved                                              */
    } ipscsr_swap_burst_reg_b;
  } ;
  __IM  uint32_t  RESERVED2[3];

  union {
    __IM  uint32_t ipscsr_engine_dbg_reg;       /*!< (@ 0x00000040) IPSec Command/Status Register, for engine debug            */

    struct {
      __IM  uint32_t engine_dbg : 32;           /*!< [31..0] Crypto engine debug values                                        */
    } ipscsr_engine_dbg_reg_b;
  } ;

  union {
    __IM  uint32_t ipscsr_dma_lx_dbg_reg;       /*!< (@ 0x00000044) IPSec Command/Status Register, for DMA lx debug            */

    struct {
      __IM  uint32_t dma_lx_dbg : 32;           /*!< [31..0] DMA lx debug values                                               */
    } ipscsr_dma_lx_dbg_reg_b;
  } ;

  union {
    __IM  uint32_t ipscsr_dma_rx_dbg_reg;       /*!< (@ 0x00000048) IPSec Command/Status Register, for DMA rx debug            */

    struct {
      __IM  uint32_t dma_rx_dbg : 32;           /*!< [31..0] DMA rx debug values                                               */
    } ipscsr_dma_rx_dbg_reg_b;
  } ;

  union {
    __IM  uint32_t ipscsr_dma_tx_dbg_reg;       /*!< (@ 0x0000004C) IPSec Command/Status Register, for DMA tx debug            */

    struct {
      __IM  uint32_t dma_tx_dbg : 32;           /*!< [31..0] DMA tx debug values                                               */
    } ipscsr_dma_tx_dbg_reg_b;
  } ;

  union {
    __IM  uint32_t ipscsr_sta_sdes_cfg_reg;     /*!< (@ 0x00000050) IPSec Command/Status Register, for source descriptor
                                                                    configure status                                           */

    struct {
      __IM  uint32_t sta_sdes_cfg : 32;         /*!< [31..0] Source descriptor configure status values                         */
    } ipscsr_sta_sdes_cfg_reg_b;
  } ;

  union {
    __IM  uint32_t ipscsr_sta_sdes_ptr_reg;     /*!< (@ 0x00000054) IPSec Command/Status Register, for source descriptor
                                                                    pointer status                                             */

    struct {
      __IM  uint32_t sta_sdes_ptr : 32;         /*!< [31..0] Source descriptor pointer status values                           */
    } ipscsr_sta_sdes_ptr_reg_b;
  } ;

  union {
    __IM  uint32_t ipscsr_sta_sdes_cmd1;        /*!< (@ 0x00000058) IPSec Command/Status Register, for source descriptor
                                                                    command 1 status                                           */

    struct {
      __IM  uint32_t sta_sdes_cmd1 : 32;        /*!< [31..0] Source descriptor command 1 status values                         */
    } ipscsr_sta_sdes_cmd1_b;
  } ;

  union {
    __IM  uint32_t ipscsr_sta_sdes_cmd2;        /*!< (@ 0x0000005C) IPSec Command/Status Register, for source descriptor
                                                                    command 2 status                                           */

    struct {
      __IM  uint32_t sta_sdes_cmd2 : 32;        /*!< [31..0] Source descriptor command 2 status values                         */
    } ipscsr_sta_sdes_cmd2_b;
  } ;

  union {
    __IM  uint32_t ipscsr_sta_sdes_cmd3;        /*!< (@ 0x00000060) IPSec Command/Status Register, for source descriptor
                                                                    command 3 status                                           */

    struct {
      __IM  uint32_t sta_sdes_cmd3 : 32;        /*!< [31..0] Source descriptor command 3 status values                         */
    } ipscsr_sta_sdes_cmd3_b;
  } ;

  union {
    __IM  uint32_t ipscsr_sta_ddes_cfg_reg;     /*!< (@ 0x00000064) IPSec Command/Status Register, for destination
                                                                    descriptor configure status                                */

    struct {
      __IM  uint32_t sta_ddes_cfg : 32;         /*!< [31..0] Destination descriptor configure status values                    */
    } ipscsr_sta_ddes_cfg_reg_b;
  } ;

  union {
    __IM  uint32_t ipscsr_sta_ddes_ptr_reg;     /*!< (@ 0x00000068) IPSec Command/Status Register, for destination
                                                                    descriptor pointer status                                  */

    struct {
      __IM  uint32_t sta_ddes_ptr : 32;         /*!< [31..0] Destination descriptor pointer status values                      */
    } ipscsr_sta_ddes_ptr_reg_b;
  } ;
  __IM  uint32_t  RESERVED3[37];

  union {
    __OM  uint32_t crc_rst_reg;                 /*!< (@ 0x00000100) crc reset                                       */

    struct {
      __OM  uint32_t rst        : 1;            /*!< [0..0] CRC reset                                               */
    } crc_rst_reg_b;
  } ;

  union {
    __IOM uint32_t crc_op_reg;                  /*!< (@ 0x00000104) crc operation                                   */

    struct {
      __IOM uint32_t crc_iswap  : 3;            /*!< [2..0] swap input data                                         */
      __IOM uint32_t crc_oswap  : 1;            /*!< [3..3] swap output data                                        */
      __IOM uint32_t crc_sel    : 3;            /*!< [6..4] crc sel                                                 */
      __IOM uint32_t crc_dma    : 1;            /*!< [7..7] DMA mode                                                */
      __IOM uint32_t crc_be     : 2;            /*!< [9..8] big endian                                              */

      __IM  uint32_t            : 2;
      __IOM uint32_t crc_last   : 1;            /*!< [12..12] crc last                                              */

      __IM  uint32_t            : 3;
      __IOM uint32_t crc_length : 16;           /*!< [31..16] length                                                */
    } crc_op_reg_b;
  } ;

  union {
    __IOM uint32_t crc_poly_reg;                /*!< (@ 0x00000108) CRC polynomia register                          */

    struct {
      __IOM uint32_t crc_poly   : 32;           /*!< [31..0] crc polynomial coefficients                            */
    } crc_poly_reg_b;
  } ;

  union {
    __IOM uint32_t crc_iv_reg;                  /*!< (@ 0x0000010C) CRC initial value register                      */

    struct {
      __IOM uint32_t crc_init   : 32;           /*!< [31..0] crc init value                                         */
    } crc_iv_reg_b;
  } ;

  union {
    __IOM uint32_t crc_oxor_reg;                /*!< (@ 0x00000110) CRC initial value register                      */

    struct {
      __IOM uint32_t crc_oxor   : 32;           /*!< [31..0] if output xor mode                                     */
    } crc_oxor_reg_b;
  } ;

  union {
    __IOM uint32_t crc_data_reg;                /*!< (@ 0x00000114) CRC data register                               */

    struct {
      __IOM uint32_t crc_data   : 32;           /*!< [31..0] din - Command mode : DMA==1'b0 SDBP - Source Data Buffer
                                                 *   Pointer : DMA==1'b1
                                                 */
    } crc_data_reg_b;
  } ;

  union {
    __IOM uint32_t crc_stat_reg;                /*!< (@ 0x00000118) CRC status register                             */

    struct {
      __IM  uint32_t busy       : 1;            /*!< [0..0] still busy in previous execution                        */
      __IOM  uint32_t crc_ok     : 1;           /*!< [1..1] finish execution                                        */
      __IOM uint32_t crc_intr_mask : 1;         /*!< [2..2] interrupt mask                                          */
      __OM  uint32_t crc_little_endian : 1;     /*!< [3..3] little endian                                           */
    } crc_stat_reg_b;
  } ;

  union {
    __IM  uint32_t crc_result_reg;              /*!< (@ 0x0000011C) CRC result register                             */

    struct {
      __IM  uint32_t crc_out    : 32;           /*!< [31..0] output result                                          */
    } crc_result_reg_b;
  } ;

  union {
    __IM  uint32_t crc_count_reg;               /*!< (@ 0x00000120) CRC count register                              */

    struct {
      __IM  uint32_t crc_cnt    : 16;           /*!< [15..0] count number in bytes                                  */
    } crc_count_reg_b;
  } ;

  union {
    __IM  uint32_t crc_dma_rx_dbg_reg;          /*!< (@ 0x00000124) CRC DMA rx dbg register                         */

    struct {
      __IM  uint32_t crc_dma_rx_dbg : 32;       /*!< [31..0] CRC DMA rx debug values                                */
    } crc_dma_rx_dbg_reg_b;
  } ;
  //__IM  uint32_t  RESERVED4[950]; // between 0x00000128 to 0x00001000
  __IM  uint32_t  RESERVED4[54];

  union {
    __IM  uint32_t ipsaxi_cache_reg;            /*!< (@ 0x00000200) AXI cache register                              */

    struct {
      __IOM uint32_t awcache : 4;               /*!< [3..0] AXI write cache setting                                 */
      __IM  uint32_t         : 4;               /*!< [7..4] reserved                                                */
      __IOM uint32_t arcache : 4;               /*!< [11..8] AXI read cache setting                                 */
      __IM  uint32_t         : 19;              /*!< [30..12] reserved                                              */
      __IOM uint32_t awcache_last_tran_en : 1;  /*!< [31..31] Use for the last write transition                     */
    } ipsaxi_cache_reg_b;
  } ;

  union {
    __IM  uint32_t ipsaxi_cache_resp_reg;       /*!< (@ 0x00000204) AXI response register                           */

    struct {
      __IM  uint32_t bresp : 2;                 /*!< [1..0] AXI write response                                      */
      __IM  uint32_t rresp : 2;                 /*!< [3..2] AXI read response                                       */
      __IM  uint32_t       : 28;                /*!< [31..4] reserved                                               */
    } ipsaxi_cache_resp_reg_b;
  } ;

  union {
    __IM  uint32_t ips4k_boundary_reg;          /*!< (@ 0x00000208) 4K boundary register                            */

    struct {
      __IOM uint32_t cfg_4k_boundary_en : 1;    /*!< [0..0] 4KB Address Boundary Limitation for Bursts
                                                 * 0 : disable        1 : enable
                                                 */
      __IM  uint32_t       : 31;                /*!< [31..1] reserved                                               */
    } ips4k_boundary_reg_b;
  } ;

  __IM  uint32_t  RESERVED5[185];

  union {
    __IOM uint32_t ipskey_lock_reg;             /*!< (@ 0x000004F0) secure key lock register                        */

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
      __IM  uint32_t            : 18;           /*!< [29..12] reserved                                             */
      __IOM uint32_t sk_wb_byte_swap : 1;       /*!< [30..30] Byte swap for secure key write back
                                                 * 1: enable      0: disable
                                                 */
      __IOM uint32_t sk_wb_word_swap : 1;       /*!< [31..31] Word swap for secure key write back
                                                 * 1: enable      0: disable
                                                 */
    } ipskey_lock_reg_b;
  } ;

  //__IM  uint32_t  RESERVED6[2];
  union {
    __IOM uint32_t ipskey_storage_dbg_disable_reg;  /*!< (@ 0x000004F4) secure key storage dbg disable register    */

    struct {
      __IOM uint32_t dbg_key     : 1;               /*!< [0..0] key                                                */
      __IOM uint32_t dbg_keypad  : 1;               /*!< [1..1] keypad                                             */
      __IOM uint32_t dbg_otp_key : 1;               /*!< [2..2] otp key                                            */
      __IM  uint32_t             : 29;              /*!< [31..3] reserved                                          */

    } ipskey_storage_dbg_disable_reg_b;
  } ;

  union {
    __IOM uint32_t ipskey_storage_dbg_reg;      /*!< (@ 0x000004F8) secure key storage dbg register                */

    struct {
      __IOM uint32_t dbg_read : 8;               /*!< [0..7] password:
                                                  * 0x52, 0x54, 0x4B, 0x32, 0x33, 0x37,
                                                  * 0x39, 0x43, 0x4E, 0x33, 0x88, 0x99
                                                  */
      __IM  uint32_t          : 24;              /*!< [31..8] reserved                                          */

    } ipskey_storage_dbg_reg_b;
  } ;

  union {
    __IOM uint32_t ipsotp_key_swap_reg;         /*!< (@ 0x000004FC) secure otp key swap register                    */

    struct {
      __IOM uint32_t otp_byte_swap : 1;         /*!< [0..0] Byte swap for OTP key
                                                 * 1: enable      0: disable
                                                 */
      __IOM uint32_t otp_word_swap : 1;         /*!< [1..1] Word swap for OTP key
                                                 * 1: enable      0: disable
                                                 */
      __IM  uint32_t               : 30;        /*!< [31..2] reserved                                               */

    } ipsotp_key_swap_reg_b;
  } ;

  __OM  uint32_t ipssk_reg[12][8];   /*!< (@ 0x00000500 ~ 0x0000067C)
                                      *
                                      */

  __IM  uint32_t  RESERVED7[16];

  __OM  uint32_t ipsotp_sk_reg[2][8];   /*!< (@ 0x000006C0 ~ 0x000006FC)
                                        *
                                        */

#if 0
  union {
    __OM  uint32_t ipssk0_7_reg;                /*!< (@ 0x00000500) secure key storage SK0[255:224] register        */

    struct {
      __OM  uint32_t sk0_7 : 32;                /*!< [31..0] SK0[255:224]                                           */
    } ipssk0_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk0_6_reg;                /*!< (@ 0x00000504) secure key storage SK0[223:192] register        */

    struct {
      __OM  uint32_t sk0_6 : 32;                /*!< [31..0] SK0[223:192]                                           */
    } ipssk0_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk0_5_reg;                /*!< (@ 0x00000508) secure key storage SK0[191:160] register        */

    struct {
      __OM  uint32_t sk0_5 : 32;                /*!< [31..0] SK0[191:160]                                           */
    } ipssk0_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk0_4_reg;                /*!< (@ 0x0000050C) secure key storage SK0[159:128] register        */

    struct {
      __OM  uint32_t sk0_4 : 32;                /*!< [31..0] SK0[159:128]                                           */
    } ipssk0_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk0_3_reg;                /*!< (@ 0x00000510) secure key storage SK0[127:96] register         */

    struct {
      __OM  uint32_t sk0_3 : 32;                /*!< [31..0] SK0[127:96]                                            */
    } ipssk0_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk0_2_reg;                /*!< (@ 0x00000514) secure key storage SK0[95:64] register          */

    struct {
      __OM  uint32_t sk0_2 : 32;                /*!< [31..0] SK0[95:64]                                             */
    } ipssk0_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk0_1_reg;                /*!< (@ 0x00000518) secure key storage SK0[63:32] register          */

    struct {
      __OM  uint32_t sk0_1 : 32;                /*!< [31..0] SK0[63:32]                                             */
    } ipssk0_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk0_0_reg;                /*!< (@ 0x0000051C) secure key storage SK0[31:0] register           */

    struct {
      __OM  uint32_t sk0_0: 32;                 /*!< [31..0] SK0[31:0]                                              */
    } ipssk0_0_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk1_7_reg;                /*!< (@ 0x00000520) secure key storage SK1[255:224] register        */

    struct {
      __OM  uint32_t sk1_7 : 32;                /*!< [31..0] SK1[255:224]                                           */
    } ipssk1_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk1_6_reg;                /*!< (@ 0x00000524) secure key storage SK1[223:192] register        */

    struct {
      __OM  uint32_t sk1_6 : 32;                /*!< [31..0] SK1[223:192]                                           */
    } ipssk1_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk1_5_reg;                /*!< (@ 0x00000528) secure key storage SK1[191:160] register        */

    struct {
      __OM  uint32_t sk1_5 : 32;                /*!< [31..0] SK1[191:160]                                           */
    } ipssk1_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk1_4_reg;                /*!< (@ 0x0000052C) secure key storage SK1[159:128] register        */

    struct {
      __OM  uint32_t sk1_4 : 32;                /*!< [31..0] SK1[159:128]                                           */
    } ipssk1_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk1_3_reg;                /*!< (@ 0x00000530) secure key storage SK1[127:96] register         */

    struct {
      __OM  uint32_t sk1_3 : 32;                /*!< [31..0] SK1[127:96]                                            */
    } ipssk1_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk1_2_reg;                /*!< (@ 0x00000534) secure key storage SK1[95:64] register          */

    struct {
      __OM  uint32_t sk1_2 : 32;                /*!< [31..0] SK1[95:64]                                             */
    } ipssk1_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk1_1_reg;                /*!< (@ 0x00000538) secure key storage SK1[63:32] register          */

    struct {
      __OM  uint32_t sk1_1 : 32;                /*!< [31..0] SK1[63:32]                                             */
    } ipssk1_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk1_0_reg;                /*!< (@ 0x0000053C) secure key storage SK1[31:0] register           */

    struct {
      __OM  uint32_t sk1_0: 32;                 /*!< [31..0] SK1[31:0]                                              */
    } ipssk1_0_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk2_7_reg;                /*!< (@ 0x00000540) secure key storage SK2[255:224] register        */

    struct {
      __OM  uint32_t sk2_7 : 32;                /*!< [31..0] SK2[255:224]                                           */
    } ipssk2_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk2_6_reg;                /*!< (@ 0x00000544) secure key storage SK2[223:192] register        */

    struct {
      __OM  uint32_t sk2_6 : 32;                /*!< [31..0] SK2[223:192]                                           */
    } ipssk2_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk2_5_reg;                /*!< (@ 0x00000548) secure key storage SK2[191:160] register        */

    struct {
      __OM  uint32_t sk2_5 : 32;                /*!< [31..0] SK2[191:160]                                           */
    } ipssk2_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk2_4_reg;                /*!< (@ 0x0000054C) secure key storage SK2[159:128] register        */

    struct {
      __OM  uint32_t sk2_4 : 32;                /*!< [31..0] SK2[159:128]                                           */
    } ipssk2_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk2_3_reg;                /*!< (@ 0x00000550) secure key storage SK2[127:96] register         */

    struct {
      __OM  uint32_t sk2_3 : 32;                /*!< [31..0] SK2[127:96]                                            */
    } ipssk2_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk2_2_reg;                /*!< (@ 0x00000554) secure key storage SK2[95:64] register          */

    struct {
      __OM  uint32_t sk2_2 : 32;                /*!< [31..0] SK2[95:64]                                             */
    } ipssk2_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk2_1_reg;                /*!< (@ 0x00000558) secure key storage SK2[63:32] register          */

    struct {
      __OM  uint32_t sk2_1 : 32;                /*!< [31..0] SK2[63:32]                                             */
    } ipssk2_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk2_0_reg;                /*!< (@ 0x0000055C) secure key storage SK2[31:0] register           */

    struct {
      __OM  uint32_t sk2_0: 32;                 /*!< [31..0] SK2[31:0]                                              */
    } ipssk2_0_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk3_7_reg;                /*!< (@ 0x00000560) secure key storage SK3[255:224] register        */

    struct {
      __OM  uint32_t sk3_7 : 32;                /*!< [31..0] SK3[255:224]                                           */
    } ipssk3_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk3_6_reg;                /*!< (@ 0x00000564) secure key storage SK3[223:192] register        */

    struct {
      __OM  uint32_t sk3_6 : 32;                /*!< [31..0] SK3[223:192]                                           */
    } ipssk3_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk3_5_reg;                /*!< (@ 0x00000568) secure key storage SK3[191:160] register        */

    struct {
      __OM  uint32_t sk3_5 : 32;                /*!< [31..0] SK3[191:160]                                           */
    } ipssk3_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk3_4_reg;                /*!< (@ 0x0000056C) secure key storage SK3[159:128] register        */

    struct {
      __OM  uint32_t sk3_4 : 32;                /*!< [31..0] SK3[159:128]                                           */
    } ipssk3_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk3_3_reg;                /*!< (@ 0x00000570) secure key storage SK3[127:96] register         */

    struct {
      __OM  uint32_t sk3_3 : 32;                /*!< [31..0] SK3[127:96]                                            */
    } ipssk3_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk3_2_reg;                /*!< (@ 0x00000574) secure key storage SK3[95:64] register          */

    struct {
      __OM  uint32_t sk3_2 : 32;                /*!< [31..0] SK3[95:64]                                             */
    } ipssk3_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk3_1_reg;                /*!< (@ 0x00000578) secure key storage SK3[63:32] register          */

    struct {
      __OM  uint32_t sk3_1 : 32;                /*!< [31..0] SK3[63:32]                                             */
    } ipssk3_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk3_0_reg;                /*!< (@ 0x0000057C) secure key storage SK3[31:0] register           */

    struct {
      __OM  uint32_t sk3_0: 32;                 /*!< [31..0] SK3[31:0]                                              */
    } ipssk3_0_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk4_7_reg;                /*!< (@ 0x00000580) secure key storage SK4[255:224] register        */

    struct {
      __OM  uint32_t sk4_7 : 32;                /*!< [31..0] SK4[255:224]                                           */
    } ipssk4_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk4_6_reg;                /*!< (@ 0x00000584) secure key storage SK4[223:192] register        */

    struct {
      __OM  uint32_t sk4_6 : 32;                /*!< [31..0] SK4[223:192]                                           */
    } ipssk4_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk4_5_reg;                /*!< (@ 0x00000588) secure key storage SK4[191:160] register        */

    struct {
      __OM  uint32_t sk4_5 : 32;                /*!< [31..0] SK4[191:160]                                           */
    } ipssk4_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk4_4_reg;                /*!< (@ 0x0000058C) secure key storage SK4[159:128] register        */

    struct {
      __OM  uint32_t sk4_4 : 32;                /*!< [31..0] SK4[159:128]                                           */
    } ipssk4_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk4_3_reg;                /*!< (@ 0x00000590) secure key storage SK4[127:96] register         */

    struct {
      __OM  uint32_t sk4_3 : 32;                /*!< [31..0] SK4[127:96]                                            */
    } ipssk4_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk4_2_reg;                /*!< (@ 0x00000594) secure key storage SK4[95:64] register          */

    struct {
      __OM  uint32_t sk4_2 : 32;                /*!< [31..0] SK4[95:64]                                             */
    } ipssk4_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk4_1_reg;                /*!< (@ 0x00000598) secure key storage SK4[63:32] register          */

    struct {
      __OM  uint32_t sk4_1 : 32;                /*!< [31..0] SK4[63:32]                                             */
    } ipssk4_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk4_0_reg;                /*!< (@ 0x0000059C) secure key storage SK4[31:0] register           */

    struct {
      __OM  uint32_t sk4_0: 32;                 /*!< [31..0] SK4[31:0]                                              */
    } ipssk4_0_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk5_7_reg;                /*!< (@ 0x000005A0) secure key storage SK5[255:224] register        */

    struct {
      __OM  uint32_t sk5_7 : 32;                /*!< [31..0] SK5[255:224]                                           */
    } ipssk5_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk5_6_reg;                /*!< (@ 0x000005A4) secure key storage SK5[223:192] register        */

    struct {
      __OM  uint32_t sk5_6 : 32;                /*!< [31..0] SK5[223:192]                                           */
    } ipssk5_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk5_5_reg;                /*!< (@ 0x000005A8) secure key storage SK5[191:160] register        */

    struct {
      __OM  uint32_t sk5_5 : 32;                /*!< [31..0] SK5[191:160]                                           */
    } ipssk5_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk5_4_reg;                /*!< (@ 0x000005AC) secure key storage SK5[159:128] register        */

    struct {
      __OM  uint32_t sk5_4 : 32;                /*!< [31..0] SK5[159:128]                                           */
    } ipssk5_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk5_3_reg;                /*!< (@ 0x000005B0) secure key storage SK5[127:96] register         */

    struct {
      __OM  uint32_t sk5_3 : 32;                /*!< [31..0] SK5[127:96]                                            */
    } ipssk5_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk5_2_reg;                /*!< (@ 0x000005B4) secure key storage SK5[95:64] register          */

    struct {
      __OM  uint32_t sk5_2 : 32;                /*!< [31..0] SK5[95:64]                                             */
    } ipssk5_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk5_1_reg;                /*!< (@ 0x000005B8) secure key storage SK5[63:32] register          */

    struct {
      __OM  uint32_t sk5_1 : 32;                /*!< [31..0] SK5[63:32]                                             */
    } ipssk5_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk5_0_reg;                /*!< (@ 0x000005BC) secure key storage SK5[31:0] register           */

    struct {
      __OM  uint32_t sk5_0: 32;                 /*!< [31..0] SK5[31:0]                                              */
    } ipssk5_0_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk6_7_reg;                /*!< (@ 0x000005C0) secure key storage SK6[255:224] register        */

    struct {
      __OM  uint32_t sk6_7 : 32;                /*!< [31..0] SK6[255:224]                                           */
    } ipssk6_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk6_6_reg;                /*!< (@ 0x000005C4) secure key storage SK6[223:192] register        */

    struct {
      __OM  uint32_t sk6_6 : 32;                /*!< [31..0] SK6[223:192]                                           */
    } ipssk6_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk6_5_reg;                /*!< (@ 0x000005C8) secure key storage SK6[191:160] register        */

    struct {
      __OM  uint32_t sk6_5 : 32;                /*!< [31..0] SK6[191:160]                                           */
    } ipssk6_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk6_4_reg;                /*!< (@ 0x000005CC) secure key storage SK6[159:128] register        */

    struct {
      __OM  uint32_t sk6_4 : 32;                /*!< [31..0] SK6[159:128]                                           */
    } ipssk6_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk6_3_reg;                /*!< (@ 0x000005D0) secure key storage SK6[127:96] register         */

    struct {
      __OM  uint32_t sk6_3 : 32;                /*!< [31..0] SK6[127:96]                                            */
    } ipssk6_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk6_2_reg;                /*!< (@ 0x000005D4) secure key storage SK6[95:64] register          */

    struct {
      __OM  uint32_t sk6_2 : 32;                /*!< [31..0] SK6[95:64]                                             */
    } ipssk6_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk6_1_reg;                /*!< (@ 0x000005D8) secure key storage SK6[63:32] register          */

    struct {
      __OM  uint32_t sk6_1 : 32;                /*!< [31..0] SK6[63:32]                                             */
    } ipssk6_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk6_0_reg;                /*!< (@ 0x000005DC) secure key storage SK6[31:0] register           */

    struct {
      __OM  uint32_t sk6_0: 32;                 /*!< [31..0] SK6[31:0]                                              */
    } ipssk6_0_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk7_7_reg;                /*!< (@ 0x000005E0) secure key storage SK7[255:224] register        */

    struct {
      __OM  uint32_t sk7_7 : 32;                /*!< [31..0] SK7[255:224]                                           */
    } ipssk7_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk7_6_reg;                /*!< (@ 0x000005E4) secure key storage SK7[223:192] register        */

    struct {
      __OM  uint32_t sk7_6 : 32;                /*!< [31..0] SK7[223:192]                                           */
    } ipssk7_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk7_5_reg;                /*!< (@ 0x000005E8) secure key storage SK7[191:160] register        */

    struct {
      __OM  uint32_t sk7_5 : 32;                /*!< [31..0] SK7[191:160]                                           */
    } ipssk7_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk7_4_reg;                /*!< (@ 0x000005EC) secure key storage SK7[159:128] register        */

    struct {
      __OM  uint32_t sk7_4 : 32;                /*!< [31..0] SK7[159:128]                                           */
    } ipssk7_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk7_3_reg;                /*!< (@ 0x000005F0) secure key storage SK7[127:96] register         */

    struct {
      __OM  uint32_t sk7_3 : 32;                /*!< [31..0] SK7[127:96]                                            */
    } ipssk7_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk7_2_reg;                /*!< (@ 0x000005F4) secure key storage SK7[95:64] register          */

    struct {
      __OM  uint32_t sk7_2 : 32;                /*!< [31..0] SK7[95:64]                                             */
    } ipssk7_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk7_1_reg;                /*!< (@ 0x000005F8) secure key storage SK7[63:32] register          */

    struct {
      __OM  uint32_t sk7_1 : 32;                /*!< [31..0] SK7[63:32]                                             */
    } ipssk7_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk7_0_reg;                /*!< (@ 0x000005FC) secure key storage SK7[31:0] register           */

    struct {
      __OM  uint32_t sk7_0 : 32;                /*!< [31..0] SK7[31:0]                                              */
    } ipssk7_0_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk8_7_reg;                /*!< (@ 0x00000600) secure key storage SK8[255:224] register        */

    struct {
      __OM  uint32_t sk8_7 : 32;                /*!< [31..0] SK8[255:224]                                           */
    } ipssk8_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk8_6_reg;                /*!< (@ 0x00000604) secure key storage SK8[223:192] register        */

    struct {
      __OM  uint32_t sk8_6 : 32;                /*!< [31..0] SK8[223:192]                                           */
    } ipssk8_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk8_5_reg;                /*!< (@ 0x00000608) secure key storage SK8[191:160] register        */

    struct {
      __OM  uint32_t sk8_5 : 32;                /*!< [31..0] SK8[191:160]                                           */
    } ipssk8_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk8_4_reg;                /*!< (@ 0x0000060C) secure key storage SK8[159:128] register        */

    struct {
      __OM  uint32_t sk8_4 : 32;                /*!< [31..0] SK8[159:128]                                           */
    } ipssk8_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk8_3_reg;                /*!< (@ 0x00000610) secure key storage SK8[127:96] register         */

    struct {
      __OM  uint32_t sk8_3 : 32;                /*!< [31..0] SK8[127:96]                                            */
    } ipssk8_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk8_2_reg;                /*!< (@ 0x00000614) secure key storage SK8[95:64] register          */

    struct {
      __OM  uint32_t sk8_2 : 32;                /*!< [31..0] SK8[95:64]                                             */
    } ipssk8_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk8_1_reg;                /*!< (@ 0x00000618) secure key storage SK8[63:32] register          */

    struct {
      __OM  uint32_t sk8_1 : 32;                /*!< [31..0] SK8[63:32]                                             */
    } ipssk8_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk8_0_reg;                /*!< (@ 0x0000061C) secure key storage SK8[31:0] register           */

    struct {
      __OM  uint32_t sk8_0 : 32;                /*!< [31..0] SK8[31:0]                                              */
    } ipssk8_0_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk9_7_reg;                /*!< (@ 0x00000620) secure key storage SK9[255:224] register        */

    struct {
      __OM  uint32_t sk9_7 : 32;                /*!< [31..0] SK9[255:224]                                           */
    } ipssk9_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk9_6_reg;                /*!< (@ 0x00000624) secure key storage SK9[223:192] register        */

    struct {
      __OM  uint32_t sk9_6 : 32;                /*!< [31..0] SK9[223:192]                                           */
    } ipssk9_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk9_5_reg;                /*!< (@ 0x00000628) secure key storage SK9[191:160] register        */

    struct {
      __OM  uint32_t sk9_5 : 32;                /*!< [31..0] SK9[191:160]                                           */
    } ipssk9_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk9_4_reg;                /*!< (@ 0x0000062C) secure key storage SK9[159:128] register        */

    struct {
      __OM  uint32_t sk9_4 : 32;                /*!< [31..0] SK9[159:128]                                           */
    } ipssk9_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk9_3_reg;                /*!< (@ 0x00000630) secure key storage SK9[127:96] register         */

    struct {
      __OM  uint32_t sk9_3 : 32;                /*!< [31..0] SK9[127:96]                                            */
    } ipssk9_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk9_2_reg;                /*!< (@ 0x00000634) secure key storage SK9[95:64] register          */

    struct {
      __OM  uint32_t sk9_2 : 32;                /*!< [31..0] SK9[95:64]                                             */
    } ipssk9_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk9_1_reg;                /*!< (@ 0x00000638) secure key storage SK9[63:32] register          */

    struct {
      __OM  uint32_t sk9_1 : 32;                /*!< [31..0] SK9[63:32]                                             */
    } ipssk9_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk9_0_reg;                /*!< (@ 0x0000063C) secure key storage SK9[31:0] register           */

    struct {
      __OM  uint32_t sk9_0 : 32;                /*!< [31..0] SK9[31:0]                                              */
    } ipssk9_0_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk10_7_reg;               /*!< (@ 0x00000640) secure key storage SK10[255:224] register       */

    struct {
      __OM  uint32_t sk10_7 : 32;               /*!< [31..0] SK10[255:224]                                          */
    } ipssk10_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk10_6_reg;               /*!< (@ 0x00000644) secure key storage SK10[223:192] register       */

    struct {
      __OM  uint32_t sk10_6 : 32;               /*!< [31..0] SK10[223:192]                                          */
    } ipssk10_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk10_5_reg;               /*!< (@ 0x00000648) secure key storage SK10[191:160] register       */

    struct {
      __OM  uint32_t sk10_5 : 32;               /*!< [31..0] SK10[191:160]                                          */
    } ipssk10_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk10_4_reg;               /*!< (@ 0x0000064C) secure key storage SK10[159:128] register       */

    struct {
      __OM  uint32_t sk10_4 : 32;               /*!< [31..0] SK10[159:128]                                          */
    } ipssk10_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk10_3_reg;               /*!< (@ 0x00000650) secure key storage SK10[127:96] register        */

    struct {
      __OM  uint32_t sk10_3 : 32;               /*!< [31..0] SK10[127:96]                                           */
    } ipssk10_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk10_2_reg;               /*!< (@ 0x00000654) secure key storage SK10[95:64] register         */

    struct {
      __OM  uint32_t sk10_2 : 32;               /*!< [31..0] SK10[95:64]                                            */
    } ipssk10_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk10_1_reg;               /*!< (@ 0x00000658) secure key storage SK10[63:32] register         */

    struct {
      __OM  uint32_t sk10_1 : 32;               /*!< [31..0] SK10[63:32]                                            */
    } ipssk10_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk10_0_reg;               /*!< (@ 0x0000065C) secure key storage SK10[31:0] register          */

    struct {
      __OM  uint32_t sk10_0 : 32;               /*!< [31..0] SK10[31:0]                                             */
    } ipssk10_0_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk11_7_reg;               /*!< (@ 0x00000660) secure key storage SK11[255:224] register       */

    struct {
      __OM  uint32_t sk11_7 : 32;               /*!< [31..0] SK11[255:224]                                          */
    } ipssk11_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk11_6_reg;               /*!< (@ 0x00000664) secure key storage SK11[223:192] register       */

    struct {
      __OM  uint32_t sk11_6 : 32;               /*!< [31..0] SK11[223:192]                                          */
    } ipssk11_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk11_5_reg;               /*!< (@ 0x00000668) secure key storage SK11[191:160] register       */

    struct {
      __OM  uint32_t sk11_5 : 32;               /*!< [31..0] SK11[191:160]                                          */
    } ipssk11_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk11_4_reg;               /*!< (@ 0x0000066C) secure key storage SK11[159:128] register       */

    struct {
      __OM  uint32_t sk11_4 : 32;               /*!< [31..0] SK11[159:128]                                          */
    } ipssk11_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk11_3_reg;               /*!< (@ 0x00000670) secure key storage SK11[127:96] register        */

    struct {
      __OM  uint32_t sk11_3 : 32;               /*!< [31..0] SK11[127:96]                                           */
    } ipssk11_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk11_2_reg;               /*!< (@ 0x00000674) secure key storage SK11[95:64] register         */

    struct {
      __OM  uint32_t sk11_2 : 32;               /*!< [31..0] SK11[95:64]                                            */
    } ipssk11_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk11_1_reg;               /*!< (@ 0x00000678) secure key storage SK11[63:32] register         */

    struct {
      __OM  uint32_t sk11_1 : 32;               /*!< [31..0] SK11[63:32]                                            */
    } ipssk11_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipssk11_0_reg;               /*!< (@ 0x0000067C) secure key storage SK11[31:0] register          */

    struct {
      __OM  uint32_t sk11_0 : 32;               /*!< [31..0] SK11[31:0]                                             */
    } ipssk11_0_reg_b;
  } ;
#endif

  __IM  uint32_t  RESERVED8[576];

  union {
    __IOM uint32_t dstdesc_status_reg;          /*!< (@ 0x00001000) destination descriptor status                   */

    struct {
      __IM  uint32_t fifo_empty_cnt : 16;       /*!< [15..0] Destination Descriptor FIFO empty counter              */
      __IM  uint32_t            : 8;            /*!< [23..16] reserved                                              */
      __IOM uint32_t dst_fail   : 1;            /*!< [24..24] Destination Descriptor fail interrupt                 */
      __IM  uint32_t dst_fail_status : 2;       /*!< [26..25] Destination Descriptor fail status                    */
      __IM  uint32_t            : 4;            /*!< [30..27] reserved                                              */
      __IOM uint32_t dst_rst    : 1;            /*!< [31..31] Destination Descriptor reset                          */
    } dstdesc_status_reg_b;
  } ;

  union {
    __IOM uint32_t ddfw_reg;                    /*!< (@ 0x00001004) destination descriptor first word               */

    struct {
      __IOM uint32_t ddfw       : 32;           /*!< [31..0] IPSec Destination Descriptor first word                */
    } ddfw_reg_b;
  } ;

  union {
    __IOM uint32_t ddsw_reg;                    /*!< (@ 0x00001008) destination descriptor second word              */

    struct {
      __IOM uint32_t ddsw       : 32;           /*!< [31..0] IPSec Destination Descriptor second word               */
    } ddsw_reg_b;
  } ;

  union {
    __IOM uint32_t desc_pkt_conf_reg;           /*!< (@ 0x0000100C) descriptor packet setting                       */

    struct {
      __IM  uint32_t            : 16;           /*!< [15..0] reserved                                               */
      __IM  uint32_t pk_arbiter : 2;            /*!< [17..16] Packet arbiter 2'd0 : bus1, 2'd1 : bus2, 2'd2 : bus3  */
      __IM  uint32_t            : 6;
      __IOM uint32_t bus1_priority_th : 2;      /*!< [25..24] Bus1 priority threshold                               */
      __IOM uint32_t bus2_priority_th : 2;      /*!< [27..26] Bus2 priority threshold                               */
      __IOM uint32_t            : 2;            /*!< [29..28] reserved                                              */
      __IOM uint32_t pk_arbiter_mode : 2;       /*!< [31..30] Packet arbiter mode                                   */
    } desc_pkt_conf_reg_b;
  } ;

  union {
    __IM uint32_t dstdesc_dbgsdr_reg;           /*!< (@ 0x00001010) debug source descriptor data                    */

    struct {
      __IM  uint32_t dbg_sd     : 32;           /*!< [31..0] This register is used with DBG_SPTR                    */
    } dstdesc_dbgsdr_reg_b;
  } ;

  union {
    __IOM uint32_t dstdesc_dbgddr_reg;          /*!< (@ 0x00001014) debug destination descriptor data               */

    struct {
      __IM  uint32_t dbg_dd     : 32;           /*!< [31..0] This register is used with DBG_DPTR                    */
    } dstdesc_dbgddr_reg_b;
  } ;

  union {
    __IOM uint32_t dstdesc_dbgptr_reg;          /*!< (@ 0x00001018) destination descriptor FIFO status register     */

    struct {
      __IOM uint32_t dbg_sptr  : 16;            /*!< [15..0] Source Descriptor FIFO empty counte                    */
      __IOM uint32_t dbg_dptr  : 16;            /*!< [31..16] Destination Descriptor FIFO write pointer             */
    } dstdesc_dbgptr_reg_b;
  } ;

  union {
    __IM uint32_t dstdesc_ddptr_reg;            /*!< (@ 0x0000101C) destination descriptor status                   */

    struct {
      __IM  uint32_t dwptr     : 16;            /*!< [15..0] Destination Descriptor FIFO write pointer              */
      __IM  uint32_t drptr     : 16;            /*!< [31..16] Destination Descriptor FIFO read pointer              */
    } dstdesc_ddptr_reg_b;
  } ;

  __IM  uint32_t  RESERVED9[40];

  union {
    __IM uint32_t ipskeypad_locklen0_reg;       /*!< (@ 0x000010C0) secure keypad lock & len 0 register             */

    struct {
      __IOM uint32_t keypad_len   : 5;          /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit               */
      __IM  uint32_t              : 26;         /*!< [30..5] reserved                                               */
      __IOM uint32_t keypad_lock  : 1;          /*!< [31..31] Control the write lock of keypad
                                                 * 1: keypad write is locked      0: keypad write is unlocked
                                                 */
    } ipskeypad_locklen0_reg_b;
  } ;

  union {
    __IM uint32_t ipskeypad_locklen1_reg;       /*!< (@ 0x000010C4) secure keypad lock & len 1 register             */

    struct {
      __IOM uint32_t keypad_len   : 5;          /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit               */
      __IM  uint32_t              : 26;         /*!< [30..5] reserved                                               */
      __IOM uint32_t keypad_lock  : 1;          /*!< [31..31] Control the write lock of keypad
                                                 * 1: keypad write is locked      0: keypad write is unlocked
                                                 */
    } ipskeypad_locklen1_reg_b;
  } ;

  union {
    __IM uint32_t ipskeypad_locklen2_reg;       /*!< (@ 0x000010C8) secure keypad lock & len 2 register             */

    struct {
      __IOM uint32_t keypad_len   : 5;          /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit               */
      __IM  uint32_t              : 26;         /*!< [30..5] reserved                                               */
      __IOM uint32_t keypad_lock  : 1;          /*!< [31..31] Control the write lock of keypad
                                                 * 1: keypad write is locked      0: keypad write is unlocked
                                                 */
    } ipskeypad_locklen2_reg_b;
  } ;

  union {
    __IM uint32_t ipskeypad_locklen3_reg;       /*!< (@ 0x000010CC) secure keypad lock & len 3 register             */

    struct {
      __IOM uint32_t keypad_len   : 5;          /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit               */
      __IM  uint32_t              : 26;         /*!< [30..5] reserved                                               */
      __IOM uint32_t keypad_lock  : 1;          /*!< [31..31] Control the write lock of keypad
                                                 * 1: keypad write is locked      0: keypad write is unlocked
                                                 */
    } ipskeypad_locklen3_reg_b;
  } ;

  union {
    __IM uint32_t ipskeypad_locklen4_reg;       /*!< (@ 0x000010D0) secure keypad lock & len 4 register             */

    struct {
      __IOM uint32_t keypad_len   : 5;          /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit               */
      __IM  uint32_t              : 26;         /*!< [30..5] reserved                                               */
      __IOM uint32_t keypad_lock  : 1;          /*!< [31..31] Control the write lock of keypad
                                                 * 1: keypad write is locked      0: keypad write is unlocked
                                                 */
    } ipskeypad_locklen4_reg_b;
  } ;

  union {
    __IM uint32_t ipskeypad_locklen5_reg;       /*!< (@ 0x000010D4) secure keypad lock & len 5 register             */

    struct {
      __IOM uint32_t keypad_len   : 5;          /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit               */
      __IM  uint32_t              : 26;         /*!< [30..5] reserved                                               */
      __IOM uint32_t keypad_lock  : 1;          /*!< [31..31] Control the write lock of keypad
                                                 * 1: keypad write is locked      0: keypad write is unlocked
                                                 */
    } ipskeypad_locklen5_reg_b;
  } ;

  union {
    __IM uint32_t ipskeypad_locklen6_reg;       /*!< (@ 0x000010D8) secure keypad lock & len 6 register             */

    struct {
      __IOM uint32_t keypad_len   : 5;          /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit               */
      __IM  uint32_t              : 26;         /*!< [30..5] reserved                                               */
      __IOM uint32_t keypad_lock  : 1;          /*!< [31..31] Control the write lock of keypad
                                                 * 1: keypad write is locked      0: keypad write is unlocked
                                                 */
    } ipskeypad_locklen6_reg_b;
  } ;

  union {
    __IM uint32_t ipskeypad_locklen7_reg;       /*!< (@ 0x000010DC) secure keypad lock & len 7 register             */

    struct {
      __IOM uint32_t keypad_len   : 5;          /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit               */
      __IM  uint32_t              : 26;         /*!< [30..5] reserved                                               */
      __IOM uint32_t keypad_lock  : 1;          /*!< [31..31] Control the write lock of keypad
                                                 * 1: keypad write is locked      0: keypad write is unlocked
                                                 */
    } ipskeypad_locklen7_reg_b;
  } ;

  union {
    __IM uint32_t ipskeypad_locklen8_reg;       /*!< (@ 0x000010E0) secure keypad lock & len 8 register             */

    struct {
      __IOM uint32_t keypad_len   : 5;          /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit               */
      __IM  uint32_t              : 26;         /*!< [30..5] reserved                                               */
      __IOM uint32_t keypad_lock  : 1;          /*!< [31..31] Control the write lock of keypad
                                                 * 1: keypad write is locked      0: keypad write is unlocked
                                                 */
    } ipskeypad_locklen8_reg_b;
  } ;

  union {
    __IM uint32_t ipskeypad_locklen9_reg;       /*!< (@ 0x000010E4) secure keypad lock & len 9 register             */

    struct {
      __IOM uint32_t keypad_len   : 5;          /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit               */
      __IM  uint32_t              : 26;         /*!< [30..5] reserved                                               */
      __IOM uint32_t keypad_lock  : 1;          /*!< [31..31] Control the write lock of keypad
                                                 * 1: keypad write is locked      0: keypad write is unlocked
                                                 */
    } ipskeypad_locklen9_reg_b;
  } ;

  union {
    __IM uint32_t ipskeypad_locklen10_reg;      /*!< (@ 0x000010E8) secure keypad lock & len 10 register            */

    struct {
      __IOM uint32_t keypad_len   : 5;          /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit               */
      __IM  uint32_t              : 26;         /*!< [30..5] reserved                                               */
      __IOM uint32_t keypad_lock  : 1;          /*!< [31..31] Control the write lock of keypad
                                                 * 1: keypad write is locked      0: keypad write is unlocked
                                                 */
    } ipskeypad_locklen10_reg_b;
  } ;

  union {
    __IM uint32_t ipskeypad_locklen11_reg;      /*!< (@ 0x000010EC) secure keypad lock & len 11 register            */

    struct {
      __IOM uint32_t keypad_len   : 5;          /*!< [4..0] Secure keypad length [4:0] : 8 bytes unit               */
      __IM  uint32_t              : 26;         /*!< [30..5] reserved                                               */
      __IOM uint32_t keypad_lock  : 1;          /*!< [31..31] Control the write lock of keypad
                                                 * 1: keypad write is locked      0: keypad write is unlocked
                                                 */
    } ipskeypad_locklen11_reg_b;
  } ;

  __IM  uint32_t  RESERVED10[4];

  __OM  uint32_t ipsskp_reg[12][64];   /*!< (@ 0x00001100 ~ 0x00001C3C) only [*][16] is valid
                                        *
                                        */

#if 0
  union {
    __OM  uint32_t ipsskp0_15_reg;              /*!< (@ 0x00001100) secure keypad storage skp0[511:480] register    */

    struct {
      __OM  uint32_t skp0_15 : 32;              /*!< [31..0] skp0[511:480]                                          */
    } ipsskp0_15_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp0_14_reg;              /*!< (@ 0x00001104) secure keypad storage skp0[479:448] register    */

    struct {
      __OM  uint32_t skp0_14 : 32;              /*!< [31..0] skp0[479:448]                                          */
    } ipsskp0_14_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp0_13_reg;              /*!< (@ 0x00001108) secure keypad storage skp0[447:416] register    */

    struct {
      __OM  uint32_t skp0_13 : 32;              /*!< [31..0] skp0[447:416]                                          */
    } ipsskp0_13_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp0_12_reg;              /*!< (@ 0x0000110C) secure keypad storage skp0[415:384] register    */

    struct {
      __OM  uint32_t skp0_12 : 32;              /*!< [31..0] skp0[415:384]                                          */
    } ipsskp0_12_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp0_11_reg;              /*!< (@ 0x00001110) secure keypad storage skp0[383:352] register    */

    struct {
      __OM  uint32_t skp0_11 : 32;              /*!< [31..0] skp0[383:352]                                          */
    } ipsskp0_11_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp0_10_reg;              /*!< (@ 0x00001114) secure keypad storage skp0[351:320] register    */

    struct {
      __OM  uint32_t skp0_10 : 32;              /*!< [31..0] skp0[351:320]                                          */
    } ipsskp0_10_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp0_9_reg;               /*!< (@ 0x00001118) secure keypad storage skp0[319:288] register    */

    struct {
      __OM  uint32_t skp0_9 : 32;               /*!< [31..0] skp0[319:288]                                          */
    } ipsskp0_9_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp0_8_reg;               /*!< (@ 0x00001118) secure keypad storage skp0[287:256] register    */

    struct {
      __OM  uint32_t skp0_8 : 32;               /*!< [31..0] skp0[287:256]                                          */
    } ipsskp0_8_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp0_7_reg;               /*!< (@ 0x0000111C) secure keypad storage skp0[255:224] register    */

    struct {
      __OM  uint32_t skp0_7 : 32;               /*!< [31..0] skp0[255:224]                                          */
    } ipsskp0_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp0_6_reg;               /*!< (@ 0x00001120) secure keypad storage skp0[223:192] register    */

    struct {
      __OM  uint32_t skp0_6 : 32;               /*!< [31..0] skp0[223:192]                                          */
    } ipsskp0_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp0_5_reg;               /*!< (@ 0x00001124) secure keypad storage skp0[191:160] register    */

    struct {
      __OM  uint32_t skp0_5 : 32;               /*!< [31..0] skp0[191:160]                                          */
    } ipsskp0_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp0_4_reg;               /*!< (@ 0x00001128) secure keypad storage skp0[159:128] register    */

    struct {
      __OM  uint32_t skp0_4 : 32;               /*!< [31..0] skp0[159:128]                                          */
    } ipsskp0_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp0_3_reg;               /*!< (@ 0x0000112C) secure keypad storage skp0[127:96] register     */

    struct {
      __OM  uint32_t skp0_3 : 32;               /*!< [31..0] skp0[127:96]                                           */
    } ipsskp0_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp0_2_reg;               /*!< (@ 0x00001130) secure keypad storage skp0[95:64] register      */

    struct {
      __OM  uint32_t skp0_2 : 32;               /*!< [31..0] skp0[95:64]                                            */
    } ipsskp0_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp0_1_reg;               /*!< (@ 0x00001134) secure keypad storage skp0[63:32] register      */

    struct {
      __OM  uint32_t skp0_1 : 32;               /*!< [31..0] skp0[63:32]                                            */
    } ipsskp0_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp0_0_reg;               /*!< (@ 0x0000113C) secure keypad storage skp0[31:0] register       */

    struct {
      __OM  uint32_t skp0_0 : 32;               /*!< [31..0] skp0[31:0]                                             */
    } ipsskp0_0_reg_b;
  } ;

  __IM  uint32_t  RESERVED10[48];

    union {
    __OM  uint32_t ipsskp1_15_reg;              /*!< (@ 0x00001200) secure keypad storage skp1[511:480] register    */

    struct {
      __OM  uint32_t skp1_15 : 32;              /*!< [31..0] skp1[511:480]                                          */
    } ipsskp1_15_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp1_14_reg;              /*!< (@ 0x00001204) secure keypad storage skp1[479:448] register    */

    struct {
      __OM  uint32_t skp1_14 : 32;              /*!< [31..0] skp1[479:448]                                          */
    } ipsskp1_14_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp1_13_reg;              /*!< (@ 0x00001208) secure keypad storage skp1[447:416] register    */

    struct {
      __OM  uint32_t skp1_13 : 32;              /*!< [31..0] skp1[447:416]                                          */
    } ipsskp1_13_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp1_12_reg;              /*!< (@ 0x0000120C) secure keypad storage skp1[415:384] register    */

    struct {
      __OM  uint32_t skp1_12 : 32;              /*!< [31..0] skp1[415:384]                                          */
    } ipsskp1_12_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp1_11_reg;              /*!< (@ 0x00001210) secure keypad storage skp1[383:352] register    */

    struct {
      __OM  uint32_t skp1_11 : 32;              /*!< [31..0] skp1[383:352]                                          */
    } ipsskp1_11_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp1_10_reg;              /*!< (@ 0x00001214) secure keypad storage skp1[351:320] register    */

    struct {
      __OM  uint32_t skp1_10 : 32;              /*!< [31..0] skp1[351:320]                                          */
    } ipsskp1_10_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp1_9_reg;               /*!< (@ 0x00001218) secure keypad storage skp1[319:288] register    */

    struct {
      __OM  uint32_t skp1_9 : 32;               /*!< [31..0] skp1[319:288]                                          */
    } ipsskp1_9_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp1_8_reg;               /*!< (@ 0x00001218) secure keypad storage skp1[287:256] register    */

    struct {
      __OM  uint32_t skp1_8 : 32;               /*!< [31..0] skp1[287:256]                                          */
    } ipsskp1_8_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp1_7_reg;               /*!< (@ 0x0000121C) secure keypad storage skp1[255:224] register    */

    struct {
      __OM  uint32_t skp1_7 : 32;               /*!< [31..0] skp1[255:224]                                          */
    } ipsskp1_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp1_6_reg;               /*!< (@ 0x00001220) secure keypad storage skp1[223:192] register    */

    struct {
      __OM  uint32_t skp1_6 : 32;               /*!< [31..0] skp1[223:192]                                          */
    } ipsskp1_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp1_5_reg;               /*!< (@ 0x00001224) secure keypad storage skp1[191:160] register    */

    struct {
      __OM  uint32_t skp1_5 : 32;               /*!< [31..0] skp1[191:160]                                          */
    } ipsskp1_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp1_4_reg;               /*!< (@ 0x00001228) secure keypad storage skp1[159:128] register    */

    struct {
      __OM  uint32_t skp1_4 : 32;               /*!< [31..0] skp1[159:128]                                          */
    } ipsskp1_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp1_3_reg;               /*!< (@ 0x0000122C) secure keypad storage skp1[127:96] register     */

    struct {
      __OM  uint32_t skp1_3 : 32;               /*!< [31..0] skp1[127:96]                                           */
    } ipsskp1_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp1_2_reg;               /*!< (@ 0x00001230) secure keypad storage skp1[95:64] register      */

    struct {
      __OM  uint32_t skp1_2 : 32;               /*!< [31..0] skp1[95:64]                                            */
    } ipsskp1_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp1_1_reg;               /*!< (@ 0x00001234) secure keypad storage skp1[63:32] register      */

    struct {
      __OM  uint32_t skp1_1 : 32;               /*!< [31..0] skp1[63:32]                                            */
    } ipsskp1_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp1_0_reg;               /*!< (@ 0x0000123C) secure keypad storage skp1[31:0] register       */

    struct {
      __OM  uint32_t skp1_0 : 32;               /*!< [31..0] skp1[31:0]                                             */
    } ipsskp1_0_reg_b;
  } ;

  __IM  uint32_t  RESERVED11[48];

    union {
    __OM  uint32_t ipsskp2_15_reg;              /*!< (@ 0x00001300) secure keypad storage skp2[511:480] register    */

    struct {
      __OM  uint32_t skp2_15 : 32;              /*!< [31..0] skp2[511:480]                                          */
    } ipsskp2_15_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp2_14_reg;              /*!< (@ 0x00001304) secure keypad storage skp2[479:448] register    */

    struct {
      __OM  uint32_t skp2_14 : 32;              /*!< [31..0] skp2[479:448]                                          */
    } ipsskp2_14_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp2_13_reg;              /*!< (@ 0x00001308) secure keypad storage skp2[447:416] register    */

    struct {
      __OM  uint32_t skp2_13 : 32;              /*!< [31..0] skp2[447:416]                                          */
    } ipsskp2_13_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp2_12_reg;              /*!< (@ 0x0000130C) secure keypad storage skp2[415:384] register    */

    struct {
      __OM  uint32_t skp2_12 : 32;              /*!< [31..0] skp2[415:384]                                          */
    } ipsskp2_12_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp2_11_reg;              /*!< (@ 0x00001310) secure keypad storage skp2[383:352] register    */

    struct {
      __OM  uint32_t skp2_11 : 32;              /*!< [31..0] skp2[383:352]                                          */
    } ipsskp2_11_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp2_10_reg;              /*!< (@ 0x00001314) secure keypad storage skp2[351:320] register    */

    struct {
      __OM  uint32_t skp2_10 : 32;              /*!< [31..0] skp2[351:320]                                          */
    } ipsskp2_10_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp2_9_reg;               /*!< (@ 0x00001318) secure keypad storage skp2[319:288] register    */

    struct {
      __OM  uint32_t skp2_9 : 32;               /*!< [31..0] skp2[319:288]                                          */
    } ipsskp2_9_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp2_8_reg;               /*!< (@ 0x00001318) secure keypad storage skp2[287:256] register    */

    struct {
      __OM  uint32_t skp2_8 : 32;               /*!< [31..0] skp2[287:256]                                          */
    } ipsskp2_8_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp2_7_reg;               /*!< (@ 0x0000131C) secure keypad storage skp2[255:224] register    */

    struct {
      __OM  uint32_t skp2_7 : 32;               /*!< [31..0] skp2[255:224]                                          */
    } ipsskp2_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp2_6_reg;               /*!< (@ 0x00001320) secure keypad storage skp2[223:192] register    */

    struct {
      __OM  uint32_t skp2_6 : 32;               /*!< [31..0] skp2[223:192]                                          */
    } ipsskp2_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp2_5_reg;               /*!< (@ 0x00001324) secure keypad storage skp2[191:160] register    */

    struct {
      __OM  uint32_t skp2_5 : 32;               /*!< [31..0] skp2[191:160]                                          */
    } ipsskp2_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp2_4_reg;               /*!< (@ 0x00001328) secure keypad storage skp2[159:128] register    */

    struct {
      __OM  uint32_t skp2_4 : 32;               /*!< [31..0] skp2[159:128]                                          */
    } ipsskp2_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp2_3_reg;               /*!< (@ 0x0000132C) secure keypad storage skp2[127:96] register     */

    struct {
      __OM  uint32_t skp2_3 : 32;               /*!< [31..0] skp2[127:96]                                           */
    } ipsskp2_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp2_2_reg;               /*!< (@ 0x00001330) secure keypad storage skp2[95:64] register      */

    struct {
      __OM  uint32_t skp2_2 : 32;               /*!< [31..0] skp2[95:64]                                            */
    } ipsskp2_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp2_1_reg;               /*!< (@ 0x00001334) secure keypad storage skp2[63:32] register      */

    struct {
      __OM  uint32_t skp2_1 : 32;               /*!< [31..0] skp2[63:32]                                            */
    } ipsskp2_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp2_0_reg;               /*!< (@ 0x0000133C) secure keypad storage skp2[31:0] register       */

    struct {
      __OM  uint32_t skp2_0 : 32;               /*!< [31..0] skp2[31:0]                                             */
    } ipsskp2_0_reg_b;
  } ;

  __IM  uint32_t  RESERVED12[48];

    union {
    __OM  uint32_t ipsskp3_15_reg;              /*!< (@ 0x00001400) secure keypad storage skp3[511:480] register    */

    struct {
      __OM  uint32_t skp3_15 : 32;              /*!< [31..0] skp3[511:480]                                          */
    } ipsskp3_15_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp3_14_reg;              /*!< (@ 0x00001404) secure keypad storage skp3[479:448] register    */

    struct {
      __OM  uint32_t skp3_14 : 32;              /*!< [31..0] skp3[479:448]                                          */
    } ipsskp3_14_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp3_13_reg;              /*!< (@ 0x00001408) secure keypad storage skp3[447:416] register    */

    struct {
      __OM  uint32_t skp3_13 : 32;              /*!< [31..0] skp3[447:416]                                          */
    } ipsskp3_13_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp3_12_reg;              /*!< (@ 0x0000140C) secure keypad storage skp3[415:384] register    */

    struct {
      __OM  uint32_t skp3_12 : 32;              /*!< [31..0] skp3[415:384]                                          */
    } ipsskp3_12_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp3_11_reg;              /*!< (@ 0x00001410) secure keypad storage skp3[383:352] register    */

    struct {
      __OM  uint32_t skp3_11 : 32;              /*!< [31..0] skp3[383:352]                                          */
    } ipsskp3_11_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp3_10_reg;              /*!< (@ 0x00001414) secure keypad storage skp3[351:320] register    */

    struct {
      __OM  uint32_t skp3_10 : 32;              /*!< [31..0] skp3[351:320]                                          */
    } ipsskp3_10_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp3_9_reg;               /*!< (@ 0x00001418) secure keypad storage skp3[319:288] register    */

    struct {
      __OM  uint32_t skp3_9 : 32;               /*!< [31..0] skp3[319:288]                                          */
    } ipsskp3_9_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp3_8_reg;               /*!< (@ 0x00001418) secure keypad storage skp3[287:256] register    */

    struct {
      __OM  uint32_t skp3_8 : 32;               /*!< [31..0] skp3[287:256]                                          */
    } ipsskp3_8_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp3_7_reg;               /*!< (@ 0x0000141C) secure keypad storage skp3[255:224] register    */

    struct {
      __OM  uint32_t skp3_7 : 32;               /*!< [31..0] skp3[255:224]                                          */
    } ipsskp3_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp3_6_reg;               /*!< (@ 0x00001420) secure keypad storage skp3[223:192] register    */

    struct {
      __OM  uint32_t skp3_6 : 32;               /*!< [31..0] skp3[223:192]                                          */
    } ipsskp3_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp3_5_reg;               /*!< (@ 0x00001424) secure keypad storage skp3[191:160] register    */

    struct {
      __OM  uint32_t skp3_5 : 32;               /*!< [31..0] skp3[191:160]                                          */
    } ipsskp3_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp3_4_reg;               /*!< (@ 0x00001428) secure keypad storage skp3[159:128] register    */

    struct {
      __OM  uint32_t skp3_4 : 32;               /*!< [31..0] skp3[159:128]                                          */
    } ipsskp3_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp3_3_reg;               /*!< (@ 0x0000142C) secure keypad storage skp3[127:96] register     */

    struct {
      __OM  uint32_t skp3_3 : 32;               /*!< [31..0] skp3[127:96]                                           */
    } ipsskp3_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp3_2_reg;               /*!< (@ 0x00001430) secure keypad storage skp3[95:64] register      */

    struct {
      __OM  uint32_t skp3_2 : 32;               /*!< [31..0] skp3[95:64]                                            */
    } ipsskp3_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp3_1_reg;               /*!< (@ 0x00001434) secure keypad storage skp3[63:32] register      */

    struct {
      __OM  uint32_t skp3_1 : 32;               /*!< [31..0] skp3[63:32]                                            */
    } ipsskp3_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp3_0_reg;               /*!< (@ 0x0000143C) secure keypad storage skp3[31:0] register       */

    struct {
      __OM  uint32_t skp3_0 : 32;               /*!< [31..0] skp3[31:0]                                             */
    } ipsskp3_0_reg_b;
  } ;

  __IM  uint32_t  RESERVED13[48];

    union {
    __OM  uint32_t ipsskp4_15_reg;              /*!< (@ 0x00001500) secure keypad storage skp4[511:480] register    */

    struct {
      __OM  uint32_t skp4_15 : 32;              /*!< [31..0] skp4[511:480]                                          */
    } ipsskp4_15_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp4_14_reg;              /*!< (@ 0x00001504) secure keypad storage skp4[479:448] register    */

    struct {
      __OM  uint32_t skp4_14 : 32;              /*!< [31..0] skp4[479:448]                                          */
    } ipsskp4_14_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp4_13_reg;              /*!< (@ 0x00001508) secure keypad storage skp4[447:416] register    */

    struct {
      __OM  uint32_t skp4_13 : 32;              /*!< [31..0] skp4[447:416]                                          */
    } ipsskp4_13_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp4_12_reg;              /*!< (@ 0x0000150C) secure keypad storage skp4[415:384] register    */

    struct {
      __OM  uint32_t skp4_12 : 32;              /*!< [31..0] skp4[415:384]                                          */
    } ipsskp4_12_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp4_11_reg;              /*!< (@ 0x00001510) secure keypad storage skp4[383:352] register    */

    struct {
      __OM  uint32_t skp4_11 : 32;              /*!< [31..0] skp4[383:352]                                          */
    } ipsskp4_11_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp4_10_reg;              /*!< (@ 0x00001514) secure keypad storage skp4[351:320] register    */

    struct {
      __OM  uint32_t skp4_10 : 32;              /*!< [31..0] skp4[351:320]                                          */
    } ipsskp4_10_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp4_9_reg;               /*!< (@ 0x00001518) secure keypad storage skp4[319:288] register    */

    struct {
      __OM  uint32_t skp4_9 : 32;               /*!< [31..0] skp4[319:288]                                          */
    } ipsskp4_9_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp4_8_reg;               /*!< (@ 0x00001518) secure keypad storage skp4[287:256] register    */

    struct {
      __OM  uint32_t skp4_8 : 32;               /*!< [31..0] skp4[287:256]                                          */
    } ipsskp4_8_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp4_7_reg;               /*!< (@ 0x0000151C) secure keypad storage skp4[255:224] register    */

    struct {
      __OM  uint32_t skp4_7 : 32;               /*!< [31..0] skp4[255:224]                                          */
    } ipsskp4_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp4_6_reg;               /*!< (@ 0x00001520) secure keypad storage skp4[223:192] register    */

    struct {
      __OM  uint32_t skp4_6 : 32;               /*!< [31..0] skp4[223:192]                                          */
    } ipsskp4_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp4_5_reg;               /*!< (@ 0x00001524) secure keypad storage skp4[191:160] register    */

    struct {
      __OM  uint32_t skp4_5 : 32;               /*!< [31..0] skp4[191:160]                                          */
    } ipsskp4_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp4_4_reg;               /*!< (@ 0x00001528) secure keypad storage skp4[159:128] register    */

    struct {
      __OM  uint32_t skp4_4 : 32;               /*!< [31..0] skp4[159:128]                                          */
    } ipsskp4_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp4_3_reg;               /*!< (@ 0x0000152C) secure keypad storage skp4[127:96] register     */

    struct {
      __OM  uint32_t skp4_3 : 32;               /*!< [31..0] skp4[127:96]                                           */
    } ipsskp4_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp4_2_reg;               /*!< (@ 0x00001530) secure keypad storage skp4[95:64] register      */

    struct {
      __OM  uint32_t skp4_2 : 32;               /*!< [31..0] skp4[95:64]                                            */
    } ipsskp4_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp4_1_reg;               /*!< (@ 0x00001534) secure keypad storage skp4[63:32] register      */

    struct {
      __OM  uint32_t skp4_1 : 32;               /*!< [31..0] skp4[63:32]                                            */
    } ipsskp4_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp4_0_reg;               /*!< (@ 0x0000153C) secure keypad storage skp4[31:0] register       */

    struct {
      __OM  uint32_t skp4_0 : 32;               /*!< [31..0] skp4[31:0]                                             */
    } ipsskp4_0_reg_b;
  } ;

  __IM  uint32_t  RESERVED14[48];

    union {
    __OM  uint32_t ipsskp5_15_reg;              /*!< (@ 0x00001600) secure keypad storage skp5[511:480] register    */

    struct {
      __OM  uint32_t skp5_15 : 32;              /*!< [31..0] skp5[511:480]                                          */
    } ipsskp5_15_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp5_14_reg;              /*!< (@ 0x00001604) secure keypad storage skp5[479:448] register    */

    struct {
      __OM  uint32_t skp5_14 : 32;              /*!< [31..0] skp5[479:448]                                          */
    } ipsskp5_14_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp5_13_reg;              /*!< (@ 0x00001608) secure keypad storage skp5[447:416] register    */

    struct {
      __OM  uint32_t skp5_13 : 32;              /*!< [31..0] skp5[447:416]                                          */
    } ipsskp5_13_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp5_12_reg;              /*!< (@ 0x0000160C) secure keypad storage skp5[415:384] register    */

    struct {
      __OM  uint32_t skp5_12 : 32;              /*!< [31..0] skp5[415:384]                                          */
    } ipsskp5_12_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp5_11_reg;              /*!< (@ 0x00001610) secure keypad storage skp5[383:352] register    */

    struct {
      __OM  uint32_t skp5_11 : 32;              /*!< [31..0] skp5[383:352]                                          */
    } ipsskp5_11_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp5_10_reg;              /*!< (@ 0x00001614) secure keypad storage skp5[351:320] register    */

    struct {
      __OM  uint32_t skp5_10 : 32;              /*!< [31..0] skp5[351:320]                                          */
    } ipsskp5_10_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp5_9_reg;               /*!< (@ 0x00001618) secure keypad storage skp5[319:288] register    */

    struct {
      __OM  uint32_t skp5_9 : 32;               /*!< [31..0] skp5[319:288]                                          */
    } ipsskp5_9_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp5_8_reg;               /*!< (@ 0x00001618) secure keypad storage skp5[287:256] register    */

    struct {
      __OM  uint32_t skp5_8 : 32;               /*!< [31..0] skp5[287:256]                                          */
    } ipsskp5_8_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp5_7_reg;               /*!< (@ 0x0000161C) secure keypad storage skp5[255:224] register    */

    struct {
      __OM  uint32_t skp5_7 : 32;               /*!< [31..0] skp5[255:224]                                          */
    } ipsskp5_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp5_6_reg;               /*!< (@ 0x00001620) secure keypad storage skp5[223:192] register    */

    struct {
      __OM  uint32_t skp5_6 : 32;               /*!< [31..0] skp5[223:192]                                          */
    } ipsskp5_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp5_5_reg;               /*!< (@ 0x00001624) secure keypad storage skp5[191:160] register    */

    struct {
      __OM  uint32_t skp5_5 : 32;               /*!< [31..0] skp5[191:160]                                          */
    } ipsskp5_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp5_4_reg;               /*!< (@ 0x00001628) secure keypad storage skp5[159:128] register    */

    struct {
      __OM  uint32_t skp5_4 : 32;               /*!< [31..0] skp5[159:128]                                          */
    } ipsskp5_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp5_3_reg;               /*!< (@ 0x0000162C) secure keypad storage skp5[127:96] register     */

    struct {
      __OM  uint32_t skp5_3 : 32;               /*!< [31..0] skp5[127:96]                                           */
    } ipsskp5_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp5_2_reg;               /*!< (@ 0x00001630) secure keypad storage skp5[95:64] register      */

    struct {
      __OM  uint32_t skp5_2 : 32;               /*!< [31..0] skp5[95:64]                                            */
    } ipsskp5_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp5_1_reg;               /*!< (@ 0x00001634) secure keypad storage skp5[63:32] register      */

    struct {
      __OM  uint32_t skp5_1 : 32;               /*!< [31..0] skp5[63:32]                                            */
    } ipsskp5_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp5_0_reg;               /*!< (@ 0x0000163C) secure keypad storage skp5[31:0] register       */

    struct {
      __OM  uint32_t skp5_0 : 32;               /*!< [31..0] skp5[31:0]                                             */
    } ipsskp5_0_reg_b;
  } ;

  __IM  uint32_t  RESERVED15[48];

    union {
    __OM  uint32_t ipsskp6_15_reg;              /*!< (@ 0x00001700) secure keypad storage skp6[511:480] register    */

    struct {
      __OM  uint32_t skp6_15 : 32;              /*!< [31..0] skp6[511:480]                                          */
    } ipsskp6_15_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp6_14_reg;              /*!< (@ 0x00001704) secure keypad storage skp6[479:448] register    */

    struct {
      __OM  uint32_t skp6_14 : 32;              /*!< [31..0] skp6[479:448]                                          */
    } ipsskp6_14_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp6_13_reg;              /*!< (@ 0x00001708) secure keypad storage skp6[447:416] register    */

    struct {
      __OM  uint32_t skp6_13 : 32;              /*!< [31..0] skp6[447:416]                                          */
    } ipsskp6_13_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp6_12_reg;              /*!< (@ 0x0000170C) secure keypad storage skp6[415:384] register    */

    struct {
      __OM  uint32_t skp6_12 : 32;              /*!< [31..0] skp6[415:384]                                          */
    } ipsskp6_12_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp6_11_reg;              /*!< (@ 0x00001710) secure keypad storage skp6[383:352] register    */

    struct {
      __OM  uint32_t skp6_11 : 32;              /*!< [31..0] skp6[383:352]                                          */
    } ipsskp6_11_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp6_10_reg;              /*!< (@ 0x00001714) secure keypad storage skp6[351:320] register    */

    struct {
      __OM  uint32_t skp6_10 : 32;              /*!< [31..0] skp6[351:320]                                          */
    } ipsskp6_10_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp6_9_reg;               /*!< (@ 0x00001718) secure keypad storage skp6[319:288] register    */

    struct {
      __OM  uint32_t skp6_9 : 32;               /*!< [31..0] skp6[319:288]                                          */
    } ipsskp6_9_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp6_8_reg;               /*!< (@ 0x00001718) secure keypad storage skp6[287:256] register    */

    struct {
      __OM  uint32_t skp6_8 : 32;               /*!< [31..0] skp6[287:256]                                          */
    } ipsskp6_8_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp6_7_reg;               /*!< (@ 0x0000171C) secure keypad storage skp6[255:224] register    */

    struct {
      __OM  uint32_t skp6_7 : 32;               /*!< [31..0] skp6[255:224]                                          */
    } ipsskp6_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp6_6_reg;               /*!< (@ 0x00001720) secure keypad storage skp6[223:192] register    */

    struct {
      __OM  uint32_t skp6_6 : 32;               /*!< [31..0] skp6[223:192]                                          */
    } ipsskp6_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp6_5_reg;               /*!< (@ 0x00001724) secure keypad storage skp6[191:160] register    */

    struct {
      __OM  uint32_t skp6_5 : 32;               /*!< [31..0] skp6[191:160]                                          */
    } ipsskp6_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp6_4_reg;               /*!< (@ 0x00001728) secure keypad storage skp6[159:128] register    */

    struct {
      __OM  uint32_t skp6_4 : 32;               /*!< [31..0] skp6[159:128]                                          */
    } ipsskp6_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp6_3_reg;               /*!< (@ 0x0000172C) secure keypad storage skp6[127:96] register     */

    struct {
      __OM  uint32_t skp6_3 : 32;               /*!< [31..0] skp6[127:96]                                           */
    } ipsskp6_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp6_2_reg;               /*!< (@ 0x00001730) secure keypad storage skp6[95:64] register      */

    struct {
      __OM  uint32_t skp6_2 : 32;               /*!< [31..0] skp6[95:64]                                            */
    } ipsskp6_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp6_1_reg;               /*!< (@ 0x00001734) secure keypad storage skp6[63:32] register      */

    struct {
      __OM  uint32_t skp6_1 : 32;               /*!< [31..0] skp6[63:32]                                            */
    } ipsskp6_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp6_0_reg;               /*!< (@ 0x0000173C) secure keypad storage skp6[31:0] register       */

    struct {
      __OM  uint32_t skp6_0 : 32;               /*!< [31..0] skp6[31:0]                                             */
    } ipsskp6_0_reg_b;
  } ;

  __IM  uint32_t  RESERVED16[48];

    union {
    __OM  uint32_t ipsskp7_15_reg;              /*!< (@ 0x00001800) secure keypad storage skp7[511:480] register    */

    struct {
      __OM  uint32_t skp7_15 : 32;              /*!< [31..0] skp7[511:480]                                          */
    } ipsskp7_15_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp7_14_reg;              /*!< (@ 0x00001804) secure keypad storage skp7[479:448] register    */

    struct {
      __OM  uint32_t skp7_14 : 32;              /*!< [31..0] skp7[479:448]                                          */
    } ipsskp7_14_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp7_13_reg;              /*!< (@ 0x00001808) secure keypad storage skp7[447:416] register    */

    struct {
      __OM  uint32_t skp7_13 : 32;              /*!< [31..0] skp7[447:416]                                          */
    } ipsskp7_13_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp7_12_reg;              /*!< (@ 0x0000180C) secure keypad storage skp7[415:384] register    */

    struct {
      __OM  uint32_t skp7_12 : 32;              /*!< [31..0] skp7[415:384]                                          */
    } ipsskp7_12_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp7_11_reg;              /*!< (@ 0x00001810) secure keypad storage skp7[383:352] register    */

    struct {
      __OM  uint32_t skp7_11 : 32;              /*!< [31..0] skp7[383:352]                                          */
    } ipsskp7_11_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp7_10_reg;              /*!< (@ 0x00001814) secure keypad storage skp7[351:320] register    */

    struct {
      __OM  uint32_t skp7_10 : 32;              /*!< [31..0] skp7[351:320]                                          */
    } ipsskp7_10_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp7_9_reg;               /*!< (@ 0x00001818) secure keypad storage skp7[319:288] register    */

    struct {
      __OM  uint32_t skp7_9 : 32;               /*!< [31..0] skp7[319:288]                                          */
    } ipsskp7_9_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp7_8_reg;               /*!< (@ 0x00001818) secure keypad storage skp7[287:256] register    */

    struct {
      __OM  uint32_t skp7_8 : 32;               /*!< [31..0] skp7[287:256]                                          */
    } ipsskp7_8_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp7_7_reg;               /*!< (@ 0x0000181C) secure keypad storage skp7[255:224] register    */

    struct {
      __OM  uint32_t skp7_7 : 32;               /*!< [31..0] skp7[255:224]                                          */
    } ipsskp7_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp7_6_reg;               /*!< (@ 0x00001820) secure keypad storage skp7[223:192] register    */

    struct {
      __OM  uint32_t skp7_6 : 32;               /*!< [31..0] skp7[223:192]                                          */
    } ipsskp7_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp7_5_reg;               /*!< (@ 0x00001824) secure keypad storage skp7[191:160] register    */

    struct {
      __OM  uint32_t skp7_5 : 32;               /*!< [31..0] skp7[191:160]                                          */
    } ipsskp7_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp7_4_reg;               /*!< (@ 0x00001828) secure keypad storage skp7[159:128] register    */

    struct {
      __OM  uint32_t skp7_4 : 32;               /*!< [31..0] skp7[159:128]                                          */
    } ipsskp7_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp7_3_reg;               /*!< (@ 0x0000182C) secure keypad storage skp7[127:96] register     */

    struct {
      __OM  uint32_t skp7_3 : 32;               /*!< [31..0] skp7[127:96]                                           */
    } ipsskp7_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp7_2_reg;               /*!< (@ 0x00001830) secure keypad storage skp7[95:64] register      */

    struct {
      __OM  uint32_t skp7_2 : 32;               /*!< [31..0] skp7[95:64]                                            */
    } ipsskp7_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp7_1_reg;               /*!< (@ 0x00001834) secure keypad storage skp7[63:32] register      */

    struct {
      __OM  uint32_t skp7_1 : 32;               /*!< [31..0] skp7[63:32]                                            */
    } ipsskp7_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp7_0_reg;               /*!< (@ 0x0000183C) secure keypad storage skp7[31:0] register       */

    struct {
      __OM  uint32_t skp7_0 : 32;               /*!< [31..0] skp7[31:0]                                             */
    } ipsskp7_0_reg_b;
  } ;

  __IM  uint32_t  RESERVED17[48];

    union {
    __OM  uint32_t ipsskp8_15_reg;              /*!< (@ 0x00001900) secure keypad storage skp8[511:480] register    */

    struct {
      __OM  uint32_t skp8_15 : 32;              /*!< [31..0] skp8[511:480]                                          */
    } ipsskp8_15_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp8_14_reg;              /*!< (@ 0x00001904) secure keypad storage skp8[479:448] register    */

    struct {
      __OM  uint32_t skp8_14 : 32;              /*!< [31..0] skp8[479:448]                                          */
    } ipsskp8_14_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp8_13_reg;              /*!< (@ 0x00001908) secure keypad storage skp8[447:416] register    */

    struct {
      __OM  uint32_t skp8_13 : 32;              /*!< [31..0] skp8[447:416]                                          */
    } ipsskp8_13_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp8_12_reg;              /*!< (@ 0x0000190C) secure keypad storage skp8[415:384] register    */

    struct {
      __OM  uint32_t skp8_12 : 32;              /*!< [31..0] skp8[415:384]                                          */
    } ipsskp8_12_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp8_11_reg;              /*!< (@ 0x00001910) secure keypad storage skp8[383:352] register    */

    struct {
      __OM  uint32_t skp8_11 : 32;              /*!< [31..0] skp8[383:352]                                          */
    } ipsskp8_11_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp8_10_reg;              /*!< (@ 0x00001914) secure keypad storage skp8[351:320] register    */

    struct {
      __OM  uint32_t skp8_10 : 32;              /*!< [31..0] skp8[351:320]                                          */
    } ipsskp8_10_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp8_9_reg;               /*!< (@ 0x00001918) secure keypad storage skp8[319:288] register    */

    struct {
      __OM  uint32_t skp8_9 : 32;               /*!< [31..0] skp8[319:288]                                          */
    } ipsskp8_9_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp8_8_reg;               /*!< (@ 0x00001918) secure keypad storage skp8[287:256] register    */

    struct {
      __OM  uint32_t skp8_8 : 32;               /*!< [31..0] skp8[287:256]                                          */
    } ipsskp8_8_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp8_7_reg;               /*!< (@ 0x0000191C) secure keypad storage skp8[255:224] register    */

    struct {
      __OM  uint32_t skp8_7 : 32;               /*!< [31..0] skp8[255:224]                                          */
    } ipsskp8_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp8_6_reg;               /*!< (@ 0x00001920) secure keypad storage skp8[223:192] register    */

    struct {
      __OM  uint32_t skp8_6 : 32;               /*!< [31..0] skp8[223:192]                                          */
    } ipsskp8_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp8_5_reg;               /*!< (@ 0x00001924) secure keypad storage skp8[191:160] register    */

    struct {
      __OM  uint32_t skp8_5 : 32;               /*!< [31..0] skp8[191:160]                                          */
    } ipsskp8_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp8_4_reg;               /*!< (@ 0x00001928) secure keypad storage skp8[159:128] register    */

    struct {
      __OM  uint32_t skp8_4 : 32;               /*!< [31..0] skp8[159:128]                                          */
    } ipsskp8_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp8_3_reg;               /*!< (@ 0x0000192C) secure keypad storage skp8[127:96] register     */

    struct {
      __OM  uint32_t skp8_3 : 32;               /*!< [31..0] skp8[127:96]                                           */
    } ipsskp8_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp8_2_reg;               /*!< (@ 0x00001930) secure keypad storage skp8[95:64] register      */

    struct {
      __OM  uint32_t skp8_2 : 32;               /*!< [31..0] skp8[95:64]                                            */
    } ipsskp8_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp8_1_reg;               /*!< (@ 0x00001934) secure keypad storage skp8[63:32] register      */

    struct {
      __OM  uint32_t skp8_1 : 32;               /*!< [31..0] skp8[63:32]                                            */
    } ipsskp8_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp8_0_reg;               /*!< (@ 0x0000193C) secure keypad storage skp8[31:0] register       */

    struct {
      __OM  uint32_t skp8_0 : 32;               /*!< [31..0] skp8[31:0]                                             */
    } ipsskp8_0_reg_b;
  } ;

  __IM  uint32_t  RESERVED18[48];

    union {
    __OM  uint32_t ipsskp9_15_reg;              /*!< (@ 0x00001A00) secure keypad storage skp9[511:480] register    */

    struct {
      __OM  uint32_t skp9_15 : 32;              /*!< [31..0] skp9[511:480]                                          */
    } ipsskp9_15_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp9_14_reg;              /*!< (@ 0x00001A04) secure keypad storage skp9[479:448] register    */

    struct {
      __OM  uint32_t skp9_14 : 32;              /*!< [31..0] skp9[479:448]                                          */
    } ipsskp9_14_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp9_13_reg;              /*!< (@ 0x00001A08) secure keypad storage skp9[447:416] register    */

    struct {
      __OM  uint32_t skp9_13 : 32;              /*!< [31..0] skp9[447:416]                                          */
    } ipsskp9_13_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp9_12_reg;              /*!< (@ 0x00001A0C) secure keypad storage skp9[415:384] register    */

    struct {
      __OM  uint32_t skp9_12 : 32;              /*!< [31..0] skp9[415:384]                                          */
    } ipsskp9_12_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp9_11_reg;              /*!< (@ 0x00001A10) secure keypad storage skp9[383:352] register    */

    struct {
      __OM  uint32_t skp9_11 : 32;              /*!< [31..0] skp9[383:352]                                          */
    } ipsskp9_11_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp9_10_reg;              /*!< (@ 0x00001A14) secure keypad storage skp9[351:320] register    */

    struct {
      __OM  uint32_t skp9_10 : 32;              /*!< [31..0] skp9[351:320]                                          */
    } ipsskp9_10_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp9_9_reg;               /*!< (@ 0x00001A18) secure keypad storage skp9[319:288] register    */

    struct {
      __OM  uint32_t skp9_9 : 32;               /*!< [31..0] skp9[319:288]                                          */
    } ipsskp9_9_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp9_8_reg;               /*!< (@ 0x00001A18) secure keypad storage skp9[287:256] register    */

    struct {
      __OM  uint32_t skp9_8 : 32;               /*!< [31..0] skp9[287:256]                                          */
    } ipsskp9_8_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp9_7_reg;               /*!< (@ 0x00001A1C) secure keypad storage skp9[255:224] register    */

    struct {
      __OM  uint32_t skp9_7 : 32;               /*!< [31..0] skp9[255:224]                                          */
    } ipsskp9_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp9_6_reg;               /*!< (@ 0x00001A20) secure keypad storage skp9[223:192] register    */

    struct {
      __OM  uint32_t skp9_6 : 32;               /*!< [31..0] skp9[223:192]                                          */
    } ipsskp9_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp9_5_reg;               /*!< (@ 0x00001A24) secure keypad storage skp9[191:160] register    */

    struct {
      __OM  uint32_t skp9_5 : 32;               /*!< [31..0] skp9[191:160]                                          */
    } ipsskp9_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp9_4_reg;               /*!< (@ 0x00001A28) secure keypad storage skp9[159:128] register    */

    struct {
      __OM  uint32_t skp9_4 : 32;               /*!< [31..0] skp9[159:128]                                          */
    } ipsskp9_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp9_3_reg;               /*!< (@ 0x00001A2C) secure keypad storage skp9[127:96] register     */

    struct {
      __OM  uint32_t skp9_3 : 32;               /*!< [31..0] skp9[127:96]                                           */
    } ipsskp9_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp9_2_reg;               /*!< (@ 0x00001A30) secure keypad storage skp9[95:64] register      */

    struct {
      __OM  uint32_t skp9_2 : 32;               /*!< [31..0] skp9[95:64]                                            */
    } ipsskp9_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp9_1_reg;               /*!< (@ 0x00001A34) secure keypad storage skp9[63:32] register      */

    struct {
      __OM  uint32_t skp9_1 : 32;               /*!< [31..0] skp9[63:32]                                            */
    } ipsskp9_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp9_0_reg;               /*!< (@ 0x00001A3C) secure keypad storage skp9[31:0] register       */

    struct {
      __OM  uint32_t skp9_0 : 32;               /*!< [31..0] skp9[31:0]                                             */
    } ipsskp9_0_reg_b;
  } ;

  __IM  uint32_t  RESERVED19[48];

    union {
    __OM  uint32_t ipsskp10_15_reg;             /*!< (@ 0x00001B00) secure keypad storage skp10[511:480] register   */

    struct {
      __OM  uint32_t skp10_15 : 32;             /*!< [31..0] skp10[511:480]                                         */
    } ipsskp10_15_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp10_14_reg;             /*!< (@ 0x00001B04) secure keypad storage skp10[479:448] register   */

    struct {
      __OM  uint32_t skp10_14 : 32;             /*!< [31..0] skp10[479:448]                                         */
    } ipsskp10_14_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp10_13_reg;             /*!< (@ 0x00001B08) secure keypad storage skp10[447:416] register   */

    struct {
      __OM  uint32_t skp10_13 : 32;             /*!< [31..0] skp10[447:416]                                         */
    } ipsskp10_13_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp10_12_reg;             /*!< (@ 0x00001B0C) secure keypad storage skp10[415:384] register   */

    struct {
      __OM  uint32_t skp10_12 : 32;             /*!< [31..0] skp10[415:384]                                         */
    } ipsskp10_12_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp10_11_reg;             /*!< (@ 0x00001B10) secure keypad storage skp10[383:352] register   */

    struct {
      __OM  uint32_t skp10_11 : 32;             /*!< [31..0] skp10[383:352]                                         */
    } ipsskp10_11_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp10_10_reg;             /*!< (@ 0x00001B14) secure keypad storage skp10[351:320] register   */

    struct {
      __OM  uint32_t skp10_10 : 32;             /*!< [31..0] skp10[351:320]                                         */
    } ipsskp10_10_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp10_9_reg;              /*!< (@ 0x00001B18) secure keypad storage skp10[319:288] register   */

    struct {
      __OM  uint32_t skp10_9 : 32;              /*!< [31..0] skp10[319:288]                                         */
    } ipsskp10_9_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp10_8_reg;              /*!< (@ 0x00001B18) secure keypad storage skp10[287:256] register   */

    struct {
      __OM  uint32_t skp10_8 : 32;              /*!< [31..0] skp10[287:256]                                         */
    } ipsskp10_8_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp10_7_reg;              /*!< (@ 0x00001B1C) secure keypad storage skp10[255:224] register   */

    struct {
      __OM  uint32_t skp10_7 : 32;              /*!< [31..0] skp10[255:224]                                         */
    } ipsskp10_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp10_6_reg;              /*!< (@ 0x00001B20) secure keypad storage skp10[223:192] register   */

    struct {
      __OM  uint32_t skp10_6 : 32;              /*!< [31..0] skp10[223:192]                                         */
    } ipsskp10_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp10_5_reg;              /*!< (@ 0x00001B24) secure keypad storage skp10[191:160] register   */

    struct {
      __OM  uint32_t skp10_5 : 32;              /*!< [31..0] skp10[191:160]                                         */
    } ipsskp10_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp10_4_reg;              /*!< (@ 0x00001B28) secure keypad storage skp10[159:128] register   */

    struct {
      __OM  uint32_t skp10_4 : 32;              /*!< [31..0] skp10[159:128]                                         */
    } ipsskp10_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp10_3_reg;              /*!< (@ 0x00001B2C) secure keypad storage skp10[127:96] register    */

    struct {
      __OM  uint32_t skp10_3 : 32;              /*!< [31..0] skp10[127:96]                                          */
    } ipsskp10_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp10_2_reg;              /*!< (@ 0x00001B30) secure keypad storage skp10[95:64] register     */

    struct {
      __OM  uint32_t skp10_2 : 32;              /*!< [31..0] skp10[95:64]                                           */
    } ipsskp10_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp10_1_reg;              /*!< (@ 0x00001B34) secure keypad storage skp10[63:32] register     */

    struct {
      __OM  uint32_t skp10_1 : 32;              /*!< [31..0] skp10[63:32]                                           */
    } ipsskp10_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp10_0_reg;              /*!< (@ 0x00001B3C) secure keypad storage skp10[31:0] register      */

    struct {
      __OM  uint32_t skp10_0 : 32;              /*!< [31..0] skp10[31:0]                                            */
    } ipsskp10_0_reg_b;
  } ;

  __IM  uint32_t  RESERVED20[48];

    union {
    __OM  uint32_t ipsskp11_15_reg;             /*!< (@ 0x00001C00) secure keypad storage skp11[511:480] register   */

    struct {
      __OM  uint32_t skp11_15 : 32;             /*!< [31..0] skp11[511:480]                                         */
    } ipsskp11_15_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp11_14_reg;             /*!< (@ 0x00001C04) secure keypad storage skp11[479:448] register   */

    struct {
      __OM  uint32_t skp11_14 : 32;             /*!< [31..0] skp11[479:448]                                         */
    } ipsskp11_14_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp11_13_reg;             /*!< (@ 0x00001C08) secure keypad storage skp11[447:416] register   */

    struct {
      __OM  uint32_t skp11_13 : 32;             /*!< [31..0] skp11[447:416]                                         */
    } ipsskp11_13_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp11_12_reg;             /*!< (@ 0x00001C0C) secure keypad storage skp11[415:384] register   */

    struct {
      __OM  uint32_t skp11_12 : 32;             /*!< [31..0] skp11[415:384]                                         */
    } ipsskp11_12_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp11_11_reg;             /*!< (@ 0x00001C10) secure keypad storage skp11[383:352] register   */

    struct {
      __OM  uint32_t skp11_11 : 32;             /*!< [31..0] skp11[383:352]                                         */
    } ipsskp11_11_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp11_10_reg;             /*!< (@ 0x00001C14) secure keypad storage skp11[351:320] register   */

    struct {
      __OM  uint32_t skp11_10 : 32;             /*!< [31..0] skp11[351:320]                                         */
    } ipsskp11_10_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp11_9_reg;              /*!< (@ 0x00001C18) secure keypad storage skp11[319:288] register   */

    struct {
      __OM  uint32_t skp11_9 : 32;              /*!< [31..0] skp11[319:288]                                         */
    } ipsskp11_9_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp11_8_reg;              /*!< (@ 0x00001C18) secure keypad storage skp11[287:256] register   */

    struct {
      __OM  uint32_t skp11_8 : 32;              /*!< [31..0] skp11[287:256]                                         */
    } ipsskp11_8_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp11_7_reg;              /*!< (@ 0x00001C1C) secure keypad storage skp11[255:224] register   */

    struct {
      __OM  uint32_t skp11_7 : 32;              /*!< [31..0] skp11[255:224]                                         */
    } ipsskp11_7_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp11_6_reg;              /*!< (@ 0x00001C20) secure keypad storage skp11[223:192] register   */

    struct {
      __OM  uint32_t skp11_6 : 32;              /*!< [31..0] skp11[223:192]                                         */
    } ipsskp11_6_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp11_5_reg;              /*!< (@ 0x00001C24) secure keypad storage skp11[191:160] register   */

    struct {
      __OM  uint32_t skp11_5 : 32;              /*!< [31..0] skp11[191:160]                                         */
    } ipsskp11_5_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp11_4_reg;              /*!< (@ 0x00001C28) secure keypad storage skp11[159:128] register   */

    struct {
      __OM  uint32_t skp11_4 : 32;              /*!< [31..0] skp11[159:128]                                         */
    } ipsskp11_4_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp11_3_reg;              /*!< (@ 0x00001C2C) secure keypad storage skp11[127:96] register    */

    struct {
      __OM  uint32_t skp11_3 : 32;              /*!< [31..0] skp11[127:96]                                          */
    } ipsskp11_3_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp11_2_reg;              /*!< (@ 0x00001C30) secure keypad storage skp11[95:64] register     */

    struct {
      __OM  uint32_t skp11_2 : 32;              /*!< [31..0] skp11[95:64]                                           */
    } ipsskp11_2_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp11_1_reg;              /*!< (@ 0x00001C34) secure keypad storage skp11[63:32] register     */

    struct {
      __OM  uint32_t skp11_1 : 32;              /*!< [31..0] skp11[63:32]                                           */
    } ipsskp11_1_reg_b;
  } ;

  union {
    __OM  uint32_t ipsskp11_0_reg;              /*!< (@ 0x00001C3C) secure keypad storage skp11[31:0] register      */

    struct {
      __OM  uint32_t skp11_0 : 32;              /*!< [31..0] skp11[31:0]                                            */
    } ipsskp11_0_reg_b;
  } ;
#endif

  __IM  uint32_t  RESERVED11[48];

} CRYPTO_Type;                                  /*!< Size = 7424 (0x1D00)                    */

/** @} */ /* End of group crypto_device_reg */

//#define KEY_STORAGE_VENDOR_KEY_NUM  2
//#define KEY_STORAGE_EMPTY_KEY_NUM   (12 - KEY_STORAGE_VENDOR_KEY_NUM)

/* ================================================================================================================ */
/* ================                          Device Specific Peripheral Address Map                           ===== */
/* ================================================================================================================ */


/** @addtogroup device_crypto_addr Crypto Engine Register Base Address
 *  @ingroup hs_hal_crypto
 *  @{
 */

#define CRYPTO_S_BASE                 0x90500000UL
#define CRYPTO_NS_BASE                0x40070000UL

/** @} */ /* End of group device_crypto_addr */


/* ================================================================================================================ */
/* ================                                  Peripheral declaration                                   ===== */
/* ================================================================================================================ */


/** @addtogroup device_crypto_decl Crypto Engine Device Type
 *  @ingroup hs_hal_crypto
 *  @{
 */

#define CRYPTO_S_MODULE                         ((CRYPTO_Type *)            CRYPTO_S_BASE)
#define CRYPTO_NS_MODULE                        ((CRYPTO_Type *)            CRYPTO_NS_BASE)

#define cache_alinge	64

/** @} */ /* End of group device_crypto_decl */

/// @endcond /* End of condition DOXYGEN_CRYPTO_REG_TYPE */
#ifdef __cplusplus
}
#endif

#endif /* __RTK_CRYPTO_BASE_H__ */

/** @} */ /* End of group hs_hal_crypto */


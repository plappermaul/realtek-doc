/* --------------------------------------------------------------------
**
**  This software and the associated documentation are confidential
**  and proprietary to Synopsys Inc.  Your use or disclosure of this
**  software is subject to the terms and conditions of a written
**  license agreement between you, or your company, and Synopsys Inc.
**  In the event of publication, the following notice is applicable:
**
**                 COPYRIGHT (C) 2004 SYNOPSYS INC.
**                       ALL RIGHTS RESERVED
**
** The entire notice above must be reproduced on all authorized copies
**
** --------------------------------------------------------------------
**
** File     : DW_common_types.h
** Created  : Fri Aug 15 17:23:24 IST 2003
** Modified : $Date: 2005/04/06 $
** Abstract : This header contains the various data type definitions
**            and structures.
**
** --------------------------------------------------------------------
*/



#ifndef DW_COMMON_TYPES_H
#define DW_COMMON_TYPES_H

#ifdef __cplusplus
extern "C" {    // allow C++ to use these headers
#endif

#ifndef TRUE
#define TRUE    (1)
#endif      // TRUE
#ifndef true
#define true    TRUE
#endif      // true

#ifndef FALSE
#define FALSE   (0)
#endif      // FALSE
#ifndef false
#define false   FALSE
#endif      // false

#ifndef EOF
#define EOF     (-1)
#endif      // EOF

#ifndef __STDBOOL_H__
//typedef unsigned char bool;
#endif

#ifndef MIN
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#endif

/****d* include.types/dw_state
 * NAME
 *  dw_state
 * DESCRIPTION
 *  This is a generic data type used for 1-bit wide bitfields which have
 *  a "set/clear" property.  This is used when modifying registers
 *  within a peripheral's memory map.
 * SOURCE
 */
enum dw_state {
    Dw_err = -1,
    Dw_clear = 0,
    Dw_set = 1
};
/*****/

/****d* include.types/dw_comp_type
 * NAME
 *  dw_comp_type -- component type identification numbers
 * DESCRIPTION
 *  This data type is used to identify peripheral types.  These values
 *  are typically encoded in a peripheral's portmap and are also used in
 *  the dw_device structure.
 * SEE ALSO
 *  dw_device
 * SOURCE
 */
enum dw_comp_type {
    Dw_dev_none     = 0x00000000,
    Dw_memctl       = 0x44572110,
    Dw_ahb_dmac     = 0x44571110,
    Dw_ahb_ictl     = 0x44571120,
    Dw_ahb_pci      = 0x44571130,
    Dw_ahb_usb      = 0x44571140,
    Dw_apb_gpio     = 0x44570160,
    Dw_apb_i2c      = 0x44570140,
    Dw_apb_ictl     = 0x44570180,
    Dw_apb_rap      = 0x44570190,
    Dw_apb_rtc      = 0x44570130,
    Dw_apb_ssi      = 0x44570150,
    Dw_apb_timers   = 0x44570170,
    Dw_apb_uart     = 0x44570110,
    Dw_apb_wdt      = 0x44570120
};
/*****/

/****s* include.types/dw_device
 * NAME
 *  dw_device -- low-level device handle structure
 * SYNOPSIS
 *  This is the handle used to identify and manipulate all DesignWare
 *  peripherals.
 * DESCRIPTION
 *  This is the primary structure used when dealing with all devices.
 *  It serves as a hardware abstraction layer for driver code and also
 *  allows this code to support more than one device of the same type
 *  simultaneously.  This structure needs to be initialized with
 *  meaningful values before a pointer to it is passed to a driver
 *  initialization function.
 * PARAMETERS
 *  name            name of device
 *  dataWidth       bus data width of the device
 *  baseAddress     physical base address of device
 *  instance        device private data structure pointer
 *  os              unused pointer for associating with an OS structure
 *  compParam       pointer to structure containing device's
 *                  coreConsultant configuration parameters structure
 *  compVersion     device version identification number
 *  compType        device identification number
 ***/
struct dw_device {
    const char *name;
    unsigned data_width;
    void *base_address;
    void *instance;
    void *os;
    void *comp_param;
    //uint32_t comp_version;
    unsigned int comp_version;
    enum dw_comp_type comp_type;
    struct dw_list_head list;
};

/****d* include.types/dw_callback
 * DESCRIPTION
 *  This is a generic data type used for handling callback functions
 *  with each driver.
 * ARGUMENTS
 *  dev         -- device handle
 *  eCode       -- event/error code
 * NOTES
 *  The usage of the eCode argument is typically negative for an error
 *  code and positive for an event code.
 * SOURCE
 */
//typedef void (*dw_callback)(struct dw_device *dev, int32_t eCode);
typedef void (*dw_callback)(struct dw_device *dev, int eCode);
/*****/

/****d* include.types/dw_dma_notifier_func
 * DESCRIPTION
 *  This is the data type used for specifying DMA notifier functions.
 *  These are needed when software handshaking is used with peripheral
 *  DMA transfers in order to inform the DMA controller when data is
 *  ready to be sent/received.
 * ARGUMENTS
 *  dev         -- DMA device handle
 *  channel     -- associated channel number
 *  single      -- single or burst transfer?
 *  last        -- is this the last block?
 * NOTES
 *  The single and last arguments are only necessary when the peripheral
 *  involved is also acting as the flow controller.
 * SOURCE
 */

#ifndef bool
#define bool int
#endif

typedef void (*dw_dma_notifier_func)(struct dw_device *dev, unsigned
        channel, bool single, bool last);
/*****/

/****d* include.data/dw_dma_mode
 * DESCRIPTION
 *  This is the data type used for enabling software or hardware
 *  handshaking for DMA transfers.  Using software handshaking changes
 *  how an interrupt handler processes Rx full and Tx empty interrupts.
 *  Any DesignWare peripheral which supports DMA transfers has API
 *  function which match those listed below.
 * SEE ALSO
 *  dw_*_setDmaTxMode(), dw_*_setDmaRxMode(), dw_*_getDmaTxMode(),
 *  dw_*_getDmaRxMode(), dw_*_setDmaTxNotifier(),
 *  dw_*_setDmaTxNotifier()
 * SOURCE
 */
enum dw_dma_mode {
    Dw_dma_none,            // DMA is not being used
    Dw_dma_sw_handshake,    // DMA using software handshaking
    Dw_dma_hw_handshake     // DMA using hardware handshaking
};
/*****/

/****s* include.api/dw_dma_notify
 * DESCRIPTION
 *  This is the data structure used to store a DMA notifier function
 *  and its related arguments.
 * SOURCE
 */
struct dw_dma_config {
    enum dw_dma_mode mode;
    dw_dma_notifier_func notifier;
    struct dw_device *dmac;
    unsigned channel;
};
/*****/

#ifdef __cplusplus
}
#endif

#endif      // DW_COMMON_TYPES_H


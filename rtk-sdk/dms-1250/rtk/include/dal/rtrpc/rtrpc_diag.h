/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_DIAG_H__
#define __RTRPC_DIAG_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#include <hal/mac/reg.h>
#include <rtk/diag.h>
#include <private/drv/nic/nic_diag.h>
#include <drv/gpio/ext_gpio.h>
#include <drv/gpio/generalCtrl_gpio.h>
#include <drv/i2c/i2c.h>
#include <drv/spi/spi.h>
#include <private/drv/spi/spi_private.h>



#define ioal_mem32_read                    rtrpc_ioal_mem32_read
#define ioal_mem32_write                   rtrpc_ioal_mem32_write
#define reg_idx2Addr_get                   rtrpc_reg_idx2Addr_get
#define reg_idxMax_get                     rtrpc_reg_idxMax_get
#define debug_mem_read                     rtrpc_debug_mem_read
#define debug_mem_write                    rtrpc_debug_mem_write
#define drv_nic_dbg_get                    rtrpc_nic_dbg_get
#define drv_nic_dbg_set                    rtrpc_nic_dbg_set
#define drv_nic_cntr_dump                  rtrpc_nic_cntr_dump
#define drv_nic_cntr_clear                 rtrpc_nic_cntr_clear
#define drv_nic_ringbuf_dump               rtrpc_nic_ringbuf_dump
#define drv_nic_pktHdrMBuf_dump            rtrpc_nic_pktHdrMBuf_dump
#define drv_nic_rx_status_get              rtrpc_nic_rx_status_get
#define drv_nic_tag_set                    rtrpc_nic_tag_set
#define drv_nic_txData_set                 rtrpc_nic_txData_set
#define drv_nic_diagPkt_send               rtrpc_nic_diagPkt_send
#define table_write                        rtrpc_table_write
#define table_read                         rtrpc_table_read
#define reg_info_get                       rtrpc_reg_info_get
#if defined(CONFIG_SDK_RTL8231)
#define drv_rtl8231_i2c_read               rtrpc_rtl8231_i2c_read
#define drv_rtl8231_i2c_write              rtrpc_rtl8231_i2c_write
#define drv_rtl8231_mdc_read               rtrpc_rtl8231_mdc_read
#define drv_rtl8231_mdc_write              rtrpc_rtl8231_mdc_write
#define drv_extGpio_devReady_get           rtrpc_extGpio_devReady_get
#define drv_extGpio_dev_get                rtrpc_extGpio_dev_get
#define drv_extGpio_dev_init               rtrpc_extGpio_dev_init
#define drv_extGpio_devEnable_get          rtrpc_extGpio_devEnable_get
#define drv_extGpio_devEnable_set          rtrpc_extGpio_devEnable_set
#define drv_extGpio_syncEnable_get         rtrpc_extGpio_syncEnable_get
#define drv_extGpio_syncEnable_set         rtrpc_extGpio_syncEnable_set
#define drv_extGpio_syncStatus_get         rtrpc_extGpio_syncStatus_get
#define drv_extGpio_i2c_read               rtrpc_extGpio_i2c_read
#define drv_extGpio_i2c_write              rtrpc_extGpio_i2c_write
#define drv_extGpio_i2c_init               rtrpc_extGpio_i2c_init
#define drv_extGpio_sync_start             rtrpc_extGpio_sync_start
#define drv_extGpio_pin_get                rtrpc_extGpio_pin_get
#define drv_extGpio_pin_init               rtrpc_extGpio_pin_init
#define drv_extGpio_dataBit_get            rtrpc_extGpio_dataBit_get
#define drv_extGpio_dataBit_set            rtrpc_extGpio_dataBit_set
#define drv_extGpio_reg_read               rtrpc_extGpio_reg_read
#define drv_extGpio_reg_write              rtrpc_extGpio_reg_write
#define drv_extGpio_direction_get          rtrpc_extGpio_direction_get
#define drv_extGpio_direction_set          rtrpc_extGpio_direction_set
#endif //#if defined(CONFIG_SDK_RTL8231)
#define drv_gpio_pin_init                  rtrpc_gpio_pin_init
#define drv_gpio_dataBit_init              rtrpc_gpio_dataBit_init
#define drv_gpio_dataBit_get               rtrpc_gpio_dataBit_get
#define drv_gpio_dataBit_set               rtrpc_gpio_dataBit_set
#define drv_generalCtrlGPIO_dev_init       rtrpc_generalCtrlGPIO_dev_init
#define drv_generalCtrlGPIO_dataBit_set    rtrpc_generalCtrlGPIO_dataBit_set
#define drv_generalCtrlGPIO_dataBit_get    rtrpc_generalCtrlGPIO_dataBit_get
#define drv_generalCtrlGPIO_pin_init       rtrpc_generalCtrlGPIO_pin_init
#define drv_generalCtrlGPIO_devEnable_set  rtrpc_generalCtrlGPIO_devEnable_set
#define drv_i2c_init                       rtrpc_i2c_init
#define drv_i2c_dev_init                   rtrpc_i2c_dev_init
#define drv_i2c_write                      rtrpc_i2c_write
#define drv_i2c_read                       rtrpc_i2c_read
#define drv_spiPin_init                    rtrpc_spiPin_init
#define drv_spi_write                      rtrpc_spi_write
#define drv_spi_read                       rtrpc_spi_read
#define rtk_diag_portRtctResult_get        rtrpc_diag_portRtctResult_get
#define rtk_diag_rtctEnable_set            rtrpc_diag_rtctEnable_set
#define rtk_diag_table_whole_read          rtrpc_diag_table_whole_read
#define rtk_diag_table_index_name          rtrpc_diag_table_index_name
#define rtk_diag_tableEntry_read           rtrpc_diag_tableEntry_read
#define rtk_diag_tableEntryDatareg_write   rtrpc_diag_tableEntryDatareg_write
#define rtk_diag_tableEntry_write          rtrpc_diag_tableEntry_write
#define rtk_diag_peripheral_register_dump  rtrpc_diag_peripheral_register_dump
#define rtk_diag_reg_whole_read            rtrpc_diag_reg_whole_read
#define rtk_diag_phy_reg_whole_read        rtrpc_diag_phy_reg_whole_read
#define rtk_diag_table_reg_field_get       rtrpc_diag_table_reg_field_get
#define rtk_diag_table_reg_field_set       rtrpc_diag_table_reg_field_set

// REG get
#define rtk_diag_reg_get                   rtrpc_diag_reg_get
#define rtk_diag_regField_get              rtrpc_diag_regField_get
#define rtk_diag_regArray_get              rtrpc_diag_regArray_get
#define rtk_diag_regArrayField_get         rtrpc_diag_regArrayField_get
// REG set
#define rtk_diag_reg_set                   rtrpc_diag_reg_set
#define rtk_diag_regField_set              rtrpc_diag_regField_set
#define rtk_diag_regArray_set              rtrpc_diag_regArray_set
#define rtk_diag_regArrayField_set         rtrpc_diag_regArrayField_set
// REG/Field info get
#if defined (CONFIG_SDK_DUMP_REG_WITH_NAME)
#define rtk_diag_regInfoByStr_get          rtrpc_diag_regInfoByStr_get
#define rtk_diag_regFieldInfo_get          rtrpc_diag_regFieldInfo_get
#define rtk_diag_regInfoByStr_match        rtrpc_diag_regInfoByStr_match
#endif

// TAB/Field info get
#if defined (CONFIG_SDK_DUMP_REG_WITH_NAME)
#define rtk_diag_tableInfoByStr_get rtrpc_diag_tableInfoByStr_get
#define rtk_diag_tableFieldInfo_get rtrpc_diag_tableFieldInfo_get
#define rtk_diag_tableInfoByStr_match rtrpc_diag_tableInfoByStr_match
#endif
#define rtk_diag_tableEntry_get     rtrpc_diag_tableEntry_get
#define rtk_diag_tableEntry_set     rtrpc_diag_tableEntry_set



/*
 * Function Declaration
 */

int32 rtrpc_ioal_mem32_read(uint32 unit, uint32 reg, uint32 *pValue);
int32 rtrpc_ioal_mem32_write(uint32 unit, uint32 reg, uint32 value);
int32 rtrpc_reg_idx2Addr_get(uint32 unit, uint32 regIdx, uint32 *pAddr);
int32 rtrpc_reg_idxMax_get(uint32 unit, uint32 *pMax);
int32 rtrpc_debug_mem_read(uint32 unit, uint32 addr, uint32 *pValue);
int32 rtrpc_debug_mem_write(uint32 unit, uint32 addr, uint32 value);
int32 rtrpc_nic_dbg_get(uint32 unit, uint32 *pFlags);
int32 rtrpc_nic_dbg_set(uint32 unit, uint32 flags);
int32 rtrpc_nic_cntr_dump(uint32 unit);
int32 rtrpc_nic_cntr_clear(uint32 unit);
int32 rtrpc_nic_ringbuf_dump(uint32 unit, nic_dir_t direct);
int32 rtrpc_nic_pktHdrMBuf_dump(uint32 unit, uint32 mode, uint32 start, uint32 end, uint32 flags);
int32 rtrpc_nic_rx_status_get(uint32 unit, uint32 *pStatus);
int32 rtrpc_nic_tag_set(uint32 unit, nic_txTagStatus_t tagStatus, uint8 *pTxTag, rtk_portmask_t *pPortmask);
int32 rtrpc_nic_txData_set(uint32 unit, uint8 isAuto, uint8 *pTxData, uint32 len);
int32 rtrpc_nic_diagPkt_send(uint32 unit, uint32 num);
int32 rtrpc_table_write( uint32 unit, uint32 table, uint32 addr, uint32 *pData);
int32 rtrpc_table_read( uint32 unit, uint32 table, uint32 addr, uint32 *pData);
int32 rtrpc_reg_info_get(uint32 unit, uint32 regIdx, rtk_reg_info_t *pData);
#if defined(CONFIG_SDK_RTL8231)
int32 rtrpc_rtl8231_i2c_read(uint32 unit, uint32 slave_addr, uint32 reg_addr, uint32 *pData);
int32 rtrpc_rtl8231_i2c_write(uint32 unit, uint32 slave_addr, uint32 reg_addr, uint32 data);
int32 rtrpc_rtl8231_mdc_read(uint32 unit, uint32 phy_id, uint32 page, uint32 reg_addr, uint32 *pData);
int32 rtrpc_rtl8231_mdc_write(uint32 unit, uint32 phy_id, uint32 page, uint32 reg_addr, uint32 data);
int32 rtrpc_extGpio_devReady_get(uint32 unit, uint32 dev, uint32 *pIsReady);
int32 rtrpc_extGpio_dev_get(uint32 unit, uint32 dev, drv_extGpio_devConf_t *pData);
int32 rtrpc_extGpio_dev_init(uint32 unit, uint32 dev, drv_extGpio_devConf_t *pData);
int32 rtrpc_extGpio_devEnable_get(uint32 unit, uint32 dev, rtk_enable_t *pEnable);
int32 rtrpc_extGpio_devEnable_set(uint32 unit, uint32 dev, rtk_enable_t enable);
int32 rtrpc_extGpio_syncEnable_get(uint32 unit, uint32 dev, rtk_enable_t *pEnable);
int32 rtrpc_extGpio_syncEnable_set(uint32 unit, uint32 dev, rtk_enable_t enable);
int32 rtrpc_extGpio_syncStatus_get(uint32 unit, uint32 dev, uint32 *pData);
int32 rtrpc_extGpio_i2c_read(uint32 unit, uint32 dev, uint32 reg, uint32 *pData);
int32 rtrpc_extGpio_i2c_write(uint32 unit, uint32 dev, uint32 reg, uint32 data);
int32 rtrpc_extGpio_i2c_init(uint32 unit, uint32 dev, uint32 i2c_clock, uint32 i2c_data);
int32 rtrpc_extGpio_sync_start(uint32 unit, uint32 dev);
int32 rtrpc_extGpio_pin_get(uint32 unit, uint32 dev, uint32 gpioId, drv_extGpio_conf_t *pData);
int32 rtrpc_extGpio_pin_init(uint32 unit, uint32 dev, uint32 gpioId, drv_extGpio_conf_t *pData);
int32 rtrpc_extGpio_dataBit_get(uint32 unit, uint32 dev, uint32 gpioId, uint32 *pData);
int32 rtrpc_extGpio_dataBit_set(uint32 unit, uint32 dev, uint32 gpioId, uint32 data);
int32 rtrpc_extGpio_reg_read(uint32 unit, uint32 dev, uint32 reg, uint32 *pData);
int32 rtrpc_extGpio_reg_write(uint32 unit, uint32 dev, uint32 reg, uint32 data);
int32 rtrpc_extGpio_direction_get(uint32 unit, uint32 dev, uint32 gpioId, drv_gpio_direction_t *pData);
int32 rtrpc_extGpio_direction_set(uint32 unit, uint32 dev, uint32 gpioId, drv_gpio_direction_t data);
#endif /* CONFIG_SDK_RTL8231 */
int32 rtrpc_gpio_pin_init( uint32 unit, gpioID gpioId, drv_gpio_control_t function, drv_gpio_direction_t direction, drv_gpio_interruptType_t interruptEnable);
int32 rtrpc_gpio_dataBit_init(uint32 unit, gpioID gpioId, uint32 data);
int32 rtrpc_gpio_dataBit_get(uint32 unit, gpioID gpioId, uint32 *pData);
int32 rtrpc_gpio_dataBit_set(uint32 unit, gpioID gpioId, uint32 data);
int32 rtrpc_generalCtrlGPIO_dev_init(uint32 unit, drv_generalCtrlGpio_devId_t dev, drv_generalCtrlGpio_devConf_t *pData);
int32 rtrpc_generalCtrlGPIO_dataBit_set(uint32 unit,        drv_generalCtrlGpio_devId_t dev, uint32 pinId, uint32 data);
int32 rtrpc_generalCtrlGPIO_dataBit_get(uint32 unit,        drv_generalCtrlGpio_devId_t dev, uint32 pinId, uint32 *data);
int32 rtrpc_generalCtrlGPIO_pin_init( uint32 unit, drv_generalCtrlGpio_devId_t dev, uint32 pinId, drv_generalCtrlGpio_pinConf_t *pData);
int32 rtrpc_generalCtrlGPIO_devEnable_set(uint32 unit, drv_generalCtrlGpio_devId_t dev, rtk_enable_t enable);
#if defined(CONFIG_SDK_DRIVER_I2C)
int32 rtrpc_i2c_init(uint32 unit);
int32 rtrpc_i2c_dev_init(uint32 unit, i2c_devConf_t *i2c_dev);
int32 rtrpc_i2c_write(uint32 unit, drv_i2c_devId_t i2c_dev_id, uint32 reg_idx, uint8 *pBuff);
int32 rtrpc_i2c_read(uint32 unit, drv_i2c_devId_t i2c_dev_id, uint32 reg_idx, uint8 *pBuff);
#endif /* CONFIG_SDK_DRIVER_I2C */
#if defined(CONFIG_SDK_DRIVER_SPI)
int32 rtrpc_spiPin_init(uint32 unit, spi_init_info_t *init_info);
int32 rtrpc_spi_write(uint32 unit, uint32 mAddrs, uint32 *buff);
int32 rtrpc_spi_read(uint32 unit, uint32 mAddrs, uint32 *buff);
#endif /* CONFIG_SDK_DRIVER_SPI */
int32 rtrpc_diag_portRtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult);
int32 rtrpc_diag_rtctEnable_set(uint32 unit, rtk_portmask_t *pPortmask);
int32 rtrpc_diag_table_whole_read(uint32 unit, uint32 table_index);
int32 rtrpc_diag_tableEntry_read(uint32 unit, uint32 table_index, uint32 start_index, uint32 end_index, uint32 detail);
int32 rtrpc_diag_tableEntryDatareg_write(uint32 unit, uint32 table_index, uint32 index, uint32 datareg_index,uint32 *pData);
int32 rtrpc_diag_tableEntry_write(uint32 unit, uint32 table_index, uint32 index, uint32 *pData, uint32 datareg_num);

int32 rtrpc_diag_peripheral_register_dump(uint32 unit);
int32 rtrpc_diag_reg_whole_read(uint32 unit);
int32 rtrpc_diag_phy_reg_whole_read(uint32 unit);
int32 rtrpc_diag_table_index_name(uint32 unit, uint32 table_index);

// REG get
int32 rtrpc_diag_reg_get(uint32 unit,uint32 reg, uint32 * pValue);
int32 rtrpc_diag_regField_get(uint32 unit,uint32 reg,uint32 field,uint32 * pValue);
int32 rtrpc_diag_regArray_get(uint32 unit,uint32 reg,int32 index_1,int32 index_2, uint32 * pValue);
int32 rtrpc_diag_regArrayField_get(uint32 unit,uint32 reg,int32 index_1,int32 index_2,uint32 field,uint32 * pValue);

// REG set
int32 rtrpc_diag_reg_set(uint32 unit,uint32 reg, uint32 * pValue);
int32 rtrpc_diag_regField_set(uint32 unit,uint32 reg,uint32 field,uint32 * pValue);
int32 rtrpc_diag_regArray_set(uint32 unit,uint32 reg,int32 index_1,int32 index_2,uint32 * pValue);
int32 rtrpc_diag_regArrayField_set(uint32 unit,uint32 reg,int32 index_1,int32 index_2,uint32 field,uint32 * pValue);

// REG/Field info get
#if defined (CONFIG_SDK_DUMP_REG_WITH_NAME)
int32 rtrpc_diag_regInfoByStr_get(uint32 unit, char *reg_name, rtk_diag_regInfo_t *pReg_info);
int32 rtrpc_diag_regFieldInfo_get(uint32 unit, uint32 reg, uint32 field, rtk_diag_regFieldInfo_t *pField_info);
int32 rtrpc_diag_regInfoByStr_match(uint32 unit, char *keyword, uint32 reg, rtk_diag_regInfo_t *pReg_info);
#endif

#if defined (CONFIG_SDK_DUMP_TABLE_WITH_NAME)
int32 rtrpc_diag_tableInfoByStr_get(uint32 unit,char * tbl_name,rtk_diag_tblInfo_t * pTbl_info);

int32 rtrpc_diag_tableFieldInfo_get(uint32 unit,uint32 tbl,uint32 field,rtk_diag_tblFieldInfo_t * pField_info);

int32 rtrpc_diag_tableInfoByStr_match(uint32 unit,char * keyword,uint32 tbl,rtk_diag_tblInfo_t * pTbl_info);
#endif

int32 rtrpc_diag_tableEntry_get(uint32 unit,uint32 table,uint32 entry,uint32 * pData);

int32 rtrpc_diag_tableEntry_set(uint32 unit,uint32 table,uint32 addr,uint32 * pData);



#if (defined(CONFIG_SDK_APP_DIAG_EXT) && defined (CONFIG_SDK_RTL9300))
int32 rtrpc_diag_table_reg_field_get(uint32 unit, rtk_diag_debug_t *pCfg);
int32 rtrpc_diag_table_reg_field_set(uint32 unit, rtk_diag_debug_t *pCfg);
#endif

extern int32
rtrpc_diagMapper_init(dal_mapper_t *pMapper);


#endif /* __RTRPC_DIAG_H__ */


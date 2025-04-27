/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 95681 $
 * $Date: 2019-03-12 10:34:14 +0800 (Tue, 12 Mar 2019) $
 *
 * Purpose : Define board profile structure, API, and macros.
 *
 * Feature :
 *
 */

#ifndef __UBOOT_CMD_H__
#define __UBOOT_CMD_H__

#include <common/rt_autoconf.h>
#include <common/rt_error.h>


/* Function Name:
 *      rtk_network_on
 * Description:
 *      Enable networking functionality and configure hardware to be able to send and receive packets.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_network_on(void);

/* Function Name:
 *      rtk_network_off
 * Description:
 *      Disable networking functionality and disable hardware from send and receive packets.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_network_off(void);

void rtk_uplinkPorts_enable(void);
void rtk_uplinkPorts_disable(void);

/* Function Name:
 *      rtk_forcelinkDown10Gserdes
 * Description:
 *      force linkdown 10g serdes port
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_forcelinkDown10Gserdes(uint32 unit);

/* Function Name:
 *      rtk_comboport_fiber
 * Description:
 *      Force comboport into fiber mode.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_comboport_fiber(void);

/* Function Name:
 *      rtk_comboport_copper
 * Description:
 *      Force comboport into copper mode.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_comboport_copper(void);

/* Function Name:
 *      rtk_comboport_portcopper
 * Description:
 *      Configure Combo-port as copper mode
 * Input:
 *      macId  - mac id (port number) to be configured
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_comboport_portcopper(int macId);

/* Function Name:
 *      rtk_comboport_portfiber
 * Description:
 *      Configure Combo-port as fiber mode
 * Input:
 *      macId - mac id (port number) to be configured
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_comboport_portfiber(int macId);

/* Function Name:
 *      rtk_sfp_speed_set
 * Description:
 *      Configure SFP port speed
 * Input:
 *      unit  - unit id
 *      port  - port number (mac id)
 *      speed - 100 or 1000
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_sfp_speed_set(int port, int speed);

/* Function Name:
 *      rtk_sysEsd_set
 * Description:
 *      Configure system ESD
 * Input:
 *      state - 1: enable; 0: disable
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_sysEsd_set(int state);

/* Function Name:
 *      rtk_parameter_version_show
 * Description:
 *      Display command of each version for reference
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_parameter_version_show(void);


typedef enum uboot_sdk_init_phase_e
{
    UBOOT_SDK_INIT_PHASE_RTCORE,
    UBOOT_SDK_INIT_PHASE_RTK,
    UBOOT_SDK_INIT_PHASE_MAX,
}uboot_sdk_init_phase_t;

/* Function Name:
 *      uboot_sdk_init
 * Description:
 *      Initialize sdk in U-Boot environment
 * Input:
 *      phase   - till which phase
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK - Success
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32 uboot_sdk_init(uboot_sdk_init_phase_t phase);

/* Function Name:
 *      rtk_hwProfileList_show
 * Description:
 *      Show available hardware profile list
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_hwProfileList_show(void);

/* Function Name:
 *      rtk_eee_on
 * Description:
 *      Enable all PHY EEE feature
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_eee_on(void);

/* Function Name:
 *      rtk_eee_off
 * Description:
 *      Disable all PHY EEE feature
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_eee_off(void);

/* Function Name:
 *      rtk_ledTest_run
 * Description:
 *      Run the led testing
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_ledTest_run(void);

/* Function Name:
 *      rtk_swledtest_on
 * Description:
 *      Light on the led on specific port.
 * Input:
 *      unit  - Unit id
 *      port  - Port id
 *      index - Led index
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_swledtest_on(int port,int index);

/* Function Name:
 *      rtk_swled_init
 * Description:
 *      initialize the software control led.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_swled_init(unsigned int unit);

/* Function Name:
 *      swled_isfiber
 * Description:
 *      Check whether the port is fiber
 * Input:
 *      unit  - Unit id
 *      port  - Port id
 *      index - Led index
 * Output:
 *      None
 * Return:
 *      1     - is fiber port
 *      0     - is not fiber port
 * Note:
 *      None
 */
int swled_isfiber(unsigned int unit, int port, int index);

/* Function Name:
 *      rtk_swled_out
 * Description:
 *      show out the software control led.
 * Input:
 *      unit  - unit id
 *      index - led index
 *      bit   - led status
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_swled_out(unsigned int unit, int index, unsigned int *bit);

/* Function Name:
 *      rtk_swled_uninit
 * Description:
 *      uninitialize the software control led.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_swled_uninit(unsigned int unit);

/* Function Name:
 *      rtk_intGpio_pin_set
 * Description:
 *      Set the internal GPIO status of specific pin
 * Input:
 *      pinNum    - the pin number of internal GPIO.
 *      pinStatus - the status of specific pin.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_DRIVER_NOT_FOUND
 * Note:
 *      None
 */
int rtk_intGpio_pin_set(uint pinNum, uint pinStatus);

/* Function Name:
 *      rtk_intGpio_pin_get
 * Description:
 *      Get the internal GPIO status of specific pin
 * Input:
 *      pinNum     - the pin number of internal GPIO.
 *      *pinStatus - the status of specific pin.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_DRIVER_NOT_FOUND
 * Note:
 *      None
 */
int rtk_intGpio_pin_get(uint pinNum, uint *pinStatus);

/* Function Name:
 *      rtk_extGpio_device_init
 * Description:
 *      Initial Device Conf structure for external GPIO
 * Input:
 *      unit              - Unit id
 *      deviceAddress     - RTL8231 MDC/MDIO address.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_NOT_INIT
 *      RT_ERR_DRIVER_NOT_FOUND
 * Note:
 *      Always use generalCtrl device ID 1 and MDC access mode.
 */
int rtk_extGpio_device_init(unsigned int unit, uint deviceAddress);

/* Function Name:
 *      rtk_extGpio_pin_set
 * Description:
 *      Set the external GPIO status of specific pin
 * Input:
 *      unit       - Unit id
 *      pinNum     - the pin number of internal GPIO.
 *      *pinStatus - the status of specific pin.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_NOT_INIT
 *      RT_ERR_DRIVER_NOT_FOUND
 * Note:
 *      None
 */
int rtk_extGpio_pin_set(unsigned int unit, uint pinNum, uint pinStatus);

/* Function Name:
 *      rtk_extGpio_pin_get
 * Description:
 *      Get the external GPIO status of specific pin
 * Input:
 *      unit       - Unit id
 *      pinNum     - the pin number of internal GPIO.
 *      *pinStatus - the status of specific pin.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_DRIVER_NOT_FOUND
 * Note:
 *      None
 */
int rtk_extGpio_pin_get(unsigned int unit, uint pinNum, uint *pinStatus);

/* Function Name:
 *      rtk_port_isolation_on
 * Description:
 *      Enable the port isolation.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_port_isolation_on(void);

/* Function Name:
 *      rtk_port_isolation_off
 * Description:
 *      Disable the port isolation.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_port_isolation_off(void);

/* Function Name:
 *      rtk_l2testmode_on
 * Description:
 *      Power on all ports without CPU port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      RTL838x/3xM test command only.
 */
void rtk_l2testmode_on(void);

/* Function Name:
 *      rtk_l2testmode_off
 * Description:
 *      Power off all ports.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      RTL838x/3xM test command only.
 */
void rtk_l2testmode_off(void);

#if defined(CONFIG_SDK_DRIVER_I2C)
/* Function Name:
 *      rtk_i2c_sw_init
 * Description:
 *      Initial I2C device for handle I2C interface
 * Input:
 *      i2c_dev      - I2C controller device
 *      clk_pin_dev  - specific GPIO device id of CLK pin
 *      clk_pin      - CLK pin id of specific GPIO device
 *      data_pin_dev - specific GPIO device id of DATA pin
 *      data_pin     - DATA pin id of specific GPIO device
 *      *typeStr     - "8" : 8 bits device ; "16" : 16 bits device
 *      chipid       - controlled I2C chip ID
 *      delay        - clock delay time
 *      rtl8231_addr - for ext-gpio used
 *      read_type    - 0 : Randon Read / 1 : Sequential Read
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      I2C interface is simulated by GPIO.
 */
void rtk_i2c_sw_init(unsigned int i2c_dev, unsigned int clk_pin_dev, unsigned int clk_pin, unsigned int data_pin_dev, unsigned int data_pin, char *typeStr, unsigned int chipid, unsigned int delay, unsigned int rtl8231_addr, unsigned int read_type);

/* Function Name:
 *      rtk_i2c_hw_init
 * Description:
 *      Initial I2C device for handle I2C interface
 * Input:
 *      i2c_dev      - I2C controller device
 *      clk_pin_dev  - specific GPIO device id of CLK pin
 *      clk_pin      - CLK pin id of specific GPIO device
 *      data_pin_dev - specific GPIO device id of DATA pin
 *      data_pin     - DATA pin id of specific GPIO device
 *      *typeStr     - "8" : 8 bits device ; "16" : 16 bits device
 *      chipid       - controlled I2C chip ID
 *      freq         - bus frequency
 *      intf_id      - controller ID
 *      read_type    - 0 : Randon Read / 1 : Sequential Read
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      I2C interface is HW I2C master controller.
 */
void rtk_i2c_hw_init(unsigned int i2c_dev, unsigned int data_pin, char *typeStr, unsigned int chipid, unsigned int freq, unsigned int intf_id, unsigned int read_type);


/* Function Name:
 *      rtk_i2c_read
 * Description:
 *      Read register from I2C device
 * Input:
 *      dev     - I2C controller device
 *      reg_idx - specific read register id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      I2C interface is simulated by GPIO .
 */
void rtk_i2c_read(unsigned int dev, unsigned int reg_idx);

/* Function Name:
 *      rtk_i2c_write
 * Description:
 *      Write data to I2C device register
 * Input:
 *      dev     - I2C controller device
 *      reg_idx - specific write register id
 *      wData   - write data
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      I2C interface is simulated by GPIO .
 */
void rtk_i2c_write(unsigned int dev, unsigned int reg_idx, unsigned int wData);
#endif/* CONFIG_SDK_DRIVER_I2C */

/* Function Name:
 *      rtk_phy_testmode
 * Description:
 *      Enanle specific port testmode
 * Input:
 *      argv[2] - unit
 *      argv[3] - test mode
 *      argv[4] - specific port id
 *      argv[5] - test chn
 * Output:
 *      None
 * Return:
 *      0 - specific port test
 *      1 - all port test
 * Note:
 *      None
 */
int rtk_phy_testmode(char * const argv[]);

/* Function Name:
 *      rtk_phyreg_get
 * Description:
 *      Read specific PHY register value
 * Input:
 *      argv[3] - unit id
 *      argv[4] - specific port id
 *      argv[5] - page number
 *      argv[6] - register index
 * Output:
 *      None
 * Return:
 *      0
 * Note:
 *      None
 */
int rtk_phyreg_get(char * const argv[]);

/* Function Name:
 *      rtk_phyreg_set
 * Description:
 *      Write data to specific PHY register
 * Input:
 *      argv[3] - unit id
 *      argv[4] - specific port id
 *      argv[5] - page number
 *      argv[6] - register index
 *      argv[7] - write data value
 * Output:
 *      None
 * Return:
 *      0
 * Note:
 *      None
 */
int rtk_phyreg_set(char * const argv[]);


/* Function Name:
 *      rtk_phymmd_get
 * Description:
 *      Read specific PHY MMD register value
 * Input:
 *      uint   - unit id
 *      portid - specific port id
 *      addr   - address number
 *      reg    - register index
 * Output:
 *      None
 * Return:
 *      0
 * Note:
 *      None
 */
int rtk_phymmd_get(unsigned int unit, unsigned int portid, unsigned int addr, unsigned int reg);

/* Function Name:
 *      rtk_phymmd_set
 * Description:
 *      Write data to specific PHY MMD register
 * Input:
 *      uint   - unit id
 *      portid - specific port id
 *      addr   - address number
 *      reg    - register index
 *      val    - write data value
 * Output:
 *      None
 * Return:
 *      0
 * Note:
 *      None
 */
int rtk_phymmd_set(unsigned int unit, unsigned int portid, unsigned int addr, unsigned int reg, unsigned int val);

/* Function Name:
 *      rtk_sdsreg_get
 * Description:
 *      Read specific Serdes register value
 * Input:
 *      unit - unit id
 *      sdsId - specific serdes id
 *      page - page number
 *      reg - register index
 * Output:
 *      None
 * Return:
 *      0
 * Note:
 *      None
 */
int rtk_sdsreg_get(unsigned int unit, unsigned int sdsId, unsigned int page, unsigned int reg);

/* Function Name:
 *      rtk_sdsreg_set
 * Description:
 *      Write data to specific Serdes register
 * Input:
 *      unit - unit id
 *      sdsId - specific serdes id
 *      page - page number
 *      reg - register index
 *      val - write data value
 * Output:
 *      None
 * Return:
 *      0
 * Note:
 *      None
 */
int rtk_sdsreg_set(unsigned int unit, unsigned int sdsId, unsigned int page, unsigned int reg, unsigned int val);

/* Function Name:
 *      rtk_phyreg_portmask_set
 * Description:
 *      Write data to specific PHYs register by port mask.
 * Input:
 *      argv[3] - unit id
 *      argv[4] - specific port mask
 *      argv[5] - page number
 *      argv[6] - register index
 *      argv[7] - write data value
 * Output:
 *      None
 * Return:
 *      0
 * Note:
 *      None
 */
int rtk_phyreg_portmask_set(char * const argv[]);

/* Function Name:
 *      rtk_phyreg_dump_top
 * Description:
 *      Dump Top information of specific PHY.
 * Input:
 *      argv[3] - specific port id
 * Output:
 *      None
 * Return:
 *      0
 * Note:
 *      For RTL8214FC only.
 */
int rtk_phyreg_dump_top(char * const argv[]);

/* Function Name:
 *      rtk_boardId_get
 * Description:
 *      Set board id to env
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int rtk_boardId_get (void);

/* Function Name:
 *      rtk_boardId_set
 * Description:
 *      Get board id from env
 * Input:
 *      id - board id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int rtk_boardId_set (int id);

/* Function Name:
 *      rtk_boardModel_get
 * Description:
 *      Set board model to env
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int rtk_boardModel_get (void);

/* Function Name:
 *      rtk_boardModel_set
 * Description:
 *      Get board model from env
 * Input:
 *      id - board id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int rtk_boardModel_set (char * const argv[]);

/* Function Name:
 *      rtk_10gMedia_none_set
 *      rtk_10gMedia_fiber10g_set
 *      rtk_10gMedia_fiber1g_set
 *      rtk_10gMedia_fiber100m_set
 *      rtk_10gMedia_dac50cm_set
 *      rtk_10gMedia_dac100cm_set
 *      rtk_10gMedia_dac300cm_set
 * Description:
 *      Set 10G port media as defined in rtk_port_10gMedia_t
 * Input:
 *      port        mac ID
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtk_10gMedia_none_set(unsigned int port);
void rtk_10gMedia_fiber10g_set(unsigned int port);
void rtk_10gMedia_fiber1g_set(unsigned int port);
void rtk_10gMedia_fiber100m_set(unsigned int port);
void rtk_10gMedia_dac50cm_set(unsigned int port);
void rtk_10gMedia_dac100cm_set(unsigned int port);
void rtk_10gMedia_dac300cm_set(unsigned int port);

/* Function Name:
 *      rtk_mac_sdsMode_set
 * Description:
 *      Configure mac serdes mode
 * Input:
 *      sds - serdes id
 *      mode - serdes mode
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
rtk_mac_sdsMode_set(uint32 sds, uint32 mode);

#endif /* __UBOOT_CMD_H__ */

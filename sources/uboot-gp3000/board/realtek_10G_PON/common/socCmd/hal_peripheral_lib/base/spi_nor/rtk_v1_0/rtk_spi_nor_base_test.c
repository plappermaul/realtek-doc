
// #include <stdio.h>
// #include "lx_common.h"
// #include "lx_macro.h"
// #include "rle0272_defs.h"

#include <common.h>

// Add for RXI-310 SPI_Flash_Controller
#include <malloc.h>

#include "include/DW_common.h"
#include "include/flash_vender.h"
#include "include/spi_flash_private.h"
#include "include/spi_flash_public.h"

extern struct dw_device dev;
  // extern struct spi_flash_portmap *spi_flash_map;
  // extern struct device_info dev_info;
  // extern struct spi_flash_param ps_para=
  // CC_DEFINE_SPI_FLASH_PARAMS(ps_);


#define _memctl_debug_printf printf
// #define _memctl_debug_printf(...)
#define Strtoul simple_strtoul

static int chip_select = 0;


uint32_t
flash_init(struct dw_device *dev, uint8_t cmd_rdid)
{
	// using to init flash_cmd_info

	if (flash_device_init(dev, cmd_rdid))
	return DW_ENODEV;

	// using to init write signal
	flash_disable_write(dev);

	return 0;
}

#if 1

uint32_t
flash_device_init(struct dw_device * dev, uint8_t cmd_rdid)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;

	int info, rd_data;
	int num, size;
	int comp = 0;
	char *vender = 0;

	// printf("DEF_RD_QUAD_DUMMY= %x", cmd_info.rd_quad_dummy);

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;

	// To initialize the command infomation
	dev_info->cmd_info = &cmd_info;
#if C_DEBUG
	printf("DEF_RD_QUAD_DUMMY= %x\n", dev_info->cmd_info->rd_quad_dummy);
#endif

	// disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	// printf("printf\n");
	// printf("printf 0x%x\n", spi_flash_map->auto_length);
	// printf("printf 0x%x\n", bfoSPI_FLASH_AUTO_LEN_DUM);
	// printf("printf 0x%x\n", dev_info-> cmd_info-> tunning_dummy);
	// printf("printf 0x%x\n", bfwSPI_FLASH_AUTO_LEN_DUM);

#if 0   // enable VALID_CMD(offset 0x120 bit[10] , auto polling flash busy
	// status function)
	spi_flash_map->valid_cmd |= (1 << 10);

#if C_DEBUG
	printf("\nNeed Check:spi_flash_map-> valid_cmd =0x%x,\n", spi_flash_map->valid_cmd);
#endif
#endif

#if CONFIG_SPI_DW_BITS_SET_VAL
	// Set auto_length: initialize to 0
	DW_BITS_SET_VAL(spi_flash_map->auto_length, bfoSPI_FLASH_AUTO_LEN_DUM, dev_info->cmd_info->tunning_dummy, bfwSPI_FLASH_AUTO_LEN_DUM);
#endif
	// Set Ctrlr1; 3 byte data frames
	spi_flash_setctrlr1(dev, 3);

	// Flash_cmd: cmd_rdid
	flash_rx_cmd(dev, cmd_rdid);

	rd_data = spi_flash_map->dr[DR0].half;
	info = ((rd_data & 0x000000ff) << 16) | ((rd_data & 0x0000ff00));

	rd_data = spi_flash_map->dr[DR0].byte;
	info = (rd_data & 0x000000ff) | info;

	dev_info->device_id = info;

	size = sizeof(struct FLASH_DEVICE) / sizeof(struct flash_info *);
#if C_DEBUG
	printf("Device struct size is %d,\n", size);
#endif

	for (num = 0; num < size; num++) {
		if ((info >> 16) == DEVICE.VENDER[num]->flash_id) {
			dev_info->cmd = DEVICE.VENDER[num]->cmd;
			vender = DEVICE.VENDER[num]->vender_name;
			comp = 1;
		}
	}

	spi_flash_map->ssienr = 0;
	info = 0;
#if 0
	/*
	 * For auto mode: Set valid_cmd_reg: auto_cmd
	 *
	 */
	if (dev_info->cmd_info->wr_quad_type == WR_QUAD_II) {
		info = 0x100;
		spi_flash_map->wr_quad_ii = dev_info->cmd->ppx4_ii;
		printf("SPI write type:WR_QUAD_II, cmd=0x%x\n", spi_flash_map->wr_quad_ii);

		// Setting of auto -mode's control registger
		info |= EN_WR_QUAD_II;
	} else if (dev_info->cmd_info->wr_quad_type == WR_QUAD_I) {
		info = 0x080;
		spi_flash_map->wr_quad_i = dev_info->cmd->ppx4_i;
		printf("SPI write type:WR_QUAD_I , cmd=0x%x\n", spi_flash_map->wr_quad_i);

		// Setting of auto -mode's control registger
		info |= EN_WR_QUAD_I;
	} else if (dev_info->cmd_info->wr_dual_type == WR_DUAL_II) {
		info = info | 0x040;
		spi_flash_map->wr_dual_ii = dev_info->cmd->ppx2_ii;
		printf("SPI write type:WR_DUAL_II , cmd=0x%x\n",
		spi_flash_map->wr_dual_ii);

		// Setting of auto -mode's control registger
		info |= EN_WR_DUAL_II;

	} else if (dev_info->cmd_info->wr_dual_type == WR_DUAL_I) {
		info = info | 0x020;
		spi_flash_map->wr_dual_i = dev_info->cmd->ppx2_i;
		printf("SPI write type:WR_DUAL_I , cmd=0x%x\n",
		spi_flash_map->wr_dual_i);

		// Setting of auto -mode's control registger
		info |= EN_WR_DUAL_I;
	}

	if (dev_info->cmd_info->rd_quad_type == RD_QUAD_IO) {
		// QE bit-6: MXIC
		printf("SPI: Set MXIC SPI flash QE bit!\n\r");
		flash_set_status(dev, 0x40, DATA_BYTE);

		info = info | 0x010;
		spi_flash_map->rd_quad_io = dev_info->cmd->readx4_io;
		printf("SPI read type:RD_QUAD_IO , cmd=0x%x\n", spi_flash_map->rd_quad_io);


		// Setting of auto -mode's control registger
		info |= EN_RD_QUAD_IO;

		spi_flash_set_dummy_cycle(dev, dev_info->cmd_info->rd_quad_dummy);

	} else if (dev_info->cmd_info->rd_quad_type == RD_QUAD_O) {

		// QE bit-6: MXIC
		printf("SPI: Set MXIC SPI flash QE bit!\n\r");
		flash_set_status(dev, 0x40, DATA_BYTE);

		info = info | 0x008;
		spi_flash_map->rd_quad_o = dev_info->cmd->readx4_o;
		printf("SPI read type:RD_QUAD_O , cmd=0x%x\n", spi_flash_map->rd_quad_o);

		// Setting of auto -mode's control registger
		info |= EN_RD_QUAD_O;
	} else if (dev_info->cmd_info->rd_dual_type == RD_DUAL_IO) {
		info = info | 0x004;
		spi_flash_map->rd_dual_io = dev_info->cmd->readx2_io;
		printf("SPI read type:RD_DUAL_IO , cmd=0x%x\n", spi_flash_map->rd_dual_io);

		// Setting of auto -mode's control registger
		info |= EN_RD_DUAL_IO;
	} else if (dev_info->cmd_info->rd_dual_type == RD_DUAL_O) {
		info = info | 0x002;
		spi_flash_map->rd_dual_o = dev_info->cmd->readx2_o;
		printf("SPI read type:RD_DUAL_O , cmd=0x%x\n", spi_flash_map->rd_dual_o);

		// Setting of auto -mode's control registger
		info |= EN_RD_DUAL_I;
	}

	rd_data = spi_flash_map->valid_cmd;

	// JSW: write ready check to SPI flash
	info |= EN_CHECK_SPI_FLASH_STATUS_FUNCTION;

	spi_flash_map->ssienr = 0;
	spi_flash_map->valid_cmd = (spi_flash_map->valid_cmd | info | 0x200);

#if C_DEBUG
	printf("valid_cmd= %x , info=%x\n", spi_flash_map->valid_cmd, info);
#endif

#else
	spic_init_auto_mode(dev);
#endif

	if (comp == 0) {
#if PRINTF_ON
		printf("Warnning: No SPI Flash Detected!!!\n");
		// printf ("No Device_info can be initialized.\n");
		printf("ID_num is %8x .\n", info);
#endif
		return DW_ENODEV;
	} else {
#if PRINTF_ON
		printf("\n************************************\n");
		printf("%s, ID_num is %x .\n", vender, dev_info->device_id);
		printf("************************************\n");
#endif
		// return 0;
		return dev_info->device_id;
	}
}

#else

uint32_t
flash_device_init(struct dw_device * dev, uint8_t cmd_rdid)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;

	int info, rd_data;
	int num,  size;
	int comp = 0;
	char *vender;

	// printf("DEF_RD_QUAD_DUMMY= %x", cmd_info.rd_quad_dummy);

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;

	// To initialize the command infomation
	dev_info->cmd_info = &cmd_info;
#if C_DEBUG
	printf("DEF_RD_QUAD_DUMMY= %x\n", dev_info->cmd_info->rd_quad_dummy);
#endif

	// disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	// printf("printf\n");
	// printf("printf 0x%x\n", spi_flash_map->auto_length);
	// printf("printf 0x%x\n", bfoSPI_FLASH_AUTO_LEN_DUM);
	// printf("printf 0x%x\n", dev_info-> cmd_info-> tunning_dummy);
	// printf("printf 0x%x\n", bfwSPI_FLASH_AUTO_LEN_DUM);

	// Set auto_length: initialize to 0
	DW_BITS_SET_VAL(spi_flash_map->auto_length, bfoSPI_FLASH_AUTO_LEN_DUM, dev_info->cmd_info->tunning_dummy, bfwSPI_FLASH_AUTO_LEN_DUM);

#if 0
	// Set Ctrlr1; 3 byte data frames
	spi_flash_setctrlr1(dev, 3);

	// Flash_cmd: cmd_rdid
	flash_rx_cmd(dev, cmd_rdid);

	rd_data = spi_flash_map->dr[DR0].half;
	info = ((rd_data & 0x000000ff) << 16) | ((rd_data & 0x0000ff00));

	rd_data = spi_flash_map->dr[DR0].byte;
	info = (rd_data & 0x000000ff) | info;

	dev_info->device_id = info;
#endif
	dev_info->device_id = flash_read_id(dev);

	size = sizeof(struct FLASH_DEVICE) / sizeof(struct flash_info *);
#if C_DEBUG
	printf("Device struct size is %d,\n", size);
#endif

	for (num = 0; num < size; num++) {
		if ((info >> 16) == DEVICE.VENDER[num]->flash_id) {
			dev_info->cmd = DEVICE.VENDER[num]->cmd;
			vender = DEVICE.VENDER[num]->vender_name;
			comp = 1;
		}
	}

	spi_flash_map->ssienr = 0;
	info = 0;

	// Set valid_cmd_reg: auto_cmd
	if (dev_info->cmd_info->wr_quad_type == WR_QUAD_II) {
		info = 0x100;
		spi_flash_map->wr_quad_ii = dev_info->cmd->ppx4_ii;
	} else if (dev_info->cmd_info->wr_quad_type == WR_QUAD_I) {
		info = 0x080;
		spi_flash_map->wr_quad_i = dev_info->cmd->ppx4_i;
	} else if (dev_info->cmd_info->wr_dual_type == WR_DUAL_II) {
		info = info | 0x040;
		spi_flash_map->wr_dual_ii = dev_info->cmd->ppx2_ii;
	} else if (dev_info->cmd_info->wr_dual_type == WR_DUAL_I) {
		info = info | 0x020;
		spi_flash_map->wr_dual_i = dev_info->cmd->ppx2_i;
	}

	if (dev_info->cmd_info->rd_quad_type == RD_QUAD_IO) {
		info = info | 0x010;
		spi_flash_map->rd_quad_io = dev_info->cmd->readx4_io;
	} else if (dev_info->cmd_info->rd_quad_type == RD_QUAD_O) {
		info = info | 0x008;
		spi_flash_map->rd_quad_o = dev_info->cmd->readx4_o;
	} else if (dev_info->cmd_info->rd_dual_type == RD_DUAL_IO) {
		info = info | 0x004;
		spi_flash_map->rd_dual_io = dev_info->cmd->readx2_io;
	} else if (dev_info->cmd_info->rd_dual_type == RD_DUAL_O) {
		info = info | 0x002;
		spi_flash_map->rd_dual_o = dev_info->cmd->readx2_o;
	}

	rd_data = spi_flash_map->valid_cmd;

	spi_flash_map->valid_cmd = (spi_flash_map->valid_cmd | info | 0x200);

#if C_DEBUG
	printf("valid_cmd= %x\n", spi_flash_map->valid_cmd);
#endif

	if (comp == 0) {
#if PRINTF_ON
		printf("No Device_info can be initized.\n");
		printf("ID_num is %8x .\n", info);
#endif
		return DW_ENODEV;
	} else {
#if PRINTF_ON
		printf("\n************************************\n");
		printf("%s, ID_num is %x .\n", vender, dev_info->device_id);
		printf("************************************\n");
#endif
		return 0;
	}

}

#endif

void
flash_tx_cmd(struct dw_device *dev, uint8_t cmd)
{
	struct spi_flash_portmap *spi_flash_map;
	//struct device_info inst_info;
	uint32_t rd_data;

	spi_flash_map = dev->base_address;

	// Disble SPI_FLASH
	spi_flash_map->ssienr = 0;

	// set ctrlr0: TX mode
	rd_data = spi_flash_map->ctrlr0;
	spi_flash_map->ctrlr0 = (rd_data & 0xfff0fcff);

	// set flash_cmd: wren to fifo
	spi_flash_setdr(dev, DR0, cmd, DATA_BYTE);

	// Enable SPI_FLASH
	spi_flash_map->ssienr = 1;

	spi_flash_wait_busy(dev);

	flash_wait_busy(dev);
}


void
flash_rx_cmd(struct dw_device *dev, uint8_t cmd)
{
	struct spi_flash_portmap *spi_flash_map;
	uint32_t rd_data;

	spi_flash_map = dev->base_address;

	// Disble SPI_FLASH
	spi_flash_map->ssienr = 0;

	// set ctrlr0: RX_mode
	rd_data = spi_flash_map->ctrlr0;
	spi_flash_map->ctrlr0 = (rd_data & 0xfff0ffff) | 0x00000300;

	// set flash_cmd: write cmd to fifo
	spi_flash_setdr(dev, DR0, cmd, DATA_BYTE);

	// Enable SPI_FLASH
	spi_flash_map->ssienr = 1;

	spi_flash_wait_busy(dev);

	// // Disble SPI_FLASH
	// spi_flash_map-> ssienr =0;

	// // set ctrlr0: TX_mode
	// rd_data = spi_flash_map->ctrlr0;
	// spi_flash_map->ctrlr0 = (rd_data & 0xfff0fcff);

}


void
flash_set_status(struct dw_device *dev, uint32_t data, enum spi_flash_byte_num byte_num)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;
	uint8_t cmd_wrsr, cmd_rdsr, rd_data;
	uint32_t info;

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;
	info = spi_flash_map->addr_length;

	printf("flash_set_status():spi_flash_map=0x%x\n", spi_flash_map);

	// Set flash_cmd: WREN to FIFO
	flash_tx_cmd(dev, dev_info->cmd->wren);

	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	// set ctrlr0: TX mode
	rd_data = spi_flash_map->ctrlr0;
	spi_flash_map->ctrlr0 = (rd_data & 0xfff0fcff);

	spi_flash_map->addr_length = 1;

	// Set flash_cmd: WRSR to FIFO
	cmd_wrsr = dev_info->cmd->wrsr;

	spi_flash_map->dr[DR0].byte = cmd_wrsr;

	// Set data FIFO
	if (byte_num == DATA_BYTE) {
		spi_flash_map->dr[DR0].byte = data;
	} else if (byte_num == DATA_HALF) {
		spi_flash_map->dr[DR0].byte = data;
		spi_flash_map->dr[DR1].byte = data >> 8;
	}

	spi_flash_map->ssienr = 1;
	spi_flash_wait_busy(dev);

	spi_flash_map->ssienr = 0;
	spi_flash_map->addr_length = info;

	flash_wait_busy(dev);
}



uint8_t
flash_get_status(struct dw_device *dev)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;
	uint8_t cmd_rdsr;

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;

	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	// Set Ctrlr1; 1 byte data frames
	spi_flash_map->ctrlr1 = 1;

#if 1   // JSW
	// Set tuning dummy cycles
	DW_BITS_SET_VAL(spi_flash_map->auto_length, bfoSPI_FLASH_AUTO_LEN_DUM, dev_info->cmd_info->tunning_dummy, bfwSPI_FLASH_AUTO_LEN_DUM);

#endif

	// Set flash_cmd: RDSR to FIFO
	cmd_rdsr = dev_info->cmd->rdsr;

	flash_rx_cmd(dev, cmd_rdsr);

	return spi_flash_getdr(dev, DR0, DATA_BYTE);
}


/*
 * For SPIC auto-mode init
 */
int
spic_init_auto_mode(struct dw_device *dev)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;
	// uint8_t cmd_rdsr;

	int info = 0x0;

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;

	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	/*
	 * For auto mode: Set valid_cmd_reg: auto_cmd
	 *
	 */
	if (dev_info->cmd_info->wr_quad_type == WR_QUAD_II) {
		info = 0x100;
		spi_flash_map->wr_quad_ii = dev_info->cmd->ppx4_ii;
		printf("SPI write type:WR_QUAD_II , cmd=0x%x\n", spi_flash_map->wr_quad_ii);

		// Setting of auto -mode's control registger
		info |= EN_WR_QUAD_II;

	} else if (dev_info->cmd_info->wr_quad_type == WR_QUAD_I) {
		info = 0x080;
		spi_flash_map->wr_quad_i = dev_info->cmd->ppx4_i;
		printf("SPI write type:WR_QUAD_I , cmd=0x%x\n", spi_flash_map->wr_quad_i);

		// Setting of auto -mode's control registger
		info |= EN_WR_QUAD_I;

	} else if (dev_info->cmd_info->wr_dual_type == WR_DUAL_II) {
		info = info | 0x040;
		spi_flash_map->wr_dual_ii = dev_info->cmd->ppx2_ii;
		printf("SPI write type:WR_DUAL_II , cmd=0x%x\n", spi_flash_map->wr_dual_ii);

		// Setting of auto -mode's control registger
		info |= EN_WR_DUAL_II;

	} else if (dev_info->cmd_info->wr_dual_type == WR_DUAL_I) {
		info = info | 0x020;
		spi_flash_map->wr_dual_i = dev_info->cmd->ppx2_i;
		printf("SPI write type:WR_DUAL_I , cmd=0x%x\n", spi_flash_map->wr_dual_i);

		// Setting of auto -mode's control registger
		info |= EN_WR_DUAL_I;
	}

	if (dev_info->cmd_info->rd_quad_type == RD_QUAD_IO) {
		// QE bit-6: MXIC
		printf("SPI: Set MXIC SPI flash QE bit!\n\r");
		flash_set_status(dev, 0x40, DATA_BYTE);

		info = info | 0x010;
		spi_flash_map->rd_quad_io = dev_info->cmd->readx4_io;
		printf("SPI read type:RD_QUAD_IO , cmd=0x%x\n", spi_flash_map->rd_quad_io);

		// Setting of auto -mode's control registger
		info |= EN_RD_QUAD_IO;

		/*
		 * Note: Some flash device may needed different dummy cycle
		 */
		spi_flash_set_dummy_cycle(dev, dev_info->cmd_info->rd_quad_dummy);
	} else if (dev_info->cmd_info->rd_quad_type == RD_QUAD_O) {

		// QE bit-6: MXIC
		printf("SPI: Set MXIC SPI flash QE bit!\n\r");
		flash_set_status(dev, 0x40, DATA_BYTE);

		info = info | 0x008;
		spi_flash_map->rd_quad_o = dev_info->cmd->readx4_o;
		printf("SPI read type:RD_QUAD_O , cmd=0x%x\n", spi_flash_map->rd_quad_o);

		// Setting of auto -mode's control registger
		info |= EN_RD_QUAD_O;

		/*
		 * Note: Some flash device may needed different dummy cycle
		 */
		spi_flash_set_dummy_cycle(dev, dev_info->cmd_info->rd_quad_dummy);

	} else if (dev_info->cmd_info->rd_dual_type == RD_DUAL_IO) {
		info = info | 0x004;
		spi_flash_map->rd_dual_io = dev_info->cmd->readx2_io;
		printf("SPI read type:RD_DUAL_IO , cmd=0x%x\n", spi_flash_map->rd_dual_io);

		// Setting of auto -mode's control registger
		info |= EN_RD_DUAL_IO;

		/*
		 * Note: Some flash device may needed different dummy cycle
		 */
		spi_flash_set_dummy_cycle(dev, dev_info->cmd_info->rd_dual_dummy);

	} else if (dev_info->cmd_info->rd_dual_type == RD_DUAL_O) {
		info = info | 0x002;
		spi_flash_map->rd_dual_o = dev_info->cmd->readx2_o;
		printf("SPI read type:RD_DUAL_O , cmd=0x%x\n", spi_flash_map->rd_dual_o);

		// Setting of auto -mode's control registger
		info |= EN_RD_DUAL_I;

		/*
		 * Note: Some flash device may needed different dummy cycle
		 */
		spi_flash_set_dummy_cycle(dev, dev_info->cmd_info->rd_dual_dummy);
	}
	// rd_data = spi_flash_map->valid_cmd;

	// JSW: write ready check to SPI flash
	info |= EN_CHECK_SPI_FLASH_STATUS_FUNCTION;

	spi_flash_map->ssienr = 0;
	spi_flash_map->valid_cmd = (spi_flash_map->valid_cmd | info | 0x200);


#ifdef SUPPORT_SPI_SIO
	spi_flash_map->valid_cmd = 0x600;
#if SUPPORT_SPI_SIO_FASTREAD
	spi_flash_map->valid_cmd |= EN_FRD_SINGLE;
#endif
	printf("SPI read/write type:SIO, valid_cmd =0x%x\n", spi_flash_map->valid_cmd);
	spi_flash_set_dummy_cycle(dev, DEF_RD_TUNING_DUMMY_CYCLE);
#endif

#if 1   // C_DEBUG
	printf("valid_cmd= %x , info=%x\n", spi_flash_map->valid_cmd, info);
#endif
	return CMD_RET_SUCCESS;
}


void
flash_wait_busy(struct dw_device *dev)
{
#if 1
	// Check flash is in write progress or not
	while (1) {
	if (!(flash_get_status(dev) & 0x1))
		break;
	}
#else

	return;	 // SPIC already adopt auto polling function as below
	// REG32(0xf8143120) |= EN_CHECK_SPI_FLASH_STATUS_FUNCTION;// write
	// ready check to SPI flash
#endif
}


void
flash_chip_erase(struct dw_device *dev)
{

#if 0   // debug purpose
	// Dump some debug message
	ew 0xf800200c 0x03000000	// UART_LCR
	ew 0xf8002008 0xc7000000
	ew 0xf8002004 0x0
	ew 0xf800200c 0x83000000	// UART_LCR
	ew 0xf8002000 0x36000000	// UART_DLL
	ew 0xf8002004 0x00000000	// UART_DLM
	ew 0xf800200c 0x03000000
	// delay 2
	ew 0xf8002000 0x20000000	// data
	// delay 200
	ew 0xf8002000 0x21000000	// data
	// delay 20
	ew 0xf8002000 0x22000000	// data
	// delay 20
	ew 0xf8002000 0x23000000	// data
	// delay 20
	ew 0xf8002000 0x24000000	// data
	// delay 20
#endif
#if 0
	REG32(0xf800200c) = 0x03000000; // UART_LCR

	REG32(0xf8002008) = 0xc7000000;
	REG32(0xf8002004) = 0x0;
	REG32(0xf800200c) = 0x83000000;	 // UART_LCR

	REG32(0xf8002000) = 0x36000000;
	REG32(0xf8002004) = 0x0;
	REG32(0xf800200c) = 0x03000000;

	REG32(0xf800200c) = 0x20000000;	 // data
	REG32(0xf800200c) = 0x21000000;	 // data
	REG32(0xf800200c) = 0x22000000;	 // data
	REG32(0xf800200c) = 0x23000000;	 // data
	REG32(0xf800200c) = 0x24000000;	 // data
#endif
	//struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;

	// printf("\n flash_chip_erase()\n");

	dev_info = dev->instance;

	// Set flash_cmd: WREN to FIFO
	// flash_tx_cmd(dev, dev_info-> cmd ->wren);
	flash_tx_cmd(dev, 0x6);
	// printf("\n flash_chip_erase(),wren=0x%x\n",dev_info-> cmd ->wren);

	// Set flash_cmd: Chip_erase to FIFO
	// flash_tx_cmd(dev, dev_info-> cmd ->ce);
	printf("\nErasing... flash_chip_erase(),ce=0x%x\n", dev_info->cmd->ce);
	flash_tx_cmd(dev, 0xc7);
}


void
flash_se_erase(struct dw_device *dev, uint32_t addr)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;
	uint32_t wr_addr;
	//uint8_t cmd;

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;

	// printf("\r\n flash_se_erase():spi_flash_map:0x%x\r\n",
	// spi_flash_map);
	// printf("\r\n flash_se_erase():addr:0x%x\r\n", addr);

	// Set flash_cmd: WREN to FIFO
	flash_tx_cmd(dev, dev_info->cmd->wren);

	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	// set ctrlr0: TX_mode
	DW_BITS_SET_VAL(spi_flash_map->ctrlr0, bfoSPI_FLASH_CTRLR0_TMOD, 0, bfwSPI_FLASH_CTRLR0_TMOD);

	/*
	 * 3 byte addressing
	 */
	// Set flash_cmd: Sector_erase to FIFO
	if (((spi_flash_map->addr_length) & 0x3) == 0x3) {
		wr_addr = ((dev_info->cmd->se) & 0x000000ff) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);
		// printf("\n(3 byte addressing) wr_addr =0x%x\n", wr_addr );
		// Write cmd, addr, data into FIFO
		spi_flash_setdr(dev, DR0, wr_addr, DATA_WORD);
	}
#if CONFIG_SPI_3to4BYTES_ADDRESS_SUPPORT
	/*
	 * 4 byte addressing
	 */
	// if ( (spi_flash_map-> addr_length)&(0x3)==0 )
	else {
		wr_addr = ((addr & 0xff000000) >> 24) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);
		// printf("\n(4 byte addressing) wr_addr =0x%x\n", wr_addr );

		// set flash_cmd: wren to fifo
		spi_flash_setdr(dev, DR0, (dev_info->cmd->se), DATA_BYTE);
		// Write cmd, addr, data into FIFO
		spi_flash_setdr(dev, DR0, wr_addr, DATA_WORD);
	}
#endif
	spi_flash_map->ssienr = 1;
	spi_flash_wait_busy(dev);
	flash_wait_busy(dev);
}


#if CONFIG_SPI_3to4BYTES_ADDRESS_SUPPORT
 // Set flash_cmd: EN4B (0xb7)
void
flash_EN4B(struct dw_device *dev)
{

#if 0
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;
	uint32_t wr_addr;
	uint8_t cmd;

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;

	// printf("\r\n flash_se_erase():spi_flash_map:0x%x\r\n",
	// spi_flash_map);
	// printf("\r\n flash_se_erase():addr:0x%x\r\n", addr);

	flash_tx_cmd(dev, 0xb7);

	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	// set ctrlr0: TX_mode
	DW_BITS_SET_VAL(spi_flash_map->ctrlr0, bfoSPI_FLASH_CTRLR0_TMOD, 0, bfwSPI_FLASH_CTRLR0_TMOD);

	// Set flash_cmd: Sector_erase to FIFO
	wr_addr = ((dev_info->cmd->se) & 0x000000ff) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);

	// Write cmd, addr, data into FIFO
	spi_flash_setdr(dev, DR0, wr_addr, DATA_WORD);

	spi_flash_map->ssienr = 1;
	spi_flash_wait_busy(dev);
	flash_wait_busy(dev);
#else
	printf(" Set flash_cmd: EN4B (0xb7)!\r\n");

	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;
	// Set flash_cmd: EN4B to FIFO , set to SPI device
	flash_tx_cmd(dev, 0xb7);

	spi_flash_map->ssienr = 0;

	printf(" Set SPIC to 4-byte addressing mode\r\n");
	// Set flash_cmd: EN4B to FIFO
	spi_flash_map->addr_length &= 0xFFFFFFFC;   // [1:0]=0 for 4-byte addressing , SPIC users mode
	spi_flash_map->auto_length &= 0xFFFCFFFF;   // [1:0]=0 for 4-byte addressing ,SPIC auto mode

	spi_flash_wait_busy(dev);
	flash_wait_busy(dev);
#endif
}
#endif


void
flash_disable_write(struct dw_device *dev)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;

	flash_tx_cmd(dev, dev_info->cmd->wrdi);
}


void
flash_enable_write(struct dw_device *dev)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;

	flash_tx_cmd(dev, dev_info->cmd->wren);
}


#if 0


uint32_t
flash_write_block(struct dw_device *dev, uint32_t source_addr, uint32_t byte_size, uint32_t target_addr)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;
	struct spi_flash_param *spi_para;

	uint32_t wr_addr, init_data;
	uint32_t wr_block_bound;
	uint32_t data_count, page_bound_count;
	uint32_t word_times, byte_times;
	uint32_t wr_word_times, split_times;
	uint32_t half_fifo_entry, tx_words, fifo_words;
	volatile uint32_t ser;
	uint32_t addr;
	uint32_t *word_data;
	uint8_t *byte_data;
	uint8_t cmd;

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;
	spi_para = dev->comp_param;

	if (4 > spi_para->spi_flash_tx_fifo_depth)
	return DW_EIO;

	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	// set ctrlr0: TX mode, data_ch, addr_ch
	init_data = spi_flash_map->ctrlr0;

	// Set imr: not masked TXEIR_PEND
	spi_flash_map->imr = spi_flash_map->imr | 0x200;

	// initial program command
	if (dev_info->cmd_info->wr_quad_type != WR_MULTI_NONE) {
		if (dev_info->cmd_info->wr_quad_type == WR_QUAD_II) {
			init_data = (spi_flash_map->ctrlr0 & 0xfff0fcff) | (0x000a0000);
			cmd = dev_info->cmd->ppx4_ii;
		} else if (dev_info->cmd_info->wr_quad_type == WR_QUAD_I) {
			init_data = (spi_flash_map->ctrlr0 & 0xfff0fcff) | (0x00080000);
			cmd = dev_info->cmd->ppx4_i;
		}
	} else if (dev_info->cmd_info->wr_dual_type != WR_MULTI_NONE) {
		if (dev_info->cmd_info->wr_quad_type == WR_DUAL_II) {
			init_data = (spi_flash_map->ctrlr0 & 0xfff0fcff) | (0x00050000);
			cmd = dev_info->cmd->ppx2_ii;
		} else if (dev_info->cmd_info->wr_quad_type == WR_DUAL_I) {
			init_data = (spi_flash_map->ctrlr0 & 0xfff0fcff) | (0x00040000);
			cmd = dev_info->cmd->ppx2_i;
		}
	} else {	// Single write
		init_data = spi_flash_map->ctrlr0 & 0xfff0fcff;
		cmd = dev_info->cmd->pp;
	}

	addr = target_addr;
	data_count = 0;

	flash_tx_cmd(dev, dev_info->cmd->wren);

	wr_block_bound = dev_info->cmd_info->wr_block_bound;

#if C_DEBUG
	printf("ori_bound_cound = 0x%x\n", wr_block_bound);
#endif

	page_bound_count = target_addr;

	addr = target_addr;
	word_times = byte_size >> 2;
	byte_times = source_addr & 0x3;
	byte_data = source_addr;

	// Start address is byte address not word address
	if ((byte_times != 0) && (wr_block_bound != 1)) {
#if C_DEBUG
		printf("printf byte start\n");
#endif

		flash_tx_cmd(dev, dev_info->cmd->wren);

		spi_flash_map->ssienr = 0;

		spi_flash_map->ctrlr0 = init_data;

		wr_addr = (cmd & 0x000000ff) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);

		// Write cmd, addr, data into FIFO
		spi_flash_map->dr[DR0].word = wr_addr;
		if (byte_times == 3) {
			spi_flash_map->dr[DR0].byte = byte_data[0];
			word_times = (byte_size - 1) >> 2;
		} else if (byte_times == 2) {
			spi_flash_map->dr[DR0].byte = byte_data[0];
			spi_flash_map->dr[DR0].byte = byte_data[1];
			word_times = (byte_size - 2) >> 2;
		} else if (byte_times == 1) {
			spi_flash_map->dr[DR0].byte = byte_data[0];
			spi_flash_map->dr[DR0].byte = byte_data[1];
			spi_flash_map->dr[DR0].byte = byte_data[2];
			word_times = (byte_size - 3) >> 2;
		}
		// Enable SPI_FLASH
		spi_flash_map->ssienr = 1;

		spi_flash_wait_busy(dev);

		flash_wait_busy(dev);

		// if page size ==1, addr= target_addr
		addr = target_addr & 0xfffffffc;
	}

	if (word_times != 0 || (wr_block_bound == 1)) {
#if C_DEBUG
		printf("printf word start\n");
#endif
		split_times = 0;
		half_fifo_entry = spi_para->spi_flash_tx_fifo_depth / 2;
		fifo_words = spi_para->spi_flash_tx_fifo_depth / 4;
		tx_words = half_fifo_entry / 4;
		// printf("half_entry = 0x%x, tx_words= 0x%x\n", half_fifo_entry,
		// tx_words);

		ser = spi_flash_map->ser;

		word_data = source_addr & 0xfffffffc;
		page_bound_count = source_addr & 0xfffffffc;

		while (1) {
			flash_tx_cmd(dev, dev_info->cmd->wren);

			spi_flash_map->ssienr = 0;

			// Initial data, addr channel bits
			spi_flash_map->ctrlr0 = init_data;

			wr_addr = (cmd & 0x000000ff) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);

			// Write cmd, addr, data into FIFO
			spi_flash_map->dr[DR0].word = wr_addr;
			if (wr_block_bound == 1) {  // Page size ==1
				spi_flash_map->dr[DR0].byte = byte_data[data_count];
				data_count = data_count + 1;
				page_bound_count = page_bound_count + 4;
			} else {
				// Push data to full FIFO
				for (wr_word_times = 0; (data_count < word_times) && (wr_word_times < fifo_words - 1); wr_word_times = wr_word_times + 1) {
					// data on page_bound && the data is not the first
					// data => it needs to split
					if (!((page_bound_count % wr_block_bound == 0) && wr_word_times != 0)) {
						spi_flash_map->dr[DR0].word = word_data[data_count];
						data_count = data_count + 1;
						page_bound_count = page_bound_count + 4;
					}
				}
				// Enable SER_PEND
				spi_flash_map->ctrlr2 = spi_flash_map->ctrlr2 | 0x08;
			}

			// data_count = data_count +1;
			// page_bound_count = page_bound_count+4;

			// Enable SPI_FLASH
			spi_flash_map->ssienr = 1;

			// Write data into FIFO, page_size !=9
			while ((wr_block_bound != 1) && (data_count < word_times) && (page_bound_count % wr_block_bound != 0)) {

				if (spi_flash_map->txflr <= half_fifo_entry) {
					for (wr_word_times = 0; (data_count < word_times) && (page_bound_count % wr_block_bound != 0) && (wr_word_times < tx_words); wr_word_times = wr_word_times + 1) {
						spi_flash_map->dr[DR0].word =
						word_data[data_count];
						data_count = data_count + 1;
						page_bound_count = page_bound_count + 4;
					}
				}
				// It is split to transmit.
				if ((spi_flash_map->isr & 0x200)) {
					spi_flash_wait_busy(dev);
					spi_flash_map->ssienr = 0;
					data_count = data_count - wr_word_times;
					page_bound_count = page_bound_count - (wr_word_times * 4);
					spi_flash_map->flush_fifo = 1;	  // flush FIFO
					spi_flash_map->icr = 1;

					// Disable wr_seq
					spi_flash_map->ctrlr2 = spi_flash_map->ctrlr2 & 0xfffffff7;
					spi_flash_map->ser = ser;   // Enable origial SER
					break;
				}
			}   // while data count< word_times && page_bound_count !=
			// page_size

			// BREAD conditions of transmit data and operations after
			// split data

			// Page size == 1 || Page size != 1
			if (((wr_block_bound == 1) && (data_count >= byte_size)) || ((wr_block_bound != 1) && (data_count >= word_times))) {
				break;
			} else if (wr_block_bound == 1) {
				addr = target_addr + data_count;
				spi_flash_wait_busy(dev);
			} else {	// if (page_size !=1 && data_count < word_times)
				// until the page write is finishing
				if ((page_bound_count % wr_block_bound) == 0) {
					spi_flash_wait_busy(dev);
					spi_flash_map->ssienr = 0;
					// Disable wr_seq
					spi_flash_map->ctrlr2 = spi_flash_map->ctrlr2 & 0xfffffff7;
				}
				// after split data || page_bound_data% wr_block_bound ==0
				split_times = split_times + 1;
				addr = target_addr + ((data_count) << 2);
			}
			flash_wait_busy(dev);
		} // while(1)
	} // if (page size != 1 || word_data ==0)

	spi_flash_wait_busy(dev);

	spi_flash_map->ssienr = 0;

	// Disable WR_SEQ
	spi_flash_map->ctrlr2 = spi_flash_map->ctrlr2 & 0xfffffff7;

	flash_wait_busy(dev);

#if C_DEBUG
	printf("Write_block: write_word finish.\n");
#endif
	byte_data = (source_addr + byte_size) & 0xfffffffc;
	byte_times = (source_addr + byte_size) & 0x3;

	if (byte_times != 0 && (wr_block_bound != 1)) {
#if C_DEBUG
	printf("printf byte end\n");
#endif

	flash_tx_cmd(dev, dev_info->cmd->wren);

	spi_flash_map->ssienr = 0;

	spi_flash_map->ctrlr0 = init_data;

	addr = (target_addr + byte_size) & 0xfffffffc;

	wr_addr = (cmd & 0x000000ff) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);

	// Write cmd, addr, data into FIFO
	spi_flash_map->dr[DR0].word = wr_addr;
	if (byte_times == 1)
		spi_flash_map->dr[DR0].byte = byte_data[0];
	else if (byte_times == 2) {
		spi_flash_map->dr[DR0].byte = byte_data[0];
		spi_flash_map->dr[DR0].byte = byte_data[1];
	} else if (byte_times == 3) {
		spi_flash_map->dr[DR0].byte = byte_data[0];
		spi_flash_map->dr[DR0].byte = byte_data[1];
		spi_flash_map->dr[DR0].byte = byte_data[2];
	}
	// Enable SPI_FLASH
	spi_flash_map->ssienr = 1;

	spi_flash_wait_busy(dev);

	flash_wait_busy(dev);
	}
#if PRINTF_ON
	if (split_times != 0)
	printf("Write_block function split: %d times.\n", split_times);
#endif

	return 0;
}


uint32_t
flash_read_id(struct dw_device * dev)
// uint32_t flash_read_id(struct dw_device *dev,unsigned int dummy_cycle)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;

	uint32_t rd_data, data;

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;

	_memctl_debug_printf("\r\n flash_read_id():spi_flash_map:0x%x\r\n", spi_flash_map);

	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	// Set Ctrlr1; 3 byte data frames
	spi_flash_setctrlr1(dev, 3);
	// spi_flash_map-> ctrlr1 =3;

	spi_flash_set_dummy_cycle(dev, 0);
	// spi_flash_set_dummy_cycle(dev, dummy_cycle);
	// spi_flash_map->auto_length=0x24330000;

	// flash_rx_cmd(dev, dev_info-> cmd-> rdid);
	flash_rx_cmd(dev, 0x9f);	// RDID

#if 1
	rd_data = spi_flash_map->dr[DR0].word;
	data = ((rd_data & 0x000000ff) << 24) | ((rd_data & 0x0000ff00) << 8) | ((rd_data & 0x00ff0000) >> 8) | ((rd_data & 0xff000000) >> 24);

	// return data;

	data = data >> 8;
	_memctl_debug_printf("\r\nflash_read_id():RDID data=0x%x\r\n", data);
	// _memctl_debug_printf("\r\nRrd_data =0x%x\r\n",rd_data );

	// _memctl_debug_printf("\r\nflash_read_id():dev_info-> cmd->
	// rdid:0x%x\r\n",dev_info-> cmd-> rdid);
	// _memctl_debug_printf("\r\n flash_read_id():spi_flash_map:0x%x\r\n",
	// spi_flash_map);
	// _memctl_debug_printf("\r\n
	// flash_read_id():*spi_flash_map:0x%x\r\n", *spi_flash_map);

	return data;
#endif
}



void
flash_enter_dp(struct dw_device *dev, uint8_t cmd)
{
	struct spi_flash_portmap *spi_flash_map;
	uint32_t rd_data;

	spi_flash_map = dev->base_address;

	// Disble SPI_FLASH
	spi_flash_map->ssienr = 0;

	// set ctrlr0: TX mode
	rd_data = spi_flash_map->ctrlr0;
	spi_flash_map->ctrlr0 = (rd_data & 0xfff0fcff);

	// set flash_cmd: wren to fifo
	spi_flash_setdr(dev, DR0, cmd, DATA_BYTE);

	// Enable SPI_FLASH
	spi_flash_map->ssienr = 1;

	spi_flash_wait_busy(dev);

}

void
flash_write(struct dw_device *dev, uint32_t addr, uint32_t data, enum spi_flash_byte_num byte_num)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;

	uint32_t wr_addr, init_data;

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;

	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	flash_tx_cmd(dev, dev_info->cmd->wren);

	wr_addr = ((dev_info->cmd->pp) & 0x000000ff) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);

	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	// set ctrlr0: TX mode, data_ch, addr_ch
	// DW_BITS_SET_VAL(spi_flash_map->ctrlr0, bfoSPI_FLASH_CTRLR0_TMOD,
	// 0, bfwSPI_FLASH_CTRLR0_TMOD);
	init_data = spi_flash_map->ctrlr0;
	spi_flash_map->ctrlr0 = (init_data & 0xfff0fcff);

	// Write cmd, addr, data into FIFO
	spi_flash_map->dr[DR0].word = wr_addr;
	if (byte_num == DATA_BYTE)
		spi_flash_setdr(dev, DR0, data, DATA_BYTE);
	else if (byte_num == DATA_HALF)
		spi_flash_setdr(dev, DR0, data, DATA_HALF);
	else if (byte_num == DATA_WORD)
		spi_flash_setdr(dev, DR0, data, DATA_WORD);

	// Enable SPI_FLASH
	spi_flash_map->ssienr = 1;

	spi_flash_wait_busy(dev);

	flash_wait_busy(dev);
}

#endif

uint32_t
flash_read(struct dw_device *dev, uint32_t addr, enum spi_flash_byte_num byte_num)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;

	uint32_t wr_addr, rd_data, ctrlr0;
	rd_data = 0;

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;

	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	// set ctrlr0: RX_mode
	ctrlr0 = spi_flash_map->ctrlr0;
	spi_flash_map->ctrlr0 = (ctrlr0 & 0xfff0fcff) | (0x00000300);

#if CONFIG_SPI_DW_BITS_SET_VAL
	// Set tuning dummy cycles
	DW_BITS_SET_VAL(spi_flash_map->auto_length, bfoSPI_FLASH_AUTO_LEN_DUM, dev_info->cmd_info->tunning_dummy, bfwSPI_FLASH_AUTO_LEN_DUM);
#endif

	// Set Ctrlr1; 1 byte data frames
	if (byte_num == DATA_BYTE)
		spi_flash_setctrlr1(dev, 1);
	else if (byte_num == DATA_HALF)
		spi_flash_setctrlr1(dev, 2);
	else if (byte_num == DATA_WORD)
		spi_flash_setctrlr1(dev, 4);

	/*
	 * 3-byte addressing setting
	 */
	if (((spi_flash_map->addr_length) & 0x3) == 0x3) {
		// read command
		wr_addr = (0x00000003) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);
		// Write cmd, addr, data into FIFO
		spi_flash_map->dr[DR0].word = wr_addr;
	}
#if CONFIG_SPI_3to4BYTES_ADDRESS_SUPPORT
	/*
	 * 4 byte addressing
	 */
	// if ( (spi_flash_map-> addr_length)&(0x3)==0 )
	else
	{
		wr_addr = ((addr & 0xff000000) >> 24) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);
		// printf("\n(4 byte addressing) wr_addr =0x%x\n", wr_addr );

		spi_flash_setdr(dev, DR0, (dev_info->cmd->read_4b), DATA_BYTE);	  // send read
							// command (0x3)
		// Write cmd, addr, data into FIFO
		spi_flash_setdr(dev, DR0, wr_addr, DATA_WORD);
	}
#endif

	// Enable SPI_FLASH
	spi_flash_map->ssienr = 1;

	spi_flash_wait_busy(dev);

	// Disble SPI_FLASH
	spi_flash_map->ssienr = 0;

	if (byte_num == DATA_BYTE)
		rd_data = spi_flash_getdr(dev, DR0, DATA_BYTE);
	else if (byte_num == DATA_HALF)
		rd_data = spi_flash_getdr(dev, DR0, DATA_HALF);
	else if (byte_num == DATA_WORD)
		rd_data = spi_flash_getdr(dev, DR0, DATA_WORD);

	// set ctrlr0: TX mode
	ctrlr0 = spi_flash_map->ctrlr0;
	spi_flash_map->ctrlr0 = (ctrlr0 & 0xfff0fcff);

	return rd_data;
}

void
flash_write_words(struct dw_device *dev, uint32_t target_addr, uint32_t source_addr, uint32_t data_num)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;

	uint32_t wr_addr, init_data, times;
	uint32_t *source_data;

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;

	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	flash_tx_cmd(dev, dev_info->cmd->wren);


	if (((spi_flash_map->addr_length) & 0x3) == 0x3) {
		wr_addr = ((dev_info->cmd->pp) & 0x000000ff) | ((target_addr & 0x000000ff) << 24) | ((target_addr & 0x0000ff00) << 8) | ((target_addr & 0x00ff0000) >> 8);
	}
#if CONFIG_SPI_3to4BYTES_ADDRESS_SUPPORT

	/*
	 * 4 byte addressing
	 */
	// if ( (spi_flash_map-> addr_length)&(0x3)==0 )
	else
	{
		wr_addr = ((target_addr & 0xff000000) >> 24) | ((target_addr & 0x000000ff) << 24) | ((target_addr & 0x0000ff00) << 8) | ((target_addr & 0x00ff0000) >> 8);
	// printf("\n(4 byte addressing) wr_addr =0x%x\n", wr_addr );
	}
#endif
	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	// set ctrlr0: TX mode, data_ch, addr_ch
	// DW_BITS_SET_VAL(spi_flash_map->ctrlr0, bfoSPI_FLASH_CTRLR0_TMOD,
	// 0, bfwSPI_FLASH_CTRLR0_TMOD);
	init_data = spi_flash_map->ctrlr0;
	spi_flash_map->ctrlr0 = (init_data & 0xfff0fcff);

	// Write cmd, addr, data into FIFO
	if (((spi_flash_map->addr_length) & 0x3) == 0x3) {
		spi_flash_map->dr[DR0].word = wr_addr;
	}
#if CONFIG_SPI_3to4BYTES_ADDRESS_SUPPORT
	else {
		spi_flash_setdr(dev, DR0, (dev_info->cmd->pp_4b), DATA_BYTE);
		// Write cmd, addr, data into FIFO
		spi_flash_setdr(dev, DR0, wr_addr, DATA_WORD);
	}
#endif
	source_data = 0;
	source_data = (source_addr & 0xfffffffc);

	for (times = 0; times < data_num; times = times + 1) {
		spi_flash_map->dr[DR0].word = source_data[times];
		// printf("\nsource_data[%d]=0x%x\n",times,source_data[times]);
	}

	// Enable SPI_FLASH
	spi_flash_map->ssienr = 1;

	spi_flash_wait_busy(dev);

	flash_wait_busy(dev);
}

void
flash_read_bytes(struct dw_device *dev, uint32_t addr, uint32_t data_num)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;

	uint32_t wr_addr, rd_data;
	uint8_t		 cmd;

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;

	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	// set ctrlr0: RX_mode
	rd_data = spi_flash_map->ctrlr0;

	// Set tuning dummy cycles
	if (dev_info->cmd_info->rd_quad_type != RD_MULTI_NONE) {
		spi_flash_set_dummy_cycle(dev, dev_info->cmd_info->rd_quad_dummy);
		if (dev_info->cmd_info->rd_quad_type == RD_QUAD_IO) {
			spi_flash_map->ctrlr0 = (rd_data & 0xfff0fcff) | (0x000a0300);
			cmd = dev_info->cmd->readx4_io;
		} else {
			// RD_QUAD_O
			spi_flash_map->ctrlr0 = (rd_data & 0xfff0fcff) | (0x00080300);
			cmd = dev_info->cmd->readx4_o;
		}
	} else if (dev_info->cmd_info->rd_dual_type != RD_MULTI_NONE) {
		spi_flash_set_dummy_cycle(dev, dev_info->cmd_info->rd_dual_dummy);
		if (dev_info->cmd_info->rd_dual_type == RD_DUAL_IO) {
			spi_flash_map->ctrlr0 = (rd_data & 0xfff0fcff) | (0x00050300);
			cmd = dev_info->cmd->readx2_io;
		} else {		// RD_DUAL_I
			spi_flash_map->ctrlr0 = (rd_data & 0xfff0fcff) | (0x00040300);
			cmd = dev_info->cmd->readx2_o;
		}
	} else {
		DW_BITS_SET_VAL(spi_flash_map->auto_length, bfoSPI_FLASH_AUTO_LEN_DUM, dev_info->cmd_info->tunning_dummy, bfwSPI_FLASH_AUTO_LEN_DUM);
		spi_flash_map->ctrlr0 = (rd_data & 0xfff0fcff) | (0x00000300);
		cmd = 0x3;
	}

	// Set Ctrlr1; data frames
	spi_flash_map->ctrlr1 = data_num;

	wr_addr = (cmd) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);

	// Write cmd, addr, data into FIFO
	spi_flash_map->dr[DR0].word = wr_addr;

	// Enable SPI_FLASH
	spi_flash_map->ssienr = 1;

	spi_flash_wait_busy(dev);

	// Disble SPI_FLASH
	spi_flash_map->ssienr = 0;
}


#if SUPPORT_SPI_SIO_FASTREAD
uint32_t
flash_fastread(struct dw_device *dev, uint32_t addr, enum spi_flash_byte_num byte_num)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;

	uint32_t wr_addr, rd_data;

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;

	if (dev_info->cmd_info->fast_rd_dummy != 0)
		spi_flash_set_dummy_cycle(dev, dev_info->cmd_info->fast_rd_dummy);
	else {
		DW_BITS_SET_VAL(spi_flash_map->auto_length, bfoSPI_FLASH_AUTO_LEN_DUM, dev_info->cmd_info->tunning_dummy, bfwSPI_FLASH_AUTO_LEN_DUM);
	}

	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	// set ctrlr0: RX_mode
	rd_data = spi_flash_map->ctrlr0;
	spi_flash_map->ctrlr0 = (rd_data & 0xfff0fcff) | (0x00000300);

	// Set Ctrlr1; 1 byte data frames
	if (byte_num == DATA_BYTE)
		spi_flash_setctrlr1(dev, 1);
	else if (byte_num == DATA_HALF)
		spi_flash_setctrlr1(dev, 2);
	else if (byte_num == DATA_WORD)
		spi_flash_setctrlr1(dev, 4);

	/*
	 * 3-byte addressing setting
	 */
	if ((spi_flash_map->addr_length) & (0x3) == 0x3) {
		wr_addr = (dev_info->cmd->fast_read) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);

		// Write cmd, addr, data into FIFO
		spi_flash_map->dr[DR0].word = wr_addr;
	}
#if CONFIG_SPI_3to4BYTES_ADDRESS_SUPPORT
	/*
	 * 4 byte addressing
	 */
	// if ( (spi_flash_map-> addr_length)&(0x3)==0 )
	else
	{
		wr_addr = ((addr & 0xff000000) >> 24) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);
		// printf("\n(4 byte addressing) wr_addr =0x%x\n", wr_addr );

		spi_flash_setdr(dev, DR0, (dev_info->cmd->fast_read), DATA_BYTE);
		// Write cmd, addr, data into FIFO
		spi_flash_setdr(dev, DR0, wr_addr, DATA_WORD);
	}
#endif

	// Enable SPI_FLASH
	spi_flash_map->ssienr = 1;

	spi_flash_wait_busy(dev);

	if (byte_num == DATA_BYTE)
		return spi_flash_getdr(dev, DR0, DATA_BYTE);
	else if (byte_num == DATA_HALF)
		return spi_flash_getdr(dev, DR0, DATA_HALF);
	else if (byte_num == DATA_WORD)
		return spi_flash_getdr(dev, DR0, DATA_WORD);
}
#endif



#ifdef SUPPORT_SPI_DIO  // DIO read , SIO write
uint32_t
flash_writex2(struct dw_device * dev, uint32_t addr, uint32_t data, enum spi_flash_byte_num byte_num)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;

	uint32_t wr_addr;
	uint8_t cmd;

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;

	// Not support writex2
	if (dev_info->cmd_info->wr_dual_type == WR_MULTI_NONE) {
#if PRINTF_ON
	printf("Not support Writex2 command.\n");
#endif
	return DW_EPERM;
	}
	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	flash_tx_cmd(dev, dev_info->cmd->wren);

	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	// set ctrlr0: TX mode
	data = spi_flash_map->ctrlr0;

	if (dev_info->cmd_info->wr_quad_type == WR_DUAL_II) {
		spi_flash_map->ctrlr0 = (data & 0xfff0fcff) | (0x00050000);
		cmd = dev_info->cmd->ppx2_ii;
	} else {
		// WR_DUAL_I
		spi_flash_map->ctrlr0 = (data & 0xfff0fcff) | (0x00040000);
		cmd = dev_info->cmd->ppx2_i;
	}

	/*
	 * 3-byte addressing setting
	 */
	if ((spi_flash_map->addr_length) & (0x3) == 0x3) {
		wr_addr = (cmd & 0x000000ff) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);

		// Write cmd, addr, data into FIFO
		spi_flash_map->dr[DR0].word = wr_addr;
	}
#if CONFIG_SPI_3to4BYTES_ADDRESS_SUPPORT
	/*
	 * 4 byte addressing
	 */
	// if ( (spi_flash_map-> addr_length)&(0x3)==0 )
	else
	{
		wr_addr = ((addr & 0xff000000) >> 24) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);
		// printf("\n(4 byte addressing) wr_addr =0x%x\n", wr_addr );

		spi_flash_setdr(dev, DR0, cmd, DATA_BYTE);
		// Write cmd, addr, data into FIFO
		spi_flash_setdr(dev, DR0, wr_addr, DATA_WORD);
	}
#endif

	if (byte_num == DATA_BYTE)
		spi_flash_setdr(dev, DR0, data, DATA_BYTE);
	else if (byte_num == DATA_HALF)
		spi_flash_setdr(dev, DR0, data, DATA_HALF);
	else if (byte_num == DATA_WORD)
		spi_flash_setdr(dev, DR0, data, DATA_WORD);

	// Enable SPI_FLASH
	spi_flash_map->ssienr = 1;

	spi_flash_wait_busy(dev);

	flash_wait_busy(dev);

	return 0;
}


uint32_t
flash_readx2(struct dw_device * dev, uint32_t addr, enum spi_flash_byte_num byte_num)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;

	uint32_t wr_addr, rd_data, ctrlr0;
	uint8_t cmd;

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;

	if (dev_info->cmd_info->rd_dual_dummy != 0)
		spi_flash_set_dummy_cycle(dev, dev_info->cmd_info->rd_dual_dummy);
	else {
		DW_BITS_SET_VAL(spi_flash_map->auto_length, bfoSPI_FLASH_AUTO_LEN_DUM, dev_info->cmd_info->tunning_dummy, bfwSPI_FLASH_AUTO_LEN_DUM);
	}

	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	// set ctrlr0: RX_mode
	ctrlr0 = spi_flash_map->ctrlr0;
	// if (dev_info-> cmd_info ->rd_dual_type == RD_DUAL_O ) {
	if (dev_info->cmd_info->rd_dual_type == RD_DUAL_IO) {
		// addrx2
		spi_flash_map->ctrlr0 = (ctrlr0 & 0xfff0fcff) | (0x00050300);
		cmd = dev_info->cmd->readx2_io;
		// printf("\nflash_readx2-001-spi_flash_map->ctrlr0=0x%x\n",spi_flash_map->ctrlr0);
	} else {
		// RD_DUAL_I //addrx1
		spi_flash_map->ctrlr0 = (ctrlr0 & 0xfff0fcff) | (0x00040300);
		cmd = dev_info->cmd->readx2_o;
		// printf("\nflash_readx2-002-spi_flash_map->ctrlr0=0x%x\n",spi_flash_map->ctrlr0);
	}

	// Set Ctrlr1; 1 byte data frames
	if (byte_num == DATA_BYTE)
		spi_flash_setctrlr1(dev, 1);
	else if (byte_num == DATA_HALF)
		spi_flash_setctrlr1(dev, 2);
	else if (byte_num == DATA_WORD)
		spi_flash_setctrlr1(dev, 4);


	/*
	 * 3-byte addressing setting
	 */
	if ((spi_flash_map->addr_length) & (0x3) == 0x3) {
		wr_addr = (cmd & 0x000000ff) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);
		// Write cmd, addr, data into FIFO
		spi_flash_map->dr[DR0].word = wr_addr;
	}
#if CONFIG_SPI_3to4BYTES_ADDRESS_SUPPORT
	/*
	 * 4 byte addressing
	 */
	// if ( (spi_flash_map-> addr_length)&(0x3)==0 )
	else
	{
#if 1
		wr_addr = ((addr & 0xff000000) >> 24) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);
#endif
	// printf("\n(4 byte addressing) wr_addr =0x%x\n", wr_addr );

	spi_flash_setdr(dev, DR0, cmd, DATA_BYTE);
	// Write cmd, addr, data into FIFO
	spi_flash_setdr(dev, DR0, wr_addr, DATA_WORD);
	}
#endif
	// Enable SPI_FLASH
	spi_flash_map->ssienr = 1;

	spi_flash_wait_busy(dev);

	// Disble SPI_FLASH
	spi_flash_map->ssienr = 0;

	if (byte_num == DATA_BYTE)
		rd_data = spi_flash_getdr(dev, DR0, DATA_BYTE);
	else if (byte_num == DATA_HALF)
		rd_data = spi_flash_getdr(dev, DR0, DATA_HALF);
	else if (byte_num == DATA_WORD)
		rd_data = spi_flash_getdr(dev, DR0, DATA_WORD);

	// set ctrlr0: TX mode
	ctrlr0 = spi_flash_map->ctrlr0;
	spi_flash_map->ctrlr0 = (ctrlr0 & 0xfff0fcff);

	return rd_data;
}
#endif


#ifdef SUPPORT_SPI_QIO
uint32_t
flash_writex4(struct dw_device * dev, uint32_t addr, uint32_t data, enum spi_flash_byte_num byte_num)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;

	uint32_t wr_addr, init_data;
	uint8_t cmd;

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;

	// Not support writex4
	if (dev_info->cmd_info->wr_quad_type == WR_MULTI_NONE) {
#if 1   // PRINTF_ON
		printf("Not support Writex4 command.\n");
#endif
		return DW_EPERM;
	}
	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	flash_tx_cmd(dev, dev_info->cmd->wren);

	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	// set ctrlr0: TX mode, data_ch, addr_ch
	init_data = spi_flash_map->ctrlr0;
	if (dev_info->cmd_info->wr_quad_type == WR_QUAD_II) {
		spi_flash_map->ctrlr0 = (init_data & 0xfff0fcff) | (0x000a0000);
		cmd = dev_info->cmd->ppx4_ii;
	} else {
		// WR_QUAD_I
		spi_flash_map->ctrlr0 = (init_data & 0xfff0fcff) | (0x00080000);
		cmd = dev_info->cmd->ppx4_i;
	}

	/*
	 * 3-byte addressing setting
	 */
	if ((spi_flash_map->addr_length) & (0x3) == 0x3) {
		wr_addr = (cmd & 0x000000ff) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);

		// Write cmd, addr, data into FIFO
		spi_flash_map->dr[DR0].word = wr_addr;
	}
#if CONFIG_SPI_3to4BYTES_ADDRESS_SUPPORT
	/*
	 * 4 byte addressing
	 */
	// if ( (spi_flash_map-> addr_length)&(0x3)==0 )
	else
	{
		wr_addr = ((addr & 0xff000000) >> 24) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);
		// printf("\n(4 byte addressing) wr_addr =0x%x\n", wr_addr );

		spi_flash_setdr(dev, DR0, cmd, DATA_BYTE);
		// Write cmd, addr, data into FIFO
		spi_flash_setdr(dev, DR0, wr_addr, DATA_WORD);
	}
#endif

	if (byte_num == DATA_BYTE)
		spi_flash_setdr(dev, DR0, data, DATA_BYTE);
	else if (byte_num == DATA_HALF)
		spi_flash_setdr(dev, DR0, data, DATA_HALF);
	else if (byte_num == DATA_WORD)
		spi_flash_setdr(dev, DR0, data, DATA_WORD);

	// Enable SPI_FLASH
	spi_flash_map->ssienr = 1;

	spi_flash_wait_busy(dev);

	flash_wait_busy(dev);

	return 0;
}


uint32_t
flash_readx4(struct dw_device * dev, uint32_t addr, enum spi_flash_byte_num byte_num)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;

	uint32_t wr_addr, rd_data, ctrlr0;
	uint8_t cmd;

	spi_flash_map = dev->base_address;
	dev_info = dev->instance;

	if (dev_info->cmd_info->rd_dual_dummy != 0) {
		spi_flash_set_dummy_cycle(dev, dev_info->cmd_info->rd_quad_dummy);
		// printf("\ndev_info-> cmd_info->rd_quad_dummy=0x%x\n",dev_info->
		// cmd_info->rd_quad_dummy);
	} else {
		DW_BITS_SET_VAL(spi_flash_map->auto_length, bfoSPI_FLASH_AUTO_LEN_DUM, dev_info->cmd_info->tunning_dummy, bfwSPI_FLASH_AUTO_LEN_DUM);
		// printf("\ndev_info-> cmd_info->tunning_dummy=0x%x\n",dev_info->
		// cmd_info->tunning_dummy);
	}

	// Disable SPI_FLASH
	spi_flash_map->ssienr = 0;

	// set ctrlr0: RX_mode
	ctrlr0 = spi_flash_map->ctrlr0;
	if (dev_info->cmd_info->rd_quad_type == RD_QUAD_IO) {
		spi_flash_map->ctrlr0 = (ctrlr0 & 0xfff0fcff) | (0x000a0300);
		cmd = dev_info->cmd->readx4_io;
	} else {	// RD_QUAD_O
		spi_flash_map->ctrlr0 = (ctrlr0 & 0xfff0fcff) | (0x00080300);
		cmd = dev_info->cmd->readx4_o;
	}

	// Set Ctrlr1; 1 byte data frames
	if (byte_num == DATA_BYTE)
		spi_flash_setctrlr1(dev, 1);
	else if (byte_num == DATA_HALF)
		spi_flash_setctrlr1(dev, 2);
	else if (byte_num == DATA_WORD)
		spi_flash_setctrlr1(dev, 4);

	/*
	 * 3-byte addressing setting
	 */
	if ((spi_flash_map->addr_length) & (0x3) == 0x3) {
		wr_addr = (cmd & 0x000000ff) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);

		// Write cmd, addr, data into FIFO
		spi_flash_map->dr[DR0].word = wr_addr;
	}
#if CONFIG_SPI_3to4BYTES_ADDRESS_SUPPORT
	/*
	 * 4 byte addressing
	 */
	// if ( (spi_flash_map-> addr_length)&(0x3)==0 )
	else
	{
#if 1
		wr_addr = ((addr & 0xff000000) >> 24) | ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) | ((addr & 0x00ff0000) >> 8);
#endif
		// printf("\n(JJJ 4 byte addressing) wr_addr =0x%x\n", wr_addr );

		spi_flash_setdr(dev, DR0, cmd, DATA_BYTE);
		// Write cmd, addr, data into FIFO
		spi_flash_setdr(dev, DR0, wr_addr, DATA_WORD);
	}
#endif


	// Enable SPI_FLASH
	spi_flash_map->ssienr = 1;

	spi_flash_wait_busy(dev);

	// Disble SPI_FLASH
	spi_flash_map->ssienr = 0;

	if (byte_num == DATA_BYTE)
		rd_data = spi_flash_getdr(dev, DR0, DATA_BYTE);
	else if (byte_num == DATA_HALF)
		rd_data = spi_flash_getdr(dev, DR0, DATA_HALF);
	else if (byte_num == DATA_WORD)
		rd_data = spi_flash_getdr(dev, DR0, DATA_WORD);

	// set ctrlr0: TX mode, signal channel
	ctrlr0 = spi_flash_map->ctrlr0;
	spi_flash_map->ctrlr0 = (ctrlr0 & 0xfff0fcff);

	return rd_data;
}
#endif



#if 1

// //////////////////////////////////////////////////////////////////////
/*
 * Uboot command of SPIC auto mode init
 */
// //////////////////////////////////////////////////////////////////////

static int
SPIC_AUTO_MODE_INIT(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{

	spic_init_auto_mode(&dev);

	return CMD_RET_SUCCESS;
}



U_BOOT_CMD(spicat, 1, 0, SPIC_AUTO_MODE_INIT,
	   "spicat  : SPI Controller auto mode init\n", "");



#define CONFIG_AVOID_ERROR_INPUT_HANG 1


// //////////////////////////////////////////////////////////////////////
/*
 * Uboot command of SPI flash chip erase
 */
// //////////////////////////////////////////////////////////////////////

static int
SPI_CHIP_ERASE(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	// dram_normal_patterns();
#if CONFIG_AVOID_ERROR_INPUT_HANG
	if (argv[1]) {
		/*
		 * Avoid error input format hang
		 */
	} else {
		printf("Wrong argument number!\r\n");
		printf("Usage:spice<chip_select>\r\n");
		return CMD_RET_FAILURE;
	}
#endif


	/*
	 * Select cs0 or cs1 for user mode , auto mode's setting is
	 * FLASH_SIZE(offset + 0x124)
	 */
#if 0
	u32  chip_select = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	SPI_ChipSelect(chip_select);
#endif
	SPI_ChipSelect(chip_select);

	printf("\nSPI Flash#%x chip erase\n", chip_select);



	flash_chip_erase(&dev);


	return CMD_RET_SUCCESS;
}



U_BOOT_CMD(spice, 2, 0, SPI_CHIP_ERASE,
	   "spice <chip_select> : SPI flash chip erase\n", "");


#ifdef SUPPORT_SPI_SIO

#if SUPPORT_SPI_SIO_FASTREAD
// //////////////////////////////////////////////////////////////////////
/*
 * Uboot command of fast read data from SPI flash to DRAM
 */
// //////////////////////////////////////////////////////////////////////
// int SPIcmp(int dst1 , char* src1,int length1)
int
SPIFastRead(char *src1, char *dst1, unsigned int length1)
{
	volatile unsigned int i;
	unsigned int dst, src;
	unsigned int dst_value, src_value;
	unsigned int length;
	unsigned int error;

	printf("\nSPIFastRead:dst1=0x%x,src1=0x%x,length1=0x%x\n", dst1, src1, length1);

	// dst = dst1+0xbd000000;
	dst = dst1;
	src = src1;
	length = length1;

	unsigned int readData = 0;

	// auto mode read loop
	for (i = 0; i < length; i += 4) {
	if ((i + SPI_SECTOR_SIZE) % SPI_SECTOR_SIZE == 0)
		printf(".");

	*(volatile unsigned int *) (dst + i) = flash_fastread(&dev, (src + i) % FLASH_BASE, DATA_WORD);
	// *(volatile unsigned int *)(dst+i)=flash_fastread(&dev, i,
	// DATA_WORD);
	// readData=flash_read(&dev, (src+i)%FLASH_BASE, DATA_WORD);

	// printf("\nreadData=0x%x\n", readData);
	}
	printf("\n");
}


static int
SPIFastReadx1(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	// dram_normal_patterns();

#if CONFIG_AVOID_ERROR_INPUT_HANG
	if ((argv[1]) && (argv[2]) && (argv[3]) && (argv[4])) {
		/*
		 * Avoid error input format hang
		 */
	} else {
		printf("Wrong argument number!\r\n");
		printf("Usage:spifrx1<flash_src_addr><dram_dst_addr><length><chip_select> in hex\r\n");
		return CMD_RET_FAILURE;
	}
#endif

	unsigned int flash_src_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	unsigned int dram_dest_addr = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);
	unsigned int length = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);

	/*
	 * Select cs0 or cs1 for user mode , auto mode's setting is
	 * FLASH_SIZE(offset + 0x124)
	 */
#if 0
	u32 chip_select = Strtoul((const u8 *) (argv[4]), (u8 **) NULL, 16);
	SPI_ChipSelect(chip_select);

#endif
	SPI_ChipSelect(chip_select);

#if 0
	if (length % SPI_SECTOR_SIZE != 0) {
		length = length + (SPI_SECTOR_SIZE - (length % SPI_SECTOR_SIZE));
	}
#endif


#if 1

	if (argc < 5) {
		printf("Wrong argument number!\r\n");
		printf("Usage:spifrx1<flash_src_addr><dram_dst_addr><length><chip_select> in hex\r\n");

		return CMD_RET_FAILURE;
	}

	printf("SPI Flash#%x  will read 0x%X length from 0x%X to 0x%X\n", chip_select, length, flash_src_addr, dram_dest_addr);
#endif
	SPIFastRead(flash_src_addr, dram_dest_addr, length);

	return CMD_RET_SUCCESS;
}


U_BOOT_CMD(spifrx1, 5, 0, SPIFastReadx1,
	   "spifrx1 <flash_src_addr><dram_dst_addr><length><chip_select>  : SPI flash fast readx1 by user-mode\n", "");
// //////////////////////////////////////////////////////////////////////
#endif



// //////////////////////////////////////////////////////////////////////
/*
 * Uboot command of readx1 DRAM data to SPI flash , user mode
 */
// //////////////////////////////////////////////////////////////////////
// int SPIcmp(int dst1 , char* src1,int length1)
int
SPIreadWord(char *src1, char *dst1, unsigned int length1)
{
	volatile unsigned int i;
	unsigned int dst, src;
	unsigned int dst_value, src_value;
	unsigned int length;
	unsigned int error;

	printf("\nSPIreadWordx1:dst1=0x%x,src1=0x%x,length1=0x%x\n", dst1, src1, length1);

	// dst = dst1+0xbd000000;
	dst = dst1;
	src = src1;
	length = length1;

	// unsigned int readData=0;

	for (i = 0; i < length; i += 4) {
		if ((i + SPI_SECTOR_SIZE) % SPI_SECTOR_SIZE == 0)
			printf(".");

		*(volatile unsigned int *) (dst + i) = flash_read(&dev, (src + i) % FLASH_BASE, DATA_WORD);
		// *(volatile unsigned int *)(dst+i)=flash_read(&dev, i,
		// DATA_WORD);

		// printf("\nreadData=0x%x\n", readData);
	}
	printf("\n");
	return CMD_RET_SUCCESS;
}


static int
SPI_readx1(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	// dram_normal_patterns();

#if CONFIG_AVOID_ERROR_INPUT_HANG
	if ((argv[1]) && (argv[2]) && (argv[3]) && (argv[4])) {
		/*
		 * Avoid error input format hang
		 */
	} else {
		printf("Wrong argument number!\r\n");
		printf("Usage:spirdx1<flash_src_addr><dram_dst_addr><length><chip_select> in hex\r\n");
		return CMD_RET_FAILURE;
	}
#endif

	unsigned int flash_src_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	unsigned int dram_dest_addr = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);
	unsigned int length = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);

	/*
	 * Select cs0 or cs1 for user mode , auto mode's setting is
	 * FLASH_SIZE(offset + 0x124)
	 */
#if 0
	u32 chip_select = Strtoul((const u8 *) (argv[4]), (u8 **) NULL, 16);
	SPI_ChipSelect(chip_select);
#endif
	SPI_ChipSelect(chip_select);

#if 0
	if (length % SPI_SECTOR_SIZE != 0) {
		length = length + (SPI_SECTOR_SIZE - (length % SPI_SECTOR_SIZE));
	}
#endif

	printf("SPI Flash#%x  will read 0x%X length from 0x%X to 0x%X\n", chip_select, length, flash_src_addr, dram_dest_addr);

	SPIreadWord(flash_src_addr, dram_dest_addr, length);

	return CMD_RET_SUCCESS;
}



U_BOOT_CMD(spirdx1, 5, 0, SPI_readx1,
	   "spirdx1 <flash_src_addr><dram_dst_addr><length><chip_select>  : SPI flash readx1 by user-mode\n",
	   "");
// //////////////////////////////////////////////////////////////////////
#endif

#ifdef SUPPORT_SPI_DIO
// //////////////////////////////////////////////////////////////////////
/*
 * Uboot command of readx2 DRAM data to SPI flash , user mode
 */
// //////////////////////////////////////////////////////////////////////
// int SPIcmp(int dst1 , char* src1,int length1)
int
SPIreadWordx2(char *src1, char *dst1, unsigned int length1)
{
	volatile unsigned int i;
	unsigned int dst, src;
	unsigned int dst_value, src_value;
	unsigned int length;
	unsigned int error;

	printf("\nSPIreadWordx2:dst1=0x%x,src1=0x%x,length1=0x%x\n", dst1, src1, length1);

#if 0
	if (argc < 3) {
		printf("Parameters not enough!\n");
		return 1;
	}

#endif
	// dst = dst1+0xbd000000;
	dst = dst1;
	src = src1;
	length = length1;


	unsigned int readData = 0;;


	for (i = 0; i < length; i += 4) {
		if ((i + SPI_SECTOR_SIZE) % SPI_SECTOR_SIZE == 0)
			printf(".");

		*(volatile unsigned int *) (dst + i) = flash_readx2(&dev, (src + i) % FLASH_BASE, DATA_WORD);
		// *(volatile unsigned int *)(dst+i)=flash_readx2(&dev, i,
		// DATA_WORD);
	}
	printf("\n");
}


static int
SPI_readx2(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
#if CONFIG_AVOID_ERROR_INPUT_HANG
	if ((argv[1]) && (argv[2]) && (argv[3]) && (argv[4])) {
		/*
		 * Avoid error input format hang
		 */
	} else {
		printf("Wrong argument number!\r\n");
		printf("Usage:spirdx2<flash_src_addr><dram_dst_addr><length><chip_select> in hex\r\n");
		return CMD_RET_FAILURE;
	}
#endif

	unsigned int flash_src_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	unsigned int dram_dest_addr = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);
	unsigned int length = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);

	/*
	 * Select cs0 or cs1 for user mode , auto mode's setting is
	 * FLASH_SIZE(offset + 0x124)
	 */
#if 0
	u32 chip_select = Strtoul((const u8 *) (argv[4]), (u8 **) NULL, 16);
	SPI_ChipSelect(chip_select);
#endif
	SPI_ChipSelect(chip_select);

#if 0
	if (length % SPI_SECTOR_SIZE != 0) {
		length = length + (SPI_SECTOR_SIZE - (length % SPI_SECTOR_SIZE));
	}
#endif

	printf("SPI Flash#%x  will read 0x%X length from 0x%X to 0x%X\n", chip_select, length, flash_src_addr, dram_dest_addr);

	SPIreadWordx2(flash_src_addr, dram_dest_addr, length);

	return CMD_RET_SUCCESS;
}



U_BOOT_CMD(spirdx2, 5, 0, SPI_readx2,
	   "spirdx2 <flash_src_addr><dram_dst_addr><length><chip_select>  : SPI flash readx2 by user-mode\n", "");
// //////////////////////////////////////////////////////////////////////
#endif




#ifdef SUPPORT_SPI_QIO
// //////////////////////////////////////////////////////////////////////
/*
 * Uboot command of readx4 DRAM data to SPI flash , user mode
 */
// //////////////////////////////////////////////////////////////////////
// int SPIcmp(int dst1 , char* src1,int length1)
int
SPIreadWordx4(char *src1, char *dst1, unsigned int length1
)
{
	volatile unsigned int i;
	unsigned int	dst, src;
	unsigned int	dst_value, src_value;
	unsigned int	length;
	unsigned int	error;

	printf("\nSPIreadWordx4:dst1=0x%x,src1=0x%x,length1=0x%x\n", dst1, src1, length1);

	// dst = dst1+0xbd000000;
	dst = dst1;
	src = src1;
	length = length1;

	unsigned int	readData = 0;

	for (i = 0; i < length; i += 4) {
		if ((i + SPI_SECTOR_SIZE) % SPI_SECTOR_SIZE == 0)
			printf(".");

		*(volatile unsigned int *) (dst + i) = flash_readx4(&dev, (src + i) % FLASH_BASE, DATA_WORD);
		// *(volatile unsigned int *)(dst+i)=flash_readx4(&dev, i, DATA_WORD);
	}
	printf("\n");
}


static int
SPI_readx4(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	// dram_normal_patterns();

#if CONFIG_AVOID_ERROR_INPUT_HANG
	if ((argv[1]) && (argv[2]) && (argv[3]) && (argv[4])) {
		/*
		 * Avoid error input format hang
		 */
	} else {
		printf("Wrong argument number!\r\n");
		printf("Usage:spirdx4<flash_src_addr><dram_dst_addr><length><chip_select> in hex\r\n");
		return CMD_RET_FAILURE;
	}
#endif

	unsigned int flash_src_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	unsigned int dram_dest_addr = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);
	unsigned int length = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);

	/*
	 * Select cs0 or cs1 for user mode , auto mode's setting is
	 * FLASH_SIZE(offset + 0x124)
	 */
#if 0
	u32 chip_select = Strtoul((const u8 *) (argv[4]), (u8 **) NULL, 16);
	SPI_ChipSelect(chip_select);
#endif
	SPI_ChipSelect(chip_select);

#if 0
	if (length % SPI_SECTOR_SIZE != 0) {
		length = length + (SPI_SECTOR_SIZE - (length % SPI_SECTOR_SIZE));
	}
#endif
	printf("SPI Flash#%x  will read 0x%X length from 0x%X to 0x%X\n", chip_select, length, flash_src_addr, dram_dest_addr);

	SPIreadWordx4(flash_src_addr, dram_dest_addr, length);

	return CMD_RET_SUCCESS;
}


U_BOOT_CMD(spirdx4, 5, 0, SPI_readx4,
	   "spirdx4 <flash_src_addr><dram_dst_addr><length><chip_select>  : SPI flash readx4 by user-mode\n", "");
// //////////////////////////////////////////////////////////////////////
#endif



#ifdef SUPPORT_SPI_SIO
// //////////////////////////////////////////////////////////////////////
/*
 * Uboot command of writex1 DRAM data to SPI flash , user mode
 */
// //////////////////////////////////////////////////////////////////////
// int SPIcmp(int dst1 , char* src1,int length1)
int
SPIwriteWordx1(char *dst1, char *src1, unsigned int length1
)
{
	volatile unsigned int i;
	unsigned int	dst, src;
	unsigned int	dst_value, src_value;
	unsigned int	length;
	// unsigned int error;

	printf("\ndst1=0x%x,src1=0x%x,length1=0x%x\n", dst1, src1, length1);

#if 0
	if (argc < 3) {
		printf("Parameters not enough!\n");
		return 1;
	}

#endif
	// dst = dst1+0xbd000000;
	dst = dst1;
	src = src1;
	length = length1;

	//
	for (i = 0; i < length; i += 4) {
		if ((i + SPI_SECTOR_SIZE) % (SPI_SECTOR_SIZE * 10) == 0)
			printf(".");

#if 1
		if ((i + SPI_SECTOR_SIZE) % (SPI_SECTOR_SIZE * 0x100) == 0)
			printf("i=0x%x", i);
#endif
		// 4-byte for once
		flash_write_words(&dev, (dst + i) % FLASH_BASE, (src + i), DATA_HALF);
		// flash_write_words(&dev, i, (src+i), DATA_HALF);
	}
	printf("\n");
}


static int
SPI_WRITEx1(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
#if CONFIG_AVOID_ERROR_INPUT_HANG
	if ((argv[1]) && (argv[2]) && (argv[3]) && (argv[4])) {
		/*
		 * Avoid error input format hang
		 */
	} else {
		printf("Wrong argument number!\r\n");
		printf("Usage:spiwrx1<flash_dest_addr><dram_src_addr><length><chip_select> in hex\r\n");
		return CMD_RET_FAILURE;
	}
#endif

	unsigned int flash_dest_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	unsigned int dram_src_addr = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);
	unsigned int length = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);

	/*
	 * Select cs0 or cs1 for user mode , auto mode's setting is
	 * FLASH_SIZE(offset + 0x124)
	 */
#if 0
	u32 chip_select = Strtoul((const u8 *) (argv[4]), (u8 **) NULL, 16);
	SPI_ChipSelect(chip_select);
#endif
	SPI_ChipSelect(chip_select);

#if 0
	if (length % SPI_SECTOR_SIZE != 0) {
	length = length + (SPI_SECTOR_SIZE - (length % SPI_SECTOR_SIZE));
	}
#endif

	printf("SPI Flash#%x  will write 0x%X length from 0x%X to 0x%X\n", chip_select, length, dram_src_addr, flash_dest_addr);

	SPIwriteWordx1(flash_dest_addr, dram_src_addr, length);

	return CMD_RET_SUCCESS;
}



U_BOOT_CMD(spiwrx1, 5, 0, SPI_WRITEx1,
	   "spiwrx1 <flash_dest_addr><dram_src_addr><length><chip_select>  : SPI flash writex1 by user mode\n", "");
// //////////////////////////////////////////////////////////////////////
#endif



#ifdef SUPPORT_SPI_DIO
// //////////////////////////////////////////////////////////////////////
/*
 * Uboot command of writex2 DRAM data to SPI flash , user mode
 */
// //////////////////////////////////////////////////////////////////////
// int SPIcmp(int dst1 , char* src1,int length1)
int
SPIwriteWordx2(char *dst1, char *src1, unsigned int length1
)
{
	volatile unsigned int i;
	unsigned int dst, src;
	unsigned int dst_value, src_value;
	unsigned int length;
	unsigned int error;

	printf("\ndst1=0x%x,src1=0x%x,length1=0x%x\n", dst1, src1, length1);

#if 0
	if (argc < 3) {
		printf("Parameters not enough!\n");
		return 1;
	}
#endif
	// dst = dst1+0xbd000000;
	dst = dst1;
	src = src1;
	length = length1;

	for (i = 0; i < length; i += 4) {
		if ((i + SPI_SECTOR_SIZE) % (SPI_SECTOR_SIZE * 10) == 0)
			printf(".");

		if ((i + SPI_SECTOR_SIZE) % (SPI_SECTOR_SIZE * 0x100) == 0)
			printf("i=0x%x", i);

		// printf("\ni=0x%x,,dst1=0x%x,src1=0x%x\n",i,(dst+i),(src+i));
		// //JSW: must have
		// *(volatile unsigned int *)(dst+i)=*(volatile unsigned int
		// *)(src+i);
		// spi_flash_map-> ssienr =1;

		flash_writex2(&dev, (dst + i) % FLASH_BASE, *(volatile unsigned int *) (src + i), DATA_WORD);
		// flash_writex2(&dev, i, *(volatile unsigned int *)(src+i),
		// DATA_WORD);
	}
	printf("\n");
}


static int
SPI_WRITEx2(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	// dram_normal_patterns();

#if CONFIG_AVOID_ERROR_INPUT_HANG
	if ((argv[1]) && (argv[2]) && (argv[3]) && (argv[4])) {
		/*
		 * Avoid error input format hang
		 */
	} else {
		printf("Wrong argument number!\r\n");
		printf("Usage:spiwrx2<flash_dest_addr><dram_src_addr><length><chip_select> in hex\r\n");
		return CMD_RET_FAILURE;
	}
#endif
	unsigned int flash_dest_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	unsigned int dram_src_addr = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);
	unsigned int length = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);

	/*
	 * Select cs0 or cs1 for user mode , auto mode's setting is
	 * FLASH_SIZE(offset + 0x124)
	 */
#if 0
	u32 chip_select = Strtoul((const u8 *) (argv[4]), (u8 **) NULL, 16);
	SPI_ChipSelect(chip_select);
#endif
	SPI_ChipSelect(chip_select);

#if 0
	if (length % SPI_SECTOR_SIZE != 0) {
		length = length + (SPI_SECTOR_SIZE - (length % SPI_SECTOR_SIZE));
	}
#endif

	printf("SPI Flash#%x  will write 0x%X length from 0x%X to 0x%X\n", chip_select, length, dram_src_addr, flash_dest_addr);

	SPIwriteWordx2(flash_dest_addr, dram_src_addr, length);

	return CMD_RET_SUCCESS;
}


U_BOOT_CMD(spiwrx2, 5, 0, SPI_WRITEx2,
	   "spiwrx2 <flash_dest_addr><dram_src_addr><length><chip_select>  : SPI flash writex2 by user mode\n", "");
// //////////////////////////////////////////////////////////////////////
#endif



#ifdef SUPPORT_SPI_QIO
// //////////////////////////////////////////////////////////////////////
/*
 * Uboot command of writex4 DRAM data to SPI flash , user mode
 */
// //////////////////////////////////////////////////////////////////////
// int SPIcmp(int dst1 , char* src1,int length1)
int
SPIwriteWordx4(char *dst1, char *src1, unsigned int length1)
{
	volatile unsigned int i;
	unsigned int dst, src;
	unsigned int dst_value, src_value;
	unsigned int length;
	unsigned int error;

	printf("\ndst1=0x%x,src1=0x%x,length1=0x%x\n", dst1, src1, length1);

	// dst = dst1+0xbd000000;

	dst = dst1;
	src = src1;
	length = length1;

	for (i = 0; i < length; i += 4) {
		if ((i + SPI_SECTOR_SIZE) % (SPI_SECTOR_SIZE * 10) == 0)
			printf(".");

		if ((i + SPI_SECTOR_SIZE) % (SPI_SECTOR_SIZE * 0x100) == 0)
			printf("\ni=0x%x\n", i);

		// flash_writex4(&dev, i, *(volatile unsigned int *)(src+i),
		// DATA_WORD);
		flash_writex4(&dev, (dst + i) % FLASH_BASE, *(volatile unsigned int *) (src + i), DATA_WORD);
	}
	printf("\n");
}


static int
SPI_WRITEx4(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	// dram_normal_patterns();
#if CONFIG_AVOID_ERROR_INPUT_HANG
	if ((argv[1]) && (argv[2]) && (argv[3]) && (argv[4])) {
		/*
		 * Avoid error input format hang
		 */
	} else {
		printf("Wrong argument number!\r\n");
		printf("Usage:spiwrx4<flash_dest_addr><dram_src_addr><length><chip_select> in hex\r\n");
		return CMD_RET_FAILURE;
	}
#endif
	unsigned int flash_dest_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	unsigned int dram_src_addr = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);
	unsigned int length = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);
	/*
	 * Select cs0 or cs1 for user mode , auto mode's setting is
	 * FLASH_SIZE(offset + 0x124)
	 */
#if 0
	u32 chip_select = Strtoul((const u8 *) (argv[4]), (u8 **) NULL, 16);
	SPI_ChipSelect(chip_select);
#endif
	SPI_ChipSelect(chip_select);

#if 0
	if (length % SPI_SECTOR_SIZE != 0) {
		length = length + (SPI_SECTOR_SIZE - (length % SPI_SECTOR_SIZE));
	}
#endif

	printf("SPI Flash#%x  will write 0x%X length from 0x%X to 0x%X\n", chip_select, length, dram_src_addr, flash_dest_addr);

	SPIwriteWordx4(flash_dest_addr, dram_src_addr, length);

	return CMD_RET_SUCCESS;
}


U_BOOT_CMD(spiwrx4, 5, 0, SPI_WRITEx4,
	   "spiwrx4 <flash_dest_addr><dram_src_addr><length><chip_select>  : SPI flash writex4 by user mode\n", "");
// //////////////////////////////////////////////////////////////////////
#endif




// //////////////////////////////////////////////////////////////////////
/*
 * Uboot command of write DRAM data to SPI flash (auto-mode)
 */
// //////////////////////////////////////////////////////////////////////
// int SPIcmp(int dst1 , char* src1,int length1)
int
SPIwriteWord(char *dst1, char *src1, unsigned int length1)
{
	volatile unsigned int i;
	unsigned int dst, src;
	unsigned int dst_value, src_value;
	unsigned int length;
	unsigned int error;

	printf("\ndst1=0x%x,src1=0x%x,length1=0x%x\n", dst1, src1, length1);

#if 0
	if (argc < 3) {
		printf("Parameters not enough!\n");
		return 1;
	}

#endif
	// dst = dst1+0xbd000000;
	dst = dst1;
	src = src1;
	length = length1;

	for (i = 0; i < length; i += 4) {
		if ((i + SPI_SECTOR_SIZE) % (SPI_SECTOR_SIZE * 10) == 0)
			printf(".");

		if ((i + SPI_SECTOR_SIZE) % (SPI_SECTOR_SIZE * 0x100) == 0)
			printf("i=0x%x", i);

		*(volatile unsigned int *) (dst + i) = *(volatile unsigned int *) (src + i);
		// spi_flash_map-> ssienr =1;

		/*
		 * 20140827: With valid_cmd(0x120)bit[10] is enable , it means
		 * SPIC(SPI flash controller) will auto polling flash's ready bit.
		 *
		 * So disable below to function call for performance ;
		 *
		 */
#if 1
	spi_flash_wait_busy(&dev);
	flash_wait_busy(&dev);
#endif
	}
	printf("\n");

}


// auto mode
static int
SPI_WRITE(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
#if CONFIG_AVOID_ERROR_INPUT_HANG
	if ((argv[1]) && (argv[2]) && (argv[3])) {
		/*
		 * Avoid error input format hang
		 */
	} else {
		printf("Wrong argument number!\r\n");
		printf("Usage:spiwr<flash_dest_addr><dram_src_addr><length> in hex\r\n");
		return CMD_RET_FAILURE;
	}
#endif
	unsigned int flash_dest_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	unsigned int dram_src_addr = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);
	unsigned int length = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);
	/*
	 * Select cs0 or cs1 for user mode , auto mode's setting is
	 * FLASH_SIZE(offset + 0x124)
	 */
#if 0
	u32 chip_select = Strtoul((const u8 *) (argv[4]), (u8 **) NULL, 16);
	SPI_ChipSelect(chip_select);
#endif
	SPI_ChipSelect(chip_select);

#if 0
	if (length % SPI_SECTOR_SIZE != 0) {
		length = length + (SPI_SECTOR_SIZE - (length % SPI_SECTOR_SIZE));
	}
#endif

	printf("SPI Flash will write 0x%X length from 0x%X to 0x%X\n", length, dram_src_addr, flash_dest_addr);
	SPIwriteWord(flash_dest_addr, dram_src_addr, length);

	return CMD_RET_SUCCESS;
}


U_BOOT_CMD(spiwr, 4, 0, SPI_WRITE,
	   "spiwr <flash_dest_addr><dram_src_addr><length> : SPI flash write by auto-mode\n", "");
// //////////////////////////////////////////////////////////////////////


// //////////////////////////////////////////////////////////////////////
/*
 * Uboot command of read SPI flash ID
 */
// //////////////////////////////////////////////////////////////////////
static int
SPI_RDID(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	// dram_normal_patterns();
	u32 dram_start;
	u32 dram_size, area_size;

#if CONFIG_AVOID_ERROR_INPUT_HANG
	if ((argv[1])) {
		/*
		 * Avoid error input format hang
		 */
	} else {
		printf("Wrong argument number!\r\n");
		printf("Usage:spiid <chip_select> in hex\r\n");
		return CMD_RET_FAILURE;
	}
#endif


	/*
	 * Select cs0 or cs1 for user mode , auto mode's setting is
	 * FLASH_SIZE(offset + 0x124)
	 */
#if 0
	u32 chip_select = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	SPI_ChipSelect(chip_select);
#endif

	SPI_ChipSelect(chip_select);

#if 0
	flash_read_id(&dev);
#else
	flash_device_init(&dev, 0x9f);	  // RDID
#endif
	return CMD_RET_SUCCESS;
}


U_BOOT_CMD(spiid, 2, 0, SPI_RDID,
	   "spiid <chip_select>: Read SPI flash ID\n", "");
// //////////////////////////////////////////////////////////////////////



int
SPIcmp(char *dst1, unsigned int *src1, unsigned int length1)
{
	int i;
	unsigned int dst, src;
	unsigned int dst_value, src_value;
	unsigned int length;
	unsigned int error;

	// printf("\nSPIcmp().");
	printf("\ndst1=0x%x,src1=0x%x,length1=0x%x\n", dst1, src1, length1);

#if 0
	if (argc < 3) {
	printf("Parameters not enough!\n");
	return 1;
	}
	// dst = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	// src = strtoul((const char*)(argv[1]), (char **)NULL, 16);
	// length= strtoul((const char*)(argv[2]), (char **)NULL, 16);

#endif
	// dst = dst1+0xbd000000;
	dst = dst1;
	src = src1;
	length = length1;

	// printf("\ndst=%x,src=%x,length=%x\n",dst,src,length);
	error = 0;
	for (i = 0; i < length; i += 4) {
		dst_value = *(volatile unsigned int *) (dst + i);
		src_value = *(volatile unsigned int *) (src + i);
		if (dst_value != src_value) {
			printf("\n->%dth data(dst=%x ,src=%x) error\n", i, dst_value, src_value);
			error = 1;
			// return 1;
		}
	}
	if (!error) {
		printf("\n->Compare OK\n");
	}
	return error;
}

#if 0
int
CmdCmp(int argc, char *argv[])
{
	int i;
	unsigned int dst, src;
	unsigned int dst_value, src_value;
	unsigned int length;
	unsigned int error;

	if (argc < 3) {
		printf("Parameters not enough!\n");
		return 1;
	}
	dst = strtoul((const char *) (argv[0]), (char **) NULL, 16);
	src = strtoul((const char *) (argv[1]), (char **) NULL, 16);
	length = strtoul((const char *) (argv[2]), (char **) NULL, 16);
	error = 0;
	for (i = 0; i < length; i += 4) {
		dst_value = *(volatile unsigned int *) (dst + i);
		src_value = *(volatile unsigned int *) (src + i);
		if (dst_value != src_value) {
			printf("%dth data(%x %x) error\n", i, dst_value, src_value);
			error = 1;
		}
	}
	if (!error)
		printf("No error found\n");
}

#endif


// //////////////////////////////////////////////////////////////////////
/*
 * Uboot command of compareAtoB
 */
// //////////////////////////////////////////////////////////////////////

static int
compareAtoB(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	// dram_normal_patterns();

#if 1
	if ((argv[1]) && (argv[2]) && (argv[3])) {
		/*
		 * Avoid error input format hang
		 */
	} else {
		printf("Wrong argument number!\r\n");
		printf("Usage:scmp <srcA_addr><destB_addr><length> in hex\r\n");
		return CMD_RET_FAILURE;
	}
#endif

	unsigned int srcA = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	unsigned int destB = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);
	unsigned int length = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);

	SPIcmp(srcA, destB, length);

	return CMD_RET_SUCCESS;
}



U_BOOT_CMD(scmp, 4, 0, compareAtoB,
	   "scmp <srcA> <destB> <size> : Comparing A with B \n", "");


// //////////////////////////////////////////////////////////////////////
/*
 * Uboot command of SPI flash sector erase
 */
// //////////////////////////////////////////////////////////////////////


static int
SPI_SECTOR_ERASE(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	// dram_normal_patterns();

	// "spice <flash_start_addr> <erase_size> <chip_select> : Erasing SPI
	// flash sector \n",
#if CONFIG_AVOID_ERROR_INPUT_HANG
	if ((argv[1]) && (argv[2]) && (argv[3])) {
		/*
		 * Avoid error input format hang
		 */
	} else {
		printf("Wrong argument number!\r\n");
		printf("Usage:spice<flash_start_addr><erase_size><chip_select> in hex\r\n");
		return CMD_RET_FAILURE;
	}
#endif

	u32 flash_start_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	u32 erase_length = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);


	/*
	 * Select cs0 or cs1 for user mode , auto mode's setting is
	 * FLASH_SIZE(offset + 0x124)
	 */
#if 0
	u32 chip_select = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);
	SPI_ChipSelect(chip_select);
#endif
	SPI_ChipSelect(chip_select);

	u32 sector_erase_cnt;

	if (erase_length % SPI_SECTOR_SIZE != 0) {
		erase_length = erase_length + (SPI_SECTOR_SIZE - (erase_length % SPI_SECTOR_SIZE));
	}

	printf("SPI Flash #%x will erase 0x%X bytes from 0x%X to 0x%X\n", chip_select, erase_length, flash_start_addr, flash_start_addr + erase_length);

	u32 addr_start = flash_start_addr % SPI_FLASH_BASE_ADDR; // SPI_FLASH_BASE_ADDR=0xe0000000
	u32 addr;

	for (sector_erase_cnt = 0; sector_erase_cnt < (erase_length / SPI_SECTOR_SIZE); sector_erase_cnt++) {
		addr = addr_start + (sector_erase_cnt * SPI_SECTOR_SIZE);
		if ((sector_erase_cnt % 10) == 0)
			printf(".");

		flash_se_erase(&dev, addr);
	}
	return CMD_RET_SUCCESS;
}




U_BOOT_CMD(spise, 4, 0, SPI_SECTOR_ERASE,
	   "spice <flash_start_addr> <erase_size> <chip_select> : Erasing SPI flash sector \n", "");


#if CONFIG_SPI_DUMMY_CLB
static int
SPI_SECTOR_ERASE_CLB(uint32_t flash_start_addr, uint32_t erase_length, uint32_t chip_select) {

	SPI_ChipSelect(chip_select);
	u32 sector_erase_cnt;

	if (erase_length % SPI_SECTOR_SIZE != 0) {
		erase_length = erase_length + (SPI_SECTOR_SIZE - (erase_length % SPI_SECTOR_SIZE));
	}

	printf("SPI Flash #%x will erase 0x%X bytes from 0x%X to 0x%X\n", chip_select, erase_length, flash_start_addr, flash_start_addr + erase_length);

	u32 addr_start = flash_start_addr % SPI_FLASH_BASE_ADDR; // SPI_FLASH_BASE_ADDR=0xe0000000
	u32 addr;


	for (sector_erase_cnt = 0; sector_erase_cnt < (erase_length / SPI_SECTOR_SIZE); sector_erase_cnt++) {
		addr = addr_start + (sector_erase_cnt * SPI_SECTOR_SIZE);
		if ((sector_erase_cnt % 10) == 0)
			printf(".");

		flash_se_erase(&dev, addr);
	}
	return CMD_RET_SUCCESS;
}

uint32_t spi_flash_set_dummy_cycle_clb(struct dw_device *dev, uint32_t dum_cycle, uint32_t tunning_dummy)
{
	struct spi_flash_portmap *spi_flash_map;
	struct device_info *dev_info;
	uint32_t cycle;

	spi_flash_map= dev->base_address;
	dev_info = dev-> instance;
	cycle = 0;

	// Disable SPI_FLASH
	spi_flash_map-> ssienr =0;

	//printf("SPI: pre_cycle = %d\n", cycle);
	cycle = (8 * dum_cycle * 2)+ tunning_dummy;
	//printf("cycle = (8 * dum_cycle * 2)+ tunning_dummy; => 0x%8x = (8 * 0x%8x * 2) + 0x%8x \n", cycle, dum_cycle, tunning_dummy);
	//cycle = (cycle *dum_cycle* 2);

	if (cycle > 0x10000)
		return DW_ECHRNG;

#if 1
	DW_BITS_SET_VAL(spi_flash_map->auto_length, bfoSPI_FLASH_AUTO_LEN_DUM, cycle, bfwSPI_FLASH_AUTO_LEN_DUM);
	//printf("spi_flash_map->auto_length = 0x%8x,\n bfoSPI_FLASH_AUTO_LEN_DUM = 0x%8x,\n cycle = 0x%8x,\n bfwSPI_FLASH_AUTO_LEN_DUM = 0x%8x\n", spi_flash_map->auto_length, bfoSPI_FLASH_AUTO_LEN_DUM, cycle, bfwSPI_FLASH_AUTO_LEN_DUM);
#endif

#if 0//C_DEBUG
	printf("spi_flash_set_dummy_cycle_clb nspi_flash_map->auto_length = 0x%x\n", spi_flash_map->auto_length);
	printf("spi_flash_set_dummy_cycle_clb SPI: cycle = %d\n", cycle);
	printf("spi_flash_set_dummy_cycle_clb SPI: dum_cycle = %d\n", dum_cycle);
#endif
	//dev_info->cmd_info->rd_quad_dummy = tunning_dummy;
	//spi_flash_map->auto_length = (spi_flash_map->auto_length & 0xffff0000) | cycle;

	return 0;
}


// //////////////////////////////////////////////////////////////////////
/*
 * Uboot command of Auto-Calibration NOR flash while Reading with higher baud rate
 * with baud rate 25MHz, 33.3MHz, 50MHz, 100MHz
 */
// //////////////////////////////////////////////////////////////////////
static int
SPI_RDCLB(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	unsigned short baudrate_divider_ocp;
	uint32_t tunning_clk;
	uint32_t tunning_dummy;
	unsigned int	error;

	SPI_SECTOR_ERASE_CLB(0x0, 0x40000, 0);
	SPIwriteWordx1(0x0, 0xa0500000, 0x40000);

	for(tunning_clk = 4; tunning_clk >= 1; tunning_clk--) {

		baudrate_divider_ocp=tunning_clk;

		// set baud rate to 25MHz, 33.3MHz, 50MHz, 100MHz
		spi_flash_setbaudr(&dev, baudrate_divider_ocp);
		spi_flash_wait_busy(&dev);

		for(tunning_dummy = 0; tunning_dummy <= 5; tunning_dummy++) {
			error = 0;
			spi_flash_set_dummy_cycle_clb(&dev, tunning_clk, tunning_dummy);
			spi_flash_wait_busy(&dev);
			memset(0xa0400000, 0, 0x40000);
			printf("\n==================== Start ====================\n");
			if (flash_device_init(&dev, 0x9f) != DW_ENODEV) {
#ifdef SUPPORT_SPI_SIO
				spi_flash_wait_busy(&dev);
				printf(" SPIreadWordx1\n");
				SPIreadWord(0x0, 0xa0400000, 0x40000);
				error = SPIcmp(0xa0500000, 0xa0400000, 0x40);
#endif

#ifdef SUPPORT_SPI_DIO
				spi_flash_wait_busy(&dev);
				printf(" SPIreadWordx2\n");
				SPIreadWordx2(0, 0xa0400000, 0x40000);
				error += SPIcmp(0xa0500000, 0xa0400000, 0x40000);
#endif

#ifdef SUPPORT_SPI_QIO
				spi_flash_wait_busy(&dev);
				printf(" SPIreadWordx4\n");
				SPIreadWordx4(0, 0xa0400000, 0x40000);
				error += SPIcmp(0xa0500000, 0xa0400000, 0x40000);
#endif
			} else {
				error = 1;
			}
			if (error == 0) {
				printf(" clk=%d MHZ,\n Tunning_dummy=%d,\n Calibration PASS!!!\n", (200/(tunning_clk *2)), tunning_dummy);
			} else {
				printf(" clk=%d MHZ,\n Tunning_dummy=%d,\n Calibration FAIL!!!\n", (200/(tunning_clk *2)), tunning_dummy);
			}
			printf("===================== End =====================\n");
			spi_flash_wait_busy(&dev);
		}
	}
	return CMD_RET_SUCCESS;
}

//U_BOOT_CMD(_name, _maxargs, _rep, _cmd, _usage, _help)
U_BOOT_CMD(spiclb, 2, 0, SPI_RDCLB,
	   "spiclb <chip_select>: Calibration NOR flash while Reading with higher baud rate\n", "");
#endif


// //////////////////////////////////////////////////////////////////////
/*
 * Uboot command of SPI flash EN4B command issue
 */
// //////////////////////////////////////////////////////////////////////

#if CONFIG_SPI_3to4BYTES_ADDRESS_SUPPORT
static int
SPI_SEND_EN4B(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
#if CONFIG_AVOID_ERROR_INPUT_HANG
	if ((argv[1])) {
		/*
		 * Avoid error input format hang
		 */
	} else {
		printf("Wrong argument number!\r\n");
		printf("Usage:spi4b<chip_select>\r\n");
		return CMD_RET_FAILURE;
	}
#endif

	/*
	 * Select cs0 or cs1 for user mode , auto mode's setting is
	 * FLASH_SIZE(offset + 0x124)
	 */
#if 0
	u32 chip_select = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	SPI_ChipSelect(chip_select);
#endif
	SPI_ChipSelect(chip_select);

	printf("\nSPI Flash#%x Set EN4B(0xb7) command\n", chip_select);

	flash_EN4B(&dev);

	return CMD_RET_SUCCESS;
}


U_BOOT_CMD(spi4b, 2, 0, SPI_SEND_EN4B,
	   "spi4b <chip_select> : Set EN4B(0xb7) command \n", "");
#endif


/*
 * Test if SPIC will be hanged by continously CS toggle (RDSR) like
 * RTL8198C
 */
#if 1
// //////////////////////////////////////////////////////////////////////
/*
 * Uboot command of SPI flash RDSR
 */
// //////////////////////////////////////////////////////////////////////


static int
SPI_SEND_RDSR(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
#if CONFIG_AVOID_ERROR_INPUT_HANG
	if ((argv[1]) && (argv[2])) {
		/*
		 * Avoid error input format hang
		 */
	} else {
		printf("Wrong argument number!\r\n");
		printf("Usage:spirdsr <chip_select> <loop count>\r\n");
		return CMD_RET_FAILURE;
	}
#endif

	u32 i;
#if 0
	u32 chip_select = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	SPI_ChipSelect(chip_select);
#endif
	SPI_ChipSelect(chip_select);

	u32 loop_count = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);

	printf("\nRDSR loop to test if SPIC will hang ,loop_count=0x%x\n", loop_count);

	for (i = 0; i < loop_count; i++) {
		flash_wait_busy(&dev);  // RDSR loop to test if SPIC will hang
	}
	return CMD_RET_SUCCESS;
}


U_BOOT_CMD(spirdsr, 3, 0, SPI_SEND_RDSR,
	   "spirdsr <chip_select> <loop_count> : Send RDSR command \n", "");

#endif

static int
SPI_SEND_SRWD(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
#if CONFIG_AVOID_ERROR_INPUT_HANG
	if ((argv[1])) {
		/*
		 * Avoid error input format hang
		 */
	} else {
		printf("Wrong argument number!\r\n");
		printf("Usage:spisrwd <chip_select>\r\n");
		return CMD_RET_FAILURE;
	}
#endif

	u32 i;
#if 0
	u32 chip_select = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	SPI_ChipSelect(chip_select);
#endif
	SPI_ChipSelect(chip_select);

	flash_set_status(&dev, 0xBC, DATA_BYTE);
	printf("show SR : 0x%x\n", flash_get_status(&dev));
	if(flash_get_status(&dev) & 0x80)
	{
		printf("SRWD has been set to 1, software protected mode.\n");
	}
	 return CMD_RET_SUCCESS;
}


U_BOOT_CMD(spisrwd, 2, 0, SPI_SEND_SRWD,
	   "spisrwd <chip_select>  : Set SRWD bit to 1 \n","");


static int
SPI_SEND_UNSRWD(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
#if CONFIG_AVOID_ERROR_INPUT_HANG
	if ((argv[1])) {
		/*
		 * Avoid error input format hang
		 */
	} else {
		printf("Wrong argument number!\r\n");
		printf("Usage:spisrwd <chip_select>\r\n");
		return CMD_RET_FAILURE;
	}
#endif

	u32 i;
#if 0
	u32 chip_select = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	SPI_ChipSelect(chip_select);
#endif
SPI_ChipSelect(chip_select);
	flash_set_status(&dev, 0x40, DATA_BYTE);
	printf("show SR : 0x%x\n", flash_get_status(&dev));

	if((flash_get_status(&dev) & 0x80) != 0x80)
	{
		printf("SRWD has been set to 0, software protected mode.\n");
	}
	return CMD_RET_SUCCESS;
}


U_BOOT_CMD(spiunsrwd, 2, 0, SPI_SEND_UNSRWD,
	   "spiunsrwd <chip_select>  : Set SRWD bit to 0 \n", "");


#if 1
static int
SPI_Dump_Word(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]
)
{

#if CONFIG_AVOID_ERROR_INPUT_HANG
	if ((argv[1]) && (argv[2])) {
		/*
		 * Avoid error input format hang
		 */
	} else {
		printf("Wrong argument number!\r\n");
		printf("Usage:sdw <Address> <Len>\r\n");

		return CMD_RET_FAILURE;
	}
#endif

	u32 Src;
	u32 Len, LenIndex;

	Src = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	Len = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);

	if (!argv[2])
		Len = 1;
	else
		Len = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);

	while ((Src) & 0x03)
		Src++;

	printf("Src=0x%x,len=0x%x\r\n", Src, Len);

	// for(LenIndex=0; LenIndex< Len ; LenIndex+=4,Src+=16)
	for (LenIndex = 0; LenIndex < Len; LenIndex += 16, Src += 16) {
		printf("%08X:	%08X	%08X	%08X	%08X\n", Src, *(u32 *) (Src), *(u32 *) (Src + 4), *(u32 *) (Src + 8), *(u32 *) (Src + 12));
	}
	return CMD_RET_SUCCESS;
}



U_BOOT_CMD(sdw, 3, 0, SPI_Dump_Word, "sdw <Address> <Len> : dump data\n", "");
#endif


#endif

#if 1

int
CmdSPIDumpByte(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]
)
{

#if CONFIG_AVOID_ERROR_INPUT_HANG
	if ((argv[1]) && (argv[2])) {
		/*
		 * Avoid error input format hang
		 */
	} else {
		printf("Wrong argument number!\r\n");
		printf("Usage:sdb <Address> <Len>\r\n");

		return CMD_RET_FAILURE;
	}
#endif

	unsigned long src;
	unsigned int len, i;

#if 0
	if (argc < 1) {
		printf("Wrong argument number!\r\n");
		return;
	}
#endif
	src = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	if (!argv[1])
		len = 16;
	else
		len = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);

	snor_ddump((unsigned char *) src, len);
}


U_BOOT_CMD(sdb, 3, 0, CmdSPIDumpByte,
	   "sdb <Address> <Len> : dump byte data\n", "");


// ----------------------------------------------------------------

void
snor_ddump(unsigned char *pData, int len)
{
	unsigned char  *sbuf = pData;
	int			 length = len;

	int i = 0, j, offset;
	printf(" [Addr]   .0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .A .B .C .D .E .F\r\n");

	while (i < length) {

	printf("%08X: ", (sbuf + i));

	if (i + 16 < length)
		offset = 16;
	else
		offset = length - i;


	for (j = 0; j < offset; j++)
		printf("%02x ", sbuf[i + j]);

	for (j = 0; j < 16 - offset; j++)	   // a last line
		printf("   ");


	printf("	"); // between byte and char

	for (j = 0; j < offset; j++) {
		if (' ' <= sbuf[i + j] && sbuf[i + j] <= '~')
		printf("%c", sbuf[i + j]);
		else
		printf(".");
	}
	printf("\n\r");
	i += 16;
	}
	// printf("\n\r");
}



#endif


void
SPI_ChipSelect(unsigned int chip_select)
{
	REG32(BSP_PS_I_SPIC_BASE + SPIC_SSIENR) &= ~(1 << 0);	   // SSIENR
								// bit[0]=0
	REG32(BSP_PS_I_SPIC_BASE + SPIC_SER) &= ~(3 << 0);  // clear bit[1:0]
	// REG32(BSP_PS_I_SPIC_BASE+SPIC_SER)|= (1<<0);//cs=0 is default

	if (chip_select == 1) {
		REG32(BSP_PS_I_SPIC_BASE + SPIC_SER) |= (1 << 1);	   // cs=1
	} else {
		REG32(BSP_PS_I_SPIC_BASE + SPIC_SER) |= (1 << 0);	   // cs=0
	}

}

#ifdef CONFIG_CMD_RTK_EMMC_FT
int CmdEMS(cmd_tbl_t * cmdtp, int flag, int argc, char* argv[])
{
	unsigned int ret_val;
	unsigned int setting;

	printf("argc=%d", argc);
	if(argc < 2) {
		printf("Parameters not enough!\n");
		return CMD_RET_SUCCESS;
	}
	//setting = Strtoul((const char *) (argv[0+1]), (char **) NULL, 16);

	ret_val = romcr_system_init(0);

	if (ret_val == 0) {
		printf("%s(%d): init success (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_SUCCESS;
	} else {
		printf("%s(%d): init fail (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_FAILURE;
	}

}

U_BOOT_CMD(ems, 2, 0, CmdEMS,
	   "ems : <setting>  : emmc system init\n", "");

int CmdEMFT(cmd_tbl_t * cmdtp, int flag, int argc, char* argv[])
{
	unsigned int ret_val;
	unsigned int setting;

	printf("argc=%d", argc);
	if(argc < 2) {
	printf("Parameters not enough!\n");
	return CMD_RET_SUCCESS;
	}
	//setting = Strtoul((const char *) (argv[0+1]), (char **) NULL, 16);

	ret_val = romcr_emmc_ft(0);

	if (ret_val == 1) {
		printf("%s(%d): success (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_SUCCESS;
	} else {
		printf("%s(%d): fail (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_FAILURE;
	}

}

U_BOOT_CMD(emft, 2, 0, CmdEMFT,
	   "emft : <setting>  : emmc ft test\n",
	   "");


int CmdEMTM(cmd_tbl_t * cmdtp, int flag, int argc, char* argv[])
{
	unsigned int ret_val;
	unsigned int setting;

	printf("argc=%d", argc);
	if(argc < 2) {
	printf("Parameters not enough!\n");
	return CMD_RET_SUCCESS;
	}
	//setting = Strtoul((const char *) (argv[0+1]), (char **) NULL, 16);

	ret_val = romcr_emmc_tune_map(0);

	if (ret_val == 1) {
		printf("%s(%d): success (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_SUCCESS;
	} else {
		printf("%s(%d): fail (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_FAILURE;
	}

}

U_BOOT_CMD(emtm, 2, 0, CmdEMTM,
	   "emtm : <setting>  : emmc tune map test\n", "");

int CmdEMBUSTEST(cmd_tbl_t * cmdtp, int flag, int argc, char* argv[])
{
	unsigned int ret_val;
	unsigned int setting;

	printf("argc=%d", argc);
	if(argc < 2) {
		printf("Parameters not enough!\n");
		return CMD_RET_SUCCESS;
	}
	//setting = Strtoul((const char *) (argv[0+1]), (char **) NULL, 16);

	ret_val = romcr_emmc_bus_test(0);

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(embustest, 2, 0, CmdEMBUSTEST,
	   "embustest : <setting>  : emmc bus test\n", "");


int CmdEMI(cmd_tbl_t * cmdtp, int flag, int argc, char* argv[])
{
	unsigned int ret_val;
	unsigned int setting;

	if(argc < 2) {
		printf("Parameters not enough!\n");
		return CMD_RET_SUCCESS;
	}
	setting = Strtoul((const char *) (argv[0+1]), (char **) NULL, 16);

	ret_val = romcr_card_init(setting);

	if (ret_val == 0) {
		printf("%s(%d): init success (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_SUCCESS;
	} else {
		printf("%s(%d): init fail (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_FAILURE;
	}
}

U_BOOT_CMD(emi, 2, 0, CmdEMI,
	   "emi : <setting>  : emmc init\n", "");

int CmdEMCS(cmd_tbl_t * cmdtp, int flag, int argc, char* argv[])
{
	unsigned int ret_val;
	unsigned int setting;

	printf("argc=%d", argc);
	if(argc < 2) {
		printf("Parameters not enough!\n");
		return CMD_RET_SUCCESS;
	}
	//setting = Strtoul((const char *) (argv[0+1]), (char **) NULL, 16);

	ret_val = romcr_card_chang_speed(0);

//	if (ret_val == 0) {
//		printf("%s(%d): init success (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_SUCCESS;
//	} else {
//		printf("%s(%d): init fail (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
//		return CMD_RET_FAILURE;
//	}

}

U_BOOT_CMD(emcs, 2, 0, CmdEMCS,
	   "emcs : <setting>  : emmc change speed\n", "");
#if 1//enable or disable for frank emmc ft test code
int CmdEMLR(cmd_tbl_t * cmdtp, int flag, int argc, char* argv[])
{
	unsigned int ret_val;
	unsigned int count;

	if(argc < 2) {
	printf("Parameters not enough!\n");
	return CMD_RET_SUCCESS;
	}
	count = Strtoul((const char *) (argv[0+1]), (char **) NULL, 16);

	ret_val = EMMC_RW_longrun(count);

//	if (ret_val == 0) {
//		printf("%s(%d): init success (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_SUCCESS;
//	} else {
//		printf("%s(%d): init fail (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
//		return CMD_RET_FAILURE;
//	}

}

U_BOOT_CMD(emlr, 2, 0, CmdEMLR,
	   "emlr : <count>  : emmc long run\n", "");

int CmdEMTP(cmd_tbl_t * cmdtp, int flag, int argc, char* argv[])
{
	unsigned int ret_val;
	unsigned int setting;

	if(argc < 2) {
		printf("Parameters not enough!\n");
		return CMD_RET_SUCCESS;
	}
	setting = Strtoul((const char *) (argv[0+1]), (char **) NULL, 16);

	ret_val = EMMC_RW_throughput(setting);

//	if (ret_val == 0) {
//		printf("%s(%d): init success (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_SUCCESS;
//	} else {
//		printf("%s(%d): init fail (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
//		return CMD_RET_FAILURE;
//	}

}

U_BOOT_CMD(emtp, 2, 0, CmdEMTP,
	   "emtp : <setting>  : emmc throughput\n", "");
#endif
int CmdEMRW(cmd_tbl_t * cmdtp, int flag, int argc, char* argv[])
{
	// Note: eMMC/SDcard target address must 8 byte alignment !!!
	// Usage: EMRW <addr> <start_blk_num> <len> <isWrite>
	unsigned int isWrite; // 1: write, 0: read
	unsigned int start_blk_num, blk_cnt, len, ret_val;
	unsigned char *addr;

	if(argc < 4) {
		printf("Parameters not enough!\n");
		return CMD_RET_SUCCESS;
	}

	addr = (unsigned char *) Strtoul((const char *) (argv[0+1]), (char **) NULL, 16);
	start_blk_num = Strtoul((const char *) (argv[1+1]), (char **) NULL, 16);
	len = Strtoul((const char *) (argv[2+1]), (char **) NULL, 16);
	isWrite = Strtoul((const char *) (argv[3+1]), (char **) NULL, 16);
	blk_cnt = get_emmc_blk_size(len);

	ret_val = romcr_blk_ops(isWrite, start_blk_num, blk_cnt, addr);

	if (ret_val == 0) {
		printf("%s(%d): success (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_SUCCESS;
	} else {
		printf("%s(%d): fail (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_FAILURE;
	}
}

U_BOOT_CMD(emrw, 5, 0, CmdEMRW,
	   "emrw  : <addr> <startBlkNum> <len> <isWrite>: read/write block\n", "");

#endif

#ifdef CONFIG_CMD_RTK_EMMC_DRIVER
int CmdEMI(cmd_tbl_t * cmdtp, int flag, int argc, char* argv[])
{
	unsigned int ret_val;
	unsigned int setting;

	setting = Strtoul((const char *) (argv[0+1]), (char **) NULL, 16);

	ret_val = romcr_card_init(setting);

	if (ret_val == 0) {
		printf("%s(%d): init success (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_SUCCESS;
	} else {
		printf("%s(%d): init fail (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_FAILURE;
	}

}

U_BOOT_CMD(emi, 2, 0, CmdEMI,
	   "emi : <setting>  : emmc init\n", "");

int CmdEMRW(cmd_tbl_t * cmdtp, int flag, int argc, char* argv[])
{
	// Note: eMMC/SDcard target address must 8 byte alignment !!!
	// Usage: EMRW <addr> <start_blk_num> <len> <isWrite>
	unsigned int isWrite; // 1: write, 0: read
	unsigned int start_blk_num, blk_cnt, len, ret_val;
	unsigned char *addr;

	if(argc < 4) {
		printf("Parameters not enough!\n");
		return TRUE;
	}

	addr = (unsigned char *) Strtoul((const char *) (argv[0+1]), (char **) NULL, 16);
	start_blk_num = Strtoul((const char *) (argv[1+1]), (char **) NULL, 16);
	len = Strtoul((const char *) (argv[2+1]), (char **) NULL, 16);
	isWrite = Strtoul((const char *) (argv[3+1]), (char **) NULL, 16);
	blk_cnt = get_emmc_blk_size(len);

	ret_val = romcr_blk_ops(isWrite, start_blk_num, blk_cnt, addr);

	if (ret_val == 0) {
		printf("%s(%d): success (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_SUCCESS;
	} else {
		printf("%s(%d): fail (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_FAILURE;
	}
}

U_BOOT_CMD(emrw, 5, 0, CmdEMRW,
	   "emrw  : <addr> <startBlkNum> <len> <isWrite>: read/write block\n", "");

#endif

#ifdef CONFIG_CMD_RTK_SDCARD_RW_DRIVER
int CmdSDS(cmd_tbl_t * cmdtp, int flag, int argc, char* argv[])
{
	unsigned int ret_val;
	unsigned int setting;

	printf("argc=%d", argc);
	if(argc < 2) {
		printf("Parameters not enough!\n");
		return CMD_RET_SUCCESS;
	}
	//setting = Strtoul((const char *) (argv[0+1]), (char **) NULL, 16);

	ret_val = romcr_system_init(0);

	if (ret_val == 0) {
		printf("%s(%d): init success (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_SUCCESS;
	} else {
		printf("%s(%d): init fail (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_FAILURE;
	}

}

U_BOOT_CMD(sds, 2, 0, CmdSDS,
	   "sds : <setting>  : sds system init\n", "");

int CmdSDFT(cmd_tbl_t * cmdtp, int flag, int argc, char* argv[])
{
	unsigned int ret_val;
	unsigned int setting;

	printf("argc=%d", argc);
	if(argc < 2) {
		printf("Parameters not enough!\n");
		return CMD_RET_SUCCESS;
	}
	//setting = Strtoul((const char *) (argv[0+1]), (char **) NULL, 16);

	ret_val = romcr_sdcard_ft(0);

	if (ret_val == 1) {
		printf("%s(%d): success (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_SUCCESS;
	} else {
		printf("%s(%d): fail (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_FAILURE;
	}

}

U_BOOT_CMD(sdft, 2, 0, CmdSDFT,
	   "sdft : <setting>  : sdcard ft test\n", "");


int CmdSDTM(cmd_tbl_t * cmdtp, int flag, int argc, char* argv[])
{
	unsigned int ret_val;
	unsigned int setting;

	printf("argc=%d", argc);
	if(argc < 2) {
		printf("Parameters not enough!\n");
		return CMD_RET_SUCCESS;
	}
	//setting = Strtoul((const char *) (argv[0+1]), (char **) NULL, 16);

	ret_val = romcr_sdcard_tune_map(0);

	if (ret_val == 1) {
		printf("%s(%d): success (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_SUCCESS;
	} else {
		printf("%s(%d): fail (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_FAILURE;
	}

}

U_BOOT_CMD(sdtm, 2, 0, CmdSDTM,
	   "sdtm : <setting>  : sdcard tune map test\n", "");


int CmdSDI(cmd_tbl_t * cmdtp, int flag, int argc, char* argv[])
{
	unsigned int ret_val;
	unsigned int setting;

	if(argc < 2) {
		printf("Parameters not enough!\n");
		return CMD_RET_SUCCESS;
	}
	setting = Strtoul((const char *) (argv[0+1]), (char **) NULL, 16);

	ret_val = romcr_sdcard_init(setting);

	if (ret_val == 0) {
		printf("%s(%d): init success (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_SUCCESS;
	} else {
		printf("%s(%d): init fail (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_FAILURE;
	}

}

U_BOOT_CMD(sdi, 2, 0, CmdSDI,
	   "sdi : <setting>  : sdcard init\n", "");

int CmdSDCS(cmd_tbl_t * cmdtp, int flag, int argc, char* argv[])
{
	unsigned int ret_val;
	unsigned int setting;

	printf("argc=%d", argc);
	if(argc < 2) {
		printf("Parameters not enough!\n");
		return CMD_RET_SUCCESS;
	}
	//setting = Strtoul((const char *) (argv[0+1]), (char **) NULL, 16);

	ret_val = romcr_card_chang_speed(0);

//	if (ret_val == 0) {
//		printf("%s(%d): init success (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_SUCCESS;
//	} else {
//		printf("%s(%d): init fail (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
//		return CMD_RET_FAILURE;
//	}

}

U_BOOT_CMD(sdcs, 2, 0, CmdSDCS,
	   "sdcs : <setting>  : sd change speed\n",
	   "");

int CmdSDLR(cmd_tbl_t * cmdtp, int flag, int argc, char* argv[])
{
	unsigned int ret_val;
	unsigned int count;

	if(argc < 2) {
		printf("Parameters not enough!\n");
		return CMD_RET_SUCCESS;
	}
	count = Strtoul((const char *) (argv[0+1]), (char **) NULL, 16);

	ret_val = SDCARD_RW_longrun(count);

//	if (ret_val == 0) {
//		printf("%s(%d): init success (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_SUCCESS;
//	} else {
//		printf("%s(%d): init fail (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
//		return CMD_RET_FAILURE;
//	}

}

U_BOOT_CMD(sdlr, 2, 0, CmdSDLR,
	   "sdlr : <count>  : sdcard long run\n",
	   "");

int CmdSDTP(cmd_tbl_t * cmdtp, int flag, int argc, char* argv[])
{
	unsigned int ret_val;
	unsigned int setting;

	if(argc < 2) {
		printf("Parameters not enough!\n");
		return CMD_RET_SUCCESS;
	}
	setting = Strtoul((const char *) (argv[0+1]), (char **) NULL, 16);

	ret_val = SDCARD_RW_throughput(setting);

//	if (ret_val == 0) {
//		printf("%s(%d): init success (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_SUCCESS;
//	} else {
//		printf("%s(%d): init fail (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
//		return CMD_RET_FAILURE;
//	}

}

U_BOOT_CMD(sdtp, 2, 0, CmdSDTP,
	   "sdtp : <setting>  : sdcard throughput\n", "");

int CmdSDRW(cmd_tbl_t * cmdtp, int flag, int argc, char* argv[])
{
	// Note: eMMC/SDcard target address must 8 byte alignment !!!
	// Usage: EMRW <addr> <start_blk_num> <len> <isWrite>
	unsigned int isWrite; // 1: write, 0: read
	unsigned int start_blk_num, blk_cnt, len, ret_val;
	unsigned char *addr;

	if(argc < 4) {
		printf("Parameters not enough!\n");
		return CMD_RET_SUCCESS;
	}

	addr = (unsigned char *) Strtoul((const char *) (argv[0+1]), (char **) NULL, 16);
	start_blk_num = Strtoul((const char *) (argv[1+1]), (char **) NULL, 16);
	len = Strtoul((const char *) (argv[2+1]), (char **) NULL, 16);
	isWrite = Strtoul((const char *) (argv[3+1]), (char **) NULL, 16);
	blk_cnt = get_sdcard_blk_size(len);

	ret_val = romcr_sdcard_blk_ops(isWrite, start_blk_num, blk_cnt, addr);

	if (ret_val == 0) {
		printf("%s(%d): success (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_SUCCESS;
	} else {
		printf("%s(%d): fail (0x%x) \n", __FUNCTION__, __LINE__, ret_val);
		return CMD_RET_FAILURE;
	}
}

U_BOOT_CMD(sdrw, 5, 0, CmdSDRW,
	   "sdrw  : <addr> <startBlkNum> <len> <isWrite>: read/write block\n", "");

#endif



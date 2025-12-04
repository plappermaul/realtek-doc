/*
 * Overview: Realtek Serial NOR Flash Driver
 * Copyright (c) 2016 Realtek Semiconductor Corp. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 * Modification History:
 *    #000 2016-01-01 Cortina_Owner   create file
 *    #001 2016-11-14 Flysky_Hung     proting to RTL8198F
 *    #002 
 *    #003 
 *    #004 
 ******************************************************************************/

#if 0
#include <assert.h>
#include <debug.h>
#include <io_driver.h>
#include <io_storage.h>
#include <string.h>
#include <mmio.h>
#include <g3_registers.h>
#include <platform_def.h>
#include <g3_otp.h>
#ifdef CORTINA_OTP_SUPPORT
extern G3_OTP_DRV_t * g3_pOtpDrv;
#endif /* CORTINA_OTP_SUPPORT */

/* Return codes reported by 'io_*' APIs.
 * IMPORTANT: these definitions are deprecated. Callers should use standard
 * errno definitions when checking the return value of io_* APIs. */

#define IO_SUCCESS		(0)
#define IO_FAIL			(-ENOENT)
#define IO_NOT_SUPPORTED	(-ENODEV)
#define IO_RESOURCES_EXHAUSTED	(-ENOMEM)


/* As we need to be able to keep state for seek, only one file can be open
 * at a time. Make this a structure and point to the entity->info. When we
 * can malloc memory we can change this to support more open files.
 */
typedef struct {
	/* Use the 'in_use' flag as any value for base and file_pos could be
	 * valid.
	 */
	int		in_use;
	uintptr_t	base;
	size_t		file_pos;
} file_state_t;

static file_state_t current_file = {0};

/* Identify the device type as memmap */
io_type_t device_type_sflash(void)
{
	return IO_TYPE_MEMMAP;
}

/* Memmap device functions */
static int g3_sflash_dev_open(const uintptr_t dev_spec, io_dev_info_t **dev_info);
static int g3_sflash_block_open(io_dev_info_t *dev_info, const uintptr_t spec,
			io_entity_t *entity);
static int g3_sflash_block_seek(io_entity_t *entity, int mode,
				ssize_t offset);
static int g3_sflash_block_read(io_entity_t *entity, uintptr_t buffer,
			 	size_t length, size_t *length_read);
static int g3_sflash_block_write(io_entity_t *entity, const uintptr_t buffer,
				size_t length, size_t *length_written);
static int g3_sflash_block_close(io_entity_t *entity);
static int g3_sflash_dev_close(io_dev_info_t *dev_info);

extern void udelay(int us);

/* STRAP_PIN for RTL8198F SERIAL NAND flash
 * flash_type:	000
 * flash_width:	0
 * flash_size:	00: 3-byte address, 2KB page size
 *		01: 3-byte address, 4KB page size
 *		10: 4-byte address, 2KB page size
 *		11: 4-byte address, 4KB page size
 * flash_pin:	1
 */
static int g3_sflash_init(io_dev_info_t *dev_info, const uintptr_t init_params)
{
	FLASH_TYPE_t flash_type;
	GLOBAL_BLOCK_RESET_t g_block_rst;
	GLOBAL_STRAP_t	global_strap;
        HWSIM_PROGRESS_MARK(0x85);
#if defined(IMAGE_BL1) && defined(CORTINA_OTP_SUPPORT)
	/* Return if we have hotfix here */
	if(BL1_HOTFIX_CHECKPOINT(CHECKPOINT_ID_SFLASH_INIT, g3_pOtpDrv)==1)
		return IO_SUCCESS;
        BL1_HOTFIX_CHECKPOINT(CHECKPOINT_ID_SFLASH_INIT_CONTINUE, g3_pOtpDrv);
#endif
	
	/* Reset Flash controller and wait a short duration */
	g_block_rst.wrd = mmio_read_32(GLOBAL_BLOCK_RESET);
	g_block_rst.bf.reset_flash = 1;
	mmio_write_32(GLOBAL_BLOCK_RESET, g_block_rst.wrd);
        HWSIM_PROGRESS_MARK(0x86);
#ifdef TEST_ON_G3_ASIC_SIM
        udelay(1);
#else
 	udelay(10);
#endif
        HWSIM_PROGRESS_MARK(0x87);
	g_block_rst.bf.reset_flash = 0;
	mmio_write_32(GLOBAL_BLOCK_RESET, g_block_rst.wrd);


	global_strap.wrd = mmio_read_32(GLOBAL_STRAP);

	
	flash_type.wrd = 0;
	flash_type.bf.flashWidth = global_strap.bf.flash_width ;
	flash_type.bf.flashSize = global_strap.bf.flash_size;
	//flash pin for power of 2 mode
	flash_type.bf.flashPin = global_strap.bf.flash_pin;

	/* TBD, Sflash 4 bytes address */
	
	mmio_write_32(FLASH_TYPE, flash_type.wrd);
#if defined(IMAGE_BL1) && defined(CORTINA_OTP_SUPPORT)
	BL1_HOTFIX_CHECKPOINT(CHECKPOINT_ID_SFLASH_AFTER, g3_pOtpDrv);
#endif

        HWSIM_PROGRESS_MARK(0x88);
	return IO_SUCCESS;
}

static const io_dev_connector_t g3_sflash_dev_connector = {
	.dev_open = g3_sflash_dev_open
};


static const io_dev_funcs_t g3_sflash_dev_funcs = {
	.type = device_type_sflash,
	.open = g3_sflash_block_open,
	.seek = g3_sflash_block_seek,
	.size = NULL,
	.read = g3_sflash_block_read,
	.write = g3_sflash_block_write,
	.close = g3_sflash_block_close,
	.dev_init = g3_sflash_init,
	.dev_close = g3_sflash_dev_close,
};


/* No state associated with this device so structure can be const */
static const io_dev_info_t g3_sflash_dev_info = {
	.funcs = &g3_sflash_dev_funcs,
	.info = (uintptr_t)NULL
};


/* Open a connection to the memmap device */
static int g3_sflash_dev_open(const uintptr_t dev_spec __attribute__((unused)),
			io_dev_info_t **dev_info)
{
	assert(dev_info != NULL);
	*dev_info = (io_dev_info_t *)&g3_sflash_dev_info; /* cast away const */

	return IO_SUCCESS;
}



/* Close a connection to the memmap device */
static int g3_sflash_dev_close(io_dev_info_t *dev_info)
{
	/* NOP */
	/* TODO: Consider tracking open files and cleaning them up here */
	return IO_SUCCESS;
}


/* Open a file on the memmap device */
/* TODO: Can we do any sensible limit checks on requested memory */
static int g3_sflash_block_open(io_dev_info_t *dev_info, const uintptr_t spec,
			io_entity_t *entity)
{
	int result = IO_FAIL;
        HWSIM_PROGRESS_MARK(0x89);
	const io_block_spec_t *block_spec = (io_block_spec_t *)spec;

	/* Since we need to track open state for seek() we only allow one open
	 * spec at a time. When we have dynamic memory we can malloc and set
	 * entity->info.
	 */
	if (current_file.in_use == 0) {
		assert(block_spec != NULL);
		assert(entity != NULL);

		current_file.in_use = 1;
		current_file.base = block_spec->offset | FLASH_BASE;
		/* File cursor offset for seek and incremental reads etc. */
		current_file.file_pos = 0;
		entity->info = (uintptr_t)&current_file;
		result = IO_SUCCESS;
	} else {
		WARN("A Memmap device is already active. Close first.\n");
		result = IO_RESOURCES_EXHAUSTED;
	}

	return result;
}


/* Seek to a particular file offset on the memmap device */
static int g3_sflash_block_seek(io_entity_t *entity, int mode, ssize_t offset)
{
	int result = IO_FAIL;
        HWSIM_PROGRESS_MARK(0x90);

	/* We only support IO_SEEK_SET for the moment. */
	if (mode == IO_SEEK_SET) {
		assert(entity != NULL);

		/* TODO: can we do some basic limit checks on seek? */
		((file_state_t *)entity->info)->file_pos = offset;
		result = IO_SUCCESS;
	} else {
		result = IO_FAIL;
	}

	return result;
}


/* Read data from a file on the memmap device */
static int g3_sflash_block_read(io_entity_t *entity, uintptr_t buffer,
			 size_t length, size_t *length_read)
{
	file_state_t *fp;
        HWSIM_PROGRESS_MARK(0x91);

	assert(entity != NULL);
	assert(buffer != (uintptr_t)NULL);
	assert(length_read != NULL);

	fp = (file_state_t *)entity->info;
        HWSIM_PROGRESS_VAL((unsigned long long int)(fp->base + fp->file_pos));
        HWSIM_PROGRESS_VAL(length);
	memcpy((void *)buffer, (void *)(fp->base + fp->file_pos), length);
        
	*length_read = length;

	/* advance the file 'cursor' for incremental reads */
	fp->file_pos += length;
        HWSIM_PROGRESS_MARK(0x92);
#if defined(TEST_ON_G3_ASIC_SIM) && !defined(TEST_ON_G3_ASIC_SIM_DISABLE_PROGRESS_MARK)
        unsigned int *p=(unsigned int *)buffer;
        for (int i = 0; i < (length>>2); i++) {
                HWSIM_PROGRESS_VAL(i);
                HWSIM_PROGRESS_VAL(p[i]);
        }
#endif

	return IO_SUCCESS;
}


/* Write data to a file on the memmap device */
static int g3_sflash_block_write(io_entity_t *entity, const uintptr_t buffer,
			size_t length, size_t *length_written)
{
	/* TBD */

	return IO_SUCCESS;
}


/* Close a file on the memmap device */
static int g3_sflash_block_close(io_entity_t *entity)
{
        HWSIM_PROGRESS_MARK(0x93);
	assert(entity != NULL);

	entity->info = 0;

	/* This would be a mem free() if we had malloc.*/
	memset((void *)&current_file, 0, sizeof(current_file));

	return IO_SUCCESS;
}


/* Exported functions */

/* Register the memmap driver with the IO abstraction */
int register_io_dev_sflash(const io_dev_connector_t **dev_con)
{
	int result = IO_FAIL;
	assert(dev_con != NULL);

        HWSIM_PROGRESS_MARK(0x94);
	result = io_register_device(&g3_sflash_dev_info);
        HWSIM_PROGRESS_MARK(0x95);
        
	if (result == IO_SUCCESS)
		*dev_con = &g3_sflash_dev_connector;

	return result;
}

#endif //of #if 0

/*
 * (C) Copyright 2000-2010
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2008
 * Stuart Wood, Lab X Technologies <stuart.wood@labxtechnologies.com>
 *
 * (C) Copyright 2004
 * Jian Zhang, Texas Instruments, jzhang@ti.com.
 *
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <environment.h>
#include <linux/stddef.h>
#include <malloc.h>
#include <nand.h>
#include <search.h>
#include <errno.h>
#include <asm/otto_util.h>

#if defined(CONFIG_CMD_SAVEENV) && defined(CONFIG_CMD_SPI_NAND)
#define CMD_SAVEENV
#elif defined(CONFIG_ENV_OFFSET_REDUND)
#error CONFIG_ENV_OFFSET_REDUND must have CONFIG_CMD_SAVEENV & CONFIG_CMD_SPI_NAND
#endif

#if defined(CONFIG_ENV_SIZE_REDUND) &&	\
    (CONFIG_ENV_SIZE_REDUND != CONFIG_ENV_SIZE)
#error CONFIG_ENV_SIZE_REDUND should be the same as CONFIG_ENV_SIZE
#endif

#ifndef CONFIG_ENV_RANGE
#define CONFIG_ENV_RANGE	CONFIG_ENV_SIZE
#endif

char *env_name_spec = "SPI-NAND";

#if defined(ENV_IS_EMBEDDED)
env_t *env_ptr = &environment;
#elif defined(CONFIG_SPI_NAND_ENV_DST)
env_t *env_ptr = (env_t *)CONFIG_SPI_NAND_ENV_DST;
#else /* ! ENV_IS_EMBEDDED */
env_t *env_ptr=NULL;
#endif /* ENV_IS_EMBEDDED */

DECLARE_GLOBAL_DATA_PTR;

extern uint32_t spi_nand_chip_size(uint32_t idx);
extern int spi_nand_write_ecc(uint32_t offset, uint32_t length, u_char *buffer);
extern int spi_nand_read_ecc(uint32_t offset, uint32_t length, u_char *buffer);
extern int spi_nand_erase(uint32_t offset, uint32_t length);
extern struct mtd_info * get_mtd_info(void);
uchar env_get_char_spec(int index)
{
    return *((uchar *)(gd->env_addr + index));
}

/*
 * This is called before nand_init() so we can't read NAND to
 * validate env data.
 *
 * Mark it OK for now. env_relocate() in env_common.c will call our
 * relocate function which does the real validation.
 *
 * When using a NAND boot image (like sequoia_nand), the environment
 * can be embedded or attached to the U-Boot image in NAND flash.
 * This way the SPL loads not only the U-Boot image from NAND but
 * also the environment.
 */
int env_init(void)
{
#if defined(ENV_IS_EMBEDDED) || defined(CONFIG_SPI_NAND_ENV_DST)
    int crc1_ok = 0, crc2_ok = 0;
    env_t *tmp_env1;
    
#ifdef CONFIG_ENV_OFFSET_REDUND
    env_t *tmp_env2;

    tmp_env2 = (env_t *)((ulong)env_ptr + CONFIG_ENV_SIZE);

    crc2_ok = crc32(0, tmp_env2->data, ENV_SIZE) == tmp_env2->crc;
#endif
    tmp_env1 = env_ptr;

    crc1_ok = crc32(0, tmp_env1->data, ENV_SIZE) == tmp_env1->crc;

    if (!crc1_ok && !crc2_ok) {
        gd->env_addr=0;
        gd->env_valid=0;
        return 0;
    } else if (crc1_ok && !crc2_ok) {
        gd->env_valid = 1;
    }
#ifdef CONFIG_ENV_OFFSET_REDUND
    else if (!crc1_ok && crc2_ok) {
        gd->env_valid = 2;
    } else {
		/* both ok - check serial */
    if (tmp_env1->flags == 255 && tmp_env2->flags == 0)
        gd->env_valid = 2;
    else if (tmp_env2->flags == 255 && tmp_env1->flags == 0)
        gd->env_valid = 1;
    else if (tmp_env1->flags > tmp_env2->flags)
        gd->env_valid = 1;
    else if (tmp_env2->flags > tmp_env1->flags)
        gd->env_valid = 2;
    else /* flags are equal - almost impossible */
        gd->env_valid = 1;
    }

    if (gd->env_valid == 2)
        env_ptr = tmp_env2;
    else
#endif
    if (gd->env_valid == 1)
        env_ptr = tmp_env1;
    gd->env_addr = (ulong)env_ptr->data;
#else /* ENV_IS_EMBEDDED || CONFIG_SPI_NAND_ENV_DST */
    gd->env_addr	= (ulong)&default_environment[0];
    gd->env_valid	= 1;
#endif /* ENV_IS_EMBEDDED || CONFIG_SPI_NAND_ENV_DST */
    return 0;
}

#ifdef CMD_SAVEENV
/*
 * The legacy NAND code saved the environment in the first NAND device i.e.,
 * nand_dev_desc + 0. This is also the behaviour using the new NAND code.
 */
int writeenv(size_t offset, u_char *buf)
{
    size_t end = offset + CONFIG_ENV_RANGE;
    size_t amount_saved = 0;
    size_t blocksize, len;
    u_char *char_ptr;
    nand_info_t *nand = (nand_info_t *)get_mtd_info();

    blocksize = nand->erasesize;
    len = min(blocksize, CONFIG_ENV_SIZE);

    while (amount_saved < CONFIG_ENV_SIZE && offset < end) {
        if (nand_block_isbad(nand, offset)) {
            offset += blocksize;
        } else {
        char_ptr = &buf[amount_saved];
            if (nand_write(nand, offset, &len, char_ptr))
            return 1;

        offset += blocksize;
        amount_saved += len;
    }
    }

    if (amount_saved != CONFIG_ENV_SIZE)
        return 1;

    return 0;
}

#ifdef CONFIG_ENV_OFFSET_REDUND
static unsigned char env_flags;

int saveenv(void)
{
//	env_t	env_new;
    env_t	*_env_new;
#define env_new (*(_env_new))	
    ssize_t	len;
    char	*res;
    int        ret = 0;
    nand_erase_options_t nand_erase_options;
    nand_info_t *nand = (nand_info_t *)get_mtd_info();
    
    if(CONFIG_ENV_OFFSET_REDUND == 0) {
    puts("ERROR!!! CONFIG_ENV_OFFSET_REDUND must be defined\n");
    puts("Please re-build and re-burn your image.\n");
    return 1;
    }
   
    _env_new = (env_t *)__builtin_alloca(CONFIG_ENV_SIZE);

    memset(&nand_erase_options, 0, sizeof(nand_erase_options));
    nand_erase_options.length = CONFIG_ENV_RANGE;

    if (CONFIG_ENV_RANGE < CONFIG_ENV_SIZE)
        return 1;

    res = (char *)&env_new.data;
    len = hexport_r(&env_htab, '\0', &res, ENV_SIZE, 0, NULL);
    if (len < 0) {
        error("Cannot export environment: errno = %d\n", errno);
        return 1;
    }
    
    env_new.crc = crc32(0, env_new.data, ENV_SIZE);
    env_new.flags	= ++env_flags; /* increase the serial */

    if (gd->env_valid == 1) {
        puts("Erasing redundant NAND...\n");
        nand_erase_options.offset = CONFIG_ENV_OFFSET_REDUND;
        if (nand_erase_opts(nand, &nand_erase_options))
            return 1;

        puts("Writing to redundant NAND... ");
        ret = writeenv(CONFIG_ENV_OFFSET_REDUND, (u_char *)&env_new);
    } else {
        puts("Erasing NAND...\n");
        nand_erase_options.offset = CONFIG_ENV_OFFSET;
        if (nand_erase_opts(nand, &nand_erase_options))
        return 1;

        puts("Writing to NAND... ");
        ret = writeenv(CONFIG_ENV_OFFSET, (u_char *)&env_new);
    }

    if (ret) {
        puts("FAILED!\n");
        return 1;
    }

    puts("done\n");
    
    gd->env_valid = gd->env_valid == 2 ? 1 : 2;
    
    return ret;
}
#else /* ! CONFIG_ENV_OFFSET_REDUND */
int saveenv(void)
{
//	env_t	env_new;
    env_t	*_env_new;
#define env_new (*(_env_new))	
    ssize_t	len;
    char	*res;
    nand_erase_options_t nand_erase_options;
    nand_info_t *nand = (nand_info_t *)get_mtd_info();

    _env_new = (env_t *)__builtin_alloca(CONFIG_ENV_SIZE);

    memset(&nand_erase_options, 0, sizeof(nand_erase_options));
    nand_erase_options.length = CONFIG_ENV_RANGE;
    nand_erase_options.offset = CONFIG_ENV_OFFSET;

    if (CONFIG_ENV_RANGE < CONFIG_ENV_SIZE)
        return 1;

    res = (char *)&env_new.data;
    len = hexport_r(&env_htab, '\0', &res, ENV_SIZE, 0, NULL);
    if (len < 0) {
        error("Cannot export environment: errno = %d\n", errno);
        return 1;
    }
    env_new.crc = crc32(0, env_new.data, ENV_SIZE);

    puts("Erasing Nand...\n");
    if (nand_erase_opts(nand, &nand_erase_options))
        return 1;

    puts("Writing to Nand... ");
    if (writeenv(CONFIG_ENV_OFFSET, (u_char *)&env_new)) {
        puts("FAILED!\n");
        return 1;
    }

    puts("done\n");
    return 0;
}
#endif /* ! CONFIG_ENV_OFFSET_REDUND */
#endif /* CMD_SAVEENV */


int readenv(size_t offset, u_char *buf)
{
    size_t end = offset + CONFIG_ENV_RANGE;
    size_t amount_loaded = 0;
    size_t blocksize, len;
    u_char *char_ptr;
    nand_info_t *nand = (nand_info_t *)get_mtd_info();
    blocksize = nand->erasesize;

    if (!blocksize) return 1;

    len = min(blocksize, CONFIG_ENV_SIZE);

    while (amount_loaded < CONFIG_ENV_SIZE && offset < end) {
        if (nand_block_isbad(nand, offset)) {
            offset += blocksize;
        } else {
        char_ptr = &buf[amount_loaded];
            if (nand_read_skip_bad(nand, offset,
                           &len, char_ptr))
                return 1;

        offset += blocksize;
        amount_loaded += len;
	}
    }

    if (amount_loaded != CONFIG_ENV_SIZE) return 1;

    return 0;
}

#ifdef CONFIG_ENV_OFFSET_REDUND
void env_relocate_spec(void)
{
#if !defined(ENV_IS_EMBEDDED)
	int crc1_ok = 0, crc2_ok = 0;
	env_t *ep, *tmp_env1, *tmp_env2;

	tmp_env1 = (env_t *)malloc(CONFIG_ENV_SIZE);
	tmp_env2 = (env_t *)malloc(CONFIG_ENV_SIZE);
	if (tmp_env1 == NULL || tmp_env2 == NULL) {
		puts("Can't allocate buffers for environment\n");
		set_default_env("!malloc() failed");
		goto done;
	}

	if (readenv(CONFIG_ENV_OFFSET, (u_char *) tmp_env1))
		puts("No Valid Environment Area found\n");

	if (readenv(CONFIG_ENV_OFFSET_REDUND, (u_char *) tmp_env2))
		puts("No Valid Redundant Environment Area found\n");

	crc1_ok = crc32(0, tmp_env1->data, ENV_SIZE) == tmp_env1->crc;
	crc2_ok = crc32(0, tmp_env2->data, ENV_SIZE) == tmp_env2->crc;

	if (!crc1_ok && !crc2_ok) {
		set_default_env("!bad CRC");
//#ifdef CONFIG_LUNA_MULTI_BOOT
		saveenv();
//#endif /* CONFIG_LUNA_MULTI_BOOT */
		goto done;
	} else if (crc1_ok && !crc2_ok) {
		gd->env_valid = 1;
	} else if (!crc1_ok && crc2_ok) {
		gd->env_valid = 2;
	} else {
		/* both ok - check serial */
		if (tmp_env1->flags == 255 && tmp_env2->flags == 0)
			gd->env_valid = 2;
		else if (tmp_env2->flags == 255 && tmp_env1->flags == 0)
			gd->env_valid = 1;
		else if (tmp_env1->flags > tmp_env2->flags)
			gd->env_valid = 1;
		else if (tmp_env2->flags > tmp_env1->flags)
			gd->env_valid = 2;
		else /* flags are equal - almost impossible */
			gd->env_valid = 1;
	}
	if(env_ptr!=NULL)
	free(env_ptr);

	if (gd->env_valid == 1)
		ep = tmp_env1;
	else
		ep = tmp_env2;

	env_flags = ep->flags;
	env_import((char *)ep, 0);

done:
	free(tmp_env1);
	free(tmp_env2);

#endif /* ! ENV_IS_EMBEDDED */
}
#else /* ! CONFIG_ENV_OFFSET_REDUND */
void env_relocate_spec(void)
{
#if !defined(ENV_IS_EMBEDDED)
    int ret;
    char buf[CONFIG_ENV_SIZE];

    ret = readenv(CONFIG_ENV_OFFSET, (u_char *)buf);
    if (ret) {
        set_default_env("!readenv() failed");
        return;
    }
    env_import(buf, 1);
#endif /* ! ENV_IS_EMBEDDED */
}
#endif /* CONFIG_ENV_OFFSET_REDUND */

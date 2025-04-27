/*
 * flash layout module
 * Author: bohungwu@realtek.com
 * Last modified: May 15, 2014
 */

#include <common.h>
#include <soc.h>
#include <pblr.h>

//#define FL_DEBUG
#ifdef FL_DEBUG
#define DEBUG(format, args...) printf(format, ##args)
#else
#define DEBUG(args...)
#endif

#define CL_SIZE (256)
 
#define plr_param_soc (parameters.soc)

#define GB_BITS (30)
#define MB_BITS (20)
#define KB_BITS (10)
#define GB_MASK ((0x1<<GB_BITS)-1)
#define MB_MASK ((0x1<<MB_BITS)-1)
#define KB_MASK ((0x1<<KB_BITS)-1)

typedef struct {
	char* name;
	u32 base;
	u32 size;
} part_info_t;

#define BOOT_KERNEL "linux"
#define BOOT_ROOTFS "rootfs"
static part_info_t part_info[] = {
	{"config",    0, 0}, /* opt1    */
	{"env2",      0, 0}, /* opt2    */
	{"opt3",      0, 0}, /* opt3    */
	{"opt4",      0, 0}, /* opt4    */
#ifdef CONFIG_LUNA_MULTI_BOOT
	{"k0",        0, 0}, /* kernel1 */
	{"k1",        0, 0}, /* kernel2    */
	{"r0",        0, 0}, /* rootfs1 */
	{"r1",        0, 0}, /* rootfs2 */
#else
	{BOOT_KERNEL, 0, 0}, /* kernel1 */
	{"kernel2",   0, 0}, /* kernel2    */
	{BOOT_ROOTFS, 0, 0}, /* rootfs1 */
	{"rootfs2",   0, 0}, /* rootfs2 */
#endif
	{"END",       0, 0}, /* end     */ 
	{"env",       0, 0}, /* U-Boot environment partition */
	{"boot",      0, 0}, /* BOOTLOADER partition */
};
/* Should be consistent with part_info defined above */
#define FL_OPT1 0
#define FL_OPT2 1
#define FL_OPT3 2
#define FL_OPT4 3
#define FL_KERNEL1 4
#define FL_KERNEL2 5
#define FL_ROOTFS1 6
#define FL_ROOTFS2 7
#define FL_END 8
#define FL_ENV 9
#define FL_BOOT 10

static part_info_t *pi_ptr[ARRAY_SIZE(part_info)];

#ifdef CONFIG_OTTO_FL_TO_MTDPARTS
char * to_KMGB(u32 val, char *output_p, u32 output_size) {
	if((val & GB_MASK) == 0) { /* To GB */
		sprintf(output_p, "%uG", (val >> GB_BITS));
	} else if((val & MB_MASK) == 0) {/* To MB */
		sprintf(output_p, "%uM", (val >> MB_BITS));
	} else if((val & KB_MASK) == 0) { /* To KB */
		sprintf(output_p, "%uK", (val >> KB_BITS));
	} else {
		sprintf(output_p, "%u", (val));
	}
	return output_p;
	
}

#define TS_SIZE (64) /* Magic number temp string size */
#define VIRTUAL_MTD_START 12
const u32 FL_KERNEL_ID[] = {FL_KERNEL1, FL_KERNEL2};
const u32 FL_ROOTFS_ID[] = {FL_ROOTFS1, FL_ROOTFS2};
//Generate mtdparts (root=31:? is also included)
static void flash_layout_to_mtdparts(char *mtdparts_output, u32 img_id) {
	char tmp_str0[TS_SIZE] = {0};
	char tmp_str1[TS_SIZE] = {0};
	u32 i, j, rootfs_partition;
	char *p;

	p = mtdparts_output;
	rootfs_partition = ARRAY_SIZE(part_info);

	p += sprintf(p, "mtdparts=%s:", RTK_MTD_DEV_NAME);
	for(i = 0, j = 0; i < ARRAY_SIZE(part_info); i++) {
		if(pi_ptr[i]->size != 0) {
			p += sprintf(p, "%s(%s),", to_KMGB(pi_ptr[i]->size, tmp_str0, TS_SIZE), pi_ptr[i]->name);
			//p += sprintf(p, "%s@%s(%s),", to_KMGB(pi_ptr[i]->size, tmp_str0, TS_SIZE), to_KMGB(pi_ptr[i]->base, tmp_str1, TS_SIZE), pi_ptr[i]->name);

			/* Record rootfs partition */
			if(pi_ptr[i]->name == part_info[FL_ROOTFS_ID[img_id]].name) {
				DEBUG("pi_ptr[i]->name=%s, part_info[FL_ROOTFS_ID[img_id]].name=%s,i=%d,j=%d\n", pi_ptr[i]->name, part_info[FL_ROOTFS_ID[img_id]].name, i, j);
				rootfs_partition = j;
#if (OTTO_NAND_FLASH==1)
				rootfs_partition--; /* Magic number handling for NAND platform */
				DEBUG("rootfs_partition-- for NAND platform\n");
#endif /* (OTTO_NAND_FLASH==1) */
			}
			j++;
		}
	}
#ifdef CONFIG_LUNA_MULTI_BOOT
	/* Fill padding mtd partition */
	for(;j<VIRTUAL_MTD_START;j++) {
		p += sprintf(p, "4K@0ro,");
		//p += sprintf(p, "4K@%s(m)ro,", to_KMGB(part_info[FL_END].size, tmp_str0, TS_SIZE));
	}
	/* Create alias partition for booting linux & kernel */
	p += sprintf(p, "%s@%s(%s),", to_KMGB(part_info[FL_KERNEL_ID[img_id]].size, tmp_str0, TS_SIZE), to_KMGB(part_info[FL_KERNEL_ID[img_id]].base, tmp_str1, TS_SIZE), BOOT_KERNEL);
	p += sprintf(p, "%s@%s(%s),", to_KMGB(part_info[FL_ROOTFS_ID[img_id]].size, tmp_str0, TS_SIZE), to_KMGB(part_info[FL_ROOTFS_ID[img_id]].base, tmp_str1, TS_SIZE), BOOT_ROOTFS);
#endif

	/* Replace the last ',' (comma sign) by a null character '\0' */
	if(mtdparts_output[strlen(mtdparts_output) - 1] == ',') { 
		mtdparts_output[strlen(mtdparts_output) - 1] = '\0';
	}

	if(rootfs_partition == ARRAY_SIZE(part_info)) {
		printf("Error: Cannot find rootfs partition\n");
	} else {
		DEBUG("rootfs_partition=%d\n", rootfs_partition);
		p = mtdparts_output + strlen(mtdparts_output);
		p += sprintf(p, " root=31:%d", rootfs_partition);
	}

}

#endif /* #ifdef CONFIG_OTTO_FL_TO_MTDPARTS */

void sort_part_info(part_info_t *input_pi_ptr[], u32 num) {
	u32 i, j;
	part_info_t *item_p;

	/* Use insertion sort as the number of elements in the array is not huge */
	for(i=1; i<num; i++) {
		j = i;
		item_p = input_pi_ptr[j];
		while( (j > 0) && (item_p->base < input_pi_ptr[j-1]->base) ) {
			input_pi_ptr[j] = input_pi_ptr[j - 1];
			j--;
		}
		input_pi_ptr[j]= item_p;
	}
}


 u32 otto_get_flash_size(void) {
	u8 nc, spc;
	u32 ret_val;
#if (OTTO_NAND_FLASH==1)
	ret_val=plr_param_soc.flash_info.page_size * plr_param_soc.flash_info.num_page_per_block*plr_param_soc.flash_info.num_block;
#else
	nc  = plr_param_soc.flash_info.num_chips;
	spc = plr_param_soc.flash_info.size_per_chip;
	//printf("nc=%u, spc=%u\n", nc, spc);
	ret_val = nc * (0x1 << spc);
#endif
	return ret_val;
}

void part_info_init(void) {
	u32 i;

	part_info[0].base = plr_param_soc.layout.opt1_addr;
	part_info[1].base = plr_param_soc.layout.opt2_addr;
	part_info[2].base = plr_param_soc.layout.opt3_addr;
	part_info[3].base = plr_param_soc.layout.opt4_addr;
	part_info[4].base = plr_param_soc.layout.kernel1_addr;
	part_info[5].base = plr_param_soc.layout.kernel2_addr;
	part_info[6].base = plr_param_soc.layout.rootfs1_addr;
	part_info[7].base = plr_param_soc.layout.rootfs2_addr;

	if(plr_param_soc.layout.end_addr == 0) {
		//plr_param_soc.layout.end_addr = otto_get_spi_flash_size(); //This failed as soc_t is declarded as const
		DEBUG("INFO: layout.end_addr is 0, fill flash size to the base of end partition\n");
		part_info[8].base = otto_get_flash_size();
	} else {
		part_info[8].base = plr_param_soc.layout.end_addr;
	}
	part_info[9].base = plr_param_soc.layout.env_addr;

	for(i=0; i<ARRAY_SIZE(part_info); i++) {
		pi_ptr[i] = &part_info[i];
	}
	DEBUG("Original setting\n");
	DEBUG("pi_ptr: name\tbase\tsize\n");
	for(i=0; i<ARRAY_SIZE(part_info); i++) {
		DEBUG("[%d]: %s\t0x%08x\t0x%08x\n", i, pi_ptr[i]->name, pi_ptr[i]->base, pi_ptr[i]->size);
	}

	sort_part_info(pi_ptr, ARRAY_SIZE(part_info));
	DEBUG("After sorting\n");
	DEBUG("pi_ptr: name\tbase\tsize\n");
	for(i=0; i<ARRAY_SIZE(part_info); i++) {
		DEBUG("[%d]: %s\t0x%08x\t0x%08x\n", i, pi_ptr[i]->name, pi_ptr[i]->base, pi_ptr[i]->size);
	}

	/* Fill size */
	for(i=0; i<(ARRAY_SIZE(part_info)-1); i++) {
		pi_ptr[i]->size = pi_ptr[i+1]->base - pi_ptr[i]->base;
	}
	DEBUG("After filling size\n");
	DEBUG("pi_ptr: name\tbase\tsize\n");
	for(i=0; i<ARRAY_SIZE(part_info); i++) {
		DEBUG("[%d]: %s\t0x%08x\t0x%08x\n", i, pi_ptr[i]->name, pi_ptr[i]->base, pi_ptr[i]->size);
	}

	DEBUG("Valid partitions\n");
	DEBUG("part_info: name\tbase\tsize\n");
	for(i=0; i<ARRAY_SIZE(part_info); i++) {
		if(pi_ptr[i]->size !=0) {
			DEBUG("[%d]: %s\t0x%08x\t0x%08x\n", i, pi_ptr[i]->name, pi_ptr[i]->base, pi_ptr[i]->size);
		}
	}
}

void otto_flash_layout_init(void) {
#ifdef CONFIG_OTTO_FL_TO_MTDPARTS
	char str_tmp[CL_SIZE] = {0};
#endif /* #ifdef CONFIG_OTTO_FL_TO_MTDPARTS */

	setenv_addr("pfl_env", (void *)parameters.soc.layout.env_addr); 
	setenv_addr("pfl_env_sz", (void *)parameters.soc.layout.env_size); 
	setenv_addr("pfl_opt1", (void *)parameters.soc.layout.opt1_addr); 
	setenv_addr("pfl_kernel1", (void *)parameters.soc.layout.kernel1_addr); 
	setenv_addr("pfl_rootfs1", (void *)parameters.soc.layout.rootfs1_addr); 

	part_info_init();
	setenv_addr("pfl_boot_sz", (void *)part_info[FL_BOOT].size); 
	setenv_addr("pfl_opt1_sz", (void *)part_info[FL_OPT1].size); 
	setenv_addr("pfl_kernel1_sz", (void *)part_info[FL_KERNEL1].size); 
	setenv_addr("pfl_rootfs1_sz", (void *)part_info[FL_ROOTFS1].size); 
#ifdef CONFIG_SPINOR_FLASH
	setenv_addr("img0_kernel", (void *)(CONFIG_SYS_FLASH_BASE + parameters.soc.layout.kernel1_addr)); 
#endif /* #ifdef CONFIG_SPINOR_FLASH */

#ifdef CONFIG_LUNA_MULTI_BOOT
	setenv_addr("pfl_opt2", (void *)parameters.soc.layout.opt2_addr); 
	setenv_addr("pfl_kernel2", (void *)parameters.soc.layout.kernel2_addr); 
	setenv_addr("pfl_rootfs2", (void *)parameters.soc.layout.rootfs2_addr); 
	setenv_addr("pfl_kernel2_sz", (void *)part_info[FL_KERNEL2].size); 
	setenv_addr("pfl_rootfs2_sz", (void *)part_info[FL_ROOTFS2].size); 
#ifdef CONFIG_SPINOR_FLASH
	setenv_addr("img1_kernel", (void *)(CONFIG_SYS_FLASH_BASE + parameters.soc.layout.kernel2_addr)); 
#endif /* #ifdef CONFIG_SPINOR_FLASH */
#endif /* #ifdef CONFIG_LUNA_MULTI_BOOT */

#ifdef CONFIG_OTTO_FL_TO_MTDPARTS
	flash_layout_to_mtdparts(str_tmp, 0);
	printf("mtdparts0=%s\n", str_tmp);
	setenv("mtdparts0", str_tmp);
#ifdef CONFIG_LUNA_MULTI_BOOT
	flash_layout_to_mtdparts(str_tmp, 1);
	printf("mtdparts1=%s\n", str_tmp);
	setenv("mtdparts1", str_tmp);
#endif /* #ifdef CONFIG_LUNA_MULTI_BOOT */
#endif /* #ifdef CONFIG_OTTO_FL_TO_MTDPARTS */

	return;
}




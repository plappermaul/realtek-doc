


#include <common.h>
#include <command.h>

#define CMD_BUF_SZ (256)
#define MMC_DEV_NUM "mmc_dev"


//#define PARTINFO_DEBUG
#ifdef PARTINFO_DEBUG
 #define PARTINFO_DBG(...) printf(__VA_ARGS__)
 #else
 #define PARTINFO_DBG(...)
#endif

int
do_part_info(int argc, char *const argv[])
{
	char *value;
	char *mmc_dev = "0";
	char *prefix = "par";
	char cmd_buf[CMD_BUF_SZ] = {0};
	char name_buf[CMD_BUF_SZ];
	char *part_num;
	char *part_name = argv[0];

	if (!argc) {
		return CMD_RET_USAGE;
	}

	value = env_get(MMC_DEV_NUM);
	if (value) {
		mmc_dev = value;
	}

	if(argc > 1){
		prefix = argv[1];
	}

	// reset partitions number variable
	snprintf(name_buf,sizeof(name_buf)-1, "%s_st", prefix);
	env_set(name_buf, "");
	snprintf(name_buf,sizeof(name_buf)-1, "%s_sz", prefix);
	env_set(name_buf, "");
	snprintf(name_buf,sizeof(name_buf)-1, "%s_num", prefix);
	env_set(name_buf, "");

	// get partition number
	snprintf(cmd_buf, sizeof(cmd_buf)-1, "part number mmc %s %s %s", mmc_dev, part_name, name_buf);
	PARTINFO_DBG("CMD: %s\n", cmd_buf);
	run_command(cmd_buf, 0);
	part_num = env_get(name_buf);
	if (NULL == part_num) {
		printf("Partition \"%s\" not found!\n", part_name);
		return -1;
	}
	PARTINFO_DBG("%s of %s is %s\n", name_buf, part_name, env_get(name_buf));

	// get partitions start
	snprintf(name_buf,sizeof(name_buf)-1, "%s_st", prefix);
	snprintf(cmd_buf, sizeof(cmd_buf)-1, "part start mmc %s %s %s", mmc_dev, part_name, name_buf);
	PARTINFO_DBG("CMD: %s\n", cmd_buf);
	run_command(cmd_buf, 0);
	PARTINFO_DBG("%s of %s is %s\n", name_buf, part_name, env_get(name_buf));


	// get partitions size
	snprintf(name_buf,sizeof(name_buf)-1, "%s_sz", prefix);
	snprintf(cmd_buf, sizeof(cmd_buf)-1, "part size mmc %s %s %s", mmc_dev, part_name, name_buf);
	PARTINFO_DBG("CMD: %s\n", cmd_buf);
	run_command(cmd_buf, 0);
	PARTINFO_DBG("%s of %s is %s\n", name_buf, part_name, env_get(name_buf));


	return 0;
}


int
do_part_infodec(int argc, char *const argv[])
{
	char *value;
	char *mmc_dev = "0";
	char *prefix = "par";
	char cmd_buf[CMD_BUF_SZ] = {0};
	char name_buf[CMD_BUF_SZ];
	char *part_num;
	char *part_name = argv[0];
	int value_dec;

	if (!argc) {
		return CMD_RET_USAGE;
	}

	value = env_get(MMC_DEV_NUM);
	if (value) {
		mmc_dev = value;
	}

	if(argc > 1){
		prefix = argv[1];
	}

	// reset partitions number variable
	snprintf(name_buf,sizeof(name_buf)-1, "%s_st", prefix);
	env_set(name_buf, "");
	snprintf(name_buf,sizeof(name_buf)-1, "%s_sz", prefix);
	env_set(name_buf, "");
	snprintf(name_buf,sizeof(name_buf)-1, "%s_num", prefix);
	env_set(name_buf, "");

	// get partition number
	snprintf(cmd_buf, sizeof(cmd_buf)-1, "part number mmc %s %s %s", mmc_dev, part_name, name_buf);
	PARTINFO_DBG("CMD: %s\n", cmd_buf);
	run_command(cmd_buf, 0);
	part_num = env_get(name_buf);
	if (NULL == part_num) {
		printf("Partition \"%s\" not found!\n", part_name);
		return -1;
	}
	value_dec = env_get_hex(name_buf, 0xFFFF);
	env_set_ulong(name_buf, value_dec);
	PARTINFO_DBG("%s of %s is %s\n", name_buf, part_name, env_get(name_buf));

	// get partitions start
	snprintf(name_buf,sizeof(name_buf)-1, "%s_st", prefix);
	snprintf(cmd_buf, sizeof(cmd_buf)-1, "part start mmc %s %s %s", mmc_dev, part_name, name_buf);
	PARTINFO_DBG("CMD: %s\n", cmd_buf);
	run_command(cmd_buf, 0);

	//value = env_get(name_buf);
	//value_dec = simple_strtoul(value, 16);
	value_dec = env_get_hex(name_buf, 0xFFFF);
	value_dec = value_dec * 512;
	env_set_ulong(name_buf, value_dec);
	PARTINFO_DBG("%s of %s is %s\n", name_buf, part_name, env_get(name_buf));


	// get partitions size
	snprintf(name_buf,sizeof(name_buf)-1, "%s_sz", prefix);
	snprintf(cmd_buf, sizeof(cmd_buf)-1, "part size mmc %s %s %s", mmc_dev, part_name, name_buf);
	PARTINFO_DBG("CMD: %s\n", cmd_buf);
	run_command(cmd_buf, 0);

	value_dec = env_get_hex(name_buf, 0);
	value_dec = value_dec * 512;
	env_set_ulong(name_buf, value_dec);
	PARTINFO_DBG("%s of %s is %s\n", name_buf, part_name, env_get(name_buf));
	return 0;
}


int
do_part_info_clean(int argc, char *const argv[])
{
	char name_buf[CMD_BUF_SZ];
	if (!argc) {
		return CMD_RET_USAGE;
	}
	char *prefix = argv[0];

	snprintf(name_buf,sizeof(name_buf)-1, "%s_st", prefix);
	env_set(name_buf, "");
	snprintf(name_buf,sizeof(name_buf)-1, "%s_sz", prefix);
	env_set(name_buf, "");
	snprintf(name_buf,sizeof(name_buf)-1, "%s_num", prefix);
	env_set(name_buf, "");
	return 0;
}

static int do_partinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if (argc < 2)
		return CMD_RET_USAGE;

	if (!strcmp(argv[1], "info")) {
		return do_part_info(argc - 2, argv + 2);
	} else if(!strcmp(argv[1], "infodec")) {
		return do_part_infodec(argc - 2, argv + 2);
	} else if(!strcmp(argv[1], "clean")){
		return do_part_info_clean(argc - 2, argv + 2);
	}
	return CMD_RET_USAGE;
}


U_BOOT_CMD(
	partinfo,	4,	1,	do_partinfo,
	"get infomation of partition",
	"info part <prefix>\n"
	"    get mmc part info number(num)/start(st)/size(sz) int par_num/par_st/par_sz or replace par with prefix\n"
	"infodec part <prefix>\n"
	"    get mmc part decimal info number(num)/start(st)/size(sz) int par_num/par_st/par_sz or replace par with prefix\n"
	"clean prefix\n"
	"    clean env variable number(num)/start(st)/size(sz) int <prefix>_num/<prefix>_st/<prefix>_sz\n"
);


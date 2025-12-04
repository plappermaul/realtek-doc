#include <common.h>
#include <console.h>
#include <bootretry.h>
#include <cli.h>
#include <command.h>
#include <console.h>
#include <hash.h>
#include <mapmem.h>
#include <watchdog.h>
#include <asm/io.h>
#include <linux/compiler.h>


__attribute__((weak)) unsigned int y = 1U;

__attribute__((weak)) unsigned int rand_r(unsigned int *seedp)
{
        *seedp ^= (*seedp << 13);
        *seedp ^= (*seedp >> 17);
        *seedp ^= (*seedp << 5);

        return *seedp;
}

__attribute__((weak)) unsigned int rand(void)
{
        return rand_r(&y);
}

__attribute__((weak)) void srand(unsigned int seed)
{
        y = seed;
}


#define XRAM_BASE		0xF4500000
#define XRAM_SIZE		0x2000
#define RAND_LOOP 	10000
#define NORMAL_LOOP 5000
static u8 xram_test_data[XRAM_SIZE];
u8 xram_test_data_addr_printed=0;
u32 pat = 0;

static void xram_data_randon(void){
	unsigned int *buf	;
	unsigned long seed; // NOT INITIALIZED ON PURPOSE
	u32 i;

	if(pat==0){
		seed = get_timer(0) ^ rand();
		srand(seed);


		buf = (unsigned int *)xram_test_data;
		if(xram_test_data_addr_printed == 0){
			printf("%s: Random xram_test_data at 0x%8llx\n", __func__, (u64)xram_test_data);
			xram_test_data_addr_printed=1;
		}

		for (i = 0; i < (XRAM_SIZE / 4); i++){
			*buf++ = rand();
		}
	}else{
		buf = (unsigned int *)xram_test_data;
		printf("%s: Input xram_test_data at 0x%8llx\n", __func__, (u64)xram_test_data);

		for (i = 0; i < (XRAM_SIZE / 4); i++){
			*buf++ = pat;
		}
	}
	return;
}

static int xram_data_cmp( int width){
	u32 count;
	void *buf, *start;
	void *data;
	int i;

	count = XRAM_SIZE/width;
	start = (void *)XRAM_BASE;

	data = (void *)xram_test_data;
	buf = start;


	for(i=0;i<count;i++){
		if (width == 4){
			if( *((u32 *)buf) != *((u32 *)data)){
				printf("Sequence check width(%d): buf(0x%8lx)= 0x%8x != data(0x%8lx)= 0x%8x \n", width, (unsigned long)buf,  *((u32 *)buf), (unsigned long)data, *((u32 *)data) );
				return 1;
			}
		}else if (width == 8){
			if(*((u64 *)buf) != *((u64 *)data)){
				printf("Sequence check width(%d): buf(0x%8lx)= 0x%llx != data(0x%8lx)= 0x%llx \n", width, (unsigned long)buf,  *((u64 *)buf), (unsigned long)data, *((u64 *)data) );
				return 1;
			}
		}else if (width == 2){
			if(*((u16 *)buf) != *((u16 *)data)){
				printf("Sequence check width(%d): buf(0x%8lx)= 0x%4x != data(0x%8lx)= 0x%4x \n", width, (unsigned long)buf,  *((u16 *)buf), (unsigned long)data, *((u16 *)data) );
				return 1;
			}
		}else{
			if(*((u8 *)buf) != *((u8 *)data)){
				printf("Sequence check width(%d): buf(0x%8lx)= 0x%x != data(0x%8lx)= 0x%x \n", width, (unsigned long)buf,  *((u8 *)buf), (unsigned long)data, *((u8 *)data) );
				return 1;
			}
		}
		buf += width;
		data += width;
	}

	//Reverve chk
	data = (void *)((u64)xram_test_data + (count-1)*width);
	buf = (void *)((u64)start + (count-1)*width);


	for(i=count;i>0;i--){
		if (width == 4){
			if( *((u32 *)buf) != *((u32 *)data)){
				printf("Reverse check. width(%d): buf(0x%8lx)= 0x%8x != data(0x%8lx)= 0x%8x \n", width, (unsigned long)buf,  *((u32 *)buf), (unsigned long)data, *((u32 *)data) );
				return 1;
			}
		}else if (width == 8){
			if(*((u64 *)buf) != *((u64 *)data)){
				printf("Reverse check. width(%d): buf(0x%8lx)= 0x%llx != data(0x%8lx)= 0x%llx \n", width, (unsigned long)buf,  *((u64 *)buf), (unsigned long)data, *((u64 *)data) );
				return 1;
			}
		}else if (width == 2){
			if(*((u16 *)buf) != *((u16 *)data)){
				printf("Reverse check. width(%d): buf(0x%8lx)= 0x%4x != data(0x%8lx)= 0x%4x \n", width, (unsigned long)buf,  *((u16 *)buf), (unsigned long)data, *((u16 *)data) );
				return 1;
			}
		}else{
			if(*((u8 *)buf) != *((u8 *)data)){
				printf("Reverse check. width(%d): buf(0x%8lx)= 0x%x != data(0x%8lx)= 0x%x \n", width, (unsigned long)buf,  *((u8 *)buf), (unsigned long)data, *((u8 *)data) );
				return 1;
			}
		}
		buf -= width;
		data -= width;
	}
	return 0;
}

static int do_mem_mwt(int	width)
{
	u32 count;
	void *buf, *start;
	void *data;
	int loop=0,i;
	count = XRAM_SIZE/width;

	start = (void *)XRAM_BASE;

printf("%s: LOOP(%d), data_width(%d)\n", __func__, NORMAL_LOOP, width);
	while(loop<NORMAL_LOOP){

		xram_data_randon();
		data = (void *)xram_test_data;
		buf = start;

		for(i=0;i<count;i++){
			if (width == 4){
				*((u32 *)buf) = *((u32 *)data);
			}else if (width == 8){
				*((u64 *)buf) = *((u64 *)data);
			}else if (width == 2){
				*((u16 *)buf) = *((u16 *)data);
			}else{
				*((u8 *)buf) = *((u8 *)data);
			}
			buf += width;
			data += width;
		}

		if(xram_data_cmp(width)){
			printf("%08d: Sequence Write: data_width(%d)\n", loop, width);
			goto ERROR_RETURN;
		}

	//reverse write
		//printf("\tReverse Write: data_width(%d)\n", width);
		xram_data_randon();
		data = (void *)((u64)xram_test_data + (count-1)*width);
		buf = (void *)((u64)start + (count-1)*width);
		for(i=count;i>0;i--){
			if (width == 4){
				*((u32 *)buf) = *((u32 *)data);
			}else if (width == 8){
				*((u64 *)buf) = *((u64 *)data);
			}else if (width == 2){
				*((u16 *)buf) = *((u16 *)data);
			}else{
				*((u8 *)buf) = *((u8 *)data);
			}
			buf -= width;
			data -= width;
		}
		if(xram_data_cmp(width)){
			printf("%08d: Reserve Write: data_width(%d)\n", loop, width);
			goto ERROR_RETURN;
		}

		loop++;
	}
	printf("\t\t\tPASS\n");
	return 0;
ERROR_RETURN:
	printf("\t\t\tError\n");
	return 1;
}

static int do_mem_mwt_rand_size(void)
{
	u32 real_count, count;
	void *buf;
	void *data;
	int loop=0,i;
	int width;
	int start_count=0;

	void *data_p, *xram_p;



	printf("%s: RAND_LOOP(%d)\n", __func__, RAND_LOOP);

	while(loop<RAND_LOOP){
		srand(get_timer(0) ^ rand());
		width = 1<<(rand()% 4);
		real_count = XRAM_SIZE/width;
		start_count = (rand()%real_count);
		count = real_count-start_count;

		xram_data_randon();
		data_p = (void *)xram_test_data + start_count*width;
		xram_p = (void *)XRAM_BASE + start_count*width;

		data = data_p;
		buf = xram_p;

		for(i=0;i<count;i++){
			if (width == 4){
				*((u32 *)buf) = *((u32 *)data);
			}else if (width == 8){
				*((u64 *)buf) = *((u64 *)data);
			}else if (width == 2){
				*((u16 *)buf) = *((u16 *)data);
			}else{
				*((u8 *)buf) = *((u8 *)data);
			}
			buf += width;
			data += width;
		}

		if(memcmp(data_p, xram_p, count*width)){
				printf("%d:\tSequence Write: data_width(%d),start_count(%d), count(%d)\n", loop, width, start_count, count);
					printf("%p, %p, %d\n", data_p, xram_p, count*width);
			goto ERROR_RETURN;
		}

	//reverse write

		xram_data_randon();
		data = data_p + (count-1)*width;
		buf = xram_p + (count-1)*width;
		for(i=count;i>0;i--){
			if (width == 4){
				*((u32 *)buf) = *((u32 *)data);
			}else if (width == 8){
				*((u64 *)buf) = *((u64 *)data);
			}else if (width == 2){
				*((u16 *)buf) = *((u16 *)data);
			}else{
				*((u8 *)buf) = *((u8 *)data);
			}
			buf -= width;
			data -= width;
		}
		if(memcmp(data_p, xram_p, count*width)){
				printf("%d:\tReserve Write: data_width(%d),start_count(%d), count(%d)\n", loop, width, start_count, count);
					printf("%p, %p, %d\n", data_p, xram_p, count*width);
			goto ERROR_RETURN;
		}

		loop++;
	}
	printf("\t\t\tPASS\n");
	return 0;
ERROR_RETURN:
	printf("\t\t\tError\n");
	return 1;
}
static void do_random_addr_test(void){
	int width;
	u32 count;
	u64 target;
	void *buf;
	void *data;
	u64 rand_data;
	int i;

	xram_data_randon();
	memcpy((void *)XRAM_BASE, (void *)xram_test_data, XRAM_SIZE);

	if(memcmp((void *)XRAM_BASE, (void *)xram_test_data, XRAM_SIZE)){
		printf("%s: memcmp failed\n", __func__);
		return;
	}

	printf("%s: LOOP=%d\n", __func__, RAND_LOOP);

	for(i=0;i<RAND_LOOP;i++){
		srand(get_timer(0) ^ rand());

		width = 1<<(rand()% 4);
		count = XRAM_SIZE/width;

		target = (rand()%count)*width;

		data = (void *)((u64)xram_test_data + target);
		buf = (void *)(XRAM_BASE + target);

	//	printf("buf(%p), data(%p), width(%d)\n", buf, data, width);

			if (width == 4){
				 *((u32 *)data) = rand();
				*((u32 *)buf) = *((u32 *)data);
			}else if (width == 8){
				rand_data = rand();
				*((u64 *)data) = rand_data<<32 | rand();
				*((u64 *)buf) = *((u64 *)data);
			}else if (width == 2){
				*((u16 *)data) = rand() & 0xffff;
				*((u16 *)buf) = *((u16 *)data);
			}else{
				*((u8 *)data) = rand() & 0xff;
				*((u8 *)buf) = *((u8 *)data);
			}

			if(xram_data_cmp(width)){
				printf("\t\t\tError. Stop at (%d)\n", i);
				return;
			}
	}
	printf("\t\t\tPASS\n");
	return;
}


static int do_xram_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u32 caseid = 0;

	if (argc <2)
		return CMD_RET_USAGE;

	if (argc > 1){
		caseid = simple_strtoul(argv[1], NULL, 10);

		if (argc > 2){
				pat = simple_strtoul(argv[2], NULL, 16);
		}else{
				pat = 0;
		}



		switch(caseid){
           	case 1:
                    do_mem_mwt(1);
                    break;
            case 2:
                    do_mem_mwt(2);
                    break;

           	case 4:
                    do_mem_mwt(4);
                    break;
            case 8:
                    do_mem_mwt(8);
                    break;
			case 9:
					do_mem_mwt_rand_size();
					break;
			case 0:
					do_random_addr_test();
					break;

           default:
                    printf("Not support %d\n", caseid);
                    break;
		}
	}


	return 0;
}

U_BOOT_CMD(
	xram_test ,    5,    0,     do_xram_test,
	"xram: xram",
	"\n\t\t <case>\n"
);

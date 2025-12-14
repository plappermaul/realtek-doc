/*
 * rngtest.c -- Random Number Generator FIPS 140-1/140-2 tests
 *
 * This program tests the input stream in stdin for randomness,
 * using the tests defined by FIPS 140-1/140-2 2001-10-10.
 *
 * Copyright (C) 2004 Henrique de Moraes Holschuh <hmh@debian.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335  USA
 */
//#define _GNU_SOURCE
//#ifndef HAVE_CONFIG_H
//#error Invalid or missing autoconf build environment
//#endif
//#include "rng-tools-config.h"
//#include <unistd.h>
//#include <stdint.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/time.h>
//#include <time.h>
//#include <string.h>
//#include <argp.h>
//#include <signal.h>
//#include "fips.h"
//#include "stats.h"
//#include "exits.h"
#define PROGNAME "rngtest"
const char* logprefix = PROGNAME ": ";

#define FIPS_RNG_BUFFER_SIZE 2500
#define NULL 0

typedef unsigned long long uint64_t;
typedef unsigned int u32;

/* Context for running FIPS tests */
struct fips_ctx {
	int poker[16], runs[12];
	int ones, rlength, current_bit, last_bit, longrun;
	unsigned int last32;
};
typedef struct fips_ctx fips_ctx_t;

#define N_FIPS_TESTS 5
extern const char *fips_test_names[N_FIPS_TESTS];
extern const unsigned int fips_test_mask[N_FIPS_TESTS];

/*
 * argp stuff
 */
//const char *argp_program_version =
//	PROGNAME " " VERSION "\n"
//	"Copyright (c) 2004 by Henrique de Moraes Holschuh\n"
//	"This is free software; see the source for copying conditions.  There is NO "
//	"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.";
//const char *argp_program_bug_address = PACKAGE_BUGREPORT;
//error_t argp_err_exit_status = EXIT_USAGE;
static char doc[] =
	"Check the randomness of data using FIPS 140-2 RNG tests.\n"
	"\v"
	"FIPS tests operate on 20000-bit blocks.  Data is read from stdin.  Statistics "
	"and messages are sent to stderr.\n\n"
	"If no errors happen nor any blocks fail the FIPS tests, the program will return "
	"exit status 0.  If any blocks fail the tests, the exit status will be 1.\n";
//static struct argp_option options[] = {
//	{ "blockcount", 'c', "n", 0,
//	  "Exit after processing n blocks (default: 0)" },
//	{ "pipe", 'p', 0, 0,
//	  "Enable pipe mode: work silently, and echo to stdout all good blocks" },
//	{ "timedstats", 't', "n", 0,
//	  "Dump statistics every n secods (default: 0)" },
//	{ "blockstats", 'b', "n", 0,
//	  "Dump statistics every n blocks (default: 0)" },
//	{ 0 },
//};
struct arguments {
	int blockstats;
	uint64_t timedstats;		/* microseconds */
	int pipemode;
	int blockcount;
};
static struct arguments default_arguments = {
	.blockstats	= 0,
	.timedstats	= 0,
	.pipemode	= 0,
	.blockcount	= 0,
};
#if 0
static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;
	switch(key) {
	case 'c': {
		int n;
		if ((sscanf(arg, "%i", &n) == 0) || (n < 0))
			argp_usage(state);
		else
			arguments->blockcount = n;
		break;
	}
	case 'b': {
		int n;
		if ((sscanf(arg, "%i", &n) == 0) || (n < 0))
			argp_usage(state);
		else
			arguments->blockstats = n;
		break;
	}
	case 't': {
		int n;
		if ((sscanf(arg, "%i", &n) == 0) || (n < 0))
			argp_usage(state);
		else
			arguments->timedstats = 1000000ULL * n;
		break;
	}
	case 'p':
		arguments->pipemode = 1;
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}
#endif
/* Min-Max stat */
struct rng_stat {
	uint64_t max;			/* Highest value seen */
	uint64_t min;			/* Lowest value seen */
	uint64_t num_samples;		/* Number of samples */
	uint64_t sum;			/* Sum of all samples */
};

/*
 * Globals
 */
/* RNG Buffers */
unsigned char rng_buffer[FIPS_RNG_BUFFER_SIZE];
/* Statistics */
struct {
	/* simple counters */
	uint64_t bad_fips_blocks;	/* Blocks reproved by FIPS 140-2 */
	uint64_t good_fips_blocks;	/* Blocks approved by FIPS 140-2 */
	uint64_t fips_failures[N_FIPS_TESTS]; 	/* Breakdown of block
					   failures per FIPS test */
	uint64_t bytes_received;	/* Bytes read from input */
	uint64_t bytes_sent;		/* Bytes sent to output */
	/* performance timers */
	struct rng_stat source_blockfill;	/* Block-receive time */
	struct rng_stat fips_blockfill;		/* FIPS run time */
	struct rng_stat sink_blockfill;		/* Block-send time */
	//struct timeval progstart;	/* Program start time */
} rng_stats;
/* Logic and contexts */
static fips_ctx_t fipsctx;		/* Context for the FIPS tests */
//static int exitstatus = EXIT_SUCCESS;	/* Exit status */
/* Command line arguments and processing */
struct arguments *arguments = &default_arguments;
//static struct argp argp = { options, parse_opt, NULL, doc };
/* signals */
static volatile int gotsigterm = 0;	/* Received SIGTERM/SIGINT */
/*
 * Signal handling
 */
#if 0
static void sigterm_handler(int sig)
{
	gotsigterm = sig;
}
static void init_sighandlers(void)
{
	struct sigaction action;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	action.sa_handler = sigterm_handler;
	/* Handle SIGTERM and SIGINT the same way */
	if (sigaction(SIGTERM, &action, NULL) < 0) {
		fprintf(stderr,
			"unable to install signal handler for SIGTERM: %s",
			strerror(errno));
		exit(EXIT_OSERR);
	}
	if (sigaction(SIGINT, &action, NULL) < 0) {
		fprintf(stderr,
			"unable to install signal handler for SIGINT: %s",
			strerror(errno));
	        exit(EXIT_OSERR);
	}
}
#endif

#define REG32(reg)      (*(volatile u32 *)(reg))

int test_max=0;
int test_min=0xfffff;

int test_NUM=0;    //must [0-6]
int test_ro_sel=1;   // must [1-5]
int test_wire_sel=0;  //must [0,1]

char dss_onebyte()
{
	char byte_out=0;
	int temp_data;
	char loop;
        #define SS0_CTRL 0xb8000220
        #define SS0_OUT  0xb8000224
/*
0220h      REG_SPD_SENSOR_0_CTRL                                            
        [31:26]     R      0      BIT_DSS_DBGO_0_5_0   (from hs0_platform_grp) debug output bit 5:0             
        [25:25]     R/W 0      BIT_DSS_SPEED_EN_0    (to hs0_platform_grp) speed detection enable             
        [24:5]       R/W 0      BIT_DSS_DATA_IN_0       (to hs0_platform_grp) [19:0] data in            
        [4]            R/W 0      BIT_DSS_WIRE_SEL_0     (to hs0_platform_grp) interconnect (metal) selection            
        [3:1] R/W 0      BIT_DSS_RO_SEL_0 (to hs0_platform_grp) [2:0] to select ring osc              
        [0]            R/W 0      BIT_DSS_RST_N_0  (to hs0_platform_grp) reset needed by dss controller        
        
0224h      REG_SPD_SENSOR_0_OUT                                             
        [31:22]     R      0      BIT_DSS_DBGO_0_15_6 (from hs0_platform_grp) debug output bit 15:6           
        [21]          R      0      BIT_DSS_WSORT_GO_0 (from hs0_platform_grp) go/no-go for wafer test               
        [20]          R      0      BIT_DSS_READY_0  (from hs0_platform_grp) indicate the test is done and data is ready         
        [19:0]       R      0      BIT_DSS_COUNT_OUT_0        (from hs0_platform_grp) [19:0] data out (valid if ready is high)             



speed_en 寫0
=> rst_n寫1=> rst_n寫0 => rst_n寫1
=> ro_sel & wire_sel 排列組合全部(一次一組) 
=> speed_en寫1 
=>此時量測voltage_probe 
=> 等 ready 為1 => 讀取 count_out
*/

        int NUM=test_NUM;    //must [0-6]
        int ro_sel=test_ro_sel;   // must [1-5]
        int wire_sel=test_wire_sel;  //must [0,1]

		//printf("a");
        //for(NUM=0; NUM<=6; NUM++)
		for (loop=0; loop<8 ; loop++)
        {
                //printf("\n\nSS#%d:", NUM);
                //printf("\tro=1\t\tro=2\t\tro=3\t\tro=4\t\tro=5");
                //for(wire_sel=0; wire_sel<=1; wire_sel++)             
                {
                        //printf("(%d,%d)\t", wire_sel, ro_sel);
                        //printf("\nwire=%d\t", wire_sel);
                        
                        //for(ro_sel=1; ro_sel<=5; ro_sel++)
                        {
                                int base=SS0_CTRL+NUM*8;
                                REG32(base) &= ~(1<<25);   //en=0
                                
                                REG32(base) |=    (1<<0);    //rst=1
                                REG32(base) &= ~(1<<0);    //rst=0
                                REG32(base) |=    (1<<0);    //rst=1       

                                REG32(base) &= ~(7<<1);   //clear bit [3:1]
                                REG32(base) &= ~(1<<4);   //clear bit [4]   
                                REG32(base) |= (ro_sel<<1);
                                REG32(base) |= (wire_sel<<4);

                                REG32(base) |= (1<<25);   //en=1
                                
                                //printf("Speed Sensor #%d, ro_sel=%d, wire_sel=%d\n", NUM, ro_sel, wire_sel);
                                //printf("SS_CTRL=%x,  \n",  REG32(base) );
                                while(1)
                                {       if(REG32(base+4) &(1<<20))   break;
                                }

                                
                                int data=REG32(base+4);
                                //printf("SS_OUTP=%x, [19:0]CNT=%x\n", data , data&0xfffff );
                                //printf("%x\t", data&0xfffff );
								
								//temp_data = data&0xfffff;
								//if (test_max < temp_data)
								//	test_max = temp_data;
								//if (test_min > temp_data)
								//	test_min = temp_data;
								data &=1;
								//printf(" %d", data);
								byte_out <<= 1;
								byte_out |= data;
        
                                       
                        }
                }
        //printf("\n\n");
		}
	//printf(" %x", byte_out);
	return byte_out;
}


static int xread(void *buf, unsigned int size)
{
	unsigned int off = 0;
	unsigned int r;
	char* char_buf=buf;
	while (size) {
		char_buf[off] = dss_onebyte();
		//r = read(0, buf + off, size);
		//if (r < 0) {
		//	if (gotsigterm) return -1;
		//	if ((errno == EAGAIN) || (errno == EINTR)) continue;
		//	break;
		//} else if (!r) {
		//	if (!arguments->pipemode)
		//		fprintf(stderr,
		//			"%sentropy source drained\n",
		//			logprefix);
		//	return -1;
		//}
		off += 1;
		size -= 1;
		rng_stats.bytes_received += 1;
	}
	//if (size) {
	//	fprintf(stderr,
	//		"%serror reading input: %s\n", logprefix,
	//		strerror(errno));
	//	exitstatus = EXIT_IOERR;
	//	return -1;
	//}
	return 0;
}
#if 0
static int xwrite(void *buf, size_t size)
{
	size_t off = 0;
	ssize_t r;
	while (size) {
		r = write(1, buf + off, size);
		if (r < 0) {
			if (gotsigterm) return -1;
			if ((errno == EAGAIN) || (errno == EINTR)) continue;
			break;
		} else if (!r) {
			fprintf(stderr,
				"%swrite channel stuck\n", logprefix);
			exitstatus = EXIT_IOERR;
			return -1;
		}
		off += r;
		size -= r;
		rng_stats.bytes_sent += r;
	}
	if (size) {
		fprintf(stderr,
			"%serror writing to output: %s\n", logprefix,
			strerror(errno));
		exitstatus = EXIT_IOERR;
		return -1;
	}
	return 0;
}
#endif
static void init_rng_stats(void)
{
	memset(&rng_stats, 0, sizeof(rng_stats));
	//gettimeofday(&rng_stats.progstart, 0);
	set_stat_prefix(logprefix);
}
static void dump_rng_stats(void)
{
	int j;
	char buf[256];
	//struct timeval now;
	printf( "%s\n", dump_stat_counter(buf, sizeof(buf),
			"bits received from input",
			rng_stats.bytes_received * 8));
	//if (arguments->pipemode)
	//	fprintf(stderr, "%s\n", dump_stat_counter(buf, sizeof(buf),
	//		"bits sent to output",
	//		rng_stats.bytes_sent * 8));
	printf( "%s\n", dump_stat_counter(buf, sizeof(buf),
			"FIPS 140-2 successes",
			rng_stats.good_fips_blocks));
	printf( "%s\n", dump_stat_counter(buf, sizeof(buf),
			"FIPS 140-2 failures",
			rng_stats.bad_fips_blocks));
	for (j = 0; j < N_FIPS_TESTS; j++)
		printf( "%s\n", dump_stat_counter(buf, sizeof(buf),
					fips_test_names[j],
					rng_stats.fips_failures[j]));
	//printf( "%s\n", dump_stat_bw(buf, sizeof(buf),
	//		"input channel speed", "bits",
	//		&rng_stats.source_blockfill, FIPS_RNG_BUFFER_SIZE*8));
	//printf( "%s\n", dump_stat_bw(buf, sizeof(buf),
	//		"FIPS tests speed", "bits",
	//		&rng_stats.fips_blockfill, FIPS_RNG_BUFFER_SIZE*8));
	//if (arguments->pipemode)
	//	fprintf(stderr, "%s\n", dump_stat_bw(buf, sizeof(buf),
	//		"output channel speed", "bits",
	//		&rng_stats.sink_blockfill, FIPS_RNG_BUFFER_SIZE*8));
	//gettimeofday(&now, 0);
	//fprintf(stderr, "%sProgram run time: %llu microseconds\n",
	//	logprefix,
	//	(unsigned long long) elapsed_time(&rng_stats.progstart, &now));
}
/* Return 32 bits of bootstrap data */
static int discard_initial_data(void)
{
	unsigned char tempbuf[4];
	/* Do full startup discards when in pipe mode */
	//if (arguments->pipemode)
	//	if (xread(tempbuf, sizeof tempbuf)) exit(EXIT_FAIL);
	/* Bootstrap data for FIPS tests */
	xread(tempbuf, sizeof tempbuf);
	return tempbuf[0] | (tempbuf[1] << 8) |
		(tempbuf[2] << 16) | (tempbuf[3] << 24);
}
static void do_rng_fips_test_loop( void )
{
	int j, loop;
	int fips_result;
	//struct timeval start, stop, statdump, now;
	int statruns, runs;
	runs = statruns = 0;
	//gettimeofday(&statdump, 0);
	//while (!gotsigterm) {
	for (loop=0 ; loop<1000000 ; loop++) {
		test_min=0xfffff;
		test_max=0;
		//gettimeofday(&start, 0);
		if (xread(rng_buffer, sizeof(rng_buffer))) return;
	//	for (j=0 ; j<128 ; j++)
	//		printf("%d, ", rng_buffer[j]);
		//gettimeofday(&stop, 0);
		//update_usectimer_stat(&rng_stats.source_blockfill,
		//		&start, &stop);
		//gettimeofday(&start, 0);
		fips_result = fips_run_rng_test(&fipsctx, &rng_buffer);
		//gettimeofday (&stop, 0);
		//update_usectimer_stat(&rng_stats.fips_blockfill,
		//		&start, &stop);
		if (fips_result) {
			rng_stats.bad_fips_blocks++;
			for (j = 0; j < N_FIPS_TESTS; j++)
				if (fips_result & fips_test_mask[j])
					rng_stats.fips_failures[j]++;
		} else {
			rng_stats.good_fips_blocks++;
			//if (arguments->pipemode) {
			//	gettimeofday(&start, 0);
			//	if (xwrite(rng_buffer, sizeof(rng_buffer)))
			//		return;
			//	gettimeofday (&stop, 0);
			//	update_usectimer_stat(
			//		&rng_stats.sink_blockfill,
			//		&start, &stop);
			//}
		}
		if (arguments->blockcount &&
		    (++runs >= arguments->blockcount)) break;
		//gettimeofday(&now, 0);
		if ((arguments->blockstats &&
		     (++statruns >= arguments->blockstats)) ) {
			dump_rng_stats();
			//gettimeofday(&statdump, 0);
			statruns = 0;
		}
		//printf("\ntest_max=%x, test_min=%x", test_max, test_min);
	}
}



//int rngtest_main(void)
int Start_trng_test(int test)
{

    test_NUM=((test/100)%10);    //must [0-6]
    test_ro_sel=1+((test/10)%10);   // must [1-5]
    test_wire_sel=test%10;  //must [0,1]

	arguments->blockstats = 200;
	
	//argp_parse(&argp, argc, argv, 0, 0, arguments);
	//if (!arguments->pipemode)
	//	fprintf(stderr, "%s\n\n",
	//		argp_program_version);
	//init_sighandlers();
	/* Init data structures */
	init_rng_stats();
	//if (!arguments->pipemode)
	//	fprintf(stderr, "%sstarting FIPS tests...\n",
	//		logprefix);
	/* Bootstrap FIPS tests */
	fips_init(&fipsctx, discard_initial_data());
	do_rng_fips_test_loop();
	dump_rng_stats();
	//if ((exitstatus == EXIT_SUCCESS) &&
	//    (rng_stats.bad_fips_blocks || !rng_stats.good_fips_blocks)) {
	//	exitstatus = EXIT_FAIL;
	//}
	//exit(exitstatus);
	return 0;
}

#define TEST_READ_COUNT 12500

unsigned char rng_throubhput_buffer[TEST_READ_COUNT];
int Start_trng_throughput_test(void)
{

	//int test_read_count=1000000;
	//int temp;

    //test_NUM=((test/100)%10);    //must [0-6]
    //test_ro_sel=1+((test/10)%10);   // must [1-5]
    //test_wire_sel=test%10;  //must [0,1]

	printf("\nthe throughput read buffer size =%d byte\n", sizeof(rng_throubhput_buffer));
	for (test_NUM=0 ; test_NUM<7; test_NUM++) {
		for (test_ro_sel=1; test_ro_sel<6 ; test_ro_sel++) {
			for (test_wire_sel=0 ; test_wire_sel<2 ; test_wire_sel++) {
				printf("\ntest NUM=%d, ro_sel=%d, wire_sel=%d.\n", test_NUM, test_ro_sel, test_wire_sel);
				printf("s\n");
				xread(rng_throubhput_buffer, sizeof(rng_throubhput_buffer));
				printf("e\n");
			}
		}
	}
	
	return 0;
}


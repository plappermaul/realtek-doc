#include <cli/cli_util.h>
#include <cli/cli_parser.h>
//#include <string.h>

#ifndef SECTION_CLI_UTIL
    #define SECTION_CLI_UTIL
#endif

#define _disp_chr(c) ({const u8_t chr=c; (((chr<32)||(chr>=128))?'.':chr);})

SECTION_CLI_UTIL void
cli_memory_dump(const u8_t *buff, u32_t size) {
    u32_t i, p;
    u32_t bufadd=(u32_t)buff;
    if (size>4096) size=4096;
    u32_t bufend=bufadd+size;
    u32_t curr=bufadd&~0xf;

    puts("          00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F    0123456789ABCDEF\n");
    while (1) {
        printf("%08x  ", curr);
        for (i=0, p=curr; i<16;++i) {
            if ((p<bufadd)||(p>=bufend)) {
                puts("   ");
            } else {
                printf("%02x ", *(const u8_t *)p);
            }
            ++p;
        }
        puts("   ");
        for (i=0, p=curr; i<16;++i) {
            if ((p<bufadd)||(p>=bufend)) {
                puts(" ");
            } else {
                printf("%c", _disp_chr(*(const u8_t *)p));
            }
            ++p;
        }
        curr+=16;
        puts("\n");
        if (curr>=bufend) return;
    }
}

SECTION_CLI_UTIL char *
cli_string_lookup(const str2int_entry_t *list, u32_t v, char *buf) {
    if (list==VZERO) return VZERO;
    while (list->value_str!=VZERO) {
        if (list->value==v) {
            const char *p=list->value_str;
            do *(buf++)=*p;
            while(*(p++)!='\0');
            return buf;
        }
        ++list;
    }
    return VZERO;
}

SECTION_CLI_UTIL int
cli_int_lookup(const str2int_entry_t *list, const char *value_str, u32_t *value) {
    if (list==VZERO) return -1;
    while (list->value_str!=VZERO) {
        if (inline_strcmp(value_str, list->value_str)==0) {
            *value=list->value;
            return 0;
        }
        ++list;
    }
    return -1;
}

// ------------------------cli commands ------------------------------------------------
extern const unsigned char start_of_cli_help SECTION_CMD_HELP;
extern const unsigned char end_of_cli_help SECTION_CMD_HELP;
SECTION_CLI_UTIL cli_cmd_ret_t
cli_std_help(const void *user, u32_t argc, const char *argv[]) {
    const unsigned char *b=&start_of_cli_help;
    const unsigned char *e=&end_of_cli_help;
    while (b!=e) {
        if ((*b!='\0') && (*b<=127)) _putcn(*b);
        ++b;
    }

    return CCR_OK;
}

SECTION_CLI_UTIL cli_cmd_ret_t
cli_std_exit(const void *user, u32_t argc, const char *argv[]) {
    return CCR_EXIT;
}

SECTION_CLI_UTIL cli_cmd_ret_t
cli_std_load_word(const void *user, u32_t argc, const char *argv[]) {
    if (argc<2) return CCR_INCOMPLETE_CMD;
        
    if(argc==2) {
        u32_t addr=atoi(argv[1]);
        printf("$=0x%08x\n", REG32(addr));
    } else{
        const u8_t *addr=(const u8_t *)atoi(argv[1]);
        u32_t len=atoi(argv[2])*4;
        cli_memory_dump(addr, len);
    }
    return CCR_OK;
}
SECTION_CLI_UTIL cli_cmd_ret_t
cli_std_store_word(const void *user, u32_t argc, const char *argv[]) {
    if (argc<3) return CCR_INCOMPLETE_CMD;
    u32_t addr=atoi(argv[1]);
    u32_t arg=atoi(argv[2]);
    REG32(addr)=arg;
    return CCR_OK;
}



SECTION_CLI_UTIL static void
_cli_test_udelay(u32_t unit, u32_t min) {
    u32_t sec_loop=1000000/unit;
    u32_t outer_loop=min*60;
    u32_t i, j;
    puts("00:00");
    for (i=0; i<outer_loop; ++i) {
        for (j=0; j<sec_loop; ++j) {
            udelay(unit);
        }
        printf("\r%02d:%02d", i/60, i%60);
    }
    puts(": done\n");
}
SECTION_CLI_UTIL static void
_cli_test_get_timer(u32_t min) {
    u32_t sec_delay=min*60;
    u32_t last_sec=0, sec;
    puts("00:00");
    
    u32_t start_ms=get_timer(0);
    
    do {
        sec=(get_timer(start_ms))/1000;
        if (sec!=last_sec) {
            last_sec=sec;
            printf("\r%02d:%02d", last_sec/60, last_sec%60);
        }
    } while (sec<sec_delay);
    puts(": done\n");
}

SECTION_CLI_UTIL cli_cmd_ret_t
cli_std_timer(const void *user, u32_t argc, const char *argv[]) {
    u32_t min=1;
    if (argc>=3) min=atoi(argv[2]);

    if (user==VZERO) {
        _cli_test_get_timer(min);
    } else {
        u32_t unit = (unsigned)user;
        _cli_test_udelay(unit, min);
    }
    return CCR_OK;
}

// function types
typedef u32_t (func_void_t)(void);
typedef u32_t (func_int_t)(u32_t);
typedef u32_t (func_int_int_t)(u32_t, u32_t);
typedef u32_t (func_int_int_int_t)(u32_t, u32_t, u32_t);
typedef u32_t (func_int_int_int_int_t)(u32_t, u32_t, u32_t, u32_t);

SECTION_CLI_UTIL cli_cmd_ret_t
cli_std_call(const void *user, u32_t argc, const char *argv[]) {
    if (argc<2) return CCR_INCOMPLETE_CMD;
    u32_t faddr=0;
    
    if (user==VZERO) {
        faddr=atoi(argv[1]);
        if (faddr==0) {
            puts("zero address call, skip the call\n");
            return CCR_OK;
        }
    } else {
        faddr=(u32_t)user;
    }

    u32_t res=0;
    u32_t arg1=(argc>=3)?atoi(argv[2]):0;
    u32_t arg2=(argc>=4)?atoi(argv[3]):0;
    u32_t arg3=(argc>=5)?atoi(argv[4]):0;
    u32_t arg4=(argc>=6)?atoi(argv[5]):0;

    switch (argc) {
        case 2: res=((func_void_t*)faddr)(); break;
        case 3: res=((func_int_t*)faddr)(arg1); break;
        case 4: res=((func_int_int_t*)faddr)(arg1, arg2); break;
        case 5: res=((func_int_int_int_t*)faddr)(arg1, arg2, arg3); break;
        case 6: res=((func_int_int_int_int_t*)faddr)(arg1, arg2, arg3, arg4); break;
        default: return CCR_UNSUPPORTED_PARA;
    }
    printf("$=0x%08x\n", res);
    return CCR_OK;
}

cli_cmd_ret_t __attribute__ ((weak))
cli_ddr_setup(const void *user, u32_t argc, const char *argv[]) { return CCR_OK;}

cli_cmd_ret_t __attribute__ ((weak))
cli_pll_setup(const void *user, u32_t argc, const char *argv[]) { return CCR_OK;}

#ifndef DONT_REGISTER_CLI_UTIL_CMD

cli_top_node(help, cli_std_help);
    cli_add_help(help, "help");
cli_top_node(exit, cli_std_exit);
    cli_add_help(exit, "exit");
cli_top_node(dw, cli_std_load_word);
cli_top_node(md, cli_std_load_word);
    cli_add_help(load_word, "dw/md <addr> [length]");
cli_top_node(ew, cli_std_store_word);
cli_top_node(mw, cli_std_store_word);
    cli_add_help(store_word, "ew/mw <addr> <value>");
cli_top_node(timer, cli_std_timer);
    cli_add_tail(get_timer, timer, VZERO);
    cli_add_tail(1udelay, timer, (void*)1);
    cli_add_tail(10udelay, timer, (void*)10);
    cli_add_tail(100udelay, timer, (void*)100);
    cli_add_help(timer, "timer [1udelay|10udelay|100udelay|get_timer [min]]");
cli_top_node(call, cli_std_call);
    cli_add_help(call, "call <addr>");
    cli_add_node(ddr_setup, call, cli_ddr_setup);
    cli_add_help(ddr_setup, "call ddr_setup");
    cli_add_node(pll_setup, call, cli_pll_setup);
    cli_add_help(pll_setup, "call pll_setup");
#endif //DONT_REGISTER_CLI_UTIL_CMD


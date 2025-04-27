#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <soc.h>
#include <inline_util.h>

#ifndef SECTION_CMD_NODE
    #error "SECTION_CMD_NODE should be defined"
#endif
#ifndef SECTION_CMD_MP_NODE
    #error "SECTION_CMD_MP_NODE should be defined"
#endif
#ifndef SECTION_CMD_TAIL
    #error "SECTION_CMD_TAIL should be defined"
#endif

// command parsers
#define MAX_ARGV        12
#define CMD_INP_BUF_SZ  128
#define MAX_TOKEN_SZ    48
#define CHAR_ESC        27
#define CHAR_TAB        '\t'
#define CHAR_BACKSPACE  8
#define CHAR_DEL        0x7f
#define CHAR_ENTER      0x0d

#define ITEM_UNIT_LEN   16

typedef enum {
    CCR_OK=0,
    CCR_EXIT,
    CCR_REBOOT,
    CCR_UNKNOWN_CMD,
    CCR_UNSUPPORTED_PARA,
    CCR_INCOMPLETE_CMD,
    CCR_NO_CMD,
    CCR_FAIL=0xffffffff
} cli_cmd_ret_t;


typedef cli_cmd_ret_t (cli_cmd_func_t)(const void *user, u32_t argc, const char *argv[]);

struct cli_cmd_node_s;
typedef struct cli_cmd_node_s cli_cmd_node_t;

struct cli_cmd_node_s {
    const char *name;
    const cli_cmd_node_t *parent;
    cli_cmd_func_t *func;
};
typedef struct {
    const cli_cmd_node_t *me;
    const cli_cmd_node_t *oparent;
} cli_cmd_mp_node_t;
typedef struct {
    const char *name;
    const cli_cmd_node_t *parent;
    const void *user;
} cli_cmd_tail_t;

#ifndef SECTION_CMD_NODE
    #define SECTION_CMD_NODE    __attribute__ ((section (".cli_node")))
#endif
#ifndef SECTION_CMD_MP_NODE
    #define SECTION_CMD_MP_NODE __attribute__ ((section (".cli_mp_node")))
#endif
#ifndef SECTION_CMD_TAIL
    #define SECTION_CMD_TAIL    __attribute__ ((section (".cli_tail")))
#endif
#ifndef SECTION_CMD_HELP
    #define SECTION_CMD_HELP    __attribute__ ((section (".cli_help")))
#endif

#define CLI_NODE_NAME(__n__) _CLI_NODE_ ## __n__

#define cli_top_node(name, func) \
    cli_cmd_node_t CLI_NODE_NAME(name)  SECTION_CMD_NODE = { #name, VZERO, func}

#define cli_add_node(name, parent_name, func) \
    extern cli_cmd_node_t CLI_NODE_NAME(parent_name); \
    cli_cmd_node_t CLI_NODE_NAME(name)  SECTION_CMD_NODE = { #name, &CLI_NODE_NAME(parent_name), func}
    
#define cli_add_parent(name, parent_name) \
    extern cli_cmd_node_t CLI_NODE_NAME(parent_name); \
    cli_cmd_mp_node_t CL(_CLI_MP_NODE_ ## name ## _ ) SECTION_CMD_MP_NODE = \
    {&CLI_NODE_NAME(name), &CLI_NODE_NAME(parent_name)}
    
#define cli_add_tail(name, parent_name, user) \
    extern cli_cmd_node_t CLI_NODE_NAME(parent_name); \
    cli_cmd_tail_t CL(_CLI_TAIL_ ## name ## _ ) SECTION_CMD_TAIL = \
    { #name, &CLI_NODE_NAME(parent_name), user}

#define cli_add_help(name, text) \
    const char CL(_CLI_HELP_CONTENT_ ## name ## _ )[] SECTION_CMD_HELP = text "\n"

extern int cli_strcmp(const char *a, const char *b, char end_chr);
extern cli_cmd_ret_t cli_main_loop(void);
extern const cli_cmd_node_t *cli_match_node_pure_name(const char *name);
extern const cli_cmd_node_t *cli_match_node_name(const cli_cmd_node_t *parent, const char *name);
extern const cli_cmd_tail_t *cli_match_tail_name(const cli_cmd_node_t *parent, const char *name);

#endif //CLI_PARSER_H




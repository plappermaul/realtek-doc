//#include <string.h>
#include <cli/cli_access.h>

#ifndef SECTION_CLI_ACCESS
    #define SECTION_CLI_ACCESS
#endif

extern const cli_cmd_tail_t start_of_cli_tail SECTION_CMD_TAIL;
extern const cli_cmd_tail_t end_of_cli_tail SECTION_CMD_TAIL;

SECTION_CLI_ACCESS int
cli_show_var(const cli_access_var_t *var) {
    u32_t int_value;
    char str_value[MAX_TOKEN_SZ+4];

    if ((var!=VZERO) && (var->get.get!=VZERO)) {
        if (var->is_int) {
            cli_access_get_int_t *get_int=var->get.get_int;
            (*get_int)(&int_value);
            printf(var->is_decimal_showed?"    %s: %d\n":"    %s: 0x%08x\n",
                var->name, int_value);
        } else {
            cli_access_get_str_t *get_str=var->get.get_str;
            (*get_str)(str_value);
            printf("    %s: %s\n", var->name, str_value);
        }
        return 1;
    } 
    return 0;
}

SECTION_CLI_ACCESS int
cli_show_family(const cli_cmd_node_t *family_node) {
    if (family_node==VZERO) return 0;
    
    const cli_cmd_tail_t *b=&start_of_cli_tail, *e=&end_of_cli_tail;
    while (b!=e) {
        if (family_node==b->parent) {
            cli_show_var((const cli_access_var_t*)b->user);
        }
        ++b;
    }
    
    return 1;
}

extern void dram_dump_info(void);

cli_top_node(show, VZERO);
    cli_add_help(show, "show <family-name> [variable-name] ");
    cli_add_node(ddr_info, show, (cli_cmd_func_t *)dram_dump_info);

SECTION_CLI_ACCESS static const cli_cmd_node_t *
_cli_check_family_name(const cli_cmd_node_t *cmd, const char *family_name) {
    if (family_name==VZERO) {
        puts("missing family name\n");
        return VZERO;
    }

    const cli_cmd_node_t *family_node=cli_match_node_name(cmd, family_name);
    if (family_node==VZERO) {
        printf("unknown family name: %s\n", family_name);
    }
    return family_node;
}

SECTION_CLI_ACCESS cli_cmd_ret_t cli_access_get(const void *user, u32_t argc, const char *argv[]);
SECTION_CLI_ACCESS cli_cmd_ret_t cli_access_set(const void *user, u32_t argc, const char *argv[]);

#ifndef DONT_REGISTER_CLI_ACCESS_CMD
cli_top_node(get, cli_access_get);
    cli_add_help(get, "get <family-name> [variable-name] ");

cli_top_node(set, cli_access_set);
    cli_add_help(set, "set <family-name> <variable-name> <value>");

#endif

SECTION_CLI_ACCESS cli_cmd_ret_t
cli_access_get(const void *user, u32_t argc, const char *argv[]) {
    const char *family_name=argv[1];
    const cli_cmd_node_t *family_node=_cli_check_family_name(&CLI_NODE_NAME(get), family_name);
    if (family_node==VZERO) return CCR_OK;

    if (argc>=3) {
        // show individual variable(s)
        const char **p=&(argv[2]);
        while (*p!=VZERO) {
            const cli_cmd_tail_t *tail_node=cli_match_tail_name(family_node, *p);
            if (tail_node==VZERO) {
                printf("unknown variable: %s in %s\n",*p, family_name);
            } else {
                cli_show_var((const cli_access_var_t*)tail_node->user);
            }
            ++p;
        }
    } else if (argc==2) {
        // show whole family
        cli_show_family(family_node);
    }
    return CCR_OK;
}
SECTION_CLI_ACCESS cli_cmd_ret_t 
cli_access_set(const void *user, u32_t argc, const char *argv[]) {
    const char *family_name=argv[1];
    const cli_cmd_node_t *family_node=_cli_check_family_name(&CLI_NODE_NAME(set), family_name);
    if (family_node==VZERO) return CCR_OK;
    if (argc<4) return CCR_INCOMPLETE_CMD;
    if (argc>4) return CCR_UNSUPPORTED_PARA;

    const cli_cmd_tail_t *tail_node=cli_match_tail_name(family_node, argv[2]);
    if (tail_node==VZERO) return CCR_UNSUPPORTED_PARA;
    const cli_access_var_t *var=(const cli_access_var_t*)tail_node->user;
    int set_res;
    if (var->is_int) {
        u32_t v=atoi(argv[3]);
        set_res=var->set.set_int(v);
    } else {
        set_res=var->set.set_str(argv[3]);
    }
    if (set_res) printf("unable to set by value '%s'\n", argv[3]);
    
    cli_show_var(var);

    return CCR_OK;
}

cli_add_node(cli, get, VZERO);
cli_add_parent(cli, set);

#define DEFINE_CLI_INT_VAR(name, is_dec, get_func_body, set_func_body) \
    SECTION_CLI_VAR int _CLI_VAR_CLI_ ## name ## _get_int_(u32_t *result) {get_func_body; return 0;} \
    SECTION_CLI_VAR int _CLI_VAR_CLI_ ## name ## _set_int_(u32_t value) {set_func_body; return 0;} \
    CLI_DEFINE_VAR(name, cli, 1, 0, is_dec, \
        _CLI_VAR_CLI_ ## name ## _get_int_, \
        _CLI_VAR_CLI_ ## name ## _set_int_)

extern u32_t SECTION_PARAMETERS tty_width;
DEFINE_CLI_INT_VAR(tty_width, 1, {*result=tty_width;}, {tty_width=value;});

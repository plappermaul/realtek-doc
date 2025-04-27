#ifndef CLI_ACCESS_H
#define CLI_ACCESS_H
#include <soc.h>
#include <cli/cli_parser.h>

#ifndef SECTION_CLI_VAR
    #define SECTION_CLI_VAR
#endif


typedef int (cli_access_get_int_t)(u32_t *r);
typedef int (cli_access_get_str_t)(char *r);
typedef int (cli_access_set_int_t)(u32_t r);
typedef int (cli_access_set_str_t)(const char *r);

typedef struct {
    const char                  *name;
    u32_t                       is_int:1;
    u32_t                       is_virtual:1;
    u32_t                       is_decimal_showed:1;

    union {
        cli_access_get_int_t    *get_int;
        cli_access_get_str_t    *get_str;
        void                    *get;
    } get;
    union {
        cli_access_set_int_t    *set_int;
        cli_access_set_str_t    *set_str;
        void                    *set;
    } set;
    
} cli_access_var_t;

#define CLI_DEFINE_VAR(_name, _family, _is_int, _is_vir, _is_dec, _get, _set) \
    cli_access_var_t _CLI_VAR_ ## _family ## _ ## _name ## _ SECTION_CLI_VAR = {\
        .name = #_name,\
        .is_int = _is_int,\
        .is_virtual = _is_vir,\
        .is_decimal_showed = _is_dec,\
        .get.get=_get,\
        .set.set=_set };\
    cli_add_tail(_name, _family, (void*)& _CLI_VAR_ ## _family ## _ ## _name ## _ )

#endif



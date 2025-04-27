#ifndef CLI_UTIL_H
#define CLI_UTIL_H
#include <soc.h>

extern void cli_memory_dump(const u8_t *buff, u32_t size);

typedef struct {
    const char *value_str;
    u32_t value;
} str2int_entry_t;
extern char *cli_string_lookup(const str2int_entry_t *list, u32_t v, char *buf);
extern int cli_int_lookup(const str2int_entry_t *list, const char *value_str, u32_t *value);

#endif //CLI_UTIL_H



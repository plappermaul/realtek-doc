#ifndef _SW_DEF_H_
#define _SW_DEF_H_

#include <common/type.h>

typedef struct hal_register_s
{
    uint32 reg_addr;
    uint32 write_bit;
    uint32 reset_val;
}hal_register_t;

#if defined(CONFIG_SDK_APOLLO)
extern hal_register_t reg_map_def[];
#define REG_MAP_ENTRY_NUM  4965
#endif

#if defined(CONFIG_SDK_APOLLOMP)
extern hal_register_t apollomp_reg_map_def[];
#define APOLLOMP_REG_MAP_ENTRY_NUM  4593
#endif

#if defined(CONFIG_SDK_RTL9601B)
extern hal_register_t rtl9601b_reg_map_def[];
#define RTL9601B_REG_MAP_ENTRY_NUM  2386
#endif

#if defined(CONFIG_SDK_RTL9602C)
extern hal_register_t rtl9602c_reg_map_def[];
#define RTL9602C_REG_MAP_ENTRY_NUM  4311
#endif


#endif /*#ifndef _SW_DEF_H_*/

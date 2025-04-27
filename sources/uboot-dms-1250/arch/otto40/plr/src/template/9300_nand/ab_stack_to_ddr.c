#include <init_define.h>
#include <dram/memcntlr.h>

void change_sp_to_dram(void)
{
    if(ISTAT_GET(cal) == MEM_CAL_OK){
        u32_t old_sp;
        asm volatile ("move %0, $29": "=r"(old_sp));
        inline_memcpy(NEW_STACK_AT_DRAM, old_sp, (OTTO_PLR_STACK_DEF-old_sp));
        asm volatile ("move $29, %0": : "r"(NEW_STACK_AT_DRAM));
        printf("II: Change Stack from 0x%x to 0x%x\n",OTTO_PLR_STACK_DEF, NEW_STACK_AT_DRAM);
    }else{
        puts("EE: DRAM Init fail, Stack setting deos not be changed\n");
    }
}

REG_INIT_FUNC(change_sp_to_dram, 36);

#include <soc.h>
#include <spi_nand/spi_nand_symb_func.h>

typedef struct slowdown_model_s{
    u16_t mid;
    u16_t did;
    u16_t max_sclk;    
}slowdown_model_t;

#define MID_HEYANGTEK (0xC9)

slowdown_model_t slow_model[] = {
    {.mid = MID_HEYANGTEK, .max_sclk = 80},
};

u32_t nsu_sclk_limit(u32_t cur_sclk)
{
    u32_t i;   
    for(i=0 ; i<sizeof(slow_model)/sizeof(slowdown_model_t) ; i++){
        if(slow_model[i].mid == plr_spi_nand_flash_info.man_id){
            if(cur_sclk > slow_model[i].max_sclk) return slow_model[i].max_sclk; 
            break;
        }
    }
    return cur_sclk;
}


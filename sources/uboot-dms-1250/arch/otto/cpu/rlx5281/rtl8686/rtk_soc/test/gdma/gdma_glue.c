#include "gdma_glue.h"
#include "asicregs.h"

//#if defined(CONFIG_RTL8196) || defined(CONFIG_RTL8652) || defined(CONFIG_RTL8672) || defined(CONFIG_RTL8316S) || defined(CONFIG_RTL8198)
#if 1
uint32 rtlglue_getmstime( uint32* pTime )
{
        *pTime = (uint32)((REG32(TC0CNT) >> 8)/1000);
        return 0;
}

#else
int timer_init(void)
{
        REG32(TCCNR) = 0; /*Reset timer or counter control register.*/
        //REG32(CDBR) = (CFG_HZ/1000000) << DIVF_OFFSET;
        REG32(CDBR) = (CFG_LXBUS_MHZ/1000000) << DIVF_OFFSET;
        REG32(TC0DATA) = 0xFFFFFF << 8;
        REG32(TCCNR) = TC0EN | TC0MODE_TIMER; /*Enable timer 0 or counter 0 control register.*/

        /* Enable timer interrupt */
        REG32(TCIR) = TC0IE;
        return 0;
}
#endif


static uint32 rtl_seed = 0xDeadC0de;

      /* 1: Simulate psuedo random data of kernel mode.
       * 0: use libc random for user-space program. */
/*
 * @func void      | rtlglue_srandom       | The OS independent seed function for random.
 * @parm uint32 | seed                     | seed
 * @comm
 *  */
void rtlglue_srandom( uint32 seed )
{
        rtl_seed = seed;
}

/*
 * @func uint32    | rtlglue_random        | The OS independent random function.
 * @parm void |                    |
 * @comm
 *  */
uint32 rtlglue_random( void )
{
        uint32 hi32, lo32;

        hi32 = (rtl_seed>>16)*19;
        lo32 = (rtl_seed&0xffff)*19+37;
        hi32 = hi32^(hi32<<16);
        return ( rtl_seed = (hi32^lo32) );
}


void rtlglue_free(void *APTR){free(APTR);}

void *rtlglue_malloc(uint32 NBYTES){return malloc(NBYTES);}


#include "crypto.h"

uint32 rtlglue_getmstime( uint32* pTime )
{

	*pTime = 0;
	return 0;
}


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


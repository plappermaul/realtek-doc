

#ifndef RTL865X_DEBUG
#define assert(expr) do {} while (0)
#else
#define assert(expr) \
        if(!(expr)) {					\
        rtlglue_printf( "\033[33;41m%s:%d: assert(%s)\033[m\n",	\
        __FILE__,__LINE__,#expr);		\
        }
#endif



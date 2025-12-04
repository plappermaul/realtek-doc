#ifndef PONTOD_FREQ_MAIN_H
#define PONTOD_FREQ_MAIN_H

#define DBG_PRINT(lvl,fmt,arg...)\
    do {\
        {\
          if(debug>=lvl) \
          printf(fmt,##arg);\
        }\
    } while (0)

#endif

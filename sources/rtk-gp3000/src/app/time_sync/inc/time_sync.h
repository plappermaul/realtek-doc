#ifndef TIME_SYNC_H
#define TIME_SYNC_H

#ifdef CONFIG_RTK_PTP_MASTER
void init1588MasterProcess(void);
void exit1588MasterProcess(void);
void enable1588MasterProcess(void);
void disable1588MasterProcess(void);
#endif

#ifdef CONFIG_RTK_PPSTOD
void init1PPSToDProcess(void);
void exit1PPSToDProcess(void);
void enable1PPSToDProcess(void);
void disable1PPSToDProcess(void);
#endif

#define DBG_PRINT(lvl,fmt,arg...)\
    do {\
        {\
          if(debug>=lvl) \
          printf(fmt,##arg);\
        }\
    } while (0)

#endif

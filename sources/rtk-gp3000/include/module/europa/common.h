#ifndef __EUROPA_COMMON_H__
#define __EUROPA_COMMON_H__

//typedef unsigned long uint32;
//typedef unsigned int uint16;
//typedef unsigned char uint8;
#include <common/type.h>

#ifndef NULL
#define NULL  ((void *) 0)
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef SUCCESS
#define SUCCESS 	0
#endif
#ifndef FAILED
#define FAILED -1
#endif

#ifndef ENABLE
#define ENABLE 1
#endif

#ifndef DISABLE
#define DISABLE 0
#endif



//#define TIMER_SEC_EVENT  (1<<0)



/* extern functions */

//extern void   timer_init(void);
//extern void   uart_init(void);
//extern void   setReg(uint16 address, uint16 value);
//extern uint16 getReg(uint16 address);


  /* For timer */
//extern volatile uint32 data current_time;
//extern          uint32 data system_clock;
//extern volatile uint8  idata _th0;
//extern volatile uint8  idata _tl0;
//extern          uint8  data timeCount;


  /* for Event Flags */
//extern volatile uint8 idata event_list;

    /* for firmware upgrade buffer */
//extern uint8    stack_buffer[512];

#endif /* End of __COMMON_H__ */


#ifndef _OSAL_TIMER_H_
#define _OSAL_TIMER_H_
#include "ca_types.h"
typedef enum
{
    CA_ONCE_TIMER   = 0,
    CA_CIRCLE_TIMER = 1,
} TIMER_TYPE;

#define CA_INVALID_TIMER    0

typedef enum
{
    CA_TIMER_OK      = 0,
    CA_TIMER_IN_PROC = 1,
    CA_TIMER_ERROR   = -1
} TIMER_RETURN_CODE;


ca_uint32  ca_timer_add(
                    ca_int32 interval,
                    void (*callback)(void *data),
                    void *data);
ca_uint32  ca_circle_timer_add(
                    ca_int32 interval,
                    void (*callback)(void *data),
                    void *data);
ca_int32    ca_timer_del(ca_uint32 timer_id);
ca_int32    ca_timer_stop(ca_uint32 timer_id);
void        ca_timer_start(ca_uint32 timer_id);
void        ca_timer_restart(ca_uint32 timer_id);
void        ca_timer_show(void);
ca_int32    ca_timer_count(void);
#endif

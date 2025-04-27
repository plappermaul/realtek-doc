


#ifndef __DRV_INTR_MAPPER_H__
#define __DRV_INTR_MAPPER_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/error.h>

/*
 * Symbol Definition
 */
typedef enum intr_controller_e
{
#if defined(CONFIG_SDK_RTL9310)
    INTR_R9310,
#endif
#if defined(CONFIG_SDK_RTL9300)
    INTR_R9300,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INTR_R8390,
#endif
#if defined(CONFIG_SDK_RTL8380)
    INTR_R8380,
#endif
    INTR_CTRL_END
} intr_controller_t;


typedef struct drv_intr_mapper_operation_s
{
    int32   (*swcoreIsrSts_get)(uint32, uint32 *);
    int32   (*swcoreImrEnable_set)(uint32, drv_intr_isr_t isrId, rtk_enable_t);
    int32   (*swcoreSts_get)(uint32, drv_intr_isr_t isrId, drv_intr_data_t *);
} drv_intr_mapper_operation_t;


#define INTR_CTRL(unit)         intr_ops[intr_if[unit]]
#define INTR_CHK(unit)          (intr_if[unit] != CID_GROUP_NONE)

/*
 * Data Declaration
 */
extern drv_intr_mapper_operation_t intr_ops[];
extern uint32 intr_if[];




/*
 * Macro Definition
 */

/*
 * Function Declaration
 */




#endif /* end __DRV_INTR_MAPPER_H__ */


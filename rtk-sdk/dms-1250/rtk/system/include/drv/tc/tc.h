/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of CPU Timer/Counter API.
 *
 * Feature : Init, Set and Get Timer/Counter related registers
 *
 */

#ifndef __DRV_TC_H__
#define __DRV_TC_H__

/*
 * Include Files
 */


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

typedef enum drv_tc_id_e
{
    TC_ID0 = 0,
    TC_ID1,
    TC_ID2,
    TC_ID3,
    TC_ID4,
    TC_ID5,
    TC_ID6,
    TC_ID_MAX,
} drv_tc_id_t;

typedef enum drv_tc_mode_e
{
    TC_MODE_COUNTER = 0,    /* this is defined according to register book */
    TC_MODE_TIMER = 1,      /* this is defined according to register book */
} drv_tc_mode_t;

typedef enum drv_tc_intState_e
{
    TC_INTERRUPT_PENDING = 0,
    TC_INTERRUPT_PENDING_NO,
} drv_tc_intState_t;



typedef struct tc_dataReg_s
{
    union {
        uint32              rawData;
        struct
        {
            uint32          notUsed1:4;
            uint32          tc_data:28;
        } field;
    } u;
} tc_dataReg_t;

typedef struct tc_cntReg_s
{
    union {
        uint32              rawData;
        struct
        {
            uint32          notUsed1:4;
            uint32          tc_data:28;
        } field;
    } u;
} tc_cntReg_t;

typedef struct tc_ctlReg_s
{
    union {
        uint32              rawData;
        struct
        {
            uint32          notUsed1:3;
            uint32          tcEnable:1;
            uint32          notUsed2:3;
            uint32          tcMode:1;
            uint32          notUsed3:8;
            uint32          tcDivFactor:16;
        } field;
    } u;
} tc_ctlReg_t;

typedef struct tc_intReg_s
{
    union {
        uint32              rawData;
        struct
        {
            uint32          notUsed1:11;
            uint32          intEnable:1;
            uint32          notUsed2:3;
            uint32          intPending:1;
            uint32          notUsed3:16;
        } field;
    } u;
} tc_intReg_t;


/*
 * Function Declaration
 */


/* Function Name:
 *      drv_tc_enable_set
 * Description:
 *      Enable/disale Timer/Counter of a chip
 * Input:
 *      unit            - unit id
 *      id              - the timer/counter ID
 *      enable          - enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize tc module before calling any tc APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_tc_enable_set(uint32 unit,drv_tc_id_t id, rtk_enable_t enable);

/* Function Name:
 *      drv_tc_mode_set
 * Description:
 *      Set timer mode or counter mode
 * Input:
 *      unit                - unit id
 *      id                   - the timer/counter ID
 *      mode              - timer mode or counter mode
 *      init_value         - the timer/counter ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize tc module before calling any tc APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_tc_mode_set(uint32 unit, drv_tc_id_t id, drv_tc_mode_t mode);

/* Function Name:
 *      drv_tc_divFactor_set
 * Description:
 *      Set divide factor
 * Input:
 *      unit            - unit id
 *      id              - the timer/counter ID
 *      divFactor       - the divide factor, Base clock = System_clock / divide factor, Both values 0x0000 and 0x0001 disable the clock.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize tc module before calling any tc APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_tc_divFactor_set(uint32 unit, drv_tc_id_t id, uint32 divFactor);

/* Function Name:
 *      drv_tc_dataInitValue_set
 * Description:
 *      Set the timer or counter initial value
 * Input:
 *      unit                - unit id
 *      id                  - the timer/counter ID
 *      init_value          - the timer or counter initial value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize tc module before calling any tc APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_tc_dataInitValue_set(uint32 unit, drv_tc_id_t id, uint32 init_value);

/* Function Name:
 *      drv_tc_intEnable_set
 * Description:
 *      Enable/Disable interrupt of this TC
 * Input:
 *      unit                - unit id
 *      id                  - the timer/counter ID
 *      value               - the read value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize tc module before calling any tc APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_tc_intEnable_set(uint32 unit, drv_tc_id_t id, rtk_enable_t enable);

/* Function Name:
 *      drv_tc_intState_get
 * Description:
 *      Get the interrupt state, (got interrupt or not)
 * Input:
 *      unit                - unit id
 *      id                  - the timer/counter ID
 *      state               - interrupt got or not
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize tc module before calling any tc APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_tc_intState_get(uint32 unit, drv_tc_id_t id, drv_tc_intState_t *state);

/* Function Name:
 *      drv_tc_intState_clear
 * Description:
 *      Clear the interrupt state
 * Input:
 *      unit                - unit id
 *      id                  - the timer/counter ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize tc module before calling any tc APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_tc_intState_clear(uint32 unit, drv_tc_id_t id);

/* Function Name:
 *      drv_tc_counterValue_get
 * Description:
 *      Get the current tick value of the timer/counter
 * Input:
 *      unit            - unit id
 *      id              - the timer/counter ID
 *      value           - the got value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize tc module before calling any tc APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_tc_counterValue_get(uint32 unit, drv_tc_id_t id,uint32 *value);


/* Function Name:
 *      tc_probe
 * Description:
 *      Probe tc module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
tc_probe(uint32 unit);


/* Function Name:
 *      drv_tc_init
 * Description:
 *      Initialize TC module.
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize tc module before calling any tc APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_tc_init(uint32 unit);


#endif /* __DRV_TC_H__ */


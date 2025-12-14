/*
 * Copyright (C) 2011 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * Purpose : Use to Management each device
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Initialize system
 *           2) Initialize device
 *           3) Mangement Devices
 *
 */
#ifndef __DAL_MGMT_H__ 
#define __DAL_MGMT_H__

/*  
 * Include Files 
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <dal/dal_mapper.h>

/* 
 * Symbol Definition 
 */
typedef struct dal_mgmt_info_s
{
    uint32          init;
    dal_mapper_t    *pMapper;
    uint32          device_id;
    uint32          revision;
} dal_mgmt_info_t;

typedef struct dal_mapper_info_s
{
    uint32          chip_id;
    dal_mapper_t    *pMapper;
} dal_mapper_info_t;

/* 
 * Data Declaration 
 */
extern dal_mgmt_info_t      *pMgmt_node;

/*
 * Macro Definition
 */
#define RT_MGMT       pMgmt_node
#define RT_MAPPER     RT_MGMT->pMapper

/* 
 * Function Declaration 
 */

/* Module Name : */

/* Function Name: 
 *      dal_mgmt_init
 * Description: 
 *      Initilize DAL(semaphore, database clear)
 * Input:  
 *      None
 * Output: 
 *      None 
 * Return: 
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 * Note: 
 *      RTK must call this function before do other kind of action.
 */ 
extern int32
dal_mgmt_init(void);


/* Function Name: 
 *      dal_mgmt_device_init
 * Description: 
 *      Initilize specified device(hook related driver, initialize database of MGMT, 
 *      execute initialized function of each component
 * Input:  
 *      None 
 * Output: 
 *      None 
 * Return: 
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 * Note: 
 *      RTK must call this function before do other kind of action.
 */ 
extern int32
dal_mgmt_initDevice(void);


#endif /* __DAL_MGMT_H__ */

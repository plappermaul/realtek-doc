/*
 * Copyright (C) 2010 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 13929 $
 * $Date: 2010-11-07 14:35:42 +0800 (Sun, 07 Nov 2010) $
 *
 * Purpose :
 *
 * Feature :
 *
 */

#ifndef __RSD_IPMCAST_H__
#define __RSD_IPMCAST_H__

/*
 * Include Files
 */
#include <sys_def.h>

/*
 * Symbol Definition
 */

/*
 * Data Type Declaration
 */
typedef struct sys_ipmcast_entry_s
{
    int32   indx;
    uint32  dip;
    uint32  sip;
    uint16  vid;
    sys_logic_portmask_t portmask;
	sys_logic_portmask_t fmode;
	int32 isGrpRefB;
    sys_enable_t care_vid;
} sys_ipMcastEntry_t;

typedef struct sys_l2mcast_entry_s
{
    uint8                   mac[ETHER_ADDR_LEN];
    int32                   indx;
    uint16                  vid;
    sys_logic_portmask_t    portmask;
    sys_enable_t            crossVlan;
    sys_enable_t          care_vid;
} sys_l2McastEntry_t;

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      rsd_mcast_lookup_mode_set
 * Description:
 *      Enable IGMP module globally
 * Input:
 *      mode - l2 entry lookup mode
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 *      SYS_ERR_FAILED
 *      SYS_ERR_INPUT
 * Note:
 *      None
 */
extern int32 rsd_mcast_lookup_mode_set(igmp_lookup_mode_t mode);

/* Function Name:
 *      rsd_mcast_ctrl_and_data_to_cpu_enable_set
 * Description:
 *      Enable ctrl and mcast data trap to CPU
 * Input:
 *      enable - ENABLE/DISABLE IGMP
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 *      SYS_ERR_FAILED
 *      SYS_ERR_INPUT
 * Note:
 *      None
 */
extern int32 rsd_mcast_ctrl_and_data_to_cpu_enable_set(sys_enable_t enable);

/* Function Name:
 *      rsd_igmp_ipMcastPkt2CpuEnablePerPort_set
 * Description:
 *      Enable IGMP data packet trap to CPU
 * Input:
 *      enable - ENABLE/DISABLE IGMP
 *      port   - Ingress port
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 *      SYS_ERR_FAILED
 *      SYS_ERR_INPUT
 * Note:
 *      None
 */
extern int32 rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(sys_enable_t enable, uint32 port);

/* Function Name:
 *      rsd_l2McastEntry_add
 * Description:
 *      Adds a new L2 Multicast entry to ASIC
 * Input:
 *      pEntry - L2 Multicast info
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 *      SYS_ERR_FAILED
 *      SYS_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 rsd_l2McastEntry_add(sys_l2McastEntry_t *pEntry);

/* Function Name:
 *      rsd_l2McastEntry_set
 * Description:
 *      Sets/updates a specific L2 Multicast entry to ASIC
 * Input:
 *      pEntry - L2 Multicast info
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 *      SYS_ERR_FAILED
 *      SYS_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 rsd_l2McastEntry_set(sys_l2McastEntry_t *pEntry);

/* Function Name:
 *      rsd_l2McastEntryIncCPU_set
 * Description:
 *      Sets/updates a specific L2 Multicast entry to ASIC
 * Input:
 *      pEntry - L2 Multicast info
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 *      SYS_ERR_FAILED
 *      SYS_ERR_NULL_POINTER
 * Note:
 *      This function's portmask includes CPU port
 */
extern int32 rsd_l2McastEntryIncCPU_set(sys_l2McastEntry_t *pEntry);

/* Function Name:
 *      rsd_l2McastEntry_del
 * Description:
 *      Deletes a specific L2 Multicast entry from the ASIC
 * Input:
 *      pEntry - L2 Multicast info
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 *      SYS_ERR_FAILED
 *      SYS_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 rsd_l2McastEntry_del(sys_l2McastEntry_t *pEntry);

/* Function Name:
 *      rsd_l2McastEntry_get
 * Description:
 *      Retrieves a specific L2 Multicast entry from the ASIC
 * Input:
 *      pEntry - L2 Multicast info
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 *      SYS_ERR_FAILED
 *      SYS_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 rsd_l2McastEntry_get(sys_l2McastEntry_t *pEntry);

/* Function Name:
 *      rsd_ipMcastEntry_add
 * Description:
 *      Adds a new IP Multicast entry to ASIC
 * Input:
 *      pEntry - IP Multicast info
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 *      SYS_ERR_FAILED
 *      SYS_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 rsd_ipMcastEntry_add(sys_ipMcastEntry_t *pEntry);

/* Function Name:
 *      rsd_ipMcastEntry_set
 * Description:
 *      Sets/updates a specific IP Multicast entry to ASIC
 * Input:
 *      pEntry - IP Multicast info
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 *      SYS_ERR_FAILED
 *      SYS_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 rsd_ipMcastEntry_set(sys_ipMcastEntry_t *pEntry);


/* Function Name:
 *      rsd_ipMcastEntry_del
 * Description:
 *      Deletes a specific IP Multicast entry from the ASIC
 * Input:
 *      pEntry - IP Multicast info
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 *      SYS_ERR_FAILED
 *      SYS_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 rsd_ipMcastEntry_del(sys_ipMcastEntry_t *pEntry);

/* Function Name:
 *      rsd_ipMcastEntry_get
 * Description:
 *      Retrieves a specific IP Multicast entry from the ASIC
 * Input:
 *      pEntry - IP Multicast info
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 *      SYS_ERR_FAILED
 *      SYS_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 rsd_ipMcastEntry_get(sys_ipMcastEntry_t *pEntry);

/* Function Name:
 *      rsd_ipMcGroup_get
 * Description:
 *      Make sure support ip mc table or not
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 *      SYS_ERR_FAILED
 * Note:
 *      None
 */
extern int32 rsd_ipMcGroup_get(void);

#endif /* __RSD_IPMCAST_H__ */


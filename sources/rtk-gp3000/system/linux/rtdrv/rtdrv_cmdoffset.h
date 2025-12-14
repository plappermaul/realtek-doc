
#ifndef __RTDRV_SYMOFFSET_H__
#define __RTDRV_SYMOFFSET_H__
/*
 * Symbol Definition
 */
#define RTDRV_BASE_CTL                          (64+1024+64+64+9000)

enum rtdrv_offset_num_e
{
    RTDRV_ACL_NUM=0,
    RTDRV_CLASSIFY_NUM,
    RTDRV_CPU_NUM,
    RTDRV_DEBUG_NUM,
    RTDRV_DOT1X_NUM,
    RTDRV_INTR_NUM,
    RTDRV_L2_NUM,
    RTDRV_L34_NUM,
    RTDRV_LED_NUM,
    RTDRV_MIRROR_NUM,
    RTDRV_OAM_NUM,
    RTDRV_PONMAC_NUM,
    RTDRV_PORT_NUM,
    RTDRV_QOS_NUM,
    RTDRV_RLDP_NUM,
    RTDRV_SEC_NUM,
    RTDRV_STAT_NUM,
    RTDRV_STP_NUM,
    RTDRV_SVLAN_NUM,
    RTDRV_SWITCH_NUM,
    RTDRV_TRAP_NUM,
    RTDRV_TRUNK_NUM,
    RTDRV_VLAN_NUM,
    RTDRV_GPON_NUM,
    RTDRV_PKT_NUM,
    RTDRV_RATE_NUM,
    RTDRV_REG_NUM,
    RTDRV_GPIO_NUM,
    RTDRV_I2C_NUM,
    RTDRV_L34_LITE_NUM,
    RTDRV_EPON_NUM,
    RTDRV_TIME_NUM,
    RTDRV_PPSTOD_NUM,
    RTDRV_PONLED_NUM,
    RTDRV_PBO_NUM,
    RTDRV_MDIO_NUM,
    RTDRV_LDD_NUM,
#if defined(CONFIG_COMMON_RT_API)
    RTDRV_RT_SWITCH_NUM,
    RTDRV_RT_GPON_NUM,
    RTDRV_RT_L2_NUM,
    RTDRV_RT_VLAN_NUM,
    RTDRV_RT_STAT_NUM,
    RTDRV_RT_RATE_NUM,
    RTDRV_RT_QOS_NUM,
    RTDRV_RT_INTR_NUM,
    RTDRV_RT_SEC_NUM,
    RTDRV_RT_TRAP_NUM,
    RTDRV_RT_I2C_NUM,
    RTDRV_RT_LED_NUM,
    RTDRV_RT_PORT_NUM,
    RTDRV_RT_EPON_NUM,
    RTDRV_RT_CLS_NUM,
    RTDRV_RT_PONMISC_NUM,
    RTDRV_RT_MIRROR_NUM,
    RTDRV_RT_TIME_NUM,
#endif
    RTDRV_END_NUM,
};

#define CMD_SPACE 100

#define RTDRV_ACL_OFFSET                        RTDRV_ACL_NUM * CMD_SPACE + 1 /* 0 will be used by RTDRV_INIT_RTKAPI */
#define RTDRV_CLASSIFY_OFFSET                   RTDRV_CLASSIFY_NUM * CMD_SPACE
#define RTDRV_CPU_OFFSET                        RTDRV_CPU_NUM * CMD_SPACE
#define RTDRV_DEBUG_OFFSET                      RTDRV_DEBUG_NUM * CMD_SPACE
#define RTDRV_DOT1X_OFFSET                      RTDRV_DOT1X_NUM * CMD_SPACE
#define RTDRV_INTR_OFFSET                       RTDRV_INTR_NUM * CMD_SPACE
#define RTDRV_L2_OFFSET                         RTDRV_L2_NUM * CMD_SPACE
#define RTDRV_L34_OFFSET                        RTDRV_L34_NUM * CMD_SPACE
#define RTDRV_LED_OFFSET                        RTDRV_LED_NUM * CMD_SPACE
#define RTDRV_MIRROR_OFFSET                     RTDRV_MIRROR_NUM * CMD_SPACE
#define RTDRV_OAM_OFFSET                        RTDRV_OAM_NUM * CMD_SPACE
#define RTDRV_PONMAC_OFFSET                     RTDRV_PONMAC_NUM * CMD_SPACE
#define RTDRV_PORT_OFFSET                       RTDRV_PORT_NUM * CMD_SPACE
#define RTDRV_QOS_OFFSET                        RTDRV_QOS_NUM * CMD_SPACE
#define RTDRV_RLDP_OFFSET                       RTDRV_RLDP_NUM * CMD_SPACE
#define RTDRV_SEC_OFFSET                        RTDRV_SEC_NUM * CMD_SPACE
#define RTDRV_STAT_OFFSET                       RTDRV_STAT_NUM * CMD_SPACE
#define RTDRV_STP_OFFSET                        RTDRV_STP_NUM * CMD_SPACE
#define RTDRV_SVLAN_OFFSET                      RTDRV_SVLAN_NUM * CMD_SPACE
#define RTDRV_SWITCH_OFFSET                     RTDRV_SWITCH_NUM * CMD_SPACE
#define RTDRV_TRAP_OFFSET                       RTDRV_TRAP_NUM * CMD_SPACE
#define RTDRV_TRUNK_OFFSET                      RTDRV_TRUNK_NUM * CMD_SPACE
#define RTDRV_VLAN_OFFSET                       RTDRV_VLAN_NUM * CMD_SPACE
#define RTDRV_GPON_OFFSET                       RTDRV_GPON_NUM * CMD_SPACE
#define RTDRV_PKT_OFFSET                        RTDRV_PKT_NUM * CMD_SPACE
#define RTDRV_RATE_OFFSET                       RTDRV_RATE_NUM * CMD_SPACE
#define RTDRV_REG_OFFSET                        RTDRV_REG_NUM * CMD_SPACE
#define RTDRV_GPIO_OFFSET                       RTDRV_GPIO_NUM * CMD_SPACE
#define RTDRV_I2C_OFFSET                        RTDRV_I2C_NUM * CMD_SPACE
#define RTDRV_L34_LITE_OFFSET                   RTDRV_L34_LITE_NUM * CMD_SPACE
#define RTDRV_EPON_OFFSET                       RTDRV_EPON_NUM * CMD_SPACE
#define RTDRV_TIME_OFFSET                       RTDRV_TIME_NUM * CMD_SPACE
#define RTDRV_PPSTOD_OFFSET                     RTDRV_PPSTOD_NUM * CMD_SPACE
#define RTDRV_PONLED_OFFSET                     RTDRV_PONLED_NUM * CMD_SPACE
#define RTDRV_PBO_OFFSET                        RTDRV_PBO_NUM * CMD_SPACE
#define RTDRV_MDIO_OFFSET                       RTDRV_MDIO_NUM * CMD_SPACE
#if defined(CONFIG_EUROPA_FEATURE) || defined(CONFIG_RTL8290C_FEATURE) || defined(CONFIG_RTL8291_FEATURE)
#define RTDRV_LDD_OFFSET                        RTDRV_LDD_NUM * CMD_SPACE
#endif
#if defined(CONFIG_COMMON_RT_API)
#define RTDRV_RT_SWITCH_OFFSET                  RTDRV_RT_SWITCH_NUM * CMD_SPACE
#define RTDRV_RT_GPON_OFFSET                    RTDRV_RT_GPON_NUM * CMD_SPACE
#define RTDRV_RT_L2_OFFSET                      RTDRV_RT_L2_NUM * CMD_SPACE
#define RTDRV_RT_VLAN_OFFSET                    RTDRV_RT_VLAN_NUM * CMD_SPACE
#define RTDRV_RT_STAT_OFFSET                    RTDRV_RT_STAT_NUM * CMD_SPACE
#define RTDRV_RT_RATE_OFFSET                    RTDRV_RT_RATE_NUM * CMD_SPACE
#define RTDRV_RT_QOS_OFFSET                     RTDRV_RT_QOS_NUM * CMD_SPACE
#define RTDRV_RT_INTR_OFFSET                    RTDRV_RT_INTR_NUM * CMD_SPACE
#define RTDRV_RT_SEC_OFFSET                     RTDRV_RT_SEC_NUM * CMD_SPACE
#define RTDRV_RT_TRAP_OFFSET                    RTDRV_RT_TRAP_NUM * CMD_SPACE
#define RTDRV_RT_I2C_OFFSET                     RTDRV_RT_I2C_NUM * CMD_SPACE
#define RTDRV_RT_LED_OFFSET                     RTDRV_RT_LED_NUM * CMD_SPACE
#define RTDRV_RT_PORT_OFFSET                    RTDRV_RT_PORT_NUM * CMD_SPACE
#define RTDRV_RT_EPON_OFFSET                    RTDRV_RT_EPON_NUM * CMD_SPACE
#define RTDRV_RT_CLS_OFFSET                     RTDRV_RT_CLS_NUM * CMD_SPACE
#define RTDRV_RT_PONMISC_OFFSET                 RTDRV_RT_PONMISC_NUM * CMD_SPACE
#define RTDRV_RT_MIRROR_OFFSET                  RTDRV_RT_MIRROR_NUM * CMD_SPACE
#define RTDRV_RT_TIME_OFFSET                    RTDRV_RT_TIME_NUM * CMD_SPACE
#endif
#define RTDRV_END_OFFSET                        RTDRV_END_NUM * CMD_SPACE

/***** RTDRV_SET *****/
#define RTDRV_INIT_RTKAPI                       (RTDRV_BASE_CTL)
#define RTDRV_SET_MAX                           (RTDRV_BASE_CTL + RTDRV_END_OFFSET)


enum rtdrv_ext_offset_num_e
{
	RTDRV_EXT_FC_NUM_0=1, //start from 1 , zero for omci 
	RTDRV_EXT_FC_NUM_1=2,
	RTDRV_EXT_IGMPHOOK_NUM=4,
	RTDRV_EXT_END_NUM,
};

/* for fc driver*/
#define RTDRV_EXT_FC0_OFFSET             		(RTDRV_END_OFFSET +(RTDRV_EXT_FC_NUM_0*CMD_SPACE))
#define RTDRV_EXT_FC1_OFFSET             		(RTDRV_END_OFFSET +(RTDRV_EXT_FC_NUM_1*CMD_SPACE))
#define RTDRV_EXT_OFFSET             			(RTDRV_EXT_FC0_OFFSET)
#define RTDRV_EXTEND_OFFSET            			(RTDRV_EXT_FC0_OFFSET+(RTDRV_EXT_FC_NUM_1-RTDRV_EXT_FC_NUM_0+1)*CMD_SPACE)

/*for igmp hook module */
#define RTDRV_EXT_IGMPHOOK_OFFSET				(RTDRV_END_OFFSET +(RTDRV_EXT_IGMPHOOK_NUM*CMD_SPACE))
#define RTDRV_EXT_IGMPHOOKEND_OFFSET            (RTDRV_EXT_IGMPHOOK_OFFSET+(1*CMD_SPACE))


#endif // __RTDRV_SYMOFFSET_H__

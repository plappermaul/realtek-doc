/***********************************************************************/
/* This file contains unpublished documentation and software           */
/* proprietary to Cortina Access Incorporated. Any use or disclosure,  */
/* in whole or in part, of the information in this file without a      */
/* written consent of an officer of Cortina Access Incorporated is     */
/* strictly prohibited.                                                */
/* Copyright (c) 2024 by Cortina Access Incorporated.                  */
/***********************************************************************/
/*
 * ca_types.h
 *
 * Include file containing some basic and common data types and defines
 * used by driver.
 *
 * $Id$
 */
#ifndef __CA_TYPES_H__
#define __CA_TYPES_H__

#ifndef __ASSEMBLER__
/*
 * Basic data types
 */
/*
 * Basic data types
 */
#	ifdef CONFIG_BIT64
#		define CA_ARCH_64BIT 1
#	endif

#	ifdef CA_ARCH_64BIT
typedef unsigned long ca_uint;
typedef long ca_int;
typedef unsigned long ca_uint_t;
typedef long ca_int_t;
#	else
typedef unsigned int ca_uint;
typedef int ca_int;
typedef unsigned int ca_uint_t;
typedef int ca_int_t;
#	endif

typedef unsigned long long ca_uint64;
typedef long long ca_int64;
typedef unsigned int ca_uint32;
typedef int ca_int32;
typedef unsigned short ca_uint16;
typedef short ca_int16;
typedef unsigned char ca_uint8;
typedef signed char ca_int8;
typedef ca_uint8 ca_boolean;
typedef ca_int32 ca_status;
typedef ca_uint32 ca_addr;

typedef ca_int8 ca_int8_t;
typedef ca_int16 ca_int16_t;
typedef ca_int32 ca_int32_t;
typedef ca_uint8 ca_uint8_t;
typedef ca_uint16 ca_uint16_t;
typedef ca_uint32 ca_uint32_t;

typedef ca_uint16 ca_dev_id_t;
typedef ca_uint16 ca_port_id_t;
typedef int ca_cos_t;
typedef int ca_queue_t;
typedef unsigned int ca_tunnel_id_t;
typedef unsigned int ca_rtp_id_t;
typedef unsigned short ca_session_id_t;
typedef unsigned short ca_ppp2_pro_t;
typedef unsigned int ca_ip_translate_id_t;

/* FIXME: re-visit CA_UNION and CA_DI - for now ported over from Cortina */
#	ifndef CA_UNION
#		define CA_UNION union
#	endif

#	ifndef CA_DONT_USE_DESGNTD_INITLZR
#		define CA_DI(x) x
#	else
#		define CA_DI(x)
#	endif

#	ifndef __LINE__
#		define __LINE__ 0
#	endif
#	ifndef __FILE__
#		define __FILE__ "unknown"
#	endif

/* Conflict with Linux definition
#ifndef __FUNCTION__
#define __FUNCTION__ __FILE__
#endif
*/

/*
 * MAC address struct
 */
typedef struct {
	ca_uint8 byte5;
	ca_uint8 byte4;
	ca_uint8 byte3;
	ca_uint8 byte2;
	ca_uint8 byte1;
	ca_uint8 byte0;
} ca_mac_t;

/*
 * Register data type
 */
typedef ca_uint32 ca_reg;

/*
 * Other typedef's
 */
typedef enum { CA_DISABLE = 0, CA_ENABLE = 1 } ca_ctl_t;

typedef enum {
	CA_RESET_DEASSERT = 0,
	CA_RESET_ASSERT = 1,
	CA_RESET_TOGGLE = 2
} ca_reset_action_t;

typedef enum {
	CA_TX = 0,
	CA_RX,
	CA_TX_AND_RX,
	CA_RX_AND_TX = CA_TX_AND_RX
} ca_dir_t;

typedef enum { CA_OP_READ, CA_OP_WRITE } ca_rw_t;

typedef enum { CA_FALSE = 0, CA_TRUE = 1 } ca_boolean_t;

/*
 * Chip(Driver) Type
 */
typedef enum {
	CA_CHIP_UNKNOWN = 0,
	CA_CHIP_MISANO,
	CA_CHIP_DAYTONA,
	CA_CHIP_ESTORIL,
	CA_CHIP_SUNI_TETRA
} ca_chip_type_t;

#	ifndef CA_IRQ_INFO_DEFINED
#		define CA_IRQ_INFO_DEFINED
/*
 * Interrupt info data-structure - driver fills in this
 * structure before calling the user macro, CA_IRQ_USER_HANDLER()
 */
typedef struct {
	ca_chip_type_t chip; /* Chip Name         */
	ca_uint32 mod_id; /* Module/Block Id   */
	ca_uint32 grp_id; /* Group Id          */
	const char* grp_name; /* Group name string */
	ca_uint16 irq_id; /* IRQ Id            */
	const char* irq_id_name; /* IRQ Id name string */
	ca_uint16 flags; /* flags : contain info like status valid,
	                    param1 valid and param2 valid    */
	ca_uint16 status; /* Status value, if status is valid */
	ca_uint32 param1; /* param1 value, if param1 is valid */
	ca_uint32 param2; /* param2 value, if param2 is valid */
} ca_irq_info_t;

#		define CA_IRQ_INFO_STATUS_FLAG (0x0001) /* status field valid */
#		define CA_IRQ_INFO_PARAM1_FLAG (0x0002) /* param1 field valid */
#		define CA_IRQ_INFO_PARAM2_FLAG (0x0004) /* param2 field valid */
#		define CA_IRQ_INFO_SHRD_DEVICE_FLAG (0x0008) /* shared block       */

#	endif /* CA_IRQ_INFO_DEFINED */

/*
 * Other defines
 */
#	define CA_DONE 1
#	define CA_OK 0
#	define CA_ERROR -1
#	define CA_ERR_ENTRY_NOT_FOUND -2

typedef enum {
	CA_E_ERROR = CA_ERROR,
	CA_E_OK = CA_OK,
	CA_E_NONE = CA_OK,
	CA_E_INIT,
	CA_E_DEV_ID,
	CA_E_PORT_ID,
	CA_E_RESOURCE,
	CA_E_PARAM,
	CA_E_DIR,
	CA_E_EXISTS,
	CA_E_NULL_PTR,
	CA_E_NOT_FOUND,
	CA_E_CONFLICT,
	CA_E_TIMEOUT,
	CA_E_INUSING,
	CA_E_NOT_SUPPORT,
	CA_E_MCAST_ADDR_EXISTS,
	CA_E_MCAST_ADDR_ADD_FAIL,
	CA_E_MCAST_ADDR_DELETE_FAIL,
	CA_E_MEM_ALLOC,
	CA_E_NEXTHOP_INVALID,
	CA_E_NEXTHOP_NOT_FOUND,
	CA_E_ROUTE_MAX_LIMIT,
	CA_E_ROUTE_NOT_FOUND,

} ca_status_t;

#	ifndef TRUE
#		define TRUE 1
#	endif

#	ifndef FALSE
#		define FALSE 0
#	endif

#	ifndef NULL
#		define NULL 0
#	endif

#	ifndef IN
#		define IN
#		define CA_IN

#	endif

#	ifndef OUT
#		define OUT
#		define CA_OUT
#	endif

#	ifndef CA_IN_OUT
#		define CA_IN_OUT
#	endif

#	ifdef CA_DONT_USE_INLINE
#		define CA_INLINE static
#	else
#		define CA_INLINE __inline__ static
#	endif

#endif /* !__ASSEMBLER__ */

#endif /* __CA_TYPES_H__ */

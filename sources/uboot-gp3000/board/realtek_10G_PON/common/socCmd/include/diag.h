/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _DIAG_H_
#define _DIAG_H_

#include "platform_autoconf.h"
#include "basic_types.h"

#include <stddef.h> /* for size_t */

#ifdef CONFIG_DEBUG_LOG
extern u32 ConfigDebugErr;
extern u32 ConfigDebugInfo;
extern u32 ConfigDebugTrace;
extern u32 ConfigDebugWarn;
#endif

typedef enum _MODULE_DEFINE_ {
    _DRIVER_    = 1<<0,
    _HAL_       = 1<<1,
    _DMA_       = 1<<2,
    _SDIO_      = 1<<3,
    _MBOX_      = 1<<4,
    _TIMER_     = 1<<5,
    _DBG_CRYPTO_= 0x00020000

} MODULE_DEFINE;

_LONG_CALL_
extern
u32
DiagPrintf(
    IN  char *fmt, ...
);

int
DiagSnPrintf(
	IN char *dst, 
	IN size_t count,
	IN const char * src, ...
);

int 
prvDiagPrintf(
    IN  const char *fmt, ...
);    

int
prvDiagSPrintf(
    IN char *buf,
    IN  const char *fmt, ...
);


#define _DbgDump  DiagPrintf       

#define DRIVER_PREFIX	"RTL8195A[Driver]: "
#define HAL_PREFIX      "RTL8195A[HAL]: "
#define DMA_PREFIX      "RTL8195A[DMA]: "
#define SDIO_PREFIX     "RTL8195A[SDIO]"
#define MBOX_PREFIX		"[MBOX]"
#define TIMER_PREFIX	"[TIMER]"

#define SDIO_ERR_PREFIX         "[SDIO Err]"
#define SDIO_WARN_PREFIX        "[SDIO Warn]"
#define SDIO_TRACE_PREFIX       "[SDIO Trace]"
#define SDIO_INFO_PREFIX        "[SDIO Info]"

#define IPSEC_ERR_PREFIX        "[CRYP Err]"
#define IPSEC_WARN_PREFIX       "[CRYP Wrn]"
#define IPSEC_INFO_PREFIX       "[CRYP Inf]"
//#ifdef 

#ifdef CONFIG_DEBUG_LOG

#define DBG_8195A_DRIVER(...)     do {\
    _DbgDump("\r"DRIVER_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_8195A_HAL(...)     do {\
    _DbgDump("\r"HAL_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_8195A_DMA(...)     do {\
    _DbgDump("\r"DMA_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_8195A_SDIO(...)     do {\
    _DbgDump("\r"SDIO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_SDIO_ERR(...)     do {\
        if (ConfigDebugErr & _SDIO_) \
        _DbgDump("\r"SDIO_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_CRYPTO_ERR(...)     do {\
        if (ConfigDebugErr & _DBG_CRYPTO_) \
        _DbgDump("\r"IPSEC_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_SDIO_WARN(...)     do {\
		if (ConfigDebugWarn & _SDIO_) \
			_DbgDump("\r"SDIO_WARN_PREFIX __VA_ARGS__);\
	}while(0)

#define DBG_SDIO_INFO(...)     do {\
			if (ConfigDebugInfo & _SDIO_) \
				_DbgDump("\r"SDIO_INFO_PREFIX __VA_ARGS__);\
		}while(0)

#define DBG_SDIO_TRACE(...)     do {\
				if (ConfigDebugTrace & _SDIO_) \
					_DbgDump("\r"SDIO_TRACE_PREFIX __VA_ARGS__);\
			}while(0)

#define DBG_8195A(...)     do {\
    _DbgDump("\r" __VA_ARGS__);\
}while(0)

#define MONITOR_LOG(...)     do {\
    _DbgDump( __VA_ARGS__);\
}while(0)

#define DBG_ERROR_LOG(...)     do {\
    _DbgDump( __VA_ARGS__);\
}while(0)

#define DBG_MBOX_ERR(...)     do {\
	if (ConfigDebugErr & _MBOX_) \
    	_DbgDump("\r"MBOX_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_MBOX_WARN(...)     do {\
		if (ConfigDebugWarn & _MBOX_) \
			_DbgDump("\r"MBOX_PREFIX __VA_ARGS__);\
	}while(0)

#define DBG_MBOX_INFO(...)     do {\
			if (ConfigDebugInfo & _MBOX_) \
				_DbgDump("\r"MBOX_PREFIX __VA_ARGS__);\
		}while(0)

#define DBG_MBOX_TRACE(...)     do {\
				if (ConfigDebugTrace & _MBOX_) \
					_DbgDump("\r"MBOX_PREFIX __VA_ARGS__);\
			}while(0)

#define DBG_TIMER_ERR(...)     do {\
                    if (ConfigDebugErr & _TIMER_) \
                        _DbgDump("\r"TIMER_PREFIX __VA_ARGS__);\
                }while(0)
                
#define DBG_TIMER_WARN(...)     do {\
                        if (ConfigDebugWarn & _TIMER_) \
                            _DbgDump("\r"TIMER_PREFIX __VA_ARGS__);\
                    }while(0)
                
#define DBG_TIMER_INFO(...)     do {\
                            if (ConfigDebugInfo & _TIMER_) \
                                _DbgDump("\r"TIMER_PREFIX __VA_ARGS__);\
                        }while(0)
                
#define DBG_TIMER_TRACE(...)     do {\
                                if (ConfigDebugTrace & _TIMER_) \
                                    _DbgDump("\r"TIMER_PREFIX __VA_ARGS__);\
                            }while(0)

#else
#define DBG_8195A_DRIVER(...)

#define DBG_8195A_HAL(...)

#define DBG_8195A(...)

#define DBG_8195A_DMA(...) 

#define MONITOR_LOG(...)

#define DBG_ERROR_LOG(...)

#define DBG_8195A_SDIO(...)

#define DBG_SDIO_ERR(...)
#define DBG_SDIO_WARN(...)
#define DBG_SDIO_INFO(...)
#define DBG_SDIO_TRACE(...)

#define DBG_MBOX_ERR(...)
#define DBG_MBOX_WARN(...)
#define DBG_MBOX_INFO(...)
#define DBG_MBOX_TRACE(...)

#define DBG_TIMER_ERR(...)
#define DBG_TIMER_WARN(...)
#define DBG_TIMER_INFO(...)
#define DBG_TIMER_TRACE(...)

#endif

#ifdef CONFIG_DEBUG_LOG
typedef enum _DBG_CFG_TYPE_ {
	DBG_CFG_ERR=0,
	DBG_CFG_WARN=1,
	DBG_CFG_TRACE=2,
	DBG_CFG_INFO=3	
} DBG_CFG_TYPE;

typedef struct _DBG_CFG_CMD_ {
	u8 cmd_name[16];
	u32	cmd_type;
} DBG_CFG_CMD, *PDBG_CFG_CMD;

#endif

typedef enum _CONSOLE_OP_STAGE_ {
    ROM_STAGE = 0,
    RAM_STAGE = 1
}CONSOLE_OP_STAGE;

#endif //_DIAG_H_

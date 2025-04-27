/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_MSG_H__
#define __RTRPC_MSG_H__

/*
* Include Files
*/
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/rt_error.h>

typedef uint16  rtrpc_sn_t; /* sequence number */
#define RTDRV_MSG_FLAG_SET      0x00000001
typedef struct rtdrv_msgHdr_s
{
    uint32          flag;
    int             optid;
    uint32          ret_code;
    int             data_size;
    rtrpc_sn_t      seqNum;
    uint8           data[0];
}__attribute__ ((packed)) rtdrv_msgHdr_t;


extern int rtrpc_msg_set(uint32 unit, unsigned int optid, void *varptr, unsigned int size);
extern int rtrpc_msg_get(uint32 unit, unsigned int optid, void *varptr, unsigned int size);

#define SETSOCKOPT(optid, varptr, vartype, qty) \
    do { \
        unsigned int ret; \
        if ((ret = rtrpc_msg_set(master_view_unit, optid, varptr, (sizeof(vartype) * qty))) != RT_ERR_OK) {\
            return ret; \
        }\
    } while (0)

#define GETSOCKOPT(optid, varptr, vartype, qty) \
    do { \
        unsigned int ret; \
        if ((ret = rtrpc_msg_get(master_view_unit, optid, varptr, (sizeof(vartype) * qty))) != RT_ERR_OK) {\
            return ret; \
        }\
    } while (0)


/* unit ID translate */
typedef int32 (*rtrpc_unitId2remoteUnit_f)(uint32, uint32 *);
extern rtrpc_unitId2remoteUnit_f rtrpc_unitId2remoteUnit_translate;
void rtrpc_unitIdTranslateFunc_reg(rtrpc_unitId2remoteUnit_f f);

#define RTRPC_UNIT_NOT_USED 99
#define RTRPC_UNIT_ID_XLATE(global_unit, local_unit)    \
    do {\
        if(rtrpc_unitId2remoteUnit_translate!=NULL)\
            if(rtrpc_unitId2remoteUnit_translate(global_unit, local_unit)!=RT_ERR_OK)\
                return RT_ERR_OK;\
    } while (0)


/* message tx/rx */
#ifdef USING_RTSTK_PKT_AS_RAIL
    typedef int32 (*rtrpc_pktTx_f)(uint32, uint8 *, int);
#else
    typedef int32 (*rtrpc_pktTx_f)(rail_cpuId_t *,uint8 *, int32, int16, uint32);
#endif
void rtrpc_txFunc_reg(rtrpc_pktTx_f f);


#endif //__RTRPC_MSG_H__


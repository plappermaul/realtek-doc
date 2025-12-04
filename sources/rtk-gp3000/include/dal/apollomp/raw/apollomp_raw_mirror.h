#ifndef _APOLLOMP_RAW_MIRROR_H_
#define _APOLLOMP_RAW_MIRROR_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <dal/apollomp/raw/apollomp_raw.h>
/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/


/****************************************************************/
/* Type Definition                                              */
/****************************************************************/

typedef struct apollomp_raw_mirror_cfg_s
{
    rtk_portmask_t  source_portmask;
    rtk_port_t      monitor_port;
    rtk_enable_t    mirror_tx;
    rtk_enable_t    mirror_rx;
} apollomp_raw_mirror_cfg_t;

extern int32 apollomp_raw_mirrorCfg_set(apollomp_raw_mirror_cfg_t *pMirror_cfg);
extern int32 apollomp_raw_mirrorCfg_get(apollomp_raw_mirror_cfg_t *pMirror_cfg);
extern int32 apollomp_raw_mirrorIso_set(rtk_enable_t state);
extern int32 apollomp_raw_mirrorIso_get(rtk_enable_t *pState);

#endif /*#ifndef _APOLLOMP_RAW_MIRROR_H_*/


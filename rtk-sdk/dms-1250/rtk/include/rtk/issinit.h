

#ifndef __ISS_INIT_H__
#define __ISS_INIT_H__

#include <drv/nic/nic.h>

extern int iss_init_rtk(void);
extern int32 iss_nic_init(void);
extern int32 rtnic_pkt_free(uint32, drv_nic_pkt_t *);
extern int32 rtnic_pkt_alloc(uint32, int32, uint32 , drv_nic_pkt_t **);

#endif /* __ISS_INIT_H__ */

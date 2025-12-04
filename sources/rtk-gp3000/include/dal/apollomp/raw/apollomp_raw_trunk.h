#ifndef _APOLLOMP_RAW_TRUNK_H_
#define _APOLLOMP_RAW_TRUNK_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <dal/apollomp/raw/apollomp_raw.h>
/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/
#define APOLLOMP_RAW_TRUNK_HASH_SOURCE_PORT      (0x01)
#define APOLLOMP_RAW_TRUNK_HASH_SOURCE_MAC       (0x02)
#define APOLLOMP_RAW_TRUNK_HASH_DEST_MAC         (0x04)
#define APOLLOMP_RAW_TRUNK_HASH_SOURCE_IP        (0x08)
#define APOLLOMP_RAW_TRUNK_HASH_DEST_IP          (0x10)
#define APOLLOMP_RAW_TRUNK_HASH_SOURCE_L4PORT    (0x20)
#define APOLLOMP_RAW_TRUNK_HASH_DEST_L4PORT      (0x40)

#define APOLLOMP_RAW_TRUNK_HASH_ALL  \
( APOLLOMP_RAW_TRUNK_HASH_SOURCE_PORT |   \
  APOLLOMP_RAW_TRUNK_HASH_SOURCE_MAC |    \
  APOLLOMP_RAW_TRUNK_HASH_DEST_MAC |      \
  APOLLOMP_RAW_TRUNK_HASH_SOURCE_IP |     \
  APOLLOMP_RAW_TRUNK_HASH_DEST_IP |       \
  APOLLOMP_RAW_TRUNK_HASH_SOURCE_L4PORT | \
  APOLLOMP_RAW_TRUNK_HASH_DEST_L4PORT )

#define APOLLOMP_RAW_TRUNK_ALGORITHM_NUM         (7)
#define APOLLOMP_RAW_TRUNK_HASH_VALUE_MAX        (15)

/****************************************************************/
/* Type Definition                                              */
/****************************************************************/

typedef enum apollomp_raw_trunk_mode_e
{
    APOLLOMP_RAW_TRUNK_USER_MODE = 0,
    APOLLOMP_RAW_TRUNK_DUMB_MODE,
    APOLLOMP_RAW_TRUNK_MODE_END
} apollomp_raw_trunk_mode_t;

typedef enum apollomp_raw_trunk_floodMode_e
{
    APOLLOMP_RAW_TRUNK_NORMAL_FLOOD = 0,
    APOLLOMP_RAW_TRUNK_FWD_TO_FIRST_PORT,
    APOLLOMP_RAW_TRUNK_FLOOD_MODE_END
} apollomp_raw_trunk_floodMode_t;

typedef enum apollomp_raw_trunk_port_e
{
    APOLLOMP_RAW_TRUNK_PORT_FIRST = 0,
    APOLLOMP_RAW_TRUNK_PORT_SECOND,
    APOLLOMP_RAW_TRUNK_PORT_THIRD,
    APOLLOMP_RAW_TRUNK_PORT_FOURTH,
    APOLLOMP_RAW_TRUNK_PORT_END
} apollomp_raw_trunk_port_t;

extern int32 apollomp_raw_trunk_memberPort_set(rtk_portmask_t *pPortmask);
extern int32 apollomp_raw_trunk_memberPort_get(rtk_portmask_t *pPortmask);
extern int32 apollomp_raw_trunk_hashAlgorithm_set(uint32 hashsel);
extern int32 apollomp_raw_trunk_hashAlgorithm_get(uint32 *pHashsel);
extern int32 apollomp_raw_trunk_flood_set(apollomp_raw_trunk_floodMode_t mode);
extern int32 apollomp_raw_trunk_flood_get(apollomp_raw_trunk_floodMode_t *pMode);
extern int32 apollomp_raw_trunk_mode_set(apollomp_raw_trunk_mode_t mode);
extern int32 apollomp_raw_trunk_mode_get(apollomp_raw_trunk_mode_t *pMode);
extern int32 apollomp_raw_trunk_flowControl_set(rtk_enable_t state);
extern int32 apollomp_raw_trunk_flowControl_get(rtk_enable_t *pState);
extern int32 apollomp_raw_trunk_hashMapping_set(uint32 hash_value, apollomp_raw_trunk_port_t port);
extern int32 apollomp_raw_trunk_hashMapping_get(uint32 hash_value, apollomp_raw_trunk_port_t *pPort);

#endif /*#ifndef _APOLLOMP_RAW_TRUNK_H_*/


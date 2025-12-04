#ifndef __GPON_PPPOE_ADV_H__
#define __GPON_PPPOE_ADV_H__

typedef enum gpon_pppoe_mode_s{
    GPON_PPPOE_MODE_HW = 0,
    GPON_PPPOE_MODE_SW = 1,
    GPON_PPPOE_MODE_SW_OFFLOAD = 2,
    GPON_PPPOE_MODE_END
}gpon_pppoe_mode_t;

typedef enum
gpon_pppoe_subtype_s {
    GPON_PPPOE_SUBTYPE_SW = 0,
    GPON_PPPOE_SUBTYPE_RECOVER,
    GPON_PPPOE_SUBTYPE_SW_OFFLOAD,
    GPON_PPPOE_SUBTYPE_END
} gpon_pppoe_subtype_t; 

/* Define the communication structures */

extern int gpon_pppoe_db_clear(void);
extern int gpon_pppoe_mode_get(uint32 sid, gpon_pppoe_mode_t *pMode);

#endif /* __GPON_PPPOE_ADV_H__ */


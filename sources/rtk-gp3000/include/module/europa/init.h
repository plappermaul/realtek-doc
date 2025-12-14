#ifndef __EUROPA_INIT_H__
#define __EUROPA_INIT_H__
extern void apollo_init(void);
extern void europa_intrInit(void);
extern void europa_intrDeInit(void);

extern void calibration_state_set(uint8 state);
extern void europa_debug_level_set(uint8 level);
extern uint8 europa_debug_level_get(void);

extern void ddmi_a0_init(uint8 flash_update, uint8 *flash_data[]);
extern void ddmi_a1_init(uint8 flash_update, uint8 *flash_data[]);
extern void patch_code_a4_init(uint8 flash_update, uint8 patch_len, uint8 *flash_data[]);
extern void apd_power_init(uint8 flash_update, uint8 *flash_data[]);

extern void parameter_init(void);

extern void europa_init(void);

extern void ldd_mapper_init(void);
extern void ldd_mapper_exit(void);

extern void ldd_ploamState_handler_init(void);
extern void ldd_ploamState_handler_exit(void);

/* TX_SD_TOGGLE_INT_MASK did not defined in europa_reg_definition.h */
#ifndef EUROPA_REG_INT_STATUS2_TX_SD_TOGGLE_INT_OFFSET
  #define EUROPA_REG_INT_STATUS2_TX_SD_TOGGLE_INT_OFFSET (5)
  #define EUROPA_REG_INT_STATUS2_TX_SD_TOGGLE_INT_MASK (0x1 << (5))
#endif

/* EXTRA_STATUS1 did not defined in europa_reg_definition.h */
#ifndef EUROPA_REG_EXTRA_STATUS1_ADDR
  #define EUROPA_REG_EXTRA_STATUS1_ADDR (0x3C2)
  #define EUROPA_REG_EXTRA_STATUS1_TX_SD_OFFSET (4)
  #define EUROPA_REG_EXTRA_STATUS1_TX_SD_MASK   (0x1 << (4))
#endif

#endif

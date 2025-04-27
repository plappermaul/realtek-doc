#ifndef ECC_STRUCT_H
#define ECC_STRUCT_H

/***********************************************
  * ECC MODEL Definition in "xxx_nand_info_t"
  ***********************************************/
#define UNIT_ECC_BITS   (6)
#define ECC_MODEL_6T                    ((6/UNIT_ECC_BITS)-1)
#define ECC_MODEL_12T                   ((12/UNIT_ECC_BITS)-1)
#define ECC_MODEL_18T                   ((18/UNIT_ECC_BITS)-1)
#define ECC_MODEL_24T                   ((24/UNIT_ECC_BITS)-1)
#define ECC_CORRECT_BITS(info)          ((info->_ecc_ability+1) * UNIT_ECC_BITS)
#define ECC_USE_ODE                     (0xF)


/***********************************************
  * BCH Size & Type Definition
  ***********************************************/
#define BCH_SECTS_PER_2K_PAGE (4)
#define BCH_SECTS_PER_4K_PAGE (8)
#define BCH_TAG_SIZE          (6)

#endif //ECC_STRUCT_H


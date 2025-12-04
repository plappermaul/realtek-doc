#ifndef __RTL8291_REG_ACCESS_H__
#define __RTL8291_REG_ACCESS_H__

#include <rtk/i2c.h>


//#if defined(CONFIG_LUNA_G3_SERIES)
//#include <linux/mutex.h>
//extern struct mutex ldd_mutex;
//#define LDD_MUTEX_LOCK()      mutex_lock(&ldd_mutex)
//#define LDD_MUTEX_UNLOCK()    mutex_unlock(&ldd_mutex)
//#else
#include <osal/sem.h>
extern osal_mutex_t ldd_mutex;
#define LDD_MUTEX_LOCK()      osal_sem_mutex_take(ldd_mutex, OSAL_SEM_WAIT_FOREVER)
#define LDD_MUTEX_UNLOCK()    osal_sem_mutex_give(ldd_mutex)
//#endif

#define TABLE_A2PAGE0                0
#define TABLE_A2PAGE1                1
#define TABLE_WBREG00             0x80
#define TABLE_RBREG00             0x81
#define TABLE_DIGREG              0x82
#define TABLE_LASERLUT            0x83
#define TABLE_APDLUT              0x84
#define TABLE_RSVLUT1             0x85
#define TABLE_RSVLUT2             0x86
#define TABLE_DIGEXTREG           0x87
#define TABLE_DIGEXT2REG          0x88
#define TABLE_WBREG80             0x87
#define TABLE_INDACC              0xFE
#define TABLE_INFO                0xFF

#define COMMON_BASE        0x80
#define A2L_REG_BASE       0x100
#define A2_PAGE0_BASE      0x180
#define A2_PAGE1_BASE      0x200
#define WB_REG_BASE        0x280
#define RB_REG_BASE        0x300
#define DIG_REG_BASE       0x380
#define LASERLUT_BASE      0x400
#define APDLUT_BASE        0x480
#define RSVLUT1_BASE       0x500
#define RSVLUT2_BASE       0x580
#define INDACC_BASE        0x600
#define INFO_BASE          0x680
#define DIGEX_REG_BASE     0x700
#define DIGEX2_REG_BASE    0x780

typedef int32 (*func_i2c_write)(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 data);
typedef int32 (*func_i2c_read)(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData);

extern void _ldd_delay(void);
extern int32 rtl8291_setReg(uint32 devId, uint32 address, uint32 value);
extern int32 rtl8291_getReg(uint32 devId, uint32 address, uint32 *pValue);
extern int32 rtl8291_setRegBit(uint32 devId, uint32 reg, uint32 rBit, uint32 value);
extern int32 rtl8291_getRegBit(uint32 devId, uint32 reg, uint32 rBit, uint32 *pValue);
extern int32 rtl8291_setRegBits(uint32 devId, uint32 reg, uint32 rBits, uint32 value);
extern int32 rtl8291_getRegBits(uint32 devId, uint32 reg, uint32 rBits, uint32 *pValue);

#endif

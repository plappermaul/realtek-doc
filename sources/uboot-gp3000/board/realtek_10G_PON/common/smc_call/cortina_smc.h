#ifndef __CORTINA_SMC_H__
#define __CORTINA_SMC_H__

#define FUNCID_TYPE_SHIFT		31
#define FUNCID_CC_SHIFT			30
#define FUNCID_OEN_SHIFT		24
#define FUNCID_NUM_SHIFT		0

#define FUNCID_TYPE_MASK		0x1
#define FUNCID_CC_MASK			0x1
#define FUNCID_OEN_MASK			0x3f
#define FUNCID_NUM_MASK			0xffff

#define FUNCID_TYPE_WIDTH		1
#define FUNCID_CC_WIDTH			1
#define FUNCID_OEN_WIDTH		6
#define FUNCID_NUM_WIDTH		16

#define GET_SMC_CC(id)			((id >> FUNCID_CC_SHIFT) & \
					 FUNCID_CC_MASK)
#define GET_SMC_TYPE(id)		((id >> FUNCID_TYPE_SHIFT) & \
					 FUNCID_TYPE_MASK)

#define SMC_64				1
#define SMC_32				0
#define SMC_UNK				0xffffffff
#define SMC_TYPE_FAST			1
#define SMC_TYPE_STD			0
#define SMC_PREEMPTED		0xfffffffe

/*******************************************************************************
 * Owning entity number definitions inside the function id as per the SMC
 * calling convention
 ******************************************************************************/
#define OEN_ARM_START			0
#define OEN_ARM_END			0
#define OEN_CPU_START			1
#define OEN_CPU_END			1
#define OEN_SIP_START			2
#define OEN_SIP_END			2
#define OEN_OEM_START			3
#define OEN_OEM_END			3
#define OEN_STD_START			4	/* Standard Calls */
#define OEN_STD_END			4
#define OEN_TAP_START			48	/* Trusted Applications */
#define OEN_TAP_END			49
#define OEN_TOS_START			50	/* Trusted OS */
#define OEN_TOS_END			63
#define OEN_LIMIT			64

/* SMC function IDs for SiP Service queries */
#define SIP_SVC_CALL			0x82000000
#define SIP_SVC_UID			0x82000001
/*					0x8200ff02 is reserved */
#define SIP_SVC_VERSION			0x8200ff03

/* Cortina-Access SiP Service Calls version numbers */
#define CA_SIP_SVC_VERSION_MAJOR	0x0
#define CA_SIP_SVC_VERSION_MINOR	0x1

/* Valid FID  range for SiP is 0x82000000-0x8200FFFF
  * We use bit8~11 as sub-module ID.
  * All module can use up to 256 function ID(bit0~7).
  */
#define CA_SVC_NE_ID		0x1
#define CA_SVC_PER_ID		0x2
#define CA_SVC_FBM_ID		0x3
#define CA_SVC_DMA_ID		0x4
#define CA_SVC_LDMA_ID		0x5
#define CA_SVC_PCIE_ID		0x6
#define CA_SVC_CCI_ID		0x7
#define CA_SVC_CAPSRAM_ID	0x8
#define CA_SVC_SADB_ID		0x9
#define CA_SVC_OTP_ID		0xA
#define CA_SVC_PEATU_ID		0xB
#define CA_SVC_HSIOATU_ID	0xC
#define CA_SVC_EAXI_ID		0xD
#define CA_SVC_CRYP_ID		0xE
#define CA_SVC_RTC_ID		0xF
#define CA_SVC_WDT_ID		0x10
#define CA_SVC_MISC_ID		0x11

#define CA_SVC_NE			(CA_SVC_NE_ID << 8)
#define CA_SVC_PER			(CA_SVC_PER_ID << 8)
#define CA_SVC_FBM			(CA_SVC_FBM_ID << 8)
#define CA_SVC_DMA			(CA_SVC_DMA_ID << 8)
#define CA_SVC_LDMA			(CA_SVC_LDMA_ID << 8)
#define CA_SVC_PCIE			(CA_SVC_PCIE_ID << 8)
#define CA_SVC_CCI			(CA_SVC_CCI_ID << 8)
#define CA_SVC_CAPSRAM		(CA_SVC_CAPSRAM_ID << 8)
#define CA_SVC_SADB			(CA_SVC_SADB_ID << 8)
#define CA_SVC_OTP			(CA_SVC_OTP_ID << 8)
#define CA_SVC_PEATU		(CA_SVC_PEATU_ID << 8)
#define CA_SVC_HSIOATU		(CA_SVC_HSIOATU_ID << 8)
#define CA_SVC_EAXI			(CA_SVC_EAXI_ID << 8)
#define CA_SVC_CRYP			(CA_SVC_CRYP_ID << 8)
#define CA_SVC_RTC			(CA_SVC_RTC_ID << 8)
#define CA_SVC_WDT			(CA_SVC_WDT_ID << 8)
#define CA_SVC_MISC			(CA_SVC_MISC_ID << 8)

#define CA_SVC_MODULE_MASK		0x0000FF00

#define CA_SVC_FID_MASK			0x000000FF

/* FID for CA_SVC_WDT */
#define CA_SVC_WDT_ENABLE		(SIP_SVC_CALL|CA_SVC_WDT|0)
#define CA_SVC_WDT_DISABLE		(SIP_SVC_CALL|CA_SVC_WDT|1)
#define CA_SVC_WDT_SET_TIME		(SIP_SVC_CALL|CA_SVC_WDT|2)
#define CA_SVC_WDT_EN_RST		(SIP_SVC_CALL|CA_SVC_WDT|3)
#define CA_SVC_WDT_RELOAD		(SIP_SVC_CALL|CA_SVC_WDT|4)
#define CA_SVC_WDT_READ_CNT		(SIP_SVC_CALL|CA_SVC_WDT|5)

/*  RTC */

/* CRYPTO */
#define RTK_SVC_CRYP		CA_SVC_CRYP
#define RTK_SVC_AES256_CBC  (SIP_SVC_CALL|RTK_SVC_CRYP|0)

/* SADB */

/* EAXI */

/* HSIOATU */

/* OTP */
/* FID for CA_SVC_OTP */
#define CA_SVC_OTP_ROTPK_READ            (SIP_SVC_CALL|CA_SVC_OTP|3)
#define CA_SVC_OTP_ROTPK_WRITE	         (SIP_SVC_CALL|CA_SVC_OTP|4)
#define CA_SVC_OTP_CHIP_MODE_READ        (SIP_SVC_CALL|CA_SVC_OTP|11)
#define CA_SVC_OTP_GPHY_PATCH_READ       (SIP_SVC_CALL|CA_SVC_OTP|12)
#define CA_SVC_OTP_DRAM_MODEL_READ       (SIP_SVC_CALL|CA_SVC_OTP|13)
#define CA_SVC_OTP_BL33_AES_KEY_READ     (SIP_SVC_CALL|CA_SVC_OTP|14)
#define CA_SVC_OTP_BL33_AES_KEY_WRITE    (SIP_SVC_CALL|CA_SVC_OTP|15)
#define CA_SVC_OTP_AES_UNIQUE_KEY_READ   (SIP_SVC_CALL|CA_SVC_OTP|16)
#define CA_SVC_OTP_AES_UNIQUE_KEY_WRITE  (SIP_SVC_CALL|CA_SVC_OTP|17)

/* MISC */
#define CA_SVC_REG_READ			(SIP_SVC_CALL|CA_SVC_MISC|0)
#define CA_SVC_REG_WRITE		(SIP_SVC_CALL|CA_SVC_MISC|1)
#define CA_SVC_REG_SET_BIT		(SIP_SVC_CALL|CA_SVC_MISC|2)
#define CA_SVC_REG_CLR_BIT		(SIP_SVC_CALL|CA_SVC_MISC|3)
#define CA_SVC_GET_SOC_VERSION	(SIP_SVC_CALL|CA_SVC_MISC|6)
#define CA_SVC_GET_ATF_VERSION	(SIP_SVC_CALL|CA_SVC_MISC|7)

/* NE */

/* FBM */

/* CAPSRAM */

/* SMC API */
#ifndef __UBOOT__
u64 ca_smc_call(unsigned int smc_id, unsigned long x1, unsigned long x2,
		unsigned long x3, unsigned long x4);

#define CA_SMC_CALL_REG_READ(addr)	ca_smc_call(CA_SVC_REG_READ, addr, 0, 0, 0)
#define CA_SMC_CALL_REG_WRITE(addr, val) 	ca_smc_call(CA_SVC_REG_WRITE, addr, val, 0, 0)
#define CA_SMC_CALL_REG_READ64(addr)	ca_smc_call(CA_SVC_REG_READ, addr, 0, 1, 0)
#define CA_SMC_CALL_REG_WRITE64(addr, val) 	ca_smc_call(CA_SVC_REG_WRITE, addr, val, 1, 0)
#define CA_SMC_CALL_REG_BIT_SET(addr, bit)  	ca_smc_call(CA_SVC_REG_SET_BIT, addr, bit, 0, 0)
#define CA_SMC_CALL_REG_BIT_CLEAR(addr, bit)  	ca_smc_call(CA_SVC_REG_CLR_BIT, addr, bit, 0, 0)

#else               /* #ifndef __UBOOT__ */
u64 ca_smc_call(unsigned int smc_id, unsigned long x1, unsigned long x2, unsigned long x3, unsigned long x4);

/* src (x3):   src_data_addr */
#define SMC_CALL_OTP_READ_ROTPK_HASH(src)       ca_smc_call(CA_SVC_OTP_ROTPK_READ,            0, 0, src, 0)
#define SMC_CALL_OTP_WRITE_ROTPK_HASH(src)      ca_smc_call(CA_SVC_OTP_ROTPK_WRITE,           0, 0, src, 0)
#define SMC_CALL_OTP_READ_CHIP_MODE(src)        ca_smc_call(CA_SVC_OTP_CHIP_MODE_READ,        0, 0, src, 0)
#define SMC_CALL_OTP_READ_DRAM_MODEL(src)       ca_smc_call(CA_SVC_OTP_DRAM_MODEL_READ,       0, 0, src, 0)
#define SMC_CALL_GET_SOC_VERSION()              ca_smc_call(CA_SVC_GET_SOC_VERSION,           0, 0, 0,   0)
#define SMC_CALL_GET_ATF_VERSION()              ca_smc_call(CA_SVC_GET_ATF_VERSION,           0, 0, 0,   0)
#define SMC_CALL_OTP_READ_BL33_AESKEY()         ca_smc_call(CA_SVC_OTP_BL33_AES_KEY_READ,     0, 0, 0,   0)
#define SMC_CALL_OTP_WRITE_BL33_AESKEY(src)     ca_smc_call(CA_SVC_OTP_BL33_AES_KEY_WRITE,    0, 0, src, 0)
#define SMC_CALL_OTP_READ_AES_UNIQUE_KEY()      ca_smc_call(CA_SVC_OTP_AES_UNIQUE_KEY_READ,   0, 0, 0,   0)
#define SMC_CALL_OTP_WRITE_AES_UNIQUE_KEY(src)  ca_smc_call(CA_SVC_OTP_AES_UNIQUE_KEY_WRITE,  0, 0, src, 0)
#define SMC_CALL_OTP_IS_ROTPK_WRITTEN()         ca_smc_call(CA_SVC_OTP_IS_ATF_AUTHENTICATE,   0, 0, 0,   0)

#endif              /* #ifndef __UBOOT__ */
#endif				/* __CORTINA_SMC_H__ */

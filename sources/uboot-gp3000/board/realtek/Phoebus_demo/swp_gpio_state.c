#include <common.h>

enum GPIO_DEF {
	GPIO_0 = 0, GPIO_1, GPIO_2, GPIO_3, GPIO_4, GPIO_5, GPIO_6, GPIO_7, 
	GPIO_8, GPIO_9, GPIO_10, GPIO_11, GPIO_12, GPIO_13, GPIO_14, GPIO_15, 
	GPIO_16, GPIO_17, GPIO_18, GPIO_19, GPIO_20, GPIO_21, GPIO_22, GPIO_23, 
	GPIO_24, GPIO_25, GPIO_26, GPIO_27, GPIO_28, GPIO_29, GPIO_30, GPIO_31,
	GPIO_32, GPIO_33, GPIO_34, GPIO_35, GPIO_36, GPIO_37, GPIO_38, GPIO_39,
	GPIO_40, GPIO_41, GPIO_42, GPIO_43, GPIO_44, GPIO_45, GPIO_46, GPIO_47,
	GPIO_48, GPIO_49, GPIO_50, GPIO_51, GPIO_52, GPIO_53, GPIO_54, GPIO_55,
	GPIO_56, GPIO_57, GPIO_58, GPIO_59, GPIO_60, GPIO_61, GPIO_62, GPIO_63,
	GPIO_64, GPIO_65, GPIO_66, GPIO_67, GPIO_68, GPIO_69, GPIO_70, GPIO_71,
	GPIO_72, GPIO_73, GPIO_74, GPIO_75, GPIO_76, GPIO_77, GPIO_78, GPIO_79,
	GPIO_80, GPIO_81, GPIO_82, GPIO_83, GPIO_84, GPIO_85, GPIO_86, GPIO_87,
	GPIO_88, GPIO_89, GPIO_90, GPIO_91, GPIO_92, GPIO_93, GPIO_94, GPIO_95,
	GPIO_END
};

#define SOC_GPIO_PABCD_DIR (0xb8003308)
#define SOC_GPIO_PABCD_DAT (0xb800330C)
#define SOC_GPIO_PJKMN_DIR (0xb8003348)
#define SOC_GPIO_PJKMN_DAT (0xb800334C)
#define SOC_GPIO_REG_OFFSET_BASE (0x1C)

int gpioRead(int gpio_num) 
{
	unsigned int val;
	int ret = 0;
	unsigned int reg_num = ((gpio_num>>5)<<2);
	
	if ((gpio_num >= GPIO_END)||(gpio_num < 0)) 
		return 0;
	
	if (gpio_num <= GPIO_95) {
		if (gpio_num <= GPIO_63) {
			reg_num >>= 2;
			reg_num *= SOC_GPIO_REG_OFFSET_BASE;
			val = REG32(SOC_GPIO_PABCD_DAT + reg_num);
			ret = (val & (1 << (gpio_num%32))) ? 1:0;
		}else { //64~95
			val = REG32(SOC_GPIO_PJKMN_DAT);
			ret = (val & (1 << (gpio_num%32))) ? 1:0;
		}
	}
	else {
		printf("ERROR: GPIO %d is not supported!!!\n", gpio_num);
	}
	return ret;
}

int get_wps_button(void)
{
	return !gpioRead(GPIO_34);
}
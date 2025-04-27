#ifndef CPU_UTILS_H
#define CPU_UTILS_H


#define SET_SP(val) do {                  \
		__asm__ __volatile__ ("move $29, %0;" \
		                      ::"r" (val));   \
	} while(0)

#define GET_SP(val) do {                  \
		__asm__ __volatile__ ("move %0, $29;" \
		                      :"=r" (val));   \
	} while(0)

#define COUNTER_INIT() do {                \
		__asm__ __volatile__ ("mtc0 $0, $9;"); \
	} while(0)

#define COUNTER_GET(var) do {	           \
		__asm__ __volatile__ ("mfc0 %0, $9;" \
		                      :"=r" (var));  \
	} while(0)

#define SWBREAK() do {               \
		__asm__ __volatile__ ("sdbbp;"); \
	} while(0)

#endif

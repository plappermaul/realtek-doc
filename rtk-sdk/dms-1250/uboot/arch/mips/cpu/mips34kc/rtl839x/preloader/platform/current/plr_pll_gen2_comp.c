#ifdef __cplusplus
extern "C" {
#endif

#include <preloader.h>
#include <plr_pll_gen2.h>

/* 0:1, 1:2, 2:4, 3:8. */
#define SEL_PREDIV(x) ( (x)        & 0x3)
/* 0:1, 1:4 */
#define SEL_D4(x)     (((x) >> 2)  & 0x1)
#define NODE_IN(x)    (((x) >> 4)  & 0xFF)
#define DIVN2(x)      (((x) >> 12) & 0xFF)

/* 0:2, 1:3, 2:4, 3:6 */
#define DIVN3_SEL(x)  ((x) & 0x3)
#define DIVN2_SELB(x) (((x) >> 2)  & 0x1)

pll_result_t
_pll_gen2_get_to_mhz(const pll_info_t *pll_reg,
                     pll_gen2_mhz_t *pll_mhz) {
	const u8_t divn3[] = {2, 3, 4, 6};
	const u8_t extra_divisor[] = {2, 2, 4}; //CPU, LX, MEM
	u32_t ctrl0[3], ctrl1[3], mhz[3], dev;
	pll_result_t res = PLL_RES_OK;

	ctrl0[PLL_DEV_CPU] = pll_reg->sys_cpu_pll_ctl0;
	ctrl0[PLL_DEV_LX]  = pll_reg->sys_lx_pll_ctl0;
	ctrl0[PLL_DEV_MEM] = pll_reg->sys_mem_pll_ctl0;
	ctrl1[PLL_DEV_CPU] = pll_reg->sys_cpu_pll_ctl1;
	ctrl1[PLL_DEV_LX]  = pll_reg->sys_lx_pll_ctl1;
	ctrl1[PLL_DEV_MEM] = pll_reg->sys_mem_pll_ctl1;

	for (dev=0; dev<3; dev++) {
		mhz[dev] = (25 *
                    (SEL_D4(ctrl0[dev]) == 0 ? 1 : 4) *
		            (NODE_IN(ctrl0[dev]) + 4) /
		            (1 << SEL_PREDIV(ctrl0[dev])) /
		            (DIVN2_SELB(ctrl1[dev]) == 1  ?
		             divn3[DIVN3_SEL(ctrl1[dev])] :
		             (DIVN2(ctrl0[dev]) + 4)) /
		            extra_divisor[dev]);
	}
	pll_mhz->cpu = mhz[PLL_DEV_CPU];
	pll_mhz->lx  = mhz[PLL_DEV_LX];
	pll_mhz->mem = mhz[PLL_DEV_MEM];

	return res;
}

                              /*    400,     450,    500,    550,    600,    650,    700,    750 */
static u32_t CPU_pll_ctl0[] =  { 0x414C,  0x417C, 0x41AC, 0x412C, 0x414C, 0x416C, 0x418C, 0x41AC };
static u32_t CPU_pll_ctl1[] =  {    0x5,     0x5,    0x5,    0x4,    0x4,    0x4,    0x4,    0x4 };
                               /*     50,     100,    150,    200  */
static u32_t LX_pll_ctl0[]=    { 0x1414C, 0x814C, 0x414C, 0x414C };
static u32_t LX_pll_ctl1[]=    {     0x3,    0x3,    0x3,    0x7 };
                              /*     100,    125,    150,     175,    200,    225,    250,    275,    300,    325,    350,    375,     400 */
static u32_t MEM_pll_ctl0[] =  {  0x41CC, 0x41AC, 0x414C,  0x418C, 0x41CC, 0x417C, 0x41AC, 0x412C, 0x414C, 0x416C, 0x418C, 0x41AC,  0x41CC };
static u32_t MEM_pll_ctl1[] =  {     0x0,    0x7,    0x6,     0x6,    0x6,    0x5,    0x5,    0x4,    0x4,    0x4,    0x4,    0x4,     0x4 };

pll_result_t
_pll_gen2_set_from_mhz(pll_info_t *pll_reg,
                       pll_gen2_mhz_t *pll_mhz) {
	pll_result_t res = PLL_RES_OK;

	if (pll_mhz->mode == PLL_MODE_BY_SW) {
		pll_reg->set_by = 1;
	} else if (pll_mhz->mode == PLL_MODE_BY_PIN) {
		pll_reg->set_by = 0;
	} else {
		res = PLL_RES_BAD_MODE;
		return res;
	}

	if (pll_mhz->cpu < 400) {
		pll_mhz->cpu = 400;
		res = PLL_RES_FREQ_OUT_OF_RANGE;
	}
	if (pll_mhz->cpu > 750) {
		pll_mhz->cpu = 750;
		res = PLL_RES_FREQ_OUT_OF_RANGE;
	}
	if (pll_mhz->lx  < 50) {
		pll_mhz->lx  = 50;
		res = PLL_RES_FREQ_OUT_OF_RANGE;
	}
	if (pll_mhz->lx  > 200) {
		pll_mhz->lx  = 200;
		res = PLL_RES_FREQ_OUT_OF_RANGE;
	}
	if (pll_mhz->mem < 100) {
		pll_mhz->mem = 100;
		res = PLL_RES_FREQ_OUT_OF_RANGE;
	}
	if (pll_mhz->mem > 400) {
		pll_mhz->mem = 400;
		res = PLL_RES_FREQ_OUT_OF_RANGE;
	}

  pll_reg->sys_cpu_pll_ctl0 = CPU_pll_ctl0[((pll_mhz->cpu - 400) / 50)];
  pll_reg->sys_cpu_pll_ctl1 = CPU_pll_ctl1[((pll_mhz->cpu - 400) / 50)];
  pll_reg->sys_lx_pll_ctl0  = LX_pll_ctl0[((pll_mhz->lx - 50) / 50)];
  pll_reg->sys_lx_pll_ctl1  = LX_pll_ctl1[((pll_mhz->lx - 50) / 50)];
  pll_reg->sys_mem_pll_ctl0 = MEM_pll_ctl0[((pll_mhz->mem - 100) / 25)];
  pll_reg->sys_mem_pll_ctl1 = MEM_pll_ctl1[((pll_mhz->mem - 100) / 25)];

  return res;
}

#ifdef __cplusplus
}
#endif

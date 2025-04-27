#ifndef _REGISTER_MAP_H_
#define _REGISTER_MAP_H_

#include <reg_map_util.h>

#define RVAL(__reg)   (*((regval)__reg##ar))
#define RMOD(__reg, ...) rset(__reg, __reg##rv, __VA_ARGS__) 
//#define RMOD(__reg, ...) rset(__reg, __reg##rv, __VA_ARGS__); printf("DD:(0x%08X)=0x%08X..."#__reg"\n", __reg##ar, __reg##rv)
#define RIZS(__reg, ...) rset(__reg, 0, __VA_ARGS__)
#define RFLD(__reg, fld) (*((const volatile __reg##_T *)__reg##ar)).f.fld

#include <reg_map/reg_map_sys.h>
#include <reg_map/reg_map_uart.h>
#include <reg_map/reg_map_timer.h>
#include <reg_map/reg_map_lxto.h>
#include <reg_map/reg_map_bus_ctrl.h>
#include <reg_map/reg_map_dpi_crt.h>
#include <reg_map/reg_map_glb_pll.h>
#include <reg_map/reg_map_memcntlr.h>
#include <reg_map/reg_map_dint.h>

#endif  //_REGISTER_MAP_H_

#include <symb_define.h>

char tkinfo[] = TKINFO;

symb_idefine(build_version,
             ENV_VCS_VERSION,
             VCS_VER);

symb_idefine(build_date,
             ENV_BUILD_DATE,
             MAKE_DATE);

symb_pdefine(toolkit_info,
             ENV_BUILD_KIT,
             &tkinfo[0]);

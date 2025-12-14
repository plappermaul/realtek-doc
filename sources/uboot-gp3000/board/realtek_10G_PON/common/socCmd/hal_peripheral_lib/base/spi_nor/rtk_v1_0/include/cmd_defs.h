//#include "rle0272_defs.h"
#ifndef CMD_DEFS
#define CMD_DEFS

#define TIMES      0x00000040 //<0x0010_0000

#define MXIC_WRSR           0x01
#define MXIC_PP             0x02
#define MXIC_PPX2           0x02
#define MXIC_PPX4           0x38
#define MXIC_READX2         0xbb
#define MXIC_READX4         0xeb
#define MXIC_WRDI           0x04
#define MXIC_RDSR           0x05
#define MXIC_WREN           0x06
#define MXIC_FAST_READ      0x0b
#define MXIC_RDID           0x9f
#define MXIC_BE             0xc7
#define MXIC_SE             0xd8

#define SST_WRSR            0x01
#define SST_PP              0x02
#define SST_PPX2            0x02
#define SST_PPX4            0x02
#define SST_READX2          0x03
#define SST_READX4          0x03
#define SST_WRDI            0x04
#define SST_RDSR            0x05
#define SST_WREN            0x06
#define SST_FAST_READ       0x0b
#define SST_RDID            0x9f
#define SST_BE              0xc7
#define SST_SE              0xd8

#define WINBOND_WRSR            0x01
#define WINBOND_PP              0x02
#define WINBOND_PPX2            0x02
#define WINBOND_PPX4            0x02
#define WINBOND_READX2          0x03
#define WINBOND_READX4          0x03
#define WINBOND_WRDI            0x04
#define WINBOND_RDSR            0x05
#define WINBOND_WREN            0x06
#define WINBOND_FAST_READ       0x0b
#define WINBOND_RDID            0x9f
#define WINBOND_BE              0xc7
#define WINBOND_SE              0xd8

#endif

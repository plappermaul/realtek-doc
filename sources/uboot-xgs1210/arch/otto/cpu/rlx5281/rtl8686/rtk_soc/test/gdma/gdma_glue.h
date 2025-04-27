#ifndef __GDMA_GLUE_H__
#define __GDMA_GLUE_H__
#include "rtl_types.h"
#include <exports.h>
#define rtlglue_printf printf
//int rtlglue_open(const char *path, int flags, int mode );
//int rtlglue_read(int fd, void* buf, int nbytes);
//int rtlglue_write(int fd, void* buf, int nbytes);
//int rtlglue_close(int fd);
void *rtlglue_malloc(uint32);
void rtlglue_free(void *APTR);
#endif //#ifndef __GDMA_GLUE_H__

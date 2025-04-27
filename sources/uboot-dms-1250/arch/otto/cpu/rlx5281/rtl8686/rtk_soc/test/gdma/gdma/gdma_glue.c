#include "gdma_glue.h"
#if 0
/*
 * @ open in user space
 *  */
int rtlglue_open(const char *path, int flags, int mode )
{
        int ret;
        ret = open( path, flags, mode );
        return ret;
}

/*
 * @ write in user space
 * */
int rtlglue_write(int fd, void* buf, int nbytes)
{
        int ret;
        ret = write( fd, buf, nbytes );
        return ret;
}
/*
 *
 */
int rtlglue_read(int fd, void* buf, int nbytes)
{
	return read(fd, buf, nbytes);
}
/*
 * @ close for user space
 *  */
int rtlglue_close(int fd)
{
        int ret;
        ret = close( fd );
        return ret;
}
#endif

void rtlglue_free(void *APTR){free(APTR);}

void *rtlglue_malloc(uint32 NBYTES){return malloc(NBYTES);}


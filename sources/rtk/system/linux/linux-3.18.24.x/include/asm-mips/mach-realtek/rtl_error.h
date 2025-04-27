#ifndef __RTL_ERROR_H__
#define __RTL_ERROR_H__


/*Each product(ex:8389) should has its definition of drvErrMsg(ex:rtl8389_errno.c)*/
extern const char *drvErrMsg(int errCode);

#define PRINT_ERRMSG( code ) { rtlglue_printf("ASIC Driver Error: %s\n", drvErrMsg(code)); }
#define PRINT_ERRMSG_RETURN( code ) { rtlglue_printf("ASIC Driver Error: %s\n", (const char *)drvErrMsg(code)); return code; }

#endif /* __RTL_ERROR_H__ */


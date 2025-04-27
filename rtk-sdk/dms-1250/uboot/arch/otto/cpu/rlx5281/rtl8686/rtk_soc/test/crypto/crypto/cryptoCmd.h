
#ifndef __CRYPTOCMD_H__
#define __CRYPTOCMD_H__
#include "rtl_types.h"
int32 _crypto_mixCmdAsic(int32 round, int32 cryptoStart, int32 cryptoEnd, int32 authStart, int32 authEnd, int32 lenCryptoKeyStart, int32 lenCryptoKeyEnd, int32 lenAuthKeyStart, int32 lenAuthKeyEnd, int32  a2eoStart, int32 a2eoEnd, uint32 enumBalMask);

#endif //__CRYPTOCMD_H__

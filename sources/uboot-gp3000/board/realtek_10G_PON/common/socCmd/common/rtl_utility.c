/******************************************************************************
 *
 * Copyright(c) 2007 - 2012 Realtek Corporation. All rights reserved.
 *                                        
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/

//#include "os_support.h"
//#include "strproc.h"
#include "hal_all.h"
#include "rtl_utility.h"

/*extern void *
memset( void *s, int c, SIZE_T n );
extern void *
memcmp( const void *av, const void *bv, SIZE_T len );
extern void *
memcpy( void *s1, const void *s2, SIZE_T n );
*/

VOID 
RtlMemcpy(
    IN  VOID* dst, 
    IN  VOID* src, 
    IN  u32 sz
)
{
	memcpy(dst, src, sz);
}

u32	
RtlMemcmp(
    IN  VOID *dst, 
    IN  VOID *src, 
    IN  u32 sz
)
{
	if (!(memcmp(dst, src, sz)))
		return _TRUE;
	else
		return _FALSE;	
}

VOID 
RtlMemset(
    IN  VOID *pbuf, 
    IN  u32 c, 
    IN  u32 sz
)
{
    memset(pbuf, c, sz);
}

s8 *
RtlStrncpy(
    IN  s8 *dest, 
    IN  const s8 *src, 
    IN  SIZE_T count
){
    return ((s8 *)strncpy((char *)dest, (const char *)src, (SIZE_T) count));    
}

s8 *
RtlStrcpy(
    IN  s8 *dest, 
    IN  const s8 *src
){
    return (s8 *)(strcpy((char *)dest, (const char *)src));
}

SIZE_T
RtlStrlen(
    IN  const s8 *s
){
    return (SIZE_T)(strlen((const char *)s));
}

SIZE_T
RtlStrnlen(
    IN  const s8 *s, 
    IN  SIZE_T count
){
    return (SIZE_T)(strnlen((const char *)s, (SIZE_T) count));
}


int 
RtlStrcmp(
    IN  const s8 *cs, 
    IN  const s8 *ct

){
    return (int)(strcmp((const char *)cs, (const char *)ct));
}


int
RtlStrncmp(
    IN  const s8 *cs, 
    IN  const s8 *ct, 
    IN  SIZE_T count
){
    return (int)(strncmp((const char *)cs, (const char *)ct, (SIZE_T)count));
}

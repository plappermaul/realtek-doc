/**
 * \file     cparser_token_tbl.c
 * \brief    Parser token processing tables.
 * \version  \verbatim $Id: cparser_token_tbl.c 136 2009-05-25 05:38:25Z henry $ \endverbatim
 */
/*
 * Copyright (c) 2008, Henry Kwok
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the project nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY HENRY KWOK ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL HENRY KWOK BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#if defined(__linux__) /* Add the line for eCos, 2010-05-07 Fixed Me!!! */
#include <stdint.h>
#endif /* Add the line for eCos, 2010-05-07 Fixed Me!!! */
#include <parser/cparser.h>
#include <parser/cparser_priv.h>
#include <parser/cparser_token.h>

/**
 * \brief    An table of match functions.
 * \details  This array is indexed by CLI Parser node type. Each element
 *           contains a function pointer of a match function that checks
 *           if a token conforms to a certain node type.
 */
cparser_match_fn cparser_match_fn_tbl[CPARSER_MAX_NODE_TYPES] = {
    cparser_match_root,
    cparser_match_end,
    cparser_match_keyword, 
    cparser_match_string,
    cparser_match_uint,
    cparser_match_uint,
    cparser_match_int,
    cparser_match_int,
    cparser_match_hex,
    cparser_match_hex,
    cparser_match_float,
    cparser_match_macaddr,
    cparser_match_ipv4addr,
    cparser_match_file,
    cparser_match_port_list,
    cparser_match_ipv6addr,
    cparser_match_mask_list
};

/**
 * \brief    An table of completion functions.
 * \details  This array is indexed by CLI Parser node type. Each element
 *           contains a function pointer of a completion function that
 *           attempts to complete a token given its node type.
 */
cparser_complete_fn cparser_complete_fn_tbl[CPARSER_MAX_NODE_TYPES] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cparser_complete_file,
    NULL,
    NULL,
    NULL,
};

cparser_get_fn cparser_get_fn_tbl[CPARSER_MAX_NODE_TYPES] = {
    NULL,
    NULL,
    NULL, 
    cparser_get_string,
    cparser_get_uint,
    cparser_get_uint64,
    cparser_get_int,
    cparser_get_int64,
    cparser_get_hex,
    cparser_get_hex64,
    cparser_get_float,
    cparser_get_macaddr,
    cparser_get_ipv4addr,
    cparser_get_file,
    cparser_get_port_list,
    cparser_get_ipv6addr,
    cparser_get_mask_list
};

/**
 * \file     cparser_token.c
 * \brief    Parser token parsing and completion routines.
 * \version  \verbatim $Id: cparser_token.c 136 2009-05-25 05:38:25Z henry $ \endverbatim
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

#include <ctype.h>
#include <stdio.h>
#if defined(__linux__) /* Add the line for eCos, 2010-05-07 Fixed Me!!! */
#include <stdint.h>
#endif /* Add the line for eCos, 2010-05-07 Fixed Me!!! */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <diag_util.h>
#include <parser/cparser.h>
#include <parser/cparser_priv.h>
#include <parser/cparser_token.h>

/***********************************************************************
 * TOKEN MATCH FUNCTIONS - These functions are used by cparser_match()
 *     to check if a token matches a node type.
 ***********************************************************************/
/*
 * cparser_match_root - Token matching function for root. There is no
 *     parsing for root node. Parser should crash if this function 
 *     is ever called.
 */
cparser_result_t 
cparser_match_root (const char *token, const int token_len, 
                    cparser_node_t *node, int *is_complete)
{
    if (!(node && (CPARSER_NODE_ROOT == node->type)))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }  
    diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);

    return CPARSER_NOT_OK;
}

/*
 * cparser_match_end - Token matching function for end. There is no
 *     parsing for end node. It always returns no match.
 */
cparser_result_t 
cparser_match_end (const char *token, const int token_len, 
                   cparser_node_t *node, int *is_complete)
{
    if (!(node && (CPARSER_NODE_END == node->type)))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    } 

    return CPARSER_NOT_OK;
}

/*
 * cparser_match_keyword - Token matching function for keywords. It can 
 *     be alphanumeric string with '-' and '_'.
 */
cparser_result_t
cparser_match_keyword (const char *token, const int token_len, 
                       cparser_node_t *node, int *is_complete)
{
    int kw_len, match_len;

    if (!(token && node && (CPARSER_NODE_KEYWORD == node->type) && is_complete))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    } 

    //kw_len = strnlen(node->param, CPARSER_MAX_TOKEN_SIZE);
    kw_len = strlen(node->param);
    if (token_len > kw_len) {
	*is_complete = 0;
	return CPARSER_NOT_OK;
    }
    match_len = (kw_len < token_len ? kw_len : token_len);
    
    if (!strncmp(token, node->param, match_len)) {
	*is_complete = (match_len == kw_len);
	return CPARSER_OK;
    }

    *is_complete = 0;
    return CPARSER_NOT_OK;
}

/*
 * cparser_match_string - Token matching function for strings. There is 
 *     no restriction on strings.
 */
cparser_result_t
cparser_match_string (const char *token, const int token_len, 
                      cparser_node_t *node, int *is_complete)
{
    if (!(token && node && (CPARSER_NODE_STRING == node->type) && is_complete))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    } 
    *is_complete = 1;
    return CPARSER_OK;
}

/*
 * cparser_match_uint - Token matching function for 32-bit or 64-bit unsigned 
 *     decimals or hexadecimals. Match against /[0-9]+|0x[0-9a-fA-F]+/.
 */
cparser_result_t
cparser_match_uint (const char *token, const int token_len, 
                    cparser_node_t *node, int *is_complete)
{
    int n, is_dec = 1;
 
    if (!(token && node && is_complete && ((CPARSER_NODE_UINT == node->type) || (CPARSER_NODE_UINT64 == node->type))))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    } 

    *is_complete = 0;
    if (!(token_len > 0))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    } 

    /* The first character must be 0-9 */
    if (!isdigit(token[0])) return CPARSER_NOT_OK;
    if (1 == token_len) {
	*is_complete = 1;
	return CPARSER_OK;
    }

    /* The 2nd character (optional) must be 0-9 or 'x' */
    if ('x' == token[1]) {
	if ('0' != token[0]) {
	    return CPARSER_NOT_OK;
	}
	is_dec = 0;
    } else if (!isdigit(token[1])) {
	return CPARSER_NOT_OK;
    }
    if (2 == token_len) {
	*is_complete = is_dec;
	return CPARSER_OK;
    }

    /*
     * The rest of characters are either decmial or hex depending on
     * the first 2 characters are equal to '0x' or not 
     */
    if (is_dec) {
	for (n = 2; n < token_len; n++) {
	    if (!isdigit(token[n])) return CPARSER_NOT_OK;
	}
    } else {
	for (n = 2; n < token_len; n++) {
	    if (!isxdigit(token[n])) return CPARSER_NOT_OK;
	}
    }
    *is_complete = 1;
    return CPARSER_OK;
}

/*
 * cparser_match_int - Token matching function for 32-bit or 64-bit signed 
 *     decimals. Match against /[01]{0,1}[0-9]+/.
 */
cparser_result_t
cparser_match_int (const char *token, const int token_len, 
                   cparser_node_t *node, int *is_complete)
{
    int n;

    if (!(token && node && is_complete && ((CPARSER_NODE_INT == node->type) || (CPARSER_NODE_INT64 == node->type))))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    } 

    *is_complete = 0;
    if (!(token_len > 0))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    } 

    /* 1st digit can be 0-9,-,+ */
    if (!isdigit(token[0]) && ('-' != token[0]) && ('+' != token[0])) 
	return CPARSER_NOT_OK;
    if (1 == token_len) {
	if (isdigit(token[0])) *is_complete =  1;
	return CPARSER_OK;
    }

    /* All subsequent characters must be digits */
    for (n = 1; n < token_len; n++) {
	if (!isdigit(token[n])) return CPARSER_NOT_OK;
    }
    *is_complete = 1;
    return CPARSER_OK;
}

/*
 * cparser_match_hex - Token matching function for 32-bit or 64-bit 
 *     hexadecimals.
 */
cparser_result_t
cparser_match_hex (const char *token, const int token_len, 
                   cparser_node_t *node, int *is_complete)
{
    int n;

    if (!(token && node && is_complete && ((CPARSER_NODE_HEX == node->type) || (CPARSER_NODE_HEX64 == node->type))))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    } 

    *is_complete = 0;

    if (!(token_len > 0))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    } 
    if ('0' != token[0]) return CPARSER_NOT_OK;
    if (1 == token_len) return CPARSER_OK;
    if ('x' != token[1]) return CPARSER_NOT_OK;
    if (2 == token_len) return CPARSER_OK;
    for (n = 2; n < token_len; n++) {
	if (!isxdigit(token[n])) return CPARSER_NOT_OK;
    }
    *is_complete = 1;
    return CPARSER_OK;
}

/*
 * cparser_match_float - Token matching function for double precision floating 
 *     point value.
 */
cparser_result_t
cparser_match_float (const char *token, const int token_len, 
                     cparser_node_t *node, int *is_complete)
{
    int base = 0, has_dec_pt = 0, n;

    if (!(token && node && (CPARSER_NODE_FLOAT == node->type) && is_complete))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    } 
    *is_complete = 0;

    if (!(token_len > 0))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    } 
    
    /* Handle '+', '-' separately */
    if (('+' == token[base]) || ('-' == token[base])) {
        if (1 == token_len) {
            return CPARSER_OK;
        }
        base++;
    }

    /* Handle a first '.' */
    if ('.' == token[base]) {
        if (2 == token_len) {
            return CPARSER_OK;
        }
        has_dec_pt = 1;
    } else if (!isdigit(token[base])) {
        return CPARSER_NOT_OK;
    }

    /* Work on the rest of them */
    *is_complete = 1;
    for (n = base; n < token_len; n++) {
        if ('.' == token[n]) {
            if (has_dec_pt) {
                *is_complete = 0;
                return CPARSER_NOT_OK;
            }
            has_dec_pt = 1;
            continue;
        }
        if (!isdigit(token[n])) {
            return CPARSER_NOT_OK;
        }
    }
    return CPARSER_OK;
}

/*
 * cparser_match_macaddr - Token matching function for 48-bit MAC address.
 *     Match against /($hex{1,2}:){5,5}$hex/ where $hex = "[0-9a-fA-F]".
 */
cparser_result_t
cparser_match_macaddr (const char *token, const int token_len, 
                       cparser_node_t *node, int *is_complete)
{
    int n, num_digit = 0, num_colon = 0;

    if (!(token && node && (CPARSER_NODE_MACADDR == node->type)))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    } 
    *is_complete = 0;
    for (n = 0; n < token_len; n++) {
	if (!num_digit) {
	    if (!isxdigit(token[n])) return CPARSER_NOT_OK;
	    num_digit = 1;
	} else if (1 == num_digit) {
	    if (!isxdigit(token[n]) && (':' != token[n])) return CPARSER_NOT_OK;
	    num_digit = 2;
	    if (':' == token[n]) {
		num_colon++;
		if (num_colon > 5) return CPARSER_NOT_OK;
		num_digit = 0;
	    }
	} else {
	    if (':' != token[n]) return CPARSER_NOT_OK;
	    num_colon++;
	    if (num_colon > 5) return CPARSER_NOT_OK;
	    num_digit = 0;
	}
    }
    if ((5 == num_colon) && (0 < num_digit)) *is_complete = 1;
    return CPARSER_OK;
}

/*
 * cparser_match_ipv4addr - Token matching function for IPv4 address.
 *     Match against /([0-9]{1,3}\.){3,3}[0-9]{1,3}/
 */
cparser_result_t
cparser_match_ipv4addr (const char *token, const int token_len, 
                        cparser_node_t *node, int *is_complete)
{
    int n, num_digit = 0, num_dot = 0;

    if (!(token && node && (CPARSER_NODE_IPV4ADDR == node->type)))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    *is_complete = 0;
    for (n = 0; n < token_len; n++) {
    if (!((unsigned int)num_digit < 4))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
	if (!num_digit) {
	    if (!isdigit(token[n])) return CPARSER_NOT_OK;
	    num_digit++;
	} else if (3 == num_digit) {
	    if ('.' != token[n]) return CPARSER_NOT_OK;
	    num_dot++;
	    if (num_dot > 3) return CPARSER_NOT_OK;
	    num_digit = 0;
	} else {
	    if (!isdigit(token[n]) && ('.' != token[n])) return CPARSER_NOT_OK;
	    num_digit++;
	    if ('.' == token[n]) {
		num_dot++;
		if (num_dot > 3) return CPARSER_NOT_OK;
		num_digit = 0;
	    }
	}
    }
    if ((3 == num_dot) && (0 < num_digit)) *is_complete = 1;
    return CPARSER_OK;
}

/*
 * cparser_match_file - Token matching function for a file path. Treat
 *     this like a string.
 */
cparser_result_t
cparser_match_file (const char *token, const int token_len, 
                    cparser_node_t *node, int *is_complete)
{
    if (!(token && node && (CPARSER_NODE_FILE == node->type)))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    *is_complete = 1;
    return CPARSER_OK;
}


/*
 * cparser_match_port_list - Token matching function for port list.
 *     Match against /([0-9]{1,2}\,){0,n},([0-9]{1,2}-[0-9]{1,2})(0,n}/
 */
cparser_result_t
cparser_match_port_list (const char *token, const int token_len, 
                        cparser_node_t *node, int *is_complete)
{
    int n, hyphen_comma_flag = 0;
    
    if (!(token && node && (CPARSER_NODE_PORT_LIST == node->type)))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    
    *is_complete = 0;
    
    for (n = 0; n < token_len; n++)
    {
        if (!isdigit(token[n]))
        {
            /*check 'cpu' or 'pon' port*/
            if (token[n] == 'c')
            {
                if(n==0|| hyphen_comma_flag ==1)
                {
                    hyphen_comma_flag = 0;    
                    continue;    
                }
            }            
            else if (token[n] == 'p' )
            {
                if(n==0|| hyphen_comma_flag ==1)
                {
                    hyphen_comma_flag = 0;    
                    continue;    
                }

                if(token[n-1] != 'c')
                    return CPARSER_NOT_OK;    
                else
                {
                    hyphen_comma_flag = 0;    
                    continue;
                }

            }
            else if (token[n] == 'u' )
            {
                if(n==0|| hyphen_comma_flag ==1)
                    return CPARSER_NOT_OK;    

                if(token[n-1] != 'p')
                    return CPARSER_NOT_OK;    
                else
                {
                    hyphen_comma_flag = 0;    
                    continue;
                }
            }
            else if (token[n] == 'o' )
            {
                if(n==0|| hyphen_comma_flag ==1)
                    return CPARSER_NOT_OK;    

                if(token[n-1] != 'p')
                    return CPARSER_NOT_OK;    
                else
                {
                    hyphen_comma_flag = 0;    
                    continue;
                }
            }
            else if (token[n] == 'n' )
            {
                if(n==0|| hyphen_comma_flag ==1)
                    return CPARSER_NOT_OK;    

                if(token[n-1] != 'o')
                    return CPARSER_NOT_OK;    
                else
                {
                    hyphen_comma_flag = 0;    
                    continue;
                }
            }
            
            if (!((token[n] == ',') || (token[n] == '-')))
            {
                return CPARSER_NOT_OK;
            }
            
            if (1 == hyphen_comma_flag)
            {
                return CPARSER_NOT_OK;
            }
            hyphen_comma_flag = 1;
        }
        else
        {
            hyphen_comma_flag = 0;
        }
    }
    
    if (1 == hyphen_comma_flag)
    {
        *is_complete = 0;
    }
    else
    {
        *is_complete = 1;
    }
    return CPARSER_OK;
}

cparser_result_t
cparser_match_ipv6addr (const char *token, const int token_len, 
                        cparser_node_t *node, int *is_complete)
{
    int n, num_digit = 0, num_colon = 0, double_colon = 0;
    
    if (!(token && node && (CPARSER_NODE_IPV6ADDR == node->type)))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    
    *is_complete = 0;
    for (n = 0; n < token_len; n++)
    {
        if (!((unsigned int)num_digit < 5))
        {
            diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
            return  CPARSER_NOT_OK;
        }
    	if (!num_digit) 
    	{ /* check :: */
    	    if ((!isxdigit(token[n])) && (token[n] != ':')) 
    	    {
    	        return CPARSER_NOT_OK;
    	    }
    	    if (isxdigit(token[n]))
    	    {
    	        num_digit++;
    	    }
    	    if (token[n] == ':')
    	    {
        	    if (n >= 2)
        	    {
        	        if (token[n-2] == ':')
        	        {/* check ::: */
        	            return CPARSER_NOT_OK;
        	        }
        	    }    	        
    	        num_colon++;
    	        
    	        if (n >= 1)
    	        {
    	            if (token[n-1] == ':')
    	            {
    	                double_colon ++;
    	            }
    	        }
    	        if (double_colon >= 2)
    	        {/* check more than one :: */
        	        return CPARSER_NOT_OK;
        	    }
    	    }
    	} 
    	else if (4 == num_digit)
    	{
    	    if (':' != token[n])
    	    {
    	        return CPARSER_NOT_OK;
    	    }
    	    num_colon++;
    	    if (num_colon > 7)
    	    {
    	        return CPARSER_NOT_OK;
    	    }
    	    num_digit = 0;
    	} 
    	else
    	{
    	    if (!isxdigit(token[n]) && (':' != token[n]))
    	    {
    	        return CPARSER_NOT_OK;
    	    }
    	    if (isxdigit(token[n]))
    	    {
    	        num_digit++;
    	    }
    	    if (':' == token[n])
    	    {
    		    num_colon++;
    		    if (num_colon > 7)
    		    {
    		        return CPARSER_NOT_OK;
    		    }
    		    num_digit = 0;
    	    }
    	}
    }
    if ((((num_colon <= 7) && (double_colon == 1)) || (num_colon == 7)) && (0 < num_digit)) 
    {
        *is_complete = 1;
    }
    return CPARSER_OK;
}
/*
 * cparser_match_port_list - Token matching function for port list.
 *     Match against /([0-9]{1,2}\,){0,n},([0-9]{1,2}-[0-9]{1,2})(0,n}/
 */
cparser_result_t
cparser_match_mask_list (const char *token, const int token_len,
                        cparser_node_t *node, int *is_complete)
{
    int n, hyphen_comma_flag = 0;

    if (!(token && node && (CPARSER_NODE_MASK_LIST == node->type)))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }

    *is_complete = 0;

    for (n = 0; n < token_len; n++)
    {
        if (!isdigit(token[n]))
        {
            if (!((token[n] == ',') || (token[n] == '-')))
            {
                return CPARSER_NOT_OK;
            }

            if (1 == hyphen_comma_flag)
            {
                return CPARSER_NOT_OK;
            }
            hyphen_comma_flag = 1;
        }
        else
        {
            hyphen_comma_flag = 0;
        }
    }

    if (1 == hyphen_comma_flag)
    {
        *is_complete = 0;
    }
    else
    {
        *is_complete = 1;
    }
    return CPARSER_OK;
}

/***********************************************************************
 * TOKEN COMPLETE FUNCTIONS - These functions are used by 
 *     cparser_complete() to provide context-sensitive help on 
 *     parameters.
 ***********************************************************************/
/*
 * cparser_complete_file - Token complete function for a file path.
 */
cparser_result_t
cparser_complete_file (const char *token, const int token_len)
{
    if (!(token && token_len))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    /* hack alert - fill in the real thing later */
    return CPARSER_OK;
}

/***********************************************************************
 * TOKEN GET FUNCTIONS - These functions are used by glue functions
 *     to extract the parameters and call the action function.
 ***********************************************************************/
/*
 * cparser_get_string - Token get function for a string.
 */
cparser_result_t
cparser_get_string (const cparser_token_t *token, void *value)
{
    char **val = (char **)value;

    if (!(token && val))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    if (!token->token_len) {
        *val = NULL;
        return CPARSER_NOT_OK; /* optional argument wasn't provided */
    }
    *val = (char *)token->buf;
    return CPARSER_OK;
}

/*
 * cparser_get_uint_internal - Token get function for 32-bit unsigned integer.
 */
static cparser_result_t
cparser_get_uint_internal (const char *token, const int token_len, void *value)
{
    uint32_t new = 0, old, d = 0, n;
    uint32_t *val = (uint32_t *)value;
    int i;
    if (!(token && val))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    *val = old = 0;
    for (n = 0; n < token_len; n++) {
	if (('0' <= token[n]) && ('9' >= token[n])) {
	    d = token[n] - '0';
	} else {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);

        for(i=0;i< token_len ;i++)
            diag_util_printf("token[%d]:%c\n",i,token[i]);
        return  CPARSER_NOT_OK;
	}
	new = (old * 10) + d;
	if (((new - d) / 10) != old) return CPARSER_ERR_INVALID_PARAMS;
	old = new;
    }
    *val = new;
    return CPARSER_OK;
}

/*
 * cparser_get_uint64_internal - Token get function for 64-bit unsigned integer.
 */
static cparser_result_t
cparser_get_uint64_internal (const char *token, const int token_len, void *value)
{
    uint64_t new = 0, old, d = 0, n;
    uint64_t *val = (uint64_t *)value;

    if (!(token && val))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    *val = old = 0;
    for (n = 0; n < token_len; n++) {
	if (('0' <= token[n]) && ('9' >= token[n])) {
	    d = token[n] - '0';
	} else {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
	}
	new = (old * 10) + d;
	if (((new - d) / 10) != old) return CPARSER_ERR_INVALID_PARAMS;
	old = new;
    }
    *val = new;
    return CPARSER_OK;
}

/*
 * cparser_get_uint - Token get function for 32-bit unsigned integer.
 */
cparser_result_t
cparser_get_uint (const cparser_token_t *token, void *value)
{
    if (!(token && value))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    
    if (!token->token_len) {
        *((uint32_t *)value) = 0;
        return CPARSER_NOT_OK; /* optional argument wasn't provided */
    }
    if ((1 < token->token_len) && ('x' == token->buf[1])) {
	/* Hexadecmial format use cparser_get_hex() */
	return cparser_get_hex(token, value);
    }
    return cparser_get_uint_internal(token->buf, token->token_len, value);
}

/*
 * cparser_get_uint64 - Token get function for 64-bit unsigned integer.
 */
cparser_result_t
cparser_get_uint64 (const cparser_token_t *token, void *value)
{
    if (!(token && value))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }

    if (!token->token_len) {
        *((uint64_t *)value) = 0;
        return CPARSER_NOT_OK; /* optional argument wasn't provided */
    }
    if ((1 < token->token_len) && ('x' == token->buf[1])) {
	/* Hexadecmial format use cparser_get_hex64() */
	return cparser_get_hex64(token, value);
    }
    return cparser_get_uint64_internal(token->buf, token->token_len, value);
}

/*
 * cparser_get_int - Token get function for 32-bit integer.
 */
cparser_result_t
cparser_get_int (const cparser_token_t *token, void *value)
{
    int32_t sign = +1, *val = (int32_t *)value;
    uint32_t tmp, init_pos = 0;

    if (!(token && val))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    *val = 0;
    if (!token->token_len) {
        return CPARSER_NOT_OK; /* optional argument wasn't provided */
    }

    if (!(token->token_len > 0))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    if ('-' == token->buf[0]) {
	sign = -1;
	init_pos = 1;
    }
    if ('+' == token->buf[0]) {
	init_pos = 1;
    }
    if (CPARSER_OK != cparser_get_uint_internal(&token->buf[init_pos], 
                                                token->token_len - init_pos, 
                                                &tmp)) {
	return CPARSER_NOT_OK;
    }
    if (+1 == sign) {
    	if (tmp > 0x7fffffff) return CPARSER_NOT_OK;
        	*val = (int32_t)tmp;
    } else {
        #if 0 /*remove dead code*/
        if (!(-1 == sign))
        {
            diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
            return  CPARSER_NOT_OK;
        }
        #endif    	
    	if (tmp > 0x80000000) return CPARSER_NOT_OK;
    	*val = -((int32_t)tmp);
    }
    return CPARSER_OK;
}

/*
 * cparser_get_int64 - Token get function for 64-bit integer.
 */
cparser_result_t
cparser_get_int64 (const cparser_token_t *token, void *value)
{
    int64_t sign = +1, *val = (int64_t *)value;
    uint64_t tmp, init_pos = 0;

    if (!(token && val))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    *val = 0;
    if (!token->token_len) {
        return CPARSER_NOT_OK; /* optional argument wasn't provided */
    }
    if (!(token->token_len > 0))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    if ('-' == token->buf[0]) {
	sign = -1;
	init_pos = 1;
    }
    if ('+' == token->buf[0]) {
	init_pos = 1;
    }
    if (CPARSER_OK != cparser_get_uint64_internal(&token->buf[init_pos], 
                                                  token->token_len - init_pos, 
                                                  &tmp)) {
	return CPARSER_NOT_OK;
    }
    if (+1 == sign) {
	if (tmp > 0x7fffffffffffffffULL) return CPARSER_NOT_OK;
	*val = (int64_t)tmp;
    } else {
        #if 0 /*remove dead code*/
        if (!(-1 == sign))
        {
            diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
            return  CPARSER_NOT_OK;
        }
        #endif
    	if (tmp > 0x8000000000000000ULL) return CPARSER_NOT_OK;
    	*val = -((int64_t)tmp);
    }
    return CPARSER_OK;
}

/*
 * cparser_get_hex - Token get function for 32-bit hexadecimal.
 */
cparser_result_t
cparser_get_hex (const cparser_token_t *token, void *value)
{
    int n;
    uint32_t new = 0, old, d = 0, *val = (uint32_t *)value;

    if (!(token && val))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }    
    if (!token->token_len) {
        *val = 0;
        return CPARSER_NOT_OK; /* optional argument wasn't provided */
    }

    if (!((token->token_len > 2) && ('0' == token->buf[0]) && ('x' == token->buf[1])))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    *val = old = 0;
    for (n = 2; n < token->token_len; n++) {
	if (('0' <= token->buf[n]) && ('9' >= token->buf[n])) {
	    d = token->buf[n] - '0';
	} else if (('a' <= token->buf[n]) && ('f' >= token->buf[n])) {
	    d = token->buf[n] - 'a' + 10;
	} else if (('A' <= token->buf[n]) && ('F' >= token->buf[n])) {
	    d = token->buf[n] - 'A' + 10;
	} else {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
	}
	new = (old << 4) + d;
	if (((new - d) >> 4) != old) return CPARSER_NOT_OK;
	old = new;
    }
    *val = new;
    return CPARSER_OK;
}

/*
 * cparser_get_hex64 - Token get function for 64-bit hexadecimal.
 */
cparser_result_t
cparser_get_hex64 (const cparser_token_t *token, void *value)
{
    int n;
    uint64_t new = 0, old, d = 0, *val = (uint64_t *)value;

    if (!(token && val))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }     
    if (!token->token_len) {
        *val = 0;
        return CPARSER_NOT_OK; /* optional argument wasn't provided */
    }
    if (!((token->token_len > 2) && ('0' == token->buf[0]) && ('x' == token->buf[1])))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }  
    *val = old = 0;
    for (n = 2; n < token->token_len; n++) {
	if (('0' <= token->buf[n]) && ('9' >= token->buf[n])) {
	    d = token->buf[n] - '0';
	} else if (('a' <= token->buf[n]) && ('f' >= token->buf[n])) {
	    d = token->buf[n] - 'a' + 10;
	} else if (('A' <= token->buf[n]) && ('F' >= token->buf[n])) {
	    d = token->buf[n] - 'A' + 10;
	} else {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
	}
	new = (old << 4) + d;
	if (((new - d) >> 4) != old) return CPARSER_NOT_OK;
	old = new;
    }
    *val = new;
    return CPARSER_OK;
}

/*
 * cparser_get_float - Token get function for 64-bit floating point value.
 */
cparser_result_t
cparser_get_float (const cparser_token_t *token, void *value)
{
    double *val = (double *)value;

    if (!(token && val))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    } 
    if (!token->token_len) {
        return CPARSER_NOT_OK; /* optional argument wasn't provided */
    }
    if (1 != sscanf(token->buf, "%lf", val)) {
        *val = 0.0;
        return CPARSER_NOT_OK;
    }
    return CPARSER_OK;
}

/*
 * cparser_get_macaddr - Token get function for MAC address.
 */
cparser_result_t
cparser_get_macaddr (const cparser_token_t *token, void *value)
{
    unsigned long a, b, c, d, e, f;
    cparser_macaddr_t *val = (cparser_macaddr_t *)value;

    if (!(token && val))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    } 
    if (!token->token_len) {
        memset(val, 0, sizeof(*val));
        return CPARSER_NOT_OK; /* optional argument wasn't provided */
    }
    if ((6 != sscanf(token->buf, "%lx:%lx:%lx:%lx:%lx:%lx", 
		     &a, &b, &c, &d, &e, &f)) ||
	(a > 255) || (b > 255) || (c > 255) || (d > 255) || (e > 255) || 
	(f > 255)) {
	val->octet[0] = val->octet[1] = val->octet[2] = 
            val->octet[3] = val->octet[4] = val->octet[5] = 0;
	return CPARSER_NOT_OK;
    }
    val->octet[0] = (uint8_t)a;
    val->octet[1] = (uint8_t)b;
    val->octet[2] = (uint8_t)c;
    val->octet[3] = (uint8_t)d;
    val->octet[4] = (uint8_t)e;
    val->octet[5] = (uint8_t)f;

    return CPARSER_OK;
}

/*
 * cparser_get_ipv4addr - Token get function for IPv4 address.
 */
cparser_result_t
cparser_get_ipv4addr (const cparser_token_t *token, void *value)
{
    unsigned long a, b, c, d;
    uint32_t *val = (uint32_t *)value;

    if (!(token && val))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    } 
    if (!token->token_len) {
        *val = 0;
        return CPARSER_NOT_OK; /* optional argument wasn't provided */
    }
    if ((4 != sscanf(token->buf, "%lu.%lu.%lu.%lu", &a, &b, &c, &d)) ||
	(a > 255) || (b > 255) || (c > 255) || (d > 255)) {
        *val = 0;
	return CPARSER_NOT_OK;
    }
    *val = ((uint32_t)a) << 24;
    *val |= ((uint32_t)b) << 16;
    *val |= ((uint32_t)c) << 8;
    *val |= ((uint32_t)d);
    
    return CPARSER_OK;
}

/*
 * cparser_get_file - Token get function for file path.
 */
cparser_result_t
cparser_get_file (const cparser_token_t *token, void *value)
{
    struct stat stat_buf;
    char **val = (char **)value;

    if (!(token && val))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    } 
    *val = NULL;
    if (!token->token_len) {
        return CPARSER_NOT_OK; /* optional argument wasn't provided */
    }

#if defined(__linux__) /* Add the line for eCos, 2010-05-07 Fixed Me!!! */
    if (stat(token->buf, &stat_buf) || !(stat_buf.st_mode & S_IFREG)) {
#else /* Add the line for eCos, 2010-05-07 Fixed Me!!! */
    if (stat(token->buf, &stat_buf)) {  /* Add the line for eCos, 2010-05-07 Fixed Me!!! */
#endif /* Add the line for eCos, 2010-05-07 Fixed Me!!! */
	return CPARSER_NOT_OK;
    }
    *val = (char *)token->buf;
    return CPARSER_OK;
}

/*
 * cparser_get_port_list - Token get function for port list.
 */
cparser_result_t
cparser_get_port_list (const cparser_token_t *token, void *value)
{
    char **val = (char **)value;

    if (!(token && val))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    if (!token->token_len) {
        *val = NULL;
        return CPARSER_NOT_OK; /* optional argument wasn't provided */
    }
    *val = (char *)token->buf;
    return CPARSER_OK;
}
/*
 * cparser_get_mask_list - Token get function for mask list.
 */
cparser_result_t
cparser_get_mask_list (const cparser_token_t *token, void *value)
{
    char **val = (char **)value;

    if (!(token && val))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    if (!token->token_len) {
        *val = NULL;
        return CPARSER_NOT_OK; /* optional argument wasn't provided */
    }
    *val = (char *)token->buf;
    return CPARSER_OK;
}


/*
 * cparser_get_ipv6addr - Token get function for port list.
 */
cparser_result_t
cparser_get_ipv6addr (const cparser_token_t *token, void *value)
{
    char **val = (char **)value;
    if (!(token && val))
    {
        diag_util_printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    if (!token->token_len) {
        *val = NULL;
        return CPARSER_NOT_OK; /* optional argument wasn't provided */
    }
    *val = (char *)token->buf;
    return CPARSER_OK;
}

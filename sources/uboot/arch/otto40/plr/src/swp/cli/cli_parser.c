//#include <string.h>
#include <cli/cli_parser.h>

#ifndef SECTION_CLI_PARSER
    #define SECTION_CLI_PARSER
#endif


extern const cli_cmd_node_t start_of_cli_node SECTION_CMD_NODE;
extern const cli_cmd_node_t end_of_cli_node SECTION_CMD_NODE;
extern const cli_cmd_mp_node_t start_of_cli_mp_node SECTION_CMD_MP_NODE;
extern const cli_cmd_mp_node_t end_of_cli_mp_node SECTION_CMD_MP_NODE;
extern const cli_cmd_tail_t start_of_cli_tail SECTION_CMD_TAIL;
extern const cli_cmd_tail_t end_of_cli_tail SECTION_CMD_TAIL;

SECTION_CLI_PARSER int
cli_strcmp(const char *a, const char *b, char end_chr) {
    while (*a==*b) {
        if ((*a=='\0')||(*a==end_chr)) return 0;
        ++a; ++b;
    }
    if (((*a=='\0')||(*a==end_chr))&&((*b=='\0')||(*b==end_chr))) return 0;
    return *a - *b;
}

SECTION_CLI_PARSER const cli_cmd_node_t *
cli_match_node_pure_name(const char *name) {
    const cli_cmd_node_t *b1=&start_of_cli_node, *e1=&end_of_cli_node;
    while (b1!=e1) {
        if (cli_strcmp(name, b1->name, ' ')==0) return b1;
        ++b1;
    }
    return VZERO;
}

SECTION_CLI_PARSER const cli_cmd_node_t *
cli_match_node_name(const cli_cmd_node_t *parent, const char *name) {
    const cli_cmd_node_t *b1=&start_of_cli_node, *e1=&end_of_cli_node;
    while (b1!=e1) {
        if ((parent==b1->parent) && (cli_strcmp(name, b1->name, ' ')==0)) return b1;
        ++b1;
    }
    const cli_cmd_mp_node_t *b2=&start_of_cli_mp_node, *e2=&end_of_cli_mp_node;
    while (b2!=e2) {
        if (parent==b2->oparent) {
            const cli_cmd_node_t *me=b2->me;
            if (cli_strcmp(name, me->name, ' ')==0) return me;
        }
        ++b2;
    }
    return VZERO;
}

SECTION_CLI_PARSER const cli_cmd_tail_t *
cli_match_tail_name(const cli_cmd_node_t *parent, const char *name) {
    const cli_cmd_tail_t *b=&start_of_cli_tail, *e=&end_of_cli_tail;
    while (b!=e) {
        if ((parent==b->parent) && (cli_strcmp(name, b->name, ' ')==0)) return b;
        ++b;
    }
    return VZERO;
}

SECTION_CLI_PARSER static const char *
_cli_first_token(const char **cmd) {
    const char *s=*cmd;
    const char *r=VZERO;
    
    while ((*s==' ')&&(*s!='\0')) s++;
    if (*s!='\0') {
        r=s;
        while ((*s!=' ')&&(*s!='\0')) s++;
    }
    *cmd=s;
    return r;
}

#define DONT_MATCH_ANY_MORE ((void*)0xffffffff)
SECTION_CLI_PARSER static cli_cmd_ret_t
_cli_exec(const char *cmd) {
    const char *p=cmd;
    cli_cmd_func_t *cmd_func=VZERO;
    const void *cmd_user=VZERO;
    const cli_cmd_node_t *cmd_node;
    const cli_cmd_tail_t *cmd_tail;
    const cli_cmd_node_t *parent=VZERO;
    const char *argv[MAX_ARGV+1];
    u32_t wc=0;
    
    while (1) {
        const char *tkn=_cli_first_token(&p);
        if (tkn==VZERO) {
            if (wc==0) return CCR_NO_CMD;
            if (cmd_func==VZERO) return CCR_UNKNOWN_CMD;
            argv[wc]=VZERO;
            return (*cmd_func)(cmd_user, wc, argv);
        }
        if (DONT_MATCH_ANY_MORE!=parent) {
            if ((cmd_node=cli_match_node_name(parent, tkn))==VZERO) {
                if (wc==0) return CCR_UNKNOWN_CMD;
                cmd_tail=cli_match_tail_name(VZERO, tkn);
                if (cmd_tail!=VZERO) cmd_user=cmd_tail->user;
                parent=DONT_MATCH_ANY_MORE;
            } else {
                if (cmd_node->func!=VZERO) cmd_func=cmd_node->func;
                parent=cmd_node;
            }
        }
        if (wc<MAX_ARGV) {
            if (*p!='\0') {
                *(char *)p='\0';
                ++p;
            }
            argv[wc]=tkn;
            ++wc;
        }
    }
}

SECTION_CLI_PARSER static int
_cli_is_prefix(const char *prefix, const char *s) {
    if (prefix==VZERO) return 1;
    while (*prefix!='\0') {
        if (*(prefix++)!=*(s++)) return 0;
    }
    return 1;
}
u32_t SECTION_PARAMETERS tty_width=80;
SECTION_CLI_PARSER static u32_t
_print_suggest_item(const char *s, u32_t current_pos) {
    u32_t l=strlen(s);
    u32_t space=ITEM_UNIT_LEN-(l%ITEM_UNIT_LEN);
    current_pos += l+space;
    if (current_pos>=tty_width) {
        _putcn('\n');
        current_pos-=tty_width;
    }
    puts(s);
    while ((space--)>0) _putc(' ');
    return current_pos;
}

// return number of matched item
SECTION_CLI_PARSER static int
_cli_sort_suggest_text(const cli_cmd_node_t *suggest_parent, 
    const char *prefix, int to_print, char *to_finish) {
    u32_t current_pos=0;
    u32_t sg=0;
    {
        const cli_cmd_node_t *b=&start_of_cli_node, *e=&end_of_cli_node;
        while (b!=e) {
            if ((suggest_parent==b->parent) && _cli_is_prefix(prefix, b->name)) {
                if (to_print) current_pos=_print_suggest_item(b->name, current_pos);
                if (to_finish!=VZERO) {
                    strcpy(to_finish, b->name);
                    return 1;
                }
                ++sg;
            }
            ++b;
        }
    }
    {
        const cli_cmd_mp_node_t *b=&start_of_cli_mp_node, *e=&end_of_cli_mp_node;
        while (b!=e) {
            const cli_cmd_node_t *me=b->me;
            if ((suggest_parent==b->oparent) && _cli_is_prefix(prefix, me->name)) {
                if (to_print) current_pos=_print_suggest_item(me->name, current_pos);
                if (to_finish!=VZERO) {
                    strcpy(to_finish, me->name);
                    return 1;
                }
                ++sg;
            }
            ++b;
        }
    }
    {
        const cli_cmd_tail_t *b=&start_of_cli_tail, *e=&end_of_cli_tail;
        while (b!=e) {
            if ((suggest_parent==b->parent) && _cli_is_prefix(prefix, b->name)) {
                if (to_print) current_pos=_print_suggest_item(b->name, current_pos);
                if (to_finish!=VZERO) {
                    strcpy(to_finish, b->name);
                    return 1;
                }
                ++sg;
            }
            ++b;
        }
    }
    return sg;
}
SECTION_CLI_PARSER static int
_cli_suggest_text(char *cmd) {
    const char *p=cmd;
    const cli_cmd_node_t *cmd_node;
    const cli_cmd_node_t *parent=VZERO;
    const cli_cmd_node_t *suggest_parent=VZERO;
    const char *prefix=VZERO;
    while (1) {
        char *q=(char*)p;
        const char *tkn=_cli_first_token(&p);
        if (tkn==VZERO) {
            *(q++)=' ';
            *q='\0';
            prefix=q;
            break;
        }
        if ((cmd_node=cli_match_node_name(parent, tkn))==VZERO) {
            prefix=tkn;
            break;
        }
        parent=cmd_node;
    }
    
    suggest_parent=parent;
    u32_t sg=_cli_sort_suggest_text(suggest_parent, prefix, 0, VZERO);
    if (sg==0) return 0;
    if (sg==1) {
        _cli_sort_suggest_text(suggest_parent, prefix, 0, (char*)prefix);
        puts("        \r]");
        puts(cmd);

        return 1;
    }
    _putcn('\n');
    _cli_sort_suggest_text(suggest_parent, prefix, 1, VZERO);
    puts("\n]");
    puts(cmd);
    
    return sg;
}

SECTION_CLI_PARSER cli_cmd_ret_t 
cli_main_loop(void) {
    char cmd_buf[CMD_INP_BUF_SZ+4], *p, cmd_backup[CMD_INP_BUF_SZ];
    cmd_backup[0]='\0';
    
    p=cmd_buf;
    while(1) {
        char chr=_getc();
        if (chr==0) continue;
            
        // echo char
        if ((chr>=32)&&(chr<='z')) {
        	_putc(chr);
        } else if (chr==CHAR_ESC) {
            puts("\n]");
            p=cmd_buf;
            continue;
        } else if (chr==CHAR_BACKSPACE) {
            if (p!=cmd_buf) {
                _putc(CHAR_BACKSPACE);
                _putc(' ');
                _putc(CHAR_BACKSPACE);
                --p;
            }
            continue;
        } else if (chr==CHAR_TAB) {
            // looking for suggestions
            *p='\0';
            _cli_suggest_text(cmd_buf);
            while (*p!='\0') ++p;
            continue;
        }
        
        if (chr==CHAR_ENTER) {
            *p='\0'; 
            inline_wmemcpy((void*)cmd_backup, (const void*)cmd_buf, CMD_INP_BUF_SZ);
        } else if (chr==CHAR_DEL) {
            // repeat
            inline_wmemcpy((void*)cmd_buf, (const void*)cmd_backup, CMD_INP_BUF_SZ);
            if (p!=cmd_buf) _putc(';');
            puts(cmd_buf);
            chr=CHAR_ENTER;
        }
        if (chr==CHAR_ENTER) {
            // execute command
            p=cmd_buf;
            // command echo            
            #if 0
            puts("exec:");
            puts(cmd_buf);
            char *x=cmd_buf, cc;
            while (1) {
                if ((cc=*(x++))=='\0') break;
                printf("%c(%02d)", cc, (u32_t)cc);
            }
            #endif
            _putcn('\n');

            // command parsing
            if (cmd_buf[0]=='.') continue;
            cli_cmd_ret_t r=_cli_exec(cmd_buf);
            switch (r) {
                case CCR_EXIT: 
                    return CCR_EXIT;
                case CCR_UNKNOWN_CMD:
                    puts("unknown command\n]");
                    break;
                case CCR_UNSUPPORTED_PARA:
                    puts("unsupported parameters\n]");
                    break;
                case CCR_INCOMPLETE_CMD:
                    puts("incomplete comand\n]");
                    break;
                default:
                case CCR_OK: 
                    puts("]");
                    break;
            }
            cmd_buf[0]='\0';
            continue;
        }
        if ((chr<32)||(chr>'z')) {
            printf("(%02x)", (unsigned char)chr);
            continue;
        }
        if ((p-cmd_buf)<CMD_INP_BUF_SZ) *(p++)=chr;
    }
}


 

/*
cli_top_node(set, VZERO);
cli_top_node(get, VZERO);
cli_add_node(ddr, set, VZERO);
cli_add_parent(ddr, get);
cli_add_tail(mcs1, ddr, VZERO);
*/

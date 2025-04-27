#!/usr/bin/env python
# $Id: mk_parser.py 139 2009-05-25 09:55:58Z henry $

# Copyright (c) 2008-2009, Henry Kwok
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the project nor the names of its contributors
#       may be used to endorse or promote products derived from this software
#       without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY HENRY KWOK ''AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL HENRY KWOK BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import re, sys, glob, os, copy
import datetime
from shutil import move
from os import remove

print_tree = False
debug = False
end_node = None
#RTK integrate eol node
integrate_eol = True
#RTK end
token_node_str = '_TN'

#RTK-SQUT ++
rtkSdkQaUnitTest_enable = False
squt_op_idx = 1
#RTK-SQUT --


#RTK check duplicate include file
included_file_list = ''


##
# \brief     Debug printf
# \details   Print out a object preceeded by a string header if global
#            'debug' is True.
#
# \param     hdr A header string
# \param     s   Data to be displayed
def DBG(hdr, s):
    global debug
    if debug:
        sys.stdout.write(hdr)
        print(s)


## A class of parse tree node
class Node:
    TOKENS = [ 'ROOT', 'END', 'KEYWORD', 'STRING', 'UINT', 'UINT64', 'INT',
               'INT64', 'HEX', 'HEX64', 'FLOAT', 'MACADDR', 'IPV4ADDR',
               'FILE', 'PORT_LIST', 'PORT_TRUNK_LIST', 'DEV_PORT', 'DEV_PORT_LIST', 'IPV6ADDR', 'MASK_LIST', 'PECU' ]
    TYPES = { 'ROOT'       : None,
              'END'        : None,
              'KEYWORD'    : None,
              'STRING'     : 'char *',
              'UINT'       : 'uint32_t ',
              'UINT64'     : 'uint64_t ',
              'INT'        : 'int32_t ',
              'INT64'      : 'int64_t ',
              'HEX'        : 'uint32_t ',
              'HEX64'      : 'uint64_t ',
              'FLOAT'      : 'double ',
              'MACADDR'    : 'cparser_macaddr_t ',
              'IPV4ADDR'   : 'uint32_t ',
              'FILE'       : 'char *',
              'PORT_LIST'  : 'char *',
              'PORT_TRUNK_LIST'  : 'char *',
              'DEV_PORT'  : 'char *',
              'DEV_PORT_LIST'  : 'char *',
              'IPV6ADDR'   : 'char *',
              'MASK_LIST'  : 'char *',
              'PECU': 'char *'
              }

    ## Constructor.
    def __init__(self, node_type, param, desc, flags):
        '''
        Constructor.
        '''

        self.type = node_type
        self.param = param
        if desc:
            self.desc = '    "%s",\n' % desc
        else:
            self.desc = '    NULL,\n'
        self.flags = flags
        self.children = []

        # Cannot fill these out until we insert the node to the tree
        self.parent = None
        self.depth = 0
        self.path = ''
        self.next = None

        # Set flag to record whether this node's information is written to file or not.
        self.write_c_struct = False
        self.write_glue_fn = False
        self.write_action_fn = False

        return

    ##
    # \brief     Add a child node to a tree node.
    #
    # \param     child A Node object that is added to this node as its child.
    def add_child(self, child_data):
        for c in self.children:
            if (c.type == child_data.type) and (c.param == child_data.param):
                # The node already exists. Re-use the existing node.
                return c
        ## RTK add start
        ## To fix tree compile error.
        if 'END' == child_data.type:
            child = child_data
        else:
            child = copy.copy(child_data)
        ## RTK add end

        # Fill out some information that are tree structure dependent.
        # These information are actually embedded in the tree already
        # However, we compute them and cache them to reduce the
        # processing time later. This approach increases the storage
        # requirement but decreases the processing time.
        #RTK integrate eol node
        if integrate_eol:
            if child.depth < (self.depth + 1):
                child.parent = self
                child.depth = self.depth + 1
        else:
            child.parent = self
            child.depth = self.depth + 1
        #RTK end
        if child.is_param() or child.is_keyword():
            child.path = self.path + '_' + child.param.replace('-','_')
        elif 'END' == child.type:
            child.path = self.path + '_eol'
        elif 'ROOT' == child.type:
            # If we are adding a child ROOT node, the parent ('self' here)
            # must be an END node. For END node, we insert a string
            # 'cparser_glue' to the front of self.path. So, we must
            # remove it first before creating the submode root path
            child.path = self.param.replace('cparser_glue', '') + '_root'

        #print 'child.flags=%s' % child.flags
        #print 'len(self.children)=%d' % len(self.children)

        if len(self.children) > 0:
            self.children[-1].next = child

        # Insert the node into the children list
        self.children.append(child)
        return child

    ## Return True if it is a parameter node; False otherwise.
    def is_param(self):
        if (('ROOT' == self.type) or ('END' == self.type) or ('KEYWORD' == self.type)):
            return False
        return True

    ## Return True if it is a keyword node; False otherwise.
    def is_keyword(self):
        return ('KEYWORD' == self.type)

    ## Return True if it is an optional keyword / parameter; False otherwise.
    def is_optional(self):
        return (0 < (self.flags.count('CPARSER_NODE_FLAGS_OPT_END') +
                     self.flags.count('CPARSER_NODE_FLAGS_OPT_PARTIAL')))

    ## Display a summary of the node.
    def display(self, fout=sys.stdout):
        if 'ROOT' == self.type:
            fout.write('<ROOT')
        elif 'KEYWORD' == self.type:
            fout.write('<KEYWORD:%s' % self.param)
        elif 'END' == self.type:
            fout.write('<END')
        else:
            fout.write('<%s:%s' % (self.type, self.param))
        if len(self.flags) > 0:
            # Create a local copy and strip all CPARSER_NODE_FLAGS_ prefix.
            tmp_list = self.flags[:]
            for n in range(0,len(tmp_list)):
                tmp_list[n] = tmp_list[n].replace('CPARSER_NODE_FLAGS_','')
            fout.write(tmp_list.__repr__())
        fout.write('> ')

    ##
    # \brief     Walk the tree
    #
    # \param     fn     Function to be called per node.
    # \param     mode   'pre-order', 'func', 'post-order'.
    # \param     cookie An opaque object to be passed into 'fn'.
    #
    # \return    Return the number of callback made.
    def walk(self, fn, mode, cookie):
        last = len(self.children)
        count = 0
        if (mode == 'pre-order') or (mode == 'func'):
            if ((mode == 'pre-order') or
                ((self.type == 'END') and
                 (not self.flags.count('CPARSER_NODE_FLAGS_OPT_PARTIAL')))):
                fn(self, cookie)
                count += 1
            for n in range(last):
                count += self.children[n].walk(fn, mode, cookie)
        elif 'post-order' == mode:
            for n in range(last):
                count += self.children[last-1-n].walk(fn, mode, cookie)
            fn(self, cookie)
            count += 1
        else:
            raise ValueError, 'Unknown walk mode: %s' % mode
        return count

    ## Generate the C structure name.
    def c_struct(self):
        if self.parent == None:
            msg = 'cparser_node_t cparser_root = {\n'
        else:
            ## RTK modify start
            ##  Fix duplicate cparser_node_t issue when token has the same name as another command's keyword.
            ##  E.g.:
            ##      xxx set <MACADDR:mac>
            ##      xxx set mac <MACADDR:mac_addr> ip <IPV4ADDR:ip>
            ##  there will have duplicate "cparser_node_t cparser_node_xxx_set_mac".
            ## Original code:
            #msg = 'cparser_node_t cparser_node%s = {\n' % self.path
            ## New code:
            append_str = ''
            if self.is_param(): append_str = token_node_str
            msg = 'cparser_node_t cparser_node%s%s = {\n' % (self.path, append_str)
            ## RTK modify end
        #----------------------------------------------------------------------------------
        # type
        msg += '    CPARSER_NODE_%s,\n' % self.type
        # flags
        if len(self.flags) == 0:
            msg += '    0,\n'
        else:
            msg += '    ' + ' | '.join(self.flags) + ',\n'
        # param
        if 'ROOT' == self.type:  msg += '    NULL,\n'
        elif 'END' == self.type: msg += '    %s,\n' % self.param
        elif 'KEYWORD' == self.type: msg += '    "%s",\n' % self.param
        else: msg += '    "<%s:%s>",\n' % (self.type, self.param)
        # desc
        msg += self.desc
        # sibling
        if self.next:
            #if (self.flags.count('CPARSER_NODE_FLAGS_SEL_END') > 0):
            #    msg += '    NULL,\n'
            #else:
            ## RTK modify start
            ##  Fix duplicate cparser_node_t issue when token has the same name as another command's keyword.
            ## Original code:
            #msg += '    &cparser_node%s,\n' % self.next.path
            ## New code:
            append_str = ''
            if self.next.is_param(): append_str = token_node_str
            msg += '    &cparser_node%s%s,\n' % (self.next.path, append_str)
            ## RTK modify end
        else:
            msg += '    NULL,\n'
        # children
        if len(self.children) > 0:
            ## RTK modify start
            ##  Fix duplicate cparser_node_t issue when token has the same name as another command's keyword.
            ## Original code:
            #msg += '    &cparser_node%s\n' % self.children[0].path
            ## New code:
            append_str = ''
            if self.children[0].is_param(): append_str = token_node_str
            msg += '    &cparser_node%s%s\n' % (self.children[0].path, append_str)
            ## RTK modify end
        else:
            msg += '    NULL\n'
        msg += '};\n'
        return msg

    ## Return a list of Node objects that forms a path from root to this node.
    def walk_up_to_root(self):
        assert self.type == 'END'
        p = []
        cur_node = self
        while cur_node.parent:
            p.insert(0, cur_node)
            cur_node = cur_node.parent
            if cur_node.type == 'ROOT':
                break
        return p

    ## Generate the action function.
    def action_fn(self):
        # Build a list of parse nodes that forms the path from the root.
        # to this end node
        path = self.walk_up_to_root()

        # Declare the action function
        msg = ('cparser_result_t %s(cparser_context_t *context' %
               self.param.replace('cparser_glue', 'cparser_cmd'))

        # Declare the variable list
        for n in path:
            if not n.is_param(): continue
            #if (n.flags.count('CPARSER_NODE_FLAGS_SEL_START') > 0): continue
            #if (n.flags.count('CPARSER_NODE_FLAGS_SEL_PARTIAL') > 0): continue
            #if (n.flags.count('CPARSER_NODE_FLAGS_SEL_END') > 0): continue
            msg += ',\n    %s*%s_ptr' % (Node.TYPES[n.type], n.param)
        msg += ');\n'
        return msg

    ## Generate the glue function.
    def glue_fn(self):
        # Build a list of parse nodes that forms the path from the root.
        # to this end node
        path = self.walk_up_to_root()

        # Build the glue function
        msg =  'cparser_result_t\n'
        msg += '%s (cparser_t *parser)\n' % self.param
        msg += '{\n'

        # Declare the variable list
        skip = ''
        for n in path:
            #print 'n.param=%s n.type=%s' % (n.param, n.type)
            if not n.is_param(): continue
            #if (n.flags.count('CPARSER_NODE_FLAGS_SEL_START') > 0): continue
            #if (n.flags.count('CPARSER_NODE_FLAGS_SEL_PARTIAL') > 0): continue
            #if (n.flags.count('CPARSER_NODE_FLAGS_SEL_END') > 0): continue
            skip = '\n'
            val_type = Node.TYPES[n.type]
            msg += '    %s%s_val;\n' % (val_type, n.param)
            msg += '    %s*%s_ptr = NULL;\n' % (val_type, n.param)
        if skip: msg += '    cparser_result_t rc;\n'
        msg += skip

        # Extract the parameters
        k = -1
        for n in path:
            k = k + 1
            if not n.is_param(): continue
            #if (n.flags.count('CPARSER_NODE_FLAGS_SEL_START') > 0): continue
            #if (n.flags.count('CPARSER_NODE_FLAGS_SEL_PARTIAL') > 0): continue
            #if (n.flags.count('CPARSER_NODE_FLAGS_SEL_END') > 0): continue
            msg += '    if (%d <= parser->cmd_tokens) {\n' % (k+1)
            msg += ('        rc = cparser_get_%s(&parser->tokens[%d], &%s_val);\n' %
                    (n.type.lower(), k, n.param))
            if n.is_optional():
                msg += '        if (CPARSER_OK == rc) {\n'
                msg += '            %s_ptr = &%s_val;\n' % (n.param, n.param)
                msg += '        } else {\n'
                msg += '            assert(%d > parser->token_tos);\n' % (k+1)
                msg += '        }\n'
            else:
                #msg += '    assert(CPARSER_OK == rc);\n'
                msg += '        CPARSER_PARSER_TOKEN_CHECK(parser, rc);\n'
                msg += '        %s_ptr = &%s_val;\n' % (n.param, n.param)
            msg += '    }\n'
            msg += '    CPARSER_NULL_CHECK(%s_ptr);\n' % (n.param)

        # Call the user-provided action function
        msg += ('    %s(&parser->context' %
                self.param.replace('cparser_glue', 'cparser_cmd'))
        for n in path:
            #if (n.flags.count('CPARSER_NODE_FLAGS_SEL_START') > 0): continue
            #if (n.flags.count('CPARSER_NODE_FLAGS_SEL_PARTIAL') > 0): continue
            #if (n.flags.count('CPARSER_NODE_FLAGS_SEL_END') > 0): continue
            if n.is_param(): msg += ',\n        %s_ptr' % n.param
        msg += ');\n'
        msg += '    return CPARSER_OK;\n'
        msg += '}\n\n'
        return msg

##
# \brief     Check command tokens and the number of comments
#
# \param     line     A line of command from a CLI file.
# \param     comment  The comment that goes with this line of command.
#
# \return    None
def comment_check(line, comment):
    tmp_line = re.sub('[{}\(\)|]', '', line).strip()
    tokens = re.split(" +", tmp_line)
    numOfToken = len(tokens)
    numOfComment = len(comment)
    if (numOfToken != 0) and (numOfToken != numOfComment):
        print ('Help Message Error!! (command-token=%d, help-message=%d) of command "%s"' % (numOfToken, numOfComment, line))

##
# \brief     Add one line of CLI to the parse tree.
#
# \param     root     Root node of the parse tree.
# \param     line     A line of command from a CLI file.
# \param     comment  The comment that goes with this line of command.
#
# \return    Return a new root node.
def add_cli(root, line, comment):
    global end_node
    nodes = []
    flags = []
    num_opt_start = 0
    num_opt_end = 0
    num_sel_start = 0
    num_sel_end = 0
    num_sel_element = 0
    sel_start = [0,0,0,0,0,0,0,0,0,0,0,0,0]         #currently support twelve select+option
    sel_element = [1,1,1,1,1,1,1,1,1,1,1,1,1]       #currently support twelve select+option

    # Convert a line into a token list
    line = line.replace('\n','')
    # Strip leading and trailing spaces
    line = line.strip()
    if len(line) != 0:
        DBG('\n  LINE: ', line)
        # check comment
        comment_check(line, comment)
        #RTK-SQUT ++
        if rtkSdkQaUnitTest_enable == True:
            line, comment = rtkSdkUnitTest_cmd_add_test_option(line, comment)
        #RTK-SQUT --

    tokens = re.split(" +", line)

    # Delete all token that is ''
    cnt = tokens.count('')
    for k in range(0, cnt):
        tokens.remove('')
    if len(tokens) == 0:
        return root # this is a blank line. quit

    # Check each token to make sure that it is valid
    for t in tokens:
        m = re.search('\<(.+):(.+)\>', t)
        if m:
            # A parameter. Check type and variable name
            if not Node.TYPES.has_key(m.group(1)):
                raise ValueError, 'Unknown parameter type "%s".' % m.group(1)
            if not re.search('^[a-zA-Z]([a-zA-Z0-9_]*)$', m.group(2)):
                raise ValueError, 'Invalid parameter name "%s".' % m.group(2)
        elif (t == '{') or (t == '}'):
            continue
        elif (t == '(') or (t == ')') or (t == '|'):
            continue
        else:
            if not re.search('^[a-zA-Z0-9_-]+$', t):
                raise ValueError, 'Invalid keyword "%s".' % t

    # Convert tokens to parse tree nodes. '{' and '}' do not produce tree
    # nodes. But they do affect the flags used in some nodes.
    for t in tokens:
        # Parse each token
        # Look for '{'
        if '{' == t:
            # In the last node, its flags field is a reference to
            # 'flags'. So, if we append to it, the last node will get
            # a new flag.
            #flags.append('CPARSER_NODE_FLAGS_OPT_START')
            flags = ['CPARSER_NODE_FLAGS_OPT_START',]
            num_opt_start = num_opt_start + 1
            num_sel_start = num_sel_start + 1
            sel_start[num_sel_start] = sel_start[num_sel_start] + 2
            continue
        # Look for '}'
        if '}' == t:
            # See comment in '{' case.
            #flags.append('CPARSER_NODE_FLAGS_OPT_END')
            flags = ['CPARSER_NODE_FLAGS_OPT_END',]
            num_opt_end = num_opt_end + 1
            num_sel_end = num_sel_end + 1
            #if num_opt_end == num_opt_start:
            #    flags.remove('CPARSER_NODE_FLAGS_OPT_PARTIAL')
            if (nodes[-1].flags.count('CPARSER_NODE_FLAGS_OPT_START') == 0):
                nodes[-1].flags = flags
            continue
        # Look for '('
        if '(' == t:
            flags = ['CPARSER_NODE_FLAGS_SEL_START',]
            num_sel_start = num_sel_start + 1
            sel_start[num_sel_start] = sel_start[num_sel_start] + 1
            continue
        # Look for '|'
        if '|' == t:
            if num_opt_start > num_opt_end:
                flags = ['CPARSER_NODE_FLAGS_OPT_PARTIAL',]
            elif num_sel_start > num_sel_end:
                flags = ['CPARSER_NODE_FLAGS_SEL_PARTIAL',]
            sel_start[num_sel_start] = sel_start[num_sel_start] + 1
            continue
        # Look for ')'
        if ')' == t:
            flags = ['CPARSER_NODE_FLAGS_SEL_END',]
            num_sel_end = num_sel_end + 1
            nodes[-1].flags = ['CPARSER_NODE_FLAGS_SEL_END',]
            continue

        if num_opt_start > num_opt_end:
            #flags = ['CPARSER_NODE_FLAGS_OPT_PARTIAL',]
            pass
        elif num_sel_start > num_sel_end:
            pass
        else:
            flags = []
        #print 'num_opt_start=%d num_opt_end=%d' % (num_opt_start, num_opt_end)
        #print 'num_sel_start=%d num_sel_end=%d' % (num_sel_start, num_sel_end)

        # Look for a parameter
        m1 = re.search('\<(.+):(.+)\>', t)
        m2 = re.search('\<(.+):(.+):(.+)\>', t)
        if m1 or m2:
            if m2:
                node_type = m2.group(1)
                param = m2.group(2)
                desc = m2.group(3)
            else:
                desc = None
                node_type = m1.group(1)
                param = m1.group(2)
            nodes.append(Node(node_type, param, desc, flags))
            continue
        # Look for a keyword
        m = re.search('(.+)', t)
        if m:
            nodes.append(Node('KEYWORD', m.group(1), None, flags))

    # hack alert - Check that if there are optional parameters, the format is ok

    DBG('TOKENS: ', tokens)
    if debug:
        sys.stdout.write(' NODES: ')
        for nn in nodes[:-1]:
            nn.display()
            sys.stdout.write('\n        ')
        nodes[-1].display()
        sys.stdout.write('\n')


    # We need to create the glue function name. Since the path of the node
    # is set up when the node is inserted, we don't have it here. So,
    # we manually walk all the nodes and generate the glue function name
    glue_fn = 'cparser_glue' + root.param
    for n in nodes:
        glue_fn = glue_fn + '_' + n.param.replace('-','_')
    #RTK: add end node for integrate end of line function node
    if integrate_eol:
        nodes.append(Node('END', glue_fn, comment[0], []))
    #RTK end
    # Insert them into the parse tree
    num_sel_element = 1
    for n in sel_start:
        if (n > 0):
            num_sel_element = num_sel_element * n

    if num_sel_element > 2:
        num_sel_element = num_sel_element - 1
    for sel_times in range(0, num_sel_element+1):
        DBG('   CLI: ', sel_times)
        num_braces = 0
        sel_drop = False
        cur_node = root
        sel_candidate = [0,0,0,0,0,0,0,0,0,0,0,0,0]    #currently support twelve select+option
        num_rounds = 0

        times_base = 1
        for start_times in range(0, len(sel_element)):
            if (sel_start[start_times] <= 0):
                continue
            if (sel_times >= (times_base * sel_start[start_times])):
                sel_times_temp = (sel_times % (times_base * sel_start[start_times]))
            else:
                sel_times_temp = sel_times
            sel_element[start_times] = (sel_times_temp / times_base) + 1
            times_base = times_base * sel_start[start_times]

        #if num_sel_element == sel_times:
        #RTK: add end node for integrate end of line function node
        if not integrate_eol:
            end_node = Node('END', glue_fn, comment[0], [])
        #RTK end
        #else:
        #    end_node = Node('END', glue_fn, None, ['CPARSER_NODE_FLAGS_SEL_PARTIAL'])
        token_idx = 0
        for n in nodes:
            token_idx = token_idx + 1
            if (n.flags.count('CPARSER_NODE_FLAGS_SEL_START') > 0):
                num_rounds = num_rounds + 1
                sel_candidate[num_rounds] = sel_candidate[num_rounds] + 1
                if (sel_element[num_rounds] == sel_candidate[num_rounds]):
                    sel_drop = False
                else:
                    sel_drop = True

            elif (n.flags.count('CPARSER_NODE_FLAGS_SEL_PARTIAL') > 0):
                sel_candidate[num_rounds] = sel_candidate[num_rounds] + 1
                if (sel_element[num_rounds] == sel_candidate[num_rounds]):
                    sel_drop = False
                else:
                    sel_drop = True

            elif (n.flags.count('CPARSER_NODE_FLAGS_SEL_END') > 0):
                sel_candidate[num_rounds] = sel_candidate[num_rounds] + 1
                if (sel_element[num_rounds] == sel_candidate[num_rounds]):
                    sel_drop = False
                else:
                    sel_drop = True

            elif (n.flags.count('CPARSER_NODE_FLAGS_OPT_START') > 0):
                num_rounds = num_rounds + 1
                sel_candidate[num_rounds] = sel_candidate[num_rounds] + 1

                if (sel_candidate[num_rounds] > (sel_start[num_rounds] - 1)):
                    sel_candidate[num_rounds] = 0

                if (sel_element[num_rounds] == sel_candidate[num_rounds]):
                    sel_drop = False
                else:
                    sel_drop = True

                #print 'OPT_START token=%s' % n.param
            elif (n.flags.count('CPARSER_NODE_FLAGS_OPT_PARTIAL') > 0):
                #num_rounds = num_rounds + 1
                sel_candidate[num_rounds] = sel_candidate[num_rounds] + 1

                #if (sel_candidate[num_rounds] > (sel_start[num_rounds] - 1)):
                if (sel_candidate[num_rounds] > 1):
                    sel_candidate[num_rounds] = 0

                if (sel_element[num_rounds] == sel_candidate[num_rounds]):
                    sel_drop = False
                else:
                    sel_drop = True

                #print 'OPT_PARTIAL token=%s' % (n.param)
                #print 'sel_element[%d]=%d' % (num_rounds, sel_element[num_rounds])
                #print 'sel_candidate[%d]=%d' % (num_rounds, sel_candidate[num_rounds])
                #print 'sel_drop=%d' % (sel_drop)

            elif (n.flags.count('CPARSER_NODE_FLAGS_OPT_END') > 0):
                sel_candidate[num_rounds] = sel_candidate[num_rounds] + 1

                #if (sel_candidate[num_rounds] > (sel_start[num_rounds] - 1)):
                #    sel_candidate[num_rounds] = 0

                if (sel_element[num_rounds] == sel_candidate[num_rounds]):
                    sel_drop = False
                else:
                    sel_drop = True

                #print 'OPT_END token=%s' % n.param

            if sel_drop:
                sel_drop = False
                continue
            #if (n.flags.count('CPARSER_NODE_FLAGS_OPT_START') +
            #    n.flags.count('CPARSER_NODE_FLAGS_OPT_END') > 0):
            #    if num_braces == k:
            #        drop = True
            #    num_braces = num_braces + 1
            if debug:
                sys.stdout.write('        ')
                cur_node.display()
                sys.stdout.write('-> ')
                n.display()
                sys.stdout.write('\n')
            #print 'token_idx=%d, len(comment)=%d' % (token_idx, len(comment))
            if (token_idx <= len(comment)):
                n.desc = '    "%s",\n' % comment[(token_idx-1)]
            cur_node = cur_node.add_child(n)
        if debug:
            sys.stdout.write('        ')
            cur_node.display()
            sys.stdout.write('-> ')
            #RTK: add end node for integrate end of line function node
            if not integrate_eol:
                end_node.display()
            #RTK end
            sys.stdout.write('\n')
        #RTK: add end node for integrate end of line function node
        if not integrate_eol:
            cur_node.add_child(end_node)
        #RTK end
    return root

##
#   \brief      add flag file of file (filename) in the same directory
#   \filedir    where the flag file live
#   \filename   which list the actual flag file
#   \labels     array of flags
##
def process_flag_file(filedir, filename, labels):
    try:
        flagFileIn = open(os.path.join(filedir,filename), 'r')
    except:
        print 'Fail to open %s.' % os.path.join(filedir,filename)
        sys.exit(-1)

    lineNum = 0
    skip = 0
    reverseSkip = 0
    labelStack = []
    l = ""
    disable = 0
    disableFlag = ""
    global debug

    for line in flagFileIn:
        lineNum = lineNum + 1
        parserLine = line.strip()
        if 0 == len(parserLine):
            continue

        #print parserLine
        m = re.search('^#endif', parserLine)
        if m:
            if len(labelStack) == 0:
                print('%s:%d: Unmatched #ifdef' % (flagFileIn, lineNum))
                sys.exit(-1)

            if 0 == len(labelStack):
                skip = 0
                reverseSkip = 0
            else:
                label = labelStack[-1][0]
                if label == disableFlag:
                    disable = 0

                skip = labelStack[-1][1]
                reverseSkip = labelStack[-1][2]
                labelStack.pop()

            #print "skip %d reverse %d labelStack %d" % (skip, reverseSkip, len(labelStack))
            continue

        m = re.search('^#else', parserLine)
        if m:
            if len(labelStack) == 0:
                print('%s:%d: Unmatched #ifdef' % (flagFileIn, lineNum))
                sys.exit(-1)

            reverseSkip = 1

            #print "skip %d reverse %d labelStack %d" % (skip, reverseSkip, len(labelStack))
            continue

        m = re.search('^#ifdef (.+)', parserLine)
        if m:
            labelStack.append([l, skip, reverseSkip])

            if skip ^ reverseSkip:
                disable = 1
                disableFlag = l

            ##remove whitespace
            l = re.sub(r'\s', '', m.group(1))
            if not (labels.has_key(l) and labels[l]):
                skip = 1
            else:
                skip = 0
            reverseSkip = 0

            #print "skip %d reverse %d labelStack %d" % (skip, reverseSkip, len(labelStack))
            continue

        m = re.search('^#ifndef (.+)', parserLine)
        if m:
            labelStack.append([l, skip, reverseSkip])

            if skip ^ reverseSkip:
                disable = 1
                disableFlag = l

            ##remove whitespace
            l = re.sub(r'\s', '', m.group(1))
            if not (labels.has_key(l) and labels[l]):
                skip = 0
            else:
                skip = 1
            reverseSkip = 0

            #print "skip %d reverse %d labelStack %d" % (skip, reverseSkip, len(labelStack))
            continue


        if 1 == (skip ^ reverseSkip) or 1 == disable:
            continue

        m = re.search('^#include "(.+)"', parserLine)
        if m:
            if len(glob.glob(os.path.join(filedir,m.group(1)))) == 0:
                print('%s:%d: file %s does not exist.' %
                      (filename, lineNum, m.group(1)))
                flagFileIn.close()
                sys.exit(-1)
            else:
                try:
                    flagFile = open(os.path.join(filedir, m.group(1)), 'r')
                    #print "Open %s\n" % m.group(1)
                except:
                    print 'Fail to open %s.' % (os.path.join(filedir, m.group(1)))
                    sys.exit(-1)

                for flagLine in flagFile:
                    flagLine = flagLine.strip()
                    n = re.search('^#define (.+)', flagLine)
                    if n:
                        ##remove whitespace
                        x = re.sub(r'\s|\t', '', n.group(1))
                        labels[x] = 1;
                    n = re.search('^#undef (.+)', flagLine)
                    if n:
                        ##remove whitespace
                        x = re.sub(r'\s|\t', '', n.group(1))
                        labels[x] = 0;

                flagFile.close()

    flagFileIn.close()
    if debug:
        print labels
    return

##
# \brief     Process one .cli file. This includes handling all
#            preprocessors directive.
#
# \param     filename Name of the .cli file.
# \param     root     Root Node object of the parse tree.
# \param     mode     'compile', 'preprocess' or 'mkdep'
# \param     labels   A dictionary containing all defined labels used in
#                     preprocessing.
#
# \return    Return the new root node.
def process_cli_file(filename, root, mode, labels):
    num_disable = 0
    label_stack = []
    deplist = []
    comment = []
    line = ''
    last_cli_root = None
    last_cli_end = None
    submode = False
    global included_file_list

    try:
        fin = open(filename, 'r')
    except:
        print 'Fail to open %s.' % filename
        sys.exit(-1)

    line_num = 0
    for line in fin:
        line_num = line_num + 1

        # Process the file line by line. The orderof processing
        # These different directives is extremely important. And the
        # order below is not arbitrary.
        #
        # We must process #endif not matter what. Otherwise, once
        # we start a #ifdef block that is omitted, we'll never be able
        # to terminate it. Then, we omit every other type of line as long
        # as there is at least one disable #ifdef left. Afterward, we
        # check for illegal directives. A normal command line is handled
        # last. Illegal tokens are checked inside add_cli().

        # #endif
        m = re.search('^#endif', line)
        if m:
            if len(label_stack) == 0:
                print('%s:%d: Unmatched #ifdef/#ifndef' % (filename, line_num))
                sys.exit(-1)
            num_disable = num_disable - label_stack.pop(0)[1]
            continue
        # Skip the rest of processing because some #ifdef/#ifndef is
        # keeping this line from being processed.
        ##if (num_disable > 0):
        ##    continue
        # Check for illegal preprocessor directives
        if (re.search('^#', line) and
            (not re.search('^#ifdef(\S*\/\/.*)*', line) and
             not re.search('^#ifndef(\S*\/\/.*)*', line) and
             not re.search('^#submode(\S*\/\/.*)*', line) and
             not re.search('^#endsubmode(\S*\/\/.*)*', line) and
             not re.search('^#include(\S*\/\/.*)*', line))):
            print('%s:%d: Unknown preprocessor directive.' % (filename, line_num))
            sys.exit(-1)
        # Comment
        m = re.search('^\s*\/\/\s*(.*)', line)
        if m:
            if 'compile' == mode:
                #comment = m.group(1)
                comment.append(m.group(1))
            if 'preprocess' == mode:
                sys.stdout.write(line)
            continue
        # #ifdef
        m = re.search('^#ifdef (.+)', line)
        if m:
            ##remove whitespace
            #l = m.group(1)
            l = re.sub(r'\s', '', m.group(1))
            val = 0
            if not (labels.has_key(l) and labels[l]):
                val = 1
                num_disable = num_disable + 1
            label_stack.insert(0, [l, val])
            continue
        # #ifndef
        m = re.search('^#ifndef (.+)', line)
        if m:
            ##remove whitespace
            #l = m.group(1)
            l = re.sub(r'\s', '', m.group(1))
            val = 0
            if labels.has_key(m.group(1)) and labels[m.group(1)]:
                val = 1
                num_disable = num_disable + 1
            label_stack.insert(0, [l, val])
            continue
        if (num_disable > 0):
            continue
        # #include
        m = re.search('^#include "(.+)"', line)
        if m:
            if len(glob.glob(m.group(1))) == 0:
                print('%s:%d: file %s does not exist.' %
                      (filename, line_num, m.group(1)))
                sys.exit(-1)
            #RTK check duplicate include file
            if re.search(m.group(1), included_file_list):
                #print ("Duplicated include file: %s " % (m.group(1)))
                continue
            included_file_list += ' ' + m.group(1)
            if ('compile' == mode):
                process_cli_file(m.group(1), root, mode, labels)
            elif ('mkdep' == mode):
                deplist.append(m.group(1))
            elif ('preprocess' == mode):
                process_cli_file(m.group(1), root, mode, labels)
            else:
                print('%s:%d: unknown mode %s' % (filename, line_num, mode))
            continue
        # #submode
        m = re.search('^#submode "(.+)"', line)
        if m:
            if submode:
                print('%s:%d: nested submode is invalid.' % (filename, line_num))
            else:
                submode = True
                last_cli_root = Node('ROOT', '_' + m.group(1),
                                     'Root of submode %s' % m.group(1), [])
                last_cli_end.add_child(last_cli_root)
            continue
        # #endsubmode
        m = re.search('^#endsubmode', line)
        if m:
            if not submode:
                print('%s:%d: #endsubmode without a #submode.' %
                      (filename, line_num))
                sys.exit(-1)
            else:
                submode = False
                last_cli_root = None
                last_cli_end = None
            continue
        # What survive must be either an empty line or a command
        if ('compile' == mode):
            try:
                if not submode:
                    root = add_cli(root, line, comment)
                    last_cli_end = end_node
                else:
                    last_cli_root = add_cli(last_cli_root, line, comment)
            except ValueError, msg:
                print('%s:%d: %s' % (filename, line_num, msg))
                sys.exit(-1)
        elif ('preprocess' == mode):
            sys.stdout.write(line)
        #comment = None
        comment = []
    if 'mkdep' == mode:
        sys.stdout.write('%s:' % filename)
        for d in deplist: sys.stdout.write(' %s' % d)
        sys.stdout.write('\n')
    return root

# walker_gen_dbg - Display each node
def walker_gen_dbg(node, fout):
    fout.write('  ' * node.depth)
    node.display()
    fout.write('\n')

def check_glue_fn(node, fout):
    if node.write_glue_fn == False:
        out_msg = node.glue_fn()
        node.write_glue_fn = True
    else:
        out_msg = ''

    return fout.write(out_msg)

def check_c_struct(node, fout):
    if node.write_c_struct == False:
        out_msg = node.c_struct()
        node.write_c_struct = True
    else:
        out_msg = ''

    return fout.write(out_msg)

def check_action_fn(node, fout):
    if node.write_action_fn == False:
        cmd_func = node.param.replace('cparser_glue', 'cparser_cmd')
        out_msg = node.action_fn()
        node.write_action_fn = True
    else:
        out_msg = ''

    return fout.write(out_msg)


def rtkSdkUnitTest_cmd_add_test_option(line, comment):
    global squt_op_idx

    tokens = line.split(')')
    #print ('cmd:"%s" split by ")" = %d' % (line, len(tokens)))
    num = len(tokens)
    if num <= 1:
        return (line, comment)
    else:
        # Parentheses is found, add additional option for unit-test usage.
        new_line = ''
        last_idx = num - 1
        for i in range(len(tokens)):
            #print (' >>>%d "%s"' % (i, tokens[i]))
            new_line += tokens[i]
            if i != last_idx:
                # if there is token in parentheses, don't add additional option to it
                m = re.search("\(\s+\<", tokens[i])
                if m:
                    new_line += ')'
                else:
                    new_line +=  '| <PECU:tOp%X> )' % squt_op_idx
                    squt_op_idx = squt_op_idx + 1

        # Add comment for additional option
        new_comment = []
        tmp_line = re.sub('[{}\(\)|]', '', new_line).strip()
        tokens = re.split(" +", tmp_line)
        org_comment_num = len(comment)
        idx = 0
        for t in tokens:
            #print ('idx %d t=[%s]' % (idx, t))
            if re.match('<PECU:tOp[0-9A-F]+>', t):
                new_comment.append('')
            else:
                if (idx < org_comment_num):
                    new_comment.append(comment[idx])
                    idx = idx + 1
                else:
                    #print ('Error!! Insufficient help messages (%2d) of command "%s"' % (org_comment_num, line))
                    break

        #print('new_command: "%s"' % (new_line))
        #for i in range(len(new_comment)):
        #    print ('    new_comment[%d] :"%s"' % (i, new_comment[i]))
        return (new_line, new_comment)


def rtkSdkUnitTest_fn_name_remove_test_option(org_fname, keep_org = False):
    #print ('Process file: %s' % org_fname)
    new_fname = org_fname + '.new'

    try:
        new_file = open(new_fname, 'w+')
    except:
        print 'Fail to open %s.' % new_fname
        sys.exit(-1)

    try:
        org_file = open(org_fname, 'r')
    except:
        print 'Fail to open %s.' % org_file
        sys.exit(-1)

    for line in org_file:
        if re.match('.*cparser_glue_', line) or re.match('.*cparser_cmd_', line):
            new_file.write(re.sub('_tOp[0-9A-F]+', '', line))
        else:
            new_file.write(line)

    new_file.close()
    org_file.close()
    if keep_org == True:
        move(org_fname, org_fname + '.org')
    else:
        remove(org_fname)
    move(new_fname, org_fname)


## Process the command-line argument
def main():
    filelist = []
    labels = {}
    mode = 'compile'
    out_dir = '.'
    c_fname = 'cparser_tree.c'
    h_fname = 'cparser_tree.h'
    f_dir   = '.'
    f_fname = ''
    global rtkSdkQaUnitTest_enable
    global debug

    startTime = datetime.datetime.now()

    # Parse input arguments
    sys.argv.pop(0) # remove mk_parser.py itself
    while (len(sys.argv) > 0):
        item = sys.argv.pop(0)
        if '-MM' == item:
            mode = 'mkdep'
        elif '-P' == item:
            mode = 'preprocess'
        elif '-D' == item:
            l = sys.argv.pop(0)
            labels[l] = 1;
        elif '-o' == item:
            out_dir = sys.argv.pop(0)
        elif '-c' == item:
            c_fname = sys.argv.pop(0)
        elif '-i' == item:
            h_fname = sys.argv.pop(0)
        elif '-fd' == item:
            f_dir = sys.argv.pop(0)
        elif '-f' == item:
            f_fname = sys.argv.pop(0)
            process_flag_file(f_dir, f_fname, labels)
        elif '-sdktest' == item:
            #RTK-SQUT
            rtkSdkQaUnitTest_enable = True
        else:
            filelist.append(item)


    # Process each file
    root = Node('ROOT', '', 'Root node of the parser tree', [])
    for f in filelist:
        if ('mkdep' != mode):
            print('Processing %s...' % f)
        root = process_cli_file(f, root, mode, labels)
    if print_tree:
        root.walk(walker_gen_dbg, 'pre-order', sys.stdout)

    # Generate .c file that contains glue functions and parse tree
    c_fname = out_dir + '/' + c_fname
    try:
        fout = open(c_fname, 'w+')
    except:
        print 'Fail to open %s.' % c_fname
        sys.exit(-1)
    fout.write('/*----------------------------------------------------------------------\n' +
               ' * This file is generated by mk_parser.py.\n' +
               ' *----------------------------------------------------------------------*/\n' +
               '#include <assert.h>\n' +
               '#include <stdint.h>\n' +
               '#include <stdio.h>\n' +
               '#include <parser/cparser.h>\n' +
               '#include <parser/cparser_priv.h>\n' +
               '#include <parser/cparser_token.h>\n' +
               '#include <parser/cparser_tree.h>\n\n')
    #n_cmds = root.walk(lambda n,f: f.write(n.glue_fn()), 'func', fout)
    n_cmds = root.walk(lambda n,f: check_glue_fn(n, f), 'func', fout)

    #n_nodes = root.walk(lambda n,f: f.write(n.c_struct()), 'post-order', fout)
    n_nodes = root.walk(lambda n,f: check_c_struct(n, f), 'post-order', fout)

    fout.close()

    h_fname = out_dir + '/' + h_fname
    try:
        fout = open(h_fname, 'w+')
    except:
        print 'Fail to open %s.' % h_fname
        sys.exit(-1)
    fout.write('/*----------------------------------------------------------------------\n' +
               ' * This file is generated by mk_parser.py.\n' +
               ' *----------------------------------------------------------------------*/\n' +
               '#ifndef __CPARSER_TREE_H__\n' +
               '#define __CPARSER_TREE_H__\n\n' +
               'extern cparser_node_t cparser_root;\n\n')
    #root.walk(lambda n,f: f.write(n.action_fn()), 'func', fout)
    root.walk(lambda n,f: check_action_fn(n, f), 'func', fout)
    fout.write('\n#endif /* __CPARSER_TREE_H__ */\n')
    fout.close()

    #RTK-SQUT ++
    if rtkSdkQaUnitTest_enable == True:
        rtkSdkUnitTest_fn_name_remove_test_option(c_fname, False)
        rtkSdkUnitTest_fn_name_remove_test_option(h_fname, False)
    #RTK-SQUT --

    # Print out a summary
    print '%d commands.' % n_cmds
    print '%d parse tree nodes (%d bytes).' % (n_nodes, n_nodes * 24)

    finishTime = datetime.datetime.now()
    print ('Process time %s' % (finishTime - startTime))

    return

# Entry point of the script
if __name__ == '__main__':
    main()


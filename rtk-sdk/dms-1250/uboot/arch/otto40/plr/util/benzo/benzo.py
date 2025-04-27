#!/usr/bin/env python
#coding=utf-8
#
import sys

def generate_dump_funcs(strname, fld):
	fn_name_pt0 = 'MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_'
	fn_name_pt1 = '(' + strname + ' *str) {'
	print fn_name_pt0 + fld[0] + fn_name_pt1

	if 'register_set' in strname:
		rvar = '.v'
		fmt = '%08x'
	else:
		rvar = ''
		fmt = '%d'

	if (fld[1].isdigit()):
		print '\tu32_t i;'
		print '\tfor (i=0; i<' + fld[1] + '; i+=4) {'
		print_ary = '\t\tprintf("  ' + fld[0] + '[%02d]: '
		print_ary += fmt + ', '
		print_ary += fmt + ', '
		print_ary += fmt + ', '
		print_ary += fmt + '\\n", i, '
		print_ary += 'str->' + fld[0] + '[i]' + rvar + ', '
		print_ary += 'str->' + fld[0] + '[i+1]' + rvar + ', '
		print_ary += 'str->' + fld[0] + '[i+2]' + rvar + ', '
		print_ary += 'str->' + fld[0] + '[i+3]' + rvar + ');'
		print print_ary
		print '\t}'
	else:
		print '\tprintf("  ' + fld[0] + ': ' + fmt + '\\n", str->' + fld[0] + rvar + ');'

	print '\treturn;'
	print '}'

def generate_main_dump_func(strname, fldinfo):
	print 'MEMCNTLR_SECTION static inline void ' + strname[0:-2] + '_dump(void) {'
	print '\t' + strname + ' *str = meminfo.' + strname[3:-2] + ';'
	print '\tprintf("II: meminfo.' + strname[0:-2] + ' @ %p (%dB)\\n", str, sizeof(' + strname + '));'
	for fld in fldinfo:
		print '\tmc_dump_' + fld[0] + '(str);'
	print '}'

def generate_xlat_n_assign_interface(fld):
	fn_name_pt0 = 'MEMCNTLR_SECTION static inline void mc_xlat_n_assign_'
	fn_unused_attr = '__attribute__ ((unused))'
	fn_name_pt1 = '(mc_dram_param_t * '
	fn_name_pt1 += fn_unused_attr
	fn_name_pt1 += ', mc_cntlr_opt_t * '
	fn_name_pt1 += fn_unused_attr
	fn_name_pt1 += ', mc_register_set_t *);'
	print fn_name_pt0 + fld[0] + fn_name_pt1

def generate_main_xlat_n_assign_func(meminfo):
	print("MEMCNTLR_SECTION static inline void mc_xlat_n_assign(void) {")
	print("\tmc_dram_param_t   *dp = meminfo.dram_param;")
	print("\tmc_cntlr_opt_t    *co = meminfo.cntlr_opt;")
	print("\tmc_register_set_t *rs = meminfo.register_set;")

	for sub_struct in meminfo:
		strname = sub_struct[0]
		if ('mc_register_set_t' == strname): continue

		for field in sub_struct[1:]:
			if (field[1].isdigit()):
				print("\tmc_xlat_n_assign_%s(dp, co, rs);" % (field[0]))
			else:
				print('\tif (meminfo.%s->%s != -1) {' % (strname[3:-2], field[0]))
				print("\t\tmc_xlat_n_assign_%s(dp, co, rs);" % (field[0]))
				print('\t\tif (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;')
				print('\t}')

	print '\treturn;'
	print '}'


def main(argv):
	meminfo = []
	sub_struct = []

	if (len(argv)) != 2:
		print('#error Syntax: {0} [DRAM header file]'.format(argv[0]))
		print('#error Example: {0} dram_gen2.h'.format(argv[0]))
		exit(1)

	dram_header_fn = argv[1]

	f = open(dram_header_fn, 'r')
	lines = f.readlines()
	f.close()

	# find out all fields in meminfo.
	for line in lines:
		if (('\tFLD'  in line) or
		    ('\tBOOL' in line) or
		    ('\tARY'  in line)):
			fldname = line.split('(')[1].split(')')[0].split(',')[0]
			fldtype = line.split('(')[0]

			if ('\tARY' in fldtype):
				fldtype = line.split(', ')[2].split(')')[0]

			if (len(sub_struct) == 0):
				sub_struct.append('unknwon')

			sub_struct.append([fldname, fldtype])

		if ((len(sub_struct) > 0) and
		    ('} ' in line)):
			strname = line.split('} ')[1].split(';')[0]
			sub_struct[0] = strname
			meminfo.append(sub_struct)
			sub_struct = []

	# generate dump functions and xna interfaces
	for sub_struct in meminfo:
		strname = sub_struct[0]

		for field in sub_struct[1:]:
			generate_dump_funcs(strname, field)
			if ('mc_register_set_t' not in strname):
				generate_xlat_n_assign_interface(field)

		generate_main_dump_func(strname, sub_struct[1:])

	generate_main_xlat_n_assign_func(meminfo)

if __name__ == '__main__':
	main(sys.argv)

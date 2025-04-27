#!/usr/bin/env python
#coding=utf-8
#

import xml.etree.ElementTree as ET
import os.path as OP
import time
import xml
import sys

def gen_a_line_of_c(s):
	print s

def error_handled_parse(f):
	try:
		tree = ET.parse(f)
	except IOError:
		gen_a_line_of_c('#error {0} does not exist.'.format(f))
		exit(1)

	return tree

def get_mod_bases(elem):
	module_base = {}

	for mod in elem.findall('./ASICMOD'):
		module_base[mod.find('NAME').text] = int(mod.find('OFFSET').text, 16) + register_base - 4;

	return module_base

def get_xml_files(elem):
	xml_files = []

	for mod_xml in elem.findall('./DATA_MUX_FILE'):
		modtree = error_handled_parse(mod_xml.text)
		for reg_xml in modtree.findall('REG_FILE'):
			xml_files.append(reg_xml.text)

	return xml_files

def gen_version(elem):
	elem = elem.findall('HISTORY')[-1]
	mtime = time.gmtime(OP.getmtime(index_filename))

	gen_a_line_of_c('/*-----------------------------------------------------')
	gen_a_line_of_c(' Extraced from {0}: {1}|{2}'.format(index_filename,
	                                                     elem.find('REV').text,
	                                                     elem.find('DATE').text))
	gen_a_line_of_c(' RP Last modified at {0}'.format(time.asctime(mtime)))
	gen_a_line_of_c('-----------------------------------------------------*/')

def gen_c_prologue(elem):
	# generate header guardian flag
	gen_a_line_of_c('#ifndef _REGISTER_MAP_H_')
	gen_a_line_of_c('#define _REGISTER_MAP_H_')

	gen_version(elem)

	# generate some common codes, macros, etc.
	gen_a_line_of_c('typedef volatile unsigned int * regval;')
	gen_a_line_of_c('')
	gen_a_line_of_c('#define RLOAD(_var, _reg)  _reg##_T _var = { .v = _reg##rv }');
	gen_a_line_of_c('#define RTYPE(_var, _reg)  _reg##_T _var = { .v = 0 }');
	gen_a_line_of_c('#define RSTORE(_var, _reg) _reg##rv = _var.v');
	gen_a_line_of_c('')

	# generate __va_narg__ macros
	gen_a_line_of_c('/* for __VA_NARG__ */')
	gen_a_line_of_c('#define PP_NARG(...)  PP_NARG_(__VA_ARGS__,PP_RSEQ_N())')
	gen_a_line_of_c('#define PP_NARG_(...) PP_ARG_N(__VA_ARGS__)')
	gen_a_line_of_c('#define PP_ARG_N(_01,_02,_03,_04,_05,_06,_07,_08,_09,_10, \\')
	gen_a_line_of_c('                 _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \\')
	gen_a_line_of_c('                 _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \\')
	gen_a_line_of_c('                 _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \\')
	gen_a_line_of_c('                 _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \\')
	gen_a_line_of_c('                 _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \\')
	gen_a_line_of_c('                 _61,_62,_63,  N,...) N')
	gen_a_line_of_c('#define PP_RSEQ_N()                      63, _rset62, 61, _rset60, \\')
	gen_a_line_of_c('  59, _rset58, 57, _rset56, 55, _rset54, 53, _rset52, 51, _rset50, \\')
	gen_a_line_of_c('  49, _rset48, 47, _rset46, 45, _rset44, 43, _rset42, 41, _rset40, \\')
	gen_a_line_of_c('  39, _rset38, 37, _rset36, 35, _rset34, 33, _rset32, 31, _rset30, \\')
	gen_a_line_of_c('  29, _rset28, 27, _rset26, 25, _rset24, 23, _rset22, 21, _rset20, \\')
	gen_a_line_of_c('  19, _rset18, 17, _rset16, 15, _rset14, 13, _rset12, 11, _rset10, \\')
	gen_a_line_of_c('  09,  _rset8, 07,  _rset6, 05,  _rset4, 03,  _rset2, 01,      00')
	gen_a_line_of_c('#define rset(rtype, ival, ...) do {            \\')
	gen_a_line_of_c('\t\trtype##_T dummyr = { .v = ival };          \\')
	gen_a_line_of_c('\t\tPP_NARG(__VA_ARGS__)(dummyr, __VA_ARGS__); \\')
	gen_a_line_of_c('\t\trtype##rv = dummyr.v;                      \\')
	gen_a_line_of_c('\t} while(0)')

	# generate _rsetn macros
	for narg in range(2, 63):
		if narg % 2:
			continue

		code = '#define _rset' + str(narg) + '(var'
		ident_space = ' ' * (len(code) - 3)

		for nfld in range(0, narg/2):
			num = "%02d"%nfld
			if nfld % 4 == 0:
				code = code + ', \\\n' + ident_space
			else:
				code = code + ', '
			code = code + 'f' + num + ', v' + num

		code = code + ') do { \\\n'

		for nfld in range(0, narg/2):
			code = code + '\t\tvar.f.f%02d = v%02d; \\\n' % (nfld, nfld)

		code = code + '\t} while(0)\n'
		print code

def gen_c_epilogue(elem):
	gen_a_line_of_c('#endif')
	return

# Removed description tag if found any invalid characters.
def xml_recover_from_code4(xml_string, e):
	if e.code != 4:
		print '#error EE: XML parsing error code: ' + str(e.code) + ': ' + str(e)
		exit(1)

	lines = xml_string.split('\n');
	if '<DESCRIPTION>' in lines[e.lineno - 1]:
		lines[e.lineno - 1] = ''
	else:
		print '#error EE: Unable to handle code 4: ' + str(e) + ': ' + lines[e.lineno - 1]
		exit(1)

	return '\n'.join(lines)

def gen_c_struct(xmlf, mod_bases):
	xml_string = open(xmlf).read();
	elem = None

	# There could be invalid characters in XML string.
	while (elem == None):
		try:
			elem = ET.fromstring(xml_string)
		except xml.parsers.expat.ExpatError, e:
			xml_string = xml_recover_from_code4(xml_string, e)

	gen_a_line_of_c('/*-----------------------------------------------------')
	gen_a_line_of_c(' Extraced from {0}'.format(xmlf))
	gen_a_line_of_c('-----------------------------------------------------*/')

	for rpreg in elem.findall('./REGISTER'):
		regname = rpreg.find('NAME').text.upper()
		modtype = rpreg.find('MODTYPE').text
		regaddr = mod_bases[modtype] + 4

		mod_bases[modtype] = regaddr

		if ('RESERVED' in regname) or ('DUMMY' in regname):
			if rpreg.find('LARRAY').text and rpreg.find('HARRAY').text:
				mod_bases[modtype] = regaddr + 4 * int(rpreg.find('HARRAY').text)
			continue

		gen_a_line_of_c('typedef union {')
		gen_a_line_of_c('\tstruct {')

		bitcnt = 0
		mbzcnt = 0
		regval = 0
		fldval = 0
		for field in rpreg.findall('FIELD'):
			fieldname = field.find('NAME').text.lower()
			fieldwid = int(field.find('MSB').text) - int(field.find('LSB').text) + 1

			if ("reserved" in fieldname) or ("dummy" in fieldname):
				fieldname = 'mbz_' + str(mbzcnt)
				mbzcnt = mbzcnt + 1
				fldval = 0
			else:
				fldval = int(field.find('VALUE').text, 16)

			gen_a_line_of_c('\t\tunsigned int {0}:{1}; //{2}'.format(fieldname,
			                                                         str(fieldwid),
			                                                         fldval))
			regval = regval | (fldval << int(field.find('LSB').text))
			bitcnt = bitcnt + fieldwid

		if bitcnt != 32:
			gen_a_line_of_c('#error EE: Only {0} bits in this reg.!'.format(bitcnt))

		gen_a_line_of_c('\t} f;')
		gen_a_line_of_c('\tunsigned int v;')
		gen_a_line_of_c('}} {0}_T;'.format(regname))

		gen_a_line_of_c('#define %srv (*((regval)0x%08x))' % (regname, regaddr))
		gen_a_line_of_c('#define %sdv (0x%08x)' % (regname, regval))
		gen_a_line_of_c('#define RMOD_{0}(...) rset({0}, {0}rv, __VA_ARGS__)'.format(regname))
		gen_a_line_of_c('#define RIZS_{0}(...) rset({0}, 0, __VA_ARGS__)'.format(regname))
		gen_a_line_of_c('#define RFLD_{0}(fld) (*((const volatile {0}_T *){1})).f.fld'.format(regname, hex(regaddr)))

		gen_a_line_of_c('')

		if rpreg.find('LARRAY').text and rpreg.find('HARRAY').text:
			mod_bases[modtype] = regaddr + 4 * int(rpreg.find('HARRAY').text)

def main(argv):
	global index_filename, register_base

	if (len(argv)) != 3:
		gen_a_line_of_c('#error Syntax: {0} [reg_profile_path] [addr_base]'.format(argv[0]))
		gen_a_line_of_c('#error Example: {0} ./chip_index.xml 0xb8000000'.format(argv[0]))
		exit(1)

	index_filename = argv[1]
	register_base  = int(argv[2], 16)

	tree = error_handled_parse(index_filename)
	root = tree.getroot()

	gen_c_prologue(root)

	xml_files = get_xml_files(root)
	mod_bases = get_mod_bases(root)

	for xmlf in xml_files:
		gen_c_struct(xmlf, mod_bases)

	gen_c_epilogue(root);

if __name__ == "__main__":
	main(sys.argv)

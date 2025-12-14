# Include this file to utilize provided functions.
# Don't use `export get_otto_file`, since it requires double `$` for escaping to
# prevent from variable substitution. I.e., '$$(wildcard $$2)' instead of '$(wildcard $2)'
# Google 'Makefile function export' for more detail.

# $1: dst file
# $2: src file
get_otto_file = \
	echo -n "  OTTO/F  Verifying $(abspath $1)... "; \
	$(if $(wildcard $2), \
		if [ -z $1 ]; then echo "Missing destination operand!"; exit 1; fi; \
		cmp $2 $1 > /dev/null 2>&1; \
		if [ "$$?" -ne 0 ]; then \
			echo -n "synchronizing... "; \
			rm -rf $1; \
			mkdir -p $(dir $1); \
			cp -rL $2 $1; \
			chmod 544 $1; \
		fi, \
		$(if $(wildcard $1,), \
			echo -n "", \
			echo -n "FAIL!"; exit 1 \
		) \
	); \
	echo "done"; \

# $1: dst dir
# $2: src dir
get_otto_dir = \
	echo -n "  OTTO/D  Verifying $(abspath $1)... "; \
	$(if $(wildcard $2), \
		if [ -z $1 ]; then echo "Missing destination operand!"; exit 1; fi; \
		diff -r $1 $2 --exclude *.o --exclude *.svn* > /dev/null 2>&1; \
		if [ "$$?" -ne 0 ]; then \
			echo -n "synchronizing... "; \
			rm -rf $1; \
			cp -r $2 $1; \
			rm -rf $1/.svn; \
			chmod 444 $1/*; \
		fi, \
		$(if $(wildcard $1), \
			echo -n "", \
			echo "FAIL!"; exit 1 \
		) \
	); \
	echo "done"; \

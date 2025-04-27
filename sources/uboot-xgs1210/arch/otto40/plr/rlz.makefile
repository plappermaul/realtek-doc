# script to get version from SVN and GIT. d for dirty, c for clean
SVN_VCODE := svn info 2>/dev/null | grep 'Last Changed Rev' | sed -e 's|Last Changed Rev: ||'
SVN_MCODE := svn status 2>/dev/null | grep '^M' > /dev/null 2>&1 && echo 'd' || echo 'c'

GIT_VCODE := git show 2>/dev/null | head -c 14 | sed -e 's|commit ||'
GIT_MCODE := git status 2>/dev/null | grep 'modified:' > /dev/null 2>&1 && echo 'd' || echo 'c'

VCS_SVER := 0x$(shell $(SVN_VCODE))$(shell $(SVN_MCODE))
VCS_GVER := 0x$(shell $(GIT_VCODE))$(shell $(GIT_MCODE))
VCS_VER  := 0xbad0c0de
ifneq ($(VCS_SVER),0xc)
  VCS_VER := $(VCS_SVER)
endif
ifneq ($(VCS_GVER),0xc)
  VCS_VER := $(VCS_GVER)
endif

MAKE_DATE := 0x$(shell date +%y%m%d%H)

rlz_ver:
	@echo "VCS_VER := $(VCS_VER)" > $@

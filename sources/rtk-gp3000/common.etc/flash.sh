#!/bin/sh
#
# usage: flash.sh [cmd] ...
#

CFG_FILE="/var/config/cfg.txt"

if [ ! -f $CFG_FILE ] ; then
	cp /etc/cfg.txt $CFG_FILE
fi

case "$1" in
  "get")
	if [ "$2" != "" ]; then
		grep $2 $CFG_FILE
		if [ "$?" = "0" ]; then
			exit 0
		fi
	else
		/bin/sh $0 -h
		exit 1
	fi
	;;
  "set")
	if [ "$2" != "" ] && [ "$3" != "" ]; then
		sed -i '/^'"$2"'=/d' $CFG_FILE
		echo $2=$3 >> $CFG_FILE
	else
		/bin/sh $0 -h
		exit 1
	fi
	;;
  "all")
	cat $CFG_FILE
	if [ "$?" = "0" ]; then
		exit 0
	fi
	;;
  "-h")
	echo 'Usage: flash.sh [cmd]'
	echo '  all                       : Show all settings.'
	echo '  get MIB-NAME              : get a specific mib parameter.'
	echo '  set MIB-NAME MIB-VALUE    : set a specific mib parameter into flash memory.'
	;;
  *)
	/bin/sh $0 -h
	exit 1
esac

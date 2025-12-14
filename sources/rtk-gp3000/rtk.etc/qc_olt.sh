#!/bin/sh
echo 0 > /proc/qc_test/olt_init
mode=`cat /proc/qc_test/olt_init`

if [ "$mode" == 1 ] ; then
	echo "Start OLT testing..."
	/tmp/qc_olt.sh
fi

#!/bin/sh

#check mode
mode=`cat /proc/ft2/mode; cat /var/ft2Mode`
gpio_set=1

if [ "$mode" = "OLT" ] ; then
	#Start reboot timer 5min
	echo 300000 > /proc/ft2/mode;
	/etc/ft2_test.sh init_olt; \
	while [ 1 ] ; do \
		/etc/ft2_test.sh olt 1 $gpio_set; \
		if [ $gpio_set ] ; then \
			gpio_set=0; \
		else \
			gpio_set=1; \
		fi \
	done
fi

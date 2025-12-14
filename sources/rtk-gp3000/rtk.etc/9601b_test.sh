#!/bin/sh

#Disable LED control for get GPIO 2 value
diag led set group 0 parallel-state disable
gpio_val=`diag debug gpio get pin 2 databit`;
diag led set group 0 parallel-state enable

if [ ${gpio_val:43:1} -eq 1 ]; then
	./etc/ol_test.sh all
fi


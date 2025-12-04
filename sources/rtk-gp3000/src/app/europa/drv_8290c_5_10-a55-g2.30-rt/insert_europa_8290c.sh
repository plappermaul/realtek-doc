#!/bin/sh

#MIBPONMODE=$(flash get PON_MODE | awk -F'=' '{print $2}')
#MIBPONMODE=$(mib get PON_MODE | awk -F'=' '{print $2}')

MIBPONMODE=$(mib get PON_MODE | sed -e 's/[[:space:]]//g' -e 's/PON_MODE=//g')

if [ -f  /lib/modules/europa_8290c_drv.ko ]; then
	EUROPA8290C_DRV=/lib/modules/europa_8290c_drv.ko
else
	EUROPA8290C_DRV=europa_8290c_drv
fi

if [ $MIBPONMODE -eq 2 ]; then
	echo "EPON mode."
else
#while [ $(ps | grep -c omci_app) -lt 2 ]; do
#	echo "omci_app is not ready."
#	sleep 1
#done
#	echo "omci_app is up."
	echo "GPON mode."
fi

if [ -f  /var/config/rtl8290c_dev0.data ]; then
	echo  "/var/config/rtl8290c_dev0.data existed. "
	/bin/europacli update flash 8290c
else
	echo  "/var/config/rtl8290c_dev0.data not existed, generate it."
	/bin/europacli init flash 8290c
fi

		
insmod $EUROPA8290C_DRV PON_MODE=$MIBPONMODE \

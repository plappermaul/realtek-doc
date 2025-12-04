#!/bin/sh

#MIBPONMODE=$(flash get PON_MODE | awk -F'=' '{print $2}')
#MIBPONMODE=$(mib get PON_MODE | awk -F'=' '{print $2}')

MIBPONMODE=$(mib get PON_MODE | sed -e 's/[[:space:]]//g' -e 's/PON_MODE=//g')

if [ -f  /lib/modules/europa_8291_drv.ko ]; then
	EUROPA8291_DRV=/lib/modules/europa_8291_drv.ko
else
	EUROPA8291_DRV=europa_8291_drv
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
		
if [ -f  /var/config/rtl8291_dev0.data ]; then
	echo  "/var/config/rtl8291_dev0.data existed, do nothing."
	/bin/europacli update flash 8291	
else
	echo  "/var/config/rtl8291_dev0.data not existed, generate it."
	/bin/europacli init flash 8291
fi

		
insmod $EUROPA8291_DRV PON_MODE=$MIBPONMODE \

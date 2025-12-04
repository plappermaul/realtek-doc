#!/bin/sh

#MIBPONMODE=$(flash get PON_MODE | awk -F'=' '{print $2}')
#MIBPONMODE=$(mib get PON_MODE | awk -F'=' '{print $2}')

MIBPONMODE=$(flash get PON_MODE | sed -e 's/[[:space:]]//g' -e 's/PON_MODE=//g')

if [ -f  /lib/modules/omcidrv.ko ]; then
	EUROPA8290B_DRV=/lib/modules/europa_8290b_drv.ko
else
	EUROPA8290B_DRV=europa_8290b_drv
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

if [ -f  /var/config/europa.data ]; then
	echo  "/var/config/europa.data existed, do nothing."
else
	echo  "/var/config/europa.data not existed, generate it."
	/bin/europacli init flash europa
fi
		
if [ -f  /var/config/rtl8290b.data ]; then
	echo  "/var/config/rtl8290b.data existed, do nothing."
else
	echo  "/var/config/rtl8290b.data not existed, generate it."
	/bin/europacli init flash 8290b
fi

		
insmod $EUROPA8290B_DRV PON_MODE=$MIBPONMODE \

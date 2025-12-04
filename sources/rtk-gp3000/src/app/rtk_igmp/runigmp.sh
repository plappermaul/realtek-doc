#!/bin/sh

if [ -d /proc/rg ]; then
	rg_igmp=1
else
	rg_igmp=0
fi

igmp_conf_debug=`cat /var/config/igmp_gpon.conf | grep DEBUG_MODE | sed 's/# *DEBUG_MODE=//g'`
create_igmp_conf ()
{
	if [ ! -f /var/config/igmp_gpon.conf ] || [ "$igmp_conf_debug" = "0" ] || [ -z "$igmp_conf_debug" ]; then
		cp /etc/igmp_gpon.conf /var/config/igmp_gpon.conf
	fi
}

get_sdk_type ()
{
	verStr=`cat /proc/version  | grep OpenWrt`
	sdkType="luna"
	if [ "$verStr" != "" ]; then
		sdkType="OpenWrt"
	fi

    echo $sdkType
}


if [ "$rg_igmp" == "0" ]; then
	create_igmp_conf
	insmod /lib/modules/igmp_drv.ko
	wait 1
	igmpd -f /var/config/igmp_gpon.conf -s $(get_sdk_type) &
fi

#!/bin/sh

pon_mode=`mib get PON_MODE`
pon_dect_ctrl=`mib get PON_DETECT_CONTROL_FLAG | awk -F '=' '{print $2}'`
pon_dect_gpon_flag=`mib get PON_DETECT_AVAILABLE_GPON_FLAG | awk -F '=' '{print $2}'`
pon_dect_epon_flag=`mib get PON_DETECT_AVAILABLE_EPON_FLAG | awk -F '=' '{print $2}'`
pon_dect_eth_flag=`mib get PON_DETECT_AVAILABLE_ETH_FLAG | awk -F '=' '{print $2}'`
if [ "$pon_mode" == "PON_MODE=1" ]; then
	if [ ! -f /var/config/run_customized_sdk.sh ]; then
		cp /etc/run_customized_sdk.sh /var/config/run_customized_sdk.sh
	fi
		/var/config/run_customized_sdk.sh
		if [ -f /lib/modules/pf_rt_fc.ko ]; then
			ifconfig eth0 up
		fi
		/etc/runomci.sh
		/bin/pondetect $pon_dect_ctrl $pon_dect_gpon_flag $pon_dect_epon_flag $pon_dect_eth_flag &
		echo "running GPON mode ..."
elif [ "$pon_mode" == "PON_MODE=2" ]; then
cartkmod=`modprobe -l | grep ca-rtk-epon-drv.ko`
if [ -f /lib/modules/`uname -r`/$cartkmod ] ; then
        modprobe ca-rtk-epon-drv
else
	rtk_soc_family=`cat /sys/devices/soc0/family`
	if [ "$rtk_soc_family" == "Taurus" ]; then
		echo "RTK SoC: Taurus"
		rtk_mod_postfix=77c
	elif [ "$rtk_soc_family" == "Elnath" ]; then
		echo "RTK SoC: Elnath"
		rtk_mod_postfix=07f
	else
		echo "unknown rtk_soc_family = $rtk_soc_family"
	fi

	cartk_epon_mod=`modprobe -l | grep ca-rtk-epon-$rtk_mod_postfix-drv.ko`

	echo $cartk_epon_mod
	if [ -f /lib/modules/`uname -r`/$cartk_epon_mod ] ; then
		insmod /lib/modules/`uname -r`/$cartk_epon_mod
	fi
fi
        /etc/runoam.sh
        sleep 2
	/bin/pondetect $pon_dect_ctrl $pon_dect_gpon_flag $pon_dect_epon_flag $pon_dect_eth_flag &
        echo "running EPON mode ..."
elif [ "$pon_mode" == "PON_MODE=3" ]; then
        echo "running Ether mode..."
else
        echo "Not supported PON/WAN mode..."
fi


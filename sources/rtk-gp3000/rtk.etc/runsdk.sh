#!/bin/sh

str_cfg="\ 
PON_MODE \
FIBER_MODE \
PON_DETECT_CONTROL_FLAG \
PON_DETECT_AVAILABLE_GPON_FLAG \
PON_DETECT_AVAILABLE_EPON_FLAG \
PON_DETECT_AVAILABLE_ETH_FLAG \
DUAL_MGMT_MODE"

boa_get=`mib get $str_cfg`

pon_mode=`echo $boa_get | awk '{for(i=1;i<=NF;i++) {if ($i ~ /PON_MODE=/) print $i}}'`
fiber_mode=`echo $boa_get | awk '{for(i=1;i<=NF;i++) {if ($i ~ /FIBER_MODE=/) {gsub(/FIBER_MODE=/,"", $i); print $i}}}'`
pon_dect_ctrl=`echo $boa_get | awk '{for(i=1;i<=NF;i++) {if ($i ~ /PON_DETECT_CONTROL_FLAG=/) {gsub(/PON_DETECT_CONTROL_FLAG=/,"",$i); print $i}}}'`
pon_dect_gpon_flag=`echo $boa_get | awk '{for(i=1;i<=NF;i++) {if ($i ~ /PON_DETECT_AVAILABLE_GPON_FLAG=/) {gsub(/PON_DETECT_AVAILABLE_GPON_FLAG=/,"",$i); print $i}}}'`
pon_dect_epon_flag=`echo $boa_get | awk '{for(i=1;i<=NF;i++) {if ($i ~ /PON_DETECT_AVAILABLE_EPON_FLAG=/) {gsub(/PON_DETECT_AVAILABLE_EPON_FLAG=/,"",$i); print $i}}}'`
pon_dect_eth_flag=`echo $boa_get | awk '{for(i=1;i<=NF;i++) {if ($i ~ /PON_DETECT_AVAILABLE_ETH_FLAG=/) {gsub(/PON_DETECT_AVAILABLE_ETH_FLAG=/,"",$i); print $i}}}'`
dualMgmt=`echo $boa_get | awk '{for(i=1;i<=NF;i++) {if ($i ~ /DUAL_MGMT_MODE=/) {gsub(/DUAL_MGMT_MODE=/,"",$i); print $i}}}'`

if [ "$pon_mode" == "PON_MODE=1" ]; then
    if [ -f  /lib/modules/omcidrv.ko ]; then
        insmod /lib/modules/omcidrv.ko
   
        if [ -f /lib/modules/pf_rt_fc.ko ]; then
            insmod /lib/modules/pf_rt_fc.ko
            ifconfig eth0 up
        else
            if [ -d /proc/rg ]; then
                if [ -f /lib/modules/pf_rg.ko ]; then
                    insmod /lib/modules/pf_rg.ko
                else
                    echo "Warning no exist rg platform"
                fi
            else
                if [ -f /lib/modules/pf_rtk.ko ]; then
                    insmod /lib/modules/pf_rtk.ko
                else
                    echo "Warning no exist rtk platform"
                fi
            fi
        fi
		
    else
        echo "Warning no insert OMCI module in GPON mode"
    fi

    if [ ! -f /var/config/run_customized_sdk.sh ]; then
        cp /etc/run_customized_sdk.sh /var/config/run_customized_sdk.sh
    fi
    /var/config/run_customized_sdk.sh
    
    if [ "$dualMgmt" == "1" ]; then
        if [ -f /lib/modules/rtk_tr142.ko ]; then
            echo "Loading TR-142 Module..."
            /bin/insmod /lib/modules/rtk_tr142.ko
            /bin/tr142_app
        else
            echo "/lib/modules/rtk_tr142.ko not found\n"
        fi
    fi
    
    if [ -f /bin/nice ]; then nice -n -18 /etc/runomci.sh ; else /etc/runomci.sh; fi

    /bin/pondetect $pon_dect_ctrl $pon_dect_gpon_flag $pon_dect_epon_flag $pon_dect_eth_flag &

    echo "running GPON mode ..."
    
elif [ "$pon_mode" == "PON_MODE=2" ]; then
    if [ -f /etc/scfg.txt ]; then
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

    else
        insmod /lib/modules/epon_drv.ko
        /etc/runoam.sh
        insmod /lib/modules/epon_polling.ko
        insmod /lib/modules/epon_mpcp.ko
		sleep 2
    fi
    
    /bin/pondetect $pon_dect_ctrl $pon_dect_gpon_flag $pon_dect_epon_flag $pon_dect_eth_flag &
    
    echo "running EPON mode ..."
    
elif [ "$pon_mode" == "PON_MODE=3" ]; then
    if [ ! -f /etc/scfg.txt ]; then
        echo $fiber_mode > proc/fiber_mode
    fi
else
    echo "running Ether mode..."
fi

if [ -f /lib/modules/rldp_drv.ko ]; then
    insmod /lib/modules/rldp_drv.ko
fi

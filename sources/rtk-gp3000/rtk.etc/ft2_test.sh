#!/bin/sh

#mount -t proc proc proc
#mount -t ramfs ramfs /var

reulst_file=/var/ft2_result

rm -rf $reulst_file
echo "Your choice is $1, PRBS Time $2";

LINE="================="

sleep 1

case $1 in
	"pcm")
		echo "Start to test PCM ...";
		echo 1 > /proc/ft2/pcm_test ;
		echo "$LINE";
		cat /proc/ft2/pcm_test;
		echo "$LINE";
	;;
	"prbs")
		echo "Start to test PRBS ...";
		echo 31 > /proc/ft2/prbs_test;
		sleep $2;
		echo "$LINE";
		cat /proc/ft2/prbs_test;
		echo "$LINE";
	;;
	"usb")
		echo "Start to test USB ..."
		
		echo "$LINE";
		/etc/usb.sh 1
		/etc/usb.sh 2
		echo "$LINE";
	;;
	"pcie")
		echo "Start to test PCIE ..."
		iwpriv wlan0 set_mib authtype=1;
		iwpriv wlan1 set_mib authtype=1;
		PCIE0=`iwpriv wlan0 get_mib authtype`
		PCIE0=`echo $PCIE0` 
		HOPE0="wlan0 get_mib: 00 00 00 01"
		PCIE1=`iwpriv wlan1 get_mib authtype`
                PCIE1=`echo $PCIE1`
                HOPE1="wlan1 get_mib: 00 00 00 01"
		echo "$LINE"
		if [ "$PCIE0" = "$HOPE0" ] ; then 
			echo "PCIE0 Test PASS"
			echo "PCIE0 Test PASS" >> $reulst_file
		else
			echo "PCIE0 Test FAIL"
			echo "PCIE0 Test FAIL" >> $reulst_file
		fi
		if [ "$PCIE1" = "$HOPE1" ] ; then
			echo "PCIE1 Test PASS"
			echo "PCIE1 Test PASS" >> $reulst_file
		else
			echo "PCIE1 Test FAIL"
			echo "PCIE1 Test FAIL" >> $reulst_file
		fi
		echo "$LINE"
	;;
	"init_olt")
		echo "Init OLT test"
		echo 2 > /proc/ft2/utp;
	;;
	"olt")
		echo "Start to OLT test ...";
		#echo 2 > /proc/ft2/utp;
		echo 31 > /proc/ft2/prbs_test;
		echo 1 > /proc/ft2/pcm_test;
		echo 1 > /proc/ft2/cpu;
		echo $3 > /proc/ft2/gpio;
		sleep $2
		#echo 0 > /proc/ft2/utp;
		echo 0 > /proc/ft2/cpu;
		echo 0 > /proc/ft2/gpio;		
	;;
	"all")
		echo "Start to test All ...";
		echo "";
		echo 1 > /proc/ft2/pcm_test ;
 		echo 31 > /proc/ft2/prbs_test;
		echo 1 > /proc/ft2/utp;
		echo 1 > /proc/ft2/cpu;
		echo $3 > /proc/ft2/gpio;
		sleep $2;
		echo "$LINE";
		#PCM
		cat /proc/ft2/pcm_test;
		#PRBS
		cat /proc/ft2/prbs_test;
		#USB
		/etc/usb.sh 1;
		/etc/usb.sh 2;
		#PCIE 
		iwpriv wlan0 set_mib authtype=1;
                iwpriv wlan1 set_mib authtype=1;
                PCIE0=`iwpriv wlan0 get_mib authtype`
                PCIE0=`echo $PCIE0`
                HOPE0="wlan0 get_mib: 00 00 00 01"
                PCIE1=`iwpriv wlan1 get_mib authtype`
                PCIE1=`echo $PCIE1`
                HOPE1="wlan1 get_mib: 00 00 00 01"
                if [ "$PCIE0" = "$HOPE0" ] ; then
                        echo "PCIE0 Test PASS"
			echo "PCIE0 Test PASS" >> $reulst_file
                else
                        echo "PCIE0 Test FAIL"
			echo "PCIE0 Test FAIL" >> $reulst_file
                fi
                if [ "$PCIE1" = "$HOPE1" ] ; then
                        echo "PCIE1 Test PASS"
			echo "PCIE1 Test PASS" >> $reulst_file
                else
                        echo "PCIE1 Test FAIL"
			echo "PCIE1 Test FAIL" >> $reulst_file
		fi
		#UTP
		sleep 1;
		cat /proc/ft2/utp;
		#GPIO
		cat /proc/ft2/gpio;
		echo "done.";
		echo "$LINE";
		echo 0 > /proc/ft2/utp
                echo 0 > /proc/ft2/cpu;
                echo 0 > /proc/ft2/gpio;

	;;
	*)
		echo "Usage:";
		echo "ft2.sh [pcm | prbs | usb | pcie |  all | olt] [prbs time] [gpio set]" ;
		exit;
	;;
esac

#!/bin/sh

LINE="================="

case $1 in
        "gphy")
		echo "$LINE";
                echo "Start to config GPHY TX ...";
		diag debug set ind-phy 0 0xa408 0x1e1;
		diag debug set ind-phy 0 0xa400 0x1340;
		while [ 1 ]
		do
			reg_valg=`diag debug get ind-phy 0 0xa402`;
			if [ $(( ${reg_valg:48:6} & 0x0004 )) -eq 4 ]; then
				break;
			fi 
			sleep 1;
		done	
                diag debug set ind-phy 0 0xc802 0x73;
		diag debug set ind-phy 0 0xc808 0x0;
		diag debug set ind-phy 0 0xc80a 0x0;
		diag debug set ind-phy 0 0xc800 0x5a23;
		echo 1 > /proc/ol_test/gphy;
                echo "$LINE";
        ;;
	"prbs")
		echo "$LINE";
		echo "Start to config GPON PRBS ...";
		diag register set 0x88 0x8;
		diag register set 0x22038 0x1;
		diag register set 0x22034 0xc0ff;
		diag register set 0x22034 0x40ff;
		sleep 1;
		echo "Start to config LAN port PRBS ...";
		diag register set 0x84 0x43;
		diag debug set ind-serdes 0 1 12 0x804;
		diag register set 0x64 0xb00;
		reg_val=`diag debug get ind-serdes 0 0 28`;
		reg_val1=$(( ( ${reg_val:61:10} & 0xFC00 ) + 0x101 ));
		diag debug set ind-serdes 0 0 28 $reg_val1; 
		diag debug set ind-serdes 0 1 4 0;
		diag debug set ind-serdes 0 1 5 0;
		echo "$LINE";
	;;
	"gpio")
	        echo "$LINE";
                echo "Start to config GPIO ...";
		echo 1 > /proc/ol_test/gpio;
		echo "$LINE";
	;;
	"led")
		echo "$LINE";
                echo "Start to config LED ...";
		diag register set 0x23000 0xf;
		diag register set 0x1e01c 0xf;
		diag register set 0x1e000 0x10f78;
		diag register set 0x1e004 0x10f78;
		diag register set 0x1e008 0x10f78;
		diag register set 0x1e00c 0x10f78;
		echo "$LINE";
	;;
	"pcm")
		echo "$LINE";
                echo "Start to test VoIP loop ...";
		#zsi
		echo 3 > /proc/ol_test/pcm;
		#isi
		#echo 4 > /proc/ol_test/pcm; 
		echo "$LINE";
	;;
	"cpu")
		echo "$LINE";
		echo "Start to config CPU ...";
		#Drop all packets from switch 
		brctl delif br0 eth0;
		#Disable source port block
		diag register set 0x1c078 1;
		echo "$LINE";
	;;
	"dram")
		echo "$LINE";
		echo "Start to test DRAM read write ...";
		echo 1 > /proc/ol_test/dram;
		echo "$LINE";
	;;
	"power")
		echo "$LINE";
		echo "Start to Change core power ...";
		diag debug set memory 0x1c8 0x150000;
		diag debug get memory 0x1c8;
		diag debug set memory 0x1c4 0x55ed;
		diag debug get memory 0x1c4;
		diag debug set memory 0x1cc 0xc;
		diag debug set memory 0x1cc 0xe;
		diag debug set memory 0x1cc 0x9;
		diag debug set memory 0x1cc 0xa;
		diag debug get memory 0x1c8;
		echo "$LINE";
	;;
	"all")
		
	        echo "$LINE";
                echo "Start to config CPU ...";
                #Drop all packets from switch
                brctl delif br0 eth0;
                #Disable source port block
                diag register set 0x1c078 1;
                echo "$LINE";
                sleep 1;
		echo "Start to Change core power ...";
                diag debug set memory 0x1c8 0x150000;
                diag debug get memory 0x1c8;
                diag debug set memory 0x1c4 0x55ed;
                diag debug get memory 0x1c4;
                diag debug set memory 0x1cc 0xc;
                diag debug set memory 0x1cc 0xe;
                diag debug set memory 0x1cc 0x9;
                diag debug set memory 0x1cc 0xa;
                diag debug get memory 0x1c8;
		echo "$LINE";
                sleep 1;
		echo "Start to config GPHY TX ...";
                diag debug set ind-phy 0 0xa408 0x1e1;
                diag debug set ind-phy 0 0xa400 0x1340;
                while [ 1 ]
                do
                        reg_valg=`diag debug get ind-phy 0 0xa402`;
                        if [ $(( ${reg_valg:48:6} & 0x0004 )) -eq 4 ]; then
                                break;
                        fi
                        sleep 1;
                done
		diag debug set ind-phy 0 0xc802 0x73;
                diag debug set ind-phy 0 0xc808 0x0;
                diag debug set ind-phy 0 0xc80a 0x0;
                diag debug set ind-phy 0 0xc800 0x5a23;
                echo 1 > /proc/ol_test/gphy;
                echo "$LINE";
		sleep 1;
                echo "Start to config GPON PRBS ...";
                diag register set 0x88 0x8;
                diag register set 0x22038 0x1;
                diag register set 0x22034 0xc0ff;
                diag register set 0x22034 0x40ff;
                sleep 1;
                echo "Start to config LAN port PRBS ...";
                diag register set 0x84 0x43;
                diag debug set ind-serdes 0 1 12 0x804;
                diag register set 0x64 0xb00;
                reg_val=`diag debug get ind-serdes 0 0 28`;
                reg_val1=$(( ( ${reg_val:61:10} & 0xFC00 ) + 0x101 ))
                diag debug set ind-serdes 0 0 28 $reg_val1;
                diag debug set ind-serdes 0 1 4 0;
                diag debug set ind-serdes 0 1 5 0;
                echo "$LINE";
		sleep 1;
                echo "Start to config GPIO ...";
                echo 1 > /proc/ol_test/gpio;
                echo "$LINE";
		sleep 1;
                echo "Start to config LED ...";
                diag register set 0x23000 0xf;
                diag register set 0x1e01c 0xf;
                diag register set 0x1e000 0x10f78;
                diag register set 0x1e004 0x10f78;
                diag register set 0x1e008 0x10f78;
                diag register set 0x1e00c 0x10f78;
                echo "$LINE";
		sleep 1;
                echo "Start to test VoIP loop ...";
                #zsi
		echo 3 > /proc/ol_test/pcm;
                echo "$LINE";
		sleep 1;
                echo "Start to test DRAM read write ...";
		echo 1 > /proc/ol_test/dram;
		echo "$LINE";

	;;
        *)
                echo "Usage:";
                echo "ol_test.sh [pcm | prbs | gphy | gpio |  led | cpu | dram | power | all]" ;
                exit;
        ;;
esac



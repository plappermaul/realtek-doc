#!/bin/sh

exeCmd="eponoamd "
i=0
result=`mib get EPON_LLID_TBL.$i.macAddr | awk -F '=' '/macAddr/ {print $2}'`
if [ "$result" == "" ]
then
    macaddr=`mib get ELAN_MAC_ADDR | sed 's/ELAN_MAC_ADDR=//g'`
    exeCmd=$exeCmd" -mac "$i" "$macaddr" "
    i=$((i+1))
	result=`mib get EPON_LLID_TBL.$i.macAddr | awk -F '=' '/macAddr/ {print $2}'`
fi

while [ "$result" != "" ]
do
	macaddr=$result

	exeCmd=$exeCmd"-mac "$i" "$macaddr" "

	i=$((i+1))
	result=`mib get EPON_LLID_TBL.$i.macAddr | awk -F '=' '/macAddr/ {print $2}'`

done

$exeCmd &


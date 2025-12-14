#!/bin/sh

USB_TEST_FILE="/image/dsp.img"
USB_TEM_DIR="/var/usb"
USB_TEST="/var/c"
CHECK_FILE="/var/checkfile"

result_file="/var/ft2_result"

cp $USB_TEST_FILE $USB_TEST 2>$CHECK_FILE
result=`cat $CHECK_FILE`

if [ "$result" != "" ] ; then
	echo "USB$1 Test FAIL!"
	echo "USB$1 Test FAIL!" >> $result_file 
	rm -rf $CHECK_FILE
	exit
fi

mkdir $USB_TEM_DIR 2>$CHECK_FILE
result=`cat $CHECK_FILE`

if [ "$result" != "" ] ; then
	echo "USB$1 Test FAIL!"
	echo "USB$1 Test FAIL!" >> $result_file
	rm -rf $CHECK_FILE
	rm -rf $USB_TEST
	exit
fi

if [ "$1" = "1" ] ; then
	dev=/dev/sda1
else
	dev=/dev/sdb1
fi

mount $dev /var/usb 2> $CHECK_FILE
result=`cat $CHECK_FILE`

if [ "$result" != "" ] ; then
	echo "USB$1 Test FAIL!"
	echo "USB$1 Test FAIL!" >> $result_file
	rm -rf $CHECK_FILE
	rm -rf $USB_TEM_DIR
	rm -rf $USB_TEST
	exit
fi

cp $USB_TEST $USB_TEM_DIR/a 2> $CHECK_FILE;
result=`cat $CHECK_FILE`;

if [ "$result" != "" ] ; then
	echo "USB$1 Test FAIL"
	echo "USB$1 Test FAIL!" >> $result_file
	rm -rf $CHECK_FILE
	rm -rf $USB_TEM_DIR
	rm -rf $USB_TEST
	exit
fi

sleep 1

usbw=`diff $USB_TEST $USB_TEM_DIR/a 2>$CHECK_FILE`
result=`cat $CHECK_FILE`;

if [ "$result" != "" ]; then
	echo "USB$1 Test FAIL"
	echo "USB$1 Test FAIL!" >> $result_file
	rm -rf $CHECK_FILE
	rm -rf $USB_TEM_DIR
	rm -rf $USB_TEST
	exit
fi

if [ $usbw != "" ]; then 
	echo "USB$1 Test FAIL!"
	echo "USB$1 Test FAIL!" >> $result_file
	umount $dev 2>/dev/null
	rm -rf $USB_TEM_DIR
	rm -rf $USB_TEST
	exit
fi

cp $USB_TEM_DIR/a /var/b 2> $CHECK_FILE
result=`cat $CHECK_FILE`

if [ "$result" != "" ]; then
	echo "USB$1 Test FAIL!"
	echo "USB$1 Test FAIL!" >> $result_file
	rm -rf $CHECK_FILE
	rm -rf $USB_TEM_DIR
	rm -rf $USB_TEST
fi

usbr=`diff $USB_TEM_DIR/a /var/b 2>$CHECK_FILE`
result=`cat $CHECK_FILE`


if [ "$result" != "" ]; then
        echo "USB$1 Test FAIL!"
	echo "USB$1 Test FAIL!" >> $result_file
        rm -rf $CHECK_FILE
        rm -rf $USB_TEM_DIR
        rm -rf $USB_TEST
fi


if [ $usbr != "" ]; then
	echo "USB$1 Test FAIL!"
	echo "USB$1 Test FAIL!" >> $result_file
	umount $dev 2>/dev/null
	rm -rf $USB_TEM_DIR
	rm -rf $USB_TEST
	rm -rf /var/b
	exit
fi

echo "USB$1 Test PASS";
echo "USB$1 Test PASS" >> $result_file;


umount $dev 2>/dev/null
rm -rf /var/b;
rm -rf /var/usb/a;
rm -rf /var/usb
rm -rf /var/c;

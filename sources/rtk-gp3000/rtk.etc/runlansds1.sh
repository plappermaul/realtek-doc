#!/bin/sh

lan_sds1_mode=`mib get LAN_SDS1_MODE | sed 's/LAN_SDS1_MODE=//g'`
echo $lan_sds1_mode > proc/lan_sds/lan_sds1_cfg


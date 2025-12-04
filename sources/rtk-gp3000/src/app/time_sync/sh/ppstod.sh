#!/bin/sh

case "$1" in
    start)
        echo -n "Starting 1PPSToD"
        echo 1 > /proc/ppstod_drv/enable
        echo "."
        ;;
    stop) 
        echo -n "Stopping 1PPSToD"
        echo 0 > /proc/ppstod_drv/enable
        echo "."
        ;;
    restart)
        echo -n "Stopping 1PPSToD"
        echo 0 > /proc/ppstod_drv/enable
        echo "."
        echo -n "Starting 1PPSToD"
        echo 1 > /proc/ppstod_drv/enable
        echo "."
        ;;
    *)
        echo "Usage: /etc/ppstod.sh start|stop|restart"
        exit 1
        ;;
esac

#!/bin/sh

case "$1" in
    start)
        echo -n "Starting PTP Master"
        pidof ptpmaster 2>/dev/null 1>/dev/null || /bin/ptpmaster
        echo "."
        ;;
    stop) 
        echo -n "Stopping PTP Master"
        killall ptpmaster 2>/dev/null 1>/dev/null
        echo "."
        ;;
    restart)
        echo -n "Stopping PTP Master"
        killall ptpmaster 2>/dev/null 1>/dev/null
        echo "."
        echo -n "Starting PTP Master"
        pidof ptpmaster 2>/dev/null 1>/dev/null || /bin/ptpmaster
        echo "."
        ;;
    *)
        echo "Usage: /etc/ptpmaster.sh start|stop|restart"
        exit 1
        ;;
esac

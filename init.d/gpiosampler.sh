#!/bin/sh

### BEGIN INIT INFO
#Provides:          gpiosampler
#Required-Start:    $all
#Required-Stop:     $all
#Default-Start:     2 3 4 5
#Default-Stop:      0 1 6
#Short-Description: Simple script to start gpiosampler at boot
#Description:       [...]
### END INIT INFO

case "$1" in
    start)
        /usr/bin/gpiosampler.sh
        ;;
    stop)
        killall gpiosampler
        ;;
    *)
        exit 1
        ;;
esac 
exit 0


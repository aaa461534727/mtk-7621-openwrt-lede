#!/bin/sh
export PATH=/bin:/sbin:/usr/bin:/usr/sbin
BOOT_TIMES=/etc/config/boot_times


if [ ! -f $BOOT_TIMES ] ;then
        echo "boot_times=1" > $BOOT_TIMES
else
. $BOOT_TIMES

        boot_times=`expr  $boot_times + 1`
        echo "boot_times=$boot_times" > $BOOT_TIMES
fi

#reset button

# File Name: init_all.sh
# Author: van9ogh
# mail: van9ogh(alt)gmail(dot)com
# Created Time: Mon 08 Oct 2012 09:46:28 PM CST

#!/bin/bash

PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin
export PATH

#make

TGX_ROOT=`pwd`
TGX_SENSOR=$TGX_ROOT/src/sensor
TGX_DB=$TGX_ROOT/src/db
TGX_WEB=$TGX_ROOT/bin

if [ -f "$TGX_DB/db_init.sh" ]; then
	cd $TGX_DB
	./db_init.sh
	echo "db initialized success.\n"
else
	echo "db can not initialized..\n"
	exit 0
fi

cd $TGX_SENSOR
#make
./tgx_sensor_server 1234 1235 > /dev/null 2>&1 &
echo "tinyos-graphics sensor server start done.\n"

cd $TGX_WEB
./tinyos-graphics

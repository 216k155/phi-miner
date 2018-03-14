#!/bin/bash
#
#USE THIS STARTUP SCRIPT TO AUTOMATICALLY MINE THE MOST PROFITABLE COIN IN ZPOOL UNDER LINUX

while :
do



ccminer \
-a phi \
-u LYoJEAkfzm82MH5DoSQRLthChWa6c368r1 \
-o stratum+tcp://bpool.io:3033 \
-d 1

sleep 5
done

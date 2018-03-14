#!/bin/bash

while :
do

ccminer \
-a phi \
-u your-wallet-address \
-p 12 \
-o bpool.io:3033 \
-t 2 \
-d 0


sleep 5
done

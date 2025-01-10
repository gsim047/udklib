#!/bin/bash

# get $SCAN == snapscan:libusb:003:003
SCAN=snapscan:libusb:003:003

#DAT1=`date +%Y-%m-%d`
DAT2=`date +%Y%m%d-%H%M%S`

#mkdir -p ~/scan
scanimage -d $SCAN --format=jpeg -o /var/www/html/scan_/scan/scan-$DAT2.jpg &


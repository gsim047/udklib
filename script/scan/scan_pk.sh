#!/bin/bash

rm -f /var/www/html/scan_dl.zip
cd /var/www/html/scan_/scan
7z a /var/www/html/scan_dl.zip scan-*.jpg


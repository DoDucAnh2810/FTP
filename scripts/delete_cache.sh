#!/bin/bash

find Clusters -mindepth 2 ! -name ".port" -delete
find Clients -type f ! -name "README.md" -delete
rm -f ftpclient ftpserver ftpmaster
rm -f Clusters/ftpserver
rm -f Master/ftpmaster 
rm -f Master/log.txt
rm -f *.o

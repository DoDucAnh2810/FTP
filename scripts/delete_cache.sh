#!/bin/bash

rm -f *.o
rm -f Master/ftpmaster
find Clients -type f -delete
find . -name "log.txt" -exec rm -f {} +
find Clusters -type f -name "ftpserver" -delete

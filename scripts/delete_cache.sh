#!/bin/bash

find Clusters -mindepth 2 ! -name ".port" -delete
rm -f Clusters/ftpserver
rm -f Master/ftpmaster 
rm -f Master/log.txt
rm -f Clients/*
rm -f *.o

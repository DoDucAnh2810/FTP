#!/bin/bash

if [[ ! -e ftpclient ]] || [[ ! -e ftpserver ]] || [[ ! -e ftpmaster ]]
then
    make
    exit
fi

mv ftpclient Clients
cd Clients
for dir in */
do
    if [[ -d "$dir" ]]
    then
        cp ftpclient "$dir"
    fi 
done
cd ..

mv ftpserver Clusters
cd Clusters
for dir in */
do
    if [[ -d "$dir" ]]
    then
        cp -r ../Files/* $dir
        cp ftpserver "$dir"
    fi 
done
cd ..

mv ftpmaster Master
#!/bin/bash

for dir in Clusters/*/
do
    if [[ -d "$dir" ]]
    then
        cd "$dir"
        if [[ ! -x "ftpserver" ]]
        then
            echo "Please run 'make' first"
            cd ../..
            exit 1
        fi
        stdbuf -o0 ./ftpserver $(<.port) > log.txt &
        cd ../..
    fi
done

cd Master
if [[ ! -x "ftpmaster" ]]
then
    echo "Please run 'make' first"
    cd ..
    exit 1
fi
stdbuf -o0 ./ftpmaster > log.txt &
cd ..

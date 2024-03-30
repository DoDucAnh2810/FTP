#!/bin/bash

cd Clients
if [[ ! -x "ftpclient" ]]
then
    echo "Please run 'make' first"
    cd ..
    exit 1
fi
./ftpclient localhost
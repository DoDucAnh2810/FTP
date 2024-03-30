#!/bin/bash

for dir in Clusters/*/
do
    if [[ -d  "$dir" ]]
    then
        find "$dir" -mindepth 1 ! -name ".port" -delete
        cp -r Files/* $dir
    fi
done
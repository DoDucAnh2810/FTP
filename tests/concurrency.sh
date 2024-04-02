#!/bin/bash

echo "Concurrency test: Dowload all pictures from server (34MB total)"
echo "Available servers: 4 clusters, each with a process pool of size 2"

echo

echo "Time required to handle 1 client:"
time {
    make client < tests/pictures.test > /dev/null
}

echo

echo "Time required to handle 8 clients:"
time {
    make client < tests/pictures.test > /dev/null & 
    make client < tests/pictures.test > /dev/null &
    make client < tests/pictures.test > /dev/null &
    make client < tests/pictures.test > /dev/null &
    make client < tests/pictures.test > /dev/null &
    make client < tests/pictures.test > /dev/null &
    make client < tests/pictures.test > /dev/null &
    make client < tests/pictures.test > /dev/null
}

echo

echo "For more information, read 'log.txt' found at Master and Clusters"
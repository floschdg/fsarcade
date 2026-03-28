#!/bin/sh

cd runtree

if [[ "$1" == "debug" ]] then
    gdb ./fsarcade
else
    ./fsarcade
fi


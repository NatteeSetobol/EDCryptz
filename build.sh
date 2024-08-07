#!/bin/bash

MAIN_FILE="../main.cpp"
COMPILER="g++"
MACRO="-DDEBUG"
CPP="../cutil.cpp ../Encryption.cpp ../aes.c ../ErrorHandle.cpp"
FLAGS="-w -g -fsanitize=address"
OUTPUT="edcryptz"

if [ -d "build" ]
then
    rm -rf build
fi

mkdir build
cd build
$COMPILER $MACRO $MAIN_FILE $CPP $FLAGS -o $OUTPUT
cd ..
#!/bin/bash

cd ../
gcc -o build/test/test.o src/test/test.c
cd build/test
./test

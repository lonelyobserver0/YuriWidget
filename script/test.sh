#!/bin/bash

gcc -o ../build/test/test src/test/test.c
cd ../build/test
./test

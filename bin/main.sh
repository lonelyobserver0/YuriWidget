#!/bin/bash
cd ../
make
cd build
./myVandal 920 0 1000 300 100 file://$HOME/.config/Vandal/widget.html

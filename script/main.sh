#!/bin/bash
cd ../
make
mv build/myVandal bin
cd bin
./myVandal 920 0 1000 300 100 file://$HOME/.config/Vandal/widget.html

#!/bin/bash
cd ../
make
mv build/yuriwidget bin
cd bin
./myVandal -t niermenuwidget -cf file://$HOME/.config/yuriwidget/widget.html
 
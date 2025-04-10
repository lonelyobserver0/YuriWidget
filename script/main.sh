#!/bin/bash
cd ../
make
cd build
./yuriwidget -t niermenuwidget -cf file://$HOME/.config/yuriwidget/widget.html
 
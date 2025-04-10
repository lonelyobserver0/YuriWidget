#!/bin/bash
cd ../
make
cd build
./yuriwidget --title niermenuwidget --config-file file://$HOME/.config/yuriwidget/widget.html
 
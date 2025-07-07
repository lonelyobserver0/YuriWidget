#!/bin/bash

make
cd build
./yuriwidget --title niermenuwidget --config-file file://$HOME/.config/yuriwidget/nier/widget.html --width 1920 --height 1080 --x 0 --y 0

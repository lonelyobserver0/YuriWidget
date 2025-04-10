#!/bin/bash
gcc `pkg-config --cflags gtk4 webkit2gtk-4.0` ../src/yuriwidget.c -o ../build/yuriwidget `pkg-config --libs gtk4 webkit2gtk-4.0` -pthread

#!/bin/bash

gcc yuriwidget.c -o yuriwidget `pkg-config --cflags --libs gtk4`

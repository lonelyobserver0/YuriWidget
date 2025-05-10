#!/bin/bash

gcc yuriwidget.c deps/tomilc99/toml.c -o yuriwidget `pkg-config --cflags --libs gtk4`

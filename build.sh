#!/bin/bash

gcc -I./deps/tomlc99 yuriwidget.c deps/tomlc99/toml.c -o yuriwidget `pkg-config --cflags --libs gtk4`

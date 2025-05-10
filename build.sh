#!/bin/bash

gcc yuriwidget.c deps/tomlc99/toml.c -o yuriwidget `pkg-config --cflags --libs gtk4`

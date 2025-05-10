#!/bin/bash

gcc yuriwidget.c toml.c -o yuriwidget `pkg-config --cflags --libs gtk4`

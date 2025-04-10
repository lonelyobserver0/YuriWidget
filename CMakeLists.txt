cmake_minimum_required(VERSION 3.10)
project(myVandal C)

include_directories(/usr/include)
include_directories(/usr/include/gtk-4.0)
include_directories(/usr/include/webkitgtk-4.0)

add_executable(myVandal src/main.c)

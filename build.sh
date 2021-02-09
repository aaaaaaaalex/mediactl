#!/bin/sh

# if you really want to squeeze your performance, use -mtune=native and build from source :D
gcc -mtune=generic -march=x86-64\
    -I. $(pkg-config --cflags dbus-1) main.c ./mpris/* ./util/* \
    $(pkg-config --libs dbus-1) \
    -g -O0\
    -o mediactl

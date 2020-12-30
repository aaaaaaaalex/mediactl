#!/bin/sh
gcc -I. $(pkg-config --cflags dbus-1) main.c ./mpris/* ./util/* \
    $(pkg-config --libs dbus-1) \
    -g -O0\
    -o playplause
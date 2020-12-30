#!/bin/sh
gcc $(pkg-config --cflags dbus-1) main.c\
    $(pkg-config --libs dbus-1)\
    -g -O0\
    -o playplause
#!/bin/sh
set -e
mkdir -r ../dist
gcc ../src/main.c     \
    ../src/util.c     \
    ../src/vector.c   \
    ../src/parser.c   \
    -o ../dist/buzzer \
    -llgpio           \ # This is the library that we will use to control the GPIO pins
    -lm               \ # This is the math library
    -Wall             \
    -O2
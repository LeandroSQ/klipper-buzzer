#!/bin/sh
set -e
mkdir -r ../dist
gcc ../src/main.c     \
    ../src/util.c     \
    ../src/vector.c   \
    ../src/parser.c   \
    -o ../dist/buzzer \
    -llgpio           \ # Link LGPIO
    -lm               \ # Link math
    -Wall             \
    -O2               \
    -v
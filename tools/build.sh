#!/bin/sh
set -e

mkdir -p dist

# If -v is one of the args, add -DVERBOSE define
VERBOSE_FLAG=""
for arg in "$@"; do
  if [ "$arg" = "-v" ]; then
    VERBOSE_FLAG="-DVERBOSE"
    break
  fi
done

gcc src/main.c     \
    src/util.c     \
    src/vector.c   \
    src/parser.c   \
    -o dist/buzzer \
    -llgpio        \
    -lm            \
    -Wall          \
    -O2            \
    $VERBOSE_FLAG  \
    -v             \
    "$@"

#!/bin/sh
# This script will copy all files in ./v2 to the buzzer server
# Usage: ./copy.sh

set -e
rsync -av --exclude "include" --exclude "lg" . biqu@192.168.25.120:/home/biqu/buzzer/v2
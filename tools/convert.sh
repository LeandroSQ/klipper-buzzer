#!/bin/sh
# This script will convert a text file from M300 S<note> P<time> to <note> <time>
# Usage: ./convert.sh <input_file> [<output_file>]

set -e

# Validate input
if [ "$#" -lt 1 ] || [ "$#" -gt 2 ]; then
    echo "Usage: ./convert.sh <input_file> [<output_file>]"
    exit 1
fi

# If no output file is provided, overwrite the input file
if [ "$#" -eq 1 ]; then
    echo "This will overwrite the input file! are you sure? (y/n)"
    read -r answer
    if [ "$answer" != "y" ]; then
        exit 1
    fi

    output_file="$1"
else
    output_file="$2"
fi

input_file="$1"
output_file="$2"

# Process the input file and write to the output file
echo "0" > "$output_file" # Pitch shift, starts at 0
while IFS= read -r line; do
    note=$(echo "$line" | sed -n 's/^M300 S\([0-9]*\) P\([0-9]*\)$/\1/p')
    time=$(echo "$line" | sed -n 's/^M300 S\([0-9]*\) P\([0-9]*\)$/\2/p')
    if [ -n "$note" ] && [ -n "$time" ]; then
        echo "$note $time" >> "$output_file"
    fi
done < "$input_file"

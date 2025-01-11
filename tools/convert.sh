#!/bin/sh
# This script will convert a text file from M300 S<note> P<time> to <note> <time>
# Usage: ./convert.sh <input_file> [<output_file>]

set -e

# Validate input
if [ "$#" -lt 1 ] || [ "$#" -gt 2 ]; then
    echo "Usage: ./convert.sh <input_file> [<output_file>]"
    exit 1
fi

input_file="$1"
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


# Create a temporary buffer file
buffer_file=$(mktemp)

# Write the header to the buffer file
echo "0" > "$buffer_file" # Pitch

# Read the input file line by line
while IFS= read -r line; do
    note=$(echo "$line" | sed -n 's/.*S\([0-9]*\).*/\1/p')
    time=$(echo "$line" | sed -n 's/.*P\([0-9]*\).*/\1/p')
    if [ -n "$note" ] && [ -n "$time" ]; then
        echo "Note: $note, Time: $time"
        echo "$note $time" >> "$buffer_file"
    else
        echo "Skipping line: $line!" >&2
    fi
done < "$input_file"

# Move the buffer file to the output file
mv "$buffer_file" "$output_file"

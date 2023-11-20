#!/bin/bash

# Compile the programs
gcc main.c -o main
gcc sender.c -o sender
gcc reader.c -o reader

# Run the father program
./master

echo "Programs executed"

rm -f master reader sender
#!/bin/bash

# Compile the programs
gcc master.c -o master
gcc writer.c -o writer
gcc reader.c -o reader

# Run the father program
./master

echo "Programs executed"
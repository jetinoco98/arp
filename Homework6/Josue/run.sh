#!/bin/bash

# Compile the programs
gcc master.c -o master
gcc sender.c -o sender
gcc reader.c -o reader

# Run the father program
./master

echo "Programs executed"
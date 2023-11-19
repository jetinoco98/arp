#!/bin/bash

# Compile the programs
gcc father.c -o father
gcc writer.c -o writer

# Run the father program
./father

echo "Programs executed"
#!/bin/bash

# Compile the programs
gcc father.c -o father
gcc child.c -o child

# Run the father program
./father

echo "Programs executed"
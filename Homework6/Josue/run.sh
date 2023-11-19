#!/bin/bash

# Compile the programs
gcc father.c -o father -lrt
gcc child.c -o child -lrt

# Run the father program
./father

# Clean up compiled files
rm -f father child

echo "Programs executed and cleanup complete."
#!/bin/bash

# Compile the C file
gcc -o father father.c 
gcc -o reader reader.c 
gcc -o writer writer.c

# Run the compiled program
./father

#!/bin/bash
# Default values
SIZE=0.1  # Changed from 1 to 0.1 GB
OUTPUT="rdrand_output.bin"

# Run the rdrand_test program
rdrand_test "$SIZE" "$OUTPUT" 
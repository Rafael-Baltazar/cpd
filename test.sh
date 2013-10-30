#!/bin/bash

#Execute the tests...
#./wolves-squirrels-serial tests/ex3.in 	3 4 4 4 > tests/out_serial
#diff tests/out tests/out_seriali

# Run sequencial version
echo "Sequencial version"
time ./wolves-squirrels-serial tests/t1.in 500 500 1000 1000000 > tests/out_serial

echo " "

# Run parallel version
echo "Parallel version"
time ./wolves-squirrels-parallel tests/t1.in 500 500 1000 1000000 > tests/out_parallel

# Compare both
diff tests/out_serial tests/out_parallel

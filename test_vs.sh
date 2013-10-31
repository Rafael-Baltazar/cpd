#!/bin/bash

#Execute the tests...
#./wolves-squirrels-serial tests/ex3.in 	3 4 4 4 > tests/out_serial
#diff tests/out tests/out_seriali



#My own tests
PARAMETERS="500 500 1000 1000000"

FILES=tests/*.in
for f in $FILES
do
   	echo "-------------"
	echo "Test file: $f"
	# take action on each file. $f store current file name
	# Run sequencial version
	echo "Sequencial version"
	time ./wolves-squirrels-serial $f $PARAMETERS > tests/out_serial
	echo " "
	# Run parallel version
	echo "Parallel version"
	time ./wolves-squirrels-parallel $f $PARAMETERS > tests/out_parallel
	# Compare both
	echo "Differences:"
	diff tests/out_serial tests/out_parallel
done


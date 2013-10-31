#!/bin/bash

#Execute the tests...
#./wolves-squirrels-serial tests/ex3.in 	3 4 4 4 > tests/out_serial
#diff tests/out tests/out_seriali


#Default Tests
TEST1="world_10"
TEST2="world_100"
TEST3="world_1000"
TEST4="world_10000"

PARAMETERS_T1_1="20 5 30 5"
PARAMETERS_T1_2="20 50 30 1000000"
PARAMETERS_T2_1="20 50 30 100000"
PARAMETERS_T3_1="20 50 30 100000"
PARAMETERS_T4_1="20 50 30 100"

TEST1_1_OUT="world_10_1.out"
TEST1_2_OUT="world_10_2.out"


echo "Run tests purposed by teacher"
echo "-------------"
echo "$TEST1.in"
echo "Sequencial version"
time ./wolves-squirrels-serial tests/$TEST1.in $PARAMETERS_T1_1 > tests/out/out_serial1_1
echo "Diferences:"
diff tests/$TEST1_1_OUT tests/out/out_serial1_1
echo "Parallel version"
time ./wolves-squirrels-parallel tests/$TEST1.in $PARAMETERS_T1_1 > tests/out/out_parallel1_1
echo "Diferences"
diff tests/$TEST1_1_OUT tests/out/out_parallel1_1
echo "Sequencial vs Parallel:"
diff tests/out/out_serial1_1 tests/out/out_parallel1_1

echo "-------------"
echo "$TEST1.in"
echo "Sequencial version"
time ./wolves-squirrels-serial tests/$TEST1.in $PARAMETERS_T1_2 > tests/out/out_serial1_2
echo "Diferences:"
diff tests/$TEST1_2_OUT tests/out/out_serial1_2
echo "Parallel version"
time ./wolves-squirrels-parallel tests/$TEST1.in $PARAMETERS_T1_2 > tests/out/out_parallel1_2
echo "Diferences"
diff tests/$TEST1_2_OUT tests/out/out_parallel1_2
echo "Sequencial vs Parallel:"
diff tests/out/out_serial1_2 tests/out/out_parallel1_2

echo "-------------"
echo "$TEST2.in"
echo "Sequencial version"
time ./wolves-squirrels-serial tests/$TEST2.in $PARAMETERS_T2_1 > tests/out/out_serial2
echo "Diferences:"
diff tests/$TEST2.out tests/out/out_serial2
echo "Parallel version"
time ./wolves-squirrels-parallel tests/$TEST2.in $PARAMETERS_T2_1 > tests/out/out_parallel2
echo "Diferences"
diff tests/$TEST2.out tests/out/out_parallel2
echo "Sequencial vs Parallel:"
diff tests/out/out_serial2 tests/out/out_parallel2

echo "-------------"
echo "$TEST3.in"
echo "Sequencial version"
time ./wolves-squirrels-serial tests/$TEST3.in $PARAMETERS_T3_1 > tests/out/out_serial3
echo "Diferences:"
diff tests/$TEST3.out tests/out/out_serial3
echo "Parallel version"
time ./wolves-squirrels-parallel tests/$TEST3.in $PARAMETERS_T3_1 > tests/out/out_parallel3
echo "Diferences"
diff tests/$TEST3.out tests/out/out_parallel3
echo "Sequencial vs Parallel:"
diff tests/out/out_serial3 tests/out/out_parallel3

echo "-------------"
echo "$TEST4.in"
echo "Sequencial version"
time ./wolves-squirrels-serial tests/$TEST4.in $PARAMETERS_T4_1 > tests/out/out_serial4
echo "Diferences:"
diff tests/$TEST4.out tests/out/out_serial4
echo "Parallel version"
time ./wolves-squirrels-parallel tests/$TEST4.in $PARAMETERS_T4_1 > tests/out/out_parallel4
echo "Diferences"
diff tests/$TEST4.out tests/out/out_parallel4
echo "Sequencial vs Parallel:"
diff tests/out/out_serial4 tests/out/out_parallel4



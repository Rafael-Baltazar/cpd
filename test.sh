#!/bin/bash

#Execute the tests...
./wolves-squirrels-serial tests/ex3.in 	3 4 4 4 > tests/out_serial
diff tests/out tests/out_serial

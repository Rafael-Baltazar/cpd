#!/bin/bash

USER="ist169350"

N_PROCS=4

for file in submit*_$N_PROCS
do
		echo "Test: $file"
		condor_submit $file
done

condor_q $USER

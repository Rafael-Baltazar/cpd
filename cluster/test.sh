#!/bin/bash

USER="ist169350"

N_PROCS=4

echo "Number of processes:"
read nprocs

for file in submit*_$nprocs
do
		echo "Test: $file processes $nprocs"
		condor_submit $file
done

condor_q $USER

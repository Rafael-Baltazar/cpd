#!/bin/bash

USER="ist169350"

for file in submit*
do
		echo "Test: $file"
		condor_submit file
done

condor_q $USER

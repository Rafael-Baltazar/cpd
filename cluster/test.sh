#!/bin/bash

USER="ist169350"


for file in submit*
do
	echo "Submit to condor: $file"
	condor_submit $file
done

condor_q $USER

#!/bin/bash
echo generating data
./gendata square 3 100000 0.1
echo training models
for learningrate in 0.001 0.2 0.3
do
	for batchsize in 5 10 15 25 50
	do
		for numnodes in 100
		do
			echo training $learningrate $batchsize $numnodes
			python trainmodel.py square 3 100000 0.1 $numnodes $batchsize $learningrate
		done
	done
done
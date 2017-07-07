#!/bin/bash
echo generating data
./gendata square 5 100000 0.1
echo training models
for learningrate in 0.002 0.01 0.05 0.1 
do
	for batchsize in 5 10 20 40
	do
		for numnodes in 50 100 200 400
		do
			echo training $learningrate $batchsize $numnodes
			python trainmodel.py square 5 100000 0.1 $numnodes $batchsize $learningrate results/opt5.csv
		done
	done
done
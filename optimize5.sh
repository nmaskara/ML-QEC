#!/bin/bash -x
latsize=4
numtrials=200000
p=0.05
ptxt=50
filename="results.csv"
echo generating data
./gendata square $latsize $numtrials $p
echo training models
for learningrate in 0.01
do
	for batchsize in 1024
	do
		for numnodes in 1000
		do
			echo training $learningrate $batchsize $numnodes
			python trainmodel.py square $latsize $numtrials $p $numnodes $batchsize $learningrate $filename
		done
	done
done
sudo shutdown -h now
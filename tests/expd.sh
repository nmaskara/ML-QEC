#!/bin/bash -x
latsize=5
numtrials=10000000
p1=0.2
ptxt=100

cd surf2017
./gendata square $latsize $numtrials $p1 -r
./gendata square $latsize 100000 $p1 -r
python trainmodel.py square $latsize $numtrials 400 1000 0.01 \
	square_5_10000000_200_random square_5_100000_200_random
aws s3 cp models/square_5_10000000_200_random_300_1000_10.hdf5 \
	s3://nmaskara-models/square_5_10000000_200_random_300_1000_10.hdf5
aws s3 cp log2.txt s3://nmaskara-models/results/log3.txt
sudo shutdown -h now

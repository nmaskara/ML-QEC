#!/bin/bash -x
latsize=5
numtrials=1000000
p1=0.2
ptxt=100

./gendata square $latsize $numtrials $p1 -r
./gendata square $latsize 100000 $p1 -r
python trainmodel.py square $latsize $numtrials 300 1000 0.01 \
	square_5_1000000_200_random square_5_1000000_200_random
aws s3 cp models/square_5_1000000_200_300_1000_10.hdf5 \
	s3://nmaskara-models/square_5_1000000_200_300_1000_10.hdf5
shutdown -h now

#!/bin/bash -x
latsize=5
numtrials=100000
p1=0.1
ptxt=100

#./gendata square $latsize $numtrials $p1
#python trainmodel.py square $latsize $numtrials 0.1 1600 1024 0.01 \
#	square_5_100000_100
aws s3 cp models/square_5_100000_100_1600_1024_10.hdf5 \
	s3://nmaskara-models/square_5_100000_100_800_1024_10.hdf5
shutdown -h now
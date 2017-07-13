#!/bin/bash -x
lattype='square'
latsize=3
p=0.1
ptxt=100

#batchsize=100
learningrate=0.01
lrtxt=10
numlayers=1
datasize=$1
valsize=$((datasize/10 + 1))

./gendata $lattype $latsize $valsize $p
valname=$lattype'_'$latsize'_'$valsize'_'$ptxt

./gendata $lattype $latsize $datasize $p
dataname=$lattype'_'$latsize'_'$datasize'_'$ptxt
batchsize=$((datasize/1000))
for numnodes in 5 10 20 50 100 200 500
do
	python trainmodel.py $lattype 'sgd' $latsize $datasize $numnodes \
	$numlayers $batchsize $learningrate $dataname $valname

	aws s3 cp 'models/'$dataname'_'$numnodes'_'$batchsize'_'$lrtxt'_sgd.hdf5' \
	s3://nmaskara-models/$dataname'_'$numnodes'_'$batchsize'_'$lrtxt'_sgd.hdf5'

	aws s3 cp 'results/'$dataname'_'$numnodes'_'$batchsize'_'$lrtxt'_sgd.csv' \
	s3://nmaskara-models/results/$dataname'_'$numnodes'_'$batchsize'_'$lrtxt'_sgd.csv'
done

sudo shutdown -h now
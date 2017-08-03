#!/bin/bash -x
lattype='square'
latsize=7
p=0.1
ptxt=100
opt='adam'

batchsize=1000
learningrate=0.01
lrtxt=10
kernelsize=4
numfilters=4
numlayers=0
numnodes=100

datasize=50000000
valsize=100000
steps=2000
epochs=500

dt=$(date '+%Y-%m-%d_%H-%M-%S')
mkdir 'models/'$dt
mkdir 'results/'$dt


#./gendata $lattype $latsize $valsize $p
valname=$lattype'_'$latsize'_'$valsize'_'$ptxt

#./gendata $lattype $latsize $datasize $p
dataname=$lattype'_'$latsize'_'$datasize'_'$ptxt
#batchsize=$((datasize/1000))
for numnodes in 60
do
	python cnnwraparound.py $lattype $opt $latsize $steps $epochs $kernelsize $numfilters \
	$numnodes $numlayers $batchsize $learningrate $dataname $valname $dt

	aws s3 cp 'models/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$lrtxt'_'$opt'.hdf5' \
	s3://nmaskara-models/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$lrtxt'_'$opt'.hdf5'

	aws s3 cp 'results/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$lrtxt'_'$opt'.csv' \
	s3://nmaskara-models/results/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$lrtxt'_'$opt'.csv'
done


echo 'done'
#sudo shutdown -h now

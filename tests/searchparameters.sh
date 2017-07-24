#!/bin/bash -x
if [ $# -eq 0 ]
	then
		echo 'not enough arguments'
		exit 1
fi
lattype='square'
latsize=3
p=0.05
ptxt=50

batchsize=10
learningrate=0.01
opt='sgd'
lrtxt=10
numlayers=1
datasize=$1
valsize=$((datasize-1))

steps=1000
epochs=50

dt=$(date '+%Y-%m-%d_%H-%M-%S')
mkdir 'models/'$dt
mkdir 'results/'$dt


./gendata $lattype $latsize $valsize $p
valname=$lattype'_'$latsize'_'$valsize'_'$ptxt

./gendata $lattype $latsize $datasize $p
dataname=$lattype'_'$latsize'_'$datasize'_'$ptxt
#batchsize=$((datasize/1000))
for numnodes in 200
do
	python trainmodel.py $lattype $opt $latsize $steps $epochs $numnodes \
	$numlayers $batchsize $learningrate $dataname $valname $dt

	aws s3 cp 'models/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$lrtxt'_'$opt'.hdf5' \
	s3://nmaskara-models/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$lrtxt'_'$opt'.hdf5'

	aws s3 cp 'results/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$lrtxt'_'$opt'.csv' \
	s3://nmaskara-models/results/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$lrtxt'_'$opt'.csv'
done
echo 'here'
#sudo shutdown -h now
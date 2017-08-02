#!/bin/bash -x

lattype='square'
latsize=9
p=0.1
ptxt=100
opt='sgd'

batchsize=1000
act='relu'
numlayers=3
datasize=50000000
valsize=100000
steps=1000
epochs=500

dt='sq5act_'$(date '+%Y-%m-%d_%H-%M-%S')
mkdir 'models/'$dt
mkdir 'results/'$dt


#./gendata $lattype $latsize $valsize $p
valname=$lattype'_'$latsize'_'$valsize'_'$ptxt

#./gendata $lattype $latsize $datasize $p
dataname=$lattype'_'$latsize'_'$datasize'_'$ptxt
#batchsize=$((datasize/1000))
for numnodes in 400
do
for numlayers in 5 6 4 3 2
do
	python trainmodel.py $lattype $opt $act $latsize $steps $epochs $numnodes \
	$numlayers $batchsize $dataname $valname $dt

	aws s3 cp 'models/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$act'_'$opt'.hdf5' \
	s3://nmaskara-models/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$act'_'$opt'.hdf5'

	aws s3 cp 'results/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$act'_'$opt'.csv' \
	s3://nmaskara-models/results/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$act'_'$opt'.csv'
done
done


echo 'done'
#sudo shutdown -h now

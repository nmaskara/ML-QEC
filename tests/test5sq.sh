#!/bin/bash -x

lattype='square'
latsize=5
p=0.1
ptxt=100
opt='adam'

batchsize=1000
numlayers=3
numnodes=300
datasize=1000000
valsize=100000
steps=200
epochs=14

dt='sq5' #_'$(date '+%Y-%m-%d_%H-%M-%S')
if [ ! -d 'models/'$dt ]; then
	mkdir 'models/'$dt
fi
if [ ! -d 'results/'$dt ]; then
	mkdir 'results/'$dt
fi
#./gendata $lattype $latsize $valsize $p
valname=$lattype'_'$latsize'_'$valsize'_'$ptxt

#./gendata $lattype $latsize $datasize $p
dataname=$lattype'_'$latsize'_'$datasize'_'$ptxt
#python csvtohdf5.py $dataname 
#batchsize=$((datasize/1000))
for numlayers in 2
do
	python trainmodel.py $lattype $opt $latsize $steps $epochs $numnodes \
	$numlayers $batchsize $datasize $p $valsize $dt

	aws s3 cp 'models/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.hdf5' \
	s3://nmaskara-models/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.hdf5'

	aws s3 cp 'models/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'_best.hdf5' \
	s3://nmaskara-models/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'_best.hdf5'

	aws s3 cp 'results/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.csv' \
	s3://nmaskara-models/results/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.csv'
done

echo 'done'
#sudo shutdown -h now

#!/bin/bash -x

lattype=$1
latsize=$2
p=$3
ptxt=$4
opt=$5

batchsize=$6
numlayers=$7
numnodes=$8
datasize=$9
valsize=$10
steps=$11
epochs=$12

dt=$13 #_'$(date '+%Y-%m-%d_%H-%M-%S')
if [ ! -d 'models/'$dt ]; then
	mkdir 'models/'$dt
fi
if [ ! -d 'results/'$dt ]; then
	mkdir 'results/'$dt
fi
valname=$lattype'_'$latsize'_'$valsize'_'$ptxt
if [ ! -e 'data/'$valname'.csv' ]; then
	./gendata $lattype $latsize $valsize $p
fi
dataname=$lattype'_'$latsize'_'$datasize'_'$ptxt
if [ ! -e 'data/'$dataname'.h5' ]; then
	./gendata $lattype $latsize $datasize $p
	python csvtohdf5.py $dataname
fi

python trainmodel.py $lattype $opt $latsize $steps $epochs $numnodes \
$numlayers $batchsize $dataname $valname $dt

aws s3 cp 'models/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.hdf5' \
s3://nmaskara-models/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.hdf5'

aws s3 cp 'models/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'_best.hdf5' \
s3://nmaskara-models/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'_best.hdf5'

aws s3 cp 'results/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.csv' \
s3://nmaskara-models/results/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.csv'

echo 'done'
#sudo shutdown -h now

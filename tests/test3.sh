#!/bin/bash -x

lattype='cc'
latsize=3
p=0.08
ptxt=80
opt='adam'

batchsize=1000
numlayers=1
numnodes=60
datasize=1000000
valsize=100000
steps=1000
epochs=40

dt='sq3' #_'$(date '+%Y-%m-%d_%H-%M-%S')
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
#batchsize=$((datasize/1000))
for numnodes in 400
do
	python trainmodel.py $lattype $opt $latsize $steps $epochs $numnodes \
	$numlayers $batchsize $dataname $valname $dt

	aws s3 cp 'models/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.hdf5' \
	s3://nmaskara-models/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.hdf5'

	aws s3 cp 'models/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'_best.hdf5' \
	s3://nmaskara-models/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'_best.hdf5'

	aws s3 cp 'results/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.csv' \
	s3://nmaskara-models/results/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.csv'
done

echo 'done'
#sudo shutdown -h now

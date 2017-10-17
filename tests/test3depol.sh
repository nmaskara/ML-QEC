#!/bin/bash -x

lattype='cc2'
latsize=3
p=0.15
ptxt=150
opt='adam'

batchsize=1000
numlayers=1
numnodes=100
datasize=-1
valsize=100000
steps=1000
epochs=10

dt='cc2depol' #_'$(date '+%Y-%m-%d_%H-%M-%S')
if [ ! -d 'models/'$dt ]; then
	mkdir 'models/'$dt
fi
if [ ! -d 'results/'$dt ]; then
	mkdir 'results/'$dt
fi
./gendata $lattype $latsize $valsize $p -d
valname=$lattype'_'$latsize'_'$valsize'_'$ptxt'_depol'

#./gendata $lattype $latsize $datasize $p -c $pratio
dataname=$lattype'_'$latsize'_'$datasize'_'$ptxt'_depol'
#python csvtohdf5.py $dataname 
#batchsize=$((datasize/1000))
for numnodes in 1 2 3 4
do
	python trainmodel.py $lattype $opt $latsize $steps $epochs $numnodes \
	$numlayers $batchsize $datasize $p $valsize $dt -d

	aws s3 cp 'models/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.hdf5' \
	s3://nmaskara-models/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.hdf5'

	aws s3 cp 'models/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'_best.hdf5' \
	s3://nmaskara-models/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'_best.hdf5'

	aws s3 cp 'results/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.csv' \
	s3://nmaskara-models/results/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.csv'
done

echo 'done'
#sudo shutdown -h now

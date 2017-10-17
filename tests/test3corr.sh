#!/bin/bash -x

lattype='cc2'
latsize=5
p=0.030
ptxt=30
pratio=1
opt='adam'

batchsize=5000
numlayers=1
numnodes=100
datasize=-1
valsize=100000
steps=1000
epochs=10

dt='cc2_3_corr' #_'$(date '+%Y-%m-%d_%H-%M-%S')
if [ ! -d 'models/'$dt ]; then
	mkdir 'models/'$dt
fi
if [ ! -d 'results/'$dt ]; then
	mkdir 'results/'$dt
fi
./gendata $lattype $latsize $valsize $p -c $pratio
valname=$lattype'_'$latsize'_'$valsize'_'$ptxt'_corr_'$pratio

#./gendata $lattype $latsize $datasize $p -c $pratio
dataname=$lattype'_'$latsize'_'$datasize'_'$ptxt'_corr_'$pratio
#python csvtohdf5.py $dataname 
#batchsize=$((datasize/1000))
for numnodes in 50
do
	python trainmodel.py $lattype $opt $latsize $steps $epochs $numnodes \
	$numlayers $batchsize $datasize $p $valsize $dt -c $pratio

	aws s3 cp 'models/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.hdf5' \
	s3://nmaskara-models/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.hdf5'

	aws s3 cp 'models/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'_best.hdf5' \
	s3://nmaskara-models/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'_best.hdf5'

	aws s3 cp 'results/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.csv' \
	s3://nmaskara-models/results/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'.csv'
done

echo 'done'
#sudo shutdown -h now

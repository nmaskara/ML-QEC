#!/bin/bash -x

lattype='square'
latsize=5
p=0.1
ptxt=100
opt='adam'

batchsize=1000
numlayers=1
numnodes=100
datasize=20000000
valsize=100000
steps=1000
epochs=50
numcopies=3
dt='sq5wl' #_'$(date '+%Y-%m-%d_%H-%M-%S')
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
for numnodes in 2
do
	python trainmodel.py $lattype $opt $latsize $steps $epochs $numnodes \
	$numlayers $batchsize $dataname $valname $dt -c $numcopies

	for n in $(seq 1 1 $numcopies)
	do

		aws s3 cp 'models/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'_'$n'.hdf5' \
		s3://nmaskara-models/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'_'$n'.hdf5'

		aws s3 cp 'models/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'_'$n'_best.hdf5' \
		s3://nmaskara-models/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'_'$n'_best.hdf5'

		aws s3 cp 'results/'$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'_'$n'.csv' \
		s3://nmaskara-models/results/$dt'/'$dataname'_'$numnodes'_'$numlayers'_'$batchsize'_'$opt'_'$n'.csv'
	done
done

echo 'done'
#sudo shutdown -h now

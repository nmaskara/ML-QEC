from keras.models import Sequential
from  keras.layers import convolutional, Activation, BatchNormalization, \
	Dense, Dropout, Flatten, pooling
from keras import optimizers, initializers
from keras.callbacks import ModelCheckpoint, EarlyStopping
import sys, os
import numpy as np
import pandas as pd
import random

def wrapped(nums, latsize, kernelsize):
	nums = np.reshape(nums, (-1, latsize, latsize, 1))
	nums = np.append(nums, nums[:, :, 0:kernelsize-1, :], axis=2)
	nums = np.append(nums, nums[:, 0:kernelsize-1, :, :], axis=1)
	return nums


def genbatches(filename, latsize, kernelsize, chsize, batchsize):
	insize = latsize * latsize
	while True:
		for chunk in pd.read_csv(filename, chunksize=chsize):
			vals = chunk.values
			random.shuffle(vals)
			ct = 0
			while ct < len(chunk):
				x_batch = wrapped(vals[ct:ct+batchsize, 0:insize], latsize, kernelsize)
				#x_batch = vals[ct:ct+batchsize, 0:insize]
				y_batch = vals[ct:ct+batchsize, insize:insize+4]

				yield (x_batch, y_batch)
				ct += batchsize	

def makeModel(latsize, kernel_size, num_filters, num_nodes, hiddenlayers, opt_type):
	input_shape = (latsize + kernelsize - 1, latsize + kernelsize - 1, 1)
	model = Sequential()
	#upsample = convolutional.UpSampling2D(size=(1.2,1.2),input_shape=(latsize, latsize, 1) )
	clayer = convolutional.Conv2D(num_filters, kernel_size, input_shape=input_shape)
	pool = pooling.MaxPooling2D(pool_size=(2,2), padding='same')
	#h1 = Dense(units=num_nodes, kernel_initializer='he_normal')
	output = Dense(units=4, activation='softmax')
	#model.add(upsample)
	model.add(clayer)
	model.add(Flatten())
	model.add(BatchNormalization())
	model.add(Activation('relu'))

	while (hiddenlayers > 0):
		model.add(Dense(units=num_nodes, kernel_initializer='he_normal'))
		model.add(BatchNormalization())
		model.add(Activation('relu'))
	#model.add(pool)
	#print upsample.output_shape
	print clayer.output_shape
	#print pool.output_shape
	model.add(output)

	if opt_type == 'sgd':
		opt = optimizers.SGD()
	elif opt_type == 'adam':
		opt = optimizers.Adam()
	else:
		print 'Invalid optimizer'
		sys.exit()
	model.compile(loss='categorical_crossentropy', optimizer=opt, metrics=['accuracy'])
	return model

if __name__ == "__main__":
	'''if (len(sys.argv) != 15):
		print "usage: type opttype latsize stepsperepoch epochs kernelsize numfilters numnodes hiddenlayers batchsize learningrate dataname valname date"
		sys.exit()'''

	# read inputs
	
	lattype = sys.argv[1]
	opttype = sys.argv[2]
	latsize = int(sys.argv[3])
	stepsperepoch = int(sys.argv[4])
	numepochs = int(sys.argv[5])
	kernelsize = int(sys.argv[6])
	numfilters = int(sys.argv[7])
	numnodes = int(sys.argv[8])
	hiddenlayers = int(sys.argv[9])
	batchsize = int(sys.argv[10])
	learningrate = float(sys.argv[11])
	filename = sys.argv[12]
	valname = sys.argv[13]
	date = sys.argv[14]

	'''
	lattype = 'square'
	opttype = 'adam'
	latsize = 7
	stepsperepoch = 1000
	numepochs = 100

	numfilters = 64
	numnodes = 100
	hiddenlayers = 0
	batchsize = 1000
	learningrate = 0.01
	filename = 'square_7_50000000_100'
	valname = 'square_7_100000_100'
	date = 'test1

	kernelsize = 4'''

	insize = latsize*latsize

	inname = "data/" + filename + ".csv"
 	valdata = np.genfromtxt("data/" + valname + ".csv", delimiter=',')
 	x_val = wrapped(valdata[:, 0:insize], latsize, kernelsize)
 	#x_val = valdata[:, 0:insize]
 	#print x_val.shape
 	y_val = valdata[:, insize:insize+4]
 	#print y_val.shape

 	model = makeModel(latsize, kernelsize, numfilters, numnodes, hiddenlayers, opttype)

 	early_stopping = EarlyStopping(monitor='loss', patience=10)
	#make_exist("models/" + filename + "_" + str(numnodes) + "_" + str(batchsize))
	filepath = "models/" + date + '/' + filename + "_" + str(numnodes) + '_' + str(hiddenlayers) + \
		"_" + str(batchsize) + "_" + str(int(1000*learningrate)) + "_" + opttype + ".hdf5"
	checkpt = ModelCheckpoint(filepath, save_best_only=True)

	hist = model.fit_generator(genbatches(inname, latsize, kernelsize, 100000, batchsize), stepsperepoch,\
		epochs=numepochs, callbacks=[early_stopping, checkpt], verbose=1, \
		validation_data=(x_val, y_val) )

	outpath = "results/" + date + '/' + filename + "_" + str(numnodes) + "_" + str(hiddenlayers) + \
		"_" + str(batchsize) + "_" + str(int(1000*learningrate)) + "_" + opttype + ".csv"
	fout = open(outpath, 'w')

	count = 0
	for (loss, acc, val_loss, val_acc) in zip(hist.history['loss'], hist.history['acc'], \
	hist.history['val_loss'], hist.history['val_acc']):
		count += 1
		fout.write(str(count * stepsperepoch) + ', ')
		fout.write(str(loss) + ', ')
		fout.write(str(acc) + ', ')
		fout.write(str(val_loss) + ', ')
		fout.write(str(val_acc) + ', ')
		fout.write('\n')
	fout.close()


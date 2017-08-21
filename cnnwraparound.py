from keras.models import Sequential, Model
from  keras.layers import convolutional, Activation, BatchNormalization, \
	Dense, Dropout, Flatten, pooling, Reshape, Input, Lambda, Concatenate
from keras import optimizers, initializers
from keras.callbacks import ModelCheckpoint, EarlyStopping, Callback
import sys, os
import numpy as np
import pandas as pd
import random

class mycallback(Callback):

	def __init__(self, filename, batchesperepoch):
		self.filename = filename
		self.inc = batchesperepoch


	def on_train_begin(self, logs=None):
		filename = self.filename
		if os.path.isfile(filename):
			self.outfile = open(filename, 'a')
		else:
			self.outfile = open(filename, 'w')		

	def on_train_end(self, logs=None):
		self.outfile.close()
		
	def on_epoch_end(self, epoch, logs=None):
		self.outfile.write(str(epoch * self.inc) + ',')
		self.outfile.write(str(logs['loss']) + ',')
		self.outfile.write(str(logs['acc']) + ',')
		self.outfile.write(str(logs['val_loss']) + ',')
		self.outfile.write(str(logs['val_acc']) + ',')
		self.outfile.write('\n')

def wrapped(nums, latsize, kernelsize):
	nums = np.reshape(nums, (-1, latsize, latsize, 1))
	nums = np.append(nums, nums[:, :, 0:kernelsize-1, :], axis=2)
	nums = np.append(nums, nums[:, 0:kernelsize-1, :, :], axis=1)
	return nums

def wrap(latsize, kernelsize):
	# Extends the input by neccessary amount so a conv layer with kernelsize
	# has an output of latsize
	def func(x):
		x = np.append(x, x[ :, 0:kernelsize-1, :], axis=1)
		x = np.append(nums, nums[ 0:kernelsize-1, :, :], axis=0)
		return x
	return Lambda(func) 

def genbatches(filename, latsize, kernelsize, batchsize):
	df = pd.read_hdf(filename)
	numlines = len(df.values)
	values = df.values
	count = 0
	while True:
		dat = np.unpackbits(values[count:count+batchsize], axis=-1)
		#x_batch = wrapped(dat[:, 0:insize], latsize, kernelsize)
		x_batch = dat[:, 0:insize]
		y_batch = dat[:, insize:insize+4]
		yield (x_batch, y_batch)
		count += batchsize

def makeFuncModel(latsize, kernel_size):
	inputs = Input(shape=(latsize*latsize,))
	'''in2d = Reshape((latsize, latsize, -1))(inputs)
	print in2d.shape
	#wrapped = wrap(latsize, kernel_size)(in2d)
	conv1 = convolutional.Conv2D(4, kernel_size, padding="same")(in2d)
	print conv1.shape

	combined = Concatenate()([conv1, in2d])
	flattened = Flatten()(combined)'''
	
	fullyconnected = Dense(units=100, kernel_initializer='he_normal', activation='relu')(inputs)#(flattened)
	normalize = BatchNormalization()(fullyconnected)
	output = Dense(units=4, activation='softmax')(normalize)
	print output.shape
	model = Model(inputs=inputs, outputs=output)
	model.compile(optimizer='adam', loss='categorical_crossentropy', metrics=['accuracy'])
	return model



def makeModel(latsize, kernel_size, num_filters, num_nodes, hiddenlayers, opt_type):
	input_shape = (latsize + kernelsize - 1, latsize + kernelsize - 1, 1)
	model = Sequential()
	#upsample = convolutional.UpSampling2D(size=(1.2,1.2),input_shape=(latsize, latsize, 1) )
	clayer1 = convolutional.Conv2D(num_filters, kernel_size, input_shape=input_shape)
	#clayer2 = convolutional.Conv2D(num_filters, 1)
	print numfilters
	#h1 = Dense(units=num_nodes, kernel_initializer='he_normal')
	output = Dense(units=4, activation='softmax')
	#model.add(upsample)
	model.add(clayer1)
	#model.add(Activation('relu'))
	#model.add(clayer2)
	model.add(Flatten())
	model.add(BatchNormalization())
	model.add(Activation('relu'))

	while (hiddenlayers > 0):
		model.add(Dense(units=num_nodes, kernel_initializer='he_normal'))
		model.add(BatchNormalization())
		model.add(Activation('relu'))
		hiddenlayers -= 1
	#model.add(pool)
	#print upsample.output_shape
	#print clayer.output_shape
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
	# read inputs
	lattype = 'square'
	latsize = 7
	stepsperepoch = 1000
	numepochs = 10
	numnodes = 100
	hiddenlayers = 3
	batchsize = 1000
	filename = 'square_7_50000000_100'
	valname = 'square_7_100000_100'
	dirname = 'cnntest'
	opttype = 'adam'

	trainfilename = "data/" + filename + ".h5"
	valfilename = "data/" + valname + ".csv"

	if not os.path.isfile(valfilename):
		print "Couldn't find validation data"
		sys.exit(0)
	if not os.path.isfile(trainfilename):
		print "Couldn't find training data"
	
	insize = latsize * latsize

	modelpath = "models/" + dirname + '/' + filename + "_" + str(numnodes) + '_' + str(hiddenlayers) + \
		"_" + str(batchsize) + "_" + opttype + ".hdf5"
	bestmodelpath = "models/" + dirname + '/' + filename + "_" + str(numnodes) + '_' + str(hiddenlayers) + \
		"_" + str(batchsize) + "_" + opttype + "_best.hdf5"
	resultpath = "results/" + dirname + '/' + filename + "_" + str(numnodes) + "_" + str(hiddenlayers) + \
		"_" + str(batchsize) + "_" + opttype + ".csv"
	bestcheckpt = ModelCheckpoint(bestmodelpath, save_best_only=True)
	lastcheckpt = ModelCheckpoint(modelpath, save_best_only=False)
	record = mycallback(resultpath, stepsperepoch)
	#early_stopping = EarlyStopping(monitor='loss', patience=10)
	if (lattype == "cc"):
		numcat = 16
	elif (lattype == "open_square"):
		numcat = 16
	elif (lattype == "surface"):
		numcat = 2
	else:
		numcat = 4

	model = makeFuncModel(latsize, 4)
	print 'compiled model'

	initial_epoch = 0

	'''if os.path.isfile(resultpath):
		toread = open(resultpath, 'r')
		initial_epoch = len(toread.readlines())
		toread.close()
		print "Initial Epoch: " + str(initial_epoch)'''

 	valdata = np.genfromtxt("data/" + valname + ".csv", delimiter=',')
 	print 'loaded valdata'

	print numcat
	hist = model.fit_generator(genbatches(trainfilename, insize, batchsize, stepsperepoch),\
		stepsperepoch,  \
		epochs=numepochs, initial_epoch=initial_epoch, \
		callbacks=[bestcheckpt, lastcheckpt, record], verbose=1, \
		validation_data=(valdata[:,0:insize], valdata[:,insize:insize+numcat]))


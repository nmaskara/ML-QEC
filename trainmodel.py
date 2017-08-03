from keras.models import Sequential
from keras.layers import Dense, Activation, BatchNormalization, Dropout, \
	advanced_activations, convolutional, Flatten
from keras.callbacks import ModelCheckpoint, EarlyStopping
from keras.regularizers import l2
from keras import optimizers, initializers
import numpy as np
import sys
import os
import errno
import pandas as pd
import random

def make_exist(path):
	try:
		os.makedirs(path)
	except OSError as exception:
		if exception.errno != errno.EEXIST:
			raise

def genbatches2(filename, insize, chsize, batchsize, latisze):
	while True:
		for chunk in pd.read_csv(filename, chunksize=chsize):
			#print chunk
			vals = chunk.values
			random.shuffle(vals)
			ct = 0
			while ct < len(chunk):
				#print chunk[ct:ct+batchsize, 0:insize]
				newshape = (-1, latsize, latsize, 1)
				yield (np.reshape(vals[ct:ct+batchsize, 0:insize], newshape), \
					vals[ct:ct+batchsize, insize:insize+4])
				ct += batchsize

def genbatches(filename, insize, batchsize):
	df = pd.read_hdf(filename)
	numlines = len(df.values)
	values = df.values
	count = 0
	while True:
		dat = np.unpackbits(values[count:count+batchsize], axis=-1)
		yield dat[:, 0:insize], dat[:, insize:insize+4]
		count += batchsize
		if count > numlines:
			count = 0
			#random.shuffle(values)


def makeModel2(input_size, num_nodes, hidden_layers, opt_type):
	model = Sequential()
	filters = 100
	kernelsize = 3
	layer1 = convolutional.Conv2D(filters, kernelsize, input_shape=(input_size, input_size, 1))
	layer2 = Dense(units=num_nodes, kernel_initializer='he_normal')
	model.add(layer1)
	model.add(Flatten())
	model.add(BatchNormalization())
	model.add(Activation('relu'))
	model.add(layer2)
	model.add(BatchNormalization())
	model.add(Activation('relu'))
	hidden_layers -= 1
	while (hidden_layers > 0):
		hidden_layers -= 1
		model.add(Dense(units=num_nodes, kernel_initializer='he_normal'))
		model.add(BatchNormalization())
		model.add(Activation('relu'))
	layer3 = Dense(units=4, activation='softmax')
	model.add(layer3)
	if opt_type == 'sgd':
		opt = optimizers.SGD()
	elif opt_type == 'adam':
		opt = optimizers.Adam()
	else:
		print 'Invalid optimizer'
		sys.exit()
	model.compile(loss='categorical_crossentropy', optimizer=opt, metrics=['accuracy'])
	return model
 

def makeModel(input_size, num_nodes, hidden_layers, opt_type, acttype):
	model = Sequential()
	first = True
	while (hidden_layers > 0):
		hidden_layers -= 1
		if (first):
			model.add(Dense(units=num_nodes, kernel_initializer='he_normal', input_dim=input_size))
			first = False
		else:
			model.add(Dense(units=num_nodes, kernel_initializer='he_normal'))
		model.add(BatchNormalization())
		model.add(Activation(acttype))
	layer3 = Dense(units=4, activation='softmax')
	model.add(layer3)
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
	if (len(sys.argv) != 13):
		print "usage: type opttype latsize stepsperepoch epochs numnodes hiddenlayers batchsize learningrate dataname valname date"
		sys.exit()

	# read inputs
	lattype = sys.argv[1]
	opttype = sys.argv[2]
	acttype = sys.argv[3]
	latsize = int(sys.argv[4])
	stepsperepoch = int(sys.argv[5])
	numepochs = int(sys.argv[6])
	numnodes = int(sys.argv[7])
	hiddenlayers = int(sys.argv[8])
	batchsize = int(sys.argv[9])
	filename = sys.argv[10]
	valname = sys.argv[11]
	date = sys.argv[12]
	inname = "data/" + filename + ".h5"

	if not os.path.isfile("data/" + valname + ".csv"):
		print "Couldn't find validation data"
		sys.exit(0)
 	valdata = np.genfromtxt("data/" + valname + ".csv", delimiter=',')
	
	insize = latsize * latsize

	# generate model
	model = makeModel(insize, numnodes, hiddenlayers, opttype, acttype)
	#model = makeModel2(latsize, numnodes, hiddenlayers, opttype)

	early_stopping = EarlyStopping(monitor='loss', patience=10)
	#make_exist("models/" + filename + "_" + str(numnodes) + "_" + str(batchsize))
	filepath = "models/" + date + '/' + filename + "_" + str(numnodes) + '_' + str(hiddenlayers) + \
		"_" + str(batchsize) + "_" + acttype + "_" + opttype + ".hdf5"
	checkpt = ModelCheckpoint(filepath, save_best_only=True)

	hist = model.fit_generator(genbatches(inname, insize, batchsize),\
		stepsperepoch,  \
		epochs=numepochs, callbacks=[early_stopping, checkpt], verbose=1, \
		validation_data=(valdata[:,0:insize], valdata[:,insize:insize+4]))

	#hist = model.fit_generator(genbatches2(inname, insize, 100000, batchsize, latsize),\
	#	stepsperepoch,  \
	#	epochs=numepochs, callbacks=[early_stopping, checkpt], verbose=1, \
	#	validation_data=(np.reshape(valdata[:,0:insize], (-1, latsize, latsize, 1)), valdata[:,insize:insize+4]))

	outpath = "results/" + date + '/' + filename + "_" + str(numnodes) + "_" + str(hiddenlayers) + \
		"_" + str(batchsize) + "_" + acttype + "_" + opttype + ".csv"
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
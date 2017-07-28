from keras.models import Sequential
from keras.layers import Dense, Activation, BatchNormalization, Dropout, \
	advanced_activations, convolutional, Flatten
from keras.callbacks import ModelCheckpoint, EarlyStopping
from keras.regularizers import l2
from keras import optimizers, initializers
import numpy as np
import matplotlib.pyplot as plt
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

def genbatches(filename, insize, chsize, batchsize):
	while True:
		for chunk in pd.read_csv(filename, chunksize=chsize):
			#print chunk
			vals = chunk.values
			random.shuffle(vals)
			ct = 0
			while ct < len(chunk):
				#print vals[ct:ct+batchsize, insize:insize+1]
				yield (vals[ct:ct+batchsize, 0:insize], \
					vals[ct:ct+batchsize, insize:insize+1])
				ct += batchsize
 

def makeModel(input_size, num_nodes, hidden_layers, opt_type):
	model = Sequential()
	layer1 = Dense(units=num_nodes, kernel_initializer='he_normal', input_dim=input_size)
	model.add(layer1)
	model.add(BatchNormalization())
	model.add(Activation('relu'))
	hidden_layers -= 1
	while (hidden_layers > 0):
		hidden_layers -= 1
		model.add(Dense(units=num_nodes, kernel_initializer='he_normal'))
		model.add(BatchNormalization())
		model.add(Activation('relu'))
	layer3 = Dense(units=1, activation='sigmoid')
	model.add(layer3)
	if opt_type == 'sgd':
		opt = optimizers.SGD()
	elif opt_type == 'adam':
		opt = optimizers.Adam()
	else:
		print 'Invalid optimizer'
		sys.exit()
	model.compile(loss='binary_crossentropy', optimizer=opt, metrics=['accuracy'])
	return model


if __name__ == "__main__":
	if (len(sys.argv) != 13):
		print "usage: type opttype latsize stepsperepoch epochs numnodes hiddenlayers batchsize learningrate dataname valname date"
		sys.exit()

	# read inputs
	lattype = sys.argv[1]
	opttype = sys.argv[2]
	latsize = int(sys.argv[3])
	stepsperepoch = int(sys.argv[4])
	numepochs = int(sys.argv[5])
	numnodes = int(sys.argv[6])
	hiddenlayers = int(sys.argv[7])
	batchsize = int(sys.argv[8])
	learningrate = float(sys.argv[9])
	filename = sys.argv[10]
	valname = sys.argv[11]
	date = sys.argv[12]
	inname = "data/open_" + filename + ".csv"

	if not os.path.isfile("data/open_" + valname + ".csv"):
		print "Couldn't find validation data"
		os.system('/gendata square 3 100000 0.1')
		valname = "data/square_3_100000_100.csv"
 	valdata = np.genfromtxt("data/open_" + valname + ".csv", delimiter=',')
	
	insize = latsize * latsize

	print np.sum(valdata[:, insize:insize+1])

	# generate model
	model = makeModel(insize, numnodes, hiddenlayers, opttype)
	#model = makeModel2(latsize, numnodes, hiddenlayers, opttype)

	early_stopping = EarlyStopping(monitor='loss', patience=10)
	#make_exist("models/" + filename + "_" + str(numnodes) + "_" + str(batchsize))
	filepath = "models/" + date + '/' + filename + "_" + str(numnodes) + '_' + str(hiddenlayers) + \
		"_" + str(batchsize) + "_" + str(int(1000*learningrate)) + "_" + opttype + ".hdf5"
	checkpt = ModelCheckpoint(filepath, save_best_only=True)

	hist = model.fit_generator(genbatches(inname, insize, 100000, batchsize),\
		stepsperepoch,  \
		epochs=numepochs, callbacks=[early_stopping, checkpt], verbose=1, \
		validation_data=(valdata[:,0:insize], valdata[:,insize:insize+1]))

	#hist = model.fit_generator(genbatches2(inname, insize, 100000, batchsize, latsize),\
	#	stepsperepoch,  \
	#	epochs=numepochs, callbacks=[early_stopping, checkpt], verbose=1, \
	#	validation_data=(np.reshape(valdata[:,0:insize], (-1, latsize, latsize, 1)), valdata[:,insize:insize+4]))

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
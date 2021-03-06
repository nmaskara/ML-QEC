from keras.models import Sequential, load_model
from keras.layers import Dense, Activation, BatchNormalization, Dropout, \
	advanced_activations, convolutional, Flatten
from keras.callbacks import ModelCheckpoint, EarlyStopping, Callback
from keras.regularizers import l2
from keras import optimizers, initializers
import numpy as np
import sys
import os
import errno
import pandas as pd
import random

regfac = 0.0001

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




def make_exist(path):
	try:
		os.makedirs(path)
	except OSError as exception:
		if exception.errno != errno.EEXIST:
			raise

def genbatches(filename, insize, numcat, batchsize, initial_epoch, stepsperepoch):
	df = pd.read_hdf(filename)
	numlines = len(df.values)
	values = df.values
	count = (initial_epoch * stepsperepoch * batchsize) % numlines
	while True:
		dat = np.unpackbits(values[count:count+batchsize], axis=-1)
		yield dat[:, 0:insize], dat[:, insize:insize+numcat]
		count += batchsize
		if count > numlines:
			count = 0

def makeModel(input_size, num_nodes, hidden_layers, opt_type, numcat):
	model = Sequential()
	first = True
	while (hidden_layers > 0):
		hidden_layers -= 1
		if (first):
			model.add(Dense(units=num_nodes, kernel_initializer='he_normal', input_dim=input_size))
		else:
			model.add(Dense(units=num_nodes, kernel_initializer='he_normal', ))
		model.add(BatchNormalization())
		model.add(Dropout(0.5))
		model.add(Activation('relu'))
		if (first):
			first = False
	layer3 = Dense(units=numcat, activation='softmax')
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
	if (len(sys.argv) != 12):
		print "usage: type opttype latsize stepsperepoch epochs numnodes hiddenlayers batchsize dataname valname date"
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
	filename = sys.argv[9]
	valname = sys.argv[10]
	dirname = sys.argv[11]
	trainfilename = "data/" + filename + ".h5"
	valfilename = "data/" + valname + ".csv"

	if not os.path.isfile(valfilename):
		print "Couldn't find validation data"
		sys.exit(0)
	if not os.path.isfile(trainfilename):
		print "Couldn't find training data"
 	valdata = np.genfromtxt("data/" + valname + ".csv", delimiter=',')
	
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
	else:
		numcat = 4

	# If model already exists, load model
	if os.path.isfile(modelpath):
		print "Loaded Previous Model"
		model = load_model(modelpath)
	else:
		# otherwise, generate model
		model = makeModel(insize, numnodes, hiddenlayers, opttype, numcat)

	initial_epoch = 0

	if os.path.isfile(resultpath):
		toread = open(resultpath, 'r')
		initial_epoch = len(toread.readlines())
		toread.close()
		print "Initial Epoch: " + str(initial_epoch)

	hist = model.fit_generator(genbatches(trainfilename, insize, numcat, batchsize, initial_epoch, stepsperepoch),\
		stepsperepoch,  \
		epochs=numepochs, initial_epoch=initial_epoch, \
		callbacks=[bestcheckpt, lastcheckpt, record], verbose=1, \
		validation_data=(valdata[:,0:insize], valdata[:,insize:insize+numcat]))

	'''
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
	fout.close()'''
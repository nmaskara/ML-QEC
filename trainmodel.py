from keras.models import Sequential
from keras.layers import Dense, Activation, BatchNormalization, Dropout, advanced_activations
from keras.callbacks import ModelCheckpoint, EarlyStopping
from keras.regularizers import l2
from keras import optimizers, initializers
import numpy as np
import matplotlib.pyplot as plt
import sys
import os
import errno
import pandas as pd

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
			ct = 0
			while ct < len(chunk):
				#print chunk[ct:ct+batchsize, 0:insize]
				yield (vals[ct:ct+batchsize, 0:insize], \
					vals[ct:ct+batchsize, insize:insize+4])
				ct += batchsize

def makeModel(input_size, num_nodes, hidden_layers):
	model = Sequential()
	layer1 = Dense(units=num_nodes, kernel_initializer='he_normal', input_dim=input_size)
	model.add(layer1)
	model.add(Activation('relu'))
	hidden_layers -= 1
	while (hidden_layers > 0):
		model.add(Dense(units=num_nodes, kernel_initializer='he_normal'))
		model.add(Activation('relu'))
	layer3 = Dense(units=4, activation='softmax')
	model.add(layer3)
	opt = optimizers.SGD()
	model.compile(loss='categorical_crossentropy', optimizer=opt, metrics=['accuracy'])
	return model


if __name__ == "__main__":
	if (len(sys.argv) != 12):
		print "usage: type opttype latsize datasize numnodes hiddenlayers batchsize learningrate dataname valname date"
		sys.exit()

	# read inputs
	lattype = sys.argv[1]
	opttype = sys.argv[2]
	latsize = int(sys.argv[3])
	datasize = int(sys.argv[4])
	numnodes = int(sys.argv[5])
	hiddenlayers = int(sys.argv[6])
	batchsize = int(sys.argv[7])
	learningrate = float(sys.argv[8])
	filename = sys.argv[9]
	valname = sys.argv[10]
	date = sys.argv[11]
	inname = "data/" + filename + ".csv"

	if not os.path.isfile("data/" + valname + ".csv"):
		print "Couldn't find validation data"
		os.system('/gendata square 3 100000 0.1')
		valname = "data/square_3_100000_100.csv"
 	valdata = np.genfromtxt("data/" + valname + ".csv", delimiter=',')
	
	insize = latsize * latsize

	# generate model
	model = makeModel(insize, numnodes, hiddenlayers)

	early_stopping = EarlyStopping(monitor='val_loss', patience=5)
	#make_exist("models/" + filename + "_" + str(numnodes) + "_" + str(batchsize))
	filepath = "models/" + date + '/' + filename + "_" + str(numnodes) + '_' + str(hiddenlayers) + \
		"_" + str(batchsize) + "_" + str(int(1000*learningrate)) + "_" + opttype + ".hdf5"
	checkpt = ModelCheckpoint(filepath, save_best_only=True)

	hist = model.fit_generator(genbatches(inname, insize, 100000, batchsize), datasize/(batchsize),  \
		epochs=500, callbacks=[early_stopping, checkpt], verbose=1, \
		validation_data=(valdata[:,0:insize], valdata[:,insize:insize+4]))

	outpath = "results/" + date + '/' + filename + "_" + str(numnodes) + "_" + str(hiddenlayers) + \
		"_" + str(batchsize) + "_" + str(int(1000*learningrate)) + "_" + opttype + ".csv"
	fout = open(outpath, 'w')

	count = 0
	samplerate = 1
	for (loss, acc, val_loss, val_acc) in zip(hist.history['loss'], hist.history['acc'], \
	hist.history['val_loss'], hist.history['val_acc']):
		count += 1
		fout.write(str(count * samplerate) + ', ')
		fout.write(str(loss) + ', ')
		fout.write(str(acc) + ', ')
		fout.write(str(val_loss) + ', ')
		fout.write(str(val_acc) + ', ')
		fout.write('\n')
	fout.close()
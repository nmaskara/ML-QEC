from keras.models import Sequential
from keras.layers import Dense, Activation, BatchNormalization, Dropout, advanced_activations
from keras.callbacks import ModelCheckpoint, EarlyStopping
from keras.regularizers import l2
from keras import optimizers
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



if __name__ == "__main__":
	if (len(sys.argv) != 10):
		print "usage: type learning latsize datasize numnodes batchsize learningrate dataname valname"
		sys.exit()
	lattype = sys.argv[1]
	opttype = sys.argv[2]
	latsize = int(sys.argv[3])
	datasize = int(sys.argv[4])
	numnodes = int(sys.argv[5])
	batchsize = int(sys.argv[6])
	learningrate = float(sys.argv[7])
	#outname = sys.argv[8]
	filename = sys.argv[8]
	valname = sys.argv[9]
	inname = "data/" + filename + ".csv"
	#filename = lattype + "_" + str(latsize) + "_" + str(numtrials) + "_" + str(int(p*1000))
 	sqdata = np.genfromtxt("data/" + valname + ".csv", delimiter=',')
	
	insize = latsize * latsize
	model = Sequential()
	layer1 = Dense(units=numnodes,kernel_initializer='he_normal', input_dim=insize)
	layer2 = Dense(units=numnodes,kernel_initializer='he_normal')
	layer3 = Dense(units=4, activation='softmax')
	model.add(layer1)
	#model.add(BatchNormalization())
	#model.add(Dropout(0.5))
	model.add(Activation('relu'))
	model.add(layer2)
	#model.add(BatchNormalization())
	#model.add(Dropout(0.5))
	model.add(Activation('relu'))
	model.add(layer3)
	opt = optimizers.SGD(lr=learningrate, nesterov=False)
	if (learning == "Adam"):
		opt = optimizers.Adam()
	model.compile(loss='categorical_crossentropy', optimizer=opt, metrics=['accuracy'])

	early_stopping = EarlyStopping(monitor='val_loss', patience=10)
	#make_exist("models/" + filename + "_" + str(numnodes) + "_" + str(batchsize))
	filepath = "models/" + filename + "_" + str(numnodes) + \
		"_" + str(batchsize) + "_" + str(int(1000*learningrate)) + "_" + opttype".hdf5"
	checkpt = ModelCheckpoint(filepath, save_best_only=True)
	hist = model.fit_generator(genbatches(inname, insize, 100000, batchsize), datasize/batchsize,  \
		epochs=1000, callbacks=[early_stopping, checkpt], verbose=1, \
		validation_data=(sqdata[:,0:insize], sqdata[:,insize:insize+4]))

from keras.models import Sequential
from keras.layers import Dense, Activation
from keras.callbacks import ModelCheckpoint, EarlyStopping
from keras.regularizers import l2
from keras import optimizers
import numpy as np
import matplotlib.pyplot as plt
import sys
import os
import errno

def make_exist(path):
	try:
		os.makedirs(path)
	except OSError as exception:
		if exception.errno != errno.EEXIST:
			raise



if __name__ == "__main__":
	if (len(sys.argv) != 7):
		print "usage: type latsize numtrials error_rate numnodes batchsize"
		sys.exit()
	lattype = sys.argv[1]
	latsize = int(sys.argv[2])
	numtrials = int(sys.argv[3])
	p = float(sys.argv[4])
	numnodes = int(sys.argv[5])
	batchsize = int(sys.argv[6])

	filename = lattype + "_" + str(latsize) + "_" + str(numtrials) + "_" + str(int(p*1000))
 	sqdata = np.genfromtxt("data/" + filename + ".csv", delimiter=',')
	
	insize = latsize * latsize
	model = Sequential()
	layer1 = Dense(units=numnodes, activation='relu',input_dim=insize)
	layer2 = Dense(units=4, activation='softmax')
	model.add(layer1)
	model.add(layer2)
	#sgd = optimizers.SGD(lr=0.1, decay=1e-3)
	model.compile(loss='categorical_crossentropy', optimizer='sgd', metrics=['accuracy'])

	early_stopping = EarlyStopping(monitor='val_loss', patience=3)
	make_exist("models/" + filename + "_" + str(numnodes) + "_" + str(batchsize))
	filepath = "models/" + filename + "_" + str(numnodes) + "_" + str(batchsize) + \
			"/{epoch:02d}_{loss:.04f}_{acc:.04f}_{val_loss:.04f}_{val_acc:.4f}.h5"
	checkpt = ModelCheckpoint(filepath)
	model.fit(sqdata[:,0:insize], sqdata[:,insize:insize+4], batch_size=batchsize,  \
		epochs=400, validation_split=0.3, callbacks=[early_stopping, checkpt])
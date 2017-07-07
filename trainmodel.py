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
	if (len(sys.argv) != 9):
		print "usage: type latsize numtrials error_rate numnodes batchsize learningrate filename"
		sys.exit()
	lattype = sys.argv[1]
	latsize = int(sys.argv[2])
	numtrials = int(sys.argv[3])
	p = float(sys.argv[4])
	numnodes = int(sys.argv[5])
	batchsize = int(sys.argv[6])
	learningrate = float(sys.argv[7])
	outname = sys.argv[8]

	filename = lattype + "_" + str(latsize) + "_" + str(numtrials) + "_" + str(int(p*1000))
 	sqdata = np.genfromtxt("data/" + filename + ".csv", delimiter=',')
	
	insize = latsize * latsize
	model = Sequential()
	layer1 = Dense(units=numnodes, activation='relu',input_dim=insize)
	layer2 = Dense(units=4, activation='softmax')
	model.add(layer1)
	model.add(layer2)
	sgd = optimizers.SGD(lr=learningrate)
	model.compile(loss='categorical_crossentropy', optimizer='sgd', metrics=['accuracy'])

	early_stopping = EarlyStopping(monitor='val_loss', patience=10)
	#make_exist("models/" + filename + "_" + str(numnodes) + "_" + str(batchsize))
	filepath = "models/" + filename + "_" + str(numnodes) + \
		"_" + str(batchsize) + "_" + str(learningrate) + ".h5"
	checkpt = ModelCheckpoint(filepath, save_best_only=True)
	hist = model.fit(sqdata[:,0:insize], sqdata[:,insize:insize+4], batch_size=batchsize,  \
		epochs=400, validation_split=0.3, callbacks=[early_stopping, checkpt], verbose=1)
	with open(outname, "a") as myfile:
		myfile.write(lattype + ", " + str(latsize) + ", " + str(numtrials) + ", " + \
			str(numnodes) + ", " + str(batchsize) + ", " + str(learningrate) + ", " + \
			str(max(hist.history['val_acc'])) + "\n")
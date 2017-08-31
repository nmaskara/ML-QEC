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
from multiprocessing import Pool, Queue, Process

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
		print "file closed"
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

def genset(lattype, latsize, p, setsize, threadid, dataqueue, numthreads, countstart):
	latsize = str(latsize)
	p = str(p)
	ptxt = str(int(float(p)*1000))
	setsize = str(setsize)
	threadid = int(threadid)
	inname = 'data/' + lattype + '_' + latsize + '_' + setsize + '_' + ptxt + '_' + str(threadid) + '.csv'
	count = countstart
	while True:
		os.system('./gendata ' + lattype + ' ' + latsize + ' ' + setsize + ' ' + p +\
		 ' -i ' + str(threadid) + ' -s ' + str(threadid + numthreads * count) + ' > data.txt')
		df = pd.read_csv(inname)
		vals = df.values[:,:-1].astype(bool)
		dataqueue.put(vals)
		count += 1
		#print str(threadid) + '\t' + str(dataqueue.qsize())

def genparallel(insize, numcat, dataqueue):
	while True:
		vals = dataqueue.get()
		#print (vals[:10,0:insize], vals[:10, insize:insize+numcat])	
		yield (vals[:,0:insize], vals[:, insize:insize+numcat])	



def genbatches(filename, insize, numcat, batchsize, initial_epoch, stepsperepoch):
	df = pd.read_hdf(filename)
	numlines = len(df.values)
	values = df.sample(frac=1)
	count = (initial_epoch * stepsperepoch * batchsize) % numlines
	while True:
		dat = np.unpackbits(values[count:count+batchsize], axis=-1)
		yield dat[:, 0:insize], dat[:, insize:insize+numcat]
		count += batchsize
		if count > numlines:
			count = 0
			values = df.sample(frac=1)
			print "shuffled"

def makeModel(input_size, num_nodes, hidden_layers, opt_type, numcat):
	model = Sequential()
	first = True
	while (hidden_layers > 0):
		hidden_layers -= 1
		if (first):
			model.add(Dense(units=num_nodes, kernel_initializer='he_normal', input_dim=input_size))
		else:
			model.add(Dense(units=num_nodes, kernel_initializer='he_normal'))
		model.add(BatchNormalization())
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

def trainModel(lattype, opttype, latsize, stepsperepoch, numepochs, numnodes, \
	hiddenlayers, batchsize, filename, valname, dirname, copy=0, gendata=False, p=0.1):

	trainfilename = "data/" + filename + ".h5"
	valfilename = "data/" + valname + ".csv"

	if not os.path.isfile(valfilename):
		print "Couldn't find validation data"
		sys.exit(0)
	if (not gendata):
		if not os.path.isfile(trainfilename):
			print "Couldn't find training data"	
			sys.exit(0)

	valdata = pd.read_csv(valfilename).values
	
	insize = latsize * latsize
	if (copy > 0):
		cstr = '_' + str(copy)
	else:
		cstr = ''
	modelpath = "models/" + dirname + '/' + filename + "_" + str(numnodes) + '_' + str(hiddenlayers) + \
		"_" + str(batchsize) + "_" + opttype + cstr + ".hdf5"
	bestmodelpath = "models/" + dirname + '/' + filename + "_" + str(numnodes) + '_' + str(hiddenlayers) + \
		"_" + str(batchsize) + "_" + opttype + cstr + "_best.hdf5"
	resultpath = "results/" + dirname + '/' + filename + "_" + str(numnodes) + "_" + str(hiddenlayers) + \
		"_" + str(batchsize) + "_" + opttype + cstr + ".csv"
	bestcheckpt = ModelCheckpoint(bestmodelpath, save_best_only=True)
	lastcheckpt = ModelCheckpoint(modelpath, save_best_only=False)
	record = mycallback(resultpath, stepsperepoch)

	if (lattype == "cc"):
		numcat = 16
	elif (lattype == "open_square"):
		numcat = 16
	elif (lattype == "surface"):
		numcat = 2
	else:
		numcat = 4

	# If model already exists, load model
	if os.path.isfile(modelpath):
		print "Loaded Previous Model"
		model = load_model(modelpath)
	else:
	# otherwise, generate model
		model = makeModel(insize, numnodes, hiddenlayers, opttype, numcat)

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
	
	processes = []
	if (gendata):
		NUMTHREADS=4
		dataqueue = Queue(int(100))
		#rd = Process(target=readdat, args=(dataqueue, insize, numcat))
		countstart = (initial_epoch * stepsperepoch) / (NUMTHREADS-1)
		for i in range(NUMTHREADS-1):
			gendat = Process(target=genset, args=(lattype, latsize, p, batchsize, i, dataqueue, NUMTHREADS-1, countstart))
			gendat.start()
			processes.append(gendat)

		iterator = genparallel(insize, numcat, dataqueue)
	else:
		iterator = genbatches(trainfilename, insize, numcat, batchsize, initial_epoch, stepsperepoch)
		
	print iterator

	hist = model.fit_generator( \
		iterator,\
		stepsperepoch,  \
		epochs=numepochs, initial_epoch=initial_epoch, \
		callbacks=[bestcheckpt, lastcheckpt, record], verbose=1, \
		validation_data=(valdata[:,0:insize], valdata[:,insize:insize+numcat]))
	print hist
	for p in processes:
		p.terminate()
		p.join()



if __name__ == "__main__":
	numparams = 12
	copies = 0
	if ('-c' in sys.argv):
		index = sys.argv.index('-c')
		copies = int(sys.argv[index+1])
		del sys.argv[index]
		del sys.argv[index]
	if (len(sys.argv) != 13):
		print "usage: type opttype latsize stepsperepoch epochs numnodes hiddenlayers batchsize datasize p valsize date"
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
	datasize = int(sys.argv[9])
	p = float(sys.argv[10])
	valsize = int(sys.argv[11])
	dirname = sys.argv[12]

	gendata = False
	if (datasize == -1):
		gendata = True
	filename = lattype + '_' + str(latsize) + '_' + str(datasize) + '_' + str(int(p*1000)) 
	valname = lattype + '_' + str(latsize) + '_' + str(valsize) + '_' + str(int(p*1000))

	if (copies > 0):
		count = 1
		while count <= copies:
			trainModel(lattype, opttype, latsize, stepsperepoch, numepochs, numnodes, \
				hiddenlayers, batchsize, filename, valname, dirname, copy=count, gendata=gendata, p=p)
			count += 1
	else:
		trainModel(lattype, opttype, latsize, stepsperepoch, numepochs, numnodes, \
				hiddenlayers, batchsize, filename, valname, dirname, gendata=gendata, p=p)


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
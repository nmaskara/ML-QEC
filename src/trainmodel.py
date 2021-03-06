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

def genset(lattype, latsize, p, pratio, setsize, threadid, dataqueue, numthreads, depol):
	latsize = str(latsize)
	p = str(p)
	ptxt = str(int(float(p)*1000))
	setsize = str(setsize)
	threadid = int(threadid)
	tail = ''
	if pratio != 0:
		tail += '_corr_' + str(pratio)
	if (depol):
		tail += '_depol'
	inname = 'data/' + lattype + '_' + latsize + '_' + setsize + '_' + ptxt + '_' + str(threadid) + tail + '.csv'
	flags = ' -s ' + str(random.randint(0, sys.maxint))
	if pratio != 0:
		flags += ' -c ' + str(pratio)
	if depol:
		flags += ' -d '
	print './gendata ' + lattype + ' ' + latsize + ' ' + setsize + ' ' + p +\
		 ' -i ' + str(threadid) + flags + ' > data.txt'
	while True:
		os.system('./gendata ' + lattype + ' ' + latsize + ' ' + setsize + ' ' + p +\
		 ' -i ' + str(threadid) + flags + ' > data.txt')
		df = pd.read_csv(inname)
		vals = df.values[:,:-1].astype(bool)
		dataqueue.put(vals)

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

def train(model, lattype, latsize, p, pratio, batchsize, stepsperepoch, numepochs, modelpath, resultpath, valname, depol=False, trainfilename=None, initial_epoch=0 ):
	insize = latsize * latsize
	if (lattype == "cc2" or lattype == "twist"):
		nrows = latsize/2
		insize = 3 * nrows * (nrows+1) / 2

	if (lattype == "cc"):
		numcat = 16
	elif (lattype == "open_square"):
		numcat = 16
	elif (lattype == "surface"):
		numcat = 2
	elif (lattype == "cc2"):
		numcat = 2
	elif (lattype == "twist"):
		numcat = 2
	else:
		numcat = 4

	if (depol or lattype == "twist"):
		insize *= 2
		numcat *= 2

	print "insize: " + str(insize)
	print "numcat: " + str(numcat)

	valfilename = "data/" + valname + ".csv"

	if not os.path.isfile(valfilename):
		print "Couldn't find validation data at : " + valfilename
		sys.exit(0)
	if (not gendata):
		if not os.path.isfile(trainfilename):
			print "Couldn't find training data at : " + trainfilename	
			sys.exit(0)

	valdata = pd.read_csv(valfilename).values

	print "error rate: " + str(p)

	processes = []
	if (trainfilename == None):
		NUMTHREADS=4
		dataqueue = Queue(int(100))
		#rd = Process(target=readdat, args=(dataqueue, insize, numcat))
		countstart = (initial_epoch * stepsperepoch) / (NUMTHREADS-1)
		for i in range(NUMTHREADS-1):
			gendat = Process(target=genset, args=(lattype, latsize, p, pratio, batchsize, i, dataqueue, NUMTHREADS-1, depol))
			gendat.start()
			processes.append(gendat)
		iterator = genparallel(insize, numcat, dataqueue)
	else:
		iterator = genbatches(trainfilename, insize, numcat, batchsize, initial_epoch, stepsperepoch)
		
	print iterator

	lastcheckpt = ModelCheckpoint(modelpath, save_best_only=False)
	record = mycallback(resultpath, stepsperepoch)


	hist = model.fit_generator( \
		iterator,\
		stepsperepoch,  \
		epochs=numepochs, initial_epoch=initial_epoch, \
		callbacks=[lastcheckpt, record], verbose=1, \
		validation_data=(valdata[:,0:insize], valdata[:,insize:insize+numcat]) )

	print hist
	for p in processes:
		p.terminate()
		p.join()

def getModel(filename, lattype, latsize, opttype, numnodes, hiddenlayers, batchsize, dirname, depol=False):
	cstr = ''
	modelpath = "models/" + dirname + '/' + filename + "_" + str(numnodes) + '_' + str(hiddenlayers) + \
		"_" + str(batchsize) + "_" + opttype + cstr + ".hdf5"
	bestmodelpath = "models/" + dirname + '/' + filename + "_" + str(numnodes) + '_' + str(hiddenlayers) + \
		"_" + str(batchsize) + "_" + opttype + cstr + "_best.hdf5"
	resultpath = "results/" + dirname + '/' + filename + "_" + str(numnodes) + "_" + str(hiddenlayers) + \
		"_" + str(batchsize) + "_" + opttype + cstr + ".csv"

	insize = latsize * latsize
	if (lattype == "cc2" or lattype == "twist"):
		nrows = latsize/2
		insize = 3 * nrows * (nrows+1) / 2

	if (lattype == "cc"):
		numcat = 16
	elif (lattype == "open_square"):
		numcat = 16
	elif (lattype == "surface"):
		numcat = 2
	elif (lattype == "cc2"):
		numcat = 2
	elif (lattype == "twist"):
		numcat = 2
	else:
		numcat = 4

	if (depol or lattype == "twist"):
		insize *= 2
		numcat *= 2

	print "insize: " + str(insize)
	print "numcat: " + str(numcat)

	new = True
	# If model already exists, load model
	if os.path.isfile(modelpath):
		print "Loaded Previous Model"
		model = load_model(modelpath)
		new = False
	else:
		# otherwise, generate model
		model = makeModel(insize, numnodes, hiddenlayers, opttype, numcat)

	return (model, modelpath, resultpath, new)

def trainModel(lattype, opttype, latsize, stepsperepoch, numepochs, numnodes, \
	hiddenlayers, batchsize, filename, valname, dirname, gendata=False, p0=(0.1, 0), depol=False):

	'''trainfilename = "data/" + filename + ".h5"
	valfilename = "data/" + valname + ".csv"

	if not os.path.isfile(valfilename):
		print "Couldn't find validation data at : " + valfilename
		sys.exit(0)
	if (not gendata):
		if not os.path.isfile(trainfilename):
			print "Couldn't find training data at : " + trainfilename	
			sys.exit(0)

	valdata = pd.read_csv(valfilename).values
	
	p = p0[0]
	p2 = 0
	if (p0[1] != 0):
		p2 = p / p0[1]

	insize = latsize * latsize
	if (lattype == "cc2"):
		nrows = latsize/2
		insize = 3 * nrows * (nrows+1) / 2

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
	elif (lattype == "cc2"):
		numcat = 2
	else:
		numcat = 4

	if (depol):
		insize *= 2
		numcat *= 2

	print "insize: " + str(insize)
	print "numcat: " + str(numcat)


	# If model already exists, load model
	if os.path.isfile(modelpath):
		print "Loaded Previous Model"
		model = load_model(modelpath)
	else:
		# otherwise, generate model
		model = makeModel(insize, numnodes, hiddenlayers, opttype, numcat)'''

	model, modelpath, resultpath = getModel(filename, lattype, latsize, opttype, numnodes, hiddenlayers,batchsize, dirname, depol=depol)

	initial_epoch = 0

	'''if os.path.isfile(resultpath):
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
			gendat = Process(target=genset, args=(lattype, latsize, p, pratio, batchsize, i, dataqueue, NUMTHREADS-1, depol))
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
		p.join()'''

	train(model, lattype, latsize, p, pratio, batchsize, stepsperepoch, numepochs, modelpath, resultpath, depol=depol, trainfilename=filename, initial_epoch=initial_epoch)


# execute
if __name__ == "__main__":
	numparams = 12
	copies = 0
	pratio = 0
	depol = False
	if ('-copy' in sys.argv):
		index = sys.argv.index('-copy')
		copies = int(sys.argv[index+1])
		del sys.argv[index]
		del sys.argv[index]
	if ('-c' in sys.argv):
		index = sys.argv.index('-c')
		pratio = int(sys.argv[index+1])
		del sys.argv[index]
		del sys.argv[index]
	if ('-d' in sys.argv):
		index = sys.argv.index('-d')
		depol = True
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

	if (pratio != 0):
		filename += '_corr_' + str(pratio)
		valname += '_corr_' + str(pratio)

	if (depol):
		filename += '_depol'
		valname += '_depol'

	model, modelpath, resultpath, new = getModel(filename, lattype, latsize, opttype, numnodes, hiddenlayers,batchsize, dirname, depol=depol)

	if (datasize == -1):
		filename = None

	initial_epoch = 0
	if (new):
		print "Starting pre-training"
		train(model, lattype, latsize, p*0.2, pratio, batchsize, stepsperepoch, 2, modelpath, resultpath, valname, depol=depol, trainfilename=filename, initial_epoch=initial_epoch)
		train(model, lattype, latsize, p*0.3, pratio, batchsize, stepsperepoch, 2, modelpath, resultpath, valname, depol=depol, trainfilename=filename, initial_epoch=initial_epoch)
		train(model, lattype, latsize, p*0.4, pratio, batchsize, stepsperepoch, 2, modelpath, resultpath, valname, depol=depol, trainfilename=filename, initial_epoch=initial_epoch)
		train(model, lattype, latsize, p*0.5, pratio, batchsize, stepsperepoch, 2, modelpath, resultpath, valname, depol=depol, trainfilename=filename, initial_epoch=initial_epoch)
		train(model, lattype, latsize, p*0.6, pratio, batchsize, stepsperepoch, 2, modelpath, resultpath, valname, depol=depol, trainfilename=filename, initial_epoch=initial_epoch)
		train(model, lattype, latsize, p*0.7, pratio, batchsize, stepsperepoch, 2, modelpath, resultpath, valname, depol=depol, trainfilename=filename, initial_epoch=initial_epoch)
		train(model, lattype, latsize, p*0.8, pratio, batchsize, stepsperepoch, 2, modelpath, resultpath, valname, depol=depol, trainfilename=filename, initial_epoch=initial_epoch)
		train(model, lattype, latsize, p*0.9, pratio, batchsize, stepsperepoch, 2, modelpath, resultpath, valname, depol=depol, trainfilename=filename, initial_epoch=initial_epoch)
		train(model, lattype, latsize, p*0.95, pratio, batchsize, stepsperepoch, 2, modelpath, resultpath, valname, depol=depol, trainfilename=filename, initial_epoch=initial_epoch)
	else:
		toread = open(resultpath, 'r')
		initial_epoch = len(toread.readlines())
		toread.close()
		print "Initial Epoch: " + str(initial_epoch)		

	print "Training"
	train(model, lattype, latsize, p, pratio, batchsize, stepsperepoch, numepochs, modelpath, resultpath, valname, depol=depol, trainfilename=filename, initial_epoch=initial_epoch)

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
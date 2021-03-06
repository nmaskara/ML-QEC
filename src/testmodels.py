from keras.models import load_model
import numpy as np
import pandas as pd
import os, sys

def getRem(model, indat, outdat):
	rem = [[],[]]
	out = model.predict(indat)
	for inpt, outpt, targ in zip(indat, out, outdat):
		if (np.argmax(outpt) != np.argmax(targ)):
			rem[0].append(inpt)
			rem[1].append(targ)
	rem[0] = np.array(rem[0])
	rem[1] = np.array(rem[1])
	return rem


'''
data = []
f = 'square_5_100000_'
for p in np.linspace(0.02, 0.2, 10):
	df = pd.read_csv('data/' + f + str(int(p*1000)) + '.csv')
	data.append(df.values)
	print "loaded: " + str(p)
lsiz = 5*5'''

'''
model5a = load_model('models/sq5wl/square_5_20000000_100_5_1_1000_adam_1.hdf5')
model5b = load_model('models/sq5wl/square_5_20000000_100_5_1_1000_adam_2.hdf5')
model5c = load_model('models/sq5wl/square_5_20000000_100_5_1_1000_adam_3.hdf5')
model10 = load_model('models/sq5wl/square_5_20000000_100_10_1_1000_adam.hdf5')
model20 = load_model('models/sq5wl/square_5_20000000_100_20_1_1000_adam.hdf5')
model40 = load_model('models/sq5wl/square_5_20000000_100_40_1_1000_adam.hdf5')

indat = data[4][:,0:lsiz].astype(int)
outdat = data[4][:,lsiz:lsiz+4].astype(int)

rem5a = getRem(model5a, indat, outdat)
rem5b = getRem(model5b, indat, outdat)
rem5c = getRem(model5c, indat, outdat)'''

datsz = str(100000)
infile = sys.argv[1]
outfile = open(sys.argv[2], 'w')
pstart = int(sys.argv[3])
pend = int(sys.argv[4])
inc = int(sys.argv[5])

#start = float(sys.argv[3])
#end = float(sys.argv[4])
#samples = int(sys.argv[5])

if (os.path.isdir(infile)):
	files = os.listdir(infile)
	direc = infile
elif (os.path.isfile(infile)):
	direc = os.path.dirname(infile)
	files = [os.path.basename(infile)]
else:
	print "Invalid input file/direcory"
	abort()

for fil in files:
	print fil
	args = fil.split('_')
	model = load_model(direc +'/' + fil)
	dist = int(args[1])
	if (args[0] == 'cc2'):
		lsiz = 3 * (dist/2) * (dist/2 + 1) / 2
		numcats = 2
	elif (args[0] == 'twist'):
		lsiz = 3 * (dist/2) * (dist/2 + 1)
		numcats = 4
	elif (args[0] == 'surface'):
		numcats = 2
		lsiz = dist * dist
	elif (args[0] == 'square'):
		numcats = 4
		lsiz = dist * dist
	elif (args[0] == 'cc'):
		numcats = 16
		lsiz = dist * dist
	else:
		print "UNKOWN TYPE : " + args[0]
		sys.exit()
	depol = False
	pratio = 0
	corr = False
	i = 4
	while (i < len(args)):
		if (args[i] == 'depol'):
			depol = True
			if ( args[0] != 'twist'):
				numcats *= 2
				lsiz *= 2

		if (args[i] == 'corr'):
			pratio = int(args[5])
			corr = True
			i += 1
		i += 1


	#for ptxt in np.arange(10, 200, 10):
	for ptxt in range(pstart, pend, inc):
		p = float(ptxt) / 1000
		print p
		ptxt = str(ptxt)

		cmd = ' '.join(['./gendata', args[0], args[1], datsz, str(p)])
		if (depol):
			cmd += ' -d'
		if (corr):
			cmd += ' -c ' + str(pratio)
		#if len(args) == 10:
		#	ptxt += '_corr_' + str(args[5])
		#	cmd += ' -c ' + str(args[5])

		filename = 'data/' + '_'.join([args[0], args[1], datsz, ptxt])
		if (corr):
			filename += '_corr_' + str(pratio)
		if (depol):
			filename += '_depol'
		filename += '.csv'

		if not os.path.exists(filename):
			print cmd
			os.system(cmd)

		dat = pd.read_csv(filename).values
		print "loaded data: " + filename

		loss, acc = model.evaluate(dat[:,0:lsiz], dat[:,lsiz:lsiz+numcats])
		print loss, 1-acc
		outfile.write(fil + ', ' + str(p) + ', ' + str(loss) + ', ' + str(1-acc) + '\n')
outfile.close()




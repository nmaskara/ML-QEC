from keras.models import load_model
import numpy as np
import os, sys
import pandas as pd


latsize = 9
numcats = 4
data = []
#f = 'cc2_11_100000_'
f = 'square_9_100000_'
lsiz = latsize * latsize
#lsiz = 3 * (latsize/2) * (latsize/2 + 1) / 2

fil = sys.argv[1]
outfile = open(sys.argv[2], 'a')
model = load_model(fil)
print "loaded model"
for p in np.linspace(0.02, 0.2, 10):
	dat = pd.read_csv('data/' + f + str(int(p*1000)) + '.csv').values
	print "loaded data: " + f + str(int(p*1000))
	loss, acc = model.evaluate(dat[:,0:lsiz], dat[:,lsiz:lsiz+numcats])
	print loss, 1-acc
	outfile.write(fil + ', ' + str(p) + ', ' + str(loss) + ', ' + str(1-acc) + '\n')
outfile.close()

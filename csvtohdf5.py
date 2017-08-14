import numpy as np
import pandas as pd
import os, sys

if len(sys.argv) != 2:
	print "usage: python csvtohdf5.py dataname"
	sys.exit(0)


filename = 'data/' + sys.argv[1] + '.csv'
chsize = 100000
outfile = 'data/' + sys.argv[1] + '.h5'

if os.path.exists(outfile):
	os.remove(outfile)

count = 0
for df in pd.read_csv(filename, chunksize=chsize):
	vals = df.values[:,:-1].astype(bool)
	vals = np.packbits(vals, axis=-1)
	df = pd.DataFrame(data=vals)
	df.to_hdf(outfile, 'data', format='table', append=True)
	print 'appended: ' + str(count)
	count += 1

'''
for (a, b) in zip(pd.read_csv(filename, chunksize=chsize), pd.read_hdf(outfile, chunksize=chsize)):
	print a
	print np.unpackbits(b.values, axis=-1)
	break'''


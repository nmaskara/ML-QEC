import numpy as np
import pandas as pd
import os, sys

if len(sys.argv) != 5:
	print "usage: python gendata.py type latsize datasize p"
	sys.exit(0)

print sys.argv[1]
print sys.argv[2]
print sys.argv[3]
print sys.argv[4]

incsize = 1000000
datasize = int(sys.argv[3])
ptxt = str(int(1000*float(sys.argv[4])))
print ptxt

inname = 'data/' + sys.argv[1] + '_' + sys.argv[2] + '_' + str(incsize) + '_' + ptxt + '.csv'
outname = 'data/'+ sys.argv[1] + '_' + sys.argv[2] + '_' + sys.argv[3] + '_' + ptxt + '.h5'

if os.path.exists(outname):
	os.remove(outname)

added = 0
chsize = 100000
while (added < datasize):
	os.system('./gendata ' + sys.argv[1] + ' ' + sys.argv[2] + ' ' + str(incsize) + ' ' + sys.argv[4])
	for df in pd.read_csv(inname, chunksize=chsize):
		vals = df.values[:,:-1].astype(bool)
		vals = np.packbits(vals, axis=-1)
		df = pd.DataFrame(data=vals)
		df.to_hdf(outname, 'data', format='table', append=True)	
	added += incsize
	print "Added: " + str(added)

'''
for (a, b) in zip(pd.read_csv(filename, chunksize=chsize), pd.read_hdf(outfile, chunksize=chsize)):
	print a
	print np.unpackbits(b.values, axis=-1)
	break'''


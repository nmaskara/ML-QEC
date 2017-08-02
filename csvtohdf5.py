import numpy as np
import pandas as pd
import os


filename = 'data/square_7_50000000_100.csv'
chsize = 1000000
outfile = 'data/square_7_50000000_100.h5'

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


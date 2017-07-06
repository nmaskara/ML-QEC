#include "Lattice.hpp"

class LatticeA: public Lattice {
	
	Lattice (int, int) {
		Lattice::ncols = width;
		Lattice::nrows = height;
		Lattice::data.resize(2 * width * height);
		Lattice::Zcheck.resize(width * height);
		Lattice::Xcheck.resize(width * height);
		Lattice::nqubits = width * height + (width)*(height);
		NCOLS = Lattice::ncols;
		NROWS = Lattice::nrows;

		Lattice::init_maps();		
	}

	void init_maps() {

	}
}

/*
 * generates 4 maps from data qubits to corresponding
 * measurement qubits, and 4 vice versa. total of 8 maps
 * are initialized. 
 */
void LatticeA::init_maps() {
	int ncols = Lattice::ncols;
	int nrows = Lattice::nrows;
	int nqubits = Lattice::nqubits;
	int nX = (ncols)*(nrows);
	int nZ = (ncols) * nrows;
	X1.resize(nqubits);
	X2.resize(nqubits);
	Z1.resize(nqubits);
	Z2.resize(nqubits);

	mX0.resize(nX);
	mX1.resize(nX);
	mX2.resize(nX);
	mX3.resize(nX);
	mZ0.resize(nZ);
	mZ1.resize(nZ);
	mZ2.resize(nZ);
	mZ3.resize(nZ);

	fill(X1.begin(), X1.end(), -1);
	fill(X2.begin(), X2.end(), -1);
	fill(Z1.begin(), Z1.end(), -1);
	fill(Z2.begin(), Z2.end(), -1);

	fill(mX0.begin(), mX0.end(), -1);
	fill(mX1.begin(), mX1.end(), -1);
	fill(mX2.begin(), mX2.end(), -1);
	fill(mX3.begin(), mX3.end(), -1);

	fill(mZ0.begin(), mZ0.end(), -1);
	fill(mZ1.begin(), mZ1.end(), -1);
	fill(mZ2.begin(), mZ2.end(), -1);
	fill(mZ3.begin(), mZ3.end(), -1);
	//generate qmaps
	for (int i = 0; i < nqubits; i++) {
		if (i < ncols * nrows) {	//map top-left lattice
			int r = itor(i);
			int c = itoc(i);
			if (r > 0)
				X1[i] = (rctoi(r-1,c, nrows-1, ncols));
			if (r < nrows - 1)
				X2[i] = (rctoi(r,c, nrows-1, ncols));
			if (c > 0)
				Z1[i] = (rctoi(r,c-1, nrows, ncols-1));
			if (c < ncols - 1)
				Z2[i] = (rctoi(r,c, nrows, ncols-1));
		}
		else {
			int j = i - ncols*nrows;	//bottom-right matrix is indexed with offset
			int r = (j / (ncols-1));
			int c = (j % (ncols-1));
			Z1[i] = (rctoi(r,c,nrows, ncols-1));
			Z2[i] = (rctoi(r+1,c, nrows, ncols-1));
			X1[i] = (rctoi(r,c, nrows-1, ncols));
			X2[i] = (rctoi(r,c+1, nrows-1, ncols));			
		}
	}
	//generate X mmaps
	for (int i = 0; i < nX; i++) {
		int r = itor(i, nrows-1, ncols);
		int c = itoc(i, nrows-1, ncols);
		mX0[i] = rctoi(r,c,nrows, ncols);
		mX1[i] = rctoi(r+1, c, nrows, ncols);
		if (c > 0)
			mX2[i] = rctoi(r, c-1, nrows-1, ncols-1) + nrows * ncols;
		if (c < ncols - 1)
			mX3[i] = rctoi(r, c, nrows-1, ncols-1) + nrows * ncols;
	}
	//generate Z mmaps
	for (int i = 0; i < nZ; i++) {
		int r = itor(i, nrows, ncols-1);
		int c = itoc(i, nrows, ncols-1);
		mZ0[i] = rctoi(r,c,nrows,ncols);
		mZ1[i] = rctoi(r,c+1, nrows, ncols);
		if (r > 0)
			mZ2[i] = rctoi(r-1, c, nrows-1, ncols-1) + nrows*ncols;
		if (r < nrows-1)
			mZ3[i] = rctoi(r,c,nrows-1, ncols-1) + nrows*ncols;
	}

}
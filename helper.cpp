#include "helper.hpp"

using namespace std;
int rctoi(int row, int col, int nrows, int ncols) {
	if (row >= nrows || col >= ncols) {
		cout << "itor: Row Column out of bounds" << endl;
		cout << "r: " << nrows << " c: " << ncols << endl;
	}
	return row * ncols + col;
};

int itor(int i, int nrows, int ncols) {
	if (i > nrows * ncols) {
		cout << "itor: Index out of bounds" << endl;
		return -1; 
	}
	return (i / ncols);
};

int itoc(int i, int nrows, int ncols) {
	if (i > nrows * ncols) {
		cout << "itor: Index out of bounds" << endl;
		return -1; 
	}
	return i % ncols;
};


/*void LatticeB::printLattice() {
	qarray::iterator z = Zcheck.begin();
	qarray::iterator x = Xcheck.begin();
	cout << "3 means two errors" << endl;
	cout << "2 means X error" << endl;
	cout << "1 means Z error" << endl;
	for (int r = 0; r < 2*ncols -1; r++) {
		for (int c = 0; c < 2*nrows-1; c++) {
			if ((r+c) % 2 == 0) {
				qubit * dat;
				if (r % 2 == 0) 
					dat = &data[rctoi(r/2,c/2)];
				else
					dat = &data[rctoi(r/2,c/2,nrows-1,ncols-1) + nrows*ncols];
				if (dat->xerr && dat->zerr)	cout << "3 ";
				else if (dat->xerr)			cout << "2 ";
				else if (dat->zerr)			cout << "1 ";
				else						cout << "0 ";
			}
			else if (r % 2 == 1) {
				if (x->xerr)				cout << "X ";
				else						cout << ". ";
				x++;				
			}
			else if (c % 2 == 1) {
				if (z->zerr)				cout << "Z ";
				else						cout << ". ";
				z++;				
			}
		}
		cout << endl;
	}
}*/

/*
 * generates 4 maps from data qubits to corresponding
 * measurement qubits, and 4 vice versa. total of 8 maps
 * are initialized. 
 */
/*void LatticeB::init_maps() {
	int nX = ncols * nrows;
	int nZ = ncols * nrows;
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

}*/

/*LatticeB::LatticeB(int width, int height) {
	this->data.resize(width * height + (width-1)*(height-1));
	this->Zcheck.resize((width-1)*height);
	this->Xcheck.resize(width*(height-1));
	this->nqubits = width * height + (width-1)*(height-1);
	NCOLS = this->ncols;
	NROWS = this->nrows;

	LatticeB::init_maps();
}*/
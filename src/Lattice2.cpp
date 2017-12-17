#include "Lattice2.hpp"

using namespace std;

// BY CONVENTION, MEASUREMENT INDEX -1 IS THE LATTICE "BOUNDARY"

void Lattice2::genCorrPairErrs(double p1, double p2) {
	assert((p1 + p2) < 1);	
	if (p2 == 0) {
		generateErrors(p1);
		return;
	}
	// number of possible correlated errors
	cout << "LAT2 GEN COOR ERRS NOT WRITTEN YET" << endl;
	abort();
}

/* For a single data qubit, returns the adjacent measurement qubit indexes */
/*vector<vector<int> > Lattice::getadj(vector<int> indexes) {
 	vector<vector<int> > fulllist(indexes.size());
 	for (vector<int>::iterator i = indexes.begin(); i != indexes.end(); i++) {
 		vector<int> adjs(4);
 		adjs.push_back(X1[*i]);
		adjs.push_back(X2[*i]);
 		adjs.push_back(Z1[*i]);
 		adjs.push_back(Z2[*i]);
 		fulllist.push_back(indexes);
 	}
 	return fulllist;
 }*/



/* Finds the indexes connecting the minimum path between two indexes */
vector<int> Lattice2::calcMinPath(int x, int y) {
	assert(x != -1);
	vector<int> path;
	// Convert indexes to rows and columns
	int rx = itor(x);
	int cx = itoc(x);
	int size = nrows * ncols;

	// special case to handle the boundary...
	if (y == -1) {
		if (rx <= nrows - rx) {
			while (rx >= 0) {
				path.push_back(rctoi(rx, cx));
				rx -= 1;
			}
		}
		else {
			rx += 1;
			while (rx < nrows) {
				path.push_back(rctoi(rx, cx));
				rx += 1;
			}
			path.push_back(rctoi(cx, ncols-1) + nrows*ncols);
		}
		return path;
	}

	int ry = itor(y);
	int cy = itoc(y);

	// check if wraparound distance is less than absolute distance
	if (min(rx+1, nrows - rx) + min(ry+1, nrows - ry) < abs(rx - ry) + abs(cx - cy)) {
		//cout << x << ", " << y << ":AAA" << endl;
		// if so connect wrapped
		path = calcMinPath(x, -1);
		vector<int> path2 = calcMinPath(y, -1);
		path.insert(path.end(), path2.begin(), path2.end());
	}
	else {
		// For X errors, data qubit columns are in the left lattice.
		while (ry > rx) {
			path.push_back(rctoi(ry, cy));
			ry -= 1;
		}
		while (rx > ry) {
			path.push_back(rctoi(rx, cx));
			rx -= 1;
		}
		// For X errors, data qubit rows are in the right (offset) lattice.
		while(cy > cx) {
			cy -= 1;
			path.push_back(rctoi(ry, cy) + size);
		}
		while(cx > cy) {
			cx -= 1;
			path.push_back(rctoi(rx, cx) + size);
		}		
	}

	return path;
}

void Lattice2::printLattice(ostream& out) {
	//qarray::iterator z = Zcheck.begin();
	vector<int>::iterator x = check.begin();
	//out << "3 means two errors" << endl;
	//out << "2 means X error" << endl;
	out << "1 means error" << endl;
	for (int r = 0; r < 2*nrows + 1; r++) {
		for (int c = 0; c < 2*ncols - 1; c++) {
			if ((r+c) % 2 == 0) {
				qubit * dat;
				if (r == 2 * nrows)
					dat = &data[rctoi(c/2, ncols-1) + nrows*ncols];
				else if (r % 2 == 0) 
					dat = &data[rctoi(r/2,c/2)];
				else
					dat = &data[rctoi(r/2,c/2) + nrows*ncols];
				if (dat->err)				out << "1 ";
				else						out << "_ ";
			}
			else if (r % 2 == 1) {
				if (*x)						out << "X ";
				else						out << ". ";
				x++;				
			}
			else if (c % 2 == 1) {
				out << "  ";
			}
		}
		out << endl;
	}
	out << endl;
}

/*
 * generates 4 maps from data qubits to corresponding
 * measurement qubits, and 4 vice versa. total of 8 maps
 * are initialized. 
 */
void Lattice2::init_maps() {
	//cout << "nr: " << nrows << " nc:" << ncols << endl;
	//ncols = this->ncols;
	//nrows = this->nrows;
	int nX = ncols * nrows;
	qmap X1(ndats);
	qmap X2(ndats);
	mmap mX0(nX);
	mmap mX1(nX);
	mmap mX2(nX);
	mmap mX3(nX);

	fill(X1.begin(), X1.end(), -1);
	fill(X2.begin(), X2.end(), -1);

	fill(mX0.begin(), mX0.end(), -1);
	fill(mX1.begin(), mX1.end(), -1);
	fill(mX2.begin(), mX2.end(), -1);
	fill(mX3.begin(), mX3.end(), -1);
	//generate qmaps

	// qmaps not made yet

	//generate X mmaps
	for (int i = 0; i < nX; i++) {
		int r = itor(i);
		int c = itoc(i);
		mX0[i] = rctoi(r,c);
		if (r < nrows - 1)
			mX1[i] = rctoi(r+1, c);
		else
			mX1[i] = rctoi(c, ncols-1) + nrows * ncols;
		if (c > 0)
			mX2[i] = rctoi(r, c-1) + nrows * ncols;
		if (c < ncols - 1)
			mX3[i] = rctoi(r, c) + nrows * ncols;
	}
	X.push_back(X1);
	X.push_back(X2);
	mX.push_back(mX0);
	mX.push_back(mX1);
	mX.push_back(mX2);
	mX.push_back(mX3);
}




Lattice2::Lattice2(int width, int height) : Base_Lattice(width, height) {
	this->data.resize(2 * width * height);
	this->ndats = width * height + (width)*(height);
	init_maps();		
}

Lattice2::~Lattice2() {}

// Non-periodic lattice, no wraparound. 
int Lattice2::calcDist(int i1, int i2) {
	assert(i1 != -1);
	int r1 = itor(i1);
	int c1 = itoc(i1);
	if (i2 == -1) {
		return min(r1+1, nrows - r1);
	}
	int r2 = itor(i2);
	int c2 = itoc(i2);
	int distwrap = calcDist(i1, -1) + calcDist(i2, -1);
	int distdirc = abs(r2 - r1) + abs(c2 - c1);
	return min(distwrap, distdirc);
}

/* Determine if the correction is valid. Each column in offset lattice and
 * each row in original lattice must have even parity, otherwise there is an
 * error. It is sufficient to check one column and one row.
 * 0 means no error
 * 1 means logical flip vertical
 * 2 means logical flip horizontal
 * 3 means double logical flip */
int Lattice2::checkCorrection() {
	int count_row = 0;
	//Check column in offset matrix
	for (int c = 0; c < ncols; c++) {
		if (data[nrows * ncols + rctoi(c, ncols - 1)].err)
			count_row += 1;
	}

	return ((count_row % 2) );
}
#include "Lattice.hpp"

using namespace std;

void Lattice::genCorrPairErrs(double p1, double p2) {
	assert((p1 + p2) < 1);	
	// number of possible correlated errors
	int ncorr = 3;
	int nX = nrows * ncols;
	for (int i = 0; i < (int) data.size(); i++){
		double randval = (double) mtrand() / mtrand.max();
		if (randval < p1){
			data[i].err = !data[i].err;
		}
		else {
			vector<int> adj;
			// generate ncorr random variables
			vector<int> rands(ncorr);
			for (uint k = 0; k < rands.size(); k++) {
				if ((double) mtrand() / mtrand.max() < p2) {
					rands[k] = 1;
				}
				else{
					rands[k] = 0;
				}
			}
			if (i < nX) {
				int r = itor(i);
				int c = itoc(i);
				// get adjacent indexes
				
				// above left
				//i0 = nX + rctoi((nrows + r - 1) % nrows, (ncols + c - 1) % ncols);
				// above vertical
				//i1 = rctoi((nrows + r - 1) % nrows, c);
				// above right
				//i2 = nX + rctoi((nrows + r - 1) % nrows, c);
				// below left
				//i3 = 
				adj.push_back(nX + rctoi(r, (ncols + c - 1) % ncols));
				// below vertical
				//i4 = 
				adj.push_back(rctoi((r+1) % nrows, c));
				// below right
				//i5 = 
				adj.push_back(nX + rctoi(r, c));

			}
			else {
				int r = itor(i - nX);
				int c = itoc(i - nX);
				//right above
				//i0 = rctoi(r, (c + 1) % ncols);
				// right horizontal
				//i1 = 
				adj.push_back(nX + rctoi(r, (c + 1) % ncols));
				// right below
				//i2 = 
				adj.push_back(rctoi((r + 1) % nrows, (c + 1) % ncols));
				// left above
				//i3 = rctoi(r, c);
				// left horizontal
				//i4 = nX + rctoi(r, (ncols + c - 1) % ncols);
				// left below
				//i5 = 
				adj.push_back(rctoi((r + 1) % nrows, c));
			}
			//cout << rands << endl;
			// flip each pair independently
			for (uint k = 0; k < adj.size(); k++) {
				if (rands[k])	{
					data[i].err = !data[i].err;	
					data[adj[k]].err = !data[adj[k]].err;				
				}
				else {
					//cout << "skip" << endl;
				}
			}

		}
	}
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
vector<int> Lattice::calcMinPath(int x, int y) {
	// Convert indexes to rows and columns
	int rx = itor(x);
	int cx = itoc(x);
	int ry = itor(y);
	int cy = itoc(y);

	// Set path row and col before switch
	int pr = rx;
	int pc = cy;

	// Switch so x < y
	if (rx > ry) {
		int temp = rx;
		rx = ry;
		ry = temp;
	}
	if (cx > cy) {
		int temp = cx;
		cx = cy;
		cy = temp;
	}
	// Determine whether direct path or going through an edge is closest,
	// and add all indexes in between to the list.
	vector<int> path;
	// For X errors, data qubit columns are in the left lattice.
	if (abs(ry - rx) < nrows - abs(ry - rx)) {
		for (int k = rx + 1; k <= ry; k++ ) {
			path.push_back(rctoi(k, pc));
		}
	}
	else {
		for (int k = 0; k <= rx; k++) {
			path.push_back(rctoi(k, pc));
		}
		for (int k = ry + 1; k < nrows; k++) {
			path.push_back(rctoi(k, pc));
		}
	}
	// For X errors, data qubit rows are in the right (offset) lattice.
	int size = nrows * ncols;
	if (abs(cy - cx) < ncols - abs(cy - cx)) {
		for (int k = cx; k < cy; k++) {
			path.push_back(size + rctoi(pr, k));
		}
	}
	else {
		for (int k = 0; k < cx; k++) {
			path.push_back(size + rctoi(pr, k));
		}
		for (int k = cy; k < ncols; k++) {
			path.push_back(size + rctoi(pr, k));
		}
	}
	return path;
}

void Lattice::printLattice(ostream& out) {
	//qarray::iterator z = Zcheck.begin();
	vector<int>::iterator x = check.begin();
	//out << "3 means two errors" << endl;
	//out << "2 means X error" << endl;
	out << "1 means error" << endl;
	for (int r = 0; r < 2*nrows; r++) {
		for (int c = 0; c < 2*ncols; c++) {
			if ((r+c) % 2 == 0) {
				qubit * dat;
				if (r % 2 == 0) 
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
void Lattice::init_maps() {
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
	for (int i = 0; i < ndats; i++) {
		if (i < ncols * nrows) {	//map top-left lattice
			int r = itor(i);
			int c = itoc(i);
			if (r > 0)
				X1[i] = (rctoi(r-1,c));
			else
				X1[i] = rctoi(nrows-1, c);
			X2[i] = (rctoi(r,c));
		}
		else {
			int j = i - ncols*nrows;	//bottom-right matrix is indexed with offset
			int r = (j / (ncols));
			int c = (j % (ncols));
			X1[i] = (rctoi(r,c));
			if (c < ncols - 1)
				X2[i] = (rctoi(r,c));
			else
				X2[i] = rctoi(r, 0);			
		}
	}
	//generate X mmaps
	for (int i = 0; i < nX; i++) {
		int r = itor(i);
		int c = itoc(i);
		mX0[i] = rctoi(r,c);
		if (r < nrows - 1)
			mX1[i] = rctoi(r+1, c);
		else
			mX1[i] = rctoi(0, c);
		if (c > 0)
			mX2[i] = rctoi(r, c-1) + nrows * ncols;
		else
			mX2[i] = rctoi(r, ncols-1) + nrows * ncols;
		mX3[i] = rctoi(r, c) + nrows * ncols;
	}
	X.push_back(X1);
	X.push_back(X2);
	mX.push_back(mX0);
	mX.push_back(mX1);
	mX.push_back(mX2);
	mX.push_back(mX3);
}




Lattice::Lattice(int width, int height) : Base_Lattice(width, height) {
	this->data.resize(2 * width * height);
	this->ndats = width * height + (width)*(height);
	init_maps();		
}

Lattice::~Lattice() {}

int Lattice::calcDist(int i1, int i2) {
	int r1 = itor(i1);
	int c1 = itoc(i1);
	int r2 = itor(i2);
	int c2 = itoc(i2);
	int dr = min(abs(r2 - r1), nrows - abs(r2 - r1));
	int dc = min(abs(c2 - c1), ncols - abs(c2 - c1));
	//if (dr + dc == 1)	return 3;
	return dr + dc;
}

/* Determine if the correction is valid. Each column in offset lattice and
 * each row in original lattice must have even parity, otherwise there is an
 * error. It is sufficient to check one column and one row.
 * 0 means no error
 * 1 means logical flip vertical
 * 2 means logical flip horizontal
 * 3 means double logical flip */
int Lattice::checkCorrection() {
	int count_col = 0;
	int count_row = 0;
	//Check column in offset matrix
	int size = nrows * ncols;
	for (int r = 0; r < nrows; r++) {
		if (data[size + rctoi(r, 0)].err)
			count_col += 1;
	}
	for (int c = 0; c < ncols; c++) {
		if (data[rctoi(0, c)].err)
			count_row += 1;
	}

	return ((count_col % 2) * 2 + (count_row % 2) );
}
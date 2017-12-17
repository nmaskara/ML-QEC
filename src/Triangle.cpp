#include "Triangle.hpp"

using namespace std;

void Triangle::genCorrPairErrs(double p1, double p2) {
	assert((p1 + p2) < 1);	
	// number of possible correlated errors
	int ncorr = 5;
	int nX = nrows * ncols;
	for (int i = 0; i < (int) data.size(); i++){
		double randval = (double) mtrand() / mtrand.max();
		if (randval < p1){
			data[i].err = !data[i].err;
		}
		else if (randval < p1 + p2) {	// horizontal edges
			data[i].err = !data[i].err;
			vector<int> adj;
			if (i < nX) {
				int r = itor(i);
				int c = itoc(i);
				//int rabv = (nrows + r - 1) % nrows;
				//int rbel = (r + 1) % nrows;
				//int cleft = (ncols + c - 1) % ncols;
				int cright = (c + 1) % ncols;
				// right diagonal above
				//adj.push_back(2*nX + rctoi(rabv, c));
				// right vertical above
				//adj.push_back(nX + rctoi(rabv, cright));
				// right horizontal
				adj.push_back(rctoi(r, cright));
				// right diagonal below
				adj.push_back(2*nX + rctoi(r, cright));
				// right vertical below
				adj.push_back(nX + rctoi(r, cright));
				// left diagonal below
				adj.push_back(2*nX + rctoi(r, c));
				// left vertical below
				adj.push_back(nX + rctoi(r, c));
				// left horizontal
				//adj.push_back(rctoi(r, cleft));
				// left diagonal above
				//adj.push_back(2*nX + rctoi(rabv, cleft));
				// left vertical above
				//adj.push_back(nX + rctoi(rabv, c));
				// above horizontal right
			}
			else if (i - nX < nX) {	// vertical edges
				int r = itor(i-nX);
				int c = itoc(i-nX);
				//int rabv = (nrows + r - 1) % nrows;
				int rbel = (r + 1) % nrows;
				int cleft = (ncols + c - 1) % ncols;
				//int cright = (c + 1) % ncols;
				// above left horizontal
				//adj.push_back(rctoi(r, cleft));
				// above left diagonal
				//adj.push_back(2*nX + rctoi(rabv, cleft));
				// above vertical
				//adj.push_back(nX + rctoi(rabv, c));
				// above right horizontal
				//adj.push_back(rctoi(r, c));
				// above right diagonal
				adj.push_back(2*nX + rctoi(r, c));
				// below right horizontal
				adj.push_back(rctoi(rbel, c));
				// below right diagonal
				adj.push_back(2*nX + rctoi(rbel, c));
				// below vertical
				adj.push_back(nX + rctoi(rbel, c));
				// below left horizontal
				adj.push_back(rctoi(rbel, cleft));
				// below left diagonal
				//adj.push_back(2*nX + rctoi(r, cleft));
			}
			else {	// diagonal edges 
				int r = itor(i - 2*nX);
				int c = itoc(i - 2*nX);
				//int rabv = (nrows + r - 1) % nrows;
				int rbel = (r + 1) % nrows;
				//int cleft = (ncols + c - 1) % ncols;
				int cright = (c + 1) % ncols;
				// left vertical down			
				//adj.push_back(nX + rctoi(r,c));
				// left horizontal left
				//adj.push_back(rctoi(r, cleft));
				// left diagonal
				//adj.push_back(rctoi(rabv, cleft));
				// left vertical up
				//adj.push_back(nX + rctoi(rabv, c));
				// left horizontal right
				//adj.push_back(rctoi(r, c));
				// right vertical up
				adj.push_back(nX + rctoi(r, cright));
				// right horizontal right
				adj.push_back(rctoi(rbel, cright));
				// right diagonal
				adj.push_back(2*nX + rctoi(rbel, cright));
				// right vertical below
				adj.push_back(nX + rctoi(rbel, cright));
				// right horizontal left
				adj.push_back(nX + rctoi(rbel, c));
			}
			int index = (int) ( (randval - p1) / (p2 / ncorr) );
			data[adj[index]].err = !data[adj[index]].err;
		}
	}
} 

void Triangle::printLattice(ostream& out) {
	vector<int>::iterator x = check.begin();
	out << "1 means error" << endl;
	int size = nrows * ncols;
	for (int r = 0; r < 2*nrows; r++) {
		for (int c = 0; c < 2*ncols; c++) {
			if (r % 2 == 0 && c % 2 == 0) {
				if (*x)						out << "X ";
				else						out << ". ";
				x++;
			}
			else {
				qubit *dat;
				if (r % 2 == 0) {
					dat = &data[rctoi(r/2, c/2)];
				}
				else if (c % 2 == 0) {
					dat = &data[size + rctoi(r/2, c/2)];
				}
				else {
					dat = &data[2*size + rctoi(r/2, c/2)];
				}
				if (dat->err)				out << "1 ";
				else						out << "_ ";
			}
		}
		out << endl;
	}
	out << endl;
}

void Triangle::init_maps() {
	int nX = ncols * nrows;
	// each data qubit is adjacent to two measurement qubits
	qmap X1(ndats);
	qmap X2(ndats);
	// each measurement qubit is adjacent to six data qubits
	mmap m0(nX);
	mmap m1(nX);
	mmap m2(nX);
	mmap m3(nX);
	mmap m4(nX);
	mmap m5(nX);


	fill(X1.begin(), X1.end(), -1);
	fill(X2.begin(), X2.end(), -1);

	fill(m0.begin(), m0.end(), -1);
	fill(m1.begin(), m1.end(), -1);
	fill(m2.begin(), m2.end(), -1);
	fill(m3.begin(), m3.end(), -1);
	fill(m4.begin(), m4.end(), -1);
	fill(m5.begin(), m5.end(), -1);
	// generate qmaps
	for (int i = 0; i < ndats; i++) {
		if (i < nX) {	// map upper lattice
			int r = itor(i);
			int c = itoc(i);
			X1[i] = rctoi(r, c);
			if (c < ncols - 1) {
				X1[i] = rctoi(r, c+1);
			}
			else {
				X1[i] = rctoi(r, 0);
			}
		}
		else if (i < nX * 2) {	// map bottom left lattice
			int r = itor(i-nX);
			int c = itoc(i-nX);
			X1[i] = rctoi(r, c);
			if (r < nrows - 1) {
				X2[i] = rctoi(r+1, c);
			}
			X2[i] = rctoi(0, c);
		}
		else { // map bottom right lattice
			int r = itor(i - nX*2);
			int c = itoc(i - nX*2);
			X1[i] = rctoi(r, c);
			if (r < nrows - 1 && c < ncols - 1) {
				X2[i] = rctoi(r+1, c+1);
			}
			else if (r == nrows && c < ncols - 1) {
				X2[i] = rctoi(0, c+1);
			}
			else if (c == ncols && r < nrows - 1) {
				X2[i] = rctoi(r+1, 0);
			}
			else {
				X2[i] = rctoi(0, 0);
			}
		}
	}

	// generate mmaps
	for (int i  = 0; i < nX; i++) {
		int r = itor(i);
		int c = itoc(i);
		m0[i] = i;			// qubit to the right
		m1[i] = nX + i;		// qubit below
		m2[i] = nX*2 + i;	// qubit down and right
		// qubit to the left
		if (c > 0) {
			m3[i] = rctoi(r, c-1);
		}
		else {
			m3[i] = rctoi(r, ncols-1);
		}
		// qubit above
		if (r > 0) {
			m4[i] = nX +  rctoi(r-1, c);
		}
		else {
			m4[i] = nX + rctoi(nrows - 1, c);
		}
		// qubit above and left
		if (c > 0 && r > 0) {
			m5[i] = nX*2 + rctoi(r-1, c-1);
		}
		else if (c == 0 && r > 0) {
			m5[i] = nX*2 + rctoi(r-1, ncols - 1);
		}
		else if (c > 0 && r == 0) {
			m5[i] = nX*2 + rctoi(nrows - 1, c-1);
		}
		else {
			m5[i] = nX*2 + rctoi(nrows - 1, ncols - 1);
		}
	}

	// add arrays to X and mX
	X.push_back(X1);
	X.push_back(X2);
	mX.push_back(m0);
	mX.push_back(m1);
	mX.push_back(m2);
	mX.push_back(m3);
	mX.push_back(m4);
	mX.push_back(m5);
}

Triangle::Triangle(int width, int height) : Base_Lattice(width, height) {
	this->data.resize(3 * width * height);
	this->ndats = 3 * width * height;
	init_maps();
}

Triangle::~Triangle() {}

int Triangle::calcDist(int i1, int i2) {
	assert(i1 != i2);
	if (i1 > i2) {
		int temp = i1;
		i2 = i1;
		i2 = temp;
	}
	int r1 = itor(i1);
	int c1 = itoc(i1);
	int r2 = itor(i2);
	int c2 = itoc(i2);
	int dr = r2 - r1;
	assert(dr >= 0);
	int dc = c2 - c1;
	int dist;
	if (dc > 0) {
		dist = min(	min(max(nrows - dr, ncols - dc), max(dr, dc)), 
					min(dr, nrows-dr)  + min(dc, ncols - dc) );
	}
	else {
		dist = min( min(max(nrows - dr, -dc), max(dr, ncols + dc)), 
					min(dr, nrows-dr) + min(ncols + dc, -dc) );
	}
	//if (dist==1) return 3;
	return dist;
}

vector<int> Triangle::calcMinPath(int i1, int i2) {
	vector<int> path;
	assert(i1 != i2);
	if (i1 > i2) {
		int temp = i1;
		i1 = i2;
		i2 = temp;
	}
	int size = nrows * ncols;
	int r1 = itor(i1);
	int c1 = itoc(i1);
	int r2 = itor(i2);
	int c2 = itoc(i2);
	int dr = r2 - r1;
	int dc = c2 - c1;
	assert(dr >= 0);
	int mindist = calcDist(i1, i2);
	if (dc >= 0) {
		if (mindist == max(dr, dc)) {
			for (int k = 0; k < min(dr, dc); k++) {
				path.push_back(2*size + rctoi(r1+k, c1+k));
			}
			if (dr < dc) {
				for (int k = dr; k < dc; k++) {
					path.push_back(rctoi(r1 + dr, c1 + k));
				}
			}
			else {
				for (int k = dc; k < dr; k ++) {
					path.push_back(size + rctoi(r1 + k, c1 + dc));
				}
			}
		}
		else if (mindist == max(nrows - dr, ncols - dc)) {
			for (int k = 0; k < min(nrows-dr, ncols-dc); k++) {
				path.push_back(2*size + rctoi((r2+k)%nrows, (c2+k)%ncols));
			}
			if ( nrows-dr < ncols-dc) {
				for (int k = nrows - dr; k < (ncols-dc); k++) {
					path.push_back(rctoi(r1, (c2+k)%ncols));
				}
			}
			else {
				for (int k = ncols - dc; k < (nrows-dr); k++) {
					path.push_back(size + rctoi((r2+k)%nrows, c1));
				}
			}
		}
		else if (mindist == min(dr, nrows-dr)  + min(dc, ncols - dc)) {
			if (dc <= ncols - dc) {
				for (int k = 0; k < dc; k++) {
					path.push_back(rctoi(r1, (c1+k)%ncols));
				}
			}
			else {
				for (int k = 0; k < ncols - dc; k++) {
					path.push_back(rctoi(r1, (c2+k)%ncols));
				}
			}
			if (dr < nrows - dr) {
				for (int k = 0; k < dr; k++) {
					path.push_back(size + rctoi((r1+k)%nrows, c2));
				}
			}
			else {
				for (int k = 0; k < nrows - dr; k++) {
					path.push_back(size + rctoi((r2+k)%nrows, c2));
				}
			}
		}
	}
	else if (dc < 0) {
		if (mindist == max(nrows - dr, -dc)) {
			for (int k = 0; k < min(nrows - dr, -dc); k++) {
				path.push_back(2*size + rctoi((r2+k)%nrows, (c2+k)%ncols));
			}
			if (nrows - dr < -dc) {
				for (int k = nrows - dr; k < -dc; k++) {
					path.push_back(rctoi(r1, (c2+k)%ncols));
				}
			}
			else {
				for (int k = -dc; k < nrows - dr; k++) {
					path.push_back(size + rctoi((r2+k)%nrows, c1));
				}
			}
		}
		else if (mindist == max(dr, ncols + dc)) {
			for (int k = 0; k < min(dr, ncols + dc); k++ ) {
				path.push_back(2*size + rctoi((r1+k)%nrows, (c1+k)%ncols));
			}
			if (dr < ncols + dc) {
				for (int k = dr; k < ncols + dc; k++) {
					path.push_back(rctoi(r2, (c1+k)%ncols));
				}
			}
			else {
				for (int k = ncols + dc; k < dr; k++) {
					path.push_back(size + rctoi((r1+k)%ncols, c2));
				}
			}
		}
		else {
			if (-dc <= ncols + dc) {
				for (int k = 0; k < -dc; k++) {
					path.push_back(rctoi(r1, (c2+k)%ncols));
				}
			}
			else {
				for (int k = 0; k < ncols + dc; k++) {
					path.push_back(rctoi(r1, (c1+k)%ncols));
				}
			}
			if (dr <= nrows - dr) {
				for (int k = 0; k < dr; k++) {
					path.push_back(size + rctoi((r1+k)%nrows, c2));
				}
			}
			else {
				for (int k = 0; k < nrows - dr; k++) {
					path.push_back(size + rctoi((r2+k)%nrows, c2));
				}
			}
		}
	}
	return path;
}

int Triangle::checkCorrection() {
	int count_col = 0;
	int count_row = 0;
	int size = nrows * ncols;
	for (int r = 0; r < nrows; r++) {
		if (data[rctoi(r, 0)].err)
			count_col += 1;
		if (data[size*2 + rctoi(r, 0)].err)
			count_col += 1;
	}
	for (int c = 0; c < ncols; c++) {
		if (data[size + rctoi(0, c)].err)
			count_row += 1;
		if (data[size*2 + rctoi(0, c)].err)
			count_row += 1;
	}
	return ((count_col % 2) * 2 + (count_row % 2) );
}
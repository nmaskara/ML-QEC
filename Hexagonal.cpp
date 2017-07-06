#include "Hexagonal.hpp"

using namespace std;

void Hexagonal::genCorrPairErrs(double p1, double p2) {
	assert((p1 + p2) < 1);	
	// number of possible correlated errors
	int ncorr = 2;
	int nX = nrows * ncols;
	for (int i = 0; i < (int) data.size(); i++) {
		double randval = (double) mtrand() / mtrand.max();
		if (randval < p1) {
			data[i].err = !data[i].err;
		}
		else if (randval < p1 + p2) {
			data[i].err = !data[i].err;
			vector<int> adj;
			if (i < nX) {
				int r = itor(i);
				int c = itoc(i);
				//int rabv = (nrows + r - 1) % nrows;
				int rbel = (r + 1) % nrows;
				//int cleft = (ncols + c - 1) % ncols;
				//int cright = (c + 1) % ncols;
				// vertical above
				//adj.push_back(rctoi(rabv, c));
				// vertical below
				adj.push_back(rctoi(rbel, c));
				if (r % 2 == 0) { // if even row, diagonal right
					// diagonal right
					adj.push_back(nX + rctoi(r/2, c));
					// diagonal left 
					//adj.push_back(nX + rctoi(r/2, cleft));
				}	
				else { // if odd row, diagonal left
					// diagonal left
					//adj.push_back(nX + rctoi((rabv)/2, cleft));
					// diagonal right
					adj.push_back(nX + rctoi((rbel)/2, c));
				}
			}
			else {
				int r = 2 * itor(i - nX);
				int c = itoc(i - nX);
				//int rabv = (nrows + r - 1) % nrows;
				int rbel = (r + 1) % nrows;
				//int cleft = (ncols + c - 1) % ncols;
				int cright = (c + 1) % ncols;
				//cout << i << " " << r << " " << c << " "<< rabv << " " << rbel 
				//	<< " " << cleft << " " << cright << " " << endl;
				// vertical left
				//adj.push_back(rctoi(r,c));
				//adj.push_back(rctoi(rabv, c));
				// vertical right
				adj.push_back(rctoi(r, cright));
				adj.push_back(rctoi(rbel, cright));	
			}
			int index = (int) ((randval - p1) / (p2 / ncorr));
			//cout << index << endl;
			data[adj[index]].err = !data[adj[index]].err;
		}
	}
}

void Hexagonal::init_maps()  {
	int nX = ncols * nrows;
	qmap X1(ndats);
	qmap X2(ndats);
	// each measurement qubit is adjacent to three data qubits
	mmap m0(nX);
	mmap m1(nX);
	mmap m2(nX);

	fill(X1.begin(), X1.end(), -1);
	fill(X2.begin(), X2.end(), -1);

	fill(m0.begin(), m0.end(), -1);
	fill(m1.begin(), m1.end(), -1);
	fill(m2.begin(), m2.end(), -1);
	// generate qmaps
	for (int i = 0; i < ndats; i++) {
		if (i < nX) {	//map vertical edges
			int r = itor(i);
			int c = itoc(i);
			X1[i] = rctoi(r, c);
			X2[i] = rctoi((r+1)%nrows, c);
		}
		else {	// map diagonal edges
			int r = 2 * itor(i - nX);
			int c = itoc(i - nX);
			X1[i] = rctoi(r, c);
			X2[i] = rctoi((r+1)%nrows, (c+1)%ncols);
		}
	}
	// generate mmaps
	for (int i = 0; i < nX; i++) {
		int r = itor(i);
		int c = itoc(i);
		// map qubit above
		m0[i] = rctoi((nrows + r-1)%nrows, c);
		// map qubit below
		m1[i] = rctoi(r, c);
		// if even row, adjecent edge is down and right
		if (r%2 == 0) {
			m2[i] = nX + rctoi(r/2, c);
		}
		else {
			m2[i] = nX + rctoi(r/2, (ncols + c-1)%ncols);
		}
	}
	X.push_back(X1);
	X.push_back(X2);
	mX.push_back(m0);
	mX.push_back(m1);
	mX.push_back(m2);
}

void Hexagonal::printLattice(ostream& out) {
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
			else if (r % 2 == 0 || (c % 2 == 1 && (r/2) % 2 == 1)) {
				out << "  ";
			}
			else {
				qubit *dat;
				if (c % 2 == 0) {
					dat = &data[rctoi(r/2, c/2)];
				}
				else {
					dat = size + &data[rctoi(r/4, c/2)];
				}
				if (dat->err)				out << "1 ";
				else						out << "_ ";
			}
		}
		out << endl;
	}
	out << endl;
}

int Hexagonal::calcDist(int i1, int i2) {
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
	int maxc1 = (dr + (r1 + 1) % 2) / 2;
	int maxc2 = (nrows - dr + (r2 + 1) % 2) / 2;
	int dleft1, dright1, dleft2, dright2;
	if (dc < 0) {
		dleft1 = -dc;
		dleft2 = ncols + dc;
		dright1 = ncols + dc;
		dright2 = -dc;
	}
	else if (dc > 0) {
		dleft1 = ncols - dc;
		dleft2 = dc;
		dright1 = dc;
		dright2 = ncols - dc;
	}
	else {
		dleft1 = 0;
		dleft2 = 0;
		dright1 = 0;
		dright2 = 0;
	}
	dright1 = max(dright1-maxc1, 0);
	dright2 = max(dright2-maxc2, 0);
	int dist1 = dr + 2 * min(dleft1, dright1);
	int dist2 = nrows - dr + 2 * min(dleft2, dright2);	
	//if (min(dist1,dist2) == 1) return 3;	
	return min(dist1, dist2);
}

vector<int> Hexagonal::calcMinPath(int i1, int i2) {
	vector<int> path;
	int nX = nrows * ncols;
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
	int maxc1 = (dr + (r1 + 1) % 2) / 2;
	int maxc2 = (nrows - dr + (r2 + 1) % 2) / 2;
	int dleft1, dright1, dleft2, dright2;
	if (dc < 0) {
		dleft1 = -dc;
		dleft2 = ncols + dc;
		dright1 = ncols + dc;
		dright2 = -dc;
	}
	else if (dc > 0) {
		dleft1 = ncols - dc;
		dleft2 = dc;
		dright1 = dc;
		dright2 = ncols - dc;
	}
	else {
		dleft1 = 0;
		dleft2 = 0;
		dright1 = 0;
		dright2 = 0;
	}
	dright1 = max(dright1-maxc1, 0);
	dright2 = max(dright2-maxc2, 0);
	int dist1 = dr + 2 * min(dleft1, dright1);
	int dist2 = nrows - dr + 2 * min(dleft2, dright2);
	int ir, ic, rfinal, cfinal, dright, dleft;

	if (dist1 <= dist2) {
		ir = r1;
		ic = c1;
		rfinal = r2;
		cfinal = c2;
		dright = dright1;
		dleft = dleft1;
	}
	else {
		ir = r2;
		ic = c2;
		rfinal = r1;
		cfinal = c1;
		dright = dright2;
		dleft = dleft2;
	}
	// Move right
	if (dright <= dleft) {
		// Move diagonally if possible and neccessary
		while (ic != cfinal && ir != rfinal) {
			if (ir % 2 == 0) {
				path.push_back(nX + rctoi(ir/2, ic));
				ir = (ir + 1) % nrows;
				ic = (ic + 1) % ncols;
			}
			else {
				path.push_back(rctoi(ir, ic));
				ir = (ir + 1) % nrows;
			}
		}
		// Move vertically between rows
		while (ir != rfinal) {
			path.push_back(rctoi(ir, ic));
			ir = (ir + 1) % nrows;
		}
		// Move horizontally by moving viertically then diagonally.
		while (ic != cfinal) {
			if (ir % 2 == 0) {
				path.push_back(nX + rctoi(ir/2, ic));
				path.push_back(rctoi(ir, (ic + 1) % ncols));
				ic = (ic + 1)% ncols;
			}
			else {
				path.push_back(rctoi((nrows+ir-1) % nrows, ic));
				path.push_back(nX + rctoi((nrows+ir-1) % nrows / 2, ic));
				ic = (ic + 1) % ncols;
			}
		}			
	}
	else {
		while (ir != rfinal) {
			path.push_back(rctoi(ir, ic));
			ir = (ir + 1) % nrows;
		}
		while (ic != cfinal) {
			if (ir % 2 == 0) {
				path.push_back(rctoi(ir, ic));
				path.push_back(nX + rctoi(ir/2, (ncols + ic - 1)%ncols));
				ic = (ncols + ic - 1) % ncols;
			}
			else {
				path.push_back(nX + rctoi((nrows+ir-1) % nrows / 2, (ncols + ic - 1) % ncols));
				path.push_back(rctoi((nrows+ir-1) % nrows, (ncols + ic - 1) % ncols));
				ic = (ncols + ic - 1) % ncols;
			}
		}
	}
	assert((int) path.size() == calcDist(i1, i2));

	return path;
}

// Columns only contain diagonal elements. Even rows contain vertical and
// diagonal edges. Odd rows contain only vertical edges.
int Hexagonal::checkCorrection() {
	int count_col = 0;
	int count_row = 0;
	int size = nrows * ncols;
	for (int r = 0; r < nrows; r++) {
		if (r%2 == 0 && data[size + rctoi(r/2, 0)].err)
			count_col += 1;
	}
	for (int c = 0; c < ncols; c++) {
		if (data[rctoi(0, c)].err)
			count_row += 1;
		if (data[size + rctoi(0, c)].err)
			count_row += 1;
	}
	return ((count_col % 2) * 2 + (count_row % 2) );
}

Hexagonal::~Hexagonal() {}

// width and height are meausred in hexagons
Hexagonal::Hexagonal(int width, int height) : Base_Lattice(width, height) {
	nrows = height*2;
	check.resize(2*width*height);
	fill(check.begin(), check.end(), 0);
	nerrs = 2 * width * height;
	data.resize(3*width*height);
	ndats = 3 * width * height;
	init_maps();
}



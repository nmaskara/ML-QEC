#include "Triangle_ColorCode.hpp"

using namespace std;

void Triangle_ColorCode::applyCorrection(pairlist matching) {
	assert(matching.size() == 0);
	vector<int> errors = getErrors();
	for (vector<int>::iterator i = errors.begin(); i != errors.end(); i++) {
		vector<int> path = pathTopLeft(*i);
		for (vector<int>::iterator p = path.begin(); p != path.end(); p++) {
			data[*p].err = !data[*p].err;
		}
	}
	checkErrors();
	// If odd number of total errors, must flip 2nd data qubit (top left upper right)
	int numleft = getErrors().size();
	if (numleft) {
		assert(numleft == 3);
		data[1].err = !data[1].err;
	}
}

void Triangle_ColorCode::genCorrPairErrs(double p1, double p2) {
	cout << "COLOR CODE: CorrPairErrs Method Not Written Yet." << endl;
	abort();
}

void Triangle_ColorCode::printLattice(ostream& out) {
	vector<int>::iterator x = check.begin();
	vector<qubit>::iterator d = data.begin();
	out << "1 means error" << endl;
	//int size = nrows * ncols;
	for (int r = 0; r < 2*nrows; r++) {
		for (int c = 0; c < 2*ncols; c++) {
			if (r % 2 == 0 && c % 2 == 0) {
				if (*x)						out << "X ";
				else						out << ". ";
				x++;
			}
			else if (r % 2 == 1 && c % 2 == 1) {
				if (d->err && (d+1)->err)	out << "= ";
				else if (d->err)			out << "< ";
				else if ((d+1)->err)		out << "> ";
				else						out << "_ ";
				d += 2;
			}
			else{
				out << "  ";
			}
		}
		out << endl;
	}
	out << endl;
}

void Triangle_ColorCode::init_maps() {
	int nX = ncols * nrows;
	// each data qubit is adjacent to three measurement qubits
	//qmap X1(ndats);
	//qmap X2(ndats);

	// each measurement qubit is adjacent to six data qubits
	mmap m0(nX);
	mmap m1(nX);
	mmap m2(nX);
	mmap m3(nX);
	mmap m4(nX);
	mmap m5(nX);


	//fill(X1.begin(), X1.end(), -1);
	//fill(X2.begin(), X2.end(), -1);

	fill(m0.begin(), m0.end(), -1);
	fill(m1.begin(), m1.end(), -1);
	fill(m2.begin(), m2.end(), -1);
	fill(m3.begin(), m3.end(), -1);
	fill(m4.begin(), m4.end(), -1);
	fill(m5.begin(), m5.end(), -1);
	// generate qmaps

	// QMAPS not written yet

	// generate mmaps
	for (int i  = 0; i < nX; i++) {
		int r = itor(i);
		int c = itoc(i);
		int rabv = (r - 1 + nrows) % nrows;
		int cleft = (c - 1 + ncols) % ncols;
		//qubits right/below
		m0[i] = 2*i;
		m1[i] = 2*i + 1;
		//qubits left/above
		m2[i] = 2*rctoi(rabv, cleft);
		m3[i] = 2*rctoi(rabv, cleft) + 1;
		//qubit left below
		m4[i] = 2*rctoi(r, cleft) + 1;
		//qubit right above
		m5[i] = 2*rctoi(rabv, c);
	}

	// add arrays to X and mX
	//X.push_back(X1);
	//X.push_back(X2);
	mX.push_back(m0);
	mX.push_back(m1);
	mX.push_back(m2);
	mX.push_back(m3);
	mX.push_back(m4);
	mX.push_back(m5);
}

Triangle_ColorCode::Triangle_ColorCode(int width, int height) : Base_Lattice(width, height) {
	this->data.resize(2 * width * height);
	this->ndats = 2 * width * height;
	init_maps();
}

Triangle_ColorCode::~Triangle_ColorCode() {}


int Triangle_ColorCode::calcDist(int i1, int i2) {
	cout << "Color Code: calcdist not written yet." << endl;
	abort();
	return 0;	
}

vector<int> Triangle_ColorCode::calcMinPath(int i1, int i2) {
	vector<int> path;
	cout << "Color Code: calcMinPath not written yet." << endl;
	abort();
	return path;	
}

vector<int> Triangle_ColorCode::pathTopLeft(int i) {
	vector<int> path;

	int r = itor(i);
	int c = itoc(i);
	
	int r0 = r % 3;
	int c0 = c % 3;

	// Determine color of vertex
	int type = -1;	// three types.
	if ( (r0 == 0 && c0 == 0) || (r0 == 1 && c0 == 2) || (r0 == 2 && c0 == 1)) {
		type = 0;
	}
	else if ( (r0 == 0 && c0 == 1) || (r0 == 1 && c0 == 0) || (r0 == 2 && c0 == 2)) {
		type = 1;
	}
	else if ( (r0 == 0 && c0 == 2) || (r0 == 1 && c0 == 1) || (r0 == 2 && c0 == 0)) {
		type = 2;
	}
	else {
		cout << "Invalid Type. Get path.";
		abort();
	}

	int dr, dc;
	if (type == 0)	{
		dr = r - 0;
		dc = c - 0;
	}
	else if (type == 1) {
		dr = r;
		dc = c - 1;
	}
	else {
		dr = r - 1;
		dc = c - 1;
	}
	/*cout << i << endl;
	cout << r0 << ", " << c0 << endl;
	cout << r << ", " << c << endl;
	cout << type << endl;
	cout << dr << ", " << dc << endl;
	cout << endl;*/

	while (dr != 0 || dc != 0) {
		int cleft = (c - 1 + ncols) % ncols;
		int c2left = (c - 2 + ncols) % ncols;
		int rabv = (r - 1 + nrows ) % nrows;
		int r2abv = (r - 2 + nrows ) % nrows;
		if (dr > dc && dc > 0) {
			// add qubits
			path.push_back(2*rctoi(rabv, cleft) + 1);
			path.push_back(2*rctoi(r2abv, cleft));
			dr -= 2; r-= 2;
			dc -= 1; c -= 1;
		}
		else if (dc > dr && dr > 0) {
			path.push_back(2*rctoi(rabv, cleft ));
			path.push_back(2*rctoi(rabv, c2left) + 1);
			dc -= 2; c -= 2;
			dr -= 1; r -= 1;
		}
		else if (dr <= 0) {
			path.push_back(2*rctoi(r, cleft) + 1);
			path.push_back(2*rctoi(r, cleft));
			dc -= 1; c -= 1;
			dr += 1; r += 1;
		}
		else {
			path.push_back(2*rctoi(rabv, c));
			path.push_back(2*rctoi(rabv, c) + 1);
			dr -= 1; r -= 1;
			dc += 1; c += 1;
		}
	}
	return path;
}


// Returns integer representation of
// 4-bit binary string. in order 
// count_bcol count_ccol count_brow count_crow
int Triangle_ColorCode::checkCorrection() {
	int count_bcol = 0;
	int count_ccol = 0;
	int count_brow = 0;
	int count_crow = 0;
	
	for (int r = 0; r < nrows; r++) {
		if (r % 3 == 0) {
			if (data[2*rctoi(r, 0)].err)
				count_bcol += 1;
			if (data[2*rctoi(r, 0) + 1].err) {
				count_bcol += 1;
				count_ccol += 1;
			}
		}
		else if (r % 3 == 1) {
			if (data[2*rctoi(r, 0)].err){
				count_bcol += 1;
				count_ccol += 1;
			}
			if (data[2*rctoi(r, 0) + 1].err) {
				count_ccol += 1;
			}
		}
		else {
			if (data[2*rctoi(r, 0)].err) {
				count_ccol += 1;
			}
			if (data[2*rctoi(r, 0) + 1].err) {
				count_bcol += 1;
			}
		}
	}
	for (int c = 0; c < ncols; c++) {
		if (c % 3 == 0) {
			if (data[2*rctoi(0, c)].err) {
				count_brow += 1;
				count_crow += 1;
			}
			if (data[2*rctoi(0, c) + 1].err) {
				count_brow += 1;
			}
		}
		else if (c % 3 == 1) {
			if (data[2*rctoi(0, c)].err) {
				count_crow += 1;
			}
			if (data[2*rctoi(0, c) + 1].err) {
				count_crow += 1;
				count_brow += 1;
			}
		}
		else {
			if (data[2*rctoi(0, c)].err) {
				count_brow += 1;
			}
			if (data[2*rctoi(0, c) + 1].err) {
				count_crow += 1;
			}
		}
	}
	return ((count_bcol % 2) * 8 + (count_ccol % 2) * 4 + (count_brow % 2) * 2 + (count_crow % 2) );
}
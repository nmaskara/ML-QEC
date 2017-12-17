#include "Triangle_ColorCode2.hpp"

using namespace std;

int rowmax(int row) {
	int n = row / 3;
	int m = row % 3;
	if (m == 2) 
		return 2*n + 2;
	else
		return 2*n + 1;
}

int latsize(int d) {
	if (d <= 0)
		return 0;
	assert(d % 2 == 1);
	return (3*d*d + 1) / 4;
}

// Helper functions are overrided for use on data qubits
// as transofrmation is more complex
int Triangle_ColorCode2::rctoi(int row, int col) {
	assert (col < rowmax(row));
	int d = 2 * (row / 3) + 1;
	int ct = latsize(d);
	int m = row % 3;
	if (m == 0) {
		ct -= d;
	}
	else if (m == 2) {
		ct += d;
	}
	return ct + col;
}

int invlatsize(int i) {
	double dd = (4*i - 1) / 3;
	int d =(int) sqrt(dd);
	if (d % 2 == 0) 
		return d-1; 
	return d;
}

int Triangle_ColorCode2::itor(int i) {
	i = i + 1;
	int d = invlatsize(i);
	int rem = i - latsize(d);
	int r0 = 3 * ( (d-1) / 2);
	int r;
	//cout << i << ", " << d << ", " << r0 << ", " << rem << endl;
	if (rem > 2*d + 1) {
		r = r0 + 3;
	}	
	else if (rem > d) {
		r = r0 + 2;
	}
	else if (rem > 0) {
		r = r0 + 1;
	}
	else {
		r = r0;
	}
	return r;
}

int Triangle_ColorCode2::itoc(int i) {
	i = i + 1;
	int d = invlatsize(i);
	int rem = i - latsize(d);
	int c;
	if (rem > 2*d + 1) {
		c = rem - (2*d + 1) - 1;
	}	
	else if (rem > d) {
		c = rem - d - 1;
	}
	else if (rem > 0) {
		c = rem - 1;
	}
	else {
		c = d - 1;
	}
	//cout << i << ", " << d << ", " << rem << ", " << c << endl;
	return c;	
}

void Triangle_ColorCode2::applyCorrection(pairlist matching) {
	assert(matching.size() == 0);
	vector<int> errors = getErrors();
	for (vector<int>::iterator i = errors.begin(); i != errors.end(); i++) {
		vector<int> path = pathToBoundary(*i);
		for (vector<int>::iterator p = path.begin(); p != path.end(); p++) {
			data[*p].err = !data[*p].err;
		}
	}
	vector<int> zerrors = getDualErrors();
	for (vector<int>::iterator i = zerrors.begin(); i != zerrors.end(); i++) {
		vector<int> path = pathToBoundary(*i);
		for (vector<int>::iterator p = path.begin(); p != path.end(); p++) {
			data[*p].derr = !data[*p].derr;
		}
	}
	/*checkErrors();
	errors = getErrors();
	cout << errors.size() << endl;
	if ( errors.size() == 1 ) {
		vector<int> path = pathToBoundary(errors[0]);
		for (vector<int>::iterator p = path.begin(); p != path.end(); p++) {
			data[*p].err = !data[*p].err;
		}
	}*/
}

vector<int> Triangle_ColorCode2::getadj(int r, int c) {
	vector<int> adj;
	if (c > 0) {
		adj.push_back(rctoi(r, c-1));	
	} 
	if (c < rowmax(r) - 1) {
		if (r % 3 == 2)
			adj.push_back(rctoi(r-1, c-1));
		else
			adj.push_back(rctoi(r-1, c));
	}
	if (r < (3*nrows)) {
		if (r % 3 == 1)
			adj.push_back(rctoi(r+1, c+1));
		else 
			adj.push_back(rctoi(r+1, c));
	}
	return adj;
}

void Triangle_ColorCode2::genCorrPairErrs(double p1, double p2) {
	assert((p1 + p2) < 1);	
	for (int i = 0; i < (int) data.size(); i++){
		double randval = (double) mtrand() / mtrand.max();
		if (randval < p1){
			data[i].err = !data[i].err;
		}
		int r = itor(i);
		int c = itoc(i);
		if ( (r % 3 == 0 && c % 2 == 0) || (r % 3 != 0 && c % 2 == 1) ) {
			// weight 2 errors
			vector<int> adj = getadj(r, c);
			for (uint k = 0; k < adj.size(); k++) {
				double randval = (double) mtrand() / mtrand.max();
				if (randval < p2)	{
					data[i].err = !data[i].err;	
					data[adj[k]].err = !data[adj[k]].err;				
				}
			}	
		}
	}
}

void Triangle_ColorCode2::genDepolCorrPairErrs(double p1, double p2) {
	assert((p1 + p2) < 1);	
	if (p2 < 0) {
		for (int i = 0; i < (int) data.size(); i++) {
			int choices = 15;	//2 ** 4 - 1
			int r = itor(i);
			int c = itoc(i);
			if ( (r % 3 == 0 && c % 2 == 0) || (r % 3 != 0 && c % 2 == 1) ) {		
				// weight 2 errors
				vector<int> adj = getadj(r,c);
				assert(adj.size() > 0);
				for (uint k = 0; k < adj.size(); k++) {

					double randval = (double) mtrand() / mtrand.max();	
					double diff = p1 - randval;
					if (diff < 0)
						continue;
					int choice = 1 + int(diff * choices / p1);
					int achoice = choice / 4;
					int bchoice = choice % 4;

					if (achoice == 1) {
						data[i].err = !data[i].err;
					}
					else if (achoice == 2) {
						data[i].derr = !data[i].derr;
					}
					else if (achoice == 3) {
						data[i].err = !data[i].err;
						data[i].derr = !data[i].derr;
					}
					else {
						assert(achoice == 0 || achoice == 4);
					}
					if (bchoice == 1) {
						data[adj[k]].err = !data[adj[k]].err;
					}
					else if (bchoice == 2) {
						data[adj[k]].derr = !data[adj[k]].derr;
					}
					else if (bchoice == 3) {
						data[adj[k]].err = !data[adj[k]].err;
						data[adj[k]].derr = !data[adj[k]].derr;
					}
					else {
						assert(bchoice == 0);
					}
				}
			}
		}
	}
	else {
		for (int i = 0; i < (int) data.size(); i++){
			double randval = (double) mtrand() / mtrand.max();
			if (randval < p1 / 3){
				data[i].err = !data[i].err;
			}
			else if (randval < 2 * p1 / 3) {
				data[i].err = !data[i].err;
				data[i].derr = !data[i].derr;
			}
			else if (randval < p1) {
				data[i].derr = !data[i].derr;
			}
			int r = itor(i);
			int c = itoc(i);
			if ( (r % 3 == 0 && c % 2 == 0) || (r % 3 != 0 && c % 2 == 1) ) {
				// weight 2 errors
				vector<int> adj = getadj(r, c);
				for (uint k = 0; k < adj.size(); k++) {				
					double randval = (double) mtrand() / mtrand.max();
					if (randval < p2 / 9) {
						// XX error
						//cout << "XX" << endl;
						data[i].err = !data[i].err;
						data[adj[k]].err = !data[adj[k]].err;
					}
					else if (randval < 2 * p2 / 9) {
						// XY error
						//cout << "XY" << endl;
						data[i].err = !data[i].err;
						data[adj[k]].err = !data[adj[k]].err;
						data[adj[k]].derr = !data[adj[k]].derr;
					}
					else if (randval < 3 * p2 / 9) {
						// XZ error
						//cout << "XZ" << endl;
						data[i].err = !data[i].err;
						data[i].derr = !data[i].derr;
					}
					else if (randval < 4 * p2 / 9) {
						// YX error
						//cout << "YX" << endl;
						data[i].err = !data[i].err;
						data[i].derr = !data[i].derr;
						data[adj[k]].err = !data[adj[k]].err;
					}
					else if (randval < 5 * p2 / 9) {
						// YY error
						//cout << "YY" << endl;
						data[i].err = !data[i].err;
						data[i].derr = !data[i].derr;		
						data[adj[k]].err = !data[adj[k]].err;
						data[adj[k]].derr = !data[adj[k]].derr;			
					}
					else if (randval < 6 * p2 / 9) {
						// YZ error
						//cout << "YZ" << endl;
						data[i].err = !data[i].err;
						data[i].derr = !data[i].derr;
						data[adj[k]].derr = !data[adj[k]].derr;						
					}
					else if (randval < 7 * p2 / 9) {
						// ZX error
						//cout << "ZX" << endl;
						data[i].derr = !data[i].derr;						
						data[adj[k]].err = !data[adj[k]].err;					
					}
					else if (randval < 8 * p2 / 9) {
						// ZY error
						//cout << "ZY" << endl;
						data[i].derr = !data[i].derr;						
						data[adj[k]].err = !data[adj[k]].err;
						data[adj[k]].derr = !data[adj[k]].derr;
					}
					else if (randval < p2){
						// ZZ error
						//cout << "ZZ" << endl;
						data[i].derr = !data[i].derr;	
						data[adj[k]].derr = !data[adj[k]].derr;
					}
				}	
			}
		}	
	}
	
}

void Triangle_ColorCode2::printLattice(ostream& out) {
	vector<int>::iterator z = dualcheck.begin();
	vector<qubit>::iterator d = data.begin();
	out << "1 means X error" << endl;
	out << "2 means Y error" << endl;
	out << "3 means Z error" << endl;
	int N = 3*nrows;
	//int size = nrows * ncols;
	// counter variables record position on the lattice
	int checkrow = 0;
	int checkcol = 0;
	for (int r = 0; r <= 3*nrows; r++) {
		for (int c = 0; c <= 6*ncols; c++) {
			if (r+c >= N && c-r <= N) {
				if ( (r+c) % 2 == N % 2) {
					if (c % 3 == 2) {
						int row = checkrow/3;
						int rem = checkrow % 3;
						int index = 3*(row * (row+1) / 2  + checkcol) + rem;
						if (check[index] && dualcheck[index])
							out << "Y";
						else if (check[index])
							out << "X";
						else if (dualcheck[index])
							out << "Z";
						else
							out << ".";

						checkcol += 1;	
						if (checkcol > row){
							checkcol = 0;
							checkrow += 1;							
						} 

					}
					else {
						if (d->err && d->derr)	out << "2";
						else if (d->err)		out << "1";
						else if (d->derr)		out << "3";
						else					out << "_";
						d++;					
					}
				}
				else {
					out << " ";
				}
			}
			else{
				out << " ";
			}
		}
		out << endl;
	}
	out << endl;
	assert(d == data.end());
}

void Triangle_ColorCode2::printDualLattice(ostream& out) {
	vector<qubit>::iterator d = data.begin();
	out << "1 means error" << endl;
	int N = 3*nrows;
	//int size = nrows * ncols;
	// counter variables record position on the lattice
	int checkrow = 0;
	int checkcol = 0;
	for (int r = 0; r <= 3*nrows; r++) {
		for (int c = 0; c <= 6*ncols; c++) {
			if (r+c >= N && c-r <= N) {
				if ( (r+c) % 2 == N % 2) {
					if (c % 3 == 2) {
						int row = checkrow/3;
						int rem = checkrow % 3;
						int index = 3*(row * (row+1) / 2  + checkcol) + rem;
						if (dualcheck[index])		out << "Z";
						else						out << ".";
						checkcol += 1;	
						if (checkcol > row){
							checkcol = 0;
							checkrow += 1;							
						} 

					}
					else {
						if (d->err && d->derr)	out << "3";
						else if (d->err)		out << "1";
						else if (d->derr)		out << "2";
						else					out << "_";
						d++;						
					}
				}
				else {
					out << " ";
				}
			}
			else{
				out << " ";
			}
		}
		out << endl;
	}
	out << endl;
	assert(d == data.end());
}

void Triangle_ColorCode2::init_maps() {
	int nX = nerrs;
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
	int i = 0;
	for (int r = 0; r < nrows; r++) {
		for (int c = 0; c <= r; c++) {
			int r0 = 3 * r;
			int c0 = 2 * c;
			// color 0
			m0[i] = rctoi(r0, c0);
			m1[i] = rctoi(r0+1, c0);
			m2[i] = rctoi(r0+2, c0+1);
			m3[i] = rctoi(r0+2, c0);
			if (c > 0) {
				m4[i] = rctoi(r0+1, c0-1);
				m5[i] = rctoi(r0, c0-1);				
			}
			i += 1;

			// color 1
			if (c < r) {
				m0[i] = rctoi(r0+1, c0+1);
				m1[i] = rctoi(r0+2, c0+2);				
			}
			m2[i] = rctoi(r0+3, c0+2);
			m3[i] = rctoi(r0+3, c0+1);
			m4[i] = rctoi(r0+2, c0+1);
			m5[i] = rctoi(r0+1, c0);
			i += 1;

			// color 2
			m0[i] = rctoi(r0+2, c0+1);
			m1[i] = rctoi(r0+3, c0+1);
			if (r < nrows - 1) {
				m2[i] = rctoi(r0+4, c0+1);
				m3[i] = rctoi(r0+4, c0);				
			}
			m4[i] = rctoi(r0+3, c0);
			m5[i] = rctoi(r0+2, c0);
			i += 1;
		}
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
	mZ = mX;
}

Triangle_ColorCode2::Triangle_ColorCode2(int width, int height) : Base_Lattice(width, height) {
	assert (width == height);
	assert (width % 2 == 1);
	ncols = width / 2;
	nrows = height / 2;
	check.resize(3 * nrows * (nrows + 1) / 2);
	fill(check.begin(), check.end(), 0);
	dualcheck.resize(3 * nrows * (nrows + 1) / 2);
	fill(dualcheck.begin(), dualcheck.end(), 0);
	nerrs = 3 * nrows * (nrows + 1) / 2;
	data.resize(latsize(width));
	ndats = latsize(width);
	init_maps();
}

Triangle_ColorCode2::~Triangle_ColorCode2() {}


int Triangle_ColorCode2::calcDist(int i1, int i2) {
	cout << "Color Code: calcdist not written yet." << endl;
	abort();
	return 0;	
}

vector<int> Triangle_ColorCode2::calcMinPath(int i1, int i2) {
	vector<int> path;
	cout << "Color Code2: calcMinPath not written yet." << endl;
	abort();
	return path;	
}

vector<int> Triangle_ColorCode2::pathToBoundary(int i) {
	vector<int> path;

	// get row, col, and color of starting plaquette
	// Color has 3 possible values
	int color = i % 3;
	int index = i / 3;
	int r = (-1 + sqrt(8*index + 1)) / 2;
	int c = index - r * (r+1) / 2;	

	// color 0 moves down left, along column
	if (color == 0) {
		while (r < nrows) {
			path.push_back(rctoi(3*r + 2, 2*c));
			path.push_back(rctoi(3*r + 3, 2*c));
			r += 1;
		}
	}
	// color 1 moves left along row
	if (color == 1) {
		while (c >= 0) {
			path.push_back(rctoi(3*r + 2, 2*c + 1));
			path.push_back(rctoi(3*r + 2, 2*c));
			c -= 1;
		}
	}
	// color 2 moves right along row
	if (color == 2) {
		while (c <= r ) {
			path.push_back(rctoi(3*r + 3, 2*c + 1));
			path.push_back(rctoi(3*r + 3, 2*c + 2));
			c += 1;
		}
	}
	return path;
}


// Returns integer representation of
// 1-bit binary string. in order 
// count
int Triangle_ColorCode2::checkCorrection() {
	// return parity of sum of all qubits.
	int count = 0;
	for (uint i = 0; i < data.size(); i++) {
		count += int(data[i].err);
	}
	return count % 2;
}

int Triangle_ColorCode2::checkDualCorrection() {
	// return parity of sum of all qubits.
	int count = 0;
	for (uint i = 0; i < data.size(); i++) {
		count += int(data[i].derr);
	}
	return count % 2;
}
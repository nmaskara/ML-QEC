#include "Twist.hpp"

using namespace std;

int Twist::stabtoi(int section, int row, int col) {
	assert(section >= 0 && section < 3);
	assert(row < nrows+1);
	assert(col < nrows);
	int sqsize = nrows * (nrows+1);
	if (section == 0) {
		return nrows * (row) + col;
	}
	if (section == 1) {
		return sqsize + (nrows) * (row) + col;
	}
	if (section == 2) {
		return 2*sqsize + nrows * (row) + col;
	}
	return -1;
}

vector<int> Twist::itostab(int i) {
	int sqsize = nrows * (nrows+1);
	
	vector<int> corr(3);

	if (i / sqsize == 0) {
		corr[0] = 0;
		corr[1] = i / nrows;
		corr[2] = i % nrows;
	}
	else if(i / sqsize == 1) {
		corr[0] = 1;
		corr[1] = (i-sqsize) / nrows;
		corr[2] = (i-sqsize) % nrows;
	}
	else if (i / sqsize == 2) {
		corr[0] = 2;
		corr[1] = (i-2*sqsize) / nrows;
		corr[2] = (i-2*sqsize) % nrows;
	}
	else {
		cout << "itostab: i out of bounds" << endl;
		abort();
	}
	return corr;
}

int Twist::qubtoi(int section, int row, int col) {
	assert(section < 3);
	assert(row < nrows);
	assert(col < nrows + 1);

	// Central qubit
	if (section == -1) {
		return 0;
	}
	// Each section is an n+1 by n rectangular lattice
	int sqsize = (nrows+1) * (nrows);
	if (section == 0) {
		return 1 + (nrows+1)*row + col;
	}
	if (section == 1) {
		return 1 + sqsize + (nrows+1)*row + col;
	}
	if (section == 2) {
		return 1 + 2*sqsize + (nrows+1)*row + col;
	}
	cout << "Invalid qubit section." << endl;
	abort();
	return -1;
}
vector<int> Twist::itoqub(int i) {
	assert(i < ndats);
	vector<int> corr(3);
	if (i == 0) {
		corr[0] = -1;
		corr[1] = 0;
		corr[2] = 0;
		return corr;
	}
	int sqsize = nrows * (nrows+1);
	i -= 1;
	if (i / sqsize == 0) {
		corr[0] = 0;
	}
	else if (i / sqsize == 1) {
		i -= sqsize;
		corr[0] = 1;
	}
	else if (i / sqsize == 2) {
		i -= 2*sqsize;
		corr[0] = 2;
	}
	else {
		cout << "itoqub: i out of bounds" << endl;
		abort();
	}
	corr[1] = i / (nrows+1);
	corr[2] = i % (nrows+1);
	return corr;
}

// Measure stabilizers
void Twist::checkErrors() {
	for (uint i = 0; i < check.size(); i++) {
		int err = 0;
		for (vector<mmap>::iterator mm = mX.begin(); mm != mX.end(); mm++) {
			if ((*mm)[i] != -1 && data[(*mm)[i]].err) err++;
		}
		for (vector<mmap>::iterator mm = mZ.begin(); mm != mZ.end(); mm++) {
			if ((*mm)[i] != -1 && data[(*mm)[i]].derr) err++;
		}
		err %= 2;
		if (err) check[i] = 1;
		else	 check[i] = 0; 
	}
}

vector<int> Twist::pathToBoundary(int index) {
	vector<int> corr = itostab(index);
	int section = corr[0];
	int row = corr[1];
	int col = corr[2];
	vector<int> path;
	if (row == nrows) {
		// edge stabilizers
		if ( (nrows - col) % 2 == 0) {
			// z edge, move along col
			for (int r = col; r < nrows; r++) {
				path.push_back(qubtoi(section, r, nrows));
			}
		}
		else {
			// x edge, move to corner along last row
			for (int c = col+1; c <= nrows; c++ ) {
				path.push_back(qubtoi(section, nrows-1, c));
			}
		}
	}
	else {
		if ( (row + col) % 2 == 0) {
			// z stabilizer
			for (int r = row; r < nrows; r++) {
				path.push_back(qubtoi(section, r, col+1));
			}
		}
		else {
			// x stabilizer
			for (int c = col+1; c <= nrows; c++) {
				path.push_back(qubtoi(section, row, c));
			}
		}
	}
	return path;
}

/*vector<int> Twist::pathToBoundary(int index) {
	vector<int> corr = itostab(index);
	int section = corr[0];
	int row = corr[1];
	int col = corr[2];
	vector<int> path;
	if (row == nrows) {
		// edge stabilizer, move back to face appropriately
		if (section == 0) {
			int i = stabtoi(section, row, col);
			if (mX[1][i] != -1) 
				path.push_back(mX[1][i]);
			else if (mZ[1][i] != -1)
				path.push_back(mZ[1][i]);
			else{
				cout << "CANNOT DETERMINE PATH from edge, NO m1" << endl;
				abort();
			}
			if ( (nrows - col) % 2 == 1) {
				row -= 1;
				if (col < ncols-1)
					col += 1;
			}
			else {
				if (col == 0) {
					section = 2;
					row = nrows - 1;
					col = 0;
				}
				else {
					row = col - 1;
					col = nrows - 1;
				}
			}
		}
		else if (section == 1) {
			int i = stabtoi(section, row, col);
			if (mX[0][i] != -1) 
				path.push_back(mX[0][i]);
			else if (mZ[1][i] != -1)
				path.push_back(mZ[0][i]);
			else{
				cout << "CANNOT DETERMINE PATH from edge, NO m" << endl;
				abort();
			}
			if ( (nrows - col) % 2 == 1) {
				if (col == 0) {
					section = 2;
					row = 0;
					col = nrows - 1;
				}
				else {
					row -= 1;
					col -= 1;
				}
			}
			else {
				row = col + 1;
				col = nrows - 1;
			}
		}
		else if (section == 2) {
			int i = stabtoi(section, row, col);
			if ( (nrows - col) % 2 == 1) {
				if (mX[1][i] != -1) 
					path.push_back(mX[1][i]);
				else if (mZ[1][i] != -1)
					path.push_back(mZ[1][i]);
				else{
					cout << "CANNOT DETERMINE PATH from edge, NO m1" << endl;
					abort();
				}
				if (col < nrows - 1)
					col += 1;
				row = nrows - 1;
			}
			else {
				if (mX[0][i] != -1) 
					path.push_back(mX[0][i]);
				else if (mZ[1][i] != -1)
					path.push_back(mZ[0][i]);
				else{
					cout << "CANNOT DETERMINE PATH from edge, NO m" << endl;
					abort();
				}	
				if (col < nrows - 1)
					row = col + 1;
				else
					row = col;
				col = nrows - 1;
			}
		}
	}
	while (section != 2 || row != nrows-1 || col != nrows-1) {
		if (section == 0) {
			int i = stabtoi(section, row, col);
			// move diagonally, in the direction of m2
			if (mX[2][i] != -1) 
				path.push_back(mX[2][i]);
			else if (mZ[2][i] != -1)
				path.push_back(mZ[2][i]);
			else{
				cout << "CANNOT DETERMINE PATH, NO m2" << endl;
				abort();
			}
			if (col < nrows - 1) {
				col += 1;			
			}	
			if (row == 0) {
				section = 2;
				row = col;
				col = 0;
			}
			else {
				row -= 1;
			}
		}
		else if (section == 1) {
			int i = stabtoi(section, row, col);
			// move diagonally, in the direction of m1
			if (mX[1][i] != -1) 
				path.push_back(mX[1][i]);
			else if (mZ[1][i] != -1)
				path.push_back(mZ[1][i]);
			else{
				cout << "CANNOT DETERMINE PATH, NO m1" << endl;
				abort();
			}
			if (row < nrows - 1) {
				row += 1;					
			}
			if (col == 0) {
				section = 2;
				col = row;
				row = 0;
			}
			else {
				col -= 1;					
			}		
		}
		else if (section == 2) {
			int i = stabtoi(section, row, col);
			// move diagonally, in the direction of m1
			if (mX[3][i] != -1) 
				path.push_back(mX[3][i]);
			else if (mZ[3][i] != -1)
				path.push_back(mZ[3][i]);
			else{
				cout << "CANNOT DETERMINE PATH, NO m3" << endl;
				abort();
			}
			if (row < nrows - 1){
				row += 1;				
			}
			if (col < nrows - 1) {
				col += 1;					
			}	
		}
	}
	int i = stabtoi(section, row, col);
	if (mX[3][i] != -1) 
		path.push_back(mX[3][i]);
	else if (mZ[3][i] != -1)
		path.push_back(mZ[3][i]);
	else{
		cout << "CANNOT DETERMINE PATH, NO m3" << endl;
		abort();
	}
	return path;
}*/

void Twist::applyCorrection(pairlist matching) {
	assert(matching.size() == 0);
	vector<int> errors = getErrors();
	for (uint i = 0; i < errors.size(); i++) {
		vector<int> path;
		path = paths[errors[i]];
		vector<int> corr = itostab(errors[i]);
		int section = corr[0];
		int row = corr[1];
		int col = corr[2];
		// Z stabilizer
		if ( ((section == 0 || section == 1) && (row + col) % 2 == 0) || (section == 2 && (row + col) % 2 == 1) ) {
			for (vector<int>::iterator p = path.begin(); p != path.end(); p++) {
				data[*p].derr = !data[*p].derr;
			}			
		}
		// X stabilizer
		else {
			for (vector<int>::iterator p = path.begin(); p != path.end(); p++) {
				data[*p].err = !data[*p].err;
			}
		}
	}
	/*checkErrors();
	errors = getErrors();
	if (errors.size() > 0) {
		//assert (errors.size() == 1);
		data[ndats-1].derr = !data[ndats-1].derr;
	}*/

}

void Twist::genCorrPairErrs(double p1, double p2) {

}
int Twist::checkCorrection() {
	int count = 0;
	for (int i = 0; i <= nrows; i++) {
		count += int(data[qubtoi(1, nrows-1, i)].derr);
	}
	for (int i = 0; i < nrows; i++) {
		count += int(data[qubtoi(2, i, nrows)].derr);
	}
	return count % 2;
}
int Twist::checkDualCorrection() {
	int count = 0;
	for (int i = 0; i < nrows; i++) {
		count += int(data[qubtoi(0, i, nrows)].err);
	}
	for (int i = 0; i <= nrows; i++) {
		count += int(data[qubtoi(2, nrows-1, i)].err);
	}
	return count % 2;
}
int Twist::calcDist(int i1, int i2) {
	return 0;
}
vector<int> Twist::calcMinPath(int i1, int i2) {
	vector<int> path;
	return path;
}
void Twist::init_maps() {
	int nX = nerrs;
	// Each measurement qubit is adjacent to 4 data qubits
	mmap x0(nX);
	mmap x1(nX);
	mmap x2(nX);
	mmap x3(nX);

	fill(x0.begin(), x0.end(), -1);
	fill(x1.begin(), x1.end(), -1);
	fill(x2.begin(), x2.end(), -1);
	fill(x3.begin(), x3.end(), -1);

	// Need separate maps for X and Z errors
	mmap z0(nX);
	mmap z1(nX);
	mmap z2(nX);
	mmap z3(nX);

	fill(z0.begin(), z0.end(), -1);
	fill(z1.begin(), z1.end(), -1);
	fill(z2.begin(), z2.end(), -1);
	fill(z3.begin(), z3.end(), -1);

	for (int i = 0; i < nX; i++) {
		vector<int> corr = itostab(i);
		int section = corr[0];
		int row = corr[1];
		int col = corr[2];
		int adj = (3 + section - 1) % 3;
		// Handle "twist" first
		if (section == 0 && col == 0) {
			if (row == 0) {
				x0[i] = qubtoi(-1, 0, 0);
				z0[i] = qubtoi(-1, 0, 0);
				x1[i] = qubtoi(0, 0, 0);
				z2[i] = qubtoi(2, 0, 0);
				z3[i] = qubtoi(0, 0, 1);
			}
			else if (row == nrows) {
				if (nrows % 2 == 1) {
					z0[i] = qubtoi(0, row-1, 0);
					x1[i] = qubtoi(0, row-1, 1);
				}
				else {
					z0[i] = qubtoi(0, 0, row);
					z1[i] = qubtoi(2, row-1, 0);
				}
			}
			else if (row % 2 == 1) {
				z0[i] = qubtoi(0, row-1, 0);
				z1[i] = qubtoi(0, row, 0);
				x2[i] = qubtoi(0, row-1, 1);
				x3[i] = qubtoi(0, row, 1);
			}
			else {
				x0[i] = qubtoi(0, row-1, 0);
				x1[i] = qubtoi(0, row, 0);
				z2[i] = qubtoi(0, row-1, 1);
				z3[i] = qubtoi(0, row, 1);
			}
		}
		else if (row == nrows) {
			if (section == 0 || section == 1) {
				if ((nrows-col) % 2 == 1) {
					assert(col < nrows);
					x0[i] = qubtoi(section, row-1, col);
					x1[i] = qubtoi(section, row-1, col+1);					
				}
				else {
					if (col == 0) {
						assert(section != 0);
						z0[i] = qubtoi(section, 0, row);
						z1[i] = qubtoi(adj, row-1, 0);						
					}
					else {
						z0[i] = qubtoi(section, col, row);
						z1[i] = qubtoi(section, col-1 , row);
					}
				}
			}
			else {
				if ((nrows-col) % 2 == 1) {
					z0[i] = qubtoi(section, row-1, col);
					z1[i] = qubtoi(section, row-1, col+1);					
				}
				else {
					if (col == 0) {
						x0[i] = qubtoi(section, 0, row);
						x1[i] = qubtoi(adj, row-1, 0);
					}
					else {
						x0[i] = qubtoi(section, col, row);
						x1[i] = qubtoi(section, col-1 , row);
					}
				}
			}
		}
		else {
			// z stabilizer
			if ( ((section == 0 || section == 1) && (row + col) % 2 == 0) || (section == 2 && (row + col) % 2 == 1) ) {
				if (row == 0) {
					if (col == 0)
						z0[i] = qubtoi(-1, 0, 0);
					else
						z0[i] = qubtoi(adj, col-1, 0);
					z2[i] = qubtoi(adj, col, 0);
				}
				else {
					z0[i] = qubtoi(section, row-1, col);
					z2[i] = qubtoi(section, row-1, col+1);
				}
				z1[i] = qubtoi(section, row, col);
				z3[i] = qubtoi(section, row, col+1);
			}
			// x stabilizer
			else {
				if (row == 0) {
					if (col == 0)
						x0[i] = qubtoi(-1, 0, 0);
					else
						x0[i] = qubtoi(adj, col-1, 0);
					x2[i] = qubtoi(adj, col, 0);
				}
				else {
					x0[i] = qubtoi(section, row-1, col);
					x2[i] = qubtoi(section, row-1, col+1);
				}
				x1[i] = qubtoi(section, row, col);
				x3[i] = qubtoi(section, row, col+1);				
			}
		}
	}
	mX.push_back(x0);
	mX.push_back(x1);
	mX.push_back(x2);
	mX.push_back(x3);
	mZ.push_back(z0);
	mZ.push_back(z1);
	mZ.push_back(z2);
	mZ.push_back(z3);
}

void printqubit(ostream& out, qubit q) {
	if (q.err && q.derr)	out << "3 ";
	else if (q.err)				out << "1 ";
	else if (q.derr)				out << "2 ";
	else								out << "0 ";	
}

void Twist::printLattice(ostream& out) {
	for (int section = 0; section < 3; section++){
		int adj = (3 + section - 1) % 3;
		// center qubit
		printqubit(out, data[0]);
		// adj qubits
		for (int c = 0; c < nrows; c++) {
			printqubit(out, data[qubtoi(adj, c, 0)]);
		}
		out << endl;
		for (int r = 1; r < 2*nrows+1; r++) {
			if (r % 2 == 0) {
				for (int c = 0; c <= nrows; c++) {	
					int i = qubtoi(section, r/2 - 1, c);
					printqubit(out, data[i]);				
				}			
			}
			else {
				for (int c = 0; c < nrows; c++) {
					int i = stabtoi(section, r/2, c);
					if (check[i])						out << " X";
					else								out << " -";					
				}
				if ( (nrows - r/2) % 2 == 0) {
					int i = stabtoi(section, nrows, r/2);
					if (check[i])						out << " X";
					else								out << " -";	
				}
			}
			out << endl;
		}	
		// edge stabilizers
		for (int c = 0; c < nrows; c++) {
			if ( (nrows - c) % 2 == 1) {
				int i = stabtoi(section, nrows, c);
				if (check[i])						out << " X";
				else								out << " -";	
			}
			else {
				out << "  ";
			}
		}	
		out << endl << endl;
	}
}

Twist::Twist(int width, int height) : Base_Lattice(width, height) {
	assert (width == height);
	assert (width % 2 == 1);
	ncols = width / 2;
	nrows = height / 2;
	nerrs = 3 * nrows * (nrows+1);
	check.resize(nerrs);
	fill(check.begin(), check.end(), 0);

	ndats = 3 * nrows * (nrows+1) + 1;
	data.resize(ndats);

	init_maps();
	for (int i = 0; i < nerrs; i++) {
		paths.push_back(pathToBoundary(i));
	}
}

Twist::~Twist() {}
#include "Base_Lattice.hpp"

int Base_Lattice::rctoi(int row, int col) {
	if (row >= nrows || col >= ncols) {
		cout << "rctoi: Row Column out of bounds" << endl;
		cout << "r: " << nrows << " c: " << ncols << endl;
		abort();
	}
	return row * ncols + col;
};

int Base_Lattice::itor(int i) {
	if (i > nrows * ncols) {
		cout << "itor: Index out of bounds: " << i << endl;
		abort();
	}
	return (i / ncols);
};

int Base_Lattice::itoc(int i) {
	if (i > nrows * ncols) {
		cout << "itoc: Index out of bounds: " << i << endl;
		abort();
	}
	return i % ncols;
};

void Base_Lattice::init(int width, int height) {
	ncols = width;
	nrows = height;
	check.resize(width * height);
	fill(check.begin(), check.end(), 0);
	mtrand.seed(time(0));
	nerrs = width * height;
}

void Base_Lattice::printErrors(ostream& out) {
	vector<int>::iterator x = check.begin();
	for (int r = 0; r < nrows; r++ ) {
		for (int c = 0; c < ncols; c++){
			if (*x)							out << "X ";
			else							out << ". ";
			x++;
		}
		out << endl;
	}
	out << endl;	
}

vector<int> Base_Lattice::getErrors() {
	vector<int> errors;
	for (uint i = 0; i < check.size(); i++) {
		if (check[i])
			errors.push_back(i);
	}
	return errors;
}

vector<int> Base_Lattice::calcErrDistances() {
	vector<int> distances;
	vector<int> errors = getErrors();
	if (errors.size() % 2 == 1)
		errors.push_back(-1);
	for (uint i = 0; i < errors.size(); i++) {
		for (uint j = i+1; j < errors.size(); j++) {
			int dist = calcDist(errors[i], errors[j]);	
			//cout << dist << ": " << i << ", " << j << ":: ";
			distances.push_back(dist);
		}
	}
	cout << endl;
	return distances;
}

vector<int> Base_Lattice::getCheck() {
	return check;
}

qarray Base_Lattice::getData() {
	return data;
}

void Base_Lattice::generateErrors(double errRate) {
	for (qarray::iterator i = data.begin(); i != data.end(); i++ ){
		double randval = (double) mtrand() / mtrand.max();
		if (randval < errRate)
			i->err = true;
		else
			i->err = false;
	}
}

void Base_Lattice::checkErrors() {
	for (uint i = 0; i < check.size(); i++) {
		int err = 0;
		for (vector<mmap>::iterator mm = mX.begin(); mm != mX.end(); mm++) {
			if ((*mm)[i] != -1 && data[(*mm)[i]].err) err++;
		}
		err %= 2;
		if (err) check[i] = 1;
		else	 check[i] = 0; 
	}
}

void Base_Lattice::genCorrPairErrs(double p, double k) {

}

int Base_Lattice::countQubitErrors() {
	int count = 0;
	for (qarray::iterator i = data.begin(); i != data.end(); i++) {
		if (i->err)	count++;
	}
	return count;
}

void Base_Lattice::clear() {
	for (qarray::iterator i = data.begin(); i != data.end(); i++) {
		i->err = false;
	}
	for (vector<int>::iterator i = check.begin(); i != check.end(); i++) {
		*i = 0;
	}
}

/* Applies a matching by connecting all qubits between listed pairs. Qubits in
 * path will be flipped. */
void Base_Lattice::applyCorrection(pairlist matching) {
	for(pairlist::iterator i = matching.begin(); i != matching.end(); i++) {
		int x = i->first;
		int y = i->second;
		// Find the shortest path between the two points, and flip all the bits
		vector<int> path = calcMinPath(x, y); 
		for (uint k = 0; k < path.size(); k++) {
			int index = path[k];
			data[index].err = !data[index].err;
		}		
	}
}


int Base_Lattice::checkCorrection() {
	return 0;
}

Base_Lattice::~Base_Lattice() {}

Base_Lattice::Base_Lattice(int width, int height) {
	init(width, height);
}

int Base_Lattice::calcDist(int i1, int i2) {
	return 0;
}

vector<int> Base_Lattice::calcMinPath(int i1, int i2) {
	vector<int> path;

	return path;
}
void Base_Lattice::printLattice(ostream& out) {

}
void Base_Lattice::init_maps() {

}
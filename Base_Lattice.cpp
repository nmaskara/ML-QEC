#include "Base_Lattice.hpp"

mt19937 RandomlySeededMersenneTwister () {
    // Magic number 624: The number of unsigned ints the MT uses as state
    vector<unsigned int> random_data(624);
    random_device source;
    generate(begin(random_data), end(random_data), [&](){return source();});
    seed_seq seeds(begin(random_data), end(random_data));
    mt19937 seededEngine (seeds);
    return seededEngine;
}

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
	dualcheck.resize(width * height);
	fill(dualcheck.begin(), dualcheck.end(), 0);
	nerrs = width * height;
	mtrand = RandomlySeededMersenneTwister();
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
	vector<int>::iterator z = dualcheck.begin();
	for (int r = 0; r < nrows; r++ ) {
		for (int c = 0; c < ncols; c++){
			if (*z)							out << "Z ";
			else							out << ". ";
			z++;
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

vector<int> Base_Lattice::getDualErrors() {
	vector<int> errors;
	for (uint i = 0; i < dualcheck.size(); i++) {
		if (dualcheck[i])
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
	//cout << endl;
	return distances;
}

vector<int> Base_Lattice::getCheck() {
	return check;
}

vector<int> Base_Lattice::getDualCheck() {
	return dualcheck;
}

qarray Base_Lattice::getData() {
	return data;
}

void Base_Lattice::generateErrors(double errRate) {
	for (qarray::iterator i = data.begin(); i != data.end(); i++ ){
		double randval = (double) mtrand() / mtrand.max();
		if (randval < errRate)
			i->err = !(i->err);
	}
}

void Base_Lattice::generateDepolarizingErrors(double errRate) {
	for (qarray::iterator i = data.begin(); i != data.end(); i++ ){
		double randval = (double) mtrand() / mtrand.max();
		if (randval < errRate / 3)
			i->err = !(i->err);
		else if (randval < 2 * errRate / 3)
			i->derr = !(i->derr);
		else if(randval < errRate){
			i->err = !(i->err);
			i->derr = !(i->derr);
		}	

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
	for (uint i = 0; i < dualcheck.size(); i++) {
		int err = 0;
		for (vector<mmap>::iterator mm = mZ.begin(); mm != mZ.end(); mm++) {
			if ((*mm)[i] != -1 && data[(*mm)[i]].derr) err++;
		}
		err %= 2;
		if (err) dualcheck[i] = 1;
		else	 dualcheck[i] = 0; 
	}
}

void Base_Lattice::genCorrPairErrs(double p, double k) {}

void Base_Lattice::genDepolCorrPairErrs(double p, double k) {}

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
		i->derr = false;
	}
	for (vector<int>::iterator i = check.begin(); i != check.end(); i++) {
		*i = 0;
	}
	for (vector<int>::iterator i = dualcheck.begin(); i != dualcheck.end(); i++) {
		*i = 0;
	}
}

void Base_Lattice::setSeed(int seed) {
	mtrand.seed(seed);
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
			assert(index < ndats);
			data[index].err = !data[index].err;
		}		
	}
}


int Base_Lattice::checkCorrection() {
	return 0;
}
int Base_Lattice::checkDualCorrection() {
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
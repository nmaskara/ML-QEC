#include <iostream>
#include "Lattice.hpp"
#include "Triangle.hpp"
#include "Hexagonal.hpp"
#include "decoder.hpp"
#include <assert.h>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <random>

void writeTestData(string fname, string type, int latsize, int numtrials, double p) {
	int nrows = latsize;
	int ncols = latsize;
	ofstream out(fname);
	Base_Lattice * L;
	Decoder D;
	mt19937 mtrand;
	if (type == "square")
		L = new Lattice(nrows, ncols);
	if (type == "triangle")
		L = new Triangle(nrows, ncols);
	if (type == "Hexagonal")
		L = new Hexagonal(nrows, ncols);
	for (int i = 0; i < numtrials; i++) {
		if (p < 0)	p = (double) mtrand() / mtrand.max();
		L->clear();
		L->genCorrPairErrs(p, 0);
		L->checkErrors();
		vector<int> errors = L->getCheck();
		pairlist matching = D.matchTopLeft(L->getErrors());
		//L.printLattice();
		L->applyCorrection(matching);
		int r2 = L->checkCorrection();
		/*if (L.getErrors().size() == 0) {
			cout << r2 << endl;
			if (r2 != 0){
				L.printLattice(cout);
				abort();
			}
		}*/
		L->checkErrors();
		assert(L->getErrors().size() == 0);
		int result = L->checkCorrection();
		assert(result == r2);
		//L.printLattice();
		///cout << result << endl;
		//cout << L.nerrs << endl;
		for (int i = 0; i < L->nerrs; i++) {
			out << errors[i] << ", ";
		}
		for (int i = 0; i < 4; i++) {
			if (result == i)
				out << "1, ";
			else 
				out << "0, ";
		}
		out << endl;
	}
	out.close();
	delete L;
}


int main(int argc, char** argv) {
	if (argc != 5) {
		cout << "usage: type latsize numtrials error_rate" << endl;
		abort();
	}
	string type = argv[1];
	int latsize = atoi(argv[2]);
	int numtrials = atoi(argv[3]);
	float error_rate = atof(argv[4]);
	string filename = "data/" + type + "_" + to_string(latsize) + "_" + to_string(numtrials) + 
		"_" + to_string(int(error_rate * 1000)) + ".csv";
	writeTestData(filename, type, latsize, numtrials, error_rate);
}
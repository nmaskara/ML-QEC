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

void writeTestData(string fname, int numtrials, double p) {
	int nrows = 4;
	int ncols = 4;
	ofstream out(fname);
	Lattice L(nrows, ncols);
	Decoder D;
	mt19937 mtrand;

	for (int i = 0; i < numtrials; i++) {
		if (p < 0)	p = (double) mtrand() / mtrand.max();
		L.clear();
		L.genCorrPairErrs(p, 0);
		L.checkErrors();
		vector<int> errors = L.getCheck();
		pairlist matching = D.matchTopLeft(L.getErrors());
		//L.printLattice();
		L.applyCorrection(matching);
		int r2 = L.checkCorrection();
		/*if (L.getErrors().size() == 0) {
			cout << r2 << endl;
			if (r2 != 0){
				L.printLattice(cout);
				abort();
			}
		}*/
		L.checkErrors();
		assert(L.getErrors().size() == 0);
		int result = L.checkCorrection();
		assert(result == r2);
		//L.printLattice();
		///cout << result << endl;
		//cout << L.nerrs << endl;
		for (int i = 0; i < L.nerrs; i++) {
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
}


int main(int argc, char** argv) {
	if (argc != 4) {
		cout << "usage: outfile numtrials error_rate" << endl;
		abort();
	}
	writeTestData(argv[1], atoi(argv[2]), atof(argv[3]));
}
#include <iostream>
#include "Lattice.hpp"
#include "Lattice2.hpp"
#include "Triangle.hpp"
#include "Triangle_ColorCode.hpp"
#include "Hexagonal.hpp"
#include "decoder.hpp"
#include <assert.h>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <random>

void writeTestData(string fname, string type, int latsize, int numtrials, double prate, bool randflag) {
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
	if (type == "cc")
		L = new Triangle_ColorCode(nrows, ncols);
	if (type == "surface")
		L = new Lattice2(nrows, ncols);

	clock_t start = clock();

	for (int i = 0; i < numtrials; i++) {
		double p;
		if (prate < 0)	p = (double) mtrand() / mtrand.max();
		else if (randflag) p = (double) prate * mtrand() / mtrand.max();
		else p = prate;
		L->clear();
		L->generateErrors(p);
		L->checkErrors();
		vector<int> errors = L->getCheck();
		pairlist matching;
		if (type != "cc")
			matching = D.matchTopLeft(L->getErrors());
		int result;
		if (type == "surface") {
			result = L->checkCorrection();
		}
		else {
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
			result = L->checkCorrection();			
			assert(result == r2);
			//L.printLattice();
			///cout << result << endl;
			//cout << L.nerrs << endl;
		}

		for (int i = 0; i < L->nerrs; i++) {
			out << errors[i] << ", ";
		}
		int numcat;
		if (type == "cc")
			numcat = 16;
		else if (type == "surface")
			numcat = 2;
		else
			numcat = 4;
		for (int i = 0; i < numcat; i++) {
			if (result == i)
				out << "1, ";
			else 
				out << "0, ";
		}
		out << endl;
		if (i % 100000 == 0) {
			cout << "Generated: " << i << endl;
		}
	}
	out.close();
	delete L;
	cout << "Total Time: " << clock() - start << endl;
}


int main(int argc, char** argv) {
	if (argc < 5) {
		cout << "usage: type latsize numtrials error_rate" << endl;
		abort();
	}
	bool randflag = false;
	string rf = "-r";
	if (argc == 6 && !rf.compare(argv[5])) {
		randflag = true;
	}
	string type = argv[1];
	int latsize = atoi(argv[2]);
	int numtrials = atoi(argv[3]);
	float error_rate = atof(argv[4]);
	string filename = "data/" + type + "_" + to_string(latsize) + "_" + to_string(numtrials) + 
		"_" + to_string(int(error_rate * 1000));
	if (randflag) {
		filename += "_random";
	}
	filename += ".csv";
	writeTestData(filename, type, latsize, numtrials, error_rate, randflag);
}
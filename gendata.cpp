#include <iostream>
#include "Lattice.hpp"
#include "Lattice2.hpp"
#include "Triangle.hpp"
#include "Triangle_ColorCode.hpp"
#include "Triangle_ColorCode2.hpp"
#include "Hexagonal.hpp"
#include "decoder.hpp"
#include <assert.h>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <random>

void writeTestData(string fname, string type, int latsize, int numtrials, double prate, bool randflag=false, bool depol=false, int seed=-1, float p2=0) {
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
	if (type == "cc2")
		L = new Triangle_ColorCode2(nrows, ncols);
	if (type == "surface")
		L = new Lattice2(nrows, ncols);

	/*if (seed != -1) {
		L->setSeed(seed);
		cout << "seed: " << seed << endl;
	}*/

	clock_t start = clock();

	for (int i = 0; i < numtrials; i++) {
		double p;
		if (prate < 0)	p = (double) mtrand() / mtrand.max();
		else if (randflag) p = (double) prate * mtrand() / mtrand.max();
		else p = prate;
		L->clear();
		if (p2 == 0 && !depol)
			L->generateErrors(p);
		else if (p2 == 0 && depol)
			L->generateDepolarizingErrors(p);
		else
			L->genCorrPairErrs(p, p2);
		L->checkErrors();
		vector<int> errors = L->getCheck();
		vector<int> zerrors = L->getDualCheck();
		pairlist matching;
		if (type != "cc" && type != "cc2")
			matching = D.matchTopLeft(L->getErrors());
		int result;
		int dresult;
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
			if (depol)	
				dresult = L->checkDualCorrection();
			assert(result == r2);
			//L.printLattice();
			///cout << result << endl;
			//cout << L.nerrs << endl;
		}

		for (int i = 0; i < L->nerrs; i++) {
			out << errors[i] << ", ";
			if (depol) {
				out << zerrors[i] << ", ";
			}
		}
		int numcat;
		if (type == "cc")
			numcat = 16;
		else if (type == "surface" || type == "cc2")
			numcat = 2;
		else
			numcat = 4;
		for (int i = 0; i < numcat; i++) {
			if (result == i)
				out << "1, ";
			else 
				out << "0, ";
			if (depol) {
				if (dresult == i)
					out << "1, ";
				else
					out << "0, ";
			}
		}
		out << endl;
		if (i % 100000 == 0) {
			cout << "Generated: " << i << endl;
		}
	}
	out.close();
	delete L;
	cout << "Generated: " << numtrials << endl;
	cout << "Total Time: " << clock() - start << endl;
}


int main(int argc, char** argv) {
	if (argc < 5) {
		cout << "usage: type latsize numtrials error_rate" << endl;
		abort();
	}
	int pos = 5;
	int id = -1;
	bool randflag = false;
	bool depolflag = false;
	int seed = -1;
	int pratio = 0;
	while (pos < argc) {

		string rf = "-r";
		string sid = "-i";		
		string ssid = "-s";
		string corr = "-c";
		string depol = "-d";
		if (!rf.compare(argv[pos])) {
			randflag = true;
		}
		else if (!sid.compare(argv[pos])) {
			assert(pos+1 < argc);
			id = atoi(argv[pos+1]);
		}		
		else if (!ssid.compare(argv[pos])) {
			assert(pos+1 < argc);
			seed = atoi(argv[pos+1]);
		}
		else if (!corr.compare(argv[pos])) {
			assert(pos+1 < argc);
			pratio = atoi(argv[pos+1]);
		}
		else if (!depol.compare(argv[pos])) {
			depolflag = true;
		}
		pos++;
	}

	string type = argv[1];
	int latsize = atoi(argv[2]);
	int numtrials = atoi(argv[3]);
	float error_rate = atof(argv[4]);
	float p2 = 0;
	if (pratio != 0)
		p2 = error_rate / pratio;

	string filename = "data/" + type + "_" + to_string(latsize) + "_" + to_string(numtrials) + 
		"_" + to_string(int(error_rate * 1000));
	if (randflag) {
		filename += "_random";
	}
	if (id != -1) {
		filename += "_" + to_string(id);
	}
	if (pratio != 0) {
		filename += "_corr_" + to_string(pratio);
	}
	if (depolflag) {
		filename += "_depol";
	}
	filename += ".csv";
	cout << id << ", " << seed << endl;
	writeTestData(filename, type, latsize, numtrials, error_rate, randflag, seed=seed, p2=p2);
}
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

void writeTestData(string fname, string type, int latsize, int numtrials, double prate, bool randflag) {
	int nrows = latsize+1;
	int ncols = latsize+1;
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
		double p;
		if (prate < 0)	p = (double) mtrand() / mtrand.max();
		else if (randflag) p = (double) prate * mtrand() / mtrand.max();
		else p = prate;
		L->clear();
		L->genCorrPairErrs(p, 0);
		L->checkErrors();
		vector<int> syn = L->getCheck();
		qarray dat = L->getData();
		vector<int> errors;
		for (int r = 0; r < latsize; r++){
			for (int c = 0; c < latsize; c++){
				errors.push_back(syn[L->rctoi(r, c)]);
			}
		}
		bool abv = false;
		bool bel = false;
		bool left = false;
		bool right = false;
		
		for (int c = 0; c < latsize; c++) {
			// testabv
			if (dat[L->rctoi(0, c)].err) 
				abv = !abv;
			// testbel
			if (dat[L->rctoi(latsize, c)].err)
				bel = !bel;
		}

		for (int r = 0; r < latsize; r++) {
			// test right
			if (dat[nrows * ncols + L->rctoi(r, latsize-1)].err)
				right = !right;
			// testleft
			if (dat[nrows * ncols + L->rctoi(r, latsize)].err)
				left = !left;
		}

		L->printLattice();
		cout << abv << "\t" << bel << "\t" << left << "\t" << right << "\t" << endl;
		for (int i = 0; i < latsize * latsize; i++) {
			out << errors[i] << ", ";
			//cout << errors[i] << ", ";
		}
		if (abv)	out << "1, ";
		else		out << "0, ";
		if (bel)	out << "1, ";
		else		out << "0, ";
		if (left)	out << "1, ";
		else		out << "0, ";
		if (right)	out << "1, ";
		else		out << "0, ";
		out << endl;
		if ((i+1) % 100000 == 0) {
			cout << "Generated: " << i << endl;
		}
	}
	out.close();
	delete L;
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
	string filename = "data/open_" + type + "_" + to_string(latsize) + "_" + to_string(numtrials) + 
		"_" + to_string(int(error_rate * 1000));
	if (randflag) {
		filename += "_random";
	}
	filename += ".csv";
	writeTestData(filename, type, latsize, numtrials, error_rate, randflag);
}
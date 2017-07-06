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

using namespace std;


void testnum() {
	int nrows = 6;
	int ncols = 6;
	mt19937 mtrand;
	Hexagonal L(nrows, ncols);
	Decoder D;
	vector<int> nfailed(L.ndats/2);
	vector<int> nhits(L.ndats/2);
	//float counter = 0;
	int cycles = 1;
	auto min = min_element(nhits.begin(), nhits.begin() + nhits.size()/3);
	while(*min < 100) {
		L.clear();
		int min_index = min - nhits.begin();
		double p= (double) min_index / nhits.size();
		/*if (nhits[min_index] == 0) {
			p = (double) min_index / nhits.size();
		}
		else {
			p = (double) nfailed[min_index] / nhits[min_index];			
		}*/

		L.genCorrPairErrs(p, 0);
		L.checkErrors();
		int numerrs = L.countQubitErrors();
		nhits[numerrs/2]++;
		int result = D.runDecode(L, "_edges.txt", "_matched.txt");
		if (!result)	nfailed[numerrs/2]++;
		if (cycles % 100 == 0) {
			for (uint i = 0; i < nhits.size()/3; i++) {
				cout << i << "\t"<< nfailed[i] << "\t" << nhits[i] << endl;
			}
			cout << endl;
		}
		min = min_element(nhits.begin(), nhits.begin() + nhits.size()/3);
		cycles++;
	}
	for (uint i = 0; i < nhits.size(); i++) {
		cout << i << "\t"<< nfailed[i] << "\t" << nhits[i] << endl;
	}
	cout << endl;
	cout << nhits.size() << endl;
}



void simpletest(string dirname = "", string filename = "") {

	int nrows = 8;
	int ncols = 8;
	Triangle L(nrows, ncols);
	if (dirname == "")	dirname = "logs/";
	if (filename == "")	filename = "last";
	do {
	ofstream out(dirname + filename + "_info.txt");
	//L.generateErrors(0.1);
	L.genCorrPairErrs(0,0.02);
	L.checkErrors();
	L.printErrors(out);
	L.printLattice(out);
	L.printErrors(cout);
	L.printLattice(cout);	
	Decoder D;
	vector< pair<int, int> > matching = D.decode(L.getErrors(), L.calcErrDistances(),
		dirname + filename + "_edges.txt", 
		dirname + filename + "_matched.txt");
	L.applyCorrection(matching);
	L.checkErrors();
	L.printLattice(out);
	//L.printLattice(cout);
	L.printErrors(out);
	out.close();
	//if (!save && L.getXerrors().size() == 0) remove(fname.c_str());
	L.printLattice(cout);
	assert((L.getErrors()).size() == 0);
	cout << "Correction: " << L.checkCorrection() << endl;
	} while (L.checkCorrection());
	L.printLattice(cout);
}

int main(int argc, char** argv) {
	//simpletest();
	//testnum();

	/*Hexagonal L(16, 16);
	Decoder D;
	int count = 0;
	int numtrials = 4000;
	for (int i = 1; i <= numtrials; i++) {
		L.clear();
		L.genCorrPairErrs(0,0.2);
		L.checkErrors();
		bool result = D.runDecode(L, "_edges.txt", "_matched.txt");
		if (!result) count++;
		if (i % (numtrials / 100) == 0) {
			cout << count << " " << i << endl;
			cout << (float) count / i << endl;			
		}
 	}*/


	/*Triangle  L(16, 16);
	L.genCorrPairErrs(0, 0.2);
	L.checkErrors();
	L.printLattice();
	cout << L.getErrors().size() << endl;*/

	/*Hexagonal L(6, 6);
	L.data[0].err = true;
	L.data[1].err = true;
	L.checkErrors();
	L.printLattice(cout);
	vector< pair<int, int> > matching;
	pair<int, int> p;
	vector<int> errors = L.getErrors();
	p.first = 2;
	p.second = 12;
	matching.push_back(p);
	L.applyCorrection(matching);
	L.checkErrors();
	L.printLattice(cout);*/


}
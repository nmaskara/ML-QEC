#include <iostream>
#include <cstdlib>
#include <vector>
#include "Lattice.hpp"
#include "Triangle.hpp"
#include "Hexagonal.hpp"
#include "decoder.hpp"
#include <math.h>
#include <string>
#include <sstream>

/*tuple<double, int, int> runtrial(int nrows, int ncols, int p1, int p2, string basename, 
	int maxtrials=1000, int minfails=400, string basename="", string type="square") {
	Base_Lattice *L;
	Decoder D;
	if (type == "square") {
		L = new Lattice(nrows, ncols);
	}
	if (type == "triangle") {
		L = new Triangle(nrows, ncols);
	}
	if (type == "hexagonal") {
		L = new Hexagonal(nrows, ncols);
	}
	uint nfailed = 0;
	uint count = 0;
	int numerrs = 0;
	while (nfailed < (uint) minfails && count < (uint) maxtrials) {
		L->clear();
		L->genCorrPairErrs(p1, p2);
		L->checkErrors();
		numerrs += L->countQubitErrors();
		bool result = D.runDecode(&L, basename + "_edges.txt", basename + "_matched.txt");
		if (!result)	nfailed++;
		count++;
	}
	double pPhys = (double) numerrs / (count * L->ndats);
	tuple<double, int, int> toret(pPhys, nfailed, count);
	return toret;
}*/

void runtest(string finame, string foname) {
	ifstream infile(finame);
	ofstream outfile(foname);
	string line = "";
	string flags = "";
	// by convention first line of infile should be running parameters
	getline(infile, flags);
	stringstream flagstream(flags);
	string latticetype, decodertype;
	int maxfails, maxtrials; 
	float p1coeff, p2coeff;
	flagstream >> latticetype;
	flagstream >> decodertype;
	flagstream >> maxfails;
	flagstream >> maxtrials;
	flagstream >> p1coeff;
	flagstream >> p2coeff;

	//stringstream thisline;
	int latsize;
	float p;
	int nfailed, count;
	while (!infile.eof()) {
		getline(infile, line);
		stringstream stream(line);
		stream >> latsize;
		Base_Lattice * L;
		if (latticetype == "square")	
			L = new Lattice(latsize, latsize);
		if (latticetype == "triangle")	
			L = new Triangle(latsize,latsize);
		if (latticetype == "hexagonal") 
			L = new Hexagonal(latsize, latsize);
		Decoder D(decodertype);
		while(stream >> p) {
			nfailed = 0;
			count = 0;
			long numerrs = 0;
			long starttime = time(0);
			while (nfailed < maxfails && count < maxtrials ) {
				L->clear();
				//L.generateErrors(p)
				L->genCorrPairErrs(p1coeff * p, p2coeff * p);
				L->checkErrors();
				//L->printLattice(cout);
				numerrs += L->countQubitErrors();
				int result = D.runDecode(*L, "_edges.txt", "_matched.txt");
				if (result){
					nfailed++;
					//L->printLattice(cout);
					//cout << result << endl;
					//cout << ".";
				}
				count++;
				//if (true || count % (max / 10) == 0) cout << nfailed << " " << count << endl;
			}
			//cout << endl;
			double pin = (double) numerrs / (count * L->ndats);
			double peff = (double) nfailed / count;
			double unc  = sqrt(nfailed) / count;
			double avtime = (double) (time(0) - starttime) / count;
			outfile << latsize << " " << count << " " <<  avtime << " " << p << " " << pin << " " << peff << " " << unc << endl;
			cout << latsize << " " << count << " " << avtime << " " << p << " " << pin << " " << peff << " " << unc << endl;
		}
		delete L;
	}
	infile.close();
	outfile.close();
}

int main(int argc, char ** argv) {
	if (argc != 3) {
		cout << "USAGE: infile outfile" << endl;
		return 1;
	}
	string infile(argv[1]);
	string outfile(argv[2]);
	runtest(infile, outfile);
}
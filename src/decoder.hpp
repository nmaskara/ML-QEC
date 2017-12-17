#ifndef DECODER_H
#define DECODER_H
#include "Lattice.hpp"
#include <algorithm>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sys/wait.h>
#include <sstream>

class Decoder {

public:
	string type;
	void match(string infile, string outfile);

	void writeErrors(string filename, vector<int> errors, vector<int> distances);

	vector< pair<int, int> > readMatching(string infile, vector<int> errors);

	vector<int> genCorrection(vector<int> errors, vector<int, int> matching);

	vector< pair<int, int> > decode(vector<int> errors, vector<int> dists, string dfile, 
		string mfile);

	pairlist matchTopLeft(vector<int> errors);

	int runDecode(Base_Lattice &L, string dfile, string mfile);
	Decoder(string type="default");
};

#endif
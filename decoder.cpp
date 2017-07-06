#include "decoder.hpp"

using namespace std;

/*!
 * Writes the errors and their distances to a file in a format useable by
 * Blossom5. Output file is filename.
 */
void Decoder::writeErrors(string filename, vector<int> errors, 
	vector<int> distances) {

	ofstream out (filename.c_str());
	int count = 0;
	out << errors.size() << " " << distances.size() << endl;
	for (uint i = 0; i < errors.size(); i++) {
		for (uint j = i+1; j < errors.size(); j++) {
			out << i << " " << j << " " << distances[count++] << endl;
		}
	}
	out.close();
}

/*!
 * Runs blossom5 on distances saved at infile. Stores the output
 * in outfile.
 */
void Decoder::match(string infile, string outfile)
{
	string b5 = "./blossom5/blossom5";
	string cmd = b5 + " -e " + infile + " -w " + outfile + " >nul";
	system(cmd.c_str());
}

/*!
 * Reads blossom5 matching output and returns a vector holding matched
 * pairs of indexes. Indexes are taken from the errors vector, and represent
 * indexes relative to the entire lattice.
 */
vector< pair<int, int> > Decoder::readMatching(string errfile, vector<int> errors) {
	ifstream infile(errfile.c_str());
	int a, b;
	vector< pair<int, int> > matching;
	infile >> a >> b; //throw out first line
	while (infile >> a >> b) {
		matching.push_back(make_pair(errors[a], errors[b]));
	}
	infile.close();
	return matching;
}

vector< pair<int, int> > Decoder::decode(vector<int> errors, vector<int> dists, string dfile, 
	string mfile) {
	vector< pair<int, int> > matching;
	if (errors.size() == 0)
		return matching;
	writeErrors(dfile, errors, dists);
	// Create output file
	ofstream out (mfile.c_str());
	out.close();
	match(dfile, mfile);
	matching = readMatching(mfile, errors);
	return matching;
}

void fandr(vector<int> &list, int tofind, int torep) {
	for (uint i = 0; i < list.size(); i++) {
		if (list[i] == tofind)
			list[i] = torep;
	}
}

void oddandr(vector<int> &list, int torep) {
	for (uint i = 0; i < list.size(); i++) {
		if (list[i] % 2 == 1)
			list[i] = torep;
	}
}

void oddplusplus(vector<int> &list) {
	for (uint i = 0; i < list.size(); i++) {
		if (list[i] %2 == 1)
			list[i]++;
	}
}

pairlist Decoder::matchTopLeft(vector<int> errors) {
	pairlist matching;
	for (uint i = 0; i < errors.size(); i++) {
		matching.push_back(make_pair(0, errors[i]));
	}
	return matching;
}
// Returns 
// 0 means no error
// 1 means logical flip vertical
// 2 means logical flip horizontal
// 3 means double logical flip 
int Decoder::runDecode(Base_Lattice &L, string dfile, string mfile) {
	L.checkErrors();
	vector<int> errs = L.getErrors();
	vector<int> dists = L.calcErrDistances();
	if (dists.size() == 0)
		return L.checkCorrection();
	//cout << type << endl;
	if (type == "1->3"){
		fandr(dists, 1, 3);
		//cout << *min_element(dists.begin(), dists.end()) << endl;
	}
	if (type == "odd->L") {
		oddandr(dists, min(L.nrows, L.ncols));
	}
	if (type == "odd=even") {
		oddplusplus(dists);
	}
	pairlist matching = decode(errs, dists, dfile, mfile);
	L.applyCorrection(matching);
	L.checkErrors();
	return L.checkCorrection();
}

Decoder::Decoder(string t) {
	type = t;
}
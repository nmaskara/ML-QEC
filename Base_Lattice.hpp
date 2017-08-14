#ifndef BASE_LATTICE_H
#define BASE_LATTICE_H

#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <csignal>
#include <random>
#include <assert.h>
using namespace std;

struct qubit {
	bool err;
};

typedef vector<int> qmap;	// Maps a list of qubits to a set of measurement bits
typedef vector<int> mmap;	// Maps a list of measurement bits to a list of qubits
typedef vector<qubit> qarray;	//List of all qubits.
typedef vector< pair<int, int> > pairlist;

class Base_Lattice {

public:
	int nrows, ncols, ndats, nerrs;
	vector<qmap> X;
	vector<mmap> mX;

	qarray data;
	vector<int> check;

	mt19937 mtrand;
	// Helper functions
	int rctoi(int row, int col);

	int itor(int i);

	int itoc(int i);

	// Returns an array of locations of errors in check matrix
	vector<int> getErrors();
	// Returns an arry of distances between each pair of errors
	// in th order (0,1), (0,2)...(1,0), (1,1)...
	// where the numbers are the indexes of the errors in the
	// array returned by getErrors
	vector<int> calcErrDistances();
	// Returns the array of data qubits
	qarray getData();
	// Returns the check array
	vector<int> getCheck();
	// prints the error syndrome to the ostream specified
	void printErrors(ostream& = cout);
	// gnerate random errors with rate errRate
	void generateErrors(double errRate);
	//recalculate check matrix based on data array
	void checkErrors();
	// applies a matching to the lattice by connecting all qubits 
	// between each pair
	virtual void applyCorrection(pairlist matching);

	void clear();

	int countQubitErrors();

	virtual void genCorrPairErrs(double p, double k);
	
	// Determine if the correction produced a logical error (false)
	virtual int checkCorrection();
	// Calculate the distance between two measurement qubits
	virtual int calcDist(int i1, int i2);
	// Calculate the data qubits connecting two meausrememnt qubits
	// Chooses one of the available shortest paths
	virtual vector<int> calcMinPath(int i1, int i2);
	// Initialize X and mX to define the connections between 
	// data and measurement qubits
	virtual void init_maps();
	// prints a visual representation of the entire lattice
	virtual void printLattice(ostream& = cout);
	virtual ~Base_Lattice();

	Base_Lattice(int width, int height);
	void init(int width, int height);
	Base_Lattice();
};



#endif
#ifndef LATTICE_H
#define LATTICE_H

#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <csignal>
#include <random>
#include "Base_Lattice.hpp"

using namespace std;

// Basic fully periodic square lattice
class Lattice : public Base_Lattice {
public:
	virtual void genCorrPairErrs(double p, double k);
	virtual int checkCorrection();
	virtual int calcDist(int i1, int i2);
	virtual vector<int> calcMinPath(int i1, int i2);
	virtual void init_maps();
	virtual void printLattice(ostream& = cout);

	Lattice(int width, int height);
	virtual ~Lattice();
};


#endif
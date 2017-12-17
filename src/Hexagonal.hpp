#ifndef HEXAGONAL_H
#define HEXAGONAL_H

#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <csignal>
#include <random>
#include <assert.h>
#include "Base_Lattice.hpp"

using namespace std;

// fully periodic hexagonal lattice
class Hexagonal : public Base_Lattice {
public:
	virtual void genCorrPairErrs(double p1, double p2);
	virtual int checkCorrection();
	virtual int calcDist(int i1, int i2);
	virtual vector<int> calcMinPath(int i1, int i2);
	virtual void init_maps();
	virtual void printLattice(ostream& = cout);

	Hexagonal(int width, int height);
	virtual ~Hexagonal();
};

#endif

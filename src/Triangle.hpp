#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <csignal>
#include <random>
#include <assert.h>
#include "Base_Lattice.hpp"

using namespace std;

// fully periodic triangular lattice
class Triangle : public Base_Lattice {
public:
	virtual void genCorrPairErrs(double p1, double p2);
	virtual int checkCorrection();
	virtual int calcDist(int i1, int i2);
	virtual vector<int> calcMinPath(int i1, int i2);
	virtual void init_maps();
	virtual void printLattice(ostream& = cout);

	Triangle(int width, int height);
	virtual ~Triangle();
};

#endif
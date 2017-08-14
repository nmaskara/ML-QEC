#ifndef TRIANGLECC_H
#define TRIANGLECC_H

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
class Triangle_ColorCode : public Base_Lattice {
public:
	vector<int> pathTopLeft(int i);

	virtual void applyCorrection(pairlist matching);

	virtual void genCorrPairErrs(double p1, double p2);
	virtual int checkCorrection();
	virtual int calcDist(int i1, int i2);
	virtual vector<int> calcMinPath(int i1, int i2);
	virtual void init_maps();
	virtual void printLattice(ostream& = cout);

	Triangle_ColorCode(int width, int height);
	virtual ~Triangle_ColorCode();
};

#endif
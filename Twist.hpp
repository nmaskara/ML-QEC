#ifndef TWIST_H
#define TWIST_H

#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <csignal>
#include <random>
#include <assert.h>
#include <cmath>
#include "Base_Lattice.hpp"

using namespace std;


// fully periodic triangular lattice
class Twist : public Base_Lattice {
public:
	vector<int> pathToBoundary(int i);
	int stabtoi(int x, int y, int z);
	vector<int> itostab(int i);

	int qubtoi(int x, int y, int z);
	vector<int> itoqub(int i);

	vector<vector<int>> paths;
	virtual void checkErrors();

	virtual void applyCorrection(pairlist matching);
	virtual void genCorrPairErrs(double p1, double p2);
	virtual int checkCorrection();
	virtual int checkDualCorrection();
	virtual int calcDist(int i1, int i2);
	virtual vector<int> calcMinPath(int i1, int i2);
	virtual void init_maps();
	virtual void printLattice(ostream& = cout);

	Twist(int width, int height);
	virtual ~Twist();
};

#endif
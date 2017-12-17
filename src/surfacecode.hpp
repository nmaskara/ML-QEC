#include <iostream>
#include <cstdlib>

struct qubit {
	bool xerr;
	bool zerr;
};

class Lattice {
	int width, height;
	qubit data[];
	qubit Zcheck[];
	qubit Xcheck[];
	public:
		Lattice (int, int);

}
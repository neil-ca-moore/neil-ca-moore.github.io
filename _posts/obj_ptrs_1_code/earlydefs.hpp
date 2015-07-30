#include <iostream>

class StupidAdder {
public:
	virtual unsigned add(unsigned a, unsigned b) {
		std::cout << "slow" << std::endl;
		unsigned retval = 0;
		while(a--) { retval++; }
		while(b--) { retval++; }
		return retval;
	}
};

class SmartAdder : public StupidAdder {
public:
	virtual unsigned add(unsigned a, unsigned b) {
		std::cout << "fast" << std::endl;
		return a + b;
	}
};

#include <iostream>

class Incrementer {
public:
	unsigned increment(unsigned a) {
		return a + 1;
	} 
};

class DumbAdder {
public:	
	unsigned add(unsigned a, unsigned b) {
		unsigned retval = 0;
		while(a--) {
			retval = i.increment(retval); //(J)
		}
		while(b--) {
			retval = i.increment(retval);
		}
		return retval;
	};

private:
	Incrementer i;
};

int main() {
	DumbAdder dumbAdder;
	std::cout << dumbAdder.add(6, 7) << std::endl;
}

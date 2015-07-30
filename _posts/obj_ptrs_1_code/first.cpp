#include "earlydefs.hpp"

int main() {
	StupidAdder stupid;
	SmartAdder smart;

	stupid.add(4, 5); //(A): prints "slow"

	smart.add(4, 5); //(B): prints "fast"
}

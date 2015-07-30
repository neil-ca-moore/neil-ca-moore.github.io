#include "earlydefs.hpp"

int main() {
	StupidAdder stupid;
	SmartAdder smart;

	StupidAdder& reallyStupid = smart;
	reallyStupid.add(6, 7); //(D): prints "fast"

	StupidAdder* reallyReallyStupid = &smart;
	reallyReallyStupid->add(6, 7); //(E): prints "fast"
}

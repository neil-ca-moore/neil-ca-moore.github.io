#include "earlydefs.hpp"

int main() {
	StupidAdder stupid;
	SmartAdder smart;

	StupidAdder reallyStupid = smart;
	reallyStupid.add(6, 7); //(C): prints "slow"
}

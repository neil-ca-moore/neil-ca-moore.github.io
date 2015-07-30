#include "earlydefs.hpp"
#include <memory>

int main() {
	std::shared_ptr<StupidAdder> notStupid(std::make_shared<SmartAdder>());
	notStupid->add(6, 7);
}
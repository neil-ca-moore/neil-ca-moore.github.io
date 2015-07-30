#include <iostream>

template<class T>
class BaseAdder {
public:	
	unsigned add(unsigned a, unsigned b) {
		unsigned retval = 0;
		while(a--) { retval = increment(retval); } //(F)
		while(b--) { retval = increment(retval); }
		return retval;
	}

	unsigned increment(unsigned a) { 
		return derived().increment(a); //(G)
	} 

private:
	T& derived() {
		return *static_cast<T*>(this);
    	}
};

class DerivedAdder : public BaseAdder<DerivedAdder> {
public:
	unsigned increment(unsigned a) {
		return a + 1;
	} 
};

int main() {
	DerivedAdder staticDispatchAdder;
	std::cout << staticDispatchAdder.add(6, 7) << std::endl; //(H)
}

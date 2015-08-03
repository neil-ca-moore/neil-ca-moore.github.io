#include <boost/mpl/vector.hpp>
#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/member.hpp>
#include <iostream>
#include <vector>

namespace mpl = boost::mpl;
using namespace boost::type_erasure;

class ParentPrinter {
public:
	int a;
	virtual void Print() { std::cout << "ParentPrinter(" << a << ")" << std::endl; }		
};

class ChildPrinter : public ParentPrinter {
public:
	 virtual void Print() { std::cout << "ChildPrinter(" << a << ")" << std::endl; }		
};

class AbstractParent {
public:
	int a;
	virtual void Print() = 0;
};

class ConcreteChild : public AbstractParent {
public:
	 virtual void Print() { std::cout << "ConcreteChild(" << a << ")" << std::endl; }
};

BOOST_TYPE_ERASURE_MEMBER((has_print), Print, 0)

typedef any<mpl::vector<copy_constructible<>, has_print<void()>, relaxed>> AnyPrinter;

int main()
{
	ParentPrinter pp;
	pp.a = 0;
	
	ChildPrinter cp;
	cp.a = 1;

	std::vector<AnyPrinter> printables;
	printables.push_back(pp); //pushing copies
	printables.push_back(cp);

	pp.a = 3;
	cp.a = 4;

	ConcreteChild cc;
	AbstractParent& concreteChildRef = cc;
	//printables.push_back(concreteChildRef);
	concreteChildRef.a = 2;

	for(auto& p : printables) {
		p.Print();
	}
}

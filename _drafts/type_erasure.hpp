#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/member.hpp>
#include <iostream>
#include <vector>

using namespace boost::type_erasure;

class ConcretePrinter {
public:
	 void Print() const { std::cout << "ConcretePrinter" << std::endl; }		
};

class StonePrinter {
public:
	 void Print() const { std::cout << "StonePrinter" << std::endl; }		
};

BOOST_TYPE_ERASURE_MEMBER((has_print), Print, 0)

typedef any<has_print<void()>, _self&> AnyPrinter;

int main()
{
	ConcretePrinter cp;
	
	StonePrinter sp;

	std::vector<AnyPrinter> printables;
	printables.push_back(cp); //pushing copies
	printables.push_back(sp);

	for(const auto& p : printables) {
		p.Print();
	}
}

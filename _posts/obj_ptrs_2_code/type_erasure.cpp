#include <boost/mpl/vector.hpp>
#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/member.hpp>
#include <iostream>
#include <vector>

namespace mpl = boost::mpl;
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

typedef any<mpl::vector<
		copy_constructible<>, 
		has_print<void()>>> 
	AnyPrinter;

int main()
{
	ConcretePrinter cp;
	
	StonePrinter sp;

	std::vector<AnyPrinter> printables;
	printables.push_back(cp); //pushing copies
	printables.push_back(sp);

	for(auto& p : printables) {
		p.Print();
	}

	AnyPrinter copy = cp;
}

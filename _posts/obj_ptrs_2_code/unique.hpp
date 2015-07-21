#include <iostream>
#include <memory>

class Printer {
public:
	virtual void Print() const { std::cout << "Printer" << std::endl; }		
};

class ConcretePrinter {
public:
	virtual void Print() const { std::cout << "ConcretePrinter" << std::endl; }		
};

int main()
{
	std::unique_ptr<ConcretePrinter> cp(new ConcretePrinter());
	cp->Print(); //prints: ConcretePrinter
	
	//std::unique_ptr<ConcretePrinter> notACopy = cp; //doesn't work
	
	std::unique_ptr<ConcretePrinter> notACopy2 = std::move(cp);
	notACopy2->Print(); //prints: ConcretePrinter
	
	cp->Print(); //dangerous, cp isn't there any more
}

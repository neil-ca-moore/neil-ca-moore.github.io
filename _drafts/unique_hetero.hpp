
#include <iostream>
#include <memory>
#include <vector>

class Printer {
public:
	virtual void Print() const { std::cout << "Printer" << std::endl; }		
};

class ConcretePrinter {
public:
	virtual void Print() const { std::cout << "ConcretePrinter" << std::endl; }		
};

class StonePrinter {
public:
	virtual void Print() const { std::cout << "StonePrinter" << std::endl; }		
};

int main()
{
	std::unique_ptr<ConcretePrinter> cp(new ConcretePrinter());
	
	std::unique_ptr<StonePrinter> sp(new StonePrinter());

	std::vector<std::unique_ptr<Printer>> doesntWork;
	//doesntWork.push_back(cp); //won't compile
	//doesntWork.push_back(sp); //won't compile
}
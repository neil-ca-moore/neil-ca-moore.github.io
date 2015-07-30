#include <iostream>

int main()
{
	int a = 1;
	int b = a; //value semantics...
	b += 1;    //a is unchanged
	
	std::cout << "a=" << a << ",b=" << b << std::endl;
	//prints: a=1,b=2

	int& ptrA = a;
	int& ptrB = ptrA; //reference semantics
	ptrB += 1;        //ptrA is changed
	
	std::cout << "a=" << a << ",ptrA=" << ptrA << ",ptrB=" << ptrB << std::endl;
	//prints: a=2,ptrA=2,ptrB=2
}
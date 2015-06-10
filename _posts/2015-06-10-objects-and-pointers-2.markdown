---
layout: post
title:  "Objects and pointers part II"
date:   2015-6-10 21:29:19
categories: c++
---

<!--
Outline:

- static polymorphism
 - set the scene
  - value semantics
  - polymorphism
  - not polymorphism
 - unique_ptr
  - cannot go into homogeneous container
  - assignment doesn't work as expected
 - hand rolled
 - boost any
-->

I described in a [previous post]({% post_url 2014-11-30-objects-and-pointers %}) how difficult it is to work with dynamic dispatch in c++. I described various ways to deal with it. However I've been reading about an even more powerful and easy to use way to do things.

Lets say that I want the following properties of a variable for an object:

* dynamic dispatch of its methods
* memory safety

We can achieve combinations of those properties using the techniques I described last time: i.e. use of safe pointers (dynamic dispatch + memory safety), CRTP (memory safety and inheritance but dispatch done statically) or inclusion (memory safety but static dispatch and no inheritance).

However my eyes have been opened a bit after watching Sean Parent's talk called ["C++ seasoning"](http://channel9.msdn.com/Events/GoingNative/2013/Cpp-Seasoning). Now I also want to achieve /value semantics/. Value semantics means that assigning a value to a different variable copies it. The opposite is reference semantics meaning that only the pointer is copied. With value semantics a value is guaranteed to be the sole way to access the corresponding area of memory.

{% highlight c++ linenos %}
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
{% endhighlight %}

Using value semantics helps to make code easy to understand, since you don't need to check what other variables (possibly on other threads) might be sharing the same data.

I said in my earlier post that in C++ polymorphism is only available via a pointer/reference variable. That is true, but there are a variety of techniques to maintain value semantics without losing polymorphism, and the purpose of this post is to describe those.

unique_ptr
===========
The simplest way to get close to value semantics with pointers is definitely std::unique_ptr. An area of memory is guaranteed to be uniquely owned. 

{% highlight c++ linenos %}
#include <iostream>
#include <memory>

class Printer {
public:
	virtual void Print() { std::cout << "Printer" << std::endl; }		
};

class ConcretePrinter {
public:
	virtual void Print() { std::cout << "ConcretePrinter" << std::endl; }		
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
{% endhighlight %}

However we haven't really achieved proper value semantics, because assigning a value doesn't work and we have to move it around explicitly.

The other problem with this code is that you cannot use homogeneous containers properly to process unique_ptrs:

{% highlight c++ linenos %}
#include <iostream>
#include <memory>
#include <vector>

class Printer {
...	
};

class ConcretePrinter : public Printer {
...	
};

class StonePrinter : public Printer {
public:
	virtual void Print() { std::cout << "StonePrinter" << std::endl; }		
};

int main()
{
	std::unique_ptr<ConcretePrinter> cp(new ConcretePrinter());
	
	std::unique_ptr<StonePrinter> sp(new StonePrinter());

	std::vector<std::unique_ptr<Printer>> doesntWork;
	//doesntWork.push_back(cp); //won't compile
	//doesntWork.push_back(sp); //won't compile
}
{% endhighlight %}

That's two fairly big gotchas, let's look for a better solution.

Hand rolled value semantics with polymorphism
=============================================

boost type_erasure
==================



References
==========
[Boost.TypeErasure](http://www.boost.org/doc/libs/1_58_0/doc/html/boost_typeerasure.html)

[C++ Seasoning](http://channel9.msdn.com/Events/GoingNative/2013/Cpp-Seasoning)

C++ in a nutshell - O'Reilly

[Inheritance is the Base Class of Evil code sample](https://gist.github.com/berkus/7041546)

[Intrusive value semantic polymorphism example.](https://gist.github.com/sean-parent/6798797)

[Value semantics](https://akrzemi1.wordpress.com/2012/02/03/value-semantics/)

[What is value semantics?](https://isocpp.org/wiki/faq/value-vs-ref-semantics#val-vs-ref-semantics)

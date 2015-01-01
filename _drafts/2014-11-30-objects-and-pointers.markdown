---
layout: post
title:  "Objects and pointers"
date:   2015-1-1 19:54:19
categories: c++
---

<!--
Outline:

- C++ polymorphism
- slicing
- use of pointers for polymorphism (forcing use of pointers for polymorphism, leading to pointer hell)
- CRTP as a workaround for getting polymorphism in a non-pointer
- other workarounds
- not using polymorphism
- different pointer types available
- I have never seen slicing be deliberate
- comparison with go
-->

A comparison of pointers and objects in C++ and go.

Polymorphism and pointers in C++
================================

A variable for an object can be used for most things. You can reassign it, call non-virtual functions, access member variables, etc. However there are a few gotchas in this area.

Suppose you have a polymorphic function in your object:

{% highlight c++ linenos %}
class StupidAdder {
public:
	virtual unsigned add(unsigned a, unsigned b) {
		unsigned retval = 0;
		while(a--) {
			retval++;
		}
		while(b--) {
			retval++;
		}
		return retval;
	}
};

class SmartAdder : public Base {
	virtual unsigned add(unsigned a, unsigned b) {
		return a + b;
	}
};

void objectStuff() {
	StupidAdder stupid;
	SmartAdder smart;

	stupid.add(4, 5); //(1)

	smart.add(4, 5); //(2)
}
{% endhighlight %}

This program is mostly understandable: line (1) uses StupidAdder's version of add() and line (2) uses SmartAdder's version.

However only a little change makes things a little confusing:

{% highlight c++ linenos %}
void objectStuff2() {
	StupidAdder stupid;
	SmartAdder smart;

	StupidAdder reallyStupid = smart;
	reallyStupid.add(6, 7); //(3)
}
{% endhighlight %}

Line (3) will use StupidAdder's version of add(), rather than SmartAdder's version as the same Java program would do.

Why is that? It's because dynamic dispatch of functions can only be achieved via pointers or references to objects. A non-pointer, non-reference object variable will dispatch functions based on the /static type/ of an object, i.e. the type of the variable. This avoids the need for a virtual function call in most cases. I think this is part of the C++ design principle that there should be no performance penalty for language features that are not being used. In this case it is polymorphism that is not being used. But it's beginning to make things a little confusing for programmers.

There are no variables in StupidAdder or SmartAdder, but if there were then slicing would also have happened at line (3). That would have meant that any member variables added in SmartAdder would be lost in the assignment. Also the true dynamic type of smart has been lost, there is no runtime information to say that reallyStupid was ever a SmartAdder at all.

I don't think I have ever seen a deliberate use of slicing in a C++ program.

If you want dynamic dispatch of virtual functions you have to use a pointer or a reference:

{% highlight c++ linenos %}
void objectStuff3() {
	StupidAdder stupid;
	SmartAdder smart;

	StupidAdder& reallyStupid = smart;
	reallyStupid.add(6, 7); //(4)

	StupidAdder* reallyReallyStupid = &smart;
	reallyReallyStupid.add(6, 7); //(5)
}
{% endhighlight %}

Lines (4) and (5) use SmartAdder's version of add(). This is very unfortunate as it encourages use of raw pointers and references which are both a source of many programming errors. I think this is quite an important point. Pointers are one of the most difficult features to use correctly in C++ and polymorphism one of the most useful features (used judiciously). In C++ they come as a package. A radioactive dogshit.

Safe technique 1: safe use of pointers
======================================

This topic has been covered so many times before and I don't want to go into too much detail. Basically by using safe pointer types like shared_ptr, scoped_ptr, unique_ptr, etc. you can get safe (and sometimes efficient) access to polymorphic behaviours without passing around raw pointers and references. I've certainly done a fair amount of this myself. It is a good solution, but what always rankles with me is that the syntax is verbose and inflexible, and you have to pick which pointer types to use and you cannot easily change your mind.

{% highlight c++ linenos %}
void objectStuff4() {
	StupidAdder stupid;
	SmartAdder smart;

	boost::scoped_ptr<StupidAdder> notStupid = smart;
	notStupid.add(6, 7); //(6)
}
{% endhighlight %}

You could use typedefs, wrapping, auto keyword and various other techniques to get short and easily changed pointer types, but it doesn't quite reach the convenience of a build in language feature.

Safe technique 2: Curiously recurring template pattern (CRTP)
=============================================================

This one is not a silver bullet. It's kind of hard and laborious to use, but you can achieve some subtype polymorphism without using dynamic dispatch (meaning a pointer is not needed). Here's some code:

{% highlight c++ linenos %}
template<class T>
class BaseAdder {
public:	
	unsigned add(unsigned a, unsigned b) {
		unsigned retval = 0;
		while(a--) {
			retval = increment(retval);
		}
		while(b--) {
			retval = increment(retval);
		}
		return retval;
	};

	unsigned increment(unsigned a) {
		return derived()->increment(a);
	} 

private:
	T& derived()
    {
        return *static_cast<T*>(this);
    }
};

class DerivedAdder : public BaseAdder<DerivedAdder> {
public:
	unsigned increment(unsigned a) {
		return a + 1;
	} 
};

void objectStuff5() {
	DerivedAdder staticDispatchAdder;
	staticDispatchAdder.add(6, 7); //(6)
	staticDispatchAdder.increment(5); //(7)
}
{% endhighlight %}

As you can see, it's a bit less convenient to use than dynamic polymorphism, though that could probably be mitigated by code generation. What this does is gets you some kind of inheritance, provided that the type of the object is known at compile time. In this example the add() function from BaseAdder is inherited by DerivedAdder, but won't use dynamic dispatch since it's non-virtual. However, the increment() function behaves like a virtual function. In line (6) the add function uses DerivedAdder's increment() function, which is roughly want dynamic dispatch of virtual functions normally achieves, but in this case there is no dynamic dispatch.

This is not really a technique for every day use.

Safe technique 3: don't use code inheritance at all
===================================================

This is a great tip that is quite an eye opener when you get used to it. The idea is simply not to use direct code inheritance at all, to avoid having to deal with pointers at all. That way you don't need to worry about freeing the pointers or being so careful with ownership. Use of inheritance is frequently held up as a quality goal in object oriented languages when it is often not necessary to use it at all. It is perfectly possible to design an object oriented language that does not have code inheritance, and instead relies on interfaces and inclusion of other classes to share functionality.

Here's a C++ example that doesn't use inheritance at all:

{% highlight c++ linenos %}
class Incrementer {
public:
	unsigned increment(unsigned a) {
		return a + 1;
	} 
};

class DumbAdder {
public:	
	unsigned add(unsigned a, unsigned b) {
		unsigned retval = 0;
		while(a--) {
			retval = i.increment(retval);
		}
		while(b--) {
			retval = i.increment(retval);
		}
		return retval;
	};

private:
	Incrementer i;
};

void objectStuff5() {
	DumbAdder dumbAdder;
	staticDispatchAdder.add(6, 7); //(8)
}
{% endhighlight %}

At line (8), the instance of incrementer embedded inside DumbAdder is used to provide increment functionality. In common with CRTP, this doesn't use subtyping, but then again, it's not really needed.

The frustration with this technique is not to be able to create a GoodAdder class, and let them both be instances of an Adder interface. For that you do need dynamic dispatch, and that means pointers.

Comparison with another language: go
====================================

Although I haven't used it for a big project yet, I like the look of go. It is a much simpler language than go, and lets you do dynamic dispatch very naturally.

<!--
Example involving:
- interfaces
- inclusion
- pointers and non-pointers using dynamic dispatch
-->

TODO: compile the examples
==========================

References
==========
[The Curiously Recurring Template Pattern in C++](http://eli.thegreenplace.net/2011/05/17/the-curiously-recurring-template-pattern-in-c)







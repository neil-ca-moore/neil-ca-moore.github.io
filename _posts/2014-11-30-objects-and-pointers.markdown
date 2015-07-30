---
layout: post
title:  "Objects and pointers"
date:   2014-11-30 21:29:19
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

A comparison of pointers and objects in C++ and go. More detail on the C++, go mainly for comparison.

Polymorphism and pointers in C++
================================

A variable for an object can be used for most things. You can reassign it, call non-virtual functions, access member variables, etc. However there are a few gotchas in this area.

Suppose you have a virtual function in your object:

{% highlight c++ linenos %}
{% include_relative obj_ptrs_1_code/earlydefs.hpp %}
{% endhighlight %}

{% highlight c++ linenos %}
{% include_relative obj_ptrs_1_code/first.cpp %}
{% endhighlight %}

This program should be understandable under any possible interpretation: line (A) uses StupidAdder's version of add() and line (B) uses SmartAdder's version.

However only a little change using subtyping makes things a little confusing:

{% highlight c++ linenos %}
{% include_relative obj_ptrs_1_code/second.cpp %}
{% endhighlight %}

Line (C) will use StupidAdder's version of add(), rather than SmartAdder's version as the identical Java program would do.

Why is that? It's because dynamic dispatch of functions can only be achieved via pointers or references to objects. A non-pointer, non-reference object variable will dispatch functions based on the *static type* of an object, i.e. the type of the variable (there is also a *dynamic type* that is the type of the object, sometimes different.) 

Because there is no dynamic dispatch there is no need for runtime function lookup. I think this is down to the C++ design principle that there should be no performance penalty for language features that are not being used. In this case it is polymorphism that is not being used. But it's beginning to make things a little confusing for programmers.

There are no instance variables in StupidAdder or SmartAdder, but if there were then slicing would also have happened at line (C). That would have meant that any member variables added in SmartAdder would be lost in the assignment. Also the true dynamic type of smart has been lost, there is no runtime information to say that reallyStupid was ever a SmartAdder at all.

I don't think I have ever seen a deliberate use of slicing in a C++ program.

Getting back to the main point, if you want dynamic dispatch of virtual functions you have to use a pointer or a reference:

{% highlight c++ linenos %}
{% include_relative obj_ptrs_1_code/third.cpp %}
{% endhighlight %}

Lines (D) and (E) use SmartAdder's version of add(). This is very unfortunate as it encourages use of raw pointers and references which are both a source of many programming errors. I think this is quite an important point. Pointers are one of the most difficult features to use correctly in C++ and polymorphism one of the most useful features (used judiciously). In C++ they come as a package. A radioactive cake that you both want and don't want.

Safe technique 1: safe use of pointers
======================================

This topic has been covered so many times before and I don't want to go into too much detail. Basically by using safe pointer types like shared_ptr, scoped_ptr, unique_ptr, etc. you can get safe (and sometimes efficient) access to polymorphic behaviours without passing around raw pointers and references. It is a good solution, but what always rankles with me is that the syntax is verbose and inflexible, and you have to pick which pointer types to use and you cannot easily change your mind.

{% highlight c++ linenos %}
{% include_relative obj_ptrs_1_code/fourth.cpp %}
{% endhighlight %}

You could use typedefs, wrapping, auto keyword and various other techniques to get short and easily changed pointer types, but it doesn't quite reach the convenience of a built in language feature.

Safe technique 2: Curiously recurring template pattern (CRTP)
=============================================================

This one is not a silver bullet. It's kind of hard and laborious to use, but you can achieve some subtype polymorphism without using dynamic dispatch (meaning a pointer is not needed). Here's some code:

{% highlight c++ linenos %}
{% include_relative obj_ptrs_1_code/fifth.cpp %}
{% endhighlight %}

As you can see, it's a bit less convenient to use than dynamic polymorphism, though that could probably be mitigated by code generation. What this does is gets you some kind of inheritance, provided that the type of the object is known at compile time. In this example the add() function from BaseAdder is inherited by DerivedAdder, but won't use dynamic dispatch since it's non-virtual. The increment() function is special because a version of it is provided in both BaseAdder and DerivedAdder. Line (F) uses BaseAdder::increment(). However at line (G) the call is delegated statically to DerivedAdder::increment(). Hence this behaves a little like code inheritance, but without any virtual functions.

This is not really a technique for every day use. But it does allow you to use code inheritance of sorts without using pointers or references.

Safe technique 3: don't use code inheritance at all
===================================================

This is a great tip that is quite an eye opener when you get used to it. The idea is simply not to use direct code inheritance at all, to avoid having to deal with pointers at all. That way you don't need to worry about freeing the pointers or being so careful with ownership. Use of inheritance is frequently held up as a quality goal in object oriented languages when it is often not necessary to use it at all. It is perfectly possible to design an object oriented language that does not have inheritance, and instead relies on interfaces and inclusion of other classes to share functionality.

Here's a C++ example that doesn't use inheritance at all:

{% highlight c++ linenos %}
{% include_relative obj_ptrs_1_code/sixth.cpp %}
{% endhighlight %}

At line (J), the instance of incrementer embedded inside DumbAdder is used to provide increment functionality. In common with CRTP, this doesn't use subtyping, but then again, it's not really needed.

The frustration with this technique is not being able to create a GoodAdder class and have them both be instances of an Adder interface. For that you do need dynamic dispatch, and that means pointers.

C++ conclusion
==============

I can't think of any other ways beside these right now off the top of my head. Leave a comment if you know another and I may add it.

Now I'm going to look at polymorphic functions in go, to see the difference.

Comparison with another language: go
====================================

Although I haven't used it for a big project yet, I like the look of go. It is a much simpler language than C++, and lets you do dynamic dispatch very naturally. I hope I don't make any mistakes here, but I might because I'm not so familiar with the language.

Go doesn't have subtyping. Hence to use code from another object type you cannot inherit it, you must invoke a different object to access that functionality. However it does have interface types, a variable of an interface type can store any object that conforms to the interface. So there is polymorphism but between on objects that share the same interface. Also the fear factor with using pointers is slightly milder, because go has garbage collection.

{% highlight go linenos %}
package main

import "fmt"

type Incrementer struct{}

func (i Incrementer) increment(x uint) uint {
	return x + 1
}

type Adder interface {
	add(x uint, y uint) uint
}

type DumbAdder struct {
	incr Incrementer
}

func (a DumbAdder) add(x uint, y uint) uint {
	var retval uint = 0
	var i uint
	for i = 1; i <= x; i++ {
		retval = a.incr.increment(retval)
	}
	for i = 1; i <= y; i++ {
		retval = a.incr.increment(retval)
	}
	return retval
}

func main() {
	var dumb DumbAdder = DumbAdder{}
	var poly Adder = dumb
	var polyPointer Adder = &dumb //N.B. don't even need to say Adder*

	fmt.Println(dumb.add(5, 6))
	fmt.Println(poly.add(5, 6))
	fmt.Println(polyPointer.add(5, 6))
}
{% endhighlight %}

This example shows polymorphism using interfaces, using both pointers and non-pointers. There is no need to resort to anything complex.

References
==========
[The Curiously Recurring Template Pattern in C++](http://eli.thegreenplace.net/2011/05/17/the-curiously-recurring-template-pattern-in-c)

[Polymorphism - Wikipedia](http://en.wikipedia.org/wiki/Polymorphism_(computer_science))

C++ in a nutshell - O'Reilly






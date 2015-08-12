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

However my eyes have been opened a bit after watching Sean Parent's talk called ["C++ seasoning"](http://channel9.msdn.com/Events/GoingNative/2013/Cpp-Seasoning). Now I also want to achieve *value semantics*. Value semantics means that assigning a value to a different variable copies it (or, with std::move, moves it without copying). The opposite is reference semantics meaning that only the pointer is copied. With value semantics a value is guaranteed to be the sole way to access the corresponding area of memory.

{% highlight c++ linenos %}
{% include_relative obj_ptrs_2_code/value.cpp %}
{% endhighlight %}

Using value semantics helps to make code easy to understand, since you don't need to check what other variables (possibly on other threads) might be sharing the same data. There is no sharing.

Also you don't have to worry about cleaning up pointers any longer, because there are none. 

I said in my earlier post that in C++ polymorphism is only available via a pointer/reference variable. That is true, but there are a variety of techniques to maintain value semantics without losing polymorphism, and the purpose of this post is to describe those.

unique_ptr
===========
The simplest way to get closer to value semantics with pointers is definitely std::unique_ptr. An area of memory is guaranteed to be uniquely owned. 

{% highlight c++ linenos %}
{% include_relative obj_ptrs_2_code/unique.cpp %}
{% endhighlight %}

However we haven't really achieved proper value semantics, because assigning a value doesn't work and we have to move it around explicitly.

Let's look for a better solution.

Hand rolled value semantics with polymorphism
=============================================

Lets make a type with value semantics that can take anything with a Print() method:

{% highlight c++ linenos %}
{% include_relative obj_ptrs_2_code/wrapper.cpp %}
{% endhighlight %}

There is no such thing as a free lunch though. Polymorphism is still achieved using a pointer, but it is hidden away so that it doesn't contaminate client code.

We get value semantics. There is no inheritance in the printable objects. We can also make heterogeneous containers.

boost type_erasure
==================

The only problem is that the code is rather long and laborious, every time we have a different concept, we would have to define 3 new classes for it.

Let's check out boost::type_erasure to make it short and sweet.

{% highlight c++ linenos %}
{% include_relative obj_ptrs_2_code/type_erasure.cpp %}
{% endhighlight %}

Conclusion
========

Normally in C++, polymorphism brings an unwelcome friend along with it - pointers. But I've described a couple of ways to get polymorphism without pointers being in view (at least in the client code).


References
==========
[Boost.TypeErasure](http://www.boost.org/doc/libs/1_58_0/doc/html/boost_typeerasure.html)

[C++ Seasoning](http://channel9.msdn.com/Events/GoingNative/2013/Cpp-Seasoning)

C++ in a nutshell - O'Reilly

[Inheritance is the Base Class of Evil code sample](https://gist.github.com/berkus/7041546)

[Intrusive value semantic polymorphism example.](https://gist.github.com/sean-parent/6798797)

[Value semantics](https://akrzemi1.wordpress.com/2012/02/03/value-semantics/)

[What is value semantics?](https://isocpp.org/wiki/faq/value-vs-ref-semantics#val-vs-ref-semantics)

[Minion AnyVarRef](https://bitbucket.org/stacs_cp/minion/src/c64e2526b4b0156b9881e3ebe0fc9d55709335aa/minion/variables/AnyVarRef.h?at=default)

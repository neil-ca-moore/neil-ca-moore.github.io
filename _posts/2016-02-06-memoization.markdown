---
layout: post
date:   2016-02-06 15:22:16
title: "Memoization in C++"
categories: c++
---

<!-- 

https://pimiddy.wordpress.com/2012/04/20/pure-functions-in-cc/
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3744.pdf
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4296.pdf
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3597.html

What is memoization
- first example code
Why does an impure function not work well there?
What is a pure function
How to approximate purity in C++
What are all the ways that a function can be impure in C++
- solution using object with no data in it
- want to avoid compiling code that may be wrong
- solution using constexpr with a dummy function (didn't work)
- SFINAE solutions (not sure if this worked)
just assert that the function is stateless

-->

Memoization of a function F is the creation of a new function G that produces exactly the same results as F except by lookup instead of repeated computation. The first time G is used for a particular argument the result is computed normally and stored; the second and subsequent times it is not computed but returned from storage.

For a function that's very quick to compute, this may result in a decrease in performance. For expensive computations it may result in a speedup. I'm not going to talk about how to decide when to use memoization, this is purely a nuts and bolts discussion of how to memoize a function in C++. 

# Basic memoization

Here is how to write a very basic generic memoization routine in C++14:

{% highlight c++ linenos %}
{% include_relative memoization_code/basic.cpp %}
{% endhighlight %}

Line 6 shows a function that we will try to memoize, called funnyThing.

At line 13 the MemoizeG class begins. MemoizeG is short for "memoize generic". Its members are a function object called _f and a table of memoized values called _table. _f is a copy of the function called into the constructor. _table is a vector where each entry is a tuple consisting of arguments plus a result. E.g. when MemoizeG(funnyThing) is applied to (1, 1.0) the result is 3.141. The value stored in the table will be (1, 1.0, 3.141). No attempt has been made to store the data efficiently, it may be quicker to sort the values or store them in something other than a vector.

The operator() function of MemoizeG at line 21 implements the function application operator of the class. It first attempts to find the value already in the table, in which case it is returned. Failing that the value is computed and stored.

This class uses some fairly new C++ features. The "using" keyword is new and replaces code like

{% highlight c++ linenos %}
typedef ResultType<ArgTypes...)> StoredResultType;
{% endhighlight %}

The MemoizeG template is variadic, meaning that it can take a variable number of template parameters. Unpredictable numbers of template arguments are not individually named, i.e. ResultType is named, but ArgTypes is a name for several parameters. ArgTypes can be processed using new [parameter pack](http://en.cppreference.com/w/cpp/language/parameter_pack) syntax for unpacking the parameters. E.g. at line 17 a new tuple type of composed of ResultType plus each type in ArgTypes is built using the syntax ArgTypes... which expands to ArgType[0], ArgType[1], ..., ArgType[n-1].

Line 47 shows an example of the class being instantiated and used. The syntax is a little verbose, unfortunately we had to type out the function signature in the definition. To get around that issue I made a factory method called MakeMemoizeG at line 41. It exploits the fact that function templates types can be inferred and uses that fact to allow functions to be passed in without restating the types.

# Pure functions

The danger with using MemoizeG is that there is nothing to stop you passing in a function which is [impure](https://en.wikipedia.org/wiki/Pure_function), meaning that it might get different values if it's computed a second time. Functions may be impure because they contain (directly or indirectly by calling other functions)

- accesses to state like static variables and class members;
- accesses to disk, random numbers and other IO;
- undefined behaviour like reading uninitialised memory, 
- etc.

When a function is impure MemoizeG will not bother to execute it a second time on the same inputs, even though it may have side-effects that we want. Hence we should not sure MemoizeG on impure functions.

I started thinking about how we could begin to verify that a function is pure before allowing MemoizeG to use it. There are a few existing pages on this topic [here](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3744.pdf) and [here](https://pimiddy.wordpress.com/2012/04/20/pure-functions-in-cc/).

# Constexpr

To recap a constexpr is a compile time expression. It can be computed by the compiler.

The restrictions on constexprs seem to make them pure expressions, in the sense that if the compiler computes them repeatedly then they must be same every time. That is based on my reading of section 5.20 of the [C++ standard](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4296.pdf) and I might be wrong.

A constexpr function is a function that can be used in a constexpr. Constexpr functions must satisfy [constraints](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3597.html) including the fact that they contain only a single return statement (in C++11, in C++14 it is more complicated).

I had incorrectly surmised that a constexpr function must follow the rules on constexprs for all possible inputs, which would make them pure functions. However I eventually figured out that declaring a function as constexpr only means that it produces a constexpr result when it is applied to constexpr arguments. I had hoped that it would be verified as constexpr for all possible inputs, which would prove it is pure.

Howeever even if it doesn't verify constexpr for every possible input, the compiler will prevent you from using static variables and doing some other things that are obviously impure. For example the following function compiles because b = 0 is constexpr, which is not a good thing for our purposes:

{% highlight c++ linenos %}
constexpr int f(bool b)
{ return b ? throw 0 : 0; }
{% endhighlight %}

However the next one fails to compile because it uses a static variable:

{% highlight c++ linenos %}
static int a;
constexpr int usesA(int x) {
    return x + a;
}
{% endhighlight %}

Hence I first tried to enforce that a function is constexpr, because it's pretty close to the concept of purity.

## Typecheck constexpr

I put quite a lot of effort into writing a variant of MemoizeG to enforce that the function provided is constexpr. My basic approach was to try creating a constexpr const member variable and then to invoke the function on some arguments, but that didn't really work out for a variety of reasons:

* A function can have any number of arguments of any type. I didn't find a way to make an expression to call the provided function with arbitrary values.

* As stated above, just because a function is constexpr for one set of inputs doesn't mean that's true of all inputs.

Here is a short example of 4 failed attempts at adding a member that would fail to compile when Functor wasn't constexpr:

{% highlight c++ linenos %}
//This compiles just fine even when Functor::operator() is not constexpr, because the only requirement for
//constexpr is that there exists at least one parameter choice that is a constexpr. It's not necessary that
//every single combination of parameters produces a constexpr.
constexpr ResultType checkFunction(ArgTypes... args) {
    return Functor()(args...);
}

//This doesn't work because apply_from_tuple is not constexpr.
static const ResultType constExprChecked = apply_from_tuple(Functor(), std::tuple<ArgTypes...>());

//This doesn't work because the function is not necessarily a unary function on a number
static const ResultType constExprChecked = Functor()(1);

//This doesn't work because ArgTypes... doesn't create a tuple of default values. I couldn't find any
//parameter pack expansion that would create default values for each type in the pack.
static const ResultType constExprChecked = Functor()({ArgTypes...});

//I gave up at this stage!
{% endhighlight %}

I might come back to the problem of compile time enforcing that a function is constexpr at some stage, but it wasn't something I was able to solve in a reasonable time on this occasion. If anybody has any ideas please leave a comment or send me a message.

# Stateless object

Since I wasn't able to enforce constexpr. I looked for another solution.

[An Adobe colleague](http://sean-parent.github.io) suggested to me that a function is relatively safe to memoize when it is either

- a pointer to a function, or
- a stateless object

This constraint doesn't completely prevent impurity because such a function can still do IO and access static data, but it does help a bit because it removes the chance to use non-static variables.

## Typecheck stateless

I had more luck implementing this idea. The idea exploits the [empty base class optimisation (EBCO)](http://en.cppreference.com/w/cpp/language/ebo) (thanks to [Sean Parent](http://sean-parent.github.io) for telling me about this). The EBCO means that when a base class is empty, it does not occupy any memory in a derived class.

{% highlight c++ linenos %}
template<typename Functor, typename ResultType, typename... ArgTypes>
class MemoizeStateless<Functor, ResultType(ArgTypes...)>
{
private:
    using StoredResultType = std::pair<std::tuple<ArgTypes...>, ResultType>;
    
    constexpr ResultType checkFunction(ArgTypes... args) {
        return Functor()(args...);
    }
    
    class TestSubclass : Functor {
        int x;
    };
    
    static_assert(sizeof(TestSubclass) == sizeof(int),
                  "Functor appears not to be stateless, is it really memoizable?");

    ...
{% endhighlight %}

If Functor has state then the static assertion at line 15 will fail because TestSubClass will have an int plus other state in it. If the Functor is stateless then the EBCO will apply, and TestSubclass will only occupy the memory of an int. Hence this class will only compile when the Functor argument is stateless.

Pretty neat!

# Conclusion

It was pretty straightforward to do a generic memoization wrapper. However I didn't really manage to enforce that only pure functions were put into the function. I couldn't find a way to enforce that a function is constexpr, and moreover constexpr is not exactly the same thing as pure. I managed to enforce that the function was stateless, but I don't consider that to be a perfect solution either. If anybody has any good ideas how to do this please let me know.

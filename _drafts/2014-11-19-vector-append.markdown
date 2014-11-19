---
layout: post
title:  "Appending to vector in C++"
date:   2014-11-19 19:54:19
categories: c++
---
Thanks to [Chris Jefferson](http://caj.host.cs.st-andrews.ac.uk) for chatting to me about this.

I was busy working on some code today and I couldn't understand why it is not possible to write the following sort of code.

~~~ cplusplus
#import <vector>

void vectorStuff() {
	std::vector<int> numbers;
	std::vector<int> other_numbers = {1, 2, 3, 4, 5};

	numbers.insert(numbers.end(), other_numbers);

	--or even--

	numbers.push_back(other_numbers);
}
~~~

Instead I've got to say

~~~ cplusplus
std::vector<int> numbers;
std::vector<int> other_numbers = {1, 2, 3, 4, 5};

numbers.insert(numbers.end(), other_numbers.begin(), other_numbers.end());
~~~

Contrast this code with go:

~~~ go
var numbers = []int{}
var other_numbers = []int{1, 2, 3, 4, 5};

numbers = append(numbers, other_numbers...)
~~~	

I know there can be a lack of affordances for programmers in C++, but this is pretty annoying! Why the heck is there no function like the following?!

~~~ cplusplus
template<typename T>
class vector<T> {	
	...
	void push_back(const std::vector<T>& toInsert);
	...
}
~~~

It turns out to be related to the abandoned c++11 concepts idea. I didn't really appreciate what this was all about until now.

Suppose that we do add the above function

---
layout: post
title:  "Appending to vector in C++"
date:   2014-11-19 19:54:19
categories: c++
---
Thanks to [Chris Jefferson](http://caj.host.cs.st-andrews.ac.uk) for chatting/informing me about this.

I was busy working on some code today and I was thinking about why the C++ standard library does not allow me to write the following code:

{% highlight c++ linenos %}
#import <vector>

void vectorStuff() {
	std::vector<int> numbers;
	std::vector<int> other_numbers = {1, 2, 3, 4, 5};

	numbers.insert(numbers.end(), other_numbers);

	--or even--

	numbers.push_back(other_numbers);
}
{% endhighlight %}

Instead I've got to say

{% highlight c++ linenos %}
std::vector<int> numbers;
std::vector<int> other_numbers = {1, 2, 3, 4, 5};

numbers.insert(numbers.end(), other_numbers.begin(), other_numbers.end());
{% endhighlight %}

Contrast this code with go:

{% highlight go linenos %} 
var numbers = []int{}
var other_numbers = []int{1, 2, 3, 4, 5};

numbers = append(numbers, other_numbers...)
{% endhighlight %}	

I know there can be a lack of affordances for programmers in C++, but this is pretty annoying! The function I am imagining would have this signature:

{% highlight c++ linenos %}
template<typename T>
class vector<T> {	
	...
	void insert(vector<T>::iterator __position, const vector<int>& to_add);
	...
}
{% endhighlight %}

According to Chris this function would not get standardised because it's not general enough (he should know, he is on the standards committee). One way of making it general would be the version using iterators I wrote above. The other way would be adding this:

{% highlight c++ linenos %}
template<typename Container>
void insert(vector<T>::iterator __position, const Container& to_add);
{% endhighlight %}

However, std::vector already has a function with this signature:

{% highlight c++ linenos %}
template<typename Container>
void insert(vector<T>::iterator __position, const T& value);
{% endhighlight %}

Going back to the int example above I can add a short to a std::vector<int> using that overload, but if 
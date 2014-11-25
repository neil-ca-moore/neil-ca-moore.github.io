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

First world problems, eh? Contrast with this code written in go:

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

According to Chris this function would not get standardised because it's not general enough (he should know, he is on the standards committee). One way of making it general would be the version using iterators I used above. The other way would be adding a version that takes any container as argument (let's call this **function (1)**):

{% highlight c++ linenos %}
template<typename Container>
void insert(vector<T>::iterator __position, const Container& to_add);
{% endhighlight %}

However, std::vector already has a function with the following signature (**function (2)**):

{% highlight c++ linenos %}
template<typename Container>
void insert(vector<T>::iterator __position, const T& value);
{% endhighlight %}

Because of function (2) I can add a short to a std::vector<int>. That is definitely too useful to give up. However if we don't want to give up function (1) we cannot add function (2) as well, because the function template is exactly the same, the compiler won't know which one to use!

{% highlight c++ linenos %}
std::vector<int> numbers; //includes insert variant (1)

short a = 5;
numbers.insert(numbers.begin(), a); //doesn't work, how annoying!
{% endhighlight %}

So that's why you cannot insert a vector into a vector! If I had the characterise the reasons, I'd say:

1. Overly dogmatic library design. Really, there should be a function to just insert a vector. It's possible. It's useful.
2. When working in combination with templates, overload resolution cannot resolve functions with the same number of parameters properly. That is a problem "concepts" is designed to solve. Concepts is a candidate for a future version of C++, and after writing this I understand why it needs to be included in the language.

If you have any corrections or something else to say please leave a comment.

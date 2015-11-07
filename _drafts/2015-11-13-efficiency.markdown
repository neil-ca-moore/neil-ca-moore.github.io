- - -
layout: post
date:   2015-11-13 15:22:16
title: "Efficiency"
categories: me talks
- - -

Essay to get my thoughts together for a short talk to Graham Kirby's [CS3051 Software Engineering class](https://info.cs.st-andrews.ac.uk/student-handbook/modules/CS3051.html).

- - -

<!-- 
Outline:

- why to be efficient
- - save time and memory
- - save energy (important for mobile)

- point out that efficiency and other attributes are often not in conflict, but I'm going to talk about trade offs when they are

- What is efficiency?
- - memory, disk, network, CPU, liveness, threads, processes, etc.

Example code for each and every one

- keep code simple (MAINTAINABILITY)
- - compare a couple of things, e.g.
- - - write XML to disk
- - - binary dump to disk
- - or
- - - compare ruby on rails with writing a website in rails?
- - or
- - - stuffing bits and pieces into structs
- - - using clean data structures
- - simple code has genuine value
- - - it is easier to maintain, this is the big one
- - - - code has to be capable of being changed
- - - quicker to write
- - - less likely to harbour bugs

- premature optimisation (SPEED)
- - pareto principal
- - actually use a profiler live?
- - when to stop? there is such a thing as fast enough
- - http://programmers.stackexchange.com/questions/89620/clean-readable-code-vs-fast-hard-to-read-code-when-to-cross-the-line
- - opinion: It's probably easier to optimise a clean program written without excessive attention to performance, than to optimise a program written for speed from the outset. But it's definitely easier to maintain the clean program.
- - http://c2.com/cgi/wiki?PrematureOptimization

- choice of language (MAINTAINABILITY + VELOCITY)
- - big area
- - maintainable languages are not always slow, but they tend to be
- - why: to get speed you need to take control of more elements, e.g. types, optimising simple concepts, function call method
- - not always, but often, give counterexamples:
- - - e.g. letting the compiler do its stuff
- - - e.g. being more abstracted lets the compiler/runtime optimise
- - - e.g. modern languages can recover the losses that come from vagueness
- - - e.g. domain specific languages like constraints

- safety (SAFETY)
- - runtime checking to make sure it's not being used as an attack vector

- maximise short term progress, fix later
- - we need to complete projects
- - - prototypes, optimise for quick feedback
- - we can fix it up later once we know it's the right thing
- - usually we don't get an opportunity to rewrite so it's a good idea to make the code nice but slow

- in a pinch to speed development (VELOCITY)
- - this is the worst kind because we are just trying to complete the functionality without paying attention to speed, it could turn out to be a mistake

- not work efficient (WORK EFFICIENCY)
- - e.g. bittorrent, can't find any reference to this
- - https://en.wikipedia.org/wiki/Algorithmic_efficiency
- - is precaching an example of this? We may get data that is never needed. That is optimising time at the expense of network.

- small problem (VELOCITY)
- - If the problem is and will remain small then it doesn't matter how it's written.
- - if the constant factor is small then worse algorithms might be better e.g. map vs vector
- - - e.g. memory locality, etc.
- - - show storage hierarchy charts
- - example: talk about the Sabre thing of writing to disk vs. sending over named pipe

conclusion:
- - for me writing efficient programs is fun, I like thinking up efficient algorithms, using profilers and using powerful languages like C++
- - but there are many good reasons not to take it too far
- - please leave a comment if you can think of anything else
-->

This is about why you might want software to be efficient and when you might not. The main reasons to be efficient are to make software run really well on peoples' computers: not to be slow, use up too much memory, too much bandwidth or too much power. The main reasons for deciding to accept a level of inefficiency fall under the headings "maintainability", "velocity" and "safety". By maintainbility I mean creating software that is simple for teams to fix bugs, add features, port to new platforms, adapt to new APIs, etc. Velocity is a measure of how quickly work is being done on a software project. "Safety" is when users can run programs without worrying that they will behave really badly, e.g. crash or be attack vectors.

I'm going to go into efficiency under all of these headings.

# What do I mean by efficiency?

Basically using as few of the following computer resources as we can:

- processor
- memory
- cache
- network
- power
- disk space
- etc.

# Why be efficient?

I hope this question is completely unnecessary to ask any software engineer. There is probably a generational difference around efficiency. In the olden days, computers were severely resource constrained for what now seem little very simple problems. Not a single byte of memory could be wasted. 14.4k modems were just astonishingly slow. People had to struggle against resource limitations just to be able to make a program run at all.

In my day to day work I don't hit up against those types of hard limits on resources much. It does happen, but it's not something that I absolutely need to keep in mind all the time.

For certain classes of problems that are close to the limits of practical computers, performance considerations are in the forefront of people's mind. For example, imaging, videos, games, AI, big data, and so on. I worked on a project like this in St Andrews. That was the Minion constraint solver problem which was written from the outset to be blazing fast. I frequently thought the programs's use of CPU, memory, data locality and so on.

Normally in my work for Adobe, efficiency is not strictly necessary to make the program work in the most basic way. I work on a file sync program in some ways comparable to Dropbox. On human scale problems involving thousands of files and edits being made fairly slowly the computer has enough resources that naive algorithms will solve the problem after a fashion.

The reason why we still need to care about efficiency is that we want the program to work brilliantly and not merely limp along. It's not OK to use the CPU so much that the fan turns on! It's not OK to have memory leaks. It's not OK to cause momentary hangs while somebody's trying to work. We would be embarrassed by our work if they were excessively wasteful.

It's also an intellectual challenge to do as much as possible with as little as possible. 

It also waste's the user's money for a program to be inefficient. That's not a direct cost to the person writing the software, but it's still our responsibility. I worked out that the software I write is running as a daemon on so many computers worldwide that it's probably [using the energy](https://blog.mozilla.org/nnethercote/2015/08/26/what-does-the-os-x-activity-monitors-energy-impact-actually-measure/) of a [fairly sturdy generator](https://en.wikipedia.org/wiki/Power_station#Typical_power_output) (definitely in the megawatt range), so I can make a small contribution to stopping climate change by optimising my code!

# Is there any conflict between efficiency and other things?

I will argue that sometimes efficiency gets in the way of some of our other aims in the software development process. Usually it doesn't and we can get sufficient efficiency with no trade off. However getting to 100% efficiency is always to the detriment of every other measure of quality. Writing code in assembly always dominates other ways to write software, but it takes forever, is unmaintainable and completely insecure.

But as I've said above there is always a serious tension. We can save users' energy, money and time by making our software more efficient.

How do we decide where to draw the line?

I'm going to go through a few places where I think there is an defensible argument for letting efficiency drop a little bit, but never at the expense of software working poorly. 

# Code maintainability

By maintainability I mean features of the code that make it easier to change later, such as loose coupling, simplicity, good naming, cohesion and so on.

Often there is a good argument for taking maintainability over efficiency. Maintainable software tends to be contain fewer bugs, and when bugs do creep in they are easier to fix. It is quicker to change and quicker to find and fix bugs. These properties allow you to deliver real benefits to users that they will probably value more than a little bit of performance. 

If there is one thing that I didn't fully appreciate from doing my degree in Computer Science, it is definitely that maintainability of code is a very important issue in day to day software engineering. I spent probably 90% of my time learning algorithms and techniques that I don't use on a day to day basis. However, I got just a handful of lectures on structuring code for simplicity, plus a few practical projects that were supposed to give us focussed practice by having another person make changes to our code. However it didn't really sink in until I was working on the same code for months and years, having to change what it does very frequently. You are constantly having to think what changes might be made in the future and how best to write something so that another engineer can understand it quickly just by reading the code.

How do we decide when to reduce maintainability in favour of efficiency? I would say never to do it, unless the project cannot be achieved without blistering speed and only then if you've already learned to do things maintainably first. This is almost never true in practice.

This is a ludicrously bad program that probably uses [very little extra memory](http://www.catb.org/esr/structure-packing/) to get the job done:

{% highlight c++ linenos %}
#define READABLE    (1)
#define WRITABLE    (2)
#define CUSTOM_ICON (4)
#define MAX_NAME_LEN (1024)

struct MyData {
  //bitfield
  char savedToDisk:1; //is this saved to disk?
  char settings:3;    //bit mask of settings
  char* name;         //On Windows this UTF-16, on Mac it's UTF-8.
};

int myFunc() {
  MyData data;
  data.savedToDisk = 0;
  data.settings = READABLE | WRITABLE;
  data.name = malloc(MAX_NAME_LEN);
#ifdef _MSC_DEV
  *data.name = L"neil";
#else
  *data.name = "neil";
#endif
}

void printReadable(MyData* data) {
  if(data->settings & READABLE != 0) {
    printf("it's readable\n");
  } 
}
{% endhighlight %}

but there is almost never a scenario where you wouldn't be better off with this code (and it can be still improved):

{% highlight c++ linenos %}
#include <string>
#include <ostream>

class MyData {
public:
  MyData() : savedToDisk(false), isReadable(false), isWritable(false), hasCustomIcon(false) {}

  bool savedToDisk;
  bool isReadable;
  bool isWritable;
  bool hasCustomIcon;
  std::string name;   //UTF-8
};

int myFunc() {
  MyData data;
  data.savedToDisk = true;
  data.isReadable = true;
  data.isWritable = true;
  data.name = "neil";
}

void printReadable(MyData data) {
  if(data.isReadable) {
    std::cout << "it's readable" << std::endl;
  } 
}
{% endhighlight %}

# Premature optimisation

The great thing about choosing maintainability over efficiency is that usually it doesn't matter anyway. You can optimise your way out of problems with a relatively small number of changes. I am going to quote again perhaps the most used quote in Computer Science:

> Programmers waste enormous amounts of time thinking about, or worrying about, the speed of noncritical parts of their programs, and these attempts at efficiency actually have a strong negative impact when debugging and maintenance are considered. We should forget about small efficiencies, say about 97% of the time: premature optimization is the root of all evil. Yet we should not pass up our opportunities in that critical 3%. -- Donald Knuth

What he is getting at is that programs actually tend to spend a lot of their runtime in a relatively small number of code paths. Even with a lot of experience it can be hard to guess in advance what these will be. Imagine we optimise the start up code for our program. We could get it to run in 50 microseconds, when it might take 500 microseconds if we didn't bother. That code will only run once, nobody will notice it. Yet we spent our valuable time making it faster and it will probably suck up more time later in maintenance.

There is a principle called the [Pareto Principle](https://en.wikipedia.org/wiki/Pareto_principle) that says 80% of the effects can often be had for 20% of the effort. This definitely applies to software optimisation in the sense that we can generally optimise a handful of pieces of code (probably less 20% of all code) and reduce our program's runtime by a large amount (80% is not unrealistic).

Hence the best way to engineer a program is write for maintainability, then optimise according to the Pareto Principle. 

The tool for doing this is a profiler and it's something every software engineer needs learn to use properly and use often. Profilers can find code that uses a CPU, memory, energy and much more. Examples of such tools include [Instruments in XCode](https://developer.apple.com/library/watchos/documentation/DeveloperTools/Conceptual/InstrumentsUserGuide/index.html), [valgrind](http://valgrind.org) and they are typically can be found for all languages and platforms with a bit of investigation.

It's often hard to know when to stop. But if you removed the obvious and significant bottlenecks, and the program is subjectively good enough, then you can stop and the job is done.

You will probably end up with a more efficient program by writing it for maintainability without any interest in efficiency, than by writing for efficiency from the start. This definitely does not apply to everyone. [Chris Jefferson](https://caj.host.cs.st-andrews.ac.uk) wrote [minion](http://constraintmodelling.org/minion/) for speed, and that is exactly what he got, but that is because he is an expert. But for normal programmers working on normal problems if code is well structured then performance optimisations are easier because changes in general are easier. Conversely already optimised code is hard to change and the correct way to make a performance optimisation may be hard to fathom.

# Choice of implementation language

It's well known that using particular languages helps to improve development speed, maintainability, efficiency and various other attributes we need to choose between. New and better languages mean we need to make fewer trade offs, for example maybe C++ does better than [whitespace](http://compsoc.dur.ac.uk/whitespace/tutorial.html) on all those axes[^lang-note]. 

[^lang-note]: This is a really weak example, it's actually hard to think of any language that everybody would agree is strictly better than any other.

Some languages that people find to be highly maintainable are not as fast as they could be. For something like ruby which you can use to create very simple and short programs, the cost is that the language is interpreted and optimisations involving types are not as advanced as a statically typed language like C++. The language has a lot of powerful abstractions which may or may not be fast. Some of the reasons why maintainable languages are slower are intrinsic, for example abstraction costs a little in runtime because of the indirection involved.  Conversely super fast languages like C++ require you to take control of many aspects of programming like memory management, decisions about how functions are resolved and so on. There are many choices and you can take less for granted. These all make a program a little more brittle than when the language designers have decided for you.

Types are interesting because depending on your viewpoint they add both maintainability and efficiency. Maintainability because you can be more sure that changes you make still make semantic sense if the compiler is checking types. Efficiency because static type checking avoids runtime overhead and allows more optimisation by the compiler. But many people would disagree that statically typed programs are more maintainable, because they are perhaps more verbose. This is where modern languages like Haskell, go, rust and C++11 save the day by providing type inference, letting you leave out explicit types in many cases, but still get static checking.

Other times we have the exact opposite problem, that using a very maintainable language buys you great performance characteristics. Domain specific languages like regular expressions, logic programs, constraint programs and so on are likely to get a far better result than you could achieve writing an algorithm from scratch in a conventional type of language, as well as being much clearer to read and maintain.

This is a huge area and I've only touched on a few issues briefly. However it's clear that choice of language is important and that you have to choose a language that you can use to meet your efficiency requirements. For performance critical code it may be necessary to pick an efficient language, but for most problems you have a lot of choices of language that will be fast enough and you can freely choose to optimise for other goals besides efficiency.

# Safety

Customers care about safety in our programs. They shouldn't crash; they shouldn't allow cross site scripting or code injection or buffer overflow attacks. This is one feature that often costs efficiency, but is not a trade off. Programs just have to be safe. You just have to take the efficiency hit of using encryption, checking parameters carefully, not overfilling buffers, etc. Using a modern safe language makes this easier but often costs in efficiency (rust is notable counterexample to this).

# Maximising short time progress

Now for a bit of a change of direction. Sometimes we need to write code fast, rather than write fast code. In businesses big and small you've got to meet deadlines and compete to get something finished fast. 

The agile mindset is very widespread in the computer industry, where getting something in front of customers quickly is considered to be really important. This is not supposed to mean a reduction is quality, just a reduction in scope. But sometimes the pressure to work fast comes at the expense of software quality. In this case I would personally priorise various aims over efficiency. Correctness, maintainability, safety are all much more important than efficiency when deadlines loom. Hopefully performance problems will be uncovered by informal testing. The key thing is that performance needs to be *good enough* for most people.

Prototypes are another problem. They are code that is designed to demonstrate the functionality rather than to become the product. In my experience there is no such thing as a good prototype, if it looks good then it is by definition not a prototype because somebody will try to make it the product. The same reasoning w.r.t. efficiency applies though, make the code as good as possible because performance can usually be fixed later.

For desktop software that runs on users' machines it is more crucial to get performance right first time, because it costs users time and money to put up with the problem, then even more when they have to install an update to fix the problem. For server side code it costs us money, so it's not so crucial and also easier to fix later with an update. 

# Work efficiency

Work efficiency basically means doing within a constant factor of the same amount of work as the fastest possible algorithm. So why would we ever choose to do more work than we need to? One good reason is for usability. Let me give a few examples:

Precaching
: We read from disk or download data that the user doesn't necessarily need. This is choosing time over disk or network bandwidth. We do it because we perceive that one of resources we want to be efficient with is more important than the others. For example Adobe Illustrator can be seen loading various plugins at start up (including Adobe Idea File Format which I helped write), because we perceive that users would prefer to put up with a little slowness in getting started, rather than have to wait later on once they're in the zone. 

Redundancy
: I can't find any reference to this written down, but some towards the end of downloads some Bittorrent clients used to download chunks multiple times. Say that you're down to the last 2 chunks but you have enough bandwidth to fetch from 4 peers. You can request each chunk from two different peers then if one of them happens to be faster than the others you finish sooner. 

# Small problems


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

- maximise short term progress, fix later
- - we need to complete projects
- - - prototypes, optimise for quick feedback
- - we can fix it up later once we know it's the right thing
- - usually we don't get an opportunity to rewrite so it's a good idea to make the code nice but slow

- in a pinch to speed development (VELOCITY)
- - this is the worst kind because we are just trying to complete the functionality without paying attention to speed, it could turn out to be a mistake

- small problem (VELOCITY)
- - If the problem is and will remain small then it doesn't matter how it's written.
- - if the constant factor is small then worse algorithms might be better e.g. map vs vector
- - - e.g. memory locality, etc.
- - - show storage hierarchy charts
- - example: talk about the Sabre thing of writing to disk vs. sending over named pipe

- safety (SAFETY)
- - runtime checking to make sure it's not being used as an attack vector

- choice of language (MAINTAINABILITY + VELOCITY)
- - big area
- - maintainable languages are not always slow, but they tend to be
- - why: to get speed you need to take control of more elements, e.g. types, optimising simple concepts, function call method
- - not always, but often, give counterexamples:
- - - e.g. letting the compiler do its stuff
- - - e.g. being more abstracted lets the compiler/runtime optimise
- - - e.g. modern languages can recover the losses that come from vagueness
- - - e.g. domain specific languages like constraints

- premature optimisation (SPEED)
- - actually use a profiler live?
- - when to stop? there is such a thing as fast enough
- - http://programmers.stackexchange.com/questions/89620/clean-readable-code-vs-fast-hard-to-read-code-when-to-cross-the-line
- - opinion: It's probably easier to optimise a clean program written without excessive attention to performance, than to optimise a program written for speed from the outset. But it's definitely easier to maintain the clean program.

- not work efficient (WORK EFFICIENCY)
- - e.g. bittorrent, can't find any reference to this
- - https://en.wikipedia.org/wiki/Algorithmic_efficiency

conclusion:
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

# Code simplicity


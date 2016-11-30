---
layout: post
date:   2017-02-10 15:22:16
title: "Tech Summit - Unpicking locks: using actors to simplify concurrency in desktop apps"
categories: talks c++ work
---

This is an outline of a talk entitled "Unpicking locks: using actors to simplify concurrency in desktop apps" that I gave at Adobe Tech Summit 2017 in San Jose.

<!--
Actor
- what is the actor pattern
- what is it good for
- what are the alternatives: NOT threading but arbitrary threading design
- concrete real world use cases
- real world implementations

Our use case
- Sabre
- characteristics of the problem
- - entities, coordination, concurrency on the desktop
- - vulcan messaging bridge

How it works
- messaging - boost signals
- actors - boost asio, locks, actor structures and ownership, tasks
- knotty issues - deadlocks, ordering parent and child operations

Improvements
- threading design

-->

What is the actor pattern?
==========================

<!-- 
Refs: 
http://doc.akka.io/docs/akka/snapshot/general/actor-systems.html#actor-systems
http://www.javaworld.com/article/2077999/java-concurrency/understanding-actor-concurrency--part-1--actors-in-erlang.html?page=2
https://en.wikipedia.org/wiki/Actor_model#Fundamental_concepts
-->

An actor is a software entity that can receive messages and when it does it can

- do some arbitrary processing,
- send new messages to other actors, and
- create other actors.

Actors encapsulate state and behaviour, in much the same way an object does in object oriented programming. However in addition they have messaging capabilites.
Messaging capabilities are analogous to member functions in object oriented programming, in that they enable mutating and accessing the state of the actor.
The messaging is asynchronous: actors have in incoming task queue that they take messages from in order. Sending actors do not have to wait for a response to their message, indeed there may be no response.

There can be many actors in a program and processing different actors' messages may be interleaved (concurrently) or run simultaneously (in parallel).

Actors make intuitive sense by thinking of them as people who are responsible for a job. You ask somebody to do a job, they handle it and may make further requests
in turn to get the job done. 

Actors should handle messages without blocking, so that the processing of later messages is not delayed. Messages should be immutable data, so that concurrency control
of messages is not required. 

Alternatives to actors
======================

<!--
Refs:
https://en.wikipedia.org/wiki/Message_queue
https://en.wikipedia.org/wiki/Publish–subscribe_pattern
https://en.wikipedia.org/wiki/Observer_pattern
https://msdn.microsoft.com/en-us/library/jj152938(v=vs.110).aspx
-->

Actors are build on top of concurrency primitives like threads, locks, futures, channels, continuations, etc. 

Using actors is an alternative to writing concurrent code in an ad-hoc way, by creating threads manually and having data structures with concurrency controls. I'm going to go into much more detail about what the ad-hoc concurrency in our codebase was like before we rearchitected with actors.

When you use actors you should have very few locks and threads in your code, ideally mainly in the library code that provides actors and messaging.

On a similar level of abstraction as actors are patterns such as 

- task processing/scheduling systems, 
- event based programming with callbacks, 
- observer, and
- publish-subscribe.

What is it good for?
====================

<!--
http://doc.akka.io/docs/akka/snapshot/general/actor-systems.html#actor-systems
http://www.javaworld.com/article/2077999/java-concurrency/understanding-actor-concurrency--part-1--actors-in-erlang.html?page=2
https://en.wikipedia.org/wiki/Actor_model#Current_issues
-->

The advantages of actors are that 

- State can be encapsulated without locking, since state should be private to the actor and messages are processed sequentially meaning that only one potentially mutating operation can happen at any time.
- They create natural concurrency that can take advantage of parallel processing for efficiency. Avoiding locking should also speed up processing.
- The model is intuitive.

The disadvantages are that

- Like all techniques where control flow is inverted, code using actors can be disjointed compared to other code. Examples:
-- Rather than one function that does X then Y, we may end 
up with a function doing X then sending a message and other function doing Y. 
- Blocking code is damaging because it holds up not only the processing of the current message but also subsequent messages. This might result in salami slicing 
of functionality into multiple steps that logically belong together.

Actors seem best suited to implementing coordination, concurrency and concurrency control aspects of a system. Other patterns like object orientation are better suited to implementing algorithms and data structures and business logic. I will give an example below of how we used actors only for the coordination problem in CoreSync.

Real world use cases and implementations
========================================

I will just give a couple of examples of actor implementations that I'm most familiar with. 

Erlang popularised actors in contemporary practice. If Java is object-oriented, then Erlang is process-oriented. Processes and messages are build into the language, processes are essentially actors. Erlang is used widely in industry for server side code, frequently for applications such a messaging, chat, telephony. For example Facebook Chat and Whatsapp run at huge scale and are implementing using Erlang. 

Akka is a Java toolkit specifically providing actors. It is included with the Play web framework. In Play actors are used for server side background processes, for
example in a chat system messages might be published by using a web API (not implemented using actors) and then a message would be propagated to recipients via actors using web sockets.

Our use case - Sabre
====================

Adobe's Creative Cloud product includes a file synchronisation service. The code was able to provide a single sync "job", i.e. sync relationship between a single local folder and a single remote folder. This was a limitation in the UI layer. Initially the code had its own UI which allowed a single sync job to be created. When the UI was removed, the UI code was repurposed to work over IPC instead, using an IPC protocol that supported a single sync job. There was/is no limitation in the file sync code preventing multiple jobs from running at once.

The Sabre project was intended to replace the old UI and IPC-related code with a new IPC protocol supporting multiple simultaneous sync jobs. This fits in with the actor pattern's strengths at coordination and concurrency as opposed to business logic, since the sync code is already written in an object oriented style and has its own internal threading and locking. The job of the new code is to 

- ensure authentication is available for users owning sync jobs;
- create, destroy and maintain the lifecycle of sync jobs;
- sending out updates of job status to interested listeners; and
- receive general application settings like setting an HTTP proxy.

In depth on Sabre
=================



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

How it works
- vulcan messaging bridge
- messaging - boost signals
- actors - boost asio, locks, actor structures and ownership, tasks
- knotty issues - deadlocks, ordering parent and child operations

Improvements
- threading design
- strongly typed notifications
- using more C++ concurrency primitives like lambda, task queue

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

Actors encapsulate state and behaviour, in much the same way an object does in object oriented programming. However in addition they have messaging capabilities.
Messaging capabilities are analogous to member functions in object oriented programming, in that they enable mutating and accessing the state of the actor.
The messaging is asynchronous: actors have in incoming task queue that they take messages from in order. Sending actors do not have to wait for a response to their message, indeed there may be no response.

There can be many actors in a program and processing different actors' messages may be interleaved (concurrently) or run simultaneously (in parallel).

Actors make intuitive sense by thinking of them as people who are responsible for a job. You ask somebody to do a job, they handle it and may make further requests
in turn to get the job done. 

Actors should handle messages without blocking, so that the processing of later messages is not delayed. Messages should be immutable data, so that concurrency control
of messages is not required. 

<img src="/resources/tech-summit-img/Screen Shot 2016-12-05 at 15.42.41.png" />

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

Akka is a Java toolkit specifically providing actors. It is included with the Play web framework. In Play, actors are used for server side background processes, for
example in a chat system messages might be published by using a web API (not implemented using actors) and then a message would be propagated to recipients via actors using web sockets.

Our use case - Sabre
====================

Adobe's Creative Cloud product includes a file synchronisation service. The code was able to provide a single sync "job", i.e. sync relationship between a single local folder and a single remote folder. This was a limitation in the UI layer. Initially the code had its own UI which allowed a single sync job to be created. When the UI was removed, the UI code was repurposed to work over IPC instead, using an IPC protocol that supported a single sync job. There was/is no limitation in the file sync code preventing multiple jobs from running at once.

The Sabre project was intended to replace the old UI and IPC-related code with a new IPC protocol supporting multiple simultaneous sync jobs. This fits in with the actor pattern's strengths at coordination and concurrency as opposed to business logic, since the sync code is already written in an object oriented style and has its own internal threading and locking. The job of the new code is to 

- ensure authentication is available for users owning sync jobs;
- create, destroy and maintain the lifecycle of sync jobs;
- sending out updates of job status to interested listeners; and
- receive general application settings like setting an HTTP proxy.

<img src="/resources/tech-summit-img/sync-against-multiple-endpoints.png" />

In depth on Sabre
=================

The sync program already had objects for users, authentication tokens, sync jobs, IPC messages and so on. What we need to do is to implement handlers for a new
IPC message protocol, to let jobs be created, enabled, disabled, queried, etc. For example the following message will create a new sync job:

{% highlight xml linenos %}
<?xml version="1.0" encoding="utf-8"?>
<message>
  <userid>blah@domain.com</userid>
  <cloudid>00000000-0000-0000-0000-000000000000</cloudid>
  <job>files</job>
  <localpath>/Users/username/CCfiles</localpath>
  <remotepath>/files</remotepath>
  <requestid>690382b3-95a5-4626-824c-3d17ea38a307</requestid>
  <type>sync.in.request.job.create</type>
  <version>1.1.4</version>
</message>
{% endhighlight %}

After this is received new data structures and threads are set up for the sync job, and the following response is sent later:

{% highlight xml linenos %}
<?xml version="1.0" encoding="utf-8"?>
<message>
  <userid>blah@domain.com</userid>
  <cloudid>00000000-0000-0000-0000-000000000000</cloudid>
  <job>files</job>
  <status>success</status>
  <requestid>690382b3-95a5-4626-824c-3d17ea38a307</requestid>
  <type>sync.out.response.job.create</type>
  <version>1.1.4</version>
</message>
{% endhighlight %}

We need to expect that these messages will be received at any time, perhaps several at once. Handling messages may take some time while HTTP requests are made, IO is performed and so on. Hence some responses will be delayed but meanwhile other work should proceed.

Our solution - components
=========================

We created a number of reusable components in C++:

- internal notification/messaging engine,
- actor,
- IPC controller, 
- IPC observers and observation bridge,
- task class, and
- timer class.

I'm going to describe the actor class, notification system and IPC controllers in a bit more detail.

There are also a set of components to handle our particular problem of spinning up sync jobs, I'm going to describe the 
app controller, user controller and job controller but leave out other details.

Our solution - notification centre
==================================

The notification centre is implemented on top of boost signals. A notification is an immutable key-value map, with a type and name. The type is used to group similar notifications.

Classes that are interested in particular types and/or names of notification will register with the NotificationEngine to receive them via callback function (boost::function).

When a notification is dispatched, all the registered listeners will be provided with a copy. Dispatching a notification to all registered listeners is synchronous for the dispatching caller. Hence listeners are encouraged not to take a long time.

The class API is:

{% highlight xml linenos %}
typedef boost::function<void(const Notification &event)> NotificationListener;

class NotificationEngine
{
public:
	virtual void dispatch(const Notification &notification) const = 0;

	virtual connection_t registerListener(const NotificationType &type,
		NotificationListener listener) = 0;

	virtual connection_t registerListener(const NotificationType &type,
		const NotificationName &name, NotificationListener listener) = 0;

};
{% endhighlight %}

Our solution - actor class
==========================

The actor class has the following abridged API:

{% highlight xml linenos %}
class Actor : private boost::noncopyable
{
public:
	Actor(jura::NotificationEngineWeakPtr notificationEngine,
		const jura::NotificationType& timerNotificationType);

	void stop();
    
	void post(const Functor& handler);

private:
	void runWorker();

private:
	jura::NotificationCenter _notificationCenter;
	boost::asio::io_service _io_service;
	boost::scoped_ptr< boost::asio::io_service::work > _work;
	boost::asio::io_service::strand _strand;
	boost::thread _worker;
	boost::uint32_t _hasStopped;
};
{% endhighlight %}

As soon as it is constructed, a dedicated thread is created for the actor (this is unnecessary, more detail later).
Work represented by a Functor object can be "posted" to the actor, i.e. get it to run a piece of code.

This Actor object does not capture with purity the concept of actor described above, because it runs arbitrary pieces of code rather than handling messages. In this sense it is more like a task queue.

The intention is that another class type RealActor will extend or contain Actor, making RealActor conform to the concept of an actor. When other code wants to send a message to a RealActor it will be dispatched via the notification centre. RealActor
will be a subscriber for this message type, and when it receives the message it will immediately post the work to be done to
its internal actor.

I think there is a simpler implementation of Actor possible, using a virtual method 

{% highlight C++ %}
void handle(const Notification &notification) = 0;
{% endhighlight %}

to process messages received. However I have presented the code as it currently exists. It would not be a difficult refactoring to achieve that.

Our solution - IPC bridge
=========================

I mentioned above that Sabre is handler for an IPC protocol. IPC messages can be converted to internal notifications and vice-versa. For that reason, we have an IPC bridge class to 

- convert incoming IPC into internal notifications, and
- convert internal notifications with a particular IPC type back into IPC.

Hence we have restricted IPC handling to a single class, making it quite easy to switch the underlying IPC transport provider or change the details of the IPC protocol.

Putting the reusable pieces together
====================================

Now I will present how we put the reusable notifications, actor and IPC bridge pieces together to create an asynchronous
protocol handler for the Sabre protocol.

The classes required were the app controller, user controller and job controller.

The following sequence diagram shows how IPC message and notifications are sent between components. I have left the notification engine out of the diagram for brevity. Instead of showing a message going to the engine and then to the recipient, it is shown as being sent directly.

<!--
https://bramp.github.io/js-sequence-diagrams/

participant Client
participant IPCBridge
participant AppController
participant SyncController
participant JobController
Client->IPCBridge: job.create IPC msg
IPCBridge->IPCBridge: convert to notification
IPCBridge->AppController: send job.create notification
AppController->AppController: does user's SyncController exist?
AppController->SyncController: create a new SyncController
AppController->SyncController: send job.create notification
SyncController->SyncController: queue internally
SyncController->SyncController: handle job.create
SyncController->SyncController: does JobController exist?
SyncController->JobController: create a new JobController
SyncController->JobController: send job.create notification
JobController->JobController: queue internally
JobController->JobController: handle job.create
JobController->JobController: set up sync job data structures
JobController->IPCBridge: send job.create response notification
IPCBridge->IPCBridge: convert to IPC message
IPCBridge->Client: send job.create IPC response msg
-->

<img src="/resources/tech-summit-img/Screen Shot 2016-12-01 at 14.44.08.png" />

Steps:

1. The client sends an IPC message of type job.create
2. The IPCBridge received it and converts it into a notification
3. The IPC bridge sends it to the AppController actor
4. The AppController makes sure that a SyncController actor exists for the user whose job it is
5. Since no such actor exists, it is created
6. The notification is forwarded to the new SyncController
7. The SyncController receives the notification and checks that a JobController actor exists for the job
8. It doesn't because the job is only just being created, so a new JobController actor is created
9. The notification is forwarded to the new JobController
10. The JobController will handle the message, by setting up a new sync job
11. The JobController builds a response as a notification and sends it to the IPCBridge
12. The IPCBridge converts it to IPC and sends it

It is noticable that notifications relating to a job are being touched by the AppController and SyncController. That is necessary
in the case of job creation because the JobController actor does not yet exist, it is created by its parent. It is also necessary for various other requests such as destroying a job, where the the JobController actor is destroyed. Hence we take this approach for all messages to make the processing uniform. The intermediate processing in quite light, so the loss of efficiency should be small.

Now imagine that users dispatch a number of mutating IPC requests, such as job.enable and job.disable for the same job. These have a big effect on the underlying job's data structures: they cause threads to be created or terminated, various state variables to be set, etc. Hence this is a case where careful concurrency control is required. The vaunted advantage of actors was that we could get concurrency controls without explicit locking, so how does this work. Let's show by way of another sequence diagram (assuming that the AppController and JobController already exist):

<!--
https://bramp.github.io/js-sequence-diagrams/

participant Client1
participant Client2
participant IPCBridge
participant JobController
Client1->IPCBridge: job.disable IPC msg
IPCBridge->IPCBridge: convert to notification
IPCBridge->AppController: send job.disable notification
JobController->JobController: queue internally
Client2->IPCBridge: job.enable IPC msg
IPCBridge->IPCBridge: convert to notification
IPCBridge->AppController: send job.enable notification
JobController->JobController: queue internally
JobController->JobController: handle job.disable
JobController->JobController: disable job: kill threads, set state, etc.
JobController->IPCBridge: send job.disable response notification
IPCBridge->IPCBridge: convert to IPC message
IPCBridge->Client1: send job.disable IPC response msg
JobController->JobController: handle job.enable
JobController->JobController: enable job: start threads, set state, etc.
JobController->IPCBridge: send job.enable response notification
IPCBridge->IPCBridge: convert to IPC message
IPCBridge->Client2: send job.enable IPC response msg
-->

<img src="/resources/tech-summit-img/Screen Shot 2016-12-01 at 15.33.58.png" />

The interesting part of this is that although the job.disable and job.enable messages come from different clients potentially simultaneously, they are serialised by the JobController actor. Hence although they mutate state in the job data structures without any locking that is completely safe, because only one can run at any time.

This is the main advantage of actors: by sharing state only via immutable notifications and by serialising access to state inside actors, you can avoid having to have explicit concurrency control in your code.

Knotty issues with actors
=========================

One thing that is important to bear in mind is that although each actor has its own thread, it is best to avoid blocking operations inside message handlers. This is simply to ensure that other later messages are processed in a timely fashion. 

[See restart handler for an issue?]

Improvements in our actors implementation
=========================================

The most obvious limitation in our current design is that each actor uses a thread. This is not necessary and it wastes 
reasonably expensive resources. It would be better to use a smaller pool of threads, but still ensure that tasks for a particular actor are serialised. However we have to be careful to choose the size of the threadpool to avoid deadlock. Deadlock could occur if actor A is waiting on actor B finishing something. This could happen in practice by actor A repeatedly sending a message to actor B asking if something is complete and repeatedly receiving a negative result. If there is only one thread then actor B has no chance to do the work, as actor A is hogging the sole thread. Ideally actor A would not block the thread but would asynchronously await the response, giving B a chance to do the work. However in principle actors can block, so it is simplest to ensure there are enough threads.

Another limitation is that notifications are untyped. They always consist of a key-value map. If notifications could be arbitrary types we would benefit from typechecking. 

A final limitation in our implementation is that we have not used C++'s new concurrency primitives like futures. We wrote our own because compiler support was not good at the time.

TODOs:
- review and improve
- write abstract
- submit to speaker portal

{% highlight xml linenos %}
void createHandler(const jura::Notification& request, 
				   jura::Notification& response) {
				   
	LOG_SCOPER_INFO("job", "Creating " << jobIdentifier());

	_job->m_settings.SetSignedInUser(_jobIdentifier.userId());
	_job->m_settings.SetLocalUrl(sLocalUrl);
	_job->m_settings.SetRemoteUrl(sRemoteUrl, sRemotePath);
	_job->m_settings.SetConnectedState(true);
    auto concurrentTransfers = _data.concurrentTransfers();
	
    sibcrux::CsSetupJob(_job->m_settings, *_job);
}
{% endhighlight %}
## Task Concurrency

Within this document, we adhere to the following definitions:

A _**Task**_ is a coherent, self-contained piece of work, typically represented by a distinct, encapsulated part of a program. In other words: A task refers to the abstract concept of some work to do. It does not refer to any specific technical realization to represent that work in a program.

_**Concurrency**_: Several tasks are executed concurrently, i.e., at the same time, in the sense that one task is started before another one is finished. Note that concurrent execution of tasks does not imply that tasks make progress simultaneously (concurrency does not require nor imply parallelism). 


### The Challenge

Trivial task concurrency occurs if tasks do not share any information or resources. Each task is executed on its own hardware, uses only local information and has exclusive, unrestricted access to all resources it uses.

In any other case, we need means for

* _Task Scheduling_, i.e., a way to pause/interrupt and resume tasks,
* _Inter Task Communication_, i.e., a possibility to use information produced by one task in another task,
* _Resource Protection_, i.e., a mechanism to ensure that usage of the same resources from different tasks cannot lead to loss of resource integrity.

We will focus on the ability to pause and resume tasks to enable task scheduling hereinafter.


### Pause and Resume

There is different ways to realize the ability to pause/interrupt and resume a task. For all of these, we need to be able to store the state of a task when it is paused and re-store the state before it is resumed. Storing the state of one task and re-storing the state of another one so that it can be resumed is often called a context switch.

There are essentially three dimensions along which we need to take a decision for realizing the ability to pause and resume tasks:

1) _Kernel-Space_ or _User-Space_ concurrency. Kernel-space uses mechanisms provided by the operating system to achieve concurrency. User-space uses mechanisms provided by the programming language. Kernel-space is typically the only option if parallelism (usage of multiple CPU cores) is a requirement. Otherwise, user-space mechanisms provide flexibility that can be used to reduce the resource overhead required for storing task states and for context switches.

2) _Stack-less_ or _Stack-ful_ task implementations. A stack-ful task has its own stack associated with it, which represents the task's state. A stack-less task shares the same stack with other tasks. It uses other ways to store its state, typically by explicitly or implicitly modeling a state machine. The actual state of this state machine may be stored on the stack or on the heap. Using a stack is maybe the most natural way to save the state of a program, but it comes with some overhead. While the size of a state machine's state representation in memory is typically known exactly at compile time, the maximum possible stack size is often difficult to estimate. Consequently, stack-ful tasks will often have higher memory usage. Kernel-space concurrency typically implies stack-ful tasks.

3) _Cooperative_ or _Preemptive_ scheduling. In preemptive scheduling, the scheduling sub-system decides when to interrupt (preempt) a task, without the task being aware of it. In cooperative scheduling, in contrast, the task itself decides when to yield control back to the scheduling sub-system. Advantages of cooperative scheduling include that it often allows easier inter-task communication and resource protection, because the task can decide to only yield control when resources are in a consistent state, ready to be used by others. Disadvantages of cooperative scheduling include that one task may (erroneously or maliciously) starve all other tasks and, very often, some aspects of concurrency are mixed with the code to implement the tasks logic (see, e.g., [colored functions](https://journal.stuffwithstuff.com/2015/02/01/what-color-is-your-function/)). Kernel-space concurrency often implies preemptive scheduling.

And we can distinguish styles to implement concurrency along two major dimensions

1) _Sequential_ or _Non-Sequential_. In a sequential implementation, we write code, well, sequentially: "Do this, then that, then ...". There might of course be all kind of control flows, that allow for choosing different branches of execution, but there will typically be one main function or routine modeling the task. Once the function returns, the task is finished. Non-sequential implementations are basically state machines. While a task's state is represented by a _program counter_ (where are we in the execution of the main function) and local variables in sequential code, the state is explicitly modeled in non-sequential implementations. There are typically many small functions (often callbacks) that modify this state based on external triggers. Sequential code may be easier to read, specifically if the control flow is mostly linear. Non-sequential state machines may be more explicit in some cases.

2) _Blocking_ or _Non-Blocking/Event-Driven_. When a blocking implementation requires a result of a long-running computation or some external resource, it waits in a blocked state until that result or resource is available. A non-blocking, event-driven implementation triggers a request and then yields control back to the scheduling sub-system waiting to resume once a result is available. Sometimes blocking is called synchronous and non-blocking is called asynchronous. Event-driven implementations often match well with non-sequential code. Blocking implementations often lead to more complex task synchronization issues (inter task communication, resource protection)


### Solutions

Typical solutions include

| Solution          | Level        | Save State | Interrupt   | Code           | Interfaces   |
| ----------------- | ------------ | ---------- | ----------- | -------------- | ------------ |
| OS/Native Threads | Kernel-Space | Stack-ful  | Preemptive  | Sequential     | Blocking     | 
| Green Threads     | User-Space   | Stack-ful  | Preemptive  | Sequential     | Blocking     |
| Co-routines       | User-Space   | Stack-less | Cooperative | Sequential     | Non-blocking |
| State-machines    | User-Space   | Stack-less | Cooperative | Non-Sequential | Non-blocking |

In this picture, _co-routines_ are in between threads and state-machines. They allow to apply a sequential coding style for better readability combined with event-driven, non-blocking interfaces for low synchronization overhead and high throughput.

While there is no generally accepted definition of the term co-routine, this is exactly the definition I find helpful and adhere to: A _co-routine_ is a user-space, cooperative task (typically stack-less), which is intended to realize sequential non-blocking implementations of concurrency.


### References

* [What color is your function. _Bob Nystrom, 2015_](https://journal.stuffwithstuff.com/2015/02/01/what-color-is-your-function/)
* [Why async Rust? _Without Boats, 2023_](https://without.boats/blog/why-async-rust/)


## (C++) Co-Routines Toy Example

The toy example contains three implementation of a `Lights` class. The user of the light class will repeatedly call the `processInput` method, which will cause the implementation to advance a state machine and print some messages to standard out.

* `StateMachineLights` has an explicit, non-sequential implementation of the state machine, that is executed synchronously in the caller context. 
* `CoRoutineLights` has a sequential, non-blocking implementation in a `run()` method, that is implicitly compiled into a state machine (whose state is stored in the heap). Whenever the implementation attempts to read an input and no input is available yet, it will yield control and resume once an input is available. The implementation comes with quite a bit of boiler plate required for the definition of types for a return object (`CoRoutineLights::Task`) and an awaiter/awaitable object (`CoRoutineLights::Input`).
* `ThreadLights` spawns a thread which executes the task in a sequential, blocking style in a `run()` method. The `run()` method is very similar to the implementation in `CoRoutineLights`, with two differences: The thread based implementation has some extra code to terminate the thread (implemented by throwing `Interrupted`), and the non-blocking `co_await m_input` statements in the co-routine based implementation are replaced by blocking calls to `get()`. The latter change seems small, but it results in the need for resource protection (implemented using a mutex), which was not needed in the other two implementations.

There is no real concurrency in this toy example. But that could be added easily by having several instances of the Lights class used in parallel, with possibly the input of one instance depending on the output of another instance (to simulate inter-task communication). No additional resource protection mechanisms would be required for this, as long as we use a single thread to drive all the instances.

### References

* [Coroutines. _cppreference.com, C++20_](https://en.cppreference.com/w/cpp/language/coroutines)
* [My tutorial and take on C++20 coroutines. _David Mazières, 2021_](https://www.scs.stanford.edu/~dm/blog/c++-coroutines.html), including example generator implementation
* [C++ Coroutines: Understanding operator co_await. _Lewis Baker, 2017_](https://lewissbaker.github.io/2017/11/17/understanding-operator-co-await), with example for simple thread synchronization
* [C++20 Coroutines — Complete Guide. _Šimon Tóth, 2021_](https://itnext.io/c-20-coroutines-complete-guide-7c3fc08db89d)

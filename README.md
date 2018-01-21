# C containers and miscellaneous utility

A collection of code, mainly intrusive containers, to facilitate C coding.\
Most of it originates from my tests to find an optimal priority queue for my
experimental 32-bit operating system kernel, to manage thread and event queues.

To provide compile-time polymorphism, this library is arranged as "poor man's
templates", with C preprocessor macros to be used to expand concrete headers
to include in your program and implementations to link with your program.\
This allows customization without run-time overhead (for example, due to
function pointer callbacks).

## Containers
The following containers are included. They are mostly **intrusive** containers,
that is your elements are responsible to include the book-keeping to add
themselves to the container.\
This allows to use such containers in environments where dynamically allocating
memory is inconvenient or impossible, such as in operating system kernels.
* **AvlTree**: intrusive self-balancing binary tree with very good insertion
  performance and, perhaps counterintuitively, even better removal performance,
  especially if elements come partially sorted.
* **BinaryHeap**: a semi-intrusive binary heap (that is, you must allocate nodes
  separately from elements, but elements must be aware of nodes). Provides
  quasi-constant time insertion (better than balanced trees) and logarithmic
  removal (worse than balanced trees).
* **IntrusiveBinaryHeap**: the intrusive version of the binary heap, where
  elements can embed hooks directly with a little performance hit.
* **LeftistHeap**: strongly unbalanced binary heap that exhibits similar
  logarithmic performance for both insertion and removal, midway between normal
  binary heaps and balanced trees. Moreover, it allows efficient merge of two
  heaps.
* **LimitedPriorityQueue**: unbeatable constant time insertions and removals,
  but with a fixed and limited count of distinct priorities and rather big space
  needs. Uses a linked list for each priority level.
* **NaryTrie**: intrusive n-ary bitwise trie, a.k.a. prefix tree with binary
  numbers as keys. The depth of the tree is proportional to the number of bits
  of the key. Can perform considerably better than balanced trees, and for a
  limited range of keys is only slightly worse than LimitedPriorityQueue
  without its space requirements.
* **OrderedListPriorityQueue**: a naive O(n) implementation of a priority queue
  based on an ordered doubly linked list. This is here only to provide a
  baseline, as tests indicate it is not to be preferred even
  for very small count of elements.
* **RedBlackTree**: intrusive version of what is usually considered
  "the" general purpose self-balancing binary tree. Red-black trees are usually
  regarded as more efficient than AVL trees, although in my tests AVL trees
  usually outperform red-black trees, but could just be my implementations.
  Moreover, I find the mechanics of AVL trees much clearer.
* **UnorderedListPriorityQueue**: a naive O(n) implementation of a priority
  queue based on an unordered doubly linked list. This is here only to provide
  a baseline, and in my tests it is even worse than the ordered list version.

## Miscellaneous utility
* **tscStopWatch**: functions to measure elapsed time using the x86 timestamp
  counter (TSC), with proper serialization to account for instruction reordering
  performed by the CPU.

## Repository structure
The **include** directory contains header files for utility functions and the
"poor man's templates". The latter cannot be used directly but must be
instantiated using the provided macros to create your container.

The **src** directory contains files that must be linked to your program
in order to use the provided functionality. This is common code not dependent
of container instantiations.

The **tests** directory contains code for testing and benchmarking.
They also shows how to use functionality of this library.

The **Benchmarks.ods** file in the root of the repository is a spreadsheet
with the results of the above benchmarks on my vanilla i5-3570K computer using
gcc 7.2 on Ubuntu 17.10 targeting 32-bit execution.\
Numbers represent TSC ticks, that on Ivy Bridge are normalized clock cycles
(at 3.4 GHz on my CPU) not affected by fluctuations of the real clock
or power management.

## Licensing terms
Permissive, two-clause BSD license. See licensing headers on each file.
Thread-Safe High Performance Allocator

In this assignment, you'll make a high-performance thread-safe allocator.

You can treat code that you or your teammate submitted for HW07 as starter code for this assignment. Once you've seen your partner's solution for HW07 you shouldn't make additional HW07 submissions.

Test Programs
Provided with the assignment download are two test programs:

ivec_main - Collatz Conjecture simulation with dynamic array
list_main - Collatz Conjecture simulation with linked list
A Makefile is included that links these two programs with each of three different memory allocators: the system allocator and two allocators you will need to provide.

Task #1 - Adapt HW07 to be Thread Safe
Select one of the hmalloc implementations submitted by either partner for HW07 and update it as follows:

Make it thread safe by adding a single mutex guarding your free list. There should be no data races when calling hmalloc/hfree functions concurrently from multiple threads.
Implement realloc.
Edit hw07_malloc.c in the CH02 starter code to either include or call your updated hmalloc.
Task #2 - Implement an optimized memory allocator
Edit par_malloc.c to either implement or call an optimized concurrent memory allocator. This allocator should run the test cases as quickly as possible - definitely faster than hw07_malloc, and optimally faster than sys_malloc.

The two things that slow down hw07_malloc are 1.) lock contention and 2.) algorithmic complexity. Your optimized allocator should try to reduce these issues as much as possible.

Task #3 - Graph & Report
Time all six versions of the program ({sys, hw7, par} X {ivec, list}). Select input values so that you can distinguish the speed differences between the versions.

Include a graph.png showing your results.

Include a report.txt with:

ASCII art table of results
Information on your test hardware / OS.
Discussion of your strategy for creating a fast allocator.
Discussion of your results

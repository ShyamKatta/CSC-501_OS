# CSC-501_OS
**Kernel level programming, Implemented -> Demand Paging, reader writer locks, priority Inheretence, Linux like scheduler**

## PA0 - 
The objective of this introductory lab is to familiarize you with the process of compiling and running XINU, the tools involved, and the run-time environment and segment layout. 

## PA1 (Linux like Scheduler) - 
The objectives of this lab are to get familiar with the concepts of process management like process priorities, scheduling and context switching. 
Implemnted - 1) Random Scheduler 2) Linux-like Scheduler (based loosely on the 2.2 Linux kernel)

## PA2 (Reader Writer Locks, Priority Inheretence) - 
You are going to implement readers/writer locks as described in this handout. Additionally, you will implement a priority inheritance mechanism to prevent the priority inversion problem when using locks. 
Implemented 1-
 (1) Lock Deletion 
 (2) Locking Policy 
 (3) Wait on Locks with Priority 
 (4) Releasing Locks 
 (5) Using Variable Arguments 
 2 - Priority Inheritance 

## PA3 (Demand Paging) - 
Demand paging is a method of mapping a large address space into a relatively small amount of physical memory. It allows a program to use an address space that is larger than the physical memory, and access non-contiguous sections of the physical memory in a contiguous way. Demand paging is accomplished by using a "backing store" (usually disk) to hold pages of memory that are not currently in use.

From this point on, only the details of this project are discussed. It is assumed that you have read the Intel documents and are comfortable with paging concepts and the Intel specific details.

The goal of this project is to implement the following system calls and their supporting infrastructure.
* System Calls
```
SYSCALL xmmap (int virtpage, bsd_t source, int npages)
```
Much like its Unix counterpart (see man mmap), it maps a source file ("backing store" here) of size npages pages to the virtual page virtpage. A process may call this multiple times to map data structures, code, etc.
```
SYSCALL xmunmap (int virtpage)
```
This call, like munmap, should remove a virtual memory mapping. See man munmap for the details of the Unix call.
```
SYSCALL vcreate (int *procaddr, int ssize, int hsize, int priority, char *name, int nargs, long args)
```
This call will create a new Xinu process. The difference from create() is that the process' heap will be private and exist in its virtual memory.
The size of the heap (in number of pages) is specified by the user through hsize.

create() should be left (mostly) unmodified. Processes created with create() should not have a private heap, but should still be able to use xmmap().
```
WORD *vgetmem (int nbytes)
```
Much like getmem(), vgetmem() will allocate the desired amount of memory if possible. The difference is that vgetmem() will get the memory from a process' private heap located in virtual memory. getmem() still allocates memory from the regular Xinu kernel heap.
```
SYSCALL srpolicy (int policy)
```
This function will be used to set the page replacement policy to Second-Chance (SC) or Least-Frequently-Used (LFU). You can declare constant SC as 3 and LFU as 4 for this purpose.
```
SYSCALL vfreemem (block_ptr, int size_in_bytes)
```
You will implement a corresponding vfreemem() for vgetmem() call. vfreemem() takes two parameters and returns OK or SYSERR. The two parameters are similar to those of the original freemem() in Xinu. The type of the first parameter block_ptr depends on your own implementation. 

* Backing Store Emulation
In the implementation, you need to "steal" physical memory frames 2048 - 4095 (take a close look at sys/i386.c, and pay attention to the variables npages and maxaddr). As a result, this portion of memory will not be used for other purposes. You can assume that our grading program will not modify this part of memory. 
 
* The Interrupt Service Routine (ISR)
 As you know, a page fault triggers an interrupt 14. When an interrupt occurs the machine pushes CS:IP and then an error code (see Intel Volume III chapter 5) 
 
* Faults and Replacement Policies

* Page Replacement Policies

Detailed description in Project page

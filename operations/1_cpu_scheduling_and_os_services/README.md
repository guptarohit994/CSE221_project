# CSE221_project

## CPU, Scheduling, and OS Services
Measurement overhead: Report the overhead of reading time, and report the overhead of using a loop to measure many iterations of an operation.
Procedure call overhead: Report as a function of number of integer arguments from 0-7. What is the increment overhead of an argument?
System call overhead: Report the cost of a minimal system call. How does it compare to the cost of a procedure call? Note that some operating systems will cache the results of some system calls (e.g., idempotent system calls like getpid), so only the first call by a process will actually trap into the OS.
Task creation time: Report the time to create and run both a process and a kernel thread (kernel threads run at user-level, but they are created and managed by the OS; e.g., pthread_create on modern Linux will create a kernel-managed thread). How do they compare?
Context switch time: Report the time to context switch from one process to another, and from one kernel thread to another. How do they compare? In the past students have found using blocking pipes to be useful for forcing context switches.
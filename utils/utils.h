#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <sched.h>
#include <assert.h>
#include <sys/resource.h>

// defines from measurement_overhead
#define READING_TIME_OVERHEAD 32
// per iteration
#define LOOP_OVERHEAD 6

//verbosity levels
#define LOW 1
#define MEDIUM 2
#define FULL 3
#define CVERBOSE LOW

//quick memory denominations
#define KB 1024ULL
#define MB 1024ULL*1024ULL
#define GB 1024ULL*1024ULL*1024ULL
#define BLOCKSIZE 4ULL*KB

// string1 = int1, string2 = int2, string3 = int3\n
#define cnprintfsisisi(lvl,caller,str1,int1,str2,int2,str3,int3) \
		((CVERBOSE>=lvl) ? fprintf(stdout, "%s: %s = %d, %s = %d, %s = %d\n",caller,str1,int1,str2,int2,str3,int3) : 0);

// string1 = int1, string2 = int2\n
#define cnprintfsisi(lvl,caller,str1,int1,str2,int2) \
		((CVERBOSE>=lvl) ? fprintf(stdout, "%s: %s = %d, %s = %d\n",caller,str1,int1,str2,int2) : 0);

//string1 = int1
#define cnprintfsi(lvl,caller,str,arg) ((CVERBOSE>=lvl) ? fprintf(stdout, "%s: %s = %d\n",caller,str,arg) : 0);

// string1 = uint64_t
#define cnprintfsui64(lvl,caller,str,arg) ((CVERBOSE>=lvl) ? fprintf(stdout, "%s: %s = %llu\n",caller,str,arg) : 0);

// string
#define cnprintf(lvl,caller,str) ((CVERBOSE>=lvl) ? fprintf(stdout, "%s: %s\n",caller,str) : 0);

// string1 = 
#define cnprintfa(lvl,caller,str,arg) ((CVERBOSE>=lvl) ? fprintf(stdout, "%s: %s = %.2lf\n",caller,str,arg) : 0);

//custom print with two arguments (double)
#define cnprintfaa(lvl,caller,str,arg1,arg2) ((CVERBOSE>=lvl) ? fprintf(stdout, "%s: %s = %.2lf:%.2lf\n",caller,str,arg1,arg2) : 0);
//custom print with first argument (integer) and second argument double
#define cnprintfaia(lvl,caller,str,arg1,arg2) ((CVERBOSE>=lvl) ? fprintf(stdout, "%s: %s = %d:%.2lf\n",caller,str,arg1,arg2) : 0);

#define cprintf(lvl,caller,str) ((CVERBOSE>=lvl) ? fprintf(stdout, "%s",str) : 0);

#define cprintfa(lvl,caller,str,arg) ((CVERBOSE>=lvl) ? fprintf(stdout, "%s %.2lf",str,arg) : 0);

#define cprintfaia(lvl,caller,str,arg1, arg2) ((CVERBOSE>=lvl) ? fprintf(stdout, "%s %d(%.2lf)",str,arg1, arg2) : 0);
//custom print with an argument (integer)
#define cprintfai(lvl,caller,str,arg) ((CVERBOSE>=lvl) ? fprintf(stdout, "%s %d",str,arg) : 0);
//custom print with two arguments (integer)
#define cnprintfaai(lvl,caller,str,arg1,arg2) ((CVERBOSE>=lvl) ? fprintf(stdout, "%s: %s = %d:%d\n",caller,str,arg1,arg2) : 0);

#define TRUE 1
#define FALSE 0

#define handle_error_en(en, msg) \
       do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
       do { perror(msg); exit(EXIT_FAILURE); } while (0)


/* sets the priority of the current process to the specified priority
   Needs to run as superuser to increase priority
   priority can be from [-20,20]
*/
void set_nice(int new_priority) {
	int prior_priority, later_priority;
	int ret;
	pid_t pid;

	pid = getpid();
	prior_priority = getpriority(PRIO_PROCESS,pid);

	ret = nice(new_priority);

	if (ret != new_priority)
		handle_error("set_nice");

	later_priority = getpriority(PRIO_PROCESS,pid);

	cnprintfsisisi(LOW, "set_nice","pid", pid, "old_priority", prior_priority, "new_priority", later_priority);

}

/* Timer macro
   - Usage:
    Timer struct timer;

    Usage:
    struct Timer timer;
    tic(timer);
    // code to measure
    toc(timer);

    uint64_t cycles_taken = timer_diff(timer);
*/
struct Timer {
    uint32_t cycles_high0, cycles_low0, cycles_high1, cycles_low1;
};

/* asm code snippet for reading cycles
   ref: http://cseweb.ucsd.edu/classes/fa18/cse221-a/timing.html
*/
#define tic(timer) \
    asm volatile ("cpuid\n\t"   \
          "rdtsc\n\t"           \
          "mov %%edx, %0\n\t"   \
          "mov %%eax, %1\n\t"   \
          : "=r" (timer.cycles_high0), "=r" (timer.cycles_low0)  \
          :: "%rax", "%rbx", "%rcx", "%rdx")

#define toc(timer) \
    asm volatile ("rdtscp\n\t"   \
          "mov %%edx, %0\n\t"    \
          "mov %%eax, %1\n\t"    \
          "cpuid\n\t"            \
          : "=r" (timer.cycles_high1), "=r" (timer.cycles_low1)  \
          :: "%rax", "%rbx", "%rcx", "%rdx");

#define combine_cycles(hi, lo) (((uint64_t)hi << 32) | lo)

#define timer_diff(timer) \
    (combine_cycles(timer.cycles_high1, timer.cycles_low1) - \
     combine_cycles(timer.cycles_high0, timer.cycles_low0))

#endif //COMMON_UTILS_H
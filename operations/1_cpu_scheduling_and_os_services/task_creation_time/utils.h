#ifndef TASK_CREATION_UTILS_H
#define TASK_CREATION_UTILS_H

#include "../../../utils/utils.h"

struct Timer {
    uint32_t cycles_high0, cycles_low0, cycles_high1, cycles_low1;
};

/* asm code snippet for reading cycles
   ref: http://cseweb.ucsd.edu/classes/fa18/cse221-a/timing.html

   Usage:
    struct Timer timer;
    tic(timer);
    // code to measure
    toc(timer);

    uint64_t cycles_taken = timer_diff(timer);
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

#endif //TASK_CREATION_UTILS_H
#include <stdint.h>
#include "../../../utils/utils.h"
#include <inttypes.h>
#include <time.h>

void call0()
{
}


int main () {

	cnprintf(LOW, "main", "\n\n");
  	cnprintf(LOW, "main", "***************** SYSTEM_CALL *****************");
  	set_nice(-20);

	uint32_t cycles_high0, cycles_low0;
	uint32_t cycles_high1, cycles_low1;
	uint64_t cycles_before, cycles_after, cycles_taken;

	asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

	call0();

    asm volatile ("rdtscp\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  "cpuid\n\t"
		  : "=r" (cycles_high1), "=r" (cycles_low1)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

    cycles_before = ((uint64_t)cycles_high0 << 32) | cycles_low0;
    cycles_after = ((uint64_t)cycles_high1 << 32) | cycles_low1;

    assert(cycles_before <= cycles_after);

    cycles_taken = cycles_after - cycles_before;

    cnprintf(LOW, "main", "\n\n");
    cnprintf(LOW, "main", "***************** RESULT USER PROCESS*****************");
    printf("%" PRIu64 "\n", cycles_taken);


    asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

	time(NULL);

    asm volatile ("rdtscp\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  "cpuid\n\t"
		  : "=r" (cycles_high1), "=r" (cycles_low1)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

    cycles_before = ((uint64_t)cycles_high0 << 32) | cycles_low0;
    cycles_after = ((uint64_t)cycles_high1 << 32) | cycles_low1;

    assert(cycles_before <= cycles_after);

    cycles_taken = cycles_after - cycles_before;

    cnprintf(LOW, "main", "\n\n");
    cnprintf(LOW, "main", "***************** RESULT SYSTEM PROCESS*****************");
    printf("%" PRIu64 "\n", cycles_taken);

}

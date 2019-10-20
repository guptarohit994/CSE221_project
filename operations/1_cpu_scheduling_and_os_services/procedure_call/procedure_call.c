#include "stdint.h"
#include "../../../utils/utils.h"
#include <inttypes.h>


void call0()
{
}

void call1(int n1)
{
}

void call2(int n1, int n2)
{
}

void call3(int n1, int n2, int n3)
{
}

void call4(int n1, int n2, int n3, int n4)
{
}

void call5(int n1, int n2, int n3, int n4, int n5)
{
}

void call6(int n1, int n2, int n3, int n4, int n5, int n6)
{
}

void call7(int n1, int n2, int n3, int n4, int n5, int n6, int n7)
{
}

int main () {

	cnprintf(LOW, "main", "\n\n");
  	cnprintf(LOW, "main", "***************** PROCEDURE_CALL_OVERHEAD *****************");
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
    cnprintf(LOW, "main", "***************** RESULT 0*****************");
    printf("%" PRIu64 "\n", cycles_taken);

asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

	call1(1);

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
    cnprintf(LOW, "main", "***************** RESULT 1*****************");
    printf("%" PRIu64 "\n", cycles_taken);

asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

	call2(1,2);

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
    cnprintf(LOW, "main", "***************** RESULT 2*****************");
    printf("%" PRIu64 "\n", cycles_taken);

asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

	call3(1,2,3);

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
    cnprintf(LOW, "main", "***************** RESULT 3*****************");
    printf("%" PRIu64 "\n", cycles_taken);

asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

	call4(1,2,3,4);

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
    cnprintf(LOW, "main", "***************** RESULT 4*****************");
    printf("%" PRIu64 "\n", cycles_taken);

asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

	call5(1,2,3,4,5);

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
    cnprintf(LOW, "main", "***************** RESULT 5*****************");
    printf("%" PRIu64 "\n", cycles_taken);

asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

	call6(1,2,3,4,5,6);

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
    cnprintf(LOW, "main", "***************** RESULT 6*****************");
    printf("%" PRIu64 "\n", cycles_taken);

asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

	call7(1,2,3,4,5,6,7);

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
    cnprintf(LOW, "main", "***************** RESULT 7*****************");
    printf("%" PRIu64 "\n", cycles_taken);
    //cnprintfsui64(LOW, "main", "(per iteration) cycles_taken", cycles_taken);
}

#include "stdint.h"
#include "../../../utils/utils.h"
#include <inttypes.h>

#define NUM_ITERATIONS 100000

struct Timer timer;

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

	uint64_t cycles_taken;
	int i = 0;

	tic(timer);

    for (; i < NUM_ITERATIONS; ++i) {
	   call0();
    } 

    toc(timer);

    cycles_taken = timer_diff(timer)/NUM_ITERATIONS;

    cnprintf(LOW, "main", "\n\n");
    cnprintf(LOW, "main", "***************** RESULT 0*****************");
    cnprintfsi(LOW, "main", "iterations", NUM_ITERATIONS);
    cnprintfsui64(LOW, "main", "(per iteration) args:0 cycles_taken", cycles_taken );

    /* ******************************************************************************** */
    i = 0;
    tic(timer);
    
    for (; i < NUM_ITERATIONS; ++i)
	   call1(1);

    toc(timer);

    cycles_taken = timer_diff(timer)/NUM_ITERATIONS;

    cnprintf(LOW, "main", "\n\n");
    cnprintf(LOW, "main", "***************** RESULT 1*****************");
    cnprintfsi(LOW, "main", "iterations", NUM_ITERATIONS);
    cnprintfsui64(LOW, "main", "(per iteration) args:1 cycles_taken", cycles_taken );

    /* ******************************************************************************** */
    i = 0;
    tic(timer);
    
    for (; i < NUM_ITERATIONS; ++i)
	   call2(1,2);

    toc(timer);

    cycles_taken = timer_diff(timer)/NUM_ITERATIONS;

    cnprintf(LOW, "main", "\n\n");
    cnprintf(LOW, "main", "***************** RESULT 2 *****************");
    cnprintfsi(LOW, "main", "iterations", NUM_ITERATIONS);
    cnprintfsui64(LOW, "main", "(per iteration) args:2 cycles_taken", cycles_taken );

    /* ******************************************************************************** */
    i = 0;
    tic(timer);

	for (; i < NUM_ITERATIONS; ++i) call3(1,2,3);

    toc(timer);

    cycles_taken = timer_diff(timer)/NUM_ITERATIONS;

    cnprintf(LOW, "main", "\n\n");
    cnprintf(LOW, "main", "***************** RESULT 3 *****************");
    cnprintfsi(LOW, "main", "iterations", NUM_ITERATIONS);
    cnprintfsui64(LOW, "main", "(per iteration) args:3 cycles_taken", cycles_taken);

    /* ******************************************************************************** */
    i = 0;
    tic(timer);

	for (; i < NUM_ITERATIONS; ++i)
		call4(1,2,3,4);

    toc(timer);

    cycles_taken = timer_diff(timer)/NUM_ITERATIONS;

    cnprintf(LOW, "main", "\n\n");
    cnprintf(LOW, "main", "***************** RESULT 4 *****************");
    cnprintfsi(LOW, "main", "iterations", NUM_ITERATIONS);
    cnprintfsui64(LOW, "main", "(per iteration) args:4 cycles_taken", cycles_taken);

    /* ******************************************************************************** */
    i = 0;
    tic(timer);

	for (; i < NUM_ITERATIONS; ++i)
		call5(1,2,3,4,5);

    toc(timer);

    cycles_taken = timer_diff(timer)/NUM_ITERATIONS;

    cnprintf(LOW, "main", "\n\n");
    cnprintf(LOW, "main", "***************** RESULT 5 *****************");
    cnprintfsi(LOW, "main", "iterations", NUM_ITERATIONS);
    cnprintfsui64(LOW, "main", "(per iteration) args:5 cycles_taken", cycles_taken );

    /* ******************************************************************************** */
    i = 0;
    tic(timer);

	for (; i < NUM_ITERATIONS; ++i)
		call6(1,2,3,4,5,6);

    toc(timer);

    cycles_taken = timer_diff(timer)/NUM_ITERATIONS;

    cnprintf(LOW, "main", "\n\n");
    cnprintf(LOW, "main", "***************** RESULT 6 *****************");
    cnprintfsi(LOW, "main", "iterations", NUM_ITERATIONS);
    cnprintfsui64(LOW, "main", "(per iteration) args:6 cycles_taken", cycles_taken );

    /* ******************************************************************************** */
    i = 0;
    tic(timer);

	for (; i < NUM_ITERATIONS; ++i) 
		call7(1,2,3,4,5,6,7);

    toc(timer);

    cycles_taken = timer_diff(timer)/NUM_ITERATIONS;

    cnprintf(LOW, "main", "\n\n");
    cnprintf(LOW, "main", "***************** RESULT 7 *****************");
    cnprintfsi(LOW, "main", "iterations", NUM_ITERATIONS);
    cnprintfsui64(LOW, "main", "(per iteration) args:7 cycles_taken", cycles_taken );

}

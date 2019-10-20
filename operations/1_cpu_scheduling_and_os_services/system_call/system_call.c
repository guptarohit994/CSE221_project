#include <stdint.h>
#include "../../../utils/utils.h"
#include <inttypes.h>
#include <time.h>
#define NUM_ITERATIONS 100000

// void call0()
// {
// }


int main () {

	cnprintf(LOW, "main", "\n\n");
  	cnprintf(LOW, "main", "***************** SYSTEM_CALL *****************");
  	set_nice(-20);

	uint64_t cycles_before, cycles_after, cycles_taken;
    int i = 0;
    struct Timer timer;

	// tic(timer);

	// for (; i<NUM_ITERATIONS; i++)
	//     call0();

 //    toc(timer);

 //    cycles_taken = timer_diff(timer)/NUM_ITERATIONS;

 //    cnprintf(LOW, "main", "\n\n");
 //    cnprintf(LOW, "main", "***************** RESULT USER PROCESS*****************");
 //    printf("%" PRIu64 "\n", cycles_taken);

    i = 0;

    tic(timer);

	for (; i<NUM_ITERATIONS; i++)
        time(NULL);

    toc(timer);

    //per iteration
    cycles_taken = timer_diff(timer) / NUM_ITERATIONS;

    cnprintf(LOW, "main", "\n\n");
    cnprintf(LOW, "main", "***************** RESULT SYSTEM PROCESS*****************");
    cnprintfsi(LOW, "main", "iterations", NUM_ITERATIONS);
    cnprintfsui64(LOW, "main", "(per iteration minus loop_overhead) cycles_taken", cycles_taken - LOOP_OVERHEAD);

}

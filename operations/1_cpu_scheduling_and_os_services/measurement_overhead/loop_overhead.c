#include "../../../utils/utils.h"
#define NUM_ITERATIONS 1000

int main () {

	cnprintf(LOW, "main", "\n\n");
  	cnprintf(LOW, "main", "***************** LOOP_OVERHEAD *****************");
  	set_nice(-20);

    struct Timer timer;
    tic(timer);

    for (int i = 0; i < NUM_ITERATIONS; ++i);

    toc(timer);
    uint64_t cycles_taken = timer_diff(timer) / NUM_ITERATIONS;

    cnprintf(LOW, "main", "\n\n");
    cnprintf(LOW, "main", "***************** RESULT *****************");
    cnprintfsui64(LOW, "main", "(per iteration) cycles_taken", cycles_taken);
}
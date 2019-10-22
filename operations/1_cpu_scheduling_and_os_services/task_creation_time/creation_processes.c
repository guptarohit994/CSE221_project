#include "../../../utils/utils.h"
#include <pthread.h>
#include <sys/wait.h>
#define NUM_ITERATIONS 1000

int main() {
    cnprintf(LOW, "main", "\n\n");
    cnprintf(LOW, "main", "***************** CREATION_PROCESSES *****************");
    set_nice(-20);

    pid_t pid;
    struct Timer timer;

    tic(timer);

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        if ((pid = fork()) == -1) {
            handle_error("fork");
        } else if (pid == 0) {
            // child, exit
            exit(-1);
        } else {
            wait(NULL);
        }
    }

    toc(timer);

    uint64_t cycles_taken = timer_diff(timer) / NUM_ITERATIONS;

    cnprintf(LOW, "main", "\n\n");
    cnprintf(LOW, "main", "***************** RESULT *****************");
    cnprintfsui64(LOW, "main", "(per iteration) cycles_taken", cycles_taken);
}

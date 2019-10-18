#include "../../../utils/utils.h"
#include <pthread.h>
#define NUM_ITERATIONS 100

void * thread_start(void *arg) {
    pthread_exit(0);
}

int main() {
    cnprintf(LOW, "main", "\n\n");
    cnprintf(LOW, "main", "***************** CREATION_KTHREADS *****************");
    set_nice(-20);

    struct Timer timer;
    pthread_t thread_id;

    tic(timer);

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        pthread_create(&thread_id, NULL, thread_start, NULL);
        // should I include pthread_join here?
    }

    toc(timer);

    uint64_t cycles_taken = timer_diff(timer) / NUM_ITERATIONS;

    cnprintf(LOW, "main", "\n\n");
    cnprintf(LOW, "main", "***************** RESULT *****************");
    cnprintfsui64(LOW, "main", "(per iteration) cycles_taken", cycles_taken);
}

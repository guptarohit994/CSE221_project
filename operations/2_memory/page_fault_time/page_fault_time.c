#include <sys/mman.h>
#include <fcntl.h>
#include <math.h>
#include "../../../utils/utils.h"

#define PAGE_SIZE 4096 // 4 bytes
#define PAGE_READ_OFFSET 16<<20 // 16 MB
#define FILE_SIZE 1<<30 // 1 GB
#define FAKE_FILE "build/temp_1GB_file"

struct MMem { 
  int fd;

  char* ptr;
  int faulted_data;
  int nreads;
  int page_size;
  int file_size;
  int offset;
};

// This permutation code is taken from lmbench3
// It returns a random permutation
// Usage:
//  size_t* perm = permutation(4, 1);
//  // perm has been randomized, e.g. [3, 1, 0, 2]
//  size_t* perm10 = permutation(4, 10);
//  // 10 is the unit in perm10, e.g. [30, 10, 0, 20]
//  free(perm);
//  free(perm10); // remember to free it
// Note: as a result of experiments, permutation doesn't make a significant difference
size_t* permutation(int max, int scale) {
    size_t  i, v;
    static size_t r = 0;
    size_t* result = (size_t*)malloc(max * sizeof(size_t));

    if (result == NULL) return NULL;

    for (i = 0; i < max; ++i) {
        result[i] = i * (size_t)scale;
    }

    if (r == 0)
        r = (getpid()<<6) ^ getppid() ^ rand() ^ (rand()<<10);

    /* randomize the sequence */
    for (i = max - 1; i > 0; --i) {
        r = (r << 1) ^ rand();
        v = result[r % (i + 1)];
        result[r % (i + 1)] = result[i];
        result[i] = v;
    }

#ifdef _DEBUG
    fprintf(stderr, "permutation(%d): {", max);
    for (i = 0; i < max; ++i) {
      fprintf(stderr, "%zu", result[i]);
      if (i < max - 1) 
        fprintf(stderr, ",");
    }
    fprintf(stderr, "}\n");
    fflush(stderr);
#endif /* _DEBUG */

    return (result);
}

struct linear_regression_result_t { 
    double slope;
    double intercept;
    double max_diff;
};

struct linear_regression_result_t linear_regression(double* xs, double* ys, int n) {
    if (n < 0) {
        handle_error("linear_regression error: non-positive n");
    }

    double x, x2, y, xy;
    for (int i = 0; i < n; ++i) {
        x += xs[i];
        y += ys[i];
        x2 += xs[i] * xs[i];
        xy += xs[i] * ys[i];
    }

    if (n * x2 - x * x == 0) {
        handle_error("linear_regression error: zero division");
    }

    struct linear_regression_result_t result;

    result.slope = (n * xy - x * y) / (n * x2 - x * x);
    result.intercept = (x2 * y - x * xy) / (n * x2 - x * x);

    double max_diff = 0.0;
    for (int i = 0; i < n; ++i) {
        double y_fit = result.slope * xs[i] + result.intercept;
        double diff = fabs(ys[i] - y_fit);
        max_diff = max_diff < diff ? diff : max_diff;
#ifdef _DEBUG
        printf("x=%.2f, y = %.2f, y_fit=%.2f\n", xs[i], ys[i], y_fit);
#endif
    }

    result.max_diff = max_diff;
    return result;
}

// Benchmark for page faults
// In each call, `mmem->nreads` page faults are triggered
uint64_t benchmark(struct MMem* mmem) {
    struct Timer timer;
    char poker = 0;

     // sync: complete all disk writes, purge: rest disk caches
    if (system("sync") == -1) handle_error_en(-1, "sync");
    if (system("purge") == -1) handle_error_en(-1, "purge");

    mmem->ptr = (char*)mmap(0, mmem->file_size, PROT_READ, MAP_SHARED, mmem->fd, 0);
    tic(timer);
    for (uint64_t j = 0; j < mmem->nreads; ++j) {
        // printf("off=%llu\n", ((j * mmem->offset) % mmem->file_size));
        poker = mmem->ptr[(j * mmem->offset) % mmem->file_size];
    }
    toc(timer);
    munmap(mmem->ptr, mmem->file_size);

    return timer_diff(timer);
}

int main() {
    set_nice(-20);

    struct MMem mmem;
    mmem.page_size = PAGE_SIZE;
    mmem.file_size = FILE_SIZE;
    mmem.offset = PAGE_READ_OFFSET;

    // prepare file
    mmem.fd = open(FAKE_FILE, O_RDONLY);
    if (mmem.fd <= 0)
        handle_error("No file named " FAKE_FILE " found");

    const int nreads[] = {1, 10, 20, 30, 40, 50};
    const int nruns = sizeof(nreads) / sizeof(int);
    double sizes[nruns], overheads[nruns];

    for (int i = 0; i < nruns; ++i) {
        mmem.nreads = nreads[i];
        mmem.faulted_data = PAGE_SIZE * nreads[i];
        uint64_t cycles = benchmark(&mmem);

#ifdef _DEBUG
        printf("%d, %llu\n", mmem.faulted_data, cycles);
#endif
        sizes[i] = (double) (mmem.faulted_data >> 10); // unit: 1KB
        overheads[i] = ((double)cycles/1e3);  // unit: 1K cycles
    }

    close(mmem.fd);

    struct linear_regression_result_t reg = linear_regression(sizes, overheads, nruns);

#ifdef _DEBUG
    printf("linear fit: overhead = %.5f size (KB) + %.5f (Kcycles), max_diff=%.5f\n", reg.slope, reg.intercept, reg.max_diff);
#endif
    // reg.slope: Kcycles/KB
    // bandwidth: MB/s = MB/KB * KB/Kcycles * Kcycles/cycles * cycles/s
    //                 = 1/1024 * 1/reg.slope * 1/1e3 * 2.7e9
    // page fault service time (ns) = PAGE_SIZE / bandwidth * 1e9
    // page fault service time: cycles/page
    //  = Kcycles/KB * KB/page * cycles/Kcycles
    //  = reg.slope * PAGE_SIZE/1024 * 1K
    double service_time = reg.slope * 1e3 * PAGE_SIZE / 1024.0;
    cnprintf(LOW, "main", "\n\n");
    cnprintf(LOW, "main", "***************** RESULT *****************");
    cnprintfsi(LOW, "main", "cycles_taken", (int) service_time);
#ifdef _DEBUG
    double bandwidth = 2.7e6/1024/reg.slope;
    printf("%.9f (ms/page), bandwidth=%.2f(MB/s)\n", service_time/2.7e6, bandwidth);
#endif
}

// Note: Without permutation, overhead is constant 


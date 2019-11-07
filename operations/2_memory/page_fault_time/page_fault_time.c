#include <sys/mman.h>
#include <fcntl.h>
#include <math.h>
#include "../../../utils/utils.h"

#define NUM_ITERATIONS 10000
#define PAGE_SIZE 4096
#define MAX_N_PAGES 20
#define FAKE_FILE "build/temp_1GB_file"

struct MMem { 
  int fd;

  char* ptr;
  int size;
  int npages;
  int page_size;
  size_t* pages;
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
      fprintf(stderr, "%d", result[i]);
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
// In each iteration, `mmem->npages` page faults are triggered
// NUM_ITERATIONS iterations in total
uint64_t benchmark(struct MMem* mmem) {
    struct Timer timer;
    char poker = 0;

    tic(timer);
    for (int i=0; i < NUM_ITERATIONS; ++i) {
        mmem->ptr = (char*)mmap(0, mmem->size, PROT_READ, MAP_SHARED, mmem->fd, 0);
        for (int j = 0; j < mmem->npages; ++j) {
            poker = *((mmem->ptr + mmem->pages[j]));
        }
        munmap(mmem->ptr, mmem->size);
    }
    toc(timer);
    return (uint64_t) (timer_diff(timer));
}


int main() {
    struct MMem mmem;
    mmem.fd = open(FAKE_FILE, O_RDONLY);

    if (mmem.fd <= 0)
        handle_error("No file named " FAKE_FILE " found");

    mmem.page_size = PAGE_SIZE;

    double sizes[MAX_N_PAGES], overheads[MAX_N_PAGES];

    for (int i = 0; i < MAX_N_PAGES; ++i) {
        mmem.size = PAGE_SIZE * (i+1);
        mmem.npages = 1 + (mmem.size - 1) / mmem.page_size;
        mmem.pages = permutation(mmem.npages, mmem.page_size);
        uint64_t cycles = benchmark(&mmem);
        free(mmem.pages);
        printf("%d, %llu\n", mmem.size, cycles);

        if ((int) cycles != cycles) {
            handle_error("time overflow");
        }

        sizes[i] = (double) (mmem.size >> 10); // unit: 1KB
        overheads[i] = (double) (cycles/1e6);  // unit: 1M cycles
    }

    close(mmem.fd);

    struct linear_regression_result_t reg = linear_regression(sizes, overheads, MAX_N_PAGES);
    printf("linear fit: overhead = %.2f size (KB) + %.2f (Mcycles), max_diff=%.2f\n", reg.slope, reg.intercept, reg.max_diff);
}

// Note: Without permutation, overhead is constant 


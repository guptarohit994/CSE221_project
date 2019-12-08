#include "../../../utils/utils.h"
#include <sys/fcntl.h>
#include <stdint.h>

#define BLOCK_SIZE (4*KB)

// Estimate file cache size
// Methodology:
//  * read a file 2 rounds
// 	* In the 1st round, try to load the entire file into page cache
//  * In the 2nd round, if file size doesn't exceed cache size, all access will be fast RAM accesses
//  * If file size exceeds, slow accesses even in the 2nd round
// Note:
//  1. `purge` is necessary before each 1st round
//  2. block size makes a difference
//  3. disable readahead

#define NONNEG(msg, status) \
    ({ __typeof__ (status) _status = (status);  \
    if (_status < 0) handle_error_en(_status, (msg)); })

struct Timer timer;

uint64_t do_reads( int file_fd, char* buffer, uint64_t read_size) {
	uint64_t bytes_read = 0;
	tic(timer);
	while (bytes_read < read_size) {
	    bytes_read += read(file_fd, buffer, BLOCK_SIZE);
	}
	toc(timer);
	return timer_diff(timer);
}

int main(int argc, char *argv[]){
	set_nice(-20);

	if (argc != 2) {
		handle_error("Usage: ./file_cache_size filename");
	}

	char *filename;
	filename = argv[1];

	// open the file for reading
	int file_fd = open(filename, O_RDONLY);
	NONNEG("open", file_fd);

	// disable read ahead and enable disk cache
	NONNEG("fcntl_f_rdahead", fcntl(file_fd, F_RDAHEAD, 0));
	NONNEG("fcntl_f_nocache", fcntl(file_fd, F_NOCACHE, 0));

	printf("Opened filename:%s for reading with fd:%d\n", filename, file_fd);

	/* determine the size of file */
	uint64_t file_size = lseek(file_fd, 0L, SEEK_END);
	NONNEG("fseek: SEEK_END", file_size);
	printf("Size of file is %lluB\n", file_size);

	printf("Block size is %lluB\n", BLOCK_SIZE);

	uint64_t file_sizes_to_read[] = {
	    1*MB,   2*MB,   3*MB,   4*MB,   5*MB,   6*MB,   7*MB,   8*MB,   9*MB,  10*MB,
	    100*MB, 200*MB, 300*MB, 400*MB, 500*MB, 600*MB, 700*MB, 800*MB, 900*MB,
	    1*GB,   2*GB,   3*GB,   4*GB,
		5000 * MB, 5080 * MB, 5160 * MB, 5240 * MB, 5320 * MB, 5400 * MB, 5480 * MB,
		5560 * MB, 5640 * MB, 5720 * MB, 5800 * MB, 5880 * MB, 5960 * MB,
		6*GB, 7*GB, 8*GB,
    };
	assert( (file_size >> 30) >= 8);

	uint64_t cycles_taken = 0;
	char *buffer = (char *) malloc(BLOCK_SIZE);

	printf("%s, %s, %s\n", "File Size(MB)", "Time(ms)", "Bandwidth (GB/s)");
	for (int i=0; i < (sizeof(file_sizes_to_read)/sizeof(uint64_t)); ++i) {
		uint64_t size_to_read = file_sizes_to_read[i];

		NONNEG("sync", system("sync"));    // sync: complete all disk writes
		NONNEG("purge", system("purge"));  // purge: rest disk caches

		// NONNEG("fseek: SEEK_SET", lseek(file_fd, rand() % (file_size - file_size_to_read), SEEK_SET));
		NONNEG("fseek: SEEK_SET", lseek(file_fd, 0, SEEK_SET));
		cycles_taken = do_reads(file_fd, buffer, size_to_read);

#ifdef _DEBUG
		printf("cache warmup round: %.2fMB, %.2fms, %.2fGB/s\n", (size_to_read*1.0/MB), cycles_taken/2.7e6, (size_to_read/(MB))/(cycles_taken/2.7e6));
#endif

		NONNEG("fseek: SEEK_SET", lseek(file_fd, 0, SEEK_SET));
		cycles_taken = do_reads(file_fd, buffer, size_to_read);

		// file size(MB), time (ms), bandwidth (GB/s)
		printf("%.2f, %.2f, %.2f\n", (size_to_read*1.0/MB), cycles_taken/2.7e6, (size_to_read*1.0/MB)/(cycles_taken/2.7e6));
	}

	free(buffer);
	NONNEG("close", close(file_fd));

	exit(EXIT_SUCCESS);
}

#include "../../../utils/utils.h"
//for open, O_RDONLY
#include <sys/fcntl.h>
#include <stdint.h>

#define NUM_ITERATIONS 1025


uint64_t do_reads(uint64_t num_iterations, int file_fd, uint64_t read_size_bytes){
	uint64_t status;

	// timer to measure time
    struct Timer timer;
    uint64_t cycles_taken;

	// read the file
	char *buffer;
	buffer = (char *) malloc(read_size_bytes);

	/* **************** **************** **************** time starts now **************** **************** **************** */
    tic(timer);

	for (uint64_t i = 0ULL; i < num_iterations; i++){
		status = read(file_fd, buffer, read_size_bytes);
		// strict checking since we expect full size to be read
		if (status < read_size_bytes)
			handle_error_en(status, "read");
		else
			printf("Read %lluB from file\n", status);
	}

	/* **************** **************** **************** time ends now **************** **************** **************** */
    toc(timer);

    cycles_taken = (uint64_t) (timer_diff(timer));

	return cycles_taken;
}

int main(int argc, char *argv[]){
	assert(argc == 2);

	char *filename;
	filename = argv[1ULL];

	int file_fd;
	uint64_t file_size;
	uint64_t temp;
	uint64_t status;

	uint64_t temp_size[] = {
	      1*MB,   2*MB,   3*MB,   4*MB,   5*MB,   6*MB,   7*MB,   8*MB,   9*MB,  10*MB,
	    100*MB, 200*MB, 300*MB, 400*MB, 500*MB, 600*MB, 700*MB, 800*MB, 900*MB,   1*GB, 
	              2*GB,   3*GB,   4*GB,   5*GB,   6*GB,   7*GB,   8*GB,   9*GB,  10*GB,
    };

    uint64_t test = 5*MB*MB*1024ULL;

	cnprintf(LOW, "main", "\n\n");

	// open the file for reading
	file_fd = open(filename, O_RDONLY);
	if (file_fd < 0)
		handle_error_en(file_fd, "open");
	else
		printf("Opened filename:%s for reading with fd:%d\n", filename, file_fd);

	/* determine the size of filename */
	file_size = lseek(file_fd, 0L, SEEK_END);
	if (file_size < 0ULL)
		handle_error_en(file_size, "fseek: SEEK_END");

	printf("Size of file is %lluB\n", file_size);


	for (uint64_t i = 19ULL; i < (sizeof(temp_size)/sizeof(uint64_t)); i++) {
		printf("i:%llu\n", i);
		// restore the seek to 0 position before reading
		temp = lseek(file_fd, 0L, SEEK_SET);

		// TODO temp is uint64_t
		if (temp < 0ULL)
			handle_error_en(temp, "fseek: SEEK_SET");

		uint64_t read_size_bytes = temp_size[i];
		uint64_t max_iterations_possible = (uint64_t) (file_size / read_size_bytes);

		if (max_iterations_possible < 1ULL)
			break;
		else{
			printf("temp:%llu\n", temp);
			//printf("max_iterations_possible:%llu for read_size_bytes:%lluMB\n", max_iterations_possible, read_size_bytes>>20);
			if (max_iterations_possible >= NUM_ITERATIONS)
				max_iterations_possible = NUM_ITERATIONS;
		}

		uint64_t cycles_taken_per_iteration = (uint64_t) (do_reads(max_iterations_possible, file_fd, read_size_bytes)/(max_iterations_possible));

		// subtract overheads
	    cycles_taken_per_iteration -= (READING_TIME_OVERHEAD + LOOP_OVERHEAD);

	    if (i == 0ULL)
			cnprintf(LOW, "main", "***************** RESULT: FILE_CACHE_SIZE *****************");
	    //cnprintfsui64(LOW, "main", "iterations", max_iterations_possible);
	    printf("main: (per iteration / %llu iterations of %lluMB each) cycles_taken = %llu\n", max_iterations_possible, temp_size[i]>>20ULL, cycles_taken_per_iteration);
	}

	// close the file
	status = close(file_fd);
	if (status < 0ULL)
		handle_error_en(status, "close");
	
	exit(EXIT_SUCCESS);

}
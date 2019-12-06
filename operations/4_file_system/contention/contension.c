#include "stdint.h"
#include "../../../utils/utils.h"
#include <inttypes.h>
//for open, O_RDONLY
#include <sys/fcntl.h>
#define MAX_BLOCK_READ_COUNT 1024

uint64_t readFile(char* filename)
{
	uint64_t read_size_bytes = 4096;
	char *buffer;
	buffer = (char *) malloc(read_size_bytes);	

	int file_fd;
	int fileStatus;
	
    system("purge");
	/* open the file for reading */
	file_fd = open(filename, O_RDONLY);
	if (file_fd < 0)
		handle_error_en(file_fd, "open");
	// else
	// 	printf("Opened filename:%s for reading with fd:%d\n", filename, file_fd);

	fileStatus = fcntl(file_fd, F_NOCACHE, 1);
	if (fileStatus < 0)
		handle_error_en(fileStatus, "fcntl_f_nocache");

	fileStatus = fcntl(file_fd, F_RDAHEAD, 0);
	if (fileStatus < 0)
		handle_error_en(fileStatus, "fcntl_f_rdahead");

	uint64_t status;
	uint64_t cycles_taken;

	status = read(file_fd, buffer, read_size_bytes);
	struct Timer timer;

	/* **************** **************** **************** time starts now **************** **************** **************** */
    	tic(timer);

	// perform the reads
	status = read(file_fd, buffer, read_size_bytes);
	// strict checking since we expect full size to be read
	if (status < read_size_bytes)
		handle_error_en(status, "read");
	// else
	// 	printf("%llu, Read %lluB from file\n", count_nodes,status);

	/* **************** **************** **************** time ends now **************** **************** **************** */
    	toc(timer);
	uint64_t this_time = (uint64_t) (timer_diff(timer));
	status = close(file_fd);
	if (status < 0)
		handle_error_en(status, "close");

	return this_time;
}

int main(int argc, char *argv[]) {

	if (argc != 2) {
		handle_error_en(argc, "2_arguments_required: <executable> <number_of_child_proceses>");
	}
	int children = atoll(argv[1]);
	if(children > 10)
	{
		handle_error_en(children, "Maximum of 10 children");
	}
	uint64_t times[] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	char* fileNames[] = {"file0","file1","file2","file3","file4","file5","file6","file7","file8","file9","file10"};

	for(int i = 0; i < children; i++)
	{
		if(fork() == 0)
		{
			uint64_t tempValue = readFile(fileNames[i+1]);
			times[i] = tempValue; //I wrote these two lines and stored them so that the compiler does not optimize the reads
			printf("Finished Contension with %d children at cycles:%llu\n",i+1, tempValue);
		}
		else
		{
            if (i == 0) {
			    uint64_t tempValue = readFile(fileNames[i]);
			    printf("Finished Contension with %d children at cycles:%llu\n", children, tempValue);
            }
		}
	}
	return 0;
}

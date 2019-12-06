#include "../../../utils/utils.h"
//for open, O_RDONLY
#include <sys/fcntl.h>
#define MAX_BLOCK_READ_COUNT 1024

/* struct for linked list */
typedef struct Node  
{ 
  uint64_t block;
  struct Node *next; 
} Node;

Node * prepare_linked_list(uint64_t total_blocks){
	Node *array;
	// printf("preparing linked list with %llu nodes\n", total_blocks);

	uint64_t num_nodes_possible = total_blocks;

	array = (Node*) malloc(sizeof(Node) * num_nodes_possible);
	memset(array, 0x00, sizeof(Node) * num_nodes_possible);

	if( array == NULL )
	  	handle_error("malloc");
	// else
	// 	printf("malloc gave %p\n", array);

	uint64_t i;
	for (i=0; i<num_nodes_possible; i++){
    	array[i].block = i;
  	}

	//setup linked list
	uint64_t random_offset = 0;
	uint64_t previous_offset = 0;
	int rand_nums_occurred[num_nodes_possible];

	for (i = 0; i < num_nodes_possible; i++){
		rand_nums_occurred[i] = 0;
	}
	rand_nums_occurred[0] = 1;

	for (i = 0; i < num_nodes_possible; i++){

#ifndef SEQUENTIAL_ACCESS
		int repeat = 1;
		uint64_t count = 0;
		uint64_t max_count = 10*i;
		while (repeat == 1 && count < max_count){
			random_offset += rand();
			random_offset %= num_nodes_possible;
			//printf("checking for random_offset:%zu\n", random_offset);
			if (rand_nums_occurred[random_offset] == 1){
				repeat = 1;
				count++;
				//printf("count:%zu\n",count );
				if (count >= max_count)
					handle_error("Failed to select a node");
			} else{
				rand_nums_occurred[random_offset] = 1;
				repeat = 0;
			}
		}

		array[previous_offset].next = &array[random_offset];

#endif
#ifdef SEQUENTIAL_ACCESS
		random_offset = i+1;
		if (i == (num_nodes_possible - 1))
			array[previous_offset].next = NULL;
		else
			array[previous_offset].next = &array[random_offset];
#endif
		//printf("random_offset:%zu (%p), previous_offset:%zu (%p)\n", random_offset, array[previous_offset].next, previous_offset, &array[previous_offset]);

		previous_offset = random_offset;
	}

	//printf("prepared memory of size:%zuK\n", memory_region_size>>10);
	return array;
  
}

uint64_t do_reads(Node *array, int file_fd, uint64_t read_size_bytes){
	uint64_t status;

    uint64_t cycles_taken;
    cycles_taken = 0ULL;

	// read the file
	char *buffer;
	buffer = (char *) malloc(read_size_bytes);

	Node* current = array;
	Node* next = array;
	uint64_t count_nodes = 0;

	while (next != NULL){
		count_nodes++;
		// timer to measure time
    	struct Timer timer;

    	/* **************** **************** **************** time starts now **************** **************** **************** */
    	tic(timer);

    	// move the seek to corresponding block
		status = lseek(file_fd, ((current->block)*BLOCKSIZE*1ULL), SEEK_SET);
		if (status != (current->block)*BLOCKSIZE*1ULL)
			handle_error("lseek");

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
    	//printf("count_nodes:%llu, node_num:%llu, cycles:%llu\n", count_nodes, current->block, this_time);
    	cycles_taken += this_time;
		
		next = current->next;
		current = next;

#ifndef NO_LIMIT
		if (count_nodes >= MAX_BLOCK_READ_COUNT)
			break;
#endif
	}
	//printf("count_nodes:%llu\n", count_nodes);
	return cycles_taken;
}


/* main function to setup the linked list and start multi-threaded execution */
int main(int argc, char *argv[]) {
	if (argc != 4) {
		handle_error_en(argc, "4_arguments_required: <executable> <path_to_file_for_reading> <sizeof_file_to_read> <num_of_child_process>");
	}
	assert(argc == 4);

	char *filename;
	filename = argv[1];

	uint64_t file_size = (uint64_t) atoll(argv[2]);
	assert( file_size <= 7*GB);
	assert( file_size >= BLOCKSIZE);

	int num_of_child_process = (int) atoi(argv[3]);
	printf("num_of_child_process:%d\n", num_of_child_process);
	assert( num_of_child_process <= 9);


	set_nice(-20);
	int *child_pids = (int*)malloc(sizeof(int) * num_of_child_process);

	for (int child = 1; child <= num_of_child_process; child++){
		char child_file_name[sizeof(filename)+100];
		sprintf(child_file_name, "%s%d", filename, child);
		printf("child:%d accessing file:%s\n", child, child_file_name);

		int child_pid = fork();
		if (child_pid == 0) {
			int file_fd;
			int status;
			struct Node *array;

			/* open the file for reading */
			file_fd = open(child_file_name, O_RDONLY);
			if (file_fd < 0)
				handle_error_en(file_fd, "open");
			// else
			// 	printf("Opened filename:%s for reading with fd:%d\n", filename, file_fd);

			status = fcntl(file_fd, F_NOCACHE, 1);
			if (status < 0)
				handle_error_en(status, "fcntl_f_nocache");

			status = fcntl(file_fd, F_RDAHEAD, 0);
			if (status < 0)
				handle_error_en(status, "fcntl_f_rdahead");

			/* determine the size of filename */
			uint64_t file_size_auto = lseek(file_fd, 0L, SEEK_END);
			// file_size_auto is less than claimed
			if (file_size_auto < file_size)
				handle_error_en(file_size_auto, "file_size_different");

			/* restore the seek to 0 position before reading */
			file_size_auto = lseek(file_fd, 0L, SEEK_SET);
			if (file_size_auto != 0ULL)
				handle_error_en(file_size_auto, "fseek: SEEK_SET");

			uint64_t total_blocks = (uint64_t) (file_size / (BLOCKSIZE*1ULL));
			
			array = prepare_linked_list(total_blocks);

			//system("sync");
			system("purge");

		#ifndef NO_LIMIT
			if (total_blocks >= MAX_BLOCK_READ_COUNT)
				total_blocks = MAX_BLOCK_READ_COUNT;
		#endif

			uint64_t cycles_taken_per_iteration = (uint64_t) (do_reads(array, file_fd, BLOCKSIZE*1ULL)/total_blocks);
			if (file_size>>30ULL >= 1)
				printf("child:%d - total access:%lluGB, Average access time/block(%llu):%llu clock cycles\n", child, file_size>>30ULL, total_blocks,cycles_taken_per_iteration);
			else if (file_size>>20ULL >= 1)
				printf("child:%d - total access:%lluMB, Average access time/block(%llu):%llu clock cycles\n", child, file_size>>20ULL, total_blocks,cycles_taken_per_iteration);
			else
				printf("child:%d - total access:%lluKB, Average access time/block(%llu):%llu clock cycles\n", child, file_size>>10ULL, total_blocks,cycles_taken_per_iteration);

			// close the file
			status = close(file_fd);
			if (status < 0)
				handle_error_en(status, "close");
			
			free(array);

			return EXIT_SUCCESS;
		} else {
			printf("Parent - child:%d launched\n", child);
			child_pids[child - 1] = child_pid;
		}
	}
	// make parent wait for childs to finish
	for (int i = 0; i < num_of_child_process; i++)
		wait(&child_pids[i]);

return EXIT_SUCCESS;
}
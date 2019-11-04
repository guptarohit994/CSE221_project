#include "../../../utils/utils.h"
//for open, O_RDONLY
#include <sys/fcntl.h>

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

    	// move the seek to corresponding block
		status = lseek(file_fd, ((current->block)*BLOCKSIZE*1ULL), SEEK_SET);
		if (status != (current->block)*BLOCKSIZE*1ULL)
			handle_error("lseek");

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

    	cycles_taken += (uint64_t) (timer_diff(timer));
		
		next = current->next;
		current = next;
	}
	return cycles_taken;
}


/* main function to setup the linked list and start multi-threaded execution */
int main(int argc, char *argv[]) {
	if (argc != 3) {
		handle_error_en(argc, "3_arguments_required: <executable> <path_to_file_for_reading> <sizeof_file_to_read>");
	}
	assert(argc == 3);

	char *filename;
	filename = argv[1];

	uint64_t file_size = (uint64_t) atoll(argv[2]);
	assert( file_size <= 7*GB);
	assert( file_size >= BLOCKSIZE);


	int file_fd;
	int status;
	struct Node *array;

	set_nice(-20);

	// uint64_t temp_size[] = {
	//       1*MB,   2*MB,   3*MB,   4*MB,   5*MB,   6*MB,   7*MB,   8*MB,   9*MB,  10*MB,
	//     100*MB, 200*MB, 300*MB, 400*MB, 500*MB, 600*MB, 700*MB, 800*MB, 900*MB,   1*GB, 
	//               2*GB,   3*GB,   4*GB,   5*GB,   6*GB,   7*GB,
 //    };

	/* open the file for reading */
	file_fd = open(filename, O_RDONLY);
	if (file_fd < 0)
		handle_error_en(file_fd, "open");
	// else
	// 	printf("Opened filename:%s for reading with fd:%d\n", filename, file_fd);

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

	// Node* current = array;
	// Node* next = array;

	// //printf("->(%zu) ", current->id);
	// while (next != NULL){
	// 	printf("->(%llu) ", current->block);
	// 	next = current->next;
	// 	current = next;
	// }
	// printf("\n");

	uint64_t cycles_taken_per_iteration = (uint64_t) (do_reads(array, file_fd, BLOCKSIZE*1ULL)/total_blocks);
	printf("main: total access:%lluMB, Average access time/block:%llu clock cycles\n", file_size>>20ULL, cycles_taken_per_iteration);

	// close the file
	status = close(file_fd);
	if (status < 0)
		handle_error_en(status, "close");
	
	free(array);



	return EXIT_SUCCESS;
}
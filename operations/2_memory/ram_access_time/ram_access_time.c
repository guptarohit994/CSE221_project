#include "../../../utils/utils.h"


const int SECONDS_PER_NS = 1000000000;

/* struct for linked list */
typedef struct Node  
{ 
  // 8
  size_t id;
  // 2 * 24 = 48
  uint16_t data[24];
  // 8
  struct Node *next; 
} Node;


// This function does accesses (read, read-write) to the contents of linked list starting from  
// the given node 
void do_accesses(Node *n, size_t mem_size) 
{ 
  Node *prev_node = n;
  size_t stride = 0;
  size_t count_nodes = 0;
  struct Timer timer;
  size_t access_time = 0;
  size_t total_access_time = 0;

  while (n != NULL)
  { 
    count_nodes++;
    size_t temp;
    int data_to_access = rand() % 24;
    asm (
      "mfence\n\t"        // memory fence
      :
      :
    );
    //clock_gettime(CLOCK_REALTIME, &startAccess); //start clock
    tic(timer);
    asm (
      "mov %0, %%al\n\t"  // load data
      :
      : "m" (n->data[data_to_access])
    );


    //clock_gettime(CLOCK_REALTIME, &endAccess); //end clock
    toc(timer);

    asm (
      "mfence\n\t"        // memory fence
      :
      :
    );
    access_time = (uint64_t) (timer_diff(timer));
    total_access_time += access_time;
    //AccessTime = ((endAccess.tv_sec - startAccess.tv_sec) * SECONDS_PER_NS) + (endAccess.tv_nsec - startAccess.tv_nsec);
    //totalAccessTime += AccessTime;
    //printf("Thread:%d# data[%d]:0x%X, n:0x%X, AccessTime:%0.2f ns\n", tid, data_to_access,n->data[data_to_access], n, AccessTime);
    if (prev_node > n){
        stride = prev_node - n;
    }else{
        stride = n - prev_node;
    }
    //printf("n:%p, prev_node:%p\n", n, prev_node);
    printf("data[%d]:0x%X, stride:%zuK, AccessTime:%zu cycles\n", data_to_access,n->data[data_to_access], stride, access_time);
    prev_node = n;
    n = n->next; 
  } 
  printf("total_linked_list_nodes:%zu, mem_size:%zuK, averageAccessTime:%0.2f cyles\n", count_nodes,mem_size>>10, (double)total_access_time/count_nodes);
}

/* this function creates a linked list either in sequential or non-sequential manner
as dictated by the defines and returns a pointer to the head of the LL*/
Node * prepare_linked_list (size_t memory_region_size){
  Node *array;
  //printf("preparing memory of size:%zuK\n", memory_region_size>>10);

  size_t num_nodes_possible = (size_t) (memory_region_size / sizeof(Node));
  printf("num_nodes_possible:%zu\n", num_nodes_possible);

  array = (Node*) malloc(sizeof(Node) * num_nodes_possible);
  memset(array, 0x00, memory_region_size);

  if( array == NULL ){
      printf("malloc failed!\n");
      exit(1);
  }else{
    //printf("malloc gave %p\n", array);
  }

  size_t i;
  for (i=0; i<num_nodes_possible; i++){
    array[i].id = i;
  }

  //setup linked list
  size_t max_count = num_nodes_possible;
  size_t random_offset = 1;
  size_t previous_offset = 0;
  int rand_nums_occurred[num_nodes_possible];

  for (i = 0; i < num_nodes_possible; i++){
    rand_nums_occurred[i] = 0;
  }
  rand_nums_occurred[0] = 1;
    
  for (i = 0; i < num_nodes_possible - 1; i++){

#ifndef SEQUENTIAL_ACCESS
    int repeat = 1;
    int count = 0;
    while (repeat == 1 && count < 300){
      random_offset += rand();
      random_offset %= num_nodes_possible;
      if (rand_nums_occurred[random_offset] == 1){
        repeat = 1;
        count++;
        //printf("count:%d\n",count );
        if (count >= 1000){
          printf("Failed to select a node even after 1000 tries!\n");
          exit(1);
        }
      } else{
        rand_nums_occurred[random_offset] = 1;
        repeat = 0;
      }
    }

#endif
#ifdef SEQUENTIAL_ACCESS
    random_offset = i+1;
#endif
    array[previous_offset].next = &array[random_offset];
    //printf("random_offset:%zu (%p), previous_offset:%zu (%p)\n", random_offset, array[previous_offset].next, previous_offset, &array[previous_offset]);
    
    previous_offset = random_offset;
  }
  
  //printf("prepared memory of size:%zuK\n", memory_region_size>>10);
  return array;
  
}


/* main function to setup the linked list and start multi-threaded execution */
int main(int argc, char **argv) {
  struct Node *array;
  int i, rc;

  float base_latency = 0.0;
  size_t mem_size = 0;
  size_t temp_size[] = {
      1<<10,  2<<10,  3<<10,  4<<10,  5<<10,  6<<10,  7<<10,  8<<10,  9<<10,
     10<<10, 11<<10, 12<<10, 13<<10, 14<<10, 15<<10, 16<<10, 17<<10, 18<<10,
     19<<10, 20<<10, 21<<10, 22<<10, 23<<10, 24<<10, 25<<10, 26<<10, 27<<10,
     28<<10, 29<<10, 30<<10, 31<<10, 32<<10, 48<<10, 64<<10, 80<<10, 96<<10,
    112<<10,128<<10,192<<10,256<<10,320<<10,384<<10,448<<10,512<<10,  1<<20,
      2<<20,  3<<20,  4<<20,  5<<20,  6<<20,  7<<20,  8<<20,  9<<20, 10<<20,
     11<<20, 12<<20, 13<<20, 14<<20, 15<<20, 16<<20, 17<<20, 18<<20, 19<<20,
     20<<20, 21<<20, 22<<20, 23<<20, 24<<20, 25<<20, 26<<20, 27<<20, 28<<20,
     29<<20, 30<<20, 31<<20, 32<<20, 64<<20,128<<20,256<<20,512<<20,1024<<20,
    };
  int l;
  for (l=16; l<17; l++){
    printf("-----------------------------------------------------------------\n");
    //printf("l:%d\n",l);
    mem_size = temp_size[l];
  //for (mem_size = 512; mem_size <= 1024*1024*1024; mem_size = mem_size*2){
    //printf("mem_size:%zuK\n", mem_size>>10);
    array = prepare_linked_list(mem_size);

    //do_accesses(array, mem_size);
    
    Node* current = array;
    Node* next = array;


    //printf("->(%zu) ", current->id);
    while (next != NULL){
      printf("->(%zu) ", current->id);
      next = current->next;
      //printf("current:%p, next:%p\n", current, next);
      current = next;
    }
    printf("\n");

    //printf("Main_thread: hello, successfully completed for mem_size:%zuK\n", mem_size>>10);
    printf("-----------------------------------------------------------------\n");
    free(array);
  }
  
 
  return EXIT_SUCCESS;
}
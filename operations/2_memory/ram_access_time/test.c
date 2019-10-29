#include <stdio.h>
#include <stdint.h>

/* struct for linked list */
typedef struct Node  
{ 
  uint16_t data[16];
  uint32_t data2[6];
  struct Node *next; 
} Node;

int main(){
	printf("hello\n");
	printf("%lu\n", sizeof(Node));

	int *array;
	int num;
	num = 9;

	array = &num;

	//array = (int*) malloc(sizeof(int));
	printf("array:%X\n", array);
	*array = 10;
	printf("num:%d\n", num);
}
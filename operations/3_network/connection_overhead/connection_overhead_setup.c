#include "../../../utils/utils.h"

// for socket, PF_INET, SOCK_STREAM
#include <sys/socket.h> 
// for IPPROTO_TCP, sockaddr_in
#include <netinet/in.h>
// for inet_addr
#include <arpa/inet.h>

// using count of just 100 since the variations are too high
// instead of using average (like in code) of 1000 iterations,
// we plan on using median by using the repeat_cmd script on this 100 times
#define NUM_ITERATIONS 100

uint64_t setup_connection(int num_iterations) {
	int sockfd;
	int status;

   uint64_t cycles_taken = 0;

	// use socket address internet style to describe passive participant
	struct sockaddr_in server_addr;

	/* **************** assign server details **************** */
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	server_addr.sin_port = htons(SERVERPORT);

    for (int i = 0; i < num_iterations; i++) {
    	// timer to measure time
		struct Timer timer;
		
		/* **************** **************** **************** time starts now **************** **************** **************** */
		tic(timer);

		/* **************** create a stream socket for internet using TCP protocol **************** */
		sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sockfd < 0)
			handle_error_en(sockfd, "socket");
		// else
		// 	printf("Got sockfd:%d\n", sockfd);

		/* **************** connect to the server **************** */
		status = connect(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr));
		if (status < 0)
			handle_error_en(status, "connect");
		// else
		// 	printf("connected to %s at port %d\n", SERVERADDR, SERVERPORT);

		/* **************** **************** **************** time ends now **************** **************** **************** */
		toc(timer);
		cycles_taken += (uint64_t) (timer_diff(timer));

		/* **************** close the socket **************** */
		status = close(sockfd);
		if (status < 0)
			handle_error_en(status, "close");
		// printf("Socket closed!\n");
	}

	return cycles_taken;
}


int main(){
	cnprintf(LOW, "main", "\n\n");
	uint64_t cycles_taken_per_iteration = (uint64_t) (setup_connection(NUM_ITERATIONS)/(NUM_ITERATIONS));

	// subtract overheads
    cycles_taken_per_iteration -= (READING_TIME_OVERHEAD);

    cnprintf(LOW, "main", "***************** RESULT: CONNECTION_OVERHEAD_SETUP *****************");
    cnprintfsi(LOW, "main", "iterations", NUM_ITERATIONS);
    cnprintfsui64(LOW, "main", "(per iteration) cycles_taken", cycles_taken_per_iteration);
	
	exit(EXIT_SUCCESS);

}
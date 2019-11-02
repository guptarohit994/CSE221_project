#include "../../../utils/utils.h"

// for socket, PF_INET, SOCK_STREAM
#include <sys/socket.h> 
// for IPPROTO_TCP, sockaddr_in
#include <netinet/in.h>
// for inet_addr
#include <arpa/inet.h>

#define NUM_ITERATIONS 100

uint64_t setup_send_receive(int num_iterations) {
	int sockfd;
	int status;

	// msg for sending and buf for receving
	char buf[DATABYTES];
    char msg[DATABYTES];
    int msg_len;

    // timer to measure time
    struct Timer timer;
    uint64_t cycles_taken;

	// use socket address internet style to describe passive participant
	struct sockaddr_in server_addr;

	/* **************** assign server details **************** */
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	server_addr.sin_port = htons(SERVERPORT);

	/* **************** setup msg to be sent **************** */
	for (int i = 0; i < DATABYTES; i++)
        msg[i] = 'a';

    msg_len = strlen(msg);


    /* **************** **************** **************** time starts now **************** **************** **************** */
    tic(timer);

    for (int i = 0; i < num_iterations; i++) {
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

		/* **************** send the msg to server **************** */
	    // status is actually count of bytes sent!
	    status = send(sockfd, msg, msg_len, 0);
	    if (status < msg_len)
	    	handle_error_en(status, "send");
	    // else
	    // 	printf("Sent %d bytes\n", status);
	    //printf("%s\n", msg);

	    /* **************** receive response from server **************** */
	    // in this case, it is a echo server
	    status = recv(sockfd, buf, DATABYTES, 0);
	    if (status < DATABYTES)
	    	handle_error_en(status, "recv");
	    // else
	    // 	printf("Received %d bytes\n", status);

	    //printf("%s\n", buf);

		/* **************** close the socket **************** */
		status = close(sockfd);
		if (status < 0)
			handle_error_en(status, "close");
		// printf("Socket closed!\n");
	}

	/* **************** **************** **************** time ends now **************** **************** **************** */
    toc(timer);

    cycles_taken = (uint64_t) (timer_diff(timer)/(num_iterations));

    return cycles_taken;
}


int main(){
	cnprintf(LOW, "main", "\n\n");
	uint64_t cycles_taken_per_iteration = setup_send_receive(NUM_ITERATIONS);

	// subtract overheads
    cycles_taken_per_iteration -= (READING_TIME_OVERHEAD + LOOP_OVERHEAD);

    cnprintf(LOW, "main", "***************** RESULT *****************");
    cnprintfsi(LOW, "main", "iterations", NUM_ITERATIONS);
    cnprintfsui64(LOW, "main", "(per iteration) cycles_taken", cycles_taken_per_iteration);
	
	exit(EXIT_SUCCESS);

}
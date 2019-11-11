#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "../../../utils/utils.h"

#define NUM_ITERATIONS 100
#define DATABYTES (1ull<<20) // 1MB

#define NONNEG(msg, status) \
    ({ __typeof__ (status) _status = (status);  \
    if (_status < 0) handle_error_en(_status, (msg)); })

// Maximize the socket buffer size
// also enable addr reuse
// from lmbench3 code
// Note: no siginificant difference with or w/o buffer size optimization
void sock_optimize(int sock) {
    int sockbuf;

    // try to use the maximum buffer size
    sockbuf = 1 << 30; // initial socket buff = 1GB, possibly infeasible
    while (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &sockbuf, sizeof(int))) {
        sockbuf >>= 1;
    }

    sockbuf = 1 << 30;
    while (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &sockbuf, sizeof(int))) {
        sockbuf >>= 1;
    }

#ifdef _DEBUG
    int val;
    socklen_t len = sizeof(int);
    getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &val, &len);
    printf("rcv buf size = %dKB\n", val >> 10);
    getsockopt(sock, SOL_SOCKET, SO_SNDBUF, &val, &len);
    printf("snd buf size = %dKB\n", val >> 10);
#endif

    // enable addr reuse so that we don't need to kill the process everytime
    int reuse_addr = 1;
    NONNEG("SO_REUSEADDR", setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)));
}

void server_main() {
    // create server sock
    int server_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    NONNEG("socket", server_sock);
    sock_optimize(server_sock);

    struct sockaddr_in server_addr, client_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVERPORT);
    NONNEG("server bind",   bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)));

listen:
    NONNEG("server listen", listen(server_sock, 1));

    // accept client sock
    socklen_t client_len = sizeof(client_addr);
    int client_sock = accept(server_sock, (struct sockaddr *)&server_addr, &client_len);
    NONNEG("server accept", client_sock);

    // prepare data to send
    char msg[DATABYTES];
    memset(msg, 's', DATABYTES);

    // keep sending data
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        int sent = send(client_sock, msg, DATABYTES, 0);
#ifdef _DEBUG
        printf("server sent: %d KB\n", sent >> 10);
#endif
    }

    // close sockets
    NONNEG("server close",   close(client_sock));

    goto listen;
}

uint64_t client_main() {
    // create client sock
    int client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    sock_optimize(client_sock);
	NONNEG("client socket",  client_sock);

    // connect to server sock
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
    server_addr.sin_port = htons(SERVERPORT);

    NONNEG("client connect", connect(client_sock, (struct sockaddr*) &server_addr, sizeof(server_addr)));

    // prepare buffer
	char buf[DATABYTES];

    // time it: keep receiving datar
    struct Timer timer;

    tic(timer);
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        int rcvd = recv(client_sock, buf, DATABYTES, MSG_WAITALL);
#ifdef _DEBUG
        printf("client received: %d KB\n", rcvd >> 10);
#endif
    }
    toc(timer);

    // close sockets
	NONNEG("client close",   close(client_sock));
    return timer_diff(timer);
}


int main(int argc, char** argv){
    int c = getopt(argc, argv, "s");
    if (c == 's') {
        server_main();
    } else {
        uint64_t cycles_taken = client_main();

        cnprintf(LOW, "main", "***************** RESULT: PEAK_BANDWIDTH *****************");
        cnprintfsi(LOW, "main", "iterations", NUM_ITERATIONS);
        cnprintfsui64(LOW, "main", "(per iteration) cycles_taken", cycles_taken / NUM_ITERATIONS);

#ifdef _DEBUG
        double bandwidth = (8 * (double)(DATABYTES)/(GB)) / ((double)cycles_taken / NUM_ITERATIONS / 2.7e9);
        printf("bandwidth: %.2f Gbps\n", bandwidth);
#endif
    }
}
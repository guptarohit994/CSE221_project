#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "../../../utils/utils.h"

/* Methodology
 * 1. In each iteration, server sends a 1M package
 * 2. Client will receive that package in 4KB segments
 * 3. Skip the first package because of the initial bursting
 * 4. tic after the second package arrived
 * 5. toc after the last package arrived
 * Notes
 * 1. The initial bursting phenomenon
 * 2. The slow-start algorithm used by TCP
 * 3. The impact of package size
 * 4. The tradeoff of num_iterations
 * 5. Catalina reports its bandwidth as 304.2 Mbps, which is much slower than the actual bandwidth(1~2 GB/s on loopback)
 *      This is believed to be a bug(https://discussions.apple.com/thread/250711080)
 * 6. The choice of databytes: large enough to cancel out slow-start algorithm
*/ 

#define NUM_ITERATIONS 20
#define DATABYTES (8*MB)
#define BUF_SIZE (4*KB)
#define INIT_PKG 100

uint64_t history[DATABYTES/BUF_SIZE];

#define NONNEG(msg, status) \
    ({ __typeof__ (status) _status = (status);  \
    if (_status < 0) handle_error_en(_status, (msg)); })

// Enable addr reuse so that we don't need to kill the process everytime
void reuse_addr(int sock) {
#ifdef _DEBUG
    int val;
    socklen_t len = sizeof(int);
    getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &val, &len);
    printf("rcv buf size = %dKB\n", val >> 10);
    getsockopt(sock, SOL_SOCKET, SO_SNDBUF, &val, &len);
    printf("snd buf size = %dKB\n", val >> 10);
#endif

    int reuse_addr = 1;
    NONNEG("SO_REUSEADDR", setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)));
}

void server_main() {
    // create server sock
    int server_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    NONNEG("socket", server_sock);
    reuse_addr(server_sock);

    struct sockaddr_in server_addr, client_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVERPORT);
    NONNEG("server bind",   bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)));

    // prepare data to send
    char* msg = (char*)malloc(DATABYTES);
    memset(msg, 's', DATABYTES);

listen:
    NONNEG("server listen", listen(server_sock, 1));

    // accept client sock
    socklen_t client_len = sizeof(client_addr);
    int client_sock = accept(server_sock, (struct sockaddr *)&server_addr, &client_len);
    NONNEG("server accept", client_sock);

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

    free(msg);
}

uint64_t client_main() {
    // create client sock
    int client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    reuse_addr(client_sock);
	NONNEG("client socket",  client_sock);

    // connect to server sock
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
    server_addr.sin_port = htons(SERVERPORT);

    NONNEG("client connect", connect(client_sock, (struct sockaddr*) &server_addr, sizeof(server_addr)));

    // prepare buffer
	char* buf = (char*)malloc(BUF_SIZE);

    // time it: keep receiving data
    struct Timer timer;
    uint64_t min_t = 0; 

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        int rcvd = 0;
        int pkg_count = 0;
        while( (rcvd += recv(client_sock, buf, BUF_SIZE, MSG_WAITALL)) < DATABYTES) {
            if (pkg_count == INIT_PKG) {
                tic(timer);
            } else if (pkg_count > INIT_PKG) {
                toc(timer);
                history[pkg_count] = timer_diff(timer);
            }
            ++pkg_count;
        }

#ifdef _DEBUG
        printf("client received: %d KB\n", rcvd >> 10);
        for (int i = 0; i < pkg_count; ++i) {
            if (i <= INIT_PKG) {
                printf("%llu, N/A\n", i * BUF_SIZE);
            } else {
                printf("%llu, %llu\n", i * BUF_SIZE, history[i]);
            }
        }
#endif
        uint64_t t = timer_diff(timer) - 32 * pkg_count;
        if (min_t == 0 || t < min_t) min_t = t;
    }

    // close sockets
	NONNEG("client close",   close(client_sock));
    free(buf);
    return min_t;
}


int main(int argc, char** argv){
    int c = getopt(argc, argv, "s");
    if (c == 's') {
        server_main();
    } else {
        uint64_t cycles_taken = client_main();

        cnprintf(LOW, "main", "***************** RESULT: PEAK_BANDWIDTH *****************");
        cnprintfsi(LOW, "main", "iterations", NUM_ITERATIONS);
        cnprintfsui64(LOW, "main", "min cycles_taken", cycles_taken);

#ifdef _DEBUG
        double bandwidth = (double)(DATABYTES - (INIT_PKG + 1) * BUF_SIZE)/ (MB) / (cycles_taken / 2.7e9);
        printf("max bandwidth: %.2f MB/s\n", bandwidth);
#endif
    }
}
#include "../../../utils/utils.h"
#include <pthread.h>
#include <sys/wait.h>
#define NUM_ITERATIONS 100000

struct Pipeinfo {
    int fd_to_read;
    int fd_to_write;
};


void ping_pong_processes(struct Pipeinfo pipes, int num_iterations, char buf_wr[2]) {
    int ret;
    int pid = getpid();
    cnprintfsisisi(LOW, "ping_pong_processes", "pid", pid, "starting ping_pong. readfd", pipes.fd_to_read, \
                                           "writefd", pipes.fd_to_write);

    char buf[2];
    for (int i = 0; i < num_iterations; i++) {
      ret = read(pipes.fd_to_read, buf, 1);
      if (ret != 1)
          handle_error("process_pipe_read");
      //printf("pid:%d, %c\n", pid, buf[0]);

      ret = write(pipes.fd_to_write, buf_wr, 1);
      if (ret != 1)
          handle_error("process_pipe_write");
    }
}

int main(int argc, char *argv[]) {

  cnprintf(LOW, "main", "\n\n");
  cnprintf(LOW, "main", "***************** CONTEXT_SWITCH_PROCESSES *****************");
  set_nice(-20);

  int s;
  int child_pid;
  int parent_pid;
  struct Timer timer;
  uint64_t cycles_taken;

  int p2c_pipefd [2]; /* 1 for parent, 0 for child */
  int c2p_pipefd [2]; /* 0 for parent, 1 for child */
  char buff;
  struct Pipeinfo child_pipes, parent_pipes;
    
  s = pipe(p2c_pipefd);
  if (s == -1)
      handle_error_en(s, "p2c_pipefd");

  s = pipe(c2p_pipefd);
  if (s == -1)
      handle_error_en(s, "c2p_pipefd");

  child_pipes.fd_to_read = p2c_pipefd[0];
  child_pipes.fd_to_write = c2p_pipefd[1];

  parent_pipes.fd_to_read = c2p_pipefd[0];
  parent_pipes.fd_to_write = p2c_pipefd[1];

  // pipe test
  write(parent_pipes.fd_to_write, "s", 1);
  read(child_pipes.fd_to_read, &buff, 1);
  assert(buff == 's');

  // pipe test
  write(child_pipes.fd_to_write, "s", 1);
  read(parent_pipes.fd_to_read, &buff, 1);
  assert(buff == 's');

  
  parent_pid = getpid();

  if ((child_pid = fork()) == -1) {
      handle_error("fork");
  } else if (child_pid == 0) {
      char buf_wr[2];
      buf_wr[0] = 'c';
      //child
      ping_pong_processes(child_pipes, NUM_ITERATIONS, buf_wr);
  } else {
      char buf_wr[2];
      buf_wr[0] = 'p';
      //parent
      write(parent_pipes.fd_to_write, "s", 1);

      /* ***************** measurement starts now ***************** */

      tic(timer);

      ping_pong_processes(parent_pipes, NUM_ITERATIONS, buf_wr);

      toc(timer);

      /* ***************** measurement ends now ***************** */
      s = waitpid(child_pid, NULL, 0);
      if (s == -1)
          handle_error_en(s, "join");

      cnprintfsi(MEDIUM, "main", "Joined with process", child_pid);


      cycles_taken = (uint64_t) (timer_diff(timer)/(2 * NUM_ITERATIONS));

      cnprintf(LOW, "main", "\n\n");
      cnprintf(LOW, "main", "***************** RESULT *****************");
      cnprintfsi(LOW, "main", "iterations", NUM_ITERATIONS);
      cnprintfsui64(LOW, "main", "(per iteration) cycles_taken", cycles_taken);

      // Displaying precise stats
      struct rusage ru;
      if (getrusage(RUSAGE_SELF, &ru)) {
          handle_error("getrusage");
      } else {
          cnprintf(MEDIUM, "main", "From getrusage");
          cnprintfsui64(MEDIUM, "main", "voluntary switches", (uint64_t)ru.ru_nvcsw);
          cnprintfsui64(MEDIUM, "main", "involuntary switches",  (uint64_t) ru.ru_nivcsw);
      }
  }

  exit(EXIT_SUCCESS);
}
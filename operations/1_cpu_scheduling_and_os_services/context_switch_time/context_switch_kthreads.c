#include "../../../utils/utils.h"
#include <pthread.h>
#define NUM_ITERATIONS 100000

struct Pipeinfo {
    int fd_to_read;
    int fd_to_write;
};

struct thread_info {    /* Used as argument to thread_start() */
  pthread_t       thread_id;      /* ID returned by pthread_create() */
  int             thread_num;
  struct Pipeinfo pipes;
};

void ping_pong_threads(struct Pipeinfo pipes, int num_iterations) {
    int ret;
    unsigned tid = (unsigned) pthread_self();
    cnprintfsisi(LOW, "ping_pong_threads", "starting ping_pong. readfd", pipes.fd_to_read, \
                                           "writefd", pipes.fd_to_write);

    char buf;
    for (int i = 0; i < num_iterations; i++) {
      ret = read(pipes.fd_to_read, &buf, 1);
      if (ret != 1)
          handle_error("thread_pipe_read");

      ret = write(pipes.fd_to_write, "g", 1);
      if (ret != 1)
          handle_error("thread_pipe_write");
    }
}

static void * thread_start(void *arg) {
  struct thread_info *tinfo = arg;
  struct Pipeinfo pipes = (struct Pipeinfo) tinfo->pipes;
  
  ping_pong_threads(pipes, NUM_ITERATIONS);

  return NULL;
}

int main(int argc, char *argv[]) {

  cnprintf(LOW, "main", "\n\n");
  cnprintf(LOW, "main", "***************** CONTEXT_SWITCH_KTHREADS *****************");
  set_nice(-20);

  int s;
  struct thread_info *tinfo;
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

  
  /* Allocate memory for pthread_create() arguments */

  tinfo = calloc(1, sizeof(struct thread_info));
  if (tinfo == NULL)
     handle_error("calloc");

  /* Create one thread */

  tinfo[0].thread_num = 1;
  tinfo[0].pipes = child_pipes;

  /* The pthread_create() call stores the thread ID into
    corresponding element of tinfo[] */

  s = pthread_create(&tinfo[0].thread_id, NULL, &thread_start, &tinfo[0]);
  if (s != 0)
      handle_error_en(s, "pthread_create");

  write(parent_pipes.fd_to_write, "s", 1);

  /* ***************** measurement starts now ***************** */

  tic(timer);

  ping_pong_threads(parent_pipes, NUM_ITERATIONS);

  toc(timer);

  /* ***************** measurement ends now ***************** */

  s = pthread_join(tinfo[0].thread_id, NULL);
  if (s != 0)
     handle_error_en(s, "pthread_join");

  cnprintfsi(MEDIUM, "main", "Joined with thread", tinfo[0].thread_num);

  cycles_taken = (uint64_t) (timer_diff(timer)/(2 * NUM_ITERATIONS));

  cnprintf(LOW, "main", "\n\n");
  cnprintf(LOW, "main", "***************** RESULT *****************");
  cnprintfsi(LOW, "main", "iterations", NUM_ITERATIONS);
  cnprintfsui64(LOW, "main", "(per iteration) cycles_taken", cycles_taken);

  free(tinfo);

  // Displaying precise stats
  struct rusage ru;
  if (getrusage(RUSAGE_SELF, &ru)) {
      handle_error("getrusage");
  } else {
      cnprintf(MEDIUM, "main", "From getrusage");
      cnprintfsui64(MEDIUM, "main", "voluntary switches", (uint64_t)ru.ru_nvcsw);
      cnprintfsui64(MEDIUM, "main", "involuntary switches",  (uint64_t) ru.ru_nivcsw);
  }
  exit(EXIT_SUCCESS);
}
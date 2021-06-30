#include "csapp.h"

static void signal_handler(int signum, siginfo_t *info, void *data) {
  if (signum == SIGINT) {
    safe_printf("(%d) Screw you guys... I'm going home!\n", getpid());
    _exit(0);
  }
}

static void play(pid_t next, const sigset_t *set) {
  for (;;) {
    printf("(%d) Waiting for a ball!\n", getpid());
	Sigsuspend(set);
    usleep((300 + random() % 400) * 1000);
    Kill(next, SIGUSR1);
    printf("(%d) Passing ball to (%d)!\n", getpid(), next);
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2)
    app_error("Usage: %s [CHILDREN]", argv[0]);

  int children = atoi(argv[1]);
  if (children < 4 || children > 20)
    app_error("Give number of children in range from 4 to 20!");

  /* Register signal handler for SIGUSR1 */
  struct sigaction action = {.sa_sigaction = signal_handler};
  Sigaction(SIGINT, &action, NULL);
  Sigaction(SIGUSR1, &action, NULL);
 
  sigset_t mask, prev;
  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);
  Sigprocmask(SIG_BLOCK, &mask, &prev);
  pid_t children_pids[20];
  for(int i=0;i<children;i++){
	children_pids[i] = Fork();
    if(children_pids[i] != 0){
		if(i==children-1) Kill(children_pids[i],SIGUSR1);
	}
    else{
 		if(i > 0) play(children_pids[i-1],&prev);	
		else play(getppid(),&prev);
    }
  }
  play(children_pids[children-1],&prev);
 /* TODO: Start all processes and make them wait for the ball! */
  	
  Sigprocmask(SIG_UNBLOCK, &mask, NULL);
  return EXIT_SUCCESS;
}

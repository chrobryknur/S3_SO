#include "csapp.h"

static __unused void outc(char c) {
  Write(STDOUT_FILENO, &c, 1);
}

static __thread unsigned seed;

static sem_t tobacco;
static sem_t matches;
static sem_t paper;
static sem_t doneSmoking;

/* TODO: If you need any extra global variables, then define them here. */
static sem_t mutex;
static sem_t guy_with_tobacco;
static sem_t guy_with_matches;
static sem_t guy_with_paper;
int tobacco_on_the_table = false;
int matches_on_the_table = false;
int paper_on_the_table = false;


static void *agent(void *arg) {
  seed = pthread_self();

  while (true) {
    Sem_wait(&doneSmoking);

    int choice = rand_r(&seed) % 3;

    if (choice == 0) {
      Sem_post(&tobacco);
      Sem_post(&paper);
    } else if (choice == 1) {
      Sem_post(&tobacco);
      Sem_post(&matches);
    } else {
      Sem_post(&paper);
      Sem_post(&matches);
    }
  }

  return NULL;
}

static void tobacco_dealer(void){
  while(true){
    Sem_wait(&tobacco);  
    Sem_wait(&mutex);
    if(matches_on_the_table){
      matches_on_the_table = 0;
      Sem_post(&guy_with_paper);
    }
    else if(paper_on_the_table){
      paper_on_the_table = 0;
      Sem_post(&guy_with_matches);
    }
    else{
      tobacco_on_the_table = 1;
    }
    Sem_post(&mutex);
  }
}

static void matches_dealer(void){
  while(true){
    Sem_wait(&matches);
    Sem_wait(&mutex);
    if(tobacco_on_the_table){
      tobacco_on_the_table = 0;
      Sem_post(&guy_with_paper);
    }
    else if(paper_on_the_table){
      paper_on_the_table = 0;
      Sem_post(&guy_with_tobacco);
    }
    else{
      matches_on_the_table = 1;
    }
    Sem_post(&mutex);
  }
}

static void paper_dealer(void){
  while(true){
    Sem_wait(&paper);
    Sem_wait(&mutex);
    if(tobacco_on_the_table){
      tobacco_on_the_table = 0;
      Sem_post(&guy_with_matches);
    }
    else if(matches_on_the_table){
      matches_on_the_table = 0;
      Sem_post(&guy_with_tobacco);
    }
    else{
      paper_on_the_table = 1;
    }
    Sem_post(&mutex);
  }
}

/* TODO: If you need extra threads, then define their main procedures here. */

static void randsleep(void) {
  usleep(rand_r(&seed) % 1000 + 1000);
}

static void make_and_smoke(char smoker) {
  randsleep();
  Sem_post(&doneSmoking);
  outc(smoker);
  randsleep();
}

static void *smokerWithMatches(void *arg) {
  seed = pthread_self();

  while (true) {
    /* TODO: wait for paper and tobacco */
    Sem_wait(&guy_with_matches);
    make_and_smoke('M');
  }

  return NULL;
}

static void *smokerWithTobacco(void *arg) {
  seed = pthread_self();

  while (true) {
    /* TODO: wait for paper and matches */
    Sem_wait(&guy_with_tobacco);
    make_and_smoke('T');
  }

  return NULL;
}

static void *smokerWithPaper(void *arg) {
  seed = pthread_self();
 
  while (true) {
    /* TODO: wait for tobacco and matches */
    Sem_wait(&guy_with_paper);
    make_and_smoke('P');
  }

  return NULL;
}

int main(void) {
  Sem_init(&tobacco, 0, 0);
  Sem_init(&matches, 0, 0);
  Sem_init(&paper, 0, 0);
  Sem_init(&doneSmoking, 0, 1);
  
  /* TODO: Initialize your global variables here. */
  Sem_init(&mutex, 1, 1);
  Sem_init(&guy_with_paper, 1, 0);
  Sem_init(&guy_with_matches, 1, 0);
  Sem_init(&guy_with_tobacco, 1, 0);

  pthread_t agentThread;
  Pthread_create(&agentThread, NULL, agent, NULL);

  pthread_t smokerPaperThread, smokerMatchesThread, smokerTobaccoThread;
  Pthread_create(&smokerPaperThread, NULL, smokerWithPaper, NULL);
  Pthread_create(&smokerMatchesThread, NULL, smokerWithMatches, NULL);
  Pthread_create(&smokerTobaccoThread, NULL, smokerWithTobacco, NULL);

  pthread_t tobaccoDealer, paperDealer, matchesDealer;
  Pthread_create(&tobaccoDealer, NULL,  (void *)tobacco_dealer, NULL);
  Pthread_create(&matchesDealer, NULL, (void *)matches_dealer, NULL);
  Pthread_create(&paperDealer, NULL, (void *)paper_dealer, NULL);

  Pthread_join(agentThread, NULL);
  Pthread_join(smokerPaperThread, NULL);
  Pthread_join(smokerMatchesThread, NULL);
  Pthread_join(smokerTobaccoThread, NULL);

  Pthread_join(tobaccoDealer, NULL);
  Pthread_join(matchesDealer, NULL);
  Pthread_join(paperDealer, NULL);
  return 0;
}

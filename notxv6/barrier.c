#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

static int nthread = 1;
static int round = 0;
pthread_mutex_t lock;            // declare a lock

struct barrier {
  pthread_mutex_t barrier_mutex;
  pthread_cond_t barrier_cond;
  int nthread;      // Number of threads that have reached this round of the barrier
  int round;     // Barrier round
} bstate;

static void
barrier_init(void)
{
  assert(pthread_mutex_init(&bstate.barrier_mutex, NULL) == 0);
  assert(pthread_cond_init(&bstate.barrier_cond, NULL) == 0);
  bstate.nthread = 0;
}

static void barrier_destroy(void) {
  assert(pthread_mutex_destroy(&bstate.barrier_mutex) == 0);
  assert(pthread_cond_destroy(&bstate.barrier_cond) == 0);
}


static void 
barrier()
{
  // YOUR CODE HERE
  //
  // Block until all threads have called barrier() and
  // then increment bstate.round.
  //
  // Lock the barrier's mutex to ensure exclusive access
  pthread_mutex_lock(&bstate.barrier_mutex);
  
  if (++bstate.nthread < nthread) {
    // If not all threads have reached the barrier yet
    // Wait on the condition variable, releasing the lock
    pthread_cond_wait(&bstate.barrier_cond, &bstate.barrier_mutex);
  } else {
    // When all threads have reached the barrier
    // Reset the counter for the next round, increment the round, and signal all waiting threads
    bstate.nthread = 0;
    ++bstate.round;
    pthread_cond_broadcast(&bstate.barrier_cond);
  }
  
  // Release the lock on the barrier's mutex
  pthread_mutex_unlock(&bstate.barrier_mutex);
}

static void *
thread(void *xa)
{
  //long n = (long) xa;
  //long delay;
  int i;

  for (i = 0; i < 20000; i++) {
    int t = bstate.round;
    assert (i == t);
    barrier();
    usleep(random() % 100);
  }

  return 0;
}

int
main(int argc, char *argv[])
{
  pthread_t *tha;
  void *value;
  long i;
  //double t1, t0;
  pthread_mutex_init(&lock, NULL); // initialize the lock

  if (argc < 2) {
    fprintf(stderr, "%s: %s nthread\n", argv[0], argv[0]);
    exit(-1);
  }
  nthread = atoi(argv[1]);
  tha = malloc(sizeof(pthread_t) * nthread);
  srandom(0);

  barrier_init();

  for(i = 0; i < nthread; i++) {
    assert(pthread_create(&tha[i], NULL, thread, (void *) i) == 0);
  }
  for(i = 0; i < nthread; i++) {
    assert(pthread_join(tha[i], &value) == 0);
  }

  barrier_destroy();

  printf("OK; passed\n");
}

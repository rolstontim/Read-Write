/*Required Headers*/
//Tim Rolston #V00920780
//Code utilized and inspired from csc360 lecture slides

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "rw.h"
#include "resource.h"

/*
 * Declarations for reader-writer shared variables -- plus concurrency-control
 * variables -- must START here.
 */
sem_t r,w;
int num_reader, num_writer, num_reader_curr;
resource_t* d;

static resource_t data;

void initialize_readers_writer() {
    /*
     * Initialize the shared structures, including those used for
     * synchronization.
     */
     if (sem_init(&r, 0, 0) != 0){
       fprintf(stderr, "Error during init of r\n");
       exit(1);
     }

     num_reader = 0;
     num_writer = 0;
     num_reader_curr = 0;
     d = &data;

     if (sem_init(&w, 0, 0) != 0){
       fprintf(stderr, "Error during init of w\n");
       exit(1);
     }
}


void rw_read(char *value, int len) {

  num_reader++;
  while (!(num_writer) == 0 || !(num_reader_curr == 0)){

    sem_wait(&r);
  }
  num_reader_curr++;
  read_resource(d, value, len);
  num_reader_curr--;
  num_reader--;

  if (num_reader == 0){
    sem_post(&w);
  }
  sem_post(&r);

}


void rw_write(char *value, int len) {

  while (!(num_reader == 0) || num_writer == 1){

    sem_wait(&w);
  }
  num_writer++;

  write_resource(d, value, len);
  num_writer--;

  if(num_reader != 0){
    sem_post(&r);
  }
  sem_post(&w);

}

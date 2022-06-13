/*Required Headers*/
//Tim Rolston #V00920780
//Code utilized and inspired from csc360 lecture slides

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "meetup.h"
#include "resource.h"

/*
 * Declarations for barrier shared variables -- plus concurrency-control
 * variables -- must START here.
 */
 int count, size, meet, gen, this_gen, num_r, num_w, cur_writers;
 pthread_mutex_t m, lock_code;
 pthread_cond_t barrier_read, barrier_write;
 resource_t c;
 resource_t *code;

void initialize_meetup(int n, int mf) {
    char label[100];
    int i;

    if (n < 1) {
        fprintf(stderr, "Who are you kidding?\n");
        fprintf(stderr, "A meetup size of %d??\n", n);
        exit(1);
    }

    if (pthread_cond_init(&barrier_read, NULL) != 0){
      fprintf(stderr, "Error initializing condition variable\n");
      exit(1);
    }
    if (pthread_cond_init(&barrier_write, NULL) != 0){
      fprintf(stderr, "Error initializing condition variable\n");
      exit(1);
    }
    if (pthread_mutex_init(&m, NULL) != 0){
      fprintf(stderr, "Error initializing mutex\n");
      exit(1);
    }
    if (pthread_mutex_init(&lock_code, NULL) != 0){
      fprintf(stderr, "Error initializing mutex\n");
      exit(1);
    }

    count = 0;
    size = n;
    meet = mf;
    code = &c;
    gen = 0;
    this_gen = 0;
    num_r = 0;
    /*
     * Initialize the shared structures, including those used for
     * synchronization.
     */
}

/*Function to synchronize numerous input threads based on an input number and
condition. Function will set group of threads to have a certain code word in the
order recieved*/
void join_meetup(char *value, int len) {

  if (size == 1){
    return;
  }

  pthread_mutex_lock(&lock_code);

  pthread_mutex_lock(&m);
  count = count + 1;

//DO WHEN CODE IS GIVEN TO FIRST BOBA
  if (meet == MEET_FIRST){

    if (count == 1){ //PUT TO WRITER BARRIER
      pthread_mutex_unlock(&lock_code);
      cur_writers = 0;
      num_w = num_w + 1;
      while (cur_writers == 0){
        pthread_cond_wait(&barrier_write, &m);
      }
      count = 0;
      gen = gen + 1;
      write_resource(code, value, len);
      pthread_cond_broadcast(&barrier_read);
    }

    else{ //PUT TO READER BARRIER
      if (count < size){
        pthread_mutex_unlock(&lock_code);
        this_gen = gen;
        num_r = num_r + 1;
        while (this_gen == gen){
          pthread_cond_wait(&barrier_read, &m);
        }
      }
      else{
        cur_writers = 1;
        pthread_cond_signal(&barrier_write);
        this_gen = gen;
        num_r = num_r + 1;
        while (this_gen == gen){
          pthread_cond_wait(&barrier_read, &m);
        }
      }
      read_resource(code, value, len);
    }
    //printf("%d:NUMBER OF R, %d:NUMBER OF READS DONE\n", num_r, code->num_reads);
    //printf("%d:NUMBER OF W, %d:NUMBER OF WRITES DONE\n", num_w, code->num_writes);
    if ((num_r == code->num_reads) && (num_w == code->num_writes)){
      pthread_mutex_unlock(&lock_code);
    }
    pthread_mutex_unlock(&m);

  }

//DO WHEN CODE IS GIVEN TO LAST BOBA
  if (meet == MEET_LAST){

    if (count >= size){ //PUT TO WRITER BARRIER
      count = 0;
      gen = gen + 1;
      num_w = num_w + 1;
      write_resource(code, value, len);
      pthread_cond_broadcast(&barrier_read);
    }
    else{
      if (count < size){
        pthread_mutex_unlock(&lock_code);
        this_gen = gen;
        num_r = num_r + 1;
        while (this_gen == gen){
          pthread_cond_wait(&barrier_read, &m);
        }
      }
      read_resource(code, value, len);
    }
    //printf("%d:NUMBER OF R, %d:NUMBER OF READS DONE\n", num_r, code->num_reads);
    //printf("%d:NUMBER OF W, %d:NUMBER OF WRITES DONE\n", num_w, code->num_writes);
    if ((num_r == code->num_reads) && (num_w == code->num_writes)){
      pthread_mutex_unlock(&lock_code);
    }
    pthread_mutex_unlock(&m);
  }

}

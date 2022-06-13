
# CSC 360, March, 2021
## Assignment #2 submission

* Name: `Tim, Rolston`
* VNum: `V00920780`

---

### Description of code for task 1, `rw.c`

**This implementation of `rw.c` is reader priority**

#### `void initialize_readers_writer()`
This function initializes global variables for use within read and write
functions. As well as checks for proper initialization of *semaphore* 
variables. `r` for reading and `w` for writing. The `d` variable is a pointer
to the given `resource_t` variable so as to be able to use it when reading
and writing with the resource.

#### `void rw_read(char *value, int len)`
The purpose of this function is to funnel threads one at a time to use 
read_resource without a race condition occuring. This is done by first 
ensuring there are no threads actively writing or reading, done with a while
loop. If there are any threads currently reading/writing then we add that
thread to a queue to be completed when the current action is complete. Once 
the thread has completed reading it checks if another thread is in queue to
be read. If not then and only then can the write threads continue, other wise
keep reading the threads until the last one in the queue is read.

#### `void rw_write(char *value, int len)`
The purpose of this function is to funnel threads one at a time to use
write_resource() without a race condition occuring. Since `rw.c` has been
implemented with readers priority, first the thread must check if there are
any readers in queue or writers currently writing. This has been done with 
a while loop where if the conditions are true then the thread is added to 
a queue to be completed. After the writing has been completed the thread will
check if any new readers have been added into the reading queue. If not, ie
the reader queue is empty, then the next writer, if any, can proceed to write.

---

### Description of code for task 2 `meetup.c`

#### `void initialize_meetup(int n, int mf)`
This function ensures proper initialization of all global variables used
for the synchronization of `join_meetup` as well as the mutex and condition
variables used to create barriers. *code* is the pointer value to `reference_t`
which is used for `write_resource()` `read_resource()` as well as checking
the total number of reads and writes done using *code*.

#### `void join_meetup(char *value, int len)`
This functions purpose is to take a series of calls to a server, threads, and
give a specified amount of them, *n*, a value that one thread has, specified by
the user (`MEET_FIRST` or `MEET_LAST`). One thread write their *codeword*
into a `resource_t` variable and then the rest of *n-1* threads will read that
code word. Within the function I utilize a locking system with mutexes to 
ensure that during the synchronization of *n* threads that no other thread
is accidentally let in to modify specific values needed for the synchronization
to work correctly. Within the mutexes I have set up 2 condition variables that
act along with the mutexes to create barriers. One for reading and one for writing
The function is able to determine what thread is writing based off of both
the count and input specification. It then knows that the other *n-1* threads
must be readers. Using some code from the csc360 lecture slides, the last 
thread in to make *n* threads, either: 
*MEET_FIRST*
awakens the first thread to write, which will then write the codeword and
awaken the *n-1* read threads. Then once read, at the end of the function
the read threads will run through a test to see if they are the last thread
out of the critical section of the code. If so then the initial mutex 
`lock_code` will be unlocked and sequentially the mutex `m` will also be
unlocked for the next sereies or *generation* of threads to arrive.

*MEET_LAST*
will immediately write its code word and then awaken any waiting read 
threads to start reading the codeword. At the end just like in *MEET_FIRST*
there is a test to see if it is the last thread leaving the critical section
and if so unlock the initial mutex `lock_code` as well as after the mutex
`m` to get the next *n* generation of threads.

It should also be noted that a case at the beginning of the function is tested
if the size of each group is 1, then each input thread will return its own
*codeword*. Many global variables were used in both *MEET_FIRST* and
*MEET_LAST* for ensuring no spontaneous awakenings occur as well as continuity
and testing of the code.


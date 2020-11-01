/*
NEXT STEPS:
Figure out algorithms for deadlock handling (detection / avoidance)
 - I need to move the "obtained" array from tablerunner to an nXm global variable (for detection algo at least)
  - https://www.geeksforgeeks.org/dynamically-allocate-2d-array-c/
FINISH Request Function (may differ depending on detection or avoidance)
Join Threads back at the end
Split program into 2, one for each kind of deadlock handling
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "Queue.h"


int tipBox = 0;
sem_t tipUpdate;

pthread_t *tid;

int numTables;
int crayTypes;

int *availableCrayons;
sem_t crayonBox;

bool *tableOccupied;
bool start = false;

bool custBufferReady = true;
int custName = 0;
int *wants;

struct timespec req;

void *tableRunner(void *params);
int Request(int tableId, int crayonType);
int pickTable(int max); //picks an empty table to assign a customer to randomly
bool tablesFilled();

int main(int argc, char const *argv[]) {
  sem_init(&tipUpdate, 0, 1);
  sem_init(&crayonBox, 0, 1);

  srand48(time(0));

  FILE *f;
  f = fopen(argv[1], "r");

  //read in how many tables & number of crayon types
  fscanf(f, "%d", &numTables);
  fscanf(f, "%d", &crayTypes);

  tableOccupied = malloc(numTables*sizeof(bool));
  tid = malloc(numTables*sizeof(pthread_t));
  availableCrayons = malloc(crayTypes*sizeof(int));
  wants = malloc(crayTypes*sizeof(int));

  for (int j = 0; j < crayTypes; j++) {
    fscanf(f, "%d", &availableCrayons[j]);
    printf("%d\n", availableCrayons[j]);
  }


  int ars[numTables];
  for(int j = 0; j<numTables; j++) {
    ars[j] = j;
    pthread_create(&tid[j], NULL, tableRunner, &ars[j]);
  }

  while (!feof(f)) {
    while(tablesFilled()) {}

    int table = pickTable(numTables);
    while(!custBufferReady) {}

    fscanf(f, "%d", &custName);
    for(int j = 0; j < crayTypes; j++) {
      fscanf(f, "%d", &wants[j]);
    }
    tableOccupied[table] = true;
    custBufferReady = false;

    if(!start)
      if(tablesFilled())
        start = true;
  }

//TODODODODODODODODODOD FINISH JOINING THREADS BACK AT THE ENDDDDDDDDDDDD

  free(tableOccupied);
  free(tid);
  free(availableCrayons);
  free(wants);

  return 0;
}


Each subsequent line denotesa customer. Each line has k+1 numbers. The first number is
the customer number (integer), the next k numbers denote the number of crayons for each type that
the customer needs. The customer number may not be unique, and you are not required to check for uniqueness.


bool tablesFilled() {
  for(int j = 0; j < numTables; j++)
    if(!tableOccupied)
      return false;

  return true;
}

/*
The following function is used to randomly seat customers at the
available tables. I chose to make it random because I didn't want to
run into a problem where some tables are never used (which could
happen if tables at "lower indeces/threads" serve customers faster than
other indeces/threads)
*/
int pickTable(int max) {
  Queue *indeces = malloc(sizeof(Queue));
  initialize(indeces, max);

  for (int j = 0; j < max; j++) {
    if (!tableOccupied[j])
      enQueue(indeces, j);
  }
//handle if all tables are full
  if (getCount(indeces)==0)
    return -1;

  int ind = lrand48()%getCount(indeces);

  for (int j = 0; j < ind; j++) {
    deQueue(indeces);
  }
  int ret = deQueue(indeces);
  free(indeces);
  return ret;
} //end pickTable

/*
This function simulates a table, and is the main function used by the
threads spawned in the program. Threads wait for customers to be sitting at
every table. Then, customers will have the opportunity to get the crayons
they want in order to draw/color
*/
void *tableRunner(void *params) {
  int *args = (int*)params;
  int index = args[0];

  //TODO Figure out when to stop the function
  while(!done) {
    while(!tableOccupied[index]) { if(done) return; }

    int cust = custName;
    int obtained[crayTypes];  //array of how many crayons of each type a customer has received
    int wantC[crayTypes];     //array of how many crayons of each type a customer still needs
    int totalC = 0;           //keep track of how many crayons (of any type) customer still needs
    for(int j = 0; j < crayTypes; j++) {
      wantC[j] = wants[j];
      totalC += wants[j];
      obtained[j] = 0;
    }
    printf("Customer <%d> is seated at table <%d>\n", custName, index);

    if(tablesFilled)
      start = true;

    while(!start) {}

    //TODODODODODODODODODOD Finish picking crayons & requesting them
    int toReq = -1; //integer to represent what crayon type a customer is going to try and select

    /*while loop to select a random crayon that a customer still wants.
    The loop is used to not allow for customers to try requesting crayons
    which they don't need anymore of*/
    do {
      toReq = lrand48()%crayTypes;
    } while(wantC[toReq] == 0);

    printf("Customer <%d> request crayon type <%d>\n", custName, toReq);

    req.tv_sec = 0;
    req.tv_nsec = lrand48()%1000 + 1;
    nanosleep(&req, NULL);

//TODODODODODODODODODOD MAKE REQUEST HERE


    if(totalC == 0) {
      printf("Customer <%d> received all the crayons\n", custName);
      //sleep for 1-3 seconds
      req.tv_sec = 0;
      req.tv_nsec = lrand48()%2001 + 1000;
      nanosleep(&req, NULL);

      //create a random tip amount, get mutex for tipBox, and then deposit tip
      int tip = lrand48()%10 + 1;
      sem_wait(&tipUpdate);
      tipBox += tip;
      sem_post(&tipUpdate);
      printf("Customer <%d> leaves a tip of <%d>\n", custName, tip);

    }

    //TODODODODODODODODODOD Return crayons to the bin

    //signify customer is leaving
    tableOccupied[index] = false;

  }

}

//                                              ORTJOEIJSDOLJFSDL FJSDOFSJDF OSDJFOIS DJFSODJF SDO DJFOISJ
int Request(int tableId, int crayonType) {
  sem_wait(&crayonBox);

  int accden = 0; //signifies whether request was accepted or denied

  //if there are no more crayons of given type, deny request
  //Otherwise, subtract 1 from crayon type, and allow request
  if(availableCrayons[crayonType] == 0) {
    //deny request
  } else {
    availableCrayons[crayonType] -= 1;
    accden = 1;
  }

  sem_post(&crayonBox);
  return accden;
}

/*
NEXT STEPS:
Figure out algorithms for deadlock handling (detection / avoidance)
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


int tipBox = 0; //The total for the tipbox where customers pay money
sem_t tipUpdate; //Semaphor for using the tixBox variable

pthread_t *tid; //Array of thread id's for each table thread

int numTables; //How many tables are at the place
int crayTypes; //How many types of crayons are there

int *availableCrayons; //Array to hold how many crayons of each type are available
sem_t crayonBox; //Semaphore for using the availableCrayons array

bool *tableOccupied; //Array to tell when a table is occupied or not
bool start = false; //Boolean to tell tables when they are all full (and customers can start asking for crayons)
bool done = false; //Boolean to signal when the art businesss is ready to close

bool custBufferReady = true; //Boolean to tell threads that it is safe to pull customer information from global variables
int custName = 0; //Stores the name of a customer that is looking to sit at a table
int **wants; //Multi-dimensional array that holds what crayons each customer at a table currently wants
int **maxNeeds; //Multi-dimensional arrray that holds the max number of crayons of each type a customer may need at any given time
int **obtained; //Multi-dimensional array that holds how many crayons of each type a customer has received
int **request; //Multi-dimensional array that holds if a customer is requesting a type of crayon

struct timespec req; //timespec used to make a customer sleep



//The functions defined below are explained when they are implemented
void *tableRunner(void *params);
int Request(int tableId, int crayonType);
int pickTable(int max);
bool tablesFilled();

int main(int argc, char const *argv[]) {
  sem_init(&tipUpdate, 0, 1);
  sem_init(&crayonBox, 0, 1);

  srand48(time(0));

  FILE *f; //Variable to hold the opened file
  f = fopen(argv[1], "r");

  //read in how many tables & number of crayon types
  fscanf(f, "%d", &numTables);
  fscanf(f, "%d", &crayTypes);

  tableOccupied = malloc(numTables * sizeof(bool));
  tid = malloc(numTables * sizeof(pthread_t));
  availableCrayons = malloc(crayTypes * sizeof(int));

  wants = (int **)malloc(numTables * sizeof(int *));
  /*
  this allocates memory for the array of crayons each table wants using
  the number of crayon types. It loops a number of times equal to the number of
  tables
  */
  for(int j = 0; j < numTables; j++) {
    wants[j] = (int *)malloc(crayTypes * sizeof(int));
  }

  obtained = (int **)malloc(numTables * sizeof(int *));
  /*
  this allocates memory for the array of crayons each occupied table
  has obtained. It loops a number of times equal to the number of tables
  */
  for(int j = 0; j < numTables; j++) {
    obtained[j] = (int *)malloc(crayTypes * sizeof(int));
  }

  request = (int **)malloc(numTables * sizeof(int *));
  /*
  this allocates memory for the array of crayons each occupied table
  is requesting. It loops a number of times equal to the number of tables
  */
  for(int j = 0; j < numTables; j++) {
    request[j] = (int *)malloc(crayTypes * sizeof(int));
  }


  maxNeeds = (int **)malloc(numTables * sizeof(int *));
  /*
  this allocates memory for an array of the max number of each type of crayons
  each occupied table may request. It loops a number of times equal to the
  number of tables
  */
  for(int j = 0; j < numTables; j++) {
    maxNeeds[j] = (int *)malloc(crayTypes * sizeof(int));
  }

  /*
  This reads in from the input file how many of each crayon type are available
  at the business. It loops a number of times equal to the number of types of
  crayons
  */
  for (int j = 0; j < crayTypes; j++) {
    fscanf(f, "%d", &availableCrayons[j]);
  }


  int ars[numTables];
  /*
  This creates the threads which will simulate all the tables for the business.
  It loops a number of times equal to the number of tables
  */
  for(int j = 0; j<numTables; j++) {
    ars[j] = j;
    pthread_create(&tid[j], NULL, tableRunner, &ars[j]);
  }

  /*
  While loop that continues while there are still lines to read in the input
  file. It deals with the input and assigns customers to tables
  */
  while (!feof(f)) {
    //While all tables are filled, do nothing. When a table becomes empty it breaks

    int table = pickTable(numTables); //This number corresponds to the index of a table where a customer is assigned

    /*
    while the buffer for customer information is still waiting to be read, do
    nothing. When the buffer is ready for a new customer, break
    */
    while(!custBufferReady) {}

    fscanf(f, "%d", &custName);

    /*
    For a customer being sat at a table, loop to read in the number of each
    type of crayon they require for their art. Loops a number of times equal to
    the number of types of crayons
    */
    for(int j = 0; j < crayTypes; j++) {
      fscanf(f, "%d", &wants[table][j]);
      maxNeeds[table][j] = wants[table][j];
      obtained[table][j] = 0;
    }

    tableOccupied[table] = true;
    custBufferReady = false;

    /*
    checks if all tables are occupied and the business can start. When the
    business has not started, this will check if all tables are full, if they
    are, it will signal that customers can start requesting crayons
    */
    if(tablesFilled())
      start = true;

    while(tablesFilled()) {}
  }

  done = true;

  /*
  This loop joins all of the created threads back to the main thread.
  It loops equal to the number of tables in the business
  */
  for (int j = 0; j < numTables; j++) {
    pthread_join(tid[j], NULL);
  }

  free(tableOccupied);
  free(tid);
  free(availableCrayons);
  free(wants);
  free(obtained);
  free(request);

  return 0;
} //end of main() function



/*
This function will check if all of the tables are occupied or not
*/
bool tablesFilled() {
  /*
  Checks all the tables to see if on is not occupied. Max loops a number of
  times equal to the number of tables. Will exit immediately if an unoccupied
  table is found
  */
  for(int j = 0; j < numTables; j++)
    //Checks if a table isn't occupied or not
    if(!tableOccupied[j])
      return false;

  return true;
} //end of tablesFilled() function




/*
The following function is used to randomly seat customers at the
available tables. I chose to make it random because I didn't want to
run into a problem where some tables are never used (which could
happen if tables at "lower indeces/threads" serve customers faster than
other indeces/threads)
*/
int pickTable(int max) {
  Queue *indeces = malloc(sizeof(Queue)); //A queue to hold what table indeces are available at a given time
  initialize(indeces, max);

  /*
  loops through all the tables to see which tables are not currently occupied
  */
  for (int j = 0; j < max; j++) {
    //checks if a table isn't occupied. If so, it adds that table to a list
    if (!tableOccupied[j])
      enQueue(indeces, j);
  }

  //handle if all tables are full
  if (getCount(indeces)==0)
    return -1;

  int ind = lrand48()%getCount(indeces); //number that represents a randomly selected empty table

  /*
  If multiple tables aren't occupied, get rid of a random number of tables to
  find the table a customer will be assigned to
  */
  for (int j = 0; j < ind; j++) {
    deQueue(indeces);
  }
  int ret = deQueue(indeces); //represents the final table which a customer will be assigned to
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
  int *args = (int*)params; //casts the input parameter(s)
  int index = args[0]; //the input parameter used is an index that refers to the table number where a customer is sat

  //TODO Figure out when to stop the function
  /*
  While loop for running the table while the business is open. When the main
  thread signals that the business is closing, it'll stop after the current customer leaves
  */
  while(!done) {
    //Stalls while a customer is not sitting at the table. Breaks when a customer does sit down
    while(!tableOccupied[index]) { if(done) return; }

    int cust = custName;      //The customer's "name"
    int totalC = 0;           //keep track of how many total crayons (of any type) customer still needs

    /*
    Loops through the crayon types and accumulates a total number of crayons which a customer wants
    */
    for(int j = 0; j < crayTypes; j++) {
      totalC += wants[index][j];
    }
    printf("Customer <%d> is seated at table <%d>\n", cust, index);
    custBufferReady = true;

    //If after sitting a customer at the table, all tables are filled, start the business
    if(tablesFilled)
      start = true;

    //Stall the table while the business hasn't started
    while(!start) {}

    //While loop for simulating a customer at the table. Ends when the customer leaves
    while(tableOccupied[index]) {

      //TODODODODODODODODODOD MAKE REQUEST HERE
      int s = 0; //integer signifying whether a request was allowed or not
      do {
        int toReq = -1; //integer to represent what crayon type a customer is going to try and select

        /*
        while loop to select a random crayon that a customer still wants.
        The loop is used to not allow for customers to try requesting crayons
        which they don't need anymore of
        */
        do {
          toReq = lrand48()%crayTypes;
        } while(wants[index][toReq] == 0);

        printf("Customer <%d> request crayon type <%d>\n", cust, toReq);

        s = Request(index, toReq);
        if(s == 0)
          printf("Request denied\n");
      } while(s == 0);
      printf("Request accepted\n");
      totalC--;



      req.tv_sec = 0;
      req.tv_nsec = lrand48()%1000 + 1;
      nanosleep(&req, NULL);

      /*
      Checks if a customer has obtained all the crayons which they desire. If so,
      they'll complete their art and leave
      */
      if(totalC == 0) {
        printf("Customer <%d> received all the crayons\n", cust);
        //sleep for 1-3 seconds
        req.tv_sec = 0;
        req.tv_nsec = lrand48()%2001 + 1000;
        nanosleep(&req, NULL);

        //create a random tip amount, get mutex for tipBox, and then deposit tip
        int tip = lrand48()%10 + 1; //How much a customer is going to tip the business
        sem_wait(&tipUpdate);
        tipBox += tip;
        sem_post(&tipUpdate);
        printf("Customer <%d> leaves a tip of <%d>\n", cust, tip);

        //TODODODODODODODODODOD Return crayons to the bin

        /*
        for loop to return crayons which a customer has used. Loops equal to the
        number of types of crayons
        */
        for(int j = 0; j<crayTypes; j++) {
          availableCrayons[j] += obtained[index][j];
        }

        //signify customer is leaving
        tableOccupied[index] = false;
      } //end if customer is done at the table
    }//end while customer is at the table


  } //end while !done

} //end TableRunner function




//                                              ORTJOEIJSDOLJFSDL FJSDOFSJDF OSDJFOIS DJFSODJF SDO DJFOISJ
/*
This function takes care of requests a table is making for a type of crayon.
Each request is for 1 instance of a specific crayon
*/
int Request(int tableId, int crayonType) {
  printf("Request is being made...\n");
  printf("Tipbox currently contains: $%d\n", tipBox);
  printf("List of Crayons still available: ");
  for(int k = 0; k < crayTypes; k++) {
    printf("%d, ", availableCrayons[k]);
  }
  printf("\nCrayons still wanted by tables in order: ");
  for(int j = 0; j < numTables; j++) {
    printf("Table %d: ", j);
    for(int k = 0; k < crayTypes; k++) {
      printf("%d, ", wants[j][k]);
    }
    printf("\n");
  }

  printf("\nCrayons that have been obtained by tables in order");
  for(int j = 0; j < numTables; j++) {
    printf("Table %d: ", j);
    for(int k = 0; k < crayTypes; k++) {
      printf("%d, ", obtained[j][k]);
    }
    printf("\n");
  }

  sem_wait(&crayonBox);

  request[tableId][crayonType] = 1; //this might go outside of the semaphore
  int accden = 0; //signifies whether request was accepted or denied

  //if there are no more crayons of given type, deny request
  //Otherwise, check if allowing request would result in a deadlock
  if(availableCrayons[crayonType] == 0) {
    //deny request due to insufficient resources
  } else {
    //pretend to give crayon to customer to see if deadlock occurs
    availableCrayons[crayonType] -= 1;
    wants[tableId][crayonType] -= 1;
    obtained[tableId][crayonType] += 1;

    int work[crayTypes];
    for(int j = 0; j < crayTypes; j++) {
      work[j] = availableCrayons[j];
    }

    bool finish[numTables];
    bool flag = false;
    for(int j = 0; j < numTables; j++) {

      for(int k = 0; k < crayTypes; k++) {
        if(request[j][k] > 0) {
          flag = true;
          break;
        } //end of if
      } //end of inner for loop
      if(flag)
        finish[j] = false;
      else
        finish[j] = true;
    } //end of outer for loop

    flag = false;
    for(int j = 0; j < numTables; j++) {
      bool flag_two = false;
      for(int k = 0; k < crayTypes; k++) {
        if(request[j][k] > work[k])
          flag_two = true;
      } //end of inner forloop

      if(!finish[j] && !flag_two)
        flag = true;
    } //end of outer forloop

    bool flag_three = false;
    if(!flag) {
      for(int j = 0; j < numTables; j++) {
        if(!finish[j]) {
          flag_three = true;
        }
      }
    }

    /*
    The flag_three will only trigger if a deadlock was found, in which case
    we undo the crayon request
    */
    if(flag_three) {
      //undo the request
      availableCrayons[crayonType] += 1;
      wants[tableId][crayonType] += 1;
      obtained[tableId][crayonType] -= 1;
    } else {
      //request is being granted, so don't undo anything
      accden = 1;
    }
  }

  sem_post(&crayonBox);
  return accden;
} //end of Request function

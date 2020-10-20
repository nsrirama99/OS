#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "Queue.h"

//there are likely other global variables needed to communicate
//between main and the threads, figure it out

//TODO ADD ALL THE PRINT STATEMENTS HE WANTS FOR DOCUMENTATION

bool isOpen = false;
int tipBox = 0;
Queue *Foodqueue;
bool done = false;
int numTables = 0;

pthread_t *tid;


bool *tableOccupied; //array to see if table is occupied
bool *tableDesire; //array to see if table still MAY want the food currently at the front of queue
sem_t useQ;
sem_t tipUpdate;

//customer info
bool custBufferReady = true;
int custName = 0;
int numDish = 0;
int *wants;


void createDishes(int n); //makes dishes
void *tableRunner(void *params); //runs the threaded tables
void *trashRunner(void *params); //runs thread that is in charge of getting rid of dishes if nobody wants them
int pickTable(int max); //picks an empty table to assign a customer to randomly

int main(int argc, char *argv[]) {
    sem_init(&useQ, 0, 1);
    sem_init(&tipUpdate, 0, 1);
//    sem_init(&rw_mutex, 0, 1);
//    sem_init(&atomic_read, 0, 1);

    srand48(time(0));

    if(argc > 1) {
       int foodCap = 0;

       FILE *f;
       f = fopen(argv[1], "r");
	     fscanf(f, "%d", &numTables);
       fscanf(f, "%d", &foodCap);

       Foodqueue = malloc(sizeof(Queue));
       initialize(Foodqueue, foodCap);

       tid = malloc(numTables * sizeof(pthread_t));
       tableOccupied = malloc(numTables * sizeof(bool));
       tableDesire = malloc(numTables * sizeof(bool));

       int is[numTables];
	     for(int j = 0; j < numTables; j++) {
	       tableOccupied[j] = false; //default table unoccupied
	       tableDesire[j] = true; //default table doesn't care about food

//possible arguments: int for process id (j value in for loop)
//TODO: FINISH CORRECTLY MAKING THREADS
//arguments  = j, ??
         is[j] = j;
	       pthread_create(&tid[j], NULL, tableRunner, &is[j]);
       }

//create another thread specifically for checking if food should be thrown out
       pthread_t trashid;
       pthread_create(&trashid, NULL, trashRunner, NULL);

       char op;

       while(!feof(f)) {
         fscanf(f, "%c", &op);

	       if(op == 'C') {
//TODO finish dealing with customers
          while(!custBufferReady) {}
          int table = pickTable(numTables);
          //as long as there's an open table, try to seat the customer
          if(table != -1) {

  	        fscanf(f, "%d", &custName);
  	        printf("C %d \n", custName);
  	        fscanf(f, "%d", &numDish);

  	        wants = malloc(numDish * sizeof(int));
            for(int j = 0; j < numDish; j++) {
              fscanf(f, "%d", &wants[j]);
            }

            tableOccupied[table] = true;
            custBufferReady = false;
          } else {
            int temp;
            int temp2;
            char *tempBuffer[300];
            fscanf(f, "%d %d", &temp, &temp2);
            printf("C %d\n", temp);
            fscanf(f, "%[^\n]%*c", &tempBuffer);
            printf("<%d> dropped\n", temp);
          }

        } else if(op == 'P') {
	         int numDish = 0;
	         fscanf(f, "%d", &numDish);
	         printf("P %d \n", numDish);
	         createDishes(numDish);
	      } else if(op == 'O') {
	         printf("O \n");
           isOpen = !isOpen;
	      } //end of if statements for lines
	    } //end of file was reached

//Signal for closing of restaurant
      done = true;

//Let all threads come to an end and be joined back to main
      for (int j = 0; j < numTables; j++) {
        pthread_join(tid[j], NULL);
      }
      //join the queue trasher thread back into main
      pthread_join(trashid, NULL);


      printf("Total Tip Collected: %d\n", tipBox);
//FREE ALL USED MEMORY ALLOCATIONS
      free(Foodqueue);
      free(tid);
      free(tableOccupied);
      free(tableDesire);


    } else {
	     printf("There was a problem with your command line argument \n");
    }

    //let threads finish, kill w/ pthread_exit(NULL); <-- this doesn't work
    return 0;
}

void createDishes(int n) {
    int dish = 0;
    for(int j = 0; j < n; j++) {
	     dish = lrand48() % 10;
       sem_wait(&useQ);
	     if(!isFull(Foodqueue)) {
	        enQueue(Foodqueue, dish);
	        printf("Dish <%d> produced \n", dish);
	     } else {
          isOpen = true;
          bool flag = false;
          for (int j = 0; j < numTables; j++) {
            if(tableDesire[j] == true && tableOccupied[j] == true) {
              flag = true;
              break;
            }
          }
          if(!flag) {
            printf("Dish <%d> dropped\n", deQueue(Foodqueue));
            for (int j = 0; j < numTables; j++) {
              tableDesire[j] = true;
            }
  	        enQueue(Foodqueue, dish);
  	        printf("Dish <%d> produced \n", dish);
          }
	     } //end else (if queue is full)
       sem_post(&useQ);
    }
} //end createDishes

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


void *trashRunner(void *params) {
  while(true) {
    if(!isEmpty(Foodqueue)) {

      bool flag = false;
      for (int j = 0; j < numTables; j++) {
        if(tableDesire[j] == true && tableOccupied[j] == true) {
          flag = true;
          break;
        }
      }

      if(!flag) {
        printf("Dish <%d> dropped\n", deQueue(Foodqueue));
//        deQueue(Foodqueue);
        for (int j = 0; j < numTables; j++) {
          tableDesire[j] = true;
        } //end forloop
      } //end if !flag

    } //end !isEmpty

    if(done)
      break;
  }
}

void *tableRunner(void *params) {
//TODO figure out what params there are
    int *args = (int*)params;
    int index = args[0];
    while(!done) {
      while(!tableOccupied[index]) { if (done) return; }

      //when customer arrives at table, create 1 dish and add to queue
      createDishes(1);

      //get the customer info
      tableDesire[index] = true;
      int cust = custName;
      int num = numDish;
      int wantedDishes[num];
      bool tookDish[num];
      for (int j = 0; j < num; j++) {
          wantedDishes[j] = wants[j];
          tookDish[j] = false;
      }
      free(wants);
      printf("<%d> assigned to <%d>\n", cust, (int)tid[index]);


      custBufferReady = true;

      int left = num;
      while(left > 0) {
	         while(!isOpen) {}

	         sem_wait(&useQ);

	          //int dishInLine = peek(Foodqueue);
	         bool took = false;
	         for(int j = 0; j < num; j++) {
//              printf("DISH %d ON TOP\n", peek(Foodqueue));
//              printf("Customer %d looking for %d\n", cust, wantedDishes[j]);
	            if(peek(Foodqueue) == wantedDishes[j] && !tookDish[j]) {
                  deQueue(Foodqueue);
	                took = true;
	                tookDish[j] = true;
	                left -= 1;

                  //reset all other table's desires since the front of queue has changed
                  for (int j = 0; j < numTables; j++) {
                    if(j != index)
                      tableDesire[j] = true;
                  }

                  printf("<%d> picked up <%d>\n", (int)tid[index], wantedDishes[j]);

                  struct timespec req;
	                req.tv_sec = 0;
	                req.tv_nsec = lrand48()%5000 + 1;
	                nanosleep(&req, NULL);

	                break;
	            } //end of if statement
	         } //end of forloop of wantedfood

	         if(!took) {
              //printf("DIDNT FIND FOOD CUST %d\n", cust); this works
	            tableDesire[index] = false;
	         }

	         sem_post(&useQ);
      }//customer got all the food they wanted at this point

      int tip = lrand48()%10 + 1;

      sem_wait(&tipUpdate);
      tipBox += tip;
      sem_post(&tipUpdate);
      printf("<%d> leaving <%d>\n", cust, tip);

      tableOccupied[index] = false;

    }//Thread is done

} //end of tableRunner

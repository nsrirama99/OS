#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Queue.h"

//there are likely other global variables needed to communicate between main and the threads,
//figure it out

bool isOpen = false;
int tipBox = 0;
Queue *q;

void tryMe();

int main(int argc, char *argv[]) {
//    Queue *q;
    q = malloc(sizeof(Queue));
    initialize(q,1);

    alterSize(q, 3);
    enQueue(q, 1);
    enQueue(q, 5);
    printf("%d\n", peek(q));
    printf("%d\n", deQueue(q));
    printf("%d\n", peek(q));

    tryMe();
    deQueue(q);
    deQueue(q);
    printf("%d\n", peek(q));

    return 0;
}


void tryMe() {
   enQueue(q, 7);
   enQueue(q, 8);
   enQueue(q, 9);
}






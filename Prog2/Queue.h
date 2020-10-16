#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//Including code to create the FIFO queue within this 
//program instead of making a separate program
struct node {
    int data;
    struct node *next;
};
typedef struct node Node;

struct queue {
    int count;
    int maxSize;
    Node *front;
    Node *rear;
};
typedef struct queue Queue;

//void initialize(Queue *q, int size);
//void alterSize(Queue *q, int size);
//void enQueue(Queue *q, int value);
//int deQueue(Queue *q);
//bool isEmpty(Queue *q);
//bool isFull(Queue *q);
//int peek(Queue *q);

//FIFO queue implemented functions
void initialize(Queue *q, int size) {
   q->count = 0;
   q->maxSize = size;
   q->front = NULL;
   q->rear = NULL;
}

bool isEmpty(Queue *q) { return (q->front == NULL); }
bool isFull(Queue *q) { return (q->count == q->maxSize); }
int peek(Queue *q) { return (q->front->data); }

void enQueue(Queue *q, int value) {
    Node *tmp;
    tmp = malloc(sizeof(Node));
    tmp->data = value;
    tmp->next = NULL;

    if(!isEmpty(q)) {
	q->rear->next = tmp;
	q->rear = tmp;
    } else {
	q->front = q->rear = tmp;
    }

    q->count++;
}


int deQueue(Queue *q) {
    Node *tmp;
    int n = q->front->data;
    tmp = q->front;
    q->front = q->front->next;
    q->count--;
    free(tmp);
    return(n);
}

void alterSize(Queue *q, int size) {
    q->maxSize = size;
}


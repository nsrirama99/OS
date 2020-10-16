#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

void *runner(void *param);

enum {SIZE = 10000};

int counts[8] = {0,0,0,0,0,0,0,0};
char file[SIZE];
int flength;

int main(void) {
	pthread_t tid;
	pthread_attr_t attr;

	char fileName[100];

	printf("Hello, please enter the name of a file to read:\n");
	gets(fileName);

	FILE *fp = fopen(fileName, "r");
	size_t code = fread(file, sizeof *file, SIZE, fp);
	
	flength = strlen(file);	
	pthread_attr_init(&attr);
	int args[] = {0, flength};	

	for(int j = 2; j <= 9; j++) {
		args[0] = j;
		pthread_create(&tid, &attr, runner, args);		
		pthread_join(tid, NULL);
	}


	printf("(");
	for(int j = 0; j < 8; j++) {
	  printf("%d", counts[j]);
	  if(j < 7)
	    printf(", ");
	}
	printf(")\n");
}

void *runner(void *param) {
	//cast parameters here
	int *args = (int*)param;
	int divisor = args[0];
	pthread_t myid = pthread_self();
	
	for(int k = 0; k < flength; k++) {
		if(isdigit(file[k])) {
			int num = file[k] - '0';
			int temp = num%divisor;
			if(temp == 0) {
				counts[divisor-2]+=1;
			}
		}

	}
	pthread_exit(0);
}

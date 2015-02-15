/**
 ** Sarracini
 ** Ursula
 ** Section Z
 ** 211535432
 ** CSE13208
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "helper.h"

// some useful global variables
Process processes[MAX_PROCESSES+1];
int numberOfProcesses = 0;
int nextProcess = 0;
int totalWaitingTime = 0;
int totalContextSwitches = 0;
int cpuTimeUtilized = 0;
int simulationTime = 0;
int sumTurnarounds = 0;

Process_queue readyQueue;
Process_queue waitingQueue;

Process *CPUS[NUMBER_OF_PROCESSORS];

/* to do:
- implementation for next scheduled process function (dequeues next schedules process from ready queue)
- implmentation for function for enqueuing new processes in ready queue
- implmentation for function for moving waiting processes to ready after i/o burst done
- implmentation for function for moving ready process into running state
- implmentation for function for moving from running to waiting
- functions to updates states (waiting, ready, running)
- clean up code and add comments
*/
void error(char *message){
	fprintf(stderr, "%s\n", message);
	exit(-1);
}

Process_node *createProcessNode(Process *p){
	Process_node *node = (Process_node*)malloc(sizeof(Process_node));
	if (node == NULL){
		error("out of memory");
	}
	node->data = p;
	node->next = NULL;
	return node;
}

void initializeProcessQueue(Process_queue *q){
	q->front = q->back = NULL;
	q->size = 0;
}

void enqueueProcess(Process_queue *q, Process *p){
	Process_node *node = createProcessNode(p);
	if (q->front == NULL){
		assert(q->back == NULL);
		q->front = q->back = node;
	}
	else{
		assert(q->back != NULL);
		q->back->next = node;
		q->back = node;
	}
	q->size++;
}

void dequeueProcess(Process_queue *q) {
    Process_node *deleted = q->front;
    assert(q->size > 0);
    if (q->size == 1) {
        q->front = NULL;
        q->back = NULL;
    } else {
        assert(q->front->next != NULL);
        q->front = q->front->next;
    }
    free(deleted);
    q->size--;  
}

float averageWaitTime(int theWait){
	float result = 0.0;
	result = theWait / (double) numberOfProcesses;
	return result;
}

float averageTurnaroundTime(int theTurnaround){
	float result = 0.0;
	result = theTurnaround / (double) numberOfProcesses;
	return result;
}
 
float averageUtilizationTime(int theUtilization){
 	float result = 0.0;
 	result = 100.0 * theUtilization / simulationTime;
 	return result;
 }
// sort arrival times to get the next arrival time
int compareArrivalTime(const void *a, const void *b){
	Process *first = (Process *) a;
	Process *second = (Process *) b;
	if (first->arrivalTime < second->arrivalTime){
		return -1;
	}
	if (first->arrivalTime > second->arrivalTime){
		return 1;
	}
	return 0;
}

// returns if cpu is free or not for next process, also keeps track of total number of running proceeses (for cpu utilization calculation later)
int runningProcesses(void){
	int runningProcesses = 0;
	int i;
	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
		if (CPUS[i] != NULL){
			runningProcesses++;
		}
	}
	return runningProcesses;
}

Process *nextScheduledProcess(void){
	//if ready queue == 0 return null bc there is nothing to process
	//else take the next process in the queue and dequeue that process from the ready queue
	return NULL;
}

void addNewIncomingProcess(void){
	// place incoming processes into end of ready queue
}

// checks to see how many more processes are left to process
int totalIncomingProcesses(void){
	return numberOfProcesses - nextProcess;
}

 void waitingToReady(void){
 	// after they're done their i/o burst move them to ready
 }

 void readyToRunning(void){
 	// move from ready to running once cpu is free by calling nextScheduledProcess
 }
 void runningToWaiting(void){
 	// move from running to waiting while i/o burst is happening
 }

void displayResults(float awt, float atat, int sim, float aut, int cs){
	printf( "Average waiting time:   %.2f units\n"
			"Average turnaround time:   %.2f units\n"
			"Time CPU finished all processes:   %d\n"
			"Average CPU utilization:   %.2f units\n"
			"Number of context Switces:   %d\n", 
			awt, atat, sim, aut, cs);
}

int main(){
	int i;
	int status = 0;
	float ut, wt, tat;
	
	// clear CPU'S, and initialize queues
	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
		CPUS[i] = NULL;
	}
	initializeProcessQueue(&readyQueue);
	initializeProcessQueue(&waitingQueue);

	while(status = readProcess(&processes[numberOfProcesses])){
		if (status == 1){
			numberOfProcesses++;
		}
	}

	qsort(processes, numberOfProcesses, sizeof(Process), compareArrivalTime);
	
	// main execution loop
	while (1){
		addNewIncomingProcess();
		runningToWaiting();
		waitingToReady();
		readyToRunning();

		if (runningProcesses() == 0 && totalIncomingProcesses() == 0 && waitingQueue.size == 0){
			break;
		}
		simulationTime++;
	}

	// calculations
	for(i = 0; i < numberOfProcesses; i++){
		sumTurnarounds +=processes[i].endTime - processes[i].arrivalTime;
		totalWaitingTime += processes[i].waitingTime;
	}
	cpuTimeUtilized += runningProcesses();
	wt = averageWaitTime(totalWaitingTime);
	tat = averageTurnaroundTime(sumTurnarounds);
	ut = averageWaitTime(cpuTimeUtilized);
	
	displayResults(wt, tat, simulationTime, ut, totalContextSwitches);
	return 0;
}
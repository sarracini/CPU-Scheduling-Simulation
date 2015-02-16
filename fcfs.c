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
int theClock = 0;
int sumTurnarounds = 0;

Process_queue readyQueue;
Process_queue waitingQueue;

Process *CPUS[NUMBER_OF_PROCESSORS];

Process *tmpQueue[MAX_PROCESSES+1];
int tmpQueueSize;

/* to do:
- implmentation for function for enqueuing new processes in ready queue
- implmentation for function for moving ready process into running state
- implmentation for function for moving from running to waiting
- functions to updates states (waiting, ready, running)
- clean up code and add comments
*/

Process_node *createProcessNode(Process *p){
	Process_node *node = (Process_node*)malloc(sizeof(Process_node));
	if (node == NULL){
		error("out of memory");
	}
	node->data = p;
	node->next = NULL;
	return node;
}

void resetVariables(void){
	numberOfProcesses = 0;
	nextProcess = 0;
	totalWaitingTime = 0;
	totalContextSwitches = 0;
	cpuTimeUtilized = 0;
	theClock = 0;
	sumTurnarounds = 0;
	tmpQueueSize = 0;
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
	float result = theWait / (double) numberOfProcesses;
	return result;
}

float averageTurnaroundTime(int theTurnaround){
	float result = theTurnaround / (double) numberOfProcesses;
	return result;
}
 
float averageUtilizationTime(int theUtilization){
 	float result = (theUtilization / theClock) * 100.0;
 	return result;
 }

int totalIncomingProcesses(void){
	return numberOfProcesses - nextProcess;
}

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

int compareProcessIds(const void *a, const void *b){
	Process *first = *((Process **) a);
	Process *second = *((Process **) b);
	if (first->pid < second->pid){
		return -1;
	}
	if (first->pid > second->pid){
		return 1;
	}
	return 0;
}

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
	if (readyQueue.size == 0){
		return NULL;
	}
	else{
		Process *grabNext = readyQueue.front->data;
		dequeueProcess(&readyQueue);
		return grabNext;
	}
}

void addNewIncomingProcess(void){
	// place incoming processes into end of ready queue
}

void waitingToReady(void){
 	int i;
 	for(i = 0; i < waitingQueue.size; i++){
 		process *grabNext = waitingQueue.front->data;
 		dequeueProcess(&waitingQueue);
 		if(grabNext->burst[grabNext->currentBurst].step == grabNext->burst[grabNext->currentBurst].length){
 			grabNext->currentBurst++;
 			tmpQueue[tmpQueueSize++] = grabNext;
 		}
 		else{
 			enqueueProcess(&waitingQueue, grabNext);
 		}
 	}
 }

void readyToRunning(void){
 	int i;
 	
 	// also enqueue all processes from tmp queue to ready and reset tmp queue size to 0

 	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
 		if (CPUS[i] == NULL){
 			CPUS[i] = nextScheduledProcess();
 		}
 	}
 }

void runningToWaiting(void){
 	// move from running to waiting while i/o burst is happening
 }

void updateReadyState(void){
 	// updating processes in ready state
 }

void updateWaitingState(void){
 	// updating processes in waiting state
 }

void updateRunningState(void){
 	// updating processes in running state
 }

void displayResults(float awt, float atat, int sim, float aut, int cs, int pids){
	printf( "Average waiting time:   %.2f units\n"
		"Average turnaround time:   %.2f units\n"
		"Time CPU finished all processes:   %d\n"
		"Average CPU utilization:   %.2f units\n"
		"Number of context Switces:   %d\n" 
		"PID(s) of last process(es) to finish:   %d\n", awt, atat, sim, aut, cs, pids);
}

int main(){
	int i;
	int status = 0;
	float ut, wt, tat;
	int lastPID;
	
	// clear CPU'S, and initialize queues
	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
		CPUS[i] = NULL;
	}
	resetVariables();
	initializeProcessQueue(&readyQueue);
	initializeProcessQueue(&waitingQueue);

	// read in workload and store processes
	while( (status = (readProcess(&processes[numberOfProcesses]))) ){
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

		updateWaitingState();
		updateReadyState();
		updateRunningState();

		if (runningProcesses() == 0 && totalIncomingProcesses() == 0 && waitingQueue.size == 0){
			break;
		}
		theClock++;
		cpuTimeUtilized += runningProcesses();

	}

	// calculations
	for(i = 0; i < numberOfProcesses; i++){
		sumTurnarounds +=processes[i].endTime - processes[i].arrivalTime;
		totalWaitingTime += processes[i].waitingTime;

		if (processes[i].endTime == theClock){
			lastPID = processes[i].pid;
		}
	}

	wt = averageWaitTime(totalWaitingTime);
	tat = averageTurnaroundTime(sumTurnarounds);
	ut = averageUtilizationTime(cpuTimeUtilized);
	
	displayResults(wt, tat, theClock, ut, totalContextSwitches, lastPID);
	
	return 0;
}
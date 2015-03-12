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

// Some useful global variables
Process processes[MAX_PROCESSES+1];
int numberOfProcesses;
int nextProcess;
int totalWaitingTime;
int totalContextSwitches;
int cpuTimeUtilized;
int theClock;
int sumTurnarounds;
int timeQuantum;

// Ready process queue and waiting process queue
Process_queue readyQueue;
Process_queue waitingQueue;

// CPU's
Process *CPUS[NUMBER_OF_PROCESSORS];

// Temporary "Pre-Ready" queue
Process *tmpQueue[MAX_PROCESSES+1];
int tmpQueueSize;

/**
 * Creates a single process node with pointer to data and next
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
/**
 * Resets all global variables to 0 
 */
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
/**
 * Initializes a process queue. Makes an empty queue
 */
void initializeProcessQueue(Process_queue *q){
	q = (Process_queue*)malloc(sizeof(Process_queue));
	q->front = q->back = NULL;
	q->size = 0;
}
/**
 * Equeues a process
 */
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
/**
 * Dequeues a process
 */
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
/**
 * Calulates average wait time 
 */
double averageWaitTime(int theWait){
	double result = theWait / (double) numberOfProcesses;
	return result;
}
/**
 * Calculates average turnaround time
 */
double averageTurnaroundTime(int theTurnaround){
	double result = theTurnaround / (double) numberOfProcesses;
	return result;
}
 /**
  * Calculates average CPU utilization
  */
double averageUtilizationTime(int theUtilization){
 	double result = (theUtilization * 100.0) / theClock;
 	return result;
 }
/**
 * Return the total number of incoming processes. These processes have yet
 * to arrive in the system
 */
int totalIncomingProcesses(void){
	return numberOfProcesses - nextProcess;
}
/**
 * Compare arrival time of two processes
 */
int compareArrivalTime(const void *a, const void *b){
	Process *first = (Process *) a;
	Process *second = (Process *) b;
	return first->arrivalTime - second->arrivalTime;
}
/**
 * Compare process ID of two processes
 */
int compareProcessIds(const void *a, const void *b){
	Process *first = (Process *) a;
	Process *second = (Process *) b;
	if (first->pid == second->pid){
		error_duplicate_pid(first->pid);
	}
	return first->pid - second->pid;
}
/**
 * Iterates over all CPU's and to find and return the total number of 
 * currently running processes
 */
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
/**
 * Grabs the next scheduled process in the queue (first process currently at
 * the front of the ready queue). Increments the waiting time in order to update
 * the ready state. Returns the next process to be run
 */
Process *nextScheduledProcess(void){
	if (readyQueue.size == 0){
		return NULL;
	}
	Process *grabNext = readyQueue.front->data;
	dequeueProcess(&readyQueue);
	return grabNext;
}
/**
 * Add any new incoming processes to a temporary queue to be sorted and later added
 * to the ready queue. These incoming processes are put in a "pre-ready queue"
 */
void addNewIncomingProcess(void){
	while(nextProcess < numberOfProcesses && processes[nextProcess].arrivalTime <= theClock){
		tmpQueue[tmpQueueSize] = &processes[nextProcess];
		tmpQueue[tmpQueueSize]->quantumRemaining = timeQuantum;
		tmpQueueSize++;
		nextProcess++;
	}
}
/**
 * Get the first process in the waiting queue, check if their I/O burst is complete.
 * If the current I/O burst is complete, move on to next I/O burst and add the process
 * to the "pre-ready queue". Dequeue the waiting queue and update waiting state by 
 * incrementing the current burst's step
 */
void waitingToReady(void){
 	int i;
 	int waitingQueueSize = waitingQueue.size;
 	for(i = 0; i < waitingQueueSize; i++){
 		Process *grabNext = waitingQueue.front->data;
 		dequeueProcess(&waitingQueue);
 		if(grabNext->bursts[grabNext->currentBurst].step == grabNext->bursts[grabNext->currentBurst].length){
 			grabNext->currentBurst++;
 			grabNext->quantumRemaining = timeQuantum;
 			grabNext->endTime = theClock;
 			tmpQueue[tmpQueueSize++] = grabNext;
 		}
 		else{
 			enqueueProcess(&waitingQueue, grabNext);
 		}
 	}
 }
/**
 * Sort elements in "pre-ready queue" in order to add them to the ready queue
 * in the proper order. Enqueue all processes in "pre-ready queue" to ready queue.
 * Reset "pre-ready queue" size to 0. Find a CPU that doesn't have a process currently 
 * running on it and schedule the next process on that CPU
 */
void readyToRunning(void){
 	int i;
 	qsort(tmpQueue, tmpQueueSize, sizeof(Process*), compareProcessIds);
 	for (i = 0; i < tmpQueueSize; i++){
 		enqueueProcess(&readyQueue, tmpQueue[i]);
 	}
	tmpQueueSize = 0;
 	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
 		if (CPUS[i] == NULL){
 			CPUS[i] = nextScheduledProcess();
 		}
 	}
 }
/**
 * If a currently running process has finished their CPU burst, move them to the waiting queue 
 * and terminate those who have finished their CPU burst. Start the process' next I/O burst. If 
 * CPU burst is not finished, move the process to the waiting queue and free the current CPU. 
 * If the CPU burst is finished, terminate the process by setting the end time to the current 
 * simulation time. Alternatively, the time slice expires before the current burst is over.
 */
void runningToWaiting(void){
	int num = 0;
	Process *preemptive[NUMBER_OF_PROCESSORS];
 	int i, j;
 	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
 		if (CPUS[i] != NULL){
 			if (CPUS[i]->bursts[CPUS[i]->currentBurst].step == CPUS[i]->bursts[CPUS[i]->currentBurst].length){
 				CPUS[i]->currentBurst++;
 				if (CPUS[i]->currentBurst < CPUS[i]->numOfBursts){
 					enqueueProcess(&waitingQueue, CPUS[i]);
 				}
 				else{
 					CPUS[i]->endTime = theClock;
 				}
 				CPUS[i] = NULL;
 			}
 			// context switch takes longer than time slice
 			else if(CPUS[i]->quantumRemaining == 0){
 				preemptive[num] = CPUS[i];
 				preemptive[num]->quantumRemaining = timeQuantum;
 				num++;
 				totalContextSwitches++;
 				CPUS[i] = NULL;
 			}
 		}	
 	}
 	// sort preemptive processes by process ID's and enqueue in the ready queue
 	qsort(preemptive, num, sizeof(Process*), compareProcessIds);
 	for (j = 0; j < num; j++){
 		enqueueProcess(&readyQueue, preemptive[j]);
 	}
 }
 /**
 * Function to update waiting processes, ready processes, and running processes
 */
 void updateStates(void){
 	int i;
 	int waitingQueueSize = waitingQueue.size;
 	// update waiting state
 	for (i = 0; i < waitingQueueSize; i++){
 		Process *grabNext = waitingQueue.front->data;
 		dequeueProcess(&waitingQueue);
 		grabNext->bursts[grabNext->currentBurst].step++;
 		enqueueProcess(&waitingQueue, grabNext);
 	}
 	// update ready process
 	for (i = 0; i < readyQueue.size; i++){
 		Process *grabNext = readyQueue.front->data;
 		dequeueProcess(&readyQueue);
 		grabNext->waitingTime++;
 		enqueueProcess(&readyQueue, grabNext);
 	}
 	// update CPU's
 	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
 		if(CPUS[i] != NULL){
 			CPUS[i]->bursts[CPUS[i]->currentBurst].step++;
 			CPUS[i]->quantumRemaining--;
 		}
 	}
 }
/**
 * Display results for average waiting time, average turnaround time, the time
 * the CPU finished all processes, average CPU utilization, number of context
 * switches, and the process ID of the last process to finish.
 */
void displayResults(float awt, float atat, int sim, float aut, int cs, int pids){
	printf("------------------Round-Robin------------------\n"
		"Average waiting time\t\t:%.2f units\n"
		"Average turnaround time\t\t:%.2f units\n"
		"Time CPU finished all processes\t:%d\n"
		"Average CPU utilization\t\t:%.1f%%\n"
		"Number of context Switces\t:%d\n" 
		"PID of last process to finish\t:%d\n"
		"------------------------------------------------\n", awt, atat, sim, aut, cs, pids);
}

int main(int argc, char *argv[]){
	int i;
	int status = 0;
	double ut, wt, tat;
	int lastPID;
	timeQuantum = atoi(argv[1]);

	// input error handling
	if (argc > 2){
		printf("Incorrect number of arguments, only add one time slice.\n");
		exit(-1);
	}
	else if (argc < 2){
		printf("Must add time slice.\n");
		exit(-1);
	}

	// clear CPU'S, initialize queues, and reset global variables
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
		if (numberOfProcesses > MAX_PROCESSES || numberOfProcesses == 0){
			error_invalid_number_of_processes(numberOfProcesses);
		}
	}
	
	qsort(processes, numberOfProcesses, sizeof(Process*), compareArrivalTime);

	// main execution loop
	while (numberOfProcesses){
		addNewIncomingProcess();
		runningToWaiting();
		readyToRunning();
		waitingToReady();
		
		updateStates();

		// break when there are no more running or incoming processes, and the waiting queue is empty
		if (runningProcesses() == 0 && totalIncomingProcesses() == 0 && waitingQueue.size == 0){
			break;
		}

		cpuTimeUtilized += runningProcesses();
		theClock++;
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
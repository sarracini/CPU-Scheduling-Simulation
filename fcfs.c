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

//process_queue readyQueue;
//process_queue waitingQueue;
Process *CPUS[NUMBER_OF_PROCESSORS];

/* things i need:
- read in the data file and populate queue
- initialize ready and waiting queue function
- implementation for next scheduled process function (dequeues next schedules process from ready queue)
- implmentation for function for enqueuing new processes in ready queue
- implmentationfunction for moving waiting processes to ready after i/o burst done
- implmentation function for moving ready process into running state
- implmentation function for moving from running to waiting
- functions to updates states (waiting, ready, running)
- more error handling 
*/
void error(char *message){
	fprintf(stderr, "%s\n", message);
	exit(-1);
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
	return (second->arrivalTime - first->arrivalTime);
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
	float ut, wt, tat;
	// clear CPU'S in order to initialize them
	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
		CPUS[i] = NULL;
	}
	// also initialize both ready and waiting queue before doing anything - do this with two methods
	
	// main execution loop
	while (1){
		addNewIncomingProcess();
		runningToWaiting();
		waitingToReady();
		readyToRunning();

		/* break out of loop once there are no processes left running and no processes left to process
		 also check for if the waiting queue size is 0*/
		if (runningProcesses() == 0 && totalIncomingProcesses() == 0){
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
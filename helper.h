#define MAX_PROCESSES 100
#define MAX_BURSTS 1000
#define MAX_TOKEN_LENGTH 30
#define MAX_LINE_LENGTH (1<<16)
#define NUMBER_OF_PROCESSORS 4

typedef struct burst{
	int length;
	int step;
} Burst;

typedef struct process{
	int pid;
	int arrivalTime;
	int startTime;
	int endTime;
	int waitingTime;
	int currentBurst;
	int numOfBursts;
	struct burst bursts[MAX_BURSTS];
	int priority;
	int quantumRemaining;
	int currentQueue;
} Process;

void error(char *);
float averageWaitTime(int theWait);
float averageTurnaroundTime(int theTurnaround);
float averageUtilizationTime(int theUtilization);
Process *nextScheduledProcess(void);
int compareArrivalTime(const void *a, const void *b);
bool isCPUFree(void);
void newIncomingProcess(void);
void waitingToReady(void);
void readyToRunning(void);
void runningToWaiting(void);
void displayResults(float awt, float atat, int sim, float aut, int cs);
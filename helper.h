#define MAX_PROCESSES 100
#define MAX_BURSTS 1000
#define MAX_TOKEN_LENGTH 30
#define MAX_LINE_LENGTH (1<<16)
#define NUMBER_OF_PROCESSES 4

typedef struct burst burst;
typedef struct process process;

struct burst{
	int length;
	int step;
};

struct process{
	int pid;
	int arrivalTime;
	int startTime;
	int endTime;
	int waitingTime;
	int currentBurst;
	int numOfBursts;
	burst bursts[MAX_BURSTS];
	int priority;
	int quantumRemaining;
	int currentQueue;
};

void error(char *);
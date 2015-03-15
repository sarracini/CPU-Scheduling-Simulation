#define printable(a) (((a) >= 32 && (a) <= 126) || (a) == '\t' || (a) == '\n')

#define MAX_PROCESSES 100
#define MAX_BURSTS 1000
#define MAX_TOKEN_LENGTH 30
#define MAX_LINE_LENGTH (1<<16)
#define NUMBER_OF_PROCESSORS 4
#define NUMBER_OF_LEVELS 3

#define COMMENT_CHAR '#'
#define COMMENT_LINE -1

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

typedef struct process_node{
	struct process *data;
	struct process_node *next;
} Process_node;

typedef struct process_queue{
	int size;
	struct process_node *front;
	struct process_node *back;
} Process_queue;

/* Error management functions */
void error(char *);
void error_malformed_input_line(char *);
void error_too_many_bursts(int);
void error_duplicate_pid(int pid);
void error_bad_quantum(void);
void error_invalid_number_of_processes(int numberOfProcesses);

/* Scheduling functions */
double averageWaitTime(int theWait);
double averageTurnaroundTime(int theTurnaround);
double averageUtilizationTime(int theUtilization);
Process *nextScheduledProcess(void);
int compareArrivalTime(const void *a, const void *b);
int runningProcesses(void);
void addNewIncomingProcess(void);
int totalIncomingProcesses(void);
void waitingToReady(void);
void readyToRunning(void);
void runningToWaiting(void);
void displayResults(float awt, float atat, int sim, float aut, int cs, int pid);
void resetVariables(void);
void updateStates(void);

/* Queue management functions */
Process_node *createProcessNode(Process *);
void initializeProcessQueue(Process_queue *);
void enqueueProcess(Process_queue *, Process *);
void dequeueProcess(Process_queue *);

/* Input/Output functions */
char *readLine(void);
char *readLineHelper(char *, int);
int readInt(char **);
int readBracedInt(char **);
int empty(char *);
int readProcess(Process *);

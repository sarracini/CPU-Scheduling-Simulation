#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "helper.h"

/** Input/output functions **/

/* reads a line from stdin and returns it (in dynamically allocated space) */
char *readLine(void) {
    char *prefix = (char*) calloc(1, 1);
    if (prefix == NULL) error("out of memory");
    return readLineHelper(prefix, 16);
}

/* recursive helper for readLine.
   reads a line and returns it (in dynamically allocated space).
   note: prefix should be dynamically allocated, and will be freed.
         n > strlen(prefix) is the expected size of the string.
           memory allocated for the string will be doubled in size
           every time the line does not fit entirely within it.
*/
char *readLineHelper(char *prefix, int n) {
    int prefixlen = strlen(prefix);
    char *result = (char*) calloc(n, 1);
    if (result == NULL) error("out of memory");
    
    assert(prefixlen < n);
    memcpy(result, prefix, prefixlen);
    
    if (fgets(result+prefixlen, n-prefixlen, stdin) == NULL) return NULL;
    if (strchr(result, '\n') == NULL) return readLineHelper(result, 2*n);
    
    free(prefix);
    return result;
}

/* reads a non-negative integer from *buf and returns it.
   before the result is returned, *buf is incremented to point to the
       first location after the integer.
   if there is no such integer, or it does not fit in a 32-bit signed int,
       return value is -1. */
int readInt(char **buf) {
    int result = 0;
    
    /* skip whitespace */
    while (isspace(**buf)) (*buf)++;
    
    /* check if buffer is empty */
    if (**buf == '\0') return -1;
    
    /* accumulate integer value */
    while (**buf && !isspace(**buf)) {
        int new_result = 0;
    
        /* check for invalid characters */
        if (**buf < '0' || **buf > '9') return -1;
        
        /* accumulate digit (checking for overflow) and advance pointer */
        new_result = result * 10 + (**buf - '0');
        if (new_result < result) return -1;
        result = new_result;
        (*buf)++;
    }
    
    return result;
}

/* same as readInt, but expects an int enclosed in braces ().
   returns -1 if no suitable int exists, according to rules of readInt,
       OR if the next int is not enclosed in 1 set of braces. */
int readBracedInt(char **buf) {
    int result = 0;
    
    /* skip whitespace */
    while (isspace(**buf)) (*buf)++;
    
    /* check if buffer is empty */
    if (**buf == '\0') return -1;

    /* skip open brace */
    if (**buf == '(') (*buf)++;
    else return -1;
    
    /* accumulate integer value */
    while (**buf && !isspace(**buf) && **buf != ')') {
        int new_result = 0;
    
        /* check for invalid characters */
        if (**buf < '0' || **buf > '9') return -1;
        
        /* accumulate digit (checking for overflow) and advance pointer */
        new_result = result * 10 + (**buf - '0');
        if (new_result < result) return -1;
        result = new_result;
        (*buf)++;
    }
    
    if (**buf == ')') (*buf)++;
    else return -1;
    
    return result;
}

/* returns non-zero value iff. any characters in s are whitespace */
int empty(char *s) {
    /* skip white space and check first non-whitespace character */
    while (isspace(*s)) s++;
    return (*s == '\0');
}

/* reads all information pertaining to a single process from standard input,
   and stores the result into `dest'.  the return value is 1 if there are
   more process(es) to be read in the input, and 0 otherwise.
   (also returns COMMENT_LINE in event of a comment, signified by a leading
    COMMENT_CHAR character, being read.) */
int readProcess(Process *dest) {
    int pid = -1;
    int arrivalTime = 0;
    int firstBurst = 0;
    int cpuBurstLength = 0;
    int ioBurstLength = 0;
    
    char *line = readLine();
    char *ptr = line;
    
    if (line == NULL) return 0;
    
    /* ignore blank lines and handle 'comments' in input file (added feature) */
    if (empty(line) || line[0] == COMMENT_CHAR) return COMMENT_LINE;
    
    /* retrieve pid, arrival time and first burst */
    if ((pid = readInt(&ptr)) < 0) error_malformed_input_line(line);
    if ((arrivalTime = readInt(&ptr)) < 0) error_malformed_input_line(line);
    if ((firstBurst = readInt(&ptr)) < 0) error_malformed_input_line(line);
    
    /* save these parameters to the process */
    dest->pid = pid;
    dest->arrivalTime = arrivalTime;
    dest->bursts[0].length = firstBurst;
    dest->bursts[0].step = 0;
    dest->numOfBursts = 1;
    dest->priority = 0;
    
    /* read in the rest of the io and cpu bursts in pairs ([IO, CPU], ...) */
    while (!empty(ptr)) {
        
        /* get and save I/O burst, erroring out if it cannot be read */
        if ((ioBurstLength = readBracedInt(&ptr)) == -1) {
            error_malformed_input_line(line);
        }
        dest->bursts[dest->numOfBursts].step = 0;
        dest->bursts[dest->numOfBursts].length = ioBurstLength;
        dest->numOfBursts++;
        
        /* get and save CPU burst */
        if ((cpuBurstLength = readInt(&ptr)) == -1) {
            error_malformed_input_line(line);
        }
        /* check for too many bursts first!
          (here is where we will see the 1001th burst if it occurs) */
        if (dest->numOfBursts == MAX_BURSTS) {
            error_too_many_bursts(dest->pid);
        }
        dest->bursts[dest->numOfBursts].step = 0;
        dest->bursts[dest->numOfBursts].length = cpuBurstLength;
        dest->numOfBursts++;
    }
    free(line);
    
    return 1;
}
/** Error management functions **/

void error(char *message){
    fprintf(stderr, "%s\n", message);
    exit(-1);
}
/* print error message displaying the first errant line in the input,
   or a special message in the presence of unprintable characters,
   and terminate abnormally. */
void error_malformed_input_line(char *line) {
    int i;

    /* handle lines with unprintable characters */
    for (i=0;i<strlen(line);i++) {
        if (!printable(line[i])) {
            line = "<cannot display line due to unprintable characters>\n";
            break;
        }
    }
    fprintf(stderr, "Error - malformed input line:\n%s\n"
                    "Expected integral values in range [0, 2^31-1], in format:\n"
                    "    \"pid arrival cpu-burst (io-burst) cpu-burst "
                    "(io-burst) ... cpu-burst\"\n"
                    "    (every process having between 1 and %d bursts, "
                         "ending on a CPU burst)\n",
                    line, MAX_BURSTS);
    exit(-1);
}

/* print appropriate error for the case wherein a process has too many bursts,
   then abnormally terminate. */
void error_too_many_bursts(int pid) {
    fprintf(stderr, "Error - too many bursts provided for process with id %d.\n"
                    "Total number of CPU or I/O bursts must not exceed %d (or %d combined).\n",
                    pid, MAX_BURSTS/2, MAX_BURSTS);
    exit(-1);
}
/* print appropriate error for the case wherein there are too many processes,
    or none at all. */
void error_invalid_number_of_processes(int numberOfProcesses) {
    if (numberOfProcesses == 0) {
        fprintf(stderr, "Error: no processes specified in input.\n");
        exit(-1);
    } 
    else if (numberOfProcesses > MAX_PROCESSES) {
        fprintf(stderr, "Error: too many processes specified in input; "
                        "they cannot number more than %d.\n", MAX_PROCESSES);
        exit(-1);
    }
}

/* print appropriate error for the case wherein a process id appears more than
   once in the input, then abnormally terminate. */
void error_duplicate_pid(int pid) {
    fprintf(stderr, "Error - more than one process has id %d.\n", pid);
    exit(-1);
}

/* print appropriate error for the case wherein a time quantum specified in the
   command line arguments is invalid. */
void error_bad_quantum(void) {
    printf("Invalid time quantum: expected integer in range [1,2^31-1].\n");
    exit(-1);
}

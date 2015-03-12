# CPU-Scheduling-Simulation
Some C programs to simulate three CPU scheduling the following three algorithms for a multi-core computing system consisting of four homogeneous CPU’s:
- FCFS (first-coming-first-serving) scheduling.
- RR (round robin) scheduling with time quantum q=2 milliseconds, q=12 milliseconds, q=50 milliseconds, respectively.
- Three-level feedback-queue (FBQ) preemptive scheduling with q1=10 milliseconds and q2=30 milliseconds, respectively.

Program will calculate and answer all the following questions for each of the above CPU schedulers:

1. What is the average waiting time?

2. What is the average turnaround time?

3. When does the CPU finish all these processes? What is average CPU utilization by this time point?  (At any time instance, CPU utilization is 400% if all 4 CPUs are running, 300% if only 3 CPUs are running, 200% for 2 CPUs, 100% for only 1 CPU, 0% if no CPU is running.)

4. How many context switches occur in total during the execution? 

5. Which process is the last one to finish?

## Build

Compile with:

``` sh
$ gcc fcfs.c input_output.c
```
``` sh
$ gcc rr.c input_output.c
```
``` sh
$ gcc fbq.c input_output.c
```

## Notes
Sample static CPU workload (CPULoad.dat) is given to run with each file.

## Usage

Run first come first serve with:

``` sh
$ fcfs  < CPULoad.dat
```
Run round robin with any time slice:

``` sh
$ rr 2  < CPULoad.dat
```
Run three level feedback queue with any two time slices:

``` sh
$ fbq 10 30  < CPULoad.dat
```

## License

MIT &copy; Ursula Sarracini

/*
 * student.c
 * Multithreaded OS Simulation for CS 2200, Project 5
 * Fall 2016
 *
 * This file contains the CPU scheduler for the simulation.
 * Name: Yihan Zhou 
 * GTID: 903053761
 */

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "os-sim.h"


/*
 * current[] is an array of pointers to the currently running processes.
 * There is one array element corresponding to each CPU in the simulation.
 *
 * current[] should be updated by schedule() each time a process is scheduled
 * on a CPU.  Since the current[] array is accessed by multiple threads, you
 * will need to use a mutex to protect it.  current_mutex has been provided
 * for your use.
 */
static pcb_t **current;
static pthread_mutex_t current_mutex;

static pcb_t *head;
static int timeslice;
static int algo; //0 => FIFO, 1=>RR, 2=>PRIORITY
static int num_cpu;
static pthread_cond_t cond;
static pthread_mutex_t q_mutex;

void enq(pcb_t *nd) {
    pcb_t *pt;
    nd->next = NULL;
    pthread_mutex_lock(&q_mutex);
    if (!head) {
        head = nd;
    } else {
        pt = head;
        while (pt->next != NULL) {
            pt = pt->next;
        }
        pt->next = nd;
    }
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&q_mutex);
}


pcb_t* deq() {
    pcb_t* rt;
    if (!head) {
        rt = NULL;
    } else {
        pthread_mutex_lock(&q_mutex);
        rt = head;
        head = head->next;
        pthread_mutex_unlock(&q_mutex);
    }
    return rt;
}

pcb_t* priority_deq() {
    if (!head) {
        return NULL;
    }

    pcb_t* prev_temp = NULL;
    pcb_t* temp = head;
    pcb_t* prev_pt = NULL;
    pcb_t* pt = head;

    while (pt != NULL) {
        if (pt->static_priority>temp->static_priority) {
            prev_temp = prev_pt;
            temp = pt;
        }
        prev_pt = pt;
        pt = pt->next;
    }

    pthread_mutex_lock(&q_mutex);
    if (prev_temp==NULL) {        
        head = head->next;
    } else {
        prev_temp->next = temp->next;
    }
    pthread_mutex_unlock(&q_mutex);

    temp->next = NULL;
    return temp;

}


/*
 * schedule() is your CPU scheduler.  It should perform the following tasks:
 *
 *   1. Select and remove a runnable process from your ready queue which 
 *	you will have to implement with a linked list or something of the sort.
 *
 *   2. Set the process state to RUNNING
 *
 *   3. Call context_switch(), to tell the simulator which process to execute
 *      next on the CPU.  If no process is runnable, call context_switch()
 *      with a pointer to NULL to select the idle process.
 *	The current array (see above) is how you access the currently running
 *	process indexed by the cpu id. See above for full description.
 *	context_switch() is prototyped in os-sim.h. Look there for more information 
 *	about it and its parameters.
 */
static void schedule(unsigned int cpu_id)
{
    /* FIX ME */
    pcb_t* sel;

    if (algo<2) {
        sel = deq();
    } else {
        sel = priority_deq();
    }

    if (sel != NULL) {       
        sel->state = PROCESS_RUNNING;
        pthread_mutex_lock(&current_mutex);
        current[cpu_id] = sel;
        pthread_mutex_unlock(&current_mutex);
    }

    context_switch(cpu_id, sel, timeslice);
}


/*
 * idle() is your idle process.  It is called by the simulator when the idle
 * process is scheduled.
 *
 * This function should block until a process is added to your ready queue.
 * It should then call schedule() to select the process to run on the CPU.
 */
extern void idle(unsigned int cpu_id)
{
    /* FIX ME */
    pthread_mutex_lock(&q_mutex);
    while (!head) {
        pthread_cond_wait(&cond, &q_mutex);
    }
    pthread_mutex_unlock(&q_mutex);
    schedule(cpu_id);

    /*
     * REMOVE THE LINE BELOW AFTER IMPLEMENTING IDLE()
     *
     * idle() must block when the ready queue is empty, or else the CPU threads
     * will spin in a loop.  Until a ready queue is implemented, we'll put the
     * thread to sleep to keep it from consuming 100% of the CPU time.  Once
     * you implement a proper idle() function using a condition variable,
     * remove the call to mt_safe_usleep() below.
     */
    //mt_safe_usleep(1000000);
}


/*
 * preempt() is the handler called by the simulator when a process is
 * preempted due to its timeslice expiring.
 *
 * This function should place the currently running process back in the
 * ready queue, and call schedule() to select a new runnable process.
 */
extern void preempt(unsigned int cpu_id)
{
    /* FIX ME */
    pcb_t* temp = current[cpu_id];
    temp->state = PROCESS_READY;
    enq(temp);
    schedule(cpu_id);
}


/*
 * yield() is the handler called by the simulator when a process yields the
 * CPU to perform an I/O request.
 *
 * It should mark the process as WAITING, then call schedule() to select
 * a new process for the CPU.
 */
extern void yield(unsigned int cpu_id)
{
    /* FIX ME */
    pthread_mutex_lock(&current_mutex);
    current[cpu_id]->state = PROCESS_WAITING;
    pthread_mutex_unlock(&current_mutex);
    schedule(cpu_id);
}


/*
 * terminate() is the handler called by the simulator when a process completes.
 * It should mark the process as terminated, then call schedule() to select
 * a new process for the CPU.
 */
extern void terminate(unsigned int cpu_id)
{
    /* FIX ME */
    pthread_mutex_lock(&current_mutex);
    current[cpu_id]->state = PROCESS_TERMINATED;
    pthread_mutex_unlock(&current_mutex);
    schedule(cpu_id);
}


/*
 * wake_up() is the handler called by the simulator when a process's I/O
 * request completes.  It should perform the following tasks:
 *
 *   1. Mark the process as READY, and insert it into the ready queue.
 *
 *   2. If the scheduling algorithm is static priority, wake_up() may need
 *      to preempt the CPU with the lowest priority process to allow it to
 *      execute the process which just woke up.  However, if any CPU is
 *      currently running idle, or all of the CPUs are running processes
 *      with a higher priority than the one which just woke up, wake_up()
 *      should not preempt any CPUs.
 *	To preempt a process, use force_preempt(). Look in os-sim.h for 
 * 	its prototype and the parameters it takes in.
 */
extern void wake_up(pcb_t *process)
{
    /* FIX ME */
    int replace = -1;

    process->state = PROCESS_READY;
    enq(process);

    if (algo==2) {
        pthread_mutex_lock(&current_mutex);
        pcb_t* temp;
        for (int i=0;i<num_cpu;i++) {
            temp = current[i];
            if (!temp) {
                replace = num_cpu;
            } else {
                if (process->static_priority > temp->static_priority) {
                    replace = i;
                }
            }
        }
        pthread_mutex_unlock(&current_mutex);
        if (replace<num_cpu && replace>-1) {
            force_preempt(replace);
        }
    }
}


/*
 * main() simply parses command line arguments, then calls start_simulator().
 * You will need to modify it to support the -r and -p command-line parameters.
 */
int main(int argc, char *argv[])
{
    int cpu_count;

    /* Parse command-line arguments */
    if (argc < 2 || argc >4)
    {
        fprintf(stderr, "CS 2200 Project 5 Fall 2016 -- Multithreaded OS Simulator\n"
            "Usage: ./os-sim <# CPUs> [ -r <time slice> | -p ]\n"
            "    Default : FIFO Scheduler\n"
            "         -r : Round-Robin Scheduler\n"
            "         -p : Static Priority Scheduler\n\n");
        return -1;
    }
    cpu_count = atoi(argv[1]);
    num_cpu = cpu_count;

    /* FIX ME - Add support for -r and -p parameters*/
    /********** TODO **************/
    algo = 0;
    timeslice = -1;

    if (argc>2) {
        if (argv[2][1] == 'r') {
            algo = 1;
            if (argc<4) {
                return -1;
            }
            timeslice = atoi(argv[3]);
        } else if (argv[2][1] == 'p') {
            algo = 2;
        } else {
            return -1;
        }
    }

    /* Allocate the current[] array and its mutex */
    /*********** TODO *************/






    current = malloc(sizeof(pcb_t*) * cpu_count);
    assert(current != NULL);
    pthread_mutex_init(&current_mutex, NULL);

    /* Start the simulator in the library */
    start_simulator(cpu_count);

    return 0;
}



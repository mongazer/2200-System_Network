#ifndef PROCESS_H
#define PROCESS_H

#include "pagetable.h"
#include "util.h"
#include "global.h"
#include "string.h"

typedef struct task_struct_t {
	int pid;
	char name[256];
	pte_t *pagetable;

	struct task_struct_t *next;
} task_struct;

extern task_struct *current_process;

task_struct *add_process(int pid, char name[256]);
task_struct *get_process(int pid);
task_struct *remove_process(int pid);
void free_processes(void);

#endif

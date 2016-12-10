#include "process.h"

task_struct *current_process;

static task_struct *head;
static task_struct *tail;

task_struct* add_process(int pid, char name[256])
{
	task_struct *new_process = (task_struct *) calloc(sizeof(task_struct), 1);
	if (!new_process) {
		perror_exit("process : Could not allocate memory for new process");
	}

	new_process->pid = pid;
	strcpy(new_process->name, name);

	new_process->pagetable = (pte_t *) malloc(sizeof(pte_t) * (1 << (virtual_address_size - page_size)));
	if (!new_process->pagetable) {
		perror_exit("process : Could not allocate memory for new process's page table");
	}
	for (uint64_t i = 0; i < (1ull << (virtual_address_size - page_size)); i++) {
		new_process->pagetable[i].valid = 0;
	}
	new_process->next = NULL;

	if (head == NULL) {
		head = new_process;
		tail = new_process;
	} else {
		tail->next = new_process;
		tail = new_process;
	}
	return new_process;
}

task_struct *get_process(int pid)
{
	task_struct *curr = head;
	while(curr) {
		if (curr->pid == pid) {
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

task_struct *remove_process(int pid)
{
	task_struct *curr = head;
	task_struct *prev = NULL;
	while (curr) {
		prev = head;
		if (curr->pid == pid) {
			prev->next = curr->next;
			break;
		}
		curr = curr->next;
	}
	return curr;
}

void free_processes(void)
{
	task_struct *curr = head;
	while (curr) {
		head = head->next;
		free(curr->pagetable);
		free(curr);
		curr = head;
	}
}

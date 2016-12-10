#ifndef REVERSE_LOOKUP_H
#define REVERSE_LOOKUP_H

#include "process.h"
#include "pagetable.h"

typedef struct reverse_lookup_entry_t {
	task_struct *task_struct;
	uint64_t vpn;

	// Need to know if frame is free or not
	uint8_t valid;
} rlte_t;

extern rlte_t *rlt;

void rlt_init(void);
void rlt_free(void);

#endif

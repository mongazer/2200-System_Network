#ifndef STATS_H
#define STATS_H

#include "util.h"

typedef struct stats_t
{
	// Reads, writes and accesses
	uint64_t writes;
	uint64_t reads;
	uint64_t accesses;

	// Accesses that result in a page fault
	uint64_t page_faults;

	// Accesses that result in a TLB miss
	uint64_t translation_faults;

	uint64_t writes_to_disk;
	uint64_t reads_from_disk;
	// Average Access Time
	double AAT;

	// Constants
	uint64_t TLB_READ_TIME;
	uint64_t DISK_READ_TIME;
	uint64_t DISK_WRITE_TIME;
	uint64_t MEMORY_READ_TIME;
} stats_t;

void compute_stats(stats_t *stats);

#endif

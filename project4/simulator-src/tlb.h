#ifndef TLB_H
#define TLB_H

#include "util.h"
#include "global.h"
#include "stats.h"

typedef struct tlb_entry_t {
	uint64_t vpn;
	uint64_t pfn;
	uint8_t valid;
	uint8_t dirty;

	// For clock sweeping the TLB
	uint8_t used;
} tlbe_t;

extern tlbe_t * tlb;

uint64_t tlb_lookup(uint64_t vpn, uint64_t offset, char rw, stats_t *stats);

void tlb_clear(void);

void tlb_clearOne(uint64_t vpn);

void tlb_init(void);

void tlb_free(void);

#endif

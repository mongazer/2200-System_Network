#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include "util.h"
#include "stats.h"
#include "global.h"

typedef struct page_table_entry_t {
	uint64_t pfn;
	uint8_t valid;
	uint8_t dirty;
	//uint8_t used; // Used page table
	uint64_t frequency;
} pte_t;

extern pte_t *current_pagetable;

uint64_t page_lookup(uint64_t vpn, uint64_t offset, char rw, stats_t *stats);

uint64_t page_fault_handler(uint64_t vpn, char rw, stats_t *stats);

uint64_t get_offset(uint64_t virtual_address);
uint64_t get_vpn(uint64_t virtual_address);

#endif

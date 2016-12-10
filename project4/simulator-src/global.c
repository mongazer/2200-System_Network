#include "global.h"

// All sizes are in bits. To compute the byte equivalent, left shift 1 by
// the size in bits. For example, page size is (1 << page_size) bytes.

// Page size in bits.
uint64_t page_size = 12;

// Virtual address size in bits
uint64_t virtual_address_size = 20;

// Physical address size in bits
uint64_t physical_address_size = 18;

// RLT Size
uint64_t rlt_size = 6;

// TLB Size
uint64_t tlb_size = 3;


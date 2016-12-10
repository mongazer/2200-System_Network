#include "pagetable.h"

/*
 * Current pagetable register. This can be used in the TLB for updating page
 * table entries
 */
pte_t *current_pagetable;

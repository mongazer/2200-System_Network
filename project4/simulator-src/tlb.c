#include "tlb.h"
#include <string.h>

tlbe_t *tlb;

void tlb_clear(void)
{
    memset(tlb, 0, sizeof(tlbe_t) * (1 << tlb_size));
}

void tlb_clearOne(uint64_t vpn)
{
    for (uint64_t i = 0; i < (1 << tlb_size); i++) {
        if (tlb[i].valid && tlb[i].vpn == vpn) {
            memset(&tlb[i], 0, sizeof(tlbe_t));
            break;
        }
    }
}

void tlb_init(void)
{
    tlb = (tlbe_t *)calloc(sizeof(tlbe_t), (1 << tlb_size));
    if (!tlb) {
        perror_exit("tlb: Could not allocate memory for TLB");
    }
}

void tlb_free(void)
{
    free(tlb);
}

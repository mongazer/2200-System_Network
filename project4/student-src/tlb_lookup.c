#include "tlb.h"
#include "pagetable.h"
#include <assert.h>
/**
 * This function simulates a the TLB lookup, and uses the second chance algorithm
 * to evict an entry
 *
 * @param vpn The virtual page number that has to be translated
 * @param offset The page offset of the virtual address
 * @param rw Specifies if the access is a read or a write
 * @param stats The struct for statistics
 */
uint64_t tlb_lookup(uint64_t vpn, uint64_t offset, char rw, stats_t *stats)
{
    
    // (1) Look for the pfn in the TLB.
    // If you find it here
    // (2) update the frequency count of the page table entry using the
    //     current_pagetable global.
    // (3) Mark the TLB entry as used - for clock sweep
    // (4) Make sure to mark the entry dirty in the TLB if it is a write access
    // (5) return the PFN you just found in the TLB
    

    /********* TODO ************/

    stats->accesses++;

    uint8_t dt;

    if (rw == 'r') {
        stats->reads++;
        dt=0;
    } else {
        stats->writes++;
        dt=1;
    }

    

    
    for (uint64_t i = 0; i < 1<<tlb_size; i++) {
        if (tlb[i].vpn == vpn && tlb[i].valid) {

            tlb[i].used=1;
            tlb[i].dirty |= dt;

            current_pagetable[vpn].frequency++;
            current_pagetable[vpn].valid = 1;
            current_pagetable[vpn].dirty |= dt;

            return ((tlb[i].pfn<<page_size)|offset);
        }
    }




    // The below function is called if it is a TLB miss
	/* DO NOT MODIFY */
	uint64_t pfn = page_lookup(vpn, offset, dt, stats);
	/*****************/
    
    // (1) Update the relevant stats
    // (2) Update the TLB with this new mapping
    //      (a) Find an invalid block in the TLB - use it
    //      If you cannot find an invalid block
    //      (i) Run the clock sweep algorithm to find a victim
    //      (ii) Update the current_pagetable at that VPN to dirty if
    //           the evicted TLB entry is dirty
    //      (b) Put the new mapping into the TLB - mark it used
    
    /**** THESE WILL HELP YOU MAKE SURE THAT YOUR CODE IS CORRECT *****/
    //assert(current_pagetable[vpn].valid);
    //assert(pfn == current_pagetable[vpn].pfn);
    /********* TODO ************/

    for (uint64_t i = 0; i < 1<<(tlb_size);i++) {
        if (!tlb[i].valid) {
            tlb[i].vpn=vpn;
            tlb[i].pfn=pfn;
            tlb[i].valid = 1;
            tlb[i].used=1;
            tlb[i].dirty = current_pagetable[vpn].dirty;

            return ((pfn<<page_size)|offset);
        }
    }
    
    for (uint64_t i = 0; i <(2+(1<<(tlb_size))); i++) {   
        if (tlb[i%(1<<tlb_size)].used == 0) {

            tlb[i%(1<<tlb_size)].vpn=vpn;
            tlb[i%(1<<tlb_size)].pfn=pfn;
            tlb[i%(1<<tlb_size)].valid = 1;
            tlb[i%(1<<tlb_size)].used=1;
            tlb[i%(1<<tlb_size)].dirty = current_pagetable[vpn].dirty;

            return ((pfn<<page_size)|offset);
        } else {
            tlb[i%(1<<tlb_size)].used = 0;
        }
    }



    /******* TODO *************/
    // Make sure to return the entire address here, this is just a place holder
    return (pfn << page_size) | offset;
}

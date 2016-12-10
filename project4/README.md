# Guide to the Virtual Memory Project

## Below is the order of operations each function will perform. We assume that you know how to split the virtual address into VPN and offset.

`uint64_t page_lookup(uint64_t vpn, char rw, stats_t *stats)`
* Check if the `current_pagetable` is valid for the vpn you are translating
    * If it is valid, use the PFN in the page table entry
    * If it is not valid, call the `page_fault_handler()` function to get a PFN
* Update the dirty bit if it is a write
* Update the frequency in the page table entry
* return the page frame number

`uint64_t page_fault_handler(uint64_t vpn, char rw, stats_t *stats)`
* Find victim frame - Already done for you
* Set the mapping from PFN to VPN in the `rlt`
    * Make sure to set the task_struct, valid and vpn fields
* Set the mappings from VPN to PFN in `current_pagetable`
    * Make sure to set the PFN, valid, frequency and dirty fields
* return the page frame number where you inserted the faulting page

`uint64_t find_free_frame(stats_t *stats)`
* Find an invalid frame to return
    * if found invalid frame, return this, and you are done
* Did not find invalid frame - Perform LFU eviction
    * Make sure to mark the pagetable entry of victim to invalid
    * Make sure to increment `writes_to_disk` if required.
* Use `tlb_clearOne()` to remove TLB entry for victim should the victim belong to the current_process. *(This has been done for you!)*
* return the victim frame

`uint64_t tlb_lookup(uint64_t vpn, uint64_t offset, char rw, stats_t *stats)`
* Check for VPN in the TLB
    * If entry exists
        * Update the dirty bit in TLB
        * Update the dirty bit in page table - write through policy
        * Update the frequency in `current_pagetable`
        * Mark it used in the TLB for future clock sweeps
    * If entry does not exist
        * Find invalid entry or do do clock sweep depending on avaialability of space
        * Update the invalid entry information
            * Make sure to update `VPN, PFN, VALID, DIRTY`, etc.
* Make sure to return the entire physical address

### **Note**: We expect you to know where and what stats are incremented. Also the compute stats function has been left as a challenge.

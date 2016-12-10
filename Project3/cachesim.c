
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cachesim.h"

#define TRUE 1
#define FALSE 0

/**
 * The stuct that you may use to store the metadata for each block in the L1 and L2 caches
 */
typedef struct block_t {
    uint64_t tag; // The tag stored in that block
    uint8_t valid; // Valid bit
    uint8_t dirty; // Dirty bit

    /**************** TODO ******************/

    /*
        Add another variable here to perform the LRU replacement. Look into using a counter
        variable that will keep track of the oldest block in a set
    */
    uint64_t lru;

} block;

/**
 * A struct for storing the configuration of both the L1 and L2 caches as passed in the
 * cache_init function. All values represent number of bits. You may add any parameters
 * here, however I strongly suggest not removing anything from the config struct
 */
typedef struct config_t {
    uint64_t C1; /* Size of cache L1 */
    uint64_t C2; /* Size of cache L2 */
    uint64_t S; /* Set associativity of L2 */
    uint64_t B; /* Block size of bth caches */

} config;

typedef struct set {
    block *block;
}set;

config* cache_in_use;
int counter;
set* cache_l1;
set* cache_l2;


/****** Do not modify the below function headers ******/
static uint64_t get_tag(uint64_t address, uint64_t C, uint64_t B, uint64_t S);
static uint64_t get_index(uint64_t address, uint64_t C, uint64_t B, uint64_t S);
static uint64_t convert_tag(uint64_t tag, uint64_t index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S);
static uint64_t convert_index(uint64_t tag, uint64_t index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S);
static uint64_t convert_tag_l1(uint64_t l2_tag, uint64_t l2_index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S);
static uint64_t convert_index_l1(uint64_t l2_tag, uint64_t l2_index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S);

/****** You may add Globals and other function headers that you may need below this line ******/

config* cache_in_use;
uint64_t cnt;

void rewritel1inl2(uint64_t index, uint64_t tag, uint64_t cnt, uint64_t target, struct cache_stats_t *stats);
void missl1(char rw, uint64_t address, struct cache_stats_t *stats, uint64_t cnt);
void missl2(char rw, uint64_t address, struct cache_stats_t *stats, uint64_t cnt);
uint64_t replace(uint64_t index, struct cache_stats_t *stats, uint64_t target);



/**
 * Subroutine for initializing your cache with the passed in arguments.
 * You may initialize any globals you might need in this subroutine
 *
 * @param C1 The total size of your L1 cache is 2^C1 bytes
 * @param C2 The tatal size of your L2 cache is 2^C2 bytes
 * @param S The total number of blocks in a line/set of your L2 cache are 2^S
 * @param B The size of your blocks is 2^B bytes
 */
void cache_init(uint64_t C1, uint64_t C2, uint64_t S, uint64_t B)
{
    /* 
        Initialize the caches here. I strongly suggest using arrays for representing
        meta data stored in the caches. The block_t struct given above maybe useful
    */

    /**************** TODO ******************/
    //printf("fuck init");
    counter = 0;
    int blocks_per_set_l1 = 1<<0;
    int blocks_per_set_l2 = 1<<S;
    int set_num_1 = 1 << (C1 - 0 - B);
    int set_num_2 = 1 << (C2 - S - B);
    cache_l1 = (set*) (malloc(set_num_1 * sizeof(set)));//set up l1 cache
    cache_l2 = (set*) (malloc(blocks_per_set_l2* sizeof(set)));// set up l2 cache
    cache_in_use = (config*)malloc(sizeof(config));
    cache_in_use->C1 = C1;
    cache_in_use->C2 = C2;
    cache_in_use->S = S;
    cache_in_use->B = B;
    for (int i = 0; i < blocks_per_set_l1; i++) {
        cache_l1[i].block = (block*) (malloc(set_num_1 * sizeof(block)));
        for (int j = 0; j < set_num_1; j++) {
            cache_l1[i].block[j].dirty = 0;
            cache_l1[i].block[j].tag = 0;
            cache_l1[i].block[j].valid = 0;
        }
    }

    for (int i = 0; i < blocks_per_set_l2; i++) {
        cache_l2[i].block = (block*) (malloc(set_num_2 * sizeof(block)));
        for (int j = 0; j < set_num_2; j++) {
            cache_l2[i].block[j].dirty = 0;
            cache_l2[i].block[j].tag = 0;
            cache_l2[i].block[j].valid = 0;
            cache_l2[i].block[j].lru = 0;
        }
    }
}

/**
 * Subroutine that simulates one cache event at a time.
 * @param rw The type of access, READ or WRITE
 * @param address The address that is being accessed
 * @param stats The struct that you are supposed to store the stats in
 */
void cache_access (char rw, uint64_t address, struct cache_stats_t *stats)
{
    /* 
        Suggested approach:
            -> Find the L1 tag and index of the address that is being passed in to the function
            -> Check if there is a hit in the L1 cache
            -> If L1 misses, check the L2 cache for a hit (Hint: If L2 hits, update L1 with new values)
            -> If L2 misses, need to get values from memory, and update L2 and L1 caches
            
            * We will leave it upto you to decide what must be updated and when
     */

    /**************** TODO ******************/
    cnt++;

    uint64_t C1 = cache_in_use->C1;
    uint64_t C2 = cache_in_use->C2;
    uint64_t B = cache_in_use->B;
    uint64_t S = cache_in_use->S;
    uint64_t tag = get_tag(address, C1, B, 0);
    uint64_t index = get_index(address,C1, B, 0);

    block b;

    int l1hit = 0;

    stats -> accesses++;

    if (rw == 'w') {
        stats -> writes++;
    } else {
        stats-> reads++;
    }

    b = cache_l1[0].block[index];

    if(b.tag==tag && b.valid) {
        l1hit = 1;
        
        uint64_t cvt_tag =  get_tag(address, C2, B, S);
        uint64_t cvt_index = get_index(address,C2,B,S);
        
        block bb;
        for (int j = 0; j < 1<<cache_in_use->S;j++) {
            bb = cache_l2[j].block[cvt_index];
            if (bb.tag==cvt_tag && bb.valid) {
                cache_l2[j].block[cvt_index].lru = cnt;
                break;
            }
        }
        if (rw == 'w') {
            cache_l1[0].block[index].dirty = 1;
        }
    }

    if (!l1hit) {
        missl1(rw,address,stats,cnt);
    }
    
}



void missl1(char rw, uint64_t address, struct cache_stats_t *stats, uint64_t cnt) {

    stats -> misses++;

    if (rw == 'w') {
        stats -> l1_write_misses++;
        stats -> write_misses++;               
    } else {
        stats-> l1_read_misses++;
        stats -> read_misses++;
    }
 
    uint64_t C1 = cache_in_use->C1;
    uint64_t C2 = cache_in_use->C2;
    uint64_t B = cache_in_use->B;
    uint64_t S = cache_in_use->S;

    uint64_t tag = get_tag(address, C2, B, S);
    uint64_t index = get_index(address,C2, B, S);

    uint64_t l1tag = get_tag(address, C1, B, 0);
    uint64_t l1index = get_index(address,C1, B, 0);

    if (cache_l1[0].block[l1index].dirty) {
/*        uint64_t taggg= cache_l1[0].block[l1index].tag;
        rewritel1inl2(l1index, taggg, cnt, -1, stats);    */  
        uint64_t newtag1 = cache_l1[0].block[l1index].tag;
        uint64_t newtag2 = convert_tag(newtag1,l1index,C1,C2,B,S);
        uint64_t newindex2 = convert_index(newtag1,l1index,C1,C2,B,S);
        for(int k = 0; k < 1<<S; k++) {
            int reached = 0;
            if (!reached) {
                if (cache_l2[k].block[newindex2].valid && cache_l2[k].block[newindex2].tag == newtag2) {
                    cache_l2[k].block[newindex2].dirty = 1;
                    reached = 1;
                }
            }
        }      
    }
    cache_l1[0].block[l1index].dirty = 0;

/*    if (rw == 'w') {
        cache_l1[0].block[l1index].dirty = 1;     
    } else {
        cache_l1[0].block[l1index].dirty = 0;
    }*/
    


    block b; 

    int l2hit = 0;
    int i = 0;
    for (i = 0; i < 1<<cache_in_use->S; i++) {
        b = cache_l2[i].block[index];
        if (b.tag == tag && b.valid) {
            l2hit = 1;
            cache_l2[i].block[index].lru = cnt;
            if (rw == 'w') {
                cache_l2[i].block[index].dirty = 1;
                //cache_l1[0].block[l1index].dirty = 1;
            }
            break;
        }

    }

    //cache_l1[0].block[l1index].dirty = 0;

    if (!l2hit) {
        missl2(rw,address,stats,cnt);
    }

    cache_l1[0].block[l1index].tag = l1tag;
    cache_l1[0].block[l1index].valid = 1;
}

void missl2(char rw, uint64_t address, struct cache_stats_t *stats, uint64_t cnt) {

    stats->misses++;
    if (rw == 'w') {
        stats->l2_write_misses++;
        stats->write_misses++;
    } else {
        stats->l2_read_misses++;
        stats->read_misses++;
    }

    uint64_t C1 = cache_in_use->C1;
    uint64_t C2 = cache_in_use->C2;
    uint64_t B = cache_in_use->B;
    uint64_t S = cache_in_use->S;

    
    uint64_t tag = get_tag(address, C2, B, S);
    uint64_t index = get_index(address,C2, B, S);

    
    int i = 0;
    block b;
    for (i = 0; i < 1<<cache_in_use->S; i++) {
        b = cache_l2[i].block[index];
        if (!b.valid) {
            break;
        }
    }

    if (i < 1<<cache_in_use->S) {
        cache_l2[i].block[index].tag = tag;    //
        cache_l2[i].block[index].valid = 1;
        cache_l2[i].block[index].lru = cnt;

        if (rw == 'w') {
            cache_l2[i].block[index].dirty = 1;
        } else {
            cache_l2[i].block[index].dirty = 0;
        }    
    } else {
        int new_index = replace(index,stats, -1);
        uint64_t newtag2 = cache_l2[new_index].block[index].tag;
        uint64_t tag11 = convert_tag_l1(newtag2, index, C1, C2, B, S);
        uint64_t index11 = convert_index_l1(newtag2,index,C1,C2,B,S);

        if(cache_l2[new_index].block[index].dirty) {
            stats->write_backs++;

        }
        if (tag11 == cache_l1[0].block[index11].tag ) {
                cache_l1[0].block[index11].valid = 0;
                cache_l1[0].block[index11].dirty = 0;
        }

        if (rw == 'w') {
            cache_l2[new_index].block[index].dirty = 1;
        } else {
            cache_l2[new_index].block[index].dirty = 0;
        }
        cache_l2[new_index].block[index].lru= cnt;
        cache_l2[new_index].block[index].valid = 1;
        cache_l2[new_index].block[index].tag = tag;
    }



}

void rewritel1inl2(uint64_t index, uint64_t tag, uint64_t cnt, uint64_t target, struct cache_stats_t *stats) {

    uint64_t C1 = cache_in_use->C1;
    uint64_t C2 = cache_in_use->C2;
    uint64_t B = cache_in_use->B;
    uint64_t S = cache_in_use->S;
    uint64_t cvt_tag = convert_tag(tag,index,C1,C2,B,S);
    uint64_t cvt_index = convert_index(tag,index,C1,C2,B,S);

    for(int i = 0; i < 1<<S; i++) {
        if(cache_l2[i].block[cvt_index].tag == cvt_tag && cache_l2[i].block[cvt_index].valid) {
            cache_l2[i].block[cvt_index].dirty = 1;
            break;
        }
    }

    //cache_l1[0].block[index].dirty = 0;
} 


uint64_t replace(uint64_t index, struct cache_stats_t *stats, uint64_t target) {
    int victim = 0;
    uint64_t hehe = cache_l2[victim].block[index].lru;
    for (int i = 0; i < 1<<cache_in_use->S; i++) {
        if ((cache_l2[i].block[index].lru<hehe) && i != target) {
            victim = i;
            hehe = cache_l2[i].block[index].lru;
        }
    }

    return victim;
}





/**
 * Subroutine for freeing up memory, and performing any final calculations before the statistics
 * are outputed by the driver
 */
void cache_cleanup (struct cache_stats_t *stats)
{
    /*
        Make sure to free up all the memory you malloc'ed here. To check if you have freed up the
        the memory, run valgrind. For more information, google how to use valgrind.
    */

    /**************** TODO ******************/


    stats->l1_miss_rate = (stats->l1_read_misses + stats->l1_write_misses)/(stats->accesses*1.0);
    stats->l2_miss_rate = (stats->l2_read_misses + stats->l2_write_misses)/((stats->l1_read_misses + stats->l1_write_misses)*1.0);
    stats->miss_rate = (stats->misses)/(1.0 * stats->accesses);
    stats->l2_avg_access_time = stats->l2_access_time + stats->l2_miss_rate*stats->memory_access_time;
    stats->avg_access_time = stats->l1_access_time + stats->l1_miss_rate * stats->l2_avg_access_time;
    free(cache_l1);
    uint64_t S = cache_in_use -> S;
    for(int i = 0; i < 1<< S; i++) {
         free(cache_l2[i].block);
    }
    free(cache_l2);
    free(cache_in_use);
}


/**
 * Subroutine to compute the Tag of a given address based on the parameters passed in
 *
 * @param address The address whose tag is to be computed
 * @param C The size of the cache in bits (i.e. Size of cache is 2^C)
 * @param B The size of the cache block in bits (i.e. Size of block is 2^B)
 * @param S The set associativity of the cache in bits (i.e. Set-Associativity is 2^S)
 * 
 * @return The computed tag
 */
static uint64_t get_tag(uint64_t address, uint64_t C, uint64_t B, uint64_t S)
{
    /**************** TODO ******************/
    
    //return address>>(C-S-B);
    //return address>>(C-B);
    uint64_t mask = ~(1<< (64 - C +S));
    return (address >> (C - S)) & mask;

}

/**
 * Subroutine to compute the Index of a given address based on the parameters passed in
 *
 * @param address The address whose tag is to be computed
 * @param C The size of the cache in bits (i.e. Size of cache is 2^C)
 * @param B The size of the cache block in bits (i.e. Size of block is 2^B)
 * @param S The set associativity of the cache in bits (i.e. Set-Associativity is 2^S)
 *
 * @return The computed index
 */
static uint64_t get_index(uint64_t address, uint64_t C, uint64_t B, uint64_t S)
{
    /**************** TODO ******************/
    
    //return (address)&((1<<(C-B-S))-1);
    //return (address>>B)&((1<<(C-B-S))-1);
    return (address>>B)&((1<<(C-B-S))-1);
}


/**** DO NOT MODIFY CODE BELOW THIS LINE UNLESS YOU ARE ABSOLUTELY SURE OF WHAT YOU ARE DOING ****/

/*
    Note:   The below functions will be useful in converting the L1 tag and index into corresponding L2
            tag and index. These should be used when you are evicitng a block from the L1 cache, and
            you need to update the block in L2 cache that corresponds to the evicted block.

            The newly added functions will be useful for converting L2 indecies ang tags into the corresponding
            L1 index and tags. Make sure to understand how they are working.
*/

/**
 * This function converts the tag stored in an L1 block and the index of that L1 block into corresponding
 * tag of the L2 block
 *
 * @param tag The tag that needs to be converted (i.e. L1 tag)
 * @param index The index of the L1 cache (i.e. The index from which the tag was found)
 * @param C1 The size of the L1 cache in bits
 * @param C2 The size of the l2 cache in bits
 * @param B The size of the block in bits
 * @param S The set associativity of the L2 cache
 */
static uint64_t convert_tag(uint64_t tag, uint64_t index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S)
{
    uint64_t reconstructed_address = (tag << (C1 - B)) | index;
    return reconstructed_address >> (C2 - B - S);
}

/**
 * This function converts the tag stored in an L1 block and the index of that L1 block into corresponding
 * index of the L2 block
 *
 * @param tag The tag stored in the L1 index
 * @param index The index of the L1 cache (i.e. The index from which the tag was found)
 * @param C1 The size of the L1 cache in bits
 * @param C2 The size of the l2 cache in bits
 * @param B The size of the block in bits
 * @param S The set associativity of the L2 cache
 */
static uint64_t convert_index(uint64_t tag, uint64_t index, uint64_t C1, uint64_t C2, uint64_t B,  uint64_t S)
{
    // Reconstructed address without the block offset bits
    uint64_t reconstructed_address = (tag << (C1 - B)) | index;
    // Create index mask for L2 without including the block offset bits
    return reconstructed_address & ((1 << (C2 - S - B)) - 1);
}

/**
 * This function converts the tag stored in an L2 block and the index of that L2 block into corresponding
 * tag of the L1 cache
 *
 * @param l2_tag The L2 tag
 * @param l2_index The index of the L2 block
 * @param C1 The size of the L1 cache in bits
 * @param C2 The size of the l2 cache in bits
 * @param B The size of the block in bits
 * @param S The set associativity of the L2 cache
 * @return The L1 tag linked to the L2 index and tag
 */
static uint64_t convert_tag_l1(uint64_t l2_tag, uint64_t l2_index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S) {
    uint64_t reconstructed_address = (l2_tag << (C2 - B - S)) | l2_index;
    return reconstructed_address >> (C1 - B);
}

/**
 * This function converts the tag stored in an L2 block and the index of that L2 block into corresponding
 * index of the L1 block
 *
 * @param l2_tag The L2 tag
 * @param l2_index The index of the L2 block
 * @param C1 The size of the L1 cache in bits
 * @param C2 The size of the l2 cache in bits
 * @param B The size of the block in bits
 * @param S The set associativity of the L2 cache
 * @return The L1 index of the L2 block
 */
static uint64_t convert_index_l1(uint64_t l2_tag, uint64_t l2_index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S) {
    uint64_t reconstructed_address = (l2_tag << (C2 - B - S)) | l2_index;
    return reconstructed_address & ((1 << (C1 - B)) - 1);
}

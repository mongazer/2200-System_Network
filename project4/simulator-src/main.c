#include "util.h"
#include "process.h"
#include "pagetable.h"
#include "stats.h"
#include "tlb.h"
#include "reverselookup.h"
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

void print_statistics(stats_t *stats);

void print_help_and_exit() {
	printf("vm-sim [OPTIONS] < traces/file.trace\n");
    printf("  -V V\t\tVirtual memory space is 2^V bytes\n");
    printf("  -P P\t\tPhysical memory is 2^P bytes\n");
    printf("  -p p\t\tSize of each page is 2^p bytes\n");
    printf("  -t t\t\tSize of the TLB is 2^t entries\n");
    printf("  -d d\t\tDebug flag to print each physical address (1, 0)\n");
    printf("  -h\t\tThis helpful output\n");
    exit(0);
}

static int old_pid = -1;
static int debug_flag = 0;

void sim_access(int pid, char rw, uint64_t address, stats_t *stats)
{
	if (pid != old_pid) {
		// Context switch - Clear the TLB
		tlb_clear();
		current_process = get_process(pid);

		// Create process if it does not exist
		if (!current_process) {
			char name[256];
			sprintf(name, "process %d", pid);
			current_process = add_process(pid, name);
		}
		current_pagetable = current_process->pagetable;
		old_pid = pid;
	}

	// Call student implemented functions
	uint64_t vpn = get_vpn(address);
	uint64_t offset = get_offset(address);

	// Check TLB first
	uint64_t ret = tlb_lookup(vpn, offset, rw, stats);
	if (debug_flag) {
		printf("%" PRIu64 "lu\n", ret);	
	}
}

int main (int argc, char **argv)
{
	FILE *fin = stdin;

	int opt;

	while (-1 != (opt = getopt(argc, argv, "V:P:p:t:i:d:h"))) {
		switch (opt) {
			case 'V':
				virtual_address_size = atoi(optarg);
				break;
			case 'P':
				physical_address_size = atoi(optarg);
				break;
			case 'p':
				page_size = atoi(optarg);
				break;
			case 't':
				tlb_size = atoi(optarg);
				break;
			case 'i':
				fin = fopen(optarg, "r");
				break;
			case 'd':
				debug_flag = atoi(optarg);
				break;
			case 'h':
			default:
				print_help_and_exit();
				break;
		}
	}

	rlt_size = physical_address_size - page_size;

	printf("VM settings\n");
	printf("Page Size: %" PRIu64 "\n", page_size);
	printf("Virual Address Size: %" PRIu64 "\n", virtual_address_size);
	printf("Physical Address Size: %" PRIu64 "\n", physical_address_size);
	printf("TLB size: %" PRIu64 "\n", tlb_size);
	printf("Debug Flag: %d\n", debug_flag);
	printf("\n");

	
	stats_t *stats = malloc(sizeof(stats_t));
	memset(stats, 0, sizeof(stats_t));
	stats->TLB_READ_TIME = 2;
	stats->MEMORY_READ_TIME = 100;
	stats->DISK_READ_TIME = 100000;
	stats->DISK_WRITE_TIME = 200000;
	

	// Initialize hardware
	tlb_init();
	rlt_init();

	char rw;
	uint64_t address;
	int pid;
	while (!feof(fin)) {
		int ret = fscanf(fin, "%d %c %" PRIx64 "\n", &pid, &rw, &address);
		if (ret == 3) {
		 	//printf("%d, %c, %" PRIu64 "\n", pid, rw, address);
		 	sim_access(pid, rw, address, stats);
		}
	}

	compute_stats(stats);

	print_statistics(stats);

	// Free the hardware
	tlb_free();
	rlt_free();
	// Free the processes
	free_processes();
	// Free statistics struct
	free(stats);
	fclose(fin);

	return 0;
}

void print_statistics(stats_t *stats) {
    /* Overall stats */
    printf("Virtual Memory Statistics\n");
    printf("Accesses: %" PRIu64 "\n", stats->accesses);
    printf("Reads: %" PRIu64 "\n", stats->reads);
    printf("Writes: %" PRIu64 "\n", stats->writes);
    printf("Translation Faults: %" PRIu64 "\n", stats->translation_faults);
    printf("Page Faults: %" PRIu64 "\n", stats->page_faults);
    printf("Writes to Disk: %" PRIu64 "\n", stats->writes_to_disk);
    printf("TLB Read Time: %" PRIu64 "\n", stats->TLB_READ_TIME);
    printf("Memory Read Time: %" PRIu64 "\n", stats->MEMORY_READ_TIME);
    printf("Disk Read Time: %" PRIu64 "\n", stats->DISK_READ_TIME);
    /* Average Access Times */
    printf("Average Access Time: %f\n", stats->AAT);
    
}
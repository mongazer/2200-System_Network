#include "reverselookup.h"

rlte_t *rlt;

void rlt_init(void)
{
	rlt = calloc(sizeof(rlte_t), (1llu << rlt_size));
	if (!rlt) {
		perror_exit("RLT: Could not allocate reverse lookup table");
	}
}

void rlt_free(void)
{
	free(rlt);
}

#include "util.h"

char READ = 'r';
char WRITE = 'w';

void perror_exit(char *err)
{
	fprintf(stderr, "%s\n", err);
	exit(errno);
}

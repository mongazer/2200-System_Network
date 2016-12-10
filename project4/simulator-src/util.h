#ifndef UTIL_H
#define UTIL_H

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

extern char READ;
extern char WRITE;

void perror_exit(char *err);

#endif

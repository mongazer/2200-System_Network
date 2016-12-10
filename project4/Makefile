# Makefile for the virtual memory project in CS 2200
# Modify at your risk
# Written by - Anirudh Jain
SIM 	= simulator-src
STUDENT = student-src
SUBMIT = $(SIM) $(STUDENT) Makefile traces

CC 		= gcc
OPTIONS = -g -I$(SIM) -I$(STUDENT)
CFLAGS 	= $(OPTIONS) -Wall -std=c99 -pedantic -pipe -Werror

SIM_OBJS	= 	global.o \
				pagetable.o \
				process.o \
				reverselookup.o \
				util.o \
            	tlb.o \
				main.o

STUDENT_OBJS	= 	address_split.o \
            		compute_stats.o \
            		page_fault.o \
            		page_lookup.o \
            		tlb_lookup.o \

OBJS 	=	$(SIM_OBJS:%.o=$(SIM)/%.o) \
			$(STUDENT_OBJS:%.o=$(STUDENT)/%.o)

ALL		= vm-sim
all: $(ALL)

vm-sim: $(OBJS)
		$(CC) $(CFLAGS) -o $@ $^

submit: clean
		tar czvf prj4-submit.tar.gz $(SUBMIT)

.PHONY: clean
clean:
		rm -rf $(ALL) $(SIM)/*.o $(STUDENT)/*.o

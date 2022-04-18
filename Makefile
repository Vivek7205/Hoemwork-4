CC=gcc
CFALGS=-g -O0 -Wall

DEPS=queue.c

PROGRAM=main.c
EXE=pshed

.PHONY: all
all:
	$(CC) $(CFALGS) $(DEPS) -o $(EXE) $(PROGRAM)

.PHONY: clean
clean:
	rm -rfv $(EXE)

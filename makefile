CC = gcc
CFLAGS = -Wall -g -lpthread
LDFLAGS = -lm 

all: thr_reduce thread_atomic

thr_reduce: thr_reduce.o
	$(CC) $(CFLAGS) -o thr_reduce thr_reduce.o $(LDFLAGS)

thread_atomic: thread_atomic.o
	$(CC) $(CFLAGS) -o thr_atomic thr_atomic.o $(LDFLAGS)

thr_reduce.o: thr_reduce.c
	$(CC) $(CFLAGS) -c thr_reduce.c

thread_atomic.o: thr_atomic.c
	$(CC) $(CFLAGS) -c thr_atomic.c

clean:
	rm -f thr_reduce thr_atomic thr_reduce.o thr_atomic.o
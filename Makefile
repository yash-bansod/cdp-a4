SRCS=main.cpp Transaction.cpp LockMgr.cpp
EXECUTABLES=main.out

CC=g++
CFLAGS=-g -Wall -lpthread
LDFLAGS=

OBJS=$(SRCS:.cpp=.o)

all: $(EXECUTABLES)

$(EXECUTABLES): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(EXECUTABLES) $(OBJS)

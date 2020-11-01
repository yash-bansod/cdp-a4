SRCS=main.cpp Transaction.cpp
EXECUTABLES=main.out

CC=g++
CFLAGS=-g -Wall
LDFLAGS=

OBJS=$(SRCS:.cpp=.o)

all: $(EXECUTABLES)

$(EXECUTABLES): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(EXECUTABLES) $(OBJS)
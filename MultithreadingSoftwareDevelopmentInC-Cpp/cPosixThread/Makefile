SRC=testCpthread.c
EXECUTABLE=testCpthread
CC=gcc
CFLAGS=-pthread -Wall
LIBS=-lpthread

#Object files
OBJS=$(SRC:.c=.o)

all: $(EXECUTABLE)
	@echo Compiled succesfully!

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(EXECUTABLE) $(OBJS) $(LIBS)
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
clean:
	rm *.o $(EXECUTABLE)

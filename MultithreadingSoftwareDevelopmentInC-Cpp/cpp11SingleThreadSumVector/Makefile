SRC=testCppSingleThreadSumVector.cpp
EXECUTABLE=testCppSingleThreadSumVector
CC=g++
CFLAGS=-pthread
LIBS=-lpthread

#Object files
OBJS=$(SRC:.cpp=.o)

all: $(EXECUTABLE)
	@echo Compiled succesfully!

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(EXECUTABLE) $(OBJS) $(LIBS)
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
clean:
	rm *.o $(EXECUTABLE)

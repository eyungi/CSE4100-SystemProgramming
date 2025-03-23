CC = gcc
CFLAGS = -w -Wall
TARGET = testlib
OBJS = bitmap.o debug.o hash.o hex_dump.o list.o main.o

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

hex_dump.o : round.h
bitmap.o : limits.h round.h hex_dump.h hex_dump.h
%.o : %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY : clean
clean :
	rm -f $(TARGET) $(OBJS)
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

TARGET = search_index
SRCS = main.c system.c heap.c tree.c file.c keyword.c utils.c

OBJS = $(SRCS:.c=.o)

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c search_index.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: build clean
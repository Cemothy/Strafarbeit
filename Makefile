CC = gcc
CFLAGS = -Wall -Werror -Wextra -g -pthread
RM = rm -f
OBJECTS = BinaryTree.o
SOURCE = $(OBJECTS:.o=.c) $(OBJECTS:.o=.h)
TARGET = $(OBJECTS:.o=)

.PHONY: all clean

all: $(TARGET)

clean: $(TARGET) $(OBJECTS)
	$(RM) $^

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJECTS): $(SOURCE)
	$(CC) $(CFLAGS) -c $^

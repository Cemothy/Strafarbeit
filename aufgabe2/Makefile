CC = gcc
CFLAGS = -Wall -Werror -Wextra -g -pthread
RM = rm -f

SOURCE = aufgabe2.c
TARGET = $(SOURCE:.c=)


.PHONY: all clean

all: $(TARGET)

clean: $(TARGET)
	$(RM) $^

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $@ $^
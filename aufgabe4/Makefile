CFLAGS = -Wall -Werror -Wextra -g -pthread
RM = rm -f
TARGET = hoh_list list
OBJECTS = hoh_list.o


.PHONY: all clean

all: $(TARGET)

clean: $(TARGET)
	$(RM) $^

hoh_list: hoh_list.o
	$(CC) $(CFLAGS) -o $@ $<

hoh_list.o: hoh_list.c
	$(CC) $(CFLAGS) -c $<

list: list.c
	$(CC) $(CFLAGS) -o $@ $<
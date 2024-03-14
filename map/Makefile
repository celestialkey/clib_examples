CC 		:= gcc
CFLAGS 	:= -Wall -Wextra -g
LDFLAGS := -pthread
OUTDIR	:= ../bins/
TARGET 	:= map_example

SRCS 	:= $(wildcard *.c)
OBJS 	:= $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $(OUTDIR)/$@

%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(OUTDIR)/$(TARGET)
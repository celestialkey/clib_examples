CC 		:= gcc
CFLAGS 	:= -Wall -Wextra -g --std=gnu99
LDFLAGS := -pthread
INCLUDES:= 
OUTDIR	:= ../bins/
TARGET 	:= commlinks_example
RB_SRC 	:= ../ringbuffer/
SRCS 	:= $(wildcard *.c) $(wildcard $(RB_SRC)/ring*.c)
OBJS 	:= $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $(OUTDIR)/$@

%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(OUTDIR)/$(TARGET)
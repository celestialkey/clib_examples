CC 		:= gcc
CFLAGS 	:= -Wall -Wextra -g
OUTDIR	:= ../bins/
TARGET 	:= commands_example

LIST_SRC:= ../list/li*.c
MAP_SRC	:= ../map/map*.c
VEC_SRC	:= ../vector/vec*.c
SRCS 	:= $(wildcard *.c) $(wildcard $(LIST_SRC)) $(wildcard $(MAP_SRC)) $(wildcard $(VEC_SRC))
OBJS 	:= $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(OUTDIR)/$@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(OUTDIR)/$(TARGET)
CC 		:= gcc
CFLAGS 	:= -Wall -Wextra -g -DDEBUG
OUTDIR	:= ../bins/
TARGET 	:= parser_example

LIST_SRC:= ../list/li*.c
SRCS 	:= $(wildcard *.c) $(wildcard $(LIST_SRC))
OBJS 	:= $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(OUTDIR)/$@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(OUTDIR)/$(TARGET)
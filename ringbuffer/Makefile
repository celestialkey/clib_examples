CC 		:= gcc
CFLAGS 	:= -Wall -Wextra -g
OUTDIR	:= ../bins/
TARGET 	:= rb_example

SRCS 	:= $(wildcard *.c)
OBJS 	:= $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(OUTDIR)/$@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(OUTDIR)/$(TARGET)
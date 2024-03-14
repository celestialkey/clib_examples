CC 		:= gcc
CFLAGS 	:= -Wall -Wextra -g --std=gnu99
LDFLAGS := -pthread
INCLUDES:= 

OUTDIR	:= ../bins/
TARGET 	:= dynamicports_example

RB_SRC 		:= ../ringbuffer/ring*.c
MAP_SRC 	:= ../map/map*.c
COMMLINK_SRC:= ../commlinks/comm*.c
WEPOLL_SRC	:= ../wepoll/wepoll*.c
WSOCKET_SRC	:= ../wsocket/wsock*.c
SRCS 		:= $(wildcard *.c) $(wildcard $(RB_SRC)) $(wildcard $(MAP_SRC)) $(wildcard $(COMMLINK_SRC)) $(wildcard $(WEPOLL_SRC)) $(wildcard $(WSOCKET_SRC))
OBJS 		:= $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $(OUTDIR)/$@

%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(OUTDIR)/$(TARGET)
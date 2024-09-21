CFLAGS += --std=gnu11 -Wall -Wextra -Werror -fstack-protector -fPIE
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-sign-compare
CFLAGS += -DDEBUG


all: main.o table.o play.o
	gcc -o $@ $^ -lm -g -O0  $(CFLAGS)


clean:
	rm -rf *.o *~ debug*

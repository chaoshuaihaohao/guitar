CFLAGS += --std=gnu11 -Wall -Wextra -Werror -fstack-protector -fPIE
CFLAGS += -g -O0
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-sign-compare
CFLAGS += -DDEBUG


all: main.o table.o play.o
	gcc -o $@ $^ -lm $(CFLAGS)


clean:
	rm -rf *.o *~ debug*

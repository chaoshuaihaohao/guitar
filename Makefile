CFLAGS += --std=gnu11 -Wall #-Wextra -Werror -fstack-protector -fPIE


all:
	gcc guitar.c -lm -g -O0  $(CFLAGS)

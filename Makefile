CFLAGS += --std=gnu11 -Wall -Wextra -Werror -fstack-protector -fPIE
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-sign-compare


all:
	gcc guitar.c table.c -lm -g -O0  $(CFLAGS)

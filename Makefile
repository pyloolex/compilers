CFLAGS+=-I/usr/lib/gcc/x86_64-linux-gnu/7/include/ -L/usr/lib/gcc/x86_64-linux-gnu/7/
LDLIBS+=-lgccjit

all: jitcalc

jitcalc: jitcalc_Lf.c
	$(CC) $< -o $@ $(LDLIBS) $(CFLAGS)
SRCDIR := src
TXTDIR := data/llvm-mca
TESTDIR := test

SRC := $(notdir $(wildcard $(SRCDIR)/*.c))
TXT := $(SRC:.c=.txt)

CC := gcc
CFLAGS := -I./include -O2 -march=x86-64-v2 -masm=intel

%.s: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $< -S -o $@

%.txt: %.s
	mkdir -p $(TXTDIR)

	llvm-mca $< --march x86-64 --mcpu=skylake --timeline --timeline-max-cycles=1000 --iterations=1 \
		> $(TXTDIR)/$(basename $@)_skylake.txt 2> /dev/null
	llvm-mca $< --march x86-64 --mcpu=nehalem --timeline --timeline-max-cycles=1000 --iterations=1 \
		> $(TXTDIR)/$(basename $@)_nehalem.txt 2> /dev/null

%: %.s
	$(CC) $(CFLAGS) $< $(TESTDIR)/variance.c -o $(TESTDIR)/test_$@
	./test/test_$@

all: $(TXT) base v1 jhash2 hsiphash siphash

clean:
	rm -fr $(TESTDIR)/test_* 2> /dev/null

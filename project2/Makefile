CC=gcc
FLEX=flex
YACC=bison

DEBUG ?= 0
VERBOSE ?= 0
ifeq ($(DEBUG), 1)
    CFLAGS:=-DDEBUG -g $(CFLAGS)
endif

ifeq ($(VERBOSE), 1)
    CFLAGS:=-DVERBOSE $(CFLAGS)
endif

splc: splc.y splc.l APT.c APT.h
	@mkdir -p bin/
	$(YACC) -t -d splc.y
	$(FLEX) splc.l
	$(CC) splc.tab.c APT.c -o bin/splc -ly -ll $(CFLAGS)
clean:
	@rm -rf bin/
	@rm -f *.yy.* *.tab.* splc
.PHONY: clean
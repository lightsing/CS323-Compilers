CC=gcc
FLEX=flex
YACC=bison
splc: splc.y splc.l APT.c APT.h
	$(YACC) -t -d splc.y
	$(FLEX) splc.l
	$(CC) splc.tab.c APT.c -o splc -ly -ll
clean:
	@rm -rf bin/
	@rm -f *.yy.* *.tab.* splc
.PHONY: clean
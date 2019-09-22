CC=gcc
FLEX=flex
YACC=bison
splc: splc.y splc.l APT.c APT.h
	@mkdir -p bin/
	$(YACC) -t -d splc.y
	$(FLEX) splc.l
	$(CC) splc.tab.c APT.c -o bin/splc -ly -ll -g
clean:
	@rm -rf bin/
	@rm -f *.yy.* *.tab.* splc
.PHONY: clean
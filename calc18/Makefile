CFLAGS = -g

all: calc18


lex.yy.c: calc18.l
	lex calc18.l

y.tab.c: calc18.y
	yacc -d calc18.y

calc18: lex.yy.c y.tab.c y.tab.h comp18.c
	$(CC) $(CFLAGS) -o $@ y.tab.c lex.yy.c comp18.c -ll

clean:
	$(RM) lex.yy.c
	$(RM) y.tab.c y.tab.h
	$(RM) calc18

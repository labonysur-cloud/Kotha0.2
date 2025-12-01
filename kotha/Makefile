all: kotha

kotha: lex.yy.c parser.tab.c
	gcc -o kotha lex.yy.c parser.tab.c -ll

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

clean:
	rm -f kotha lex.yy.c parser.tab.c parser.tab.h

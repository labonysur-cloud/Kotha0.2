# Kotha Makefile

CC = gcc
CFLAGS = -Wall -Wno-unused-function -Wno-unused-variable
LDFLAGS = -lm

# Source files
SRCS = main.c parser.tab.c lex.yy.c optimizer.c symtab.c ast.c interp.c ir.c vm.c codegen_vm.c string_lib.c file_io.c math_lib.c array_lib.c repl.c
OBJS = main.o parser.tab.o lex.yy.o optimizer.o symtab.o ast.o interp.o ir.o vm.o codegen_vm.o string_lib.o file_io.o math_lib.o array_lib.o repl.o

all: kotha

kotha: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o kotha $(LDFLAGS)


optimizer.o: optimizer.c optimizer.h
	$(CC) $(CFLAGS) -c optimizer.c

symtab.o: symtab.c symtab.h
	$(CC) $(CFLAGS) -c symtab.c

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c

interp.o: interp.c interp.h
	$(CC) $(CFLAGS) -c interp.c

ir.o: ir.c ir.h
	$(CC) $(CFLAGS) -c ir.c

vm.o: vm.c vm.h
	$(CC) $(CFLAGS) -c vm.c

codegen_vm.o: codegen_vm.c
	$(CC) $(CFLAGS) -c codegen_vm.c

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y


main.o: main.c
	$(CC) $(CFLAGS) -c main.c

string_lib.o: string_lib.c string_lib.h
	$(CC) $(CFLAGS) -c string_lib.c

file_io.o: file_io.c file_io.h
	$(CC) $(CFLAGS) -c file_io.c

math_lib.o: math_lib.c math_lib.h
	$(CC) $(CFLAGS) -c math_lib.c

array_lib.o: array_lib.c array_lib.h
	$(CC) $(CFLAGS) -c array_lib.c

repl.o: repl.c repl.h
	$(CC) $(CFLAGS) -c repl.c

clean:
	rm -f kotha lex.yy.c parser.tab.c parser.tab.h *.o

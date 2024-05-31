rundebug:
	gcc compile.c vm.c stack.c ./compiler/lexer.c ./compiler/parser.c -D DEBUG
	@echo === running compiler ===
	./a.out simple_prog.dra prog
	gcc run.c vm.c stack.c ./compiler/lexer.c ./compiler/parser.c -D DEBUG
	@echo === running interpreter ===
	./a.out prog

runrelease:
	gcc compile.c vm.c stack.c ./compiler/lexer.c ./compiler/parser.c
	./a.out simple_prog.dra prog
	gcc run.c vm.c stack.c ./compiler/lexer.c ./compiler/parser.c
	./a.out prog

install:
	gcc -o compile compile.c vm.c stack.c ./compiler/lexer.c ./compiler/parser.c
	gcc -o run run.c vm.c stack.c  ./compiler/lexer.c ./compiler/parser.c


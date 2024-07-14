rundebug:
	gcc compile.c vm.c stack.c ./compiler/lexer.c ./compiler/parser.c -D DEBUG  
	@echo === running compiler ===
	./a.out ./programs/math+jumps.dra prog
	gcc run.c vm.c stack.c ./compiler/lexer.c ./compiler/parser.c -D DEBUG - Wall
	@echo === running interpreter ===
	./a.out prog

runrelease:
	gcc compile.c vm.c stack.c ./compiler/lexer.c ./compiler/parser.c  
	./a.out ./programs/math+jumps.dra prog
	gcc run.c vm.c stack.c ./compiler/lexer.c ./compiler/parser.c  
	./a.out prog

install:
	gcc -o ./.build/dracompile compile.c vm.c stack.c ./compiler/lexer.c ./compiler/parser.c 
	gcc -o ./.build/drarun run.c vm.c stack.c  ./compiler/lexer.c ./compiler/parser.c 

installdebug:
	gcc -o ./.build/dracompile compile.c vm.c stack.c ./compiler/lexer.c ./compiler/parser.c -D DEBUG
	gcc -o ./.build/drarun run.c vm.c stack.c  ./compiler/lexer.c ./compiler/parser.c -D DEBUG

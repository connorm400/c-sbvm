rundebug:
	gcc compile.c vm.c stack.c -D DEBUG
	@echo === running compiler ===
	./a.out prog
	gcc run.c vm.c stack.c -D DEBUG
	@echo === running interpreter ===
	./a.out prog

runrelease:
	gcc compile.c vm.c stack.c
	./a.out prog
	gcc run.c vm.c stack.c
	./a.out prog

install:
	gcc -o compile compile.c vm.c stack.c 
	gcc -o run run.c vm.c stack.c  


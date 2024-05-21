runcompiler:
	gcc -o compile compile.c vm.c stack.c 
	./compile prog

runrunner:
	gcc -o run run.c vm.c stack.c 
	./run prog

install:
	gcc -o compile compile.c vm.c stack.c 
	gcc -o run run.c vm.c stack.c 
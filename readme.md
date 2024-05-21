neat little stack based virtual machine thingy. Doesn't have conditionals or a way to prevent UB yet.

# how to use it

Compile the two programs with `make install`. `./compile` will compile the program written in `compile.c` to `argv[1]`. `./run` will run the program.  

# how it currently works

its a stack based vitual machine: you can push to the top of the stack with OP_PUSH instructions, and the other instructions will pop those items as arguments and possibly push a result. When using strings you need to put it in the `strings[]` array and use `.str_idx` to refer to it's `strings[]` index. 
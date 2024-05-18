neat little stack based virtual machine thingy. Doesn't have conditionals or a way to prevent UB yet.

# how to use it

The program is currently a data structure in `compile.c`. To create the program, just compile `compile.c stack.c vm.c` and give the filename as the first argument when you run it. To run the program do the same with `run.c` isntead of `compile.c`.

# how it currently works

its a stack based vitual machine: you can push to the top of the stack with OP_PUSH instructions, and the other instructions will pop those items as arguments and possibly push a result. When using strings you need to put it in the `strings[]` array and use `.str_idx` to refer to it's `strings[]` index. 
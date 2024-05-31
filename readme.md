Stack based vitual machine with it's own assembly language

# install
use `make install` in the root directory to make two binaries: `compile` and `run`. 

# using
Use `compile` with the first argument being the input `.dra` file, the second being the output file.

use `run` with the output to run it :).

# syntax

You can look at some example programs in `./programs/`. The main / starting label must be the first one.
Here's a list of some of the instructions:
```
    p() - push to the stack. Can take a string or an integer
    cr - push a newline to the stack. the same as p("<\n>").
    jmp() - jump to a label. Requires a label name. Will panic in compile if the label doesn't exist
    cmp - compares top two elements. Pushes a comparison to the stack.
    exit - exit with status code poped from stack
    dup - duplicate top element
    discard - pop top element, discarding it
    add / subtract / multiply / divide - math. Popes top two elements, pushes result

    === conditional jumps ===
    these jumps will pop an element from the top of the stack, jumping if true.
        - je - jump if equal
        - jne - jump if not equal
        - jgt - jump if greater than
        - jlt - jump if less than
    
```
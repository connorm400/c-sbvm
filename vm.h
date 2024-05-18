#ifndef VM_HEADER_
#define VM_HEADER_
#include <stdint.h>
#include "stack.h"


typedef enum { 
    OP_PUSH, // pushes to the stack. requires an item 
    // math
    // pops top two elements, pushing the result of the two .integer operated on each other
    OP_ADD, 
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_PRINT, // pops the top elements and prints it
    OP_EXIT, // exits with error code popped from top of stack
} OpCode;

typedef struct {
    OpCode code;
    union {
        StackItem item;
    };
} Instruction;

extern void inst_print(Instruction inst, char** stringtable);
void item_print(StackItem item, char** stringtable);
extern int eval(Instruction* program, char** stringtable);

#endif
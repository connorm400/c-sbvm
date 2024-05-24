#ifndef VM_HEADER_
#define VM_HEADER_ 
#include <stdint.h>
#include "stack.h"


typedef enum { 
    OP_PUSH, // pushes to the stack. requires an item 
    OP_DUP, // duplicate top stack item
    // math
    // pops top two elements, pushing the result of the two .integer operated on each other
    OP_ADD, 
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_PRINT, // pops the top elements and prints it

    // conditionals and control flow
    OP_CMP,
    // jumps use label_idx union member
    OP_JMP,
    OP_JE,
    OP_JNE,
    OP_JLT,
    OP_JGT,

    OP_EXIT, // exits with error code popped from top of stack
} OpCode;

typedef struct {
    OpCode code;
    union {
        StackItem item;
        size_t label_idx;
    };
} Instruction;

typedef struct {
    size_t size;
    Instruction* instructions;
    char* name;
} Label;

#define LABEL(Ptr) \
    { .name = #Ptr, .size = sizeof((Ptr)) / sizeof(Instruction), .instructions = (Ptr) }

extern void inst_print(Instruction inst, char** stringtable);
void item_print(StackItem item, char** stringtable);
extern int eval(Label* labels, char** stringtable);

#endif
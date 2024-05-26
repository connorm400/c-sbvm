#ifndef VM_HEADER_
#define VM_HEADER_ 
#include <stdint.h>
#include "stack.h"


typedef enum { 
    OP_PUSH, // pushes to the stack. requires an item 
    OP_DUP, // duplicate top stack item
    OP_DIG, // pushes copy of stack[top - idx_from_top] to the top
    //OP_RM, // removes element stack[top - idx_from_top]. O(n)
    OP_DISCARD, // pops the top element discarding it
    // math
    // pops top two elements, pushing the result of the two .integer operated on each other
    OP_ADD, 
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,

    OP_PRINT, // pops the top elements and prints it

    // conditionals and control flow
    OP_CMP,
    // jumps use segment_idx union member
    OP_JMP, // jump
    OP_JE, // jump if top element is EQ (equal)
    OP_JNE, // jump if top element is not EQ
    OP_JLT, // jump if top element is LT (less than)
    OP_JGT, // jump if top element is GT (greater than)

    OP_EXIT, // exits with error code popped from top of stack
} OpCode;

typedef struct {
    OpCode code;
    union {
        StackItem item;
        size_t segment_idx;
        size_t idx_from_top;
    };
} Instruction;

typedef struct {
    size_t size;
    Instruction* instructions;
    char* name;
} Segment;

#define SEGMENT(Ptr) \
    { .name = #Ptr, .size = sizeof((Ptr)) / sizeof(Instruction), .instructions = (Ptr) }

extern void inst_print(Instruction inst, char** stringtable);
void item_print(StackItem item, char** stringtable);
extern int eval(Segment* segments, char** stringtable);

#endif
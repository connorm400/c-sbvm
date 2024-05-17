#ifndef VM_HEADER_
#define VM_HEADER_
#include <stdint.h>
#include "stack.h"


typedef enum { OP_PUSH, OP_ADD, OP_PRINT, OP_EXIT } OpCode;
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
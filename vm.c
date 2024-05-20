#include <stdio.h>
#include "vm.h"

#define MATH_OP(Infix) \
    {StackItem a = stack_pop(); \
     StackItem b = stack_pop(); \
     StackItem res = { \
        .type = T_INT, .integer = a.integer Infix b.integer, }; \
     stack_push(res);}

extern int eval(Instruction* program, char** stringtable) {
    for (;;) {
        switch (program->code) {
            case OP_PUSH:
                stack_push(program->item);
                break;
            case OP_PRINT:
                item_print(stack_pop(), stringtable);
                break;
            case OP_ADD: MATH_OP(+); break;
            case OP_SUBTRACT: MATH_OP(-); break;
            case OP_MULTIPLY: MATH_OP(*); break;
            case OP_DIVIDE: MATH_OP(/); break;
            case OP_EXIT:
                return stack_pop().integer;
                break;
        }
        program++;
    }
}

extern void inst_print(Instruction inst, char** stringtable) {
    switch (inst.code) {
        case OP_PUSH:
            printf("{push instruction; item ");
            item_print(inst.item, stringtable);
            putchar('}');
            break;
        case OP_ADD:
            printf("{add instruction}");
            break;
        case OP_SUBTRACT:
            printf("{subtract instruction}");
            break;
        case OP_MULTIPLY:
            printf("{multiply instruction}");
            break;
        case OP_DIVIDE:
            printf("{divide instruction}");
            break;
        case OP_PRINT:
            printf("{print instruction}");
            break;
        case OP_EXIT:
            printf("{exit instruction}");
            break;
        
    }
}

void item_print(StackItem item, char** stringtable) {
    switch (item.type) {
        case T_NULL:
            printf("null");
            break;
        case T_INT:
            printf("%d", item.integer);
            break;
        case T_STR:
            printf("%s", stringtable[item.str_idx]);
            break;
    }
}
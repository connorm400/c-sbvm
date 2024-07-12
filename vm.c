#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "vm.h"
#include "err_macro.h"

#define MATH_OP(Infix) \
    {StackItem a = stack_pop(); \
     StackItem b = stack_pop(); \
     StackItem res = { \
        .type = T_INT, .integer = a.integer Infix b.integer, }; \
     stack_push(res);}

#define JMP \
    { segment_idx = segments[segment_idx].instructions[inst_idx].segment_idx; \
      inst_idx = 0; \
      continue; } 

extern int eval(Segment* segments, char** stringtable) 
{
    size_t segment_idx = 0; // segment index. start with 0 (main/start)
    size_t inst_idx = 0;

    struct {
        size_t len, capacity;
        struct CallerAddress* arr;
    } call_stack = { .len = 0, .capacity = 0, .arr = NULL };

    for (;;) {
        // ok I know that gotos are bad but I need this to skip the for loop increment its fine ok
        
        #if DEBUG
        printf("on segment %s\n", segments[segment_idx].name);
        printf("inst_idx: %zu\n", inst_idx);
        printf("instruction: "); inst_print(segments[segment_idx].instructions[inst_idx], stringtable); putchar('\n'); 
        #endif

        switch (segments[segment_idx].instructions[inst_idx].code) {
            // stack operations
            case OP_PUSH:
                stack_push(segments[segment_idx].instructions[inst_idx].item);
                break;
            case OP_DUP:
                StackItem item = stack_dig(0);
                stack_push(item);
                break;

            case OP_DIG:
                stack_push(stack_dig(segments[segment_idx].instructions[inst_idx].idx_from_top));
                break;
            case OP_DISCARD:
                stack_pop();
                break;

            case OP_PRINT:
                item_print(stack_pop(), stringtable);
                break;

            // integer math operations
            case OP_ADD: MATH_OP(+); break;
            case OP_SUBTRACT: MATH_OP(-); break;
            case OP_MULTIPLY: MATH_OP(*); break;
            case OP_DIVIDE: MATH_OP(/); break;

            // comparison
            case OP_CMP: {
                StackItem a = stack_pop();
                StackItem b = stack_pop();
                comparisons cmp;
                if (a.integer == b.integer) cmp = EQ;
                else if (a.integer > b.integer) cmp = LT;
                else if (a.integer < b.integer) cmp = GT;

                StackItem res = { .type = T_CMP, .cmp = cmp };
                stack_push(res);
            } break;
            
            
            // function handling
            case OP_CALL: {
                if (call_stack.len >= call_stack.capacity) {
                    call_stack.capacity = call_stack.capacity == 0 ? 1 : call_stack.capacity * 2;
                    call_stack.arr = realloc(call_stack.arr, call_stack.capacity * sizeof(struct CallerAddress));
                    CHECK_ALLOC(call_stack.arr);
                }

                call_stack.arr[call_stack.len++] = (struct CallerAddress) {
                    .inst_idx = inst_idx,
                    .segment_idx = segment_idx,
                };

                JMP;
            } break;

            case OP_RET: {
                assert(call_stack.len >= 1);

                struct CallerAddress* ca = call_stack.arr + (--call_stack.len);

                inst_idx = ca->inst_idx + 1; // move onto the next instruction after the call
                segment_idx = ca->segment_idx;

                continue;
            } break;

            // control flow / jumps
            case OP_JMP: JMP; break;
            case OP_JE:  if (stack_pop().cmp == EQ) JMP; break;
            case OP_JNE: if (stack_pop().cmp != EQ) JMP; break;
            case OP_JLT: if (stack_pop().cmp == LT) JMP; break;
            case OP_JGT: if (stack_pop().cmp == GT) JMP; break;

            case OP_EXIT:
                return stack_pop().integer;
                break;

        }
        inst_idx++;
    }

    free(call_stack.arr);
}

extern void inst_print(Instruction inst, char** stringtable) 
{
    switch (inst.code) {
        case OP_PUSH:
            printf("{push instruction; item ");
            item_print(inst.item, stringtable);
            putchar('}');
            break;
        case OP_DUP: printf("{duplicate instruction}"); break;
        case OP_DIG: printf("{dig instruction}"); break;
        case OP_DISCARD: printf("{discard instruction}"); break;
        case OP_ADD: printf("{add instruction}"); break;
        case OP_SUBTRACT: printf("{subtract instruction}"); break;
        case OP_MULTIPLY: printf("{multiply instruction}"); break;
        case OP_DIVIDE: printf("{divide instruction}"); break;
        case OP_PRINT: printf("{print instruction}"); break;
        case OP_CALL: printf("{call instruction}"); break;
        case OP_RET: printf("{return instruction}"); break;
        case OP_CMP: printf("{compare instruction}"); break;
        case OP_JNE: printf("{jump if not equal instruction}"); break;
        case OP_JE: printf("{jump if equal instruction}"); break;
        case OP_JMP: printf("{jump instruction}"); break;
        case OP_JLT: printf("{jump if less than instruction}"); break;
        case OP_JGT: printf("{jump if greater than instruction}"); break;
        case OP_EXIT: printf("{exit instruction}"); break;
    }
}

void item_print(StackItem item, char** stringtable) 
{
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
        case T_CMP:
            switch (item.cmp) {
                case LT:
                    printf("less than");
                    break;
                case GT:
                    printf("greater than");
                    break;
                case EQ:
                    printf("equal");
                    break;
            }
            break;
    }
}
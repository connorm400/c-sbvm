#include "stack.h"
#include <string.h>
#include <stdlib.h>
#include "err_macro.h"

extern void stack_push(StackItem item) 
{
    if (stack.size >= stack.capacity) {
        stack.capacity = stack.capacity == 0 ? 1 : stack.capacity * 2;
        stack.arr = realloc(stack.arr, sizeof(StackItem) * stack.capacity);
        CHECK_ALLOC(stack.arr);
    }
    stack.arr[stack.size++] = item;
}

extern StackItem stack_pop() 
{
    return (stack.size == 0) ? (StackItem) { .type = T_NULL } : stack.arr[--stack.size];
}

extern StackItem stack_dig(size_t idx_from_top) 
{
    if (idx_from_top >= stack.size) return (StackItem) { .type = T_NULL };
    return stack.arr[stack.size - 1 - idx_from_top];
}

/*
extern int stack_remove(size_t idx_from_top) {
    if (idx_from_top > stack.size || idx_from_top <= 0) return -1;
    StackItem* ptr = stack.arr + (stack.size - (idx_from_top - 1));
    memmove(ptr, ptr + 1, sizeof(StackItem) * idx_from_top);
    return 0;   
}*/
#include "stack.h"
#include <string.h>

extern int stack_push(StackItem item) {
    if (stack.size == STACK_SIZE) return -1;

    stack.size++;
    stack.arr[stack.size] = item;
    return 0;
}

extern StackItem stack_pop() {
    if (stack.size == 0) return (StackItem) { .type = T_NULL };
    stack.size--;
    return stack.arr[stack.size + 1];
}

extern StackItem stack_dig(size_t idx_from_top) {
    if (idx_from_top > stack.size) return (StackItem) { .type = T_NULL };
    return stack.arr[stack.size - idx_from_top];
}

/*
extern int stack_remove(size_t idx_from_top) {
    if (idx_from_top > stack.size || idx_from_top <= 0) return -1;
    StackItem* ptr = stack.arr + (stack.size - (idx_from_top - 1));
    memmove(ptr, ptr + 1, sizeof(StackItem) * idx_from_top);
    return 0;   
}*/
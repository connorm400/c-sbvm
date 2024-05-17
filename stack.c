#include "stack.h"

int stack_push(StackItem item) {
    if (stack.size == STACK_SIZE) return -1;

    stack.size++;
    stack.arr[stack.size] = item;
    return 0;
}

StackItem stack_pop() {
    if (stack.size == 0) return (StackItem) { .type = T_NULL };
    stack.size--;
    return stack.arr[stack.size + 1];
}
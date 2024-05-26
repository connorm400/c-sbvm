#ifndef STACH_HEADER_
#define STACK_HEADER_

#define STACK_SIZE 100

#include <stdint.h>
#include <stddef.h>

typedef enum { LT, GT, EQ } comparisons;

typedef struct {
    enum { T_STR, T_INT, T_NULL, T_CMP } type;
    union {
        size_t str_idx;
        int32_t integer;
        comparisons cmp;
    };
} StackItem;

static struct {
    size_t size;
    StackItem arr[STACK_SIZE];
} stack = { .size = 0 };

extern int stack_push(StackItem item);
extern StackItem stack_pop();
extern StackItem stack_dig(size_t idx_from_top);
//extern int stack_remove(size_t idx_from_top);

#endif
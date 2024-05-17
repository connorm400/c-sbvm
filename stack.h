#ifndef STACH_HEADER_
#define STACK_HEADER_

#define STACK_SIZE 100

#include <stdint.h>
#include <stddef.h>

typedef enum { T_STR, T_INT, T_NULL } Type;
typedef struct {
    Type type;
    union {
        size_t str_idx;
        int32_t integer;
    };
} StackItem;

static struct {
    size_t size;
    StackItem arr[STACK_SIZE];
} stack = { .size = 0 };

int stack_push(StackItem item);
StackItem stack_pop();

#endif
#ifndef ERR_MACRO_HEADER__
#define ERR_MACRO_HEADER__

#include <stdlib.h>
#include <stdio.h>

#define err_quit(str, ...) \
    do { \
        fprintf(stderr, "ERROR: " str "\n" __VA_OPT__(,) ##__VA_ARGS__); \
        exit(-1); \
    } while (0)

#define CHECK_ALLOC(ptr) \
    do { \
        if ((ptr) == NULL) { \
            err_quit("null pointer - most likely ran out of memory: ptr %s file %s, line %zu", #ptr, __FILE__, __LINE__); \
        } \
    } while (0)

#endif
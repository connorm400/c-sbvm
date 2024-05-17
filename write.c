#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "vm.h"

int main(int argc, char** argv) {
    assert(argc == 2);
    FILE* f = fopen(argv[1], "wb");
    assert(f && "failled to open file");

    {
        // DATA TO WRITE
        char* strings[] = {
            "1 + 2 is ", //0
            ".\n" //1
        };
        Instruction program[] = {
            { .code = OP_PUSH, .item = { .type = T_STR, .str_idx = 0 } },
            { .code = OP_PRINT },
            { .code = OP_PUSH, .item = { .type = T_INT, .integer = 1 } },
            { .code = OP_PUSH, .item = { .type = T_INT, .integer = 2 } },
            { .code = OP_ADD },
            { .code = OP_PRINT },
            { .code = OP_PUSH, .item = { .type = T_STR, .str_idx = 1 } },
            { .code = OP_PRINT },
            { .code = OP_PUSH, .item = { .type = T_INT, .integer = 0 } },
            { .code = OP_EXIT },
        };

        size_t strarrsize = sizeof(strings) / sizeof(char*);
        fwrite(&strarrsize, sizeof(size_t), 1, f);

        for (size_t i = 0; i < sizeof(strings) / sizeof(char*); i++) {
            size_t strsize = strlen(strings[i]) + 1;
            fwrite(&strsize, sizeof(size_t), 1, f);
            fwrite(strings[i], sizeof(char), strsize, f);
        }

        size_t nmemb = sizeof(program) / sizeof(*program);
        fwrite(&nmemb, sizeof(size_t), 1, f);
        fwrite(program, sizeof(Instruction), nmemb, f);
    }

    fclose(f);
    return 0;
}
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "vm.h"

int main(int argc, char** argv) {
    assert(argc == 2);
    FILE* f = fopen(argv[1], "wb");
    assert(f && "failled to open file");
    
    #if 1
    // DATA TO WRITE
    char* strings[] = {
        /* 0 */ "(1 > 2) is ", 
        /* 1 */ ".\n",
        /* 2 */ "false",
        /* 3 */ "true"
    };

    Instruction main[] = {
        { .code = OP_PUSH, .item = { .type = T_STR, .str_idx = 0 } },
        { .code = OP_PRINT },
        { .code = OP_PUSH, .item = { .type = T_INT, .integer = 1 } },
        { .code = OP_PUSH, .item = { .type = T_INT, .integer = 2 } },
        { .code = OP_CMP },
        { .code = OP_JGT, .label_idx = 1 },
        // else 
        { .code = OP_JMP, .label_idx = 2 },
    };

    Instruction gt_branch[] = {
        { .code = OP_PUSH, .item = { .type = T_STR, .str_idx = 3 } },
        { .code = OP_PRINT },
        { .code = OP_JMP, .label_idx = 3 },
    };

    Instruction lt_branch[] = {
        { .code = OP_PUSH, .item = { .type = T_STR, .str_idx = 2}},
        { .code = OP_PRINT },
        { .code = OP_JMP, .label_idx = 3 },
    };

    Instruction end[] = {
        { .code = OP_PUSH, .item = { .type = T_STR, .str_idx = 1 } },
        { .code = OP_PRINT },
        { .code = OP_PUSH, .item = { .type = T_INT, .integer = 42 } },
        { .code = OP_EXIT },
    }; 

    Label program[] = {
        /* 0 */ LABEL(main), 
        /* 1 */ LABEL(gt_branch),
        /* 2 */ LABEL(lt_branch),
        /* 3 */ LABEL(end),
    };
    
    #endif

    // write stringtable
    {
        size_t strarrsize = sizeof(strings) / sizeof(char*);
        fwrite(&strarrsize, sizeof(size_t), 1, f);

        for (size_t i = 0; i < strarrsize; i++) {
            size_t strsize = strlen(strings[i]) + 1;
            fwrite(&strsize, sizeof(size_t), 1, f);
            fwrite(strings[i], sizeof(char), strsize, f);
        }
    }

    // write labels
    {
        size_t programs_nmemb = sizeof(program) / sizeof(Label);
        fwrite(&programs_nmemb, sizeof(size_t), 1, f);

        for (size_t i = 0; i < programs_nmemb; i++) {
            // first write label name
            size_t strsize = strlen(program[i].name) + 1;
            fwrite(&strsize, sizeof(size_t), 1, f);
            fwrite(program[i].name, sizeof(char), strsize, f);

            // and then write the instructions
            size_t nmemb = program[i].size;
            fwrite(&nmemb, sizeof(size_t), 1, f);
            fwrite(program[i].instructions, sizeof(Instruction), nmemb, f);
        }
    }
    

    fclose(f);
    return 0;
}
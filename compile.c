#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "vm.h"

int main(int argc, char** argv) {
    assert(argc == 2 && "compile <ouput>");
    FILE* f = fopen(argv[1], "wb");
    assert(f && "failled to open file");
    
    char* stringtable[] = {
        "hello, world!\n",
    };

    Instruction start[] = {
        { .code = OP_PUSH, .item = { .type = T_STR, .str_idx = 0 } },
        { .code = OP_PRINT },
        { .code = OP_PUSH, .item = { .type = T_INT, .integer = 0 } },
        { .code = OP_EXIT }, 
    };

    Segment program[] = {
        SEGMENT(start),
    };

    // write stringtable
    {
        size_t strarrsize = sizeof(stringtable) / sizeof(char*);
        fwrite(&strarrsize, sizeof(size_t), 1, f);

        for (size_t i = 0; i < strarrsize; i++) {
            size_t strsize = strlen(stringtable[i]) + 1;
            fwrite(&strsize, sizeof(size_t), 1, f);
            fwrite(stringtable[i], sizeof(char), strsize, f);
        }
    }

    // write labels
    {
        size_t programs_nmemb = sizeof(program) / sizeof(Segment);
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
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include "vm.h"

int main(int argc, char** argv) {
    assert(argc == 2 && "lacking arguments");
    FILE* f = fopen(argv[1], "rb");
    assert(f && "failed to open file");

    size_t strarrsize;
    fread(&strarrsize, sizeof(size_t), 1, f);

    char** strings = (char**)malloc(sizeof(char*) * strarrsize);
    assert(strings && "buy more ram");

    for (size_t i = 0; i < strarrsize; i++) {
        size_t strsize;
        fread(&strsize, sizeof(size_t), 1, f);

        strings[i] = (char*)malloc(sizeof(char) * strsize);
        assert(strings[i] && "buy more ram");
        
        fread(strings[i], sizeof(char), strsize, f);
    } 

    #if DEBUG
    puts("string table:");
    putchar('[');
    for (size_t i = 0; i < strarrsize; i++) {
        printf("\"%s\"%c", 
            strings[i], (i+1 == strarrsize) ? ']' : ',');
    }
    putchar('\n');
    #endif
    
    size_t nmemb;
    fread(&nmemb, sizeof(size_t), 1, f);

    Instruction* program = (Instruction*)malloc(sizeof(Instruction) * nmemb);
    assert(program && "buy more ram");
    fread(program, sizeof(Instruction), nmemb, f);
    
    fclose(f);
    
    #if DEBUG
    puts("Instructions:");
    putchar('[');
    for (size_t i = 0; i < nmemb; i++) {
        inst_print(program[i], strings);
        putchar(i+1 == nmemb ? ']' : ',');
    }
    putchar('\n');
    puts("=======");
    #endif

    int exit_res = eval(program, strings);

    // freeing stuff
    free(program);
    for (size_t i = 0; i < strarrsize; i++) {
        free(strings[i]);
    }   
    free(strings);
    
    return exit_res;
}
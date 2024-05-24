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
        printf("\"%s\"%s", 
            strings[i], (i+1 == strarrsize) ? "" : ", ");
    }
    printf("]\n");
    #endif
    
    size_t label_nmemb;
    fread(&label_nmemb, sizeof(size_t), 1, f);

    #if DEBUG
    printf("number of labels: %zu;\n", label_nmemb);
    #endif

    Label* labels = (Label*)malloc(sizeof(Label) * label_nmemb);
    assert(labels && "buy more ram");

    for (size_t i = 0; i < label_nmemb; i++) {
        // allocate and write name string
        size_t strsize;
        fread(&strsize, sizeof(size_t), 1, f);
        labels[i].name = (char*)malloc(sizeof(char) * strsize);
        assert(labels[i].name && "buy more ram");
        fread(labels[i].name, sizeof(char), strsize, f);

        // allocate and write instructions
        fread(&labels[i].size, sizeof(size_t), 1, f);
        labels[i].instructions = (Instruction*)malloc(sizeof(Instruction) * labels[i].size);
        assert(labels[i].instructions);
        fread(labels[i].instructions, sizeof(Instruction), labels[i].size, f);

        #if DEBUG
        puts("====");
        printf("strsize: %zu, str: \"%s\"\n", strsize, labels[i].name);
        printf("labelsize: %zu\n", labels[i].size);
        puts("instructions:");
        putchar('[');
        for (size_t ii = 0; ii < labels[i].size; ii++) {
            inst_print(labels[i].instructions[ii], strings);
            printf("%s", (ii + 1 == labels[i].size) ? "" : ", ");
        }
        printf("]\n");
        puts("====");
        #endif
    }

    fclose(f);

    // evaluate the program (eval returns the exit status)
    int exit_res = eval(labels, strings);

    #if DEBUG
    printf("program result: %d; exiting with: %d\n", exit_res, (uint8_t)exit_res);
    #endif

    
    // freeing stuff
    for (size_t i = 0; i < strarrsize; i++) {
        free(strings[i]);
    }   
    free(strings);
    
    for (size_t i = 0; i < label_nmemb; i++) {
        free(labels[i].instructions);
        free(labels[i].name);
    }
    free(labels);
    return exit_res;
}
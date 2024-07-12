#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include "vm.h"

int main(int argc, char** argv) 
{
    if (argc != 2) {
        fprintf(stderr, "%s: <inputfile>\n", argv[0]);
        exit(-1);
    }
    FILE* f = fopen(argv[1], "rb");
    if (f == NULL) {
        fprintf(stderr, "failed to open file %s\n", argv[1]);
        exit(-1);
    }

    size_t strarrsize;
    fread(&strarrsize, sizeof(size_t), 1, f);

    char** strings = malloc(sizeof(char*) * strarrsize);
    assert(strings && "buy more ram");

    for (size_t i = 0; i < strarrsize; i++) {
        size_t strsize;
        fread(&strsize, sizeof(size_t), 1, f);

        strings[i] = malloc(sizeof(char) * strsize);
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
    
    size_t segment_nmemb;
    fread(&segment_nmemb, sizeof(size_t), 1, f);

    #if DEBUG
    printf("number of segments: %zu;\n", segment_nmemb);
    #endif

    Segment* segments = malloc(sizeof(Segment) * segment_nmemb);
    assert(segments && "buy more ram");

    for (size_t i = 0; i < segment_nmemb; i++) {
        // allocate and write name string
        size_t strsize;
        fread(&strsize, sizeof(size_t), 1, f);
        segments[i].name = malloc(sizeof(char) * strsize);
        assert(segments[i].name && "buy more ram");
        fread(segments[i].name, sizeof(char), strsize, f);

        // allocate and write instructions
        fread(&segments[i].size, sizeof(size_t), 1, f);
        segments[i].instructions = malloc(sizeof(Instruction) * segments[i].size);
        assert(segments[i].instructions && "buy more ram");
        fread(segments[i].instructions, sizeof(Instruction), segments[i].size, f);

        #if DEBUG
        puts("====");
        printf("strsize: %zu, str: \"%s\"\n", strsize, segments[i].name);
        printf("segment size: %zu\n", segments[i].size);
        puts("instructions:");
        putchar('[');
        for (size_t ii = 0; ii < segments[i].size; ii++) {
            inst_print(segments[i].instructions[ii], strings);
            printf("%s", (ii + 1 == segments[i].size) ? "" : ", ");
        }
        printf("]\n");
        puts("====");
        #endif
    }

    fclose(f);

    // evaluate the program (eval returns the exit status)
    int exit_res = eval(segments, strings);

    #if DEBUG
    printf("program result: %d; exiting with: %d\n", exit_res, (uint8_t)exit_res);
    #endif

    
    // freeing stuff
    for (size_t i = 0; i < strarrsize; i++) {
        free(strings[i]);
    }   
    free(strings);
    
    for (size_t i = 0; i < segment_nmemb; i++) {
        free(segments[i].instructions);
        free(segments[i].name);
    }
    free(segments);
    return exit_res;
}
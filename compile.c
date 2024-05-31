#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "vm.h"
#include "compiler/lexer.h"
#include "compiler/parser.h"
#include <sys/stat.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    assert(argc == 3 && "compile <input.dra> <ouput>");
    
    // open and read input file
    FILE* input = fopen(argv[1], "rb");
    assert(input && "error opening file");

    #ifdef __linux__

    int fd = fileno(input);
    struct stat statbuf;
    assert(!fstat(fd, &statbuf));
    size_t input_filesize = (size_t)statbuf.st_size / sizeof(char);
    char* inputfilebuf = (char*)malloc(sizeof(char) * input_filesize);
    assert(inputfilebuf && "buy more ram");
    getdelim(&inputfilebuf, &input_filesize, '\0', input);
    #else
    #error we only support linux right now sorry
    #endif

    fclose(input);

    lex* l = lex_new(inputfilebuf);
    parser* p = parser_new(l);
    parser_res res = parse(p);
   
    free(inputfilebuf);

    #if DEBUG
    printf(
        "stringtable len: %zu\n"
        "number of segments: %zu\n",
        res.stringtable.len, res.segment_nmemb);
    #endif

    if (!res.r) {
        fprintf(stderr, "COMPILER ERROR: %s", res.err_msg);
        exit(1);
    }

    FILE* f = fopen(argv[2], "wb");
    assert(f && "failled to open file");
    // write stringtable
    {
        fwrite(&res.stringtable.len, sizeof(size_t), 1, f);

        for (size_t i = 0; i < res.stringtable.len; i++) {
            size_t strsize = strlen(res.stringtable.arr[i]) + 1;
            fwrite(&strsize, sizeof(size_t), 1, f);
            fwrite(res.stringtable.arr[i], sizeof(char), strsize, f);
        }
    }

    // write labels
    {
        //size_t programs_nmemb = sizeof(program) / sizeof(Segment);
        fwrite(&res.segment_nmemb, sizeof(size_t), 1, f);

        for (size_t i = 0; i < res.segment_nmemb; i++) {
            // first write label name
            size_t strsize = strlen(res.segments[i].name) + 1;
            fwrite(&strsize, sizeof(size_t), 1, f);
            fwrite(res.segments[i].name, sizeof(char), strsize, f);

            // and then write the instructions
            size_t nmemb = res.segments[i].size;
            fwrite(&nmemb, sizeof(size_t), 1, f);
            fwrite(res.segments[i].instructions, sizeof(Instruction), nmemb, f);
        }
    }
    
    parser_free(p);
    

    fclose(f);
    return 0;
}
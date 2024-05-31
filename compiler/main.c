#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char** argv) {
    assert(argc == 2 && "compile <input.dra> <output.dr>");
    FILE* f = fopen(argv[1], "rb");
    assert(f && "error opening file");

    #ifdef __linux__
    // get file size (linux-specific)
    int fd = fileno(f);
    struct stat statbuf;
    assert(!fstat(fd, &statbuf));
    size_t size = (size_t)statbuf.st_size / sizeof(char);
    
    // this only works like this because a char is one byte otherwise I would have to do math on the (long)size
    char* filebuf = (char*)malloc(size * sizeof(char));
    assert(filebuf && "buy more ram");
    getdelim(&filebuf, &size, '\0', f);
    fclose(f);

    #else
    // only supports linux right now 
    #error platform not supported
    #endif

    lex* l = lex_new(filebuf);
    parser* p = parser_new(l);
    parser_res res = parse(p);

    printf("parser result: %s\n", res.r ? "pass" : "fail");

    parser_free(p);
    free(filebuf);

    return 0;
}
#ifndef PARSER_HEADER_
#define PARSER_HEADER_
#include <stddef.h>
#include "lexer.h"
#include "../vm.h"

typedef struct {
    char** arr;
    size_t len;
    size_t capacity;
} cstr_vec;

typedef struct {
    tokens* input;
    size_t cursor;
    token* current;
    cstr_vec stringtable;

    // segment vec
    struct {
        Segment* arr;
        size_t capacity;
        size_t len;
    } segments;
} parser;

typedef struct {
    bool r; // result - wether or not it worked
    char* err_msg;
    cstr_vec stringtable;
    size_t segment_nmemb;
    Segment* segments;
} parser_res;

parser* parser_new(lex* lexer);
extern parser_res parse(parser* p);
static void advance_token(parser* p);
extern void parser_free(parser* p);
static void parse_segment(parser*);
static Instruction parse_instruction(parser*);

#endif
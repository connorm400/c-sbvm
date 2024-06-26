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

    char* err_msg;
    void (*err_dealloc)(void*);
    // segment vec
    struct {
        Segment* arr;
        size_t capacity;
        size_t len;
    } segments;

    struct {
        char** arr;
        size_t capacity;
        size_t len;
    } labels;
} parser;

typedef struct {
    bool r; // result - wether or not it worked
    char* err_msg; void (*err_dealloc)(void*);
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
static Instruction parse_jump(parser* p, OpCode t);
static void add_label(parser*);
static size_t find_label(parser*);
static void reset_parser_pos(parser*);

#endif
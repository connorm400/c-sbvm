#ifndef LEXER_HEADER_
#define LEXER_HEADER_
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    const char* input;
    size_t cursor;
    size_t input_len;
    char ch;
} lex;

extern lex* lex_new(const char* input);
extern void lex_free(lex* l);
static void _lex_nextchar(lex* l);
static void _lex_skip_whitespace(lex* l);
static bool _is_ident_letter(char c);

typedef enum {
    IDENT,
    INTEGER,
    STRING,
    LABEL,

    LPAREN,
    RPAREN,
    LSQUIRLY,
    RSQUIRLY,

    ILLEGAL,
    T_EOF,
} token_type;

typedef struct {
    token_type type;
    union {
        char* ident;
        int32_t integer;
        char* string;
        char illegal;
        char* label;
    };
} token;

extern token* lex_nexttoken(lex* l);
extern void token_free(token* t);
extern void print_token(token* t);
static token* _token_new(token_type t);


static token* _read_integer(lex* l);
static token* _read_ident(lex* l);
static token* _read_string(lex* l);


typedef struct {
    token** arr;
    size_t len;
} tokens;
// consume a lexer, converting to tokens plural
extern tokens* lexer_collect(lex* l);
extern void tokens_free(tokens* t);

#endif
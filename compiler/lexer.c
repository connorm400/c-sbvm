#include "lexer.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#define LEXER_COLLECTOR_STARTING_SIZE 100

extern lex* lex_new(const char* input) {
    lex* temp = (lex*)malloc(sizeof(lex));
    assert(temp && "buy more ram");
    temp->input = input;
    temp->input_len = strlen(input);
    assert(temp->input_len >= 1);
    temp->cursor = 0;
    temp->ch = input[temp->cursor];
    return temp;
}

extern token* lex_nexttoken(lex* l) {
    _lex_skip_whitespace(l);

    token* tok;
    switch (l->ch) {
        case '\0':
            tok = _token_new(T_EOF);
            break;
        case '{':
            tok = _token_new(LSQUIRLY);
            break;
        case '}':
            tok = _token_new(RSQUIRLY);
            break;
        case '(':
            tok = _token_new(LPAREN);
            break;
        case ')':
            tok = _token_new(RPAREN);
            break;
        case '"':
            tok = _read_string(l);
            break;
        case '%':
            _lex_nextchar(l);
            tok = _read_ident(l);
            tok->type = LABEL;
            return tok;
            break;
        default:
            if (isdigit(l->ch)) {
                return _read_integer(l);
            } else if (_is_ident_letter(l->ch)) {
                return _read_ident(l);
            } else {
                tok = _token_new(ILLEGAL);
                tok->illegal = l->ch;
            }
            break;
    }

    _lex_nextchar(l);
    return tok;
}

static void _lex_nextchar(lex* l) {
    l->cursor++;
    l->ch = l->input_len >= l->cursor ? l->input[l->cursor] : '\0';
}

static void _lex_skip_whitespace(lex* l) {
    while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n' || l->ch == '\r') 
        _lex_nextchar(l);
}

static char* __read(lex* l,  bool(*pred)(char)) { 
    size_t position = l->cursor;
    while ((*pred)(l->ch)) _lex_nextchar(l);
    char* newstr = (char*)malloc((l->cursor - position + 1) * sizeof(char));
    assert(newstr && "buy more ram");
    memcpy(newstr, l->input + position, (l->cursor - position) * sizeof(char));
    newstr[l->cursor - position] = '\0';
    return newstr;
}

static bool is_digit(char c) { return isdigit(c); }
static token* _read_integer(lex* l) {
    char* str = __read(l, is_digit);
    token* tok = _token_new(INTEGER);
    tok->integer = atoi(str);
    free(str);
    
    return tok;
}

static bool _is_ident_letter(char c) {
    return isalpha(c) || c == '!' || c == '?' || c == '-' ;
}
static token* _read_ident(lex* l) {
    char* str = __read(l, _is_ident_letter);
    token* tok = _token_new(IDENT);
    tok->ident = str;
    return tok;
}

static bool is_not_unquote(char c) { return c != '"'; }
static token* _read_string(lex* l) {
    _lex_nextchar(l);
    char* str = __read(l, is_not_unquote);
    token* tok = _token_new(STRING);
    tok->string = str;
    return tok;
}

extern void lex_free(lex* l) {
    free(l);
}

extern void token_free(token* t) {
    if (t->type == STRING || t->type == IDENT || t->type == LABEL)
        free(t->string); /* all the union member will be at the same spot so this should work */

    free(t);
}

token* _token_new(token_type type) {
    token* t = (token*)malloc(sizeof(token));
    assert(t && "buy more ram");
    t->type = type;
    return t;
}

extern void print_token(token* t) {
    switch (t->type) {
        case IDENT:
            printf("[Identifier %s]", t->ident);
            break;
        case INTEGER:
            printf("[Integer %d]", t->integer);
            break;
        case STRING:
            printf("[String \"%s\"]", t->string);
            break;
        case LABEL:
            printf("[Label %%%s]", t->label);
            break;
        case LPAREN:
            printf("(");
            break;
        case RPAREN:
            printf(")");
            break;
        case LSQUIRLY:
            printf("{");
            break;
        case RSQUIRLY:
            printf("}");
            break;
        case ILLEGAL:
            printf("[Illegal char %c]", t->illegal);
            break;
        case T_EOF:
            printf("[End of File]");
            break;
    }
}

extern tokens* lexer_collect(lex* l) {
    // making sort of a vector class for this
    struct {
        token** arr;
        size_t len;
        size_t capacity;
    } vec = { .len = 0, .capacity = LEXER_COLLECTOR_STARTING_SIZE };
    vec.arr = (token**)malloc(vec.capacity * sizeof(token*));
    assert(vec.arr && "buy more ram");

    for (token* t = lex_nexttoken(l); t->type != T_EOF; t = lex_nexttoken(l)) {
        if (vec.capacity <= vec.len) {
            vec.capacity += vec.capacity / 3;
            vec.arr = realloc(vec.arr, vec.capacity * sizeof(token*));
            assert(vec.arr && "buy more ram");
        }
        
        vec.arr[vec.len] = t;
        vec.len++;
    }
    
    // reallocate the array so its more memory efficient
    vec.arr = realloc(vec.arr, vec.len * sizeof(token*));
    tokens* temp = (tokens*)malloc(sizeof(tokens));
    assert(temp && "buy more ram");
    temp->len = vec.len;
    temp->arr = vec.arr;
    return temp;
}

extern void tokens_free(tokens* t) {
    for (size_t i = 0; i < t->len; i++) {
        token_free(t->arr[i]);
    }
    free(t);
}
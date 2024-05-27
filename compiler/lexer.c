#include "lexer.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

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
        case '#':
            _lex_nextchar(l);
            tok = _read_ident(l);
            tok->type = LABEL;
            return tok;
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

static token* _read_integer(lex* l) {
    size_t position = l->cursor;

    while (isdigit(l->ch)) _lex_nextchar(l);

    char* newstr = (char*)malloc((l->cursor - position + 1) * sizeof(char));
    assert(newstr && "buy more ram");
    memcpy(newstr, &l->input[position], (l->cursor - position) * sizeof(char));
    newstr[l->cursor - position] = '\0';
    
    token* tok = _token_new(INTEGER);
    tok->integer = atoi(newstr);
    free(newstr);

    return tok;
}

static token* _read_ident(lex* l) {
    size_t position = l->cursor;

    while (_is_ident_letter(l->ch)) _lex_nextchar(l);

    char* newstr = (char*)malloc((l->cursor - position + 1) * sizeof(char));
    assert(newstr && "buy more ram");
    memcpy(newstr, &l->input[position], (l->cursor - position) * sizeof(char));
    newstr[l->cursor - position] = '\0';

    token* tok = _token_new(IDENT);
    tok->ident = newstr;

    return tok;
}

static token* _read_string(lex* l) {
    _lex_nextchar(l);
    size_t position = l->cursor;

    while (l->ch != '"') _lex_nextchar(l);

    char* newstr = (char*)malloc((l->cursor - position + 1) * sizeof(char));
    assert(newstr && "buy more ram");
    memcpy(newstr, &l->input[position], (l->cursor - position) * sizeof(char));
    newstr[l->cursor - position] = '\0';

    token* tok = _token_new(STRING);
    tok->string = newstr;
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

static bool _is_ident_letter(char c) {
    return isalpha(c) || c == '!' || c == '?' || c == '-' ;
}

static token* _token_new(token_type type) {
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
#include "lexer.h"
#include <stdio.h>

int main(int argc, char** argv) {
    lex* l = lex_new("200 ok a-b-c \n \"hello\" \n \"world d\" () {}");

    
    for (token* t = lex_nexttoken(l); t->type != T_EOF; t = lex_nexttoken(l)) {
        print_token(t);
        token_free(t);
        putchar('\n');
    }
    
    lex_free(l);
    return 0;
}
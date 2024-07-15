#include "parser.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../err_macro.h"

#define IMPLICIT_FUNCTION_CALLS 1

#define DEFAULT_SEGMENT_SIZE 20
#define DEAFULT_STRINGTABLE_SIZE 10
#define DEFAULT_LABELS_SIZE 5

parser* parser_new(lex* lexer) 
{
    parser* temp = malloc(sizeof(parser));
    CHECK_ALLOC(temp);
    temp->cursor = 0;
    temp->input = lexer_collect(lexer);
    free(lexer);

    assert(temp->input->len > 0);
    temp->current = temp->input->arr[0];

    temp->stringtable.capacity = DEAFULT_STRINGTABLE_SIZE;
    temp->stringtable.len = 0;
    temp->stringtable.arr = malloc(sizeof(char*) * DEAFULT_STRINGTABLE_SIZE);
    CHECK_ALLOC(temp->stringtable.arr);
    temp->stringtable.arr[0] = "\n";
    temp->stringtable.len++;

    temp->segments.arr = malloc(sizeof(Segment) * DEFAULT_SEGMENT_SIZE);
    CHECK_ALLOC(temp->segments.arr);
    temp->segments.capacity = DEFAULT_SEGMENT_SIZE;
    temp->segments.len = 0;

    temp->labels.arr = malloc(sizeof(char*) * DEFAULT_LABELS_SIZE);
    CHECK_ALLOC(temp->labels.arr); 
    temp->labels.capacity = DEFAULT_LABELS_SIZE;
    temp->labels.len = 0;

    temp->err_msg = NULL;
    temp->err_dealloc = NULL;

    return temp;
}

static void advance_token(parser* p) 
{
    p->cursor++;
    p->current = p->input->len > p->cursor ? p->input->arr[p->cursor] : _token_new(T_EOF);
}

static void reset_parser_pos(parser* p) 
{
    p->cursor = 0;
    p->current = p->input->arr[0];
}

extern parser_res parse(parser* p) 
{
    parser_res res;
    res.r = true;
    res.err_dealloc = NULL;

    // first time we go through it we are just looking for label names
    while (p->current->type != T_EOF) {
        if (p->current->type == LABEL) {
            add_label(p);
            while (p->current->type != RSQUIRLY)
                advance_token(p);
        } else {
            res.r = false;
            res.err_msg = "non-label found outside of segment";
            return res;
        }
        advance_token(p);
    }

    reset_parser_pos(p);

    // here we do the rest
    while (p->current->type != T_EOF) {
        parse_segment(p); 
    
        if (p->err_msg != NULL) {
            res.r = false;
            res.err_msg = p->err_msg;
            return res;
        }
        advance_token(p);
    }

    // free up unused vec space
    p->segments.arr = realloc(p->segments.arr, p->segments.len * sizeof(Segment));
    CHECK_ALLOC(p->segments.arr);
    p->stringtable.arr = realloc(p->stringtable.arr, p->stringtable.len * sizeof(char*));
    CHECK_ALLOC(p->stringtable.arr);

    // move stuff from parser state struct to parser_res
    res.segments = p->segments.arr;
    res.segment_nmemb = p->segments.len;

    res.stringtable.arr = p->stringtable.arr;
    res.stringtable.len = p->stringtable.len;
    res.stringtable.capacity = p->stringtable.capacity;

    return res;
}

static void parse_segment(parser* p) 
{
    Segment seg = {0};
    seg.name = p->current->label;
    seg.size = 0;

    add_label(p); //
    advance_token(p); // move past %label
    advance_token(p); // move past right bracket
    
    size_t arr_capacity = DEFAULT_SEGMENT_SIZE;
    seg.instructions = malloc(sizeof(Instruction) * arr_capacity);
    CHECK_ALLOC(seg.instructions);
    while (p->current->type != RSQUIRLY) {
        #if DEBUG
        print_token(p->current); putchar('\n');
        #endif

        if (seg.size > arr_capacity) {
            arr_capacity *= 2;
            seg.instructions = realloc(seg.instructions, arr_capacity * sizeof(Instruction));
            CHECK_ALLOC(seg.instructions);
        }

        seg.instructions[seg.size] = parse_instruction(p);
        seg.size++;
        

        advance_token(p);
    }
    
    // realloc to free up memory
    if (seg.size != arr_capacity && seg.size != 0) {
        seg.instructions = realloc(seg.instructions, seg.size * sizeof(Instruction));
        CHECK_ALLOC(seg.instructions);
    }
    
    if (p->segments.len >= p->segments.capacity) {
        p->segments.capacity *= 2;
        p->segments.arr = realloc(p->segments.arr, p->segments.capacity * sizeof(Segment));
        CHECK_ALLOC(p->segments.arr);
    }
    p->segments.arr[p->segments.len] = seg;
    p->segments.len++;
}

static Instruction parse_instruction(parser* p) 
{
    Instruction inst;
    
    // TODO: all the rest of the instructions to parse
    // PUSH OPCODE
    if (p->current->type != IDENT) {
        inst.code = OP_PUSH;
        
        switch (p->current->type) {
            case INTEGER:
                inst.item.type = T_INT;
                inst.item.integer = p->current->integer;
                break;
            
            case STRING:
                inst.item.type = T_STR;
                
                // push to stringtable
                if (p->stringtable.capacity <= p->stringtable.len) {
                    p->stringtable.capacity *= 2;
                    p->stringtable.arr = realloc(p->stringtable.arr, p->stringtable.capacity * sizeof(char*));
                    CHECK_ALLOC(p->stringtable.arr);
                }

                p->stringtable.arr[p->stringtable.len] = p->current->string;
                inst.item.str_idx = p->stringtable.len;
                p->stringtable.len++;
                break;
            
            // otherwise
            default:
                inst.item.type = T_NULL;
                break;
        }

    // jumps
    } else if (strcmp(p->current->ident, "jmp") == 0) {
        inst = parse_jump(p, OP_JMP);
    } else if (strcmp(p->current->ident, "je") == 0) {
        inst = parse_jump(p, OP_JE);
    } else if (strcmp(p->current->ident, "jne") == 0) {
        inst = parse_jump(p, OP_JNE);
    } else if (strcmp(p->current->ident, "jgt") == 0) {
        inst = parse_jump(p, OP_JGT);
    } else if (strcmp(p->current->ident, "jlt") == 0) {
        inst = parse_jump(p, OP_JLT);
    
    #if ! IMPLICIT_FUNCTION_CALLS
    // function calls n stuff
    } else if (strcmp(p->current->ident, "call") == 0) {
        advance_token(p); // advance past call
        advance_token(p); // advance past (
        if (p->current->type != IDENT) p->err_msg = "call must take label as argument";

        inst.code = OP_CALL;
        inst.segment_idx = find_label(p);
        advance_token(p);
    #endif
    // other
    } else if (strcmp(p->current->ident, "exit") == 0) {
        inst.code = OP_EXIT;
    } else if (strcmp(p->current->ident, "print") == 0) {
        inst.code = OP_PRINT;
    } else if (strcmp(p->current->ident, "dup") == 0) {
        inst.code = OP_DUP;
    } else if (strcmp(p->current->ident, "discard") == 0) {
        inst.code = OP_DISCARD;
    } else if (strcmp(p->current->ident, "cmp") == 0) {
        inst.code = OP_CMP;
    } else if (strcmp(p->current->ident, "dig") == 0) {
        inst.code = OP_DIG;
        // advance over dig and (
        advance_token(p);
        advance_token(p);
        
        if (p->current->type != INTEGER) p->err_msg = "dig instruction must take integer";
        inst.idx_from_top = (size_t)p->current->integer;
        advance_token(p);

    } else if (strcmp(p->current->ident, "ret") == 0) {
        inst.code = OP_RET;
    
    
    // math
    } else if (strcmp(p->current->ident, "add") == 0) {
        inst.code = OP_ADD;
    } else if (strcmp(p->current->ident, "subtract") == 0) {
        inst.code = OP_SUBTRACT;
    } else if (strcmp(p->current->ident, "multiply") == 0) {
        inst.code = OP_MULTIPLY;
    } else if (strcmp(p->current->ident, "divide") == 0) {
        inst.code = OP_DIVIDE;
    } else if (strcmp(p->current->ident, "cr") == 0) {
        inst =  (Instruction) { .code = OP_PUSH, .item = { .type = T_STR, .str_idx = 0 }};
    } else {
        #if IMPLICIT_FUNCTION_CALLS

        inst.code = OP_CALL,
        inst.segment_idx = find_label(p);

        #else
        // forgot sprintf existed here dw about it
        char* msg1 = "identifier '";
        size_t msg1len = strlen(msg1);

        char* msg2 = p->current->ident;
        size_t msg2len = strlen(msg2);

        char* msg3 = "' not matched";
        size_t msg3len = strlen(msg3);

        size_t msglen = msg1len + msg2len + msg3len + 1;
        p->err_msg = malloc(sizeof(char) * msglen);
        CHECK_ALLOC(p->err_msg);

        strcpy(p->err_msg, msg1);
        strcpy(p->err_msg + msg1len, msg2);
        strcpy(p->err_msg + msg1len + msg2len, msg3);
        p->err_msg[msglen - 1] = '\0';
        
        p->err_dealloc = free;
        #endif

    }

    return inst;
}

static Instruction parse_jump(parser* p, OpCode t) 
{
    advance_token(p); // advance past jmp
    advance_token(p); // jump past (
    if (p->current->type != IDENT) p->err_msg = "jmp must take ident/label as argument";
    Instruction inst = { .code = t, .segment_idx = find_label(p), };
    advance_token(p);

    return inst;
}

static void add_label(parser* p) 
{
    assert(p->current->type == LABEL);
    if (p->labels.capacity <= p->labels.len) {
        p->labels.capacity *= 2;
        p->labels.arr = realloc(p->labels.arr, p->labels.capacity * sizeof(char**));
        CHECK_ALLOC(p->labels.arr);
    }

    p->labels.arr[p->labels.len] = p->current->label;
    p->labels.len++;
}

static size_t find_label(parser* p) 
{
    
    for (size_t i = 0; i < p->labels.len; i++) {
        if (strcmp(p->labels.arr[i], p->current->label) == 0) {
            return i;
        }
    }

    char* msg1 = "label '";
    size_t msg1len = strlen(msg1);

    char* msg2 = p->current->label;
    size_t msg2len = strlen(msg2);

    char* msg3 = "' not found";
    size_t msg3len = strlen(msg3);

    size_t msglen = msg1len + msg2len + msg3len + 1;
    p->err_msg = malloc(sizeof(char) * msglen);
    CHECK_ALLOC(p->err_msg);

    strcpy(p->err_msg, msg1);
    strcpy(p->err_msg + msg1len, msg2);
    strcpy(p->err_msg + msg1len + msg2len, msg3);
    p->err_msg[msglen - 1] = '\0';

    p->err_dealloc = free;    

    return 0;
}

extern void parser_free(parser* p) 
{
    tokens_free(p->input);
    if (p->stringtable.arr != NULL) free(p->stringtable.arr);
    if (p->segments.arr != NULL) free(p->segments.arr);
    free(p);
}

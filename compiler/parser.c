#include "parser.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_SEGMENT_SIZE 20
#define DEAFULT_STRINGTABLE_SIZE 10

parser* parser_new(lex* lexer) {
    parser* temp = (parser*)malloc(sizeof(parser));
    assert(temp && "buy more ram");
    temp->cursor = 0;
    temp->input = lexer_collect(lexer);
    free(lexer);
    
    assert(temp->input->len >= 1 && "input is empty");
    temp->current = temp->input->arr[0];

    temp->stringtable.capacity = DEAFULT_STRINGTABLE_SIZE;
    temp->stringtable.len = 0;
    temp->stringtable.arr = (char**)malloc(sizeof(char*) * DEAFULT_STRINGTABLE_SIZE);
    assert(temp->stringtable.arr && "buy more ram");

    temp->segments.arr = (Segment*)malloc(sizeof(Segment) * DEFAULT_SEGMENT_SIZE);
    assert(temp->segments.arr && "buy more ram");
    temp->segments.capacity = DEFAULT_SEGMENT_SIZE;
    temp->segments.len = 0;

    return temp;
}

static void advance_token(parser* p) {
    p->cursor++;
    p->current = p->input->len > p->cursor ? p->input->arr[p->cursor] : _token_new(T_EOF);
}

extern parser_res parse(parser* p) {
    parser_res res = {0};
    res.r = true;
    while (p->current->type != T_EOF) {
        if (p->current->type == LABEL) {
            parse_segment(p); 
            
        } else {
            res.r = false;
            res.err_msg = "non-label found outside of segment";
            return res;
        }
        advance_token(p);
    }

    // free up unused vec space
    p->segments.arr = realloc(p->segments.arr, p->segments.len * sizeof(Segment));
    assert(p->segments.arr && "buy more ram");
    p->stringtable.arr = realloc(p->stringtable.arr, p->stringtable.len * sizeof(char*));
    assert(p->stringtable.arr && "buy more ram");

    // move stuff from parser state struct to parser_res
    res.segments = p->segments.arr;
    res.segment_nmemb = p->segments.len;

    res.stringtable.arr = p->stringtable.arr;
    res.stringtable.len = p->stringtable.len;
    res.stringtable.capacity = p->stringtable.capacity;

    return res;
}

static void parse_segment(parser* p) {
    Segment seg = {0};
    seg.name = p->current->label;
    seg.size = 0;
    advance_token(p); // move past %label
    advance_token(p); // move past right bracket
    
    size_t arr_capacity = DEFAULT_SEGMENT_SIZE;
    seg.instructions = (Instruction*)malloc(sizeof(Instruction) * arr_capacity);
    assert(seg.instructions && "buy more ram");
    while (p->current->type != RSQUIRLY) {
        #if DEBUG
        print_token(p->current); putchar('\n');
        #endif

        if (seg.size > arr_capacity) {
            arr_capacity += arr_capacity / 3;
            seg.instructions = realloc(seg.instructions, arr_capacity * sizeof(Instruction));
            assert(seg.instructions && "buy more ram");
        }

        seg.instructions[seg.size] = parse_instruction(p);
        seg.size++;
        

        advance_token(p);
    }
    
    // realloc to free up memory
    seg.instructions = realloc(seg.instructions, seg.size * sizeof(Instruction));
    
    if (p->segments.len >= p->segments.capacity) {
        p->segments.capacity += p->segments.capacity / 3;
        p->segments.arr = realloc(p->segments.arr, p->segments.capacity * sizeof(Segment));
        assert(p->segments.arr && "buy more ram");
    }
    p->segments.arr[p->segments.len] = seg;
    p->segments.len++;
}

static Instruction parse_instruction(parser* p) {
    assert(p->current->type == IDENT);
    Instruction inst = {0};
    
    //TODO: all the rest of the instructions to parse
    // PUSH OPCODE
    if (strcmp(p->current->ident, "p") == 0) {
        inst.code = OP_PUSH;
        advance_token(p); // advance past p
        advance_token(p); // advance past (
        
        switch (p->current->type) {
            case INTEGER:
                inst.item.type = T_INT;
                inst.item.integer = p->current->integer;
                break;
            
            case STRING:
                inst.item.type = T_STR;
                
                // push to stringtable
                if (p->stringtable.capacity <= p->stringtable.len) {
                    p->stringtable.capacity += p->stringtable.capacity / 3;
                    p->stringtable.arr = realloc(p->stringtable.arr, p->stringtable.capacity * sizeof(char*));
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
        advance_token(p);

    // EXIT OPCODE
    } else if (strcmp(p->current->ident, "exit") == 0) {
        inst.code = OP_EXIT;
    // PRINT
    } else if (strcmp(p->current->ident, "print") == 0) {
        inst.code = OP_PRINT;
    } else {
        assert(0 && "PANIC");
    }

    return inst;
    
}

extern void parser_free(parser* p) {
    tokens_free(p->input);
    if (p->stringtable.arr != NULL) free(p->stringtable.arr);
    if (p->segments.arr != NULL) free(p->segments.arr);
    free(p);
}

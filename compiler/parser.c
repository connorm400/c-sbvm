#include "parser.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_SEGMENT_SIZE 20
#define DEAFULT_STRINGTABLE_SIZE 10
#define DEFAULT_LABELS_SIZE 5

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
    temp->stringtable.arr[0] = "\n";
    temp->stringtable.len++;

    temp->segments.arr = (Segment*)malloc(sizeof(Segment) * DEFAULT_SEGMENT_SIZE);
    assert(temp->segments.arr && "buy more ram");
    temp->segments.capacity = DEFAULT_SEGMENT_SIZE;
    temp->segments.len = 0;

    temp->labels.arr = (char**)malloc(sizeof(char*) * DEFAULT_LABELS_SIZE);
    assert(temp->labels.arr && "buy more ram"); 
    temp->labels.capacity = DEFAULT_LABELS_SIZE;
    temp->labels.len = 0;

    return temp;
}

static void advance_token(parser* p) {
    p->cursor++;
    p->current = p->input->len > p->cursor ? p->input->arr[p->cursor] : _token_new(T_EOF);
}

static void reset_parser_pos(parser* p) {
    p->cursor = 0;
    p->current = p->input->arr[0];
}

extern parser_res parse(parser* p) {
    parser_res res = {0};
    res.r = true;

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

    add_label(p); //
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
            arr_capacity *= 2;
            seg.instructions = realloc(seg.instructions, arr_capacity * sizeof(Instruction));
            assert(seg.instructions && "buy more ram");
        }

        seg.instructions[seg.size] = parse_instruction(p);
        seg.size++;
        

        advance_token(p);
    }
    
    // realloc to free up memory
    seg.instructions = realloc(seg.instructions, seg.size * sizeof(Instruction));
    assert(seg.instructions && "buy more ram");
    
    if (p->segments.len >= p->segments.capacity) {
        p->segments.capacity *= 2;
        p->segments.arr = realloc(p->segments.arr, p->segments.capacity * sizeof(Segment));
        assert(p->segments.arr && "buy more ram");
    }
    p->segments.arr[p->segments.len] = seg;
    p->segments.len++;
}

static Instruction parse_instruction(parser* p) {
    assert(p->current->type == IDENT);
    Instruction inst = {0};
    
    // TODO: all the rest of the instructions to parse
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
                    p->stringtable.capacity *= 2;
                    p->stringtable.arr = realloc(p->stringtable.arr, p->stringtable.capacity * sizeof(char*));
                    assert(p->stringtable.arr && "buy more ram");
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
        assert(p->current->type = INTEGER);
        inst.idx_from_top = (size_t)p->current->integer;
        advance_token(p);
    
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
        assert(0 && "not implemented");
    }

    return inst;
}

static Instruction parse_jump(parser* p, OpCode t) {
    advance_token(p); // advance past jmp
    advance_token(p); // jump past (
    Instruction inst = { .code = t, .segment_idx = find_label(p), };
    advance_token(p);

    return inst;
}

static void add_label(parser* p) {
    assert(p->current->type == LABEL);
    if (p->labels.capacity <= p->labels.len) {
        p->labels.capacity *= 2;
        p->labels.arr = realloc(p->labels.arr, p->labels.capacity * sizeof(char**));
        assert(p->labels.arr && "buy more ram");
    }

    p->labels.arr[p->labels.len] = p->current->label;
    p->labels.len++;
}

static size_t find_label(parser* p) {
    for (size_t i = 0; i < p->labels.len; i++) {
        if (strcmp(p->labels.arr[i], p->current->label) == 0) {
            return i;
        }
    }
    assert(0 && "PANIC - LABEL NOT FOUND");
}

extern void parser_free(parser* p) {
    tokens_free(p->input);
    if (p->stringtable.arr != NULL) free(p->stringtable.arr);
    if (p->segments.arr != NULL) free(p->segments.arr);
    free(p);
}

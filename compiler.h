#ifndef compiler_h
#define compiler_h

#include <stdio.h>
#include <stdlib.h>
#include "tokenizer.h"
#include "vm.h"

struct Parser_ {
    Token* current;
    Token* previous;
    bool hadError;
    bool panicMode;
};
typedef struct Parser_ Parser;

Parser* initParser() {
    Parser* parser = malloc(sizeof(Parser));
    parser->hadError = false;
    parser->panicMode = false;
    return parser;
}

void errorAt(Parser* parser, Token* token, const char* message) {
    if (parser->panicMode) {
        return;
    }
    parser->panicMode = true;
    fprintf(stderr, "[line %d] Error", token->line);
    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    }
    else if (token->type == TOKEN_ERR) {
        
    }
    else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }
    fprintf(stderr, ": %s\n", message);
    parser->hadError = true;
}

void error(Parser* parser, const char* message) {
    errorAt(parser, parser->previous, message);
}

void errorAtCurrent(Parser* parser, const char* message) {
    errorAt(parser, parser->current, message);
}

void advance(Parser* parser, Tokenizer* tokenizer) {
    parser->previous = parser->current;
    for (;;) {
        parser->current = scanToken(tokenizer);
        if (parser->current->type != TOKEN_ERR) {
            break;
        }
        errorAtCurrent(parser, parser->current->start);
    }
}

void consume(Parser* parser, Tokenizer* tokenizer, TokenType type, const char* message) {
    if (parser->current->type == type) {
        advance(parser, tokenizer);
        return;
    }
    errorAtCurrent(message);
}

void compile(Parser* parser, const char* source, Chunk* chunk) {
    Tokenizer* tokenizer = initTokenizer(source);
    Parser* parser = initParser();
    advance(parser, tokenizer);
    expression();
    consume(TOKEN_EOF, "Expect end of expression.");
    return !parser->hadError;
}

#endif
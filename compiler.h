#ifndef compiler_h
#define compiler_h

#include "scanner.h"

void compile(const char* source);

void compile(const char* source) {
    Scanner* scanner = initScanner(source);
    int line = -1;
    for (;;) {
        Token* token = scanToken(scanner);
        if (token->line != line) {
            printf("%4d ", token->line);
            line = token->line;
        }
        else {
            printf("   | ");
        }
        printf("%2d '%*.s'\n", token->type, token->length, token->start);
        if (token->type == TOKEN_EOF) {
            free(token);
            break;
        }
        free(token);
    }
}

#endif
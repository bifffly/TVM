#ifndef tokenizer_h
#define tokenizer_h

struct Tokenizer_ {
    const char* start;
    const char* current;
    int line;
};
typedef struct Tokenizer_ Tokenizer;

enum TokenType_ {
    // Bracketing Tokens
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN, TOKEN_LEFT_SQ, TOKEN_RIGHT_SQ, 
    TOKEN_LEFT_CURLY, TOKEN_RIGHT_CURLY, TOKEN_COMMA, TOKEN_DOT, 
    TOKEN_SEMICOLON, TOKEN_COLON,

    // Arithmetic Operators
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH, TOKEN_MOD,

    // Increment Operators
    TOKEN_INCR, TOKEN_DECR,

    // Comparison Operators
    TOKEN_LESS, TOKEN_LESS_EQ, TOKEN_GREATER, TOKEN_GREATER_EQ, TOKEN_EQ,

    // Negative Comparison Operators
    TOKEN_NOT_EQ,

    // Boolean Operators
    TOKEN_AND, TOKEN_OR, TOKEN_NOT,

    // Functional Operators
    TOKEN_ASSIGN, TOKEN_PERFORM, TOKEN_UNDERSCORE,

    // Literals
    TOKEN_ID, TOKEN_NUMBER, TOKEN_STRING, TOKEN_BOOLEAN,

    // Keywords
    TOKEN_FN, TOKEN_STRUCT, TOKEN_RETURN, TOKEN_THIS, TOKEN_SUPER, TOKEN_NIL,
    TOKEN_INCLUDE, TOKEN_EXCLUDE, TOKEN_MATCH, TOKEN_IF, TOKEN_ELIF, 
    TOKEN_ELSE, TOKEN_FOR, TOKEN_WHILE, TOKEN_BLOCK, TOKEN_NAMESPACE, 
    TOKEN_ERROR, TOKEN_TRY, TOKEN_CATCH, TOKEN_PRIVATE, TOKEN_PUBLIC,
    TOKEN_PROTECTED, TOKEN_ENUM, TOKEN_EXTENDS, TOKEN_CONS, TOKEN_GENERIC,

    // Types
    TOKEN_REAL, TOKEN_CHAR, TOKEN_BOOL, TOKEN_VOID, TOKEN_FREE,

    // Error
    TOKEN_ERR,
    
    // End of File
    TOKEN_EOF
};
typedef enum TokenType_ TokenType;

struct Token_ {
    TokenType type;
    const char* start;
    int length;
    int line;
};
typedef struct Token_ Token;

Tokenizer* initTokenizer(const char* source);
bool isAtEnd(Tokenizer* tokenizer);
char peek(Tokenizer* tokenizer);
char peekNext(Tokenizer* tokenizer);
Token* scanToken(Tokenizer* tokenizer);
char advance(Tokenizer* tokenizer);
bool match(Tokenizer* tokenizer, char expected);
Token* makeToken(Tokenizer* tokenizer, TokenType type);
Token* errorToken(Tokenizer* tokenizer, const char* message);
void skipWhitespace(Tokenizer* tokenizer);
Token* string(Tokenizer* tokenizer, char delimiter);
bool isDigit(char c);
Token* number(Tokenizer* tokenizer);
bool isAlpha(char c);
TokenType checkKeyword(Tokenizer* tokenizer, int start, int length, const char* remainder, TokenType type);
TokenType identifierType(Tokenizer* tokenizer);

Tokenizer* initTokenizer(const char* source) {
    Tokenizer* tokenizer = malloc(sizeof(Tokenizer));
    tokenizer->start = source;
    tokenizer->current = source;
    tokenizer->line = 1;
    return tokenizer;
}

bool isAtEnd(Tokenizer* tokenizer) {
    return *(tokenizer->current) == '\0';
}

char peek(Tokenizer* tokenizer) {
    return *tokenizer->current;
}

char peekNext(Tokenizer* tokenizer) {
    if (isAtEnd(tokenizer)) {
        return '\0';
    }
    return tokenizer->current[1];
}

char advance(Tokenizer* tokenizer) {
    tokenizer->current++;
    return tokenizer->current[-1];
}

bool match(Tokenizer* tokenizer, char expected) {
    if (isAtEnd(tokenizer)) {
        return false;
    }
    if (*tokenizer->current != expected) {
        return false;
    }
    tokenizer->current++;
    return true;
}
 
Token* makeToken(Tokenizer* tokenizer, TokenType type) {
    Token* token = malloc(sizeof(Token));
    token->type = type;
    token->start = tokenizer->start;
    token->length = (int)(tokenizer->current - tokenizer->start);
    token->line = tokenizer->line;
    return token;
}

Token* errorToken(Tokenizer* tokenizer, const char* message) {
    Token* token = malloc(sizeof(Token));
    token->type = TOKEN_ERR;
    token->start = message;
    token->length = (int)strlen(message);
    token->line = tokenizer->line;
    return token;
}

void skipWhitespace(Tokenizer* tokenizer) {
    for (;;) {
        char c = peek(tokenizer);
        switch(c) {
            case ' ':
            case '\r':
            case '\t': advance(tokenizer); break;
            case '\n':
                tokenizer->line++;
                advance(tokenizer);
                break;
            case '#': {
                while (peek(tokenizer) != '\n' && !isAtEnd(tokenizer)) {
                    advance(tokenizer);
                }
            }
            default:
                return;
        }
    }
}

Token* string(Tokenizer* tokenizer, char delimiter) {
    while (peek(tokenizer) != delimiter && !isAtEnd(tokenizer)) {
        if (peek(tokenizer) == '\n') {
            tokenizer->line++;
        }
        advance(tokenizer);
    }
    if (isAtEnd(tokenizer)) {
        return errorToken(tokenizer, "Unterminated string.");
    }
    advance(tokenizer);
    return makeToken(tokenizer, TOKEN_STRING);
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

Token* number(Tokenizer* tokenizer) {
    while (isDigit(peek(tokenizer))) {
        advance(tokenizer);
    }
    if (peek(tokenizer) == '.' && isDigit(peekNext(tokenizer))) {
        advance(tokenizer);
        while (isDigit(peek(tokenizer))) {
            advance(tokenizer);
        }
    }
    return makeToken(tokenizer, TOKEN_NUMBER);
}

bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

TokenType checkKeyword(Tokenizer* tokenizer, int start, int length, const char* remainder, TokenType type) {
    if (tokenizer->current - tokenizer->start == start + length &&
        memcmp(tokenizer->start + start, remainder, length) == 0) {
        return type;
    }
    return TOKEN_ID;
}

TokenType identifierType(Tokenizer* tokenizer) {
    switch (tokenizer->start[0]) {
        case 'a': return checkKeyword(tokenizer, 1, 2, "nd", TOKEN_AND);
        case 'b': {
            if (tokenizer->current - tokenizer->start > 1) {
                switch (tokenizer->start[1]) {
                    case 'l': return checkKeyword(tokenizer, 2, 3, "ock", TOKEN_BLOCK);
                    case 'o': return checkKeyword(tokenizer, 2, 2, "ol", TOKEN_BOOL);
                }
            }
            break;
        }
        case 'c': {
            if (tokenizer->current - tokenizer->start > 1) {
                switch (tokenizer->start[1]) {
                    case 'a': return checkKeyword(tokenizer, 2, 3, "tch", TOKEN_CATCH);
                    case 'h': return checkKeyword(tokenizer, 2, 2, "ar", TOKEN_CHAR);
                    case 'o': return checkKeyword(tokenizer, 2, 2, "ns", TOKEN_CONS);
                }
            }
            break;
        }
        case 'e': {
            if (tokenizer->current - tokenizer->start > 1) {
                switch (tokenizer->start[1]) {
                    case 'l': {
                        if (tokenizer->current - tokenizer->start > 2) {
                            switch(tokenizer->start[2]) {
                                case 'i': return checkKeyword(tokenizer, 3, 1, "f", TOKEN_ELIF);
                                case 's': return checkKeyword(tokenizer, 3, 1, "s", TOKEN_ELSE);
                            }
                        }
                        break;
                    }
                    case 'n': return checkKeyword(tokenizer, 2, 2, "um", TOKEN_ENUM);
                    case 'r': return checkKeyword(tokenizer, 2, 3, "ror", TOKEN_ERROR);
                    case 'x': {
                        if (tokenizer->current - tokenizer->start > 2) {
                            switch(tokenizer->start[2]) {
                                case 'c': return checkKeyword(tokenizer, 3, 4, "lude", TOKEN_EXCLUDE);
                                case 't': return checkKeyword(tokenizer, 3, 4, "ends", TOKEN_EXTENDS);
                            }
                        }
                        break;
                    }
                }
            }
            break;
        }
        case 'f': {
            if (tokenizer->current - tokenizer->start > 1) {
                switch(tokenizer->start[1]) {
                    case 'n': return TOKEN_FN;
                    case 'o': return checkKeyword(tokenizer, 2, 1, "r", TOKEN_FOR);
                    case 'r': return checkKeyword(tokenizer, 2, 2, "ee", TOKEN_FREE);
                }
            }
            break;
        }
        case 'g': return checkKeyword(tokenizer, 1, 6, "eneric", TOKEN_GENERIC);
        case 'i': {
            if (tokenizer->current - tokenizer->start > 1) {
                switch(tokenizer->start[1]) {
                    case 'f': return TOKEN_IF;
                    case 'n': return checkKeyword(tokenizer, 2, 5, "clude", TOKEN_INCLUDE);
                }
            }
            break;
        }
        case 'm': return checkKeyword(tokenizer, 1, 4, "atch", TOKEN_MATCH);
        case 'n': {
            if (tokenizer->current - tokenizer->start > 1) {
                switch (tokenizer->start[1]) {
                    case 'a': return checkKeyword(tokenizer, 2, 7, "mespace", TOKEN_NAMESPACE);
                    case 'i': return checkKeyword(tokenizer, 2, 1, "l", TOKEN_NIL);
                }
            }
            break;
        }
        case 'o': return checkKeyword(tokenizer, 1, 1, "r", TOKEN_OR);
        case 'p': {
            if (tokenizer->current - tokenizer->start > 1) {
                switch (tokenizer->start[1]) {
                    case 'r': {
                        if (tokenizer->current - tokenizer->start > 2) {
                            switch (tokenizer->start[2]) {
                                case 'i': return checkKeyword(tokenizer, 3, 4, "vate", TOKEN_PRIVATE);
                                case 'o': return checkKeyword(tokenizer, 3, 6, "tected", TOKEN_PROTECTED);
                            }
                        }
                        break;
                    }
                    case 'u': return checkKeyword(tokenizer, 2, 4, "blic", TOKEN_PUBLIC);    
                }
            }
            break;
        }
        case 'r': {
            if (tokenizer->current - tokenizer->start > 1) {
                switch (tokenizer->start[1]) {
                    case 'e': {
                        if (tokenizer->current - tokenizer->start > 2) {
                            switch (tokenizer->start[2]) {
                                case 'a': return checkKeyword(tokenizer, 3, 1, "l", TOKEN_REAL);
                                case 't': return checkKeyword(tokenizer, 3, 3, "urn", TOKEN_RETURN);
                            }
                        }
                        break;
                    }
                }
            }
            break;
        }
        case 's': {
            if (tokenizer->current - tokenizer->start > 1) {
                switch (tokenizer->start[1]) {
                    case 't': return checkKeyword(tokenizer, 2, 4, "ruct", TOKEN_STRUCT);
                    case 'u': return checkKeyword(tokenizer, 2, 3, "per", TOKEN_SUPER);
                }
            }
            break;
        }
        case 't': {
            if (tokenizer->current - tokenizer->start > 1) {
                switch (tokenizer->start[1]) {
                    case 'h': return checkKeyword(tokenizer, 2, 2, "is", TOKEN_THIS);
                    case 'r': return checkKeyword(tokenizer, 2, 1, "y", TOKEN_TRY);
            }
            break;
        }                
        case 'v': return checkKeyword(tokenizer, 1, 3, "oid", TOKEN_VOID);
        case 'w': return checkKeyword(tokenizer, 1, 4, "hile", TOKEN_WHILE);
        }
    }
    
    return TOKEN_ID;
}

Token* identifier(Tokenizer* tokenizer) {
    while (isAlpha(peek(tokenizer)) || isDigit(peek(tokenizer))) {
        advance(tokenizer);
    }
    return makeToken(tokenizer, identifierType(tokenizer));
}
 
Token* scanToken(Tokenizer* tokenizer) {
    skipWhitespace(tokenizer);
    tokenizer->start = tokenizer->current;
    
    if (isAtEnd(tokenizer)) {
        return makeToken(tokenizer, TOKEN_EOF);
    }
    
    char c = advance(tokenizer);
    if (isAlpha(c)) {
        return identifier(tokenizer);
    }
    if (isDigit(c)) {
        return number(tokenizer);
    }
    switch (c) {
        case '(': return makeToken(tokenizer, TOKEN_LEFT_PAREN);
        case ')': return makeToken(tokenizer, TOKEN_RIGHT_PAREN);
        case '[': return makeToken(tokenizer, TOKEN_LEFT_SQ);
        case ']': return makeToken(tokenizer, TOKEN_RIGHT_SQ);
        case '{': return makeToken(tokenizer, TOKEN_LEFT_CURLY);
        case '}': return makeToken(tokenizer, TOKEN_RIGHT_CURLY);
        case '+': return makeToken(tokenizer, TOKEN_PLUS);
        case '-': return makeToken(tokenizer, match(tokenizer, '>') ? TOKEN_PERFORM : TOKEN_MINUS);
        case '*': return makeToken(tokenizer, TOKEN_STAR);
        case '/': return makeToken(tokenizer, TOKEN_SLASH);
        case ',': return makeToken(tokenizer, TOKEN_COMMA);
        case '.': return makeToken(tokenizer, TOKEN_DOT);
        case ';': return makeToken(tokenizer, TOKEN_SEMICOLON);
        case ':': return makeToken(tokenizer, TOKEN_COLON);
        case '_': return makeToken(tokenizer, TOKEN_UNDERSCORE);
        case '<': return makeToken(tokenizer, match(tokenizer, '=') ? TOKEN_LESS_EQ : TOKEN_LESS);
        case '>': return makeToken(tokenizer, match(tokenizer, '=') ? TOKEN_GREATER_EQ : TOKEN_GREATER);
        case '=': return makeToken(tokenizer, match(tokenizer, '=') ? TOKEN_EQ : TOKEN_ASSIGN);
        case '!': return makeToken(tokenizer, match(tokenizer, '=') ? TOKEN_NOT_EQ : TOKEN_NOT);
        case '"': return string(tokenizer, '"');
        case '\'': return string(tokenizer, '\'');
    }
    
    return errorToken(tokenizer, "Unexpected character.");
}

#endif 
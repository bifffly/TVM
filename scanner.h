#ifndef scanner_h
#define scanner_h

struct Scanner_ {
    const char* start;
    const char* current;
    int line;
};
typedef struct Scanner_ Scanner;

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
    TOKEN_PROTECTED, TOKEN_ENUM, TOKEN_EXTENDS, TOKEN_CONS,

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

Scanner* initScanner(const char* source);
Token* scanToken(Scanner* scanner);

Scanner* initScanner(const char* source) {
    Scanner* scanner = malloc(sizeof(Scanner));
    scanner->start = source;
    scanner->current = source;
    scanner->line = 1;
    return scanner;
}

static bool isAtEnd(Scanner* scanner) {
    return *(scanner->current) == '\0';
}
 
static Token* makeToken(Scanner* scanner, TokenType type) {
    Token* token = malloc(sizeof(Token));
    token->type = type;
    token->start = scanner->start;
    token->length = (int)(scanner->current - scanner->start);
    token->line = scanner->line;
    return token;
}

static Token* errorToken(Scanner* scanner, const char* message) {
    Token* token = malloc(sizeof(Token));
    token->type = TOKEN_ERR;
    token->start = message;
    token->length = (int)strlen(message);
    token->line = scanner->line;
    return token;
}
 
Token* scanToken(Scanner* scanner) {
    scanner->start = scanner->current;
    
    if (isAtEnd(scanner)) {
        return makeToken(scanner, TOKEN_EOF);
    }
    
    char c = advance();
    switch (c) {
        case '(': return makeToken(scanner, TOKEN_LEFT_PAREN);
        case ')': return makeToken(scanner, TOKEN_RIGHT_PAREN);
        case '[': return makeToken(scanner, TOKEN_LEFT_SQ);
        case ']': return makeToken(scanner, TOKEN_RIGHT_SQ);
        case '{': return makeToken(scanner, TOKEN_LEFT_CURLY);
        case '}': return makeToken(scanner, TOKEN_RIGHT_CURLY);
        case '+': return makeToken(scanner, TOKEN_PLUS);
        case '-': return makeToken(scanner, TOKEN_MINUS);
        case '*': return makeToken(scanner, TOKEN_STAR);
        case '/': return makeToken(scanner, TOKEN_SLASH);
        case ',': return makeToken(scanner, TOKEN_COMMA);
        case '.': return makeToken(scanner, TOKEN_DOT);
        case ';': return makeToken(scanner, TOKEN_SEMICOLON);
        case ':': return makeToken(scanner, TOKEN_COLON);
        case '_': return makeToken(scanner, TOKEN_UNDERSCORE);
    }
    
    return errorToken(scanner, "Unexpected character.");
}

#endif 
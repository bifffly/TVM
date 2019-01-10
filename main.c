#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

void repl(VM* vm) {
    char* line = malloc(1024*sizeof(char));
    for (;;) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }
        interpret(vm, line);
    }
    free(line);
}

char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file %s.\n", path);
        exit(4);
    }
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);
    
    char* buffer = calloc(fileSize + 1, sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read file %s.\n", path);
    }
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not read file %s.\n", path);
    }
    
    fclose(file);
    return buffer;
}

void runFile(VM* vm, const char* path) {
    char* source = readFile(path);
    InterpretResult result = interpret(vm, source);
    free(source);
    
    if (result == INTERPRET_COMPILE_ERROR) {
        exit(2);
    }
    if (result == INTERPRET_RUNTIME_ERROR) {
        exit(3);
    }
}

int main(int argc, const char* argv[]) {
    VM* vm = initVM();
    
    if (argc == 1) {
        repl(vm);
    }
    else if (argc == 2) {
        runFile(vm, argv[1]);
    }
    else {
        fprintf(stderr, "Usage: ctcomp <file>\n");
        exit(1);
    }
    
    freeVM(vm);
    
    return 0;
}

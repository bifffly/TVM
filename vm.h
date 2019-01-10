#ifndef vm_h
#define vm_h

#include <stdio.h>
#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "value.h"
#include "compiler.h"

#define STACK_MAX 256

struct VM_ {
    Chunk* chunk;
    uint8_t* ip;
    Value stack[STACK_MAX];
    Value* stackTop;
};
typedef struct VM_ VM;

enum InterpretResult_ {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
};
typedef enum InterpretResult_ InterpretResult;

void resetStack(VM* vm) {
    vm->stackTop = vm->stack;
}

VM* initVM() {
    VM* vm = malloc(sizeof(VM));
    vm->chunk = NULL;
    vm->ip = NULL;
    resetStack(vm);
    return vm;
}

void freeVM(VM* vm) {
    free(vm);
}

void push(VM* vm, Value value) {
    *vm->stackTop = value;
    vm->stackTop++;
}

Value pop(VM* vm) {
    vm->stackTop--;
    return *vm->stackTop;
}

InterpretResult run(VM* vm) {
    #define READ_BYTE() (*vm->ip++)
    #define READ_CONSTANT() (vm->chunk->constants->values[READ_BYTE()])
    #define BINARY_OP(op) \
        do { \
            double b = pop(vm); \
            double a = pop(vm); \
            push(vm, a op b); \
        } while (false)
    for (;;) {
        #ifdef DEBUG_TRACE_EXECUTION
            printf("          ");
            for (Value* slot = vm->stack; slot < vm->stackTop; slot++) {
                printf("[ ");
                printValue(*slot);
                printf(" ]");
            }
            printf("\n");
            disassembleInstruction(vm->chunk, (int)(vm->ip - vm->chunk->code));
        #endif
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_RETURN:
                printValue(pop(vm));
                printf("\n");
                return INTERPRET_OK;
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(vm, constant);
                break;
            }
            case OP_NEGATE: {
                push(vm, -pop(vm));
                break;
            }
            case OP_ADD: BINARY_OP(+); break;
            case OP_SUB: BINARY_OP(-); break;
            case OP_MULT: BINARY_OP(*); break;
            case OP_DIV: BINARY_OP(/); break;
        }
    }
    #undef READ_BYTE
    #undef READ_CONSTANT
    #undef BINARY_OP
}
    

InterpretResult interpret(VM* vm, const char* source) {
    Chunk* chunk = initChunk();
    if (!compile(source, chunk)) {
        freeChunk(chunk);
        return INTERPRET_COMPILE_ERROR;
    }
    vm->chunk = chunk;
    vm->ip = vm->chunk->code;
    
    InterpretResult result = run(vm);
    
    freeChunk(chunk);
    return result;
}

#endif
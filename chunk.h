#ifndef chunk_h
#define chunk_h

#include "common.h"
#include "value.h"

enum OpCode_ {
    OP_RETURN,
    OP_NEGATE,
    OP_ADD,
    OP_SUB,
    OP_MULT,
    OP_DIV,
    OP_CONSTANT
};
typedef enum OpCode_ OpCode;

struct Chunk_ {
    int count;
    int capacity;
    uint8_t* code;
    int* lines;
    ValueArray* constants;
};
typedef struct Chunk_ Chunk;

Chunk* initChunk();
void writeChunk(Chunk* chunk, uint8_t byte, int line);
void freeChunk(Chunk* chunk);
int addConstant(Chunk* chunk, Value value);

Chunk* initChunk() {
    Chunk* chunk = malloc(sizeof(Chunk));
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lines = NULL;
    chunk->constants = initValueArray();
    return chunk;
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = grow_capacity(oldCapacity);
        chunk->code = grow_array(chunk->code, uint8_t, oldCapacity,
                chunk->capacity);
        chunk->lines = grow_array(chunk->lines, int, oldCapacity,
                chunk->capacity);
    }
    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

int addConstant(Chunk* chunk, Value value) {
    writeValueArray(chunk->constants, value);
    return chunk->constants->count - 1;
}

void freeChunk(Chunk* chunk) {
    free_array(uint8_t, chunk->code, chunk->capacity);
    free_array(int, chunk->lines, chunk->capacity);
    freeValueArray(chunk->constants);
    chunk = initChunk(chunk);
}

#endif

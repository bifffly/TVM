#ifndef value_h
#define value_h

#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "common.h"

typedef double Value;

struct ValueArray_ {
    int capacity;
    int count;
    Value* values;
};
typedef struct ValueArray_ ValueArray;

ValueArray* initValueArray();
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);
void printValue(Value value);

ValueArray* initValueArray() {
    ValueArray* array = malloc(sizeof(ValueArray));
    array->capacity = 0;
    array->count = 0;
    array->values = NULL;
    return array;
}

void writeValueArray(ValueArray* array, Value value) {
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;
        array->capacity = grow_capacity(oldCapacity);
        array->values = grow_array(array->values, Value, oldCapacity,
                array->capacity);
    }
    array->values[array->count] = value;
    array->count++;
}

void freeValueArray(ValueArray* array) {
    free_array(Value, array->values, array->capacity);
    array = initValueArray();
}

void printValue(Value value) {
    printf("%g", value);
}


#endif

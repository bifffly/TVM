#ifndef memory_h
#define memory_h

#define grow_capacity(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define grow_array(previous, type, oldCount, count) \
    (type*)reallocate(previous, sizeof(type)*(oldCount), \
            sizeof(type)*(count))

#define free_array(type, pointer, oldcount) \
    reallocate(pointer, sizeof(type)*(oldcount), 0)

void* reallocate(void* previous, size_t oldSize, size_t newSize);

void* reallocate(void* previous, size_t oldSize, size_t newSize) {
    if (newSize == 0) {
        free(previous);
        return NULL;
    }
    return realloc(previous, newSize);
}

#endif

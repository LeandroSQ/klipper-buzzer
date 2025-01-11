#include "vector.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

const int INITIAL_CAPACITY = 16;
const int GROWTH_FACTOR = 2;

Vector* createVector(size_t elementSize) {
    Vector* vector = malloc(sizeof(Vector));
    if (vector == NULL) {
        return NULL; // Allocation failed
    }
    vector->data = malloc(elementSize * INITIAL_CAPACITY);
    if (vector->data == NULL) {
        free(vector);
        return NULL; // Allocation failed
    }
    vector->size = 0;
    vector->capacity = INITIAL_CAPACITY;
    vector->elementSize = elementSize;

    return vector;
}

bool vectorAppend(Vector* vector, void* element) {
    if (vector->size == vector->capacity) {
        size_t newCapacity = vector->capacity * GROWTH_FACTOR;
        void* newData = realloc(vector->data, vector->elementSize * newCapacity);
        if (newData == NULL) {
            return false; // Reallocation failed
        }
        vector->data = newData;
        vector->capacity = newCapacity;
    }

    // Copy the element into the vector's data buffer
    memcpy((char*)vector->data + vector->size * vector->elementSize, element, vector->elementSize);
    vector->size++;
    return true;
}

void* vectorGet(Vector* vector, size_t index) {
    if (index >= vector->size) {
        return NULL; // Out of bounds
    }
    return (char*)vector->data + index * vector->elementSize;
}

void freeVector(Vector* vector) {
    free(vector->data);
    free(vector);
}

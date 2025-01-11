#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    void** data;
    size_t size;
    size_t capacity;
    size_t elementSize;
} Vector;

/**
 * @brief Create a new Vector object
 *
* @param elementSize The size of each element in the Vector
* @return Vector* The new Vector object
 */
Vector* createVector(size_t elementSize);

/**
 * @brief Add an element to the Vector
 *
 * @param vector The Vector object
 * @param element The element to add
 * @return true If the element was added successfully
 */
bool vectorAppend(Vector* vector, void* element);

/**
 * @brief Get the element at the specified index
 *
 * @param vector The Vector object
 * @param index The index to get
 * @return void* The element at the specified index
 */
void* vectorGet(Vector* vector, size_t index);

/**
 * @brief Free the Vector object
 *
 * @param vector The Vector object
 */
void freeVector(Vector* vector);


#endif
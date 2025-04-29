#ifndef TYPES_H
#define TYPES_H
#include <stdlib.h>
// Dynamic String Array
typedef struct StringArray {
    char **strings;
    size_t count;
    size_t capacity;
} StringArray;

inline void appendString(StringArray *array, char *string) {
    if (array->count >= array->capacity) {
        if (array->capacity == 0) {
            array->capacity = 256;
            array->strings = malloc(array->capacity * sizeof(char *));
        }
        else {
            array->capacity *= 2;
            array->strings = realloc(array->strings, array->capacity * sizeof(char *));
        }
    }
    array->strings[array->count++] = string;
}
#endif //TYPES_H

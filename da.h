#ifndef cdiff_da_h
#define cdiff_da_h

#include <stdio.h>
#include <stdlib.h>

#define DA_INITIAL_CAPACITY 8

#define da_append(da, item)                                                                         \
    do {                                                                                            \
        if ((da)->count >= (da)->capacity) {                                                        \
            (da)->capacity = (da)->capacity == 0 ? DA_INITIAL_CAPACITY : (da)->capacity * 3 / 2;    \
            (da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items));              \
            if ((da)->items == NULL) exit(1);                                                       \
        }                                                                                           \
                                                                                                    \
        (da)->items[(da)->count++] = (item);                                                        \
    } while (0)

#define da_free(da)     \
    do {                \
        da->size = 0;   \
        free(a->items); \
    } while (0)

#endif

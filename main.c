#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "matrix.h"
#include "da.h"

#define MAX(x, y) x > y ? x : y

typedef struct {
    size_t count;
    size_t capacity;
    char **items;
} Buffer;

typedef enum {
    DIFF_ADDITION,
    DIFF_REMOVAL,
    DIFF_UNCHANGED,
} Diff_Type;

typedef struct {
    Diff_Type type;
    char *line;
} Diff_Element;

typedef struct {
    size_t count;
    size_t capacity;
    Diff_Element *items;
} Diff_Array;

void readlines(FILE *fp, Buffer *buffer) {
    char chunk[128];

    size_t len = sizeof(chunk);
    char *p = malloc(len);
    p[0] = '\0';

    while (fgets(chunk, sizeof(chunk), fp) != NULL) {
        size_t used_len = strlen(p);
        size_t chunk_size = strlen(chunk);

        if (chunk_size > len - used_len) {
            len = len + chunk_size;
            p = realloc(p, len);

            if (p == NULL) {
                printf("Could not realloc");
                exit(1);
            }
        }

        strncpy(p + used_len, chunk, len - used_len);
        used_len += chunk_size;

        if (p[used_len - 1] == '\n') {
            p[used_len - 1] = '\0';

            char *line = malloc(strlen(p) + 1);
            strcpy(line, p);
            da_append(buffer, line);
            p[0] = '\0';
        }
    }

    free(p);
}

Buffer read_file(char *name) {
    FILE *fp = fopen(name, "r");

    if (fp == NULL) {
        printf("Could not open file: %s\n", name);
        exit(1);
    }

    Buffer buffer = {0};
    readlines(fp, &buffer);

    fclose(fp);

    return buffer;
}

void free_buffer(Buffer *buffer) {
    for (size_t i = 0; i < buffer->count; ++i) {
        free(buffer->items[i]);
    }
}

void print_matrix(int **matrix, int rows, int columns) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            printf("%d ", matrix[i][j]);
        }

        printf("\n");
    }
}

void print_buffer(Buffer *buffer) {
    for (size_t i = 0; i < buffer->count; ++i) {
        printf("Count: %zu, Capacity: %zu\n\n", buffer->count, buffer->capacity);
        printf("%3zu: %s\n", i + 1, buffer->items[i]);
    }
}

void lcs_matrix(int **matrix, Buffer *buffer1, Buffer *buffer2) {
    for (size_t i = 0; i < buffer1->count + 1; ++i) {
        for (size_t j = 0; j < buffer2->count + 1; ++j) {
            if (i == 0 || j == 0) {
                matrix[i][j] = 0;
            } else if (!strcmp(buffer1->items[i - 1], buffer2->items[j - 1])) {
                matrix[i][j] = 1 + matrix[i - 1][j - 1];
            } else {
                matrix[i][j] = MAX(matrix[i - 1][j], matrix[i][j - 1]);
            }
        }
    }
}

Buffer lcs(int **matrix, Buffer *buffer1, Buffer *buffer2) {
    Buffer result = {0};
    size_t i = buffer1->count, j = buffer2->count;

    while (i != 0 && j != 0) {
        if (!strcmp(buffer1->items[i - 1], buffer2->items[j - 1])) {
            da_append(&result, buffer1->items[i - 1]);

            i -= 1;
            j -= 1;
        } else if (matrix[i - 1][j] <= matrix[i][j - 1]) {
            j -= 1;
        } else {
            i -= 1;
        }
    }

    char *temp;
    for (size_t i = 0, k = result.count - 1; i < k; i++, k--) {
        temp = result.items[i];
        result.items[i] = result.items[k];
        result.items[k] = temp;
    }

    return result;
}

Diff_Array get_diff(int **matrix, Buffer *buffer1, Buffer *buffer2) {
    Diff_Array diff = {0};

    size_t i = buffer1->count, j = buffer2->count;
    while (i != 0 || j != 0) {
        Diff_Element element;

        if (i == 0) {
            element = (Diff_Element) {.type = DIFF_ADDITION, .line = buffer2->items[j - 1]};

            j -= 1;
        } else if (j == 0) {
            element = (Diff_Element) {.type = DIFF_REMOVAL, .line = buffer1->items[i - 1]};

            i -= 1;
        } else if (!strcmp(buffer1->items[i - 1], buffer2->items[j - 1])) {
            element = (Diff_Element) {.type = DIFF_UNCHANGED, .line = buffer1->items[i - 1]};

            i -= 1;
            j -= 1;
        } else if (matrix[i - 1][j] <= matrix[i][j - 1]) {
            element = (Diff_Element) {.type = DIFF_ADDITION, .line = buffer2->items[j - 1]};

            j -= 1;
        } else {
            element = (Diff_Element) {.type = DIFF_REMOVAL, .line = buffer1->items[i - 1]};

            i -= 1;
        }

        da_append(&diff, element);
    }

    Diff_Element temp;

    for (int i = 0, k = diff.count - 1; i < k; i++, k--) {
        temp = diff.items[i];
        diff.items[i] = diff.items[k];
        diff.items[k] = temp;
    }

    return diff;
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: diff file1 file2\n");
        return 1;
    }

    Buffer buffer1 = read_file(argv[1]);
    Buffer buffer2 = read_file(argv[2]);

    Matrix matrix = allocate_matrix(buffer1.count + 1, buffer2.count + 1);
    lcs_matrix(matrix.m, &buffer1, &buffer2);

    Diff_Array diff = get_diff(matrix.m, &buffer1, &buffer2);
    free_matrix(&matrix);

    for (size_t i = 0; i < diff.count; i++) {
        Diff_Element diff_element = diff.items[i];

        switch (diff_element.type) {
        case DIFF_ADDITION:
            printf("\033[0;32m");
            printf("+%s\n", diff_element.line);
            printf("\033[0m");
            break;
        case DIFF_REMOVAL:
            printf("\033[0;31m");
            printf("-%s\n", diff_element.line);
            printf("\033[0m");
            break;
        default:
            break;
        }
    }

    free_buffer(&buffer1);
    free_buffer(&buffer2);

    return 0;
}

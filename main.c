#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "matrix.h"

#define MAX_LINES 1024
#define MAX_LINE_SIZE 1024
#define MAX(x, y) x > y ? x : y

typedef struct {
    char *lines[MAX_LINES];
    int size; // in lines
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
    Diff_Element elements[MAX_LINE_SIZE * 2];
    int size;
} Diff;

size_t readline(FILE *fp, char *s, int max) {
    if (fgets(s, max, fp) != NULL) {
        return strlen(s);
    }

    return 0;
}

int readlines(FILE *fp, char **lines) {
    char *p, s[1024];
    size_t size;
    int lines_count = 0;

    while ((size = readline(fp, s, MAX_LINE_SIZE)) > 0) {
        p = malloc(size);

        if (p == NULL || lines_count >= MAX_LINES) {
            printf("Most likely lines count exceeds limit");
            break;
        } else {
            s[size - 1] = '\0';
            strcpy(p, s);
            lines[lines_count++] = p;
        }
    }

    return lines_count;
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
    for (int i = 0; i < buffer->size; ++i) {
        printf("%3d: %s\n", i + 1, buffer->lines[i]);
    }
}

void lcs_matrix(int **matrix, Buffer *buffer1, Buffer *buffer2) {
    for (int i = 0; i < buffer1->size + 1; ++i) {
        for (int j = 0; j < buffer2->size + 1; ++j) {
            if (i == 0 || j == 0) {
                matrix[i][j] = 0;
            } else if (!strcmp(buffer1->lines[i - 1], buffer2->lines[j - 1])) {
                matrix[i][j] = 1 + matrix[i - 1][j - 1];
            } else {
                matrix[i][j] = MAX(matrix[i - 1][j], matrix[i][j - 1]);
            }
        }
    }
}

Buffer lcs(int **matrix, Buffer *buffer1, Buffer *buffer2) {
    Buffer result;
    int i = buffer1->size, j = buffer2->size;
    char *p;

    int size = 0;
    while (i != 0 && j != 0) {
        if (!strcmp(buffer1->lines[i - 1], buffer2->lines[j - 1])) {
            p = malloc(strlen(buffer1->lines[i - 1]));
            strcpy(p, buffer1->lines[i - 1]);
            result.lines[size++] = p;

            i -= 1;
            j -= 1;
        } else if (matrix[i - 1][j] <= matrix[i][j - 1]) {
            j -= 1;
        } else {
            i -= 1;
        }
    }

    result.size = size;

    char *temp;
    for (int i = 0, k = size - 1; i < k; i++, k--) {
        temp = result.lines[i];
        result.lines[i] = result.lines[k];
        result.lines[k] = temp;
    }

    return result;
}

Diff get_diff(int **matrix, Buffer *buffer1, Buffer *buffer2) {
    /* TODO: dynamic array */
    Diff diff;

    int i = buffer1->size, j = buffer2->size;
    int size = 0;
    while (i != 0 || j != 0) {
        if (i == 0) {
            diff.elements[size++] = (Diff_Element) {.type = DIFF_ADDITION, .line = buffer2->lines[j - 1]};
            j -= 1;
        } else if (j == 0) {
            diff.elements[size++] = (Diff_Element) {.type = DIFF_REMOVAL, .line = buffer1->lines[i - 1]};
            i -= 1;
        } else if (!strcmp(buffer1->lines[i - 1], buffer2->lines[j - 1])) {
            diff.elements[size++] = (Diff_Element) {.type = DIFF_UNCHANGED, .line = buffer1->lines[i - 1]};
            i -= 1;
            j -= 1;
        } else if (matrix[i - 1][j] <= matrix[i][j - 1]) {
            diff.elements[size++] = (Diff_Element) {.type = DIFF_ADDITION, .line = buffer2->lines[j - 1]};
            j -= 1;
        } else {
            diff.elements[size++] = (Diff_Element) {.type = DIFF_REMOVAL, .line = buffer1->lines[i - 1]};
            i -= 1;
        }
    }

    Diff_Element temp;
    for (int i = 0, k = size - 1; i < k; i++, k--) {
        temp = diff.elements[i];
        diff.elements[i] = diff.elements[k];
        diff.elements[k] = temp;
    }

    diff.size = size;

    return diff;
}

Buffer read_file(char *name) {
    FILE *fp = fopen(name, "r");

    if (fp == NULL) {
        printf("Could not open file: %s\n", name);
        exit(1);
    }

    Buffer buffer;
    buffer.size = readlines(fp, buffer.lines);

    fclose(fp);

    return buffer;
}

void free_buffer(Buffer *buffer) {
    for (int i = 0; i < buffer->size; ++i) {
        free(buffer->lines[i]);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: diff file1 file2\n");
        return 1;
    }

    Buffer buffer1 = read_file(argv[1]);
    Buffer buffer2 = read_file(argv[2]);

    Matrix matrix = allocate_matrix(buffer1.size + 1, buffer2.size + 1);
    lcs_matrix(matrix.m, &buffer1, &buffer2);

    Diff diff = get_diff(matrix.m, &buffer1, &buffer2);
    free_matrix(&matrix);

    for (int i = 0; i < diff.size; i++) {
        Diff_Element diff_element = diff.elements[i];

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


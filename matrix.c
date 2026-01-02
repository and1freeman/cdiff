#include <stdlib.h>
#include <stdio.h>

#include "matrix.h"

Matrix *m_alloc(unsigned int rows, unsigned int columns) {
    if (rows == 0 || columns == 0) return NULL;

    Matrix *matrix = malloc(sizeof(Matrix));

    matrix->rows = rows;
    matrix->columns = columns;
    matrix->size = rows * columns;
    matrix->data = calloc(rows * columns, sizeof(int));

    return matrix;
}

void m_free(Matrix *matrix) {
    free(matrix->data);
    free(matrix);
}

int m_get(Matrix *matrix, unsigned int i, unsigned int j) {
    if (i < 0 || i >= matrix->rows || j < 0 || j >= matrix->columns) {
        printf("Error occured attempting to get matrix element");
        exit(1);
    }

    return matrix->data[i * matrix->columns + j];
}

void m_set(Matrix *matrix, unsigned int i, unsigned int j, int value) {
    if (i < 0 || i >= matrix->rows || j < 0 || j >= matrix->columns) {
        printf("Error occured attempting to set matrix element");
        exit(1);
    }

    matrix->data[i * matrix->columns + j] = value;
}

int m_comp(Matrix *matrix, unsigned int i1, unsigned int j1, unsigned int i2, unsigned int j2) {
    int v1 = m_get(matrix, i1, j1);
    int v2 = m_get(matrix, i2, j2);

    return v1 - v2;
}

void m_print(Matrix *matrix) {
    for (int i = 0; i < matrix->rows; ++i) {
        for (int j = 0; j < matrix->columns; ++j) {
            printf("%d ", matrix->data[i * matrix->columns + j]);
        }

        printf("\n");
    }
}

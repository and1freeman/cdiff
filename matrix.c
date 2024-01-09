#include <stdlib.h>

#include "matrix.h"

 Matrix allocate_matrix(int rows, int columns) {
    Matrix matrix;
    matrix.m = malloc(rows * sizeof(int *));

    for (int i = 0; i < rows; ++i) {
        matrix.m[i] = malloc(columns * sizeof(int));
    }

    matrix.rows = rows;
    matrix.columns = columns;

    return matrix;
}

void free_matrix(Matrix *matrix) {
    for (int i = 0; i < matrix->rows; ++i) {
        free(matrix->m[i]);
    }

    free(matrix->m);
}

#ifndef cdiff_matrix_h
#define cdiff_matrix_h

typedef struct {
    int **m;
    int rows;
    int columns;
} Matrix;

Matrix allocate_matrix(int rows, int columns);
void free_matrix(Matrix *matrix);

#endif

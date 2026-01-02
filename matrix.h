#ifndef cdiff_matrix_h
#define cdiff_matrix_h

typedef struct {
    int *data;
    int rows;
    int columns;
    int size;
} Matrix;

Matrix *m_alloc(unsigned int rows, unsigned int columns);
int m_get(Matrix *matrix, unsigned int i, unsigned int j);
void m_set(Matrix *matrix, unsigned int i, unsigned int j, int value);
int m_comp(Matrix *matrix, unsigned int i1, unsigned int j1, unsigned int i2, unsigned int j2);
void m_free(Matrix *matrix);
void m_print(Matrix *matrix);

#endif

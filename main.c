#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINES 1024
#define MAX_LINE_SIZE 1024
#define MAX(x, y) x > y ? x : y

typedef struct {
    char *lines[MAX_LINES];
    int size; // in lines
} File;


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

int **allocate_matrix(int rows_n, int columns_n) {
    int **matrix = malloc(rows_n * sizeof(int *));

    for (int i = 0; i < rows_n; ++i) {
        matrix[i] = malloc(columns_n * sizeof(int));
    }

    return matrix;
}

void lcs(int **matrix, File *file1, File *file2) {
    for (int i = 0; i < file1->size + 1; ++i) {
        for (int j = 0; j < file2->size + 1; ++j) {
            printf("first: %s\n", file1->lines[i]);

            if (i == 0 || j == 0) {
                matrix[i][j] = 0;
            } else if (!strcmp(file1->lines[i - 1], file2->lines[j - 1])) {
                matrix[i][j] = 1 + matrix[i - 1][j - 1];
            } else {
                matrix[i][j] = MAX(matrix[i - 1][j], matrix[i][j - 1]);
            }
        }
    }
}

File open_file(char *name) {
    FILE *fp = fopen(name, "r");

    if (fp == NULL) {
        printf("Could not open file: %s\n", name);
        exit(1);
    }

    File file;
    file.size = readlines(fp, file.lines);

    return file;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: diff file1 file2\n");
        return 1;
    }

    File file1 = open_file(argv[1]);
    File file2 = open_file(argv[2]);

    int **matrix = allocate_matrix(file1.size + 1, file2.size + 1);

    lcs(matrix, &file1, &file2);

    printf("==========\n");
    print_matrix(matrix, file1.size + 1, file2.size + 1);

    return 0;
}


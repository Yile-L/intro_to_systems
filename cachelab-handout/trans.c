/*
 * Name : Kun Woo Yoo
 * Andrew ID : kunwooy
 *
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(size_t M, size_t N, double A[N][M], double B[M][N], double *tmp);
 * A is the source matrix, B is the destination
 * tmp points to a region of memory able to hold TMPCOUNT (set to 256) doubles as temporaries
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 2KB direct mapped cache with a block size of 64 bytes.
 *
 * Programming restrictions:
 *   No out-of-bounds references are allowed
 *   No alterations may be made to the source array A
 *   Data in tmp can be read or written
 *   This file cannot contain any local or global doubles or arrays of doubles
 *   You may not use unions, casting, global variables, or
 *     other tricks to hide array data in other forms of local or global memory.
 */
#include <stdio.h>
#include <stdbool.h>
#include "cachelab.h"
#include "contracts.h"

/* Forward declarations */
bool is_transpose(size_t M, size_t N, double A[N][M], double B[M][N]);
void trans(size_t M, size_t N, double A[N][M], double B[M][N], double *tmp);
void trans_tmp(size_t M, size_t N, double A[N][M], double B[M][N], double *tmp);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";


int min(int x, int y) {
    if (x > y) return y;
    else return x;
}

void transpose_submit(size_t M, size_t N, double A[N][M], double B[M][N], double *tmp)
{
    size_t row, col, i, j; /* i = index for row, j = index for col*/
    size_t d; /* for indexing diagonals */
    /* if A is 32 x 32 matrix */
    /* since a block can hold 8 double ints, we transfer 8 doubles at once */
    /* also, to utlizie spatial locality of B, we proceed to next row after
     * transferring 8 words */

    if ((M == N) && (M == 32)) {
        for (row = 0; row < N; row += 8) {
            for (col = 0; col < M; col += 8) {
                for (i = row; i < row + 8; i++) {
                    for (j = col; j < col + 8; j++) {
                        if (i == j) {
                            tmp[i*2] = A[i][i];
                            d = i;
                        } else {
                            B[j][i] = A[i][j];
                        }
                    }
                    if (row == col) {
                        B[d][d] = tmp[d*2];
                    }
                }
            }
        }
    } else {      /* now, for A : 65 x 63 matrix */
        if (N == 65 && M == 63) {
            for (row = 0; row < N; row += 4) {
                for (col = 0; col < M; col += 4) {
                        for (i = row; i < min((row+4), N); i++) {
                            for (j = col; j < min((col+4), M); j++) {
                                if (i == j) {
                                    tmp[i] = A[i][i];
                                    d = i;
                                } else {
                                    B[j][i] = A[i][j];
                                }
                            }
                                B[d][d] = tmp[d];
                        }
                    }
                }
        }
        else {
            trans(M, N, A, B, tmp);
        }
    }
    return;
}




/*
 * You can define additional transpose functions below. We've defined
 * some simple ones below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";

/*
 * The following shows an example of a correct, but cache-inefficient
 *   transpose function.  Note the use of macros (defined in
 *   contracts.h) that add checking code when the file is compiled in
 *   debugging mode.  See the Makefile for instructions on how to do
 *   this.
 *
 *   IMPORTANT: Enabling debugging will significantly reduce your
 *   cache performance.  Be sure to disable this checking when you
 *   want to measure performance.
 */
void trans(size_t M, size_t N, double A[N][M], double B[M][N], double *tmp)
{
    size_t i, j;

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            B[j][i] = A[i][j];
        }
    }

    ENSURES(is_transpose(M, N, A, B));
}

/*
 * This is a contrived example to illustrate the use of the temporary array
 */

char trans_tmp_desc[] =
    "Simple row-wise scan transpose, using a 2X2 temporary array";

void trans_tmp(size_t M, size_t N, double A[N][M], double B[M][N], double *tmp)
{
    size_t i, j;
    /* Use the first four elements of tmp as a 2x2 array with row-major ordering. */
    REQUIRES(M > 0);
    REQUIRES(N > 0);

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            int di = i%2;
            int dj = j%2;
            tmp[2*di+dj] = A[i][j];
            B[j][i] = tmp[2*di+dj];
        }
    }

    ENSURES(is_transpose(M, N, A, B));
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);
    registerTransFunction(trans_tmp, trans_tmp_desc);

}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
bool is_transpose(size_t M, size_t N, double A[N][M], double B[M][N])
{
    size_t i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return false;
            }
        }
    }
    return true;
}


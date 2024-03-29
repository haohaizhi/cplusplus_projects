/**
 * @file
 * @brief [Matrix Chain Order]
 * 矩阵链乘法(或矩阵链排序问题)
 * 是一个优化问题，涉及到对给定的矩阵序列进行最有效的乘法。
 * 问题实际上不是执行乘法，而仅仅是决定所涉及的矩阵乘法的顺序。
 */

#include <assert.h> /// for assert
#include <stdio.h>  /// for IO operations
#include <limits.h> /// for INT_MAX macro
#include <stdlib.h> /// for malloc() and free()

/**
 * @brief Finds the optimal sequence using the classic O(n^3) algorithm.
 * @param l length of cost array
 * @param p costs of each matrix
 * @param s location to store results
 * @returns number of operations
 */
int matrixChainOrder(int l,const int *p, int *s) {
    // mat stores the cost for a chain that starts at i and ends on j (inclusive on both ends)
    int mat[l][l];
    for (int i = 0; i < l; ++i) {
        mat[i][i] = 0;
    }
    // cl denotes the difference between start / end indices, cl + 1 would be chain length.
    for (int cl = 1; cl < l; ++cl) {
        for (int i = 0; i < l - cl; ++i) {
            int j = i + cl;
            mat[i][j] = INT_MAX;
            for (int div = i; div < j; ++div) {
                int q = mat[i][div] + mat[div + 1][j] + p[i] * p[div] * p[j];
                if (q < mat[i][j]) {
                    mat[i][j] = q;
                    s[i * l + j] = div;
                }
            }
        }
    }
    return mat[0][l - 1];
}

/**
 * @brief Recursively prints the solution
 * @param l dimension of the solutions array
 * @param s solutions
 * @param i starting index
 * @param j ending index
 * @returns void
 */
void printSolution(int l,int *s,int i,int j) {
    if(i == j) {
        printf("A%d",i);
        return
    }
    putchar('(');
    printSolution(l,s,i,s[i * l + j]);
    printSolution(l,s,s[i * l + j] + 1,j);
    putchar(')');
}

/**
 * @brief Self-test implementations
 * @returns void
 */
static void test() {
    int sizes[] = {35,15,5,10,20,25};
    int len = 6;
    int *sol = malloc(len * len * sizeof(int));
    int r = matrixChainOrder(len,sizes,sol);
    assert(r == 18625);
    printf("Result : %d\n",r);
    printf("Optimal ordering : ");
    printSolution(len,sol,0,5);
    free(sol);

    printf("\n");
}

/**
 * @brief Main function
 * @returns 0
 */
int main() {
    test();  // run self-test implementations
    return 0;
}
